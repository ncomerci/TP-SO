#ifndef _SEM_H
#define _SEM_H

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
    uint32_t lock; 
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

void spin_lock(uint32_t * lock);
void spin_unlock(uint32_t * lock);
sem_id sem_open(char * name);
sem_id sem_init_open(char * name, uint64_t init_val);
int sem_wait(sem_id sem);
int sem_post(sem_id sem);
int sem_close(sem_id sem);
uint64_t sem_getvalue(sem_id sem, int * sval);
int sem_destroy(sem_id sem);
unsigned int sem_get_semaphores_info(sem_info * arr, unsigned int max_size);

#endif