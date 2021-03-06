void runSeavy2() {
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");

  //Load libraries. Need to have ANITA_UTIL_INSTALL_DIR/lib and ROOTSYS/lib in the LD_LIBRARY_PATH               
  gSystem->Load("libfftw3.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libGeom.so");  
  gSystem->Load("libMinuit.so");  
  gSystem->Load("libRootFftwWrapper.so");         
  gSystem->Load("libAnitaEvent.so");      
  gSystem->Load("libAnitaCorrelator.so");


  gSystem->CompileMacro("seavyPlot2.C","k");
    seavyPlot2();

  // gSystem->CompileMacro("anglePlotterOpt.C","k");
  //   anglePlotterOpt();

}
