#include "TempHumiditySensor.h"

void TempHumiditySensor::setup()
{
    this->_dht.begin();
}

float TempHumiditySensor::getTemperature()
{
    sensors_event_t event;
    this->_dht.temperature().getEvent(&event);
    return event.temperature;
}

float TempHumiditySensor::getHumidity()
{
    sensors_event_t event;
    this->_dht.humidity().getEvent(&event);
    return event.relative_humidity;
}
