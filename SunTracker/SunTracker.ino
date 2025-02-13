#define STEP_OUTER 3
#define DIR_OUTER  2

// Define Arduino pins connected to A4988 (Inner Shaft)
#define STEP_INNER 5
#define DIR_INNER  4

void setup() {
  // put your setup code here, to run once:
  pinMode(STEP_OUTER, OUTPUT);
  pinMode(DIR_OUTER, OUTPUT);
  pinMode(STEP_INNER, OUTPUT);
  pinMode(DIR_INNER, OUTPUT);

  //set initial direction
  digitalWrite(DIR_OUTER, HIGH); // outer shaft clockwise
  digitalWrite(DIR_INNER, LOW);  // inner shaft counterclockwise
}

void loop() {
  // put your main code here, to run repeatedly:
  //step both motors
  digitalWrite(STEP_OUTER, HIGH);
  digitalWrite(STEP_INNER, HIGH);
  delayMicroseconds(1000);  // Adjust for speed
  digitalWrite(STEP_OUTER, LOW);
  digitalWrite(STEP_INNER, LOW);
  delayMicroseconds(1000);
}
