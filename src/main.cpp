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
#include "I2C_SOFTRESET.h"

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
int commStatus;

unsigned long timing_comm;
unsigned long timing_adjustment;
unsigned long timing_joystick;
unsigned long timing_sunpulse;

int sunPulseIndex;
TrackingState trackingState;

bool joystickMotorMoving;
bool joystickLAMoving;

int isDynamicPanel;

void assertI2CFunctions();
void flashLED(int num);
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

    commStatus = comm.setup(isDynamicPanel);
    Serial.print("Comm Status: ");
    Serial.println(commStatus);

    int rssi = WiFi.RSSI();

    Serial.println("Setup Complete.");

    char buffer[100];
    if (isDynamicPanel)
        snprintf(buffer, sizeof(buffer), "Setup: Power Monitor=%i | LDRs=%i | Gyro=%i | Comm=%i, RSSI=%i", powerMonitorStatus, ldrStatus, gyroStatus, commStatus, rssi);
    else
        snprintf(buffer, sizeof(buffer), "Setup: Power Monitor=%i | LDRs=%i | Comm=%i, RSSI=%i", powerMonitorStatus, ldrStatus, commStatus, rssi);

    comm.sendLog(buffer);

    timing_comm = 0;
    timing_adjustment = 0;
    timing_joystick = 0;
    timing_sunpulse = 0;
}

void loop()
{
    if (powerMonitorStatus != 0 || gyroStatus != 0 || commStatus != 0)
    {
        Serial.println("Error...");

        if (powerMonitorStatus != 0)
            flashLED(1);
        else if (gyroStatus != 0)
            flashLED(2);
        else
            flashLED(3);

        delay(1000);
        return;
    }
    comm.poll();

    unsigned long now = millis();

    if (now - timing_sunpulse >= SUN_PULSE_ANIM_DELAY)
    {
        matrix.loadFrame(sunpulse[sunPulseIndex]);
        sunPulseIndex++;
        if (sunPulseIndex == 8)
            sunPulseIndex = 0;
        timing_sunpulse = now;
    }

    now = millis();
    if (now - timing_comm >= LOOP_DELAY)
    {
        // assertI2CFunctions();
        int res;
        if (isDynamicPanel)
            res = comm.sendDataDynamic(dht.getTemperature(), dht.getHumidity(), i2cMux.getTopLeftLDRLight(), i2cMux.getBotLeftLDRLight(),
                                       i2cMux.getTopRightLDRLight(), i2cMux.getBotRightLDRLight(), servoMotor.getAzimuthFromCounter(), i2cMux.getZenith(),
                                       powerMonitor.getVoltage(PM_PANEL_CHANNEL), powerMonitor.getCurrent(PM_PANEL_CHANNEL),
                                       powerMonitor.getVoltage(PM_SYSTEM_CHANNEL), powerMonitor.getCurrent(PM_SYSTEM_CHANNEL));
        else
            res = comm.sendDataStatic(dht.getTemperature(), dht.getHumidity(), i2cMux.getTopLeftLDRLight(), i2cMux.getBotLeftLDRLight(),
                                      i2cMux.getTopRightLDRLight(), i2cMux.getBotRightLDRLight(), powerMonitor.getVoltage(PM_PANEL_CHANNEL),
                                      powerMonitor.getCurrent(PM_PANEL_CHANNEL));

        Serial.print("Sending sensor data... | ");
        Serial.println(res);
        if (res == 0 || WiFi.status() != WL_CONNECTED)
        {
            WiFi.begin(SECRET_SSID, SECRET_PASS);
            comm.sendLog("Wifi Disconnected, Reconnected.");
        }
        timing_comm = now;
    }

    if (!isDynamicPanel)
        return;

    //// Solar Tracking ////

    now = millis();

    if ((trackingState == TrackingState::WAITING) &&
        ((now - timing_joystick) >= JOYSTICK_DELAY))
    {
        handleJoystick();
        timing_joystick = now;
    }

    if ((trackingState == TrackingState::WAITING) &&
        ((now - timing_adjustment) >= ADJUSTMENT_DELAY))
    {
        servoMotor.stop();
        trackingState = TrackingState::ZENITH_SETUP;
        comm.sendLog("Tracking State -> Zenith Setup");
    }

    if (trackingState == TrackingState::ZENITH_SETUP)
    {
        if (sunTracking.trackZenithSetup(i2cMux, linearActuator, comm))
        {
            comm.sendLog("Zenith already homed. Tracking State -> Azimuth Setup");
            trackingState = TrackingState::AZIMUTH_SETUP;
        }
        else
        {
            comm.sendLog("Tracking State -> Zenith");
            trackingState = TrackingState::ZENITH;
        }
    }

    if (trackingState == TrackingState::ZENITH)
    {
        if (sunTracking.trackZenith(i2cMux, linearActuator))
        {
            comm.sendLog("Tracking State -> Azimuth Setup");
            trackingState = TrackingState::AZIMUTH_SETUP;
        }
    }

    if (trackingState == TrackingState::AZIMUTH_SETUP)
    {
        if (sunTracking.trackAzimuthSetup(i2cMux, servoMotor, comm))
        {
            comm.sendLog("Azimuth already homed. Tracking State -> Waiting");
            trackingState = TrackingState::WAITING;
            timing_adjustment = millis();
        }
        else
        {
            comm.sendLog("Tracking State -> Azimuth");
            trackingState = TrackingState::AZIMUTH;
        }
    }

    if (trackingState == TrackingState::AZIMUTH && servoMotor.atPosition())
    {
        servoMotor.stop();
        servoMotor.resetEncoderTracking();

        comm.sendLog("Tracking State -> Waiting");

        trackingState = TrackingState::WAITING;
        timing_adjustment = millis();
    }
}

