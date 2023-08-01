#include "master.h"

void I2C_Init() {
	MSS_I2C_init( &g_mss_i2c0, DUMMY_I2C_ADDR, MSS_I2C_PCLK_DIV_960 );
	I2C_init(&g_core_i2c1, COREI2C_1_0, DUMMY_I2C_ADDR, I2C_PCLK_DIV_960);
	I2C_init(&g_core_i2c2, COREI2C_2_0, DUMMY_I2C_ADDR, I2C_PCLK_DIV_960);
	I2C_init(&g_core_i2c3, COREI2C_3_0, DUMMY_I2C_ADDR, I2C_PCLK_DIV_960);
	I2C_init(&g_core_i2c4, COREI2C_4_0, DUMMY_I2C_ADDR, I2C_PCLK_DIV_960);
	I2C_init(&g_core_i2c5, COREI2C_5_0, DUMMY_I2C_ADDR, I2C_PCLK_DIV_960);

}

void GPIO_Init() {
	MSS_GPIO_init();
	MSS_GPIO_config(EN_SENSOR_BOARD,MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_config(EN_UART,MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_config( SD_CARD_GPIO , MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( TX_INV_EN , MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( RX_INV_EN , MSS_GPIO_OUTPUT_MODE );
}

void SPI_Init() {
	MSS_SPI_init(&g_mss_spi1);
	MSS_SPI_configure_master_mode(&g_mss_spi1, MSS_SPI_SLAVE_0, MSS_SPI_MODE0, 512, 8);
	SPI_init(&g_core_spi0,CORESPI_0_0,8);
	SPI_configure_master_mode(&g_core_spi0);
}

void Uart_Init() {
	MSS_UART_init(&g_mss_uart0,MSS_UART_9600_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
	MSS_UART_init(&g_mss_uart1,MSS_UART_9600_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
}

uint8_t Pilot_Peripherals_Init() {
	uint8_t res = 0;
	SYSREG->WDOG_CR = 0;
	GPIO_Init();
	I2C_Init();
	Uart_Init();
	SPI_Init();
//	res = SD_Init();
//	print_num("SD card init result \0",res);
	MSS_GPIO_set_output(TX_INV_EN,0);
	MSS_GPIO_set_output(RX_INV_EN,0);
	return res;
}


void init_test() {

	Pilot_Peripherals_Init();
	echo_str("Pilot Commanding\n\r\0");
//	i2c_argu_t i2c_argu;
//	i2c_results_t i2c_result;
//	i2c_argu.msg_type = 0;
//	i2c_argu.no_bytes = 200;
//	uint16_t clk_speed[] = {256,224,192,160,960,120,60};
//	uint8_t channel = 0,clock_rate = 0;
//	echo_str("PiLOT Starting\n\r\0");
//	echo_str("I2C_Testing\n\r\0");
//	for(;channel<=5;channel++) {
//		print_num("I2C Channel \0",channel);
//		print_num("No of bytes: \0",i2c_argu.no_bytes);
//		i2c_argu.i2c_chx = channel;
//		for(clock_rate = 0;clock_rate <= 6;clock_rate++){
//			i2c_argu.clock_speed = clock_rate;
//			i2c_test(&i2c_argu,&i2c_result);
//			print_num("I2C Clock Speed \0",(double)clk_speed[clock_rate]);
//			print_num("I2C write \0",(double)i2c_result.Tx_Status);
//			print_num("I2C write time \0",(double)i2c_result.Tx_time);
//			print_num("I2C read \0",(double)i2c_result.Rx_Status);
//			print_num("I2C read time \0",(double)i2c_result.Rx_time);
//			print_num("I2C mismatch \0",(double)i2c_result.no_Incorrect_bytes);
//		}
//	}
}

void master() {
	init_test();
	cli_init();
	while(1) {
        if(command_index > 1){
            if(c[command_index - 1] == '\r'){
                serial_responder();
            }

        }
	}
}
