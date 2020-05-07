#include <lib.h>
#include <process.h>
#include <ksem.h>

static int getIndexOnSem(int pid, sem_t * sem);
static void enqueue(sem_t * sem, sem_queue * sq);
static int dequeuePid(sem_t * sem);

static sem_t semaphores[MAX_SEMAPHORES];
static unsigned int sem_size = 0; 
static unsigned int sem_amount = 0; 

sem_id ksem_open(const char * name) {
    return ksem_init_open(name, 1);
}

sem_id ksem_init_open(const char * name, unsigned int init_val) {
    if (name == NULL || *name == '\0')
        return -1;
    unsigned int i = 0;
    int pid;
    getPid(&pid);
    while (i < sem_size && semaphores[i].name[0] != '\0') {
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
            if (j == semaphores[i].processes_size)
                semaphores[i].processes_size++; 
            semaphores[i].processes[j].pid = pid; // Add pid to the processes appended to this semaphore
            semaphores[i].processes_amount++;   
            return i;
        }
        i++;
    }
    if (i < MAX_SEMAPHORES) {
        strcpy(semaphores[i].name, name);
        semaphores[i].first = NULL;
        semaphores[i].last = NULL;
        semaphores[i].value = 1;
        semaphores[i].lock = 0;
        semaphores[i].processes_waiting = 0;
        semaphores[i].processes[0].pid = pid;
        semaphores[i].processes[0].next = NULL;
        semaphores[i].processes_amount = 1;
        semaphores[i].processes_size = 1;
        semaphores[i].value = init_val;
        if (i == sem_size)
            sem_size++;
        sem_amount++;
        return i;
    }
    else
        return -1;
}

int ksem_wait(sem_id sem){
    if (sem < 0 || sem >= sem_size || semaphores[sem].name[0] == '\0')
        return -1; // Semaphore does not exist    

    int pid;
    getPid(&pid);
    int idx = getIndexOnSem(pid, &(semaphores[sem]));
    if (idx < 0)
        return -1;

    spin_lock(&(semaphores[sem].lock));
    if (semaphores[sem].value == 0) {
        enqueue(&(semaphores[sem]), &(semaphores[sem].processes[idx]));
        spin_unlock(&(semaphores[sem].lock));
        changeState(pid, BLOCKED);   
        spin_lock(&(semaphores[sem].lock));
    }
    semaphores[sem].value--;
    spin_unlock(&(semaphores[sem].lock));

    return 0; 
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

int ksem_post(sem_id sem) {
    if (sem < 0 || sem >= sem_size || semaphores[sem].name[0] == '\0')
        return -1; // Semaphore does not exist

    spin_lock(&(semaphores[sem].lock));
    semaphores[sem].value++;
    spin_unlock(&(semaphores[sem].lock));

    if (semaphores[sem].processes_waiting > 0) {
        int pid = dequeuePid(&(semaphores[sem]));
        if (pid < 0)
            return -1; // Failed at dequeuing
        changeState(pid, READY);
    }
    return 0;
}

int ksem_close(sem_id sem) { //remove a ps from semaphore
    if (sem < 0 || sem >= sem_size || semaphores[sem].name[0] == '\0')
        return -1; // Semaphore does not exist
    int pid;
    getPid(&pid);
    unsigned int i = 0;
    while (i < semaphores[sem].processes_size) {
        if (semaphores[sem].processes[i].pid == pid) {
            semaphores[sem].processes[i].pid = -1;
            if (i == semaphores[sem].processes_size - 1)
                semaphores[sem].processes_size--;
            semaphores[sem].processes_amount--;
            if (semaphores[sem].processes_amount == 0) { // delete semaphore
                semaphores[sem].name[0] = '\0';
                if (sem == sem_size - 1)
                    sem_size--;
                sem_amount--;
            }
            return 0;
        }
        i++;
    }
    return -1; // process was not found on semaphore
}

int ksem_getvalue(sem_id sem, int * sval) { // sval is either 0 is returned; or a negative number whose absolute value is the count of the number of processes and threads currently blocked in sem_wait(3)
    if (sem < 0 || sem >= sem_size || semaphores[sem].name[0] == '\0')
        return -1; // Semaphore does not exist
    *sval = - semaphores[sem].processes_waiting; 
    return (int) semaphores[sem].value; 
}