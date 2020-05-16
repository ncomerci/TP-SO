#ifndef _SHELL_H_
    #define _SHELL_H_

    #define BUFFER_SIZE 2000
    #define COMMANDS_BUFFER_SIZE 50
    #define LONGEST_PARAM 20
    #define MAX_PARAMS 3
    #define STUCKED_BEEP_FREQ 230

    #define USER_COLOR 0xfff4a3;
    #define USER_BACKGROUND_COLOR 0x000000

    #define TAB '\t'

    void startShell(void);
    void setUserWritingColor(uint32_t color);
    void resetUserWritingColor(void);

#endif