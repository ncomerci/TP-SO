#ifndef _SEM_H
#define _SEM_H

#define MAX_SEMAPHORES 15
#define MAX_PROCESSES_PER_SEMAPHORE 10
#define SEM_NAME_MAX_LENGTH 30

typedef int sem_id;

// p1 wait()
//              p2 wait()
//                          p3 wait()
//                                      p4 post()
//

typedef struct sem_queue {
   uint64_t pid;
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
sem_id sem_open(const char * name);
sem_id sem_init_open(const char * name, unsigned int init_val);
int sem_wait(sem_id sem);
int sem_post(sem_id sem);
int sem_close(sem_id sem);
int sem_getvalue(sem_id sem, int * sval);
int sem_destroy(sem_id sem);

#endif