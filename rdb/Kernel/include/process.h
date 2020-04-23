#ifndef PROCESS_H
    #define PROCESS_H

    #include <lib.h>

    #define MAX_PROCESSES 50
    #define MAX_NAME_LENGTH 50
    #define MAX_FDS 20
    #define MAX_STACK_PER_PROCESS (1 << 11) //2KB
    #define INT_PUSH_STATE 5
    #define PUSH_STATE_REGS 15

    #define MIN_PRIORITY 0
    #define MAX_PRIORITY 6
    #define BASE_PRIORITY ((MAX_PRIORITY - MIN_PRIORITY)/2)
    #define MIN_TICKS 10
    #define MAX_TICKS 100

    #define PRIOR_SLOPE ((MAX_TICKS - MIN_TICKS)/(MIN_PRIORITY - MAX_PRIORITY)) //To calculate the quantum of each queue (quantum(priorirty) = m*p + b)
    #define PRIOR_INDVAR (MAX_TICKS - MIN_PRIORITY * PRIOR_SLOPE)

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

    typedef enum PROCESS_STATE {READY = 0, BLOCKED, KILLED} process_state;

    typedef struct PCB {
        char name[MAX_NAME_LENGTH];
        void * stack;
        void * rsp;
        void * rbp;
        int pid;
        int ppid;
        int foreground;
        unsigned int state;
        unsigned int priority;
        unsigned int given_time;
        unsigned int aging; 
        struct PCB * next_in_queue;
    } PCB;

    typedef struct QUEUE_HD {
        PCB * first;
        PCB * last;
    } QUEUE_HD;

    typedef struct main_func_t {
        int (*f)(int, char **);
        int argc;
        char ** argv;
    } main_func_t;

    void * scheduler(void * rsp);

    int createProcess(main_func_t * main_f, char * name, int foreground);
    int kill(int pid);
    int getPid(void);
    unsigned int getProcessesAmount(void);
    int getProcessesInfo(PCB * arr, unsigned int max_size);
    int exit();
    int changePriority(int pid, unsigned int new_priority);
    int changeState(int pid, unsigned int new_state);
    int changeForegroundStatus(int pid, unsigned int state);
    int sys_process(void * option, void * arg1, void * arg2, void * arg3); 

#endif