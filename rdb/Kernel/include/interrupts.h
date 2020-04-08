#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <idtLoader.h>

void _irq00Handler(void);
void timer_handler(void);
void int_20(void);

void _irq01Handler(void);
void keyboard_handler(void);
void int_21(void);

void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);

void _exception0Handler(void);
void _exception6Handler(void);

int _int80Handler(uint64_t sysCallCode, uint64_t arg1, uint64_t arg2);

void _cli(void);
void _sti(void);
void _hlt(void);

void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);

#endif /* INTERRUPTS_H_ */
