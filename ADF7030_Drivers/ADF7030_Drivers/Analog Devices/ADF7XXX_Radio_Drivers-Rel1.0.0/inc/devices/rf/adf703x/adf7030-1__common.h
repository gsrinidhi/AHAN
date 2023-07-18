/*!
 *****************************************************************************
  @file:	adf7030-1__common.h 
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


#ifndef _ADF7030_1__COMMON_H_
#define _ADF7030_1__COMMON_H_


/*
 * Macros related to alignment in the different toolchains supported.
 * This is an expended version of adi_drivers_general.h of the aducm4050.
 */

/*
 * These macros are designed to be used as follows:
 * ADI_ALIGNED_PRAGMA(<required_alignment>)
 * <variable_declaration> ADI_ALIGNED_ATTRIBUTE(<required_alignment>)
 */

#if defined ( __ICCARM__ )
/*
* IAR MISRA C 2004 error suppressions.
*
*
* Pm120 (rule 19.10): In the definition of a function-like macro each parameter
*                     shall be enclosed in parenthesis.
*                     This is not possible in attributes and pragmas
* Pm154 (rule 19.13): The # and ## preprocessor operators shall not be used.
*                     We need to do this to abstract the macros for the
*                     different toolchains
*/
#pragma diag_suppress=Pm120,Pm154
#endif

#define PRAGMA(x) _Pragma(#x)
#define ATTRIBUTE(x) __attribute__((x))

#if defined (__GNUC__)
  /* Gcc uses attributes */
  #define ADI_ALIGNED_PRAGMA(num)
  #define ADI_ALIGNED_ATTRIBUTE(num) ATTRIBUTE(aligned(num))
  #define ADI_UNUSED_ATTRIBUTE ATTRIBUTE(unused)

  #define KEEP_VAR(var) var ATTRIBUTE(used)

#elif defined ( __ICCARM__ )
  /* IAR uses a pragma */
  #define ADI_ALIGNED_ATTRIBUTE(num)
  #define ADI_ALIGNED_PRAGMA(num) PRAGMA(data_alignment=num)
  #define ADI_UNUSED_ATTRIBUTE

  #define KEEP_VAR(var) __root var

#elif defined (__CC_ARM)
  /* Keil uses a decorator which is placed in the same position as pragmas */
  #define ADI_ALIGNED_ATTRIBUTE(num)
  #define ADI_ALIGNED_PRAGMA(num) __align(##num)
  #define ADI_UNUSED_ATTRIBUTE ATTRIBUTE(unused)

  #define KEEP_VAR(var) var ATTRIBUTE(used)

#else
#error "Toolchain not supported"
#endif


#if defined ( __ICCARM__ )
#pragma diag_default=Pm120,Pm154
#endif

/*

#if defined (__CC_ARM)
	//#error Unimplemented KEEP_VAR
	#define KEEP_VAR(var)                  var __attribute__((used))

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
	#define KEEP_VAR(var)                  var __attribute__((used))

#elif defined (__GNUC__)
	#define KEEP_VAR(var)                  var __attribute__((used))

#elif defined (__ICCARM__)
	#define KEEP_VAR(var)                  __root var

#else
	#error "Toolchain not supported"
#endif
*/

/** \addtogroup adf7030-1 adf7030-1 Driver
 *  @{
 */

/** \addtogroup adf7030-1_headers Headers
 *  @{
 */

/** \addtogroup adf7030-1_common Global
 *  @{
 */

#include <drivers/spi/adi_spi.h>

#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
#include <services/gpio/adi_gpio.h>
#elif defined (__ADUCM4050__)
#include <drivers/gpio/adi_gpio.h>
#endif

/************* Radio Driver features ***************/

/*!
   Enable PHY Radio error reporting support in the driver code.\n
   1 -  To have the Error reporting code.\n
   0 -  To eliminate Error reporting code.
*/
#define ADF7030_1_PHY_ERROR_REPORT_ENABLE	1

