//Includes the Arduino Stepper Library
#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2048;
const int rpm = 10;

const int extend_pin_in1 = 12;
const int retract_pin_in2 = 13;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
//Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {

  Serial.begin(9600); // Start serial communication at 9600 baud rate
  Serial.println("Enter a character:");

  pinMode(extend_pin_in1, OUTPUT);
  pinMode(retract_pin_in2, OUTPUT);
  digitalWrite(extend_pin_in1, LOW);
  digitalWrite(retract_pin_in2, LOW);
}

void loop() {

  if (Serial.available() > 0) { // Check if data has been sent from the computer
    char incomingByte = Serial.read(); // Read the oldest byte in the serial buffer

    if(incomingByte == 'E') {
      Serial.print("extend\n");
      digitalWrite(extend_pin_in1, HIGH);
      digitalWrite(retract_pin_in2, LOW);
    }
    else if(incomingByte == 'R') {
      Serial.print("retract\n");
      digitalWrite(extend_pin_in1, LOW);
      digitalWrite(retract_pin_in2, HIGH);
    } else if(incomingByte == 'S') {
      digitalWrite(extend_pin_in1, LOW);
      digitalWrite(retract_pin_in2, LOW);
    }
  }

  // analogWrite(extend_pin_in1, 255);
  // analogWrite(retract_pin_in2, 0);
  // digitalWrite(extend_pin_in1, HIGH);
  // digitalWrite(retract_pin_in2, LOW);

  //digitalWrite(extend_pin_in1, HIGH);
  //digitalWrite(retract_pin_in2, LOW);
  
  // myStepper.setSpeed(rpm);
  // myStepper.step(stepsPerRevolution);
  // delay(500);

  // // Rotate CCW quickly at 10 RPM
  // myStepper.setSpeed(rpm);
  // myStepper.step(-stepsPerRevolution);
  // delay(500);
}