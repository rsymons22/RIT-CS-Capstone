#include "SunTracking.h"

bool zenithBounds(bool isRetracting, double sunZenith, double panelZenith)
{
    return (isRetracting && (panelZenith <= sunZenith || panelZenith <= ZENITH_MIN)) ||
           (!isRetracting && (panelZenith >= sunZenith || panelZenith >= ZENITH_MAX));
}

bool azimuthBounds(bool panelSpinsCW, double sunAzimuth, double panelAzimuth)
{
    const double TOLERANCE = 1;
    return (panelSpinsCW && (panelAzimuth >= sunAzimuth - TOLERANCE || panelAzimuth >= AZIMUTH_MAX)) ||
           (!panelSpinsCW && (panelAzimuth <= sunAzimuth + TOLERANCE || panelAzimuth <= AZIMUTH_MIN));
}

double SunTracking::getAzimuth()
{
    RTCTime currentTime;

    RTC.getTime(currentTime);

    double az, _;
    calcHorizontalCoordinates(currentTime.getUnixTime(), LATITUDE, LONGITUDE, az, _);

    return az;
}

double SunTracking::getZenith()
{
    RTCTime currentTime;

    RTC.getTime(currentTime);

    double _, ze;
    calcHorizontalCoordinates(currentTime.getUnixTime(), LATITUDE, LONGITUDE, _, ze);

    return ze;
}

bool SunTracking::trackZenithSetup(I2CMux &i2cMux, LinearActuator &la)
{
    sunZenith = 90 - getZenith();
    double panelZenith = i2cMux.getZenith();
    isRetracting = (sunZenith - panelZenith) < 0;

    if (zenithBounds(isRetracting, round(sunZenith), round(panelZenith)))
        return 1;

    if (isRetracting)
        la.retract();
    else
        la.extend();

    return 0;
}

bool SunTracking::trackZenith(I2CMux &i2cMux, LinearActuator &la)
{
    double panelZenith = i2cMux.getZenith();

    if (zenithBounds(isRetracting, round(sunZenith), round(panelZenith)))
    {
        la.stop();
        return 1;
    }

    return 0;
}

bool SunTracking::trackAzimuthSetup(I2CMux &i2cMux, ServoMotor &sm)
{
    sunAzimuth = getAzimuth();
    double panelAzimuth = sm.getAzimuthFromCounter();
    int diff = sunAzimuth - panelAzimuth;
    panelSpinsCW = diff >= 0;

    if (azimuthBounds(panelSpinsCW, sunAzimuth, panelAzimuth))
        return 1;

    sm.initEncoderTracking(diff);

    if (panelSpinsCW)
        sm.spin(1, 0.1);
    else
        sm.spin(0, 0.1);

    return 0;
}
