/*
 * computation.c
 *
 *  Created on: Nov 26, 2022
 *      Author: jakub
 */
#include "computation.h"

int last10_rolls[10];
int last10_pitchs[10];

/*ROLL angle - angle around X axis*/
int compute_roll(float acc[3])
{
	double theta = atan( (double)(acc[1]/(sqrt(pow(acc[0],2) + pow(acc[2],2)))) );
	return rad_to_deg(theta);
}

/*PITCH angle - angle around Y axis*/
int compute_pitch(float acc[3])
{
	double theta = atan( (double)(acc[0]/(sqrt(pow(acc[1],2) + pow(acc[2],2)))) );
	return rad_to_deg(theta);
}

/*function to convert RAD to DEG*/
int rad_to_deg(double rad_angle)
{
	return (int)(rad_angle*180.0/M_PI);
}

/*function called from lsm6dsl.c where is initial fulfillment of arrays when initializing*/
void set_last10_rolls_pitchs(int last10_rolls_temp[10],int last10_pitchs_temp[10])
{

	for (int i=0;i<10; i++)
	{
		last10_rolls[i] = last10_rolls_temp[i];
		last10_pitchs[i] = last10_pitchs_temp[i];
	}
}

/*function to compute filtered ROLL angle*/
int compute_filtered_roll(float acc[3])
{
	/*adding new roll item to the 10th position*/
	for(int i=0;i<9;i++){
		last10_rolls[i] = last10_rolls[i+1];
	}
	last10_rolls[9] = compute_roll(acc);

	/*weighted sum - last element (added 2lines above) has weight 10,
	 * where 10th element from the past has weight 1 - this forms filter with total weight of 55*/
	int sum_roll = 0;
	for (int i=1;i<11;i++)
	{
		sum_roll += last10_rolls[i] * i;
	}

	return sum_roll/55;
}

/*function to compute filtered ROLL angle*/
int compute_filtered_pitch(float acc[3])
{
	/*adding new roll item to the 10th position*/
	for(int i=0;i<9;i++){
		last10_pitchs[i] = last10_pitchs[i+1];
	}
	last10_pitchs[9] = compute_pitch(acc);

	/*weighted sum - last element (added 2lines above) has weight 10,
	 * where 10th element from the past has weight 1 - this forms filter with total weight of 55*/
	int sum_pitch = 0;
	for (int i=1;i<11;i++)
	{
		sum_pitch += last10_pitchs[i] * i;
	}

	return sum_pitch/55;
}


