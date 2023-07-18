/*!
 *****************************************************************************
  @file:	adf703x_RX_packet.c
  @brief:	This is a test file for adf7030_1 Radio driver development.
  @details:	This example continuously receives a packet and monitor baseband rx data on the adf7030_1.
			Signal available on PHY Radio GPIOs:
				- pin0 -> Baseband TRX Clock
				- pin1 -> Baseband TRX Data
				- pin2 -> External LNA framing
				- pin3 -> External IRQ0 - Toggles after each EOF_IRQn event
				- pin4 -> External IRQ1 - Toggles after each PREAMBLE_IRQn or
                                                                             PREAMBLE_GONE_IRQn or
                                                                             SYNCWORD_IRQn or
                                                                             LENGTH_IRQn or
                                                                             PAYLOAD_IRQn or
                                                                             PAYLOAD_BLOC_IRQn or
                                                                             CRC_CHK_IRQn or
                                                                             EOF_IRQn
				- pin5 -> External TRIG0
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

/** \addtogroup adf7030-1_examples_rx_gpio ADF7030-1 GPIO triggered Abort Packet Reception
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

/*!
   Define whether the application override the default packet configuration.\n
   1 -  Custom packet configuration.\n
   0 -  Default configuration.\n
*/
#define PACKET_CONFIG_OVERRIDE 1


//#define __INTERNAL_DEV__


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
   
/* GPIO port and pin to which default trigger pin0 of the adf7030-1 is connected */
#define ADF7030_1_TRIG0_GPIO_PORT               ADI_GPIO_PORT2
#define ADF7030_1_TRIG0_GPIO_PIN                ADI_GPIO_PIN_8
#define ADF7030_1_TRIG0_GPIO_PHY_PIN            ADF7030_1_GPIO5
   
/* GPIO port and pin to which default trigger pin1 of the adf7030-1 is connected */
#define ADF7030_1_TRIG1_GPIO_PORT               0
#define ADF7030_1_TRIG1_GPIO_PIN                0
#define ADF7030_1_TRIG1_GPIO_PHY_PIN            0

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
 * Default ADF7030_1 GPIO Trig information data structure 
 */
