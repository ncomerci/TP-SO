#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include <video_vm.h>
#include <screen.h>
#include <process.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x800000;

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

int main() {

	init_VM_Driver();
	init_screen();

	_cli(); //deshabilito las ints
	load_idt();

	main_func_t aux = {(int (*)(int, char **)) sampleCodeModuleAddress, 0, NULL};
	ps_info_t ps_aux = {&aux, "Shell", 1};
	fd_info_t fd_aux = {NULL, NULL};
	uint64_t pid;
	createProcess(&ps_aux, &fd_aux, &pid);

	_sti(); //las seteo de nuevo
	_int81();

	return 0;
}
