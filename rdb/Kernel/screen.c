#include <stdint.h>
#include <screen.h>
#include <video_vm.h>
#include <timet.h>

static void _64Hexfill(int n, char * buffer);

static int CURSOR_SHOW_STATUS;
static int CURSOR_WIDTH;
static int CURSOR_HEIGHT;

static int CURSOR_COL;
static int CURSOR_ROW;

static int SCREEN_WIDTH;
static int SCREEN_HEIGHT;

static int MAX_CHARS_PER_ROW;
static int MAX_CHARS_PER_COL;

static int BACKGROUND_COLOR;

static void shiftCursor(int sign);
static void displayCursor(int status);
static void switchCursor(void);

void init_screen() {
    // Dibujar un palito 
    CURSOR_COL = 0;
    CURSOR_ROW = 0;
    CURSOR_WIDTH = CHAR_WIDTH;
    CURSOR_HEIGHT = CHAR_HEIGHT;
    SCREEN_HEIGHT = getScreenHeight();
    SCREEN_WIDTH = getScreenWidth();
    MAX_CHARS_PER_ROW = SCREEN_WIDTH / CHAR_WIDTH;
    MAX_CHARS_PER_COL = SCREEN_HEIGHT / CHAR_HEIGHT;
    BACKGROUND_COLOR = BLACK_COLOR;
}

void setCursor(unsigned int x, unsigned int y) {
    displayCursor(0);
    if ((x / CHAR_WIDTH) >= MAX_CHARS_PER_ROW)
        CURSOR_COL = CHAR_WIDTH * (MAX_CHARS_PER_ROW - 1);
    else
        CURSOR_COL = (x / CHAR_WIDTH) * CHAR_WIDTH;
    
    if ((y / CHAR_HEIGHT) >= MAX_CHARS_PER_COL)
        CURSOR_ROW = CHAR_HEIGHT * (MAX_CHARS_PER_COL - 1);
    else
        CURSOR_ROW = (y / CHAR_HEIGHT) * CHAR_HEIGHT;
}

void toggleCursor(void) {
    CURSOR_SHOW_STATUS = 1 - CURSOR_SHOW_STATUS;
    switchCursor();
}

static void switchCursor(void) {
    invertRectangleColor(CURSOR_COL, CURSOR_ROW, CURSOR_WIDTH, CURSOR_HEIGHT);
}

static void displayCursor(int status) {
    if ( (status == 0 && CURSOR_SHOW_STATUS == 1) || (status == 1 && CURSOR_SHOW_STATUS == 0) ) 
        toggleCursor();
}

void showCursor(int status) {
    if (status == 0) {
        removeFunction(toggleCursor);
        if (CURSOR_SHOW_STATUS == 1)
            toggleCursor();
    }
    else if (status == 1) {
        addFunction(toggleCursor, PIT_FREQUENCY / 2);
    }
}

static void shiftCursor(int sign) {
    displayCursor(0);
    if (CURSOR_SHOW_STATUS == 1) {} 
    if (sign == -1) {
        if (CURSOR_COL >= CHAR_WIDTH) {
            CURSOR_COL -= CHAR_WIDTH;
        }
        else if (CURSOR_ROW - CHAR_HEIGHT >= 0) {
            CURSOR_ROW -= CHAR_HEIGHT;
            CURSOR_COL = CHAR_WIDTH * (MAX_CHARS_PER_ROW - 1);
        }
    }
    else if (sign == 1) {
        if (CURSOR_COL >= CHAR_WIDTH * (MAX_CHARS_PER_ROW - 1)) {
            if (CURSOR_ROW + CHAR_HEIGHT >= SCREEN_HEIGHT)
                scrollUp(CHAR_HEIGHT);
            else
                CURSOR_ROW += CHAR_HEIGHT;
            CURSOR_COL = 0;
        }
        else
            CURSOR_COL += CHAR_WIDTH;
    }
}

void deleteLastChar() {
    displayCursor(0);
    shiftCursor(-1);
    putChar(' ');
    displayCursor(0);
    shiftCursor(-1);
}

void clearScreen() {
    displayCursor(0);
    clearDisplay(BACKGROUND_COLOR);
    CURSOR_COL = CURSOR_ROW = 0;
}

int putChar(char c) {
    putColorChar(c, WHITE_COLOR);
    return 0;
}

int putColorChar(char c, int color) {
    if (c == '\n')
        printNewLine();
    else {
        displayCursor(0);
        drawChar(CURSOR_COL, CURSOR_ROW, c, color, BACKGROUND_COLOR);
        shiftCursor(1);
    }
    return 0;
}

int printNewLine() {
    displayCursor(0);
    if (CURSOR_ROW + 2*CHAR_HEIGHT >= SCREEN_HEIGHT) {
        scrollUp(CHAR_HEIGHT);
    }
    else {
        CURSOR_ROW += CHAR_HEIGHT;
    }
    CURSOR_COL = 0;
    return 0;
}

int printString(char * str, unsigned int str_size) {
    return printColorString(str, str_size, WHITE_COLOR);
}

int printColorString(char * str, unsigned int str_size, int color) {
    for (int i = 0; i < str_size && str[i] != 0; i++) {
        putColorChar(str[i], color);
    }
    return 0;
}

int printError(char * str, unsigned int str_size) {
    printColorString(str, str_size, ERROR_COLOR);
    return 0;
}

void setBackgroundColor(uint32_t color) {
    BACKGROUND_COLOR = color;
}

int sys_screen(void * option, void * arg1, void * arg2, void * arg3) {
    switch ((uint64_t) option) {
        case 0:
            clearScreen();
            break;
        case 1:
            printColorString((char *) arg1, (uint64_t) arg2, (uint64_t) arg3);
            break;
        case 2:
            deleteLastChar();
            break;
        case 3:
            showCursor((uint64_t) arg1);
            break;
        case 4:
            shiftCursor((uint64_t) arg1);
            break;
        case 5:
            setBackgroundColor((uint64_t) arg1);
            break;
        case 6:
            setCursor((uint64_t) arg1, (uint64_t) arg2);
    }
    return 0;
}

static char buffer[64] = { '0' };

int printDec(uint64_t value) {
	return printBase(value, 10);
}

int printBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    return printString(buffer, 64);
}

int print64Hex(uint64_t value) {
	int digits = uintToBase(value, buffer, 16);
	_64Hexfill(16 - digits, buffer);
	return printColorString(buffer, 64, ERROR_COLOR);
}

static void _64Hexfill(int n, char * buffer) {
	for (int i = 15; i >= 0; i--) {
		if (i >= n)
			buffer[i] = buffer[i - n];
		else
			buffer[i] = '0';
	}
	buffer[16] = 0;
}

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}