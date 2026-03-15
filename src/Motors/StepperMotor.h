#ifndef _STEPPER_MOTOR_H_
#define _STEPPER_MOTOR_H_

#include "constants.h"

enum MotorState
{
    STOP = 0,
    CW = 1,
    CCW = 2
};

class StepperMotor
{
private:
    int _step = 0;

public:
    StepperMotor();
    void step(MotorState state);
    long getDelay(long speed);
};

#endif