/*
 * computation_roll_pitch.c
 *
 *  Created on: Nov 26, 2022
 *      Author: jakub
 */
#include "computation.h"

int last10_rolls[10];
int last10_pitchs[10];
int max_tilt_angle = 30; //in deg for roll & pitch

/*function to check if angle is not bigger than allowed*/
int compute_limited_angle(int theta_temp)
{
	if (theta_temp > max_tilt_angle)
	{
		return max_tilt_angle;
	}
	else if (theta_temp < -max_tilt_angle)
	{
		return -max_tilt_angle;
	}
	else{
		return theta_temp;
	}
}

/*ROLL angle - angle around X axis*/
int compute_roll(float acc[3])
{
	double theta = atan( (double)(acc[1]/(sqrt(pow(acc[0],2) + pow(acc[2],2)))) );
	int theta_temp = rad_to_deg(theta);

	return compute_limited_angle(theta_temp);

}

/*PITCH angle - angle around Y axis*/
int compute_pitch(float acc[3])
{
	double theta = atan( (double)(acc[0]/(sqrt(pow(acc[1],2) + pow(acc[2],2)))) );
	int theta_temp = rad_to_deg(theta);

	return compute_limited_angle(theta_temp);
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
		sum_roll += last10_rolls[i-1] * i;
	}

	return (int)(sum_roll/55);
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
		sum_pitch += last10_pitchs[i-1] * i;
	}

	return (int)(sum_pitch/55);
}

/*function to compute speed in general*/
int compute_speed(int angle, int max_speed, int control_type)
{
	/*quadratic control*/
	if( control_type == 2)
	{
		if (angle >=0)
		{
			return (int)((max_speed/(float)(pow(max_tilt_angle,2)))*pow(angle,2));
		}
		else
		{
			return (int)(-1*(max_speed/(float)(pow(max_tilt_angle,2)))*pow(angle,2));
		}
	}
	/*linear control - serves as default*/
	else
	{
		return (int)((max_speed/(float)(max_tilt_angle))*angle);
	}
}

/*function to compute ROLL speed*/
int compute_roll_speed(float acc[3], int max_roll_speed, int control_type)
{
	int roll_temp = compute_filtered_roll(acc);
	return compute_speed(roll_temp, max_roll_speed, control_type);

}

/*function to compute PITCH speed*/
int compute_pitch_speed(float acc[3], int max_pitch_speed, int control_type)
{
	int pitch_temp = compute_filtered_pitch(acc);
	return compute_speed(pitch_temp, max_pitch_speed, control_type);
}


