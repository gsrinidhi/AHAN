/**
 * @file    :   pslv_interface.h
 * @author  :   Srinidhi G
 * @brief   :   This function contains drivers for the VC sensor on the pslv interface board
 * @version :   1.0
 * @date    :   2023-01-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _PSLV_INTERFACE_
#define _PSLV_INTERFACE_

#include "peripherals.h"

#define VC1 0x40
#define VC2 0x41
#define VC3 0x43
#define VC_SENSOR_I2C &g_core_i2c2

// Reg Addresses
#define VC_BUSV_CHx(x)                  ((0x2 * x))
#define VC_SHUNTV_CHx(x)                ((0x2 * x - 1))

#define VC_INIT_CMD                     0x7007
#define VC_CONFIG_REG 					0x00

/**
 * @brief Function to write to the vc sensor 
 * 
 * @param addr      : the i2c address of the vc sensor
 * @param tx        : the buffer containing the data which has to written to the vc sensor
 * @param tx_size   : the number of bytes to be written to the vc sensor
 * @return uint8_t  : returns 0 if successful, non-zero otherwise
 */
uint8_t vc_write(uint8_t addr, uint8_t *tx, uint8_t tx_size);

/**
 * @brief Function to read from the vc sensor
 * 
 * @param addr      : the i2c address of the vc sensor
 * @param rx        : the address to store the data read from the vc sensor
 * @param rx_size   : the number of bytes to read from the vc sensor
 * @return uint8_t  : returns 0 if successful, non-zero otherwise
 */
uint8_t vc_read(uint8_t addr, uint8_t *rx, uint8_t rx_size);

/**
 * @brief Function to read a register from the vc sensor
 * 
 * @param vc_addr       : the address of the vc sensor
 * @param reg_addr      : the address of the register to be read
 * @param rx            : the two byte array to store the data read
 * @return uint8_t      : returns 0 if successful, non-zero otherwise
 */
uint8_t vc_read_reg(uint8_t vc_addr, uint8_t reg_addr,uint8_t *rx);

/**
 * @brief Function to write to a register in the vc sensor
 * 
 * @param vc_addr       : the address of the vc sensor
 * @param reg_addr      : the address of the vc sensor to be written to
 * @param tx            : the buffer containing the data to be written to the register
 * @param tx_size       : the number of bytes to be written to the register
 * @return uint8_t      : returns 0 if successful, non-zero otherwise
 */
uint8_t vc_write_reg(uint8_t vc_addr, uint8_t reg_addr,uint8_t *tx,uint8_t tx_size);

/**
 * @brief Function to initialise a vc sensor
 * 
 * @param addr          : the address of the vc sensor to be initialised
 * @return uint8_t      : returns 0 if successful, non-zero otherwise
 */
uint8_t vc_init(uint8_t addr);

/**
 * @brief Function to read the bus voltage from the vc sensor
 * 
 * @param addr          : the i2c address of the vc sensor
 * @param chx           : the channel of the vc sensor to read the bus voltage from
 * @param flag          : this flag is set to 0 if the read was successful, 1 otherwise
 * @return uint16_t     : returns the value of the bus voltage register for the given channel
 */
uint16_t read_bus_voltage(uint8_t addr, uint8_t chx,uint8_t *flag);

/**
 * @brief Function to read the shunt voltage from the vc sensor
 * 
 * @param addr          : the i2c address of the vc sensor
 * @param chx           : the channel of the vc sensor to read the shunt voltage from
 * @param flag          : this flag is set to 0 if the read was successful, 1 otherwise
 * @return uint16_t     : returns the value of the shunt voltage register for the given channel
 */
uint16_t read_shunt_voltage(uint8_t addr, uint8_t chx,uint8_t *flag);


#endif
