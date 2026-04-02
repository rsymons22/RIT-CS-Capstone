#include "SolarCalculator.h"
#include "TimeLib.h"
#include "constants.h"
#include "Arduino.h"
#include "RTC.h"
#include "Sensors\I2CMux.h"
#include "Motors\LinearActuator.h"
#include "Motors\ServoMotor.h"

enum TrackingState
{
    WAITING = 0,
    ZENITH_SETUP = 1,
    ZENITH = 2,
    AZIMUTH_SETUP = 3,
    AZIMUTH = 4
};

class SunTracking
{
private:
    double sunZenith;
    bool isRetracting;

    double sunAzimuth;
    bool panelSpinsCW;

public:
    double getAzimuth();
    double getZenith();
    bool trackZenithSetup(I2CMux &i2cMux, LinearActuator &la);
    bool trackZenith(I2CMux &i2cMux, LinearActuator &la);
    bool trackAzimuthSetup(I2CMux &i2cMux, ServoMotor &sm);
};