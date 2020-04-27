#include <process.h>
#include <timet.h>
#include <screen.h>
#include <mm.h>

static void prepareStackProcess(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp);
static void * getNextProcess(void * rsp);
static void updateProcess(PCB * pp);
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
unsigned int processes_alive = 0; //without the killed ones
unsigned int processes_ready = 0;
unsigned int processes_so_far = 0; //processes created, for the pid to be uique
int started = 0;

void * scheduler(void * rsp) {

    //printString("scheduler\n", 11);

    if (!quantum_started) {
        initQuantums();
        quantum_started = 1;
    }

    if (processes_ready > 0) {
        //printString("hay proceso\n",13);
        act_queue = queues[actual_queue];
        exp_queue = queues[1 - actual_queue];

        if (curr_process != NULL) { // process running
            (curr_process->given_time)--;

            if (curr_process->given_time <= 0) { //quantum finished --> go 
                // update info and go to expired
                
                // update aging
                (curr_process->aging)++;

                updateProcess(curr_process);
                curr_process->rsp = rsp;

                #ifdef _RR_AGING_ 
                    // set new priority
                    changePriority(curr_process->pid, (prior < MAX_PRIOR)? prior+1 : prior);
                #endif

                if (curr_process->state != BLOCKED) // Blocked current process
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

    else { //there are no processes ready
        ////printString("no hay proceso\n", 16);
        
        if ( ((uint64_t *) rsp)[16] >= 0x400000 ) // If RIP >= sampleCodeModuleAddress => Kernel is not running
            _halt_and_wait(); 

        return rsp;
    }

}

static void initQuantums() {
    for (unsigned int p = 0; p < MAX_PRIORITY - MIN_PRIORITY + 1; p++) {
        quantum[p] = PRIOR_SLOPE * p + PRIOR_INDVAR;
    }
}

static void updateProcess(PCB * pp) {
    // update act_queue, delete it from queue
    if (curr_process == pp) {
        act_queue[prior].first = curr_process->next_in_queue;
        if (act_queue[prior].last == curr_process)
            act_queue[prior].last = NULL;
    }
    
    pp->next_in_queue = NULL;
}

static void * getNextProcess(void * rsp) {
    while (prior <= MAX_PRIORITY && act_queue[prior].first == NULL) //go to a queue with lower priority
    prior++;

    if (prior <= MAX_PRIORITY) {
        if ((act_queue[prior].first)->state == KILLED || (act_queue[prior].first)->state == BLOCKED) {
            act_queue[prior].first = (act_queue[prior].first)->next_in_queue;
            return getNextProcess(rsp);
        }
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

int createProcess(main_func_t * main_f, char * name, int foreground, int * pid) {
    ////printString("Creando proceso!", 17);
    int i = 0;
    while(i < processes_size && processes[i].state != KILLED)
        i++;

    if (i < MAX_PROCESSES) {
        strcpy(processes[i].name, name);
        processes[i].pid = (processes_so_far + 1);
        processes[i].ppid = (curr_process != NULL)?curr_process->pid:0; 
        processes[i].foreground = (curr_process == NULL)?1:(curr_process->foreground)?foreground:0;
        processes[i].priority = BASE_PRIORITY;
        processes[i].state = READY;
        processes[i].next_in_queue = NULL;
        processes[i].aging = 0; 
        processes[i].given_time = quantum[BASE_PRIORITY];

        processes[i].stack = malloc(MAX_STACK_PER_PROCESS); // 600010

        /*
        00 |         |
        |         | 600010 ->  |
        |   0202  |            | > MAX_STACK_PER_PROCESS
        |  rsp    | rbp    ->  |
        ff |   0     |
        */

        // 8, 0      1000 o 0000       -> 111111111111111111000 & 11101  -> 11000

        processes[i].rbp = (void *)(((uint64_t) processes[i].stack + MAX_STACK_PER_PROCESS) & -8);
        processes[i].rsp = (void *) (processes[i].rbp - (INT_PUSH_STATE + PUSH_STATE_REGS) * sizeof(uint64_t));

        if (i >= processes_size)
            processes_size++;

        //printString("Process created at index:", 26);
        //printDec(i);
        //printString("\n", 2);

        prepareStackProcess(main_f->f, main_f->argc, main_f->argv, processes[i].rbp, processes[i].rsp);

        enqueueProcess(&(processes[i]));

        processes_alive++;
        processes_ready++; 
        processes_so_far++;

        *pid = processes[i].pid;

        if (curr_process != NULL && processes[i].foreground) {
            changeState(curr_process->pid, BLOCKED);
        }
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

int getPid(int * pid) {
    *pid = curr_process->pid;
    return 0;
}

int getProcessesAlive(unsigned int * amount) {
    *amount = processes_alive;
    return 0; 
}

int getProcessesInfo(PCB_info * arr, unsigned int max_size, unsigned int * size) {
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
            arr[j].aging = processes[i].aging; 
            arr[j].given_time = processes[i].given_time;
            j++;
        }
    }
    *size = j;
    return 0;
}

int exit() {
    return kill(curr_process->pid);
}

int changePriority(int pid, unsigned int new_priority) {
    
    if(new_priority >= MIN_PRIORITY && new_priority <=MAX_PRIORITY){
        for(int i=0; i < processes_size; i++){
            if(processes[i].pid == pid) {
                if (processes[i].state == KILLED)
                    return -1;
                processes[i].priority = new_priority; 
                return 0;
            }
        }
    }
    return -1; 
}

int changeState(int pid, process_state new_state) {
    for(int i=0; i < processes_size; i++){
        if(processes[i].pid == pid) {
            int last_state = processes[i].state;

            if (last_state == KILLED)
                return -1;

            processes[i].state = new_state;

            if (last_state == BLOCKED && new_state == READY) {
                enqueueProcess(&(processes[i]));
                processes_ready++;
            }
            else if (last_state == READY && new_state == BLOCKED){
                processes_ready--;
                if (curr_process != NULL && curr_process->pid == pid) {

                    if (act_queue == NULL || exp_queue == NULL) {
                        act_queue = queues[actual_queue];
                        exp_queue = queues[1 - actual_queue];
                    }
                    curr_process->state = BLOCKED;
                    curr_process->given_time = 1;

                    _sti();
                    _int81(); //wait for the next ps
                }
                else {
                    updateProcess(&(processes[i]));
                }
            }

            if (new_state == KILLED){
                if (processes[i].foreground && processes[i].ppid > 0) //if current is not shell
                    changeState(processes[i].ppid, READY); //bring parent back

                if (curr_process != NULL && curr_process->pid == pid)
                    updateProcess(&(processes[i]));
                if (curr_process != NULL && curr_process->pid == pid) {
                    updateProcess(&(processes[i]));
                    curr_process = NULL;
                    free(processes[i].stack);
                    _sti();
                    _int81();
                }
                free(processes[i].stack);
                processes_alive--;
                processes_ready--;
            }

            return 0;
        }
    }
    return -1; 
}

int changeForegroundStatus(int pid, int status) {
    for(int i=0; i < processes_size; i++){
        if(processes[i].pid == pid) {
            if (processes[i].state == KILLED)
                return -1;
            processes[i].foreground = status; 
            return 0;
        }
    }
    return -1; 
}

int getProcessState(int pid, process_state * state) {
    for(int i=0; i < processes_size; i++){
        if(processes[i].pid == pid) {
            *state = processes[i].state; 
            return 0;
        }
    }
    return -1; 
}

int isCurrentForeground(void) {
    if (curr_process == NULL)
        return -1;
    return curr_process->foreground;
}

int sys_process(void * option, void * arg1, void * arg2, void * arg3, void * arg4) {
    
    switch ((uint64_t) option) {
        case 0:
            return createProcess((main_func_t *) arg1, (char *) arg2, (int)(uint64_t) arg3, (int *) arg4);
            break;
        case 1:
            return kill((int)(uint64_t) arg1); 
            break;
        case 2:
            return getPid((int *) arg1);
            break;
        case 3:
            return getProcessesAlive((unsigned int *) arg1);
            break;
        case 4:
            return getProcessesInfo((PCB_info *) arg1, (unsigned int)(uint64_t) arg2, (unsigned int *) arg3);  
            break;
        case 5:
            return exit(); 
            break;
        case 6:
            return changePriority((int)(uint64_t) arg1, (int)(uint64_t) arg2);
            break;
        case 7:
            return changeState((int)(uint64_t) arg1, (process_state)(uint64_t) arg2); 
            break;
        case 8:
            return changeForegroundStatus((int)(uint64_t) arg1, (int)(uint64_t) arg2);
            break;
        case 9:
            return getProcessState((int)(uint64_t) arg1, (process_state *) arg2);
            break;
    }
    return 0;
}