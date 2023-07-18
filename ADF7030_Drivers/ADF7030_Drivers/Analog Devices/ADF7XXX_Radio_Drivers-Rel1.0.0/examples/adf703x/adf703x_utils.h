/*!
 *****************************************************************************
  @file:	adf703x_utils.h 
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


#ifndef _ADF703X_UTILS_H_
#define _ADF703X_UTILS_H_


/** \addtogroup adf7030-1_examples Example Applications
 *  @{
 */

/** \addtogroup adf7030-1_utils Auxillary Utils
 *  @{
 */

#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
  #define EZBOARD_PB0_GPIO_PORT				ADI_GPIO_PORT1
  #define EZBOARD_PB0_GPIO_PIN				ADI_GPIO_PIN_14

  #define EZBOARD_PB1_GPIO_PORT				ADI_GPIO_PORT2
  #define EZBOARD_PB1_GPIO_PIN				ADI_GPIO_PIN_6
#elif defined (__ADUCM4050__)
  #define EZBOARD_PB0_GPIO_PORT				ADI_GPIO_PORT2
  #define EZBOARD_PB0_GPIO_PIN				ADI_GPIO_PIN_9
  //#define EZBOARD_PB0_GPIO_PORT				ADI_GPIO_PORT1
  //#define EZBOARD_PB0_GPIO_PIN				ADI_GPIO_PIN_14

  #define EZBOARD_PB1_GPIO_PORT				ADI_GPIO_PORT2
  #define EZBOARD_PB1_GPIO_PIN				ADI_GPIO_PIN_6
#endif

extern ADI_UART_HANDLE hDevOutput;

#ifdef ADI_DEBUG

void CheckReturn(ADI_ADF7030_1_SPI_INFO * pSPIDevInfo,	ADI_ADF7030_1_RESULT eResult);

#define DBG(...) \
  do { \
    sprintf(aDebugString,__VA_ARGS__); \
    test_Display(aDebugString); \
  } while(0);
#else

#define CheckReturn(...) ;
#define DBG(...) ;

#endif

void test_Display(char *InfoString);

void CHECK( uint32_t condition );

void DISPLAY_PATCH_INFO(char * PatchName, version_info_t * pPathInfo);

void DISPLAY_PROFILE_INFO(DATA_BLOCK_DESCRIPTOR_Type * pPROFILE_BLOCK);

void DISPLAY_PACKET_INFO(DATA_BLOCK_DESCRIPTOR_Type * pPACKET_CFG__BLOCK);

void DISPLAY_BLOCK_CONTENT(DATA_BLOCK_DESCRIPTOR_Type * pDATA__BLOCK, void * pOffset, uint32_t size);

void DISPLAY_CALIBRATION_ENABLES(ADI_ADF7030_1_RADIO_CALIBRATION cal_enables);

uint32_t WAIT_PB(ADI_ADF7030_1_INFO * pDevInfo, ADI_ADF7030_1_TRIGPIN * ePB);


/** @} */ /* End of group adf7030-1_utils Auxillary Utils */
/** @} */ /* End of group adf7030-1_examples Example Applications */

#endif /* _ADF703X_UTILS_H_ */
