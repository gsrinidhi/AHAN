/*!
 *****************************************************************************
  @file:	adf7030-1_mem.c
  
  @brief:	adf7030-1 Memory Accress Functions.
				- SPI communication with between Host and Radio.
				- Shadow Memory MAP
  
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

/** \addtogroup adf7030-1__mem Memory Interface
 *  @{
 */
#ifndef _ADF7030_1__MEM_C_
/*! \cond PRIVATE */
#define _ADF7030_1__MEM_C_

#include <sys/platform.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>
#include <adf7030-1__spi.h>

#include "adf7030-1__mem.h"

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
 * @brief       Function call to write Configuration to Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pCONFIG         Pointer to Host configuration structure.
 *
 * @note                        Configuration data is provided by ADI RF Setting Calculator.
 *                              @sa USER_CONFIG_20.h for an example.
 *                              This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Configuration was written successfully in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Configuration transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__WriteConfig(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                             
    MEM_CFG_DESCRIPTOR_Type *   pCONFIG                               
)
{
    return(adf7030_1__XferConfig( pSPIDevInfo,
                                  pCONFIG,
                                  PHY_MEM_WRITE,
                                  false));
}


/**
 * @brief       Function call to read Configuration from Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in|out]  pCONFIG     Pointer to Host configuration structure.
 *
 * @note                        Configuration data is provided by ADI RF Setting Calculator.
 *                              @sa USER_CONFIG_20.h for an example.
 *                              This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Configuration was readback successfully in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Configuration transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__ReadConfig(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                             
    MEM_CFG_DESCRIPTOR_Type *   pCONFIG                               
)
{
    return(adf7030_1__XferConfig( pSPIDevInfo,
                                  pCONFIG,
                                  PHY_MEM_READ,
                                  false));
}


/**
 * @brief       Function call to verify the integrity of the patch
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pCONFIG         Pointer to Host configuration structure.
 *
 * @note                        Configuration data is provided by ADI RF Setting Calculator.
 *                              @sa USER_CONFIG_20.h for an example.
 *                              This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Configuration in the PHY matches the reference in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Configuration is different.
 */
ADI_ADF7030_1_RESULT adf7030_1__VerifyConfig(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                              
    MEM_CFG_DESCRIPTOR_Type *   pCONFIG                                
)
{
    return(adf7030_1__XferConfig( pSPIDevInfo,
                                  pCONFIG,
                                  PHY_MEM_READ,
                                  true));
}


/**
 * @brief       Generic Configuration transfers function between Host and the Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pCONFIG         Pointer to Host configuration structure.
 *
 * @param [in]  bREAD_nWRITE    Boolean  -TRUE to write Config from Host to the Radio PHY
 *                                       -FALSE to read Config from Radio PHY back to Host
 *
 * @param [in]  bCHECK_ONLY     Boolean  -TRUE to check PHY Config versus Host Reference
 *                                       -FALSE [D] to perform READ_nWRITE operation
 *
 * @note                        This function can modify pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__XferConfig(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    MEM_CFG_DESCRIPTOR_Type *           pCONFIG,
    bool                                bREAD_nWRITE,
    bool                                bCHECK_ONLY
)
{
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT eResult = ADI_ADF7030_1_SPI_COMM_FAILED;

    bool bEXIT;
    do{
  
        for(uint32_t i = 0; i < pCONFIG->nbBlock; i++)
        {
          if((eResult = adf7030_1__XferDataBlock( pSPIDevInfo,
                                                  &pCONFIG->BLOCKS[i],
                                                  0,
                                                  0,
                                                  bREAD_nWRITE,
                                                  bCHECK_ONLY )) != ADI_ADF7030_1_SUCCESS)
          {
              return eResult;
          }
        }  
    
        /* Check if nested MEM Configuration transfert is present */
        if(pCONFIG->pNext)
        {
            /* Move pointer to MEM structure to the following one */
            pCONFIG = pCONFIG->pNext;
            bEXIT = false;
        }
        else
        {
            bEXIT = true;
        }
    
    }while(bEXIT == false);
    
    return eResult;
}


