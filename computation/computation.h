/*
 * computation.h
 *
 *  Created on: Nov 26, 2022
 *      Author: jakub
 */

#ifndef COMPUTATION_H_
#define COMPUTATION_H_

#include "math.h"
#include "stdio.h"

int compute_pitch(float acc[3]);
int compute_roll(float acc[3]);
int compute_filtered_pitch(float acc[3]);
int compute_filtered_roll(float acc[3]);
int compute_roll_speed(float acc[3], int max_roll_speed, int control_type);
int compute_pitch_speed(float acc[3], int max_pitch_speed, int control_type);

int rad_to_deg(double rad_angle);
void set_last10_rolls_pitchs(int last10_rolls_temp[10],int last10_pitchs_temp[10]);

void find_vertical_peak(float acc[3]);
int8_t compute_vertical_speed(float acc[3]);
void find_yaw_peak(float gyro[3]);
int8_t compute_yaw_speed(float acc[3]);

#endif /* COMPUTATION_H_ */
