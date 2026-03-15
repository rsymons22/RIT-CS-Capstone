#include "PowerMonitor.h"
#include <Wire.h>

int PowerMonitor::setup()
{
    if (!this->_monitor.begin(POWER_MONITOR_ADDR, &Wire))
        return 1;

    if (!this->_monitor.setAveragingMode(INA3221_AVG_16_SAMPLES))
        return 2;

    return 0;
}

float PowerMonitor::getCurrent(uint8_t channel)
{
    return this->_monitor.getCurrentAmps(channel);
}

float PowerMonitor::getVoltage(uint8_t channel)
{
    return this->_monitor.getBusVoltage(channel);
}