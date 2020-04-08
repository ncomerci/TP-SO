#ifndef _ARCND_H
#define _ARCND_H

#include <stdint.h>

#define FPS 60

#define BALL_RADIUS 10
#define BALL_COLOR 0xFFA454

#define BRICK_HEIGHT 32 //With a resolution of 768 for instance, we can fit 24 bricks in a col - we are not using that many bricks - 
#define BRICK_WIDTH 64  //With a resolution of 1024 for instance, we can fit 16 bricks in a row

#define STICK_HEIGHT_FROM_FLOOR 40 // Check before that you have enough space to set it (0.4 * SCREEN_HEIGHT > STICK_HEIGHT_FROM_FLOOR) && (STICK_HEIGHT_FROM_FLOOR >= STICK_HEIGHT)

#define STICK_HEIGHT 15
#define STICK_WIDTH 81
#define STICK_COLOR 0xFF4E4E

#define STICK_BALL_RELATIVITY 5

#define STARTING_VELOCITY 2 // Pixels per frame_time
#define MAX_VELOCITY 6

#define STARTING_LIVES 3

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define SCREEN_HEIGHT_PCTG_BLOCKS 60 //Sets which percentage of screen can be occuppied by bricks

#define BRICKS_PER_ROW ( SCREEN_WIDTH / BRICK_WIDTH )
#define BRICKS_PER_COL ( ( (SCREEN_HEIGHT * SCREEN_HEIGHT_PCTG_BLOCKS) / 100 ) / BRICK_HEIGHT )
#define N_BRICKS ( BRICKS_PER_ROW * BRICKS_PER_COL )

#define STICK_INDEX N_BRICKS
#define BALL_INDEX -2

#define SPEEDING_SOUND_FREQ 1500
#define SPEEDING_TIME 40

typedef struct point2d {
    int x;
    int y;
} point2d;

typedef struct movement {
    int movingDirX; // Possible values are -1 or 1, depending on the x direction of the entity.
    int movingDirY; // Possible values are -1 or 1, depending on the y direction of the entity.
    int velocity; // Adjusted through speeding frame rate.
} movement;

typedef struct entity {
    point2d point;
    movement mov;
} entity;

typedef struct gameState {
    unsigned long seconds_elapsed;
    int remaining_lives;
    int shooted;
    int bricks[N_BRICKS];     // Brick´s lives matrix.
    entity ball;
    entity stick;
} gameState;

void aracnoid(gameState * save_file, int * saved); // Should it return a save file?
void updater(void);

#endif