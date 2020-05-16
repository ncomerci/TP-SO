#ifndef _KSEM_H
    #define _KSEM_H

    #define MAX_SEMAPHORES 40
    #define MAX_PROCESSES_PER_SEMAPHORE 10
    #define SEM_NAME_MAX_LENGTH 40

    typedef int sem_id;

    // p1 sleep()
    //              p2 sleep()
    //                          p3 sleep()
    //                                      p4 post()
    //

    typedef struct sem_queue {
        uint64_t pid;
        struct sem_queue * next;
    } sem_queue;

    typedef struct sem_t {
        char name[SEM_NAME_MAX_LENGTH];
        char lock; 
        uint64_t value;
        sem_queue processes[MAX_PROCESSES_PER_SEMAPHORE];
        unsigned int processes_amount; //processes not closed 
        unsigned int processes_size; //all processes
        unsigned int processes_waiting; 
        sem_queue * first;
        sem_queue * last;
    } sem_t;

    typedef struct sem_info {
        sem_id id;
        char name[SEM_NAME_MAX_LENGTH];
        uint64_t value;
        uint64_t processes_waiting[MAX_PROCESSES_PER_SEMAPHORE];
        unsigned int processes_waiting_amount;
    }sem_info;

    void spin_lock(char * lock);
    void spin_unlock(char * lock);
    sem_id ksem_open(char * name);
    sem_id ksem_init_open(char * name, uint64_t init_val) ;
    int ksem_wait(sem_id sem);
    int ksem_post(sem_id sem);
    int ksem_close(sem_id sem);
    uint64_t ksem_getvalue(sem_id sem, int * sval);
    int ksem_destroy(sem_id sem);
    int ksem_get_semaphores_amount(unsigned int * size);
    int ksem_get_semaphores_info(sem_info * arr, unsigned int max_size, unsigned int * size);
    int sys_ksem(void * option, void * arg2, void * arg3, void * arg4);

#endif