#ifndef ROOT_THcSROApparatus
#define ROOT_THcSROApparatus

#include "TString.h"

#include "THaAnalysisObject.h"
#include "THaApparatus.h"
#include "TClonesArray.h"

class TDatime;


class THcSROApparatus : public THaApparatus {
public:
  THcSROApparatus(const char* name, const char* description);
  virtual ~THcSROApparatus();

  virtual Int_t CoarseReconstruct();
  virtual Int_t Reconstruct();

  TString GetKwPrefix();

  enum EStagesDone {
    kCoarseTrack = BIT(0),
    kCoarseRecon = BIT(1),
    kTracking    = BIT(2),
    kReconstruct = BIT(3)
  };

protected:
  TClonesArray* fTracks;
  
  virtual Int_t ReadRunDatabase(const TDatime& date);
  virtual Int_t ReadDatabase(const TDatime& date);
  virtual Int_t DefineVariables(EMode mode=kDefine);

  TList*          fNonTrackingDetectors;  //Non-tracking detectors

  
  std::string fKwPrefix;

  UInt_t          fStagesDone;            //Bitfield of completed analysis stages

private:
  Bool_t          fListInit;      //Detector lists initialized

  void            ListInit();     //Initializes lists of specialized detectors

  THcSROApparatus();
  ClassDef(THcSROApparatus, 0);
};


#endif  // ROOT_THcSROApparatus
