#include <lib_user.h>
#include <sem.h>

static int getIndexOnSem(int pid, sem_t * sem);
static void enqueue(sem_t * sem, sem_queue * sq);
static int dequeuePid(sem_t * sem);

static sem_t semaphores[MAX_SEMAPHORES];
static unsigned int sem_size = 0; 
static unsigned int sem_amount = 0; 

sem_id sem_open(const char * name) {
    unsigned int i = 0;
    int pid = getPid();
    while (i < sem_size && semaphores[i].name != NULL) {
        if (strcmp(semaphores[i].name, name) == 0) {
            unsigned int j = 0;
            while (j < semaphores[i].processes_size && semaphores[i].processes[j].pid != -1) {
                if(semaphores[i].processes[j].pid == pid) // Process already opened this semaphore
                    return -1;
                j++;
            }
            if (j >= MAX_PROCESSES_PER_SEMAPHORE) {
                return -1;
            }
            semaphores[i].processes[j].pid = pid; // Add pid to the processes appended to this semaphore
            if (semaphores[i].processes_amount == semaphores[i].processes_size)
                semaphores[i].processes_size++; 
            semaphores[i].processes_amount++;   
            return i;
        }
        i++;
    }
    if (i < MAX_SEMAPHORES) {
        semaphores[i].name = name;
        semaphores[i].lock = 0;
        if (sem_amount == sem_size)
            sem_size++;
        sem_amount++;
    }
    else
        return -1;
}

sem_id sem_init_open(const char * name, unsigned int init_val) {
    sem_id idx = sem_open(name);
    if (idx < 0)
        return idx;
    semaphores[idx].value = init_val;
    return idx;
}

int sem_wait(sem_id sem){
    if (sem < 0 || sem >= sem_size || semaphores[sem].name == NULL)
        return -1; // Semaphore does not exist    

    int pid = getPid();
    int idx = getIndexOnSem(pid, &(semaphores[sem]));
    if (idx < 0)
        return -1;

    while (semaphores[sem].lock);
    if (semaphores[sem].value == 0) {
        enqueue(&(semaphores[sem]), &(semaphores[sem].processes[idx]));
        changeState(pid, BLOCKED);   
    }
    semaphores[sem].value--;
    semaphores[sem].lock = 1;
}

static void enqueue(sem_t * sem, sem_queue * sq) {
    if (sem->last == NULL) {
        sem->first = sq;
        sem->last = sq;
    }
    else {
        (sem->last)->next = sq;
        sem->last = sq;
        sq->next = NULL;
    }
    sem->processes_waiting++;
}

static int dequeuePid(sem_t * sem) {
    if (sem->first == NULL)
        return -1;
    int pid = (sem->first)->pid;
    if (sem->last == sem->first)
        sem->last = NULL;
    sem->first = (sem->first)->next;
    sem->processes_waiting--;
    return pid;
}   

static int getIndexOnSem(int pid, sem_t * sem) {
    for (unsigned int j = 0; j < sem->processes_size && (sem->processes)[j].pid != -1; j++) {
        if((sem->processes)[j].pid == pid)
            return j; 
    }
    return -1;
}

int sem_post(sem_id sem) {
    if (sem < 0 || sem >= sem_size || semaphores[sem].name == NULL)
        return -1; // Semaphore does not exist

    while (semaphores[sem].lock);
    semaphores[sem].value++;
    semaphores[sem].lock = 1;

    if (semaphores[sem].processes_waiting > 0) {
        int pid = dequeuePid(&(semaphores[sem]));
        if (pid < 0)
            return -1; // Failed at dequeuing
        changeState(pid, READY);
    }
    return 0;
}

int sem_close(sem_id sem) { //remove a ps from semaphore
    if (sem < 0 || sem >= sem_size || semaphores[sem].name == NULL)
        return -1; // Semaphore does not exist
    int pid = getPid();
    unsigned int i = 0;
    while (i < semaphores[sem].processes_size) {
        if (semaphores[sem].processes[i].pid == pid) {
            semaphores[sem].processes[i].pid = -1;
            if (i == semaphores[sem].processes_size - 1)
                semaphores[sem].processes_size--;
            semaphores[sem].processes_amount--;
            return 0;
        }
        i++;
    }
    return -1; // process was not found on semaphore
}

int sem_getvalue(sem_id sem, int * sval) { // sval is either 0 is returned; or a negative number whose absolute value is the count of the number of processes and threads currently blocked in sem_wait(3)
    if (sem < 0 || sem >= sem_size || semaphores[sem].name == NULL)
        return -1; // Semaphore does not exist
    *sval = - semaphores[sem].processes_waiting; 
    return (int) semaphores[sem].value; 
}

int sem_unlink(const char * name){ //remove semaphore
    unsigned int i = 0;
    while (i < sem_size) {
        if (semaphores[i].name != NULL && strcmp(semaphores[i].name, name) == 0) {
            semaphores[i].name = NULL;
            if (i == sem_size - 1)
                sem_size--;
            sem_amount--;
            return 0;
        }
        i++;
    }
    return -1; // semaphore was not found
}
