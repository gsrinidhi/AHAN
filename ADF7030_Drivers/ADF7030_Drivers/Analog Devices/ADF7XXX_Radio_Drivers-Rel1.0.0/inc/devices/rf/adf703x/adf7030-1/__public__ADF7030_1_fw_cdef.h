/*!
 *****************************************************************************
  @file:	__public__ADF7030_1_fw_cdef.h 
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


#ifndef ADF7030_FW_CDEF_H
#define ADF7030_FW_CDEF_H

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

/** @addtogroup FW_Struct Firmware Structures Definition
  * @{
  */
  

/* ================================================================================ */
/* ================                    SM_CONFIG                   ================ */
/* ================================================================================ */


/**
  * @brief Internal State Machine Configuration (SM_CONFIG)
  */

typedef struct {                                    /*!< SM_CONFIG Structure                                                   */
         uint32_t  RESERVED0[11];
       uint8_t   GPIO_CMD_0;                        /*!< State machine command triggered by IRQ_IN0                            */
         uint8_t   RESERVED1[1];
       uint8_t   GPIO_CMD_1;                        /*!< State machine command triggered by IRQ_IN1                            */
} ADI_SM_CONFIG_Type;


/* ================================================================================ */
/* ================                     SM_DATA                    ================ */
/* ================================================================================ */


/**
  * @brief Live State Machine Running data (SM_DATA)
  */

typedef struct {                                    /*!< SM_DATA Structure                                                     */
         uint32_t  RESERVED0[4];
       uint32_t  CALIBRATION;                       /*!< Only for documentation                                                */
} ADI_SM_DATA_Type;


/* ================================================================================ */
/* ================                     PROFILE                    ================ */
/* ================================================================================ */


/**
  * @brief Radio Profile Configuration (PROFILE)
  */

