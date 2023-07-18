/*!
 *****************************************************************************
  @file:	adf703x_utils.c
  @brief:	This file contains a collection of small general purpose routines used in various tests.
			They are not part of the adf7030_1 Radio drivers, but provided as examples.
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


/** \addtogroup adf7030-1_examples Example Applications
 *  @{
 */

/** \addtogroup adf7030-1_utils Auxillary Utils 
 *  @{
 */

#include <sys/platform.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>


#include <common.h>

#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>
#include <adf7030-1__core.h>
#include <adf7030-1__irq.h>
#include <adf7030-1__spi.h>
#include <adf7030-1__mem.h>
#include <adf7030-1__patch.h>
#include <adf7030-1__cfg.h>
#include <adf7030-1__state.h>
#include <adf7030-1__gpio.h>


#include "adf703x_utils.h"


void test_Display(char *InfoString)
{
#ifdef REDIRECT_OUTPUT_TO_UART
    /* ignore return codes since there's nothing we can do if it fails */
#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
	adi_uart_Write(hDevOutput, InfoString, strlen(InfoString));
#elif defined (__ADUCM4050__)
	adi_uart_Write(hDevOutput, InfoString, strlen(InfoString), false, pHwError);
#endif
#else
    printf(InfoString);
#endif
}



#ifdef ADI_DEBUG

/*!
   Define max number of logged Host Radio driver instance error.\n
*/
#define NUM_INSTANCE_ERROR 19

/*!
   Define max number of logged PHY Radio error.\n
*/
#define NUM_PHY_ERROR 29


/*!
   Instance error debug messages array.\n
*/
char *instance_error_msgs[] =
{
		"ADI_ADF7030_1_SUCCESS -> The API is successful.",
		"ADI_ADF7030_1_FAILURE -> The API failed to complete the operation.",
		"ADI_ADF7030_1_HW_ERROR -> A hardware error was detected.",
		"ADI_ADF7030_1_INVALID_DEVICE_NUM -> The given device number is outside the number of instances supported by the driver.",
		"ADI_ADF7030_1_INVALID_HANDLE -> The given device handle is invalid.",
		"ADI_ADF7030_1_INSUFFICIENT_MEMORY -> The given memory is not sufficient to operate the device.",
		"ADI_ADF7030_1_NULL_POINTER -> One of the given pointer parameter points to NULL.",
		"ADI_ADF7030_1_UNALIGNED_MEM_PTR -> The given pointer to the device memory is not aligned to word boundary.",
		"ADI_ADF7030_1_DEVICE_ALREADY_OPENED -> The given device instance is already opened.",
		"ADI_ADF7030_1_DEVICE_NOT_OPENED -> The given device instance is not opened.",
		"ADI_ADF7030_1_INVALID_OPERATION -> The given operation is not permitted in the current state of the driver.",
		"ADI_ADF7030_1_DEVICE_ACCESS_FAILED -> Failed to read or write the device registers.",
		"ADI_ADF7030_1_GPIO_DEV_FAILED -> Failed to register or unregister callback with GPIO service.",
		"ADI_ADF7030_1_SPI_DEV_FAILED -> SPI device driver failure.",
		"ADI_ADF7030_1_SPI_DEV_POLL_EXPIRE -> SPI device driver polling timeout.",
		"ADI_ADF7030_1_SPI_COMM_FAILED -> SPI Communication device related failure.",
		"ADI_ADF7030_1_INVALID_PHY_CONFIGURATION -> The given PHY Radio configuration is invalid.",
		"ADI_ADF7030_1_PENDING_IO -> If trying to perform a read when another read/write in progress.",
		"ADI_ADF7030_1_DMA_NOT_SUPPORTED -> If trying to enable the DMA mode when communication device is TWI."
};