/*!
   If PHY Radio error reporting is enabled, this select the reporting scheme to be used.\n
   1 -  IRQ Error reporting code.\n
   0 -  SPI Polling Error reporting code.
*/
#define ADF7030_1_PHY_ERROR_REPORT_SCHEME	0

/*!
   If enabled, this overwrite the SPI API scheme for short size transfers.\n
   see ADF7030_1_SPI_FAST_SIZE_THRESHOLD parameter for size threshold.\n
   1 -  Use custom fast SPI API.\n
   0 -  Keep standart SPI API.
*/
#define ADF7030_1_SPI_FAST_SHORTXFER_ENABLE	1

/************* Radio Driver configurations ***************/

/*!
 *  Defines the number of ADF7030-1 Radio instances to support
 *  in the driver.
 */
#define ADF7030_1_NUM_INSTANCES         	1

/*!
 *  Defines the number of interrupt pins connected between the
 *  MCU and the PHY Radio
 */
#define ADF7030_1_NUM_INT_PIN               2   

/*!
 *  Defines the number of trigger pin connected between the
 *  MCU and the PHY Radio
 */
#define ADF7030_1_NUM_TRIG_PIN              2   

/*!
 *  Defines size of SPI rx and tx buffers used to communicate with the PHY Radio.
 */
#define ADF7030_1_SPI_BUFFER_SIZE 256u

/*!
 *  Defines the maximum size of an SPI transaction.
 *  The Radio drivers requires a minimum of 8bytes used for command framing
 *  during word transfer operations.
 *  For instance, a 256 bytes buffer allows:
 *  - up to 62 words (32bits) per SPI transaction.
 *  - up to 252 bytes (8bits) per SPI transaction.
 */
#define ADF7030_1_SPI_MAX_TRX_SIZE (ADF7030_1_SPI_BUFFER_SIZE - 8u)

/*!
 *  Defines the maximum numbers of byte transfert for which "Fast SPI mode" will
 *  be used. Above this number, standart SPI API is used.
 */
#define ADF7030_1_SPI_FAST_SIZE_THRESHOLD 20
   
/*!
 *  Defines the numbers of SPI pointers address the driver will keep to schedule
 *  SPI transactions.
 */
#define ADF7030_1_SPI_PNTR_NUM 8
  

/*! Enumeration of return codes from ADF7030_1 driver.
 *
 */
typedef enum
{
    /*! The API is successful. */
    ADI_ADF7030_1_SUCCESS,

    /*! The API failed to complete the operation. */
    ADI_ADF7030_1_FAILURE,

    /*! A hardware error was detected. */
    ADI_ADF7030_1_HW_ERROR,

    /*! The given device number is outside the number of instances supported by the driver.*/
    ADI_ADF7030_1_INVALID_DEVICE_NUM,

    /*! The given device handle is invalid. */
    ADI_ADF7030_1_INVALID_HANDLE,

    /*! The given memory is not sufficient to operate the device. */
    ADI_ADF7030_1_INSUFFICIENT_MEMORY,

    /*! One of the given pointer parameter points to NULL */
    ADI_ADF7030_1_NULL_POINTER,

    /*! The given pointer to the device memory is not aligned to word boundary. */
    ADI_ADF7030_1_UNALIGNED_MEM_PTR,

    /*! The given device instance is already opened. */
    ADI_ADF7030_1_DEVICE_ALREADY_OPENED,

    /*! The given device instance is not opened. */
    ADI_ADF7030_1_DEVICE_NOT_OPENED,

    /*! The given operation is not permitted in the current state of the driver. */
    ADI_ADF7030_1_INVALID_OPERATION,

    /*! Failed to read or write the device registers. */
    ADI_ADF7030_1_DEVICE_ACCESS_FAILED,

    /*! Failed to register or unregister callback with GPIO service */
    ADI_ADF7030_1_GPIO_DEV_FAILED,

    /*! SPI device driver failure. */
    ADI_ADF7030_1_SPI_DEV_FAILED,

    /*! SPI device driver polling timeout. */
    ADI_ADF7030_1_SPI_DEV_POLL_EXPIRE,

    /*! SPI Communication device related failure. */
    ADI_ADF7030_1_SPI_COMM_FAILED,
    
    /*! The given PHY Radio configuration is invalid */
    ADI_ADF7030_1_INVALID_PHY_CONFIGURATION,

    /*! If trying to perform a read when another read/write in progress. */
    ADI_ADF7030_1_PENDING_IO,

    /*! If trying to enable the DMA mode when communication device is TWI. */
    ADI_ADF7030_1_DMA_NOT_SUPPORTED

} ADI_ADF7030_1_RESULT;


