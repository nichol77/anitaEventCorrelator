//TTree *headTree; 
//TTree *adu5PatTree; 
//TTree *corTree; 



void runNewOpt_Nadir() {

  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");

  double startVal=0;               
  double stepSize=0.1;       
  double minVal=-0.5;           
  double maxVal=0.5;            
  Double_t p0 = 0;

  //Load libraries. Need to have ANITA_UTIL_INSTALL_DIR/lib and ROOTSYS/lib in the LD_LIBRARY_PATH               
  gSystem->Load("libfftw3.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libGeom.so");  
  gSystem->Load("libMinuit.so");  
  gSystem->Load("libRootFftwWrapper.so");         
  gSystem->Load("libAnitaEvent.so");      
  gSystem->Load("libAnitaCorrelator.so");

  AnitaGeomTool *fGeomTool = AnitaGeomTool::Instance();
  gSystem->CompileMacro("newOptNadir.C","k");
  
  Double_t relDeltaOut=0;

  TMinuit *myMin = new TMinuit(150);
  myMin->SetObjectFit(newOptNadir);
  myMin->SetFCN(iHateRoot);
  //setArray();

  double startValZ[32] ={0};
  double startValR[32] ={0};
  double startValPhi[32] ={0};




//  for(int y = 0; y <32; y++){

//    char name[30];
//    sprintf(name,"r%d",y);
//    myMin->DefineParameter(y, name, startValR[y], stepSize, minVal, maxVal);
//    sprintf(name,"z%d",y);
//    myMin->DefineParameter(y+32, name, startValZ[y], stepSize, minVal, maxVal);
//    sprintf(name,"phi%d",y);
//    myMin->DefineParameter(y+64, name, startValPhi[y], stepSize, minVal, maxVal);
//  }

for(int y = 0; y <8; y++){

   char name[30];
   sprintf(name,"r%d",y);
   myMin->DefineParameter(y, name, startValR[y], stepSize, minVal, maxVal);
   sprintf(name,"z%d",y);
   myMin->DefineParameter(y+8, name, startValZ[y], stepSize, minVal, maxVal);
   sprintf(name,"phi%d",y);
   myMin->DefineParameter(y+16, name, startValPhi[y], stepSize, minVal, maxVal);
   sprintf(name,"dt%d",y);
   myMin->DefineParameter(y+32, name, startValPhi[y], stepSize, minVal, maxVal);

  }

//  for(int y = 0; y <16; y++){

  //  char name[30];
//    sprintf(name,"r%d",y);
//    myMin->DefineParameter(y, name, startValR[y], stepSize, minVal, maxVal);
//    sprintf(name,"z%d",y);
//    myMin->DefineParameter(y+16, name, startValZ[y], stepSize, minVal, maxVal);
//    sprintf(name,"phi%d",y);
//    myMin->DefineParameter(y+32, name, startValPhi[y], stepSize, minVal, maxVal);
//    sprintf(name,"dt%d",y);
//    myMin->DefineParameter(y+48, name, startValPhi[y], stepSize, minVal, maxVal);

//  }


	Double_t deltaR[32],deltaRErr[32];
	Double_t deltaZ[32],deltaZErr[32];
	Double_t deltaPhi[32],deltaPhiErr[32];
	Double_t deltaT[32],deltaTErr[32];  

	//*********MINUIT METHOD*******************
	myMin->SetPrintLevel(-1);
	myMin->Migrad();   

	for(int u = 0; u <8; u++){
	  myMin->GetParameter(u,deltaR[u],deltaRErr[u]);
	  //cout << "deltaR[" << u << "] = " << deltaR[u] ;
	  myMin->GetParameter(u+8,deltaZ[u],deltaZErr[u]);
	  //cout << " deltaZ[" << u << "] = " << deltaZ[u] ;	
	  myMin->GetParameter(u+16,deltaPhi[u],deltaPhiErr[u]);
	  myMin->GetParameter(u+32,deltaT[u],deltaTErr[u]);
	  //cout << " deltaPhi[" << u << "] = " << deltaPhi[u] << ";" << endl;
	  cout << u << "  " <<  deltaT[u] << "  " << deltaR[u]<< "  " << deltaPhi[u]<< "  " << deltaZ[u]<< endl;
	}





// char name[30];
//  for(int y = 0; y <16; y++){
//    sprintf(name,"z%d",y);
//    myMin->DefineParameter(y, name, startValDeltaT[y], stepSize, minVal, maxVal);
//  }

//  // sprintf(name,"phi1");
//  //myMin->DefineParameter(16, name, startValDeltaT[0], stepSize, minVal, maxVal);
//  //sprintf(name,"phi2");
//  //myMin->DefineParameter(17, name, startValDeltaT[0], stepSize, minVal, maxVal);

// 	Double_t deltaR[32],deltaRErr[32];
// 	Double_t deltaZ[32],deltaZErr[32];
// 	Double_t deltaPhi[32],deltaPhiErr[32];
  
// 	//*********MINUIT METHOD*******************
// 	myMin->SetPrintLevel(-1);
// 	myMin->Migrad();   

// 	myMin->GetParameter(16,deltaZ[0],deltaZErr[0]);
// 	myMin->GetParameter(17,deltaZ[1],deltaZErr[1]);
// 	cout <<  "phi1  " << deltaZ[0] << endl;
// 	cout <<  "phi2  " << deltaZ[1] << endl;
// 	for(int u = 0; u <16; u++){
// 	  myMin->GetParameter(u,deltaZ[u],deltaZErr[u]);
// 	  cout << u << "  " << deltaZ[u] << endl;
// 	}


  
}

void iHateRoot(Int_t& npar, Double_t* gin,
	       Double_t& f, Double_t* par, Int_t flag){


  double diffErr = newOptNadir(par);
  //Double_t diffErr = antOpt(par,headTree,adu5PatTree,corTree);
  f=diffErr;

}


