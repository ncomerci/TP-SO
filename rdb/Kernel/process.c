
#include <process.h>
#include <timet.h>
#include <screen.h>
#include <fd.h>
#include <mm.h>

static void prepareStackProcess(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp);
static void * getNextProcess(void * rsp);
static void updateProcess(PCB * pp);
static void enqueueProcess(PCB * pp);
static void initQuantums(void);
static void createHalter(void);

static stackProcess stackModel = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x8, 0x202, 0, 0};
static PCB processes[MAX_PROCESSES];

static PCB halter;
static uint64_t halterStack[HALTER_STACK];

static QUEUE_HD queues[2][MAX_PRIORITY - MIN_PRIORITY + 1];
static int actual_queue = 0;
static QUEUE_HD * act_queue = NULL; //queue with active processes
static QUEUE_HD * exp_queue = NULL; //queue with those processes that have expired their quantum or are new or priority changed
static unsigned quantum[MAX_PRIORITY - MIN_PRIORITY + 1];
static int initialized = 0;
static PCB * curr_process = NULL;
static unsigned int prior = 0;
static uint64_t processes_size = 0; //array size
static uint64_t processes_alive = 0; //without the killed ones
static uint64_t processes_ready = 0;
static uint64_t processes_so_far = 0; //processes created, for the pid to be unique

static uint64_t isValidProcess(PCB *p){
  if (!p) return 1;

  int i;
  for (i = 0; i < MAX_PROCESSES ; i++)
    if (p == &processes[i]) return 1;

  return 0;
}

static uint64_t checkQueue(QUEUE_HD *p){
  //vacia
  if (p->first == NULL && p->last == NULL) return 1;

  //Uno NULL y el otro no
  if (p->first == NULL || p->last == NULL) return 0;

  PCB *pcb_p = p->first;
  int count = 0;

  //desde first debo llegar a last en menos de MAX_PROCESSES pasos y last->next_in_queue debe ser NULL
  while(pcb_p != p->last && count < MAX_PROCESSES){
    if (pcb_p == NULL || !isValidProcess(pcb_p)) return 0;
    pcb_p = pcb_p->next_in_queue;
    count++;
  }

  if (pcb_p == p->last && isValidProcess(pcb_p) && pcb_p->next_in_queue == NULL) return 1;

  return 0;
}

static uint64_t checkQueues(){
   int i;
   for (i = 0; i < MAX_PRIORITY - MIN_PRIORITY + 1; i++){
     if (!checkQueue(&queues[0][i])) return 0;
     if (!checkQueue(&queues[1][i])) return 0;
   }
  return 1;
}


void * scheduler(void * rsp) {

    if (!initialized) {
        initQuantums();
        createHalter();
        initialized = 1;
    }

    if (curr_process != NULL || processes_ready > 0) { // curr_process == NULL && process_ready = 0 in case current was blocked
        act_queue = queues[actual_queue];
        exp_queue = queues[1 - actual_queue];

        if (curr_process != NULL) { // process running
            (curr_process->given_time)--;

            if (curr_process->given_time <= 0) { //quantum finished --> go 
                // update info and go to expired
                
                // update aging
                (curr_process->aging)++;

                updateProcess(curr_process); //delete it from act queue
                curr_process->rsp = rsp;

                #ifdef _RR_AGING_ 
                    // set new priority
                    changePriority(curr_process->pid, (prior < MAX_PRIOR)? prior+1 : prior);
                #endif

                if (curr_process->state == BLOCKED)
                    processes_ready--;
                else // Blocked current process
                    enqueueProcess(curr_process); // Expires the current process

                if (processes_ready <= 0) {
                    curr_process = NULL;
                    _outportb(0x20, 0x20);
                    return halter.rsp;
                }
            }
            else {
                _outportb(0x20, 0x20);
                return rsp; //keep running
            }
        }
        // get next process
        return getNextProcess(rsp);
    }

    else { //there are no processes ready
        _outportb(0x20, 0x20);
        return halter.rsp;
    }
}

static void createHalter(void) {
    strcpy(halter.name, "Halter");
    halter.pid = 0;
    halter.ppid = 0; 
    halter.foreground = 0;
    halter.priority = MAX_PRIORITY;
    halter.state = READY;
    halter.next_in_queue = NULL;
    halter.aging = 0; 
    halter.given_time = quantum[BASE_PRIORITY];
    halter.stack = halterStack;
    halter.rbp = (void *)(((uint64_t) halter.stack + sizeof(halterStack)) & -8);
    halter.rsp = (void *) (halter.rbp - (INT_PUSH_STATE + PUSH_STATE_REGS) * sizeof(uint64_t));

    stackModel.rbp = (uint64_t) halter.rbp;
    stackModel.rsp = (uint64_t) halter.rbp;
    stackModel.rip = (uint64_t) _halter;
    
    memcpy(halter.rsp, (void *) &stackModel, sizeof(stackModel));
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
}

static void * getNextProcess(void * rsp) {
    while (prior <= MAX_PRIORITY && act_queue[prior].first == NULL) //go to a queue with lower priority
    prior++;

    if (prior <= MAX_PRIORITY) {
        if ((act_queue[prior].first)->state == KILLED || (act_queue[prior].first)->state == BLOCKED) {
            if (act_queue[prior].last == act_queue[prior].first)
                act_queue[prior].last = NULL;
            act_queue[prior].first = (act_queue[prior].first)->next_in_queue;
            return getNextProcess(rsp);
        } 

        curr_process = act_queue[prior].first; 

        curr_process->given_time = quantum[prior];
   
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

int createProcess(ps_info_t * ps_info, fd_info_t * fd_info, uint64_t * pid) {
    
    if (ps_info == NULL || fd_info == NULL || pid == NULL)
        return -1;

    main_func_t * main_f = ps_info->main;
    char * name = ps_info->name;
    int foreground = ps_info->foreground;

    char * in = fd_info->in;
    char * out = fd_info->out;

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

        assignInAndOut(i, in, out);

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
    pp->next_in_queue = NULL;
}

int kill(uint64_t pid) {
    return changeState(pid, KILLED);
}

int getPid(uint64_t * pid) {
    *pid = curr_process->pid;
    return 0;
}

int getCurrentIdx(void) {
    if (curr_process == NULL)
        return -1;
    return (( (uint64_t) curr_process - (uint64_t) processes ) / sizeof(PCB));
}

int getProcessesAlive(uint64_t * amount) {
    *amount = processes_alive;
    return 0; 
}

int getProcessesInfo(PCB_info * arr, uint64_t max_size, uint64_t * size) {
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

int changePriority(uint64_t pid, unsigned int new_priority) {
    
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

int changeState(uint64_t pid, process_state new_state) {
    for(int i=0; i < processes_size; i++){
        if(processes[i].pid == pid) {
            process_state last_state = processes[i].state;

            if (last_state == new_state || last_state == KILLED)
                return -1;

            processes[i].state = new_state;

            if (last_state == BLOCKED && new_state == READY) {
                enqueueProcess(&(processes[i]));
                processes_ready++;
            }
            else if (last_state == READY && new_state == BLOCKED){

                if (curr_process != NULL && curr_process->pid == pid) {
                    curr_process->given_time = 1;
                    _outportb(0x20, 0x20);
                    _sti();

                    _int81(); //wait for the next ps
                }
                else {
                    processes_ready--;
                    updateProcess(&(processes[i]));
                }
            }
            else if (new_state == KILLED){
                if (processes[i].foreground && processes[i].ppid > 0) //if current is not shell
                    changeState(processes[i].ppid, READY); //bring parent back

                processes_alive--;
                if (last_state == READY)
                    processes_ready--;

                if (curr_process != NULL && curr_process->pid == pid) {
                    updateProcess(&(processes[i]));
                    curr_process = NULL;
                    free(processes[i].stack);
                    _outportb(0x20, 0x20);
                    _sti();
                    _int81();
                }
                else {
                    free(processes[i].stack);
                }
            }
            return 0;
        }
    }
    return -1; 
}

int changeForegroundStatus(uint64_t pid, int status) {
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

int getProcessState(uint64_t pid, process_state * state) {
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

int sys_process(void * option, void * arg1, void * arg2, void * arg3) {
    
    switch ((uint64_t) option) {
        case 0:
            return createProcess((ps_info_t *) arg1, (fd_info_t *) arg2, (uint64_t *) arg3);
            break;
        case 1:
            return kill((uint64_t) arg1); 
            break;
        case 2:
            return getPid((uint64_t *) arg1);
            break;
        case 3:
            return getProcessesAlive((uint64_t *) arg1);
            break;
        case 4:
            return getProcessesInfo((PCB_info *) arg1, (uint64_t) arg2, (uint64_t *) arg3);  
            break;
        case 5:
            return exit(); 
            break;
        case 6:
            return changePriority((uint64_t) arg1, (unsigned int)(uint64_t) arg2);
            break;
        case 7:
            return changeState((uint64_t) arg1, (process_state)(uint64_t) arg2); 
            break;
        case 8:
            return changeForegroundStatus((uint64_t) arg1, (int)(uint64_t) arg2);
            break;
        case 9:
            return getProcessState((uint64_t) arg1, (process_state *) arg2);
            break;
    }
    return 0;
}