/**
 * @brief       Function call to write the Data block
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBLOCK          Pointer to Host data block structure.
 *
 * @note                        This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Data block was written successfully in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Data block transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__WriteDataBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                              
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK                               
)
{
    return(adf7030_1__XferDataBlock( pSPIDevInfo,
                                     pBLOCK,
                                     0,
                                     0,
                                     PHY_MEM_WRITE,
                                     false));
}


/**
 * @brief       Function call to read the Data block
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBLOCK          Pointer to Host data block structure.
 *
 * @note                        This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Data block was readback successfully in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Data block transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__ReadDataBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                              
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK                                
)
{
    return(adf7030_1__XferDataBlock( pSPIDevInfo,
                                     pBLOCK,
                                     0,
                                     0,
                                     PHY_MEM_READ,
                                     false));
}


/**
 * @brief       Function call to verify the integrity of Data block
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBLOCK          Pointer to Host data block structure.
 *
 * @note                        This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Configuration in the PHY matches the reference in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Configuration is different.
 */
ADI_ADF7030_1_RESULT adf7030_1__VerifyDataBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK
)
{
    return(adf7030_1__XferDataBlock( pSPIDevInfo,
                                     pBLOCK,
                                     0,
                                     0,
                                     PHY_MEM_READ,
                                     true));
}


/**
 * @brief       Function call to partially write a section of the Host Data block
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBLOCK          Pointer to Host data block structure.
 *
 * @param [in]  Offset          Offset relative to the Host data block structure starting address.
 *
 * @param [in]  Size	        Size in byte to be transfered from Host data block structure.
 *
 * @note                        This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Data block was written successfully in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Data block transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__WritePartialDataBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK,
    uint16_t				Offset,
    uint16_t				Size
)
{
    return(adf7030_1__XferDataBlock( pSPIDevInfo,
                                     pBLOCK,
                                     Offset,
                                     Size,
                                     PHY_MEM_WRITE,
                                     false));
}


/**
 * @brief       Function call to partially read a section of the Host Data block
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBLOCK          Pointer to Host data block structure.
 *
 * @param [in]  Offset          Offset relative to the Host data block structure starting address.
 *
 * @param [in]  Size	        Size in byte to be transfered to Host data block structure.
 *
 * @note                        This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Data block was readback successfully in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Data block transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__ReadPartiaDataBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK,
    uint16_t				Offset,
    uint16_t				Size
)
{
    return(adf7030_1__XferDataBlock( pSPIDevInfo,
                                     pBLOCK,
                                     Offset,
                                     Size,
                                     PHY_MEM_READ,
                                     false));
}


/**
 * @brief       Function call to verify the integrity of a portion of a Host Data block
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBLOCK          Pointer to Host data block structure.
 *
 * @note                        This function modifies SPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Configuration in the PHY matches the reference in the Host.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Configuration is different.
 */
ADI_ADF7030_1_RESULT adf7030_1__VerifyPartialDataBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK,
    uint16_t			        Offset,
    uint16_t			        Size
)
{
    return(adf7030_1__XferDataBlock( pSPIDevInfo,
                                     pBLOCK,
                                     Offset,
                                     Size,
                                     PHY_MEM_READ,
                                     true));
}


