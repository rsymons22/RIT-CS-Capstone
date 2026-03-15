#ifndef _LINEAR_ACTUATOR_H_
#define _LINEAR_ACTUATOR_H_

#include "constants.h"

class LinearActuator
{
public:
    LinearActuator();
    void extend();
    void retract();
    void stop();
};

#endif