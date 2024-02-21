#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "transform.h"
#include "ADXL343.h"

#define ACCEL_SDA_PIN   16
#define ACCEL_SCL_PIN   17

void initialize_transform(transform *trans)
{
    printf("inttin\n");
    adxl343_setup(&trans->accelerometer, i2c0, ACCEL_SDA_PIN, ACCEL_SCL_PIN, ADXL343_DEFAULT_ADDRESS);
    printf("adx initted\n");
}


void update_transform(transform *trans)
{
    int16_t ax_raw, ay_raw, az_raw; // current adxl343 readings
    double jx, jy, jz;  // jerk for x, y, z
    double delta_time_s;
    uint64_t now_us = time_us_64();

    if (trans->prev_calc_time_us == 0) {
        // in order to ensure the first transform update isn't wildly innacurate, make
        // the first update of the transform effectively do nothing
        delta_time_s = 0.0;  
    }
    else {
        delta_time_s = (double)(now_us - trans->prev_calc_time_us) / 1000000.0;
    }

    // get the x, y, and z accelerometer values
    adxl343_getx(&trans->accelerometer, &ax_raw);
    adxl343_gety(&trans->accelerometer, &ay_raw);
    adxl343_getz(&trans->accelerometer, &az_raw);

    // convert them to m/s^3
    jx = (double)(ax_raw - trans->prev_ax_raw) * ADXL3XXVAL_TO_MSS * delta_time_s;
    jy = (double)(ay_raw - trans->prev_ay_raw) * ADXL3XXVAL_TO_MSS * delta_time_s;
    jz = (double)(az_raw - trans->prev_az_raw) * ADXL3XXVAL_TO_MSS * delta_time_s;

}


void reset_transform(transform *trans)
{
    trans->x = 0;
    trans->y = 0;
    trans->z = 0;

    trans->vx = 0;
    trans->vy = 0;
    trans->vz = 0;

    trans->prev_adxl343_x_reading = 0;
    trans->prev_adxl343_y_reading = 0;
    trans->prev_adxl343_z_reading = 0;

    trans->prev_calc_time_us = 0;
}