typedef struct {                                    /*!< PROFILE Structure                                                     */
         uint32_t  RESERVED0;
  
  union {
         uint32_t  REF_CLK_CFG;                     /*!< Reference clock configuration                                         */
    
    struct {
           uint32_t             : 26;
           uint32_t  CLK_TYPE   :  1;               /*!< External Reference Clock Source Type                                  */
    } REF_CLK_CFG_b;                                /*!< BitSize                                                               */
  };
       uint32_t  CH_FREQ;                           /*!< RF channel frequency                                                  */
         uint32_t  RESERVED1;
  
  union {
         uint32_t  PACKET_CFG;                      /*!< Packet handler configuration                                          */
    
    struct {
           uint32_t             : 14;
           uint32_t  TYPE_FRAME0:  2;               /*!< Select the packet format                                              */
    } PACKET_CFG_b;                                 /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  RADIO_MODES;                     /*!< Radio mode configuration                                              */
    
    struct {
           uint32_t             :  5;
           uint32_t  COMBINED_TRX_MATCH:  2;        /*!< Combined match configuration                                          */
           uint32_t             : 12;
           uint32_t  GPIO_CLK_FREQ_SEL:  3;         /*!< Selection of Clock Frequency on selected GPIO with command GEN_GPIO_CLK */
    } RADIO_MODES_b;                                /*!< BitSize                                                               */
  };
         uint32_t  RESERVED2[2];
  
  union {
         uint32_t  RADIO_DIG_TX_CFG0;               /*!< Tx Configuration 0                                                    */
    
    struct {
           uint32_t             :  1;
           uint32_t  TX_FILTER_ENABLE:  1;          /*!< TX Filter state                                                       */
           uint32_t  TX_GAUSSIAN_BT:  2;            /*!< Time Constant (Bt Value) of Gaussian Filter                           */
           uint32_t             :  8;
           uint32_t  PA_COARSE  :  4;               /*!< PA Output Power Coarse Value                                          */
           uint32_t  PA_FINE    :  7;               /*!< PA Output Power Fine Value. Value may be 0 or 3 to 127 inclusive.     */
           uint32_t  PA_MICRO   :  7;               /*!< PA Output Power Micro Value                                           */
           uint32_t  PA_SEL     :  1;               /*!< PA selected for Tx                                                    */
    } RADIO_DIG_TX_CFG0_b;                          /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  RADIO_DIG_TX_CFG1;               /*!< Tx Configuration 1                                                    */
    
    struct {
           uint32_t             : 12;
           uint32_t  PA_RAMP_RATE:  3;              /*!< Internal PA Ramp Rate                                                 */
           uint32_t  EXT_PA_OOK_BIT_FRAMING_EN:  1; /*!< External PA OOK Bit Framing Configuration                             */
           uint32_t  EXT_PA_FRAMING_EN:  1;         /*!< External PA Framing Configuration                                     */
           uint32_t  EXT_PA_PIN_SEL:  3;            /*!< GPIO selection for External PA control                                */
           uint32_t             :  4;
           uint32_t  EXT_LNA_FRAMING_EN:  1;        /*!< External LNA Framing Configuration                                    */
           uint32_t  EXT_LNA_PIN_SEL:  3;           /*!< GPIO selection for External LNA                                       */
    } RADIO_DIG_TX_CFG1_b;                          /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  RADIO_DIG_TX_CFG2;               /*!< Tx Configuration 2                                                    */
    
    struct {
           uint32_t  PAOLDO_VOUT_CON:  4;           /*!< LDO output Voltage Regulator Trim voltage                             */
    } RADIO_DIG_TX_CFG2_b;                          /*!< BitSize                                                               */
  };
         uint32_t  RESERVED3[4];
  
  union {
         uint32_t  RADIO_AFC_CFG2;                  /*!< AFC Configuration Register 2                                          */
    
    struct {
           uint32_t  AFC_MODE   :  3;               /*!< AFC mode                                                              */
    } RADIO_AFC_CFG2_b;                             /*!< BitSize                                                               */
  };
         uint32_t  RESERVED4[17];
       uint32_t  RADIO_CAL_CFG0;                    /*!< Calibration Configuration 0                                           */
  
  union {
         uint32_t  RADIO_CAL_CFG1;                  /*!< Calibration Configuration 1                                           */
    
    struct {
           uint32_t             : 29;
           uint32_t  CAL_SUCCESS:  1;               /*!< Cal Success                                                           */
    } RADIO_CAL_CFG1_b;                             /*!< BitSize                                                               */
  };
       uint32_t  RADIO_CAL_CFG2;                    /*!< Calibration Configuration 2                                           */
  
  union {
         uint32_t  RSSI_CFG;                        /*!< RSSI configuration                                                    */
    
    struct {
           uint32_t  WB_OFFSET  : 10;               /*!< Wideband RSSI offset in units of 0.36dBm. The offset value is
                                                         an unsigned 10-bit number.                                            */
           uint32_t             :  6;
           uint32_t  NB_OFFSET  : 10;               /*!< Narrowband RSSI offset in units of 0.25dBm. The offset value
                                                         is an unsigned 10-bit number.                                         */
    } RSSI_CFG_b;                                   /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  CCA_CFG;                         /*!< CCA configuration                                                     */
    
    struct {
           uint32_t  TICK_RATE  :  4;               /*!< This field sets the number of CCA ticks per RX data bit period        */
           uint32_t  TICK_POSTSCALAR:  4;           /*!< Sets the number of CCA ticks between RSSI samples                     */
           uint32_t  DETECTION_TIME:  8;            /*!< The number of RSSI samples taken before CCA detection period
                                                         ends. A value 0 implies 'infinity mode' whereby CCA live status
                                                          is continually updated and the ADF7030-1 stays in the CCA state      */
           uint32_t  THRESHOLD  : 11;               /*!< Signed 11-bit number representing the CCA RSSI threshold, in
                                                         units of 0.25 dBm.                                                    */
    } CCA_CFG_b;                                    /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  CCA_READBACK;                    /*!< CCA readback                                                          */
    
    struct {
           uint32_t  VALUE      : 11;               /*!< Signed 11-bit number representing the CCA RSSI value read in
                                                         units of 0.25 dBm                                                     */
           uint32_t             :  3;
           uint32_t  LIVE_STATUS:  1;               /*!< Live indication of CCA status, updated every RSSI sample              */
           uint32_t  STATUS     :  1;               /*!< Indicates CCA status at end of DETECTION_TIME                         */
    } CCA_READBACK_b;                               /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  LPM_CFG0;                        /*!< Low power mode configuration                                          */
    
    struct {
           uint32_t             : 10;
           uint32_t  RTC_EN     :  1;               /*!< Configure RTC alarm to wake device on expiry                          */
           uint32_t             :  1;
           uint32_t  RTC_RESYNC :  1;               /*!< Realign the RTC clock on entering PHY_SLEEP                           */
           uint32_t  RTC_RECONFIG_EN:  1;           /*!< Autoclearing RTC configuration enable flag. Set to 1 to trigger
                                                         an RTC reconfiguration on CMD_CFG_DEV                                 */
           uint32_t             :  1;
           uint32_t  RTC_LF_SRC_SEL:  1;            /*!< Use LFRC or LFXTAL as RTC source                                      */
           uint32_t  RETAIN_SRAM:  1;               /*!< Enable retention of the BBRAM during PHY_SLEEP                        */
           uint32_t             : 14;
           uint32_t  ENABLE     :  1;               /*!< Global enable/disable for RTC/sequencer/BBRAM retention/GPIO
                                                         behavior on startup                                                   */
    } LPM_CFG0_b;                                   /*!< BitSize                                                               */
  };
       uint32_t  LPM_CFG1;                          /*!< RTC configuration                                                     */
         uint32_t  RESERVED5;
  
  union {
         uint32_t  MONITOR1;                        /*!< Monitor readback                                                      */
    
    struct {
           uint32_t  TEMP_OUTPUT: 12;               /*!< Temperature as a signed 12bit number in units of 0.0625 degrees
                                                         Celsius.                                                              */
    } MONITOR1_b;                                   /*!< BitSize                                                               */
  };
         uint32_t  RESERVED6;
  
  union {
         uint32_t  GPCON0_3;                        /*!< GPIO0 to GPIO3 pin functionality selection                            */
    
    struct {
           uint32_t  PIN0_CFG   :  6;               /*!< GPIO0 Configuration                                                   */
           uint32_t             :  2;
           uint32_t  PIN1_CFG   :  6;               /*!< GPIO1 Configuration                                                   */
           uint32_t             :  2;
           uint32_t  PIN2_CFG   :  6;               /*!< GPIO2 Configuration                                                   */
           uint32_t             :  2;
           uint32_t  PIN3_CFG   :  6;               /*!< GPIO3 Configuration                                                   */
    } GPCON0_3_b;                                   /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  GPCON4_7;                        /*!< GPIO4 to GPIO7 pin functionality selection                            */
    
    struct {
           uint32_t  PIN4_CFG   :  6;               /*!< GPIO4 Configuration                                                   */
           uint32_t             :  2;
           uint32_t  PIN5_CFG   :  6;               /*!< GPIO5 Configuration                                                   */
           uint32_t             :  2;
           uint32_t  PIN6_CFG   :  6;               /*!< GPIO6 Configuration                                                   */
           uint32_t             :  2;
           uint32_t  PIN7_CFG   :  6;               /*!< GPIO7 Configuration                                                   */
    } GPCON4_7_b;                                   /*!< BitSize                                                               */
  };
         uint32_t  RESERVED7;
       uint32_t  SPARE0;                            /*!< Spare Register 0                                                      */
       uint32_t  SPARE1;                            /*!< Spare Register 1                                                      */
       uint32_t  SPARE2;                            /*!< Spare Register 2                                                      */
       uint32_t  SPARE3;                            /*!< Spare Register 3                                                      */
       uint32_t  SPARE4;                            /*!< Spare Register 4                                                      */
       uint32_t  SPARE5;                            /*!< Spare Register 5                                                      */
       uint32_t  SPARE6;                            /*!< Spare Register 6                                                      */
       uint32_t  SPARE7;                            /*!< Spare Register 7                                                      */
       uint32_t  SPARE8;                            /*!< Spare Register 8                                                      */
       uint32_t  SPARE9;                            /*!< Spare Register 9                                                      */
       uint32_t  RADIO_CAL_RESULTS0;                /*!< Radio Calibration Results 0                                           */
       uint32_t  RADIO_CAL_RESULTS1;                /*!< Radio Calibration Results 1                                           */
       uint32_t  RADIO_CAL_RESULTS2;                /*!< Radio Calibration Results 2                                           */
       uint32_t  RADIO_CAL_RESULTS3;                /*!< Radio Calibration Results 3                                           */
       uint32_t  RADIO_CAL_RESULTS4;                /*!< Radio Calibration Results 4                                           */
       uint32_t  RADIO_CAL_RESULTS5;                /*!< Radio Calibration Results 5                                           */
       uint32_t  RADIO_CAL_RESULTS6;                /*!< Radio Calibration Results 6                                           */
       uint32_t  RADIO_CAL_RESULTS7;                /*!< Radio Calibration Results 7                                           */
       uint32_t  RADIO_CAL_RESULTS8;                /*!< Radio Calibration Results 8                                           */
} ADI_PROFILE_Type;


