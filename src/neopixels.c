#include "neopixels.h"


#define TX_PIN  0   // TX is hooked up to GPIO0


void setup_ws2812() 
{
    // setup code pulled from https://github.com/hutscape/hutscape.github.io/blob/master/_tutorials/code/pico-neopixel/pico-neopixel.c
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, TX_PIN, 800000, true);
}