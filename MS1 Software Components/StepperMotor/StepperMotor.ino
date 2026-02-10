#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2048;
const int rpm = 10;

// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {}

void loop() {
  myStepper.setSpeed(rpm);
  // stepsPerRevolution can be negated to spin CCW
  myStepper.step(stepsPerRevolution);
  delay(500);
}
