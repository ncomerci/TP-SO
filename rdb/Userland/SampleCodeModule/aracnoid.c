#include <aracnoid.h>
#include <lib_user.h>

#define BACKGROUND_COLOR 0x444444

// Pause Message
#define PAUSE_MENU_X 200
#define PAUSE_MENU_Y 300
#define PAUSE_MENU_WIDTH (SCREEN_WIDTH - 2 * PAUSE_MENU_X)
#define PAUSE_MENU_HEIGHT 200
#define PAUSE_FRAME_WIDTH 10

// Help Message
#define HELP_MESSAGE_X 375
#define HELP_MESSAGE_Y ((SCREEN_HEIGHT_PCTG_BLOCKS * SCREEN_HEIGHT) / 100) + 103
#define HELP_MESSAGE_WIDTH (SCREEN_WIDTH - 2 * HELP_MESSAGE_X)
#define HELP_MESSAGE_HEIGHT 40
#define HELP_MESSAGE_FRAME_WIDTH 5

// Lives and Secs Elapsed Message
#define LASE_MESSAGE_X 300
#define LASE_MESSAGE_Y (HELP_MESSAGE_Y + HELP_MESSAGE_HEIGHT + 10)
#define LASE_MESSAGE_WIDTH (SCREEN_WIDTH - 2 * LASE_MESSAGE_X)
#define LASE_MESSAGE_HEIGHT 40
#define LASE_MESSAGE_FRAME_WIDTH 5

#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8

#define BRICK_FRAME_WIDTH 5
#define STICK_FRAME_WIDTH 5

#define DARKER_PCTG 30
#define DARKER (DARKER_PCTG * 256 / 100)

#define SHADER ((DARKER << 16) + (DARKER << 8) + DARKER)

static void copyBackup(gameState * dst, gameState * src);
static int pauseMenu(gameState * save_file, int * saved);
static void showHelpMessage(void);
static void hideHelpMessage(void);
static void showInfoMessage(void);
static void hideInfoMessage(void);
static void finalMessage(void);
static void showFinalHelpMessage(void);
static void newGameState(void);
static void initGame(void);
static void setScene(void);
static void fillGameBoard(void);
static int * levelGenerator(int * bricks);
static int directionDecision(void);
static void move_stick(void);
static void move_ball(void);
static void check_and_move_ball(void);
static void updateBricks(void);
static void ball_fell_down(void);

static int game_board[SCREEN_HEIGHT][SCREEN_WIDTH];  // Game matrix, containing each pixel from each brick. (Each pixel from the same brick has the same number: the relative position of the brick)
static int last_bricks[N_BRICKS];
gameState gs;
int last_key;
static int player_won;
static int speed;
static int last_sec;
static int loaded_save;
static int showingInfoMessage;

uint32_t colors[] = {BACKGROUND_COLOR, 0xC8D6B9, 0x9DBAD5, 0x8FC1A9, 0x769ECB, 0x7CAA98}; // Index represents lives left.

void aracnoid(gameState * save_file, int * saved) { // Debería devolver un save file?

    clearScreen();
    if (*saved) {
        copyBackup(&gs, save_file);
        loaded_save = 1;
    }
    else
        newGameState();
    
    initGame();
    setScene();

    if (!loaded_save)
        addTimeFunction(updater, PIT_FREQUENCY / FPS); // (60 fps?) FALTA VELOCIDAD DE LA BARRITA EN EL UPDATE
    while (gs.remaining_lives && !player_won) {
        if (!loaded_save && !gs.shooted && !showingInfoMessage) {
            showHelpMessage();
            showInfoMessage();
            showingInfoMessage = 1;
        }
        if (loaded_save || (last_key = scanChar()) == ESC) {
            if (loaded_save)
                loaded_save = 0;
            else
                removeTimeFunction(updater);
            showInfoMessage();
            int aux = pauseMenu(save_file, saved);
            if (aux > 0)
                return;
            setScene();
            addTimeFunction(updater, PIT_FREQUENCY / FPS);
        }
        else if (!gs.shooted && last_key == ' ') {
            gs.ball.mov.movingDirX = directionDecision();
            gs.ball.mov.movingDirY = -1;
            gs.shooted = 1;
            last_sec = getSecondsElapsed();
            if (showingInfoMessage) {
                hideHelpMessage();
                hideInfoMessage();
                showingInfoMessage = 0;
            }
        }
    }
    removeTimeFunction(updater);
    finalMessage();
}

