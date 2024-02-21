#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/double.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "neopixels.h"
#include "ADXL343.h"

#define LED_PIN         25

#define ADX_SDA_PIN     16
#define ADX_SCL_PIN     17

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

    // initialize the accelerometer
    adxl343 accelerometer;
    adxl343_setup(&accelerometer, i2c0, ADX_SDA_PIN, ADX_SCL_PIN, ADXL343_DEFAULT_ADDRESS);

    // initialize the LED strip
    setup_ws2812();
    put_30_pixels(urgb_u32(0x0f, 0xbf, 0x0f));

    int16_t ax_raw, ay_raw, az_raw;
    double acceleration;

    while(1) {
        // update raw adx readings
        adxl343_getx(&accelerometer, &ax_raw); 
        adxl343_gety(&accelerometer, &ay_raw);
        adxl343_getz(&accelerometer, &az_raw);

        // calculate the magnitude of the acceleration, excluding gravity
        acceleration = sqrt((double)pow(ax_raw, 2) + (double)pow(ay_raw, 2) + (double)pow(az_raw, 2));
        acceleration = (acceleration - ADXL3XXVAL_1G) * ADXL3XXVAL_TO_MSS;

        printf("%d\t%d\t%d\t%f\n", ax_raw, ay_raw, az_raw, acceleration);

    } 

    return 1;
}