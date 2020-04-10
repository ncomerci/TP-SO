#include <errno.h>

#define MEMORY_CAPACITY 20000

void *mov_sbrk(int increment)
{
    static char global_mem[MEMORY_CAPACITY] = {0};
    static char *p_break = global_mem;

    char *const limit = global_mem + MEMORY_CAPACITY;
    char *const original = p_break;

    if (increment < global_mem - p_break  ||  increment >= limit - p_break)
    {
        errno = ENOMEM;
        return (void*)-1;
    }
    p_break += increment;

    return original;
}