/*!
   PHY Radio hardware error debug messages array.\n
*/
char *phy_error_msgs[] =
{
		"SM_NOERROR -> No error",																	// 0x00
		"SM_INVALID_STATE -> Requested state does not exist", 										// 0x01
		"SM_TRANS_BLOCKED -> State transition has been denied", 									// 0x02
		"SM_TIMEOUT -> State machine timeout", 														// 0x03
		"SM_ILLEGAL_SCRIPT_CMD -> Illegal script command", 											// 0x04
		"SM_UNKNOWN_SPECIAL_CMD -> Unknown state machine special command", 							// 0x05
		"SM_UNPOPULATED_CUSTOM_CMD -> Special command is not configured", 							// 0x06
		"SM_PROFILE_CRC_INCORRECT -> Backup up configuration is not valid", 						// 0x07
		"SM_LPM_CRC_INCORRECT -> Not available ",													// 0x08
		"SM_INVALID_SCRIPT_DATA -> Not available", 													// 0x09
		"SM_ROM_CRC_INCORRECT -> Not available", 													// 0x30
		"SM_PROFILE_CRC_CALCULATING -> Startup CRC calculation running (not really an error code)", // 0x31
		"HW_MAIN_PLL_VCO_CAL_FAILURE_COARSE_CAL_FAILED -> PLL course calibration failure", 			// 0x11
		"HW_MAIN_PLL_VCO_CAL_FAILURE_AMP_CAL_FAILED -> PLL amplitude calibration failure", 			// 0x12
		"HW_MAIN_PLL_VCO_CAL_FAILURE_FINE_CAL_FAILED -> PLL fine calibration failure", 				// 0x13
		"HW_ANC_PLL_LOCK_FAILED -> Auxiliary PLL lock failure", 									// 0x14
		"HW_MAIN_PLL_VCO_CAL_MODE_INVALID -> PLL calibration mode invalid", 						// 0x15
		"HW_TCXO_NOT_READY -> Temperature Compensated Crystal Oscillator failure", 					// 0x20
		"HW_XTAL_NOT_READY -> Crystal Oscillator failure", 											// 0x21
		"HW_OCL_CAL_FAILED -> Offset calibration failure", 											// 0x23
		"HW_DIV_BY_ZERO -> Hardware divide by zero", 												// 0x24
		"HW_VCO_KV_CAL_SINGLE_SHOT_FAILED -> VCO KV calibration step failure", 						// 0x25
		"HW_VCO_KV_CAL_FINE_CAL_FAILED -> VCO KV fine calibration failure", 						// 0x26
		"HW_VCO_KV_CAL_AMP_CAL_FAILED -> VCO KV amplitude calibration failure", 					// 0x27
		"HW_VCO_KV_HFXTAL_INVALID -> VCO KV clock source invalid", 									// 0x28
		"HW_VCO_KV_CAL_COARSE_CAL_FAILED -> VCO KV coarse calibration failure", 					// 0x29
		"HW_TEMP_SENSE_FAILED -> Temperature sensor failure", 										// 0x32
		"HW_LFRC_CAL_FAILED -> Low frequency oscillator calibration failure", 						// 0x33
		"HW_HARD_FAULT_FAIL -> Firmware hard fault \0" 												// 0x3F
};


/*!
   Instance error debug type definition.\n
*/
typedef struct __adf7030_1_instance_error_debug_type
{
	uint8_t instance_error_id;
	char ** instance_error_str;
} ADF7030_1_INSTANCE_ERROR_DEBUG_Type;


/*!
   PHY Radio error debug type definition.\n
*/
typedef struct __adf7030_1_phy_error_debug_type
{
	uint8_t phy_error_id;
	char ** phy_error_str;
} ADF7030_1_PHY_ERROR_DEBUG_Type;


