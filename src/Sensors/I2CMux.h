#ifndef _I2CMUX_H_
#define _I2CMUX_H_

#include <ArtronShop_BH1750.h>
#include <Adafruit_LIS2MDL.h>
#include <Adafruit_LSM303_Accel.h>
#include "constants.h"
#include "Vector3D.h"

struct xyz
{
    float x;
    float y;
    float z;

    xyz(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

class I2CMux
{
private:
    Adafruit_LIS2MDL _mag;
    Adafruit_LSM303_Accel_Unified _accel;
    ArtronShop_BH1750 _ldrTopLeft;
    ArtronShop_BH1750 _ldrBotLeft;
    ArtronShop_BH1750 _ldrTopRight;
    ArtronShop_BH1750 _ldrBotRight;
    void tcaselect(uint8_t channel);

public:
    I2CMux();
    int setupLDRs();
    float getTopLeftLDRLight();
    float getBotLeftLDRLight();
    float getTopRightLDRLight();
    float getBotRightLDRLight();
    void printLDRData();

    int setupGyro();
    Vector3D getMagData();
    Vector3D getAccelData();
    double getAzimuth();
    double getZenith();
    void printGyroData();
};

#endif