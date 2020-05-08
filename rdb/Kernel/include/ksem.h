#ifndef _KSEM_H
#define _KSEM_H

#define MAX_SEMAPHORES 15
#define MAX_PROCESSES_PER_SEMAPHORE 10
#define SEM_NAME_MAX_LENGTH 40

typedef int sem_id;

// p1 wait()
//              p2 wait()
//                          p3 wait()
//                                      p4 post()
//

typedef struct sem_queue {
   int pid;
   struct sem_queue * next;
} sem_queue;

typedef struct sem_t {
    char name[SEM_NAME_MAX_LENGTH];
    char lock; 
    unsigned int value;
    sem_queue processes[MAX_PROCESSES_PER_SEMAPHORE];
    unsigned int processes_amount; //processes not closed 
    unsigned int processes_size; //all processes
    unsigned int processes_waiting; 
    sem_queue * first;
    sem_queue * last;
} sem_t;

void spin_lock(char * lock);
void spin_unlock(char * lock);
sem_id ksem_open(const char * name);
sem_id ksem_init_open(const char * name, unsigned int init_val);
int ksem_wait(sem_id sem);
int ksem_post(sem_id sem);
int ksem_close(sem_id sem);
int ksem_getvalue(sem_id sem, int * sval);

#endif