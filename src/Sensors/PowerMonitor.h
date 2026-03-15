#ifndef _POWER_MONITOR_H_
#define _POWER_MONITOR_H_

#include <Adafruit_INA3221.h>
#include <constants.h>

class PowerMonitor
{
private:
    Adafruit_INA3221 _monitor;

public:
    int setup();
    float getVoltage(uint8_t channel);
    float getCurrent(uint8_t channel);
};

#endif