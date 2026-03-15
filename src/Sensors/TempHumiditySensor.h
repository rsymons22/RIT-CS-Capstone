#ifndef _TEMPHUMIDITYSENSOR_H_
#define _TEMPHUMIDITYSENSOR_H_

#include <Adafruit_Sensor.h>
#include "constants.h"
#include <DHT.h>
#include <DHT_U.h>

class TempHumiditySensor
{
private:
    DHT_Unified _dht = DHT_Unified(TEMPHUMIDITY_PIN, DHT11);

public:
    void setup();
    float getTemperature();
    float getHumidity();
};

#endif