/*!
   Instance error debug array.\n
*/
ADF7030_1_INSTANCE_ERROR_DEBUG_Type instance_error_id[NUM_INSTANCE_ERROR] =
{
		{ADI_ADF7030_1_SUCCESS, &instance_error_msgs[0]},
		{ADI_ADF7030_1_FAILURE, &instance_error_msgs[1]},
		{ADI_ADF7030_1_HW_ERROR, &instance_error_msgs[2]},
		{ADI_ADF7030_1_INVALID_DEVICE_NUM, &instance_error_msgs[3]},
	    {ADI_ADF7030_1_INVALID_HANDLE, &instance_error_msgs[4]},
	    {ADI_ADF7030_1_INSUFFICIENT_MEMORY, &instance_error_msgs[5]},
	    {ADI_ADF7030_1_NULL_POINTER, &instance_error_msgs[6]},
	    {ADI_ADF7030_1_UNALIGNED_MEM_PTR, &instance_error_msgs[7]},
	    {ADI_ADF7030_1_DEVICE_ALREADY_OPENED, &instance_error_msgs[8]},
	    {ADI_ADF7030_1_DEVICE_NOT_OPENED, &instance_error_msgs[9]},
	    {ADI_ADF7030_1_INVALID_OPERATION, &instance_error_msgs[10]},
	    {ADI_ADF7030_1_DEVICE_ACCESS_FAILED, &instance_error_msgs[11]},
		{ADI_ADF7030_1_GPIO_DEV_FAILED, &instance_error_msgs[12]},
		{ADI_ADF7030_1_SPI_DEV_FAILED, &instance_error_msgs[13]},
		{ADI_ADF7030_1_SPI_DEV_POLL_EXPIRE, &instance_error_msgs[14]},
		{ADI_ADF7030_1_SPI_COMM_FAILED, &instance_error_msgs[15]},
		{ADI_ADF7030_1_INVALID_PHY_CONFIGURATION, &instance_error_msgs[16]},
		{ADI_ADF7030_1_PENDING_IO, &instance_error_msgs[17]},
		{ADI_ADF7030_1_DMA_NOT_SUPPORTED, &instance_error_msgs[18]}
};

/*!
   PHY Radio error debug array.\n
*/
ADF7030_1_PHY_ERROR_DEBUG_Type phy_error_id[NUM_PHY_ERROR] =
{
		{0x00, &phy_error_msgs[0]},
		{0x01, &phy_error_msgs[1]},
		{0x02, &phy_error_msgs[2]},
		{0x03, &phy_error_msgs[3]},
	    {0x04, &phy_error_msgs[4]},
	    {0x05, &phy_error_msgs[5]},
	    {0x06, &phy_error_msgs[6]},
	    {0x07, &phy_error_msgs[7]},
	    {0x08, &phy_error_msgs[8]},
	    {0x09, &phy_error_msgs[9]},
	    {0x30, &phy_error_msgs[10]},
	    {0x31, &phy_error_msgs[11]},
		{0x11, &phy_error_msgs[12]},
		{0x12, &phy_error_msgs[13]},
		{0x13, &phy_error_msgs[14]},
		{0x14, &phy_error_msgs[15]},
		{0x15, &phy_error_msgs[16]},
		{0x20, &phy_error_msgs[17]},
		{0x21, &phy_error_msgs[18]},
		{0x23, &phy_error_msgs[19]},
		{0x24, &phy_error_msgs[20]},
		{0x25, &phy_error_msgs[21]},
		{0x26, &phy_error_msgs[22]},
		{0x27, &phy_error_msgs[23]},
		{0x28, &phy_error_msgs[24]},
		{0x29, &phy_error_msgs[25]},
		{0x32, &phy_error_msgs[26]},
		{0x33, &phy_error_msgs[27]},
		{0x3F, &phy_error_msgs[28]}
};


/**
 * @brief       Check erroneous return code of the Radio driver functions
 * 				and display extra informations
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  eResult       	Error code return from enum ADI_ADF7030_1_RESULT
 *
 * @return      None
 */
