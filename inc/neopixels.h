#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"


// taken from hutscape.github.io. Puts a pixel onto the neopixel array
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// lights up a full 30 pixel (1m) neopixel strip as one color
static inline void put_30_pixels(uint32_t pixel_grb) {
    /*
    pio_sm_put_blocking will always put out 32 bits, but we want to send 24 bit chunks in a
    continuous stream. Therefore, some slightly more complicated looping behaviour is required
    */
    int color_channel = 0; // 0: green, 1: red, 2: blue
    int transmitted_bytes = 0;
    int i;
    while(1) {
        uint32_t msg = 0;
        for(i=3; i>=0; i--) {
            msg |= ((pixel_grb >> ((2-color_channel) * 8u)) && 0xff) << (i * 8u); // put the next color chanel into the chain, in order
            color_channel = (color_channel + 1) % 3;
            transmitted_bytes++;
        }

        pio_sm_put_blocking(pio0, 0, msg);

        if (transmitted_bytes >= 90) {
            // 90 bytes of color data, for 30 pixels, have been transmitted
            break;
        }
    }

}

// taken from hutscape.github.io. Converts r, g, b, to urgb
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return  ((uint32_t)(b) << 0)    |
            ((uint32_t)(r) << 8)    |
            ((uint32_t)(g) << 16);
}

// sets up the ws2812 controller
void setup_ws2812();

