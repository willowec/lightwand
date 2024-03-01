#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/double.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "neopixels.h"
#include "ADXL343.h"
#include "alphabet.h"

// gpio pin defines
#define LED_PIN         25
#define ADX_SDA_PIN     16
#define ADX_SCL_PIN     17

// defines relating to wand position
#define ACCEL_MAX_MSS           10
#define DIRECTION_TIMEOUT_US    1000 * 1000

// defines relating to text display
#define COL_SHOW_TIME_US        1000 * 10


// choose the message to display on the wand
#define MESSAGE_LEN     3
const uint32_t *message[MESSAGE_LEN] = {CHAR_E, CHAR_C, CHAR_E};

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

    // variables relating to wand position
    int16_t az_raw;
    double accel_mss;
    int dir = 0;    // what direction is the wand moving in
    uint64_t last_moved_time_us = 0;

    // variables relating to the text message
    uint64_t last_changed_col_time_us = 0;
    int message_index = 0;

    while(1) {
        // update raw adx reading
        adxl343_getz(&accelerometer, &az_raw);

        // convert to acceleration in meters/s^2
        accel_mss = (double)az_raw * ADXL3XXVAL_TO_MSS;

        // update "direction" of stick when the acceleration changes suddenly
        if (accel_mss < -ACCEL_MAX_MSS) {
            last_moved_time_us = time_us_64();
            dir = -1;
        }
        if (accel_mss >  ACCEL_MAX_MSS) {
            last_moved_time_us = time_us_64();
            dir =  1;
        }

        // if the wand has not 'changed direction' in a while, assume it has stopped moving
        if (last_moved_time_us < (time_us_64() - DIRECTION_TIMEOUT_US)) dir = 0;


        // move through the columns of the characters of the message


        // 
        /*
        // change color of stick based on position
        if (dir < 0) {
            // stick is moving to the left with reference to the user
            put_30_pixels(urgb_u32(0xff, 0x00, 0x00));
        }
        else if (dir > 0) {
            // stick is moving to the right with reference to the user
            put_30_pixels(urgb_u32(0x00, 0xff, 0x00));
        }
        else {
            // stick is assumed to be not moving
            put_30_pixels(urgb_u32(0x00, 0x00, 0xff));
        }
        */
    } 

    return 1;
}