void handleJoystick()
{
    int x = analogRead(JOYSTICK_X_PIN);
    int y = analogRead(JOYSTICK_Y_PIN);

    if (joystickLAMoving && x > 10 && x < 1000)
    {
        linearActuator.stop();
        joystickLAMoving = false;
    }
    else if (!joystickLAMoving && x <= 10)
    {
        linearActuator.extend();
        joystickLAMoving = true;
    }
    else if (!joystickLAMoving && x >= 1000)
    {
        linearActuator.retract();
        joystickLAMoving = true;
    }

    if (joystickMotorMoving && y > 10 && y < 1000)
    {
        servoMotor.stop();
        joystickMotorMoving = false;
    }
    else if (!joystickMotorMoving && y <= 10)
    {
        servoMotor.spin(1, 0.1);
        joystickMotorMoving = true;
    }
    else if (!joystickMotorMoving && y >= 1000)
    {
        servoMotor.spin(0, 0.1);
        joystickMotorMoving = true;
    }

    // Home panel back to 180 degrees forward
    if (digitalRead(JOYSTICK_BUTTON_PIN) == 0)
    {
        comm.sendLog("Running home sequence");
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

        // Delay so operator has ample time to shut system off, as this home sequence will probably be used
        // when the operator wants to cease data collection at the end of the day.
        delay(20000);
    }
}

void assertI2CFunctions()
{
    Wire.beginTransmission(I2C_MUX_ADDR); // Valid address
    byte error = Wire.endTransmission();  // returns 0 when device ACKs

    if (error != 0)
    {
        comm.sendLog("Detected i2c error (NO ACK), attempting restart.");
        Wire.end();
        I2CSoftReset(WIRE_SDA_PIN, WIRE_SCL_PIN);
        Wire.begin();
        comm.sendLog("i2c reset complete.");
    }

    Wire.beginTransmission(3); // 3 is NOT a used address and should NOT ACK
    error = Wire.endTransmission();
    if (error == 0)
    {
        comm.sendLog("Detected i2c error (ACK), attempting restart.");
        Wire.end();
        I2CSoftReset(WIRE_SDA_PIN, WIRE_SCL_PIN);
        Wire.begin();
        comm.sendLog("i2c reset complete.");
    }
}

void flashLED(int num)
{
    for (int i = 0; i < num; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}