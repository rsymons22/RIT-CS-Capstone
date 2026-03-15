#include "StepperMotor.h"
#include "Arduino.h"

StepperMotor::StepperMotor()
{
    pinMode(SM_IN1_PIN, OUTPUT);
    pinMode(SM_IN2_PIN, OUTPUT);
    pinMode(SM_IN3_PIN, OUTPUT);
    pinMode(SM_IN4_PIN, OUTPUT);
}

void StepperMotor::step(MotorState state)
{
    if (state == MotorState::STOP)
        return;

    switch (this->_step)
    {
    case 0: // 1010
        digitalWrite(SM_IN1_PIN, HIGH);
        digitalWrite(SM_IN3_PIN, LOW);
        digitalWrite(SM_IN2_PIN, HIGH);
        digitalWrite(SM_IN4_PIN, LOW);
        break;
    case 1: // 0110
        digitalWrite(SM_IN1_PIN, LOW);
        digitalWrite(SM_IN3_PIN, HIGH);
        digitalWrite(SM_IN2_PIN, HIGH);
        digitalWrite(SM_IN4_PIN, LOW);
        break;
    case 2: // 0101
        digitalWrite(SM_IN1_PIN, LOW);
        digitalWrite(SM_IN3_PIN, HIGH);
        digitalWrite(SM_IN2_PIN, LOW);
        digitalWrite(SM_IN4_PIN, HIGH);
        break;
    case 3: // 1001
        digitalWrite(SM_IN1_PIN, HIGH);
        digitalWrite(SM_IN3_PIN, LOW);
        digitalWrite(SM_IN2_PIN, LOW);
        digitalWrite(SM_IN4_PIN, HIGH);
        break;
    }

    if (state == MotorState::CW)
        _step = _step == 3 ? 0 : _step + 1;
    else
        _step = _step == 0 ? 3 : _step - 1;
}
/*
 * speed in revolutions per minute
 * returns delay in microseconds
 */
long StepperMotor::getDelay(long speed)
{
    return 60L * 1000L * 1000L / 2048L / speed;
}
