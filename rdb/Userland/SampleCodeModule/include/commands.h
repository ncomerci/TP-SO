#ifndef _CMDS_H
#define _CMDS_H

#include <aracnoid.h>

void startAracnoid(gameState * save_file, int * saved);
void printUserManual(void); 
void getLocalTime(void);
void printRegistersInfo(void);
void clear(void);
void printProcesses(void);
void killProcess(int pid);
void changeProcessPriority(int pid, unsigned int priority);
void block(int pid);
void loop(void);
void printMemoryStatus(long int offset); 
void test(char * option);
void command_set(char * option, char * color);
void testDivException(void);
void testInvOpCode(void);
void testMM(void);
void testProcess(void);
int testProcess1Main(int argc, char **argv);
int loopMain(int argc, char ** argv);

#endif