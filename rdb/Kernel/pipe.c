#include <pipe.h>
#include <process.h>
#include <ksem.h>
#include <lib.h>

static int getPipeIndex(char * name);

pipe_t pipes[MAX_PIPES];
unsigned int pipes_size = 0;
unsigned int pipes_amount = 0;

int pipeWrite(int gate, char * str, unsigned int str_size) { //post
    int idx = gate/2;

    if (idx <= 0 && idx > MAX_PIPES)
        return -1;

    char sem_aux_name[SEM_NAME_MAX_LENGTH];

    strcpy(sem_aux_name, pipes[idx].name);
    strcat(sem_aux_name, "w"); 
    sem_id sem_write = ksem_open(sem_aux_name); 
    if(sem_write == -1 )
        return -1; 
    
    strcpy(sem_aux_name, pipes[idx].name);
    strcat(sem_aux_name, "r"); 
    sem_id sem_read = ksem_open(sem_aux_name); 
    if(sem_read == -1)
        return -1; 
         
    ksem_wait(sem_write);  //si alguien esta escribiendo, no escribas
    
    unsigned int size = str_size;
    unsigned int buff_idx = pipes[idx].idx;
    unsigned int i = 0;

    while ( MAX_PIPE_BUFFER_SIZE < size ) {//no alcanza el buffer 
        i = 0;
        while (i < size) {
            pipes[idx].buffer[(buff_idx + pipes[idx].size + i) % MAX_PIPE_BUFFER_SIZE] = str[i];
            i++;
        }      
        size -= i;
        pipes[idx].size += i;
        ksem_wait(sem_read); //escribi una parte y espera a que la lean               
    }
    i = 0;
    while (i < size) {
        pipes[idx].buffer[(buff_idx + pipes[idx].size + i) % MAX_PIPE_BUFFER_SIZE] = str[i];
        i++;
    }
    pipes[idx].size += i;

    ksem_post(sem_write); // avisa que terminaste de escribir
    
    ksem_close(sem_write);
    ksem_close(sem_read); 
    return 0; 
}

int pipeRead(int gate, char * buff, unsigned int count) { //wait
    int idx = gate/2; 

    if (idx <= 0 && idx > MAX_PIPES)
        return -1;

    char sem_aux_name[SEM_NAME_MAX_LENGTH];

    strcpy(sem_aux_name, pipes[idx].name);
    strcat(sem_aux_name, "w"); 
    sem_id sem_write = ksem_open(sem_aux_name); 
    if(sem_write == -1 )
        return -1; 
    
    strcpy(sem_aux_name, pipes[idx].name);
    strcat(sem_aux_name, "r"); 
    sem_id sem_read = ksem_open(sem_aux_name); 
    if(sem_read == -1)
        return -1; 

    if (count == 0)
        return 0;

    ksem_wait(sem_read); // Si alguien esta leyendo, espera a que termine. Cuando me desbloquea leo
    
    if (pipes[idx].size == 0)
        ksem_wait(sem_write); // Espera a que alguien escriba algo
  
    unsigned int size = ((count < pipes[idx].size)? count : pipes[idx].size) * sizeof(char);
    unsigned int buff_idx = pipes[idx].idx;
    
    unsigned int i = 0;
    while (i < size) {
        buff[i] = pipes[idx].buffer[(buff_idx + i) % MAX_PIPE_BUFFER_SIZE];
        i++;
    }
    
    pipes[idx].idx = (buff_idx + i) % MAX_PIPE_BUFFER_SIZE;
    pipes[idx].size -= size;

    ksem_post(sem_read); // ya lei

    ksem_close(sem_read);
    ksem_close(sem_write); 

    return size;
}

int getPipeInIndex(char * name) {
    if (name == NULL || *name == '\0')
        return -1;

    int idx = getPipeIndex(name);
    if (idx < 0) {
        idx = openPipe(name);
        if (idx < 0)
            return -1;
    }
    
    return 2*idx;
}

int getPipeOutIndex(char * name) {
    if (name == NULL || *name == '\0')
        return -1;

    int idx = getPipeIndex(name);
    if (idx < 0) {
        idx = openPipe(name);
        if (idx < 0)
            return -1;
    }

    return 2*idx + 1;
}

static int getPipeIndex(char * name) {
    if (name == NULL || *name == '\0')
        return -1;

    for(unsigned int i = 0; i < MAX_PIPES; i++) {
        if( (pipes[i].name)[0] != '\0' && strcmp(pipes[i].name, name) == 0 )
            return pipes[i].idx;
    }
    return -1; 
}

int openPipe(char * name) {
    if (name == NULL || *name == '\0')
        return -1;

    unsigned int i = 0;
    while (i < pipes_size && (pipes[i].name)[0] != '\0') {
        i++;
    }

    if (i >= MAX_PIPES)
        return -1;

    strcpy(pipes[i].name, name);
    pipes[i].size = 0;
    pipes[i].idx = 0;
    (pipes[i].buffer)[0] = '\0';

    if (i == pipes_size)
        pipes_size++;
    pipes_amount++;

    return i;
}