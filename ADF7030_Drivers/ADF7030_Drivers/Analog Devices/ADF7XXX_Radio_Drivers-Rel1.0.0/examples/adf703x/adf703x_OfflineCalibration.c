/*!
 *****************************************************************************
  @file:	adf703x_OfflineCalibration.c
  @brief:	This is a test file for adf7030_1 Radio driver development.
  @details:	This example demonstrates how to perform Offline calibration on the adf7030_1.
			Signal available on PHY Radio GPIOs:
                                - pin0 -> Not used
				- pin1 -> Not used
				- pin2 -> Not used
				- pin3 -> External IRQ0 - SM_IDLE_IRQn
				- pin4 -> External IRQ1 - Not used
				- pin5 -> Not used
				- pin6 -> Not used
				- pin7 -> Not used  
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

/** \addtogroup adf7030-1_examples_offline_calibrations ADF7030-1 Offline Calibrations
 *  @{
 */

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#if defined ( __ICCARM__ )
#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
	#include "system.h"
#elif defined (__ADUCM4050__)
        #include "system_ADuCM4050.h"
#endif
#elif defined ( __GNUC__ )
	#include <sys/platform.h>
	#include "adi_initialize.h"
#else
	#error Unknown compiler
#endif

#include "common.h"

/*! \cond PRIVATE */

/*!
   Define the PHY Radio main clock source.\n
   1 -  Use XTAL.\n
   0 -  Use TCXO.\n
*/
#define REF_CLOCK_OVERRIDE 1


/*! \endcond */

#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
#include <services/gpio/adi_gpio.h>
#include <services/pwr/adi_pwr.h>
#elif defined (__ADUCM4050__)
#include <drivers/gpio/adi_gpio.h>
#include <drivers/pwr/adi_pwr.h>
#endif

#include <drivers/spi/adi_spi.h>

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

//ADI debug format files...
//Customer should use "*.cfg" instead as this format is the only supported one.
#include <patch/CALIBRATIONS.h>
/*
KEEP_VAR(static const uint8_t ADI_OfflineCalibrations[]) = {
#include <patch/OfflineCalibrations.cfg>
};
*/

#include <config/Usecase_20/USER_CONFIG_20.h>

KEEP_VAR(static const uint8_t ADI_RF_CONFIG[]) = {
//#include <config/Usecase_20/CFG3_usecase_20.cfg>
//#include "TRX_168_test.cfg"
#include "TRX_168_test_125kbps_868MHz.cfg"
};

//Set the Default Usecase size
#define ADI_RF_CONFIG_Size = sizeof(ADI_RF_CONFIG)

  
/*  Boudrate divider for PCLK-26000000
 +------------------------------------------------------------------------+
 | CALCULATING UART DIV REGISTER VALUE FOR THE  INPUT CLOCK: 26000000     |
 |------------------------------------------------------------------------|
 |       BAUDRATE       DIV-C     DIV-M     DIV-N         OSR    DIFF     |
 |------------------------------------------------------------------------|
 |       00009600        0022      0003      1734        0003    0000     |
 |------------------------------------------------------------------------|
 |       00019200        0011      0003      1735        0003    0000     |
 |------------------------------------------------------------------------|
 |       00038400        0017      0001      0501        0003    0000     |
 |------------------------------------------------------------------------|
 |       00057600        0007      0002      0031        0003    0000     |
 |------------------------------------------------------------------------|
 |       00115200        0007      0002      0031        0002    0000     |
 |------------------------------------------------------------------------|
 |       00230400        0007      0002      0031        0001    0000     |
 |------------------------------------------------------------------------|
 |       00460800        0007      0002      0031        0000    0001     |
 |------------------------------------------------------------------------|
 */


/* Select the boudrate divider for 00460800 */
#define UART_DIV_C      7
#define UART_DIV_M      2
#define UART_DIV_N      31
#define UART_OSR        0


