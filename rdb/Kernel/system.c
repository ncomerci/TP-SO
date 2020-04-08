#include <system.h>
#include <stdint.h>

static void getMemory(uint64_t * pos, uint64_t * mem_buffer, unsigned int dim) {
    for (int i = 0; i < dim; i++)
        mem_buffer[i] = pos[i];
    
}

int sys_system(void * option, void * arg1, void * arg2, void * arg3) {
    switch ((uint64_t) option) {
	case 0:
		getMemory((uint64_t *) arg1, (uint64_t *) arg2, (uint64_t) arg3);
		break;
	}
	return 0;
}