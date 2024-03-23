


#ifndef _CLI_DEFINITIONS_
#define _CLI_DEFINITIONS_
#include<drivers/mss_uart/mss_uart.h>
#include "peripherals.h"
#include "packet_definitions.h"
#include "../../AHAN_FSW/AHAN_sb_MSS_CM3_app/device_drivers/adf7030.h"
#define BAUD_RATE_ID 1;
#define TX_INV_ENA_ID 2;
#define RX_INV_ENA_ID 3;
#define BAUD_RATE_MSG "setbaudrate"
#define TX_INV_MSG "tx_inv_enable"
#define RX_INV_MSG "rx_inv_enable"
#define CMD_NOT_EXISTS "\n\rCommand Does not exists\0"

#define BAUD_RATE_FEEDBACK "\n \r BAUDRATE SET SUCCESSFULL!!\0"
#define TX_INV_FEEDBACK "\r TX_INV SUCCESSFULL!!"
#define RX_INV_FEEDBACK "\r RX_INV SUCCESSFULL!!"
#define COSMOS_UART g_mss_uart1
#define COSMOS_UART_SEND MSS_UART_polled_tx
#define DEBUG_UART	g_mss_uart0
#define DEBUG_UART_SEND MSS_UART_polled_tx
#define DEBUG_UART_SEND_STR MSS_UART_polled_tx_string
void set_baud_rate(char* arg, uint8_t s);
void change_interface_mode(char *arg,uint8_t size);
void get_ADC_correct_values(char *arg,uint8_t size);
void get_imu_acc(char *data,uint8_t size);
void get_imu_gyro(char *data,uint8_t size);
void i2c_test_cmd(char *data,uint8_t size);
void echo(char *data,uint8_t size);
void echo_str(char *data);
void ftos(double s,char *value,uint8_t dot);
void i2c_test(i2c_argu_t *argu,i2c_results_t *res);
void sd_test(char *data,uint8_t size);
char num_in_str[10];
void print_num(char *data,double num);
void i2c_signal_check(char *data, uint8_t size);
void test_reset(char *data,uint8_t size);
void rs485_tx_test(char *data,uint8_t size);
void read_vc_sensor(char *data,uint8_t size);
void read_vc_sensor_i(char *data,uint8_t size);
void adc_test(char *data,uint8_t size);
void argu_to_i2cargu(char *argu,i2c_argu_t * i2c);
void thermistor_calib(char *data,uint8_t size);
void adf_init(char *data, uint8_t size);
void core_spi_test(char *data, uint8_t size);

/**
 * Command to write to adf memory
 * example:
 * adf_mem_write 20004004 00001B1B
 * the first parameter is the address, the second parameter is the data to be written
 * Note: The addressess should be word aligned(Multiple of 4). Also make sure that eight digits are given
 * for both data and address. Fill extra digits with zeros
 */
void adf_mem_write(char *data, uint8_t size);

/**
 * Command to read from adf memory
 * example: adf_mem_read 20004004
 * the first parameter is the address. It should be word aligned
 */
void adf_mem_read(char *data,uint8_t size);

/**
 * @brief Command to check adf_read_from_memory. Calls the function in an infinite loop, which can be broken by hitting any key
 * 
 * @param data 
 * @param size 
 */

void check_write_to_memory(char *data,uint8_t size);
void check_read_from_memory(char *data,uint8_t size);

void adf_reset(char *data,uint8_t size);
void adf_init_chk(char *data, uint8_t size);

void set_adf_state(char *data, uint8_t size);
void get_adf_state(char *data,uint8_t size);
void get_adf_freq(char *data, uint8_t size);
void adf_transmit_carrier(char *data,uint8_t size);
void adf_set_gpio(char* data, uint8_t size);
void read_cmd_buf(char* data, uint8_t size);
void get_temp(char* data, uint8_t size);
void get_rssi(char* data, uint8_t size);
void get_rx(char* data, uint8_t size);
void get_adf_status();
void clr_intr();
void pkt_tx();
 char c[50];
 char argu[50];

 uint32_t val;
 uint8_t command_index ;
 uint8_t command_flag , argument_flag ;
 uint8_t uart0_irq_rx_buffer[3];
 char all_msg[50][50];// = {BAUD_RATE_MSG, TX_INV_MSG, RX_INV_MSG};

  uint8_t  argu_index , msg_index , uart0_irq_size;



//  void set_baud_rate(char* arg, uint8_t s);
//  void get_ADC_correct_values(char *arg,uint8_t size);
//  void get_imu_acc(char *data,uint8_t size);
//  void get_imu_gyro(char *data,uint8_t size);
   void (*SET_BAUD_RATE)(char* arg, uint8_t s);// = &set_baud_rate;


   void scpy(char *src,char *dest);
   uint8_t scmp(char *s1,char *s2,uint8_t size);
void time_to_count(uint32_t ms,uint32_t *upper_count,uint32_t *lower_count);
void cli_init();
void feedback(uint8_t command_flag);
void call_function(uint8_t ID, char* data, uint8_t size);
void uart0_rx_handler(mss_uart_instance_t* this_uart);
void serial_responder();
void ftos(double s,char *value,uint8_t dot);
uint32_t s_to_i(char* str);

#endif
