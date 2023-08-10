#ifndef AHAN_HW_PLATFORM_H_
#define AHAN_HW_PLATFORM_H_
/*****************************************************************************
*
*Created by Microsemi SmartDesign  Sat Jun 25 14:15:30 2022
*
*Memory map specification for peripherals in AHAN
*/

/*-----------------------------------------------------------------------------
* CM3 subsystem memory map
* Master(s) for this subsystem: CM3 
*---------------------------------------------------------------------------*/
#define COREI2C_0_0                     0x50000000U
#define COREI2C_1_0                     0x50001000U
#define COREI2C_2_0                     0x50002000U
#define COREI2C_3_0                     0x50003000U
#define COREI2C_4_0                     0x50004000U
#define COREI2C_5_0                     0x50005000U
//#define COREGPIO_0_0                    0x50006000U
#define COREUARTAPB_1_0                 0x50007000U
#define COREUARTAPB_2_0                 0x50008000U
#define COREUARTAPB_3_0                 0x50009000U
#define CORETIMER_0_0					0x5000A000U
#define CORETIMER_1_0					0x5000B000U
#define CORETIMER_2_0					0x5000C000U
#define CORESPI_0_0                     0x50006000U

#endif /* AHAN_HW_PLATFORM_H_*/
