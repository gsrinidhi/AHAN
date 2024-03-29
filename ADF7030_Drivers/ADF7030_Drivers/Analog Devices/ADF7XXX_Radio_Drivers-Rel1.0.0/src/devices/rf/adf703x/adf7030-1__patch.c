/*!
 *****************************************************************************
  @file:	adf7030-1__patch.c
  
  @brief:	adf7030-1 Patch Memory Access Functions.
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

/** \addtogroup adf7030-1__patch Patch Interface
 *  @{
 */
#ifndef _ADF7030_1__PATCH_C_
/*! \cond PRIVATE */
#define _ADF7030_1__PATCH_C_

#include <sys/platform.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>
#include <adf7030-1__mem.h>
#include <adf7030-1__spi.h>
#include <adf7030-1__irq.h>
#include <adf7030-1__state.h>

#include "adf7030-1__patch.h"


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

static void adf7030_1__SelfCheckPatch_callback(
    void                   *pCBParam,
    uint32_t                Port,
    void                   *PinIntData
);

extern void adf7030_1__IRQ_Callback (
    void                   *pCBParam,
    uint32_t                Port,
    void                   *PinIntData
);

/*! \endcond */

/* Flag which indicates whether an selfcheck interrupt is detected */
static volatile bool gbselfcheck_finished = false;

/**
 * @brief       Function call to write patch to Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pPatch          Pointer to Host "patch" descriptor.
 *
 * @note                        This function only transfert patch data to the PHY. 
 *                              Patch must be enabled at a later stage by calling the 
 *                              adf7030_1__InjectPatch() function.
 *                              This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the patch was written successfully in the Host.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the patch transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__WritePatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                              
    PATCH_DESCRIPTOR_Type *     pPATCH                                
)
{
    return(adf7030_1__XferPatch( pSPIDevInfo,
                                 pPATCH,
                                 PHY_MEM_WRITE,
                                 false));
}


/**
 * @brief       Function call to readback the patch from Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in|out]  pPatch      Pointer to Host "patch" descriptor.
 *
 * @note                        This function only readback patch data from the PHY. 
 *                              pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR] could be modified.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the patch was readback successfully in the Host.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the patch transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__ReadPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                             
    PATCH_DESCRIPTOR_Type *     pPATCH                                
)
{
    return(adf7030_1__XferPatch( pSPIDevInfo,
                                 pPATCH,
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
 * @param [in]  pPatch          Pointer to Host "patch" descriptor.
 *
 * @note                        This function only disable the patch, it can be 
 *                              re-enable at a later stage by calling the 
 *                              adf7030_1__InjectPatch() function.
 *                              This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the patch in the PHY matches the reference in the Host.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the patch is different.
 */
ADI_ADF7030_1_RESULT adf7030_1__VerifyPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                              
    PATCH_DESCRIPTOR_Type *     pPATCH                                
)
{
    return(adf7030_1__XferPatch( pSPIDevInfo,
                                 pPATCH,
                                 PHY_MEM_READ,
                                 true));
}

/**
 * @brief       Function call to perform PHY Radio patch self-checking
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pPatch          Pointer to Host "patch" descriptor.
 *
 * @param [in]  bCHECK          Boolean  -TRUE to verify the Inject sequence in the PHY
 *                                       -FALSE to discard any verifications
 *
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to be used.
 *
 * @param [in]  eCmd            PHY Radio Command to execute to trigger the start
 *                              of the PHY Radio selftest internal routine.
 *                              See ADI_ADF7030_1_RADIO_STATE enum for available commands.
 *
 * @param [in]  pCHECKSUM_CFG   Pointer to Host "patch" checksum configuration descriptor.
 *                              If NULL, the adf7030_1__SelfCheckPatch() will use the
 *                              default provided with the "patch" header file.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the patch intergrity is correct.
 *  - #ADI_ADF7030_1_FAILURE            If the patch intergrity is incorrect.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the patch is different.
 */
ADI_ADF7030_1_RESULT adf7030_1__SelfCheckPatch(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                              
    PATCH_DESCRIPTOR_Type *             pPATCH,
    bool                                bCHECK,
    ADI_ADF7030_1_INTPIN                eIntPin,
    ADI_ADF7030_1_RADIO_STATE           eCmd,
    PATCH_CHECKSUM_DESCRIPTOR_Type *    pCHECKSUM_CFG
)
{
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT eResult;
    
    DATA_BLOCK_DESCRIPTOR_Type * pPATCH_SelfcheckBlock = &pPATCH->BLOCKS[pPATCH->CheckBlockId];
    
    /* Download the patch selfcheck code */
    if((eResult = adf7030_1__WritePatchBlock( pSPIDevInfo,
                                              pPATCH_SelfcheckBlock)) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }
    
    if(bCHECK == true)
    {
        /* Verify the patch selfcheck code integrity */
        if((eResult = adf7030_1__VerifyPatchBlock( pSPIDevInfo,
                                                   pPATCH_SelfcheckBlock )) != ADI_ADF7030_1_SUCCESS)
        {
            return eResult;
        }
    }
   
    /* Setup patch selfcheck checksum descriptor */
    if(pCHECKSUM_CFG == NULL) pCHECKSUM_CFG = pPATCH->pChecksum;
    
    /* Configure patch selfcheck */
    if((eResult = adf7030_1__WritePatchCheckumInfo( pSPIDevInfo,
                                                    pPATCH,
                                                    pCHECKSUM_CFG )) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }
    
    /* Enabling the patch selfcheck function */
    if((eResult = adf7030_1__SetupPatch( pSPIDevInfo,
                               	   	 pPATCH->CheckSeq,
                                         true )) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }
    
    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO * pDevInfo = (ADI_ADF7030_1_INFO *)pSPIDevInfo->hDevInfo;

    /* Clearing all events on patch selfcheck IRQ pin */  
    if((eResult = adf7030_1__IRQ_ClrAllStatus( pDevInfo, eIntPin)) != ADI_ADF7030_1_SUCCESS )
    {
    	return eResult;
    }

    /* Save current IRQ Map config */
    if((eResult = adf7030_1__IRQ_GetMap(pDevInfo, eIntPin)) != ADI_ADF7030_1_SUCCESS )
    {
    	return eResult;
    }
    
    uint32_t tmp_nIntMap = pDevInfo->IntGPIOInfo[eIntPin].nIntMap;
    
    /* Setting BUFF_HALF_IRQn_Msk and BUFF_FULL_IRQn_Msk event for patch selfcheck IRQ pin */  
    if((eResult = adf7030_1__IRQ_SetMap( pDevInfo,
                              	  	 eIntPin,
                                         BUFF_HALF_IRQn_Msk | BUFF_FULL_IRQn_Msk )) != ADI_ADF7030_1_SUCCESS )
    {
    	return eResult;
    }
    
    /* Override IRQ callback function */
    if((eResult = adf7030_1__IRQ_SetCallback( pDevInfo,
                                              eIntPin,
                                              adf7030_1__SelfCheckPatch_callback )) != ADI_ADF7030_1_SUCCESS )
    {
    	return eResult;
    }
 
    gbselfcheck_finished = false;
    
    /* Disable PHY Error readback if required */
    bool bPhyErrorCheck = pSPIDevInfo->bPhyErrorCheck;    
    if(bPhyErrorCheck == true) pSPIDevInfo->bPhyErrorCheck = false;
    
    /* Trigger the Offline calibration selfcheck execution */          
    if((eResult = adf7030_1__STATE_PhyCMD( pSPIDevInfo, eCmd )) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }
    
    /* Wait for interrupt */
    while(gbselfcheck_finished == false) __WFE();
    
    /* Recall PHY Error readback setting */
    if(bPhyErrorCheck == true) pSPIDevInfo->bPhyErrorCheck = bPhyErrorCheck;
     
    /* Read IRQ status register of selfcheck IRQ pin */
    if((eResult = adf7030_1__IRQ_GetStatus( pDevInfo, eIntPin )) != ADI_ADF7030_1_SUCCESS )
    {
        return eResult;
    }
    
    eResult = (pDevInfo->IntGPIOInfo[eIntPin].nIntStatus & BUFF_FULL_IRQn_Msk) ?  ADI_ADF7030_1_SUCCESS : ADI_ADF7030_1_FAILURE;
    
    /* Clear selfcheck IRQ pin */
    if((eResult = adf7030_1__IRQ_ClrStatus( pDevInfo,
                                            eIntPin,
                                            pDevInfo->IntGPIOInfo[eIntPin].nIntStatus )) != ADI_ADF7030_1_SUCCESS )
    {
        return eResult;
    }

    /* Restore the default IRQ callback function */
    if((eResult = adf7030_1__IRQ_SetCallback( pDevInfo,
                                              eIntPin,
                                              adf7030_1__IRQ_Callback )) != ADI_ADF7030_1_SUCCESS )
    {
    	return eResult;
    }
    
    /* Restore IRQ Map */
    eResult = adf7030_1__IRQ_SetMap( pDevInfo,
                                     eIntPin,
                                     tmp_nIntMap );
 
    return eResult;
}


/* Test Callback overide */   
static void adf7030_1__SelfCheckPatch_callback (
    void                   *pCBParam,
    uint32_t                Port,
    void                   *PinIntData
)
{
	/* Pointer to Device Instance data */
	ADI_ADF7030_1_INFO * pDevInfo = (ADI_ADF7030_1_INFO *) pCBParam;

	uint32_t irq_event = 0;

	if((Port == pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN0].ePort) &&
			(*(uint32_t *)PinIntData & pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN0].nPin))
	{
		/* If callback Port and pin matches the Instance Irq0 port & pin definition */
		irq_event = ADI_ADF7030_1_EVENT_INT0;
                gbselfcheck_finished = true;
	}
	else if((Port == pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN1].ePort) &&
			(*(uint32_t *)PinIntData & pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN1].nPin))
	{
		/* If callback Port and pin matches the Instance Irq1 port & pin definition */
		irq_event = ADI_ADF7030_1_EVENT_INT1;
                gbselfcheck_finished = true;
	}
	else
	{
		/* Should not end up here */
	}
}

/**
 * @brief       Function call to setup the patch
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
   * @param [in]  SetupSeq        Setup patch sequence desired:
 *                               - InjectSeq to enable the patch
 *                               - EjectSeq to disable the patch
 *                               - CheckSeq to enable patch selfcheck and auto inject
 *
 * @param [in]  bCHECK          Boolean  -TRUE to verify the Inject sequence in the PHY
 *                                       -FALSE to discard any verifications
 *
 * @note                        This function assume the following the patch has
 *                              already been transfered and that its integrity is
 *                              valid. User can check the Ram for the patch presence
 *                              by calling the function adf7030_1__VerifyPatch().
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Inject sequence was succesfull.
 *  - #ADI_ADF7030_1_FAILURE            If the Inject sequence was not correct.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Inject failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__SetupPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                               
    uint16_t                    SetupSeq,
    bool                        bCHECK                                
)
{
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT eResult;
    
    if(SetupSeq == 0)
    {
        eResult = ADI_ADF7030_1_FAILURE;
        return eResult;
    }
    
    /* Write Inject sequence to radio PHY */
    uint32_t Seq = 0x20000000 + SetupSeq;

    if((eResult = adf7030_1__SPI_wr_word_b_a( pSPIDevInfo,
                                              0x20000130,
                                              1,
                                              &Seq )) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
    
    /* Readback Inject Sequence number if requested */
    if(bCHECK == true)
    {
        uint32_t Seq_rb;
        
        if((eResult = adf7030_1__SPI_rd_word_b_a( pSPIDevInfo,
                                                  0x20000130,
                                                  1,
                                                  &Seq_rb )) != ADI_ADF7030_1_SUCCESS)
        {
            return eResult;
        }
        
        /* Compare Setup patch sequence with the readback sequence */
        if(Seq != Seq_rb)
        {
            eResult = ADI_ADF7030_1_FAILURE;
        }
    }
      
    return eResult;
}


/**
 * @brief       Function to write a single Patch block to the PHY Radio
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBlock          Pointer to single Host patch memory block.
 *
 * @note                        This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Patch block has been written to the PHY.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Patch block could not be written.
 */
ADI_ADF7030_1_RESULT adf7030_1__WritePatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                               
    DATA_BLOCK_DESCRIPTOR_Type *        pBlock
)
{    
    return(adf7030_1__XferPatchBlock( pSPIDevInfo,
                                      pBlock,
                                      PHY_MEM_WRITE,
                                      false ));
}


/**
 * @brief       Function to read a single Patch block to the PHY Radio
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in|out] pBlock       Pointer to single Host patch memory block.
 *
 * @note                        This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Patch block has been read back into pBlock->pdata.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Patch block could not be read.
 */
ADI_ADF7030_1_RESULT adf7030_1__ReadPatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                                
    DATA_BLOCK_DESCRIPTOR_Type *        pBlock
)
{    
    return(adf7030_1__XferPatchBlock( pSPIDevInfo,
                                      pBlock,
                                      PHY_MEM_READ,
                                      false ));
}


/**
 * @brief       Function to verify content of a single Patch block in the PHY.
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pBlock          Pointer to single Host patch memory block.
 *
 * @note                        This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the Patch block in the PHY matches the reference in the Host.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the Patch block of the PHY is different than the reference in the Host.
 */
ADI_ADF7030_1_RESULT adf7030_1__VerifyPatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    DATA_BLOCK_DESCRIPTOR_Type *        pBlock
)
{
    return(adf7030_1__XferPatchBlock( pSPIDevInfo,
                                      pBlock,
                                      PHY_MEM_READ,
                                      true ));
}


