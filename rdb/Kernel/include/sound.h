#ifndef _SOUND_H
#define _SOUND_H

#include <stdint.h>

void play_timed_sound(uint32_t nFrequence, long duration);
void play_sound(uint32_t nFrequence);
void nosound(void);

int sys_sound(void * option, void * arg1, void * arg2);

#endif