/* ================================================================================ */
/* ================                   GENERIC_PKT                  ================ */
/* ================================================================================ */


/**
  * @brief Generic Packet Configuration (GENERIC_PKT)
  */

typedef struct {                                    /*!< GENERIC_PKT Structure                                                 */
  
  union {
         uint32_t  BUFF_CFG0;                       /*!< Transmit/Receive Buffer Configuration 0                               */
    
    struct {
           uint32_t  PTR_RX_BASE: 11;               /*!< Rx base buffer offset pointer. The base address of the Rx payload
                                                         is 0x2000000 + (PTR_RX_BASE x 4)                                      */
           uint32_t  PTR_TX_BASE: 11;               /*!< Tx base buffer offset pointer. The base address of the Tx payload
                                                         is 0x2000000 + (PTR_TX_BASE x 4)                                      */
           uint32_t  BIT2AIR    :  1;               /*!< For generic packet format Tx: specifies which bit of payload
                                                         bytes is Tx first (0 = most significant bit (MSB) first). For
                                                          generic packet format Rx: specifies into which bit the first
                                                          bit received of payload is written (0 = MSB)                         */
           uint32_t             :  1;
           uint32_t  ROLLING_BUFF_EN:  1;           /*!< Enable the rolling buffer mode. When the number of bytes received
                                                         equals RX_SIZE/2 or TX_SIZE/2, the half full IRQ is asserted.
                                                          When number of bytes received equals RX_SIZE or TX_SIZE, the
                                                          full IRQ is asserted.                                                */
    } BUFF_CFG0_b;                                  /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  BUFF_CFG1;                       /*!< Transmit/Receive Buffer Configuration 1                               */
    
    struct {
           uint32_t  RX_SIZE    :  9;               /*!< Maximum size of the Rx buffer.                                        */
           uint32_t  TX_SIZE    :  9;               /*!< Maximum size of the Tx buffer in octets. In nonrolling buffer
                                                         mode, the ADF7030-1 does not transmit data written beyond the
                                                          buffer delimited by this size. In rolling buffer mode, this
                                                          is the size of the Tx buffer.                                        */
           uint32_t  TRX_BLOCK_SIZE:  8;            /*!< Set the multiple of bytes for which the PAYLOAD_BLOC_IRQ interrupt
                                                         is asserted during packet reception or transmission (not used
                                                          for IEEE802.15.4g). e.g. Set to 4 to cause an IRQ every 4 bytes.     */
           uint32_t             :  1;
           uint32_t  TX_BUFF_RAWDATA:  1;           /*!< Transmit only the payload                                             */
           uint32_t             :  1;
           uint32_t  TURNAROUND_RX:  1;             /*!< Enable automatic PHY_RX to PHY_TX transition on completion of
                                                         packet reception if packet with correct CRC (if CRC enabled)          */
           uint32_t             :  1;
           uint32_t  TURNAROUND_TX:  1;             /*!< Enable automatic PHY_TX to PHY_RX transition on completion of
                                                         packet transmission                                                   */
    } BUFF_CFG1_b;                                  /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  FRAME_CFG0;                      /*!< Generic Packet Frame Configuration 0                                  */
    
    struct {
           uint32_t  PREAMBLE_LEN:  8;              /*!< Number of units of preamble at start of Tx packet (TX only);
                                                         see also PREAMBLE_UNIT                                                */
           uint32_t             :  8;
           uint32_t  SYNC0_LEN  :  6;               /*!< Length of the Syncword 0 in bits (Rx and Tx)                          */
           uint32_t             :  2;
           uint32_t  CRC_LEN    :  6;               /*!< Generic packet: CRC length used in Rx and Tx. IEEE802.15.4g:
                                                         FCS length used in Tx only; Rx FCS length inferred from received
                                                          PHR.                                                                 */
    } FRAME_CFG0_b;                                 /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  FRAME_CFG1;                      /*!< Generic Packet Frame Configuration 1                                  */
    
    struct {
           uint32_t  PAYLOAD_SIZE: 12;              /*!< Generic packet only: sets number of payload bytes in the Tx
                                                         packet (Raw mode only). Sets the number of payload bytes that
                                                          are received in the incoming packet payload (Rx raw mode only).      */
           uint32_t  PREAMBLE_UNIT:  1;             /*!< Unit of preamble length for Tx                                        */
           uint32_t             :  3;
           uint32_t  TRX_IRQ0_TYPE:  8;             /*!< Select sources of interrupt on IRQ_OUT0 during Rx and Tx              */
           uint32_t  TRX_IRQ1_TYPE:  8;             /*!< Select sources of interrupt on IRQ_OUT1 during Rx & Tx                */
    } FRAME_CFG1_b;                                 /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  FRAME_CFG2;                      /*!< Generic Packet Frame Configuration 2                                  */
    
    struct {
           uint32_t             :  3;
           uint32_t  SYNC1_LEN  :  6;               /*!< Length of the Syncword 1 in bits. Only used in IEEE802.15.4g
                                                         Rx                                                                    */
           uint32_t             :  2;
           uint32_t  CRC_SHIFT_IN_ZEROS:  1;        /*!< Shift in CRC length of zeros after all bytes have passed through
                                                         CRC calculation. Determines whether the final register value
                                                          is reversed                                                          */
           uint32_t  LEN_SEL    :  2;               /*!< Selects the size of the length field in the received or transmitted
                                                         message (Rx and Tx generic packet only)                               */
           uint32_t             :  2;
           uint32_t  PREAMBLE_VAL:  8;              /*!< For Tx this is the preamble pattern used in the outgoing packet.
                                                         For Rx, this must be set to the expected preamble, e.g. 0x55
                                                          or 0xAA. If this field is set to 0 a default value of 0x55 is
                                                          used.                                                                */
           uint32_t  ENDEC_MODE :  8;               /*!< Line coding scheme (generic packet Tx only)                           */
    } FRAME_CFG2_b;                                 /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  FRAME_CFG3;                      /*!< Generic Packet Frame Configuration 3                                  */
    
    struct {
           uint32_t             : 16;
           uint32_t  RX_LENGTH  : 16;               /*!< Generic packet Rx: The contents of the length field in the received
                                                         packet. IEEE802.15.4g: the received PHR.                              */
    } FRAME_CFG3_b;                                 /*!< BitSize                                                               */
  };
         uint32_t  RESERVED0;
  
  union {
         uint32_t  FRAME_CFG5;                      /*!< Generic Packet Frame Configuration 5                                  */
    
    struct {
           uint32_t  TX_PHR     : 16;               /*!< PHY Header (PHR) used as first two octets of IEEE802.15.4g Tx
                                                         packet                                                                */
    } FRAME_CFG5_b;                                 /*!< BitSize                                                               */
  };
       uint32_t  SYNCWORD0;                         /*!< Sync Word 0                                                           */
       uint32_t  SYNCWORD1;                         /*!< Sync Word 1                                                           */
       uint32_t  CRC_POLY;                          /*!< CRC polynomial                                                        */
       uint32_t  CRC_SEED;                          /*!< CRC initial seed                                                      */
       uint32_t  CRC_FINAL_XOR;                     /*!< CRC XOR value                                                         */
         uint32_t  RESERVED1[4];
  
  union {
         uint32_t  LIVE_LINK_QUAL;                  /*!< RX link quality readback                                              */
    
    struct {
           uint32_t             : 16;
           uint32_t  RSSI       : 11;               /*!< RSSI as a signed 11-bit value in units of 0.25 dBm measured
                                                         during packet reception                                               */
    } LIVE_LINK_QUAL_b;                             /*!< BitSize                                                               */
  };
       uint32_t  MISC0;                             /*!< Miscellaneous register 0                                              */
       uint32_t  MISC1;                             /*!< Miscellaneous register 1                                              */
  
  union {
         uint32_t  LPM_CFG;                         /*!< Low Power Mode Configuration                                          */
    
    struct {
           uint32_t             :  8;
           uint32_t  PREAMBLE_DWELL_TIME:  8;       /*!< Number of symbols allowed for start of syncword                       */
           uint32_t  PREAMBLE_QUAL_DWELL_TIME:  8;  /*!< Number of symbols allowed for AFC qualification                       */
           uint32_t  PREAMBLE_DETECT_DWELL_TIME:  8;/*!< Number of symbols allowed for RSSI qualification                      */
    } LPM_CFG_b;                                    /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  TEST_MODES0;                     /*!< Test Mode Configuration 0                                             */
    
    struct {
           uint32_t             : 16;
           uint32_t  TX_TEST    :  4;               /*!< Continuous Tx test modes                                              */
    } TEST_MODES0_b;                                /*!< BitSize                                                               */
  };
         uint32_t  RESERVED2[2];
  
  union {
         uint32_t  LCPSM1;                          /*!< Low Current Packet Search Mode Configuration 1                        */
    
    struct {
           uint32_t  LCPSM_ENERGY_CNT: 16;          /*!< Number of samples of energy to take during LCPSM energy detection
                                                         phase                                                                 */
           uint32_t  LCPSM_LOW_CNT: 16;             /*!< Number of SERDES interrupts to to remain in LCPSM idle state
                                                         for                                                                   */
    } LCPSM1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
         uint32_t  LCPSM2;                          /*!< Low Current Packet Search Mode Configuration 2                        */
    
    struct {
           uint32_t  LCPSM_AFC_THRESHOLD: 16;       /*!< AFC threshold to use while in LCPSM energy detection phase            */
           uint32_t             : 15;
             uint32_t  LCPSM_ENABLED:  1;             /*!< Indicate whether LCPSM is enabled or not. Note that this bit
                                                         only gets set on entry into PHY_RX when LCPSM is enabled. When
                                                          disabling LCPSM, this bit should be updated immediately.             */
    } LCPSM2_b;                                     /*!< BitSize                                                               */
  };
       uint32_t  MISC2;                             /*!< MISC2 Register                                                        */
       uint32_t  LCPSM3;                            /*!< Low Current Packet Search Mode Configuration 3                        */
} ADI_GENERIC_PKT_Type;


