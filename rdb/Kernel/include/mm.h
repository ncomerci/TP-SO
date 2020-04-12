#ifndef _MM_H
    #define _MM_H

    #include <lib.h>
    #define MIN_ADDRESS 0x600000
    #define MAX_ADDRESS 0x1000000

    void *malloc( uint64_t );
    void free( void * );

#endif