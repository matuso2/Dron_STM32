/*
 * computation.h
 *
 *  Created on: Nov 26, 2022
 *      Author: jakub
 */

#ifndef COMPUTATION_H_
#define COMPUTATION_H_

#include "math.h"

int computation_pitch(float acc[3]);
int computation_roll(float acc[3]);
int rad_to_deg(double rad_angle);

#endif /* COMPUTATION_H_ */
