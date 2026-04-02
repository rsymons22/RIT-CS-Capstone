#include <Arduino.h>
#include <Wire.h>
#include "constants.h"
#include "Sensors\I2CMux.h"
#include "Sensors\PowerMonitor.h"
#include "Sensors\TempHumiditySensor.h"
#include "Motors\LinearActuator.h"
#include "Motors\ServoMotor.h"
#include "SunTracking.h"
#include "Communication.h"
#include "Arduino_LED_Matrix.h"

SunTracking sunTracking;
Communication comm;
I2CMux i2cMux;
PowerMonitor powerMonitor;
TempHumiditySensor dht;
LinearActuator linearActuator;
ServoMotor servoMotor;
ArduinoLEDMatrix matrix;

// Status for various parts of the system. Generally, 0 is good and anything else is an error.
int powerMonitorStatus;
int ldrStatus;
int gyroStatus;
int rtcStatus;
int commStatus;

unsigned long timing_comm = 0;
unsigned long timing_adjustment = 0;
unsigned long timing_joystick = 0;
unsigned long timing_sunpulse = 0;

int sunPulseIndex = 0;
TrackingState trackingState;

int isDynamicPanel;

void handleJoystick();

void setup()
{
    while (!Serial)
        delay(10);
    delay(5000);

    Serial.begin(9600);

    Wire.begin();

    matrix.begin();
    matrix.loadFrame(LEDMATRIX_BOOTLOADER_ON);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
    pinMode(PANEL_TYPE_PIN, INPUT_PULLUP);
    isDynamicPanel = digitalRead(PANEL_TYPE_PIN);

    Serial.print("Setup Start ");
    Serial.print(isDynamicPanel ? "(Dynamic)" : "(Static)");
    Serial.println("...");

    dht.setup();

    // Best setup before the other i2c devices due to some inconsistency in the Wire library.
    powerMonitorStatus = powerMonitor.setup();
    Serial.print("Power Monitor Status: ");
    Serial.println(powerMonitorStatus);

    ldrStatus = i2cMux.setupLDRs();
    Serial.print("LDR Status: ");
    Serial.println(ldrStatus);

    if (isDynamicPanel)
    {
        gyroStatus = i2cMux.setupGyro();
        Serial.print("Gyro Status: ");
        Serial.println(gyroStatus);

        servoMotor.setup();

        linearActuator.setup();
    }

    commStatus = comm.setup();
    Serial.print("Comm Status: ");
    Serial.println(commStatus);

    rtcStatus = !RTC.begin();
    Serial.print("RTC Status: ");
    Serial.println(rtcStatus);

    comm.setupRTC();

    Serial.println("Setup Complete.");
}

void flashLED(int num)
{
    for (int i = 0; i < num; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(20);
        digitalWrite(LED_BUILTIN, LOW);
        delay(20);
    }
}