KEEP_VAR(static const ADI_ADF7030_1_GPIO_TRIG_INFO DEFAULT_GPIO_TRIG[ADF7030_1_NUM_TRIG_PIN]) = {
  {
    /*! Host GPIO port to which the interrupt pin is connected */
    .eTrigStatus = NONE,

    /*! Host GPIO pin within the GPIO port */
    .ePort = ADF7030_1_TRIG0_GPIO_PORT,

    /*! PHY Radio GPIO pin */
    .nPin = ADF7030_1_TRIG0_GPIO_PIN,
    
    /*! PHY Radio Command to execute on trigger */
    .ePhyPin = ADF7030_1_TRIG0_GPIO_PHY_PIN,
    
    /*! Current trigger status */
    .nTrigCmd = 0
  },
  {
    /*! Host GPIO port to which the interrupt pin is connected */
    .eTrigStatus = NONE,

    /*! Host GPIO pin within the GPIO port */
    .ePort = ADF7030_1_TRIG1_GPIO_PORT,

    /*! PHY Radio GPIO pin */
    .nPin = ADF7030_1_TRIG1_GPIO_PIN,
    
    /*! PHY Radio Command to execute on trigger */
    .ePhyPin = ADF7030_1_TRIG1_GPIO_PHY_PIN,
    
    /*! Current trigger status */
    .nTrigCmd = 0
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


/* Count for number of transmitted packet from the PHY Radio */
static volatile uint32_t nEvent0 = 0;

/* Count for number of transmitted packet from the PHY Radio */
static volatile uint32_t nEvent1 = 0;

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
            nEvent0++;
            break;
        
        case ADI_ADF7030_1_EVENT_INT1:
            nEvent1++;
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

    //DBG("\n+ Toggle the Reset pin the the PHY Radio...");
    if((eResult = adf7030_1__PulseReset( pDevInfo )) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }else{
        //DBG("PASS\n");
    }

    /*! Wait for the part to be ready by issuing NOP. For some reason, the PHY
     *  radio returns an error SM_INVALID_STATE
     */
    /*if((eResult = adf7030_1__STATE_WaitReady( pSPIDevInfo, (uint8_t)255)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }*/

    /* Issue PHY_OFF and until in the state */
    //DBG("+ Waiting for PHY Radio to be in PHY_OFF...");
    if((eResult = adf7030_1__STATE_PhyCMD_WaitReady( pSPIDevInfo, PHY_OFF, PHY_OFF)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }else{
    	//DBG("PASS\n");
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


#if (PACKET_CONFIG_OVERRIDE == 1)

/*! Custom frame example */
typedef struct __FRAME_Type {
	uint8_t		Format;
	uint8_t		Seq_number;
	uint8_t         Payload[124];
	uint16_t 	Size;
} FRAME_Type;

char *data = "Greeting from the adf7030-1, Analog Devices\0";

#endif


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
                                    (ADI_ADF7030_1_GPIO_TRIG_INFO *)&DEFAULT_GPIO_TRIG,
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
    
    adf7030_1__WRITE_FIELD(PROFILE_RADIO_DIG_TX_CFG1_EXT_LNA_FRAMING_EN, 1);
    adf7030_1__WRITE_FIELD(PROFILE_RADIO_DIG_TX_CFG1_EXT_LNA_PIN_SEL, ADF7030_1_GPIO2);
    
    /* Route PHY Radio irq pins to desired GPIO */
    adf7030_1__IRQ_SetGPIOPin(pDevInfo, ADI_ADF7030_1_INTPIN0);
    adf7030_1__IRQ_SetGPIOPin(pDevInfo, ADI_ADF7030_1_INTPIN1);
    
    /* Route baseband TRX clock to GPIO 0 */
    adf7030_1__GPIO_PINCFG(ADF7030_1_GPIO0, SPORT_TRX_CLOCK);
    
    /* Route baseband TRX data to GPIO 1 */
    adf7030_1__GPIO_PINCFG(ADF7030_1_GPIO1, SPORT_RX_DATA);

    //----------------------------------------------------------------------
    
    /* Setup external trigger 0 on GPIO0 with PHY_ON command */
    DBG("--> Configure GPIO command for ON...");     
    if((eResult = adf7030_1__SetupPhyTrigger( pDevInfo,
                                              ADI_ADF7030_1_TRIGPIN0,
                                              PHY_ON )) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
    
    DBG("--> Enable GPIO trigger 0...");
    if((eResult = adf7030_1__EnablePhyTrigger(pDevInfo, TRIGPIN0_ONLY)) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
     
    //----------------------------------------------------------------------    
    
#ifdef __INTERNAL_DEV__
    /* Clear any loopback setting - just to be sure */
    adf7030_1__WRITE_FIELD(PROFILE_GPIO_CFG_GP_LOOP, 0x00);
    adf7030_1__WRITE_FIELD(PROFILE_GPIO_CFG_GP_LOOP_PADS, 0x00); 
    /* Change TRX transition time to 500us */
    adf7030_1__WRITE_FIELD(SM_CONFIG_TRANSITION_TIME_VAL, 0);
#else    
    /* Clear any loopback setting - just to be sure */
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x2000039CUL, 8, 8, 0);
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x2000039CUL, 0, 8, 0);
    /* Change TRX transition time to 500us */
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x200000F8UL, 0, 16, 500);
#endif /* __INTERNAL_DEV__ */
    
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
        
    DBG("--> Setting EOF_IRQn_Msk event to raise IRQ0 pin...");   
    if((eResult = adf7030_1__IRQ_SetMap( pDevInfo,
                                         ADI_ADF7030_1_INTPIN0,
                                         CRC_CHK_IRQn_Msk )) != ADI_ADF7030_1_SUCCESS )
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }

    DBG("--> Setting Packet event to raise IRQ1 pin...");   
    if((eResult = adf7030_1__IRQ_SetMap( pDevInfo,
                                         ADI_ADF7030_1_INTPIN1,
                                           PREAMBLE_IRQn_Msk | 
                                           PREAMBLE_GONE_IRQn_Msk |
                                           SYNCWORD_IRQn_Msk |
                                           LENGTH_IRQn_Msk |
                                           PAYLOAD_IRQn_Msk |
                                           PAYLOAD_BLOC_IRQn_Msk |
                                           CRC_CHK_IRQn_Msk |
                                           EOF_IRQn_Msk )) != ADI_ADF7030_1_SUCCESS )
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        DBG("PASS\n");
    }
        
    //----------------------------------------------------------------------
    
    adf7030_1__WRITE_FIELD(GENERIC_PKT_TEST_MODES0_TX_TEST, 0);
    
    uint32_t nb_rx_packet = 0xFFFF;

#ifdef __INTERNAL_DEV__
    /* Enable internal PER testmode */
    adf7030_1__WRITE_FIELD(GENERIC_PKT_TEST_MODES0_PER_EN, 1);
    /* Automatically clear external frame irqs */
    adf7030_1__WRITE_FIELD(GENERIC_PKT_TEST_MODES0_PER_IRQ_SELF_CLEAR, 1);
    /* Set the tx packet target count */
    adf7030_1__WRITE_FIELD(GENERIC_PKT_TEST_MODES1_PACKET_CNT, nb_rx_packet);
    /* Clear the current tx packet count */
    adf7030_1__WRITE_FIELD(GENERIC_PKT_TEST_MODES1_CURRENT_CNT, 0);
#else    
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x20000548UL, 0, 1, 1);
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x20000548UL, 12, 1, 1);
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x2000054cUL, 0, 16, nb_rx_packet);
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x2000054cUL, 16, 16, 0);
#endif /* __INTERNAL_DEV__ */
    
    //----------------------------------------------------------------------

