#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "neopixels.h"
#include "ADXL343.h"
#include "transform.h"

#define LED_PIN         25

int main() {
    int i, err;

    stdio_init_all();

    // Delay with some LED blinking on startup
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    for (int i = 0; i < 5; i++) {
        sleep_ms(250);
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
    }

    // initialize the transform
    transform *trans = malloc(sizeof(transform));
    initialize_transform(trans);

    // initialize the LED strip
    setup_ws2812();
    put_30_pixels(urgb_u32(0x0f, 0xbf, 0x0f));

    while(1) {
        update_transform(trans);

        //printf("%f\t%f\t%f\t\n", trans->x, trans->y, trans->z);
    } 

    return 1;
}