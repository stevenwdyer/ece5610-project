#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "motor.h"

#define POT_PIN         26
#define POT_ADC_INPUT   0

#define IN1_PIN         8
#define IN2_PIN         9
#define IN3_PIN         10
#define IN4_PIN         11

#define BALANCE_POINT   2048
#define DEADBAND        150

static void motor_step_pulse(uint32_t pulse_width_us) {
    gpio_put(MOTOR_STEP_PIN, 1);
    sleep_us(pulse_width_us);
    gpio_put(MOTOR_STEP_PIN, 0);
}

static void motor_step(int dir, uint32_t step_delay_us) {
    gpio_put(MOTOR_DIR_PIN, (dir > 0) ? 1 : 0);

    sleep_us(5);

    motor_step_pulse(10);

    sleep_us(step_delay_us);
}

static uint32_t compute_delay_us(int error_mag) {
    if (error_mag > 1600) return 300;
    if (error_mag > 1200) return 600;
    if (error_mag > 800)  return 1000;
    if (error_mag > 400)  return 1800;
    return 3000;
}

int main() {
    stdio_init_all();

    gpio_init(MOTOR_STEP_PIN);
    gpio_set_dir(MOTOR_STEP_PIN, GPIO_OUT);
    gpio_put(MOTOR_STEP_PIN, 0);

    gpio_init(MOTOR_DIR_PIN);
    gpio_set_dir(MOTOR_DIR_PIN, GPIO_OUT);
    gpio_put(MOTOR_DIR_PIN, 0);

    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(POT_ADC_INPUT);

    sleep_ms(1000);

    while (1) {
        uint16_t pot_value = adc_read();
        int error = (int)pot_value - BALANCE_POINT;
        int mag = (error >= 0) ? error : -error;

        printf("pot=%u error=%d\n", pot_value, error);

        if (pot_value > BALANCE_POINT + DEADBAND) {
            motor_step(+1, compute_delay_us(mag));
        }
        else if (pot_value < BALANCE_POINT - DEADBAND) {
            motor_step(-1, compute_delay_us(mag));
        }
        else {
            sleep_ms(2);
        }
    }

    return 0;
}