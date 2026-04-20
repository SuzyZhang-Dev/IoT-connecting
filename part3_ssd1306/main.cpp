//
// Created by 张悦 on 29.3.2026.
//

#include "pico/stdlib.h"
#include "OLEDDisplay.h"
#include "hardware/pwm.h"

#define BRIGHTNESS 50 // 5% of 1000
#define LED_COUNT 3

const uint pins[LED_COUNT]           = {22, 21, 20};
const uint32_t toggle_periods[LED_COUNT] = {125, 250, 500};
uint32_t last_toggles[LED_COUNT]     = {0, 0, 0};
bool states[LED_COUNT]               = {false, false, false};

OLEDDisplay oled;

int main(void) {
    stdio_init_all();
    for (int i = 0; i < LED_COUNT; i++) {
        gpio_set_function(pins[i], GPIO_FUNC_PWM);
        uint slice = pwm_gpio_to_slice_num(pins[i]);
        uint channel = pwm_gpio_to_channel(pins[i]);
        pwm_set_wrap(slice, 999);
        pwm_set_chan_level(slice, channel, 0);
        pwm_set_enabled(slice, true);
    }

    uint32_t last_print = 0;
    uint32_t last_oled = 0;

    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());

        for (int i = 0; i < LED_COUNT; i++) {
            if (now - last_toggles[i] >= toggle_periods[i]) {
                states[i] = !states[i];
                pwm_set_chan_level(
                    pwm_gpio_to_slice_num(pins[i]),
                    pwm_gpio_to_channel(pins[i]),
                    states[i] ? BRIGHTNESS : 0
                );
                last_toggles[i] = now;
            }
        }

        if (now - last_oled >= 1000) {
            oled.show_time_image(now/1000);
            last_oled = now;
        }

        if (now - last_print >= 1000) {
            printf("Elapsed: %u s\n", now / 1000);
            last_print = now;
        }

    }
}

