#include <timet.h>
#include <keyboard.h>
#include <stdint.h>
#include <interrupts.h>

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case 1:
			int_21();
			return;
	}
	return;
}

void * int_20(void * rsp) {
	return timer_handler(rsp);
}

void int_21() {
	keyboard_handler();
}