/* SPI instance and speed */
#define ADF7030_1_SPI_DEV_NUM               2
#define ADF7030_1_SPI_DEV_BITRATE           3000000
#define ADF7030_1_SPI_DEV_BITRATE_FAST      ADF7030_1_SPI_DEV_BITRATE << 1

/* CS port to which the CSN line of adf7030-1 is connected */ 
#define ADF7030_1_SPI_CS_NUM                ADI_SPI_CS3
   
/* GPIO port and pin to which default interrupt pin0 of adf7030-1 is connected */
//(adf7030_1 GPIO 3) --> P3-3 daughter board --> P0_01/GPIO01/SPI0_MOSI/SPT0_BFS (Glue EZ kit) 
#define ADF7030_1_INT0_GPIO_PORT                ADI_GPIO_PORT0
#define ADF7030_1_INT0_GPIO_PIN                 ADI_GPIO_PIN_1
#define ADF7030_1_INT0_GPIO_GROUP               SYS_GPIO_INTA_IRQn
#define ADF7030_1_INT0_GPIO_PHY_PIN             ADF7030_1_GPIO3

/* GPIO port and pin to which default interrupt pin1 of adf7030-1 is connected */
//(adf7030_1 GPIO 4) --> P3-17 daughter board --> P2_03/GPIO35/ADC0_VIN0 (Glue EZ kit) 
#define ADF7030_1_INT1_GPIO_PORT                ADI_GPIO_PORT2
#define ADF7030_1_INT1_GPIO_PIN                 ADI_GPIO_PIN_3
#define ADF7030_1_INT1_GPIO_GROUP               SYS_GPIO_INTA_IRQn
#define ADF7030_1_INT1_GPIO_PHY_PIN             ADF7030_1_GPIO4

/* GPIO port and pin to which default reset pin of the adf7030-1 is connected */
#define ADF7030_1_RESET_GPIO_PORT		ADI_GPIO_PORT0
#define ADF7030_1_RESET_GPIO_PIN		ADI_GPIO_PIN_3
   
/*!
 * Memory required for GPIO service
 */
uint8_t GPIOCallbackMem[ADI_GPIO_MEMORY_SIZE];

/*!
 * Memory required for SPI driver
 */
uint8_t SPIMem0[ADI_SPI_MEMORY_SIZE];
//uint8_t SPIMem1[ADI_SPI_MEMORY_SIZE];

/*!
 * SPI transfers buffers 
 */
uint8_t spi_txbuf[ADF7030_1_SPI_BUFFER_SIZE];
uint8_t spi_rxbuf[ADF7030_1_SPI_BUFFER_SIZE];

/*!
 * Memory required for ADF7030-1 driver
 */
uint8_t DeviceMem0[ADI_ADF7030_1_MEMORY_SIZE];
//uint8_t DeviceMem1[ADI_ADF7030_1_MEMORY_SIZE];

/*!
 * Optional memory to hold runtime shadow configuration of the Radio PHY
 */
uint8_t PHY_Mem[1024];

/*!
 * Optional memory to hold runtime Radio PHY packet payloads
 * within the Host memory space.
 */
uint8_t TRX_Mem[512];

/* Count for number of interrupts received from accelerometer */
static uint32_t nNumInt = 0;

/* Flag which indicates whether an interrupt is detected */
static volatile bool gbIntDetected = false;

/*! 
 * Default ADF7030_1 Driver instance information
 */
KEEP_VAR(ADI_ADF7030_1_DEVICE gADF7030_1_Dev[ADF7030_1_NUM_INSTANCES]) =
{
    {
      ADI_ADF7030_1_STATE_NOT_OPENED,
      NULL
    }

    /* Add more instances here */
};

/*!
 * Default ADF7030_1 SPI instance information data structure 
 */
