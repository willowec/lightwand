#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "ADXL343.h"


// i2c buffer
uint8_t buffer[2] = {0};

void adxl343_setup(adxl343 *accelerometer, i2c_inst_t *i2c, uint8_t SDA_pin, uint8_t SCL_pin, uint8_t address)
{
    // initialize i2c
    i2c_init(i2c, 100 * 1000);  // select standard communication rate of 100kHz
    gpio_set_function(SDA_pin, GPIO_FUNC_I2C);
    gpio_set_function(SCL_pin, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_pin);
    gpio_pull_up(SCL_pin);

    accelerometer->address = address;
    accelerometer->i2c = i2c;   

    // get the device id
    uint8_t result;
    printf("getting devid\n");
    adxl343_read_register_8(accelerometer, ADXL3XX_REG_DEVID, &result);
    printf("DEvice id: %#x\n", result);
}


void adxl343_write_register(adxl343 *accelerometer, uint8_t reg, uint8_t value)
{
    buffer[0] = reg;
    buffer[1] = value;
    i2c_write_blocking(accelerometer->i2c, accelerometer->address, buffer, 2, false);
}


void adxl343_read_register_8(adxl343 *accelerometer, uint8_t reg, uint8_t *out_val)
{
    buffer[0] = reg;
    i2c_write_blocking(accelerometer->i2c, accelerometer->address, buffer, 1, true);
    i2c_read_blocking(accelerometer->i2c, accelerometer->address, out_val, 1, false);
}


void adxl343_read_register_16(adxl343 *accelerometer, uint8_t reg, int16_t *out_val)
{

}


int16_t adxl343_getx(adxl343 *accelerometer)
{

}


int16_t adxl343_gety(adxl343 *accelerometer)
{

}


int16_t adxl343_getz(adxl343 *accelerometer)
{

}