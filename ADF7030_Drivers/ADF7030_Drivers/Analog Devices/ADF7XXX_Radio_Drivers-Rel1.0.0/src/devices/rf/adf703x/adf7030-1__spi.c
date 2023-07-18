/*!
 *****************************************************************************
  @file:	adf7030-1_spi.c
  
  @brief:	SPI communication with between PHY Radio and the Host.
				- SPI Xfer with between Host and Radio.
				- Inject, Eject
  
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

/** \addtogroup adf7030-1__spi SPI Communication Interface
 *  @{
 */

#ifndef _ADF7030_1__SPI_C_
/*! \cond PRIVATE */
#define _ADF7030_1__SPI_C_

#include <sys/platform.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <adi_types.h>




#ifdef ADI_DEBUG 
#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
#include <ssdd_common/common_def.h>
#include <services/dma/adi_dma.h>
#include <services/gpio/adi_gpio.h>
#include <../src/drivers/spi/adi_spi_def_v1.h>
#elif defined (__ADUCM4050__)
#include <drivers/gpio/adi_gpio.h>
#include <../Source/drivers/spi/adi_spi_def.h>
#endif
#endif

#include <drivers/spi/adi_spi.h>

#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>
#include <adf7030-1__core.h>
#include <adf7030-1__state.h>

