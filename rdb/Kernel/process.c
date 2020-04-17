#include <process.h>
#include <timet.h>
#include <screen.h>
#include <mm.h>

static void prepareStackProcess(int (*main)(int argc, char *argv), int argc, char * argv, void * rsp);
static void * getNextProcess(void * rsp);
static void updateCurrent(void);
static void enqueueProcess(PCB * pp);
static void expireCurrent(void);

static PCB processes[MAX_PROCESSES];
static QUEUE_HD queues[2][MAX_PROCESSES];
static int actual_queue = 0;
static QUEUE_HD * act_queue; //queue with active processes
static QUEUE_HD * exp_queue; //queue with those processes that have expired their quantum or are new or priority changed
static PCB * curr_process = NULL;
static unsigned int prior = 0;
static unsigned int processes_size = 0; //array size
static unsigned int process_amount = 0; //without the killed ones
static int started = 0;

void * scheduler(void * rsp) {

    if (process_amount > 0) {

        if (curr_process != NULL) { // process running
            (curr_process->given_time)--;

            if (curr_process->given_time == 0) { //quantum finished --> go 

                act_queue = queues[actual_queue];
                exp_queue = queues[1 - actual_queue];

                // update info and go to expired
                updateCurrent();
                curr_process->rsp = rsp;
                expireCurrent();
            }
        }
        // get next process
        return getNextProcess(rsp);
    }

    else { //there is no processes
        return rsp;
    }
}

static void updateCurrent() {
    // update info
    (curr_process->aging)++;

    // update act_queue
    act_queue[prior].first = curr_process->next_in_queue;
    curr_process->next_in_queue = NULL;
}

static void expireCurrent(void) {
    // if aging is used then go to a lower priority expired queue
    #ifdef _RR_AGING_ 
        // set new priority
        changePriority(curr_process->pid, (prior < MAX_PRIOR)? prior+1 : prior);
    #endif

    if (exp_queue[curr_process->priority].last != NULL)
        exp_queue[curr_process->priority].last->next_in_queue = curr_process;
    exp_queue[curr_process->priority].last = curr_process;
}

static void * getNextProcess(void * rsp) {
    while (prior <= MAX_PRIORITY && act_queue[prior].first == NULL) //go to a queue with lower priority
    prior++;

    if (prior <= MAX_PRIORITY) {
        curr_process = act_queue[prior].first;
        curr_process->given_time = act_queue[prior].quantum;
        return curr_process->rsp;  
    }

    else {
        actual_queue = 1 - actual_queue; //change from active queue to expired queue
        prior = 0;
        curr_process = NULL;
        return scheduler(rsp); //do it again but with the expired one
    }
}

int createProcess(main_func_t * main_f, char * name, int foreground) {
    //printString("Creando proceso!", 17);
    int i = 0;
    while(i < processes_size && processes[i].state != KILLED)
        i++;

    if (i < MAX_PROCESSES) {
        strcpy(processes[i].name, name); 
        processes[i].foreground = foreground;
        processes[i].priority = BASE_PRIORITY;
        processes[i].state = READY;
        processes[i].rbp = processes[i].rsp = malloc(MAX_STACK_PER_PROCESS);
        processes[i].next_in_queue = NULL;
        processes[i].aging = 0; 
        processes[i].given_time = act_queue[BASE_PRIORITY].quantum;

        if (i >= processes_size)
            processes_size++;

        prepareStackProcess(main_f->f, main_f->argc, main_f->argv, processes[i].rsp); 

        enqueueProcess(&(processes[i]));

        process_amount++; 

        return 0;
    }

    return -1;
}

static void prepareStackProcess(int (*main)(int argc, char *argv), int argc, char * argv, void * rsp) {
    void * new_rsp = rsp;
    if( ((uint64_t) new_rsp & portBYTE_ALIGNMENT_MASK) != 0 ) { // si no esta alineado
		new_rsp = (void *) ((uint64_t) rsp + ( portBYTE_ALIGNMENT - 1 )); //then do it
		new_rsp = (void *) ((uint64_t) rsp & ~( (uint64_t) portBYTE_ALIGNMENT_MASK ));
	}
    
    uint64_t stack[INT_PUSH_STATE + PUSH_STATE_REGS] = {0x0, (uint64_t) new_rsp, 0x202, 0x8, (uint64_t) _start, 1, 2, 3, 4, 5, (uint64_t) main, (uint64_t) argc, (uint64_t) argv, 9, 10, 11, 12, 13, 14, 15}; // Para debuguear linea a linea, despues debería usar una struct
    printString("\n", 1);
    for (unsigned int i = 0; i < sizeof(stack)/sizeof(uint64_t); i++) {
        memcpy(new_rsp, (void *) &(stack[i]), 8);
        print64Hex(stack[i]);
        printString("\n", 1);
        new_rsp = (void *) ((uint64_t) new_rsp + 8);
    }
}

static void enqueueProcess(PCB * pp) {
    pp->next_in_queue = exp_queue[pp->priority].first;
    exp_queue[pp->priority].first = pp;
    if (exp_queue[pp->priority].last == NULL) //there was no other process
        exp_queue[pp->priority].last = pp;
}

int kill(int pid) {
    return changeState(pid, KILLED);
}

int getPid(void) {
    return curr_process->pid;
}

unsigned int getProcessesAmount(void) {
    return process_amount; 
}

int getProcessesInfo(PCB * arr, unsigned int max_size) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < processes_size && j < max_size; i++) {
        if (processes[i].state != KILLED) {
            strcpy(arr[j].name, processes[i].name); 
            arr[j].foreground = processes[i].foreground;
            arr[j].priority = processes[i].priority;
            arr[j].state = processes[i].state;
            arr[j].rbp = processes[i].rbp;
            arr[j].rsp = processes[i].rsp;
            arr[j].next_in_queue = processes[i].next_in_queue;
            arr[j].aging = processes[i].aging; 
            arr[j].given_time = processes[i].given_time;
        }
    }
    return j;
}

int exit() {
    kill(curr_process->pid);
    _halt_and_wait();
    return 0;
}

int changePriority(int pid, unsigned int new_priority) {
     for(int i=0; i < process_amount; i++){
        if(processes[i].pid == pid) {
            processes[i].priority = new_priority; 
            return 0;
        }
    }
    return 1; 
}

int changeState(int pid, unsigned int new_state) {
    for(int i=0; i < process_amount; i++){
        if(processes[i].pid == pid) {
            processes[i].state = new_state;
            if (new_state == KILLED){
                updateCurrent();
                curr_process = NULL; 
                process_amount--;
            }
            return 0;
        }
    }
    return 1; 

}

int changeForegroundStatus(int pid, unsigned int status) {
     for(int i=0; i < process_amount; i++){
        if(processes[i].pid == pid) {
            processes[i].foreground = status; 
            return 0;
        }
    }
    return 1; 
}

int sys_process(void * option, void * arg1, void * arg2, void * arg3) {
    
    switch ((uint64_t) option) {
        case 0:
            return createProcess((main_func_t *) arg1, (char *) arg2, (int)(uint64_t) arg3);
            break;
        case 1:
            kill((int)(uint64_t) arg1); 
            break;
        case 2:
            getPid();
            break;
        case 3:
            getProcessesAmount();
            break;
        case 4:
            getProcessesInfo((PCB *) arg1, (unsigned int)(uint64_t) arg2);  
            break;
        case 5:
            exit(); 
            break;
        case 6:
            changePriority((int)(uint64_t) arg1, (int)(uint64_t) arg2);
            break;
        case 7:
            changeState((int)(uint64_t) arg1, (int)(uint64_t) arg2); 
            break;
        case 8:
            changeForegroundStatus((int)(uint64_t) arg1, (int)(uint64_t) arg2);
            break;
            
    }
    return 0;
}