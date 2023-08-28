

#include "cli.h"
uint8_t prompt_msg[30] =  "\n\r PiLOT Commanding : \0" ;
char cmd_not_exists[50] = CMD_NOT_EXISTS;
command_t cmd_list[30];
uint8_t cmd_in;
void print_num(char *data,double num) {
	echo_str(data);
	ftos(num,num_in_str,3);
	echo_str(num_in_str);
	echo_str("\n\r\0");
	num_in_str[0] = '\0';
	num_in_str[1] = '\0';
}
uint8_t scmp(char *s1,char *s2,uint8_t size) {
	uint8_t k = 0;
	if(size == 0) {
		return 1;
	}
	for(;k<size;k++) {
		if(s1[k] != s2[k]) {
			return 1;
		}
	}
	return 0;
}
void scpy(char *src,char *dest) {
	uint8_t ci = 0;
	while(1) {
		dest[ci] = src[ci];
		if(!src[ci]) {
			break;
		}
		ci++;
	}
}

uint32_t s_to_i(char* str){
	uint32_t val = 0;
	uint8_t k,i = 0;
	while(1){
	    	if(str[i] == '\0' || str[i] == '\r' || str[i] == ' '){
	    		break;
	    	}

	        val = val*10;
	        k = (int)(str[i] - 48);
	        val = val+k;
	        i++;

	    }
	return val;
}

void argu_to_i2cargu(char *argu,i2c_argu_t * i2c){
	i2c->i2c_chx = (int)(argu[0] - 48);
	i2c->clock_speed = (int)(argu[2] - 48);
	char temp[7];
	uint8_t i =8;
	while(argu[i] != ' '){
		temp[i-8] = argu[i];
		i++;
	}
	temp[i] = '\0';

	i2c->no_bytes = s_to_i(temp);
	i2c->msg_type = (int)(argu[4] - 48);
	i2c->output_type = (int)(argu[6]-48);
	//i2c->custom_msg = &argu[i+1];
}

void add_command(char name[],void (*work)(char *arg,uint8_t size),char fb[]) {
	cmd_list[cmd_in].id = cmd_in;
	scpy(name,cmd_list[cmd_in].name);
	cmd_list[cmd_in].work = work;
	scpy(fb,cmd_list[cmd_in].feedback);
	cmd_in++;
}
void uart0_rx_handler(mss_uart_instance_t* this_uart) {
    uart0_irq_size = MSS_UART_get_rx(this_uart,uart0_irq_rx_buffer,1);
    if(uart0_irq_rx_buffer[0] == 127){
        c[command_index] = '\0';
        c[command_index - 1] = 127;
        MSS_UART_polled_tx(&g_mss_uart0, &c[command_index - 1], 1);
        command_index = command_index - 1;
    }
    else{
        c[command_index] =  uart0_irq_rx_buffer[0];
        MSS_UART_polled_tx(&g_mss_uart0, &c[command_index], 1);
        command_index = command_index + 1;
    }

}
void cli_init(){
    MSS_UART_init(&g_mss_uart0, 9600, MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT );
    MSS_UART_init(&g_mss_uart1, 9600, MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT );
    MSS_UART_set_rx_handler(&g_mss_uart0,uart0_rx_handler,MSS_UART_FIFO_SINGLE_BYTE);
	c[command_index - 1] = '\0';
	command_flag = 0;
	command_index = 0;
	argu_index = 0;
	argument_flag = 0;
	msg_index = 0;
	val = 0;
	cmd_in = 0;
	set_adf_spi_instance(&g_core_spi0);
	add_command("setbaudrate\0",set_baud_rate,"\n\rbaud rate set\0");
	// add_command("view_thermistor\0",get_ADC_correct_values,"\n\rThermistor values displayed\0");
	add_command("disp_acc\0",get_imu_acc,"\n\rAcc displayed\0");
	add_command("disp_gyro\0",get_imu_gyro,"\n\rGyro displayed\0");
	add_command("I2C_TEST\0",i2c_test_cmd,"\n\rI2C Test Done");
	add_command("echo\0",echo,"\n\r\0");
	// add_command("sd_test\0",sd_test,"SD test done");
	// add_command("i2c_sigcheck\0",i2c_signal_check,"I2C Signal transmitted");
	add_command("reset\0",test_reset,"\n\rPerformed Reset\0");
	// add_command("rs485_test\0",rs485_tx_test,"RS485 tx complete");
	// add_command("vc_test_v\0",read_vc_sensor,"VC voltage test done");
	// add_command("vc_test_i\0",read_vc_sensor_i,"VC current test done");
	// add_command("adc_test\0", adc_test, "ADC_test done");
	// add_command("thermistor_calib\0", thermistor_calib, "Thermistor_calib done");
	add_command("adf_init\0",adf_init,"\n\rADF Initialised\0");
	add_command("adf_mem_write\0",adf_mem_write,"\n\rWritten to ADF memory\0");
	add_command("adf_mem_read\0",adf_mem_read,"\n\rRead from ADF memory\0");
	add_command("core_spi_test\0",core_spi_test,"\n\rTested Core spi\0");
	add_command("check_adf_spi_read\0",check_read_from_memory,"\n\rTested adf_read_from_memory\0");
	add_command("check_adf_spi_write\0", check_write_to_memory, "\n\rTested adf_write_to_memory\0");
	add_command("adf_reset\0",adf_reset,"\n\rADF Reset\0");
	add_command("adf_init_chk\0", adf_init_chk, "\n\rChecking Done\0");
	add_command("get_adf_state\0",get_adf_state,"\n\rObtained adf state\0");
	add_command("set_adf_state\0",set_adf_state,"\n\rADF state set\0");
	add_command("get_adf_freq\0",get_adf_freq,"\n\rObtained adf frequency\0");
	add_command("adf_Carrier_test\0",adf_transmit_carrier,"\n\rTransmitted carrier\0");
	MSS_UART_polled_tx_string(&g_mss_uart0,prompt_msg);

}
void cli_reset() {
	c[command_index - 1] = '\0';
	command_flag = 0;
	command_index = 0;
	argu_index = 0;
	argument_flag = 0;
	msg_index = 0;
	val = 0;
	c[0] = '\0';
	c[1] = '\0';

	MSS_UART_polled_tx_string(&g_mss_uart0,prompt_msg);
}
void feedback(uint8_t idc){
//    if(command_flag == 1){
//        uint8_t message[50] = BAUD_RATE_FEEDBACK;
//        MSS_UART_polled_tx_string(&g_mss_uart0, message);
//    }
	MSS_UART_polled_tx_string(&g_mss_uart0, cmd_list[idc].feedback);
    cli_reset();
}


void call_function(uint8_t ID, char* data, uint8_t size){

	if(ID < cmd_in) {
		cmd_list[ID].work(data,size);
	}

//	if(ID == 1){
//		(*SET_BAUD_RATE)(data,size);
//	}

//	else if(ID == 2){
//		(TX_INV_ENABLE)(data, size);
//	}
//
//	else if(ID == 3){
//		(RX_INV_ENABLE)(data, size);
//	}


	else{
		MSS_UART_polled_tx_string(&g_mss_uart0, cmd_not_exists);
	}

	feedback(ID);
}

void chk_msg(){



//    for(i = 0;i<msg_index;i++){
//                   if(c[i] != add_cmd_msg[i]){
//                	   command_flag = 0;
//                       break;
//                   }
//                   else{
//        				  if(i == msg_index -1){
//        					  MSS_UART_polled_tx_string(&g_mss_uart0, name_msg);
//        					  MSS_UART_set_rx_handler(&g_mss_uart0,uart0_rx_handler,MSS_UART_FIFO_SINGLE_BYTE);
//        				  }
//
//                       }
//                   }
uint8_t j,i = 0;

	for(j = 0;j<cmd_in;j++) {
		if(scmp(c,cmd_list[j].name,msg_index) == 0) {
			command_flag = j;
			i = 1;
			break;
		}
	}
	if(i == 0) {
		command_flag = cmd_in + 1;
	}
//    for(j=0;j<5;j++){
//		for(i = 0;i<msg_index;i++){
//
//			   if(c[i] != all_msg[j][i]){
//				   command_flag = 0;
//				   break;
//			   }
//			   else{
//					  if(i == msg_index -1){
//						  command_flag = j + 1;
//						  break;
//					  }
//
//				   }
//			   }
//		if(command_flag == j + 1){
//			break;
//		}
//    }


    call_function(command_flag,argu,command_index - msg_index);

 }

void serial_responder(){
    uint8_t i=0;

    while(i <= command_index){
        if(c[i] == ' '){
            msg_index = i;
            for(argu_index = 0;argu_index < command_index - msg_index;argu_index++){
                argu[argu_index] = c[i+1];
                i = i + 1;
            }
            argu[argu_index] = '\0';

        }
        i = i + 1;
    }
    chk_msg();

 }

void time_to_count(uint32_t ms,uint32_t *upper_count,uint32_t *lower_count) {
    *lower_count = (ms%FULL_SCALE_TIME_MS) * TIMER_COUNT_PER_MS;
    *upper_count = (ms/FULL_SCALE_TIME_MS);
}

void ftos(double s,char *value,uint8_t dot) {
	uint16_t k = (uint16_t)s;
	if(s == 0) {
		value[0] = '0';
		value[1] = '\0';
	} else {
		char stk[10];
		uint8_t i = 0,j = 0;
		while(k > 0) {
			stk[i] = k%10;
			k/=10;
			i++;
		}
		for(j = 0;j<i;j++) {
			if(i > 0) {
				value[j] = stk[i-1-j] + 48;
			}

		}
		if(dot != 0) {
			value[j] = '.';
			for(i = 0;i<dot;i++) {
				k = (uint16_t)(s * 10);
				value[j+i+1] = (k%10) + 48;
				s*=10;
			}
			value[j+i+1] = '\0';
		} else {
			value[j] = '\0';
		}
	}


}

