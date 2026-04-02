#include "LinearActuator.h"
#include <Arduino.h>

void LinearActuator::setup()
{
    pinMode(LA_EXTEND_PIN, OUTPUT);
    pinMode(LA_RETRACT_PIN, OUTPUT);
    pinMode(LA_SPEED_PIN, OUTPUT);

    stop();
    setSpeed(LA_SPEED_PERCENT);
}

void LinearActuator::extend()
{
    digitalWrite(LA_EXTEND_PIN, HIGH);
    digitalWrite(LA_RETRACT_PIN, LOW);
}

void LinearActuator::retract()
{
    digitalWrite(LA_EXTEND_PIN, LOW);
    digitalWrite(LA_RETRACT_PIN, HIGH);
}

void LinearActuator::stop()
{
    digitalWrite(LA_EXTEND_PIN, LOW);
    digitalWrite(LA_RETRACT_PIN, LOW);
}

void LinearActuator::setSpeed(double percent)
{
    if (percent < 0 || percent > 1)
        return;
    analogWrite(LA_SPEED_PIN, percent * 255);
}
