#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "ADXL343.h"


// i2c buffer
uint8_t buffer[2] = {0};

int adxl343_setup(adxl343 *accelerometer, i2c_inst_t *i2c, uint8_t SDA_pin, uint8_t SCL_pin, uint8_t address)
{
    int err;

    // initialize i2c
    i2c_init(i2c, 100 * 1000);  // select standard communication rate of 100kHz
    gpio_set_function(SDA_pin, GPIO_FUNC_I2C);
    gpio_set_function(SCL_pin, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_pin);
    gpio_pull_up(SCL_pin);

    accelerometer->address = address;
    accelerometer->i2c = i2c;   

    // Confirm the device ID is correct
    uint8_t devid;
    err = adxl343_read_register_8(accelerometer, ADXL3XX_REG_DEVID, &devid);
    if (err < 0) 
        return err;
    if (devid != 0xe5) 
        return PICO_ERROR_GENERIC;

    // select 16G range, 10-bit measurements: 0b0000 0011 (0x03)
    err = adxl343_write_register(accelerometer, ADXL3XX_REG_DATA_FORMAT, ADXL3XX_RANGE_16G);
    if (err < 0) 
        return err;

    // enable measurements
    err = adxl343_write_register(accelerometer, ADXL3XX_REG_POWER_CTL, 0x08);
    if (err < 0) 
        return err;

    return PICO_ERROR_NONE;
}


int adxl343_write_register(adxl343 *accelerometer, uint8_t reg, uint8_t value)
{
    int err;

    buffer[0] = reg;
    buffer[1] = value;
    err = i2c_write_timeout_us(accelerometer->i2c, accelerometer->address, buffer, 2, false, ADXL343_I2C_TIMEOUT_US);
    
    return err;
}


int adxl343_read_register_8(adxl343 *accelerometer, uint8_t reg, uint8_t *out_val)
{
    int err;

    buffer[0] = reg;

    // request the register
    err = i2c_write_timeout_us(accelerometer->i2c, accelerometer->address, buffer, 1, true, ADXL343_I2C_TIMEOUT_US);
    if (err < 0) 
        return err;

    // read from that register
    err = i2c_read_timeout_us(accelerometer->i2c, accelerometer->address, out_val, 1, false, ADXL343_I2C_TIMEOUT_US);
    return err;
}


int adxl343_read_register_16(adxl343 *accelerometer, uint8_t reg, int16_t *out_val)
{
    int err;

    buffer[0] = reg;

    // request the register
    err = i2c_write_timeout_us(accelerometer->i2c, accelerometer->address, buffer, 1, true, ADXL343_I2C_TIMEOUT_US);
    if (err < 0) 
        return err;

    // read from that register, and the one immediately after it
    err = i2c_read_timeout_us(accelerometer->i2c, accelerometer->address, buffer, 2, false, ADXL343_I2C_TIMEOUT_US);

    // squash buffer[0] and buffer[1] into out_val, using the first register as LSByte and second register as MSByte
    *out_val = (buffer[0]) | (buffer[1] << 8);
    return err;
}


int adxl343_getx(adxl343 *accelerometer, int16_t *out_val)
{
    return adxl343_read_register_16(accelerometer, ADXL3XX_REG_DATAX0, out_val);
}


int adxl343_gety(adxl343 *accelerometer, int16_t *out_val)
{
    return adxl343_read_register_16(accelerometer, ADXL3XX_REG_DATAY0, out_val);
}


int adxl343_getz(adxl343 *accelerometer, int16_t *out_val)
{
    return adxl343_read_register_16(accelerometer, ADXL3XX_REG_DATAZ0, out_val);
}