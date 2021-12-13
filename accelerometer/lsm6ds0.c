/*
 * lsm6ds0.c
 *
 *  Created on: 17. 11. 2019
 *      Author: Stancoj
 */

#include "lsm6ds0.h"

uint8_t addres = LSM6DS0_DEVICE_ADDRESS_0;

uint8_t lsm6ds0_read_byte(uint8_t reg_addr)
{
	uint8_t data = 0;
	return *(i2c_master_read(&data, 1, reg_addr, addres, 0));
}


void lsm6ds0_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, addres, 0);
}


void lsm6ds0_readArray(uint8_t * data, uint8_t reg, uint8_t length)
{
	i2c_master_read(data, length, reg, addres, 0);
}


int8_t lsm6ds0_get_temp()
{
	uint8_t temp[2];
	lsm6ds0_readArray(temp, LSM6DS0_ADDRESS_TEMP_L, 2);

	return (((int16_t)((temp[1] << 8) | temp[0])) >> 3)  + 25;
}


void lsm6ds0_get_acc(float* x, float* y, float* z)
{
	uint8_t data[6];
	int16_t xx, yy, zz;

	uint8_t temp;

	//get current scale and use it for final calculation
    temp = lsm6ds0_read_byte(LSM6DS0_ADDRESS_CTRL1);

	temp = temp >> 2;
    temp &= 0x03;			//full scale bits extracted

	lsm6ds0_readArray(data, LSM6DS0_ADDRESS_ACCX, 6);

	xx = ((uint16_t)data[1]) << 8 | data[0];
	yy = ((uint16_t)data[3]) << 8 | data[2];
	zz = ((uint16_t)data[5]) << 8 | data[4];

	*x = (xx >> 4) / 1000.0f;
	*y = (yy >> 4) / 1000.0f;
	*z = (zz >> 4) / 1000.0f;
}


uint8_t lsm6ds0_init(void)
{

	uint8_t status = 1;

	//LIS3MDL_ACC_ON;

	LL_mDelay(100);

	uint8_t val = lsm6ds0_read_byte(LSM6DS0_WHO_AM_I_ADDRES);

	if(val == LSM6DS0_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		addres = LSM6DS0_DEVICE_ADDRESS_1;
		val = lsm6ds0_read_byte(LSM6DS0_WHO_AM_I_ADDRES);
		if(val == LSM6DS0_WHO_AM_I_VALUE)
		{
			status = 1;
		}
		else
		{
			status = 0;
			//return status;
		}
	}

	//acc device init
	uint8_t ctrl1 = lsm6ds0_read_byte(LSM6DS0_ADDRESS_CTRL1);
	ctrl1 &= ~0xFC;
	ctrl1 |= 0x70;
	lsm6ds0_write_byte(LSM6DS0_ADDRESS_CTRL1, ctrl1);

	return status;
}