void loop()
{
    if (powerMonitorStatus != 0 || ldrStatus != 0 || gyroStatus != 0 || rtcStatus != 0 || commStatus != 0)
    {
        Serial.println("Error...");

        if (powerMonitorStatus != 0)
            flashLED(1);
        else if (ldrStatus != 0)
            flashLED(2);
        else if (gyroStatus != 0)
            flashLED(3);
        else if (rtcStatus != 0)
            flashLED(4);
        else
            flashLED(5);

        delay(1000);
        return;
    }
    comm.poll();

    unsigned long now = millis();

    if (now - timing_sunpulse >= 100)
    {

        matrix.loadFrame(sunpulse[sunPulseIndex]);
        sunPulseIndex++;
        if (sunPulseIndex == 8)
            sunPulseIndex = 0;
        timing_sunpulse = now;
    }

    if (now - timing_comm >= LOOP_DELAY)
    {
        if (isDynamicPanel)
            comm.sendDataDynamic(dht.getTemperature(), dht.getHumidity(), i2cMux.getTopLeftLDRLight(), i2cMux.getBotLeftLDRLight(),
                                 i2cMux.getTopRightLDRLight(), i2cMux.getBotRightLDRLight(), servoMotor.getAzimuthFromCounter(), i2cMux.getZenith(),
                                 powerMonitor.getVoltage(PM_PANEL_CHANNEL), powerMonitor.getCurrent(PM_PANEL_CHANNEL),
                                 powerMonitor.getVoltage(PM_SYSTEM_CHANNEL), powerMonitor.getCurrent(PM_SYSTEM_CHANNEL));
        else
            comm.sendDataStatic(dht.getTemperature(), dht.getHumidity(), i2cMux.getTopLeftLDRLight(), i2cMux.getBotLeftLDRLight(),
                                i2cMux.getTopRightLDRLight(), i2cMux.getBotRightLDRLight(), powerMonitor.getVoltage(PM_PANEL_CHANNEL),
                                powerMonitor.getCurrent(PM_PANEL_CHANNEL));
        timing_comm = now;
    }

    if (!isDynamicPanel)
        return;

    //// Solar Tracking ////

    if (now - timing_joystick >= (LOOP_DELAY / 5) && trackingState == TrackingState::WAITING)
    {
        handleJoystick();
        timing_joystick = now;
    }

    if (trackingState == TrackingState::WAITING && now - timing_adjustment >= ADJUSTMENT_DELAY)
    {
        servoMotor.stop();
        trackingState = TrackingState::ZENITH_SETUP;
    }

    if (trackingState == TrackingState::ZENITH_SETUP)
    {
        int zenithAlreadyHomed = sunTracking.trackZenithSetup(i2cMux, linearActuator);
        trackingState = zenithAlreadyHomed ? TrackingState::AZIMUTH_SETUP : TrackingState::ZENITH;
    }

    if (trackingState == TrackingState::ZENITH)
    {
        if (sunTracking.trackZenith(i2cMux, linearActuator))
            trackingState = TrackingState::AZIMUTH_SETUP;
    }

    if (trackingState == TrackingState::AZIMUTH_SETUP)
    {
        int azimuthAlreadyHomed = sunTracking.trackAzimuthSetup(i2cMux, servoMotor);
        if (azimuthAlreadyHomed)
        {
            trackingState = TrackingState::WAITING;
            timing_adjustment = millis();
        }
        else
        {
            trackingState = TrackingState::AZIMUTH;
        }
    }

    if (trackingState == TrackingState::AZIMUTH && servoMotor.atPosition())
    {
        servoMotor.stop();
        servoMotor.resetEncoderTracking();
        trackingState = TrackingState::WAITING;
        timing_adjustment = millis();
    }
}

bool isRelayLow = true;

void handleJoystick()
{
    int x = analogRead(JOYSTICK_X_PIN);
    int y = analogRead(JOYSTICK_Y_PIN);

    if (x <= 10)
        linearActuator.extend();
    else if (x >= 1000)
        linearActuator.retract();
    else
        linearActuator.stop();

    if (y <= 10)
        servoMotor.spin(1, 0.1);
    else if (y >= 1000)
        servoMotor.spin(0, 0.1);
    else
        servoMotor.stop();

    // Home panel back to 180 degrees forward
    if (digitalRead(JOYSTICK_BUTTON_PIN) == 0)
    {
        int diff = 180 - servoMotor.getAzimuthFromCounter();

        servoMotor.initEncoderTracking(diff);

        if (diff >= 0)
            servoMotor.spin(1, 0.1);
        else
            servoMotor.spin(0, 0.1);

        while (!servoMotor.atPosition())
            ;

        servoMotor.stop();
        servoMotor.resetEncoderTracking();

        linearActuator.retract();

        // Panel can physically go lower, but makes ominous cracking noises when doing so and occasionally has a difficult time extending afterwards.
        while (i2cMux.getZenith() > ZENITH_MIN)
            ;
        linearActuator.stop();

        // This will probably be used when the operator wants to cease data collection for a day.
        // This delay gives a pause to allow the operator to power down the system before it starts tracking again.
        delay(10000);
    }
}