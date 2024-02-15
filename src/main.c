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

    int16_t x, y, z;

    while(1) {
        err = adxl343_getx(&acceleromter, &x);
        if (err < 0)
            printf("ERROR!!! %d\n", err);
        err = adxl343_gety(&acceleromter, &y);
        if (err < 0)
            printf("ERROR!!! %d\n", err);
        err = adxl343_getz(&acceleromter, &z);
        if (err < 0)
            printf("ERROR!!! %d\n", err);

        printf("\tx: %d\ty: %d\tz: %d\n", x, y, z);
    } 

    return 1;
}