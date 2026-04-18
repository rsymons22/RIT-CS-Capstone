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

double SunTracking::getAzimuth(unsigned long time)
{
    double az, _;
    calcHorizontalCoordinates(time, LATITUDE, LONGITUDE, az, _);

    return az;
}

double SunTracking::getZenith(unsigned long time)
{
    double _, ze;
    calcHorizontalCoordinates(time, LATITUDE, LONGITUDE, _, ze);

    return ze;
}

bool SunTracking::trackZenithSetup(I2CMux &i2cMux, LinearActuator &la, Communication &comm)
{
    double panelZenith = i2cMux.getZenith();
    sunZenith = 90 - getZenith(comm.getTime());

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Panel Zenith: %.2f | Adjusted Sun Zenith: %.2f", panelZenith, sunZenith);
    comm.sendLog(buffer);

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

bool SunTracking::trackAzimuthSetup(I2CMux &i2cMux, ServoMotor &sm, Communication &comm)
{
    sunAzimuth = getAzimuth(comm.getTime());
    double panelAzimuth = sm.getAzimuthFromCounter();

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Panel Azimuth: %.2f | Sun Azimuth: %.2f", panelAzimuth, sunAzimuth);
    comm.sendLog(buffer);

    int diff = sunAzimuth - panelAzimuth;
    panelSpinsCW = diff >= 0;

    if (azimuthBounds(panelSpinsCW, sunAzimuth, panelAzimuth))
        return 1;

    if (panelSpinsCW)
        sm.spin(1, 0.1);
    else
        sm.spin(0, 0.1);

    sm.initEncoderTracking(diff);

    return 0;
}
