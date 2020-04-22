#include <process.h>
#include <timet.h>
#include <screen.h>
#include <mm.h>

static void prepareStackProcess(int (*main)(int argc, char *argv), int argc, char * argv, void * rbp, void * rsp);

typedef struct stackProcess {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} stackProcess;

static PCB processes[MAX_PROCESSES];
static unsigned int current = 0;
static unsigned int amount = 0; 
static int started = 0;
stackProcess stackModel = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x8, 0x202, 0, 0};

void * scheduler(void * rsp) {
    if (started) {
        if (amount == 0) {
            printString("Halting !", 10);
            _halt_and_wait();
        }
        else {
            printString("Entre !", 8);
            started = 0;
            return processes[0].rsp;
        }
    }
    return rsp;
}

int createProcess(main_func_t * main_f, char * name, int foreground) {
    printString("Creando proceso!", 17);
    strcpy(processes[amount].name, name); 
    processes[amount].foreground = foreground;
    processes[amount].priority = BASE_PRIORITY;
    processes[amount].state = READY;
    uint64_t aux = (uint64_t) malloc(MAX_STACK_PER_PROCESS); // 600010

    /*
    00 |         |
       |         | 600010 ->  |
       |   0202  |            | > MAX_STACK_PER_PROCESS
       |  rsp    | rbp    ->  |
    ff |   0     |
    */

    // 8, 0      1000 o 0000       -> 111111111111111111000 & 11101  -> 11000

    processes[amount].rbp = (void *)((aux + MAX_STACK_PER_PROCESS) & -8);
    processes[amount].rsp = (void *) (processes[amount].rbp - (INT_PUSH_STATE + PUSH_STATE_REGS) * sizeof(uint64_t));
    prepareStackProcess(main_f->f, main_f->argc, main_f->argv, processes[amount].rbp, processes[amount].rsp); 
    amount++; 
    if (!started) {
        started = 1;
    }
    return 0;
}

static void prepareStackProcess(int (*main)(int argc, char *argv), int argc, char * argv, void * rbp, void * rsp) {
    stackModel.rbp = (uint64_t) rbp;
    stackModel.rsp = (uint64_t) rbp;
    stackModel.rip = (uint64_t) _start_process;
    stackModel.rdi = (uint64_t) main; 
    stackModel.rsi = (uint64_t) argc;
    stackModel.rdx = (uint64_t) argv; 
    
    memcpy(rsp, (void *) &stackModel, sizeof(stackModel));
}

int kill(int pid) {
    changeState(pid, KILLED);
    amount--; 
    return 0;
}

int getPid(void) {
    return processes[current].pid;
}

unsigned int getProcessesAmount(void) {
    return amount; 
}

int getProcessesInfo(PCB * arr, unsigned int max_size) {
    return 0;
}

int exit() {
    kill(current);
    _halt_and_wait();
    return 0;
}

int changePriority(int pid, int new_priority) {
     for(int i=0; i < amount; i++){
        if(processes[i].pid == pid)
            processes[i].priority = new_priority; 
            return 0;
    }
    return 1; 
}

int changeState(int pid, int new_state) {
    for(int i=0; i < amount; i++){
        if(processes[i].pid == pid)
            processes[i].state = new_state; 
            return 0;
    }
    return 1; 

}

int changeForegroundStatus(int pid, int status) {
     for(int i=0; i < amount; i++){
        if(processes[i].pid == pid)
            processes[i].foreground = status; 
            return 0;
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