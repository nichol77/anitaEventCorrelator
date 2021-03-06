

void testConversion()
{

  for (double lat = -88; lat <= -88; lat+=1) 
  {
    for (double lon = -1; lon <= 1; lon+=0.1) 
    {
      printf("Lat: %g, Lon: %g\n", lat,lon); 

      AntarcticCoord c(AntarcticCoord::WGS84, lat, lon, 1000); 

      AntarcticCoord direct = c.as(AntarcticCoord::STEREOGRAPHIC); 
      printf("  Direct Stereographic: (%g %g %g)\n", direct.x, direct.y, direct.z); 

      AntarcticCoord indirect = c.as(AntarcticCoord::CARTESIAN).as(AntarcticCoord::STEREOGRAPHIC); 
      printf("  Indirect Stereographic: (%g %g %g)\n", indirect.x, indirect.y, indirect.z); 

      printf("  Delta:  (%g, %g, %g)\n", direct.x - indirect.x, direct.y - indirect.y, direct.z - indirect.z); 

      direct = c.as(AntarcticCoord::CARTESIAN); 
      printf("  Direct Cartesian: (%g %g %g)\n", direct.x, direct.y, direct.z); 

      indirect = c.as(AntarcticCoord::STEREOGRAPHIC).as(AntarcticCoord::CARTESIAN); 
      printf("  Indirect Cartesian: (%g %g %g)\n", indirect.x, indirect.y, indirect.z); 
      printf("  Delta:  (%g, %g, %g)\n", direct.x - indirect.x, direct.y - indirect.y, direct.z - indirect.z); 
    }
  }



}
