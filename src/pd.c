#include "pd.h"

void pd_init(controller_t *pd, float kp, float kd, float dt) {
    pd->kp = kp;
    pd->kd = kd;
    pd->dt = dt;
    pd->prev_error = 0;
    pd->derivative = 0;
}

float pd_update(controller_t *pd, float error) {
    float raw_deriviative = (error - pd->prev_error) / pd->dt;
    pd->derivative = 0.8 * pd->derivative + 0.2 * raw_deriviative;

    float output = pd->kp * error + pd->kd * pd->derivative;

    pd->prev_error = error;
    return output;
}

void pd_reset(controller_t *pd) {
    pd->prev_error = 0;
    pd->derivative = 0;
}