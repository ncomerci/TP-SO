#ifndef _FFMM_H
    #define _FFMM_H

    #include <mm.h>

    static void const * maxAddress = (void *) MAX_ADDRESS;

    static void const * minAddress = (void *) MIN_ADDRESS;

    static char const * sys_name = "FirstFit";
    
    /* Define the linked list structure.  This is used to link free blocks in order
    of their memory address. */
    typedef struct A_BLOCK_LINK
    {
        struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
        uint64_t xBlockSize;						/*<< The size of the free block. */
    } BlockLink_t; //Free List

    /* Block sizes must not get too small. */
    #define heapMINIMUM_BLOCK_SIZE	( ( uint64_t ) ( xHeapStructSize << 1 ) )

    /* Assumes 8bit bytes! */
    #define heapBITS_PER_BYTE		( ( uint64_t ) 8 )

    /* 16 MB for the Heap in this case */
    #define configTOTAL_HEAP_SIZE ( (uint64_t) maxAddress - ( uint64_t ) minAddress )

#endif