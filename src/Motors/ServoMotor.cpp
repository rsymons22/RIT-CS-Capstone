#include "ServoMotor.h"

void ServoMotor::setup()
{
    Serial1.begin(115200);
    _motor.initialize();
    _motor.enableTorque();

    _count = INT32_MAX;
    _atPosition = false;

    _encoder = new RotaryEncoder(ENCODER_PINA, ENCODER_PINB, RotaryEncoder::LatchMode::TWO03);

    attachInterruptParam(digitalPinToInterrupt(ENCODER_PINA), handleInterrupt, CHANGE, this);
    attachInterruptParam(digitalPinToInterrupt(ENCODER_PINB), handleInterrupt, CHANGE, this);
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
    _count = constrain(getPosition() + degreeDiff * ENCODER_COUNTS_PER_DEGREE, -ENCODER_MINMAX, ENCODER_MINMAX);
}

bool ServoMotor::atPosition()
{
    return _atPosition;
}

long ServoMotor::getPosition()
{
    return -1 * _encoder->getPosition();
}

void ServoMotor::resetEncoderTracking()
{
    _atPosition = false;
}

int ServoMotor::getAzimuthFromCounter()
{
    return 180 + getPosition() / ENCODER_COUNTS_PER_DEGREE;
}

ServoMotor::~ServoMotor()
{
    stop();
}

void handleInterrupt(void *param)
{
    ServoMotor *m = static_cast<ServoMotor *>(param);

    m->_encoder->tick();

    long pos = m->getPosition();

    if (((m->_dir == 1) && (pos >= m->_count)) ||
        ((m->_dir == 0) && (pos <= m->_count)))
    {
        m->_atPosition = true;
    }
}
