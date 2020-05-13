#include <commands.h>
#include <stdint.h>
#include <shell.h>
#include <lib_user.h>
#include <sem.h>

static int clockUpdater(int argc, char ** argv);
static int findColor(char * color);
static int semProccess1(int argc, char ** argv);
static int semProccess2(int argc, char ** argv); 
static int pipeProccess(int argc, char ** argv);

static char * color_names[] = {"black", "red", "green", "yellow", "blue", "pink", "light_blue", "white"};
static uint32_t color_rgb[] = {0x000000, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF};
static char * processess_states[] = {"READY", "BLOCKED", "KILLED"};

int i = 0;
int started = 0;
time_struct tm = {0, 0, 0};
unsigned last_sec;
unsigned long actual_sec;

static int semProccess1(int argc, char ** argv) {
    unsigned int init_val = 1;
    sem_id sem;
    const char *name = "sem_test";

    sem = sem_init_open(name, init_val);

    sem_wait(sem);
    printf("Sem Process 1 is active now!\n");

    wait(2000);

    printf("Sem Process 1 is dead :O\n");
    sem_post(sem);

    sem_close(sem);

    return 0;
}

static int semProccess2(int argc, char ** argv) {
    const char *name = "sem_test";
    sem_id sem = sem_open(name);

    sem_wait(sem);
    printf("Sem Process 2 here!\n");

    wait(2000);

    printf("Sem Process 2 dead :(\n");

    sem_post(sem);

    sem_close(sem);

    return 0;
}

static int pipeProccess(int argc, char ** argv) {
    char c;
    const int buff_size = 31;
    char buff[buff_size];
    int count = 0;
    while((c = scanChar()) != '\n') {
        if(count < buff_size-1)
            buff[count++] = c;
    }
    buff[count++] = '\n';
    buff[count] = '\0';
    printf(buff);

    return 0;
}

void printUserManual(){
    println(" _   _                ___  ___                        _ "); 
    println("| | | |               |  \\/  |                       | |");
    println("| | | |___  ___ _ __  | .  . | __ _ _ __  _   _  __ _| |");
    println("| | | / __|/ _ \\ '__| | |\\/| |/ _` | '_ \\| | | |/ _` | |");
    println("| |_| \\__ \\  __/ |    | |  | | (_| | | | | |_| | (_| | |");
    println(" \\___/|___/\\___|_|    \\_|  |_/\\__,_|_| |_|\\__,_|\\__,_|_|");
    println("");
    println("Commands are listed below:\n");
    //println("- aracnoid                             --> A classic brick breaker like game.");
    println("- clock                                --> Information about the local time in Buenos Aires.");
    println("- inforeg                              --> Prints registers status.");
    println("- printmem <starting_point>            --> Prints RAM status, starting at <starting_point>.");
    println("- mem                                  --> Prints managed memory status");
    println("- clear                                --> Clear shell screen.");
    println("- ps                                   --> Show living processes info.");
    println("- nice <pid> <priority>                --> Changes priority of process with PID <pid> to <priority>.");
    println("- kill <pid>                           --> Kills process with PID <pid>.");
    println("- block <pid>                          --> Switches process PID <pid> between BLOCKED and READY state.");     
    println("- loop                                 --> Creates background process named loop.");  
    println("- wc                                   --> Count input lines"); 
    println("- filter                               --> Filter vowels from input"); 
    println("- cat                                  --> Prints input");
    println("- set                                  --> Sets some properties of the shell.");
    println("       + writing_color                 --> Sets user writing color.");
    println("                       + [color_name]");
    println("                       + default");  
    println("- test                                 --> Tests exceptions.");
    //println("       + zero_div                      --> Tests Zero-Division.");
    //println("       + inv_op_code                   --> Tests Invalid Op-code.");
    println("       + mem                           --> Tests Memory Allocation."); 
    println("       + process                       --> Tests Processes");
    println("       + sem                           --> Tests Semaphores");
    println("       + pipe                          --> Tests Pipes");
    println("");
}

