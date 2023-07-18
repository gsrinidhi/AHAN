/*!
 *****************************************************************************
  @file:	adf7030-1__core.c
  
  @brief:	This file provide all necessary API function to setup the Radio driver.
  
  @details:	For more information, plesase refer to the Software Reference Manual.
  
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

/** \addtogroup adf7030-1__core Core Interface
 *  @{
 */

#ifndef _ADF7030_1__CORE_C_

/*! \cond PRIVATE */
#define _ADF7030_1__CORE_C_
   
#include <sys/platform.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#include <drivers/spi/adi_spi.h>

#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
#include <services/gpio/adi_gpio.h>
#include <services/pwr/adi_pwr.h>
#elif defined (__ADUCM4050__)
#include <drivers/gpio/adi_gpio.h>
#include <drivers/pwr/adi_pwr.h>
#endif


#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>

#include <adf7030-1__spi.h>
#include <adf7030-1__irq.h>
#include <adf7030-1__mem.h>
#include <adf7030-1__cfg.h>
#include <adf7030-1__patch.h>
#include <adf7030-1__state.h>
#include <adf7030-1__gpio.h>

#include "adf7030-1__core.h"

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


/* ADF7030_1 instance information */
extern ADI_ADF7030_1_DEVICE    gADF7030_1_Dev[ADF7030_1_NUM_INSTANCES];

/**
 * @brief       Opens the adf7030-1 driver.
 *
 * @details     This opens the ADF7030-1 driver and upon success returns the handle
 *              to the driver.
 *
 *              The ADF7030-1 uses SPI for communicating with the host processor.
 *
 *              After the ADF7030-1 driver is opened, the part will be in standby
 *              mode. Once the required configuration is done, the API
 *              adf7030_1__Setup() and adf7030_1__Enable() should be called prior 
 *              RX or TX operatoions.
 *

 * @param [in]  nDeviceNum          ADF7030-1 device instance number. There can be
 *                                  more than one ADF7030-1 devices on a board. This
 *                                  determines the instance that is to be opened.
 *
 * @param [in]  pDeviceMemory       Memory required for ADF7030-1 device operation.
 *\n
 *
 * @param [in]  nMemSize            The size of the memory passed in bytes.
 *
 * @param [in]  pfCallback          Application supplied callback function, which
 *                                  will be called by the driver to notify interrupts
 *                                  from the PHY radio.
 * \n
 *                                  Upon getting the interrupt callback, application
 *                                  can use the API #adf7030_1__IRQ_GetIntStatus to know
 *                                  what caused the interrupts. It can be one or more
 *                                  of interrupts in the enumeration 
 *                                  #ADI_AF7030_1_EXT_IRQn_Msk_Type that could have 
 *                                  caused the interrupt.
 *
 * @param [in]  pCBParam            Application supplied callback parameter, which
 *                                  will be passed back when the callback function
 *                                  is called. 
 *
 * @param [out] phDevice            Pointer to a location where the device handle
 *                                  is written.
 * @return      Status
 *
 *  - #ADI_ADF7030_1_SUCCESS                      If successfully opened the device.
 *
 *  - #ADI_ADF7030_1_INVALID_DEVICE_NUM       [D] If the given device number is outside
 *                                                the number of instances supported.
 *
 *  - #ADI_ADF7030_1_NULL_POINTER             [D] If one of the given pointers are
 *                                                pointing to NULL.
 *
 *  - #ADI_ADF7030_1_INSUFFICIENT_MEMORY      [D] If the given memory is not sufficient
 *                                                to operate the device.
 *
 *  - #ADI_ADF7030_1_UNALIGNED_MEM_PTR        [D] If the given device memory pointer
 *                                                 is not aligned to a word boundary.
 *
 *  - #ADI_ADF7030_1_DEVICE_ALREADY_OPENED    [D] If the given device is already opened.
 *
 * @note        SPI mode is not supported.
 *
 * @sa          adf7030_1__Close()
 * @sa          adf7030_1__Setup()
 * @sa          adf7030_1__Enable()
 */