KEEP_VAR(static const ADI_ADF7030_1_SPI_INFO DEFAULT_SPI0_INFO) = {
   
    /*! SPI Device number to be used for communicating with ADF7030_1 */
    .nDeviceNum = ADF7030_1_SPI_DEV_NUM,

    /*! Pointer to a 32 bit aligned buffer */
    .pDevMemory = &SPIMem0,
    
    /*! Size of buffer pointer by pDevMemory */
    .nMemorySize = ADI_SPI_MEMORY_SIZE,
    
    /*! Chip select number to address ADF7030_1 */
    .eChipSelect = ADF7030_1_SPI_CS_NUM,

    /*! SPI clock frequency for default communication */
    .nClkFreq = ADF7030_1_SPI_DEV_BITRATE,

#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)    
    /*! SPI clock frequency for fast SPI transfer */
    .nClkFreq_Fast = ADF7030_1_SPI_DEV_BITRATE_FAST,
    
    /*! Currently used SPI clock frequency */
    .nClkFreq_Current = 0,
#endif
    
    /*! SPI PHY pointer */
    .PHY_PNTR = { SPI_HOST_PNTR0_Addr,
                  0x20000000,
                  0x40004000,
                  0x40004100,
                  ADI_IRQ_CTRL_BASE,
                  0x20000000,
                  0x20000000,
                  0x20000000 },
                  
    /*! SPI TX transaction buffer */
    .pSPI_TX_BUFF = spi_txbuf,
    
    /*! SPI RX transaction buffer */
    .pSPI_RX_BUFF = spi_rxbuf,
    
    /*! SPI device handle */
    .hSPIDevice =  NULL,
    
    /*! SPI status from last transaction */
    .nStatus = 0,
    
    /*! PHY firmware state from last transaction */
    .nPhyState = PHY_SLEEP,

#if (ADF7030_1_PHY_ERROR_REPORT_ENABLE == 1)
    /*! Dynamic enable/disable of ePhyError checking */
    .bPhyErrorCheck = false,

    /*! PHY firmware error code */
    .ePhyError = SM_NOERROR
#endif
};

/*! 
 * Default ADF7030_1 GPIO Irq information data structure 
 */
KEEP_VAR(static const ADI_ADF7030_1_GPIO_INT_INFO DEFAULT_GPIO_INT[ADF7030_1_NUM_INT_PIN]) = {
  {
    /*! Interrupt IRQ */
    .eIRQType = ADF7030_1_INT0_GPIO_GROUP,
    
    /*! GPIO port to which the interrupt pin is connected */
    .ePort = ADF7030_1_INT0_GPIO_PORT,
    
    /*! GPIO pin within the GPIO port */
    .nPin = ADF7030_1_INT0_GPIO_PIN,
    
    /*! PHY Radio GPIO pin */
    .ePhyPin = ADF7030_1_INT0_GPIO_PHY_PIN,
    
    /*! Callback function pointer */
    .pfCallback = NULL,
    
    /*! Radio PHY interrupt mask configuration */
    .nIntMap = 0,
    
    /*! Last IRQ status */
    .nIntStatus = 0
  },
  {
    /*! Interrupt IRQ */
    .eIRQType = ADF7030_1_INT1_GPIO_GROUP,
    
    /*! GPIO port to which the interrupt pin is connected */
    .ePort = ADF7030_1_INT1_GPIO_PORT,
    
    /*! GPIO pin within the GPIO port */
    .nPin = ADF7030_1_INT1_GPIO_PIN,
    
    /*! PHY Radio GPIO pin */
    .ePhyPin = ADF7030_1_INT1_GPIO_PHY_PIN,
    
    /*! Callback function pointer */
    .pfCallback = NULL,
    
    /*! Radio PHY interrupt mask configuration */
    .nIntMap = 0,
    
    /*! Last IRQ status */
    .nIntStatus = 0
  }
};

 
/*!
 *  Default ADF7030_1 GPIO Reset information data structure
 */
KEEP_VAR(static const ADI_ADF7030_1_GPIO_RESET_INFO DEFAULT_GPIO_RESET) = {

    /*! Host GPIO port to which the interrupt pin is connected */
    .ePort = ADF7030_1_RESET_GPIO_PORT,

    /*! Host GPIO pin within the GPIO port */
    .nPin = ADF7030_1_RESET_GPIO_PIN
};


