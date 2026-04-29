#include "ServoMotor.h"

void ServoMotor::setup()
{
    Serial1.begin(115200);
    _motor.initialize();
    _motor.enableTorque();

    _count = INT32_MAX;
    _atPosition = false;

    pinMode(ENCODER_PINA, INPUT_PULLUP);
    pinMode(ENCODER_PINB, INPUT_PULLUP);

    // Attach interrupt to one pin to detect change, then read high/low signal on both to determine direction
    attachInterruptParam(digitalPinToInterrupt(ENCODER_PINA), handleInterrupt, CHANGE, this);
}

void ServoMotor::stop()
{
    _motor.setMotorMode(0);
}

void ServoMotor::spin(int dir, double speed)
{
    _motor.setMotorMode(1000 * speed * (dir ? 1 : -1));
    _dir = dir;
}

void ServoMotor::initEncoderTracking(int degreeDiff)
{
    _atPosition = false;
    _count = constrain(_counter + (degreeDiff * ENCODER_COUNTS_PER_DEGREE), -ENCODER_MINMAX, ENCODER_MINMAX);
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
    return 180 + (_counter / ENCODER_COUNTS_PER_DEGREE);
}

ServoMotor::~ServoMotor()
{
    stop();
}

void handleInterrupt(void *param)
{
    ServoMotor *m = static_cast<ServoMotor *>(param);

    m->_counter += ((digitalRead(ENCODER_PINA) == LOW) ? -1 : 1) * ((digitalRead(ENCODER_PINB) == LOW) ? 1 : -1);

    if (((m->_dir == 1) && (m->_counter >= m->_count)) ||
        ((m->_dir == 0) && (m->_counter <= m->_count)))
    {
        m->_atPosition = true;
    }
}
