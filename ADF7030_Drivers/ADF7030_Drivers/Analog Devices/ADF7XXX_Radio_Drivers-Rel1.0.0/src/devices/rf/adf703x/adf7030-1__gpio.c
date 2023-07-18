/*!
 *****************************************************************************
  @file:	adf7030-1__gpio.c
  
  @brief:	adf7030-1 PHY Radio GPIO interface Functions.
  
  @details:	...
  
  @version:	$Revision:
  
  @date:	$Date:
 -----------------------------------------------------------------------------
Copyright (c) 2017, Analog Devices, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted (subject to the limitations in the disclaimer below) provided that
the following conditions are met:
  - Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
  - Neither the name of Analog Devices, Inc. nor the names of its contributors 
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY 
THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/


/** \addtogroup adf7030-1 adf7030-1 Driver
 *  @{
 */

/** \addtogroup adf7030-1__gpio GPIO Interface
 *  @{
 */
#ifndef _ADF7030_1__GPIO_C_
/*! \cond PRIVATE */
#define _ADF7030_1__GPIO_C_

#include <sys/platform.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#include <drivers/spi/adi_spi.h>

#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>
#include <adf7030-1__core.h>
#include <adf7030-1__spi.h>
#include <adf7030-1__mem.h>
#include <adf7030-1__state.h>
   
#include "adf7030-1__gpio.h"





#ifdef __ICCARM__
/*
* IAR MISRA C 2004 error suppressions.
*
* Pm140 (rule 11.3): a cast should not be performed between a pointer type and an integral type
*   The rule makes an exception for memory-mapped register accesses.
*
* Pm073 (rule 14.7): a function should have a single point of exit.
* Pm143 (rule 14.7): a function should have a single point of exit at the end of the function.
*   Multiple returns are used for error handling.
*/
#pragma diag_suppress=Pm073,Pm143
#endif /* __ICCARM__ */

/*! \endcond */



/**
 * @brief       Setup individual GPIO configuration on the PHY Radio
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  GPIO            PHY Radio GPIO pin to be configured
 *
 * @param [in]  CFG             PHY Radio GPIO pin configuration desired (see ADI_ADF7030_1_GPIO_CFG)
 *
 * @param [in]  bDirect         Boolean  -TRUE to directly change the current GPIO pin configuration
 *                                       -FALSE to setup GPIO pin configuration in the profile.
 *                                              The Host must issue a CFG_DEV command to trigger
 *                                              the new setting.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS              If the Radio PHY gpio configuration was applied correctly.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED      [D] If the communication with Radio PHY failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__GPIO_SetCfg(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    ADI_ADF7030_1_GPIO_PIN      eGPIO,
    ADI_ADF7030_1_GPIO_CFG      eCFG,
    bool                        bDirect
)
{
    /* Setup configuration address */
    uint32_t Addr = ((bDirect == true) ? 0x40000800UL : 0x20000394UL) + (uint32_t)eGPIO;
    
    /* Transfer configuration over SPI */
    adf7030_1__SPI_SetField( pSPIDevInfo,
                             (Addr >> 2) << 2,
                             (Addr & 0x3) << 3,
                             8,
                             (uint32_t)eCFG );
      
    /* Return SPI transfer status */
    return(pSPIDevInfo->eXferResult);  
}
  


/**
 * @brief       Get individual GPIO configuration from the PHY Radio
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  GPIO            PHY Radio GPIO pin to be configured
 *
 * @param [in]  pCFG            Pointer to PHY Radio GPIO pin configuration (see ADI_ADF7030_1_GPIO_CFG)
 *
 * @param [in]  bDirect         Boolean  -TRUE to directly change the current GPIO pin configuration
 *                                       -FALSE to setup GPIO pin configuration in the profile.
 *                                              The Host must issue a CFG_DEV command to trigger
 *                                              the new setting.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS              If the Radio PHY gpio configuration was read correctly.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED      [D] If the communication with Radio PHY failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__GPIO_GetCfg(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    ADI_ADF7030_1_GPIO_PIN      eGPIO,
    ADI_ADF7030_1_GPIO_CFG *    peCFG,
    bool                        bDirect
)
{
    /* Setup configuration address */
    uint32_t Addr = ((bDirect == true) ? 0x40000800UL : 0x20000394UL) + (uint32_t)eGPIO;
    
    /* Readback configuration over SPI */    
    *peCFG = (ADI_ADF7030_1_GPIO_CFG)adf7030_1__SPI_GetField( pSPIDevInfo,
                                                              (Addr >> 2) << 2,
                                                              (Addr & 0x3) << 3,
                                                              8 );
      
    /* Return SPI transfer status */
    return(pSPIDevInfo->eXferResult);  
}
  

/**
 * @brief       Set an individual PHY Radio GPIO pin to 1
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  eGPIO           PHY Radio GPIO pin to be set
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS              If the Radio PHY gpio pin output was asserted correctly.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED      [D] If the communication with Radio PHY failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__GPIO_SetPin(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    ADI_ADF7030_1_BUSIO_PIN     ePin
)
{   
    /* Transfer configuration over SPI */
    adf7030_1__SPI_SetMem32( pSPIDevInfo,
                             0x4000081CUL,
                             ePin );
      
    /* Return SPI transfer status */
    return(pSPIDevInfo->eXferResult);
}


/**
 * @brief       Clear an individual PHY Radio GPIO pin to 0
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  eGPIO           PHY Radio GPIO pin to be cleared
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS              If the Radio PHY gpio pin output was cleared correctly.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED      [D] If the communication with Radio PHY failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__GPIO_ClrPin(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    ADI_ADF7030_1_BUSIO_PIN     ePin
)
{
    /* Transfer configuration over SPI */
    adf7030_1__SPI_SetMem32( pSPIDevInfo,
                             0x40000820UL,
                             ePin );
      
    /* Return SPI transfer status */
    return(pSPIDevInfo->eXferResult);
}

#endif /* _ADF7030_1__GPIO_C_ */

/** @} */ /* End of group adf7030-1__gpio GPIO Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */
