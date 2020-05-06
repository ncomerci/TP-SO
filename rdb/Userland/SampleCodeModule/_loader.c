/* _loader.c */
#include <stdint.h>
#include <lib_user.h>
#include <sampleCodeModule.h>

extern char bss;
extern char endOfBinary;

int _start() {
	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);
	
	main();
	return 0;
}
