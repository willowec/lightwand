#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "neopixels.h"
#include "ADXL343.h"

#define LED_PIN         25

#define ACCEL_SDA_PIN   16
#define ACCEL_SCL_PIN   17

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

    // initialize the acceleromter
    printf("Seting up accel\n");
    adxl343 acceleromter;
    adxl343_setup(&acceleromter, i2c0, ACCEL_SDA_PIN, ACCEL_SCL_PIN, ADXL343_DEFAULT_ADDRESS);

    // initialize the LED strip
    setup_ws2812();
    put_30_pixels(urgb_u32(0x0f, 0xbf, 0x0f));

    int16_t x, y, z;    // z is the horizontal axis in this config

    uint64_t prev_time = time_us_64();
    double position = 0;

    while(1) {
        // print out the readings
        err = adxl343_getx(&acceleromter, &x);
        if (err < 0)
            printf("ERROR!!! %d\n", err);
        err = adxl343_gety(&acceleromter, &y);
        if (err < 0)
            printf("ERROR!!! %d\n", err);
        err = adxl343_getz(&acceleromter, &z);
        if (err < 0)
            printf("ERROR!!! %d\n", err);

        // printf("\tx: %d\ty: %d\tz: %d\n", x, y, z);

        // only pay atttention to movements above 1G
        if (abs(z) <= 32) z = 0;

        // update position
        uint64_t now = time_us_64();
        double delta = ((double)(now - prev_time) / 1000000.0f);
        position = position + z * delta * 4;

        if (position >=  127) position =  127;
        if (position <= -127) position = -127;

        uint8_t color = (int)position + 128;

        printf("col: %d, z: %d\tpos: %f\n", color, z, position);

        // convert to rgb
        put_30_pixels(urgb_u32(color, 255 - color, 0));

        
        prev_time = now;
    } 

    return 1;
}