/**
 * @brief       Generic Data block transfers function between Host and the Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBLOCK          Pointer to Host configuration structure sub data block.
 *
 * @param [in]  Offset          Offset relative to the Host data block structure starting address.
 *
 * @param [in]  Size	        Size in byte to be transfered from Host data block structure.
 *
 * @param [in]  bREAD_nWRITE    Boolean  -TRUE to write data block from Host to the Radio PHY
 *                                       -FALSE to read data block from Radio PHY back to Host
 *
 * @param [in]  bCHECK_ONLY     Boolean  -TRUE to check PHY data block versus Host Reference
 *                                       -FALSE [D] to perform READ_nWRITE operation
 *
 * @note                        This function can modify pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfer was successful to the adf7030-1.
 *  - #ADI_ADF7030_1_INVALID_OPERATION  If the memory transfer is not allowed
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfer failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__XferDataBlock(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    DATA_BLOCK_DESCRIPTOR_Type *pBLOCK,
    uint16_t			Byte_Offset,
    uint16_t			Byte_Size,
    bool                        bREAD_nWRITE,
    bool                        bCHECK_ONLY
)
{
    /* 8bits Mode SPI transfers mode by default */
    if(pBLOCK->WordXfer == 0)
    {
        /* Get Memory Block Address */
        uint32_t PHYAddr = pBLOCK->Addr + Byte_Offset;

        uint32_t size = (Byte_Size == 0) ? pBLOCK->Size : Byte_Size;

    	ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_id;
        int32_t PNTR_offset;
    
        /* Get good PNTRid from pSPIDevInfo->PHY_PNTR[] */
        adf7030_1__SPI_FindMMapPointer( pSPIDevInfo, PHYAddr, &PNTR_id, &PNTR_offset);
        
        /* Setup pointer to Host start of data */
        uint8_t * pHostMem = (uint8_t *)(pBLOCK->pData + Byte_Offset);
        uint8_t * pHostRef = NULL;
        
        if(bCHECK_ONLY)
        {
            pHostRef = pHostMem;
            pHostMem = NULL;
        }
        
        /* No valid PHY SPI pointer available, use 8bits access with no offset */
        if(PNTR_id == 0)
        {
            /* Change the SPI custom pointer 0 */
            if(adf7030_1__SPI_SetMMapCustomPntr0( pSPIDevInfo, PHYAddr ) != ADI_ADF7030_1_SUCCESS)
            {
                return ADI_ADF7030_1_SPI_COMM_FAILED;
            }
            
            if(bREAD_nWRITE == true)
            {
                /* Read from radio PHY */
                if(adf7030_1__SPI_rd_cmp_byte_p_a( pSPIDevInfo,
                                                   PNTR_CUSTOM0_ADDR,
                                                   size,
                                                   pHostMem,
                                                   pHostRef) != ADI_ADF7030_1_SUCCESS)
                {
                    if(pBLOCK->Volatile)
                    {
                        /* In case block contains volatile data, force return SUCCESS for debug */ 
                        //\todo Return different value
                        return ADI_ADF7030_1_SUCCESS;
                    }
                    else
                    {
                      return ADI_ADF7030_1_SPI_COMM_FAILED;
                    }
                }                    
            }else{
                /* Write to radio PHY */
                if(adf7030_1__SPI_wr_byte_p_a( pSPIDevInfo, PNTR_CUSTOM0_ADDR, size, pHostMem) != ADI_ADF7030_1_SUCCESS)
                {
                    return ADI_ADF7030_1_SPI_COMM_FAILED;
                }  
            }
        }
        /* Valid PHY SPI pointer available, use 8bits access with pointer offset */
        else
        {
            if(bREAD_nWRITE == true)
            {
                /* Read from radio PHY */
                if(adf7030_1__SPI_rd_cmp_byte_b_a( pSPIDevInfo,
                                                   PNTR_id,
                                                   PNTR_offset,
                                                   size,
                                                   pHostMem,
                                                   pHostRef) != ADI_ADF7030_1_SUCCESS)
                {
                    if(pBLOCK->Volatile)
                    {
                        /* In case block contains volatile data, force return SUCCESS for debug */ 
                        //\todo Return different value
                        return ADI_ADF7030_1_SUCCESS;
                    }
                    else
                    {
                      return ADI_ADF7030_1_SPI_COMM_FAILED;
                    }
                }                    
            }else{
                /* Write to radio PHY */
                if(adf7030_1__SPI_wr_byte_b_a( pSPIDevInfo, PNTR_id, PNTR_offset, size, pHostMem) != ADI_ADF7030_1_SUCCESS)
                {
                    return ADI_ADF7030_1_SPI_COMM_FAILED;
                }  
            }
        }
    }
    /* This block only support 32bits Mode SPI transfers, Let us just use block modes */
    else
    {
        /* Intercept non-word transfer error due to Byte_Offset */
    	if(Byte_Offset & 0x3)
        {
        	return ADI_ADF7030_1_INVALID_OPERATION;
        }

    	/* Get Memory Block Address */
        uint32_t PHYAddr = pBLOCK->Addr + Byte_Offset;

        uint32_t size = (Byte_Size == 0) ? (pBLOCK->Size >> 2) : (Byte_Size >> 2);

        /* Setup pointer to Host start of data */
        uint32_t * pHostMem = (uint32_t *)(pBLOCK->pData + Byte_Offset);
        uint32_t * pHostRef = NULL;

        if(bCHECK_ONLY)
        {
            pHostRef = pHostMem;
            pHostMem = NULL;
        }
        
        if(bREAD_nWRITE == true)
        {
            /* Read from radio PHY */
            if(adf7030_1__SPI_rd_cmp_word_b_a( pSPIDevInfo,
                                               PHYAddr,
                                               size,
                                               pHostMem,
                                               pHostRef) != ADI_ADF7030_1_SUCCESS)
            {
                if(pBLOCK->Volatile)
                {
                      /* In case block contains volatile data, force return SUCCESS for debug */ 
                      //\todo Return different value
                      return ADI_ADF7030_1_SUCCESS;
                }
                else
                {
                      return ADI_ADF7030_1_SPI_COMM_FAILED;
                }
            }                    
        }else{
            /* Write to radio PHY */
            if(adf7030_1__SPI_wr_word_b_a( pSPIDevInfo, PHYAddr, size, pHostMem) != ADI_ADF7030_1_SUCCESS)
            {
                return ADI_ADF7030_1_SPI_COMM_FAILED;
            }  
        } 
    }
    
    return ADI_ADF7030_1_SUCCESS;
}

