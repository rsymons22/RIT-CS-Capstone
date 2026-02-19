const int extend_pin_in1 = 12;
const int retract_pin_in2 = 13;

void setup()
{

  Serial.begin(9600); // Start serial communication at 9600 baud rate
  Serial.println("Enter a character:");

  pinMode(extend_pin_in1, OUTPUT);
  pinMode(retract_pin_in2, OUTPUT);
  digitalWrite(extend_pin_in1, LOW);
  digitalWrite(retract_pin_in2, LOW);
}

void loop()
{

  if (Serial.available() > 0)
  {                                    // Check if data has been sent from the computer
    char incomingByte = Serial.read(); // Read the oldest byte in the serial buffer

    if (incomingByte == 'E')
    {
      Serial.print("extend\n");
      digitalWrite(extend_pin_in1, HIGH);
      digitalWrite(retract_pin_in2, LOW);
    }
    else if (incomingByte == 'R')
    {
      Serial.print("retract\n");
      digitalWrite(extend_pin_in1, LOW);
      digitalWrite(retract_pin_in2, HIGH);
    }
    else if (incomingByte == 'S')
    {
      digitalWrite(extend_pin_in1, LOW);
      digitalWrite(retract_pin_in2, LOW);
    }
  }
}