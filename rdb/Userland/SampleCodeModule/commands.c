#include <commands.h>
#include <stdint.h>
#include <lib_user.h>
#include <sem.h>

static int processCreation(int (func)(int argc, char**argv), int argc, char**argv, char* name, int foreground, char *in, char *out, uint64_t *pid);
static int clockUpdater(int argc, char ** argv);
static int findColor(char * color);
static int semProccess1(int argc, char ** argv);
static int semProccess2(int argc, char ** argv); 
static int semProccess3(int argc, char ** argv);
static int pipeProccess(int argc, char ** argv);
static void testPrior(void);
static void testSync(void);
static void testNoSync(void);
static int isVowel(int c);
static int main_pipeSh(int argc, char **argv); 

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
    char *name = "sem_test";

    sem = ksem_init_open(name, init_val);


    for (int i = 0; i < 500; i++) {
        ksem_wait(sem);
        printf("Sem Process 1 is active now!\n");

        //sleep(100);

        printf("Sem Process 1 is dead :O\n");
        ksem_post(sem);
    }

    ksem_close(sem);

    return 0;
}

static int semProccess2(int argc, char ** argv) {
    char *name = "sem_test";
    sem_id sem = ksem_open(name);

    for (int i = 0; i < 100; i++) {
        ksem_wait(sem);
        printf("Sem Process 2 here!\n");

        //sleep(5000);

        printf("Sem Process 2 dead :(\n");
        ksem_post(sem);
    }
    

    ksem_close(sem);

    return 0;
}

static int semProccess3(int argc, char ** argv) {
    char *name = "sem_test";
    sem_id sem = ksem_open(name);

    for (int i = 0; i < 50; i++) {
        ksem_wait(sem);
        printf("Sem Process 3 here!\n");

        sleep(1000);

        printf("Sem Process 3 dead :(\n");
        ksem_post(sem);  
    }

    ksem_close(sem);

    return 0;
}

