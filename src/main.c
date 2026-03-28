#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "motor.h"

#define POT_PIN         26
#define POT_ADC_INPUT   0
#define ALPHA           0.1

#define BALANCE_POINT   2176 // 1874 fully tilted back, 2477 fully tilted forward
#define DEADBAND        15

float filtered_pot = BALANCE_POINT;

static void motor_step_pulse() {
    gpio_put(MOTOR_STEP_PIN, 1);
    sleep_us(STEP_PULSE);
    gpio_put(MOTOR_STEP_PIN, 0);
}

static void motor_step(int dir, uint32_t step_delay_us) {
    gpio_put(MOTOR_DIR_PIN, (dir > 0) ? 1 : 0);

    sleep_us(5);

    motor_step_pulse(10);

    sleep_us(step_delay_us);
}

static uint32_t compute_delay_us(int error_mag) {
    if (error_mag > 250) return 400;
    if (error_mag > 180) return 700;
    if (error_mag > 120)  return 1100;
    if (error_mag > 60)  return 1800;
    if (error_mag > 25)  return 2600;
    return 4000;
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

        filtered_pot += ALPHA * ((float)pot_value - filtered_pot);

        int error = BALANCE_POINT - filtered_pot;
        int mag = (error >= 0) ? error : -error;

        printf("pot=%u error=%d\n", pot_value, error);

        if (error > DEADBAND) {
            motor_step(-1, compute_delay_us(mag));
        }
        else if (error < -DEADBAND) {
            motor_step(+1, compute_delay_us(mag));
        }
        else {
            sleep_ms(2);
        }
    }

    return 0;
}