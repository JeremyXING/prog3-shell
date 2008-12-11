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

static char * commande = NULL;
static int longueur = 0;
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

int executer_cmd(Expression * e);
void analyse_cmd(Expression * e);
void arbre(Expression * e);

int ecrire_history(Expression *e){

  longueur = 0;
  commande = NULL;
  arbre(e);

  char * chemin = malloc((strlen(home) + strlen("/history.tmp") + 1) * sizeof(char));
  chemin = memcpy(chemin, home, (strlen(home) + 1) * sizeof(char));
  chemin = strncat(chemin, "/history.tmp", strlen("/history.tmp"));

  int fichier = open(chemin, O_WRONLY | O_CREAT | O_APPEND, 0644);
  free(chemin);

  if(fichier == -1){
    perror("");
    return 1;
  }

  write(fichier, commande, strlen(commande) * sizeof(char));
  write(fichier, "\n", sizeof(char));

  close(fichier);  
  free(commande);
    return 0;
}

static void tube(Expression * gauche, Expression * droite){
  int tube[2];
  pipe(tube);
  switch(fork()){
  case 0://fils
    close(tube[0]);
    dup2(tube[1], 1);
    close(tube[1]);
    if(gauche->type == SIMPLE){
      executer_cmd(gauche);
      exit(1);
    }
    else
      analyse_cmd(gauche);
    break;
  default://pere
    wait(NULL);
    close(tube[1]);
    dup2(tube[0],0);
    close(tube[0]);
    if(droite->type == SIMPLE){
      executer_cmd(droite);
      exit(1);
    }
    else
      analyse_cmd(droite);
    break;
  }
}

void redirection_stdout(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_TRUNC, 0664);
  if(fd == -1){
    perror(e->arguments[0]);
    exit(1);
  }
  dup2(fd, 1);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void redirection_stdin(Expression * e){
  int fd = open(e->arguments[0], O_RDONLY, 0664);
  if(fd == -1){
    perror(e->arguments[0]);
    exit(1);
  }
  dup2(fd, 0);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void redirection_stdout_append(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_APPEND, 0664);
  if(fd == -1){
    perror(e->arguments[0]);
    exit(1);
  }
  dup2(fd, 1);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void redirection_stderr(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_TRUNC, 0664);
  if(fd == -1){
    perror("");
    exit(1);
  }
  dup2(fd, 2);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void sequence(Expression * e){
  analyse_cmd(e->gauche);
  wait(NULL);
  analyse_cmd(e->droite);
  wait(NULL);
}

void redirection_stdout_stderr(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_TRUNC, 0664);
  dup2(fd, 1);
  dup2(fd, 2);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void sequence_et(Expression * e)
{
  interpreter(e->gauche);
  wait(NULL);
  if(status == 0 ){
    interpreter(e->droite);
  }
}

void sequence_ou(Expression * e)
{
  interpreter(e->gauche);
  wait(NULL);
  if(status != 0 )
      interpreter(e->droite);
}

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
    printf("Seules les commandes SIMPLES sont execut�es.\n");
    break;
  }
}

void afficher_prompt(void){

  char * chemin = malloc((strlen(home) + strlen("/.profile") + 1) * sizeof(char));
  chemin = memcpy(chemin, home, (strlen(home) + 1) * sizeof(char));
  chemin = strncat(chemin, "/.profile", strlen("/.profile"));
  
  FILE * fichier = fopen(chemin, "r");
  free(chemin);
  
  if(fichier == NULL){
    perror(".profile");
    return ;
  }
  
  char * buffer = malloc(64 * sizeof(char));
  char * save = buffer;
  size_t len = strlen("USER");
  int retour;
  
  while((retour = fscanf(fichier, "%s", buffer)) != EOF)
    {
      if(strncmp(buffer, "USER", len) == 0){
	printf("%s", strrchr(buffer, '=')+1);
	break;
      }
    } 
  free(save);
  fclose(fichier);
  printf(" - %d > ", status);
}

