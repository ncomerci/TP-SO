#include <process.h>
#include <timet.h>
#include <screen.h>
#include <mm.h>

static void prepareStackProcess(int (*main)(int argc, char *argv), int argc, char * argv, void * rsp);

static PCB processes[MAX_PROCESSES];
static unsigned int current = 0;
static unsigned int amount = 0; 
static int started = 0;

void * scheduler(void * rsp) {
    if (started) {
        if (amount == 0) {
            printString("Halting !", 10);
            _halt_and_wait();
        }
        else {
            printString("Entre !", 8);
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
    processes[amount].rbp = processes[amount].rsp = malloc(MAX_STACK_PER_PROCESS);
    prepareStackProcess(main_f->f, main_f->argc, main_f->argv, processes[amount].rsp); 
    amount++; 
    if (!started) {
        started = 1;
    }
    return 0;
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