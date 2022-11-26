/*
 * computation.c
 *
 *  Created on: Nov 26, 2022
 *      Author: jakub
 */
#include "computation.h"

/*ROLL angle - angle around X axis*/
int computation_roll(float acc[3])
{
	double theta = atan( (double)(acc[1]/(sqrt(pow(acc[0],2) + pow(acc[2],2)))) );
	return rad_to_deg(theta);
}

/*PITCH angle - angle around Y axis*/
int computation_pitch(float acc[3])
{
	double theta = atan( (double)(acc[0]/(sqrt(pow(acc[1],2) + pow(acc[2],2)))) );
	return rad_to_deg(theta);
}

/*function to convert RAD to DEG*/
int rad_to_deg(double rad_angle)
{
	return (int)(rad_angle*180.0/M_PI);
}

