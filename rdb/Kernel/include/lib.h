#ifndef LIB_H
#define LIB_H

#define NULL ((void *) 0)
#define portBYTE_ALIGNMENT 8
#define portBYTE_ALIGNMENT_MASK 7 

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

int strcpy(char *dst, const char *src);

char *cpuVendor(char *result);
uint8_t getRTC(char arg);

unsigned char _inportb(unsigned short portid);
void _outportb(unsigned short portid, unsigned char value);
void _sti(void);
void _cli(void);
void _set_rsp(void * value);
void _halt_and_wait(void);
int _start_process(int (*main)(int argc, char * argv), int argc, char * argv);

#endif
