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
#define ACCEL_MAX_MSS           40
#define DIRECTION_TIMEOUT_US    1000 * 100

// defines relating to text display
#define COL_SHOW_TIME_US        1000 * 3


// choose the message to display on the wand
#define MESSAGE_LEN             3
#define MESSAGE_LEN_COLUMNS     MESSAGE_LEN * CHAR_WIDTH
const uint32_t *message[MESSAGE_LEN] = {CHAR_A, CHAR_B, CHAR_C};

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
    printf("Accelerometer setuo complete...\n");  
    // initialize the LED strip
    setup_ws2812();
    put_15_pixels_on(urgb_u32(0x0f, 0xbf, 0x0f));
    printf("LED's lit green\n");
    // variables relating to wand position
    int16_t az_raw;
    double accel_mss;
    int dir = 0;    // what direction is the wand moving in
    uint64_t last_moved_time_us = 0;

    // variables relating to the text message
    uint64_t last_changed_col_time_us = 0;
    int message_index = -1;

    while(1) {
        //printf("Main loop.\n");
        //put_15_pixels_on(urgb_u32(0x01, 0x01, 0x01));
        //sleep_ms(200);
        
        uint64_t now = time_us_64();

        // update raw adx reading
        adxl343_getz(&accelerometer, &az_raw);

        // convert to acceleration in meters/s^2
        accel_mss = (double)az_raw * ADXL3XXVAL_TO_MSS;

        // update "direction" of stick when the acceleration changes suddenly
        if (accel_mss < -ACCEL_MAX_MSS) {
            last_moved_time_us = now;
            dir = -1;

            if (message_index == -1) 
                message_index = MESSAGE_LEN_COLUMNS - 1;
        }
        else if (accel_mss >  ACCEL_MAX_MSS) {
            last_moved_time_us = now;
            dir =  1;
            
            if (message_index == -1) 
                message_index = 0;
        }
        else if (last_moved_time_us < (now - DIRECTION_TIMEOUT_US)) {
            // if the wand has not 'changed direction' in a while, assume it has stopped moving
            dir = 0;
            message_index = -1;
            put_15_pixels_on(urgb_u32(0xff, 0, 0));
        }

        // scroll through the columns of the characters of the message
        if ((last_changed_col_time_us + COL_SHOW_TIME_US < now) && message_index != -1) {
            message_index = message_index + dir;
            if (message_index > MESSAGE_LEN_COLUMNS - 1) message_index = 0;
            if (message_index < 0) message_index = MESSAGE_LEN_COLUMNS -1;
            last_changed_col_time_us = now;

            printf("moving to col %d\n", message_index);

            put_15_pixels(message[message_index/CHAR_WIDTH][message_index % CHAR_WIDTH], urgb_u32(0x0f, 0xbf, 0x0f), urgb_u32(0, 0, 0));
        }
        
        
    } 

    return 1;
}