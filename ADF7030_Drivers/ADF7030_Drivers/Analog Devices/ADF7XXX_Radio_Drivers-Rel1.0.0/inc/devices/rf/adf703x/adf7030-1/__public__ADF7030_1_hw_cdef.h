/*!
 *****************************************************************************
  @file:	__public__ADF7030_1_hw_cdef.h 
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


#ifndef ADF7030_HW_CDEF_H
#define ADF7030_HW_CDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup adf7030-1 ADF7030-1 Driver
  * @{
  */

/** @addtogroup adf7030-1_headers Headers
 *  @{
 */
    
/** @addtogroup PHY_headers PHY
  * @{
  */

/** @addtogroup HW_Struct Hardware Structures Definition
  * @{
  */

/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================                 VCO_CAL_RESULTS                ================ */
/* ================================================================================ */


/**
  * @brief VCO Calibration Results (VCO_CAL_RESULTS)
  */

typedef struct {                                    /*!< VCO_CAL_RESULTS Structure                                             */
  __IO uint32_t  DATA0;                             /*!< Not Available                                                         */
  __IO uint32_t  DATA1;                             /*!< Not Available                                                         */
  __IO uint32_t  DATA2;                             /*!< Not Available                                                         */
  __IO uint32_t  DATA3;                             /*!< Not Available                                                         */
  __IO uint32_t  DATA4;                             /*!< Not Available                                                         */
  __IO uint32_t  DATA5;                             /*!< Not Available                                                         */
  __IO uint32_t  DATA6;                             /*!< Not Available                                                         */
  __IO uint32_t  DATA7;                             /*!< Not Available                                                         */
} ADI_VCO_CAL_RESULTS_Type;


/* ================================================================================ */
/* ================                       PMU                      ================ */
/* ================================================================================ */


/**
  * @brief Power Management (PMU)
  */

typedef struct {                                    /*!< PMU Structure                                                         */
  __I  uint32_t  RESERVED0[2];
  
  union {
    __IO uint32_t  KEY;                             /*!< Gateway for Software-Keyed Instructions                               */
    
    struct {
      __IO uint32_t  SW_KEY     :  6;               /*!< Software-keyed instruction to the PMU.                                */
    } KEY_b;                                        /*!< BitSize                                                               */
  };
} ADI_PMU_Type;


/* ================================================================================ */
/* ================                    SPI_HOST                    ================ */
/* ================================================================================ */


/**
  * @brief Some description. (SPI_HOST)
  */

typedef struct {                                    /*!< SPI_HOST Structure                                                    */
  
  union {
    __IO uint32_t  PNTR0;                           /*!< SPI Slave Pointer 0                                                   */
    
    struct {
      __IO uint32_t  SPIS_PNTR0 : 32;               /*!< SPI Pointer 0                                                         */
    } PNTR0_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PNTR1;                           /*!< SPI Slave Pointer 1                                                   */
    
    struct {
      __IO uint32_t  SPIS_PNTR1 : 32;               /*!< SPI Pointer 1                                                         */
    } PNTR1_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PNTR2;                           /*!< SPI Slave Pointer 2                                                   */
    
    struct {
      __IO uint32_t  SPIS_PNTR2 : 32;               /*!< SPI Pointer 2                                                         */
    } PNTR2_b;                                      /*!< BitSize                                                               */
  };
} ADI_SPI_HOST_Type;


/* ================================================================================ */
/* ================                    IRQ_CTRL                    ================ */
/* ================================================================================ */


/**
  * @brief IRQ Control Register (IRQ_CTRL)
  */