#if (PACKET_CONFIG_OVERRIDE == 1)

    /* Setup our custom TX payload */
    FRAME_Type *pRXPCK = (FRAME_Type *)pDevInfo->pRX_PCKBUFF__BLOCK->pData;

    /* Clear any data into Host RX Payload Buffer */
    memset(pRXPCK, 0, pDevInfo->pRX_PCKBUFF__BLOCK->Size );

    DBG("--> Clearing up RX payload buffer...");
    if((eResult = adf7030_1__WriteDataBlock( pSPIDevInfo,
                                             pDevInfo->pRX_PCKBUFF__BLOCK)) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
    	DBG("PASS\nRX payload content:\n");
    	DISPLAY_BLOCK_CONTENT(pDevInfo->pRX_PCKBUFF__BLOCK, NULL, pDevInfo->pRX_PCKBUFF__BLOCK->Size);
    }

    //----------------------------------------------------------------------

	#define PARTIAL_PCK_CFG_SIZE (13 << 2)

    DBG("--> Readback current Packet Configuration...");
    if((eResult = adf7030_1__ReadDataBlock( pSPIDevInfo,
                                            pDevInfo->pPACKET_CFG__BLOCK)) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
    	DBG("PASS\n\n");
    	DISPLAY_BLOCK_CONTENT(pDevInfo->pTX_PCKBUFF__BLOCK, NULL, PARTIAL_PCK_CFG_SIZE);
    }

	DBG("--> Modifying Packet Configuration...\n\n");

    /* Declare Host Shadow Packet Configuration structure */
    ADI_GENERIC_PKT_Type * pGENERIC_PKT = (ADI_GENERIC_PKT_Type *)pDevInfo->pPACKET_CFG__BLOCK->pData;

    /* Configure Preamble */
    pGENERIC_PKT->FRAME_CFG1_b.PREAMBLE_UNIT = GENERIC_PKT_FRAME_CFG1_PREAMBLE_UNIT_BIT_PAIR_Eval;
    pGENERIC_PKT->FRAME_CFG0_b.PREAMBLE_LEN = 16;
    pGENERIC_PKT->FRAME_CFG2_b.PREAMBLE_VAL = 0x55;

    /* Configure Syncword 0 */
    pGENERIC_PKT->FRAME_CFG0_b.SYNC0_LEN = 24;
    pGENERIC_PKT->SYNCWORD0 = 0x7695A3;

    /* Configure Syncword 1 */
    pGENERIC_PKT->FRAME_CFG2_b.SYNC1_LEN = 0;
    pGENERIC_PKT->SYNCWORD1 = 0;

    /* Configure Payload */
#ifdef __INTERNAL_DEV__
    pGENERIC_PKT->BUFF_CFG1_b.RX_BUFF_RAWDATA = GENERIC_PKT_BUFF_CFG1_RX_BUFF_RAWDATA_DISABLED_Eval;
#endif /* __INTERNAL_DEV__ */

    pGENERIC_PKT->BUFF_CFG1_b.TX_BUFF_RAWDATA = GENERIC_PKT_BUFF_CFG1_TX_BUFF_RAWDATA_DISABLED_Eval;
    pGENERIC_PKT->FRAME_CFG2_b.LEN_SEL = GENERIC_PKT_FRAME_CFG2_LEN_SEL_LEN_16_Eval;
    //pGENERIC_PKT->FRAME_CFG1_b.PAYLOAD_SIZE = pTXPCK->Size;
    pGENERIC_PKT->FRAME_CFG2_b.ENDEC_MODE = GENERIC_PKT_FRAME_CFG2_ENDEC_MODE_CODE_NRZ_Eval;
    pGENERIC_PKT->BUFF_CFG0_b.BIT2AIR = GENERIC_PKT_BUFF_CFG0_BIT2AIR_MSB_FIRST_Eval;
    pGENERIC_PKT->BUFF_CFG1_b.TRX_BLOCK_SIZE = 1;

    /* Configure CRC */
    pGENERIC_PKT->FRAME_CFG2_b.CRC_SHIFT_IN_ZEROS = GENERIC_PKT_FRAME_CFG2_CRC_SHIFT_IN_ZEROS_DISABLE_Eval;
    pGENERIC_PKT->FRAME_CFG0_b.CRC_LEN = 16;
    pGENERIC_PKT->CRC_POLY = 0x3D65;
    pGENERIC_PKT->CRC_SEED = 0;
    pGENERIC_PKT->CRC_FINAL_XOR = 0xFFFF;

#ifdef __INTERNAL_DEV__
    pGENERIC_PKT->FRAME_CFG2_b.CRC_REFLECT_IN = 0;
    pGENERIC_PKT->FRAME_CFG2_b.CRC_REFLECT_OUTPUT_CRC = 0;
#endif /* __INTERNAL_DEV__ */

    DBG("--> Writeback configuration to Radio...");
    if((eResult = adf7030_1__WritePartialDataBlock( pSPIDevInfo,
                                                    pDevInfo->pPACKET_CFG__BLOCK,
                                                    0,
                                                    PARTIAL_PCK_CFG_SIZE)) != ADI_ADF7030_1_SUCCESS)
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
    	DBG("PASS\n\n");
    	DISPLAY_BLOCK_CONTENT(pDevInfo->pTX_PCKBUFF__BLOCK, NULL, PARTIAL_PCK_CFG_SIZE);
    }

#else

    /* Advance mode to generate trx_block interrupt every 32bits */
    adf7030_1__WRITE_FIELD(GENERIC_PKT_BUFF_CFG1_TRX_BLOCK_SIZE, 4);

