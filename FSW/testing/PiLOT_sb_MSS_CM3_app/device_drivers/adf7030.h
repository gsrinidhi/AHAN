#ifndef __ADF7030_H__
#define __ADF7030_H__

#include <stdint.h>
#include <drivers/CoreSPI/core_spi.h>
//Code to prevent error squiggles when not using smartfusion2 libraries. Set to zero when running on smartfusion2
#define NO_CROSS_COMPILE 0
#if NO_CROSS_COMPILE == 1
int a;
#define g_core_spi0 a
#endif

//The below three macros are present to make transitioning from Core to MSS or vice-versa easier
// Set the spi instance used to communicate with the adf7030
#define ADF_SPI_INSTANCE_t SPI_instance_t

extern ADF_SPI_INSTANCE_t *adf_spi;

//Set the transfer frame function
#define ADF_SPI_TRANSFER_FRAME SPI_transfer

//Set the transer block function
#define ADF_SPI_TRANSFER_BLOCK SPI_transfer

//Set the slave select function
#define ADF_SPI_SLAVE_SELECT    SPI_slave_select

//Set the slave clear function
#define ADF_SPI_SLAVE_CLEAR     SPI_slave_select

//Set the slave cooresponding to ADF
#define ADF_SPI_SLAVE           SPI_SLAVE_0

//Set the block read command
#define ADF_SPI_BLOCK_READ      SPI_block_read

//Set the block write command
#define ADF_SPI_BLOCK_WRITE     SPI_block_write

//Set the single byte write command
#define ADF_SPI_WRITE_BYTE      SPI_write_byte

//Set the single byte read command
#define ADF_SPI_READ_BYTE       SPI_read_byte

//Not using DMA right now. Need to check if DMA can be used
#define ADF_USE_PDMA 0

//Bit definitions

//Bit Definitions for command byte in memory access mode
#define CNM     7u //Command bit
#define RNW     6u // Read not Write
#define BNR     5u // Block not random
#define ANP     4u // Address not pointer
#define LNS     3u // Long not short address

//Memory pointer definitions
#define MPNTR_0     5u
#define MPNTR_1     6u
#define MPNTR_2     7u

//Command to write to the pointers
#define MPNTR_WRITE     0x28


//Memory access modes

/* Mode 1:
    Block read/write, No pointer used, Long address
*/
#define WMODE_1     (1 << BNR) | (1 << ANP) | (1 << LNS)
#define RMODE_1     (1 << RNW) | WMODE_1

/* Mode 2:
    Block read/write, pointer base with offset address, short address
*/
#define WMODE_2     (1 << BNR) | (1 << ANP)
#define RMODE_2     (1 << RNW) | WMODE_2

/* Mode 3:
    Block read/write, pointer, long address
*/
#define WMODE_3     (1 << BNR) | (1 << LNS)
#define RMODE_3     (1 << RNW) | WMODE_3

/* Mode 4:
    Block read/write, pointer, short address
*/
#define WMODE_4     (1 << BNR)
#define RMODE_4     (1 << RNW) | WMODE_4

/* Mode 5:
    Random read/write, no pointer, long address
*/
#define WMODE_5     (1 << ANP) | (1 << LNS)
#define RMODE_5     (1 << RNW) | WMODE_5

/* Mode 6:
    Random read/write, pointer base with offset address, short address
*/
#define WMODE_6     (1 << ANP)
#define RMODE_6     (1 << RNW) | WMODE_6

/* Mode 7:
    Random read/write, pointer, long address
*/
#define WMODE_7     (1 << LNS)
#define RMODE_7     (1 << RNW) | WMODE_7

/* Mode 8:
    Random read/write, pointer, short address
*/
#define WMODE_8     0u
#define RMODE_8     (1 << RNW)

/* SPI Command macros
    Macro to indicate a command byte
*/
#define ADF_SPI_CMD     0x80

#define RMODE1_OFFSET       2

//The below are the commands to reach a particular destination state
#define CMD_PHY_SLEEP       0x80
#define CMD_PHY_OFF         0x81
#define CMD_PHY_ON          0x82
#define CMD_PHY_RX          0x83
#define CMD_PHY_TX          0x84

#define CMD_CFG_DEV         0x85 //Command to apply a configuration

#define CMD_READY           0x20 //To check if adf is ready to receive a command

#define ADF_NOP             0xFF //NOP command

//Macros to define errors
#define ERR_LENGTH_OVERFLOW     0x1//Error when length overflows 0xFFFF in config_adf()
#define ERR_CMD_FAILED          0x2//Error when send_cmd fails

void set_adf_spi_instance(ADF_SPI_INSTANCE_t *instance);

/**
 * @brief function to write to adf memory
 * 
 * @param mode : Mode to access memory. Select from any of the above eight modes(presently only mode one is implemented)
 * @param addr : Address to write to memory
 * @param data : data to write to memory
 * @param size : no of bytes to write
 * @return uint8_t 
 */
uint8_t adf_write_to_memory(uint8_t mode,uint32_t addr,uint8_t *data,uint32_t size);

uint8_t* adf_read_from_memory(uint8_t mode,uint32_t addr,uint8_t *data,uint32_t size);

uint8_t adf_send_cmd(uint8_t command);

extern uint8_t radio_memory_configuration[];


uint8_t config_adf7030();
#endif