#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "motor.h"

#define POT_PIN           26
#define POT_ADC_INPUT     0
#define ALPHA             0.1

#define BALANCE_POINT     2176
#define DEADBAND          8

#define CONTROL_DT        0.01
#define CONTROL_DT_US     10000

#define ACCEL_LIMIT       1200
#define MAX_SPEED         3000
#define INTEGRAL_LIMIT    300

float filtered_pot = BALANCE_POINT;

float command_speed = 0;
float target_speed = 0;

float integral = 0;
float prev_error = 0;
float derivative = 0;

float Kp = 8;
float Ki = 0;
float Kd = 0;

uint64_t last_step_us = 0;
uint64_t last_control_us = 0;

static void motor_step_pulse() {
    gpio_put(MOTOR_STEP_PIN, 1);
    sleep_us(STEP_PULSE);
    gpio_put(MOTOR_STEP_PIN, 0);
}

static void update_speed_ramp() {
    float max_delta = ACCEL_LIMIT * CONTROL_DT;

    if (command_speed < target_speed) {
        command_speed += max_delta;
        if (command_speed > target_speed) {
            command_speed = target_speed;
        }
    } else if (command_speed > target_speed) {
        command_speed -= max_delta;
        if (command_speed < target_speed) {
            command_speed = target_speed;
        }
    }
}

static void service_stepper() {
    float speed = command_speed;
    float mag = fabsf(speed);

    if (mag < 1.0f) {
        return;
    }

    gpio_put(MOTOR_DIR_PIN, (speed > 0) ? 0 : 1);

    uint32_t interval_us = (uint32_t)(1000000.0f / mag);
    uint64_t now = time_us_64();

    if (now - last_step_us >= interval_us) {
        motor_step_pulse();
        last_step_us = now;
    }
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

    last_control_us = time_us_64();
    last_step_us = time_us_64();

    while (1) {
        uint64_t now = time_us_64();

        if (now - last_control_us >= CONTROL_DT_US) {
            uint16_t pot_value = adc_read();
            filtered_pot += ALPHA * ((float)pot_value - filtered_pot);

            float error = BALANCE_POINT - filtered_pot;

            if (fabsf(error) < DEADBAND) {
                error = 0.0f;
            }

            integral += error * CONTROL_DT;
            if (integral > INTEGRAL_LIMIT) integral = INTEGRAL_LIMIT;
            if (integral < -INTEGRAL_LIMIT) integral = -INTEGRAL_LIMIT;

            float raw_derivative = (error - prev_error) / CONTROL_DT;
            derivative = 0.8f * derivative + 0.2f * raw_derivative;

            float output = Kp * error + Ki * integral + Kd * derivative;

            if (output > MAX_SPEED) output = MAX_SPEED;
            if (output < -MAX_SPEED) output = -MAX_SPEED;

            target_speed = output;
            prev_error = error;

            update_speed_ramp();

            printf("pot=%.1f err=%.1f out=%.1f cmd=%.1f int=%.1f der=%.1f\n",
                   filtered_pot, error, target_speed, command_speed, integral, derivative);

            last_control_us += CONTROL_DT_US;
        }

        service_stepper();
    }

    return 0;
}