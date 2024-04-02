/*
Main C file for the light wand project
*/

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
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
#define ACCEL_MAX_MSS               30
#define DIRECTION_TIMEOUT_US        1000 * 500
#define DIRECTION_HYSTERESIS_MASK   0x00ffffff

// defines relating to text display
#define COL_SHOW_TIME_US        1000 * 2

#define PIXEL_CHAR_COLOR        urgb_u32(90, 149, 207)
#define PIXEL_BG_COLOR          urgb_u32(0, 0, 5)   
#define PIXEL_REST_COLOR        urgb_u32(5, 5, 5)

// choose the message to display on the wand
#define MESSAGE_LEN             3
#define MESSAGE_LEN_COLUMNS     MESSAGE_LEN * CHAR_WIDTH
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

    printf("Starting...\n");
    // initialize the accelerometer
    adxl343 accelerometer;
    err = adxl343_setup(&accelerometer, i2c0, ADX_SDA_PIN, ADX_SCL_PIN, ADXL343_DEFAULT_ADDRESS);
    if (err < 0) {
        printf("ADXL343 Setup failed... error %d\n", err);
    }
    printf("Accelerometer setup complete...\n");  

    // initialize the LED strip
    setup_ws2812();
    put_15_pixels_on(urgb_u32(0x0f, 0xbf, 0x0f));
    printf("LED's lit green\n");

    // variables relating to wand position
    int16_t ay_raw;
    float accel_mss = 0;
    float prev_accel_mss = 0;
    float jerk_msss = 0;
    uint64_t prev_time_us = 0;
    uint64_t hidden_direction_history = 0;  // lsb is current hidden direction
    uint64_t diplay_direction_history = 0;  // lsb is current display direction.

    // variables relating to the text message
    uint64_t last_changed_col_time_us = 0;
    int message_index = -1;

    while(1) {   
        uint64_t now = time_us_64();

        // update raw adx reading
        adxl343_gety(&accelerometer, &ay_raw);

        // convert to acceleration in meters/s^2
        accel_mss = (float)ay_raw * ADXL3XXVAL_TO_MSS;

        // calculate the jerk of the wand in meters/s^3
        jerk_msss = (accel_mss - prev_accel_mss) / ((float)(now - prev_time_us) / 1000000.0f);
        
        // caculate the direction of the wand based on jerk
        if (jerk_msss < 0) {
            // if jerk is negative, wand is moving 'left' (0)
            hidden_direction_history = (hidden_direction_history << 1) | 0;
        }
        else if (jerk_msss > 0) {
            // if jerk is positive, wand is moving 'right' (1)
            hidden_direction_history = (hidden_direction_history << 1) | 1;
        }
        else {
            // otherwise, assume wand is continuing in the same direction
            i = hidden_direction_history & 1;
            hidden_direction_history = (hidden_direction_history << 1) | i;
        }

        // update the displayed direction based on the hidden direction
        if ((hidden_direction_history & DIRECTION_HYSTERESIS_MASK) == 0) {
            // all previous hidden directions in DIRECTION_HYSTERESIS_MASK were '0'
            diplay_direction_history = (diplay_direction_history << 1) | 0;
        }
        else if ((hidden_direction_history & DIRECTION_HYSTERESIS_MASK) == DIRECTION_HYSTERESIS_MASK) {
            // all previous hidden directions in DIRECTION_HYSTERESIS_MASK were '1'
            diplay_direction_history = (diplay_direction_history << 1) | 1;
        }
        else {
            i = diplay_direction_history & 1;
            diplay_direction_history = (diplay_direction_history << 1) | i;
        }

        // display the direction of the wand on the wand
        if ((diplay_direction_history & 1) == 1)
            put_15_pixels_on(urgb_u32(255, 0, 0));
        else if ((diplay_direction_history & 1) == 0)
            put_15_pixels_on(urgb_u32(0, 255, 0));
        else
            put_15_pixels_on(urgb_u32(255, 255, 255));


        /*
        // scroll through the columns of the characters of the message
        if ((last_changed_col_time_us + COL_SHOW_TIME_US < now) && message_index != -1) {
            message_index = message_index + dir;
            if (message_index > MESSAGE_LEN_COLUMNS - 1) message_index = 0;
            if (message_index < 0) message_index = MESSAGE_LEN_COLUMNS -1;
            last_changed_col_time_us = now;

            put_15_pixels(message[message_index/CHAR_WIDTH][message_index % CHAR_WIDTH], 
                PIXEL_CHAR_COLOR, PIXEL_BG_COLOR);
        }
        */

        prev_accel_mss = accel_mss;
        prev_time_us = now;
    } 

    return 1;
}