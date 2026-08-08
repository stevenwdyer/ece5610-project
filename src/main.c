#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pid.h"
#include "motor.h"

#define POT_PIN           26
#define POT_ADC_INPUT     0
#define ALPHA             0.1

#define BALANCE_POINT     2280
#define DEADBAND          10

#define CONTROL_DT        0.01
#define CONTROL_DT_US     10000

#define ACCEL_LIMIT       8000
#define MAX_SPEED         3000

float filtered_pot = BALANCE_POINT;

uint16_t read_pot_average(int samples) {
    uint32_t sum = 0;

    for (int i = 0; i < samples; i++) {
        sum += adc_read();
    }

    return (uint16_t)(sum / samples);
}

int main() {
    stdio_init_all();

    motor_hw_init();

    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(POT_ADC_INPUT);

    controller_t pid;
    pid_init(&pid, 8, 0.8, CONTROL_DT);

    motor_controller_t motor_ctrl;
    motor_controller_init(&motor_ctrl, ACCEL_LIMIT, CONTROL_DT);

    sleep_ms(1000);

    uint64_t last_control_us = time_us_64();

    while (1) {
        uint64_t now = time_us_64();

        if (now - last_control_us >= CONTROL_DT_US) {
            uint16_t pot_value = read_pot_average(8);
            filtered_pot += ALPHA * ((float)pot_value - filtered_pot);

            float error = filtered_pot - BALANCE_POINT;
            float output = 0;

            if (fabsf(error) < DEADBAND) {
                error = 0;
                output = 0;
                pid_reset(&pid);
            } else {
                output = pid_update(&pid, error);

                if (output > MAX_SPEED) {
                    output = MAX_SPEED;
                }
                if (output < -MAX_SPEED) {
                    output = -MAX_SPEED; 
                }
            }

            motor_set_target_speed(&motor_ctrl, output);
            motor_update_speed_ramp(&motor_ctrl);

            printf("pot=%.1f err=%.1f out=%.1f cmd=%.1f der=%.1f\n",
                   filtered_pot, error, output, motor_ctrl.command_speed, pid.derivative);

            last_control_us += CONTROL_DT_US;
        }

        motor_service_stepper(&motor_ctrl);
    }

    return 0;
}