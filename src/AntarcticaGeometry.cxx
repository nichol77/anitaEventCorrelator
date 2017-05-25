#include "AntarcticaGeometry.h" 
#include "AnitaGeomTool.h" 
#include "RampdemReader.h" 
#include "Adu5Pat.h" 
#include "FFTtools.h"
#include "UsefulAdu5Pat.h" 
#include "TRandom.h" 
#include "TGraph2D.h" 
#include "TString.h" 

ClassImp(AntarcticCoord); 
ClassImp(AntarcticSegmentationScheme); 
ClassImp(PayloadParameters); 
ClassImp(StereographicGrid); 


void AntarcticCoord::asString(TString * s) const
{

  if (type == WGS84) 
  {
    s->Form("%g N %g E %g m",x,y,z); 
  }
  
  if (type == STEREOGRAPHIC)
  {
    s->Form("%g m(E), %g m(N), %g m",x,y,z); 
  }

  if (type == CARTESIAN)
  {
    s->Form("(%g,%g,%g) m",x,y,z); 
  }

}

PayloadParameters::PayloadParameters() 
{
  memset(this,0,sizeof(PayloadParameters)); 

}

PayloadParameters::PayloadParameters(const Adu5Pat * gps, const AntarcticCoord & source_pos) 
  : payload(AntarcticCoord::WGS84, gps->latitude, gps->longitude, gps->altitude), 
    source(source_pos)
{
  payload.to(AntarcticCoord::CARTESIAN); 
  //vector from source to payload


  TVector3 p = payload.v(); 
  TVector3 s = source.v(); 

  TVector3 sprime = s; 
  // this is stolen from AnitaGeomTool. Seems like it should be equivalent to a rotateUZ
  sprime.RotateZ(-1 * p.Phi()); 
  sprime.RotateY(-1 * p.Theta()); 
  sprime[2]=p.Mag()-sprime.z(); 

  sprime.RotateZ(gps->heading*TMath::DegToRad()); 
  //TODO: check if these axes need modification 
  sprime.RotateY(-AnitaStaticAdu5Offsets::pitch *TMath::DegToRad()); 
  sprime.RotateX(-AnitaStaticAdu5Offsets::roll *TMath::DegToRad()); 

  source_phi = FFTtools::wrap(sprime.Phi() * TMath::RadToDeg(),360); 
  source_theta = 90 - sprime.Theta() * TMath::RadToDeg(); 

  //Now do the opposite 
  TVector3 pprime = p;
  pprime.RotateZ(-1*s.Phi()); 
  pprime.RotateY(-1*s.Theta()); 
  pprime[2] = pprime.z()-s.Mag(); 
  payload_el = 90-pprime.Theta() * TMath::RadToDeg(); 
  payload_az = pprime.Phi() * TMath::RadToDeg(); 

  distance = (source.v() - payload.v()).Mag(); 
}



void AntarcticCoord::convert(CoordType t)
{

  if (type == WGS84) 
  {
    if (t == CARTESIAN) 
    {
      double cartesian[3]; 
      AnitaGeomTool::Instance()->getCartesianCoords(x,y,z, cartesian); 
      x = cartesian[0]; 
      y = cartesian[1]; 
      z = cartesian[2]; 
    }

    else if ( t==STEREOGRAPHIC) 
    {
      RampdemReader::LonLatToEastingNorthing(y,x,x,y); 
    }

  }

  else if (type == STEREOGRAPHIC)
  {


    if (t == WGS84) 
    {
      RampdemReader::EastingNorthingToLonLat(x,y,y,x); 
    }

    else if (t == CARTESIAN) 
    {
      //be lazy 
      convert(WGS84); 
      convert(CARTESIAN); 
    }

  }

  else if (type == CARTESIAN) 
  {

    if (t == WGS84) 
    {
      double cartesian[3]; 
      cartesian[0] = x; 
      cartesian[1] = y; 
      cartesian[2] = z; 
      AnitaGeomTool::Instance()->getLatLonAltFromCartesian(cartesian, x,y,z); 
    }
    else if ( t== STEREOGRAPHIC)
    {
      //be lazy 
      convert(WGS84); 
      convert(STEREOGRAPHIC); 
    }
  }

  else 
  {
    fprintf(stderr,"Invalid CoordType: %d!", type); 
    return; 
  }


  type = t; 

}



AntarcticSegmentationScheme * AntarcticSegmentationScheme::factory(const char * key) 
{

  if (strstr(key,"stereographic_grid"))
  {
    int nx,ny,maxE,maxN; 
    ny = -1; 
    nx = 0; 
    maxE = 3330000; 
    maxN = 3330000; 
    sscanf(key,"stereographic_grid_%d_%d_%d_%d", &nx, &ny, &maxN, &maxE); 
    if (!nx) return 0; 
    if (ny < 0) ny = nx; 
    return new StereographicGrid(nx,ny,maxE,maxN); 
  }

  return 0; 
}

const int nsamples = 64; 

void AntarcticSegmentationScheme::DrawI(const char * opt, const int * idata) const 
{
  int N = NSegments(); 
  double * data = 0;
  if (idata) 
  {
    data = new double[N]; 
    for (int i = 0; i < N; i++) { data[i] = idata[i]; } 
  }
  Draw(opt,data); 
  delete data; 
}

