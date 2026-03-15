#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "imu.h"
#include "motor.h"

#define POT_PIN 26

void spin_motor_steps(int steps, bool clockwise, uint delay_us) {
    gpio_put(MOTOR_DIR_PIN, clockwise ? 1 : 0); // Set direction

    for (int i = 0; i < steps; i++) {
        gpio_put(MOTOR_STEP_PIN, 1);
        sleep_us(delay_us); // Dictates speed: lower value means faster spin
        gpio_put(MOTOR_STEP_PIN, 0);
        sleep_us(delay_us);
    }
}

int main() {
    stdio_init_all();
    gpio_init(MOTOR_DIR_PIN);
    gpio_init(MOTOR_STEP_PIN);
    gpio_set_dir(MOTOR_DIR_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_DIR_PIN, GPIO_OUT);

    while (1) {
        // Spin clockwise for 200 steps (one revolution at full steps)
        spin_motor_steps(200, true, 1000); 
        sleep_ms(2000); // Wait 2 seconds

        // Spin counter-clockwise
        spin_motor_steps(200, false, 500); 
        sleep_ms(2000); // Wait 2 seconds
    }
    return 0;
}