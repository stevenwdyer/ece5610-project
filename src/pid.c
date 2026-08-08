#include "pid.h"

void pid_init(controller_t *pid, float kp, float kd, float dt) {
    pid->kp = kp;
    pid->kd = kd;
    pid->dt = dt;
    pid->prev_error = 0;
    pid->derivative = 0;
}

float pid_update(controller_t *pid, float error) {
    float raw_deriviative = (error - pid->prev_error) / pid->dt;
    pid->derivative = 0.8 * pid->derivative + 0.2 * raw_deriviative;

    float output = pid->kp * error + pid->kd * pid->derivative;

    pid->prev_error = error;
    return output;
}

void pid_reset(controller_t *pid) {
    pid->prev_error = 0;
    pid->derivative = 0;
}