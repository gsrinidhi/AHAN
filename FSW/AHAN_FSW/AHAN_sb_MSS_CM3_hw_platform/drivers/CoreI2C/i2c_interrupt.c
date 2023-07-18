/*******************************************************************************
 * (c) Copyright 2009-2018 Microsemi SoC Products Group.  All rights reserved.
 * 
 * CoreI2C driver interrupt control.
 * 
 * SVN $Revision: 9749 $
 * SVN $Date: 2018-02-12 18:26:37 +0530 (Mon, 12 Feb 2018) $
 */
#include "hal.h"
#include "hal_assert.h"
#include "core_i2c.h"
#include "../CMSIS/m2sxxx.h"
extern i2c_instance_t g_core_i2c0;
extern i2c_instance_t g_core_i2c1;
extern i2c_instance_t g_core_i2c2;
extern i2c_instance_t g_core_i2c3;
extern i2c_instance_t g_core_i2c4;
extern i2c_instance_t g_core_i2c5;
//NVIC_EnableIRQ( FabricIrq0_IRQn );
/*------------------------------------------------------------------------------
 * This function must be modified to enable interrupts generated from the
 * CoreI2C instance identified as parameter.
 */
void I2C_enable_irq( i2c_instance_t * this_i2c )
{
    //HAL_ASSERT(0)
	if(this_i2c == &g_core_i2c0)
	{
		NVIC_EnableIRQ( FabricIrq0_IRQn );
	}

	if(this_i2c == &g_core_i2c1)
	{
		NVIC_EnableIRQ( FabricIrq1_IRQn );
	}
	if(this_i2c == &g_core_i2c2)
	{
		NVIC_EnableIRQ( FabricIrq2_IRQn );
	}
	if(this_i2c == &g_core_i2c3)
	{
		NVIC_EnableIRQ( FabricIrq3_IRQn );
	}
	if(this_i2c == &g_core_i2c4)
	{
		NVIC_EnableIRQ( FabricIrq4_IRQn );
	}
	if(this_i2c == &g_core_i2c5)
	{
		NVIC_EnableIRQ( FabricIrq5_IRQn );
	}
}

/*------------------------------------------------------------------------------
 * This function must be modified to disable interrupts generated from the
 * CoreI2C instance identified as parameter.
 */
void I2C_disable_irq( i2c_instance_t * this_i2c )
{
    //HAL_ASSERT(0)
	if(this_i2c == &g_core_i2c0)
	{
		NVIC_DisableIRQ( FabricIrq0_IRQn );
	}

	if(this_i2c == &g_core_i2c1)
	{
		NVIC_DisableIRQ( FabricIrq1_IRQn );
	}

	if(this_i2c == &g_core_i2c2)
	{
		NVIC_DisableIRQ( FabricIrq2_IRQn );
	}

	if(this_i2c == &g_core_i2c3)
	{
		NVIC_DisableIRQ( FabricIrq3_IRQn );
	}

	if(this_i2c == &g_core_i2c4)
	{
		NVIC_DisableIRQ( FabricIrq4_IRQn );
	}

	if(this_i2c == &g_core_i2c5)
	{
		NVIC_DisableIRQ( FabricIrq5_IRQn );
	}
}
