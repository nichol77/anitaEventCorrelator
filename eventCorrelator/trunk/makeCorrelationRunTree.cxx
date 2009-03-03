#include "AnitaConventions.h"
#include "PrettyAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "CalibratedAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "AnitaGeomTool.h"
#include "Adu5Pat.h"
#include "UsefulAdu5Pat.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>

void makeCorrelationRunTree(int run, int numEnts, char *baseDir, char *outDir=0);

int main(int argc, char **argv) {
  int run=17;
  if(argc>1) {
    run=atoi(argv[1]);
  }
  std::cout << "Making correlation summary tree for run: " << run << "\n";
  // makeCorrelationRunTree(run,0,"/Users/simonbevan/Desktop/","/Users/simonbevan/ANITA/outfiles/");
  makeCorrelationRunTree(run,0,"/unix/anita3/flight0809/root","/unix/anita3/rjn/corTrees");

}
  
void makeCorrelationRunTree(int run, int numEnts, char *baseDir, char *outDir) {
  //AnitaGeomTool *fGeomTool = 
  //  AnitaGeomTool::Instance();


  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  char gpsName[FILENAME_MAX];
  char outName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/calEventFile%d*.root",baseDir,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
  sprintf(gpsName,"%s/run%d/gpsFile%d.root",baseDir,run,run);

  Int_t useCalibratedFiles=0;

  RawAnitaEvent *event = 0;
  CalibratedAnitaEvent *calEvent = 0;
  RawAnitaHeader *header =0;
  Adu5Pat *pat = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);

  if(eventChain->GetEntries()>0) {
    eventChain->SetBranchAddress("event",&calEvent);
    useCalibratedFiles=1;
  }
  else {
    sprintf(eventName,"%s/run%d/eventFile%d*.root",baseDir,run,run);
    eventChain->Add(eventName);
    eventChain->SetBranchAddress("event",&event);
  }

  TFile *fpHead = TFile::Open(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);

  TFile *fpGps = TFile::Open(gpsName);
  TTree *adu5PatTree = (TTree*) fpGps->Get("adu5PatTree");
  adu5PatTree->SetBranchAddress("pat",&pat);



  //Make output files
  CorrelationSummary *theCor=0;
  TFile *fpOut;
  if(outDir) {
     sprintf(outName,"%s/corRun%d.root",outDir,run);
  }
  else {
     sprintf(outName,"corRun%d.root",run);
  }
  std::cout << outName << std::endl;
  fpOut= new TFile(outName,"RECREATE");
  Double_t thetaWave,phiWave;
  Int_t labChip;
  TTree *corTree = new TTree("corTree","Tree of Correlation Summaries");
  corTree->Branch("cor","CorrelationSummary",&theCor);
  corTree->Branch("thetaWave",&thetaWave,"thetaWave/D");
  corTree->Branch("phiWave",&phiWave,"phiWave/D");
  corTree->Branch("labChip",&labChip,"labChip/I");

  Long64_t maxEntry=headTree->GetEntries(); 
  if(numEnts && maxEntry>numEnts) maxEntry=numEnts;

  Int_t starEvery=maxEntry/1000;
  if(starEvery==0) starEvery=1;
  
  std::cout <<  "There are " << maxEntry << " events to proces\n";
  for(Long64_t entry=0;entry<maxEntry;entry++) {
     if(entry%starEvery==0) std::cerr << "*";
     //Friends only seem to work with TTree::Draw and similar commands
     //if you are manually calling GetEntry (i.e in a loop) you must call
     //the GetEntry for each tree separately.
     //  eventChain->AddFriend(headTree);
     //  eventChain->AddFriend(prettyHkTree);
     
     //Stupidly most do this to be perfectly safe  

     headTree->GetEntry(entry);
     if( (header->triggerTimeNs>0.3e6) || (header->triggerTimeNs<0.2e6) )  
      continue; 
     

     eventChain->GetEntry(entry);
     adu5PatTree->GetEntry(entry);
     
     PrettyAnitaEvent *realEvent=0;
     if(useCalibratedFiles) {
       realEvent = new PrettyAnitaEvent(calEvent);
     }
     else {
       realEvent = new PrettyAnitaEvent(event,WaveCalType::kVTFullAGCrossCorClock,header);
     }
     labChip=realEvent->getLabChip(1);
     
     UsefulAdu5Pat usefulPat(pat);
     usefulPat.getThetaAndPhiWaveWillySeavey(thetaWave,phiWave);
     int ant=realEvent->getMaxAntenna(AnitaPol::kVertical);
     //     int ant=fGeomTool->getUpperAntNearestPhiWave(phiWave);
     Double_t deltaT= 1. / (2.6*16);
     //     std::cout << deltaT << std::endl;
     //     if(entry%100==0)
     //       std::cout << ant << "\t" << realEvent.getMaxAntenna(AnitaPol::kVertical) << std::endl;
     theCor =realEvent->getCorrelationSummary(ant,AnitaPol::kVertical,deltaT);
     corTree->Fill();     
     delete theCor;
     delete realEvent;
  }
  std::cerr << "\n";
  corTree->AutoSave();
  fpOut->Close();
}
     
