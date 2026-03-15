#include "imu.h"

#include "hardware/i2c.h"
#include "pico/stdlib.h"

static uint8_t rx_buf[16];
static uint8_t tx_buf[16];

static void imu_read(uint8_t reg) {
    tx_buf[0] = reg;
    i2c_write_blocking(IMU_I2C, IMU_ADDRESS, tx_buf, 1, true);
    i2c_read_blocking(IMU_I2C, IMU_ADDRESS, rx_buf, 1, false);
    
}

static void imu_write(uint8_t reg, uint8_t val) {
    tx_buf[0] = reg;
    tx_buf[1] = val;
    i2c_write_blocking(IMU_I2C, IMU_ADDRESS, tx_buf, 1, false);
}

static void imu_init() {
    i2c_init(IMU_I2C, IMU_I2C_FREQ);
    gpio_set_function(IMU_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(IMU_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(IMU_SDA_PIN);
    gpio_pull_up(IMU_SCL_PIN);
    sleep_ms(10);
}