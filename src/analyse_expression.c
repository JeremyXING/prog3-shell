#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include "analyse_expression.h"
#include "commande.h"
#include "Shell.h"
#include "expression.h"
#include "shell_system.h"


int status;
char * home = NULL;

/*----------- Tableau des fonctions internes(commande.h) ---------- */
char * nom_fonction[NB_FONCTION] = { "pwd", "cd", "history", "builtins", 
				     "kill", "times", "exit", "pushd", 
				     "popd", "dirs", "echo", "alias", 
				     "unalias", "printenv", "umask" };
fonction  tableau_fonction[NB_FONCTION] = { pwd, cd , history, builtins, 
					    kill_, times_, toexit, pushd, 
					    popd, dirs, echo, alias_, 
					    unalias_, printenv, umask_};
/*-----------------------------------------------------------------*/

int executer_cmd(Expression * e){
  if(alias_rechercherAlias(e->arguments[0]) != -1){
    alias_(e->arguments);
  }
  else {
    bool trouver = false;
    char * nom_commande = e->arguments[0];
    int pid;
    for(int i=0; i < NB_FONCTION; i++){
      if( strcmp(nom_fonction[i], nom_commande) == 0 ){
	trouver = true;
	fonction f = tableau_fonction[i];
	status = (f)(e->arguments);
	break;
      }
    }
    if(! trouver){
      if((pid = fork()) == 0){
	execvp(e->arguments[0], e->arguments );
	fprintf(stderr, "%s : command not found\n", e->arguments[0]);
	exit(1);
      }
      waitpid(pid,&status, 0);
    }
  }
  return 0;
}

void analyse_cmd(Expression * e){
  switch(e->type){
  case VIDE:
    afficher_prompt();
    break;
  case SIMPLE:
    executer_cmd(e);
    break;
  case PIPE:
    tube(e->gauche, e->droite);
    break;
  case REDIRECTION_O:
    redirection_stdout(e);
    break;
  case REDIRECTION_I:
    redirection_stdin(e);
    break;
  case REDIRECTION_E:
    redirection_stderr(e);
    break;
  case REDIRECTION_A:
    redirection_stdout_append(e);
    break;
  case SEQUENCE:
    sequence(e);
    break;
  case SEQUENCE_ET:
    sequence_et(e);
    break;
  case SEQUENCE_OU:
    sequence_ou(e);
    break; 
  case REDIRECTION_EO:
    redirection_stdout_stderr(e);
    break;
  default:
    printf("Seules les commandes SIMPLES sont executées.\n");
    break;
  }
}
void interpreter(Expression * e){
  //ecrire_history(e);

  if(e->type == SIMPLE || e->type == VIDE || e->type == SEQUENCE_ET || e->type == SEQUENCE_OU || e->type == SEQUENCE)
    analyse_cmd(e);
  else if(fork() == 0)
    analyse_cmd(e);
  wait(NULL);
  afficher_prompt();
}
