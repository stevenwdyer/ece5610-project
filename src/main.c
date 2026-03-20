#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define POT_PIN         26
#define POT_ADC_INPUT   0

#define IN1_PIN         8
#define IN2_PIN         9
#define IN3_PIN         10
#define IN4_PIN         11

#define BALANCE_POINT   2048
#define DEADBAND        150

// Half-step sequence for smoother motion
static const uint8_t step_table[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

static int current_step = 0;

static void motor_output_step(int step_index) {
    gpio_put(IN1_PIN, step_table[step_index][0]);
    gpio_put(IN2_PIN, step_table[step_index][1]);
    gpio_put(IN3_PIN, step_table[step_index][2]);
    gpio_put(IN4_PIN, step_table[step_index][3]);
}

static void motor_release(void) {
    gpio_put(IN1_PIN, 0);
    gpio_put(IN2_PIN, 0);
    gpio_put(IN3_PIN, 0);
    gpio_put(IN4_PIN, 0);
}

static void motor_step(int dir, uint32_t delay_us) {
    current_step += dir;

    if (current_step >= 8) current_step = 0;
    if (current_step < 0)  current_step = 7;

    motor_output_step(current_step);
    sleep_us(delay_us);
}

static uint32_t compute_delay_us(int error_mag) {
    if (error_mag > 1600) return 800;
    if (error_mag > 1200) return 1200;
    if (error_mag > 800)  return 1800;
    if (error_mag > 400)  return 2500;
    return 4000;
}

int main() {
    stdio_init_all();

    gpio_init(IN1_PIN);
    gpio_init(IN2_PIN);
    gpio_init(IN3_PIN);
    gpio_init(IN4_PIN);

    gpio_set_dir(IN1_PIN, GPIO_OUT);
    gpio_set_dir(IN2_PIN, GPIO_OUT);
    gpio_set_dir(IN3_PIN, GPIO_OUT);
    gpio_set_dir(IN4_PIN, GPIO_OUT);

    motor_release();

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
            motor_release();
            sleep_ms(2);
        }
    }

    return 0;
}