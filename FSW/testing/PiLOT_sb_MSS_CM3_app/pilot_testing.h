/*
 * pilot_testing.h
 *
 *  Created on: 11-Mar-2023
 *      Author: S-SPACE
 */

#ifndef PILOT_TESTING_H_
#define PILOT_TESTING_H_

#include "peripherals.h"
#define ADC_I2C_ADDR 0x21
#define ADC_I2CU1_ADDR 0x21
#define ADC_I2CU2_ADDR 0x22
#define ADC_I2CU3_ADDR 0x21
#define ADC_I2CU4_ADDR 0x21

#define DATAHIGH_MAX_H 0x0F
#define DATAHIGH_MAX_L 0xFC
#define DATA_LOW_MIN_H 0x00
#define DATA_LOW_MIN_L 0x00

#define CONV_REG 0x00
#define AS_REG 0x01
#define CONFIG_REG 0x02
#define CYCLE_TIMER_REG 0x03
#define DATA_LOW_REG(x) ((x) * 3 + 0x4)
#define DATA_HIGH_REG(x) ((x) * 3 + 0x5)
#define HYST_REG(x) ((x) * 3 + 0x6)
#define ADC_CHX(x) (((x) + 0x8)<<4)

//IMU Macros
#define IMU_ADDR 0x6a
#define IMU_WHO_AM_I_REG 0x0F
uint8_t ADC_Init(i2c_instance_t *i2c_chx,uint8_t address);
uint16_t get_ADC_value(i2c_instance_t *i2c_chx,uint8_t address,uint8_t chx,uint8_t *flag);
uint8_t get_IMU_acc(uint16_t *a_x,uint16_t *a_y,uint16_t *a_z);
uint8_t get_IMU_gyro(uint16_t *roll_rate, uint16_t *pitch_rate,uint16_t *yaw_rate);
#endif /* PILOT_TESTING_H_ */
