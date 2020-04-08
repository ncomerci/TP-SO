#include <commands.h>
#include <stdint.h>
#include <shell.h>
#include <lib_user.h>

static int findColor(char * color);

char * color_names[] = {"black", "red", "green", "yellow", "blue", "pink", "light_blue", "white"};
uint32_t color_rgb[] = {0x000000, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF};

void printUserManual(){
    println(" _   _                ___  ___                        _ "); 
    println("| | | |               |  \\/  |                       | |");
    println("| | | |___  ___ _ __  | .  . | __ _ _ __  _   _  __ _| |");
    println("| | | / __|/ _ \\ '__| | |\\/| |/ _` | '_ \\| | | |/ _` | |");
    println("| |_| \\__ \\  __/ |    | |  | | (_| | | | | |_| | (_| | |");
    println(" \\___/|___/\\___|_|    \\_|  |_/\\__,_|_| |_|\\__,_|\\__,_|_|");
    println("");
    println("Commands are listed below:\n");
    println("- aracnoid --> A classic brick breaker like game.");
    println("- clock    --> Information about the local time in Buenos Aires.");
    println("- inforeg  --> Prints registers status.");
    println("- printmem --> Prints RAM status, starting at some value.");
    println("- clear    --> Clear shell screen.");
    println("- set      --> Sets some properties of the shell.");
    println("       + writing_color      --> Sets user writing color.");
    println("                       + [color_name]");
    println("                       + default");
    println("- test     --> Tests exceptions.");
    println("       + zero_div           --> Tests Zero-Division.");
    println("       + inv_op_code        --> Tests Invalid Op-code."); 
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
    char c;
    int i = 0;
    int started = 0;
    time_struct tm = {0, 0, 0};
    println("(Press ESC to return to shell)\n");
    print("Local time in ");
    printColored("Buenos Aires", 0xe37100);
    println(" is:");
    unsigned last_sec = getSecondsElapsed();
    while ((_sys_read((void *) &c) != 0) || c != ESC) {
        unsigned long actual_sec = getSecondsElapsed();
        if (!started || actual_sec > last_sec) {
            started = 1;
            last_sec = actual_sec;
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
        }
    }
    println("");
    println("");
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

void test(char * option) {
    if (strcmp(option, "zero_div") == 0)
        testDivException();
    else if (strcmp(option, "inv_op_code") == 0)
        testInvOpCode();
    else
        println("Invalid testing.");
}

void testInvOpCode() {
	__asm__("ud2");
}