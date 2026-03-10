#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define POT_PIN 26

int main() {
    stdio_init_all();
    sleep_ms(2000);

    adc_init();

    adc_gpio_init(POT_PIN);
    adc_select_input(0);
    
    while (true) {
        uint16_t result = adc_read();
        printf("Raw value: %d, Voltage: %.2f V\n", result, result * 3.3f / 4095.f);
        sleep_ms(250);
    }
}