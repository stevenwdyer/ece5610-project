#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "imu.h"

#define POT_PIN 26

static bool mpu_read_reg(uint8_t reg, uint8_t *value) {
    int w = i2c_write_blocking(IMU_I2C, IMU_ADDRESS, &reg, 1, true);
    if (w != 1) {
        return false;
    }
    int r = i2c_read_blocking(IMU_I2C, IMU_ADDRESS, value, 1, false);
    if (r != 1) {
        return false;
    }

    return true;
}

int main() {
    stdio_init_all();
    sleep_ms(3000);

    printf("program start\r\n");

    i2c_init(IMU_I2C, 400000);
    printf("i2c_init done\r\n");

    gpio_set_function(IMU_SDA_PIN, GPIO_FUNC_I2C);
    printf("SDA set\r\n");

    gpio_set_function(IMU_SCL_PIN, GPIO_FUNC_I2C);
    printf("SCL set\r\n");

    gpio_pull_up(IMU_SDA_PIN);
    gpio_pull_up(IMU_SCL_PIN);
    printf("pullups enabled\r\n");

    sleep_ms(1000);

    while (true) {
        uint8_t who = 0;
        printf("before WHO_AM_I read\r\n");

        if (mpu_read_reg(WHO_AM_I, &who)) {
            printf("WHO_AM_I = 0x%02X\r\n", who);
        } else {
            printf("WHO_AM_I read failed\r\n");
        }

        sleep_ms(1000);
    }
}