/*! Radio PHY device instance to be tested */
#define ADF7030_1_DEV_NUM       0


ADI_ADF7030_1_HANDLE hADF7030_1;



extern void adi_initpinmux(void);

/* Radio PHY device callback */
static void ADF7030_1_Callback(void *pCBParam, uint32_t Event, void *pArg)
{
    /* Pointer to Device Instance data */
    ADI_ADF7030_1_INFO * pDevInfo = (ADI_ADF7030_1_INFO *) pCBParam;
    
    nNumInt++;
    switch(Event)
    {
        /* Set the flag to indicate that an interrupt event is detected */
        case ADI_ADF7030_1_EVENT_INT0:
            gbIntDetected = true;
            break;
        
        case ADI_ADF7030_1_EVENT_INT1:
            gbIntDetected = true;
            break;
            
        case ADI_ADF7030_1_HOST_ERROR:
        	DBG("HOST_ERROR event registered...");
            break;

        case ADI_ADF7030_1_PHY_ERROR:
        	DBG("PHY_ERROR event 0x%02X registered...", pDevInfo->SPIInfo.ePhyError );
            break;

        default:
        	DBG("Unknown event registered...");
            break;
    }

    return;
}


ADI_ADF7030_1_RESULT MyEnable_Function (
    ADI_ADF7030_1_INFO *        pDevInfo
)
{
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;
    
    /* Setup startup configuration automatic download */
    pDevInfo->pStartupCfg = &ADI_RF_CONFIG[0];
    pDevInfo->StartupCfgSize = sizeof(ADI_RF_CONFIG);

    DBG("Executing user custom Enable function...\n\n");
    
    //----------------------------------------------------------------------

    DBG("+ Show current instance default SPI pointers\n");
    for(uint8_t i = PNTR_SETUP_ADDR; i <= PNTR_CUSTOM2_ADDR; i++)
    {
        DBG("-> ADF7030_1_SPI_PNTR[%d] = 0x%08x\n",i , pSPIDevInfo->PHY_PNTR[i]);
    }

    /* Setup PHY SPI pointers */
    /*if(adf7030_1__SPI_SetMMapPointers( pSPIDevInfo ) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_DEV_FAILED;
    }*/
    //----------------------------------------------------------------------

    DBG("\n+ Toggle the Reset pin the the PHY Radio...");
    if((eResult = adf7030_1__PulseReset( pDevInfo )) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }else{
        DBG("PASS\n");
    }

    /*! Wait for the part to be ready by issuing NOP. For some reason, the PHY
     *  radio returns an error SM_INVALID_STATE
     */
    /*if((eResult = adf7030_1__STATE_WaitReady( pSPIDevInfo, (uint8_t)255)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }*/
      
    /* Issue PHY_OFF and until in the state */
    DBG("+ Waiting for PHY Radio to be in PHY_OFF...");
    if((eResult = adf7030_1__STATE_PhyCMD_WaitReady( pSPIDevInfo, PHY_OFF, PHY_OFF)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }else{
    	DBG("PASS\n");
    }
           
    /* Send the Default usecase configuration to the part if specified */
    DBG("+ Downloading default configuration...");
    if((eResult = adf7030_1__SendConfiguration( pSPIDevInfo,
                                                (uint8_t *)pDevInfo->pStartupCfg,
                                                pDevInfo->StartupCfgSize )) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }else{
    	DBG("PASS\n");
    }
    
#if defined (REF_CLOCK_OVERRIDE)
    //Overide Main clock source... 
    adf7030_1__WRITE_FIELD(PROFILE_REF_CLK_CFG_CLK_TYPE, REF_CLOCK_OVERRIDE); //0->TCXO, 1->XTAL
#else
	#warning Main reference clock defined in PHY radio profile. Make sure it matches your hardware.
#endif

    //----------------------------------------------------------------------

    DBG("+ Initialising host shadow configuration...");
    if((eResult = adf7030_1__SetupHostPhyCfg( pDevInfo->hDevice,
                                              PHY_Mem,
                                              sizeof(PHY_Mem),
                                              &USER_CONFIG_20,
                                              true)) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }else{
    	DBG("PASS\n");
    }

#ifdef ADI_DEBUG
    /* Display debug info on profile */
    DISPLAY_PROFILE_INFO(pDevInfo->pPROFILE__BLOCK);

    /* Display debug info on packet */
    DISPLAY_PACKET_INFO(pDevInfo->pPACKET_CFG__BLOCK);
#endif

    //----------------------------------------------------------------------

    DBG("+ Initialising host shadow trx buffer...");
    if((eResult = adf7030_1__SetupHostTrxBuffer( pDevInfo,
                                                 &TRX_Mem,
                                                 sizeof(TRX_Mem) )) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }else{
    	DBG("PASS\n");
    }

    DBG("+ Readback current PHY RX payload buffer...");
    if((eResult = adf7030_1__ReadDataBlock( pSPIDevInfo,
                                            pDevInfo->pRX_PCKBUFF__BLOCK )) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }else{
    	DBG("PASS\nRX buff content:\n");
    	DISPLAY_BLOCK_CONTENT(pDevInfo->pRX_PCKBUFF__BLOCK, NULL, NULL);
    }

    DBG("+ Readback current PHY TX payload buffer...");
    if((eResult = adf7030_1__ReadDataBlock( pSPIDevInfo,
                                            pDevInfo->pTX_PCKBUFF__BLOCK )) != ADI_ADF7030_1_SUCCESS)
    {
    	return eResult;
    }else{
    	DBG("PASS\nTX buff content:\n");
    	DISPLAY_BLOCK_CONTENT(pDevInfo->pTX_PCKBUFF__BLOCK, NULL, NULL);
    }

    //----------------------------------------------------------------------

#if (ADF7030_1_PHY_ERROR_REPORT_ENABLE == 1)

    /* Enable automatic PhyError reporting from this point */
    pSPIDevInfo->bPhyErrorCheck = true;

#endif

    return ADI_ADF7030_1_SUCCESS;
}


ADI_ADF7030_1_RESULT MyDisable_Function (
    ADI_ADF7030_1_INFO *        pDevInfo
)
{
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;
    
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;
    
    DBG("... Executing user custom Disable function..."); 
    
    /* Shutdown the PHY Radio on exit */
    if((eResult = adf7030_1__STATE_PhyShutdown( pSPIDevInfo )) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }

    return ADI_ADF7030_1_SUCCESS;
}


void main(void)
{
  
#if defined ( __ICCARM__ )
  SystemInit();
  adi_initpinmux();

#elif defined ( __GNUC__ )
  adi_initComponents();

#else
  #error Unknown compiler
#endif

  test_Init();

  if(ADI_PWR_SUCCESS != adi_pwr_Init())
  {
      printf("Failed to intialize the power service\n");
      return;
  }
  if(ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_HCLK,1))
  {
      printf("Failed to intialize the power service\n");
      return;
  }
  if(ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_PCLK,1))
  {
      printf("Failed to intialize the power service\n");
      return;
  }
  
  if(ADI_PWR_SUCCESS != adi_pwr_UpdateCoreClock())
  {
      printf("Failed to update system clks\n");
      return;
  }
    