/**
 * @brief       Generic Patch transfers function between Host and the Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in|out]  pPatch      Pointer to Host "patch" descriptor.
 *
 * @param [in]  bREAD_nWRITE    Boolean  -TRUE to write Patch from Host to the Radio PHY
 *                                       -FALSE to read Patch from Radio PHY back to Host
 *
 * @param [in]  bCHECK_ONLY     Boolean  -TRUE to check PHY patch versus Host Reference
 *                                       -FALSE [D] to perform READ_nWRITE operation
 *
 * @note                        This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__XferPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo, 
    PATCH_DESCRIPTOR_Type *     pPATCH,
    bool                        bREAD_nWRITE,
    bool                        bCHECK_ONLY
)
{   
    for(uint32_t idx = 0; idx < pPATCH->nbBlock; idx++)
    {
        /* Discard transfert of Patch Informations or Selfcheck section */
        if((idx != pPATCH->InfoBlockId) && (idx != pPATCH->CheckBlockId))
        {
            if(adf7030_1__XferPatchBlock( pSPIDevInfo, 
                                          (DATA_BLOCK_DESCRIPTOR_Type *)&pPATCH->BLOCKS[idx],
                                          bREAD_nWRITE,
                                          bCHECK_ONLY ) != ADI_ADF7030_1_SUCCESS)
            {
                return ADI_ADF7030_1_SPI_COMM_FAILED;
            }
        }    
    }  
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Generic Patch Section transfer function between Host and the Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in|out]  pBLOCK      Pointer to Host "patch" sub-block descriptor.
 *
 * @param [in]  bREAD_nWRITE    Boolean  -TRUE to write Patch from Host to the Radio PHY
 *                                       -FALSE to read Patch from Radio PHY back to Host
 *
 * @param [in]  bCHECK_ONLY     Boolean  -TRUE to check PHY patch versus Host Reference
 *                                       -FALSE [D] to perform READ_nWRITE operation
 *
 * @note                        This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__XferPatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo, 
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK,
    bool                                bREAD_nWRITE,
    bool                                bCHECK_ONLY
)
{     
    /* Block Transfers Mode */     
    uint32_t PHYAddr = pBLOCK->Addr;
      
    ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_id;
    int32_t PNTR_offset;
    
    /* Get good PNTRid from pSPIDevInfo->PHY_PNTR[] */
    adf7030_1__SPI_FindMMapPointer( pSPIDevInfo, PHYAddr, &PNTR_id, &PNTR_offset);
        
    /* Lets just use 8bits Mode SPI transfers for patch*/
    {
        /* Setup pointer to Host start of data */
        uint8_t * pHostMem = (uint8_t *)pBLOCK->pData;
        uint8_t * pHostRef = NULL;
        
        if(bCHECK_ONLY)
        {
            pHostRef = pHostMem;
            pHostMem = NULL;
        }
        
        uint32_t size = pBLOCK->Size;
    
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
                    return ADI_ADF7030_1_SPI_COMM_FAILED;
                }              
            }
            else
            {
                /* Write to radio PHY */
                if(adf7030_1__SPI_wr_byte_p_a( pSPIDevInfo,
                                               PNTR_CUSTOM0_ADDR,
                                               size,
                                               pHostMem) != ADI_ADF7030_1_SUCCESS)
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
                    return ADI_ADF7030_1_SPI_COMM_FAILED;
                }                    
            }
            else
            {
                /* Write to radio PHY */
                if(adf7030_1__SPI_wr_byte_b_a( pSPIDevInfo,
                                               PNTR_id,
                                               PNTR_offset,
                                               size,
                                               pHostMem) != ADI_ADF7030_1_SUCCESS)
                {
                    return ADI_ADF7030_1_SPI_COMM_FAILED;
                }  
            }
        }
    }
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Generic Patch checksum configuration transfers function between Host and the Radio PHY
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  pPatch      Pointer to Host "patch" descriptor.
 *
 * @param [in|out]  pPATCH_CHECKSUM      Pointer to Host "patch" checksum configuration descriptor.
 *
 * @param [in]  bREAD_nWRITE    Boolean  -TRUE to write Patch from Host to the Radio PHY
 *                                       -FALSE to read Patch from Radio PHY back to Host
 *
 * @param [in]  bCHECK_ONLY     Boolean  -TRUE to check PHY patch versus Host Reference
 *                                       -FALSE [D] to perform READ_nWRITE operation
 *
 * @note                        This function modifies pSPIDevInfo->PHY_PNTR[PNTR_CUSTOM0_ADDR].
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If the transfert was succesfull to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] If the transfert failed.
 */
ADI_ADF7030_1_RESULT adf7030_1__XferChecksum(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    PATCH_DESCRIPTOR_Type *             pPATCH,
    PATCH_CHECKSUM_DESCRIPTOR_Type *    pPATCH_CHECKSUM,
    bool                                bREAD_nWRITE,
    bool                                bCHECK_ONLY
)
{
    DATA_BLOCK_DESCRIPTOR_Type PATCH_CHECKSUM_BLOCK;
    PATCH_CHECKSUM_BLOCK.Addr = pPATCH->BLOCKS[pPATCH->InfoBlockId].Addr;
    PATCH_CHECKSUM_BLOCK.Size = (uint16_t)pPATCH_CHECKSUM->Size;
    PATCH_CHECKSUM_BLOCK.pData = (uint8_t *)pPATCH_CHECKSUM;
  
    return(adf7030_1__XferPatchBlock( pSPIDevInfo,
                                      &PATCH_CHECKSUM_BLOCK,
                                      bREAD_nWRITE,
                                      bCHECK_ONLY ));
}                                         

#endif /* _ADF7030_1__PATCH_C_ */

/** @} */ /* End of group adf7030-1__patch Patch Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */
