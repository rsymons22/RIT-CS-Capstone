#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

// Addresses/pins
const int I2C_MUX_ADDR = 0x70;
const int LDR_ADDR = 0x23;
const int TEMPHUMIDITY_PIN = 5;
const int LA_RETRACT_PIN = 6;
const int LA_EXTEND_PIN = 7;
const int SM_IN1_PIN = 8;
const int SM_IN2_PIN = 9;
const int SM_IN3_PIN = 10;
const int SM_IN4_PIN = 11;
const int POWER_MONITOR_ADDR = 0x40;

// i2c mux channels
const int TOP_LEFT_LDR_CHANNEL = 2;
const int BOT_LEFT_LDR_CHANNEL = 3;
const int BOT_RIGHT_LDR_CHANNEL = 4;
const int TOP_RIGHT_LDR_CHANNEL = 5;
const int GYRO_CHANNEL = 7;

const long LOOP_DELAY_US = 200000;

// Approximate magentic declination offset for Rochester, New York. Negative means west.
const double MAGNETIC_DECLINATION = -11;

const double LATITUDE = 42.95;
const double LONGITUDE = -77.58;
const int UTC_OFFSET = -4;

// Calibration values for a different method of calculating heading, currently unused.
// // Hard-iron calibration settings
// const float HARD_IRON_CAL[3] = {
//     -4.08, -49.03, 3.70};

// // Soft-iron calibration settings
// const float SOFT_IRON_CAL[3][3] = {
//     {1.172, 0.178, -0.005},
//     {0.178, 1.057, -0.276},
//     {-0.005, -0.276, 0.902}};

const float MAG_MIN[3] = {-58.50, -77.25, -40.50};
const float MAG_MAX[3] = {67.95, 41.55, 79.80};

// Ultimately might not be necessary, accelerometer seems pretty accurate.
const float ACCEL_MIN[3] = {-9.6, -9.8, -9.97};
const float ACCEL_MAX[3] = {9.8, 9.66, 9.7};

// Physical limits of the dynamic solar panel's zenith actuation.
const int ZENITH_MIN = 20;
const int ZENITH_MAX = 65;

#endif