/*
#include <philo.h>
#include <lib_user.h>
#include <sem.h> 
  
//BASE: https://www.geeksforgeeks.org/dining-philosopher-problem-using-semaphores/

#define N 5 
#define THINKING 2 
#define HUNGRY 1 
#define EATING 0 
#define LEFT (phnum + 4) % N 
#define RIGHT (phnum + 1) % N 
#define MAX_BUFF_SIZE 20  

int state[N]; 
int phil[N] = { 0, 1, 2, 3, 4 }; 
  
sem_t mutex; 
sem_t S[N]; 
  
void test(int phnum) 
{ 
    if (state[phnum] == HUNGRY 
        && state[LEFT] != EATING 
        && state[RIGHT] != EATING) { 
        // state that eating 
        state[phnum] = EATING; 
  
        wait(2); 
  
        printf("Philosopher %d takes fork %d and %d\n", 
                      phnum + 1, LEFT + 1, phnum + 1); 
  
        printf("Philosopher %d is Eating\n", phnum + 1); 
  
        // sem_post(&S[phnum]) has no effect 
        // during takefork 
        // used to wake up hungry philosophers 
        // during putfork 
        sem_post(&S[phnum]); 
    } 
} 
  
// take up chopsticks 
void take_fork(int phnum) 
{ 
  
    sem_wait(&mutex); 
  
    // state that hungry 
    state[phnum] = HUNGRY; 
  
    printf("Philosopher %d is Hungry\n", phnum + 1); 
  
    // eat if neighbours are not eating 
    test(phnum); 
  
    sem_post(&mutex); 
  
    // if unable to eat wait to be signalled 
    sem_wait(&S[phnum]); 
  
    wait(1); 
} 
  
// put down chopsticks 
void put_fork(int phnum) 
{ 
  
    sem_wait(&mutex); 
  
    // state that thinking 
    state[phnum] = THINKING; 
  
    printf("Philosopher %d putting fork %d and %d down\n", 
           phnum + 1, LEFT + 1, phnum + 1); 
    printf("Philosopher %d is thinking\n", phnum + 1); 
  
    test(LEFT); 
    test(RIGHT); 
  
    sem_post(&mutex); 
} 
  
int philospher(int argc, char ** argv) 
{ 
    void * aux_p;
    int* i;

    while (1) { 
  
        sscanf(argv[0], "%p", &aux_p);
        i = (int *) aux_p; 
  
        wait(1); 
  
        take_fork(*i); 
  
        //sleep(0); 
  
        put_fork(*i); 
    } 
} 
  
int main() 
{ 
  
    int i; 
    char aux_buff[MAX_BUFF_SIZE];
    main_func_t aux;
    aux.f = philospher;
    aux.argc = 1;
    aux.argv = malloc(2 * sizeof(char *));
    aux.argv[0] = malloc(1 * sizeof(char));

    
    int philo_pids[N]; 
  
    // initialize the semaphores
    sem_init_open(&mutex, 1); // Replace &mutex for a name
  
    for (i = 0; i < N; i++) 
  
        sem_init_open(&S[i], 0); // Replace &S[i] for a name 
  
    for (i = 0; i < N; i++) { 
  
        // create philosopher processes
        sprintf(aux_buff, "%p", &phil[i]); // We gotta make a sprintf
        strcpy(aux.argv[0], aux_buff);
        philo_pids[i] = createProcess(&aux, "Philosopher", 0, NULL, NULL); 
  
        printf("Philosopher %d is thinking\n", i + 1); 
    } 
  
    for (i = 0; i < N; i++) 
        pthread_join(philo_pids[i], NULL); // Que hace esto??? 
}
*/