// Help Message
static void showHelpMessage(void) {
    setBackgroundColor(BACKGROUND_COLOR + 2 * SHADER);

    drawRectangle(HELP_MESSAGE_X, HELP_MESSAGE_Y, HELP_MESSAGE_WIDTH, HELP_MESSAGE_HEIGHT, BACKGROUND_COLOR + SHADER);
    drawRectangle(HELP_MESSAGE_X + HELP_MESSAGE_FRAME_WIDTH, HELP_MESSAGE_Y + HELP_MESSAGE_FRAME_WIDTH, HELP_MESSAGE_WIDTH - 2 * HELP_MESSAGE_FRAME_WIDTH, HELP_MESSAGE_HEIGHT - 2 * HELP_MESSAGE_FRAME_WIDTH, BACKGROUND_COLOR + 2 * SHADER);
    setCursor(HELP_MESSAGE_X + HELP_MESSAGE_FRAME_WIDTH + CHAR_WIDTH, HELP_MESSAGE_Y + HELP_MESSAGE_FRAME_WIDTH + CHAR_HEIGHT);
    printColored("     Press [SPACE] to SHOOT\n", 0x121212);

    setBackgroundColor(BLACK_COLOR);
}

static void hideHelpMessage(void) {
    drawRectangle(HELP_MESSAGE_X, HELP_MESSAGE_Y, HELP_MESSAGE_WIDTH, HELP_MESSAGE_HEIGHT, BACKGROUND_COLOR); 
}

// Lives and Secs Elapsed Message
static void showInfoMessage(void) {
    setBackgroundColor(BACKGROUND_COLOR + 2 * SHADER);

    drawRectangle(LASE_MESSAGE_X, LASE_MESSAGE_Y, LASE_MESSAGE_WIDTH, LASE_MESSAGE_HEIGHT, BACKGROUND_COLOR + SHADER);
    drawRectangle(LASE_MESSAGE_X + LASE_MESSAGE_FRAME_WIDTH, LASE_MESSAGE_Y + LASE_MESSAGE_FRAME_WIDTH, LASE_MESSAGE_WIDTH - 2 * LASE_MESSAGE_FRAME_WIDTH, LASE_MESSAGE_HEIGHT - 2 * LASE_MESSAGE_FRAME_WIDTH, BACKGROUND_COLOR + 2 * SHADER);
    setCursor(LASE_MESSAGE_X + LASE_MESSAGE_FRAME_WIDTH + CHAR_WIDTH, LASE_MESSAGE_Y + LASE_MESSAGE_FRAME_WIDTH + CHAR_HEIGHT);
    char buff[64];
    printColored("  Remaining lives: ", 0x121212);
    uintToBase(gs.remaining_lives, buff, 10);
    printColored(buff, 0x121212);
    printColored(" - Seconds elapsed: ", 0x121212);
    uintToBase(gs.seconds_elapsed, buff, 10);
    printColored(buff, 0x121212);

    setBackgroundColor(BLACK_COLOR);
}

static void hideInfoMessage(void) {   
    drawRectangle(LASE_MESSAGE_X, LASE_MESSAGE_Y, LASE_MESSAGE_WIDTH, LASE_MESSAGE_HEIGHT, BACKGROUND_COLOR);
}