void arbre(Expression * racine){ // parcours infixe
  if(racine != NULL){
    arbre(racine->gauche); //gauche
    switch(racine->type){ // racine
    case VIDE:
      break;
    case SIMPLE:
      longueur += (1 + strlen(racine->arguments[0]));
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande,racine->arguments[0]);
      commande = strcat(commande, " ");

      int i = 1;
      while(racine->arguments[i] != NULL){
	longueur += (1 + strlen(racine->arguments[i]));
	commande = realloc(commande, longueur * sizeof(*commande));
	commande = strcat(commande,racine->arguments[i]);
	commande = strcat(commande, " ");
	i++;
      }
      break;
    case SEQUENCE:
      longueur += 2;
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "; ");
      break;
    case SEQUENCE_ET:
      longueur += 3;
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "&& ");
      break;
    case SEQUENCE_OU:
      longueur += 3;
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "|| ");
      break;
    case BG:
      longueur += 1;
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "&");
      break;
    case PIPE:
      longueur += 2;
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "| ");
      break;
    case REDIRECTION_I:
      longueur += (3 + strlen(racine->arguments[0]));
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "< ");
      commande = strcat(commande,racine->arguments[0]);
      commande = strcat(commande, " ");
      break;
    case REDIRECTION_O:
      longueur += (3 + strlen(racine->arguments[0]));
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "> ");
      commande = strcat(commande,racine->arguments[0]);
      commande = strcat(commande, " ");
      break;
    case REDIRECTION_A:
      longueur += (4 + strlen(racine->arguments[0]));
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, ">> ");
      commande = strcat(commande,racine->arguments[0]);
      commande = strcat(commande, " ");
      break;
    case REDIRECTION_E:
      longueur += (4 + strlen(racine->arguments[0]));
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "2> ");
      commande = strcat(commande,racine->arguments[0]);
      commande = strcat(commande, " ");
      break;
    case REDIRECTION_EO:
      longueur += (4 + strlen(racine->arguments[0]));
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "&> ");
      commande = strcat(commande,racine->arguments[0]);
      commande = strcat(commande, " ");
      break;
    default:
      printf("inconnue ");
      break;
    }
    arbre(racine->droite); // droite
  }
}

int initialiser_fichier(void){

  long size;
  char *buf;
  char *ptr;
  size = pathconf(".", _PC_PATH_MAX);
  if ((buf = (char *)malloc((size_t)size)) != NULL)
    {
      ptr = getcwd(buf, (size_t)size);
    }
  free(buf);
  
  home = malloc(strlen(ptr) * sizeof(char));
  home = memcpy(home, ptr, strlen(ptr) * sizeof(char));

  FILE * fichier = fopen(".profile", "r");

  if(fichier != NULL){
    fclose(fichier);
    char * arguments[2] = {"", "."};
    cd(arguments);
    return 0;
  }
  else {
    FILE * fichier = fopen(".profile", "w+");
    /*CODE PWD*/
    long size;
    char *buf;
    char *ptr;
    size = pathconf(".", _PC_PATH_MAX);
    if ((buf = (char *)malloc((size_t)size)) != NULL)
     	ptr = getcwd(buf, (size_t)size);
    /*FIN CODE PWD*/


    /*CODE USER*/
    char * user = NULL;
    user = getenv("USER");
    if(user == NULL)
      strcpy(user, "unknown_user");
    /*FIN CODE USER*/


    char * retour = "\n";

    char * var_user = "USER=";
    char * var_pwd = "PWD=";
    char * var_home = "HOME=";
        
    
    fwrite(var_user, strlen(var_user) * sizeof(char), 1, fichier);
    fwrite(user, strlen(user) * sizeof(char), 1, fichier);
    fwrite(retour, strlen(retour) * sizeof(char), 1, fichier);

    fwrite(var_home, strlen(var_home) * sizeof(char), 1, fichier);
    fwrite(home, strlen(home) * sizeof(char), 1, fichier);
    fwrite(retour, strlen(retour) * sizeof(char), 1, fichier);

    fwrite(var_pwd, strlen(var_pwd) * sizeof(char), 1, fichier);
    fwrite(ptr, strlen(ptr) * sizeof(char), 1, fichier);
    fwrite(retour, strlen(retour) * sizeof(char), 1, fichier);

    
    free(buf);
    fclose(fichier);
    return 0;
  }
  return 0;
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
