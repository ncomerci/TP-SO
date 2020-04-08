#ifndef _KEYB_H
#define _HEYB_H

#include <stdint.h>

void keyboard_handler(void);
int special_key(uint8_t key);
int sys_read(void * buff);
uint8_t kbGet(void);
void normalKey(uint8_t aux);
void shiftedKey(uint8_t aux);

typedef void (*func)(uint8_t);

#endif