#ifdef REDIRECT_OUTPUT_TO_UART  
  if(ADI_UART_SUCCESS != adi_uart_ConfigBaudRate( hDevOutput,
                                                  UART_DIV_C,
                                                  UART_DIV_M,
                                                  UART_DIV_N,
                                                  UART_OSR))
  {
      printf("Failed to speed up system debug baudrate\n");
      return;
  };
#endif

  do
  { 

    /* Clear allocated buffer for PHY run-time configuration */
    memset((void *)&PHY_Mem, 0, 1024);
    
    //----------------------------------------------------------------------
    ADI_ADF7030_1_RESULT eResult;

    /* Initialise one instance of the adf7030-1 */
    DBG("\n\n--> Opening PHY Radio Driver instance..."); 
    if((eResult = adf7030_1__Open( ADF7030_1_DEV_NUM,
                                   DeviceMem0,
                                   ADI_ADF7030_1_MEMORY_SIZE,
                                   ADF7030_1_Callback,
                                   NULL,
                                   &hADF7030_1)) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(NULL, eResult);
        break;
    }else{
        DBG("PASS\n");
    }

    //----------------------------------------------------------------------

    DBG("--> Setup PHY Radio Driver instance..."); 
    if((eResult = adf7030_1__Setup( hADF7030_1,
                                    (ADI_ADF7030_1_SPI_INFO *)&DEFAULT_SPI0_INFO,
                                    (ADI_ADF7030_1_GPIO_INT_INFO *)&DEFAULT_GPIO_INT,
                                    NULL,
                                    (ADI_ADF7030_1_GPIO_RESET_INFO *)&DEFAULT_GPIO_RESET)) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(NULL, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
    
    //----------------------------------------------------------------------

    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hADF7030_1;
           
    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO * pDevInfo = pDevice->pDevInfo;
    
    /* Pointer to the ADF7030-1 instance SPI information */
    ADI_ADF7030_1_SPI_INFO * pSPIDevInfo = &pDevInfo->SPIInfo;

    //----------------------------------------------------------------------
    
    /* Setup custom defined power-up and power-down function pointers */
    pDevInfo->pfEnableFunc = &MyEnable_Function;
    pDevInfo->pfDisableFunc = &MyDisable_Function;

    DBG("--> Enable PHY Radio... ");
    if((eResult = adf7030_1__Enable( hADF7030_1, true )) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("\n");
    }
                   
    //----------------------------------------------------------------------       
                        
    /* Declare Host Shadow PROFILE structure */
    ADI_PROFILE_Type * pPROFILE = (ADI_PROFILE_Type *)pDevInfo->pPROFILE__BLOCK->pData;
   
    //----------------------------------------------------------------------
    
    /* Route PHY Radio irq pins to desired GPIO */
    adf7030_1__IRQ_SetGPIOPin(pDevInfo, ADI_ADF7030_1_INTPIN0);
    adf7030_1__IRQ_SetGPIOPin(pDevInfo, ADI_ADF7030_1_INTPIN1);
        
    //----------------------------------------------------------------------
         
    DBG("--> Issue CFG_DEV Command...");            
    if((eResult = adf7030_1__STATE_PhyCMD_WaitReady( pSPIDevInfo, CFG_DEV, PHY_OFF)) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }

    //----------------------------------------------------------------------
    
    DBG("--> Issue PHY_ON Command...");            
    if((eResult = adf7030_1__STATE_PhyCMD_WaitReady( pSPIDevInfo, PHY_ON, PHY_ON)) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }

    //----------------------------------------------------------------------
    
    DBG("\nTesting %s patch:\n", PATCH_CALIBRATIONS.Name);
    
    /* Transfers Offline calibration patch to the PHY Radio */
    DBG("--> Transfering %s (%d bytes) to the PHY...", PATCH_CALIBRATIONS.Name, PATCH_CALIBRATIONS.Size);
    if((eResult = adf7030_1__WritePatch( pSPIDevInfo,
                              	  	 &PATCH_CALIBRATIONS )) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
    	DBG("PASS\n");
    }
    
    /* Verifying Offline calibration patch integrity.
       This is only done as illustration purpose since we can just use
       adf7030_1__SelfCheckPatch() */
    DBG("--> Checking %s integrity...",PATCH_CALIBRATIONS.Name);
    if((eResult = adf7030_1__VerifyPatch( pSPIDevInfo,
                               	   	  &PATCH_CALIBRATIONS )) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
    	break;
    }else{
    	DBG("PASS\n");
    }
    
#ifdef ADI_DEBUG
    
    /* This section is optional and only needed to visualise */
    DATA_BLOCK_DESCRIPTOR_Type * pPATCH_CALIBRATIONS_InfoBlock = &PATCH_CALIBRATIONS.BLOCKS[PATCH_CALIBRATIONS.InfoBlockId];
    
    DBG("--> Transfert %s info data (%d bytes)...", PATCH_CALIBRATIONS.Name, pPATCH_CALIBRATIONS_InfoBlock->Size);

    if((eResult = adf7030_1__WritePatchBlock( pSPIDevInfo,
                                              pPATCH_CALIBRATIONS_InfoBlock)) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
    	break;
    }else{
    	DBG("PASS\n");
    }
    
    DBG("--> Verify %s info data (%d bytes)...", PATCH_CALIBRATIONS.Name, pPATCH_CALIBRATIONS_InfoBlock->Size);
    if((eResult = adf7030_1__VerifyPatchBlock( pSPIDevInfo,
                                               pPATCH_CALIBRATIONS_InfoBlock )) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
    	break;
    }else{     
        DBG("PASS\n");
    	DISPLAY_PATCH_INFO(PATCH_CALIBRATIONS.Name, (PATCH_INFO_Type *)pPATCH_CALIBRATIONS_InfoBlock->pData);
    }  
#endif

    //----------------------------------------------------------------------

    /* Change the default Offline calibration selfcheck CRC configuration */
    PATCH_DESCRIPTOR_Type * pPATCH_CALIBRATIONS = &PATCH_CALIBRATIONS;
    
    PATCH_CHECKSUM_DESCRIPTOR_Type CUSTOM_CHECKSUM;
    
    memcpy(&CUSTOM_CHECKSUM, pPATCH_CALIBRATIONS->pChecksum, pPATCH_CALIBRATIONS->pChecksum->Size);
    
    /* Enabling the automatic Offline calibration patch inject sequence if selfcheck is succesfull */
    CUSTOM_CHECKSUM.crc_config.inject_ok = 1;

    /* Initiate Offline calibration selfcheck */
    DBG("--> Perform %s patch selfcheck and auto-enable... ", PATCH_CALIBRATIONS.Name);            
    if((eResult = adf7030_1__SelfCheckPatch( pSPIDevInfo,
                                             &PATCH_CALIBRATIONS,
                                             false,
                                             ADI_ADF7030_1_INTPIN0,
                                             PHY_ON,
                                             &CUSTOM_CHECKSUM)) != ADI_ADF7030_1_SUCCESS)
    {
        DBG("FAIL\n");
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
    
    //----------------------------------------------------------------------

    adf7030_1__IRQ_GetMap(pDevInfo, ADI_ADF7030_1_INTPIN0);
    DBG("--> IRQ0 map = 0x%08x\n", pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN0].nIntMap);
        
    DBG("--> Clearing all event on IRQ0 pin...");   
    if((eResult = adf7030_1__IRQ_ClrAllStatus(pDevInfo, ADI_ADF7030_1_INTPIN0)) != ADI_ADF7030_1_SUCCESS )
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }

    DBG("--> Setting SM_IDLE_IRQn_Msk event to raise IRQ0 pin...");   
    if((eResult = adf7030_1__IRQ_SetMap( pDevInfo,
                              	  	 ADI_ADF7030_1_INTPIN0,
                                         SM_IDLE_IRQn_Msk )) != ADI_ADF7030_1_SUCCESS )
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
    
    adf7030_1__IRQ_GetMap(pDevInfo, ADI_ADF7030_1_INTPIN0);
    DBG("--> IRQ0 map = 0x%08x\n", pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN0].nIntMap);
        
    //----------------------------------------------------------------------
#ifdef ADI_DEBUG
    DBG("\nProfile content:\n");
    DISPLAY_BLOCK_CONTENT(pDevInfo->pPROFILE__BLOCK, NULL, NULL);
    
    DBG("\nCalibrations (pre-cal):\n");
    DISPLAY_BLOCK_CONTENT(NULL, &pPROFILE->RADIO_CAL_RESULTS0, 36);
    
    DISPLAY_CALIBRATION_ENABLES((ADI_ADF7030_1_RADIO_CALIBRATION)adf7030_1__SPI_GetMem32(pSPIDevInfo, PROFILE_RADIO_CAL_CFG0_Addr));
#endif    
    //----------------------------------------------------------------------
    
    gbIntDetected = false;
    
    DBG("--> Issue DO_CAL Command...");            
    if((eResult = adf7030_1__STATE_PhyCMD( pSPIDevInfo, DO_CAL)) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
        break;
    }
    
    /* Wait for interrupt */
    while(gbIntDetected == false) __WFE();

    /* ---> Calibration finished, should be PHY_ON idle now */
    
    /* Read IRQ status register and clear IRQ line */
    if((eResult = adf7030_1__IRQ_GetStatus( pDevInfo, ADI_ADF7030_1_INTPIN0 )) != ADI_ADF7030_1_SUCCESS )
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        gbIntDetected = false;       
        if((eResult = adf7030_1__IRQ_ClrStatus( pDevInfo, ADI_ADF7030_1_INTPIN0, pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN0].nIntStatus)) != ADI_ADF7030_1_SUCCESS )
        {
            CheckReturn(pSPIDevInfo, eResult);
            break;
        }
        DBG("Cal Finished... ");
    }
      
    /* Get Calibration result */  
    if(adf7030_1__READ_FIELD(PROFILE_RADIO_CAL_CFG1_CAL_SUCCESS) == PROFILE_RADIO_CAL_CFG1_CAL_SUCCESS_SUCCESSFUL_Eval){
        DBG("PASS\n");
    }else{
        DBG("FAIL\n");
    }

