/*
 * computation.h
 *
 *  Created on: Nov 26, 2022
 *      Author: jakub
 */

#ifndef COMPUTATION_H_
#define COMPUTATION_H_

#include "math.h"

extern int last10_rolls[10];

int compute_pitch(float acc[3]);
int compute_roll(float acc[3]);
int compute_filtered_pitch(float acc[3]);
int compute_filtered_roll(float acc[3]);

int rad_to_deg(double rad_angle);
void set_last10_rolls_pitchs(int last10_rolls_temp[10],int last10_pitchs_temp[10]);

#endif /* COMPUTATION_H_ */
