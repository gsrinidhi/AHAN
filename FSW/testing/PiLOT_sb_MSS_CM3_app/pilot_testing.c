/*
 * pilot_testing.c
 *
 *  Created on: 11-Mar-2023
 *      Author: S-SPACE
 */

#include "pilot_testing.h"

uint8_t ADC_Init(i2c_instance_t *i2c_chx,uint8_t address){
	i2c_status_t status;
	uint8_t channel = 0;
	uint8_t return_value = 0;
	//Write max and min values to DATA HIGH and DATA LOW registers respectively for all channels
	uint8_t DATA_HIGH[] = {DATA_HIGH_REG(0),DATAHIGH_MAX_H,DATAHIGH_MAX_L};
	uint8_t DATA_LOW[] = {DATA_LOW_REG(0),DATA_LOW_MIN_H,DATA_LOW_MIN_L};
	for(;channel <= 3;channel++) {
		DATA_HIGH[0] = DATA_HIGH_REG(channel);
		DATA_LOW[0] = DATA_LOW_REG(channel);
		I2C_write(&g_core_i2c4,address,DATA_HIGH,3,I2C_RELEASE_BUS);
		status = I2C_wait_complete(&g_core_i2c4, I2C_NO_TIMEOUT);
		I2C_write(i2c_chx,address,DATA_LOW,3,I2C_RELEASE_BUS);
		status = I2C_wait_complete(i2c_chx, I2C_NO_TIMEOUT);
		return_value |= (status << channel);
	}

	return return_value;
}
uint16_t get_ADC_value(i2c_instance_t *i2c_chx,uint8_t address,uint8_t chx,uint8_t *flag) {
	uint8_t adc_read_value[2];
	uint8_t ch_read[] = {chx};
	ch_read[0] |= 0x8;
	ch_read[0] = ch_read[0] << 4;
	uint8_t status;
	uint16_t voltage;
	I2C_write_read(i2c_chx,address,ch_read,1,adc_read_value,2,I2C_RELEASE_BUS);
	status = I2C_wait_complete(i2c_chx, I2C_NO_TIMEOUT);
	if(status != 0) {
		*flag = 1;
	} else {
		voltage = (adc_read_value[0] << 8 ) | adc_read_value[1];
		voltage &= 0x0FFF;
		*flag = 0;
	}
	return voltage;
}
uint8_t get_IMU_acc(uint16_t *a_x,uint16_t *a_y,uint16_t *a_z) {
	uint8_t write_CTRL_REG6_XL[2] = {0x20,0x60};
	uint8_t read_ACC_out_X_L[] = {0x28};
	uint8_t read_ACC_out_Y_L[] = {0x2A};
	uint8_t read_ACC_out_Z_L[] = {0x2C};
	uint8_t read_ACC_out_X_H[] = {0x29};
	uint8_t read_ACC_out_Y_H[] = {0x2B};
	uint8_t read_ACC_out_Z_H[] = {0x2D};
	uint8_t rx_buffer[1],rx_buffer_2[1];
	uint8_t result = 0,status;


	I2C_write_read(&g_core_i2c5,IMU_ADDR,write_CTRL_REG6_XL,2,rx_buffer,
					1,I2C_RELEASE_BUS);
	status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
	result+=status;

	I2C_write_read(&g_core_i2c5,IMU_ADDR,read_ACC_out_Z_L,1,rx_buffer,
							1,I2C_RELEASE_BUS);

	status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
	result+=status;

	I2C_write_read(&g_core_i2c5,IMU_ADDR,read_ACC_out_Z_H,1,rx_buffer_2,
							1,I2C_RELEASE_BUS);

	status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
	result+=status;

	*a_z = ((rx_buffer_2[0] << 8) | rx_buffer[0]);
	if((*a_z) > 32768) {
		*a_z = 65535-*a_z;
	}

	I2C_write_read(&g_core_i2c5,IMU_ADDR,read_ACC_out_Y_L,1,rx_buffer,
							1,I2C_RELEASE_BUS);

	status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
	result+=status;

	I2C_write_read(&g_core_i2c5,IMU_ADDR,read_ACC_out_Y_H,1,rx_buffer_2,
							1,I2C_RELEASE_BUS);

	status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
	result+=status;

	*a_y = ((rx_buffer_2[0] << 8) | rx_buffer[0]);
	if((*a_y) > 32768) {
		*a_y = 65535-*a_y;
	}

	I2C_write_read(&g_core_i2c5,IMU_ADDR,read_ACC_out_X_L,1,rx_buffer,
							1,I2C_RELEASE_BUS);

	status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
	result+=status;

	I2C_write_read(&g_core_i2c5,IMU_ADDR,read_ACC_out_X_H,1,rx_buffer_2,
							1,I2C_RELEASE_BUS);

	status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
	result+=status;

	*a_x = ((rx_buffer_2[0] << 8) | rx_buffer[0]);
	if((*a_x) > 32768) {
		*a_x = 65535-*a_x;
	}

	return status;

}

uint8_t get_IMU_gyro(uint16_t *roll_rate, uint16_t *pitch_rate,uint16_t *yaw_rate) {

	uint8_t write_CTRL_REG1_G[2] = {0x10,0x6A};
		uint8_t read_ACC_out_X_L[] = {0x18};
		uint8_t read_ACC_out_Y_L[] = {0x1A};
		uint8_t read_ACC_out_Z_L[] = {0x1C};
		uint8_t read_ACC_out_X_H[] = {0x19};
		uint8_t read_ACC_out_Y_H[] = {0x1B};
		uint8_t read_ACC_out_Z_H[] = {0x1D};
		uint8_t IMU_slave_addr = 0x6a;
		uint8_t rx_buffer[1],rx_buffer_2[1];
		uint8_t result = 0,status;


		I2C_write_read(&g_core_i2c5,IMU_slave_addr,write_CTRL_REG1_G,2,rx_buffer,
						1,I2C_RELEASE_BUS);
		status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
		result+=status;

		I2C_write_read(&g_core_i2c5,IMU_slave_addr,read_ACC_out_Z_L,1,rx_buffer,
								1,I2C_RELEASE_BUS);

		status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
		result+=status;

		I2C_write_read(&g_core_i2c5,IMU_slave_addr,read_ACC_out_Z_H,1,rx_buffer_2,
								1,I2C_RELEASE_BUS);

		status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
		result+=status;

		*roll_rate = ((rx_buffer_2[0] << 8) | rx_buffer[0]);

		I2C_write_read(&g_core_i2c5,IMU_slave_addr,read_ACC_out_Y_L,1,rx_buffer,
								1,I2C_RELEASE_BUS);

		status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
		result+=status;

		I2C_write_read(&g_core_i2c5,IMU_slave_addr,read_ACC_out_Y_H,1,rx_buffer_2,
								1,I2C_RELEASE_BUS);

		status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
		result+=status;

		*pitch_rate = ((rx_buffer_2[0] << 8) | rx_buffer[0]);

		I2C_write_read(&g_core_i2c5,IMU_slave_addr,read_ACC_out_X_L,1,rx_buffer,
								1,I2C_RELEASE_BUS);

		status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
		result+=status;

		I2C_write_read(&g_core_i2c5,IMU_slave_addr,read_ACC_out_X_H,1,rx_buffer_2,
								1,I2C_RELEASE_BUS);

		status = I2C_wait_complete(&g_core_i2c5,I2C_NO_TIMEOUT);
		result+=status;

		*yaw_rate = ((rx_buffer_2[0] << 8) | rx_buffer[0]);

		return status;
}
