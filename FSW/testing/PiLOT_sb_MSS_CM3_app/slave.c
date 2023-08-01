/*
 * slave.c
 *
 *  Created on: 28-Feb-2023
 *      Author: gonah
 */


#include "slave.h"
#define BUFFER_SIZE 256
uint8_t g_slave_tx_buffer[256];
uint8_t g_slave_rx_buffer[256];

mss_i2c_slave_handler_ret_t mss_slave_write_handler
    (
        mss_i2c_instance_t * this_i2c,
        uint8_t * p_rx_data,
        uint16_t rx_size
    )
{
	if(rx_size > BUFFER_SIZE) {
		rx_size = BUFFER_SIZE;
	}

	uint8_t loop_count;
	for(loop_count = 0;loop_count < rx_size;loop_count++){
		g_slave_tx_buffer[loop_count] = g_slave_rx_buffer[loop_count];
	}

	return MSS_I2C_REENABLE_SLAVE_RX;
}
void slave_init() {
	MSS_I2C_init(&g_mss_i2c1,I2C_SLAVE_ADDRESS,MSS_I2C_PCLK_DIV_60);
	MSS_I2C_set_slave_rx_buffer( &g_mss_i2c1, g_slave_rx_buffer,
										 sizeof(g_slave_rx_buffer) );
	MSS_I2C_set_slave_tx_buffer(&g_mss_i2c1,g_slave_tx_buffer,sizeof(g_slave_tx_buffer));
	MSS_I2C_register_write_handler( &g_mss_i2c1, mss_slave_write_handler );
	MSS_I2C_enable_slave( &g_mss_i2c1 );
    I2C_init(&g_core_i2c0, COREI2C_0_0, I2C_SLAVE_ADDRESS, I2C_PCLK_DIV_960);
    I2C_init(&g_core_i2c1, COREI2C_1_0, I2C_SLAVE_ADDRESS, I2C_PCLK_DIV_960);
    I2C_init(&g_core_i2c2, COREI2C_2_0, I2C_SLAVE_ADDRESS, I2C_PCLK_DIV_960);
    I2C_init(&g_core_i2c3, COREI2C_3_0, I2C_SLAVE_ADDRESS, I2C_PCLK_DIV_960);

    I2C_set_slave_rx_buffer( &g_core_i2c0, g0_slave_rx_buffer, sizeof(g0_slave_rx_buffer) );
    I2C_set_slave_tx_buffer(&g_core_i2c0, g0_slave_tx_buffer, sizeof(g0_slave_tx_buffer));
    I2C_register_write_handler(&g_core_i2c0, write_handler);
    I2C_enable_slave(&g_core_i2c0);



    I2C_set_slave_rx_buffer( &g_core_i2c1, g1_slave_rx_buffer, sizeof(g1_slave_rx_buffer) );
    I2C_set_slave_tx_buffer(&g_core_i2c1, g1_slave_tx_buffer, sizeof(g1_slave_tx_buffer));
    I2C_register_write_handler(&g_core_i2c1, write_handler);
    I2C_enable_slave(&g_core_i2c1);

    I2C_set_slave_rx_buffer( &g_core_i2c2, g2_slave_rx_buffer, sizeof(g2_slave_rx_buffer) );
    I2C_set_slave_tx_buffer(&g_core_i2c2, g2_slave_tx_buffer, sizeof(g2_slave_tx_buffer));
    I2C_register_write_handler(&g_core_i2c2, write_handler);
    I2C_enable_slave(&g_core_i2c2);

    I2C_set_slave_rx_buffer( &g_core_i2c3, g3_slave_rx_buffer, sizeof(g3_slave_rx_buffer) );
    I2C_set_slave_tx_buffer(&g_core_i2c3, g3_slave_tx_buffer, sizeof(g3_slave_tx_buffer));
    I2C_register_write_handler(&g_core_i2c3, write_handler);
    I2C_enable_slave(&g_core_i2c3);
}
