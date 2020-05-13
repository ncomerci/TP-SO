#include <stdint.h>

static uint64_t my_create_process(char * name){
  /*uint16_t pid; 
  main_func_t test_ps = {endless_loop, 0, NULL};
  createProcess( &test_ps,name, 0, NULL, NULL, &pid);
  return pid; */
}

static uint64_t my_sem_open(char *sem_id, uint64_t initialValue){

  return 0;
}

static uint64_t my_sem_wait(char *sem_id){
  return sem_wait(sem_id);
}

static uint64_t my_sem_post(char *sem_id){
  return sem_post(sem_id);
}

static uint64_t my_sem_close(char *sem_id){
  return sem_close(sem_id);
}

#define N 100000000
#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

uint64_t global;  //shared memory

static void slowInc(uint64_t *p, uint64_t inc){
  uint64_t aux = *p;
  aux += inc;
  *p = aux;
}

static void my_process_inc(){
  uint64_t i;

  if (!my_sem_open(SEM_ID, 1)){
    printf("ERROR OPENING SEM\n");
    return;
  }
  
  for (i = 0; i < N; i++){
    my_sem_wait(SEM_ID);
    slowInc(&global, 1);
    my_sem_post(SEM_ID);
  }

  my_sem_close(SEM_ID);
  
  printf("Final value: %d\n", global);
}

static void my_process_dec(){
  uint64_t i;

  if (!my_sem_open(SEM_ID, 1)){
    printf("ERROR OPENING SEM\n");
    return;
  }
  
  for (i = 0; i < N; i++){
    my_sem_wait(SEM_ID);
    slowInc(&global, -1);
    my_sem_post(SEM_ID);
  }

  my_sem_close(SEM_ID);

  printf("Final value: %d\n", global);
}

static void test_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process("my_process_inc");
    my_create_process("my_process_dec");
  }
  
  // The last one should print 0
}

static void my_process_inc_no_sem(){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, 1);
  }

  printf("Final value: %d\n", global);
}

static void my_process_dec_no_sem(){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, -1);
  }

  printf("Final value: %d\n", global);
}

static void test_no_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process("my_process_inc_no_sem");
    my_create_process("my_process_dec_no_sem");
  }

  // The last one should not print 0
}

int main_test_sync(int argc, char ** argv){
  test_sync();
  test_no_sync();
  return 0;
}