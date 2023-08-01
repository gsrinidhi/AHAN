
#include"test.h"
#include "master.h"
#include "slave.h"
void set_baud_rate(char* arg, uint8_t s);
void (*SET_BAUD_RATE)(char* arg, uint8_t s) = &set_baud_rate;
uint8_t add_command(char[],uint8_t);

#define CONFIG	1

int main(){
#if CONFIG == 1
	master();
#else
	slave_init();
#endif
	while(1) {

	}
    return 0;
}
