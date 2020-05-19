#ifndef _PHI_H
#define _PHI_H

    #include <lib_user.h>
    #include <sem.h> 

    #define INITIAL_PHILOS 5
    #define MIN_PHILOS 2
    #define MAX_PHILOS 35    // How many philosophers are dining tonight.
    #define PHILO_NAME_MAX_LENGTH 30
    #define MAX_ARG_LENGTH 50

    // Used for graphing
    #define SCREEN_WIDTH 1024
    #define SCREEN_HEIGHT 768

    #define TABLE_X (SCREEN_WIDTH / 2)
    #define TABLE_Y (SCREEN_HEIGHT / 2)

    #define TABLE_RADIUS 200
    #define TABLE_PERIMETER ( (2 * TABLE_RADIUS * 355) / 113 )

    #define TABLE_COLOR 0xDCB484

    #define HUNGRY_PHILO_COLOR 0xFF3D41
    #define EATING_PHILO_COLOR 0x50C878
    #define THINKING_PHILO_COLOR 0xFCF787
    
    // The philosophers are arranged around a circular table. These macros compute the index of the
    // philosopher on the left and right respectively of the given philosopher.
    //

    // The three things that philosophers do.
    typedef enum { THINKING, EATING, HUNGRY } state_t;
    typedef enum { STARTER, NEW } origin;

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
        char ** args[MAX_PHILOS];
    }philos_info_t;

    int thinking_philos_main(int argc, char ** argv);
#endif