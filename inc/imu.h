#pragma once

#include "hardware/i2c.h"

#define IMU_I2C i2c1
#define IMU_SDA_PIN 14
#define IMU_SCL_PIN 15

#define IMU_ADDRESS     0x68
#define WHO_AM_I        0x75
#define PWR_MGMT_1      0x6B
#define CONFIG          0x1A
#define GYRO_CONFIG     0x1B
#define ACCEL_CONFIG    0x1C

typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t temp;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} imu_raw_t;

static void imu_read(uint8_t reg);
static void imu_write(uint8_t reg, uint8_t val);
static void imu_init();
