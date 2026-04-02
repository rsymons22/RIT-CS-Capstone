#include "ServoMotor.h"

void ServoMotor::setup()
{
    Serial1.begin(115200);
    _motor.initialize();
    _motor.enableTorque();

    _count = INT32_MAX;

    pinMode(ENCODER_PINA, INPUT_PULLUP);
    pinMode(ENCODER_PINB, INPUT_PULLUP);

    attachInterruptParam(digitalPinToInterrupt(ENCODER_PINA), handleInterruptA, RISING, this);
    attachInterruptParam(digitalPinToInterrupt(ENCODER_PINB), handleInterruptB, RISING, this);
}

void ServoMotor::stop()
{
    _motor.setMotorMode(0);
}

void ServoMotor::spin(int dir, double speed)
{
    _motor.setMotorMode(1000 * speed * (dir ? 1 : -1));
}

void ServoMotor::initEncoderTracking(int degreeDiff)
{
    _count = _counter + degreeDiff * ENCODER_COUNTS_PER_DEGREE;
    _atPosition = false;
}

bool ServoMotor::atPosition()
{
    return _atPosition;
}

void ServoMotor::resetEncoderTracking()
{
    _atPosition = false;
}

int ServoMotor::getAzimuthFromCounter()
{
    return 180 + _counter / ENCODER_COUNTS_PER_DEGREE;
}

void handleInterruptA(void *param)
{
    ServoMotor *m = static_cast<ServoMotor *>(param);

    if (digitalRead(ENCODER_PINB) == LOW)
    {
        m->_counter--;
    }
    else
    {
        m->_counter++;
    }

    if (m->_counter == m->_count)
    {
        m->_atPosition = true;
    }
}

void handleInterruptB(void *param)
{
    ServoMotor *m = static_cast<ServoMotor *>(param);

    if (digitalRead(ENCODER_PINA) == LOW)
    {
        m->_counter++;
    }
    else
    {
        m->_counter--;
    }

    if (m->_counter == m->_count)
    {
        m->_atPosition = true;
    }
}
