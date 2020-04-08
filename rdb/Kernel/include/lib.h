#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);
uint8_t getRTC(char arg);

unsigned char _inportb(unsigned short portid);
void _outportb(unsigned short portid, unsigned char value);
void _sti(void);
void _set_rsp(void * value);

#endif
