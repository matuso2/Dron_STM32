/*
 * lsm6dsl.h
 *
 *  Created on: 17. 11. 2019
 *      Author: Stancoj
 */

#include "main.h"
#include "i2c.h"


#define 	LSM6DSL_DEVICE_ADDRESS_0				0xD5 	//adresa read 1 SLAVE
#define 	LSM6DSL_DEVICE_ADDRESS_1				0xD7 	//adresa read 2 SLAVE

#define 	LSM6DSL_WHO_AM_I_VALUE					0x6A
#define 	LSM6DSL_WHO_AM_I_ADDRESS				0x0F

// acc
#define 	LSM6DSL_ADDRESS_CTRL1					0x10	// kontrolny register, acc
#define 	LSM6DSL_ADDRESS_ACCX					0x28	//adresa acc X
#define 	LSM6DSL_ADDRESS_ACCY					0x2A	//adresa acc Y
#define 	LSM6DSL_ADDRESS_ACCZ					0x2C	//adresa acc Z

// gyro
#define 	LSM6DSL_ADDRESS_CTRL2					0x11	// kontrolny register, gyro
#define 	LSM6DSL_ADDRESS_GYROX					0x22	// adresa gyroL X
#define 	LSM6DSL_ADDRESS_GYROY					0x24	// adresa gyroL Y
#define 	LSM6DSL_ADDRESS_GYROZ					0x26	// adresa gyroL Z

uint8_t lsm6dsl_init(void);
void lsm6dsl_get_acc(float* x, float * y, float * z);
void lsm6dsl_get_gyro(float* roll, float* pitch, float* yaw);
uint8_t lsm6dsl_read_byte(uint8_t reg_addr);
void lsm6dsl_write_byte(uint8_t reg_addr, uint8_t value);