/* ================================================================================ */
/* ================                  ANAFILT_LUTS                  ================ */
/* ================================================================================ */


/**
  * @brief Analog filter LUTs (ANAFILT_LUTS)
  */

typedef struct {                                    /*!< ANAFILT_LUTS Structure                                                */
       uint32_t  DATA0;                             /*!< Not Available                                                         */
       uint32_t  DATA1;                             /*!< Not Available                                                         */
       uint32_t  DATA2;                             /*!< Not Available                                                         */
       uint32_t  DATA3;                             /*!< Not Available                                                         */
       uint32_t  DATA4;                             /*!< Not Available                                                         */
       uint32_t  DATA5;                             /*!< Not Available                                                         */
       uint32_t  DATA6;                             /*!< Not Available                                                         */
       uint32_t  DATA7;                             /*!< Not Available                                                         */
       uint32_t  DATA8;                             /*!< Not Available                                                         */
       uint32_t  DATA9;                             /*!< Not Available                                                         */
       uint32_t  DATA10;                            /*!< Not Available                                                         */
       uint32_t  DATA11;                            /*!< Not Available                                                         */
       uint32_t  DATA12;                            /*!< Not Available                                                         */
} ADI_ANAFILT_LUTS_Type;


/* ================================================================================ */
/* ================                  DIGFILT_LUTS                  ================ */
/* ================================================================================ */


