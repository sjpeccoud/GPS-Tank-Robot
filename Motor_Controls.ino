

void driveOnHeading(double target, double heading) {
  int correction = getCorrection(heading_error(target, heading));
  Serial.print(" Current heading: "); Serial.println(heading);
  setMotorsSpeed(cruiseSpeed - correction, cruiseSpeed + correction);
}

void setMotorsSpeed(int speedRight, int speedLeft) {
  //Controlling speed (0 = off and 255 = max speed):
  Serial.print("Left: "); Serial.print(speedLeft); Serial.print(" Right: "); Serial.println(speedRight);
  setRightMotorSpeed(speedRight);
  setLeftMotorSpeed(speedLeft);
}
void setRightMotorSpeed(int speed) {
  if(speed > 255) {
    speed = 255;
  } else if(speed < -255) {
    speed = -255;
  }
  bool direct;
  if (speed < 0) {
    direct = rightMotorBackward;
  } else {
    direct = rightMotorForward;
  }
  setRightMotorDirection(direct);
  
  analogWrite(rightMotorEN, abs(speed)); //ENA pin
}

void setLeftMotorSpeed(int speed) {
  if(speed > 255) {
    speed = 255;
  } else if(speed < -255) {
    speed = -255;
  }
  bool direct;
  if (speed < 0) {
    direct = leftMotorBackward;
  } else {
    direct = leftMotorForward;
  }
  setLeftMotorDirection(direct);
  analogWrite(leftMotorEN, abs(speed)); //ENB pin
}

void setRightMotorDirection(bool direct) {
  digitalWrite(rightMotorPin1, direct ? HIGH : LOW);
  digitalWrite(rightMotorPin2, !direct ? HIGH : LOW);
}

void setLeftMotorDirection(bool direct) {
  digitalWrite(leftMotorPin1, direct ? HIGH : LOW);
  digitalWrite(leftMotorPin2, !direct ? HIGH : LOW);
}
