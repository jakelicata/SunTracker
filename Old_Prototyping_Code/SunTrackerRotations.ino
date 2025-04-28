#include <Wire.h>

/**
 * Simplified A4988 Stepper Motor Control - 360° Rotation Both Ways
 * Motor specs: 17HS19-1684S-PG27 Nema 17 with 26.85:1 gearbox
 * Step angle: 0.067° per step (5373 steps per revolution)
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

// Motor parameters for geared motor
const int stepsPerRevolution = 11000;  // 0.067° per step = 5373 steps per revolution
const int stepDelay = 1000;           // Microseconds between steps (speed control)

// Global variable to track initial cycles
bool initialCyclesDone = false;

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
  
  // Set full step mode for more torque
  digitalWrite(ms1Pin, LOW);
  digitalWrite(ms2Pin, LOW);
  digitalWrite(ms3Pin, LOW);
  
  Serial.println("Simple 360° Rotation Initialized");
}

void loop() {
  // Perform the initial two cycles
  if (!initialCyclesDone) {
    for (int i = 0; i < 2; i++) {
      Serial.println("Cycle " + String(i + 1) + ": Rotating 360 degrees counterclockwise");
      rotateMotor(stepsPerRevolution, 1);  // 1 for counterclockwise
      delay(1000);  // Wait 1 second
      
      Serial.println("Cycle " + String(i + 1) + ": Rotating 360 degrees clockwise");
      rotateMotor(stepsPerRevolution, 0);  // 0 for clockwise
      delay(1000);  // Wait 1 second
    }
    initialCyclesDone = true;
    Serial.println("Initial two cycles completed. Enter 'cycle' for another cycle or 'rotate <degrees> <direction>' (e.g., 'rotate 90 cw')");
  } 
  // After initial cycles, wait for user input
  else {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();  // Remove leading/trailing whitespace
      
      if (input == "cycle") {
        // Perform one additional cycle
        Serial.println("Performing one cycle: Rotating 360 degrees counterclockwise");
        rotateMotor(stepsPerRevolution, 1);
        delay(1000);
        Serial.println("Rotating 360 degrees clockwise");
        rotateMotor(stepsPerRevolution, 0);
        delay(1000);
      } 
      else if (input.startsWith("rotate ")) {
        // Parse custom rotation command
        int spaceIndex = input.indexOf(' ', 7);  // Find space after "rotate "
        if (spaceIndex != -1) {
          String degreesStr = input.substring(7, spaceIndex);
          String directionStr = input.substring(spaceIndex + 1);
          int degrees = degreesStr.toInt();
          
          if (degrees > 0) {  // Ensure degrees is positive
            int dir;
            if (directionStr == "cw") {
              dir = 0;  // Clockwise
            } 
            else if (directionStr == "ccw") {
              dir = 1;  // Counterclockwise
            } 
            else {
              Serial.println("Invalid direction. Use 'cw' or 'ccw'.");
              return;
            }
            int steps = (degrees * stepsPerRevolution) / 360;
            Serial.println("Rotating " + String(degrees) + " degrees " + directionStr);
            rotateMotor(steps, dir);
          } 
          else {
            Serial.println("Invalid degrees. Must be a positive integer.");
          }
        } 
        else {
          Serial.println("Invalid input format. Use 'rotate <degrees> <direction>'");
        }
      } 
      else {
        Serial.println("Invalid command. Use 'cycle' or 'rotate <degrees> <direction>'");
      }
      // Prompt for next command
      Serial.println("Enter next command:");
    }
  }
}

void rotateMotor(int steps, int DIR) {
  // Set direction
  if (DIR == 1) {
    digitalWrite(dirPin, HIGH);  // Counterclockwise
  } 
  else {
    digitalWrite(dirPin, LOW);   // Clockwise
  }
  
  // Execute steps
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}
