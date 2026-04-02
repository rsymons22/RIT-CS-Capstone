#include "I2CMux.h"

int I2CMux::tcaselect(uint8_t channel)
{
    Wire.beginTransmission(I2C_MUX_ADDR);
    Wire.write(1 << channel);
    return Wire.endTransmission();
}

int I2CMux::setupLDRs()
{
    this->_ldrTopLeft = ArtronShop_BH1750(LDR_ADDR, &Wire);
    this->_ldrBotLeft = ArtronShop_BH1750(LDR_ADDR, &Wire);
    this->_ldrBotRight = ArtronShop_BH1750(LDR_ADDR, &Wire);
    this->_ldrTopRight = ArtronShop_BH1750(LDR_ADDR, &Wire);

    tcaselect(TOP_LEFT_LDR_CHANNEL);
    if (!this->_ldrTopLeft.begin())
        return TOP_LEFT_LDR_CHANNEL;

    tcaselect(BOT_LEFT_LDR_CHANNEL);
    if (!this->_ldrBotLeft.begin())
        return BOT_LEFT_LDR_CHANNEL;

    tcaselect(BOT_RIGHT_LDR_CHANNEL);
    if (!this->_ldrBotRight.begin())
        return BOT_RIGHT_LDR_CHANNEL;

    tcaselect(TOP_RIGHT_LDR_CHANNEL);
    if (!this->_ldrTopRight.begin())
        return TOP_RIGHT_LDR_CHANNEL;

    return 0;
}

int I2CMux::setupGyro()
{
    tcaselect(GYRO_CHANNEL);

    if (!this->_mpu.init())
    {
        return 1;
    }

    _mpu.setAccOffsets(xyzFloat(MPU_CAL[0], MPU_CAL[1], MPU_CAL[2]));

    this->_mpu.enableGyrDLPF();
    this->_mpu.setGyrDLPF(MPU6500_DLPF_6);
    this->_mpu.setSampleRateDivider(5);
    this->_mpu.setGyrRange(MPU6500_GYRO_RANGE_250);

    this->_mpu.setAccRange(MPU6050_ACC_RANGE_2G);
    this->_mpu.enableAccDLPF(true);
    this->_mpu.setAccDLPF(MPU6500_DLPF_6);

    return 0;
}

double I2CMux::getZenith()
{
    tcaselect(GYRO_CHANNEL);
    return -1 * _mpu.getPitch();
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
