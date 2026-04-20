#include "pico/stdlib.h"
#define D1 22
#define D2 21
#define D3 20
#define BLINK_RATE_MS 500 // 1Hz, 1000ms 500ms on, 500ms off

int main() {
    stdio_init_all();
    //gpio_init(D1);
    gpio_init(D2);
    //gpio_init(D3);
    //gpio_set_dir(D1, GPIO_OUT);
    gpio_set_dir(D2, GPIO_OUT);
    //gpio_set_dir(D3, GPIO_OUT);

    while (true) {
        //What value needs to be written to the LED to turn it on?  1,high.
        gpio_put(D2, 1);
        sleep_ms(BLINK_RATE_MS);
        gpio_put(D2, 0);
        sleep_ms(BLINK_RATE_MS);
    }


}