#include <fd.h>

int processes_fds [MAX_PROCESSES][MAX_FILE_DES];

int assignInAndOut(int idx, char * in, char * out) {
    if (idx < 0 || idx >= MAX_PROCESSES)
        return -1;
    
    int gate = -1;
    
    if (in == NULL || *in == '\0')
        gate = STDIN;
    else {
        if ( ( gate = getPipeOutIndex(in) ) < 0 )
            return -1;
    }
    processes_fds[idx][0] = gate;
    if (out == NULL || *out == '\0')
        gate = STDOUT;
    else {
        if ( ( gate = getPipeInIndex(out) ) < 0 )
            return -1;
    }
    processes_fds[idx][1] = gate;
    return 0;
}

int sys_fd(void * option, void * arg1, void * arg2, void * arg3, void * arg4) {
    switch ((uint64_t) option) {
        case 0:
            return writeFD((int)(uint64_t) arg1, (char *) arg2, (unsigned int)(uint64_t) arg3, (int)(uint64_t) arg4); // (char * str, unsigned int str_size, int color )
            break;
        case 1:
            return readFD((int)(uint64_t) arg1, (char *) arg2, (unsigned int)(uint64_t) arg3); 
            break;
    }
    return 0;
}


int writeFD(int fd, char * str, unsigned int str_size, int color) {

    if(0 <= fd && fd < MAX_FILE_DES) {
        int idx = getCurrentIdx();
        int gate = processes_fds[idx][fd];
     
        if ( gate < 0) {
            if (gate == STDOUT)
                return printColorString(str, str_size, color);
            else //STDIN
                return -1;
        }
        else{
            return pipeWrite(gate, str, str_size); 
        }
    

    }
    
    return -1;
}

int readFD(int fd, char * buff, unsigned int count) {
    
    if(0 <= fd && fd < MAX_FILE_DES) {

        int idx = getCurrentIdx();
        int gate = processes_fds[idx][fd];
    
        if (gate < 0) {
            if (gate == STDIN)
                return kbRead(buff, count); 
            else //STDOUT
                return -1;
        }
        else{
            return pipeRead(gate, buff, count); 
        }
    }

    return -1; 
}
