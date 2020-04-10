#include "buddy.h"
#include <string.h>
#include <stdio.h>

int main(void){

    char *str = (char*)b_malloc(32);
    strcpy(str, "testeando malloc");
    printf("en str hay: %s\n", str);
    b_free(str);
    
    return 0;
}