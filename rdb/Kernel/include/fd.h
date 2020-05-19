#ifndef _FD_H
#define _FD_H

#include <process.h>
#include <keyboard.h>
#include <screen.h>
#include <pipe.h>

#define MAX_FILE_DES 2
#define STDIN -1
#define STDOUT -2

int assignInAndOut(unsigned int idx, char * in, char * out);
int closeInAndOut(unsigned int idx);
int sys_fd(void * option, void * arg1, void * arg2, void * arg3, void * arg4);
int writeFD(int fd, char * str, unsigned int str_size, int color);
int readFD(int fd, char * buff, unsigned int count);

#endif