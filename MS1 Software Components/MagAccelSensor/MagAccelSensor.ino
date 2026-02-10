#include <Adafruit_LIS2MDL.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LIS2MDL lis2mdl = Adafruit_LIS2MDL(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

void setup(void) {
  Serial.begin(9600);
  while (!Serial)
    delay(10);

  /* Initialise the sensor */
  if (!lis2mdl.begin()) {  // I2C mode
    Serial.println("Error with Mag Sensor");
    while (1) delay(10);
  }
  lis2mdl.enableAutoRange(true);

  if (!accel.begin()) {
    Serial.println("Error with Accel Sensor");
    while (1) delay(10);
  }
  accel.setRange(LSM303_RANGE_4G);
  accel.setMode(LSM303_MODE_NORMAL);

}

void loop(void) {
  /* Get a new sensor event */
  sensors_event_t mag_event;
  lis2mdl.getEvent(&mag_event);

  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  Serial.print("X: ");
  Serial.print(mag_event.magnetic.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(mag_event.magnetic.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(mag_event.magnetic.z);
  Serial.print(" uT  |  ");

  sensors_event_t accel_event;
  accel.getEvent(&accel_event);

  Serial.print("X: ");
  Serial.print(accel_event.acceleration.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(accel_event.acceleration.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(accel_event.acceleration.z);
  Serial.println(" m/s^2");

  delay(200);
}