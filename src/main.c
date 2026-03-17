#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "imu.h"
#include "motor.h"

#define POT_PIN 26
#define BALANCE_POINT 3200
#define DEADBAND 10

static void motor_step(bool dir, uint delay_us) {
    gpio_put(MOTOR_DIR_PIN, dir);
    sleep_us(5);

    gpio_put(MOTOR_STEP_PIN, 1);
    sleep_us(delay_us);
    gpio_put(MOTOR_STEP_PIN, 0);
    sleep_us(delay_us);
}

static uint compute_delay_us(int error_mag) {
    if (error_mag > 1000) return 300;
    if (error_mag > 600)  return 500;
    if (error_mag > 300)  return 800;
    if (error_mag > 100)  return 1200;
    return 2000;
}

int main () {
    stdio_init_all();

    gpio_init(MOTOR_STEP_PIN);
    gpio_init(MOTOR_DIR_PIN);

    gpio_set_dir(MOTOR_STEP_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_DIR_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(0);

    while (1) {
        uint16_t pot_value = adc_read();
        int error = (int)pot_value - BALANCE_POINT;
        int mag = (error >= 0) ? error : -error;

        printf("pot = %u error = %d\n", pot_value, error);

        if (pot_value > BALANCE_POINT + DEADBAND) {
            motor_step(true, compute_delay_us(mag));
        } else if (pot_value < BALANCE_POINT - DEADBAND) {
            motor_step(false, compute_delay_us(mag));
        } else {
            sleep_ms(2);
        }

    }
}