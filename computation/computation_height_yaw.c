/*
 * computation_height_yaw.c
 *
 *  Created on: Nov 29, 2022
 *      Author: Matúš
 */

#include "computation.h"

float peak_limits[2] = {2, -1};
uint32_t currentTime = 0;
uint32_t lastTime = 0;

uint8_t find_Z_peak(float acc[3])
{
	if(acc[2] > peak_limits[0]){

		return 1;
	}
	else if(acc[2] < peak_limits[1]){

		return 2;
	}
	return 0;
}

uint8_t compute_vertical_speed(float acc[3])
{
	int vertical_speed = find_Z_peak(acc);
	return vertical_speed;
}
