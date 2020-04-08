#include <timet.h>

#define MAX_FUNCTIONS 20

static unsigned long ticks = 0;

static void functionsHandler();
static int findFunction(function func);
static void shiftFunctions(unsigned int idx);

static periodic_func functions[MAX_FUNCTIONS];
static int functions_size;

void timer_handler() {
	ticks++;
	functionsHandler();
}

unsigned long ticks_elapsed() {
	return ticks;
}

unsigned long seconds_elapsed() {
	return ticks / PIT_FREQUENCY;
}

static void functionsHandler() {
	for (int i = 0; i < functions_size; i++) {
		functions[i].ticks_left--;
		if (functions[i].ticks_left == 0) {
			functions[i].ticks_left = functions[i].ticks;
			(functions[i].f)();
		}	
	}
}

static int findFunction(function func) {
	int i = 0;
	for(i = 0; i < functions_size; i++) {
		if (functions[i].f == func)
			return i;
	}
	return -1;
}

static void shiftFunctions(unsigned int idx) {
	for (int i = idx; i < functions_size - 1; i++)
		functions[i] = functions[i+1];
}

void removeFunction(function func) {
	int idx = findFunction(func);
	if (idx >= 0) {
		shiftFunctions(idx);
		functions_size--;
	}
}

void updateFunction(function func, unsigned int new_ticks) {
	int idx = findFunction(func);
	if (idx >= 0)
		functions[idx].ticks = new_ticks;
}

int addFunction(function func, unsigned int ticks) {
	if (functions_size == MAX_FUNCTIONS)  // This should never happen
		return -1;
		
	functions[functions_size].f = func;
	functions[functions_size].ticks = functions[functions_size].ticks_left = ticks;
	functions_size++;
	return 0;
}

// Returns -1 if arguments aren´t right.
unsigned long sys_timet(void * option, void * arg1, void * arg2) {
	switch ((uint64_t) option) {
		case 0:
			return ticks_elapsed();
		case 1:
			return seconds_elapsed();
		case 2:
			return addFunction((function) arg1, (uint64_t) arg2);
		case 3:
			removeFunction((function) arg1);
			return 0;
		case 4:
			updateFunction((function) arg1, (uint64_t) arg2);
			return 0;
	}
	return -1;
}