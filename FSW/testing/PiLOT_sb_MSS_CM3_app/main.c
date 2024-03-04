
#include"test.h"
#include "master.h"
#include "slave.h"

void set_baud_rate(char* arg, uint8_t s);
void (*SET_BAUD_RATE)(char* arg, uint8_t s) = &set_baud_rate;
uint8_t add_command(char[],uint8_t);

#define CONFIG	1
#define CORE_SPI_0_0 0x50004000

int main(){

//	spi_instance_t g_new_core_spi;
//
//	uint8_t tx_buffer[3];
//	tx_buffer[0] = 0xaa;
//	tx_buffer[1] = 0xbb;
//	tx_buffer[2] = 0xcc;
//	uint8_t rx_buffer;
//
//	SPI_init(&g_new_core_spi, CORE_SPI_0_0, 16);
//
//	while(1){
//		SPI_set_slave_select(&g_new_core_spi, SPI_SLAVE_0);
//
//		SPI_transfer_block(&g_new_core_spi, tx_buffer, 3, &rx_buffer, 0);
//
//		SPI_clear_slave_select(&g_new_core_spi, SPI_SLAVE_0);
//	}



#if CONFIG == 1
	master();
#else
	slave_init();
#endif
	while(1) {

	}
    return 0;
}
