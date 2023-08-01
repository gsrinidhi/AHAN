#ifndef _TEST_H_
#define _TEST_H_

#include<stdio.h>
#include<drivers/CoreI2C/core_i2c.h>
#include "peripherals.h"
#include "SD.h"
#define MAX_BUFFER_SIZE	256

uint8_t g0_slave_rx_buffer[MAX_BUFFER_SIZE];
uint8_t g0_slave_tx_buffer[MAX_BUFFER_SIZE];

uint8_t g1_slave_rx_buffer[MAX_BUFFER_SIZE];
uint8_t g1_slave_tx_buffer[MAX_BUFFER_SIZE];

uint8_t g2_slave_rx_buffer[MAX_BUFFER_SIZE];
uint8_t g2_slave_tx_buffer[MAX_BUFFER_SIZE];

uint8_t g3_slave_rx_buffer[MAX_BUFFER_SIZE];
uint8_t g3_slave_tx_buffer[MAX_BUFFER_SIZE];

void write_handler(i2c_instance_t* this_i2c, uint8_t* rx_buffer, uint8_t n);

#endif
