#ifndef _PHI_H
#define _PHI_H

    #include <sem.h> 

    #define MAX_PHILOS 5    // How many philosophers are dining tonight.
    #define PHILO_NAME_MAX_LENGTH 30
    
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
        int pid; 
        int table_pos; 
    }philo_t;

  

#endif