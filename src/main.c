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

    put_30_pixels(urgb_u32(0x0f, 0xbf, 0x0f));

    while(1) {

    } 

    return 1;
}