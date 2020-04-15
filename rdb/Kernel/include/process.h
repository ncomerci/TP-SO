#ifndef _PRCS_H
    #define _PRCS_H

    #include <lib.h>

    #define MAX_PROCESSES 50
    #define MAX_NAME_LENGTH 50
    #define MAX_FDS 20
    #define MAX_STACK_PER_PROCESS (1 << 11) //2KB
    #define BASE_PRIORITY 120
    #define INT_PUSH_STATE 5
    #define PUSH_STATE_REGS 15

   typedef enum PROCESS_STATE {READY = 0, BLOCKED, KILLED} process_state;

    typedef struct PCB {
        char name[MAX_NAME_LENGTH];
        void * rsp;
        void * rbp;
        int pid;
        int ppid;
        int state;
        int foreground;
        unsigned int priority;
    } PCB;

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
    int changePriority(int pid, int new_priority);
    int changeState(int pid, int new_state);
    int changeForegroundStatus(int pid, int state);
    int sys_process(void * option, void * arg1, void * arg2, void * arg3);

#endif