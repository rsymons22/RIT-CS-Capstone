#include "I2CMux.h"

void I2CMux::tcaselect(uint8_t channel)
{
    if (channel > 7)
        return;

    Wire.beginTransmission(I2C_MUX_ADDR);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

I2CMux::I2CMux()
{
    this->_ldrTopLeft = ArtronShop_BH1750(LDR_ADDR, &Wire);
    this->_ldrBotLeft = ArtronShop_BH1750(LDR_ADDR, &Wire);
    this->_ldrBotRight = ArtronShop_BH1750(LDR_ADDR, &Wire);
    this->_ldrTopRight = ArtronShop_BH1750(LDR_ADDR, &Wire);

    this->_mag = Adafruit_LIS2MDL(12345);
    this->_accel = Adafruit_LSM303_Accel_Unified(54321);
}

int I2CMux::setupLDRs()
{
    tcaselect(TOP_LEFT_LDR_CHANNEL);
    // delay(1000);
    if (!this->_ldrTopLeft.begin())
        return TOP_LEFT_LDR_CHANNEL;

    tcaselect(BOT_LEFT_LDR_CHANNEL);
    // delay(100);
    if (!this->_ldrBotLeft.begin())
        return BOT_LEFT_LDR_CHANNEL;

    tcaselect(BOT_RIGHT_LDR_CHANNEL);
    // delay(100);
    if (!this->_ldrBotRight.begin())
        return BOT_RIGHT_LDR_CHANNEL;

    tcaselect(TOP_RIGHT_LDR_CHANNEL);
    // delay(100);
    if (!this->_ldrTopRight.begin())
        return TOP_RIGHT_LDR_CHANNEL;

    return 0;
}

int I2CMux::setupGyro()
{
    tcaselect(GYRO_CHANNEL);

    if (!this->_mag.begin())
        return 2;

    this->_mag.enableAutoRange(true);

    if (!this->_accel.begin())
        return 1;

    this->_accel.setRange(LSM303_RANGE_4G);

    this->_accel.setMode(LSM303_MODE_NORMAL);

    return 0;
}

Vector3D I2CMux::getMagData()
{
    tcaselect(GYRO_CHANNEL);
    sensors_event_t event;
    this->_mag.getEvent(&event);

    Vector3D m = Vector3D(event.magnetic.x, event.magnetic.y, event.magnetic.z);

    // Offset from calibration
    m.x -= (MAG_MIN[0] + MAG_MAX[0]) / 2;
    m.y -= (MAG_MIN[1] + MAG_MAX[1]) / 2;
    m.z -= (MAG_MIN[2] + MAG_MAX[2]) / 2;

    return m;
}

Vector3D I2CMux::getAccelData()
{
    tcaselect(GYRO_CHANNEL);
    sensors_event_t event;
    this->_accel.getEvent(&event);
    // Negate all acceleration values because the gyro is mounted upside down.
    return Vector3D(-event.acceleration.x, -event.acceleration.y, -event.acceleration.z);
}

// https://ozzmaker.com/compass2/
// Tilt compensation heading wasn't as accurate with this method, pitch/roll seem to work well though.
double I2CMux::getZenith()
{
    Vector3D accelData = getAccelData();
    accelData.normalize();
    double pitch = asin(accelData.x);
    double roll = -asin(accelData.y / cos(pitch));

    // Axis should theoretically be pitch, but roll gives the correct value for how the gyro is mounted.
    return -1 * roll * 180 / M_PI;
}

// https://github.com/pololu/lsm303-arduino/tree/master
double I2CMux::getAzimuth()
{
    // Positive-Y, which is where the gyro is pointing on the panel
    Vector3D from = Vector3D(0, 1, 0);

    Vector3D m = getMagData();
    Vector3D a = getAccelData();

    Vector3D E = m.cross(a);
    E.normalize();

    Vector3D N = a.cross(E);
    N.normalize();

    double heading = atan2(E.dot(from), N.dot(from)) * 180 / PI;

    // Apply magnetic declination for true north
    heading += MAGNETIC_DECLINATION;

    if (heading < 0)
        heading += 360;
    return heading;
}

void I2CMux::printGyroData()
{
    Vector3D magData = getMagData();

    Serial.print("X: ");
    Serial.print(magData.x);
    Serial.print("  ");
    Serial.print("Y: ");
    Serial.print(magData.y);
    Serial.print("  ");
    Serial.print("Z: ");
    Serial.print(magData.z);
    Serial.print(" uT | ");

    Vector3D accelData = getAccelData();
    Serial.print("X: ");
    Serial.print(accelData.x);
    Serial.print("  ");
    Serial.print("Y: ");
    Serial.print(accelData.y);
    Serial.print("  ");
    Serial.print("Z: ");
    Serial.print(accelData.z);
    Serial.print(" m/s^2 | ");
}

float I2CMux::getTopLeftLDRLight()
{
    tcaselect(TOP_LEFT_LDR_CHANNEL);
    return this->_ldrTopLeft.light();
}

float I2CMux::getBotLeftLDRLight()
{
    tcaselect(BOT_LEFT_LDR_CHANNEL);
    return this->_ldrBotLeft.light();
}

float I2CMux::getTopRightLDRLight()
{
    tcaselect(TOP_RIGHT_LDR_CHANNEL);
    return this->_ldrTopRight.light();
}

float I2CMux::getBotRightLDRLight()
{
    tcaselect(BOT_RIGHT_LDR_CHANNEL);
    return this->_ldrBotRight.light();
}

void I2CMux::printLDRData()
{
    Serial.print("TL LDR: ");
    Serial.print(getTopLeftLDRLight());
    Serial.print(" lx | ");

    Serial.print("BL LDR: ");
    Serial.print(getBotLeftLDRLight());
    Serial.print(" lx | ");

    Serial.print("TR LDR: ");
    Serial.print(getTopRightLDRLight());
    Serial.print(" lx | ");

    Serial.print("BR LDR: ");
    Serial.print(getBotRightLDRLight());
    Serial.print(" lx | ");
}
