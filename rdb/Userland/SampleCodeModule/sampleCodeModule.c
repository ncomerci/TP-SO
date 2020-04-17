/* sampleCodeModule.c */

#include <lib_user.h>
#include <shell.h>
#include <malloc.h>

int main() {
	
	main_func_t shell_main;
	shell_main.f = shellMain;
	shell_main.argc = 0;
	shell_main.argv = NULL;
	//createProcess(&shell_main, "shell", 1);
	startShell();

	return 0;
}