#include <system.h>

static void getMemory(uint64_t * pos, uint64_t * mem_buffer, unsigned int dim) {
    for (int i = 0; i < dim; i++)
        mem_buffer[i] = pos[i];
}

static void wrapMalloc(uint64_t ** ptr, uint64_t size) {
	*ptr = (uint64_t *) malloc(size);
}

static void getMMStatsWrapper(mm_stat * ptr) {
	*ptr = getMMStats();
}

int sys_system(void * option, void * arg1, void * arg2, void * arg3) {
    switch ((uint64_t) option) {
	case 0:
		getMemory((uint64_t *) arg1, (uint64_t *) arg2, (uint64_t) arg3);
		break;
	case 1:
		wrapMalloc((uint64_t **) arg1, (uint64_t) arg2);
		break;
	case 2:
		free((uint64_t *) arg1);
		break;
	case 3:
		getMMStatsWrapper((mm_stat *) arg1);
		break;
	}
	return 0;
}