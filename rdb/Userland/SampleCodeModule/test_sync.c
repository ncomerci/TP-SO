#include <stdint.h>
#include <sem.h>
#include <lib_user.h> 

static uint64_t my_create_process(int (f)(int, char**), char * name){
  uint64_t pid = 0; 
  main_func_t f_ps = {f, 0, NULL};
  int aux = createProcess( &f_ps, name, 0, NULL, NULL, &pid); //CHECKEAR ERROR!!!!!
  if(aux < 0) {
    printError("ERROR: createProcess!\n");
  }
  return pid;
}

static uint64_t my_sem_open(char *name, uint64_t initialValue){
  return sem_init_open(name, initialValue);
}

static uint64_t my_sem_wait(sem_id sem_id){
  int ret = sem_wait(sem_id);
  if (ret < 0)
    printf("Se rompio todo el wait amigo\n");
  return ret;
}

static uint64_t my_sem_post(sem_id sem_id){
  int ret = sem_post(sem_id);
  if (ret < 0)
    printf("Se rompio todo el post amigo\n");
  return ret;  
} 

static uint64_t my_sem_close(sem_id sem_id){
  int ret = sem_close(sem_id);
  if (ret < 0)
    printf("Se rompio todo el close amigo\n");
  return ret;  
}

#define N 1000000
#define SEM_NAME "sem"
#define TOTAL_PAIR_PROCESSES 2

uint64_t global;  //shared memory

static void slowInc(uint64_t *p, int64_t inc){
  uint64_t aux = *p;
  aux += inc;
  for(int i = 0; i < 100 ; i++);
  *p = aux;
}

static int my_process_inc(int argc, char ** argv){
  uint64_t i;
  sem_id sem;

  if ((sem = my_sem_open(SEM_NAME, 1)) < 0){
    printf("ERROR OPENING SEM\n");
    return -1;
  }
  
  for (i = 0; i < N; i++){
    my_sem_wait(sem);
    slowInc(&global, 1);
    my_sem_post(sem);
  }

  my_sem_close(sem);
  
  printf("SEM Final value: %d\n", global);

  return 0;
}

static int my_process_dec(int argc, char ** argv){
  uint64_t i;
  sem_id sem; 

  if ((sem = my_sem_open(SEM_NAME, 1)) < 0){
    printf("ERROR OPENING SEM\n");
    return -1;
  }
  
  for (i = 0; i < N; i++){
    my_sem_wait(sem);
    slowInc(&global, -1);
    my_sem_post(sem);
  }

  my_sem_close(sem);

  printf("SEM Final value: %d\n", global);

  return 0;
}

static void test_sync(){
  uint64_t i;
  
  global = 0;

  printf("CREATING PROCESSES...\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process(my_process_inc, "my_process_inc");
    my_create_process(my_process_dec,"my_process_dec");
  }
  
  // The last one should print 0
}

static int my_process_inc_no_sem(int argc, char ** argv){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, 1);
  }

  printf("NO Final value: %d\n", global);

  return 0;
}

static int my_process_dec_no_sem(int argc, char ** argv){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, -1);
  }

  printf("NO Final value: %d\n", global);

  return 0;
}

static void test_no_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process(my_process_inc_no_sem, "my_process_inc_no_sem");
    my_create_process(my_process_dec_no_sem, "my_process_dec_no_sem");
  }

  // The last one should not print 0
}

int main_test_sync(int argc, char ** argv){
  test_sync();
  test_no_sync();
  return 0;
}