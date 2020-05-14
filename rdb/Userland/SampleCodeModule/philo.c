#include <philo.h>
#include <test_util.h>
#include <lib_user.h>

static int viewer_main(int argc, char ** argv);
static int equals(int * v1, int * v2, unsigned int size);
static int checkState(int * state, unsigned int size);
static void think();
static void eat();
static int take_sticks( philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem);
static int drop_sticks( philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem);
static void initializePhilos(philo_t * philos, unsigned int n_philos);
static int addPhilo(philos_info_t * info);
static int removePhilo(philos_info_t * info);


// How a philosopher thinks.
static void think() {
    wait(GetUniform(2000));
}

// How a philosopher eats.
static void eat(state_t * state, sem_id state_lock, sem_id viewer_sem) {
    wait(GetUniform(1000));
}

static int take_sticks(philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem) { 

    if (philo->table_pos % 2 == 0) {
        //first take right stick
        sem_wait(right);  //wait for it to be free
        philo->hands.right = right; 
        //printf("Philo %d take right stick (stick sem_id n: %d)\n", philo->table_pos, right);

        wait(GetUniform(1000));

        //then take left stick

        sem_wait(left);

        sem_wait(state_lock);
        philo->state = EATING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        philo->hands.left = left;
        //printf("Philo %d take left stick (stick sem_id n: %d)\n", philo->table_pos, left);
        
    }else{
        //first take left stick 
        sem_wait(left);
        philo->hands.left = left; 
        //printf("Philo %d take left stick (stick sem_id n: %d)\n", philo->table_pos, left);

        wait(GetUniform(1000));

        //then take right stick 
        sem_wait(right);  //wait for it to be free

        sem_wait(state_lock);
        philo->state = EATING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        philo->hands.right = right; 
        //printf("Philo %d take right stick (stick sem_id n: %d)\n", philo->table_pos, right);
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
 
static int drop_sticks(philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem){
    
    if (philo->table_pos % 2 == 0) {
        philo->hands.left = -1;

        sem_wait(state_lock);

        sem_post(left);

        philo->state = THINKING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        //printf("Philo %d drop left stick (stick sem_id n: %d)\n", philo->table_pos, left);

        wait(GetUniform(1000));

        philo->hands.right = -1;
        sem_post(right);
        //printf("Philo %d drop right stick (stick sem_id n: %d)\n", philo->table_pos, right);
    }
    else {
        philo->hands.right = -1;

        sem_wait(state_lock);

        sem_post(right);

        philo->state = THINKING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        //printf("Philo %d drop right stick (stick sem_id n: %d)\n", philo->table_pos, right);

        wait(GetUniform(1000));

        philo->hands.left = -1;
        sem_post(left);
        //printf("Philo %d drop left stick (stick sem_id n: %d)\n", philo->table_pos, left);
    }   

    return 0;
}

static int addPhilo(philos_info_t * info){
    /*philos++ sticks++ 
    robo un palito 
    createPhilo 

    */

    return 0;
}

static int removePhilo(philos_info_t * info){
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

    sem_id state_lock = sem_open("state_lock");
    sem_id viewer_sem = sem_open("philo_viewer");

    sscanf(argv[0], "%p", &me);
    sscanf(argv[1], "%p", &sticks);
    sscanf(argv[2], "%p", &n_philos);

    while(1){

        n = *n_philos;
        //printf("Philo %d is hungry\n", me->table_pos);
        sprintf(left_stick_name, "Stick %d", (me->table_pos) % n);
        sprintf(right_stick_name, "Stick %d", ((me->table_pos) + 1) % n);

        left_stick_sem = sem_open(left_stick_name);
        right_stick_sem = sem_open(right_stick_name);

        take_sticks(me, left_stick_sem, right_stick_sem, state_lock, viewer_sem);
        //printf("Philo %d is going to eat\n", me->table_pos);
        eat(&(me->state), state_lock, viewer_sem);
        drop_sticks(me, left_stick_sem, right_stick_sem, state_lock, viewer_sem);
        //printf("Philo %d is going to think\n", me->table_pos); 
        think(&(me->state), state_lock, viewer_sem);
        sem_close(left_stick_sem);
        sem_close(right_stick_sem);
    }

    return 0;
}

int thinking_philos_main(int argc, char ** argv) {

    philos_info_t info;
    info.finished = 0;
    char ** args[MAX_PHILOS];
    int c;

    sscanf(argv[0], "%d", &(info.n_philos));

    initializePhilos(info.philos, info.n_philos);
    
    for (unsigned int i = 0; i < info.n_philos; i++) {
        args[i] = malloc(4 * sizeof(char *));
        args[i][0] = malloc(MAX_ARG_LENGTH * sizeof(char));
        args[i][1] = malloc(MAX_ARG_LENGTH * sizeof(char));
        args[i][2] = malloc(MAX_ARG_LENGTH * sizeof(char));
        args[i][3] = NULL;
    }

    main_func_t f_aux = {philo_main, 2, NULL};

    sem_id state_lock = sem_open("state_lock");
    sem_id viewer_sem = sem_open("philo_viewer");
    uint64_t viewer_pid;

    char ** viewer_args = malloc(2 * sizeof(char *));
    viewer_args[0] = malloc(MAX_ARG_LENGTH * sizeof(char));

    main_func_t f_viewer = {viewer_main, 1, viewer_args};

    sprintf(viewer_args[0], "%p", &info);

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

    createProcess(&f_viewer, "Philo Viewer", 0, NULL, NULL, &viewer_pid);

    while ((c = scanChar()) != ESC) {
        if (c == 'a')
            addPhilo(&info);
        else if (c == 'r')
            removePhilo(&info);
        else if (c == 'x')
            for(unsigned int i = 0; i < info.n_philos; i++)
                kill(info.philos[i].pid);
    }
    //finish process 

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

    for (unsigned int i = 0; i < info.n_philos; i++)
        sem_destroy(info.sticks[i]);

    sem_destroy(viewer_sem);
    sem_destroy(state_lock);

    return 0;
}

static void initializePhilos(philo_t * philos, unsigned int n_philos) {
    for(unsigned int i = 0; i < n_philos; i++) {
        philos[i].hands.left = -1;
        philos[i].hands.right = -1;
        philos[i].pid = -1;
        philos[i].state = THINKING;
        philos[i].table_pos = i;
    }
}

static int viewer_main(int argc, char ** argv) {
    philos_info_t * info;
    //int sval;
    sscanf(argv[0], "%p", &info);

    unsigned int i;
    unsigned int n;
    int states[2][MAX_PHILOS] = {{0}, {0}};
    int actual_state = 0;
    int * last_state;
    int * state;

    sem_id viewer_sem = sem_init_open("philo_viewer", 0);
    sem_id state_lock = sem_open("state_lock");
     
    while(1) {
        sem_wait(viewer_sem); //in case a philo is changing state

        n = info->n_philos;
        last_state = states[actual_state];
        state = states[1 - actual_state];

        sem_wait(state_lock);

        for(i = 0; i < info->n_philos; i++)
            state[i] = (info->philos)[i].state == EATING;
        
        sem_post(state_lock);

        if (!equals(state, last_state, n) == 0) {
            if (checkState(state, n) != 0)
                printf("An error ocurred. Two processes eating together!\n");
            for(i = 0; i < info->n_philos; i++)
                printf("%s ", (state[i])?"E":".");
            printf("\n");
            actual_state = 1 - actual_state;
        }

        /*
        for (i = 0; i < info->n_philos; i++) {
            wait()
        }

        for(i = 0; i < info->n_philos; i++)
            states[i] = ((sem_getvalue((info->sticks)[i % info->n_philos], &sval) == 0) && (sem_getvalue((info->sticks)[(i + 1) % info->n_philos], &sval) == 0));
        sem_post(state_lock);

        for(i = 0; i < info->n_philos; i++)
            printf("%s ", (states[i])?"E":".");
        printf("\n");
        */

        //((sem_getvalue((info->sticks)[i % info->n_philos], &sval) == 0) && (sem_getvalue((info->sticks)[(i + 1) % info->n_philos], &sval) == 0))
    }

    return 0;
    
}

static int equals(int * v1, int * v2, unsigned int size) {
    for (int i = 0; i < size; i++)
        if (v1[i] - v2[i] != 0)
            return v1[i] - v2[i];
    return 0;
}

static int checkState(int * state, unsigned int size) {
    for (int i = 0; i < size; i++)
        if (state[i] && state[(i+1)%size])
            return -1;
    return 0;
}