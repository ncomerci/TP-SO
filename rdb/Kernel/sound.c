#include <sound.h>
#include <lib.h>
#include <timet.h>

 //Play sound using built in speaker
void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
    //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	_outportb(0x43, 0xb6);
 	_outportb(0x42, (uint8_t) (Div) );
 	_outportb(0x42, (uint8_t) (Div >> 8));
 
    //And play the sound using the PC speaker
 	tmp = _inportb(0x61);
  	if (tmp != (tmp | 3)) {
 		_outportb(0x61, tmp | 3);
 	}
}

void play_timed_sound(uint32_t nFrequence, long duration) {
	play_sound(nFrequence);
	addFunction(nosound, duration);
}
 
// Make it shutup
void nosound() {
	uint8_t tmp = _inportb(0x61) & 0xFC;
	_outportb(0x61, tmp);
	removeFunction(nosound);
}


int sys_sound(void * option, void * arg1, void * arg2) {
	switch ((uint64_t) option) {
	case 0:
		nosound();
		break;
	case 1:
		play_sound((uint64_t) arg1);
		break;
	case 2:
		play_timed_sound((uint64_t) arg1, (long) arg2);
		break;
	}
	return 0;
}