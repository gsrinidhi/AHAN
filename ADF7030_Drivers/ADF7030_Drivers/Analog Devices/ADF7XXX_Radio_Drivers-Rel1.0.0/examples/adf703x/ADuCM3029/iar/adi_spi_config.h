/*!
 *****************************************************************************
   @file:    adi_spi_config.h
   @brief:   Configuration options for SPI driver.
             This is specific to the SPI driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 29310 $
   @date:    $Date: 2014-12-18 03:31:03 -0500 (Thu, 18 Dec 2014) $
  -----------------------------------------------------------------------------

Copyright (c) 2012-2013 Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Modified versions of the software must be conspicuously marked as such.
  - This software is licensed solely and exclusively for use with processors
    manufactured by or for Analog Devices, Inc.
  - This software may not be combined or merged with other code in any manner
    that would cause the software to become subject to terms and conditions
    which differ from those listed here.
  - Neither the name of Analog Devices, Inc. nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.
  - The use of this software may or may not infringe the patent rights of one
    or more patent holders.  This license does not release you from the
    requirement that you obtain separate licenses from these patent holders
    to use this software.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF CLAIMS OF INTELLECTUAL
PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/

#ifndef __ADI_SPI_CONIG_H__
#define __ADI_SPI_CONIG_H__

/*! Set this macro to the system clock frequency in hertz*/
#define ADI_CFG_SYSTEM_CLOCK_HZ                    100000000

/************* SPI Driver configurations ***************/

/*!
   Enable DMA support in the driver code.
   1 -  To have the DMA support code in the driver.
   0 -  To eliminate the DMA support
*/
#define ADI_SPI_CFG_ENABLE_DMA_SUPPORT                     0

/*!
  Enable/Disable DMA by default
  1 - Enable DMA by default
  0 - Disable DMA by default. PIO mode will be enabled by default.
*/
#define ADI_SPI_CFG_ENABLE_DMA                             0

/*!
   Set this macro to 1 for adding the interrupt support code to the driver.
   To eliminate the interrupt support, set the macro to 0.
*/
#define ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT                 1

/*!
   Set this macro to 1 for adding the SPI master mode code to the driver.
   To eliminate the SPI master mode, set the macro to 0.
*/
#define ADI_SPI_CFG_MASTER_MODE_SUPPORT                    1

/*!
   Set this macro to 1 for adding the SPI slave mode code to the driver.
   To eliminate the SPI slave mode, set the macro to 0.
*/
#define ADI_SPI_CFG_SLAVE_MODE_SUPPORT                     1

/*!
   Set this macro to 1 for to enable static controller initializations
   during the driver initialization routing.
   To eliminate static driver configuration, set this macro to 0.
*/
#define ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT           0



#if (ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)

/************* SPI controller configurations ***************/

/* There are three SPI instances SPI0, SPI1 and SPI2       */
/*! Set this macro to the SPI bit rate in hertz */
#define ADI_SPI0_CFG_BIT_RATE_DIV             30
#define ADI_SPI1_CFG_BIT_RATE_DIV             30
#define ADI_SPI2_CFG_BIT_RATE_DIV             30

/*! SPI enable
    SPI configuration register: Bit[0]
    1 -  Enable SPI
    0 -  Disable SPI */
#define SPI0_CFG_ENABLE                         0
#define SPI1_CFG_ENABLE                         0
#define SPI2_CFG_ENABLE                         0

/*! Master slave or slave mode operation
    SPI configuration register: Bit[1]
    1 - Master mode.
    0 - Slave mode. */
#define SPI0_CFG_MASTER_SLAVE                   1
#define SPI1_CFG_MASTER_SLAVE                   1
#define SPI2_CFG_MASTER_SLAVE                   1

/*! SPI clock phase mode
    SPI configuration register: Bit[2]
    1 - Serial clock pulses at the beginning of each serial bit transfer.
    0 - Serial clock pulses at the end of each serial bit transfer. */
#define SPI0_CFG_CLK_PHASE                      0
#define SPI1_CFG_CLK_PHASE                      0
#define SPI2_CFG_CLK_PHASE                      0

/*! SPI clock polarity
    SPI configuration register: Bit[3]
    1 - Serial clock idles high.
    0 - Serial clock idles low. */
#define SPI0_CFG_CLK_POLARITY                   0
#define SPI1_CFG_CLK_POLARITY                   0
#define SPI2_CFG_CLK_POLARITY                   0

/*! SPI wired OR mode
    SPI configuration register: Bit[4]
    1 - Enables open circuit output enable.
    0 - Normal output levels. */
#define SPI0_CFG_WIRED_OR                       0
#define SPI1_CFG_WIRED_OR                       0
#define SPI2_CFG_WIRED_OR                       0

/*! SPI LSB/MSB
    SPI configuration register: Bit[5]
    1 - MSB transmitted first.
    0 - LSB transmitted first. */
#define SPI0_CFG_LSB_MSB                        0
#define SPI1_CFG_LSB_MSB                        0
#define SPI2_CFG_LSB_MSB                        0

/*! SPI transfer initiate
    SPI configuration register: Bit[6]
    1 - SPI transfer is initiated with write to Tx FIFO register. Interrupts when Tx is empty.
    0 - SPI transfer is initiated with a read of the Rx FIFO register. Interrupts when Rx is full.*/
#define SPI0_CFG_TRANSFER_INITIATE              0
#define SPI1_CFG_TRANSFER_INITIATE              0
#define SPI2_CFG_TRANSFER_INITIATE              0

/*! SPI Tx FIFO transfers zeros or last bit upon underflow
    SPI configuration register: Bit[7]
    1 - Tx FIFO sends zeros upon undeflow.
    0 - Tx FIFO repeats last bit upon undeflow. */
#define SPI0_CFG_TX_UNDERFLOW                   0
#define SPI1_CFG_TX_UNDERFLOW                   0
#define SPI2_CFG_TX_UNDERFLOW                   0

/*! SPI Rx FIFO overflows with received data or data is discarded
    SPI configuration register: Bit[8]
    1 - Rx FIFO receives data upon overflow.
    0 - Rx FIFO discards received data upon overflow. */
#define SPI0_CFG_RX_OVERFLOW                    0
#define SPI1_CFG_RX_OVERFLOW                    0
#define SPI2_CFG_RX_OVERFLOW                    0

/*! SPI slave mode MISO enable
    SPI configuration register: Bit[9]
    1 - MISO operates as normal in slave mode.
    0 - MISO is disabled in slave mode. */
#define SPI0_CFG_MISO_ENABLE                    0
#define SPI1_CFG_MISO_ENABLE                    0
#define SPI2_CFG_MISO_ENABLE                    0

/*! SPI internal loopback enable
    SPI configuration register: Bit[10]
    1 - MISO and MOSI is loopbacked internally.
    0 - MISO and MOSI operates normally. */
#define SPI0_CFG_LOOPBACK                       0
#define SPI1_CFG_LOOPBACK                       0
#define SPI2_CFG_LOOPBACK                       0

/*! SPI transfer and interrupt mode
    SPI configuration register: Bit[11]
    1 - SPI continous transfers in which CS remains asserted until Tx is empty.
    0 - SPI disable continuous transfer, each transfer consistes of 8 bits of data.*/
#define SPI0_CFG_CONTINUOUS                     0
#define SPI1_CFG_CONTINUOUS                     0
#define SPI2_CFG_CONTINUOUS                     0

/*! SPI Rx FIFO flush enable
    SPI configuration register: Bit[12]
    1 - Rx FIFO is flushed and all rx data is ignored and no interrupts are generated.
    0 - Rx FIFO flush is disabled. */
#define SPI0_CFG_RX_FLUSH                       0
#define SPI1_CFG_RX_FLUSH                       0
#define SPI2_CFG_RX_FLUSH                       0

/*! SPI Tx FIFO flush enable
    SPI configuration register: Bit[13]
    1 - Tx FIFO is flushed.
    0 - Tx FIFO flush is disabled. */
#define SPI0_CFG_TX_FLUSH                       0
#define SPI1_CFG_TX_FLUSH                       0
#define SPI2_CFG_TX_FLUSH                       0

/*! Reset Mode for CSERR.       
    SPI configuration register: Bit[14]
    0 -  To  continue from where it stopped. SPI can receive the remaining bits 
         when CS gets asserted and Cortex has to ignore the CSERR interrupt.
    1 -  To enable tresetting the bit ounter and  reset if there is  a 
         CS error condition and the Cortex is expected to clear the SPI_EN bit.
*/
#define SPI0_CFG_CSERR_RESET                       0
#define SPI1_CFG_CSERR_RESET                       0
#define SPI2_CFG_CSERR_RESET                       0

/*! SPI clock divide
    SPI baud rate selection register: Bit[0:5]
    Value between 0-63 that is used to divide the UCLK to generate
    the SPI serial clock. */
#define SPI0_CFG_CLK_DIV                        0
#define SPI1_CFG_CLK_DIV                        0
#define SPI2_CFG_CLK_DIV                        0

/*! SPI high frequency mode
    SPI baud rate selection register: Bit[6]
    1 - High frequency mode enabled.
    0 - High frequency mode disabled. */
