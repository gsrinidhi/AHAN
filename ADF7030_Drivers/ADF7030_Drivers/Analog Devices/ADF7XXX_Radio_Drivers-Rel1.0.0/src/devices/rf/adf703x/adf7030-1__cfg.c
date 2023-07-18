/*!
 *****************************************************************************
  @file:	adf7030-1__cfg.c
  
  @brief:	This file provide all necessary API function to transfer the PHY Radio
  			using the ADI *.cfg format.
  
  @details:	cfg file are generated by the user settings in the ADF7030-1 design center.
  			The ADF7030-1 configuration file format is used to store radio settings,
  			packet settings, look up tables, and packet payload data generated by the 
  			ADF7030-1 design center.
  			Firmware modules, such as the OfflineCalibration.cfg firmware module,
  			are also supplied as configuration files.
  			For more information, plesase refer to the Software Reference Manual.
  
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

/** \addtogroup adf7030-1__cfg Configuration
 *  @{
 */
#ifndef _ADF7030_1__CFG_C_
/*! \cond PRIVATE */
#define _ADF7030_1__CFG_C_

#include <sys/platform.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>
#include <adf7030-1__spi.h>

#include "adf7030-1__cfg.h"


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

/* SPI Configuration transfer buffer */  
static uint8_t      spi_CfgBuffer[ADF7030_1_SPI_BUFFER_SIZE];


/**
 * @brief       ADI Radio Configuration transfer function
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pCONFIG         Pointer to cfg binary blob
 *
 * @note                        Currently the maximum allowed "Cfg" sequence is 
 *                              limited by ADF7030_1_SPI_BUFFER_SIZE. User must ensure
 *                              that the ADF7030-1 design center is configured correctly
 *                              not to exceed this sequence lenght.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the configuration was written transfered to the Host.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the configuration transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__SendConfiguration(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                              
    uint8_t *                   pCONFIG,
    uint32_t                    Size
)
{

    uint32_t array_position = 0;
    
    do 
    { 
      // Calculate the number of bytes to write
      uint32_t length =  (*(pCONFIG + array_position ) << 16) | 
                         (*(pCONFIG + array_position + 1) << 8) |
                         (*(pCONFIG + array_position + 2));
      
      if(length > 0xFFFF)
      {
         return ADI_ADF7030_1_SPI_COMM_FAILED;
      } 
      
      // Write the SPI data pointed to location (MEMORY_FILE + array_position) with specified length (length)
      uint8_t * pSeqData = (pCONFIG + array_position + 3);
      
      // Transfer the Configuration sequence
      if(adf7030_1__XferCfgSeq( pSPIDevInfo,
                                pSeqData,
                                length - 4) != ADI_ADF7030_1_SUCCESS)
      {
          return ADI_ADF7030_1_SPI_COMM_FAILED;
      }
      
      // Update the array position to point to the next block
      array_position += length;
    
    }while(array_position < Size); // Continue operation until full data file has been written
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       ADI Radio SPI sequence configuration transfer
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pData           Pointer to cfg sub sequence
 *
 * @note                        Currently the maximum allowed "Cfg" sequence is 
 *                              limited by ADF7030_1_SPI_BUFFER_SIZE. Future revision 
 *                              of the driver will implement automatic sequence split.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the configuration was written transfered to the Host.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the configuration transfert failed.
 */