#include "adf7030-1__spi.h"
   

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
 * @brief       Initializes SPI device for communication with the adf7030-1 PHY
 *
 * @note        This function open the SPI peripheral in "blocking mode" 
 *              without DMA transfert assistance. blablabla... 
 *
 * @param [in]  hDevice             Handle to ADF3030-1 device instance.
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If SPI peripheral was succesfully initialise
 *  - #ADI_ADF7030_1_SPI_DEV_FAILED     [D] SPI peripheral failed to initialise
 *
 * @sa  adf7030_1__SPI_UnInit().
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_Init(
    ADI_ADF7030_1_HANDLE  const         hDevice
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;
    
#ifdef ADI_DEBUG
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;

    /* Validate the given device handle */
    if((eResult = ValidateHandle(pDevice)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
#endif /* ADI_DEBUG */   
      
    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO *pDevInfo = pDevice->pDevInfo;

    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO *  pSPIDevInfo = &pDevInfo->SPIInfo;
        
    /* Setup pointer to parent device information in opened SPI device info*/
    pSPIDevInfo->hDevInfo = (void *)pDevInfo;
    
    /* Handle to SPI device */
    ADI_SPI_HANDLE hSPIDevice;

    if(adi_spi_Open(pSPIDevInfo->nDeviceNum,
                    pSPIDevInfo->pDevMemory,
                    pSPIDevInfo->nMemorySize,
                    &hSPIDevice) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
    
    /* Save SPI device handle into ADF7030-1 instance SPI information */
    pSPIDevInfo->hSPIDevice = hSPIDevice;

    /* Enable underflow errors */
    if(adi_spi_SetTransmitUnderflow(hSPIDevice, true) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
       
    /* Set the chip select */
    if(adi_spi_SetChipSelect(hSPIDevice, pSPIDevInfo->eChipSelect) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }

    /* Set the default SPI clock rate */
    if(adf7030_1__SPI_SetSpeed(pSPIDevInfo, DEFAULT_SPI_RATE) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }

#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
    /* Set the SPI clock phase */
    if(adi_spi_SetClockPhase(hSPIDevice, false) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }    
    
    /* Set the continuous transfer mode */
    if(adi_spi_SetContinousMode(hSPIDevice, true) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
    
    /* Disable DMA transfer mode */
    if(adi_spi_EnableDmaMode(hSPIDevice, false) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
#elif defined (__ADUCM4050__)
#warning "Make sure the SPI peripheral is configured with properly... No SPI DMA support"
#endif

    return ADI_ADF7030_1_SUCCESS;
}

/**
 * @brief       Uninitializes SPI device used to communication with the adf7030-1 PHY
 *
 * @note        This function release and deallocate memory used by the SPI peripheral
 *              
 * @param [in]  hDevice             Handle to ADF3030-1 device instance.
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If SPI peripheral was succesfully released
 *  - #ADI_ADF7030_1_SPI_DEV_FAILED     [D] SPI peripheral failed to release
 *
 * @sa  adf7030_1__SPI_Init().
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_UnInit(
    ADI_ADF7030_1_HANDLE  const         hDevice
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;
    
#ifdef ADI_DEBUG
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;

    /* Validate the given device handle */
    if((eResult = ValidateHandle(pDevice)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
#endif /* ADI_DEBUG */

    /* Handle to SPI device */
    ADI_SPI_HANDLE hSPIDevice = pDevice->pDevInfo->SPIInfo.hSPIDevice;
    
    /* Unregister any callbacks on the SPI driver if any were setup */
    adi_spi_RegisterCallback (hSPIDevice, NULL, NULL );

    /* Close the SPI Device */
    if(adi_spi_Close(hSPIDevice) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
    
    /* Setup pointer to parent device information in opened SPI device info*/
    pDevice->pDevInfo->SPIInfo.hDevInfo = NULL;
    
    return ADI_ADF7030_1_SUCCESS;
}

/**
 * @brief       Change SPI Host to PHY Communication bitrate
 *
 * @note        Default speed is set to 3.5MBps, anything faster should really 
 *              used DMA operation mode which is corrently not supported
 * 
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  RateHz          Target bitrate (in Hz) for SPI communication.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If SPI peripheral was succesfully released
 *  - #ADI_ADF7030_1_SPI_DEV_FAILED     [D] SPI peripheral failed to release
 *
 * @sa  adi_spi_SetBitrate().
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_SetSpeed(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    ADI_ADF7030_1_RADIO_SPI_RATE_TYPE   eSPIRate
)
{       
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    /* Get the desired SPI clock frequency from pSPIDevInfo structure */
    uint32_t RateHz = (eSPIRate == DEFAULT_SPI_RATE) ? pSPIDevInfo->nClkFreq : pSPIDevInfo->nClkFreq_Fast;
    
    /* Update the SPI datarate only if different from the current datarate */
    if(pSPIDevInfo->nClkFreq_Current != RateHz)
    {
        if(adi_spi_SetBitrate(pSPIDevInfo->hSPIDevice, RateHz) != ADI_SPI_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_DEV_FAILED;
        }
        
        pSPIDevInfo->nClkFreq_Current = RateHz;
    }
#else    
    if(adi_spi_SetBitrate(pSPIDevInfo->hSPIDevice, pSPIDevInfo->nClkFreq) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
#endif
    return ADI_ADF7030_1_SUCCESS;
}

/**
 * @brief       Set SPI Radio mmap pointers access
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @note        This function copy the content of pSPIDevInfo->PHY_PNTR[]
 *              into the PHY radio. 
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the configuration of mmap pointers was succesfull.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the configuration of mmap pointers failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_SetMMapPointers(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo
)
{   
    /* Pointer to user defined PHY mmap SPI pointers */ 
    uint32_t * pPNTR = (uint32_t *)&pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR];
      
    return(adf7030_1__SPI_wr_word_b_p( pSPIDevInfo,
                                       PNTR_SETUP_ADDR,
                                       3,
                                       pPNTR));
}

/**
 * @brief       Get SPI Radio mmap pointers access
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @note        This function recall the content of PHY radio mmap pointers back
 *              into the pSPIDevInfo->PHY_PNTR[]. 
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the readback of mmap pointers was succesfull.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the readback failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_GetMMapPointers(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo
)
{
    /* Pointer to user defined PHY mmap SPI pointers */ 
    uint32_t * pPNTR = (uint32_t *)&pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR];
    
    return(adf7030_1__SPI_rd_word_b_p( pSPIDevInfo,
                                       PNTR_SETUP_ADDR,
                                       3,
                                       pPNTR));
}

/**
 * @brief       This function return the best Pointer id + offset for subsequent
 *              SPI transfert to the PHY "Addr" provided.
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            Desired Radio PHY address.
 *
 * @param [out] pPNTR_ID        Pointer to PNTR_ID variable to access "Addr" location of mmap
 *                              eg. pSPIDevInfo->PHY_PNTR[pPNTR_ID].
 *
 * @param [out] ByteOffset      Pointer to the positive byte offset variable between
 *                              "Addr" and pSPIDevInfo->PHY_PNTR[pPNTR_ID].
 *
 * @return      None
 */

void adf7030_1__SPI_FindMMapPointer(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    uint32_t                            Addr,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE * pPNTR_ID,
    int32_t *                           pByteOffset
)
{
    uint32_t mId = 0;
    int32_t diff;
    
    *pPNTR_ID = PNTR_SETUP_ADDR;
    
    /* Search through pSPIDevInfo->PHY_PNTR[] for a Pointer within range of "Addr" */
    for(uint32_t id = PNTR_SRAM_ADDR; id <= PNTR_CUSTOM2_ADDR; id++)
    {
        /* Calculate byte offset with the current pSPIDevInfo->PHY_PNTR[id] */
        diff = (int32_t)Addr - (int32_t)pSPIDevInfo->PHY_PNTR[id];
        
        /* Stop search if offset within range */
        if((diff > 0) && (diff <= 255))
        {
           mId = id;
           *pByteOffset = diff;
        }
        else if(diff == 0)
        {
           mId = id;
           *pByteOffset = diff;
        }
    }
    
    if(mId)
    {
        /* pSPIDevInfo->PHY_PNTR[] contains "Addr", Write Back pPNTR_ID variable */
        *pPNTR_ID = (ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE)mId;
    }   
}  


/**
 * @brief       Change the SPI Radio mmap custom pointers 0 location
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            Value of the pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the "Addr" is within range for the SPI transfer.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If no PointerID is available.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_SetMMapCustomPntr0(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr
)
{
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT eResult;
  
    /* Update the Radio PHY spi pointer 0 */
    if((eResult = adf7030_1__SPI_wr_word_b_p( pSPIDevInfo,
                                              PNTR_SETUP_ADDR,
                                              1,
                                              &Addr)) == ADI_ADF7030_1_SUCCESS)
    {
        /* Update the host pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] to Addr value */
        pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] = Addr;
    }

    return eResult;
}


/**
 * @brief       Write a number of word(s) from Host to memory of the adf7030-1
 *              Generic 32bits bloc address write to memory (HRM 1.4.4.2.3).
 *              This Function should be used when writing a large block of 
 *              32bits word to the radio (above ADI_SPI_MEMORY_SIZE).
 *
 * @note        If the number of words to be transfered exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the writting operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  AddrIn          Address location at which the SPI write should 
 *                              start.
 *
 * @param [in]  num_xfrs        Number of 32bits word write to be performed.
 *
 * @param [in]  pDataIn         Pointer to the data block to be transmitted.
 *                              On exit, pDataIn is pointing at the location of 
 *                              the AddrIn + num_xfrs of the host memory. 
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 *
 * @sa  blabla()
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_wr_word_b_a(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    AddrIn,
    uint32_t                    num_xfrs,
    uint32_t *                  pDataIn
)
{   
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;
    
    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;
  
    /* Transmit SPI command - use 3bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 3) = ADF703x_SPI_MEM_WRITE |
                          ADF703x_SPI_MEM_BLOCK |
                          ADF703x_SPI_MEM_ADDRESS |
                          ADF703x_SPI_MEM_LONG;
					
    /* Write the starting address in the spi_txbuf */
    *(((uint32_t *)pSPI_TX_BUFF) + 1) = __REV(AddrIn);
    
    /* Get nbBytes transfer */
    uint32_t nbBytes = (num_xfrs << 2);

    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)    
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 5) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif
    
    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     3,
                     PNTR_SETUP_ADDR,
                     AddrIn,
                     nbBytes,
                     pDataIn,
                     NULL));
 }


/**
 * @brief       Read & Compare a number of word(s) from the adf7030-1 to memory of Host
 *              Generic 32bits bloc address read from memory (HRM 1.4.4.2.3).
 *              This Function should be used when reading a large block of 
 *              32bits word from the radio (above ADI_SPI_MEMORY_SIZE).
 *
 * @note        If the number of words to read exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the reading operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  AddrIn          Address location at which the SPI write should 
 *                              start.
 *
 * @param [in]  num_xfrs        Number of 32bits word write to be performed.
 *
 * @param [out] pDataOut        Pointer to the write back location in the host.
 *                              On exit, pDataOut is pointing at the location of 
 *                              the AddrIn + num_xfrs of the host memory. 
 *
 * @param [in]  pDataRef        Pointer to the start of the Ref Host data block.
 *                              If set, the SPI readback data will be compared with the 
 *                              data pointed by pDataRef. This can be used to compare
 *                              data between Host and PHY directly within the readback.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 *
 * @sa  blabla()
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_rd_cmp_word_b_a(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    AddrIn,
    uint32_t                    num_xfrs,
    uint32_t *                  pDataOut,
    uint32_t *                  pDataRef
)
{   
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;
    
    /* Transmit SPI command - use 1bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 1) = ADF703x_SPI_MEM_READ | 
                          ADF703x_SPI_MEM_BLOCK |
                          ADF703x_SPI_MEM_ADDRESS |
                          ADF703x_SPI_MEM_LONG;

    /* Write the starting address in the spi_txbuf */
    *((uint16_t *)pSPI_TX_BUFF + 1) = __REV16(*((uint16_t *)&AddrIn + 1));
    *((uint16_t *)pSPI_TX_BUFF + 2) = __REV16(*((uint16_t *)&AddrIn + 0));
    *((uint16_t *)pSPI_TX_BUFF + 3) = 0xFFFF;
    
    /* Get nbBytes transfer */
    uint32_t nbBytes = (num_xfrs << 2);

    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)  
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 7) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif
    
    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     1,
                     PNTR_SETUP_ADDR,
                     AddrIn,
                     nbBytes,
                     pDataOut,
                     pDataRef));
}

