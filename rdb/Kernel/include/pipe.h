#ifndef _PIPE_H
#define _PIPE_H

#define MAX_PIPES 10
#define MAX_PROCESSES_PER_PIPE 5
#define MAX_PIPE_NAME_LEN 30
#define MAX_PIPE_BUFFER_SIZE 512

typedef struct pipe_t {
    char name[MAX_PIPE_NAME_LEN];
    char buffer[MAX_PIPE_BUFFER_SIZE];
    pipe_ps processes[MAX_PROCESSES_PER_PIPE];
    unsigned int processes_amount; //processes not closed 
    unsigned int processes_size; //all processes
    unsigned int idx;  //buffer idx
    unsigned int size; //buffer size
} pipe_t;

typedef struct pipe_ps {
    char occupied;
    uint64_t pid;
} pipe_ps;

typedef struct pipe_info {
    char name[MAX_PIPE_NAME_LEN];
    unsigned int blocked_processes; 
} pipe_info;

int getPipeInIndex(char * name);
int getPipeOutIndex(char * name);
int pipeWrite(int gate, char * str, unsigned int str_size);
int pipeRead(int gate, char * buff, unsigned int count);
int openPipe(char * name);
int closePipes(int gate);

#endif