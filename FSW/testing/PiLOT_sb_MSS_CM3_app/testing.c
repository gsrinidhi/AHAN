///*
// * testing.c
// *
// *  Created on: 27-Jan-2023
// *      Author: S-SPACE
// */
//
//#include "testing.h"
//
//void testing_init() {
//	result = Pilot_Init();
//	before_time = 0xFFFFFFFF;
//}
//void get_hk_time() {
//	hk_pkt = (hk_pkt_t*)data;
//	dummy = 0;
//	TMR_init(&timer,CORETIMER_0_0,TMR_ONE_SHOT_MODE,PRESCALER_DIV_2,0xFFFFFFFF);
//	TMR_start(&timer);
//	result = get_hk(hk_pkt,100,&dummy);
//	TMR_stop(&timer);
//	after_time = TMR_current_value(&timer);
//	hk_time = (before_time - after_time) / 100;
//	dummy = 1;
//}
//
//void get_thermistor_time() {
//	thermistor_pkt = (thermistor_pkt_t*)data;
//	dummy = 0;
//	TMR_init(&timer,CORETIMER_0_0,TMR_ONE_SHOT_MODE,PRESCALER_DIV_2,0xFFFFFFFF);
//	TMR_start(&timer);
//	result = get_thermistor_vals(thermistor_pkt,100);
//	TMR_stop(&timer);
//	after_time = TMR_current_value(&timer);
//	payload_time = (before_time - after_time) / 100;
//	dummy = 1;
//}
//
//void get_aris_time() {
//	aris_pkt = (aris_pkt_t*)data;
//	dummy = 0;
//	TMR_init(&timer,CORETIMER_0_0,TMR_ONE_SHOT_MODE,PRESCALER_DIV_2,0xFFFFFFFF);
//	TMR_start(&timer);
//	result = get_aris_sample(aris_pkt,100,dummy);
//	TMR_stop(&timer);
//	after_time = TMR_current_value(&timer);
//	aris_time = (before_time - after_time) / 100;
//	dummy = 1;
//}
//
//void timer_start() {
////	before_time = 0xFFFFFFFF;
////	TMR_init(&timer,CORETIMER_0_0,TMR_ONE_SHOT_MODE,PRESCALER_DIV_2,0xFFFFFFFF);
////	TMR_start(&timer);
//}
//
//uint32_t get_time_in_us() {
////	TMR_stop(&timer);
////	after_time = TMR_current_value(&timer);
////	return (before_time - after_time) / 50;
//	return 0;
//}