void CheckReturn(
	ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
	ADI_ADF7030_1_RESULT		eResult
)
{
	char **my_instance_error;

	/* Search throught Instance ERROR messages */
	for(int i=0; i < NUM_INSTANCE_ERROR; i++)
	{
		/* Display Instance ERROR message */
		if(instance_error_id[i].instance_error_id == (uint8_t)eResult)
		{
			my_instance_error = instance_error_id[i].instance_error_str;
			DBG("FAIL\n\n** ERROR 0x%02X **\n\n%s", eResult, *my_instance_error);

			/* Display PHY error */
			if(eResult == ADI_ADF7030_1_HW_ERROR)
			{
				char **my_phy_error;
				for(int j=0; j < NUM_PHY_ERROR; j++)
				{
					if(phy_error_id[j].phy_error_id == pSPIDevInfo->ePhyError)
					{
						my_phy_error = phy_error_id[j].phy_error_str;
					}
				}
				DBG("\n\n*** ePhyError 0x%02X ***\n\n%s", pSPIDevInfo->ePhyError, *my_phy_error);
			}
		}
	}
}

#else

#warning "Examples are meant to run with ADI_DEBUG macro..."

#endif /* ADI_DEBUG */


void CHECK( uint32_t condition )
{
    if(condition)
    {
        DBG("PASS\n");
    }else
    {
        DBG("FAIL\n");
    } 
}

/**
 * @brief       Display PHY Radio firmware patch informations          
 *              
 * @param [in]  PatchName       Pointer to the ADF7030-1 patch name.
 *   
 * @param [in]  pPathInfo       Pointer to patch information structure.                       
 *             
 * @return      None
 */
void DISPLAY_PATCH_INFO(char * PatchName, version_info_t * pPathInfo)
{
    DBG("%s Informations:\n", PatchName);
    DBG("   -Name:           %s\n",     pPathInfo->patch_name);
    DBG("   -build_number:   0x%04x\n", pPathInfo->build_number);
    DBG("   -family_str:     %.4s\n",   pPathInfo->family_str);
    DBG("   -adiid:          0x%04x\n", pPathInfo->adiid);
    DBG("   -chipid:         0x%04x\n", pPathInfo->chipid);
    DBG("   -full_layer_num: 0x%02x\n", pPathInfo->full_layer_num);
    DBG("   -eco_num:        0x%02x\n", pPathInfo->eco_num);
    DBG("   -svn_version:    0x%04x\n", pPathInfo->svn_version);
}


/**
 * @brief       Display Host shadow PROFILE informations          
 *              
 * @param [in]  pPROFILE_BLOCK  Pointer to the host data block containing
 *                              the shadow PROFILE data.                     
 *             
 * @return      None
 */
void DISPLAY_PROFILE_INFO(DATA_BLOCK_DESCRIPTOR_Type * pPROFILE_BLOCK)
{      
    /* Declare Host Shadow PROFILE structure */
    ADI_PROFILE_Type * pPROFILE = (ADI_PROFILE_Type *)pPROFILE_BLOCK->pData;
    
    DBG("\nPROFILE Informations:\n");
    DBG(" - Size (bytes): %d\n", pPROFILE_BLOCK->Size);
    DBG(" - PHY Addr:  0x%08x\n", pPROFILE_BLOCK->Addr);
    DBG(" - HOST Addr: 0x%08x\n\n", &pPROFILE_BLOCK->pData);
    
    DBG("    o RF Freq (MHz): 0x%d\n", pPROFILE->CH_FREQ);
#ifdef __INTERNAL_DEV__
    DBG("    o Mod type: 0x%d\n", pPROFILE->RADIO_MODES_b.TX_MOD_TYPE);
    DBG("    o Bitrate(Hz): 0x%d\n", (pPROFILE->RADIO_DATA_RATE_b.DATA_RATE * 100) + pPROFILE->RADIO_DATA_RATE_b.NUMERATOR_FRACN_DATARATE);
    DBG("    o Freqdev: 0x%d\n", pPROFILE->RADIO_DIG_TX_CFG1_b.FDEV);
#endif
    DBG("    o PA Ramp rate = %d\n\n", pPROFILE->RADIO_DIG_TX_CFG1_b.PA_RAMP_RATE);
}


