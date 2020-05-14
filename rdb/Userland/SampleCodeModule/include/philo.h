#ifndef _PHI_H
#define _PHI_H

    #include <lib_user.h>
    #include <sem.h> 

    #define MAX_PHILOS 35    // How many philosophers are dining tonight.
    #define PHILO_NAME_MAX_LENGTH 30
    #define MAX_ARG_LENGTH 50
    
    // The philosophers are arranged around a circular table. These macros compute the index of the
    // philosopher on the left and right respectively of the given philosopher.
    //

    // The three things that philosophers do.
    typedef enum { THINKING, EATING, HUNGRY } state_t;

    typedef struct hands_t {
        sem_id left;
        sem_id right;
    }hands_t;
   
    typedef struct philo_t{
        state_t state; 
        hands_t hands; 
        uint64_t pid; 
        int table_pos; 
    }philo_t;

    typedef struct philos_info_t{
        sem_id sticks[MAX_PHILOS];
        unsigned int n_philos; 
        philo_t philos[MAX_PHILOS];
        int finished;
    }philos_info_t;

    int thinking_philos_main(int argc, char ** argv);
#endif