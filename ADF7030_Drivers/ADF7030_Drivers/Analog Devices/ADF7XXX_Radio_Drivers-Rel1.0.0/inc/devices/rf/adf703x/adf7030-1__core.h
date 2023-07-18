/*!
 *****************************************************************************
  @file:	adf7030-1__core.h 
  @brief:	...  
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


#ifndef _ADF7030_1__CORE_H_
#define _ADF7030_1__CORE_H_


/** \addtogroup adf7030-1 adf7030-1 Driver
 *  @{
 */

/** \addtogroup adf7030-1__core Core Interface
 *  @{
 */

/*! Memory required by the ADF7030_1 driver including the memory required for its dependent drivers */
#define ADI_ADF7030_1_MEMORY_SIZE  (sizeof(ADI_ADF7030_1_INFO)) //(28U)

/*! \cond PRIVATE */ 


/*! \endcond */

ADI_ADF7030_1_RESULT adf7030_1__Open(
    uint32_t                            nDeviceNum,
    void * const                        pDeviceMemory,
    uint32_t                            nMemSize,
    ADI_CALLBACK                        pfCallback,
    void * const                        pCBParam,
    ADI_ADF7030_1_HANDLE * const        phDevice
);

ADI_ADF7030_1_RESULT adf7030_1__Close(
    ADI_ADF7030_1_HANDLE const          hDevice
);

ADI_ADF7030_1_RESULT adf7030_1__Setup(
    ADI_ADF7030_1_HANDLE  const				hDevice,
    ADI_ADF7030_1_SPI_INFO * const			pSPIDevInfo,
    ADI_ADF7030_1_GPIO_INT_INFO *  const	pINTDevInfo,
    ADI_ADF7030_1_GPIO_TRIG_INFO * const	pTRIGDevInfo,
	ADI_ADF7030_1_GPIO_RESET_INFO * const	pRESETDevInfo
);

ADI_ADF7030_1_RESULT adf7030_1__SetupHostPhyCfg(
    ADI_ADF7030_1_HANDLE  const         hDevice,
    void * const                        pCfgMem,
    uint32_t                            nCfgMemSize,
    MEM_CFG_DESCRIPTOR_Type *           pCfgRef,
    bool                                bInit
);

ADI_ADF7030_1_RESULT adf7030_1__SetupHostTrxBuffer(
    ADI_ADF7030_1_INFO *                pDevInfo,
    void * const                        pTrxMem,
    uint32_t                            nTrxMemSize
);

ADI_ADF7030_1_RESULT adf7030_1__EnableHostTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_TRIGPIN               eTRIG,
    bool                                bEnable
);
      
ADI_ADF7030_1_RESULT adf7030_1__SetupPhyTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_TRIGPIN               eTRIG,
    ADI_ADF7030_1_RADIO_STATE           goState
);

ADI_ADF7030_1_RESULT adf7030_1__EnablePhyTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_RADIO_EXTENDED        ExCmd
);

ADI_ADF7030_1_RESULT adf7030_1__PulseTrigger(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_TRIGPIN               eTRIG
);

ADI_ADF7030_1_RESULT adf7030_1__PulseReset(
	ADI_ADF7030_1_INFO *                pDevInfo
);

ADI_ADF7030_1_RESULT adf7030_1__Enable(
    ADI_ADF7030_1_HANDLE const          hDevice,
    bool                                bEnable
);

/*ADI_ADF7030_1_RESULT adf7030_1__ResetDevice(
    ADI_ADF7030_1_HANDLE const          hDevice
);

ADI_ADF7030_1_RESULT adf7030_1__Hibernate(
    ADI_ADF7030_1_HANDLE const          hDevice
);*/

#ifdef ADI_DEBUG
ADI_ADF7030_1_RESULT ValidateHandle (
    ADI_ADF7030_1_DEVICE *              pDevice
);
#endif
    
                                            
ADI_ADF7030_1_RESULT adf7030_1__Get_PhyProfileAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t *                  pAddr
);

ADI_ADF7030_1_RESULT adf7030_1__Set_PhyProfileAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t                    Addr
);

ADI_ADF7030_1_RESULT adf7030_1__Get_PhyPckConfigAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t *                  pAddr
);

ADI_ADF7030_1_RESULT adf7030_1__Set_PhyPckConfigAddr(
    ADI_ADF7030_1_INFO *        pDevInfo,
    uint32_t                    Addr
);


/** @} */ /* End of group adf7030-1__core Core Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */

#endif /* _ADF7030_1__CORE_H_ */
