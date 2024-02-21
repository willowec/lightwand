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
        /*

        // update position
        uint64_t now = time_us_64();
        double delta = ((double)(now - prev_time) / 1000000.0f);

        // Calculate the velocity of the wand by taking the derivative of the acceleration
        double velocity = (double)(z - prev_accel) * delta;
        position = position + velocity * 255 * 10;

        if (position >=  127) position =  127;
        if (position <= -127) position = -127;

        uint8_t color = (int)position + 128;

        printf("col: %d, z: %d\tpos: %f\n", color, z, position);

        // convert to rgb
        put_30_pixels(urgb_u32(color, 255 - color, 0));

        */

    } 

    return 1;
}