#endif

    //----------------------------------------------------------------------
        
    /* set GPIO input */
    if(ADI_GPIO_SUCCESS != adi_gpio_InputEnable(EZBOARD_PB0_GPIO_PORT, EZBOARD_PB0_GPIO_PIN, true))
    {
        DEBUG_MESSAGE("adi_gpio_InputEnable failed\n");
        break;
    }
    
    /* Start RX */
    DBG("--> Start PER RX testmode\n");            
    if((eResult = adf7030_1__STATE_PhyCMD_WaitReady( pSPIDevInfo, PHY_RX, PHY_RX)) != ADI_ADF7030_1_SUCCESS)
    {
    	CheckReturn(pSPIDevInfo, eResult);
        break;
    }
       
    //----------------------------------------------------------------------

    DBG("\n*** Press PB0 to reinitialise the radio ***\n\n");

    uint16_t Data;
    uint32_t nEvent0_zm1 = nEvent0;
    bool bExit = false;
    
    int32_t acc_rssi = 0;
        
    /* Continuous RX polling loop - Wait until "nb_rx_packet" packets have been received */
    do{        

        /* poll PB0 */
        if(ADI_GPIO_SUCCESS != adi_gpio_GetData(EZBOARD_PB0_GPIO_PORT, EZBOARD_PB0_GPIO_PIN, &Data))
        {
            DEBUG_MESSAGE("adi_gpio_GetData failed\n");
            break;
        }else{
            if(!Data)
            {
                /* Toggle PHY Radio GPIO Trigger 0 from the Glue Host */
                adf7030_1__PulseTrigger(pDevInfo, ADI_ADF7030_1_TRIGPIN0);
                bExit = true;
            }
        }
        
        /* Poll new full packet received */
        if(nEvent0 != nEvent0_zm1)
        {
        	uint32_t tmp_nEvent1 = nEvent1;

        	nEvent1 = 0;

#if (PACKET_CONFIG_OVERRIDE == 1)
            /* Clear any data into Host RX Payload Buffer */
            memset(pRXPCK, 0, pDevInfo->pRX_PCKBUFF__BLOCK->Size );

            /* Readback received frame length from PHY Radio */
            uint32_t frame_len = adf7030_1__READ_FIELD(GENERIC_PKT_FRAME_CFG3_RX_LENGTH);

            /* Update our custom frame structure Size field (-2 bytes of CRC) */
            pRXPCK->Size = frame_len - 2;

            /* Readback received frame data */
            if((eResult = adf7030_1__ReadPartiaDataBlock( pSPIDevInfo,
                                                          pDevInfo->pRX_PCKBUFF__BLOCK,
                                                          0,
                                                          frame_len)) != ADI_ADF7030_1_SUCCESS)
            {
                CheckReturn(pSPIDevInfo, eResult);
                break;
            }

            /* Display received Frame */
            DBG("\n0x%02X | 0x%02X | ", pRXPCK->Format, pRXPCK->Seq_number);
            for(int i = 0; i < pRXPCK->Size - 2; i++) DBG("%c", pRXPCK->Payload[i]);
            DBG(" | ");
#endif

            /* PHY Radio RSSI fixpoint format Q9.2 */
            int32_t signed_rssi = (adf7030_1__READ_FIELD(GENERIC_PKT_LIVE_LINK_QUAL_RSSI) << (32 - 11));
            signed_rssi >>= (32 - 11);

            /* Accumulate the RSSI for averaging */
            acc_rssi += signed_rssi;

            int32_t sign = signed_rssi >> 31;
            uint32_t abs_rssi = (sign + signed_rssi) ^ sign;

            /* Output received packet RSSI */
            DBG("%+03d\.%02d dBm, %d irq1", (signed_rssi >> 2), (abs_rssi & 3) * 25, tmp_nEvent1);

            nEvent0_zm1 = nEvent0;
        }
    }while((nEvent0 < nb_rx_packet) && (bExit == false));
       
    //----------------------------------------------------------------------

    /* Check we are back in PHY_ON */ 
    if((eResult = adf7030_1__STATE_WaitReady( pSPIDevInfo, 10)) != ADI_ADF7030_1_SUCCESS)  
    {
        CheckReturn(pSPIDevInfo, eResult);
        break;
    }else{
        //avg_rssi = (int32_t)div(avg_rssi,nEvent0);
        
        div_t tmp = div(acc_rssi,nEvent0);
        int32_t avg_rssi = tmp.quot;
        int32_t sign = avg_rssi >> 31;
        uint32_t abs_avg_rssi = (sign + avg_rssi) ^ sign;
        
        
        DBG("\nPASS... %d packets received @ avg rssi of %+03d\.%02d dBm\n", nEvent0, avg_rssi >> 2, (abs_avg_rssi & 3) * 25);
        nEvent0 = 0;
        nEvent1 = 0;
    }

    //----------------------------------------------------------------------

#ifdef __INTERNAL_DEV__ 
    /* Disable internal PER testmode */
    adf7030_1__WRITE_FIELD(GENERIC_PKT_TEST_MODES0_PER_EN, 0);
    /* Disable automatically clearing of external frame irqs */
    adf7030_1__WRITE_FIELD(GENERIC_PKT_TEST_MODES0_PER_IRQ_SELF_CLEAR, 0);
#else  
    adf7030_1__SPI_SetField( pSPIDevInfo, 0x20000548UL, 0, 32, 0);
#endif /* __INTERNAL_DEV__ */
    
    
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

/** @} */ /* End of group adf7030-1_examples_rx_gpio ADF7030-1 GPIO triggered Abort Packet Reception */
/** @} */ /* End of group adf7030-1_examples Example Applications */

