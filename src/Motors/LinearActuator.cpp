#include "LinearActuator.h"
#include <Arduino.h>

LinearActuator::LinearActuator()
{
    pinMode(LA_EXTEND_PIN, OUTPUT);
    pinMode(LA_RETRACT_PIN, OUTPUT);

    stop();
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