// Final Message
static void finalMessage(void) {
    beeps(300);
    drawRectangle(PAUSE_MENU_X, PAUSE_MENU_Y, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, WHITE_COLOR - SHADER);
    drawRectangle(PAUSE_MENU_X + PAUSE_FRAME_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH, PAUSE_MENU_WIDTH - 2 * PAUSE_FRAME_WIDTH, PAUSE_MENU_HEIGHT - 2 * PAUSE_FRAME_WIDTH, WHITE_COLOR);
    setBackgroundColor(WHITE_COLOR);
    if (player_won) {
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 2);
        printColored("      $$\\      $$\\ $$$$$$\\ $$\\   $$\\ $$\\   $$\\ $$$$$$$$\\ $$$$$$$\\ \n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 3);
        printColored("      $$ | $\\  $$ |\\_$$  _|$$$\\  $$ |$$$\\  $$ |$$  _____|$$  __$$\\ \n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 4);
        printColored("      $$ |$$$\\ $$ |  $$ |  $$$$\\ $$ |$$$$\\ $$ |$$ |      $$ |  $$ |\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 5);
        printColored("      $$ $$ $$\\$$ |  $$ |  $$ $$\\$$ |$$ $$\\$$ |$$$$$\\    $$$$$$$  |\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 6);
        printColored("      $$$$  _$$$$ |  $$ |  $$ \\$$$$ |$$ \\$$$$ |$$  __|   $$  __$$< \n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 7);
        printColored("      $$$  / \\$$$ |  $$ |  $$ |\\$$$ |$$ |\\$$$ |$$ |      $$ |  $$ |\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 8);
        printColored("      $$  /   \\$$ |$$$$$$\\ $$ | \\$$ |$$ | \\$$ |$$$$$$$$\\ $$ |  $$ |\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 9);
        printColored("      \\__/     \\__|\\______|\\__|  \\__|\\__|  \\__|\\________|\\__|  \\__|\n", 0x00799F);
    }
    else {
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 2);
        printColored("      $$\\       $$$$$$\\   $$$$$$\\  $$$$$$$$\\ $$$$$$$\\  $$\\ $$\\ \n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 3);
        printColored("      $$ |     $$  __$$\\ $$  __$$\\ $$  _____|$$  __$$\\ $$ |$$ |\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 4);
        printColored("      $$ |     $$ /  $$ |$$ /  \\__|$$ |      $$ |  $$ |$$ |$$ |\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 5);
        printColored("      $$ |     $$ |  $$ |\\$$$$$$\\  $$$$$\\    $$$$$$$  |$$ |$$ |\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 6);
        printColored("      $$ |     $$ |  $$ | \\____$$\\ $$  __|   $$  __$$< \\__|\\__|\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 7);
        printColored("      $$ |     $$ |  $$ |$$\\   $$ |$$ |      $$ |  $$ |        \n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 8);
        printColored("      $$$$$$$$\\ $$$$$$  |\\$$$$$$  |$$$$$$$$\\ $$ |  $$ |$$\\ $$\\\n", 0x00799F);
        setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 9);
        printColored("      \\________|\\______/  \\______/ \\________|\\__|  \\__|\\__|\\__|\n", 0x00799F);        
    }
    
    setBackgroundColor(BLACK_COLOR);

    showFinalHelpMessage();

    while(scanChar() != ESC);
}

static void showFinalHelpMessage(void) {
    setBackgroundColor(BACKGROUND_COLOR + 2 * SHADER);

    drawRectangle(HELP_MESSAGE_X, HELP_MESSAGE_Y, HELP_MESSAGE_WIDTH, HELP_MESSAGE_HEIGHT, BACKGROUND_COLOR + SHADER);
    drawRectangle(HELP_MESSAGE_X + HELP_MESSAGE_FRAME_WIDTH, HELP_MESSAGE_Y + HELP_MESSAGE_FRAME_WIDTH, HELP_MESSAGE_WIDTH - 2 * HELP_MESSAGE_FRAME_WIDTH, HELP_MESSAGE_HEIGHT - 2 * HELP_MESSAGE_FRAME_WIDTH, BACKGROUND_COLOR + 2 * SHADER);
    setCursor(HELP_MESSAGE_X + HELP_MESSAGE_FRAME_WIDTH + CHAR_WIDTH, HELP_MESSAGE_Y + HELP_MESSAGE_FRAME_WIDTH + CHAR_HEIGHT);
    printColored("     Press [ESC] to EXIT\n", 0x121212);

    setBackgroundColor(BLACK_COLOR);
}

static int pauseMenu(gameState * save_file, int * saved) {
    drawRectangle(PAUSE_MENU_X, PAUSE_MENU_Y, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, WHITE_COLOR - SHADER);
    drawRectangle(PAUSE_MENU_X + PAUSE_FRAME_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH, PAUSE_MENU_WIDTH - 2 * PAUSE_FRAME_WIDTH, PAUSE_MENU_HEIGHT - 2 * PAUSE_FRAME_WIDTH, WHITE_COLOR);
    setBackgroundColor(WHITE_COLOR);
    setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 2);
    printColored("                             Pause Menu\n", 0x121212);
    setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 4);
    printColored("                 + Press [ESC] to resume.\n", 0x060606);
    setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 5);
    printColored("                 + Press [S] to save game and go to SHELL.\n", 0x060606);
    setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 6);
    printColored("                 + Press [E] to exit game.\n", 0x060606);
    setCursor(PAUSE_MENU_X + PAUSE_FRAME_WIDTH + 2 * CHAR_WIDTH, PAUSE_MENU_Y + PAUSE_FRAME_WIDTH + CHAR_HEIGHT * 9);
    printColored("    Game made by Baiges Matias, Comerci Nicolas and Pineiro Eugenia", 0xA165B8);
    setBackgroundColor(BLACK_COLOR);
    int c;
    while ((c = scanChar()) != ESC && (c != 'e') && (c != 'E') && (c != 's') && (c != 'S'));
    switch (c) {
    case ESC:
        return 0;
    case 'e':
    case 'E':
        *saved = 0;
        return 1;
        break;
    default: // in case c == 's' || c == 'S'
        *saved = 1;
        copyBackup(save_file, &gs);
        return 1;
        break;
    }
}

static void copyBackup( gameState * dst, gameState * src) {
    // Load state backup
    dst->shooted = src->shooted;
    dst->seconds_elapsed = src->seconds_elapsed;
    dst->remaining_lives = src->remaining_lives;
    
    // Load ball backup
    dst->ball.mov.movingDirX = src->ball.mov.movingDirX;
    dst->ball.mov.movingDirY = src->ball.mov.movingDirY;
    dst->ball.mov.velocity = src->ball.mov.velocity;
    dst->ball.point.x = src->ball.point.x;
    dst->ball.point.y = src->ball.point.y;

    // Load stick backup
    dst->stick.mov.movingDirX = src->stick.mov.movingDirX;
    dst->stick.mov.movingDirY = src->stick.mov.movingDirY;
    dst->stick.mov.velocity = src->stick.mov.velocity;
    dst->stick.point.x = src->stick.point.x;
    dst->stick.point.y = src->stick.point.y;

    // Load bricks backup
    for (int i = 0; i < sizeof(src->bricks)/sizeof(src->bricks[0]); i++)
        dst->bricks[i] = src->bricks[i];
}

static void newGameState(void) {
    gs.seconds_elapsed = 0;
    gs.remaining_lives = STARTING_LIVES;
    gs.shooted = 0;
    levelGenerator(gs.bricks); // Returns an array of length n_bricks
    int starting_x = (SCREEN_WIDTH - STICK_WIDTH) / 2;
    point2d stick_starting_point = {starting_x, SCREEN_HEIGHT - 1 - STICK_HEIGHT_FROM_FLOOR};
    movement stick_starting_mov = {0, 0, STICK_BALL_RELATIVITY * STARTING_VELOCITY};
    gs.stick.point = stick_starting_point;
    gs.stick.mov = stick_starting_mov;
    point2d ball_starting_point = {stick_starting_point.x + (STICK_WIDTH / 2), stick_starting_point.y - BALL_RADIUS - 1};
    movement ball_starting_mov = {0, 0, STARTING_VELOCITY}; // Could also be {-1, -1, v}, where v is the initial velocity of the ball
    gs.ball.point = ball_starting_point;
    gs.ball.mov = ball_starting_mov;
}

static void initGame(void) {
    // Copy actual bricks array
    for (int i = 0; i < N_BRICKS; i++)
        last_bricks[i] = gs.bricks[i];
    
    // Prepare Game Board
    fillGameBoard();
}

static void setScene(void) {
    // Set background color
    paintScreen(BACKGROUND_COLOR);
    
    // Draw bricks
    for (int i = 0; i < N_BRICKS; i++) {
        if (gs.bricks[i]) {
            drawRectangle( (i % BRICKS_PER_ROW) * BRICK_WIDTH, (i / BRICKS_PER_ROW) * BRICK_HEIGHT, BRICK_WIDTH, BRICK_HEIGHT, colors[gs.bricks[i]] - SHADER);
            drawRectangle( (i % BRICKS_PER_ROW) * BRICK_WIDTH, (i / BRICKS_PER_ROW) * BRICK_HEIGHT, BRICK_WIDTH - BRICK_FRAME_WIDTH, BRICK_HEIGHT - BRICK_FRAME_WIDTH, colors[gs.bricks[i]]);
        }
        else {
            drawRectangle( (i % BRICKS_PER_ROW) * BRICK_WIDTH, (i / BRICKS_PER_ROW) * BRICK_HEIGHT, BRICK_WIDTH, BRICK_HEIGHT, colors[gs.bricks[i]]);
        }
    }
    
    // Draw Stick
    drawRectangle(gs.stick.point.x, gs.stick.point.y, STICK_WIDTH, STICK_HEIGHT, STICK_COLOR - SHADER);
    drawRectangle(gs.stick.point.x, gs.stick.point.y, STICK_WIDTH - STICK_FRAME_WIDTH, STICK_HEIGHT - STICK_FRAME_WIDTH, STICK_COLOR);

    // Draw Ball
    drawCircle(gs.ball.point.x, gs.ball.point.y, BALL_RADIUS, BALL_COLOR);
}

static void fillGameBoard(void) {
    int i, j, k, l;

    for (i = 0; i < SCREEN_HEIGHT; i++)
        for (j = 0; j < SCREEN_WIDTH; j++)
            game_board[i][j] = -1;

    for (i = 0; i < N_BRICKS; i++)
        for (k = 0; k < BRICK_HEIGHT; k++)
            for (l = 0; l < BRICK_WIDTH; l++)
                game_board[(i / BRICKS_PER_ROW) * BRICK_HEIGHT + k][(i % BRICKS_PER_ROW) * BRICK_WIDTH + l] = i; 

    for (i = 0; i < STICK_HEIGHT; i++)
        for (j = 0; j < STICK_WIDTH; j++)
            game_board[gs.stick.point.y + i][gs.stick.point.x + j] = STICK_INDEX;  // It doesnt break, its the stick!
}

// Some non-sense crazy level generator
static int * levelGenerator(int * bricks) {
    int starting_at = getTime().hours / 4;
    for (int i = 0; i < N_BRICKS; i++) {
        if (i % 2 == 0 || i % 5 == 0)
            bricks[i] = 0;
        else
            bricks[i] = starting_at++ % 6;
    }
    return bricks;
}

// Another non-sense pseudo-"""random""" direction generator
static int directionDecision(void) {
    return (getTicks() % 2)?1:-1;
}

void updater(void) {
    int secs = getSecondsElapsed();

    if (secs > last_sec) {
        last_sec = secs;
        gs.seconds_elapsed++;
    }

    if (gs.ball.mov.velocity != MAX_VELOCITY && gs.seconds_elapsed / SPEEDING_TIME > speed) {
        speed = gs.seconds_elapsed / SPEEDING_TIME;
        gs.ball.mov.velocity = ((gs.ball.mov.velocity + 1) > MAX_VELOCITY)?MAX_VELOCITY:gs.ball.mov.velocity + 1;
        gs.stick.mov.velocity = gs.ball.mov.velocity * STICK_BALL_RELATIVITY;
        beeps(SPEEDING_SOUND_FREQ);
    }

    if (last_key == ARROW_LEFT || last_key == ARROW_RIGHT) {
        if(last_key == ARROW_LEFT) {
            if (!gs.shooted)
                gs.ball.mov.movingDirX = -1;        
            gs.stick.mov.movingDirX = -1; 
        }
        else {
            if (!gs.shooted)
                gs.ball.mov.movingDirX = 1;   
            gs.stick.mov.movingDirX = 1;
        }
    }
    last_key = 0;

    move_stick();
    if (gs.shooted)
        move_ball();
}

static void move_stick(void) {
    if (gs.stick.mov.movingDirX != 0) {
        int newX = gs.stick.point.x + gs.stick.mov.movingDirX * gs.stick.mov.velocity;
        if (game_board[gs.stick.point.y][newX] == BALL_INDEX) {
            gs.stick.mov.movingDirX = 0;                
            return;            
        }
        if (gs.stick.mov.movingDirX < 0) {
            if (newX < 0) {
                if (gs.stick.point.x == 0) {
                    gs.stick.mov.movingDirX = 0;                
                    return;
                }
                newX = 0;
            }
        }
        else {
            if (newX + STICK_WIDTH >= SCREEN_WIDTH) {
                if (gs.stick.point.x == SCREEN_WIDTH - STICK_WIDTH) {
                    gs.stick.mov.movingDirX = 0;                
                    return;
                }
                newX = SCREEN_WIDTH - STICK_WIDTH;
            }
        }
        drawRectangle(gs.stick.point.x, gs.stick.point.y, STICK_WIDTH, STICK_HEIGHT, BACKGROUND_COLOR);
        for (int i = 0; i < STICK_HEIGHT; i++)
            for (int j = 0; j < STICK_WIDTH; j++)
                game_board[gs.stick.point.y + i][gs.stick.point.x + j] = -1; 
        gs.stick.point.x = newX;
        drawRectangle(gs.stick.point.x, gs.stick.point.y, STICK_WIDTH, STICK_HEIGHT, STICK_COLOR - SHADER);
        drawRectangle(gs.stick.point.x, gs.stick.point.y, STICK_WIDTH - STICK_FRAME_WIDTH, STICK_HEIGHT - STICK_FRAME_WIDTH, STICK_COLOR);
        for (int i = 0; i < STICK_HEIGHT; i++)
            for (int j = 0; j < STICK_WIDTH; j++)
                game_board[gs.stick.point.y + i][gs.stick.point.x + j] = STICK_INDEX; 
        if (!gs.shooted) {
            drawCircle(gs.ball.point.x, gs.ball.point.y, BALL_RADIUS, BACKGROUND_COLOR);
            gs.ball.point.x = newX + (STICK_WIDTH / 2);
            drawCircle(gs.ball.point.x, gs.ball.point.y, BALL_RADIUS, BALL_COLOR);
        }
        gs.stick.mov.movingDirX = 0; 
    }
}

static void move_ball(void) {

    drawCircle(gs.ball.point.x, gs.ball.point.y, BALL_RADIUS, BACKGROUND_COLOR);

    for(int i = gs.ball.point.x - BALL_RADIUS; i < gs.ball.point.x + BALL_RADIUS; i++) {
		for(int j = gs.ball.point.y - BALL_RADIUS; j < gs.ball.point.y + BALL_RADIUS; j++) {
			if ((i >= 0) && (i < SCREEN_WIDTH) && (j >= 0) && (j < SCREEN_HEIGHT) && ((j - gs.ball.point.y) * (j - gs.ball.point.y) + (i - gs.ball.point.x) * (i - gs.ball.point.x) <= (BALL_RADIUS * BALL_RADIUS))) {
				game_board[j][i] = -1;
			}
		}
	}

    for (int i = 0; i < gs.ball.mov.velocity; i++)
        check_and_move_ball();   

    updateBricks();

    drawCircle(gs.ball.point.x, gs.ball.point.y, BALL_RADIUS, BALL_COLOR);

    for(int i = gs.ball.point.x - BALL_RADIUS; i < gs.ball.point.x + BALL_RADIUS; i++) {
		for(int j = gs.ball.point.y - BALL_RADIUS; j < gs.ball.point.y + BALL_RADIUS; j++) {
			if ((i >= 0) && (i < SCREEN_WIDTH) && (j >= 0) && (j < SCREEN_HEIGHT) && ((j - gs.ball.point.y) * (j - gs.ball.point.y) + (i - gs.ball.point.x) * (i - gs.ball.point.x) <= (BALL_RADIUS * BALL_RADIUS))) {
				game_board[j][i] = BALL_INDEX;
			}
		}
	}    
}

static void check_and_move_ball(void) {
    
    int x = gs.ball.point.x;
    int y = gs.ball.point.y;
    int movingX = gs.ball.mov.movingDirX;
    int movingY = gs.ball.mov.movingDirY;

    // Left Collision
    if (movingX < 0) {
        if (x - BALL_RADIUS + movingX < 0) {
            movingX = -movingX;
        }
        else if (game_board[y][x - BALL_RADIUS + movingX] >= 0) {
            if (game_board[y][x - BALL_RADIUS + movingX] == STICK_INDEX || gs.bricks[game_board[y][x - BALL_RADIUS + movingX]] > 0) {
                if (game_board[y][x - BALL_RADIUS + movingX] != STICK_INDEX)
                    gs.bricks[game_board[y][x - BALL_RADIUS + movingX]]--;
                movingX = -movingX;
            }
        }
    }

    // Right Collision
    if (movingX > 0) {
        if (x + BALL_RADIUS + movingX >= SCREEN_WIDTH) {
            movingX = -movingX;
        }
        else if (game_board[y][x + BALL_RADIUS + movingX] >= 0) {
            if (game_board[y][x + BALL_RADIUS + movingX] == STICK_INDEX || gs.bricks[game_board[y][x + BALL_RADIUS + movingX]] > 0) {
                if (game_board[y][x + BALL_RADIUS + movingX] != STICK_INDEX)
                    gs.bricks[game_board[y][x + BALL_RADIUS + movingX]]--;
                movingX = -movingX;
            }
        }
    }

    // Up Collision
    if (movingY < 0) {
        if (y - BALL_RADIUS + movingY < 0) {
            movingY = -movingY;
        }
        else if (game_board[y - BALL_RADIUS + movingY][x] >= 0) {
            if (game_board[y - BALL_RADIUS + movingY][x] == STICK_INDEX || gs.bricks[game_board[y - BALL_RADIUS + movingY][x]] > 0) {
                if (game_board[y - BALL_RADIUS + movingY][x] != STICK_INDEX)
                    gs.bricks[game_board[y - BALL_RADIUS + movingY][x]]--;
                movingY = -movingY;
            }
        }
    }

    // Down Collision
    if (movingY > 0) {
        if (y + BALL_RADIUS + movingY >= SCREEN_HEIGHT) {
            ball_fell_down();
        }
        else if (game_board[y + BALL_RADIUS + movingY][x] >= 0) {
            if (game_board[y + BALL_RADIUS + movingY][x] == STICK_INDEX || gs.bricks[game_board[y + BALL_RADIUS + movingY][x]] > 0) {
                if (game_board[y + BALL_RADIUS + movingY][x] != STICK_INDEX)
                    gs.bricks[game_board[y + BALL_RADIUS + movingY][x]]--;
                movingY = -movingY;
            }
        }
    }

    // Updates Ball
    gs.ball.point.x += movingX;
    gs.ball.point.y += movingY;
    gs.ball.mov.movingDirX = movingX;
    gs.ball.mov.movingDirY = movingY;
}

static void updateBricks(void) {
    int flag = 0;
    for (int i = 0; i < N_BRICKS; i++) {
        flag += gs.bricks[i];
        if (last_bricks[i] != gs.bricks[i]) {
            last_bricks[i] = gs.bricks[i];
            if (gs.bricks[i]) {
                drawRectangle( (i % BRICKS_PER_ROW) * BRICK_WIDTH, (i / BRICKS_PER_ROW) * BRICK_HEIGHT, BRICK_WIDTH, BRICK_HEIGHT, colors[gs.bricks[i]] - SHADER);
                drawRectangle( (i % BRICKS_PER_ROW) * BRICK_WIDTH, (i / BRICKS_PER_ROW) * BRICK_HEIGHT, BRICK_WIDTH - BRICK_FRAME_WIDTH, BRICK_HEIGHT - BRICK_FRAME_WIDTH, colors[gs.bricks[i]]);
            }
            else {
                drawRectangle( (i % BRICKS_PER_ROW) * BRICK_WIDTH, (i / BRICKS_PER_ROW) * BRICK_HEIGHT, BRICK_WIDTH, BRICK_HEIGHT, colors[gs.bricks[i]]);
            }
        }
    }
    if (flag == 0) {
        player_won = 1;
    }
}

static void ball_fell_down(void) {
    gs.remaining_lives--;
    gs.ball.point.x = gs.stick.point.x + STICK_WIDTH / 2;
    gs.ball.point.y = gs.stick.point.y - BALL_RADIUS - 1;
    gs.shooted = 0;
}