void AntarcticSegmentationScheme::Draw(const char * opt, const double * data) const 
{
  int N = NSegments(); 

  TGraph2D * g  = new TGraph2D(N*nsamples); 
  g->SetBit(kCanDelete); 

  AntarcticCoord samples[nsamples]; 

  for (int i =0; i < N; i++) 
  {
    /* do not need altitude for this! */ 
    sampleSegment(i, nsamples, samples, true,false); 

    for (int j =0;  j<  nsamples; j++) 
    {
      samples[j].to(AntarcticCoord::STEREOGRAPHIC); 
      g->SetPoint(nsamples*i+j,samples[j].x,samples[j].y,data ? data[i] : i); 
    }
  }

  g->Draw(opt); 
}



StereographicGrid::StereographicGrid(int NX, int NY, double MAX_E, double MAX_N)
  : nx(NX), ny(NY), max_E(MAX_E), max_N(MAX_N) 
{

  dx = (2*max_E)/nx; 
  dy = (2*max_N)/ny; 
}

int StereographicGrid::getSegmentIndex(const AntarcticCoord & coord) const
{
  AntarcticCoord stereo = coord.as(AntarcticCoord::STEREOGRAPHIC); 

  if (stereo.x >  max_E || stereo.x < -max_E || stereo.y > max_N || stereo.y < -max_N) return -1; 


  int xbin = nx/2 + stereo.x / dx; 
  int ybin = ny/2 - stereo.y / dy; 

  return xbin + nx * ybin; 

}

void StereographicGrid::getSegmentCenter(int idx, AntarcticCoord * fillme, bool fillalt) const 
{

  int xbin = idx % nx; 
  int ybin = idx / nx; 
  double x = (xbin +0.5) *dx - max_E; 
  double y = max_N - (ybin +0.5) *dy ; 
  double z = fillalt ? RampdemReader::SurfaceAboveGeoidEN(x,y,dataset): 0; 
  fillme->set(AntarcticCoord::STEREOGRAPHIC,x,y,z); 
}


AntarcticCoord * StereographicGrid::sampleSegment(int idx, int N, AntarcticCoord * fill, bool random, bool fillalt) const 
{

  if (!fill) fill = new AntarcticCoord[N]; 

  int xbin = idx % nx; 
  int ybin = idx / nx; 
  double lx = (xbin) *dx - max_E; 
  double ly = max_N - (ybin) *dy ; 


  if (random) 
  {
    for (int i = 0; i < N; i++)
    {
      double x = gRandom->Uniform(lx, lx+dx); 
      double y = gRandom->Uniform(ly, ly+dy); 
      double z = fillalt ? RampdemReader::SurfaceAboveGeoidEN(x,y,dataset): 0; 
      fill[i].set(AntarcticCoord::STEREOGRAPHIC,x,y,z); 
    }
  }
  else
  {
    int grid = sqrt(N) + 0.5 ; 

    for (int i = 0; i < N; i++)
    {
      //TODO check if this is what i want! 
      double x = lx + (dx / (grid+1)) * (1+(i % grid)); 
      double y = ly + (dy / (grid+1)) * (1+(i / grid)); 
      double z = fillalt ? RampdemReader::SurfaceAboveGeoidEN(x,y,dataset): 0; 
      fill[i].set(AntarcticCoord::STEREOGRAPHIC,x,y,z); 
    }
  }

  return fill; 
}



void StereographicGrid::Draw(const char * opt, const double * data) const
{
  TH2I h("tmp","Stereographic Grid", nx, -max_E, max_E, ny, -max_N, max_N); 
  for (int i = 1; i <= nx; i++) 
  {
    for (int j = 1; j <= ny; j++) 
    {
      AntarcticCoord c(AntarcticCoord::STEREOGRAPHIC, h.GetXaxis()->GetBinCenter(i), h.GetYaxis()->GetBinCenter(j));
      int idx = getSegmentIndex(c); 
      h.SetBinContent(i,j,  data ? data[idx] : idx); 
    }
  }
  h.DrawCopy(opt); 
}

void StereographicGrid::asString(TString * str) const
{
  str->Form("stereographic_grid_%d_%d_%d_%d", nx,ny,(int) max_E,(int) max_N); 
}



int StereographicGrid::getNeighbors(int segment, std::vector<int> * neighbors) const
{

  int N = 0; 

  bool left_edge = (segment % nx ) == 0; 
  bool right_edge = (segment % nx ) == nx-1 ; 
  bool top_edge = (segment / ny ) == 0; 
  bool bottom_edge = (segment / ny ) == ny-1 ; 

  int start_x = left_edge ? 0 : -1; 
  int end_x = right_edge ? 0 : 1; 
  int start_y = top_edge ? 0 : -1; 
  int end_y = bottom_edge ? 0 : 1; 

  for (int i = start_x; i <= end_x; i++) 
  {
    for (int j = start_y; j <= end_y; j++)
    {
      if (i == 0 && j == 0) continue; 

      N++; 
      if (neighbors) neighbors->push_back(segment + i + j *nx); 
    }
  }

  return N; 
}



