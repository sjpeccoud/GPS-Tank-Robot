
void configureGPS() {
  delay(1000);
  Serial.println("Initializing GPS...");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
}

void updatePosition() {
  currentLatitude = toDD(GPS.latitude, GPS.lat);
  currentLongitude = toDD(GPS.longitude, GPS.lon);
  Serial.print(currentLatitude, 6);
  Serial.print(", ");
  Serial.println(currentLongitude, 6);
}

double getNextWaypointLatitude() {
  return mission[missionPointer][0];
}

double getNextWaypointLongitude() {
  return mission[missionPointer][1];
}

double toDD (double coord, char direct) {
  double degree = (int)coord / 100;
  double minutes = coord - degree*100;
  //Serial.println(minutes);
  //double secs = (coord - (int)coord) * 100;
  
    return -(degree + minutes/60.00);
  
  //return degree + minutes/60.00;
}

double getWaypointHeading() {
  double longit = getNextWaypointLongitude() - currentLongitude;
  double latit = getNextWaypointLatitude() - currentLatitude;
  
  double rad = atan2(longit, latit);
  
  double deg = rad * (180 / PI);
  Serial.println(deg, 6);
  return  deg;
}

void checkWaypointDistance() {
  
  if (abs(getNextWaypointLongitude() - currentLongitude) <= PRECISION && 
      abs(getNextWaypointLatitude() - currentLatitude) <= PRECISION) {
     
    missionPointer++;
    cruiseSpeed = 255;
    if(missionPointer == sizeof(mission)/sizeof(mission[0])) {
      // Stops Program Mission Complete
      setMotorsSpeed(0, 0);
      digitalWrite(ledPin, HIGH);
      //Serial.println("MISSION COMPLETE");
      while(1);
    }
  }
}
