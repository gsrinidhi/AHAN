///**
// * @file 	:	pilot.h
// * @author 	:	Srinidhi G, Pratik Aher
// * @brief 	:	This file contains the functions used to gather data in PiLOT
// * @version :	1
// * @date 	:	2023-01-24
// *
// * @copyright Copyright (c) 2023
// *
// */
//
//#ifndef _PILOT_H_
//#define _PILOT_H_
//#include"peripherals.h"
//#include "SD.h"
//#include "packet_definitions.h"
//#include "pslv_interface.h"
//#include "ccsds.h"
//
////ADC Macros
//#define ADC_I2C_ADDR 0x21
//#define ADC_I2CU1_ADDR 0x21
//#define ADC_I2CU2_ADDR 0x21
//#define ADC_I2CU3_ADDR 0x21
//#define ADC_I2CU4_ADDR 0x21
//
//#define DATAHIGH_MAX_H 0x0F
//#define DATAHIGH_MAX_L 0xFC
//#define DATA_LOW_MIN_H 0x00
//#define DATA_LOW_MIN_L 0x00
//
//#define CONV_REG 0x00
//#define AS_REG 0x01
//#define CONFIG_REG 0x02
//#define CYCLE_TIMER_REG 0x03
//#define DATA_LOW_REG(x) ((x) * 3 + 0x4)
//#define DATA_HIGH_REG(x) ((x) * 3 + 0x5)
//#define HYST_REG(x) ((x) * 3 + 0x6)
//#define ADC_CHX(x) (((x) + 0x8)<<4)
//
////IMU Macros
//#define IMU_ADDR 0x6a
//#define IMU_WHO_AM_I_REG 0x0F
//
////PERIOD Macros
//#define DEFAULT_PAYLOAD_PERIOD	10000
//#define DEFAULT_HK_PERIOD		2000
//#define ONE_SPP_RATE			1000
//#define TWO_SPP_RATE			2000
//#define FIVE_SPP_RATE			5000
//#define TEN_SPP_RATE			10000
//#define PAYLOAD_PERIOD_L		0xFFFF
//#define PAYLOAD_PERIOD_H		0xFFFF
//#define HK_PERIOD_H				0xFFFF
//#define HK_PERIOD_L				0xFFFF
//
////TASK IDs
//#define THERMISTOR_TASK_ID	1
//#define HK_TASK_ID			2
//#define SD_HK_TASK_ID		3
//#define ARIS_TASK_ID		4
//
////PSLV address
//#define PSLV_TO_PILOT_ADDR			119
//
//#define PILOT_REVERSE_BYTE_ORDER(var)	(((var) << 8) | ((var) >> 8))
//
////Packet rate options
//typedef enum PACKET_RATES {
//	RATE_ONE_SPP = 0u,
//	RATE_TWO_SPP,
//	RATE_FIVE_SPP,
//	RATE_TEN_SPP
//}PACKET_RATES_t;
//
///** Function to initialise ADC
// * @brief Initialises the ADC corresponding to the given address
// *
// * @param i2c_chx: The i2c channel through which the ADC is connected
// * @param address: The address of the ADC
// *
// */
//uint8_t ADC_Init(i2c_instance_t *i2c_chx,uint8_t address);
//
///**
// * @brief Get the ADC value
// *
// * @param i2c_chx   : the i2c channel used
// * @param address   : the address of the ADC
// * @param chx       : the channel to convert
// * @return double   : returns the value given by the ADC
// */
//uint16_t get_ADC_value(i2c_instance_t *i2c_chx,uint8_t address,uint8_t chx,uint8_t *flag);
//
///**
// * @brief Initialises the I2Cs in Pilot
// *
// */
//void I2C_Init();
//
///**
// * @brief Initialises the GPIOs in Pilot
// *
// */
//void GPIO_Init();
//
///**
// * @brief Initialises the Uarts in Pilot
// *
// */
//void Uart_Init();
//
///**
// * @brief Initialises the SPI interface in Pilot
// *
// */
//void SPI_Init();
//
///**
// * @brief Calls the GPIO, UART, I2C and SPI initialisation functions. Initialises the SD card and starts the 64 bit timer
// *
// * @return uint8_t: returns 0 on successful initialisation, 1 otherwise
// */
//uint8_t Pilot_Peripherals_Init();
//
///**
// * @brief Initialises the sensor board ADCs and pslv interface board vc sensors
// *
// */
//uint8_t Pilot_Init();
//
///**
// * @brief Get the thermistor packet
// *
// * @param pkt       : thermistor packet pointer
// * @param seq_no    : sequence number of packet
// * @return uint8_t  : Returns 0 if successful, 1 otherwise
// */
//uint8_t get_thermistor_vals(thermistor_pkt_t *pkt,uint16_t seq_no);
//
///**
// * @brief Function to obtain CDH peripheral health
// *
// * @return uint8_t
// * Bit 1(LSB)   : i2c_3
// * Bit 2        : i2c_5
// * Bit 3        : IMU
// * Bit 4        : VC Sensor I2C
// * Bit 5        : SD working
// * Bit 6		: SD card can be written to
// * Bit 7		: SD card can be read from
// * Bit 8		: The SD card is off
// */
//uint8_t test_peripherals(uint8_t *sd);
//
///**
// * @brief Get the three axis acceleration values from the IMU
// *
// * @param a_x 			: pointer to store the value of x axis acceleration
// * @param a_y 			: pointer to store the value of y axis acceleration
// * @param a_z 			: pointer to store the value of z axis acceleration
// * @return uint8_t 		: returns zero if successful, non zero otherwise
// */
//uint8_t get_IMU_acc(uint16_t *a_x,uint16_t *a_y,uint16_t *a_z);
//
///**
// * @brief Get the gyroscope values from the IMU
// *
// * @param roll_rate 	: pointer to store the roll rate
// * @param pitch_rate 	: pointer to store the pitch rate
// * @param yaw_rate 		: pointer to store the yaw rate
// * @return uint8_t 		: returns zero if successful, non zero otherwise
// */
//uint8_t get_IMU_gyro(uint16_t *roll_rate, uint16_t *pitch_rate,uint16_t *yaw_rate);
//
///**
// * @brief This function converts time in milli seconds to a 64 bit count value
// *
// * @param ms 			: The time required in milliseconds
// * @param upper_count 	: pointer to store the upper 32 bits of counter
// * @param lower_count 	: pointer to store the lower 32 bits of counter
// */
//void time_to_count(uint32_t ms,uint32_t *upper_count,uint32_t *lower_count);
//
///**
// * @brief Function to get the health of Pilot
// *
// * @param hk_pkt 	: pointer to the hk packet structure where the data is stored
// * @param seq_no 	: the sequence number of the packet
// * @param sd_s 		: the state of the sd card needed to decide if or not to operate the sd card
// * @return uint8_t 	: returns 0 if successful, non-zero otherwise
// */
//uint8_t get_hk(hk_pkt_t *hk_pkt, uint16_t seq_no,uint8_t *sd_s);
//
////uint8_t get_aris_vals(aris_pkt_t *pkt, uint16_t seq_no);
//
///**
// * @brief Get a sample of aris data
// *
// * @param pkt 		: pointer to aris packet structure to store the data
// * @param time 		: the time of collecting the data
// * @param sample_no : the sample number of the sample
// * @return uint8_t 	: returns 0 if successful, non-zero otherwise
// */
//uint8_t get_aris_sample(aris_pkt_t *pkt,uint32_t time,uint8_t sample_no);
//
///**
// * @brief Obtain the status of the SD card
// *
// * @param sd 		: Pointer to the current state of the SD card. On the completion of this function, the state of the SD card is as follows:
// * Bit 1 (LSb)		: Initialised properly
// * Bit 2			: Write function working
// * Bit 3			: Read function working
// * Bit 4			: SD card is off
// * @param data 		: Testing data to be written to the SD card
// * @return uint8_t 	: returns 0
// * 0x7				: SD card can be initialised,
// */
//uint8_t sd_status(uint8_t *sd,uint8_t *data);
//
//uint8_t sd_hk_test(sd_test_t *sd,uint8_t *data,uint32_t addr);
//#endif
