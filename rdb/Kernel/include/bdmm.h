#ifndef BDMM_H

    #define BDMM_H
    
    #include <mm.h>

    /*
    * Every allocation needs an 8-byte header to store the allocation size while
    * staying 8-byte aligned. The address returned by "malloc" is the address
    * right after this header (i.e. the size occupies the 8 bytes before the
    * returned address).
    */
    #define HEADER_SIZE 8

    /*
    * The minimum allocation size is 16 bytes because we have an 8-byte header and
    * we need to stay 8-byte aligned.
    */
    #define MIN_ALLOC_LOG2 4
    #define MIN_ALLOC ((uint64_t)1 << MIN_ALLOC_LOG2)

    /*
    * This is the total size of the heap. It's technically also the maximum 
    * allocation size because the heap could consist of a single allocation of 
    * this size. But of course real heaps will have multiple allocations.
    * 
    *    __builtin_clzll : #ceros antes del 1
    *   000010011101011 -> 64                            
    *   000010000000000 -> 4
    *   -----------------> 60 - 1 = 59
    */
    #define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))
    #define MAX_ALLOC_LOG2 (LOG2((unsigned long long)(MAX_ADDRESS - MIN_ADDRESS)))
    #define MAX_ALLOC ((uint64_t)1 << MAX_ALLOC_LOG2) //16 MB

    /*
    * Allocations are done in powers of two starting from MIN_ALLOC and ending at
    * MAX_ALLOC inclusive. Each allocation size has a bucket that stores the free
    * list for that allocation size.
    *
    * Given a bucket index, the size of the allocations in that bucket can be
    * found with "(uint64_t)1 << (MAX_ALLOC_LOG2 - bucket)".
    */
    #define BUCKET_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1)

    /*
    * Free lists are stored as circular doubly-linked lists. Every possible
    * allocation size has an associated free list that is threaded through all
    * currently free blocks of that size. That means MIN_ALLOC must be at least
    * "sizeof(list_t)". MIN_ALLOC is currently 16 bytes, so this will be true for
    * both 32-bit and 64-bit.
    */
    typedef struct list_t {
        struct list_t *prev, *next;
    } list_t;

    static void const * maxAddress = (void *) MAX_ADDRESS;

    static void const * minAddress = (void *) MIN_ADDRESS;

    static char const * sys_name = "Buddy";

#endif