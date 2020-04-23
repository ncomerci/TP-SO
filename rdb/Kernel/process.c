#include <process.h>
#include <timet.h>
#include <screen.h>
#include <mm.h>

static void prepareStackProcess(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp);
static void * getNextProcess(void * rsp);
static void updateCurrent(void);
static void enqueueProcess(PCB * pp);
static void initQuantums(void);

stackProcess stackModel = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x8, 0x202, 0, 0};
PCB processes[MAX_PROCESSES];
QUEUE_HD queues[2][MAX_PRIORITY - MIN_PRIORITY + 1];
int actual_queue = 0;
QUEUE_HD * act_queue = NULL; //queue with active processes
QUEUE_HD * exp_queue = NULL; //queue with those processes that have expired their quantum or are new or priority changed
unsigned quantum[MAX_PRIORITY - MIN_PRIORITY + 1];
int quantum_started = 0;
PCB * curr_process = NULL;
unsigned int prior = 0;
unsigned int processes_size = 0; //array size
unsigned int process_amount = 0; //without the killed ones
int started = 0;

void * scheduler(void * rsp) {

    //printString("scheduler\n", 11);

    if (!quantum_started) {
        initQuantums();
        quantum_started = 1;
    }

    if (process_amount > 0) {
        //printString("hay proceso\n",13);
        act_queue = queues[actual_queue];
        exp_queue = queues[1 - actual_queue];

        if (curr_process != NULL) { // process running
            (curr_process->given_time)--;

            if (curr_process->given_time == 0) { //quantum finished --> go 
                // update info and go to expired
                updateCurrent();
                curr_process->rsp = rsp;

                #ifdef _RR_AGING_ 
                    // set new priority
                    changePriority(curr_process->pid, (prior < MAX_PRIOR)? prior+1 : prior);
                #endif

                enqueueProcess(curr_process); // Expires the current process
            }
            else {
                //printString("Keep running Forest!\n", 22);
                return rsp; //keep running
            }
        }
        //printString("proximo proceso\n", 17);
        // get next process
        return getNextProcess(rsp);
    }

    else { //there are no processes
        ////printString("no hay proceso\n", 16);
        return rsp;
    }

}

static void initQuantums() {
    for (unsigned int p = 0; p < MAX_PRIORITY - MIN_PRIORITY + 1; p++) {
        quantum[p] = PRIOR_SLOPE * p + PRIOR_INDVAR;
    }
}

static void updateCurrent() {
    // update info
    (curr_process->aging)++;

    // update act_queue
    act_queue[prior].first = curr_process->next_in_queue;
    if (act_queue[prior].last == curr_process)
        act_queue[prior].last = NULL;

    curr_process->next_in_queue = NULL;
}

static void * getNextProcess(void * rsp) {
    while (prior <= MAX_PRIORITY && act_queue[prior].first == NULL) //go to a queue with lower priority
    prior++;

    if (prior <= MAX_PRIORITY) {
        //printString("proceso encontrado\n", 20);
        curr_process = act_queue[prior].first;
        //printString("proceso seleccionado\n", 22);
        curr_process->given_time = quantum[prior];
        //printString("time given\n", 12);
        return curr_process->rsp;  
    }

    else {
        //printString("no encontre proceso\n", 22);
        actual_queue = 1 - actual_queue; //change from active queue to expired queue
        prior = 0;
        curr_process = NULL;
        return scheduler(rsp); //do it again but with the expired one
    }
}

int createProcess(main_func_t * main_f, char * name, int foreground) {
    ////printString("Creando proceso!", 17);
    int i = 0;
    while(i < processes_size && processes[i].state != KILLED)
        i++;

    if (i < MAX_PROCESSES) {
        strcpy(processes[i].name, name);
        processes[i].pid = (process_amount + 1);
        processes[i].ppid = (curr_process != NULL)?curr_process->pid:0;
        processes[i].foreground = foreground;
        processes[i].priority = BASE_PRIORITY;
        processes[i].state = READY;
        processes[i].next_in_queue = NULL;
        processes[i].aging = 0; 
        processes[i].given_time = quantum[BASE_PRIORITY];

        uint64_t aux = (uint64_t) malloc(MAX_STACK_PER_PROCESS); // 600010

        /*
        00 |         |
        |         | 600010 ->  |
        |   0202  |            | > MAX_STACK_PER_PROCESS
        |  rsp    | rbp    ->  |
        ff |   0     |
        */

        // 8, 0      1000 o 0000       -> 111111111111111111000 & 11101  -> 11000

        processes[i].rbp = (void *)((aux + MAX_STACK_PER_PROCESS) & -8);
        processes[i].rsp = (void *) (processes[i].rbp - (INT_PUSH_STATE + PUSH_STATE_REGS) * sizeof(uint64_t));

        if (i >= processes_size)
            processes_size++;

        //printString("Process created at index:", 26);
        //printDec(i);
        //printString("\n", 2);

        prepareStackProcess(main_f->f, main_f->argc, main_f->argv, processes[i].rbp, processes[i].rsp); 

        enqueueProcess(&(processes[i]));

        process_amount++; 

        return 0;
    }

    return -1;
}

static void prepareStackProcess(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp) {
    stackModel.rbp = (uint64_t) rbp;
    stackModel.rsp = (uint64_t) rbp;
    stackModel.rip = (uint64_t) _start_process;
    stackModel.rdi = (uint64_t) main; 
    stackModel.rsi = (uint64_t) argc;
    stackModel.rdx = (uint64_t) argv; 
    
    memcpy(rsp, (void *) &stackModel, sizeof(stackModel));
}

static void enqueueProcess(PCB * pp) {
    if (act_queue == NULL || exp_queue == NULL) {
        act_queue = queues[actual_queue];
        exp_queue = queues[1 - actual_queue];
    }

    if (exp_queue[pp->priority].first == NULL)
        exp_queue[pp->priority].first = pp;

    if (exp_queue[pp->priority].last != NULL)
        (exp_queue[pp->priority].last)->next_in_queue = pp;
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
            arr[j].pid = processes[i].pid;
            arr[j].ppid = processes[i].ppid;
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
            return getProcessesAmount();
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