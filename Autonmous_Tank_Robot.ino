#include <Wire.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GPS.h>

#define Kp 1
#define Ki 0.1
#define Kd 0.001

#define rightMotorPin1 2
#define rightMotorPin2 3

#define leftMotorPin1 4
#define leftMotorPin2 5

// Set up these variables to real world
#define rightMotorForward false
#define rightMotorBackward true

#define leftMotorForward true
#define leftMotorBackward false

#define rightMotorEN 9
#define leftMotorEN 10

#define PRECISION 0.000009
#define LowerSpeedThreshold 0.00007

#define magXOffset 23.95
#define magYOffset -13.82

#define loopTime 1000

#define GPSECHO  true

double currentLatitude, currentLongitude;
int sumError = 0;
int prevError;
int tempPrevError;
bool clampI = false;
int numSecondsStuck = 0;

int cruiseSpeed = 255;
int ledState = LOW;
int ledPin = 13;

int missionPointer = 0;
                         // Latitude  // Longitude
double mission[1][2] = {{40.558852, -105.050925}};

//                          { 40.558830, -105.050920},
//                          {40.559014, -105.050897},
//                          {40.558903, -105.050905},

Adafruit_LIS3MDL lis3mdl;

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

double timer = millis();

void setup() {
  Serial.begin(115200);

  configureGPS();

  if (! lis3mdl.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Failed to find LIS3MDL chip");
    while (1) { delay(10); }
  }

  configureMag(lis3mdl);
  
  // Motor direction control pins
  pinMode(ledPin, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // Motor Speed control pins (uses PWM)
  pinMode(rightMotorEN, OUTPUT); 
  pinMode(leftMotorEN, OUTPUT);
  
}

void loop() {

  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    //Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
  if (millis() - timer > 1000 && GPS.fix) {
    if (abs(getNextWaypointLongitude() - currentLongitude) <= LowerSpeedThreshold && 
      abs(getNextWaypointLatitude() - currentLatitude) <= LowerSpeedThreshold) {
        if (GPS.speed < 0.2) {
          cruiseSpeed = 255;
        } else {
          cruiseSpeed = 150;
        }
     }
      //Serial.print("Speed: "); Serial.println(GPS.speed);
     if (GPS.speed < 0.25) {
      numSecondsStuck++;
     } else {
      numSecondsStuck = 0;
     }

     if(numSecondsStuck >= 5) {
      setMotorsSpeed(-200, -200);
      delay(2000);
     }
     
    timer = millis(); // reset the timer
    sensors_event_t event; 
    lis3mdl.getEvent(&event);
    updatePosition();
    double target = getWaypointHeading();
    driveOnHeading(target, getHeading(event.magnetic.x + magXOffset, event.magnetic.y + magYOffset));
    checkWaypointDistance();
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
  
}