void command_set(char * option, char * color) {
    if (strcmp(option, "writing_color") == 0) {
        int idx = findColor(color);
        if (idx >= 0)
            setUserWritingColor(color_rgb[idx]);
        else if (strcmp(color, "default") == 0)
            resetUserWritingColor();
        else
            println("Not a valid color.");
    }
    else
        println("Not a valid argument.");
}

static int findColor(char * color) {
    for (int i = 0; i < sizeof(color_names)/sizeof(char *); i++) {
        if (strcmp(color_names[i], color) == 0)
            return i;
    }
    return -1;
}

void getLocalTime(){
    println("(Press ESC to return to shell)\n");
    print("Local time in ");
    printColored("Buenos Aires", 0xe37100);
    println(" is:");
    main_func_t aux = {clockUpdater, 0, NULL};
    
    uint64_t pid;
    if (createProcess(&aux, "Clock Updater", 0, NULL, NULL, &pid) < 0) {
        printf("Create process failed\n");
        return;
    }

    while ((scanChar() != ESC));
    println("");
    println("");
    kill(pid);
}

static int clockUpdater(int argc, char ** argv) {
    i = 0;
    while (1) {
        tm = getTime();
        if (i > 0) {
            deleteNChars(i);
            i = 0;
        }
        i += printDec(tm.hours);
        print("hs, ");
        i += printDec(tm.mins);
        print("m, ");
        i += printDec(tm.secs);
        print("s");
        i += 8; // hs, m, s

        wait(1000);
    }
    return 0;
}

void printRegistersInfo(){
    char *names[TOTAL_REGS] = {"RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", " R8", " R9", "R10", "R11", "R12", "R13", "R14", "R15"};
	uint64_t *values = getRegisters();
	println("\nREGISTRY VALUES:\n");
    int i;
	for(i = RAX; i + 2 < TOTAL_REGS ; i+= 3) {
        printf("%s: 0x%X    ", names[i], values[i]);
		printf("%s: 0x%X    ", names[i+1], values[i+1]);
        printf("%s: 0x%X\n", names[i+2], values[i+2]);
	}
}

void clear(){
    clearScreen();
}

void printMemoryStatus(long int offset){
    uint64_t mem[32];
    getMem((void *) offset, mem, 32);
    for(int i = 0 ; i + 1 < 32 ; i+= 2) {
        printf("%x: 0x%X    %x: 0x%X\n", offset + i*sizeof(uint64_t), mem[i], offset + (i+1)*sizeof(uint64_t), mem[i+1]);
    }
}

void startAracnoid(gameState * save_file, int * saved) {
    aracnoid(save_file, saved);
    clear();
}

void testMem(void){
    printf("Using %s Memory Manager !\n", getMMStats().sys_name);
    char line1[] = "Copied";
    printf("Beggining\n");
    printMMStats();
    char * line2 = malloc(sizeof(line1)/sizeof(char));
    strcpy(line2, line1);
    printf("Copied?: %s\n", line2);
    printMMStats();
    void * aux = malloc((1 << 22) - 8);
    printf("Malloc returned %p\n", (uint64_t) aux);
    printMMStats();
    void * aux2 = malloc(100);
    printf("Malloc returned %p\n", (uint64_t) aux2);
    printMMStats();
    free(line2);
    printf("Freed line2\n");
    //printMMStats();
    free(aux);
    printf("Freed aux\n");
    //printMMStats();
    free(aux2);
    printf("Freed aux2\n");
    printMMStats();
}

void printMMStats(void) {
    mm_stat aux = getMMStats();
    printf("// ----------------------------- //\n");

    printf("Total Space = %d\n", (int) aux.total);
    printf("Occupied Space = %d\n", (int) aux.occupied);
    printf("Free Space = %d\n", (int) aux.free);
    printf("Successful Frees = %d\n", (int) aux.successful_frees);
    printf("Successful Allocs = %d\n", (int) aux.successful_allocs);
    
    printf("// ----------------------------- //\n");
}

void printProcesses(void) {
    PCB_info info[MAX_PROCESSES];
    int amount = getProcessesInfo(info, MAX_PROCESSES);
    printf("PID, PPID, Name, Piority, RBP, RSP, State, Foreground, Time left, Quantums\n");
    for (unsigned int i = 0; i < amount; i++)
        printf("%d, %d, %s, %d, %p, %p, %s, %s, %d, %d\n", (int) info[i].pid, (int) info[i].ppid, info[i].name, (int) info[i].priority, (uint64_t) info[i].rbp, (uint64_t) info[i].rsp, processess_states[(int) info[i].state], (info[i].foreground != 0)?"Yes":"No", (int) info[i].given_time, (int) info[i].aging);   
}

void killProcess(uint64_t pid) {
    if (pid == 1)
        printColored("not_that_dummy though\n", color_rgb[1]);
    else
        printf("Process %s\n", (kill(pid) == 0)?"killed":"not found");
}

void block(uint64_t pid){
    if(pid == 1){
         printColored("not_that_dummy though\n", color_rgb[1]);
    }else{
        process_state state;
        int ans = getProcessState(pid, &state); 

        if(ans == -1 || state == KILLED) //it is KILLED or does not exist 
            printf("Process not found\n"); 
        
        else if ( state == READY )  
            changeState(pid, BLOCKED); 

        else if( state == BLOCKED)
            changeState(pid, READY); 
    }
   
}

void changeProcessPriority(uint64_t pid, unsigned int priority) {

    int aux = changePriority(pid, priority);
    if (aux != 0)
        printf("Process not found\n");
    else
        printf("Process priority changed\n");
}

void testProcess(void) {
    main_func_t testprocess = {testProcess1Main, 100, NULL};
    uint64_t pid;
    if (createProcess(&testprocess, "Test Process", 1, NULL, NULL, &pid) < 0) {
        printf("Create Process Failed\n");
        return;
    }
    printf("Created process pid: %d\n", (int) pid);
}

void testProcessArgs(void) {
    char ** args = malloc(6 * sizeof(char *));
    for (int i = 0; i < 5; i++) {
        args[i] = malloc(10 * sizeof(char));
    }
    strcpy(args[0],"Hola");
    strcpy(args[1],"Esta");
    strcpy(args[2],"Es");
    strcpy(args[3],"Una");
    strcpy(args[4],"Prueba");
    strcpy(args[5],NULL);

    main_func_t testprocessargs = {testProcessArgsMain, 5, args};
    uint64_t pid;
    if (createProcess(&testprocessargs, "Test Process Arguments", 1, NULL, NULL, &pid) < 0){
        printf("Create Process Failed\n");
        return;
    }
    printf("Created process pid: %d\n", (int) pid);

    for (int i = 4; i >= 0; i--) {
        free(args[i]); 
    }
    free(args);
}

void loop(void) {
    main_func_t loop = {loopMain, 0, NULL};
    uint64_t pid;
    if (createProcess(&loop, "Loop", 0, NULL, NULL, &pid) < 0){
        printf("Create Process Failed\n");
        return;
    }
    printf("Created process pid: %d\n", (int) pid);
    //changeState(pid, 1);
    //printf("Process %d %s\n", pid, (kill(pid) == 0)?"Killed":"Not Killed");
}

void printInput(void){
    char c;
    int i=0;
    char filteredInput[100]; 

    while( (c= scanChar()) != ESC){
        if(c != '\n')
            filteredInput[i++] = c; 
        else{
            filteredInput[i] = '\0'; 
            i=0;
            printf("%s\n", filteredInput); 
        }
    }
}


void countLines(void){
    int lines= 0; 
    int c; 
    while( (c=scanChar()) != ESC ){
        if(c == '\n')
            lines++;
    }

    printf("%d", lines); 
}

static int isVowel(int c){
     if(c == 'a' || c=='e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c=='E' || c == 'I' || c == 'O' || c == 'U')
        return 1;
    return 0;
 }

 void filterVowels(void){
     char c;
     int i=0;
    char filteredInput[100]; 
     while( (c= scanChar()) != '\n'){
         if(isVowel(c) == 0)
            filteredInput[i++] = c; 
     }
     filteredInput[i] = '\0'; 
     printf("%s", filteredInput); 
 }

 
int testProcess1Main(int argc, char ** argv) {
    uint64_t pid;
    printf("Received %d arguments!\n", argc);
    for (unsigned int i = 0; i < argc; i++) {
        getPid(&pid);
        printf("Sent");
        return;
    }
    printf("Hola! Soy el proceso %d\n", (int) pid);
    return 0;
}

int testProcessArgsMain(int argc, char ** argv) {
    printf("Received %d arguments:\n", argc);
    for (unsigned int i = 0; i < argc; i++) {
        printf("Arg #%d --> \"%s\"\n", i, argv[i]);
    }
    return 0;
}

int loopMain(int argc, char ** argv) {
    uint64_t pid;
    for (unsigned int i = 0; 1 ; i++) {
        wait(1000);
        if (getPid(&pid) < 0) {
            printf("getPid failed\n");
            return -1;
        }
        printf("%d - Soy el proceso id: %d!\n", i, (int) pid);
    }
    return 0;
}

void testMM(void) {
    main_func_t testmm = {main_test_mm, 0, NULL};
    uint64_t pid;
    if (createProcess(&testmm, "Test MM", 0, NULL, NULL, &pid) < 0){
        printf("Create Process Failed\n");
        return;
    }
    printf("Created process pid: %d\n", (int) pid);
}

void testPS(void) {
    main_func_t testps = {main_test_process, 0, NULL};
    uint64_t pid;
    if (createProcess(&testps, "Test PS", 0, NULL, NULL, &pid) < 0){
        printf("Create Process Failed\n");
        return;
    }
    printf("Created process pid: %d\n", (int) pid);
}

void testSem(void) {
    main_func_t sem1 = {semProccess1, 0, NULL};
    main_func_t sem2 = {semProccess2, 0, NULL};
    uint64_t pid1;
    uint64_t pid2;
    if (createProcess(&sem1, "Test Semaphore 1", 0, NULL, NULL, &pid1) < 0) {
        printf("Create Process Failed\n");
        return;
    }
    printf("Sem Proccess 1 pid: %d\n", (int) pid1);
    if (createProcess(&sem2, "Test Semaphore 2", 0, NULL, NULL, &pid2) < 0) {
        printf("Create Process Failed\n");
        return;
    }
    printf("Sem Proccess 2 pid: %d\n", (int) pid2);
}

void testPipe(void){
    char argv[][1] = {{'\n'}}; 
    uint64_t pid1;
    uint64_t pid2;
    main_func_t pipeps1 = {pipeProccess, 1, (char **)argv};
    main_func_t pipeps2 = {pipeProccess, 1, (char **)argv};
    if (createProcess(&pipeps1, "Test Pipe 1", 1, NULL, "patito", &pid1) < 0) { //lee de stdin y escribe en patito
        printf("Create Process Failed\n");
        return;
    }
    printf("Pipe Proccess 1 pid: %d\n", (int) pid1);
    if (createProcess(&pipeps2, "Test Pipe 2", 0, "patito", NULL, &pid2) < 0) { //lee de patito y escribe en stdout
        printf("Create Process Failed\n");
        return;
    }
    printf("Pipe Proccess 2 pid: %d\n", (int) pid2);
}

void test(char * option) {

    if (strcmp(option, "mem") == 0)
        testMem();
    else if(strcmp(option, "process") == 0)
        testProcess();
    else if(strcmp(option, "process_args") == 0)
        testProcessArgs();
    else if(strcmp(option, "mm") == 0)
        testMM();//testMM();
    else if (strcmp(option, "ps") == 0)
        testPS();
    else if(strcmp(option, "sem") == 0)
        testSem();
    else if(strcmp(option, "pipe") == 0)
        testPipe();
    /*
    else if (strcmp(option, "zero_div") == 0)
        testDivException();
    else if (strcmp(option, "inv_op_code") == 0)
        testInvOpCode();
    */
    else
        println("Invalid testing.");
}

void testInvOpCode() {
	__asm__("ud2");
}