/**
 * @brief       Display Host shadow GENERIC_PKT informations          
 *              
 * @param [in]  pPACKET_CFG__BLOCK  Pointer to the host data block containing
 *                                  the shadow GENERIC_PKT data.                     
 *             
 * @return      None
 */
void DISPLAY_PACKET_INFO(DATA_BLOCK_DESCRIPTOR_Type * pPACKET_CFG__BLOCK)
{      
    /* Declare Host Shadow Packet Configuration structure */
    ADI_GENERIC_PKT_Type * pGENERIC_PKT = (ADI_GENERIC_PKT_Type *)pPACKET_CFG__BLOCK->pData;
    
    DBG("\nPACKET Informations:\n");
    DBG(" - Size (bytes): %d\n", pPACKET_CFG__BLOCK->Size);
    DBG(" - PHY Addr:  0x%08x\n", pPACKET_CFG__BLOCK->Addr);
    DBG(" - HOST Addr: 0x%08x\n\n", &pPACKET_CFG__BLOCK->pData);

    DBG("    o Preamble length (\"01\" bit pair) = %d\n", pGENERIC_PKT->FRAME_CFG0_b.PREAMBLE_LEN);
    DBG("    o Syncword length = %d\n", pGENERIC_PKT->FRAME_CFG0_b.SYNC0_LEN);
    DBG("    o Lenword length = %d\n", pGENERIC_PKT->FRAME_CFG2_b.LEN_SEL);
    DBG("    o Bit2Air = %d\n", pGENERIC_PKT->BUFF_CFG0_b.BIT2AIR);
    DBG("    o Endec = %d\n\n", pGENERIC_PKT->FRAME_CFG2_b.ENDEC_MODE);
}


/**
 * @brief       Display Raw data from the host memory 
 *              
 * @note        If "pOffset" and "size" are NULL, the function will display the 
 *              entire content of the host memory block pointed by pDATA__BLOCK.
 *              Else if pDATA__BLOCK is NULL and "pOffset" plus "size" are defined,
 *              this function will print "size" Bytes starting at location pointed 
 *              by "pOffset".
 *              
 * @param [in]  pDATA__BLOCK    Pointer to the host data block structure.                     
 *             
 * @param [in]  pOffset         Pointer to desired host address location to start 
 *                              printing raw data.
 *             
 * @param [in]  size            Number of bytes to be displayed                     
 *             
 * @return      None
 */
void DISPLAY_BLOCK_CONTENT(DATA_BLOCK_DESCRIPTOR_Type * pDATA__BLOCK, void * pOffset, uint32_t size)
{
    uint8_t * pData;
  
    if(size == NULL){
        size = pDATA__BLOCK->Size;
    }
    
    if(pOffset != NULL){
        pData = (uint8_t *)pOffset;
    }else{
        pData = (uint8_t *)&pDATA__BLOCK->pData[0];
    }
  
    for(uint16_t i = 0; i < size; i++){
        DBG("0x%02x ", *(pData + i));
    }
    DBG("\n\n");
}


/**
 * @brief       Display which PHY Radio calibration is enabled           
 *              
 * @param [in]  cal_enables  Calibration enable enum from the PHY Radio profile.
 *                           (ADI_ADF7030_1_RADIO_CALIBRATION)adf7030_1__SPI_GetMem32(pSPIDevInfo, PROFILE_RADIO_CAL_CFG0_Addr)                     
 *             
 * @return      None
 */
