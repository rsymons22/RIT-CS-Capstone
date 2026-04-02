#ifndef _I2CMUX_H_
#define _I2CMUX_H_

#include <ArtronShop_BH1750.h>
#include "constants.h"
#include "MPU6500_WE.h"

class I2CMux
{
private:
    MPU6500_WE _mpu;
    ArtronShop_BH1750 _ldrTopLeft;
    ArtronShop_BH1750 _ldrBotLeft;
    ArtronShop_BH1750 _ldrTopRight;
    ArtronShop_BH1750 _ldrBotRight;
    int tcaselect(uint8_t channel);

public:
    int setupLDRs();
    float getTopLeftLDRLight();
    float getBotLeftLDRLight();
    float getTopRightLDRLight();
    float getBotRightLDRLight();
    void printLDRData();

    int setupGyro();
    double getZenith();
};

#endif