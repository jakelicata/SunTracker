
#include <Wire.h>
//#include <RTClib.h>
#include <math.h>

/**
 * A4988 Stepper Motor Control - 360° Rotation Both Ways
 * Motor specs: 1.8° per step (200 steps per revolution)
 * 
 * Connections:
 * Motor to A4988:
 *   - Black wire → 1A
 *   - Green wire → 1B
 *   - Red wire → 2A
 *   - Blue wire → 2B
 * 
 * Arduino to A4988:
 *   - Pin 12 → !EN (Enable, active low)
 *   - Pin 11 → MS1 (Microstep selection)
 *   - Pin 10 → MS2 (Microstep selection)
 *   - Pin 9 → MS3 (Microstep selection)
 *   - Pin 8 → !RST (Reset, active low)
 *   - Pin 7 → !SLP (Sleep, active low)
 *   - Pin 6 → STEP (Step pulse)
 *   - Pin 5 → DIR (Direction)
 */

// ***************************************
// NOTE NOTE NOTE
// Currently is not using the RTClib and the date/ time is hardcoded
// Additionally the IMU angle is hardcoded, so once we get those parts 
// we can integrate those properly
// ***************************************

//RTC_DS3231 rtc;  // Real-Time Clock module

// Pin definitions
const int enPin = 12;      // Enable pin
const int ms1Pin = 11;     // Microstepping control 1
const int ms2Pin = 10;     // Microstepping control 2
const int ms3Pin = 9;      // Microstepping control 3
const int rstPin = 8;      // Reset pin
const int slpPin = 7;      // Sleep pin
const int stepPin = 6;     // Step pin
const int dirPin = 5;      // Direction pin

// Motor parameters
const int stepsPerRevolution = 200;  // 1.8° per step = 200 steps per revolution
const int stepDelay = 2000;          // Microseconds between steps (speed control)

float motorAngle = 0;
float imu_angle;  // Example initial IMU angle

// Define constants
const float step_deg = 1.8/2;  // Stepper motor moves 0.9 degrees per step

int test_hour = 10;  // Set a fixed time (12:30 PM)
int test_minute = 15;
int test_day_of_year = 56;  // Assume it's day 100 of the year (April 9th)

void setup() {
  Serial.begin(9600);
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
  
   // Set 1/8 microstepping mode (MS1=HIGH, MS2=HIGH, MS3=LOW)
  digitalWrite(ms1Pin, HIGH);
  digitalWrite(ms2Pin, LOW);
  digitalWrite(ms3Pin, LOW);
      
  Serial.println("Sun Tracker Initialized");
  sleepMotor();
}

void loop() {
    float latitude = 39.9526;   // Philadelphia Latitude
    float longitude = -75.1652; // Philadelphia Longitude
    int utc_offset = -5;        // EST (Eastern Standard Time)

    float SEA = getSEA(latitude, longitude, utc_offset);
    float AZ = getAZ(latitude, longitude, utc_offset);

    Serial.print("Solar Elevation Angle: ");
    Serial.println(SEA);
    Serial.print("Solar Azimuth Angle: ");
    Serial.println(AZ);

    
    adjust_motor_az(AZ, imu_angle);
    imu_angle += 5;
    delay(3000);  // Wait 15 minutes (900000 ms) before next cycle
}

void rotateMotor(int steps, int DIR) {
  if(DIR == 1){
    digitalWrite(dirPin, HIGH);
  }
  else{
    digitalWrite(dirPin, LOW);
  }
  for(int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

// Function to calculate the day of the year
int getDayOfYear(int year, int month, int day) {
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Check if it's a leap year (for February 29th)
    bool is_leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (is_leap) {
        days_in_month[1] = 29;
    }

    int day_of_year = 0;
    for (int i = 0; i < month - 1; i++) {
        day_of_year += days_in_month[i];
    }
    day_of_year += day;

    return day_of_year;
}

// Function to get Solar Elevation Angle (SEA)
float getSEA(float latitude, float longitude, int utc_offset) {    
    float hour_minute = (test_hour + (test_minute / 60.0)) - utc_offset;
    float g = (360.0 / 365.25) * (test_day_of_year + hour_minute / 24.0);
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

// Function to get Azimuth Angle (AZ)
float getAZ(float latitude, float longitude, int utc_offset) {
    float hour_minute = (test_hour + (test_minute / 60.0)) - utc_offset;
    float g = (360.0 / 365.25) * (test_day_of_year + hour_minute / 24.0);
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

    // Apply correct quadrant adjustment
    if (SHA_corrected > 0) {
        AZ = 360 - AZ;
    }

    return AZ;
}

// Function to adjust stepper motor based on Azimuth
void adjust_motor_az(float az, float &imu_angle) {
    wakeMotor();
    while (fabs(imu_angle - az) > 2) {
        if (imu_angle < az) {
            motorAngle += step_deg;
            imu_angle += step_deg;
            rotateMotor(1, 1);
        } else {
            motorAngle -= step_deg;
            imu_angle -= step_deg;
            rotateMotor(1, 0);
        }
        Serial.print("Motor Angle: ");
        Serial.println(motorAngle);
        delay(100);  // Small delay to prevent excessive movement
    }
    sleepMotor();
    Serial.println("Position Adjusted");
}

void sleepMotor() {
  digitalWrite(slpPin, LOW);  // Set the sleep pin LOW to enter sleep mode
  Serial.println("Motor in sleep mode");
}

// Function to wake up the motor from sleep mode
void wakeMotor() {
  digitalWrite(slpPin, HIGH);  // Set the sleep pin HIGH to exit sleep mode
  delay(1);                   // Small delay for the driver to wake up
  Serial.println("Motor awake");
}

