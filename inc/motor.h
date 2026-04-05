#pragma once

#include <stdint.h>

#define MOTOR_STEP_PIN 7
#define MOTOR_DIR_PIN 6
#define STEP_PULSE 10

typedef struct {
    float command_speed;
    float target_speed;
    float accel_limit;
    float control_dt;
    uint64_t last_step_us;
} motor_controller_t;

void motor_controller_init(motor_controller_t *mc, float accel_limit, float control_dt);
void motor_hw_init();
void motor_set_target_speed(motor_controller_t *mc, float target_speed);
void motor_update_speed_ramp(motor_controller_t *mc);
void motor_service_stepper(motor_controller_t *mc);