/**
  * @brief Digital filter LUTs 1 (DIGFILT_LUTS)
  */

typedef struct {                                    /*!< DIGFILT_LUTS Structure                                                */
       uint32_t  DATA0;                             /*!< Not Available                                                         */
       uint32_t  DATA1;                             /*!< Not Available                                                         */
       uint32_t  DATA2;                             /*!< Not Available                                                         */
       uint32_t  DATA3;                             /*!< Not Available                                                         */
       uint32_t  DATA4;                             /*!< Not Available                                                         */
       uint32_t  DATA5;                             /*!< Not Available                                                         */
       uint32_t  DATA6;                             /*!< Not Available                                                         */
       uint32_t  DATA7;                             /*!< Not Available                                                         */
       uint32_t  DATA8;                             /*!< Not Available                                                         */
       uint32_t  DATA9;                             /*!< Not Available                                                         */
       uint32_t  DATA10;                            /*!< Not Available                                                         */
       uint32_t  DATA11;                            /*!< Not Available                                                         */
       uint32_t  DATA12;                            /*!< Not Available                                                         */
       uint32_t  DATA13;                            /*!< Not Available                                                         */
       uint32_t  DATA14;                            /*!< Not Available                                                         */
       uint32_t  DATA15;                            /*!< Not Available                                                         */
       uint32_t  DATA16;                            /*!< Not Available                                                         */
} ADI_DIGFILT_LUTS_Type;


