/*
 **
 ** pinmux_config.c source file generated on November 21, 2016 at 14:52:31.	
 **
 ** Copyright (C) 2000-2016 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated automatically based upon the options selected in 
 ** the Pin Multiplexing configuration editor. Changes to the Pin Multiplexing
 ** configuration should be made by changing the appropriate options rather
 ** than editing this file.
 **
 ** Selected Peripherals
 ** --------------------
 ** SPI2 (SCLK, MISO, MOSI, CS_3)
 **
 ** GPIO (unavailable)
 ** ------------------
 ** P1_02, P1_03, P1_04, P2_07
 */

#include <sys/platform.h>
#include <stdint.h>

#define SPI2_SCLK_PORTP1_MUX  ((uint16_t) ((uint16_t) 1<<4))
#define SPI2_MISO_PORTP1_MUX  ((uint16_t) ((uint16_t) 1<<6))
#define SPI2_MOSI_PORTP1_MUX  ((uint16_t) ((uint16_t) 1<<8))
#define SPI2_CS_3_PORTP2_MUX  ((uint16_t) ((uint16_t) 2<<14))

#define SPI2_SCLK_PORTP1_FER  ((uint16_t) ((uint16_t) 1<<2))
#define SPI2_MISO_PORTP1_FER  ((uint16_t) ((uint16_t) 1<<3))
#define SPI2_MOSI_PORTP1_FER  ((uint16_t) ((uint16_t) 1<<4))
#define SPI2_CS_3_PORTP2_FER  ((uint16_t) ((uint16_t) 1<<7))

int32_t adi_initpinmux(void);

/*
 * Initialize the Port Control MUX and FER Registers
 */
int32_t adi_initpinmux(void) {
    /* PORTx_MUX registers */
    *((volatile uint32_t *)REG_GPIO1_CFG) = SPI2_SCLK_PORTP1_MUX | SPI2_MISO_PORTP1_MUX
     | SPI2_MOSI_PORTP1_MUX;
    *((volatile uint32_t *)REG_GPIO2_CFG) = SPI2_CS_3_PORTP2_MUX;

    return 0;
}

