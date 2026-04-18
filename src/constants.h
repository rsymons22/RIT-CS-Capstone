#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <cstdint>

// Addresses
const int I2C_MUX_ADDR = 0x70;
const int LDR_ADDR = 0x23;
const int POWER_MONITOR_ADDR = 0x40;

// Digital Pins
const int ENCODER_PINA = 2;
const int ENCODER_PINB = 3;
const int JOYSTICK_BUTTON_PIN = 4;
const int TEMPHUMIDITY_PIN = 5;
const int LA_RETRACT_PIN = 6;
const int LA_EXTEND_PIN = 7;
const int LA_SPEED_PIN = 9;
const int PANEL_TYPE_PIN = 10;

// Analog Pins
const int JOYSTICK_Y_PIN = 14;
const int JOYSTICK_X_PIN = 15;

// i2c mux channels
const int TOP_LEFT_LDR_CHANNEL = 2;
const int BOT_LEFT_LDR_CHANNEL = 3;
const int BOT_RIGHT_LDR_CHANNEL = 4;
const int TOP_RIGHT_LDR_CHANNEL = 5;
const int GYRO_CHANNEL = 7;

// Encoder counts per rotation / # of teeth on the gear attached to encoder.
// Turntable gear has 360 teeth.
const int ENCODER_COUNTS_PER_DEGREE = 1200 / 25;
const int ENCODER_MINMAX = 90 * ENCODER_COUNTS_PER_DEGREE;

const long LOOP_DELAY = 1000;
const long ADJUSTMENT_DELAY = 60000; // 1 minute
const long SUN_PULSE_ANIM_DELAY = 100;
const long JOYSTICK_DELAY = 200;

const double LATITUDE = 42.95;
const double LONGITUDE = -77.58;
const int UTC_OFFSET = -4;

const float MPU_CAL[] = {-15369.11, 3469, -980.08};

// Limits of the dynamic solar panel's zenith actuation.
const int ZENITH_MIN = 21;
const int ZENITH_MAX = 66;

// Limits of the dynamic solar panel's azimuth rotation. Can physically go a bit farther, but safer for +-90 limits.
const int AZIMUTH_MIN = 90;
const int AZIMUTH_MAX = 270;

const double LA_SPEED_PERCENT = 0.25;

// Power monitor channels
const int PM_PANEL_CHANNEL = 0;
const int PM_SYSTEM_CHANNEL = 1;

const int COMM_PORT = 1883;

const uint32_t sunpulse[][3] = {
    {0x6,
     0xf00f00,
     0x60000000},
    {0x10806,
     0xf00f00,
     0x60108000},
    {0x6010806,
     0x2f42f40,
     0x60108060},
    {0x26410806,
     0x2f42f40,
     0x60108264},
    {0x26410806,
     0x6f66f60,
     0x60108264},
    {0x26400006,
     0x4f24f20,
     0x60000264},
    {0x20400006,
     0x4f24f20,
     0x60000204},
    {0x6,
     0x8f18f10,
     0x60000000}};

#endif