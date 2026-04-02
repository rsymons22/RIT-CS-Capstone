#ifndef _LINEAR_ACTUATOR_H_
#define _LINEAR_ACTUATOR_H_

#include "functional"
#include "constants.h"
#include "..\Sensors\I2CMux.h"

class LinearActuator
{
public:
    void setup();
    void extend();
    void retract();
    void stop();
    void setSpeed(double percent);
};

#endif