ADI_ADF7030_1_RESULT adf7030_1__XferCfgSeq(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint8_t *                   pData,
    uint32_t                    Size
)
{
    // Retrieve Size actual size of sequence
    switch( *pData & 0x78 )
    {
        // 32bits Address Block Write Access of the "Configuration Format"
        case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK |  ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_LONG ) :
                  { 
                    // Move cfg sequence pointer by 1 byte 
                    pData++;
                    
                    // Get the Xfer byte size
                    uint32_t SeqDataLen = (Size - 4);
                    
                    // Get the PHY Radio where to write sequence + change pointer to start of data sequence
                    uint32_t Addr = __REV(*(uint32_t *)pData);
                    
                    // Currently, the RADIO SPI driver is implemented to provide a one-to-one relationship
                    // between memory bloc representation of the PHY Radio and the HOST. (Provided the underlying endianness is the same)
                    // Byte transfer via the adf7030_1 SPI interface is done in natural fashion (byteaddress increment)
                    // However 32bits word transfers are wordaddress increment. Each word must byte reversed (intrinsic __REV() on ARM Cortex does just that)
                    // Current driver SPIs call are already rev-ing words to comply with SPI word transfer protocol
                    
                    // This version of ADI Radio SPI driver does not fully support the recommended *.cfg file format where
                    // rev-ing is already done as part of the sequence
                    
                    // So for now, we just need to unrev data...
                    uint32_t * pUNREV_BUFF = (uint32_t *)spi_CfgBuffer;
                    uint32_t * pSEQ_BUFF = (uint32_t *)pData;
                    
                    pSEQ_BUFF++;
                    
                    for(uint32_t i = 0; i < (SeqDataLen >> 2); i++)
                    {
                        *pUNREV_BUFF++ = __REV(*pSEQ_BUFF++);
                    }
                    
                    // Transfert data to using the existing RADIO SPI driver
                    if(adf7030_1__SPI_wr_word_b_a( pSPIDevInfo,
                                                   Addr,
                                                   SeqDataLen >> 2,
                                                   (uint32_t *)spi_CfgBuffer) != ADI_ADF7030_1_SUCCESS)
                    {
                        return ADI_ADF7030_1_SPI_COMM_FAILED;
                    }
                    break;
                  }
        
        // 32bits Pointer Block Write Access of the "Configuration Format"        
        case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_LONG ) :
                  {
                    // Get the SPI Pointer ID from the Cfg Sequence
                    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE pntrID = (ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE)(*pData++ & 0x07);
                    
                    // Get the Xfer byte size
                    uint32_t SeqDataLen = Size;

                    // So for now, we just need to unrev data...
                    uint32_t * pUNREV_BUFF = (uint32_t *)spi_CfgBuffer;
                    uint32_t * pSEQ_BUFF = (uint32_t *)pData;
                    
                    pSEQ_BUFF++;
                    
                    for(uint32_t i = 0; i < (SeqDataLen >> 2); i++)
                    {
                        *pUNREV_BUFF++ = __REV(*pSEQ_BUFF++);
                    }
                    
                    // Transfert data to using the existing RADIO SPI driver
                    if(adf7030_1__SPI_wr_word_b_p( pSPIDevInfo,
                                                   pntrID,
                                                   SeqDataLen >> 2,
                                                   (uint32_t *)spi_CfgBuffer) != ADI_ADF7030_1_SUCCESS)
                    {
                        return ADI_ADF7030_1_SPI_COMM_FAILED;
                    }
            
                    break;
                  }
        
        // 8bits Block Write Access with Offset of the "Configuration Format"   
        case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_ADDRESS | ADF703x_SPI_MEM_SHORT ) :
                  {
                    // Get the SPI Pointer ID from the Cfg Sequence
                    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE pntrID = (ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE)(*pData++ & 0x07);
                    
                    // Get the Address Offset
                    uint8_t Addr = *pData++;
                    
                    // Get the Xfer byte size
                    uint32_t SeqDataLen = (Size - 1);
                    
                    // Transfert data to using the existing RADIO SPI driver
                    if(adf7030_1__SPI_wr_byte_b_a( pSPIDevInfo,
                                                   pntrID,
                                                   (uint32_t)Addr,
                                                   SeqDataLen,
                                                   pData) != ADI_ADF7030_1_SUCCESS)
                    {
                        return ADI_ADF7030_1_SPI_COMM_FAILED;
                    }
                    
                    break;
                  }
                  
        // 8bits Block Write Access of the "Configuration Format"   
        case( ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_SHORT ) :
                  {
                    // Get the SPI Pointer ID from the Cfg Sequence
                    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE pntrID = (ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE)(*pData++ & 0x07);

                    // Get the Xfer byte size
                    uint32_t SeqDataLen = Size;

                    // Transfert data to using the existing RADIO SPI driver
                    if(adf7030_1__SPI_wr_byte_p_a( pSPIDevInfo,
                                                   pntrID,
                                                   SeqDataLen,
                                                   pData) != ADI_ADF7030_1_SUCCESS)
                    {
                        return ADI_ADF7030_1_SPI_COMM_FAILED;
                    }
                    break;
                  }
                    
        default : return ADI_ADF7030_1_SPI_COMM_FAILED;
    }
    return ADI_ADF7030_1_SUCCESS;
}


#endif /* _ADF7030_1__CFG_C_ */

/** @} */ /* End of group adf7030-1__cfg Configuration */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */
