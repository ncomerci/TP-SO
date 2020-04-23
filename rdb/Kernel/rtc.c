#include <rtc.h>
#include <lib.h>

// Returns -1 if arguments aren´t right.
int sys_rtc(void * option) {
    uint64_t opt = (uint64_t) option;
    if (opt < 0 || opt > 2)
        return -1;
    int aux = getRTC(2*opt);
    aux = aux/16 * 10 + aux % 16;
    if (opt == 2) { // Fix for GMT Local Hour
        if (aux < 3)
            aux += 21;
        else
            aux -= 3;
    }
    return aux;
}