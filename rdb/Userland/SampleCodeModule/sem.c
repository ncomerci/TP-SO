#include <lib_user.h>
#include <sem.h>

static sem_t semaphores[MAX_SEMAPHORES];
static unsigned int size = 0; 
static unsigned int waiting_ps = 0; 

sem_id sem_open(const char * name) {
    unsigned int i = 0;
    while (i < size)
        if (strcmp(semaphores[i].name, name) == 0)
            return i;
    if (size < MAX_SEMAPHORES) {
        semaphores[size].name = name;
        semaphores[size].lock = 0;
    }
    else
        return -1;
    return size++;
}

sem_id sem_init_open(const char * name, unsigned int init_val) {
    sem_id idx = sem_open(name);
    if (idx < 0)
        return idx;
    semaphores[idx].value = init_val;
    return idx;
}

int sem_wait(sem_id sem){
    
    //changestate a blocked 
    unsigned int sem_value = semaphores[sem].value;;
    if( sem_value > 0)
        sem_value--; 
    
    waiting_ps++;
    
}

int sem_post(sem_id sem) {
    //changestate a ready? 
    if(sem < MAX_SEMAPHORES){
        semaphores[sem].value++; 
        waiting_ps--; 
    }
}

int sem_close(sem_id sem) {
    
}

int sem_getvalue(sem_id sem, int * sval) { // sval is either 0 is returned; or a negative number whose absolute value is the count of the number of processes and threads currently blocked in sem_wait(3)
    if(0 < sem && sem < MAX_SEMAPHORES){
        *sval = waiting_ps; 
        return (int)semaphores[sem].value; 
    }
    return -1;
}

int sem_unlink(const char * name){
    
}
