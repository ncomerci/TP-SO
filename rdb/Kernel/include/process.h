#ifndef PROCESS_H
    #define PROCESS_H

    #include <lib.h>

    #define MAX_PROCESSES 50
    #define MAX_NAME_LENGTH 50
    #define MAX_STACK_PER_PROCESS (1 << 17) //128KB
    #define HALTER_STACK (1 << 10)
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

    typedef enum process_state {READY = 0, BLOCKED, KILLED} process_state;

    typedef struct PCB {
        char name[MAX_NAME_LENGTH];
        void * stack;
        void * rsp;
        void * rbp;
        uint64_t pid;
        uint64_t ppid;
        int foreground;
        process_state state;
        unsigned int priority;
        unsigned int given_time;
        unsigned int aging; 
        struct PCB * next_in_queue;
    } PCB;

    typedef struct PCB_info {
        char name[MAX_NAME_LENGTH];
        void * rsp;
        void * rbp;
        uint64_t pid;
        uint64_t ppid;
        int foreground;
        process_state state;
        unsigned int priority;
        int given_time;
        unsigned int aging; 
    } PCB_info;

    typedef struct QUEUE_HD {
        PCB * first;
        PCB * last;
    } QUEUE_HD;

    typedef struct main_func_t {
        int (*f)(int, char **);
        int argc;
        char ** argv;
    } main_func_t;

    typedef struct ps_info_t {
        main_func_t * main;
        char * name;
        int foreground;
    } ps_info_t;

    typedef struct fd_info_t {
        char * in;
        char * out;
    } fd_info_t;

    void * scheduler(void * rsp);

    int createProcess(ps_info_t * ps_info, fd_info_t * fd_info, uint64_t * pid);
    int kill(uint64_t pid);
    int getPid(uint64_t * pid);
    int getCurrentIdx(void);
    int getProcessesAlive(uint64_t * amount);
    int getProcessesInfo(PCB_info * arr, uint64_t max_size, uint64_t * size);
    int exit(void);
    int changePriority(uint64_t pid, unsigned int new_priority);
    int changeState(uint64_t pid, process_state new_state);
    int changeForegroundStatus(uint64_t pid, int status);
    int isCurrentForeground(void);
    int sys_process(void * option, void * arg1, void * arg2, void * arg3);
    int getCurrentIdx(void); 

#endif