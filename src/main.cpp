#include <Arduino.h>
#include <Wire.h>
#include "constants.h"
#include "Sensors\I2CMux.h"
#include "Sensors\PowerMonitor.h"
#include "Sensors\TempHumiditySensor.h"
#include "Motors\LinearActuator.h"
#include "Motors\StepperMotor.h"

I2CMux i2cMux;
PowerMonitor powerMonitor;
TempHumiditySensor dht;

LinearActuator linearActuator;
StepperMotor stepperMotor;

MotorState motorState;
int powerMonitorStatus;
int ldrStatus;
int gyroStatus;

long stepperDelayUS;
int loopCount = 0;

unsigned long timing_motorMicros = 0;
unsigned long timing_loopMicros = 0;

void stepperMotorControl(unsigned long now);
void linearActuatorControl();
void printData(unsigned long now);

void setup()
{
  while (!Serial)
    delay(10);
  delay(5000);

  Serial.begin(9600);
  Wire.begin();

  Serial.println("Setup Start...");

  // Best setup before the other i2c devices due to some inconsistency in the Wire library.
  powerMonitorStatus = powerMonitor.setup();
  Serial.print("Power Monitor Status: ");
  Serial.println(powerMonitorStatus);

  ldrStatus = i2cMux.setupLDRs();
  Serial.print("LDR Status: ");
  Serial.println(ldrStatus);

  gyroStatus = i2cMux.setupGyro();
  Serial.print("Gyro Status: ");
  Serial.println(gyroStatus);

  dht.setup();

  stepperDelayUS = stepperMotor.getDelay(10);

  Serial.println("Setup Complete.");
}

void loop()
{
  if (powerMonitorStatus != 0 || ldrStatus != 0 || gyroStatus != 0)
  {
    Serial.println("Error...");
    delay(1000);
    return;
  }

  unsigned long now = micros();

  // Manual control options
  // stepperMotorControl(now);
  // or
  // linearActuatorControl();

  printData(now);
}

void stepperMotorControl(unsigned long now)
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();

    if (c == 'R')
      motorState = MotorState::CW;
    else if (c == 'L')
      motorState = MotorState::CCW;
    else
      motorState = MotorState::STOP;
  }

  if (motorState != MotorState::STOP && now - timing_motorMicros >= stepperDelayUS)
  {
    timing_motorMicros = now;
    stepperMotor.step(motorState);
  }
}

void linearActuatorControl()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == 'E')
      linearActuator.extend();
    else if (c == 'R')
      linearActuator.retract();
    else
      linearActuator.stop();
  }
}

void printData(unsigned long now)
{
  if (now - timing_loopMicros >= LOOP_DELAY_US * 5)
  {
    timing_loopMicros = now;

    Serial.print("Temperature: ");
    Serial.print(dht.getTemperature());
    Serial.print("°C | ");

    Serial.print("Humidity: ");
    Serial.print(dht.getHumidity());
    Serial.print("% | ");

    i2cMux.printLDRData();

    i2cMux.printGyroData();

    Serial.print("Azimuth: ");
    Serial.print(i2cMux.getAzimuth());
    Serial.print("° |");
    Serial.print(" Zenith: ");
    Serial.print(i2cMux.getZenith());
    Serial.print("° | ");

    Serial.print("Voltage (Channel 0): ");
    Serial.print(powerMonitor.getVoltage(0));
    Serial.print(" V | ");

    Serial.print("Current (Channel 0): ");
    Serial.print(powerMonitor.getCurrent(0));
    Serial.print(" A | ");

    Serial.print("Voltage (Channel 1): ");
    Serial.print(powerMonitor.getVoltage(1));
    Serial.print(" V | ");

    Serial.print("Current (Channel 1): ");
    Serial.print(powerMonitor.getCurrent(1));
    Serial.print(" A");

    Serial.println();
    Serial.println();
  }
}