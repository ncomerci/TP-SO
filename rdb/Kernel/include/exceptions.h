#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H
    #include <stdint.h>
    #include <screen.h>
    
    enum REG_POS {R15 = 0, R14, R13, R12, R11, R10, R9, R8, RSI, RDI, RBP, RDX, RCX, RBX, RAX, RIP, TOTAL_REGS};
    static char *names[TOTAL_REGS] = {"R15", "R14", "R13", "R12", "R11", "R10", " R9", " R8", "RSI", "RDI", "RBP", "RDX", "RCX", "RBX", "RAX", "RIP"};
    uint64_t *getRegisters(void);

    void reg_info(uint64_t * values) {
        int i;
	    for(i = R15; i + 3 < TOTAL_REGS ; i+= 4) {
            printColorString(names[i],4, ERROR_COLOR);
            printColorString(": 0x", 5, ERROR_COLOR);
            print64Hex(values[i]);
            printColorString("    ",5, ERROR_COLOR);
            printColorString(names[i+1],4, ERROR_COLOR);
            printColorString(": 0x", 5, ERROR_COLOR);
            print64Hex(values[i+1]);
            printColorString("    ",5, ERROR_COLOR);
            printColorString(names[i+2],4, ERROR_COLOR);
            printColorString(": 0x", 5, ERROR_COLOR);
            print64Hex(values[i+2]);
            printColorString("    ",5, ERROR_COLOR);
            printColorString(names[i+3],4, ERROR_COLOR);
            printColorString(": 0x", 5, ERROR_COLOR);
            print64Hex(values[i+3]);
            printNewLine();
        }
    }

    void exceptionMessage(char * msg, unsigned int size) {
        printError(msg, size);
    }

    static void zero_division();
    static void invalid_op_code();

#endif
