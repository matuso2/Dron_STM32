/*
 * computation_height_yaw.c
 *
 *  Created on: Nov 29, 2022
 *      Author: Matúš
 */

#include "computation.h"

float peak_limits[2] = {2.5,-0.5};
uint32_t current_time, last_peak_time = 0;
int8_t current_vertical_speed = 0;

void find_Z_peak(float acc[3])
{
	current_time = getMillis();
	if(acc[2] > peak_limits[0]){
		if(current_time - last_peak_time > 500){ // Zabranenie citaniu nechcenej spicky
			last_peak_time = current_time;
			current_vertical_speed = 1;
		}
	}
	else if(acc[2] < peak_limits[1]){
		if(current_time - last_peak_time > 500){
			last_peak_time = current_time;
			current_vertical_speed = -1;
		}
	}
}

int8_t compute_vertical_speed(float acc[3])
{
	find_Z_peak(acc);
	if(current_time - last_peak_time > 2000){ // Drzanie vystupu na x [ms]
		current_vertical_speed = 0;
	}

	return current_vertical_speed;
}