/**
 * @brief       Write a number of word(s) from Host to memory of the adf7030-1
 *              starting at location pointed by "pntr"
 *              Generic 32bits bloc pointer write to memory (HRM 1.4.4.2.5/9)
 *              This Function should be used when writing a large block of 
 *              32bits word to the radio (above ADI_SPI_MEMORY_SIZE).
 *
 * @note        If the number of words to be transfered exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the writting operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pntrID          SPI pointer ID to use as base address for writing 
 *                              the block of data.
 *
 * @param [in]  num_xfrs        Number of 32bits word write to be performed.
 *
 * @param [in]  pDataIn         Pointer to the data block to be transmitted.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 *
 * @sa  blabla()
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_wr_word_b_p(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   pntrID,
    uint32_t                            num_xfrs,
    uint32_t *                          pDataIn
)
{       
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;

    /* Transmit SPI command - use 3bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 3) = ADF703x_SPI_MEM_WRITE |
                          ADF703x_SPI_MEM_BLOCK |
                          ADF703x_SPI_MEM_POINTER |
                          ADF703x_SPI_MEM_LONG |
                          pntrID;

    /* Get nbBytes transfer */
    uint32_t nbBytes = (num_xfrs << 2);
    
    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 1) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif
    
    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     3,
                     pntrID,
                     0,
                     nbBytes,
                     pDataIn,
                     NULL));
}


/**
 * @brief       Read & Compare a number of word(s) from memory of the adf7030-1 to Host
 *              starting at location pointed by "pntr"
 *              Generic 32bits bloc pointer read from memory (HRM 1.4.4.2.5/9)
 *
 * @note        If the number of words to be received exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the writting operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pntrID          SPI pointer ID to use as base address for reading 
 *                              the block of data.
 *
 * @param [in]  num_xfrs        Number of 32bits word read to be performed.
 *
 * @param [out]  pDataOut       Pointer to the write back location in the host.
 *
 * @param [in]  pDataRef        Pointer to the start of the Ref Host data block.
 *                              If set, the SPI readback data will be compared with the 
 *                              data pointed by pDataRef. This can be used to compare
 *                              data between Host and PHY directly within the readback.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 *
 * @sa  blabla()
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_rd_cmp_word_b_p(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   pntrID,
    uint32_t                            num_xfrs,
    uint32_t *                          pDataOut,
    uint32_t *                          pDataRef
)
{
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;

    /* Transmit SPI command - use 1bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 1) = ADF703x_SPI_MEM_READ |
                          ADF703x_SPI_MEM_BLOCK |
                          ADF703x_SPI_MEM_POINTER |
                          ADF703x_SPI_MEM_LONG |
                          pntrID;
					
    /* Add 16bits of NOPs in spi_txbuf */
    *((uint16_t *)pSPI_TX_BUFF + 1) = 0xFFFF;
    
    /* Get nbBytes transfer */
    uint32_t nbBytes = (num_xfrs << 2);
    
    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 3) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif

    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     1,
                     pntrID,
                     0,
                     nbBytes,
                     pDataOut,
                     pDataRef));
}


/**
 * @brief       Write a number of byte(s) from Host to memory of the adf7030-1
 *              starting at location pointed by "pntr" + AddrIn
 *              Generic 8bits bloc address write to memory (HRM 1.4.4.2.4)
 *
 * @note        If the number of bytes to be transmitted exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the writting operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pntrID          SPI pointer ID to use as base address for writing 
 *                              the block of data.
 *
 * @param [in]  AddrIn          Address location at which the SPI write should 
 *                              start.
 *
 * @param [in]  num_xfrs        Number of byte write to be performed.
 *
 * @param [in]  pDataIn         Pointer to the data block to be transmitted.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_wr_byte_b_a(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   pntrID,
    uint32_t                            AddrIn,
    uint32_t                            num_xfrs,
    uint8_t *                           pDataIn
)
{    
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;

    /* Transmit SPI command - use 2bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 2) = ADF703x_SPI_MEM_WRITE | 
                          ADF703x_SPI_MEM_BLOCK |
                          ADF703x_SPI_MEM_ADDRESS |
                          ADF703x_SPI_MEM_SHORT |
                          pntrID;
    
    /* Set Block offset from pntrID */
    *(pSPI_TX_BUFF + 3) = (uint8_t)AddrIn;

    /* Get nbBytes transfer */
    uint32_t nbBytes = num_xfrs;
    
    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 2) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif

    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     2,
                     pntrID,
                     AddrIn,
                     nbBytes,
                     pDataIn,
                     NULL));
}


/**
 * @brief       Read & Compare a number of bytes(s) from memory of the adf7030-1 to Host
 *              starting at location pointed by "pntr" + AddrIn offset
 *              Generic 8bits bloc address read from memory (HRM 1.4.4.2.4)
 *
 * @note        If the number of bytes to be received exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the writting operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pntrID          SPI pointer ID to use as base address for reading 
 *                              the block of data.
 *
 * @param [in]  AddrIn          Address location at which the SPI read should 
 *                              start.
 *
 * @param [in]  num_xfrs        Number of bytes read to be performed.
 *
 * @param [out]  pDataOut       Pointer to the data block to be transmitted.
 *
 * @param [in]  pDataRef        Pointer to the start of the Ref Host data block.
 *                              If set, the SPI readback data will be compared with the 
 *                              data pointed by pDataRef. This can be used to compare
 *                              data between Host and PHY directly within the readback.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_rd_cmp_byte_b_a(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   pntrID,
    uint32_t                            AddrIn,
    uint32_t                            num_xfrs,
    uint8_t *                           pDataOut,
    uint8_t *                           pDataRef
)
{    
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;

    /* Transmit SPI command - use 1bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 1) =  ADF703x_SPI_MEM_READ | 
                    ADF703x_SPI_MEM_BLOCK |
                    ADF703x_SPI_MEM_ADDRESS |
                    ADF703x_SPI_MEM_SHORT |
                    pntrID;
    
    /* Set Block offset from pntrID */
    *(pSPI_TX_BUFF + 2) = (uint8_t)AddrIn;
    
    /* Add 8bits of NOPs in spi_txbuf */
    *(pSPI_TX_BUFF + 3) = 0xFF;
    
    /* Get nbBytes transfer */
    uint32_t nbBytes = num_xfrs;
    
    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 3) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif

    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     1,
                     pntrID,
                     AddrIn,
                     nbBytes,
                     pDataOut,
                     pDataRef));
}

