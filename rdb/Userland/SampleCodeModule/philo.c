#include <philo.h>
#include <test_util.h>
#include <lib_user.h>

static void think(state_t * state);
static void eat(state_t * state);
static int take_sticks( philo_t * philo, sem_id left, sem_id right );
static int drop_sticks( philo_t * philo, sem_id left, sem_id right );

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


static int take_sticks( philo_t * philo, sem_id left, sem_id right) {
    philo->state = HUNGRY;

    if (philo_number % 2 == 0) {
        //first take right stick
        sem_wait(right);  //wait for it to be free
        philo->hands.right = right; 

        //then take left stick
        sem_wait(left);
        philo->hands.left = left; 
        
    }else{
        //first take left stick 
        sem_wait(left);
        philo->hands.left = left; 
        //then take right stick  
        sem_wait(right );  //wait for it to be free
        philo->hands.right = right; 
    }
    
    return 0;    
}
 
static int take(int idx, sem_id * sticks, sem_id * hand){

    if(sem_wait(sticks[idx]) == -1) {
        return -1;
    } 
    *hand = idx; 

    return 0;
}
 
static int drop_sticks(philo_t * philo, sem_id left, sem_id right){

    philo->hands.left = -1;
    sem_post(philo->hands.left);
 
    philo->hands.right = -1;
    sem_post(philo->hands.right);   

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
    sem_id * sticks;
    unsigned int * n_philos;
    unsigned int n;

    sem_id left_stick_sem, right_stick_sem;

    char left_stick_name[SEM_NAME_MAX_LENGTH];
    char right_stick_name[SEM_NAME_MAX_LENGTH];

    sscanf(argv[0], "%p", &me);
    sscanf(argv[1], "%p", &sticks);
    sscanf(argv[2], "%p", &n_philos);

    while(1){

        n = *n_philos;

        ssprintf(left_stick_name, "Stick %d", (me->table_pos) % n);
        ssprintf(right_stick_name, "Stick %d", ((me->table_pos) + 1) % n);

        left_stick_sem = sem_open(left_stick_name);
        right_stick_sem = sem_open(right_stick_name);

        take_sticks(&me, left_stick_sem, right_stick_sem);
        eat(&(me->state));
        drop_sticks(&me, left_stick_sem, right_stick_sem);
        think(&(me->state));
    }

    return 0;
}

static int thinking_philos_main(int argc, char ** argv) {

    static philos_info_t info;
    info.finished = 0;
    char ** args[MAX_PHILOS];
    int c;
    
    for (unsigned int i = 0; i < info.n_philos; i++) {
        args[i] = malloc(4 * sizeof(char *));
        args[i][0] = malloc(MAX_ARG_LENGTH * sizeof(char));
        args[i][1] = malloc(MAX_ARG_LENGTH * sizeof(char));
        args[i][2] = malloc(MAX_ARG_LENGTH * sizeof(char));
        args[i][3] = NULL;
    }

    main_func_t f_aux = {philo_main, 2, NULL};

    for (unsigned int i = 0; i < info.n_philos; i++) {
        char sem_name[SEM_NAME_MAX_LENGTH];
        sprintf(sem_name, "Stick %d", i);
        info.sticks[i] = sem_init_open(sem_name, 1);
    }

    for (unsigned int i = 0; i < info.n_philos; i++) {
        char philo_name[PHILO_NAME_MAX_LENGTH];
        sprintf(philo_name, "Philo %d", i);
        sprintf(args[i][0], "%p", &(info.philos[i]));
        sprintf(args[i][1], "%p", info.sticks);
        sprintf(args[i][2], "%p", &(info.n_philos));
        f_aux.argv = args[i];
        createProcess(&f_aux, philo_name, 0, NULL, NULL, &info.philos[i].pid);
    }

    sem_id viewer_sem = sem_open("philo_viewer");
    uint64_t viewer_pid;

    char ** viewer_args = malloc(2 * sizeof(char *));
    viewer_args[0] = malloc(MAX_ARG_LENGTH * sizeof(char));

    main_func_t f_viewer = {viewer_main, 1, NULL};

    sprintf(viewer_args[0], "%p", &info);

    createProcess(&f_viewer, "Philo Viewer", 0, NULL, NULL, &viewer_pid);

    while ((c = scanChar()) != ESC) {
        if (c == 'a')
            addPhilo();
        else if (c == 'r')
            removePhilo();
    }
    //finish process 

    info.finished = 1;
    post(viewer_sem); //viewer : revivite 
    wait(viewer_sem); //le doy tiempo a que se reviva ( if info->finished)

    sem_close(viewer_sem);
    kill(viewer_pid);

    free(viewer_args[0]);
    free(viewer_args);

    for (unsigned int i = 0; i < info.n_philos; i++)
        kill(info.philos[i].pid);

    for (unsigned int i = 0; i < info.n_philos; i++) {
        free(args[i][0]);
        free(args[i][1]);
        free(args[i][2]);
        free(args[i]);
    }

    for (unsigned int i = 0; i < info.n_philos; i++) {
        sem_close(info.sticks[i]);
    }

    return 0;
}

static int viewer_main(int argc, char ** argv) {
    philos_info_t * info;
    sscanf(argv[0], "%p", &info);   

    sem_id viewer_sem = sem_init_open("philo_viewer", 0);
     
    while(1) {
        sem_wait(viewer_sem); //in case a philo is changing state

        if (info->finished) {
            sem_post(viewer_sem);
            sem_close(viewer_sem);
            return 0;
        }
  
        for(unsigned int i = 0; i < info->n_philos; i++) {
            printf("%s ", ((info->philos)[i].state == EATING)?"E":".");
        }
        printf("\n");
    }

    return 0;
    
}