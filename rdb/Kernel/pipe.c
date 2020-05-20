#include <pipe.h>
#include <process.h>
#include <lib.h>

pipe_t pipes[MAX_PIPES];
unsigned int pipes_size = 0;
unsigned int pipes_amount = 0;

int pipeWrite(int gate, char * str, unsigned int str_size) { //post
    if (gate % 2 != 0) // READ-END
        return -1;

    int idx = gate/2;

    if (idx < 0 || idx >= pipes_size)
        return -1;

    if (str_size == 0)
        return 0;

    char sem_aux_name[SEM_NAME_MAX_LENGTH];

    sprintf(sem_aux_name, "%s-%c", pipes[idx].name, 'w');
    sem_id sem_write = ksem_open(sem_aux_name); 
    if(sem_write == -1 )
        return -1; 

    sprintf(sem_aux_name, "%s-%c", pipes[idx].name, 'p');
    sem_id sem_prod = ksem_open(sem_aux_name); 
    if(sem_prod < 0)
        return -1;

    sprintf(sem_aux_name, "%s-%c", pipes[idx].name, 'c');
    sem_id sem_cons = ksem_open(sem_aux_name); 
    if(sem_cons < 0)
        return -1;
         
    ksem_wait(sem_write);  //si alguien esta escribiendo, no escribas
    
    unsigned int i = 0;
    unsigned int size = str_size; //tamaño que quiero escribir
    unsigned int size_left = size;
    unsigned int amount;

    spin_lock(&(pipes[idx].package_lock)); //me aseguro que nadie consulte el tamaño del paquete que voy a escribir
    unsigned int buff_idx = pipes[idx].idx;
    unsigned int buff_size = pipes[idx].size;
    spin_unlock(&(pipes[idx].package_lock)); 

    /*
    (holacomo)(es
    (holacomo)(estash
    (holacomo)(esholaco)(moestash)
    */

    unsigned int package_amount = (((buff_size + size) / PACKAGE_SIZE) - (buff_size / PACKAGE_SIZE));
    if ((buff_size == 0) && ((buff_size + size) % PACKAGE_SIZE != 0))
        package_amount += 1;

    for(unsigned int j = 0; j < package_amount; j++) {
        ksem_wait(sem_cons);

        amount = (size < PACKAGE_SIZE)?size:PACKAGE_SIZE;

        i = 0;
        while (i < amount) {
            pipes[idx].buffer[(buff_idx + buff_size + i) % MAX_PIPE_BUFFER_SIZE] = str[i];
            i++;
        }
        size_left -= amount;

        spin_lock(&(pipes[idx].package_lock));
        pipes[idx].size += amount;
        spin_unlock(&(pipes[idx].package_lock));

        ksem_post(sem_prod);
    }

    ksem_post(sem_write); // avisa que terminaste de escribir

    ksem_close(sem_prod);
    ksem_close(sem_cons);    
    ksem_close(sem_write);

    return 0; 
}

int pipeRead(int gate, char * buff, unsigned int count) { //sleep
    if (gate % 2 == 0) // WRITE-END
        return -1;

    int idx = gate/2; 

    if (idx < 0 || idx >= pipes_size)
        return -1;

    if (count == 0)
        return 0;

    char sem_aux_name[SEM_NAME_MAX_LENGTH];

    sprintf(sem_aux_name, "%s-%c", pipes[idx].name, 'r');
    sem_id sem_read = ksem_open(sem_aux_name); 
    if(sem_read == -1 )
        return -1;

    sprintf(sem_aux_name, "%s-%c", pipes[idx].name, 'p');
    sem_id sem_prod = ksem_open(sem_aux_name); 
    if(sem_prod < 0)
        return -1;

    sprintf(sem_aux_name, "%s-%c", pipes[idx].name, 'c');
    sem_id sem_cons = ksem_open(sem_aux_name); 
    if(sem_cons < 0)
        return -1;

    ksem_wait(sem_read); // Si alguien esta leyendo, espera a que termine. Cuando me desbloquea leo
    
    spin_lock(&(pipes[idx].package_lock)); //me aseguro que nadie consulte el tamaño del paquete que voy a escribir
    unsigned int buff_idx = pipes[idx].idx;
    unsigned int size = ((count < pipes[idx].size)? count : pipes[idx].size) * sizeof(char);
    spin_unlock(&(pipes[idx].package_lock)); 

    unsigned int i = 0;
    unsigned int size_left = size;
    unsigned int amount;

    /*
		(holacomo)(es
		(holacomo)(estash
		(holacomo)(esholaco)(moestash)
    */

    unsigned int package_amount = (size % PACKAGE_SIZE == 0)?(size / PACKAGE_SIZE):((size / PACKAGE_SIZE) + 1);

    for(unsigned int j = 0; j < package_amount; j++) {
        ksem_wait(sem_prod);

        amount = (size_left < PACKAGE_SIZE)?size_left:PACKAGE_SIZE;

        i = 0;
        while (i < amount) {
            buff[i] = pipes[idx].buffer[(buff_idx + i) % MAX_PIPE_BUFFER_SIZE];
            i++;
        }
        
        size_left -= amount;

        spin_lock(&(pipes[idx].package_lock));
        pipes[idx].idx = (buff_idx + i) % MAX_PIPE_BUFFER_SIZE; 

        pipes[idx].size -= amount;

        if (((pipes[idx].size)/PACKAGE_SIZE) == ((pipes[idx].size + amount)/PACKAGE_SIZE))
            ksem_post(sem_prod);
        else
            ksem_post(sem_cons);

        spin_unlock(&(pipes[idx].package_lock));
    }


    /*
    spin_lock(&(pipes[idx].package_lock)); //me aseguro que nadie consulte el tamaño del paquete que voy a escribir
    if (pipes[idx].size == 0) {
        spin_unlock(&(pipes[idx].package_lock));
        ksem_wait(sem_prod);
        spin_lock(&(pipes[idx].package_lock));
    }

    unsigned int buff_idx = pipes[idx].idx;
    unsigned int size = ((count < pipes[idx].size)? count : pipes[idx].size) * sizeof(char);
    spin_unlock(&(pipes[idx].package_lock)); 
    
    unsigned int i = 0;
    while (i < size) {
        buff[i] = pipes[idx].buffer[(buff_idx + i) % MAX_PIPE_BUFFER_SIZE];
        i++;
    }
    
    spin_lock(&(pipes[idx].package_lock));
    pipes[idx].idx = (buff_idx + i) % MAX_PIPE_BUFFER_SIZE;
    // POST SEMAPHORES CONSUMER 
    pipes[idx].size -= size;
    spin_unlock(&(pipes[idx].package_lock));

    */
   
    ksem_post(sem_read); // ya lei

    ksem_close(sem_prod);
    ksem_close(sem_cons);
    ksem_close(sem_read);

    return size;
}

