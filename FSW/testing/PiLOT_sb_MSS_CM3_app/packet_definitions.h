///**
// * @file    :   packet_definitions.h
// * @author  :   Srinidhi G, Pratik Aher
// * @brief   :   This file contains the definitions of all the packet types used
// * @version :   1.0
// * @date    :   2023-01-24
// *
// * @copyright Copyright (c) 2023
// *
// */
//
#ifndef _PACKET_DEFINITIONS_
#define _PACKET_DEFINITIONS_

#define THERMISTOR_API_ID			20
#define THERMISTOR_PKT_LENGTH		sizeof(thermistor_pkt_t)
#define THERMISTOR_FLETCHER_CODE	0xCDCD

#define ARIS_API_ID           50
#define ARIS_PKT_LENGTH       sizeof(aris_pkt_t)
#define ARIS_FLETCHER_CODE    0x01

#define LOGS_API_ID			30
#define LOGS_PKT_LENGTH		sizeof(log_packet_t)
#define LOGS_FLETCHER_CODE	0x00

#define HK_API_ID			10
#define HK_PKT_LENGTH		sizeof(hk_pkt_t)
#define HK_FLETCHER_CODE	0xCDCD

#define SD_HK_API_ID    	40
#define SD_HK_PKT_LENGTH  sizeof(sd_test)
#define SD_HK_FLETCHER_CODE 0xCDCD

#define TIME_API_ID			60
#define TIME_PKT_LENGTH		sizeof(timer_pkt)

typedef struct {
    //CCSDS

	uint16_t ccsds_p1;
	uint16_t ccsds_p2;
	uint16_t ccsds_p3;

    uint32_t ccsds_s1;
    uint32_t ccsds_s2;

    uint32_t data_valid;
    uint16_t thermistor_set_A[8];
    uint16_t thermistor_set_B[8];
    uint16_t thermistor_set_C[8];
    uint16_t sensor_board_voltage;

    uint16_t Fletcher_Code;
}__attribute__((packed)) thermistor_pkt_t;

typedef struct {
	uint32_t collect_time;
	uint8_t data_valid;
	uint16_t aris_data[3];
}__attribute__((packed)) aris_sample_t;

typedef struct{
    uint16_t ccsds_p1;
    uint16_t ccsds_p2;
    uint16_t ccsds_p3;

    uint32_t ccsds_s1;
    uint32_t ccsds_s2;

    uint32_t start_time;

    aris_sample_t aris_samples[20];

    uint16_t Fletcher_Code;

}__attribute__((packed)) aris_pkt_t;

typedef struct {
    //CCSDS

    uint16_t ccsds_p1;
    uint16_t ccsds_p2;
    uint16_t ccsds_p3;

    uint32_t ccsds_s1;
    uint32_t ccsds_s2;

    uint16_t q_head;
    uint16_t q_tail;
    uint8_t CDH_Periph_Status; //For all the 8 flags
    uint16_t Acc[3];  // X,Y,Z Axis
    uint16_t Angular_Rate[3]; //Pitch, Roll, Yaw
    uint16_t Sensor_Board_VC[2];
    uint16_t CDH_VC[2];
    uint16_t Comms_VC[2];

    uint32_t HK_Read_Pointer;
    uint32_t HK_Write_Pointer;
    uint32_t Thermistor_Read_Pointer;
    uint32_t Thermistor_Write_Pointer;
    uint32_t Logs_Read_Pointer;
    uint32_t Logs_Write_Pointer;
    uint32_t SD_Test_Read_Pointer;
    uint32_t SD_Test_Write_Pointer;
    uint32_t ARIS_Read_Pointer;
    uint32_t ARIS_Write_Pointer;

    uint16_t aris_miss;
    uint16_t hk_miss;
    uint16_t payload_miss;
    uint8_t sd_dump;
//
    uint16_t Fletcher_Code;
}__attribute__((packed)) hk_pkt_t;

/**
 * @brief Each log entry has the below four fields for every task that occurs
 *
 */
typedef struct {
	uint32_t time_L;//lower time value
	uint32_t time_H;//upper time value
	uint8_t task_id;//task id
	uint8_t task_status;//task status;
}__attribute__((packed)) log_entry_t;

typedef struct {
    //CCSDS

    uint16_t ccsds_p1;
    uint16_t ccsds_p2;
    uint16_t ccsds_p3;

    uint32_t ccsds_s1;
    uint32_t ccsds_s2;

    log_entry_t logs[10];

    uint16_t Fletcher_Code;
}__attribute__((packed)) log_packet_t;


typedef struct {
    //CCSDS

    uint16_t ccsds_p1;
    uint16_t ccsds_p2;
    uint16_t ccsds_p3;

    uint32_t ccsds_s1;
    uint32_t ccsds_s2;

//    uint16_t SD_Test_count[5];
//    uint16_t SD_Test_time[5];
    uint8_t sd_result;
    uint16_t time;

    uint16_t Fletcher_Code;
}__attribute__((packed)) sd_test_t;

typedef struct {
//	uint32_t sync;
    uint16_t ccsds_p1;
    uint16_t ccsds_p2;
    uint16_t ccsds_p3;

    uint32_t ccsds_s1;
    uint32_t ccsds_s2;
    uint8_t reset;
	uint32_t lower_count;
	uint32_t upper_count;
	uint16_t tail;
}__attribute__((packed)) timer_pkt;
//
//#endif

//#ifndef _PACKET_DEFINITIONS_
//#define _PACKET_DEFINITIONS_

#define I2C_PKT_ID 1
#define I2C_PKT_LEN(pkt)	(17 + (((pkt)).argu->no_bytes*2))

typedef struct {
	  uint8_t id;
	  char name[100];
	  void (*work)(char *arg,uint8_t size);
	  char feedback[100];
} __attribute__((packed)) command_t;

typedef struct {
	uint8_t i2c_chx;
	uint8_t clock_speed;
	uint8_t msg_type;
	uint8_t output_type;
	uint16_t no_bytes;
	uint8_t *custom_msg;

} __attribute__((packed)) i2c_argu_t;

typedef struct {
	uint8_t Tx_Status;
	uint8_t Rx_Status;
	uint8_t no_Incorrect_bytes;
	uint16_t Tx_time ;
	uint16_t Rx_time;
	uint16_t Total_time;
	uint8_t *Rx_msg;

} __attribute__((packed)) i2c_results_t;

typedef struct{

	uint8_t ID;
	uint8_t length;
	i2c_argu_t *argu;
	i2c_results_t *results;

} __attribute__((packed)) i2c_packet_t;

typedef struct {
    uint16_t ccsds_p1;
    uint16_t ccsds_p2;
    uint16_t ccsds_p3;

    uint32_t ccsds_s1;
    uint32_t ccsds_s2;
};

#endif


