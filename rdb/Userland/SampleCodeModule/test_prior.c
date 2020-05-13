#include <stdint.h>
#include <lib_user.h>
#include <commands.h>

#define MINOR_WAIT 1000000                               // TODO: To prevent a process from flooding the screen
#define WAIT      100000000                              // TODO: Long enough to see theese processes beeing run at least twice

static void bussy_wait(uint64_t n);
static int endless_loop(int argc, char ** argv);
static void test_prio(void);

static uint64_t my_getpid(){
  uint64_t pid; 
  getPid(&pid);  
  return pid;
}

static uint64_t my_create_process(char * name){
  uint64_t pid; 
  main_func_t test_ps = {endless_loop, 0, NULL};
  createProcess( &test_ps,name, 0, NULL, NULL, &pid);
  return pid; 
}

static uint64_t my_nice(uint64_t pid, uint64_t newPrio){
  return (changePriority(pid, (unsigned int) newPrio) == 0)? 0: 1;
}

static uint64_t my_kill(uint64_t pid){
  return (kill(pid) == 0)? 0: 1;
}

static uint64_t my_block(uint64_t pid){
  return (changeState(pid, BLOCKED) == 0)? 0: 1;
}

static uint64_t my_unblock(uint64_t pid){
  return (changeState(pid, READY) == 0)? 0: 1;
}

static void bussy_wait(uint64_t n){
  uint64_t i;
  for (i = 0; i < n; i++);
}

static int endless_loop(int argc, char ** argv){
  uint64_t pid = my_getpid();

  while(1){
    printf("%d ",pid);
    bussy_wait(MINOR_WAIT);
  }
  return 0;
}

#define TOTAL_PROCESSES 3

static void test_prio(void){
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for(i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = my_create_process("endless_loop");

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 0); //highest priority 
        break;
      case 1:
        my_nice(pids[i], 3); //medium priority
        break;
      case 2:
        my_nice(pids[i], 6); //low priority
        break;
    }
  }

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");
  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 6); //lowest priority 
        break;
      case 1:
        my_nice(pids[i], 3); //medium priority
        break;
      case 2:
        my_nice(pids[i], 0); //highest priority
        break;
    }
  }

  printf("UNBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_kill(pids[i]);
}

int main_test_prior(int argc, char ** argv){
  test_prio();
  return 0;
}