ADI_ADF7030_1_RESULT adf7030_1__Open(
    uint32_t                            nDeviceNum,
    void * const                        pDeviceMemory,
    uint32_t                            nMemSize,
    ADI_CALLBACK                        pfCallback,
    void * const                        pCBParam,
    ADI_ADF7030_1_HANDLE * const        phDevice
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice;
    
    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO *pDevInfo;
    
#ifdef ADI_DEBUG
    
    /* Only handle a single instance of the adf7030-1 */
    if(nDeviceNum > ADF7030_1_NUM_INSTANCES)
    {
        return ADI_ADF7030_1_INVALID_DEVICE_NUM;
    }
    
    /* Check if the given device instance is already opened */
    if(gADF7030_1_Dev[nDeviceNum].eState != ADI_ADF7030_1_STATE_NOT_OPENED)
    {
        return ADI_ADF7030_1_DEVICE_ALREADY_OPENED;
    }

#endif /* ADI_DEBUG */

    pDevice = &gADF7030_1_Dev[nDeviceNum];

    pDevice->pDevInfo   =   (ADI_ADF7030_1_INFO * )pDeviceMemory;
    pDevInfo            =   pDevice->pDevInfo;

    /* Clear the Device Information structure */
    memset((void *)pDevice->pDevInfo, 0, sizeof(ADI_ADF7030_1_INFO));
   
    /* Save the given parameters */
    pDevInfo->pfCallback        =   pfCallback;
    pDevInfo->pCBParam          =   pCBParam;

#if defined (__ADUCM4050__)
    ADI_INT_STATUS_ALLOC();
#endif

    ADI_ENTER_CRITICAL_REGION();

    /*Mark the device as opened */
    pDevice->eState =   ADI_ADF7030_1_STATE_OPENED;
    
    ADI_EXIT_CRITICAL_REGION();
    
    /* Setup pointer to parent device in opened device info*/
    pDevInfo->hDevice = (void *)pDevice;
        
    /* return the handle to the opened device*/
    *phDevice = pDevice;

    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Setup the adf7030-1 instance by initialising its device info
 *              structure. It also calls adf7030_1__SPI_Init() and adf7030_1__IRQ_Init()
 *              to setup underlying SPI driver and GPIO service.
 *
 * @note        This function initialise device instance memory by copying static
 *              reference of ADI_ADF7030_1_SPI_INFO and ADI_ADF7030_1_GPIO_INFO.
 *              
 * @param [in]  hDevice             Handle to ADF7030-1 device instance.
 *             
 * @param [in]  pSPIDevInfo         Pointer to a reference ADF7030-1 instance SPI information structure.
 *             
 * @param [in]  pGPIODevInfo        Pointer to a reference ADF7030-1 instance GPIO information structure.
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY instance was succesfully setup
 *  - #ADI_ADF7030_1_DEVICE_NOT_OPENED  [D] If PHY instance is not yet opened
 *  - #ADI_ADF7030_1_INVALID_HANDLE     [D] If PHY instance does not exist
 *  - #ADI_ADF7030_1_SPI_DEV_FAILED     [D] If PHY instance SPI setup failed
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED    [D] If PHY instance GPIO setup failed
 *
 * @sa  adf7030_1__SPI_Init().
 * @sa  adf7030_1__IRQ_Init().
*/
ADI_ADF7030_1_RESULT adf7030_1__Setup(
    ADI_ADF7030_1_HANDLE  const         hDevice,
    ADI_ADF7030_1_SPI_INFO * const      pSPIDevInfo,
    ADI_ADF7030_1_GPIO_INT_INFO * const pINTDevInfo,
    ADI_ADF7030_1_GPIO_TRIG_INFO * const pTRIGDevInfo,
	ADI_ADF7030_1_GPIO_RESET_INFO * const pRESETDevInfo
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;

    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;
    
#ifdef ADI_DEBUG
    /* Validate the given device handle */
    if((eResult = ValidateHandle(pDevice)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
#endif /* ADI_DEBUG */

    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO *pDevInfo = pDevice->pDevInfo;

    /* Initialise underlying SPI driver information structure */
    memcpy((void *)&(pDevInfo->SPIInfo), (const void *)pSPIDevInfo, sizeof(ADI_ADF7030_1_SPI_INFO));
    
    /* Call the SPI driver initialisation routine */
    if(adf7030_1__SPI_Init(hDevice) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }

    /* Initialise underlying GPIO service information structure for IRQs */
    memcpy((void *)pDevInfo->IntGPIOInfo, (void *)pINTDevInfo, sizeof(ADI_ADF7030_1_GPIO_INT_INFO) * ADF7030_1_NUM_INT_PIN);
    
    /* Call the GPIO service initialisation routine for IRQs*/
    if(adf7030_1__IRQ_Init(hDevice) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
    
    if(pTRIGDevInfo)
    {
        /* Initialise GPIO service information structure for Triggers pins */
        memcpy((void *)pDevInfo->TrigGPIOInfo, (void *)pTRIGDevInfo, sizeof(ADI_ADF7030_1_GPIO_TRIG_INFO) * ADF7030_1_NUM_TRIG_PIN);
        
        /* Setup Host GPIO for trigger 0 */
        if((eResult = adf7030_1__EnableHostTrigger(pDevInfo, ADI_ADF7030_1_TRIGPIN0, true)) != ADI_ADF7030_1_SUCCESS)
        {
            return eResult;
        }

        /* Setup Host GPIO for trigger 1 */
        if((eResult = adf7030_1__EnableHostTrigger(pDevInfo, ADI_ADF7030_1_TRIGPIN1, true)) != ADI_ADF7030_1_SUCCESS)
        {
            return eResult;
        }
    }
    else
    {
        /* Just blank for now */
        memset((void *)pDevInfo->TrigGPIOInfo, 0, sizeof(ADI_ADF7030_1_GPIO_TRIG_INFO) * ADF7030_1_NUM_TRIG_PIN);
    }


    if(pRESETDevInfo)
    {
    	/* Initialise GPIO service information structure for the Reset pin */
    	memcpy((void *)&pDevInfo->ResetGPIOInfo, (void *)pRESETDevInfo, sizeof(ADI_ADF7030_1_GPIO_RESET_INFO));

        /* Setup Host GPIO for trigger */
        ADI_ADF7030_1_GPIO_RESET_INFO * pResetGPIOInfo = &pDevInfo->ResetGPIOInfo;

        if(adi_gpio_SetHigh(pResetGPIOInfo->ePort, pResetGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
        {
            return ADI_ADF7030_1_GPIO_DEV_FAILED;
        }

        if(adi_gpio_OutputEnable(pResetGPIOInfo->ePort, pResetGPIOInfo->nPin, true) != ADI_GPIO_SUCCESS)
        {
            return ADI_ADF7030_1_GPIO_DEV_FAILED;
        }
    }

    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Enable the adf7030-1.
 *
 * @note        This function initialise device instance memory by copying static
 *              reference of ADI_ADF7030_1_SPI_INFO and ADI_ADF7030_1_GPIO_INFO.
 *              
 * @param [in]  hDevice         Handle to ADF7030-1 device instance.
 *             
 * @param [in]  bEnable         Boolean  -TRUE to enable Radio PHY
 *                                       -FALSE to disable Radio PHY
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY instance was succesfully setup
 *  - #ADI_ADF7030_1_DEVICE_NOT_OPENED  [D] If PHY instance is not yet opened
 *  - #ADI_ADF7030_1_INVALID_HANDLE     [D] If PHY instance does not exist
 *  - #ADI_ADF7030_1_FAILURE            [D] If PHY instance could not be opened or closed
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED    [D] If PHY instance GPIO setup failed
 *
 * @sa  adf7030_1__SPI_Init().
 * @sa  adf7030_1__IRQ_Init().
*/
ADI_ADF7030_1_RESULT adf7030_1__Enable(
    ADI_ADF7030_1_HANDLE  const         hDevice,
    bool                                bEnable
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;
    
    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO *pDevInfo = pDevice->pDevInfo;
    
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;

    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;
    
#ifdef ADI_DEBUG
    /* Validate the given device handle */
    if((eResult = ValidateHandle(hDevice)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
#endif /* ADI_DEBUG */
    
#if (ADF7030_1_PHY_ERROR_REPORT_ENABLE == 1)
    /* Disable automatic PhyError reporting by default */
    pSPIDevInfo->bPhyErrorCheck = false;
#endif
    
    if(bEnable == true)
    {
      /* Place holder for turning on the adf7030-1 PHY */
      
        /* Execute user defined "enable" sequence */
        if(pDevInfo->pfEnableFunc)
        {
            if((eResult = pDevInfo->pfEnableFunc(pDevInfo)) !=  ADI_ADF7030_1_SUCCESS)
            {
                return eResult;
            }
        }
        else
        {
        /* Execute default "enable" sequence */
          
            /* Toggle the Reset pin the the PHY Radio */
            if((pDevInfo->ResetGPIOInfo.ePort != 0UL) && (pDevInfo->ResetGPIOInfo.nPin != 0UL))
            {
                if((eResult = adf7030_1__PulseReset(pDevInfo)) != ADI_ADF7030_1_SUCCESS)
                {
                    return eResult;
                }
            }
            
            /* Wake up the PHY Radio and wait until we are in PHY_OFF */
            if((eResult = adf7030_1__STATE_PhyCMD_WaitReady( pSPIDevInfo, PHY_OFF, PHY_OFF )) != ADI_ADF7030_1_SUCCESS)
            {
                return eResult;
            }
                    
            /* Send the Default usecase configuration to the part if specified */
            if((pDevInfo->pStartupCfg != NULL) && (pDevInfo->StartupCfgSize != 0UL)) 
            {
                if((eResult = adf7030_1__SendConfiguration( pSPIDevInfo,
                                                            (uint8_t *)pDevInfo->pStartupCfg,
                                                            pDevInfo->StartupCfgSize )) != ADI_ADF7030_1_SUCCESS)
                {
                    return eResult;
                }
            }
        }
        
        pDevice->eState |= ADI_ADF7030_1_STATE_ENABLED;
    }
    else
    {
      /* Place holder for turning off the adf7030-1 PHY */
        
        /* Clear interrupt enable register for IRQ0 */
        if(adf7030_1__IRQ_ClrAllStatus(pDevInfo, ADI_ADF7030_1_INTPIN0) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_DEV_FAILED;
        }
        
        /* Clear interrupt enable register for IRQ1 */
        if(adf7030_1__IRQ_ClrAllStatus(pDevInfo, ADI_ADF7030_1_INTPIN1) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_DEV_FAILED;
        }

        /* Execute user defined "disable" sequence */
        if(pDevInfo->pfDisableFunc)
        {
            if((eResult = pDevInfo->pfDisableFunc(pDevInfo)) !=  ADI_ADF7030_1_SUCCESS)
            {
                return eResult;
            }
        }
        else
        {
            /* Reset the PHY Radio on exit */
            if((eResult = adf7030_1__STATE_PhyShutdown(pSPIDevInfo)) != ADI_ADF7030_1_SUCCESS)
            {
                return eResult;
            }
        }
                
        /* Change driver state */
        pDevice->eState &= ~ADI_ADF7030_1_STATE_ENABLED;
    }
    
    return ADI_ADF7030_1_SUCCESS;
}

    
/**
 * @brief       Initialise the Host shadow configuration of the PHY Radio
 *
 * @note        This function initialise host shadow memory configuration buffer
 *              using a MEM_CFG_DESCRIPTOR_Type reference passed as argument.
 *              see USER_CONFIG_20.h file as a example. It setup the following
 *              parameters of the current instance:
 *                     - pDevInfo->pPROFILE__BLOCK
 *                     - pDevInfo->pPACKET_CFG__BLOCK
 *                     - pDevInfo->pTRX_PCKBUFF0__BLOCK
 *                     - pDevInfo->pTRX_PCKBUFF1__BLOCK
 *              If bInit is set, this function will automatically readback from
 *              the PHY Radio both radio profile and default generic packet 
 *              configuration. User should set this parameter if initialisation
 *              is done via a *.cfg file.
 *              
 * @param [in]  hDevice         Handle to ADF7030-1 device instance.
 * 
 * @param [in]  pCfgMem         Memory required for Host shadow phy configuration.
 *
 * @param [in]  nCfgMemSize     The size of the pCfgMem memory passed in bytes.
 *             
 * @param [in]  pCfgRef         Pointer to the MEM_CFG_DESCRIPTOR_Type reference
 *                              configuration file to use for initialisation.
 *
 * @param [in]  bInit           Boolean  -TRUE to populate pDevInfo->pPROFILE__BLOCK and pDevInfo->pPACKET_CFG__BLOCK
 *                                       -FALSE [D] to skip initialisation from PHY Radio content
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                    If PHY instance was succesfully setup
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED            [D] If Communication with the PHY Radio failed
 *  - #ADI_ADF7030_1_INSUFFICIENT_MEMORY        [D] If memory buffer is not sufficient to hold shadow configuration
 *  - #ADI_ADF7030_1_FAILURE                    [D] If PHY instance could not be opened or closed
*/
ADI_ADF7030_1_RESULT adf7030_1__SetupHostPhyCfg(
    ADI_ADF7030_1_HANDLE  const         hDevice,
    void * const                        pCfgMem,
    uint32_t                            nCfgMemSize,
    MEM_CFG_DESCRIPTOR_Type *           pCfgRef, 
    bool                                bInit
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;
    
    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO *pDevInfo = pDevice->pDevInfo;
    
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    /* Size of structure is equal to pCfgRef->Size (total size of data contained in all blocks)
       + 28bytes (header size)
       + ((sizeof(DATA_BLOCK_DESCRIPTOR_Type) + 4) * (16 blocks max)) ... 4 is to allow word alligned block start
     */
    if(nCfgMemSize <= (pCfgRef->Size + 28 + ( 16 * (sizeof(DATA_BLOCK_DESCRIPTOR_Type) + 4))))
    {
        return ADI_ADF7030_1_INSUFFICIENT_MEMORY;
    }
    
    /* Initialize header of the configuration */
    memcpy(pCfgMem, (const void *)pCfgRef, 28);
    
    /* Declare pointer to the new configuration structure */
    MEM_CFG_DESCRIPTOR_Type * pCfg = (MEM_CFG_DESCRIPTOR_Type *)pCfgMem;
    
    /* Remove pointer to info and nested structure if any */
    pCfg->pInfo = NULL;
    pCfg->pNext = NULL;
    
    /* Set the start address for first BLOCK.pData */
    uint32_t BlocAddr = (uint32_t)&pCfg->BLOCKS + (16 * sizeof(DATA_BLOCK_DESCRIPTOR_Type));
    
    /* Initialise each cfg block headers */
    for(uint32_t i = 0; i < pCfg->nbBlock; i++)
    {
        /* Copy each block description */
        memcpy((void *)&pCfg->BLOCKS[i], (const void *)&pCfgRef->BLOCKS[i], sizeof(DATA_BLOCK_DESCRIPTOR_Type));
        
        /* Change block data pointers to new ram location */        
        pCfg->BLOCKS[i].pData = (uint8_t *)BlocAddr;
        
        /* Clear each block data buffer */
        memset((void *)BlocAddr, 0, pCfg->BLOCKS[i].Size);
        
        BlocAddr +=  pCfg->BLOCKS[i].Size;
        
        /* Realign each pBloc on word boundaries */
        if((BlocAddr & 0x3ul) != 0)
        {
            BlocAddr = (((BlocAddr >> 2) + 1) << 2);
        }
    }
    
    /* Setup instance pointer to configuration structure */ 
    pDevInfo->pCfg = pCfg;
    
    /* Setup instance pointer to Host Shadow "Profile" structure */
    pDevInfo->pPROFILE__BLOCK = &pCfg->BLOCKS[9];
    
    /* Setup instance pointer to Host Shadow "Packet" structure */
    pDevInfo->pPACKET_CFG__BLOCK = &pCfg->BLOCKS[10];
       
    /* Setup instance pointer to Host Shadow "TX packet" buffer */
    pDevInfo->pTX_PCKBUFF__BLOCK = &pCfg->BLOCKS[14];

    /* Setup instance pointer to Host Shadow "RX packet" buffer */
    pDevInfo->pRX_PCKBUFF__BLOCK = &pCfg->BLOCKS[15];

    if(bInit == true)
    {
        /* Checking correct shadow profile size */
        if(pDevInfo->pPROFILE__BLOCK->Size != sizeof(ADI_PROFILE_Type))
        {
            return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
        }
    
        /* Reading PHY Radio profile location */
        uint32_t Profile_Addr;
        if(adf7030_1__Get_PhyProfileAddr(pDevInfo, &Profile_Addr ) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
        }
        
        /* Override PHY profile address with default value */
        pDevInfo->pPROFILE__BLOCK->Addr = Profile_Addr;
        
        /* Readback Profile data from PHY Radio */
        if(adf7030_1__ReadDataBlock(pSPIDevInfo, pDevInfo->pPROFILE__BLOCK) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_COMM_FAILED;
        }

        /* Checking correct shadow packet config address and size*/
        if(pDevInfo->pPACKET_CFG__BLOCK->Size != sizeof(ADI_GENERIC_PKT_Type))
        {
            return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
        }
    
        /* Reading PHY Radio packet config location */
        uint32_t PacketConfig_Addr;
        if(adf7030_1__Get_PhyPckConfigAddr(pDevInfo, &PacketConfig_Addr ) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
        }
    
        /* Override PHY packet config address with default value */
        pDevInfo->pPACKET_CFG__BLOCK->Addr = PacketConfig_Addr;
        
        /* Readback Packet configuration data from PHY Radio */
        if(adf7030_1__ReadDataBlock(pSPIDevInfo, pDevInfo->pPACKET_CFG__BLOCK) != ADI_ADF7030_1_SUCCESS)
        {
            return ADI_ADF7030_1_SPI_COMM_FAILED;
        }
    }

    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Initialise the Host shadow TRX payload buffer of the PHY Radio
 *
 * @note        This function is using host shadow memory packet configuration
 *              structure to initialise current instance parameters in:
 *                     - pDevInfo->pTRX_PCKBUFF0__BLOCK
 *                     - pDevInfo->pTRX_PCKBUFF1__BLOCK
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information.
 * 
 * @param [in]  pTrxMem         Memory required for Host shadow TRX buffer.
 *
 * @param [in]  nTrxMemSize     The size of the pTrxMem memory passed in bytes.      
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                    If PHY TRX payload buffer was succesfully setup
 *  - #ADI_ADF7030_1_INSUFFICIENT_MEMORY        If memory is not sufficient to hold PHY TRX payload buffer
 *  - #ADI_ADF7030_1_INVALID_PHY_CONFIGURATION  [D] If current PHY TRX payload layout is not a supported.
*/
ADI_ADF7030_1_RESULT adf7030_1__SetupHostTrxBuffer(
    ADI_ADF7030_1_INFO *                pDevInfo,
    void * const                        pTrxMem,
    uint32_t                            nTrxMemSize
)
{
    if(pDevInfo->pPACKET_CFG__BLOCK != NULL)
    {
        /* Pointer to Host Shadow Packet Configuration structure */
        ADI_GENERIC_PKT_Type * pGENERIC_PKT = (ADI_GENERIC_PKT_Type *)pDevInfo->pPACKET_CFG__BLOCK->pData;
        
        uint16_t ptr_tx_base = pGENERIC_PKT->BUFF_CFG0_b.PTR_TX_BASE;
        uint16_t ptr_rx_base = pGENERIC_PKT->BUFF_CFG0_b.PTR_RX_BASE;
        uint32_t PHY_Base_Addr = 0x20000000UL;
             
#ifdef __INTERNAL_DEV__ 
        // Get the TX buffer memory location and payload size
        if(pGENERIC_PKT->BUFF_CFG0_b.EXTENDED_BUFF_EN)
        {
        
            //Assign pTX_PCKBUFF__BLOCK to Extended shadow packet buffer
            pDevInfo->pTX_PCKBUFF__BLOCK->Size = pGENERIC_PKT->BUFF_CFG1_BIS_b.TRX_SIZE;
            pDevInfo->pTX_PCKBUFF__BLOCK->Addr = PHY_Base_Addr | (((pGENERIC_PKT->BUFF_CFG0_b.EXTENDED_BUFF_BASE_SEL) ? ptr_tx_base : ptr_rx_base) << 2 );
            pDevInfo->pTX_PCKBUFF__BLOCK->pData = (uint8_t *)pTrxMem;
            
            if(pDevInfo->pTX_PCKBUFF__BLOCK->Size > nTrxMemSize)
            {
                return ADI_ADF7030_1_INSUFFICIENT_MEMORY;
            }
            
            /* Clear Host TX payload buffer */
            memset((void *)pDevInfo->pTX_PCKBUFF__BLOCK->pData, 0, pDevInfo->pTX_PCKBUFF__BLOCK->Size);
        }
        else
#endif
        {    
            //Assign pTX_PCKBUFF__BLOCK to TX shadow packet buffer at 
            pDevInfo->pTX_PCKBUFF__BLOCK->Size = pGENERIC_PKT->BUFF_CFG1_b.TX_SIZE;
            pDevInfo->pTX_PCKBUFF__BLOCK->Addr = PHY_Base_Addr | (ptr_tx_base << 2);
            
            //Setup pData @ the midle of reserved Host memory
            pDevInfo->pTX_PCKBUFF__BLOCK->pData = (uint8_t *)pTrxMem;
            
            if(pDevInfo->pTX_PCKBUFF__BLOCK->Size > (nTrxMemSize >> 1))
            {
                return ADI_ADF7030_1_INSUFFICIENT_MEMORY;
            }
            
            /* Clear Host RX payload buffer */
            memset((void *)pDevInfo->pTX_PCKBUFF__BLOCK->pData, 0, pDevInfo->pTX_PCKBUFF__BLOCK->Size);

            //Assign pRX_PCKBUFF__BLOCK to RX shadow packet buffer
            pDevInfo->pRX_PCKBUFF__BLOCK->Size = pGENERIC_PKT->BUFF_CFG1_b.RX_SIZE;
            pDevInfo->pRX_PCKBUFF__BLOCK->Addr = PHY_Base_Addr | (ptr_rx_base << 2);
            
            //Setup pData @ the start of reserved Host memory
            pDevInfo->pRX_PCKBUFF__BLOCK->pData = (uint8_t *)((uint32_t)pTrxMem + (nTrxMemSize >> 1));
            
            if(pDevInfo->pRX_PCKBUFF__BLOCK->Size > (nTrxMemSize >> 1))
            {
                return ADI_ADF7030_1_INSUFFICIENT_MEMORY;
            }
            
            /* Clear Host RX payload buffer */
            memset((void *)pDevInfo->pRX_PCKBUFF__BLOCK->pData, 0, pDevInfo->pRX_PCKBUFF__BLOCK->Size);
        }
    }
    else
    {
        /* Operation not supported without host shadow memory */
        return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
    }
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Initialise the Host GPIO Trigger pin to interface with the PHY Radio
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information.
 * 
 * @param [in]  eTRIG           PHY Trigger PIN to configure.    
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                If Host GPIO has been configured for trigger functionality
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED        If Host GPIO configuration failed
*/
ADI_ADF7030_1_RESULT adf7030_1__EnableHostTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_TRIGPIN               eTRIG,
    bool                                bEnable
)
{
    /* Setup Host GPIO for trigger */
    ADI_ADF7030_1_GPIO_TRIG_INFO * pTrigGPIOInfo = &pDevInfo->TrigGPIOInfo[eTRIG];
             
    if(adi_gpio_SetLow(pTrigGPIOInfo->ePort, pTrigGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
    
    if(adi_gpio_OutputEnable(pTrigGPIOInfo->ePort, pTrigGPIOInfo->nPin, bEnable) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
    
    pTrigGPIOInfo->eTrigStatus = (bEnable == true) ? HOST_READY : NONE;
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Setup PHY gpio to for external trigger command
 *
 * @param [in]  pSPIDevInfo     Pointer to the SPI device info structure of the 
 *                              ADI RF Driver used to communicate with the
 *                              adf7030-1 PHY.
 *
 * @param [in]  TRIG            PHY Radio TRIG pin to be configured (enum ADI_ADF7030_1_TRIGPIN)
 *
 * @param [in]  GPIO            PHY Radio GPIO pin to be configured (enum ADI_ADF7030_1_GPIO)
 *
 * @param [in]  goState         PHY Radio State to execute. See ADI_ADF7030_1_RADIO_STATE
 *                              enum for available States.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS              If the Radio PHY reset was transfert to the adf7030-1.
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED      [D] If the communication with Radio PHY failed.
 *  - #ADI_ADF7030_1_INVALID_OPERATION    [D] If Host GPIO trigger has not been previously configured
 */
ADI_ADF7030_1_RESULT adf7030_1__SetupPhyTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_TRIGPIN               eTRIG,
    ADI_ADF7030_1_RADIO_STATE           goState
)
{
    /* Pointer to SPI dev info */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    /* Setup Host GPIO for trigger */
    ADI_ADF7030_1_GPIO_TRIG_INFO * pTrigGPIOInfo = &pDevInfo->TrigGPIOInfo[eTRIG];
 
    if(pTrigGPIOInfo->eTrigStatus >= HOST_READY)
    {
        /* Setup PHY Radio pinmux */
        adf7030_1__GPIO_SetCfg( pSPIDevInfo,
                                pTrigGPIOInfo->ePhyPin,
                                (eTRIG == ADI_ADF7030_1_TRIGPIN1) ? GPIO_TRIG_IN1 : GPIO_TRIG_IN0,
                                false );
        
        /* Setup trigger PHY Radio command */
        adf7030_1__SPI_SetField( pSPIDevInfo,
                                SM_CONFIG_GPIO_CMD_0_Addr + (eTRIG << 1),
                                0,
                                16, //Clear the cfg too.
                                (RADIO_CMD | (uint16_t)goState) );
        
        pTrigGPIOInfo->nTrigCmd = RADIO_CMD | (uint16_t)goState;
        pTrigGPIOInfo->eTrigStatus = PHY_READY;
    }
    else
    {
        /* Driver prevent setting PHY Radio GPIO unless Host GPIO has been configured */
        return ADI_ADF7030_1_INVALID_OPERATION;
    }
        
    /* Return SPI transfer status */
    return(pSPIDevInfo->eXferResult);  
}


ADI_ADF7030_1_RESULT adf7030_1__EnablePhyTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_RADIO_EXTENDED        ExCmd
)
{
    ADI_ADF7030_1_RESULT eResult;

    /* Pointer to SPI dev info */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;

    if((eResult = adf7030_1__STATE_PhyCMD_Ex(pSPIDevInfo, ExCmd)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
    
    /* Fix adf7030-1 bug when both triggers are enabled */
    if(ExCmd == TRIGPIN_ALL)
    {
        uint32_t fix = (*((uint8_t *)&pDevInfo->TrigGPIOInfo[ADI_ADF7030_1_TRIGPIN0].nTrigCmd + 1)) |
                       (*((uint8_t *)&pDevInfo->TrigGPIOInfo[ADI_ADF7030_1_TRIGPIN1].nTrigCmd + 1)) |
                       0x00000808;
        adf7030_1__SPI_SetField( pSPIDevInfo,
                                 0x40003818UL,
                                 0,
                                 16,
                                 fix );
        
        /* Return SPI transfer status */
        return(pSPIDevInfo->eXferResult); 
    }
    
    return ADI_ADF7030_1_SUCCESS; 
}
    

/**
 * @brief       Pulse Host GPIO Trigger pin
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information.
 * 
 * @param [in]  eTRIG           PHY Trigger PIN to configure.    
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                If Trigger GPIO has been pulsed
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED        If Trigger GPIO failed to pulse
*/
ADI_ADF7030_1_RESULT adf7030_1__PulseTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_TRIGPIN               eTRIG
)
{
    /* Setup pointer to Instance GPIO trigger info */
    ADI_ADF7030_1_GPIO_TRIG_INFO * pTrigGPIOInfo = &pDevInfo->TrigGPIOInfo[eTRIG];
    
    /* Toggle PHY Radio GPIO pin from the Glue Host */
    if(adi_gpio_SetHigh(pTrigGPIOInfo->ePort, pTrigGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
    
    if(adi_gpio_SetLow(pTrigGPIOInfo->ePort, pTrigGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Pulse Host GPIO Reset pin
 *
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information.
 *
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                If Reset GPIO has been pulsed
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED        If Reset GPIO failed to pulse
*/
ADI_ADF7030_1_RESULT adf7030_1__PulseReset(
	ADI_ADF7030_1_INFO *                pDevInfo
)
{
    /* Setup pointer to Instance GPIO reset info */
    ADI_ADF7030_1_GPIO_RESET_INFO * pResetGPIOInfo = &pDevInfo->ResetGPIOInfo;

    /* Pulse low PHY Radio GPIO pin from the Glue Host */
    if(adi_gpio_SetLow(pResetGPIOInfo->ePort, pResetGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    if(adi_gpio_SetHigh(pResetGPIOInfo->ePort, pResetGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Closes the adf7030-1 instance.
 *
 * @details     This function stops any PHY Radio operation, and closes the device, this
 *              puts the ADF7030-1 into phy_off mode.
 *
 * @param [in]  hDevice         Handle to ADF7030-1 device instance to be closed.
 *
 * @return      Status
 *
 *  - #ADI_ADF7030_1_SUCCESS                If successfully closed the ADF7030-1 device.
 *  - #ADI_ADF7030_1_INVALID_HANDLE    [D]  If the given ADF7030-1 device handle is invalid.
 *  - #ADI_ADF7030_1_DEVICE_NOT_OPENED [D]  if the given device is not yet opened.
 *  - #ADI_ADF7030_1_SPI_DEV_FAILED         Failed to terminate GPIO service.
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED        Failed to terminate SPI driver.
 *  - #ADI_ADF7030_1_FAILURE                Failed to disable Radio PHY instance.
 *
 * @sa          adf7030_1__Open()
 */
ADI_ADF7030_1_RESULT adf7030_1__Close(
    ADI_ADF7030_1_HANDLE  const         hDevice
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;

    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;

    /* Disable the current instance in case is not already done */
    if(pDevice->eState & ADI_ADF7030_1_STATE_ENABLED)
    {
        /* Disable the PHY instance */
        if((eResult = adf7030_1__Enable(hDevice, false)) != ADI_ADF7030_1_SUCCESS)
        {
            return eResult;
        }
    }

    /* Unitialize IRQ service */
    if(adf7030_1__IRQ_UnInit(hDevice) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
    
    /* Unitialize SPI driver */
    if(adf7030_1__SPI_UnInit(hDevice) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }
    
    /*
     *  Mark the device as closed
     */
#if defined (__ADUCM4050__)
    ADI_INT_STATUS_ALLOC();
#endif

    ADI_ENTER_CRITICAL_REGION();

    pDevice->eState =   ADI_ADF7030_1_STATE_NOT_OPENED;

    ADI_EXIT_CRITICAL_REGION();
    
    /* Clear pointer to parent device in closed device info structure */
    pDevice->pDevInfo->hDevice = NULL;

    return ADI_ADF7030_1_SUCCESS;
}
   

#ifdef ADI_DEBUG

ADI_ADF7030_1_RESULT ValidateHandle (
    ADI_ADF7030_1_DEVICE * pDevice
)
{
    uint32_t    nDevIdx;

    for(nDevIdx = 0u; nDevIdx < ADF7030_1_NUM_INSTANCES; nDevIdx++)
    {
        /* Check if the given device pointer matches any of the  instances */
        if(pDevice == &gADF7030_1_Dev[nDevIdx])
        {
            /* The given pointer matches an instance, now check if it is opened */
            if(pDevice->eState == ADI_ADF7030_1_STATE_NOT_OPENED)
            {
                return ADI_ADF7030_1_DEVICE_NOT_OPENED;
            }
            else
            {
                return ADI_ADF7030_1_SUCCESS;
            }
        }
    }

    return ADI_ADF7030_1_INVALID_HANDLE;
}
#endif /* ADI_DEBUG */


/**
 * @brief       Readback the PHY Radio profile location
 *
 * @note        This function is reading address location of the PHY Radio profile
 *              structure and return is value into normalised 32bits address space.
 *              This function does not handle SPI communication error.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information.
 * 
 * @param [out] pAddr           Pointer to the write back location for the PHY Radio profile location.      
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                    If PHY Radio profile location was succesfully readback. 
 *  - #ADI_ADF7030_1_INVALID_PHY_CONFIGURATION  [D] If PHY Radio profile location is invalid.
*/
ADI_ADF7030_1_RESULT adf7030_1__Get_PhyProfileAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t *                  pAddr
)
{
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    /* Readback PHY Profile address location */
    uint32_t PHY_Address = 0x20000000UL | (0x7FFF & adf7030_1__SPI_GetField( pSPIDevInfo, 0x200000faUL, 0, 16 ));

    /* Limit the range of valid locations for the PHY Profile structure */
    if((PHY_Address >= ADI_PROFILE_BASE) && ((PHY_Address + sizeof(ADI_PROFILE_Type)) <= ADI_GENERIC_PKT_BASE))
    {
        *pAddr = PHY_Address;
        
        return ADI_ADF7030_1_SUCCESS;
    }
    else
    {
        return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
    }
}


/**
 * @brief       Set the PHY Radio profile location
 *
 * @note        This function is writing the address location of the PHY Radio profile
 *              structure.
 *              This function does not handle SPI communication error.
 *              
 * @param [in]  pDevInfo       Pointer to the ADF7030-1 instance information.
 * 
 * @param [in]  Addr           Address location for the PHY Radio profile location.      
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                    If PHY Radio profile location was succesfully written. 
 *  - #ADI_ADF7030_1_INVALID_PHY_CONFIGURATION  [D] If PHY Radio profile location is invalid.
*/
ADI_ADF7030_1_RESULT adf7030_1__Set_PhyProfileAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t                    Addr
)
{
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    /* Limit the range of valid locations for the PHY Profile structure */
    if((Addr >= ADI_PROFILE_BASE) && ((Addr + sizeof(ADI_PROFILE_Type)) < ADI_GENERIC_PKT_BASE))
    {
        adf7030_1__SPI_SetField( pSPIDevInfo, 0x200000faUL, 0, 16, ((0x00007FFFUL & Addr) | 0x8000));
        
        return ADI_ADF7030_1_SUCCESS;
    }
    else
    {
        return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
    }
}


/**
 * @brief       Readback the PHY Packet configuration location
 *
 * @note        This function is reading address location of the PHY Packet configuration
 *              structure and return is value into normalised 32bits address space.
 *              This function does not handle SPI communication error.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information.
 * 
 * @param [out] pAddr           Pointer to the write back location for the PHY Packet configuration location.      
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                    If PHY Packet configuration location was succesfully readback. 
 *  - #ADI_ADF7030_1_INVALID_PHY_CONFIGURATION  [D] If PHY Packet configuration location is invalid.
*/
ADI_ADF7030_1_RESULT adf7030_1__Get_PhyPckConfigAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t *                  pAddr
)
{
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    uint32_t PHY_Address = 0x20000000UL | (adf7030_1__SPI_GetField( pSPIDevInfo, pDevInfo->pPROFILE__BLOCK->Addr + 0x10, 2, 11 ) << 2);

    /* Limit the range of valid locations for the PHY Packet configuration structure */
    if((PHY_Address >= ADI_GENERIC_PKT_BASE) && ((PHY_Address + sizeof(ADI_GENERIC_PKT_Type)) < 0x2000060CUL))
    {
        *pAddr = PHY_Address;
        
        return ADI_ADF7030_1_SUCCESS;
    }
    else
    {
        return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
    }
}


/**
 * @brief       Set the PHY Packet configuration location
 *
 * @note        This function is writing the address location of the PHY Packet configuration
 *              structure.
 *              This function does not handle SPI communication error.
 *              
 * @param [in]  pDevInfo       Pointer to the ADF7030-1 instance information.
 * 
 * @param [in]  Addr           Address location for the PHY Packet configuration location.      
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS                    If PHY Packet configuration location was succesfully written. 
 *  - #ADI_ADF7030_1_INVALID_PHY_CONFIGURATION  [D] If PHY Packet configuration location is invalid.
*/
ADI_ADF7030_1_RESULT adf7030_1__Set_PhyPckConfigAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t                    Addr
)
{
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    /* Limit the range of valid locations for the PHY Packet configuration structure */
    if((Addr >= ADI_GENERIC_PKT_BASE) && ((Addr + sizeof(ADI_GENERIC_PKT_Type)) < 0x2000060CUL))
    {
       adf7030_1__SPI_SetField( pSPIDevInfo, pDevInfo->pPROFILE__BLOCK->Addr + 0x10, 2, 11, ((Addr >> 2) & 0x07ffUL));
        
        return ADI_ADF7030_1_SUCCESS;
    }
    else
    {
        return ADI_ADF7030_1_INVALID_PHY_CONFIGURATION;
    }
}

#endif /* _ADF7030_1__CORE_C_ */

/** @} */ /* End of group adf7030-1__core Core Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */
