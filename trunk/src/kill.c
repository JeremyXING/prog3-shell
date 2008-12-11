#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "kill.h"

static char * tab_signame[32] = {"SIGNAL 0", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP",
								"SIGABRT", "SIGEMT", "SIGFPE", "SIGKILL", "SIGBUS","SIGSEGV",
								"SIGSYS", "SIGPIPE", "SIGALRM", "SIGTERM", "SIGURG", "SIGSTOP",
								"SIGTSTP", "SIGCONT", "SIGCHLD", "SIGTTIN", "SIGTTOU", "SIGIO",
								"SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO",
								"SIGUSR1", "SIGUSR2"};

#define NB_SIG  32


int kill_no_signal(char * sig){
	if(atoi(sig) != 0)
		return atoi(sig);
	for(int i=1; i < NB_SIG; i++)
		if( strcmp(sig, tab_signame[i]) == 0 )
			return i;
	printf("Signal inexistant.\n");
	return -1;
	
}

int kill_usage(void){
	printf("Usage: kill [-s sigspec | -n signum | -sigspec] pid\n");
	printf("       kill -l [sigspec]\n");
	return 1;
}

void kill_liste_signaux(void){
	int i,j;
	for(i=1, j=1; i < NB_SIG; i++, j++){
		printf("%2d) %s\t", i, tab_signame[i]);
		if(j == 4){
			printf("\n");
			j=0;
		}
	}
	printf("\n");
}


