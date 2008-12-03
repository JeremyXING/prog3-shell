#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

/*
 * Renvoie la longueur d'une liste d'arguments
 */
int LongueurListe(char **l)
{
  char **p;
  for (p=l; *p != NULL; p++)
    ;
  return p-l;
} /* LongueurListe */

static char * minuscule(const char * s){
	int taille = strlen(s);
	char * min = malloc(taille * sizeof(*min));
	for(int i=0; i < taille; i++)
		min[i] = tolower(s[i]);
	return min;
}

static char * majuscule(const char * s){
	int taille = strlen(s);
	char * min = malloc(taille * sizeof(*min));
	for(int i=0; i < taille; i++)
		min[i] = toupper(s[i]);
	return min;
}

static int kill_no_signal(char * sig){
  if(atoi(sig) != 0)
    return atoi(sig);
  int i=1;
  sig = minuscule(sig)+3;
  while(i < NSIG){
    if(strcmp(sig, sys_signame[i]) == 0)
       return i;
    i++;
  }
  	printf("Signal inexistant.\n");
    return -1;

}

static void kill_usage(void){
 printf("usage: kill [-s sigspec | -n signum | -sigspec] pid or kill -l [sigspec]");
}

void kill_liste_signaux(void){
	int i,j;
	for(i=1, j=1; i < NSIG; i++, j++){
		printf("%2d) SIG%s\t", i, majuscule(sys_signame[i]));
		if(j == 4){
			printf("\n");
			j=1;
		}
	}
	printf("\n");
}

int kill_(char ** arguments){
  int signal_default = SIGTERM;
  int argc = LongueurListe(arguments);
  if(argc < 2)
    kill_usage();
  switch(argc){
  case 2:
    if(strcmp(arguments[1], "-l") == 0)
      kill_liste_signaux();
    else
      kill(atoi(arguments[argc-1]), signal_default);
    break;
  case 3:
  	if(arguments[1][0], '-')
    	kill(atoi(arguments[argc-1]), kill_no_signal(arguments[1]+1));
    else
    	kill_usage();
    break;
  case 4:
    if(strcmp(arguments[1], "-s") == 0 || strcmp(arguments[1], "-n") == 0)
      kill(atoi(arguments[argc-1]), kill_no_signal(arguments[2]));
    else
      kill_usage();
    break;
   default:
   	kill_usage();
  }
  return 0;
}

int main(int argc, char ** argv){
	kill_(argv);
	return 0;
}