void DISPLAY_CALIBRATION_ENABLES(ADI_ADF7030_1_RADIO_CALIBRATION cal_enables)
{
    DBG("\nCalibration Enables:\n");
    DBG(" o Analog filter RC calibration       = %d\n", (ANAFILT_RC_CAL & cal_enables) ? 1:0 );
    DBG(" o ADC Notch calibration              = %d\n", (ADC_NOTCH_CAL & cal_enables) ? 1:0 );
    DBG(" o Analog QEC calibration             = %d\n", (ANA_QEC_CAL & cal_enables) ? 1:0 );
    DBG(" o Digital QEC calibration            = %d\n", (DIG_QEC_CAL & cal_enables) ? 1:0 );
    DBG(" o Anxillary PLL calibration          = %d\n", (ANCPLL_CAL & cal_enables) ? 1:0 );
    DBG(" o Low frequency RC calibration       = %d\n", (LF_RC_CAL & cal_enables) ? 1:0 );
    DBG(" o High frequency RC calibration      = %d\n", (HF_RC_CAL & cal_enables) ? 1:0 );
    DBG(" o VCO calibration                    = %d\n", (VCO_CAL & cal_enables) ? 1:0 );
    DBG(" o VCO KV calibration                 = %d\n", (VCO_KV_CAL & cal_enables) ? 1:0 );
    DBG(" o Xtal Temp compensation             = %d\n", (TEMP_XTAL_CAL & cal_enables) ? 1:0 );
    DBG(" o High frequency XTAL calibration    = %d\n", (HF_XTAL & cal_enables) ? 1:0 );
    DBG(" o Analog filter RC calibration       = %d\n\n", (INLINE_OCL_CAL & cal_enables) ? 1:0 );
}


/**
 * @brief       ADuCM302x_EZ-KIT_Lite push button polling loop          
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in|out]  ePB         Pointer to ADI_ADF7030_1_TRIGPIN enum corresponding to Push Button pressed          
 *             
 * @return      Status           
 *  - 0            If ADuCM302x_EZ-KIT_Lite push button has been pressed
 *  - -1           If an error occured when polling GPIOs
 */
uint32_t WAIT_PB(ADI_ADF7030_1_INFO * pDevInfo, ADI_ADF7030_1_TRIGPIN * ePB)
{
    ADI_GPIO_RESULT eResult;
  
    /* set GPIO input */
    if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_InputEnable(EZBOARD_PB0_GPIO_PORT, EZBOARD_PB0_GPIO_PIN, true)))
    {
        DEBUG_MESSAGE("adi_gpio_InputEnable failed\n");
        return(-1);
    }

    /* set GPIO input */
    if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_InputEnable(EZBOARD_PB1_GPIO_PORT, EZBOARD_PB1_GPIO_PIN, true)))
    {
        DEBUG_MESSAGE("adi_gpio_InputEnable failed\n");
        return(-1);
    }

    uint16_t Data;
    
    bool bExit = false;
    
    do{
        /* poll PB0 */
        if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_GetData(EZBOARD_PB0_GPIO_PORT, EZBOARD_PB0_GPIO_PIN, &Data)))
        {
            DEBUG_MESSAGE("adi_gpio_GetData failed\n");
            return(-1);
        }
        
        if(!Data)
        {
            *ePB = ADI_ADF7030_1_TRIGPIN0;
            bExit = true;
        }
        else
        {
            /* poll PB1 */
            if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_GetData(EZBOARD_PB1_GPIO_PORT, EZBOARD_PB1_GPIO_PIN, &Data)))
            {
                DEBUG_MESSAGE("adi_gpio_GetData failed\n");
                return(-1);
            }
            
            if(!Data)
            {
                *ePB = ADI_ADF7030_1_TRIGPIN1;
                bExit = true;
            }
        }
    }while(bExit == false);
    
    return(0);
}


/** @} */ /* End of group adf7030-1_utils Auxillary Utils */
/** @} */ /* End of group adf7030-1_examples Example Applications */
