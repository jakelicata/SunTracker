#include <Wire.h>
#include <math.h>
#include "RTClib.h"

RTC_DS3231 rtc;


/* 
  IMPORTANT (prior to running):
  Align lines of elevation (as pictured in ReadMe)
  Start system facing North (the normal of solar panel)
*/

/*
   Simplified A4988 Stepper Motor Control - 360° Rotation Both Ways
   Motor specs: 17HS19-1684S-PG27 Nema 17 with 26.85:1 gearbox
   Step angle: 0.067° per step (5373 steps per revolution)
 */


// Pin definitions
const int enPin = 12;      // Enable pin
const int ms1Pin = 11;     // Microstepping control 1
const int ms2Pin = 10;     // Microstepping control 2
const int ms3Pin = 9;      // Microstepping control 3
const int rstPin = 8;      // Reset pin
const int slpPin = 7;      // Sleep pin
const int stepPin = 6;     // Step pin
const int dirPin = 5;      // Direction pin



// Motor definitions : (*note* steps per revolution difer from specs due to gear-ratios of system)
const int stepsPerRevolution = 11290; 
const int stepDelaySimultaion = 1500; 


// setting intial azimuth and elevation of panel
int az_degree = 0;
int el_degree = 18; // panel starts at around 18 degrees (lowest possible elevation of system)
String season = ""; //  initializing the season variable


// function used instead of delay, that allows motor no strain driver during long delays
void sleep(int ms) {
  digitalWrite(slpPin, LOW);  // Enter sleep mode — chip shuts down
  delay(ms);
  digitalWrite(slpPin, HIGH); // Wake up
  delay(1);                     // Wait for charge pump to stabilize  
}


// checks if its a leap year
bool isLeapYear(int year) {
  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// function to get the current elevation of the sun (based on Ken Willmott SolarPosition library)
float getSEA(int dayOfYear, int hour, int minute, float latitude, float longitude, int utc_offset) {    
    float hour_minute = (hour + (minute / 60.0)) - utc_offset;
    float g = (360.0 / 365.25) * (dayOfYear + hour_minute / 24.0);
    float g_radians = radians(g);

    float declination = 0.396372 - 22.91327 * cos(g_radians) + 4.02543 * sin(g_radians) 
                        - 0.387205 * cos(2 * g_radians) + 0.051967 * sin(2 * g_radians) 
                        - 0.154527 * cos(3 * g_radians) + 0.084798 * sin(3 * g_radians);

    float time_correction = 0.004297 + 0.107029 * cos(g_radians) - 1.837877 * sin(g_radians) 
                            - 0.837378 * cos(2 * g_radians) - 2.340475 * sin(2 * g_radians);

    float SHA = (hour_minute - 12) * 15 + longitude + time_correction;
    float SHA_corrected = (SHA > 180) ? (SHA - 360) : (SHA < -180 ? SHA + 360 : SHA);

    float lat_radians = radians(latitude);
    float d_radians = radians(declination);
    float SHA_radians = radians(SHA_corrected);

    float SZA_radians = acos(sin(lat_radians) * sin(d_radians) + cos(lat_radians) * cos(d_radians) * cos(SHA_radians));
    float SEA = 90.0 - degrees(SZA_radians);

    return SEA;
}


// function to get the current azimuth angle of the sun (based on Ken Willmott SolarPosition library)
float getAZ(int dayOfYear, int hour, int minute, float latitude, float longitude, int utc_offset) {
    float hour_minute = (hour + (minute / 60.0)) - utc_offset;
    float g = (360.0 / 365.25) * (dayOfYear + hour_minute / 24.0);
    float g_radians = radians(g);

    float declination = 0.396372 - 22.91327 * cos(g_radians) + 4.02543 * sin(g_radians) 
                        - 0.387205 * cos(2 * g_radians) + 0.051967 * sin(2 * g_radians) 
                        - 0.154527 * cos(3 * g_radians) + 0.084798 * sin(3 * g_radians);

    float time_correction = 0.004297 + 0.107029 * cos(g_radians) - 1.837877 * sin(g_radians) 
                            - 0.837378 * cos(2 * g_radians) - 2.340475 * sin(2 * g_radians);

    float SHA = (hour_minute - 12) * 15 + longitude + time_correction;
    float SHA_corrected = (SHA + 180) - floor((SHA + 180) / 360.0) * 360 - 180;

    float lat_radians = radians(latitude);
    float d_radians = radians(declination);
    float SHA_radians = radians(SHA_corrected);

    float SZA_radians = acos(sin(lat_radians) * sin(d_radians) + cos(lat_radians) * cos(d_radians) * cos(SHA_radians));
    
    float cos_AZ = (sin(d_radians) - sin(lat_radians) * cos(SZA_radians)) / (cos(lat_radians) * sin(SZA_radians));
    cos_AZ = constrain(cos_AZ, -1.0, 1.0);  // Ensuring valid range

    float AZ_rad = acos(cos_AZ);
    float AZ = degrees(AZ_rad);

    // quadrant adjustment
    if (SHA_corrected > 0) {
        AZ = 360 - AZ;
    }

    return AZ;
}


// function to determine the time of the next sunrise
float calculateSunrise(int dayOfYear, float latitude, float longitude, int utc_offset) {
  // loop through each minute of the next 24 hours until elevation angle of sun > threshold (e.g. 5°) (sun is rising)
  for (int h = 0; h < 24; h++) {
    for (int m = 0; m < 60; m++) {
      float SEA = getSEA(dayOfYear, h, m, latitude, longitude, utc_offset);
      if (SEA > 5) {
        return h + (m / 60.0);  
      }
    }
  }
  return 6.0;  // if for some reason code fails, maunally set sunrise time 
}



// function that recognizes when the dayOfYear signifies a change of season
void seasonalChangeElevation(int dayOfYear) {
  //for each case, recognize if there is a season change, then update variables and change elevation accordingly
  switch(dayOfYear){
    case 79: //winter -> spring
      rotateMotor(5300,1);
      delay(1000);
      season = "spring";
      el_degree = 47;
      rotateMotor(540, 0); // first click
      delay(1000);
      az_degree += 12.4;
      break;
    case 171: //spring -> summer
      rotateMotor(2000,1);
      delay(1000);
      el_degree = 61;
      season = "summer";
      rotateMotor(540, 0); // first click
      delay(1000);
      az_degree += 12.4;
      break;
    case 265: //summer -> fall
      rotateMotor(1450,1);
      delay(1000);
      el_degree = 47;
      season = "fall";
      rotateMotor(540, 0); // first click
      delay(1000);
      az_degree += 12.4;
      break;
    case 355: //fall -> winter
      rotateMotor(1050,1);
      delay(1000);
      el_degree = 36;
      season = "winter";
      rotateMotor(540, 0); // first click
      delay(1000);
      az_degree += 12.4;
      break;
  }
  Serial.print("Season Adjusted to...");
  Serial.println(season); 
  Serial.print("System Azimuth: ");
  Serial.println(az_degree); 
  Serial.print("System Elevation: ");
  Serial.println(el_degree); 
}




// this adjusts the solar panel to the proper azimuth, and makes calls to rotatemotor to actually move the motor
void adjust_motor_az(float az, float az_angle) {
    while (fabs(az_angle - az) > 12.4) {
        if (az_angle < az) {
            Serial.println("Step Clockwise");
            rotateMotor(390, 0);
            delay(1000);
            az_degree += 12.4;
            az_angle += 12.4;
        } 
    }
    Serial.print("System Azimuth: ");
    Serial.println(az_degree); 
    Serial.print("System Elevation: ");
    Serial.println(el_degree);
}


// rotates the motor a certain ammount clockwise or counterclockwise based on input (either azimuth or elevation)
void rotateMotor(int steps, int DIR) {
  // Set direction
  if(DIR == 1){
    digitalWrite(dirPin, HIGH);  // Counterclockwise
  }
  else{
    digitalWrite(dirPin, LOW);   // Clockwise
  }

  // Execute steps
  for(int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelaySimultaion);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelaySimultaion);
  }
}

bool isDST(DateTime now) {
  int month = now.month();
  int day = now.day();
  int dow = now.dayOfTheWeek(); 

  // DST starts: Second Sunday in March
  if (month < 3 || month > 11) return false;  // Jan, Feb, Dec -> standard time
  if (month > 3 && month < 11) return true;   // Apr-Oct -> daylight time

  // March or November, need to check
  if (month == 3) {
    int secondSunday = 14 - (DateTime(now.year(), 3, 1).dayOfTheWeek());
    return (day >= secondSunday);
  }
  if (month == 11) {
    int firstSunday = 7 - (DateTime(now.year(), 11, 1).dayOfTheWeek());
    return (day < firstSunday);
  }

  return false; // fallback
}

void returnToNorth(){
  while (az_angle != 359.6) {
        rotateMotor(390, 0);
    }
  az_degree = 0;
}


void setup() {
  Serial.begin(115200);
  delay(500);  // Give time for Serial
  Serial.println("Serial started");
  
  // Configure pins as outputs
  pinMode(enPin, OUTPUT);
  pinMode(ms1Pin, OUTPUT);
  pinMode(ms2Pin, OUTPUT);
  pinMode(ms3Pin, OUTPUT);
  pinMode(rstPin, OUTPUT);
  pinMode(slpPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  
  // Initialize the driver
  digitalWrite(enPin, LOW);    // Enable the driver (active low)
  digitalWrite(rstPin, HIGH);  // Not in reset mode
  digitalWrite(slpPin, HIGH);  // Not in sleep mode
  
  // Set Full step mode for more torque
  digitalWrite(ms1Pin, LOW);
  digitalWrite(ms2Pin, LOW);
  digitalWrite(ms3Pin, LOW);

  // ensuring clock module is working
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1); // stops
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // <<<<----- sets time to compile time (good for if unplugged) - may be a couple sec behind
  Serial.println("RTC date is set");


  Serial.println("Moving Suntracker into Initial Position");

  //// elevate to average spring fo State College (~49 degrees) (NOTE THIS WILL NEED TO BE ADJUSTED IF MOVED ELSEWHERE)
  rotateMotor(1800, 1);
  delay(1000);
  el_degree = 49;

  //move motor one step to lock in place for accurate azimuth rotations
  rotateMotor(540, 0); // first click
  delay(1000);
  az_degree = az_degree + 12.4;

  // find day of the year and account for leap year
  DateTime time = rtc.now();
  int dayOfYear = (time.unixtime() - DateTime(time.year(), 1, 1).unixtime()) / 86400 + 1;
  if (isLeapYear(time.year()) && time.month() > 2) {
      dayOfYear += 1;
  }


  // if its not spring, continue to adjust elevation until it is accurate
  if(dayOfYear > 171 and dayOfYear <= 265){
    rotateMotor(2000,1);
    delay(1000);
    az_degree = 61;
    season = "summer";
    rotateMotor(540, 0); // first click
    delay(1000);
    az_degree += 12.4;
  }else if(dayOfYear > 265 and dayOfYear <= 365){
    rotateMotor(2000,1);
    delay(1000);
    az_degree = 61;
    season = "summer";

    rotateMotor(540, 0); // first click
    delay(1000);
    az_degree += 12.4;

    rotateMotor(1450,1);
    delay(1000);
    az_degree = 47;
    season = "fall";

    rotateMotor(540, 0); // first click
    delay(1000);
    az_degree += 12.4;
    
  }else if(dayOfYear > 365 or dayOfYear <= 79){
    rotateMotor(2000,1);
    delay(1000);
    az_degree = 61;
    season = "summer";
    
    rotateMotor(540, 0); // first click
    delay(1000);
    az_degree += 12.4;

    rotateMotor(1450,1);
    delay(1000);
    az_degree = 47;
    season = "fall";

    rotateMotor(540, 0); // first click
    delay(1000);
    az_degree += 12.4;

    rotateMotor(1050,1);
    delay(1000);
    el_degree = 36;
    season = "winter";
    rotateMotor(540, 0); // first click
    delay(1000);
    az_degree += 12.4;
  }
}




void loop() {

  // location and time offset of solar panel
  float latitude = 40.7934;   // State College Latitude
  float longitude = -77.8600; // State College Longitude
  int utc_offset;

  DateTime time = rtc.now();

  // need to adjust these statements depending on where tracker is placed
  if (isDST(time)) {
    utc_offset = -4; // Eeastern Daylight Time
  } else {
    utc_offset = -5; // Eastern Standard Time
  }

 
  // find day of year and adjust for leap year
  int dayOfYear = (time.unixtime() - DateTime(time.year(), 1, 1).unixtime()) / 86400 + 1;
  if (isLeapYear(time.year()) && time.month() > 2) {
    dayOfYear += 1;
  }

  // find hour and minute
  int hour = time.hour();
  int minute = time.minute();

  Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+time.timestamp(DateTime::TIMESTAMP_FULL));

  // determine if seasonal change is needed
  if (dayOfYear == 79 or dayOfYear == 171 or dayOfYear == 265 or dayOfYear == 355){
      seasonalChangeElevation(dayOfYear);
  }

  // updated position of sun 
  float SEA = getSEA(dayOfYear, hour, minute, latitude, longitude, utc_offset);
  float AZ = getAZ(dayOfYear, hour, minute, latitude, longitude, utc_offset);

  // if the sun has set, determine how long to sleep system for (until next sunrise)
  if (SEA < 5) {
    Serial.println("Night mode active: putting motor to sleep.");
    digitalWrite(slpPin, LOW);  // Sleep mode
    float sunrise = calculateSunrise(dayOfYear, latitude, longitude, utc_offset);
    float nightTime = (24-hour) + sunrise;
    float waitTime = (60000 * nightTime);
    delay(waitTime);
    returnToNorth(); //move the panel back to facing North
    return;
  } 



  //adjust motor according to position of sun
  adjust_motor_az(AZ, az_degree);

  // wait 100 seconds
  sleep(100000);

}