/**
 * @brief       Generic memory transfers function between Host shadow and the Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pMEM            Descriptor to the paired Host/PHY memory region.
 *
 * @param [in]  bREAD_nWRITE    Boolean  -TRUE to write from Host shadow to the Radio PHY
 *                                       -FALSE to read from Radio PHY back to Host shadow
 *
 * @note                        This function assume the following:
 *                              - Both Host shadow block (&(pMEM->pBuff)) and 
 *                              PHY memory blocks (pMEM->PhyAddr) are starting at 
 *                              a word alligned location.
 *                              - pSPIDevInfo->PHY_PNTR[] is in sync with PHY value.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__SPI_Xfer_Mem(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    MEM_DESCRIPTOR_Type *       pMEM,
    bool                        bREAD_nWRITE
)
{
    bool bEXIT;
    do{
  
        if(!pMEM->XferMode)
        {
          /* Block Transfers Mode */
                
            uint32_t PHYAddr = pMEM->PhyAddr + pMEM->BlockXfer.Offset;
            
            ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_id;
            int32_t PNTR_offset;
            
            /* Get good PNTRid from pSPIDevInfo->PHY_PNTR[] */
            adf7030_1__SPI_FindMMapPointer( pSPIDevInfo, PHYAddr, &PNTR_id, &PNTR_offset);
                
            /* 32bits Mode SPI transfers */
            if(!(PHYAddr & 3) && !(pMEM->BlockXfer.Size & 3))
            { 
                /* Setup pointer to Host start of data */
                uint32_t * pHostMem;
                pHostMem = (uint32_t *)pMEM->pBuff + (pMEM->BlockXfer.Offset >> 2);
    
                /* Valid PHY SPI pointer available, use 32bits bloc pointer access */
                if((PNTR_id) && !(PNTR_offset >> 2))
                {     
                    if(bREAD_nWRITE == true)
                    {
                        /* Read from radio PHY */
                        if(adf7030_1__SPI_rd_word_b_p( pSPIDevInfo, PNTR_id, pMEM->BlockXfer.Size >> 2, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }                    
                    }else{
                        /* Write to radio PHY */
                        if(adf7030_1__SPI_wr_word_b_p( pSPIDevInfo, PNTR_id, pMEM->BlockXfer.Size >> 2, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }  
                    }
                }
                /* No valid PHY SPI pointer available, use 32bits bloc address access */
                else
                {
                    if(bREAD_nWRITE == true)
                    {
                        /* Read from radio PHY */
                        if(adf7030_1__SPI_rd_word_b_a( pSPIDevInfo, PHYAddr, pMEM->BlockXfer.Size >> 2, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }                    
                    }else{
                        /* Write to radio PHY */
                        if(adf7030_1__SPI_wr_word_b_a( pSPIDevInfo, PHYAddr, pMEM->BlockXfer.Size >> 2, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }  
                    }
                }
            }
            /* 8bits Mode SPI transfers */
            else
            {
                /* Setup pointer to Host start of data */
                uint8_t * pHostMem;
                pHostMem = (uint8_t *)pMEM->pBuff + pMEM->BlockXfer.Offset;
    
                /* No valid PHY SPI pointer available, use 8bits access with no offset */
                if(PNTR_id == 0)
                {
                    /* Change the SPI custom pointer 0 */
                    if(adf7030_1__SPI_SetMMapCustomPntr0( pSPIDevInfo, PHYAddr ) != ADI_ADF7030_1_SUCCESS)
                    {
                        return ADI_ADF7030_1_SPI_COMM_FAILED;
                    }
                    
                    /* Copy Host MEM memory block to PHY memory block via the SPI */
                    if(bREAD_nWRITE == true)
                    {
                        /* Read from radio PHY */
                        if(adf7030_1__SPI_rd_byte_p_a( pSPIDevInfo, PNTR_CUSTOM0_ADDR, pMEM->BlockXfer.Size, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }                    
                    }else{
                        /* Write to radio PHY */
                        if(adf7030_1__SPI_wr_byte_p_a( pSPIDevInfo, PNTR_CUSTOM0_ADDR, pMEM->BlockXfer.Size, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }  
                    }
                }
                /* Valid PHY SPI pointer available, use 8bits access with pointer offset */
                else
                {
                    /* Copy Host MEM memory block to PHY memory block via the SPI */
                    if(bREAD_nWRITE == true)
                    {
                        /* Read from radio PHY */
                        if(adf7030_1__SPI_rd_byte_b_a( pSPIDevInfo, PNTR_id, PNTR_offset, pMEM->BlockXfer.Size, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }                    
                    }else{
                        /* Write to radio PHY */
                        if(adf7030_1__SPI_wr_byte_b_a( pSPIDevInfo, PNTR_id, PNTR_offset, pMEM->BlockXfer.Size, pHostMem) != ADI_ADF7030_1_SUCCESS)
                        {
                            return ADI_ADF7030_1_SPI_COMM_FAILED;
                        }  
                    }
                }              
            }
        }
        else
        {
          /* Random Transfers Mode */
          
        }
        
        
        /* Check if nested MEM transfert is present */
        if(pMEM->pNextBloc)
        {
            /* Move pointer to MEM structure to the following one */
            pMEM = pMEM->pNextBloc;
            bEXIT = false;
        }
        else
        {
            bEXIT = true;
        } 
    
    }while(bEXIT == false);
  
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Write a single 32bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @param [in]  Value           32bits Value to write.
 *
 * @note                        This function does not support "SPI Error Status"
 *
 * @return      None
 */
void adf7030_1__SPI_SetMem32(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint32_t                    Value
)
{
    adf7030_1__SPI_SetBytes( pSPIDevInfo, Addr, Value, 4, NULL);
}

/**
 * @brief       Read a single 32bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @note                        This function does not support "SPI Error Status"
 *
 * @return      Value           32bits readback value.
 */
uint32_t adf7030_1__SPI_GetMem32(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr
)
{  
    return(adf7030_1__SPI_GetBytes( pSPIDevInfo, Addr, 4, NULL)); 
}

/**
 * @brief       Write a single 24bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @param [in]  Value           24bits Value to write.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      None
 */
void adf7030_1__SPI_SetMem24(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint32_t                    Value
)
{
    adf7030_1__SPI_SetBytes( pSPIDevInfo, Addr, (0xFFFFFF & Value), 3, NULL);
}

/**
 * @brief       Read a 24bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      Value           24bits readback value.
 */
uint32_t adf7030_1__SPI_GetMem24(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr
)
{
    return((0xFFFFFF & adf7030_1__SPI_GetBytes( pSPIDevInfo, Addr, 3, NULL)));
}

/**
 * @brief       Write a single 16bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @param [in]  Value           16bits Value to write.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      None
 */
void adf7030_1__SPI_SetMem16(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint16_t                    Value
)
{
    adf7030_1__SPI_SetBytes( pSPIDevInfo, Addr, Value, 2, NULL);
}

/**
 * @brief       Read a 16bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      Value           16bits readback value.
 */
uint16_t adf7030_1__SPI_GetMem16(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr
)
{
    return((uint16_t)adf7030_1__SPI_GetBytes( pSPIDevInfo, Addr, 2, NULL));
}

/**
 * @brief       Write a single 8bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @param [in]  Value           8bits Value to write.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      None
 */
void adf7030_1__SPI_SetMem8(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint8_t                     Value
)
{
    adf7030_1__SPI_SetBytes( pSPIDevInfo, Addr, Value, 1, NULL);
}

/**
 * @brief       Read a 8bits memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      Value           8bits readback value.
 */
uint8_t adf7030_1__SPI_GetMem8(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr
)
{
    return((uint8_t)adf7030_1__SPI_GetBytes( pSPIDevInfo, Addr, 1, NULL));
}


/**
 * @brief       Writes "nbBytes" memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY (Byte)Address location at which the SPI will write.
 *
 * @param [in]  Value           Value to write.
 *
 * @param [in]  nbBytes         Number of Bytes to writes (1, 2, 3 or 4);
 *
 * @param [in|out]  pRegVal     Pointer to "last full 32bits value" of Addr location
 *                              in the event of Addr not being within byte accessible
 *                              range of the memory map.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      None
 * 
 * @sa  adf7030_1__SPI_GetBytes()
 */
void adf7030_1__SPI_SetBytes(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint32_t                    Value,
    uint32_t                    nbBytes,
    uint32_t *                  pRegVal
)
{   
    /* SPI transfert result */
    ADI_ADF7030_1_RESULT eResult = ADI_ADF7030_1_SUCCESS;
  
    /* Get current SPI custom pntr 0 value */ 
    int32_t AddrDiff = (int32_t)Addr - (int32_t)(pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR]);
    
    uint8_t Offset;
    
    /* Check if byte access is allowed at this address */
    bool bByteAccess = adf7030_1__MEM_CheckByteAccess(Addr);
    bool bInrange = ((AddrDiff >= 0) & (AddrDiff < (255 + nbBytes))) ? true : false;
    
    if((bInrange == true) && (bByteAccess == true))
    {
      /* Here PNTR_CUSTOM0_ADDR is within range, just change the offset */
        Offset = (uint8_t)AddrDiff; 
    }else{
      /* Here PNTR_CUSTOM0_ADDR is not within range, changing it, we shall */
            
        if(((Addr >> 2) << 2) != pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR]) //Fix for double setup of pntr0
        {
            /* Setup the PHY Radio SPI pointer 0 address to Addr */
            eResult = adf7030_1__SPI_SetMMapCustomPntr0( pSPIDevInfo, ((Addr >> 2) << 2));
            
            /* Exit if error */
            if(eResult != ADI_ADF7030_1_SUCCESS)
            {
                pSPIDevInfo->eXferResult = eResult;
                return;
            }
        }
          
        Offset = Addr & 0x03;
    }
    
    if(bByteAccess == false)
    {
        /* Construct the byte mask depending on nbBytes */
        uint32_t msk = (0x80 << ((nbBytes - 1) << 3));
        msk |= ~msk;
        msk <<= (Offset << 3);
        
        uint32_t tmp_reg = 0;
        
        /* Clear masked bytes from pRegVal */
        if(pRegVal)
        {
            tmp_reg = *pRegVal & ~msk;
        }
        
        /* Shift Value by the Offset */
        Value <<= (Offset << 3);
        
        /* Mix value in full reg */
        Value |= tmp_reg;
      
        /* Byte access is not permitted, do word access instead */
        eResult = adf7030_1__SPI_wr_word_b_p( pSPIDevInfo, PNTR_CUSTOM0_ADDR, 1, &Value);

    }
    else
    {
        if((Offset == 0) && (nbBytes == 4))
        {
          /* Write the 32bit alligned data */ 
            eResult = adf7030_1__SPI_wr_word_b_p( pSPIDevInfo, PNTR_CUSTOM0_ADDR, 1, &Value);
        }else{
          /* Write the nbBytes data */
            eResult = adf7030_1__SPI_wr_byte_b_a( pSPIDevInfo, PNTR_CUSTOM0_ADDR, Offset, nbBytes, (uint8_t *)&Value);
        }
    }

    /* Exit if error */
    pSPIDevInfo->eXferResult = eResult;
}

/**
 * @brief       Read "nbBytes" memory location via the SPI
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            PHY Address location at which the SPI will write.
 *
 * @param [in]  nbBytes         Number of Bytes to read (1, 2, 3 or 4);
 *
 * @param [in|out]  pRegVal     Pointer to full 32bits register writeback location
 *                              in the event of Addr not being within byte accessible
 *                              range of the memory map.
 *
 * @note                        This function is changing SPI custom pointer0, 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] and the PHY
 *                              SPI pntr are overwritten. It also does not support
 *                              "SPI Error Status".
 *
 * @return      Value           32bits readback value.
 * 
 * @sa  adf7030_1__SPI_SetBytes()
 */
uint32_t adf7030_1__SPI_GetBytes(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint32_t                    nbBytes,
    uint32_t *                  pRegVal
)
{
    /* SPI transfert result */
    ADI_ADF7030_1_RESULT eResult = ADI_ADF7030_1_SUCCESS;

      /* Get current SPI custom pntr 0 value */ 
    int32_t AddrDiff = (int32_t)Addr - (int32_t)(pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR]);
    
    uint8_t Offset;
    
    /* Check if byte access is allowed at this address */
    bool bByteAccess = adf7030_1__MEM_CheckByteAccess(Addr);
    bool bInrange = ((AddrDiff >= 0) & (AddrDiff < (255 + nbBytes))) ? true : false;
    
    if((bInrange == true) && (bByteAccess == true))
    {
      /* Here PNTR_CUSTOM0_ADDR is within range, just change the offset */
        Offset = (uint8_t)AddrDiff;
      
    }else{
      /* Here PNTR_CUSTOM0_ADDR is not within range, changing it, we shall */
            
        if(((Addr >> 2) << 2) != pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR]) //Fix for double setup of pntr0
        {
            /* Setup the PHY Radio SPI pointer 0 address to Addr */
            eResult = adf7030_1__SPI_SetMMapCustomPntr0( pSPIDevInfo, ((Addr >> 2) << 2));
            
            /* Exit if error */
            if(eResult != ADI_ADF7030_1_SUCCESS)
            {
                pSPIDevInfo->eXferResult = eResult;
                return(0);
            }
        }         
         
        Offset = Addr & 0x03;
    }
    
    uint32_t RetVal;
    
    if(bByteAccess == false)
    {
        /* Byte access is not permitted, do word access instead */
        eResult = adf7030_1__SPI_rd_word_b_p( pSPIDevInfo, PNTR_CUSTOM0_ADDR, 1, &RetVal);
        
        if(pRegVal)
        {
            /* Return the value of full 32bit register*/
            *pRegVal = RetVal;
        }
        
        /* Construct the byte mask depending on nbBytes */
        uint32_t msk = (0x80 << ((nbBytes - 1) << 3));
        msk |= ~msk;
        
        /* Realign value for bit 0 */
        RetVal >>= (Offset << 3);
        
        /* Apply msak to readback value */
        RetVal &= msk;
    }
    else
    {
      /* Byte access is permitted */
        if((Offset == 0) && (nbBytes == 4))
        {
            /* Read the 32bit alligned data */  
            eResult = adf7030_1__SPI_rd_word_b_p( pSPIDevInfo, PNTR_CUSTOM0_ADDR, 1, &RetVal);
            
            if(pRegVal)
            {
                /* Return the value of full 32bit register*/
                *pRegVal = RetVal;
            }
        }else{
            /* Read the nbBytes data */
            eResult = adf7030_1__SPI_rd_byte_b_a( pSPIDevInfo, PNTR_CUSTOM0_ADDR, Offset, nbBytes, (uint8_t *)&RetVal);
        }
    }
    
    /* Exit if error */
    pSPIDevInfo->eXferResult = ADI_ADF7030_1_SUCCESS;
    
    return(RetVal);
}

/**
 * @brief       Generic setting of a Radio PHY bitfield to a value via the SPI
 *
 * @note                        This function does not support "SPI Error Status".
 *                              Call adf7030_1__SPI_GetBytes and adf7030_1__SPI_GetBytes
 *                              and apply appropriate masking to the SPI value readback.
 *
 * @sa  adf7030_1__SPI_GetBytes().
 * @sa  adf7030_1__SPI_SetBytes().
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            32bit alligned PHY Address location of the bitfield.
 *
 * @param [in]  Pos             Starting bit position withing the 32bits word.
 *
 * @param [in]  Size            Bit lenght of the bitfield.
 *
 * @param [in]  Val             Bitfield value.
 *
 * @return      None            
 */
void adf7030_1__SPI_SetField(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint32_t                    Pos,
    uint32_t                    Size,
    uint32_t                    Val
)
{
    uint32_t RegVal = 0;
      
    /* Compute Bitfields parameters */
    uint32_t fStartByte = Pos >> 3;
    uint32_t fPos_fromStartByte = Pos - (fStartByte << 3);
    uint32_t fLenBytes = ((Size - 1) >> 3) + 1;
    
    /* Check for special cases where we can skip adf7030_1__SPI_GetBytes() */
    uint32_t fMsk = ((1 << Size) - 1);
    
    uint32_t fVal;
    
    if(((fMsk % 255) == 0) && ((Pos % 8) == 0) && (adf7030_1__MEM_CheckByteAccess(Addr) == true))
    {
        /* This should cover the following bitfield mask values 
           0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
           0xFFFF0000, 0x00FFFF00, 0x0000FFFF
           0xFFFFFF00, 0x00FFFFFF,
           0xFFFFFFFF... 0 is an invalid value. */
        fVal = 0;
    }
    else
    {
        /* Read Bitfield vis SPI */
        fVal = adf7030_1__SPI_GetBytes( pSPIDevInfo, Addr + fStartByte, fLenBytes, &RegVal);
    }

    /* Compute the Mask */ 
    fMsk <<= fPos_fromStartByte;
    
    /* Bound Val to the Byte Position */
    Val <<= fPos_fromStartByte;
    Val &= fMsk;
    
    /* And SPI returned value with inverted Mask */
    fVal &= (~fMsk);
    
    /* Or SPI returned value with Val*/
    fVal |= Val;
    
    /* Write back Bitfield vis SPI */
    adf7030_1__SPI_SetBytes( pSPIDevInfo, Addr + fStartByte, fVal, fLenBytes, &RegVal);
}


/**
 * @brief       Generic readback of a bitfield value from the Radio PHY via the SPI
 *
 * @note                        This function does not support "SPI Error Status".
 *                              Call adf7030_1__SPI_GetBytes and apply appropriate
 *                              masking to the SPI value readback.
 *
 * @sa  adf7030_1__SPI_GetBytes().
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  Addr            32bit alligned PHY Address location of the bitfield.
 *
 * @param [in]  Pos             Starting bit position withing the 32bits word.
 *
 * @param [in]  Size            Bit lenght of the bitfield.
 *
 * @return      fVal            Bitfield value
 */
uint32_t adf7030_1__SPI_GetField(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    uint32_t                    Addr,
    uint32_t                    Pos,
    uint32_t                    Size
)
{
    /* Compute Bitfields parameters */
    uint32_t fStartByte = Pos >> 3;
    uint32_t fPos_fromStartByte = Pos - (fStartByte << 3);
    uint32_t fLenBytes = ((Size - 1) >> 3) + 1;
      
    /* Read Bitfield vis SPI */
    uint32_t fVal = adf7030_1__SPI_GetBytes( pSPIDevInfo, Addr + fStartByte, fLenBytes, NULL);
    
    /* Compute the Mask */ 
    uint32_t fMsk = ((1 << Size) - 1) << fPos_fromStartByte;
    
    /* And SPI returned value with Mask */
    fVal &= fMsk;
    
    /* Shift right by fPos_fromStartByte */
    fVal >>= fPos_fromStartByte;
    
    /* Return Bitfield Value */
    return(fVal);
}


/**
 * @brief       Generic Function to check if byte rw operation is permitted
 *
 * @note        By default, all internal memory of the PHY Radio can be accessed
 *              at byte level. This includes ROM, RAM and Radio Peripheral Registers.
 *              APB Peripherals only support 32bits word access.
 *
 * @param [in]  Addr            32bit byte address location.
 *
 * @return      None
 */
bool adf7030_1__MEM_CheckByteAccess(
    uint32_t nAddr
)
{
    /*  */ 
    if(nAddr > 0x40000000UL){
        if((nAddr >= 0x40004000UL) && (nAddr <= 0x400042E4UL))
        {
            /* Baseband modem configuration registers */ 
            return(true);
        }
        else
        {
            /* Radio auxillary peripherals connected via APBs bus */ 
            return(false);
        }
    }
    else
    {
        /* ROM, RAM etc */
        return(true);
    }
}


#endif /* _ADF7030_1__MEM_C_ */

/** @} */ /* End of group adf7030-1__mem Memory Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */
