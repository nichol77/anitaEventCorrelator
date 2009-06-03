//////////////////////////////////////////////////////////////////////////////
/////  UsefulAdu5Pat.h        Useful ANITA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for utilising the ADU5 data                    /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef USEFULADU5PAT_H
#define USEFULADU5PAT_H

//Includes
#include <TObject.h>
#include <TGraph.h>
#include <TVector3.h>
#include "Adu5Pat.h"
#include "AnitaConventions.h"
#include "AnitaGeomTool.h"
#include "BedmapReader.h"

//!  This is the position and attitude class that inherits from Adu5Pat and has some useful methods for determining the expected plane wave angles and inter-antenna crossing times for a given source.
/*!
  As UsefulAnitaEvent is to RawAnitaEvent, UsefulAdu5Pat is to Adu5Pat. Well not quite as useful but you get the picture. Note that UsefulAdu5Pat sets the pitch, roll and heading variables according to the values defined in AnitaConventions.h
*/
class UsefulAdu5Pat: public Adu5Pat
{

 public:
  UsefulAdu5Pat(); ///< Default constructor
  UsefulAdu5Pat(Adu5Pat *patPtr, double deltaR, double doubleRL, double doubleUD); ///< Assignment constructor
  UsefulAdu5Pat(Adu5Pat *patPtr); ///< Assignment constructor
  ~UsefulAdu5Pat(); ///< Destructor

  //! For a given azimuthal and elevation angle of a plane wave (in payload coordinates) calculates the point on the Earth's surface that the source would come from.
  /*!
    \param phiWave Azimuthal angle of plane wave (in payload centric coordinates)
    \param thetaWave Elevation angle of plane wave (in payload centric coordinates)
    \param sourceLon Reference to a Double_t in which the longitude of the source will be stored.
    \param sourceLat Reference to a Double_t in which the latitude of the source will be stored.
    \return 1 on successful source localisation, o if the solution does not point to the ground.
  */
  int getSourceLonAndLatAltZero(Double_t phiWave, Double_t thetaWave, Double_t &sourceLon, Double_t &sourceLat);
  
  int getSourceLonAndLatAtAlt(Double_t phiWave, Double_t thetaWave, Double_t &sourceLon, Double_t &sourceLat,Double_t &sourceAltitude);

/*   int getSourceLonAndLatWithBedmap(BedmapReader *bedmapData,Double_t phiWave, Double_t thetaWave,Double_t &sourceLon, Double_t &sourceLat); */
//! For a given source latitude, longitude and altitude calculates the payload centric azimuthal and elevation angles of the plane wave incident at the payload.
  /*!
    
    \param sourceLon The longitude of the source.
    \param sourceLat The latitude of the source.
    \param sourceAlt The altitude of the source.
    \param phiWave Reference to a Double_t in which to store the azimuthal angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)
    \param thetaWave Reference to a Double_t in which to store the elevation angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)
  */
  void getThetaAndPhiWave(Double_t sourceLon, Double_t sourceLat, Double_t sourceAlt, Double_t &thetaWave, Double_t &phiWave);
  //! For a the Williams Field seavey antenna calculates the payload centric azimuthal and elevation angles of the plane wave incident at the payload.
  /*!  
    \param phiWave Reference to a Double_t in which to store the azimuthal angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)
    \param thetaWave Reference to a Double_t in which to store the elevation angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)  
  */
  void getThetaAndPhiWaveWillySeavey(Double_t &thetaWave, Double_t &phiWave);
  //! For a the Williams Field borehole antenna calculates the payload centric azimuthal and elevation angles of the plane wave incident at the payload.
  /*!    
    \param phiWave Reference to a Double_t in which to store the azimuthal angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)
    \param thetaWave Reference to a Double_t in which to store the elevation angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)

  */
  void getThetaAndPhiWaveWillyBorehole(Double_t &thetaWave,Double_t &phiWave);
  //! For a the Taylor Dome seavey antenna calculates the payload centric azimuthal and elevation angles of the plane wave incident at the payload.
  /*!    
    \param phiWave Reference to a Double_t in which to store the azimuthal angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)
    \param thetaWave Reference to a Double_t in which to store the elevation angle of plane wave (in payload centric coordinates with phi equals zero lying in the direction the ADU5 fore antenna)
  */
  void getThetaAndPhiWaveTaylorDome(Double_t &thetaWave, Double_t &phiWave);

  //! For a given source latitude, longitude and altitude calculates the plane wave crossing time difference between two antennas.
  /*!
    \param ant1 The first antenna.
    \param ant2 The second antenna.
    \param sourceLon The longitude of the source.
    \param sourceLat The latitude of the source.
    \param sourceAlt The altitude of the source.
    \return The plane wave crossing time difference between the two antennas (t_1-t_2).
  */
  Double_t getDeltaTExpected(Int_t ant1, Int_t ant2,Double_t sourceLon, Double_t sourceLat, Double_t sourceAlt); 

   //! Calculates the plane wave crossing time difference between two antennas for a given wave phi and theta - useful for testing prior to flight when heading etc not known/needed
  /*!
    \param ant1 The first antenna.
    \param ant2 The second antenna.
    \return The plane wave crossing time difference between the two antennas (t_1-t_2).
  */
  Double_t getDeltaTExpected(Int_t ant1, Int_t ant2,Double_t phiWave, Double_t thetaWave);

   //! Calculates the plane wave crossing time difference between two antennas for a given wave cos and sin of phi and theta - useful for testing prior to flight when heading etc not known/needed, quicker than the above method if calculating deltaT for many phi and theta values - no need to get cos and sin each time
  /*!
    \param ant1 The first antenna.
    \param ant2 The second antenna.
    \return The plane wave crossing time difference between the two antennas (t_1-t_2).
  */
  Double_t getDeltaTExpected(Int_t ant1,Int_t ant2,Double_t cosPhi,Double_t sinPhi,Double_t cosTheta,Double_t sinTheta);
   //! Calculates the plane wave crossing time difference between two antennas for the Williams Field seavey.
  /*!
    \param ant1 The first antenna.
    \param ant2 The second antenna.
    \return The plane wave crossing time difference between the two antennas (t_1-t_2).
 */
  Double_t getDeltaTWillySeavey(Int_t ant1, Int_t ant2); 
  //! Calculates the plane wave crossing time difference between two antennas for the Williams Field borehole.
  /*!
    \param ant1 The first antenna.
    \param ant2 The second antenna.
    \return The plane wave crossing time difference between the two antennas (t_1-t_2).
  */
  Double_t getDeltaTWillyBorehole(Int_t ant1, Int_t ant2);
  //! Calculates the plane wave crossing time difference between two antennas for the Taylor Dome borehole.
  /*!
    \param ant1 The first antenna.
    \param ant2 The second antenna.
    \return The plane wave crossing time difference between the two antennas (t_1-t_2).
  */
  Double_t getDeltaTTaylor(Int_t ant1, Int_t ant2);

  
  Double_t getPhiWave() 
    { return fPhiWave;} ///< Returns the (payload centric with phi equals zero lying along the direction of the ADU5 fore antenna) azimuthal angle last calculated.
  Double_t getThetaWave() 
     { return fThetaWave;} ///< Returns the (payload centric) elevation angle last calculated.
  Double_t getSourceLongitude() 
     { return fSourceLongitude;} ///< Returns the last calculated source longitude.
  Double_t getSourceLatitude() 
     { return fSourceLatitude;} ///< Returns the last calculated source latitude.
  Double_t getSourceAltitude() 
     { return fSourceAltitude;} ///< Returns the last calculated source altitude.
  
  UInt_t getTaylorDomeTriggerTimeNs();

 private:
  TVector3 fSourcePos; ///< Private variable to hold the source location in cartesian coordinates.
  Double_t fSourceLongitude; ///< The source longitude.
  Double_t fSourceLatitude; ///< The source latitude.
  Double_t fSourceAltitude; ///< The source altitude.
  Double_t fThetaWave; ///< The elevation angle of the plane wave in payload centric coordinates.
  Double_t fPhiWave; ///< The azimuthal angle of the plane wave in payload centric coordinates with phi equals zero lying along the direction of the ADU5 fore antenna.
  Double_t fBalloonCoords[3]; ///< The balloon position in cartesian coords
  TVector3 fBalloonPos; ///< The cartesian coords as a TVector3
  Double_t fBalloonTheta; ///< The balloon theta
  Double_t fBalloonPhi; ///< The balloon phi
  Double_t fBalloonHeight; ///< The balloon height

  //optimisation stuff
  Double_t getDeltaTTaylorOpt(Int_t ant1, Int_t ant2, Double_t *deltaR, Double_t *deltaZ, Double_t *deltaPhi);
  Double_t getDeltaTExpectedOpt(Int_t ant1, Int_t ant2,Double_t sourceLon, Double_t sourceLat, Double_t sourceAlt, Double_t *deltaR, Double_t *deltaZ, Double_t *deltaPhi);

 Double_t getDeltaTSeaveyOpt(Int_t ant1, Int_t ant2, Double_t *deltaR, Double_t *deltaZ, Double_t *deltaPhi);
  Double_t getDeltaTExpectedSeaveyOpt(Int_t ant1, Int_t ant2,Double_t sourceLon, Double_t sourceLat, Double_t sourceAlt, Double_t *deltaR, Double_t *deltaZ, Double_t *deltaPhi);

  ClassDef(UsefulAdu5Pat,1); ///< ROOT's magic macro.
};


#endif //USEFULADU5PAT_H