#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "neopixels.h"

#define LED_PIN         25

int main() {
    int i;

    stdio_init_all();

    // Delay with some LED blinking on startup
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    for (int i = 0; i < 5; i++) {
        sleep_ms(100);
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
    }

    setup_ws2812();


    while(1) {
        for (i=0; i<0xff; i++) {
            put_30_pixels(urgb_u32(i, 0xff, 0xff));
        }
        for (i=0; i<0xff; i++) {
            put_30_pixels(urgb_u32(0xff, i, 0xff));
        }
        for (i=0; i<0xff; i++) {
            put_30_pixels(urgb_u32(0xff, 0xff, i));
        }
    } 

    return 1;
}