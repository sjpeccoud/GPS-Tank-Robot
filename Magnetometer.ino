
void configureMag(Adafruit_LIS3MDL lis3mdl) {
  lis3mdl.setPerformanceMode(LIS3MDL_ULTRAHIGHMODE);
  lis3mdl.setDataRate(LIS3MDL_DATARATE_155_HZ);
  lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
  lis3mdl.setIntThreshold(500);
  lis3mdl.configInterrupt(false, false, true, // enable z axis
                          true, // polarity
                          false, // don't latch
                          true); // enabled!
}

// routine to calculate heading error in degrees, taking into account compass wrap

int getCorrection(int error) {
  int iBlock = 0;
  prevError = tempPrevError;
  tempPrevError = error;
  if (!clampI) {
    sumError += error;
    iBlock = Ki * sumError;
  }
  int correction = Kp * error + iBlock + Kd * ((error - prevError)/1);
  if (correction > 255) {
    if ((error > 0 && iBlock > 0) || (error < 0 && iBlock < 0)) {
      clampI = true;
    } else {
      clampI = false;
    } 
  }
  return correction;
}

int heading_error(int bearing, int current_heading)
{
 int error = current_heading - bearing;
 if (error >  180) error -= 360;
 if (error < -180) error += 360;
 return error;
}

double getHeading(double magX, double magY) {
  float heading = atan2(magY, magX) * 180/PI;
  heading += 8;
  if (heading >  180) heading -= 360;
  if (heading < -180) heading += 360;
  //heading;
  //Serial.print(heading); Serial.print("  X: "); Serial.print(magX); Serial.print("  Y: "); Serial.println(magY);
  return heading;
}
