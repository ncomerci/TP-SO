#ifndef _PRCS_H
    #define _PRCS_H

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

    typedef enum PROCESS_STATE {READY = 0, BLOCKED, KILLED} process_state;

    typedef struct PCB {
        char name[MAX_NAME_LENGTH];
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
        unsigned int quantum;
        struct PCB * first;
        struct PCB * last;
    } QUEUE_HD;

    typedef struct main_func_t {
        int (*f)(int, char *);
        int argc;
        char * argv;
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