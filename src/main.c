#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "neopixels.h"

#define LED_PIN         25

int main() {
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
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        
        put_pixel(urgb_u32(0xff, 0, 0));  // Red
        sleep_ms(500);
        put_30_pixels(urgb_u32(0, 0xff, 0));
    } 

    return 1;
}