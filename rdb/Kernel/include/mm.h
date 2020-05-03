#ifndef _MM_H
    #define _MM_H

    #include <lib.h>
    #define MIN_ADDRESS 0x900000
    #define MAX_ADDRESS 0x1900000

    typedef struct mm_stat {
        char * sys_name;  
        uint64_t total;
        uint64_t occupied;
        uint64_t free;
        uint64_t successful_allocs;
        uint64_t successful_frees;
    } mm_stat;

    void *malloc( uint64_t );
    void free( void * );
    mm_stat getMMStats( void );

#endif