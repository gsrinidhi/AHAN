///*
// * pilot_commands.c
// *
// *  Created on: 03-Mar-2023
// *      Author: S-SPACE
// */
//
//#include "pilot_commands.h"
//
//void get_ADC_correct_values(char *arg,uint8_t size) {
//	uint8_t chx = (uint8_t)arg[0];
//	uint8_t addr = (uint8_t)arg[1];
//	i2c_instance_t *dbg;
//	if(chx == '0') {
//		dbg = &i2c_3;
//	} else if(chx == '1') {
//		dbg = &i2c_5;
//	} else if(chx == '2') {
//		dbg = &g_core_i2c4;
//	}
//	if(addr == '1') {
//		addr = ADC_I2CU1_ADDR;
//	} else if(addr == '2') {
//		addr = ADC_I2CU2_ADDR;
//	}
//	uint8_t i = 0,flag;
//	uint16_t value = 0;
//	char float_value[20];
//	double fv = 0;
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	for(;i<8;i++) {
//		value = get_ADC_value(dbg,addr,i,&flag);
//		ftos(flag,float_value,2);
//		MSS_UART_polled_tx_string(&g_mss_uart0,float_value);
//		MSS_UART_polled_tx_string(&g_mss_uart0," \0");
//		fv = 3.3/1024 * value;
//		ftos(fv,float_value,2);
//		MSS_UART_polled_tx_string(&g_mss_uart0,float_value);
//		MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	}
//}
//
//void get_imu_acc(char *data,uint8_t size) {
//	short ax,ay,az;
//	char acc[10];
//	uint8_t res = get_IMU_acc(&ax,&ay,&az);
//	double xa = ax * 0.0005987,ya = ay * 0.0005987,za = az * 0.0005987;
//	ftos(xa,acc,2);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,"Ax: \0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,acc);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	ftos(ya,acc,2);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"Ay: \0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,acc);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	ftos(za,acc,2);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"Az: \0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,acc);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//}
//
//void get_imu_gyro(char *data,uint8_t size) {
//	short ax,ay,az;
//	char acc[10];
//	uint8_t res = get_IMU_gyro(&ax,&ay,&az);
//	double xa = ax * 0.00875,ya = ay * 0.00875,za = az * 0.00875;
//	ftos(xa,acc,2);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,"Ax: \0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,acc);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	ftos(ya,acc,2);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"Ay: \0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,acc);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//	ftos(za,acc,2);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"Az: \0");
//	MSS_UART_polled_tx_string(&g_mss_uart0,acc);
//	MSS_UART_polled_tx_string(&g_mss_uart0,"\n\r\0");
//}
