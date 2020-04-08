#ifndef _SCREEN_H
#define _SCREEN_H

#define WHITE_COLOR 0xFFFFFF
#define BLACK_COLOR 0
#define ERROR_COLOR 0xFF0000

void init_screen(void);
void toggleCursor(void);
void showCursor(int status);
void clearScreen(void);
int printString(char * str, unsigned int str_size);
int putChar(char c);
int printNewLine(void);
int putColorChar(char c, int color);
int printColorString(char * str, unsigned int str_size, int color);
int printError(char * str, unsigned int str_size);
void deleteLastChar(void);
void setBackgroundColor(uint32_t color);

int printDec(uint64_t value);
int printBase(uint64_t value, uint32_t base);
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
int print64Hex(uint64_t value);

int sys_screen(void * option, void * arg1, void * arg2, void * arg3);

#endif