#ifdef ADI_DEBUG
    DBG("\nReadback Profile content:\n");
    if((eResult = adf7030_1__ReadDataBlock( pSPIDevInfo,
                                            pDevInfo->pPROFILE__BLOCK )) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("\nCalibrations (post-cal):\n");
        DISPLAY_BLOCK_CONTENT(NULL, &pPROFILE->RADIO_CAL_RESULTS0, 36);   
    }
#endif
    
    //----------------------------------------------------------------------

    DBG("--> Disable all events on IRQ0 pin...");   
    if((eResult = adf7030_1__IRQ_SetMap( pDevInfo,
                                         ADI_ADF7030_1_INTPIN0,
                                         0 )) != ADI_ADF7030_1_SUCCESS )
    {
    	CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
   
    //----------------------------------------------------------------------
    
    /* Disabling the Offline calibration patch */            
    DBG("--> Disabling %s patch...", PATCH_CALIBRATIONS.Name);
    if((eResult = adf7030_1__SetupPatch( pSPIDevInfo,
                               	   	 PATCH_CALIBRATIONS.EjectSeq,
                                         true )) != ADI_ADF7030_1_SUCCESS)
    {
    	DBG("FAIL\n");
        CheckReturn(pSPIDevInfo, eResult);
    	break;
    }else{
        DBG("PASS\n");
    }

    //----------------------------------------------------------------------

    DBG("--> Issue PHY_OFF Command...");            
    if((eResult = adf7030_1__STATE_PhyCMD_WaitReady( pSPIDevInfo, PHY_OFF, PHY_OFF)) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
    
    //----------------------------------------------------------------------
    
    DBG("--> Disabling PHY Radio...");  
    if((eResult = adf7030_1__Enable( hADF7030_1, false )) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }

    //----------------------------------------------------------------------

    DBG("--> Closing PHY Radio Driver instance..."); 
    if((eResult = adf7030_1__Close(hADF7030_1)) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
    
  }while(1);
}

/** @} */ /* End of group adf7030-1_examples_offline_calibrations ADF7030-1 Offline Calibrations */
/** @} */ /* End of group adf7030-1_examples Example Applications */