/**
 * @brief       Write a number of byte(s) from Host to memory of the adf7030-1
 *              starting at location pointed by "pntr".
 *              Generic 8bits bloc pointer write to memory (HRM 1.4.4.2.6/10)
 *              This Function should be used when writing a large block of 
 *              8bits word to the radio (above ADI_SPI_MEMORY_SIZE).
 *
 * @note        If the number of bytes to be transmitted exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the writting operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pntrID          SPI pointer ID to use as base address for reading 
 *                              the block of data.
 *
 * @param [in]  num_xfrs        Number of byte write to be performed.
 *
 * @param [in]  pDataIn         Pointer to the data block to be transmitted.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_wr_byte_p_a(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   pntrID,
    uint32_t                            num_xfrs,
    uint8_t *                           pDataIn
)
{  
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;

    /* Transmit SPI command - use 3bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 3) = ADF703x_SPI_MEM_WRITE |
                          ADF703x_SPI_MEM_BLOCK |
                          ADF703x_SPI_MEM_POINTER |
                          ADF703x_SPI_MEM_SHORT |
                          pntrID;

    /* Get nbBytes transfer */
    uint32_t nbBytes = num_xfrs;
    
    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 1) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif

    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     3,
                     pntrID,
                     0,
                     nbBytes,
                     pDataIn,
                     NULL));
}


/**
 * @brief       Read & Compare a number of bytes(s) from memory of the adf7030-1 to Host
 *              starting at location pointed by "pntr"
 *              Generic 8bits bloc pointer read from memory (HRM 1.4.4.2.6/10)
 *              This Function should be used when writing a large block of 
 *              unaligned data to the radio (above ADI_SPI_MEMORY_SIZE).
 *
 * @note        If the number of bytes to be received exceeds the maximum SPI buffer 
 *              size allocated, the spi transfert is automatically cut
 *              into smaller sub spi transaction until the writting operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pntrID          SPI pointer ID to use as base address for reading 
 *                              the block of data.
 *
 * @param [in]  num_xfrs        Number of byte write to be performed.
 *
 * @param [out]  pDataOut       Pointer to the data block to be transmitted.
 *
 * @param [in]  pDataRef        Pointer to the start of the Ref Host data block.
 *                              If set, the SPI readback data will be compared with the 
 *                              data pointed by pDataRef. This can be used to compare
 *                              data between Host and PHY directly within the readback.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_rd_cmp_byte_p_a(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   pntrID,
    uint32_t                            num_xfrs,
    uint8_t *                           pDataOut,
    uint8_t *                           pDataRef
)
{   
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Always Clear the first 32bits of pSPI_TX_BUFF */
    *(uint32_t *)pSPI_TX_BUFF = 0;

    /* Transmit SPI command - use 2bytes offset to keep rest of data alligned on word boundary */
    *(pSPI_TX_BUFF + 2) = ADF703x_SPI_MEM_READ |
                          ADF703x_SPI_MEM_BLOCK |
                          ADF703x_SPI_MEM_POINTER |
                          ADF703x_SPI_MEM_SHORT |
                          pntrID;
    
    /* Add 8bits of NOPs in spi_txbuf */
    *(pSPI_TX_BUFF + 3) = 0xFF;

    /* Get nbBytes transfer */
    uint32_t nbBytes = num_xfrs;
    
    /* Setup default SPI transfer function */
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = ((nbBytes + 2) > ADF7030_1_SPI_FAST_SIZE_THRESHOLD) ? 
                                          &adf7030_1__SPI_Block_Xfer :
                                          &adf7030_1__SPI_Block_Xfer__fast;
#else
    ADI_ADF7030_1_SPI_Xfer SPI_Xfer = &adf7030_1__SPI_Block_Xfer;
#endif

    /* Call SPI transfer function */
    return(SPI_Xfer( pSPIDevInfo,
                     2,
                     pntrID,
                     0,
                     nbBytes,
                     pDataOut,
                     pDataRef));        
}


//(optional) static uint32_t adf7030_1__SPI_wr_word_r_a(ADI_SPI_HANDLE hSPIDevice, uint32_t * pAddrIn, uint32_t * pDataIn, uint32_t num_xfrs);			// (HRM 1.4.4.2.7)
//(optional) static uint32_t adf7030_1__SPI_rd_word_r_a(ADI_SPI_HANDLE hSPIDevice, uint32_t * pAddrIn, uint32_t * pDataOut, uint32_t num_xfrs);			// (HRM 1.4.4.2.7)
//(optional) static uint32_t adf7030_1__SPI_wr_byte_r_a(ADI_SPI_HANDLE hSPIDevice, uint32_t pID, uint8_t * pOffset, uint8_t * pDataIn, uint32_t num_xfrs);	// (HRM 1.4.4.2.8)
//(optional) static uint32_t adf7030_1__SPI_rd_byte_r_a(ADI_SPI_HANDLE hSPIDevice, uint32_t pID, uint8_t * pOffset, uint8_t * pDataOut, uint32_t num_xfrs);	// (HRM 1.4.4.2.8)