/*! Enumeration of different device or driver states
 *
 */
typedef enum
{
    /*! The device is not yet opened */
    ADI_ADF7030_1_STATE_NOT_OPENED = 0,

    /*! The device is opened */
    ADI_ADF7030_1_STATE_OPENED = 1,
    
    /*! The device is enabled */
    ADI_ADF7030_1_STATE_ENABLED = 2,

    /*! The device is opened, underlying drivers have been initialized */
    ADI_ADF7030_1_STATE_INITIALIZED = 4,
      
    /*! The device is awake and its profile has been applied */
    ADI_ADF7030_1_STATE_CONFIGURED = 8,
    
    /*! The device is awake and offline calibrations has been performed */
    ADI_ADF7030_1_STATE_CALIBRATED = 16,
    
    /*! The device is in Sleep mode */
    ADI_ADF7030_1_STATE_SLEEP = 32,

    /*! The device is awake and ready to perform Receive or Transmit operation mode */
    ADI_ADF7030_1_STATE_READY = 64,

    /*! The device is in transmit state */
    ADI_ADF7030_1_STATE_TRANSMITTING = 128,
    
    /*! The device is in receive state */
    ADI_ADF7030_1_STATE_RECEIVING = 256,
    
    /*! The device is in script execution mode */
    ADI_ADF7030_1_STATE_SEQUENCE_EXEC = 512,
    
} ADI_ADF7030_1_STATE;


/*! Enumeration of interrupts pins.
 *
 * The ADF7030-1 has two interrupt pin outs to the host processor. The host
 * can configure each of them independently using adf7030_1__IRQ_SetMap
 *
 * */
typedef enum
{
    /*! The interrupt output pin 0. */
    ADI_ADF7030_1_INTPIN0 =   0x00,          
    
    /*! The interrupt output pin 1. */    
    ADI_ADF7030_1_INTPIN1 =   0x01            

} ADI_ADF7030_1_INTPIN;


/*! Enumeration of external trigger pins.
 *
 * The ADF7030-1 has two interrupt pin which can trigger state transition
 * in place of a PHY Radio command through the SPI.
 *
 * */
typedef enum
{
    /*! The trigger intput pin 0. */
    ADI_ADF7030_1_TRIGPIN0 =   0x00,          
    
    /*! The trigger intput pin 1. */
    ADI_ADF7030_1_TRIGPIN1 =   0x01            

} ADI_ADF7030_1_TRIGPIN;


/*! Enumeration of events generated by ADF7030_1.
 *
 *
 * */
typedef enum
{
    /*! Interrupt is generated via interrupt pin0. */
    ADI_ADF7030_1_EVENT_INT0,

    /*! Interrupt is generated via interrupt pin1. */
    ADI_ADF7030_1_EVENT_INT1,
      
    /*! Host Driver Error */
    ADI_ADF7030_1_HOST_ERROR,
    
    /*! PHY operation Error */
    ADI_ADF7030_1_PHY_ERROR

} ADI_ADF7030_1_EVENT;


/*! Enumeration of different radio firmware states
 *
 */
typedef enum {
    /*! Lowest current state */
    PHY_SLEEP                   = 0x00,
    
    /*! Default startup state */
    PHY_OFF                     = 0x01,
    
    /*! Standby state for TRX operations */
    PHY_ON                      = 0x02,
    
    /*! Receive state */
    PHY_RX                      = 0x03,
     
    /*! Transmit state */
    PHY_TX                      = 0x04,
    
    /*! Configuring state */
    CFG_DEV                     = 0x05,
    
    /*! Energy sniffing state (no rx possible)*/
    CCA                         = 0x06,
    
    /*! Calibrating state */
    DO_CAL                      = 0x09,
    
    /*! Monitoring state */
    MON                         = 0x0A,
    
    /*! Low frequency calibration state */
    LFRC_CAL                    = 0x0C,
    
    /*! GPIO Clock output state */
    GPCLK                       = 0x10

} ADI_ADF7030_1_RADIO_STATE;


/*! Enumeration of different phy radio error codes
 *
 */
typedef enum {
    /*! No error */
	SM_NOERROR										= 0x00,

    /*! Requested state does not exist */
	SM_INVALID_STATE								= 0x01,

    /*! State transition has been denied */
	SM_TRANS_BLOCKED								= 0x02,

    /*! State machine timeout */
	SM_TIMEOUT										= 0x03,

    /*! Illegal script command */
	SM_ILLEGAL_SCRIPT_CMD							= 0x04,

    /*! Unknown state machine special command */
	SM_UNKNOWN_SPECIAL_CMD							= 0x05,

    /*! Special command is not configured */
	SM_UNPOPULATED_CUSTOM_CMD						= 0x06,

    /*! Backup up configuration is not valid */
	SM_PROFILE_CRC_INCORRECT						= 0x07,

    /*! Not available  */
	SM_LPM_CRC_INCORRECT							= 0x08,

    /*! Not available */
	SM_INVALID_SCRIPT_DATA							= 0x09,

    /*! Not available */
	SM_ROM_CRC_INCORRECT							= 0x30,

    /*! Startup CRC calculation running (not really an error code) */
	SM_PROFILE_CRC_CALCULATING						= 0x31,

	/*! PLL course calibration failure */
	HW_MAIN_PLL_VCO_CAL_FAILURE_COARSE_CAL_FAILED                           = 0x11,

	/*! PLL amplitude calibration failure */
	HW_MAIN_PLL_VCO_CAL_FAILURE_AMP_CAL_FAILED                              = 0x12,

	/*! PLL fine calibration failure */
	HW_MAIN_PLL_VCO_CAL_FAILURE_FINE_CAL_FAILED                             = 0x13,

	/*! Auxiliary PLL lock failure */
	HW_ANC_PLL_LOCK_FAILED							= 0x14,

	/*! PLL calibration mode invalid */
	HW_MAIN_PLL_VCO_CAL_MODE_INVALID                                        = 0x15,

	/*! Temperature Compensated Crystal Oscillator failure */
	HW_TCXO_NOT_READY                                                       = 0x20,

	/*! Crystal Oscillator failure */
	HW_XTAL_NOT_READY                                                       = 0x21,

	/*! Offset calibration failure */
	HW_OCL_CAL_FAILED                                                       = 0x23,

	/*! Hardware divide by zero */
	HW_DIV_BY_ZERO                                                          = 0x24,

	/*! VCO KV calibration step failure */
	HW_VCO_KV_CAL_SINGLE_SHOT_FAILED                                        = 0x25,

	/*! VCO KV fine calibration failure */
	HW_VCO_KV_CAL_FINE_CAL_FAILED                                           = 0x26,

	/*! VCO KV amplitude calibration failure */
	HW_VCO_KV_CAL_AMP_CAL_FAILED                                            = 0x27,

	/*! VCO KV clock source invalid */
	HW_VCO_KV_HFXTAL_INVALID						= 0x28,

	/*! VCO KV coarse calibration failure */
	HW_VCO_KV_CAL_COARSE_CAL_FAILED                                         = 0x29,

	/*! Temperature sensor failure */
	HW_TEMP_SENSE_FAILED							= 0x32,

	/*! Low frequency oscillator calibration failure */
	HW_LFRC_CAL_FAILED                                                      = 0x33,

	/*! Firmware hard fault */
	HW_HARD_FAULT_FAIL                                                      = 0x3F

} ADI_ADF7030_1_RADIO_ERROR;


/*! Enumeration of different firmware transition states
 *
 */
typedef enum {
    
    /*! PHY is transitionning between two state */
    TRANSITION  = 0,
    
    /*! PHY is within one of the "ADI_ADF7030_1_RADIO_STATE" state */
    STATE       = 1,
    
    /*! PHY is waiting in OFF or ON state for a command */
    IDLE        = 2,
    
    /*! PHY is currently executing a calibration routine */
    CALIBRATION = 3

} ADI_ADF7030_1_RADIO_TRANSITION;


/*! Enumeration of different radio firmware extended command
 *
 */
typedef enum {
    
    /*! Disable both PHY Radio ADI_ADF7030_1_TRIGPIN0 and ADI_ADF7030_1_TRIGPIN1 */
    TRIGPIN_NONE        = 0x0C,
    
    /*! Enable PHY Radio ADI_ADF7030_1_TRIGPIN0 and disable ADI_ADF7030_1_TRIGPIN1 */
    TRIGPIN0_ONLY       = 0x0D,
    
    /*! Disable PHY ADI_ADF7030_1_TRIGPIN0 and enable ADI_ADF7030_1_TRIGPIN1 */
    TRIGPIN1_ONLY       = 0x0E,
    
    /*! Enable both PHY Radio ADI_ADF7030_1_TRIGPIN0 and ADI_ADF7030_1_TRIGPIN1
     *  -- Does not work -- currently triggers pins are mutually exclusive
     *  only one trigger pin can be enabled at a time */
    TRIGPIN_ALL         = 0x0F

} ADI_ADF7030_1_RADIO_EXTENDED;


/*! Structure to hold the current trigger pin status.
 */
typedef enum adi_adf7030_1_GPIO_Trig_status
{
    /*! Host GPIO trigger is not configured*/
    NONE        = 0,
    
    /*! Host GPIO trigger has been configured */ 
    HOST_READY  = 1,
    
    /*! PHY GPIO trigger has been setup */ 
    PHY_READY   = 2,
            
    /*! Both Host and Phy are configured for GPIO trigger operation,
        but Radio PHY has not enabled the trigger */
    ENABLED     = 3
    
} ADI_ADF7030_1_GPIO_TRIG_STATUS;


/*! Enumeration of different PHY Radio gpio configuration
 *
 */
typedef enum {
    
    /*! General purpose PHY Radio GPIO pin 0 */
    ADF7030_1_GPIO0        = 0,

    /*! General purpose PHY Radio GPIO pin 1 */
    ADF7030_1_GPIO1        = 1,
      
    /*! General purpose PHY Radio GPIO pin 2 */
    ADF7030_1_GPIO2        = 2,
      
    /*! General purpose PHY Radio GPIO pin 3 */
    ADF7030_1_GPIO3        = 3,
      
    /*! General purpose PHY Radio GPIO pin 4 */
    ADF7030_1_GPIO4        = 4,
      
    /*! General purpose PHY Radio GPIO pin 5 */
    ADF7030_1_GPIO5        = 5,
      
    /*! General purpose PHY Radio GPIO pin 6 */
    ADF7030_1_GPIO6        = 6,
      
    /*! General purpose PHY Radio GPIO pin 7 */
    ADF7030_1_GPIO7        = 7,
      
} ADI_ADF7030_1_GPIO_PIN;


/*! Enumeration of different radio calibrations
 *
 */
typedef enum {
    /*! Analog filter RC calibration */
    ANAFILT_RC_CAL      = 0x00000001,
    
    /*! ADC Notch calibration */
    ADC_NOTCH_CAL       = 0x00000010,

    /*! Analog QEC calibration */
    ANA_QEC_CAL         = 0x00000040,

    /*! Digital QEC calibration */
    DIG_QEC_CAL         = 0x00000100,

    /*! Anxillary PLL calibration */
    ANCPLL_CAL          = 0x00000400,

    /*! Low frequency RC calibration */
    LF_RC_CAL           = 0x00001000,

    /*! High frequency RC calibration */
    HF_RC_CAL           = 0x00004000,

    /*! VCO calibration */
    VCO_CAL             = 0x00010000,

    /*! VCO KV calibration */
    VCO_KV_CAL          = 0x00040000,
    
    /*! Xtal temperature compensation calibration */
    TEMP_XTAL_CAL       = 0x00100000,

    /*! Analog filter RC calibration */
    HF_XTAL             = 0x00400000,

    /*! Analog filter RC calibration */
    INLINE_OCL_CAL      = 0x40000000,

    //DEFAULT_CALS =
    //OFFLINE_CALS = 
} ADI_ADF7030_1_RADIO_CALIBRATION;


typedef void *  ADI_ADF7030_1_HANDLE;
typedef uint32_t ADI_ADF7030_1_SPI_PNTR[ADF7030_1_SPI_PNTR_NUM];

//Forward Typedef declarartion
typedef struct DATA_BLOCK_DESCRIPTOR_Type       DATA_BLOCK_DESCRIPTOR_Type;
typedef struct MEM_DESCRIPTOR_Type              MEM_DESCRIPTOR_Type;
typedef struct MEM_CFG_DESCRIPTOR_Type          MEM_CFG_DESCRIPTOR_Type;
typedef struct PATCH_DESCRIPTOR_Type            PATCH_DESCRIPTOR_Type;
typedef struct adi_adf7030_1_info               ADI_ADF7030_1_INFO;

/**
 * @brief Device Drivers User define function definition
 */
typedef ADI_ADF7030_1_RESULT (* ADI_ADF7030_1__USER_FUNC) (     /*!< User defined RF driver function pointer */
    ADI_ADF7030_1_INFO *        pDevInfo);                       /*!< Pointer to Instance Info params */


/*! Structure to hold the information regarding the SPI PHY Status
 */    
typedef struct adi_adf7030_1_SPI_Status
{
  union {
    uint8_t  VALUE;                          /*!< SPI Slave Status                                                      */
  
    struct {
      uint8_t  MEM_ACC_STATUS:  1;            /*!< *Instantaneous* value of an AHB read data underflow condition
                                                    for memory reads. The same bit doubles up as an AHB bus error
                                                     indicator for memory writes.                                         */
      uint8_t  FW_STATUS  :  4;               /*!< *Instantaneous* value of the firmware status of the radio controller
                                                    state machine.                                                        */
      uint8_t  CMD_READY  :  1;               /*!< *Instantaneous* value of the indicator to the host that no posted
                                                    (to the SPI Slave) radio controller command is currently awaiting
                                                     servicing by the firmware radio state machine running on the
                                                     Cortex-M0.                                                           */
      uint8_t  EXT_IRQ_STATUS:  1;            /*!< *Instantaneous* value of the commoned-up (bitwise OR of the)
                                                    external interrupt outputs from the ADF7030, made available
                                                     in this format to the host for use in pin-limited systems.           */
      uint8_t  SPIS_READY :  1;               /*!< *Instantaneous* value of the confirmation to the external host
                                                    that the HCLK clock domain in the ADF7030 has power, is out
                                                     of reset and has a running clock.                                    */
    };                                     /*!< BitSize                                                               */ 
  };
} ADI_ADF7030_1_SPI_STATUS;

/*! Structure to hold the information regarding the SPI device configuration
 * 
 */
typedef struct adi_adf7030_1_SPI_Info
{
    /*! SPI Device number to be used for communicating with ADF7030_1 */
    uint32_t                	nDeviceNum;
    
    /*! Pointer to a 32 bit aligned buffer */
    void *                  	pDevMemory;
    
    /*! Size of buffer pointer by pDevMemory */
    uint32_t                	nMemorySize;

    /*! Chip select number to address ADF7030_1 */
    ADI_SPI_CHIP_SELECT     	eChipSelect;

    /*! SPI clock frequency for default communication */
    uint32_t                	nClkFreq;

#if (ADF7030_1_SPI_FAST_SHORTXFER_ENABLE == 1)    
    /*! SPI clock frequency for fast SPI transfer */
    uint32_t                	nClkFreq_Fast;
    
    /*! Currently used SPI clock frequency */
    uint32_t                	nClkFreq_Current;
#endif  
    /*! SPI PHY pointer */
    ADI_ADF7030_1_SPI_PNTR  	PHY_PNTR;
    
    /*! SPI device handle */
    ADI_SPI_HANDLE          	hSPIDevice;

    /*! SPI TX transaction buffer */
    uint8_t *               	pSPI_TX_BUFF;
    
    /*! SPI RX transaction buffer */
    uint8_t *               	pSPI_RX_BUFF;
    
    /*! Pointer to the parent device instance information */
    void *                  	hDevInfo; //ADI_ADF7030_1_INFO
    
    /*! SPI Driver communication result */
    ADI_ADF7030_1_RESULT    	eXferResult;
      
    /*! SPI status from last transaction */
    ADI_ADF7030_1_SPI_STATUS 	nStatus;
    
    /*! PHY firmware state from last transaction */
    ADI_ADF7030_1_RADIO_STATE 	nPhyState;

#if (ADF7030_1_PHY_ERROR_REPORT_ENABLE == 1)
    /*! Dynamic enable/disable of ePhyError checking */
    bool						bPhyErrorCheck;

    /*! PHY radio error code */
    ADI_ADF7030_1_RADIO_ERROR	ePhyError;
#endif

}ADI_ADF7030_1_SPI_INFO;


/*! Structure to hold mapping between ADF7030_1 interrupt pin and host processor
 * GPIO pin.
 */
typedef struct adi_adf7030_1_GPIO_Int_info
{
    /*! Interrupt IRQ */
    IRQn_Type               eIRQType;
    
    /*! GPIO port to which the interrupt pin is connected */
    ADI_GPIO_PORT           ePort;

    /*! GPIO pin within the GPIO port */
    ADI_GPIO_DATA           nPin;
    
    /*! PHY Radio GPIO pin */
    ADI_ADF7030_1_GPIO_PIN  ePhyPin;
    
    /*! Callback function pointer */
    ADI_CALLBACK            pfCallback;
    
    /*! Callback function parameters */
    //void *                  pCBParam;
    
    /*! Radio PHY interrupt mask configuration */
    uint32_t                nIntMap;
      
    /*! Last IRQ status */
    uint32_t                nIntStatus;

}ADI_ADF7030_1_GPIO_INT_INFO;


/*! Structure to hold mapping between ADF7030_1 trigger pin and host processor
 * GPIO pin.
 */
typedef struct adi_adf7030_1_GPIO_Trig_info
{
    /*! Host GPIO port to which the interrupt pin is connected */
    ADI_GPIO_PORT                       ePort;

    /*! Host GPIO pin within the GPIO port */
    ADI_GPIO_DATA                       nPin;

    /*! PHY Radio GPIO pin */
    ADI_ADF7030_1_GPIO_PIN              ePhyPin;
    
    /*! PHY Radio Command to execute on trigger */
    uint32_t                            nTrigCmd;
    
    /*! Current trigger status */
    ADI_ADF7030_1_GPIO_TRIG_STATUS      eTrigStatus;
    
}ADI_ADF7030_1_GPIO_TRIG_INFO;


/*! Structure to hold the mapping of the ADF7030_1 Hard Reset on the host
 *  processor GPIO pin.
 */
typedef struct adi_adf7030_1_GPIO_Reset_info
{
    /*! Host GPIO port to which the interrupt pin is connected */
    ADI_GPIO_PORT                       ePort;

    /*! Host GPIO pin within the GPIO port */
    ADI_GPIO_DATA                       nPin;

}ADI_ADF7030_1_GPIO_RESET_INFO;


/*! Structure to hold the ADF7030_1 device related instance data. 
 *  This structure
 *  is defined using the memory passed by the application.
 */
struct adi_adf7030_1_info
{
    /*! Callback function pointer */
    ADI_CALLBACK                        pfCallback;

    /*! Callback parameter */
    void *                              pCBParam;
    
    /*! SPI information for underlying driver */
    ADI_ADF7030_1_SPI_INFO              SPIInfo;

    /*! GPIO pin connection information for Interrupt pin 1 and 2 */
    ADI_ADF7030_1_GPIO_INT_INFO         IntGPIOInfo[ADF7030_1_NUM_INT_PIN];
    
    /*! GPIO pin connection information for Trigger pin 1 and 2 */
    ADI_ADF7030_1_GPIO_TRIG_INFO        TrigGPIOInfo[ADF7030_1_NUM_TRIG_PIN];
    
    /*! GPIO pin connection information for the Hard Reset pin */
    ADI_ADF7030_1_GPIO_RESET_INFO       ResetGPIOInfo;

    /*! Default Startup Configuration */
    uint8_t const *                     pStartupCfg;
    
    /*! Default Startup Configuration */
    uint32_t                            StartupCfgSize;
    
    /*! User defined Enable function pointer */
    ADI_ADF7030_1__USER_FUNC            pfEnableFunc;
    
    /*! User defined Disable function pointer */
    ADI_ADF7030_1__USER_FUNC            pfDisableFunc;
    
    /*! Default Startup Patch */
    PATCH_DESCRIPTOR_Type *             pPatch;

    /*! Main Configuration Descriptor */
    MEM_CFG_DESCRIPTOR_Type *           pCfg;
    
    /*! Pointer to Host Shadow "profile" structure */
    DATA_BLOCK_DESCRIPTOR_Type *        pPROFILE__BLOCK;
    
    /*! Pointer to Host Shadow "generic_pck" structure */
    DATA_BLOCK_DESCRIPTOR_Type *        pPACKET_CFG__BLOCK;    

    /*! Host TX Packet Buffer */
    DATA_BLOCK_DESCRIPTOR_Type *        pTX_PCKBUFF__BLOCK;
    
    /*! Host RX Packet Buffer */
    DATA_BLOCK_DESCRIPTOR_Type *        pRX_PCKBUFF__BLOCK;
    
    /*! Placeholder for accessing the "Start" location of PHY Payload Buffer */
    uint32_t                            PHY_TRX_BUFFER_START;

    /*! Placeholder for accessing the "Mid" location of PHY Payload Buffer */
    uint32_t                            PHY_TRX_BUFFER_MID;

    /*! Placeholder for accessing the "End" location of PHY Payload Buffer */
    uint32_t                            PHY_TRX_BUFFER_END;

    /*! Placeholder for accessing the "Current" location of PHY Payload Buffer */
    uint32_t                            PHY_TRX_BUFFER_CURRENT;
    
    /*! Pointer to the parent device instance data */
    void *                              hDevice; //ADI_ADF7030_1_DEVICE
    
};// ADI_ADF7030_1_INFO;


/*! ADF7030_1 device instance data
 *
 */
typedef struct adi_adf7030_1_device
{
    /*! State of the driver */
    ADI_ADF7030_1_STATE   eState;

    /*! Pointer to the device instance information */
    ADI_ADF7030_1_INFO    *pDevInfo;

} ADI_ADF7030_1_DEVICE;

/** @} */ /* End of group adf7030-1_common Global */

/** @} */ /* End of group adf7030-1_headers Headers */

/** @} */ /* End of group adf7030-1 adf7030-1 Driver */

#endif /* _ADF7030_1__COMMON_H_ */
