/*
Main C file for the accelerometer data acquisition build target
Sends the Z-axis acceleration and system time over serial
*/

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/double.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "ADXL343.h"


// gpio pin defines
#define LED_PIN         25
#define ADX_SDA_PIN     16
#define ADX_SCL_PIN     17

int main()
{
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

    printf("Starting...\n");
    // initialize the accelerometer
    adxl343 accelerometer;
    err = adxl343_setup(&accelerometer, i2c0, ADX_SDA_PIN, ADX_SCL_PIN, ADXL343_DEFAULT_ADDRESS);
    if (err < 0) {
        printf("ADXL343 Setup failed... error %d\n", err);
    }
    printf("Accelerometer setup complete...\n");  

    int16_t ay_raw;
    double accel_mss = 0;

    while (1) {
        // update raw adx reading
        adxl343_gety(&accelerometer, &ay_raw);

        // convert to acceleration in meters/s^2
        accel_mss = (double)ay_raw * ADXL3XXVAL_TO_MSS;

        printf("%f,%d\n", accel_mss, time_us_64());
    }
}