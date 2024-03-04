/**
 * @file    :   peripherals.h
 * @author  :   Srinidhi G
 * @brief   :   This file contains all the peripherals used be the OBC
 * @version :   1.0
 * @date    :   2023-01-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PERIPH_H
#define PERIPH_H

#include<CMSIS/hw_reg_io.h>
#include<drivers/CoreUARTapb/core_uart_apb.h>
#include<drivers/CoreI2C/core_i2c.h>
#include<drivers/mss_rtc/mss_rtc.h>
#include<drivers/mss_spi/mss_spi.h>
#include<drivers/mss_timer/mss_timer.h>
#include <PiLOT_hw_platform.h>
#include <drivers/mss_gpio/mss_gpio.h>
#include <drivers/mss_i2c/mss_i2c.h>
#include <drivers/mss_uart/mss_uart.h>
#include <drivers_config/sys_config/sys_config_mss_clocks.h>
#include <drivers/CoreTimer/core_timer.h>
#include <drivers/mss_watchdog/mss_watchdog.h>
//#include"../../AHAN_FSW/AHAN_sb_MSS_CM3_hw_platform/drivers/CoreSPI/core_spi.h"
#include <drivers/CoreSPI/core_spi_new.h>
#define WDOG_SYSREG_CR_ENABLE_MASK      0x00000001u
//UART
//defining PE bit of LSR register for MSS_UART
#define PE							2u
#define TEMT						6u
#define MSS_UART_BAUD_2000000			2000000

//I2C instances

#define DUMMY_I2C_ADDR 0x88
i2c_instance_t g_core_i2c0;
i2c_instance_t g_core_i2c1;
i2c_instance_t g_core_i2c2;
i2c_instance_t g_core_i2c3;
i2c_instance_t g_core_i2c4;
i2c_instance_t g_core_i2c5;

i2c_status_t core_i2c_status;

timer_instance_t sd_timer;

//Core SPI instance
spi_instance_t g_core_spi0;
#define SD_TIMER_BASE_ADDR	0x5000B000

#define i2c_3 g_core_i2c1
#define i2c_5 g_core_i2c3

#define I2C_SLAVE_ADDRESS 0x12

//GPIO MACROS
#define EN_UART					    MSS_GPIO_13
#define EN_SENSOR_BOARD				MSS_GPIO_14
#define SD_CARD_GPIO				MSS_GPIO_10
#define TX_INV_EN					MSS_GPIO_8
#define RX_INV_EN					MSS_GPIO_7
#define ADF_RST						MSS_GPIO_5

#define TIMER_COUNT_PER_MS 100000 //Number of counts in one millisecond 1t 100MHz
#define FULL_SCALE_TIME_MS 42949 //Number of milliseconds for the 32 bit counter to completely countdown at 100MHz

#endif
