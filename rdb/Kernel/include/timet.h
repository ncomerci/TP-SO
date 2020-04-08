#ifndef _TIMET_H_
#define _TIMET_H_

#include <stdint.h>

#define PIT_FREQUENCY 400
#define PIT_CHANNEL0 0x40  // PIT Channel 0's Data Register Port
#define PIT_CHANNEL1 0x41  // PIT Channels 1's Data Register Port, we wont be using this here
#define PIT_CHANNEL2 0x42  // PIT Channels 2's Data Register Port
#define PIT_CMDREG 0x43    // PIT Chip's Command Register Port

typedef void (*function)(void);

typedef struct periodic_func {
	function f;
	unsigned int ticks;
	unsigned int ticks_left;
} periodic_func;

void init_time(void);

void timer_handler();
unsigned long ticks_elapsed();
unsigned long seconds_elapsed();

void removeFunction(function func);
void updateFunction(function func, unsigned int new_ticks);
int addFunction(function func, unsigned int ticks);

unsigned long sys_timet(void * option, void * arg1, void * arg2);

#endif