/**
 * @brief       Generic function to read or write a number of bytes between
 *              the Host and the PHY adf703x.
 *
 * @note        This function is a faster implementation of adf7030_1__SPI_Block_Xfer()
 *              automatically call if "nbBytes" is less than the defined threshold
 *              ADF7030_1_SPI_FAST_SIZE_THRESHOLD. This function bypass the default 
 *              API to initiate transfert via the SPI from the BSP and instead takes
 *              direct control of the SPI hardware.
 *              
 *              
 * @param [in]  hSPIDevice      Handle to Glue SPI peripheral device used to 
 *                              communicate with the adf7030-1.
 *
 * @param [in]  cmdOffset       Location within the spitx_buff where the spi
 *                              command byte is located.
 *
 * @param [in]  PNTR_ID         SPI ID pointer to use as base address for writing 
 *                              the block of data.
 *
 * @param [in]  AddrIn          Address location at which the SPI write should 
 *                              start transfert in the PHY.
 *
 * @param [in]  nbBytes         Number of bytes to read or write to be performed.
 *
 * @param [in|out]  pDataIO     Pointer to the start of the Host data block.
 *                              On exit, pDataIO is pointing nbBytes away from
 *                              its original value.
 *
 * @param [in]  pDataRef        Pointer to the start of the Ref Host data block.
 *                              If set, the SPI readback data will be compared with the 
 *                              data pointed by pDataRef. This can be used to compare
 *                              data between Host and PHY directly within the readback.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_DEV_FAILED     If SPI interface configuration failed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_Block_Xfer__fast(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    uint32_t                            cmdOffset,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   PNTR_ID,
    uint32_t                            Addr,
    uint32_t                            nbBytes,
    void *                              pDataIO,
    void *                              pDataRef
)
{     
    /* Set the default SPI clock rate */
    if(adf7030_1__SPI_SetSpeed(pSPIDevInfo, FAST_SPI_RATE) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }

    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;
    
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_RX_BUFF = pSPIDevInfo->pSPI_RX_BUFF;
    
    /* Check if we are in Address mode */
    uint8_t Addr_en = ((*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_ADDRESS) >> 4);
    
    /* Check byte memory mode */
    uint8_t Word_mode = ((*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_LONG) >> 3);
        
    /* ------------ Write SPI TX buffer ------------- */
    
    //Handles byte transfert
    if(Word_mode == 0) Addr_en = 0;
    
    /* Get pointer to start of SPI data */
    uint32_t * pData = (((uint32_t *)pSPI_TX_BUFF) + 1 + Addr_en);
    
    /* Fill SPI TX buffer data */ 
    adf7030_1__SPI_Xfer_WriteBuff((void *)pData,
                                  pDataIO,
                                  nbBytes,
                                  Word_mode,
                                  (*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_READ));
    
    /* ------------ SPI transaction ------------- */
    
    /* Calculate the total SPI xfer lenght */
    uint32_t txlen = (4 - cmdOffset) + nbBytes + (Addr_en << 2);
    
    adf7030_1__SPI_ReadWrite_Fast( pSPIDevInfo,
                                  pSPI_TX_BUFF + cmdOffset,
                                  pSPI_RX_BUFF + cmdOffset,
                                  txlen );
    
    /* ------------ Readback SPI RX buffer ------------- */
 
    /* Set Block address to 2nd unit32_t */   
    pData = (((uint32_t *)pSPI_RX_BUFF) + 1 + Addr_en);
    
    if(*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_READ)
    {
    /* READ operation */
                 
        /* Return SPI status */
        pSPIDevInfo->nStatus.VALUE = *((uint8_t *)pData - 1);

        /* Readback SPI RX buffer data */
        if(adf7030_1__SPI_Xfer_ReadBuff(pDataIO,
                                        (void *)pData,
                                        pDataRef,
                                        nbBytes,
                                        Word_mode) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_COMM_FAILED;
        }
    }
    else
    {
    /* WRITE operation */
      
        /* Return SPI status */
        pSPIDevInfo->nStatus.VALUE = *((uint8_t *)pData + (nbBytes - 1)); 
    }
    
    return ADI_ADF7030_1_SUCCESS;
}

/**
 * @brief       Generic function to read or write a number of bytes between
 *              the Host and the PHY adf703x.
 *
 * @note        If the number of bytes to be transfered exceeds the maximum SPI  
 *              buffer size allocated, the spi transfert is automatically cut
 *              down into smaller sub spi transaction until the transfer operation 
 *              is completed. No buffer owerflow or boundary check is done.
 *              
 *              
 * @param [in]  hSPIDevice      Handle to Glue SPI peripheral device used to 
 *                              communicate with the adf7030-1.
 *
 * @param [in]  cmdOffset       Location within the spitx_buff where the spi
 *                              command byte is located.
 *
 * @param [in]  PNTR_ID         SPI pointer ID to use as base address for writing 
 *                              the block of data.
 *
 * @param [in]  AddrIn          Address location at which the SPI write should 
 *                              start transfert in the PHY.
 *
 * @param [in]  nbBytes         Number of bytes to read or write to be performed.
 *                              If nbBytes is bigger than ADI_SPI_MEMORY_SIZE, 
 *                              the custom spi pntr0 will be used to transfert the data.
 *                              The shadow pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR]
 *                              and the PHY value (in the radio) will be restored 
 *                              to there original value.
 *
 * @param [in|out]  pDataIO     Pointer to the start of the Host data block.
 *                              On exit, pDataIO is pointing nbBytes away from
 *                              its original value.
 *
 * @param [in]  pDataRef        Pointer to the start of the Ref Host data block.
 *                              If set, the SPI readback data will be compared with the 
 *                              data pointed by pDataRef. This can be used to compare
 *                              data between Host and PHY directly within the readback.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_DEV_FAILED     If SPI interface configuration failed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_Block_Xfer(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    uint32_t                            cmdOffset,
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE   PNTR_ID,
    uint32_t                            Addr,
    uint32_t                            nbBytes,
    void *                              pDataIO,
    void *                              pDataRef
)
{       
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;

    /* Variable holding current value off full spi block (used when nbBytes > ADF7030_1_SPI_MAX_TRX_SIZE )*/
    uint32_t full_block_cnt = 0;

    /* Store the initial value of PNTR_CUSTOM0 */
    uint32_t copy_spi_pntr_custom0 = pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR];
    uint32_t copy_spi_pntr_ID = pSPIDevInfo->PHY_PNTR[PNTR_ID];

    /* Transfert loop */
    bool_t spi_transaction_finished = (nbBytes) ? false : true;

    while(spi_transaction_finished == false)
    {  
        uint32_t size;
      
        if( nbBytes > ADF7030_1_SPI_MAX_TRX_SIZE )
        {
            /* Full block transaction */
            size = ADF7030_1_SPI_MAX_TRX_SIZE;
        }
        else
        {
            /* Partial block transaction */
            size = nbBytes;
            spi_transaction_finished = true; 
        }
                
        /* Initiate the SPI word transfer */
        if(adf7030_1__SPI_block_transaction( pSPIDevInfo,               //SPI handle
                                             cmdOffset,                 //SPI trx buffer offset
                                             size,                      //SPI bytes to transfert
                                             pDataIO,                   //pointer to Host data to transfert
                                             pDataRef                   //pointer to Host Ref data for comparison
                                            ) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_COMM_FAILED;
        }
        
        /* Increment pntrID by ADF7030_1_SPI_MAX_TRX_SIZE for next SPI transaction */
        if(spi_transaction_finished == false)
        {
            uint32_t tmp_SPI_Prolog;
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
            uint32_t tmp_nClkFreq_Fast;
#endif 
            switch(*(pSPI_TX_BUFF + cmdOffset) & 0x78)
            {
                case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK |  ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_LONG ) :
                    //ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK |  ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_LONG
              
                    /* Change the next Address in the spi_txbuf */
                    Addr += size;
                
                    /* Write the starting address in the spi_txbuf */
                    *(((uint32_t *)pSPI_TX_BUFF) + 1) = __REV(Addr);
                    
                    break;

                case( ADF703x_SPI_MEM_READ | ADF703x_SPI_MEM_BLOCK |  ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_LONG ) :
                    //ADF703x_SPI_MEM_READ | ADF703x_SPI_MEM_BLOCK |  ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_LONG  
              
                    /* Change the next Address in the spi_txbuf */
                    Addr += size;
                
                    /* Write the starting address in the spi_txbuf */
                    *((uint16_t *)pSPI_TX_BUFF + 1) = __REV16(*((uint16_t *)&Addr + 1));
                    *((uint16_t *)pSPI_TX_BUFF + 2) = __REV16(*((uint16_t *)&Addr + 0));
                    
                    break;
              
                case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_LONG ) :
                case( ADF703x_SPI_MEM_READ  | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_LONG ) :
                case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_SHORT ) :
                case( ADF703x_SPI_MEM_READ  | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_SHORT ) :
                case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_SHORT ) :
                case( ADF703x_SPI_MEM_READ  | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_SHORT ) :
          
                    //ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_LONG  | pntrID
                    //ADF703x_SPI_MEM_READ  | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_LONG  | pntrID              
                    //ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_SHORT | pntrID;
                    //ADF703x_SPI_MEM_READ  | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_SHORT | pntrID;                             
                    //ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_SHORT | pntrID;
                    //ADF703x_SPI_MEM_READ  | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_SHORT | pntrID;
          
                    /* Change the Host copy of SPI pointer 0 address for next block of byte transfer */
                    pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] = copy_spi_pntr_ID + (size * ++full_block_cnt);
            
                    /* Save current SPI prolog */
                    tmp_SPI_Prolog = *(uint32_t *)pSPI_TX_BUFF;
                    
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
                    /* Override pSPIDevInfo->nClkFreq_Fast setting to avoid toggling between SPI freq during long block transfer */
                    tmp_nClkFreq_Fast = pSPIDevInfo->nClkFreq_Fast;
                    pSPIDevInfo->nClkFreq_Fast = pSPIDevInfo->nClkFreq_Current;
