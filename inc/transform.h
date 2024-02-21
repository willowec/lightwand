/*
Functions for getting the position of the light wand in space
*/

#ifndef POSITION
#define POSITION

#include <stdlib.h>
#include <stdio.h>

#include "ADXL343.h"

// The transform of the wand
typedef struct transform_struct {
    // position of the ADXL343 compared to its origin in meters
    double x, y, z; 

    // velocity of the ADXL343 in m/s
    double vx, vy, vz;

    // acceleration of the ADXL343 in m/s^2
    double ax, ay, az;

    // previous ADXL343 readings for each axis   
    int16_t prev_ax_raw, prev_ay_raw, prev_az_raw;

    // the last time position was updated (used to calculate delta time)
    uint64_t prev_calc_time_us; 

    // the accelerometer that this transform uses to keep track of its position
    adxl343 accelerometer;
    
} transform;


/*
Sets up the components required for the position module
*/
void initialize_transform(transform *trans);

/*
Resets the transform of the wand 
*/
void reset_transform(transform *trans);

/*
Takes a G-force measurement from the ADXL343 and updates the transform of the wand based on that reading.
*/
void update_transform(transform *trans);

#endif