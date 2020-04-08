#ifndef _INT80_h
#define _INT80_h

#include<stdint.h>

uint64_t int80_handler(void * sysCallCode, void * arg1, void * arg2, void * arg3, void * arg4, void * arg5, void * arg6);

#endif