#endif                    
                    /* Update the PHY Radio SPI pointer 0 */
                    adf7030_1__SPI_wr_word_b_p( pSPIDevInfo, PNTR_SETUP_ADDR, 1, &(pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR]));

#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)   
                    /* Recall original pSPIDevInfo->nClkFreq_Fast setting */
                    pSPIDevInfo->nClkFreq_Fast = tmp_nClkFreq_Fast;
#endif
                    /* Recall current SPI prolog */
                    *(uint32_t *)pSPI_TX_BUFF = tmp_SPI_Prolog;
            
                    /* Update SPI command byte to use the SPI pointer 0 */
                    *(pSPI_TX_BUFF + cmdOffset) &= 0xF8;
                    *(pSPI_TX_BUFF + cmdOffset) |= PNTR_CUSTOM0_ADDR;
                    
                    break;
                
                default : return ADI_ADF7030_1_SPI_COMM_FAILED;
            }
        }
        

        /* Increment pDataIO by size of the previous transaction */        
        if(pDataIO != NULL)
        {
            pDataIO = (uint8_t *)pDataIO + size;   
        }
        
        /* Increment the pDataRef by size of the previous transaction */ 
        if(pDataRef != NULL)
        {
            pDataRef = (uint8_t *)pDataRef + size;
        }
        
        /* Decrement the remaining nbBytes by the size of the previous transaction*/
        nbBytes -= size;
    }

    /* Restore the initial value of PNTR_CUSTOM0 if required */
    if(pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] != copy_spi_pntr_custom0)
    {
        /* Change the Host copy of SPI pointer 0 */
        pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] = copy_spi_pntr_custom0;
        
        /* Update the PHY Radio SPI pointer 0 */
        adf7030_1__SPI_wr_word_b_p( pSPIDevInfo, PNTR_SETUP_ADDR, 1, &(pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR]));
    }
        
    return ADI_ADF7030_1_SUCCESS;    
}