/* ================================================================================ */
/* ================                  DIGFILT2_LUTS                 ================ */
/* ================================================================================ */


/**
  * @brief Digital filter LUTs (DIGFILT2_LUTS)
  */

typedef struct {                                    /*!< DIGFILT2_LUTS Structure                                               */
       uint32_t  DATA0;                             /*!< Not Available                                                         */
       uint32_t  DATA1;                             /*!< Not Available                                                         */
       uint32_t  DATA2;                             /*!< Not Available                                                         */
       uint32_t  DATA3;                             /*!< Not Available                                                         */
       uint32_t  DATA4;                             /*!< Not Available                                                         */
       uint32_t  DATA5;                             /*!< Not Available                                                         */
       uint32_t  DATA6;                             /*!< Not Available                                                         */
       uint32_t  DATA7;                             /*!< Not Available                                                         */
       uint32_t  DATA8;                             /*!< Not Available                                                         */
       uint32_t  DATA9;                             /*!< Not Available                                                         */
       uint32_t  DATA10;                            /*!< Not Available                                                         */
} ADI_DIGFILT2_LUTS_Type;


/* ================================================================================ */
/* ================                   PLLBW_LUTS                   ================ */
/* ================================================================================ */


/**
  * @brief PLL Bandwidth Lookup Tables (PLLBW_LUTS)
  */

typedef struct {                                    /*!< PLLBW_LUTS Structure                                                  */
       uint32_t  DATA0;                             /*!< Not Available                                                         */
       uint32_t  DATA1;                             /*!< Not Available                                                         */
} ADI_PLLBW_LUTS_Type;


/* ================================================================================ */
/* ================                  RSSICFG_LUTS                  ================ */
/* ================================================================================ */


/**
  * @brief RSSI Lookup Tables (RSSICFG_LUTS)
  */

typedef struct {                                    /*!< RSSICFG_LUTS Structure                                                */
       uint32_t  DATA0;                             /*!< Not Available                                                         */
       uint32_t  DATA1;                             /*!< Not Available                                                         */
       uint32_t  DATA2;                             /*!< Not Available                                                         */
       uint32_t  DATA3;                             /*!< Not Available                                                         */
       uint32_t  DATA4;                             /*!< Not Available                                                         */
       uint32_t  DATA5;                             /*!< Not Available                                                         */
       uint32_t  DATA6;                             /*!< Not Available                                                         */
       uint32_t  DATA7;                             /*!< Not Available                                                         */
       uint32_t  DATA8;                             /*!< Not Available                                                         */
       uint32_t  DATA9;                             /*!< Not Available                                                         */
       uint32_t  DATA10;                            /*!< Not Available                                                         */
       uint32_t  DATA11;                            /*!< Not Available                                                         */
       uint32_t  DATA12;                            /*!< Not Available                                                         */
       uint32_t  DATA13;                            /*!< Not Available                                                         */
       uint32_t  DATA14;                            /*!< Not Available                                                         */
} ADI_RSSICFG_LUTS_Type;




