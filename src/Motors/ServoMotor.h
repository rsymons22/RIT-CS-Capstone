#ifndef _SERVO_MOTOR_H_
#define _SERVO_MOTOR_H_

#include "constants.h"
#include "Arduino.h"
#include "LX16A-bus.h"

void handleInterrupt(void *param);

class ServoMotor
{
private:
    LX16A _motor = LX16A(1, Serial1);
    volatile bool _atPosition;
    int _count;
    int _dir;
    int _counter;

public:
    void setup();
    void stop();
    void spin(int dir, double speed);
    void initEncoderTracking(int degreeDiff);
    bool atPosition();
    void resetEncoderTracking();
    int getAzimuthFromCounter();
    ~ServoMotor();

    friend void handleInterrupt(void *param);
};

#endif