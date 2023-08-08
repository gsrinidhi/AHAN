
#ifndef commands_H_
#define commands_H_
#include"cli.h"
#include "peripherals.h"
#include "pilot.h"
#include "packet_definitions.h"
#include "pilot_testing.h"
#include "pslv_interface.h"
#include "ccsds.h"
#include "SD.h"
#include "device_drivers/adf7030.h"

#define VAL 0
#define COMMAND_FLAG 0
#define ARGUMENT_FLAG 0
#define COMMAND_INDEX 0
#define ARGU_INDEX 0
#define MSG_INDEX 0
#define I2C_SLAVE_ADD 0x88

timer_instance_t timer;
//void set_baud_rate(char* arg, uint8_t s);
//void change_interface_mode(char *arg,uint8_t size);
//void get_ADC_correct_values(char *arg,uint8_t size);
//void get_imu_acc(char *data,uint8_t size);
//void get_imu_gyro(char *data,uint8_t size);
//void i2c_test(char *data,uint8_t size);


#endif
