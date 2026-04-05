#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "motor.h"

static void motor_step_pulse() {
    gpio_put(MOTOR_STEP_PIN, 1);
    sleep_us(STEP_PULSE);
    gpio_put(MOTOR_STEP_PIN, 0);
}

void motor_hw_init() {
    gpio_init(MOTOR_STEP_PIN);
    gpio_set_dir(MOTOR_STEP_PIN, GPIO_OUT);
    gpio_put(MOTOR_STEP_PIN, 0);

    gpio_init(MOTOR_DIR_PIN);
    gpio_set_dir(MOTOR_DIR_PIN, GPIO_OUT);
    gpio_put(MOTOR_DIR_PIN, 0);
}

void motor_controller_init(motor_controller_t *mc, float accel_limit, float control_dt) {
    mc->command_speed = 0;
    mc->target_speed = 0;
    mc->accel_limit = accel_limit;
    mc->control_dt = control_dt;
    mc->last_step_us = time_us_64();
}

void motor_set_target_speed(motor_controller_t *mc, float target_speed) {
    mc->target_speed = target_speed;
}

void motor_update_speed_ramp(motor_controller_t *mc) {
    float max_delta = mc->accel_limit * mc->control_dt;

    if (mc->command_speed < mc->target_speed) {
        mc->command_speed += max_delta;
        if (mc->command_speed > mc->target_speed) {
            mc->command_speed = mc->target_speed;
        }
    } else if (mc->command_speed > mc->target_speed) {
        mc->command_speed -= max_delta;
        if (mc->command_speed < mc->target_speed) {
            mc->command_speed = mc->target_speed;
        }
    }
}

void motor_service_stepper(motor_controller_t *mc) {
    float speed = mc->command_speed;
    float mag = fabsf(speed);

    if (mag < 1) {
        return;
    }

    gpio_put(MOTOR_DIR_PIN, (speed > 0) ? 0 : 1);

    uint32_t interval_us = (uint32_t)(1000000 / mag);
    uint64_t now = time_us_64();

    if (now - mc->last_step_us >= interval_us) {
        motor_step_pulse();
        mc->last_step_us = now;
    }
}