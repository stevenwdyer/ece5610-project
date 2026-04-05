#pragma once

typedef struct {
    float kp;
    float kd;

    float dt;

    float prev_error;
    float derivative;
} controller_t;

void pid_init(controller_t *pd, float kp, float kd, float dt);
float pd_update(controller_t *pd, float error);
void pid_reset(controller_t *pd);