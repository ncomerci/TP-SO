#ifndef LIBUSER_H
#define LIBUSER_H

#include <stdint.h>
#include <stdarg.h>

#define WHITE_COLOR 0xFFFFFF
#define BLACK_COLOR 0
#define LIGHT_BLUE_COLOR 0x89CFF0
#define ERROR_COLOR 0xFF0000

#define PIT_FREQUENCY 400

#define MAX_PROCESS_NAME_LENGTH 50
#define MAX_PROCESSES 50 

#define NULL ((void *) 0)

enum SPECIAL_KEYS {SHIFT_IN = -20, SHIFT_OUT, CAPS, BACKS, ENTER, ESC,CTRL, ALT,F1, F2, ARROW_UP, ARROW_LEFT,ARROW_RIGHT, ARROW_DOWN, INS, DEL};
enum REG_POS {RAX = 0, RBX, RCX, RDX, RBP, RDI, RSI, R8, R9, R10, R11, R12, R13, R14, R15, RIP, TOTAL_REGS};

typedef void (*function)(void);

typedef struct point {
    int x;
    int y;
} point;

// ----------- Sys Calls ------------
int _sys_system(void * arg1, void * arg2, void * arg3, void * arg4);
int _sys_process(void * arg1, void * arg2, void * arg3, void * arg4);
int _sys_timet(void * arg1, void * arg2, void * arg3);
int _sys_rtc(void * arg1);
int _sys_read(void * arg1);
int _sys_screen(void * arg1, void * arg2, void * arg3, void * arg4);
int _sys_video(void * arg1, void * arg2, void * arg3, void * arg4, void * arg5);
int _sys_sound(void * arg1, void * arg2, void * arg3);

// ----------- System ------------
typedef struct mm_stat {
	char * sys_name;
	uint64_t total;
	uint64_t occupied;
	uint64_t free;
	uint64_t successful_allocs;
	uint64_t successful_frees;
} mm_stat;
int getMem(void *pos, uint64_t *mem_buffer, unsigned int dim);
void * malloc(uint64_t size);
void free(void * ptr);
mm_stat getMMStats(void);

// ----------- Process ------------
typedef struct PCB {
    char name[MAX_PROCESS_NAME_LENGTH];
    void * stack;
    void * rsp;
    void * rbp;
    int pid;
    int ppid;
    int foreground;
    unsigned int state;
    unsigned int priority;
    unsigned int given_time;
    unsigned int aging; 
    struct PCB * next_in_queue;
} PCB;
typedef struct main_func_t {
    int (*f)(int, char **);
    int argc;
    char ** argv;
} main_func_t;
int createProcess(main_func_t * main_f, char * name, int foreground);
int kill(int pid);
int getPid(void);
unsigned int getProcessesAmount(void);
int getProcessesInfo(PCB * arr, unsigned int max_size);
int exit(int pid);
int changePriority(int pid, unsigned int new_priority);
int changeState(int pid, int new_state);
int changeForegroundStatus(int pid, int state);

// ----------- Timet ------------
unsigned long getTicks(void);
unsigned long getSecondsElapsed(void);
int addTimeFunction(function f, unsigned int ticks);
int updateTimeFunction(function f, unsigned int new_ticks);
void removeTimeFunction(function f);
void wait(unsigned int millis);

// ----------- RTC ------------
typedef struct time_struct {
	int hours;
	int mins;
	int secs;
} time_struct;
time_struct getTime(void);

// ----------- Read ------------
int read(char *buffer, unsigned int buff_size);
int scan(char *buffer, unsigned int buff_size);
uint64_t *getRegisters(void);
char scanChar();

// ----------- Screen ------------
void clearScreen(void);
void setBackgroundColor(uint32_t color);
void setCursor(unsigned int x, unsigned int y);
void shiftCursor(int offset);
void showCursor(int status);
int write(const char *str, unsigned int str_size, int color);
int putChar(char c);
int putColoredChar(char c, int color);
int print(const char *str); 
int printf(const char* fmt,...);
int printColored(const char *str, int color);
int printError(const char *str);
int println(const char *str);
int deleteChar(void);
int deleteNChars(int n);
void printRegistries(void);

// ----------- Video ------------
void paintScreen(uint32_t color);
unsigned int getScreenWidth(void);
unsigned int getScreenHeight(void);
void drawEllipse(int x, int y, unsigned int a, unsigned int b, uint32_t color);
void drawCircle(int x, int y, unsigned int r, uint32_t color);
void drawRectangle(int x, int y, unsigned int b, unsigned int h, uint32_t color);
void drawSquare(int x, int y, unsigned int l, uint32_t color);

// ----------- Sound ------------
void play_sound(uint32_t freq);
void shut_sounds(void);
void beeps(uint32_t freq);

// ----------- Strings ------------
unsigned int strlen(const char *str);
int strcmp (const char * s1, const char * s2);
int strncmp(const char *s1, const char *s2, unsigned int n);
int strcpy (char *dst, const char *src);
long int strtoint(char* s);
int is_num(char *s);

// Importados de naiveConsole

int printDec(uint64_t value);
int printOct(uint64_t value);
int printHex(uint64_t value);
int print64Hex(uint64_t value);
int printBin(uint64_t value);
int printBase(uint64_t value, uint32_t base);
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

#endif