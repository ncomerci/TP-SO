#ifndef _CMDS_H
#define _CMDS_H

#include <aracnoid.h>
#include <philo.h>

#define MAX_BUFFER 100

void startAracnoid(gameState * save_file, int * saved);
void printUserManual(void); 
void getLocalTime(void);
void printRegistersInfo(void);
void clear(void);
void printProcesses(void);
void killProcess(uint64_t pid);
void changeProcessPriority(uint64_t pid, unsigned int priority);
void block(uint64_t pid);
void loop(void);
void printMemoryStatus(long int offset); 
void test(char * option);
void command_set(char * option, char * color);
void testDivException(void);
void testInvOpCode(void);
void testMem(void);
void testMM(void);
void testPS(void);
void printMMStats(void);
void testProcess(void);
void testProcessArgs(void);
int testProcess1Main(int argc, char **argv);
int testProcessArgsMain(int argc, char ** argv);
int loopMain(int argc, char ** argv);
int main_test_mm(int argc, char ** argv);
int main_test_process(int argc, char ** argv);
int main_test_prior(int argc, char ** argv);
int main_test_sync(int argc, char ** argv);
void testSem(void);
void philosDiningProblem(void); 
void countLines(void);
void filterVowels(void);
void printInput(void); 
void printSemaphores(void); 


#endif