#pragma once

typedef struct {
    float kp;
    float kd;

    float dt;

    float prev_error;
    float derivative;
} controller_t;

void pid_init(controller_t *pid, float kp, float kd, float dt);
float pid_update(controller_t *pid, float error);
void pid_reset(controller_t *pid);