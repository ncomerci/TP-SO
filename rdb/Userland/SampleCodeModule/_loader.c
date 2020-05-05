/* _loader.c */
#include <stdint.h>
#include <lib_user.h>
#include <sampleCodeModule.h>

extern char bss;
extern char endOfBinary;

static void halt_and_wait(void);

int _start() {
	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);
	
	main();
	return 0;
}

static void halt_and_wait(void) {
	_sys_system((void *) 4, 0, 0, 0);
}