/* ============================================================================================ */
/* =======================             Firmware memory map             ======================== */
/* ============================================================================================ */

#define ADI_SM_CONFIG_BASE              0x200000D0UL
#define ADI_SM_DATA_BASE                0x20000120UL
#define ADI_PROFILE_BASE                0x200002E4UL
#define ADI_GENERIC_PKT_BASE            0x200004F4UL
#define ADI_ANAFILT_LUTS_BASE           0x2000060CUL
#define ADI_DIGFILT_LUTS_BASE           0x200006B4UL
#define ADI_DIGFILT2_LUTS_BASE          0x20000794UL
#define ADI_PLLBW_LUTS_BASE             0x20000820UL
#define ADI_RSSICFG_LUTS_BASE           0x20000864UL


/* ============================================================================================ */
/* ================             Firmware RAM structure declaration             ================ */
/* ============================================================================================ */

#define ADI_SM_CONFIG                   ((ADI_SM_CONFIG_Type      *) ADI_SM_CONFIG_BASE)
#define ADI_SM_DATA                     ((ADI_SM_DATA_Type        *) ADI_SM_DATA_BASE)
#define ADI_PROFILE                     ((ADI_PROFILE_Type        *) ADI_PROFILE_BASE)
#define ADI_GENERIC_PKT                 ((ADI_GENERIC_PKT_Type    *) ADI_GENERIC_PKT_BASE)
#define ADI_ANAFILT_LUTS                ((ADI_ANAFILT_LUTS_Type   *) ADI_ANAFILT_LUTS_BASE)
#define ADI_DIGFILT_LUTS                ((ADI_DIGFILT_LUTS_Type   *) ADI_DIGFILT_LUTS_BASE)
#define ADI_DIGFILT2_LUTS               ((ADI_DIGFILT2_LUTS_Type  *) ADI_DIGFILT2_LUTS_BASE)
#define ADI_PLLBW_LUTS                  ((ADI_PLLBW_LUTS_Type     *) ADI_PLLBW_LUTS_BASE)
#define ADI_RSSICFG_LUTS                ((ADI_RSSICFG_LUTS_Type   *) ADI_RSSICFG_LUTS_BASE)
  
/** @} */ /* End of group FW_Struct Firmware Structures Definition */
/** @} */ /* End of group PHY_headers PHY */
/** @} */ /* End of group adf7030-1_headers Headers */
/** @} */ /* End of group adf7030-1 ADF7030-1 Driver */

#ifdef __cplusplus
}
#endif


#endif  /* ADF7030_FW_CDEF_H */

