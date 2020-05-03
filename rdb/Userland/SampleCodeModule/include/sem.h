#ifndef _SEM_H
#define _SEM_H

#define MAX_SEMAPHORES 10

typedef int sem_id;
typedef struct sem_t {
    char * name;
    int lock; 
    unsigned int value;
} sem_t;
sem_id sem_open(const char * name);
sem_id sem_init_open(const char * name, unsigned int init_val);
int sem_wait(sem_id sem);
int sem_post(sem_id sem);
int sem_close(sem_id sem);
int sem_getvalue(sem_id sem, int * sval);
int sem_unlink(const char * name);

#endif