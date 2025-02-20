// Pin definitions for TB238A driver
const int PWMA = 13;    // Enable pin for A phase
const int A2_PIN = 12;  // A- pin
const int A1_PIN = 11;  // A+ pin
const int B1_PIN = 10;  // B+ pin
const int B2_PIN = 9;   // B- pin
const int PWMB = 8;     // Enable pin for B phase
const int STBY = 7;     // Standby pin

// Timing configuration (milliseconds)
const int stepDelay = 50;        // Delay between steps
const int coolingPeriod = 100;   // Short pause between sequences
const int powerLevel = 128;      // PWM value (0-255) to reduce power

void setup() {
  // Configure all pins as outputs
  pinMode(PWMA, OUTPUT);
  pinMode(A2_PIN, OUTPUT);
  pinMode(A1_PIN, OUTPUT);
  pinMode(B1_PIN, OUTPUT);
  pinMode(B2_PIN, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  // Set PWM on enable pins to reduce power
  analogWrite(PWMA, powerLevel);
  analogWrite(PWMB, powerLevel);
}

void enableDriver() {
  digitalWrite(STBY, HIGH);  // Take out of standby
  delay(1);  // Short delay for stable operation
}

void disableDriver() {
  digitalWrite(STBY, LOW);   // Put into standby
}

void step(int stepNumber) {
  enableDriver();  // Make sure driver is active
  
  switch(stepNumber) {
    case 0:  // Step 1 in diagram
      digitalWrite(A1_PIN, HIGH);   // A+
      digitalWrite(A2_PIN, LOW);    // A-
      digitalWrite(B1_PIN, HIGH);   // B+
      digitalWrite(B2_PIN, LOW);    // B-
      break;
    case 1:  // Step 2 in diagram
      digitalWrite(A1_PIN, LOW);    // A-
      digitalWrite(A2_PIN, HIGH);   // A+
      digitalWrite(B1_PIN, HIGH);   // B+
      digitalWrite(B2_PIN, LOW);    // B-
      break;
    case 2:  // Step 3 in diagram
      digitalWrite(A1_PIN, LOW);    // A-
      digitalWrite(A2_PIN, HIGH);   // A+
      digitalWrite(B1_PIN, LOW);    // B-
      digitalWrite(B2_PIN, HIGH);   // B+
      break;
    case 3:  // Step 4 in diagram
      digitalWrite(A1_PIN, HIGH);   // A+
      digitalWrite(A2_PIN, LOW);    // A-
      digitalWrite(B1_PIN, LOW);    // B-
      digitalWrite(B2_PIN, HIGH);   // B+
      break;
  }
}

void powerDown() {
  // Disable all outputs and put into standby
  digitalWrite(A1_PIN, LOW);
  digitalWrite(A2_PIN, LOW);
  digitalWrite(B1_PIN, LOW);
  digitalWrite(B2_PIN, LOW);
  disableDriver();
}

void loop() {
  // Rotate clockwise for one sequence
  for(int i = 0; i < 4; i++) {
    step(i);
    delay(stepDelay);
  }
  
  // Cool down period with standby mode
  powerDown();
  delay(coolingPeriod);
}
