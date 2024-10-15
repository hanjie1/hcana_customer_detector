using namespace std;
void replay(Int_t RunNumber=0, Int_t MaxEvent=0){

  gSystem->Load("libSRO");
  const char* RunFileNamePattern="data/test_nim_trigger_%d.evio.0";

  //Only one way to replay SKIM scripts.  ALL segments, othewise they will overwite each other.
  const char* ROOTFileNamePattern;
  ROOTFileNamePattern = "rootfiles/nim_trigger_test_%d.root";

  // Add variables to global list.
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/standard.database");
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);

  // Load fadc debug parameters
  gHcParms->Load("PARAMS/debug.param");

  // Load the Hall C style detector map
  gHcDetectorMap = new THcDetectorMap();
  gHcDetectorMap->Load("MAPS/detector.map");



  //Add SRO spectrometer apparatus
  THaApparatus* SRO = new THcSROApparatus("SRO","SRO");
  gHaApps->Add(SRO);
 

  //Add active analyzer to SRO apparatus
  THcActiveAnalyzer* acta = new THcActiveAnalyzer("A", "active analyzer");
  SRO->AddDetector(acta);

  THcAnalyzer* analyzer = new THcAnalyzer;
  THaEvent* event = new THaEvent;

  //Define the run(s) that we want to analyze.
  THcRun* run = new THcRun( Form(RunFileNamePattern, RunNumber) );


  // Set to read in Hall C run database parameters
  run->SetRunParamClass("THcRunParameters");
  if(MaxEvent>0)
     run->SetEventRange(1, MaxEvent);
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();

  // Define the analysis parameters
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber);

  // Define the analysis parameters
  analyzer->SetEvent( event );
  analyzer->SetCountMode(2);  // 0 = counter is # of physics triggers
                              // 1 = counter is # of all decode reads
                              // 2 = counter is event number

  analyzer->SetOutFile(ROOTFileName.Data());
  analyzer->SetCrateMapFileName("MAPS/db_cratemap.dat");
  analyzer->SetOdefFile("DEF-files/output.def");

  analyzer->Process(run);     // start the actual analysis
}
