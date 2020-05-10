#include <philo.h>
#include <test_util.h>
#include <lib_user.h>

static void think(state_t * state);
static void eat(state_t * state);
static int take_sticks( philo_t * philo, unsigned int n_philos );
static int drop_sticks( philo_t * philo, unsigned int n_philos );

static sem_id sticks[MAX_PHILOS];

// How a philosopher thinks.
static void think(state_t * state) {
    *state = THINKING; 
    wait(GetUniform(10000));
}

// How a philosopher eats.
static void eat(state_t * state) {
    *state = EATING;
    wait(GetUniform(5000));
}


static int take_sticks( philo_t * philo, unsigned int n_philos ) {
    philo->state = HUNGRY;
    int philo_number = philo->table_pos;
    unsigned int left_stick_idx = (philo_number) % n_philos; 
    unsigned int right_stick_idx = (philo_number + 1) % n_philos; 

    if (philo_number % 2 == 0) {
        //first take right stick
        
        sem_wait(sticks[right_stick_idx]);  //wait for it to be free
        philo->hands.right = right_stick_idx; 
        sem_wait(sticks[left_stick_idx]);
        philo->hands.left= left_stick_idx; 
        
    }else{
        //first take left stick
        sem_wait(sticks[left_stick_idx]);
        philo->hands.left= left_stick_idx;
        //then take right stick 
        sem_wait(sticks[right_stick_idx]);
        philo->hands.right = right_stick_idx;
    }
    
    return 0;    
}
 
static int take(int idx, sem_id * hand){

    if(sem_wait(sticks[idx]) == -1) {
        return -1;
    } 
    *hand = idx; 

    return 0;
}
 
static int drop_sticks( philo_t * philo, unsigned int n_philos ){
    int philo_number = philo->table_pos;
    unsigned int left_stick_idx = (philo_number) % n_philos; 
    unsigned int right_stick_idx = (philo_number + 1) % n_philos; 
    
    sem_post(sticks[left_stick_idx]);
    philo->hands.left = -1; 
    sem_post(sticks[right_stick_idx]);   
    philo->hands.right = -1;

    return 0;
}

static int addPhilo(){
    return 0;
}

static int removePhilo(){
    return 0;
}

static int philo_main(int argc, char ** argv) {
    philo_t * me;
    unsigned int * act_size;
    sscanf(argv[0], "%p", me);
    sscanf(argv[1], "%p", act_size);

    while(1){
        take_sticks(me, *act_size);
        eat(&(me->state));
        drop_sticks(me, *act_size);
        think(&(me->state));
    }

    return 0;
}

static int thinking_philos_main(int argc, char ** argv) {
    int n_philos; 
    char ** args[MAX_PHILOS];
    philo_t philos[MAX_PHILOS]; 
    int c;
    
    for (unsigned int i = 0; i < n_philos; i++) {
        args[i] = malloc(2 * sizeof(char *));
        args[i][0] = malloc(sizeof(char));
        args[i][1] = malloc(sizeof(char));
        args[i][2] = NULL;
    }

    main_func_t f_aux = {philo_main, 2, NULL};

    for (unsigned int i = 0; i < n_philos; i++) {
        char sem_name[SEM_NAME_MAX_LENGTH];
        sprintf(sem_name, "Stick %d", i);
        sticks[i] = sem_init_open(sem_name, 1);
    }

    for (unsigned int i = 0; i < n_philos; i++) {
        char philo_name[PHILO_NAME_MAX_LENGTH];
        sprintf(philo_name, "Philo %d", i);
        sprintf(args[i][0], "%p", (void *) &(philos[i]));
        sprintf(args[i][1], "%p", (void *) &n_philos);
        f_aux.argv = args;
        philos[i].pid = createProcess(&f_aux, philo_name, 0, NULL, NULL);
    }

    while ((c = scanChar()) != ESC) {
        if (c == 'a')
            addPhilo();
        else if (c == 'r')
            removePhilo();
    }

    for (unsigned int i = 0; i < n_philos; i++)
        kill(philos[i].pid);

    return 0;
}
 