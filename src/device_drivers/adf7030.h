#ifndef __ADF7030_H__
#define __ADF7030_H__

#include <stdint.h>

//Code to prevent error squiggles when not using smartfusion2 libraries. Set to zero when running on smartfusion2
#define NO_CROSS_COMPILE 1
#if NO_CROSS_COMPILE == 1
int a
#define g_core_spi0 a
#endif

//The below three macros are present to make transitioning from Core to MSS or vice-versa easier
// Set the spi instance used to communicate with the adf7030
#define ADF_SPI_INSTANCE g_core_spi0

//Set the transfer frame function
#define ADF_SPI_TRANSFER_FRAME SPI_transfer_frame

//Set the transer block function
#define ADF_SPI_TRANSFER_BLOCK SPI_tranfer_block

//Set the slave select function
#define ADF_SPI_SLAVE_SELECT SPI_set_

//Not using DMA right now. Need to check if DMA can be used
#define ADF_USE_PDMA 0

extern uint8_t radio_memory_configuration[];


uint8_t config_adf7030(uint32_t size)
#endif