#include <lib_user.h>
#include <test_util.h>
#include <commands.h>

//TO BE INCLUDED
int endless_loop(int argc, char ** argv){
  while(1);
}

#define MAX_TESTED_PROCESSES 40 //Should be around 80% of the the processes handled by the kernel

static void test_processes(void);

typedef struct P_rq{
  uint64_t pid;
  process_state state;
}p_rq;

static void test_processes(){
  p_rq p_rqs[MAX_TESTED_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;

  while (1){

    // Create MAX_PROCESSES processes
    for(rq = 0; rq < MAX_TESTED_PROCESSES; rq++){
      main_func_t endfun = {endless_loop, 0, NULL};

      if (createProcess(&endfun, "Endless Loop", 0, NULL, NULL, &(p_rqs[rq].pid)) == -1){                           // TODO: Port this as required
        printf("Error creating process\n");               // TODO: Port this as required
        return;
      }else{
        p_rqs[rq].state = READY;
        alive++;
      }
    }

   

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){

      for(rq = 0; rq < MAX_TESTED_PROCESSES; rq++){
        action = GetUniform(2) % 2; 

        switch(action){
          case 0:
            if (p_rqs[rq].state == READY || p_rqs[rq].state == BLOCKED){
              if (kill(p_rqs[rq].pid) == -1){          // TODO: Port this as required
                printf("Error killing process\n");        // TODO: Port this as required
                return;
              }
              p_rqs[rq].state = KILLED; 
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == READY){
              if(changeState(p_rqs[rq].pid, BLOCKED) == -1){          // TODO: Port this as required
                printf("Error blocking process\n");       // TODO: Port this as required
                return;
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_TESTED_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2){
          if(changeState(p_rqs[rq].pid, READY) == -1){            // TODO: Port this as required
            printf("Error unblocking process\n");         // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = READY; 
        }
    }
  }
}

int main_test_process(int argc, char ** argv){
  test_processes();
  return 0;
}