/**
 * @brief       Generic SPI Block Read/Write a number of words(s) to the adf7030-1
 *                          
 * @note        Blocking SPI transfert without DMA. 
 *                          
 * @param [in]  hSPIDevice      Handle to Glue SPI peripheral device used to 
 *                              communicate with the adf7030-1.
 *
 * @param [in]  cmdOffset       Byte location from where the spi frame starts in the spi_txbuf.
 *
 * @param [in]  nbBytes         Number of Bytes transfert.
 *
 * @param [in,out]  pDataIO     In TX, data is read from address pointed by pDataIO.
 *                              In RX, data is written back at address pointed by pDataIO.
 *
 * @param [in]  pDataRef        Pointer to the start of the Ref Host data block.
 *                              If set, the SPI readback data will be compared with the 
 *                              data pointed by pDataRef. This can be used to compare
 *                              data between Host and PHY directly within the readback.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_block_transaction(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    uint32_t                            cmdOffset,
    uint32_t                            nbBytes,
    void *                              pDataIO,
    void *                              pDataRef
)
{
#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)
    /* Set the default SPI clock rate */
    if(adf7030_1__SPI_SetSpeed(pSPIDevInfo, DEFAULT_SPI_RATE) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
#endif
    
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_TX_BUFF = pSPIDevInfo->pSPI_TX_BUFF;
    
    /* Setup SPI tx buffer address */
    uint8_t * pSPI_RX_BUFF = pSPIDevInfo->pSPI_RX_BUFF;
    
    /* Check if we are in Address mode */
    uint8_t Addr_en = ((*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_ADDRESS) >> 4);
    
    /* Check byte memory mode */
    uint8_t Word_mode = ((*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_LONG) >> 3);
       
    /* ------------ Write SPI TX buffer ------------- */
    
    //Handles byte transfert
    if(Word_mode == 0) Addr_en = 0;
    
    /* Get pointer to start of SPI data */
    uint32_t * pData = (((uint32_t *)pSPI_TX_BUFF) + 1 + Addr_en);

    /* Fill SPI TX buffer data */ 
    adf7030_1__SPI_Xfer_WriteBuff((void *)pData,
                                  pDataIO,
                                  nbBytes,
                                  Word_mode,
                                  (*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_READ));
    
    /* ------------ SPI transaction ------------- */
    
    /* Calculate the total SPI xfer lenght */
    uint32_t txlen = (4 - cmdOffset) + nbBytes + (Addr_en << 2);
    
    /* SPI transceiver instance */
    ADI_SPI_TRANSCEIVER Transceiver;

    /* Initialize the transceiver */
    Transceiver.TransmitterBytes =      txlen;
    Transceiver.ReceiverBytes    =      txlen;
    Transceiver.pTransmitter     =      pSPI_TX_BUFF + cmdOffset;    
    Transceiver.pReceiver        =      pSPI_RX_BUFF + cmdOffset;    
    Transceiver.nTxIncrement     =      1u;
    Transceiver.nRxIncrement     =      1u;

#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
    /* Transmit the sequence */
    if(adi_spi_ReadWrite(pSPIDevInfo->hSPIDevice, &Transceiver) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_COMM_FAILED;
    }
#elif defined (__ADUCM4050__)
    /* Enable DMA transaction if TX and RX buffer is aligned on 16bit boundary
       Note that the transfer is still blocking since we are using the API
       adi_spi_MasterReadWrite function after however this should bring some
       extra dynamic power saving */
//    if((!(Transceiver.pTransmitter + Transceiver.TransmitterBytes) & 1) &&
//        !(Transceiver.pReceiver + Transceiver.ReceiverBytes) & 1)))
//    {
//        Transceiver.bDMA = true;
//    }
//    else
//    {
        Transceiver.bDMA = false;
//    }
      
    Transceiver.bRD_CTL = false;
    
    if(adi_spi_MasterReadWrite(pSPIDevInfo->hSPIDevice, &Transceiver) != ADI_SPI_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_COMM_FAILED;
    }
#endif
    
    /* ------------ Readback SPI RX buffer ------------- */
 
    /* Set Block address to 2nd unit32_t */   
    pData = (((uint32_t *)pSPI_RX_BUFF) + 1 + Addr_en);
    
    if(*(pSPI_TX_BUFF + cmdOffset) & ADF703x_SPI_MEM_READ)
    {
    /* READ operation */
                 
        /* Return SPI status */
        pSPIDevInfo->nStatus.VALUE = *((uint8_t *)pData - 1);

        /* Readback SPI RX buffer data */
        if(adf7030_1__SPI_Xfer_ReadBuff(pDataIO,
                                        (void *)pData,
                                        pDataRef,
                                        nbBytes,
                                        Word_mode) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_COMM_FAILED;
        }
    }
    else
    {
    /* WRITE operation */
      
        /* Return SPI status */
        pSPIDevInfo->nStatus.VALUE = *((uint8_t *)pData + (nbBytes - 1)); 
    }
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Custom FAST API for Read/Write a number of byte(s) to the adf7030-1
 *                          
 * @note        Blocking SPI transfert without DMA nor IRQ, just uses FIFO status polling. 
 *                           
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pTX_DATA        Pointer to data to be transmitted.
 *
 * @param [out]  pRX_DATA       Pointer where readback data is to be received.
 *
 * @param [in]  nSize           Number of bytes to be transfered.
 *
 * @return      None
 */
                    
void adf7030_1__SPI_ReadWrite_Fast(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint8_t *                   pTX_DATA,
    uint8_t *                   pRX_DATA,
    uint32_t                    nSize
)
{ 
/* ------------ SPI transaction ------------- */
  /* Handle to SPI device */
  ADI_SPI_HANDLE hSPIDevice = pSPIDevInfo->hSPIDevice;
  
  /* Setup pointer SPI hw */
  volatile ADI_SPI_TypeDef * pSpi = hSPIDevice->pSpi;
  
  /* Disable SPIx_EVT_IRQn */
  NVIC_DisableIRQ(hSPIDevice->pDevInfo->eIRQn);

  /* Set ChipSelect */
  pSpi->CS_CTL  = pSPIDevInfo->eChipSelect;

  /* Flush any residual data in SPI FIFOs */
  pSpi->CTL |= (uint16_t)(BITM_SPI_CTL_MASEN | BITM_SPI_CTL_RFLUSH | BITM_SPI_CTL_TFLUSH);
  pSpi->CTL &= (uint16_t)~(BITM_SPI_CTL_MASEN | BITM_SPI_CTL_RFLUSH | BITM_SPI_CTL_TFLUSH);
  
  /* Clear any residual Status Flags */
  pSpi->STAT = pSpi->STAT;
  
  pSpi->RD_CTL = 0;
  
  /* Flush TX and RX FIFO */
  pSpi->CTL |= ( BITM_SPI_CTL_MASEN |
                 BITM_SPI_CTL_CON |
                 BITM_SPI_CTL_TIM |
                 BITM_SPI_CTL_SPIEN);
  
  /* Set the total continuous transaction lenght */
  pSpi->CNT = nSize;
                  
  uint32_t nByte_tx = nSize;
  uint32_t nByte_rx = nByte_tx;
   
  /* Loop until all TX bytes have been transmitted and received */
  while(nByte_rx)
  {
    /* Get SPI FIFOs status */
    uint16_t nFIFO_STAT = pSpi->FIFO_STAT;
    
    /* If Tx FIFO not full and we have some bytes left to send */
    if((!(nFIFO_STAT & BITM_SPI_FIFO_STAT_TX & 8)) && nByte_tx)
    {
        nByte_tx--;
        pSpi->TX = *pTX_DATA++;
    }
    
    /* If RX FIFO not empty */
    if(nFIFO_STAT & BITM_SPI_FIFO_STAT_RX)
    {
        *pRX_DATA++ = pSpi->RX;
        nByte_rx--;
    }
  }
    
  /* Clear XFRDONE status flag */
  pSpi->STAT = BITM_SPI_STAT_XFRDONE;
  
  /* Re-enable SPIx_EVT_IRQn */
  NVIC_EnableIRQ(hSPIDevice->pDevInfo->eIRQn);
}


/**
 * @brief       Copy number of byte(s) from Host memory space into SPI TX buffer
 *                          
 * @note        This function handles both words and bytes transfers for Read and Write
 *              SPI transfers.
 *                           
 * @param [in]  pDest     Pointer to the SPI API TX buffer location where the 
 *                        data pointed by pSrc will be copied.
 *
 * @param [in]  pSrc      Pointer to the data that should be copied.
 *
 * @param [in]  nBytes    Number of byte to copy
 *
 * @param [in]  mode      -0 for SPI byte (8bits) operation.
 *                        -1 for SPI word (32bits) operation.
 *
 * @param [in]  op        -0 for SPI Write operation.
 *                        -Else SPI Read operation.
 *
 * @return      None
 */

void adf7030_1__SPI_Xfer_WriteBuff(
    void *      pDest,
    void *      pSrc,
    uint32_t    nBytes,
    uint32_t    mode,
    uint32_t    op
)
{
    uint32_t nb_word = nBytes >> 2;
    uint32_t nb_bytes_rem = nBytes & 3;

    /* Fill SPI TX buffer data */
    if(op)
    {
    /* READ operation */
        uint32_t tx_fill = 0xFFFFFFFF;
        for(uint32_t i = 0; i < nb_word; i++)
        {   
            *((uint32_t *)pDest + i) = tx_fill;
        }
             
        if(!(mode & 1) && nb_bytes_rem)
        {
            pDest = (uint32_t *)pDest + nb_word;
            for(uint32_t i = 0; i < nb_bytes_rem; i++)
            {
                *((uint8_t *)pDest + i) = tx_fill;
            }
        }
    }
    else
    {
    /* WRITE operation */
        for(uint32_t i = 0; i < nb_word; i++)
        {   
            uint32_t word = *((uint32_t *)pSrc + i);
            if(mode & 1) word = __REV(word);
            *((uint32_t *)pDest + i) = word;
        }
        
        if(!(mode & 1) && nb_bytes_rem)
        {
            pDest = (uint32_t *)pDest + nb_word;
            pSrc = (uint32_t *)pSrc + nb_word;
            for(uint32_t i = 0; i < nb_bytes_rem; i++)
            {
                *((uint8_t *)pDest + i) = *((uint8_t *)pSrc + i);
            }
        }
    }
}


/**
 * @brief       Copy number of byte(s) from SPI RX buffer into Host memory space. 
 *                          
 * @note        This function handles both words and bytes SPI transfers.
 *                           
 * @param [in]  pDest     Pointer to the Host memory location where data from 
 *                        pSrc (SPI API RX buffer) should be copied.
 *
 * @param [in]  pSrc      Pointer to the SPI API RX buffer location where the 
 *                        data pointed by pDest will be copied.
 *
 * @param [in]  pRef      Pointer to the start of the Ref Host data block.
 *                        If set, the SPI readback pointed by pSrc is compared 
 *                        with the data pointed by pRef. This can be used to compare
 *                        data between Host and PHY directly within the readback.
 *
 * @param [in]  nBytes    Number of byte to copy
 *
 * @param [in]  mode      -0 for SPI byte (8bits) operation.
 *                        -1 for SPI word (32bits) operation.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SPI_Xfer_ReadBuff(
    void *      pDest,
    void *      pSrc,
    void *      pRef,
    uint32_t    nBytes,
    uint32_t    mode
)
{
    uint32_t nb_word = nBytes >> 2;
    uint32_t nb_bytes_rem = nBytes & 3;
    
    if((pDest != NULL) && (pRef == NULL))
    {
        /* Copy nb_word from spi_rxbuf to pDataIO */  
        for(uint32_t i = 0; i < nb_word; i++)
        {
            uint32_t word = *((uint32_t *)pSrc + i);
            if(mode & 1) word = __REV(word);
            *((uint32_t *)pDest + i) = word;
        }
        
        if(!(mode & 1) && nb_bytes_rem)
        {           
            pDest = (uint32_t *)pDest + nb_word;
            pSrc = (uint32_t *)pSrc + nb_word;
            for(uint32_t i = 0; i < nb_bytes_rem; i++)
            {
                *((uint8_t *)pDest + i) = *((uint8_t *)pSrc + i);
            }
        }   
    }
    else if((pDest != NULL) && (pRef != NULL))
    {
        /* Copy up to nb_word from spi_rxbuf to pDataIO and Check versus pDataRef */
        for(uint32_t i = 0; i < nb_word; i++)
        {
            uint32_t word = *((uint32_t *)pSrc + i);
            if(mode & 1) word = __REV(word);
            if(*((uint32_t *)pRef + i) != word)
            {
                return ADI_ADF7030_1_SPI_COMM_FAILED;
            }  
            *((uint32_t *)pDest + i) = word;
        }
        
        if(!(mode & 1) && nb_bytes_rem)
        {
            pDest = (uint32_t *)pDest + nb_word;
            pSrc = (uint32_t *)pSrc + nb_word;
            pRef = (uint32_t *)pRef + nb_word;
            for(uint32_t i = 0; i < nb_bytes_rem; i++)
            {
                uint32_t byte = *((uint8_t *)pSrc + i);
                if(*((uint8_t *)pRef + i) != byte)
                {
                    return ADI_ADF7030_1_SPI_COMM_FAILED;
                }
                *((uint8_t *)pDest + i) = byte;
            }
        }   
    }
    else if((pDest == NULL) && (pRef != NULL))
    {
        /* Compare up to nb_word of spi_rxbuf versus pDataRef */
        for(uint32_t i = 0; i < nb_word; i++)
        {
            uint32_t word = *((uint32_t *)pSrc + i);
            if(mode & 1) word = __REV(word);
            if(*((uint32_t *)pRef + i) != word)
            {
                return ADI_ADF7030_1_SPI_COMM_FAILED;
            }
        }
        
        if(!(mode & 1) && nb_bytes_rem)
        {
            pSrc = (uint32_t *)pSrc + nb_word;
            pRef = (uint32_t *)pRef + nb_word;
            for(uint32_t i = 0; i < nb_bytes_rem; i++)
            {
                if(*((uint8_t *)pRef + i) != *((uint8_t *)pSrc + i))
                {
                    return ADI_ADF7030_1_SPI_COMM_FAILED;
                }
            }
        }   
    }
    else
    {
        /* Should never end up here */
        return ADI_ADF7030_1_SPI_COMM_FAILED;
    }
    
    return ADI_ADF7030_1_SUCCESS;
}
                    
#endif /* _ADF7030_1__SPI_C_ */

/** @} */ /* End of group adf7030-1__spi SPI Communication Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */
