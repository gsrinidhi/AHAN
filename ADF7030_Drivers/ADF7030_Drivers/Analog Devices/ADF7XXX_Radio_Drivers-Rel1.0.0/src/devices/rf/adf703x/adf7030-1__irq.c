/*!
 *****************************************************************************
  @file:	adf7030-1__irq.c
  
  @brief:	GPIO Interrupt Interface layer between PHY Radio and the Host.
  
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

/** \addtogroup adf7030-1__irq Interrupt Interface
 *  @{
 */

#ifndef _ADF7030_1__IRQ_C_

/*! \cond PRIVATE */
#define _ADF7030_1__IRQ_C_

#include <sys/platform.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#include <drivers/spi/adi_spi.h>

#if defined(__ADUCM3027__) || defined(__ADUCM3029__)
#include <services/gpio/adi_gpio.h>
#elif defined (__ADUCM4050__)
#include <drivers/gpio/adi_gpio.h>
#endif

#include <adi_adf7030-1_reg.h>
#include <adf7030-1__common.h>
#include <adf7030-1__core.h>
#include <adf7030-1__mem.h>
#include <adf7030-1__spi.h>
#include <adf7030-1__gpio.h>

#include "adf7030-1__irq.h"

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
 * @brief       Default gpio driver Callback function for interrupt pin 0
 *
 * @note        This function is getting call on detecting a rising edge of the
 *              ADI_ADF7030_1_INTPIN0 or ADI_ADF7030_1_INTPIN1 interrupt pin
 *              of the PHY Radio.
 *
 * @param [in]  pCBParam         Callback parameter passed to the radio driver application
 *                               (Pointer to Device Instance data structure information)
 *
 * @param [in]  Port	         Host GPIO service port source (ADI_GPIO_EVENT) calling the callback
 *
 * @param [in]  PinIntData       Pointer to the Host GPIO pin interrupt data source (ADI_GPIO_DATA)
 *
 * @return      None
 */

void adf7030_1__IRQ_Callback (
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
	}
	else if((Port == pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN1].ePort) &&
			(*(uint32_t *)PinIntData & pDevInfo->IntGPIOInfo[ADI_ADF7030_1_INTPIN1].nPin))
	{
		/* If callback Port and pin matches the Instance Irq1 port & pin definition */
		irq_event = ADI_ADF7030_1_EVENT_INT1;
	}
	else
	{
		/* Should not end up here */
	}

	/* If global Radio driver application callback is defined */
	if(pDevInfo->pfCallback != NULL)
	{
		/* Execute the global radio driver event callback */
		pDevInfo->pfCallback(pDevInfo, irq_event, NULL);
	}

    return;
}


/**
 * @brief       Initializes Irq handling for the adf7030-1 PHY
 *
 * @note        This function open the GPIO service, configure the Host GPIO
 *              according to the pDevInfo->IntGPIOInfo and setup callbacks events.
 *              If the GPIO interface is changing at runtime, user can call the 
 *              adf7030_1__IRQ_ConfigIntPin() followed by adf7030_1__IRQ_Init().
 *              
 * @param [in]  hDevice         Handle to ADF3030-1 device instance.
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance was succesfully initialized
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED    [D] PHY initialisation failed
 *
 * @sa  adf7030_1__SPI_UnInit().
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_Init(
    ADI_ADF7030_1_HANDLE  const         hDevice
)
{    
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;
    
#ifdef ADI_DEBUG
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;

    /* Validate the given device handle */
    if((eResult = ValidateHandle(pDevice)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
#endif /* ADI_DEBUG */   

    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO *pDevInfo = pDevice->pDevInfo;
  
    /* Initialize GPIO service with number of callbacks required */
    if(adi_gpio_Init(
            GPIOCallbackMem,
            ADI_GPIO_MEMORY_SIZE) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
      
    /* Host GPIO sensing polarity */
    ADI_GPIO_IRQ_TRIGGER_CONDITION eSense = ADI_GPIO_IRQ_RISING_EDGE;
    
    /* Check adf7030-1 pin mux setting ?! */
    //adf7030_1__IRQ_SetupPhyGPIO()
    
    /* Setup Glue Host GPIO for Irq0 */
    if(adf7030_1__IRQ_SetupHostGPIO(
            pDevInfo,
            ADI_ADF7030_1_INTPIN0,
            adf7030_1__IRQ_Callback,
            eSense) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    /* Setup Glue Host GPIO for Irq1 */
    if(adf7030_1__IRQ_SetupHostGPIO(
            pDevInfo,
            ADI_ADF7030_1_INTPIN1,
            adf7030_1__IRQ_Callback,
            eSense) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Unitializes Irq handling for the adf7030-1 PHY
 *
 * @note        This function remove IRQ callbacks and disable IRQs on the PHY
 *              
 * @param [in]  hDevice         Handle to ADF3030-1 device instance.
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance was succesfully uninitialized
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED    [D] PHY uninitialisation failed
 *
 * @sa  adf7030_1__SPI_Init().
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_UnInit(
    ADI_ADF7030_1_HANDLE  const         hDevice
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;
    
#ifdef ADI_DEBUG
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT      eResult;

    /* Validate the given device handle */
    if((eResult = ValidateHandle(pDevice)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }
#endif /* ADI_DEBUG */   

    /* Pointer to the ADF7030-1 instance information */
    ADI_ADF7030_1_INFO *pDevInfo = pDevice->pDevInfo;

    /* Unregister the interrupt handler for IRQ0 pins */
    if(adf7030_1__IRQ_SetCallback( pDevInfo,
                                   ADI_ADF7030_1_INTPIN0,
                                   NULL ) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    /* Unregister the interrupt handler for IRQ1 pins */
    if(adf7030_1__IRQ_SetCallback( pDevInfo,
                                   ADI_ADF7030_1_INTPIN1,
                                   NULL ) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    return ADI_ADF7030_1_SUCCESS;
}  
  
/**
 * @brief       Sets the processor GPIO pin to which the ADF7030-1 interrupt pin is connected.
 *
 * @details     ADF7030-1 has two interrupt pins (INT0 and INT1) which can be
 *              connected to the host processor for notifying the events.
 *              Depending upon the board design, the ADF7030-1 interrupt pins can
 *              be connected to different GPIO pins on the host processor. This
 *              API can be used to specify the GPIO pin for which each of the
 *              interrupt is connected. This API should be called separately
 *              (can be twice) to specify the GPIO pin for each of the ADF7030-1
 *              interrupt pins.
 *
 * @note        This function should not be called after data flow is enabled.
 *
 * @param [in]  hDevice         Handle to ADF7030-1 device whose interrupt pin to be
 *                              configured.
 * @param [in]  eIrq            Indicate whether the interrupt need to be mapped to GROUP-A
                                OR to GROUP-B.
 * @param [in]  eIntPin         ADF7030-1 interrupt pin to be configured.
 *
 * @param [in]  eGPIOPort       GPIO port of the host processor to which the
 *                              given ADF7030-1 interrupt pin is connected.
 *
 * @param [in]  nGPIOPin        GPIO pin within the given port to which the
 *                              given ADF7030-1 interrupt pin is connected.
 *
 * @return      Status
 *
 *  - #ADI_ADF7030_1_SUCCESS                  If successfully mapped GPIO pin
 *                                            for interrupt pin.
 *
 *  - #ADI_ADF7030_1_INVALID_HANDLE       [D] If the given ADF7030-1 device handle
 *                                            is invalid.
 *
 *  - #ADI_ADF7030_1_DEVICE_NOT_OPENED    [D] If the given device is not yet opened.
 *
 *  - #ADI_ADF7030_1_INVALID_OPERATION    [D] If trying to configure GPIO pin for
 *                                            an interrupt after measurement is enabled.
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_ConfigHostIntPin(
    ADI_ADF7030_1_HANDLE  const         hDevice,
    IRQn_Type                           eIrq,
    ADI_ADF7030_1_INTPIN                eIntPin,
    ADI_GPIO_PORT                       eGPIOPort,
    ADI_GPIO_DATA                       nGPIOPin
)
{
    /* Pointer to the ADF7030-1 device */
    ADI_ADF7030_1_DEVICE *pDevice = (ADI_ADF7030_1_DEVICE *)hDevice;
    
#ifdef ADI_DEBUG
    /* ADXL363 result code */
    ADI_ADF7030_1_RESULT      eResult;

    /* Validate the given device handle */
    if((eResult = ValidateHandle(pDevice)) != ADI_ADF7030_1_SUCCESS)
    {
        return eResult;
    }

    /* Don't allow to configure GPIO when measurement is enabled. */
    if(pDevice->eState  >=  ADI_ADF7030_1_STATE_CONFIGURED)
    {
        return ADI_ADF7030_1_INVALID_OPERATION;
    }

#endif /* ADI_DEBUG */

    /* Get the pointer to GPIO pin information structure for the given interrupt pin */
    ADI_ADF7030_1_GPIO_INT_INFO   *pIntGPIOInfo = &pDevice->pDevInfo->IntGPIOInfo[(uint8_t)eIntPin];

    /* Save the given GPIO mapping */
    pIntGPIOInfo->ePort =   eGPIOPort;

    /* GPIO service expects the pins as mask. So store the mask which will used later for all the
     * GPIO API calls
     */
    pIntGPIOInfo->nPin  =  nGPIOPin;
    /* Select the IRQ type */
    pIntGPIOInfo->eIRQType = eIrq;
 
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Setup the Host GPIO driver to trigger Callback on PHY interrupt
 *
 * @note        This function remove IRQ callbacks and disable IRQs on the PHY
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.         
 *             
 * @param [in]  pfCallback      Callback function which will be called upon interrupt detection.         
 *             
 * @param [in]  eSense          Trigger condition for GPIO interrupt.         
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance was succesfully configured
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED    [D] PHY configuration failed
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_SetupHostGPIO(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin,
    ADI_CALLBACK                        pfCallback,
    ADI_GPIO_IRQ_TRIGGER_CONDITION      eSense
)
{
    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
      
    /* Setup the GPIO pin direction to input */
    if(adi_gpio_InputEnable(
            pIntGPIOInfo->ePort,
            pIntGPIOInfo->nPin,
            true) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

     /* Setup the GPIO pin direction to input */
    if(adi_gpio_SetGroupInterruptPins(
            pIntGPIOInfo->ePort,
            pIntGPIOInfo->eIRQType,
            pIntGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    if(adi_gpio_SetGroupInterruptPolarity(
            pIntGPIOInfo->ePort,
            pIntGPIOInfo->nPin) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    /* Setup the GPIO pin direction to input */
    if(adf7030_1__IRQ_SetCallback( pDevInfo,
                                   eIntPin,
                                   pfCallback ) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }

    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Configures the PHY interrupt source mask register
 *
 * @note        This function transfers bit [31:8] of nIntMap into the Radio PHY
 *              interrupt mask resgister. Lower bits [7:0] are written into the PHY
 *              profile packet interrupt location. See all available interrupt 
 *              sources ADI_AF7030_1_EXT_IRQn_Type.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.         
 *             
 * @param [in]  nIntMap         Interrupt events which will be triggering PHY irq line.         
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance irq mask was succesfully configured
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] PHY irq mask configuration failed
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_SetMap(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin,
    uint32_t                            nIntMap
)
{
    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
              
    uint32_t non_frame_irq = nIntMap & 0xFFFFFF00;
      
    /* Setup bit [31:8] of nIntMap into IRQ_CTRL_MASK0_Addr or IRQ_CTRL_MASK1_Addr */  
    if(adf7030_1__SPI_wr_word_b_a( &pDevInfo->SPIInfo,
                                   IRQ_CTRL_MASK0_Addr + (eIntPin << 2),
                                   1,
                                   &non_frame_irq) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_COMM_FAILED;
    }
    
    /* Setup bit [7:0] of nIntMap into GENERIC_PKT_FRAME_CFG1_TRX_IRQ0_TYPE or GENERIC_PKT_FRAME_CFG1_TRX_IRQ1_TYPE */
    adf7030_1__SPI_SetBytes( &pDevInfo->SPIInfo,
                            GENERIC_PKT_FRAME_CFG1_Addr + 2 + (uint32_t)eIntPin,
                            nIntMap,
                            1,
                            NULL);
    
    /* Save the current Radio PHY interrupt mask into the current instance GPIO Pin info structure */
    pIntGPIOInfo->nIntMap = nIntMap;
    
    return pDevInfo->SPIInfo.eXferResult;
}


/**
 * @brief       Readbacks the PHY interrupt source mask register
 *
 * @note        This function transfers the Radio PHY interrupt mask resgister
 *              back into the Host interrupt pin configuration structure.
 *              See all available interrupt sources ADI_AF7030_1_EXT_IRQn_Type.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.         
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance irq mask was succesfully readback
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] PHY irq mask readback failed
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_GetMap(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin
)
{
    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
        
    /* Readback Radio PHY interrupt mask bits [31:8] into the current instance GPIO Pin info structure */
    if(adf7030_1__SPI_rd_word_b_a( &pDevInfo->SPIInfo,
                                   IRQ_CTRL_MASK0_Addr + (eIntPin << 2),
                                   1,
                                   &pIntGPIOInfo->nIntMap ) != ADI_ADF7030_1_SUCCESS)
    {
        return ADI_ADF7030_1_SPI_COMM_FAILED;
    }
    
    /* Readback Radio PHY interrupt mask bits [7:0] into the current instance GPIO Pin info structure */
    *((uint8_t *)&pIntGPIOInfo->nIntMap) = (uint8_t) adf7030_1__SPI_GetBytes( &pDevInfo->SPIInfo,
                                                                GENERIC_PKT_FRAME_CFG1_Addr + 2 + (uint32_t)eIntPin,
                                                                1,
                                                                NULL);
    return pDevInfo->SPIInfo.eXferResult;
}


/**
 * @brief       Readbacks the PHY interrupt status register
 *
 * @note        This function readback the Radio PHY interrupt status register
 *              back into the Host interrupt pin configuration structure.
 *              See all available interrupt sources ADI_AF7030_1_EXT_IRQn_Type.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.         
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance irq status was succesfully readback
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] PHY irq status readback failed
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_GetStatus(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin
)
{
    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
        
    /* Readback Radio PHY interrupt status into the current instance GPIO Pin info structure */
    return(adf7030_1__SPI_rd_word_b_a( &pDevInfo->SPIInfo,
                                       IRQ_CTRL_STATUS0_Addr + (eIntPin << 2),
                                       1,
                                       &pIntGPIOInfo->nIntStatus));
}


/**
 * @brief       Clear the PHY eIntPin interrupt pin
 *
 * @note        This function clears the Radio PHY interrupt status register
 *              by "write one to clear" operation. If PHY status register is 0,
 *              external GPIO line will be deasserted.
 *              See all available interrupt sources ADI_AF7030_1_EXT_IRQn_Type.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.         
 *             
 * @param [in]  nIntClear       Interrupt events to clear.         
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance irq status was succesfully cleared
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] PHY irq status clearing failed
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_ClrStatus(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin,
    uint32_t                            nIntClear
)
{
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT eResult;
    
    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
        
    /* Clear Radio PHY interrupt status */
    if((eResult = adf7030_1__SPI_wr_word_b_a( &pDevInfo->SPIInfo,
                                   IRQ_CTRL_STATUS0_Addr + (eIntPin << 2),
                                   1,
                                   &nIntClear)) == ADI_ADF7030_1_SUCCESS)
    {
        pIntGPIOInfo->nIntStatus &= ~nIntClear;
    }

    return eResult;
}



/**
 * @brief       Readback and Clear the PHY eIntPin interrupt pin
 *
 * @note        This function readback the Radio PHY interrupt status register
 *              and use the value immadiately clear by "write one to clear" operation.
 *              If PHY status register is 0, external GPIO line will be deasserted.
 *              In this case, pGPIOInfo->nIntStatus holds the state of the PHY
 *              status register pior to clearing it.
 *              See all available interrupt sources ADI_AF7030_1_EXT_IRQn_Type.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.         
 *             
 * @param [in]  nIntClear       Interrupt events to clear.         
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance irq status was succesfully readback and cleared
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] PHY irq status reading or clearing failed
 */

ADI_ADF7030_1_RESULT adf7030_1__IRQ_GetClrStatus(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin
)
{
    /* ADF7030-1 result code */
    ADI_ADF7030_1_RESULT eResult;

    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
        
    /* Readback Radio PHY interrupt status into the current instance GPIO Pin info structure */
    if((eResult = adf7030_1__SPI_rd_word_b_a( &pDevInfo->SPIInfo,
                                              IRQ_CTRL_STATUS0_Addr + (eIntPin << 2),
                                              1,
                                              &pIntGPIOInfo->nIntStatus)) == ADI_ADF7030_1_SUCCESS)
    {
        /* Clear Radio PHY interrupt status */
        eResult = adf7030_1__SPI_wr_word_b_a( &pDevInfo->SPIInfo,
                                              IRQ_CTRL_STATUS0_Addr + (eIntPin << 2),
                                              1,
                                              &pIntGPIOInfo->nIntStatus);
    }

    return eResult;
}


/**
 * @brief       Changes GPIO Callback on PHY interrupt
 *
 * @note        User can use this function in case default Irq callback
 *              does not suite the application needs.
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.         
 *             
 * @param [in]  pfCallback      Callback function which will be called upon interrupt detection.                
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY intance irq callback was succesfully configured
 *  - #ADI_ADF7030_1_GPIO_DEV_FAILED    [D] PHY callback configuration failed
 */
ADI_ADF7030_1_RESULT adf7030_1__IRQ_SetCallback(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin,
    ADI_CALLBACK                        pfCallback
)
{
    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
      
    /* Setup the GPIO pin direction to input */
    if(adi_gpio_RegisterCallback(
            pIntGPIOInfo->eIRQType,
            pfCallback,
            pDevInfo) != ADI_GPIO_SUCCESS)
    {
        return ADI_ADF7030_1_GPIO_DEV_FAILED;
    }
    
    pIntGPIOInfo->pfCallback = pfCallback;
    
    return ADI_ADF7030_1_SUCCESS;
}


/**
 * @brief       Setup PHY Radio GPIO irq
 *
 * @note        User can use this function prior to issuing CFG_DEV command to 
 *              configure the PHY Radio pinmux setting for each individual IRQ pin.            
 *              
 * @param [in]  pDevInfo        Pointer to the ADF7030-1 instance information structure.
 *   
 * @param [in]  eIntPin         Interrupt id (ADI_ADF7030_1_INTPIN) to configure.                       
 *             
 * @return      Status
 *  - #ADI_ADF7030_1_SUCCESS            If PHY irq GPIO was configured
 *  - #ADI_ADF7030_1_SPI_COMM_FAILED    [D] PHY irq GPIO configuration failed
 */
ADI_ADF7030_1_RESULT adf7030_1__IRQ_SetGPIOPin(
    ADI_ADF7030_1_INFO *                pDevInfo,
    ADI_ADF7030_1_INTPIN                eIntPin
)
{
    /* Pointer to IRQ GPIO Pin info */
    ADI_ADF7030_1_GPIO_INT_INFO * pIntGPIOInfo = &pDevInfo->IntGPIOInfo[eIntPin];
    
    adf7030_1__GPIO_SetCfg( &pDevInfo->SPIInfo,
                            pIntGPIOInfo->ePhyPin,
                            (eIntPin == ADI_ADF7030_1_INTPIN1) ? GPIO_IRQ_OUT1 : GPIO_IRQ_OUT0,
                            false );
    
    /* Return SPI transfer status */
    return(pDevInfo->SPIInfo.eXferResult);
}



#endif /* _ADF7030_1__IRQ_C_ */

/** @} */ /* End of group adf7030-1__irq Interrupt Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */
