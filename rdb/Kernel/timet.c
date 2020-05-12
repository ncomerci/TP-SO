#include <timet.h>
#include <process.h>
#include <screen.h>

#define MAX_FUNCTIONS 20

static unsigned long ticks = 0;

static void functionsHandler();
static int findFunction(function func);
static void shiftFunctions(unsigned int idx);

static void pTimersHandler(void);
static int addTimer(uint64_t pid, unsigned int millis);

static periodic_func functions[MAX_FUNCTIONS];
static unsigned int functions_size;

static process_waiting timers[MAX_PROCESSES];
static unsigned int processes_waiting_size;

void * timer_handler(void * rsp) {
	ticks++;
	functionsHandler();  // Deprecated
	pTimersHandler();
	/*
	printString("RSP now will be: ", 18);
	void * new_rsp = scheduler(rsp);
	print64Hex((uint64_t) new_rsp);
	return new_rsp;
	*/
	return scheduler(rsp);
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
		if (functions[i].ticks_left <= 0) {
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

static void pTimersHandler(void) {
	for (int i = 0; i < processes_waiting_size; i++) {
		timers[i].ticks_left--;
		if (timers[i].ticks_left <= 0) {
			changeState(timers[i].pid, READY);
			for (int j = i; j < processes_waiting_size - 1; j++)
				timers[j] = timers[j+1];
			processes_waiting_size--;
		}	
	}
}

static int addTimer(uint64_t pid, unsigned int millis) {
	unsigned int i = 0;
	while (i < processes_waiting_size) { //si ya tiene un timer que lo actualice
		if (timers[i].pid == pid) {
			timers[i].ticks_left = ( millis * PIT_FREQUENCY ) / 1000;
			return 0;
		}
		i++;
	}
	if (i == MAX_PROCESSES)  // This should never happen
		return -1;
		
	timers[processes_waiting_size].pid = pid;
	timers[processes_waiting_size].ticks_left = ( millis * PIT_FREQUENCY ) / 1000;
	processes_waiting_size++;
	return 0;
}

void wait(unsigned int millis) {
	uint64_t pid;
	getPid(&pid);

	if (addTimer(pid, millis) == 0)
		changeState(pid, BLOCKED); // No corre mas.
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
		case 5:
			wait((unsigned int)(uint64_t) arg1);
			return 0;
	}
	return -1;
}

/*
>>> b process.c:49
Breakpoint 1 at 0x102c69: file process.c, line 49.
>>> b process.c:81
Breakpoint 2 at 0x102cf3: file process.c, line 81.
>>> b commands.c:244
Breakpoint 3 at 0x4036c7: file commands.c, line 244.
>>> b wait
Breakpoint 4 at 0x101619: wait. (2 locations)
>>> b loop
Breakpoint 5 at 0x403265: file commands.c, line 173.
>>> b timet.c:95
Breakpoint 6 at 0x101535: file timet.c, line 95.
>>> b timet.c:114
Breakpoint 7 at 0x101624: file timet.c, line 114.
*/