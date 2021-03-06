#include "AnitaConventions.h"
#include "PrettyAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "UsefulAdu5Pat.h"
#include "CorrelationSummary.h"
#include "AnitaGeomTool.h"
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



void correlationTreeLoopOpt(int run, char *baseDir, char *corTreeDir, char *outputDir, double *deltaR, double *deltaZ, double *deltaPhi, double *deltaHeading);

  
void correlationTreeLoopOpt(int run,char *baseDir, char *corTreeDir, char *outputDir,double *deltaR, double *deltaZ, double *deltaPhi, double *deltaHeading) {
   AnitaGeomTool *fGeomTool = AnitaGeomTool::Instance();
  
 baseDir = "/Users/simonbevan/Desktop/";
 corTreeDir = "../../outFiles/";
 outputDir = "../../outFiles/";

  char eventName[100];
  char headerName[100];
  char hkName[100];
  char gpsName[100];
  char corrName[100];
  char outName[100];
  

  //sprintf(baseDir,"http://www.hep.ucl.ac.uk/uhen/anita/private/monitor2/runs/fromLoki/");
  sprintf(eventName,"%s/run%d/eventFile%d.root",baseDir,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
  sprintf(gpsName,"%s/run%d/gpsFile%d.root",baseDir,run,run);
  sprintf(corrName,"%s/corRun%d.root",corTreeDir,run);
 sprintf(outName,"%s/deltaTFile%d.root",outputDir,run); 
   
   RawAnitaEvent *event = 0;
   PrettyAnitaHk *hk = 0;
   
   RawAnitaHeader *header =0;
   Adu5Pat *pat =0;
   CorrelationSummary *corSum =0;
   
   //  TChain *eventChain = new TChain("eventTree");
   // eventChain->Add(eventName);
   // eventChain->SetBranchAddress("event",&event);
   
  //  TFile *fpHk = new TFile(hkName);
//    TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
//    prettyHkTree->SetBranchAddress("hk",&hk);
   
   TFile *fpHead = TFile::Open(headerName);
   TTree *headTree = (TTree*) fpHead->Get("headTree");
   headTree->SetBranchAddress("header",&header);
   headTree->BuildIndex("eventNumber");
   
   TFile *fpGps = TFile::Open(gpsName);
   TTree *adu5PatTree = (TTree*) fpGps->Get("adu5PatTree");
   adu5PatTree->BuildIndex("realTime");
   adu5PatTree->SetBranchAddress("pat",&pat);
   
   Int_t labChip;
   TFile *fpCor = new TFile(corrName);
   TTree *corTree = (TTree*) fpCor->Get("corTree");
   corTree->SetBranchAddress("cor",&corSum);
   corTree->SetBranchAddress("labChip",&labChip);

   // TFile *fpGp = new TFile("/unix/anita/gpLogs/gp_log_all.root");
   //TTree *gpLogTree = (TTree*) fpGp->Get("gp_log");
   // Double_t gpTriggerTime;
   //Int_t antennaFlag, sgFreq;
   //gpLogTree->SetBranchAddress("trigger_time",&gpTriggerTime);
   //gpLogTree->SetBranchAddress("sg_freq",&sgFreq);
   //gpLogTree->SetBranchAddress("antenna",&antennaFlag);
   //gpLogTree->BuildIndex("trigger_time");

   Long64_t numEntries=corTree->GetEntries();
   int counter=0;

   TFile *fpOut = new TFile(outName,"RECREATE");
//    TH1F *histSimpleDtDiff  = new TH1F("histSimpleDtDiff","All DeltaT Diffs",2000,-10,10);
//    TH1F *histDtDiffSep[16][4][11];
//    char histName[180];
//    for(int ant=0;ant<16;ant++) {
//       for(int chip=0;chip<4;chip++) {
// 	 for(int cor=0;cor<11;cor++) {
// 	    sprintf(histName,"histDtDiffSep%d_%d_%d",ant,chip,cor);
// 	    histDtDiffSep[ant][chip][cor]= new TH1F(histName,histName,2000,-10,10);
// 	 }
//       }
//    }

   Long64_t entry=0;
   UInt_t eventNumber, triggerTime, triggerTimeNs;
   Int_t firstAnt,secondAnt,maxAnt,corInd;
   Double_t deltaT,deltaTExpected,deltaTExpectedOpt;
   Double_t phiWave, phiMaxAnt;
   Double_t corPeak, corRMS;
   Double_t balloonLat, balloonLon, balloonAlt;
   Double_t heading,pitch,roll;
   TTree *deltaTTree = new TTree("deltaTTree","Tree of Delta T's");
   deltaTTree->Branch("entry",&entry,"entry/L");
   deltaTTree->Branch("firstAnt",&firstAnt,"firstAnt/I");
   deltaTTree->Branch("secondAnt",&secondAnt,"secondAnt/I");
   deltaTTree->Branch("maxAnt",&maxAnt,"maxAnt/I");
   deltaTTree->Branch("labChip",&labChip,"labChip/I");
   deltaTTree->Branch("deltaT",&deltaT,"deltaT/D");
   deltaTTree->Branch("deltaTExpected",&deltaTExpected,"deltaTExpected/D");
   deltaTTree->Branch("deltaTExpectedOpt",&deltaTExpectedOpt,"deltaTExpectedOpt/D");
   deltaTTree->Branch("corPeak",&corPeak,"corPeak/D");
   deltaTTree->Branch("corRMS",&corRMS,"corRMS/D");
   deltaTTree->Branch("phiWave",&phiWave,"phiWave/D");
   deltaTTree->Branch("phiMaxAnt",&phiMaxAnt,"phiMaxAnt/D");
   deltaTTree->Branch("eventNumber",&eventNumber,"eventNumber/i");
   deltaTTree->Branch("triggerTime",&triggerTime,"triggerTime/i");
   deltaTTree->Branch("triggerTimeNs",&triggerTimeNs,"triggerTimeNs/i");
   deltaTTree->Branch("corInd",&corInd,"corInd/I");
   deltaTTree->Branch("balloonLat",&balloonLat,"balloonLat/D");
   deltaTTree->Branch("balloonLon",&balloonLon,"balloonLon/D");
   deltaTTree->Branch("balloonAlt",&balloonAlt,"balloonAlt/D");
   deltaTTree->Branch("heading",&heading,"heading/D");
   deltaTTree->Branch("pitch",&pitch,"pitch/D");
   deltaTTree->Branch("roll",&roll,"roll/D");
   

   Double_t thetaWave;

   for(entry=0;entry<numEntries;entry++) {
  

      corTree->GetEntry(entry);
      Long64_t headEntry=headTree->GetEntryNumberWithIndex(corSum->eventNumber);
      if(headEntry<0) 
	continue;
      headTree->GetEntry(headEntry);
     
      // if(header->triggerTimeNs*1e-9< 0.097 || header->triggerTimeNs*1e-9>0.1)
      if( (header->triggerTimeNs>0.3e6) || (header->triggerTimeNs<0.2e6) )  
      continue; 

      triggerTimeNs=header->triggerTimeNs;
      triggerTime=header->triggerTime;
      eventNumber=header->eventNumber;
      //Long64_t gpEntry=gpLogTree->GetEntryNumberWithBestIndex(header->triggerTime);
      //     cout << gpEntry << endl;
      //if(gpEntry>-1) 
      //	 gpLogTree->GetEntry(gpEntry);
      //else 
      //	 continue;
     
      //  if(antennaFlag!=1 || sgFreq!=-1) 
      //	 continue;

      //     cout << entry << "\t" << gpEntry << "\t" << header->triggerTime << "\t" << (UInt_t)gpTriggerTime << "\n";
     
      //eventChain->GetEntry(entry);
      //prettyHkTree->GetEntry(entry);


      Long64_t bestEntry = adu5PatTree->GetEntryNumberWithBestIndex(header->triggerTime);
      if(bestEntry>-1) 
	adu5PatTree->GetEntry(bestEntry);
      else 
	continue;
      pat->heading+(Float_t)deltaHeading[0];
      
   // PrettyAnitaEvent realEvent(event,WaveCalType::kVTFullAGCrossCorClock,header);
      balloonLat=pat->latitude;
      balloonLon=pat->longitude;
      balloonAlt=pat->altitude;
      heading=pat->heading;
      pitch=pat->pitch;
      roll=pat->roll;
      UsefulAdu5Pat usefulPat(pat);
     
      for(corInd=0;corInd<19;corInd++) {


	//replace taylor dome
	//	 deltaTExpected=usefulPat.getDeltaTWillySeavey(corSum->firstAnt[corInd],corSum->secondAnt[corInd]);
	usefulPat.fSourceLongitude=0;
	 deltaTExpected=usefulPat.getDeltaTTaylor(corSum->firstAnt[corInd],corSum->secondAnt[corInd]);

	 //	 histSimpleDtDiff->Fill(corSum->maxCorTimes[corInd]-deltaTExpected);
	 int antInd=corSum->firstAnt[corInd]%16;
	 //	 histDtDiffSep[antInd][labChip][corInd]->Fill(corSum->maxCorTimes[corInd]-deltaTExpected);

	 firstAnt=corSum->firstAnt[corInd];
	 secondAnt=corSum->secondAnt[corInd];

	 deltaTExpectedOpt=usefulPat.getDeltaTTaylorOpt(corSum->firstAnt[corInd],corSum->secondAnt[corInd],deltaR,deltaZ,deltaPhi);


	 deltaT=corSum->maxCorTimes[corInd];
	 maxAnt=corSum->centreAntenna;
	 phiWave=usefulPat.getPhiWave()*TMath::RadToDeg();
	 phiMaxAnt=fGeomTool->getAntPhiPositionRelToAftFore(corSum->centreAntenna)*TMath::RadToDeg();
	 corPeak=corSum->maxCorVals[corInd];
	 corRMS=corSum->rmsCorVals[corInd];
	 deltaTTree->Fill();
	 //	cout << corSum->centreAntenna << "\t" << corSum->firstAnt[corInd] << "\t" << corSum->secondAnt[corInd] << "\t" << deltaTExpected << "\t" << corSum->maxCorTimes[corInd] << "\n";
      }


      //     cout << "Phi Compare:\t" << corSum->centreAntenna << "\t" << fGeomTool->getAntPhiPositionRelToAftFore(corSum->centreAntenna)*TMath::RadToDeg() << "\t"
      //	  << usefulPat.getPhiWave()*TMath::RadToDeg() << endl;
      //     if(entry>10)
      //     cout << "Source:\t" << usefulPat.getSourceLongitude() << "\t" << usefulPat.getSourceLatitude() << "\t"
      //	  << usefulPat.getSourceAltitude() << "\n";
      //     cout << "Balloon:\t" << usefulPat.longitude << "\t" << usefulPat.latitude << "\t" << usefulPat.altitude << "\t" << usefulPat.heading << endl;
      counter++; 
      // if(counter%100==0)
      //	 cerr << "*";
       }
//    histSimpleDtDiff->Write();
//    for(int ant=0;ant<16;ant++) {
//       for(int chip=0;chip<4;chip++) {
// 	 for(int cor=0;cor<11;cor++) {
// 	    histDtDiffSep[ant][chip][cor]->Write();
// 	 }
//       }
//    }

   delete event;
   delete hk;
   
   delete header;
   delete pat;
   delete corSum;

   delete headTree;
   delete fpHead;
   
   delete adu5PatTree;     
   delete fpGps;
 
   delete corTree; 
   delete fpCor;
 

  

   deltaTTree->AutoSave();
   fpOut->Close();
   //   histSimpleDtDiff->Draw();
}

