/**
 * @file 	:	SD.c
 * @author 	:	Srinidhi G
 * @brief 	:	This function implements the functions defined in SD.h
 * @version :	1.0
 * @date 	:	2023-01-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include"SD.h"
uint8_t flag;
uint8_t CMD0[] = {0x40, 0x00,0x00, 0x00,0x00, 0x95,0xff};
uint8_t CMD8[] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87,0xff};
uint8_t CMD58[] = {0x7A,0x00,0x00,0x00,0x00,0xFD,0xFF};
uint8_t CMD55[] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x65,0xff};	// last 0xff is use to give sd card buffer clock
uint8_t ACMD41[] = {0x69, 0x40, 0x00, 0x00, 0x00, 0xE5,0xff};
uint8_t CMD1[] = {0x41,0x00,0x00,0x00,0x00,0xF9,0xff};
uint8_t SD_Init() {
	flag = 0;
	MSS_GPIO_set_output( SD_CARD_GPIO,1);
	uint8_t rx_buffer[5] = {0x00,0x00,0x00,0x00,0x00};

	uint32_t i = 0;

	for(i =0; i<10; i++){
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
	}

	MSS_SPI_set_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
	do {
		MSS_SPI_transfer_block(&g_mss_spi1, CMD0, 7, rx_buffer, 1);
		i++;
		if(rx_buffer[0] == 1) {
			break;
		}
	} while(rx_buffer[0] != 1 && i < 255);


	i = 0;


	do {
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_block(&g_mss_spi1, CMD8, 7, rx_buffer, 5);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);

		i++;
		if(rx_buffer[4] == 0xaa && rx_buffer[3] == 0x01 && rx_buffer[2] == 0x00 && rx_buffer[1] == 0x00) {
			flag =  1;
			break;
		}

	} while( i < 255);
	if(i >= 255) {
		MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		return 1;
	}


	rx_buffer[0] = 0x00;
	rx_buffer[1] = 0x00;
	rx_buffer[2] = 0x00;
	rx_buffer[3] = 0x00;
	rx_buffer[4] = 0x00;
	i = 0;
	flag = 0;
	do {
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_block(&g_mss_spi1, CMD58, 7, rx_buffer, 5);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);

		i++;

		if(rx_buffer[3] == 0x80 && rx_buffer[2] == 0xFF) {
			flag = 1;
			break;
		}

	} while( i < 255);
	if(i >= 255) {
		MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		return 1;
	}



	rx_buffer[0] = 0x00;
	rx_buffer[1] = 0x00;
	rx_buffer[2] = 0x00;
	rx_buffer[3] = 0x00;
	rx_buffer[4] = 0x00;
	i = 0;
	flag = 0;


	do {
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_block(&g_mss_spi1, CMD55, 7, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_block(&g_mss_spi1, ACMD41, 7, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);

		i++;

		if(rx_buffer[0] == 0x00) {
			flag = 1;
			break;
		}


	} while( i <2550);
	if(flag == 0) {
		MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		return 1;
	}

	rx_buffer[0] = 0x00;
	rx_buffer[1] = 0x00;
	rx_buffer[2] = 0x00;
	rx_buffer[3] = 0x00;
	rx_buffer[4] = 0x00;
	i = 0;
	flag = 0;
	do {
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_block(&g_mss_spi1, CMD58, 7, rx_buffer, 5);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);

		i++;

		if(rx_buffer[3] == 0x80 && rx_buffer[2] == 0xFF) {
			flag = 1;
			break;
		}

	} while( i < 255);

	if(i >= 255) {
		MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		return 1;
	}

	if(flag == 1) {
		MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		return 0;
	}
	MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
	return 1;
}

uint8_t SD_Write(uint32_t addr,uint8_t *buff) {
	uint8_t CMD24[] = {0x58,((addr >> 24) & 0xff ),((addr >> 16) & 0xff ),((addr >> 8) & 0xff ),(addr & 0xff ),0xFF,0xff};
	uint8_t rx_buffer[5] = {0x00,0x00,0x00,0x00,0x00};
	uint8_t flag = 0;
	uint8_t start_flag = 0xFE;
	uint32_t i = 0,j = 0;

	do {
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_set_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_block(&g_mss_spi1, CMD24, 7, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);

		i++;

		if(rx_buffer[0] == 0x00) {
			MSS_SPI_transfer_block(&g_mss_spi1, &start_flag, 1, rx_buffer, 1);
			MSS_SPI_transfer_block(&g_mss_spi1, buff, 512, rx_buffer, 1);
//			MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
//			MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
			while(j<100){
				rx_buffer[0] = MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
				if((rx_buffer[0]&0x1F) == 0x05) {
					flag = 1;
					break;
				}
				j++;
			}
			if(flag == 1) {
				flag = 0;
				j = 0;
				while(j<100) {
					rx_buffer[0] = MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
					if(rx_buffer[0]!=0x00) {
						flag = 1;
						MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
						MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
						MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
						return 0;
					}
					j++;
				}
			}
			break;
		}

	} while( i < 255);

	if(i >= 255) {
		MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		return 1;
	}
	MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
	return 1;
}

uint8_t SD_Read(const uint32_t addr,uint8_t *buff) {
	uint8_t CMD17[] = {0x51,((addr >> 24) & 0xff ),((addr >> 16) & 0xff ),((addr >> 8) & 0xff ),(addr & 0xff ),0xFF,0xff};
	uint8_t rx_buffer[5] = {0x00,0x00,0x00,0x00,0x00};
	uint8_t flag = 0;
	uint8_t dummy_data = 0xff;
	uint32_t i = 0;
	uint16_t c = 0;

	do {
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_set_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
		MSS_SPI_transfer_block(&g_mss_spi1, CMD17, 7, rx_buffer, 1);
		while(c<250) {
			rx_buffer[0] = MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
			c++;
			if(rx_buffer[0] == 0xFE) {
				MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
				MSS_SPI_transfer_block(&g_mss_spi1,&dummy_data,0,buff,512);
//				MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
//				MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
				flag = 1;
				MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
				MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
				MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
				return 0;
				break;
			}
		}
		i++;
	}while(i<100);

	MSS_SPI_clear_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xff);
	return 1;
}
