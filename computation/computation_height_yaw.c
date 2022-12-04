/*
 * computation_height_yaw.c
 *
 *  Created on: Nov 29, 2022
 *      Author: Matúš
 */

#include "computation.h"
#include "stm32f3xx_it.h"

/* Constant variables */
const float peak_vertical_limits[2] = {2.5,-0.5};
const float peak_yaw_limits[2] = {2,-2};

/* Global variables */
uint32_t current_time, last_vertical_peak_time, last_yaw_peak_time = 0;
int8_t current_vertical_speed, current_yaw_speed = 0;


/* Function for finding vertical acceleration peak */
void find_vertical_peak(float acc[3])
{
	current_time = getMillis();
	if(acc[2] > peak_vertical_limits[0]){
		if(current_time - last_vertical_peak_time > 500){ // Prevent reading of unwanted peaks
			last_vertical_peak_time = current_time;
			current_vertical_speed = 1;
		}
	}
	else if(acc[2] < peak_vertical_limits[1]){
		if(current_time - last_vertical_peak_time > 500){
			last_vertical_peak_time = current_time;
			current_vertical_speed = -1;
		}
	}
}

int8_t compute_vertical_speed(float acc[3])
{
	find_vertical_peak(acc);
	if(current_time - last_vertical_peak_time > 2000){ // Hold output for certain time [ms]
		current_vertical_speed = 0;
	}

	return current_vertical_speed;
}


/* Function for finding yaw acceleration peak */
void find_yaw_peak(float gyro[3])
{
	current_time = getMillis();
	if(gyro[2] > peak_yaw_limits[0]){
		if(current_time - last_yaw_peak_time > 500){ // Prevent reading of unwanted peaks
			last_yaw_peak_time = current_time;
			current_yaw_speed = 1;
		}
	}
	else if(gyro[2] < peak_yaw_limits[1]){
		if(current_time - last_yaw_peak_time > 500){
			last_yaw_peak_time = current_time;
			current_yaw_speed = -1;
		}
	}
}

int8_t compute_yaw_speed(float gyro[3])
{
	find_yaw_peak(gyro);
	if(current_time - last_yaw_peak_time > 1000){ // Hold output for certain time [ms]
		current_yaw_speed = 0;
	}

	return current_yaw_speed;
}