int getPipeInIndex(char * name, uint64_t pid) {
    if (name == NULL || *name == '\0')
        return -1;

    int idx = openPipe(name, pid);
    if (idx < 0)
        return -1;
    
    return 2*idx;
}

int getPipeOutIndex(char * name, uint64_t pid) {
    if (name == NULL || *name == '\0')
        return -1;

    int idx = openPipe(name, pid);
    if (idx < 0)
        return -1;

    return 2*idx + 1;
}

int openPipe(char * name, uint64_t pid) {
    if (name == NULL || *name == '\0')
        return -1;

    unsigned int i = 0;

    while (i < pipes_size && pipes[i].name[0] != '\0') {
        if (strcmp(pipes[i].name, name) == 0)
        {
            unsigned int j = 0;
            while (j < pipes[i].processes_size && pipes[i].processes[j].occupied)
            {
                if (pipes[i].processes[j].pid == pid) // Process already opened this pipe
                    return -1;
                j++;
            }
            if (j >= MAX_PROCESSES_PER_PIPE)
            {
                return -1;
            }
            if (j == pipes[i].processes_size)
                pipes[i].processes_size++;
            pipes[i].processes[j].pid = pid; // Add pid to the processes appended to this pipe
            pipes[i].processes[j].occupied = 1;
            pipes[i].processes_amount++;
            return i;
        }
        i++;
    }
    if (i < MAX_PIPES) {
        strcpy(pipes[i].name, name);
        pipes[i].buffer[0] = '\0';
        pipes[i].package_lock = 0;
        pipes[i].processes[0].pid = pid;
        pipes[i].processes[0].occupied = 1;        
        pipes[i].processes_amount = 1;
        pipes[i].processes_size = 1;

        char sem_aux_name[SEM_NAME_MAX_LENGTH];

        sprintf(sem_aux_name, "%s-%c", pipes[i].name, 'p');
        sem_id sem_prod = ksem_init_open_priv(sem_aux_name, 0); 
        if(sem_prod < 0)
            return -1;
        pipes[i].sem_producer = sem_prod;

        sprintf(sem_aux_name, "%s-%c", pipes[i].name, 'c');
        sem_id sem_cons = ksem_init_open_priv(sem_aux_name, MAX_PIPE_BUFFER_SIZE / PACKAGE_SIZE); 
        if(sem_cons < 0)
            return -1;
        pipes[i].sem_consumer = sem_cons;

        if (i == pipes_size)
            pipes_size++;
        pipes_amount++;
        return i;
    }
    else
        return -1;
}

int closePipe(int gate, uint64_t pid) {
    int idx = gate/2; 

    if (idx < 0 || idx >= pipes_size)
        return -1;

    unsigned int i = 0;
    while (i < pipes[idx].processes_size)
    {
        if (pipes[idx].processes[i].occupied && pipes[idx].processes[i].pid == pid)
        {
            pipes[idx].processes[i].occupied = 0;

            if (i == pipes[idx].processes_size - 1)
                pipes[idx].processes_size--;
                
            pipes[idx].processes_amount--;
           
            if (pipes[idx].processes_amount == 0)
            { // delete pipe
                pipes[idx].name[0] = '\0';
                if (idx == pipes_size - 1)
                    pipes_size--;
                pipes_amount--;

                if (pipes_size == 0) {
                    ksem_destroy_priv(pipes[idx].sem_producer);
                    ksem_destroy_priv(pipes[idx].sem_consumer);
                }
                    
            }
            return 0;
        }
        i++;
    }
    return -1;
}

void printPipes( pipe_info * arr , uint64_t * size){
    unsigned int j = 0 ; 

    for(unsigned int i = 0; i < pipes_amount; i++){
        //arr[j].blocked_processes = pipes[i].blocked_processes;
        strcpy(arr[j].name, pipes[i].name);   
        j++;
    }
    
    *size = j; 

}