#define SPI0_CFG_HFM                            0
#define SPI1_CFG_HFM                            0
#define SPI2_CFG_HFM                            0

/*! SPI reset mode for CSERR
    SPI baud rate selection register: Bit[7]
    1 - clear bit counter on CS error.
    0 - do not clear bit counter on CS error. */
#define SPI0_CFG_CS_ERR                         0
#define SPI1_CFG_CS_ERR                         0
#define SPI2_CFG_CS_ERR                         0

/*! SPI CS interrupt
    SPI baud rate selection register: Bit[8]
    1 - In continuous mode, generate interrupt on CS.
    0 - In continuous mode, do not generate interrupt on CS. */
#define SPI0_CFG_CS_IRQ                         0
#define SPI1_CFG_CS_IRQ                         0
#define SPI2_CFG_CS_IRQ                         0

/************** Macro validation *****************************/

#if ( ADI_SPI0_CFG_BIT_RATE_HZ > 8000000 ) || \
    ( ADI_SPI1_CFG_BIT_RATE_HZ > 8000000 ) || \
    ( ADI_SPI2_CFG_BIT_RATE_HZ > 8000000 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_ENABLE > 1 ) || \
    ( SPI1_CFG_ENABLE > 1 ) || \
    ( SPI2_CFG_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_MASTER_SLAVE > 1 ) || \
    ( SPI1_CFG_MASTER_SLAVE > 1 ) || \
    ( SPI2_CFG_MASTER_SLAVE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CLK_PHASE > 1 ) || \
    ( SPI1_CFG_CLK_PHASE > 1 ) || \
    ( SPI2_CFG_CLK_PHASE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CLK_POLARITY > 1 ) || \
    ( SPI1_CFG_CLK_POLARITY > 1 ) || \
    ( SPI2_CFG_CLK_POLARITY > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_WIRED_OR > 1 ) || \
    ( SPI1_CFG_WIRED_OR > 1 ) || \
    ( SPI2_CFG_WIRED_OR > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_LSB_MSB > 1 ) || \
    ( SPI1_CFG_LSB_MSB > 1 ) || \
    ( SPI2_CFG_LSB_MSB > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_TRANSFER_INITIATE > 1 ) || \
    ( SPI1_CFG_TRANSFER_INITIATE > 1 ) || \
    ( SPI2_CFG_TRANSFER_INITIATE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_TX_UNDERFLOW > 1 ) || \
    ( SPI1_CFG_TX_UNDERFLOW > 1 ) || \
    ( SPI2_CFG_TX_UNDERFLOW > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_RX_OVERFLOW > 1 ) || \
    ( SPI1_CFG_RX_OVERFLOW > 1 ) || \
    ( SPI2_CFG_RX_OVERFLOW > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_MISO_ENABLE > 1 ) || \
    ( SPI1_CFG_MISO_ENABLE > 1 ) || \
    ( SPI2_CFG_MISO_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_LOOPBACK > 1 ) || \
    ( SPI1_CFG_LOOPBACK > 1 ) || \
    ( SPI2_CFG_LOOPBACK > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CONTINUOUS > 1 ) || \
    ( SPI1_CFG_CONTINUOUS > 1 ) || \
    ( SPI2_CFG_CONTINUOUS > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_RX_FLUSH > 1 ) || \
    ( SPI1_CFG_RX_FLUSH > 1 ) || \
    ( SPI2_CFG_RX_FLUSH > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_TX_FLUSH > 1 ) || \
    ( SPI1_CFG_TX_FLUSH > 1 ) || \
    ( SPI2_CFG_TX_FLUSH > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_IRQ_MODE > 3 ) || \
    ( SPI1_CFG_IRQ_MODE > 3 ) || \
    ( SPI2_CFG_IRQ_MODE > 3 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CLK_DIV > 63 ) || \
    ( SPI1_CFG_CLK_DIV > 63 ) || \
    ( SPI2_CFG_CLK_DIV > 63 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_HFM > 1 ) || \
    ( SPI1_CFG_HFM > 1 ) || \
    ( SPI2_CFG_HFM > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CS_ERR > 1 ) || \
    ( SPI1_CFG_CS_ERR > 1 ) || \
    ( SPI2_CFG_CS_ERR > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CS_IRQ > 1 ) || \
    ( SPI1_CFG_CS_IRQ > 1 ) || \
    ( SPI2_CFG_CS_IRQ > 1 )
#error "Invalid configuration"
#endif

      
#if ((ADI_SPI_CFG_ENABLE_DMA == 1) && (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 0))
#error "DMA cannot be enabled if DMA support is disabled"
#endif

#endif /* ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT */

#endif /* __ADI_SPI_CONIG_H__ */
