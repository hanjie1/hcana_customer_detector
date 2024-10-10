/**
\class THcSROApparatus
\ingroup Apparatuses

\brief A dummy spectrometer apparatus for testing detectors.

This class behaves as a spectrometer apparatus in that it holds detector
classes, but it does no reconstruction. It is intended to provide an elegant
way to setup a teststand for a detector or a set of detectors, without
requiring some "needed" detectors or parameters.
*/

/**
\fn THcSROApparatus::THcSROApparatus(const char* name, const char* description)

\brief A constructor.

\param[in] name Name of the apparatus. Is typically "H", "P" or "S" for
HMS, SHMS and SOS, respectively.
\param[in] description Description of the apparatus.
*/

/**
\fn virtual THcSROApparatus::~THcSROApparatus()

\brief A destructor.
*/

/**
\fn Int_t THcSROApparatus::Reconstruct()

\brief Does nothing and returns 0.

This function is usually responsible for reconstructing the event from the
detectors in the spectrometer. Since this is a dummy apparatus, we have a dummy
reconstruction.
*/

/**
\fn std::string THcSROApparatus::GetKwPrefix()

\brief Returns prefix used for parameters in `param` files.

All the parameters read by this apparatus are prefixed with the returned string.
*/

#include "THcSROApparatus.h"
#include "THaNonTrackingDetector.h"

#include "TDatime.h"

#include "THcGlobals.h"
#include "THcParmList.h"

#ifdef WITH_DEBUG
#include <iostream>
#endif
using namespace std;

THcSROApparatus::THcSROApparatus(
  const char* name, const char* description
) :
  THaApparatus(name, description),
  fKwPrefix(""),
  fStagesDone(0), fListInit(kFALSE)
{
  // Constructorn

  fTracks = 0;
  fNonTrackingDetectors = new TList;
  // Construct the kwPrefix here. Better place would be in `Setup` method.
  string kwPrefix = name;
  std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::tolower);
  fKwPrefix = kwPrefix;
}


THcSROApparatus::~THcSROApparatus() {}

//_____________________________________________________________________________
void THcSROApparatus::ListInit()
{
  // Initialize lists of specialized detectors. 
  // Private function called by Reconstruct().

  fNonTrackingDetectors->Clear();

  TIter next(fDetectors);
  while( THaDetector* theDetector = 
	 static_cast<THaDetector*>( next() )) {

    //    if( theDetector->IsTracking() )
    //      fTrackingDetectors->Add( theDetector );
    //    else
    fNonTrackingDetectors->Add( theDetector );

      //    if( theDetector->IsPid() )
      //      fPidDetectors->Add( theDetector );
  }

  // Set up PIDinfo and vertex objects that can be associated with tracks

  //  UInt_t ndet  = GetNpidDetectors();
  //  UInt_t npart = GetNpidParticles();
  //  TClonesArray& pid  = *fTrackPID;

  //  for( int i = 0; i < kInitTrackMultiplicity; i++ ) {
  //    new( pid[i] )  THaPIDinfo( ndet, npart );
  //  }
  
  fListInit = kTRUE;
}
//_____________________________________________________________________________
Int_t THcSROApparatus::CoarseReconstruct()
{
  // 2nd step: Coarse processing.  Pass the coarse tracks to the remaining
  // detectors for any processing that can be done at this stage.
  // This may include clustering and preliminary PID.
  // PID information is tacked onto the tracks as a THaPIDinfo object.

  if( !fListInit )
    ListInit();
  
  // No Tracking in NPS apparatus
  //  if( !IsDone(kCoarseTrack))a
  //    CoarseTrack();

  TIter next( fNonTrackingDetectors );
  while( THaNonTrackingDetector* theNonTrackDetector =
	 static_cast<THaNonTrackingDetector*>( next() )) {
#ifdef WITH_DEBUG
    if( fDebug>1 ) cout << "Call CoarseProcess() for " 
			<< theNonTrackDetector->GetName() << "... ";
#endif
    theNonTrackDetector->CoarseProcess( *fTracks );
#ifdef WITH_DEBUG
    if( fDebug>1 ) cout << "done.\n";
#endif
  }

  fStagesDone |= kCoarseRecon;
  return 0;
}

//_____________________________________________________________________________
Int_t THcSROApparatus::Reconstruct()
{
  // This method implements a fairly generic algorithm for spectrometer
  // reconstruction which should be useful for most situations.
  // Additional, equipment-specific processing can be done in
  // a derived class that calls this method first.
  //
  // The algorithm is as follows:
  //
  // For all tracking detectors:
  //   CoarseTrack()
  // For all non-tracking detectors:
  //   CoarseProcess()
  // For all tracking detectors:
  //   FineTrack()
  // Reconstruct tracks to target
  // For all non-tracking detectors:
  //   FineProcess()
  // Compute additional attributes of tracks (e.g. momentum, beta)
  // Find "Golden Track"
  // Combine all PID detectors to get overall PID for each track
  //

  // Do prior analysis stages if not done yet
  //  if( !IsDone(kTracking))
  //    Track();

  // Fine processing.  Pass the precise tracks to the
  // remaining detectors for any precision processing.
  // PID likelihoods should be calculated here.

  TIter next( fNonTrackingDetectors );
  while( THaNonTrackingDetector* theNonTrackDetector =
	 static_cast<THaNonTrackingDetector*>( next() )) {
#ifdef WITH_DEBUG
    if( fDebug>1 ) cout << "Call FineProcess() for " 
			<< theNonTrackDetector->GetName() << "... ";
#endif
    theNonTrackDetector->FineProcess( *fTracks );
#ifdef WITH_DEBUG
    if( fDebug>1 ) cout << "done.\n";
#endif
  }

  // Compute additional track properties (e.g. beta)
  // Find "Golden Track" if appropriate.
  
  //TrackCalc();

  // Compute combined PID

  //  if( fPID ) CalcPID();

  fStagesDone |= kReconstruct;
  return 0;
}


TString THcSROApparatus::GetKwPrefix() {
  return fKwPrefix;
}

Int_t THcSROApparatus::ReadRunDatabase(const TDatime& date) {
  
  return kOK;
}


Int_t THcSROApparatus::ReadDatabase(const TDatime& date) {

  DBRequest list[]={
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list);

  DBRequest listprefixed[]={
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&listprefixed,fKwPrefix.c_str());

  
  return kOK;
}


Int_t THcSROApparatus::DefineVariables(THaAnalysisObject::EMode mode) {
  if (mode == kDefine && fIsSetup) return kOK;
  fIsSetup = (mode == kDefine);

  return kOK;
  //  std::vector<RVarDef> vars;
  //  RVarDef end {0};
  //  vars.push_back(end);

  //  return DefineVarsFromList(vars.data(), mode);
}


THcSROApparatus::THcSROApparatus() {}


ClassImp(THcSROApparatus)
