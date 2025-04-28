#include <Wire.h>

// Pin definitions
const int enPin = 12;
const int ms1Pin = 11;
const int ms2Pin = 10;
const int ms3Pin = 9;
const int rstPin = 8;
const int slpPin = 7;
const int stepPin = 6;
const int dirPin = 5;

// Motor and mechanical constants
const int stepsPerRevolution = 11550; // 1 full elevation cycle = ~11550 steps
const float STEPS_PER_ELEV_DEGREE = 11550.0 / 180.0;
const int stepDelay = 1000; // microseconds

// State
float virtualElevation = 0.0; // Elevation angle estimate in degrees
float virtualAzimuth = 0.0;   // Azimuth angle estimate in degrees
bool initialCyclesDone = false;

void setup() {
  Serial.begin(9600);

  // Configure pins
  pinMode(enPin, OUTPUT);
  pinMode(ms1Pin, OUTPUT);
  pinMode(ms2Pin, OUTPUT);
  pinMode(ms3Pin, OUTPUT);
  pinMode(rstPin, OUTPUT);
  pinMode(slpPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // Driver setup
  digitalWrite(enPin, LOW);
  digitalWrite(rstPin, HIGH);
  digitalWrite(slpPin, HIGH);
  digitalWrite(ms1Pin, LOW);
  digitalWrite(ms2Pin, LOW);
  digitalWrite(ms3Pin, LOW);

  Serial.println("System initialized. Enter 'cycle', 'rotate <deg> <cw/ccw>', 'set <deg>', 'status', or 'reset'.");
  Serial.println("Enter a command: 'cycle', 'rotate <deg> <cw/ccw>', 'set <deg>', 'status', or 'reset'.");
}

void loop() {
  if (!initialCyclesDone) {
    for (int i = 0; i < 2; i++) {
      Serial.println("Cycle " + String(i + 1) + ": Up");
      rotateMotor(stepsPerRevolution, 1);
      Serial.println("Cycle " + String(i + 1) + ": Down");
      rotateMotor(stepsPerRevolution, 0);
    }
    virtualElevation = 0.0;
    virtualAzimuth = 0.0;
    initialCyclesDone = true;
    Serial.println("Initial cycles done. Elevation and Azimuth reset to 0.");
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "cycle") {
      rotateMotor(stepsPerRevolution, 1);
      rotateMotor(stepsPerRevolution, 0);
      virtualElevation = 0.0;
      virtualAzimuth = 0.0;
    } else if (input == "status") {
      Serial.print("Current Azimuth: ");
      Serial.print(virtualAzimuth, 2);
      Serial.print("°, Elevation: ");
      Serial.print(virtualElevation, 2);
      Serial.println("°");
    } else if (input == "reset") {
      virtualElevation = 0.0;
      virtualAzimuth = 0.0;
      Serial.println("Virtual elevation and azimuth reset to 0.0. Make sure panel is physically at home position!");
    } else if (input.startsWith("rotate ")) {
      int spaceIndex = input.indexOf(' ', 7);
      if (spaceIndex != -1) {
        String degreesStr = input.substring(7, spaceIndex);
        String directionStr = input.substring(spaceIndex + 1);
        int degrees = degreesStr.toInt();

        if (degrees > 0) {
          int dir = (directionStr == "ccw") ? 1 : (directionStr == "cw") ? 0 : -1;
          if (dir == -1) {
            Serial.println("Invalid direction. Use 'cw' or 'ccw'.");
            return;
          }

          int steps = degrees * STEPS_PER_ELEV_DEGREE;
          rotateMotor(steps, dir);

          if (dir == 1) {
            float target = virtualElevation + degrees;
            if (target > 180) {
              Serial.println("Command would exceed 0-180 degree range. Command ignored.");
              return;
            }
            virtualElevation = target;
          } else {
            virtualAzimuth += degrees;
            if (virtualAzimuth >= 360) virtualAzimuth -= 360;
          }

          Serial.print("Rotated ");
          Serial.print(degrees);
          Serial.print(" degrees ");
          Serial.print(directionStr);
          Serial.print(". Azimuth: ");
          Serial.print(virtualAzimuth, 2);
          Serial.print("°, Elevation: ");
          Serial.print(virtualElevation, 2);
          Serial.println("°");
        }
      }
    } else if (input.startsWith("set ")) {
      String degreesStr = input.substring(4);
      int target = degreesStr.toInt();

      if (target >= 0 && target <= 180) {
        float delta = target - virtualElevation;
        int dir = (delta > 0) ? 1 : 0;
        int steps = abs(delta) * STEPS_PER_ELEV_DEGREE;
        rotateMotor(steps, dir);
        virtualElevation = target;
        Serial.print("Moved to target elevation: ");
        Serial.print(target);
        Serial.print("°. Azimuth: ");
        Serial.print(virtualAzimuth, 2);
        Serial.print("°, Elevation: ");
        Serial.print(virtualElevation, 2);
        Serial.println("°");
      } else {
        Serial.println("Invalid target. Must be between 0 and 180.");
      }
    } else {
      Serial.println("Unknown command. Try 'cycle', 'rotate <deg> <cw/ccw>', 'set <deg>', 'status', or 'reset'.");
    }
  }
}

void rotateMotor(int steps, int DIR) {
  digitalWrite(dirPin, DIR == 1 ? HIGH : LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}
