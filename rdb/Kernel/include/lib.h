#ifndef LIB_H
#define LIB_H

#define NULL ((void *) 0)
#define portBYTE_ALIGNMENT 8
#define portBYTE_ALIGNMENT_MASK 7 

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

unsigned int strlen(const char *str);
int strcpy(char *dst, const char *src);
int strcat(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);

char *cpuVendor(char *result);
uint8_t getRTC(char arg);

unsigned char _inportb(unsigned short portid);
void _outportb(unsigned short portid, unsigned char value);
void _sti(void);
void _cli(void);
void _int81(void);
void _set_rsp(void * value);
void _halt_and_wait(void);
void _halter(void);
int _start_process(int (*main)(int argc, char * argv), int argc, char * argv);

#endif