static int pipeProccess(int argc, char ** argv) {
    char c;
    const int buff_size = 128;
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

static void testSync(void) {
    main_func_t f_sync = {main_test_sync, 0, NULL};
    uint64_t pid;
    createProcess(&f_sync, "Test Sync", 0, NULL, NULL, &pid);
}

static void testNoSync(void) {
    main_func_t f = {main_test_no_sync, 0, NULL};
    uint64_t pid;
    createProcess(&f, "Test No Sync", 0, NULL, NULL, &pid);
}

static void testPrior(void) {
    main_func_t f_prior= {main_test_prior, 0, NULL};
    uint64_t pid;
    createProcess(&f_prior, "Test Prior", 0, NULL, NULL, &pid);
}

/*
    System: inforeg, printmem, mem, sem, pipe
    Process: ps, nice, kill, block
    Apps: loop, phylo, wc, filter, cat, clock, sh
    Screen: clear, set
    Testing: test
*/

void printUserManual(){
    printf(" _   _                ___  ___                        _ \n"); 
    printf("| | | |               |  \\/  |                       | |\n");
    printf("| | | |___  ___ _ __  | .  . | __ _ _ __  _   _  __ _| |\n");
    printf("| | | / __|/ _ \\ '__| | |\\/| |/ _` | '_ \\| | | |/ _` | |\n");
    printf("| |_| \\__ \\  __/ |    | |  | | (_| | | | | |_| | (_| | |\n");
    printf(" \\___/|___/\\___|_|    \\_|  |_/\\__,_|_| |_|\\__,_|\\__,_|_|\n");
    printf("\n");
    printf("Commands are listed below:\n\n");
    printf("- sh <app calls>                       --> Shell calls with \"&\" and \"|\" for using with specific applications named down here.\n");
    printf("                  > wc\n");
    printf("                  > cat\n");
    printf("                  > filter\n");
    printf("                  > phylo\n");
    printf("                  > loop\n");
    printf(" < -- Remember not to use keyboard input consuming apps with background, this apps will get blocked -- >\n");
    //printf("- aracnoid                             --> A classic brick breaker like game.\n");
    printf("- clock                                --> Information about the local time in Buenos Aires.\n");
    printf("- inforeg                              --> Prints registers status.\n");
    printf("- printmem <starting_point>            --> Prints RAM status, starting at <starting_point>.\n");
    printf("- mem                                  --> Prints managed memory status.\n");
    printf("- sem                                  --> Prints semaphores info.\n"); 
    printf("- pipe                                 --> Prints pipes info.\n"); 
    printf("- clear                                --> Clear shell screen.\n");
    printf("- ps                                   --> Show living processes info.\n");
    printf("- nice <pid> <priority>                --> Changes priority of process with PID <pid> to <priority>.\n");
    printf("- kill <pid>                           --> Kills process with PID <pid>.\n");
    printf("- block <pid>                          --> Switches process PID <pid> between BLOCKED and READY state.\n");     
    printf("- loop                                 --> Creates background process named loop.\n");  
    printf("- phylo                                --> Prints philosophers dining:use \"a\" to add a philosopher and \"r\" to remove one\n");
    printf("- wc                                   --> Count input lines\n"); 
    printf("- filter                               --> Filter vowels from input\n"); 
    printf("- cat                                  --> Prints input\n");
    printf("- set                                  --> Sets some properties of the shell.\n");
    printf("       + writing_color                 --> Sets user writing color.\n");
    printf("                       + [color_name]\n");
    printf("                       + default\n");  
    printf("- test                                 --> Tests exceptions.\n");
    //printf("       + zero_div                      --> Tests Zero-Division.\n");
    //printf("       + inv_op_code                   --> Tests Invalid Op-code.\n");
    printf("       + mm                            --> Stress Tests Memory Allocation.\n"); 
    printf("       + mem                           --> Tests Memory Allocation.\n");
    printf("       + ps                            --> Stress Tests Processes.\n"); 
    printf("       + process                       --> Tests Processes\n");
    printf("       + prior                         --> Tests Process Priorities\n");
    printf("       + sync                          --> Tests Synchro\n");
    printf("       + nosync                        --> Tests No Synchro\n");
    printf("       + sem                           --> Tests Semaphores\n");
    printf("       + pipe                          --> Tests Pipes\n");
    printf("\n");
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

        sleep(1000);
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

void printBothSemaphores(void) {
    printSemaphores(USER);
    printSemaphores(KERNEL);
}

void printSemaphores(sem_location loc) {
    sem_info info[MAX_SEMAPHORES]; 
    uint64_t amount; 
    switch(loc) {
        case KERNEL:
            printf("Kernel Semaphores\n"); 
            amount = ksem_get_semaphores_info(info, MAX_SEMAPHORES);
            break;
        case USER:
            printf("User Semaphores\n"); 
            amount = sem_get_semaphores_info(info, MAX_SEMAPHORES);
            break;
    }
    printf("ID; NAME; VALUE; PROCESSES WAITING\n"); 
    for(unsigned int i = 0 ; i < amount ; i++ ) {
        printf("%d; %s; %d; {", info[i].id, info[i].name, (int) info[i].value);
        for(unsigned int j = 0; j < info[i].processes_waiting_amount; j++) {
            printf("%d", (int) info[i].processes_waiting[j]);
            if (j < info[i].processes_waiting_amount - 1)
                printf(", ");
        }
        printf("}\n");  
    } 
}

void printPipes(){ 

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
    printf("PID; PPID; Name; Piority; RBP; RSP; State; Foreground; Time left; Quantums\n");
    for (unsigned int i = 0; i < amount; i++)
        printf("%d; %d; %s; %d; %p; %p; %s; %s; %d; %d\n", (int) info[i].pid, (int) info[i].ppid, info[i].name, (int) info[i].priority, (uint64_t) info[i].rbp, (uint64_t) info[i].rsp, processess_states[(int) info[i].state], (info[i].foreground != 0)?"Yes":"No", (int) info[i].given_time, (int) info[i].aging);   
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
    if (createProcess(&loop, "Loop", 1, NULL, NULL, &pid) < 0){
        printf("Create Process Failed\n");
        return;
    }
    printf("Created process pid: %d\n", (int) pid);
    //changeState(pid, 1);
    //printf("Process %d %s\n", pid, (kill(pid) == 0)?"Killed":"Not Killed");
}

int main_printInput(int argc, char**argv){ //ARREGLAR
    int c;
    int i=0;
    char filteredInput[MAX_BUFFER]; 

    while( (c= scanChar()) != ESC){
        putChar(c);
        if(c != '\n')
            filteredInput[i++] = c; 
        else{
            filteredInput[i] = '\0'; 
            i=0;
            printf("%s\n", filteredInput); 
        }
    }

    return 0;
}


int main_countLines(int argc, char**argv){
    int lines= 0; 
    int c; 
    while( (c=scanChar()) != ESC ){
        if(c == '\n')
            lines++;
    }

    printf("%d", lines);
    return 0; 
}

static int isVowel(int c){
     if(c == 'a' || c=='e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c=='E' || c == 'I' || c == 'O' || c == 'U')
        return 1;
    return 0;
 }

 int main_filterVowels(int argc, char**argv){
    int c;
    //int i=0;
    //char filteredInput[MAX_BUFFER]; 
    while( (c= scanChar()) != ESC){
        if(!isVowel(c))
            putChar(c);
            //filteredInput[i++] = c;
    }
    //filteredInput[i] = '\0'; 
    //printf("%s", filteredInput); 
    return 0;
 }

 
int testProcess1Main(int argc, char ** argv) {
    uint64_t pid;
    if (getPid(&pid) < 0)
        return -1;
    printf("Received %d arguments!\n", argc);
    for (unsigned int i = 0; i < argc; i++) {
        printf("Hey, im process %d!\n", (int) pid);
    }
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
        sleep(1000);
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
    main_func_t sem3 = {semProccess3, 0, NULL};
    uint64_t pid1;
    uint64_t pid2;
    uint64_t pid3;
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
    if (createProcess(&sem3, "Test Semaphore 3", 0, NULL, NULL, &pid3) < 0) {
        printf("Create Process Failed\n");
        return;
    }
    printf("Sem Proccess 3 pid: %d\n", (int) pid3);
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
    else if(strcmp(option, "prior") == 0)
        testPrior();
    else if(strcmp(option, "sync") == 0)
        testSync();
    else if(strcmp(option, "nosync") == 0)
        testNoSync();
    /*
    else if (strcmp(option, "zero_div") == 0)
        testDivException();
    else if (strcmp(option, "inv_op_code") == 0)
        testInvOpCode();
    */
    else
        println("Invalid testing.");
}

void philosDiningProblem(void) {
    int cant;
    char buff[5];
    int c;
    unsigned int i;
    do {       
        i = 0;
        printf("Enter initial amount of philosophers:\n");
        while (i < 5 && (c = scanChar()) != '\n') {
            buff[i++] = (char) c;
            putChar(c);
        }
        buff[i] = '\0';
    } while(sscanf(buff, "%d", &cant) < 0);
    char ** args = malloc(2 * sizeof(char *));
    args[0] = malloc(5 * sizeof(char));
    args[1] = NULL;
    sprintf(args[0], "%d", cant);
    main_func_t f_philos = {thinking_philos_main, 1, args};
    uint64_t pid;
    printf("\n--> Philos Running with %d philosophers <--\n--> Press <a> to Add Philosophers <--\n--> Press <r> to Remove Philosophers <--\n", cant);
    createProcess(&f_philos, "philo problem", 1, NULL, NULL, &pid);
}

static int processCreation(int (func)(int argc, char**argv), int argc, char**argv, char* name, int foreground, char *in, char *out, uint64_t *pid) {
    main_func_t f = {func, argc, argv};
    return createProcess(&f, name, foreground, in, out, pid);
}

void shCommand(char (* params)[LONGEST_PARAM]) {
    
    char operator = params[1][0];
    //printf("I've read operator: %c", operator);
    if(operator != '&' && operator != '|') {
        printError("ERROR: second argument must be & or |\n");
        return;
    }

    char *main_func[] = {"cat", "wc", "filter", "loop","phylo"};
    int (*main_commands_func[])(int argc, char**argv) = {main_printInput, main_countLines, main_filterVowels, loopMain, thinking_philos_main};
    int i, j;
    int size = sizeof(main_func)/sizeof(char *);
    char err[100];
    uint64_t pid;

    for(i = 0; i < size && strcmp(params[0], main_func[i]) != 0 ; i++);

    if(i == size) {
        sprintf(err, "%s: command not found\n", params[0]);
        printError(err);
        return;
    }

    if(operator == '&') {
        
        if(i <= 2) {
            sprintf(err, "ERROR: %s cannot be followed by &\n", main_func[i]);
            printError(err);
            return;
        }
        

        int aux = processCreation(main_commands_func[i], 0, NULL, main_func[i], 0, NULL, NULL, &pid);
        
        if(aux < 0) {
            sprintf(err, "ERROR: %s cannot be executed\n", main_func[i]);
            printError(err);
            return;
        }
    }
    else {

        for(j = 0; j < size && strcmp(params[2], main_func[j]) != 0 ; j++);

        if(j == size) {
            sprintf(err, "%s: command not found\n", params[2]);
            printError(err);
            return;
        }

        sem_id sem = sem_init_open(SEM_PIPE_SH_NAME, 0);

        char **argv1;
        char **argv2;

        argv1 = malloc(2 * sizeof(char *));
        argv1[0] = malloc(MAX_ARG_LENGTH * sizeof(char));
        argv1[1] = malloc(MAX_ARG_LENGTH * sizeof(char));
        argv1[2] = NULL;

        strcpy(argv1[0], "1");
        sprintf(argv1[1], "%p", main_commands_func[j]);

        processCreation(main_pipeSh, 2, argv1, main_func[j], 0, SEM_PIPE_SH_NAME, NULL, &pid);

        argv2 = malloc(2 * sizeof(char *));
        argv2[0] = malloc(MAX_ARG_LENGTH * sizeof(char));
        argv2[1] = malloc(MAX_ARG_LENGTH * sizeof(char));
        argv2[2] = NULL;

        strcpy(argv2[0], "0");
        sprintf(argv2[1], "%p", main_commands_func[i]);

        processCreation(main_pipeSh, 2, argv2, main_func[i], 1, NULL, SEM_PIPE_SH_NAME, &pid);

        sem_wait(sem);

        free(argv1[1]);
        free(argv1[0]);
        free(argv1);

        free(argv2[1]);
        free(argv2[0]);
        free(argv2);

        sem_destroy(sem);
    }

}

static int main_pipeSh(int argc, char **argv) {
    int opt, aux = 0;
    sscanf(argv[0], "%d", &opt);
    int (*f)(int, char **);
    sscanf(argv[1], "%p", &f);
    if (opt != 0 && opt != 1)
        return -1;
    sem_id sem = sem_init_open(SEM_PIPE_SH_NAME, 1 - opt);
    if(sem < 0) {
        printError("ERROR: sem creation.");
        return -1;
    }
    f(argc-2, &(argv[2]));
    if (opt == 0) {
        aux = sem_wait(sem);
        if(aux < 0) {
            printError("ERROR: sem wait.");
            return -1;
        } 
        aux = sem_post(sem);
        if(aux < 0) {
            printError("ERROR: sem post.");
            return -1;
        } 
    }
    else {
        aux = sem_post(sem);
        if(aux < 0) {
            printError("ERROR: sem post.");
            return -1;
        } 
    }
    aux = sem_close(sem);

    if(aux < 0) {
        printError("ERROR: sem close.");
        return -1;
    }
     
    return 0;
}

void testInvOpCode() {
	__asm__("ud2");
}