typedef struct {                                    /*!< IRQ_CTRL Structure                                                    */
  
  union {
    __IO uint32_t  MASK[2];                         /*!< Masks for IRQ_OUTx                                                    */
    
    struct {
           uint32_t             :  8;
      __IO uint32_t  BUFF_HALF_IRQN:  1;            /*!< Rx: the lower half of Rx rolling buffer is full. Tx: the lower
                                                         half of Tx rolling buffer is empty (write 1 to clear event).          */
      __IO uint32_t  BUFF_FULL_IRQN:  1;            /*!< Rx: the upper half of Rx rolling buffer is full. Tx: the upper
                                                         half of Tx rolling buffer is empty (write 1 to clear event).          */
      __IO uint32_t  SM_READY_IRQN:  1;             /*!< SM_RDY event has occurred. The last state transition command
                                                         has been received and the transition from the origin state to
                                                          destination state is underway. A new command can be issued at
                                                          this point to interrupt the current transition. It indicates
                                                          that CMD_READY is 1 (write 1 to clear).                              */
      __IO uint32_t  SM_IDLE_IRQN:  1;              /*!< SM_IDLE event has occurred. The destination state has been reached
                                                         and all actions associated with the destination state have been
                                                          completed. The complete state transition is complete (write
                                                          1 to clear).                                                         */
    } MASK_b[2];                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  STATUS[2];                       /*!< Status of IRQ_OUTx                                                    */
    
    struct {
      __IO uint32_t  PREAMBLE_IRQN:  1;             /*!< Generic packet and IEEE802.15.4g: preamble has been received
                                                         (Rx); the first preamble bit is about to be transmitted (Tx).         */
      __IO uint32_t  PREAMBLE_GONE_IRQN:  1;        /*!< Generic packet and IEEE802.15.4g: preamble pattern no longer
                                                         being received in received bit stream (Rx); last preamble bit
                                                          transmitted (Tx).                                                    */
      __IO uint32_t  SYNCWORD_IRQN:  1;             /*!< Generic packet: the programmed number of bits of Syncword 0
                                                         have been received and matched (Rx). IEEE802.15.4g: the programmed
                                                          number of Syncword 0 or Syncword 1 (if enabled) bits have been
                                                          received and matched (Rx). Generic packet and IEEE802.15.4g:
                                                          the programmed number of bits of Syncword 0 have been transmitted
                                                          (Tx).                                                                */
      __IO uint32_t  LENGTH_IRQN:  1;               /*!< Generic packet: a length field has been received (Rx); the length
                                                         field has been transmitted (Tx). IEEE802.15.4g: PHR has been
                                                          received (Rx); the PHR has been transmitted (Tx).                    */
      __IO uint32_t  PAYLOAD_IRQN:  1;              /*!< Generic packet: full payload received (Rx)/full payload transmitted
                                                         (Tx). IEEE802.15.4g: full Payload (including FCS) received (Rx);
                                                          full payload (including FCS) transmitted (Tx).                       */
      __IO uint32_t  PAYLOAD_BLOC_IRQN:  1;         /*!< Generic packet only: A multiple of TRX_BLOCK_SIZE payload bytes
                                                         have been received (Rx); a multiple of TRX_BLOCK_SIZE payload
                                                          bytes have been transmitted (Tx).                                    */
      __IO uint32_t  CRC_CHK_IRQN:  1;              /*!< Generic packet and IEEE802.15.4g: The programmed number of CRC/FCS
                                                         bits has been received and are correct (Rx); the programmed
                                                          number of CRC/FCS bits have been transmitted (Tx).                   */
      __IO uint32_t  EOF_IRQN   :  1;               /*!< Generic packet and IEEE802.15.4g: the full packet has been received
                                                         (RX)/The full packet has been transmitted (Tx)                        */
      __IO uint32_t  BUFF_HALF_IRQN:  1;            /*!< RX: the lower half of Rx rolling buffer is full. Tx: the lower
                                                         half of Tx rolling buffer is empty write 1 to clear).                 */
      __IO uint32_t  BUFF_FULL_IRQN:  1;            /*!< RX: the upper half of Rx rolling buffer is full. Tx: the upper
                                                         half of Tx rolling buffer is empty (write 1 to clear event).          */
      __IO uint32_t  SM_READY_IRQN:  1;             /*!< SM_RDY Event has occurred. The last state transition command
                                                         has been received and the transition from the origin state to
                                                          destination state is underway. A new command may be issued at
                                                          this point to interrupt the current transition. It indicates
                                                          that CMD_READY is 1, write 1 to clear.                               */
      __IO uint32_t  SM_IDLE_IRQN:  1;              /*!< SM_IDLE Event has occurred. The destination state has been reached
                                                         and all actions associated with the destination state have been
                                                          completed. The complete state transition is complete, write
                                                          1 to clear.                                                          */
    } STATUS_b[2];                                  /*!< BitSize                                                               */
  };
} ADI_IRQ_CTRL_Type;


/* ================================================================================ */
/* ================                       AFC                      ================ */
/* ================================================================================ */


/**
  * @brief AFC (AFC)
  */

typedef struct {                                    /*!< AFC Structure                                                         */
  
  union {
    __IO uint32_t  CONFIG;                          /*!< Not Available                                                         */
    
    struct {
      __IO uint32_t  MODE       :  3;               /*!< Set AFC Mode                                                          */
    } CONFIG_b;                                     /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0[3];
  
  union {
    __I  uint32_t  FREQUENCY_ERROR;                 /*!< Not Available                                                         */
    
    struct {
      __I  uint32_t  READBACK   : 16;               /*!< Frequency Error correction signed 16bit readback value. Frequency
                                                         Error in Hz = readback *26,000,000 / 2^22                             */
    } FREQUENCY_ERROR_b;                            /*!< BitSize                                                               */
  };
} ADI_AFC_Type;


/* ================================================================================ */
/* ================                      CRMGT                     ================ */
/* ================================================================================ */


/**
  * @brief Clock and Reset Control (CRMGT)
  */

typedef struct {                                    /*!< CRMGT Structure                                                       */
  __I  uint32_t  RESERVED0[2];
  
  union {
    __IO uint32_t  PROC_CLK_EN;                     /*!< Processor Clock Enable                                                */
    
    struct {
      __IO uint32_t  CONFIGURATION: 32;             /*!< Processor clock configuration                                         */
    } PROC_CLK_EN_b;                                /*!< BitSize                                                               */
  };
} ADI_CRMGT_Type;


/* ================================================================================ */
/* ================                      MISC                      ================ */
/* ================================================================================ */


/**
  * @brief MISC Firmware Register (MISC)
  */

typedef struct {                                    /*!< MISC Structure                                                        */
  
  union {
    __IO uint32_t  FW;                              /*!< Firmware status and debug register                                    */
    
    struct {
      __IO uint32_t  STATUS     :  2;               /*!< Firmware status ID                                                    */
           uint32_t             :  6;
      __IO uint32_t  CURR_STATE :  6;               /*!< Current firmware state readback                                       */
    } FW_b;                                         /*!< BitSize                                                               */
  };
} ADI_MISC_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define ADI_VCO_CAL_RESULTS_BASE        0x20000844UL
#define ADI_PMU_BASE                    0x40000C00UL
#define ADI_SPI_HOST_BASE               0x40001800UL
#define ADI_IRQ_CTRL_BASE               0x40003800UL
#define ADI_AFC_BASE                    0x400041F8UL
#define ADI_CRMGT_BASE                  0x40004270UL
#define ADI_MISC_BASE                   0x400042B4UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define ADI_VCO_CAL_RESULTS             ((ADI_VCO_CAL_RESULTS_Type*) ADI_VCO_CAL_RESULTS_BASE)
#define ADI_PMU                         ((ADI_PMU_Type            *) ADI_PMU_BASE)
#define ADI_SPI_HOST                    ((ADI_SPI_HOST_Type       *) ADI_SPI_HOST_BASE)
#define ADI_IRQ_CTRL                    ((ADI_IRQ_CTRL_Type       *) ADI_IRQ_CTRL_BASE)
#define ADI_AFC                         ((ADI_AFC_Type            *) ADI_AFC_BASE)
#define ADI_CRMGT                       ((ADI_CRMGT_Type          *) ADI_CRMGT_BASE)
#define ADI_MISC                        ((ADI_MISC_Type           *) ADI_MISC_BASE)

/** @} */ /* End of group HW_Struct Hardware Structures Definition */
/** @} */ /* End of group PHY_headers PHY */
/** @} */ /* End of group adf7030-1_headers Headers */
/** @} */ /* End of group adf7030-1 ADF7030-1 Driver */

#ifdef __cplusplus
}
#endif


#endif  /* ADF7030_HW_CDEF_H */


