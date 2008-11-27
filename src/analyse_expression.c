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


/*----------- Tableau des fonctions internes(commande.h) ---------- */
char * nom_fonction[NB_FONCTION] = { "pwd", "cd", "history", "builtins", 
				     "kill", "times", "exit", "pushd", "popd" };
fonction  tableau_fonction[NB_FONCTION] = { pwd, cd , history, builtins, 
					    killer, times, toexit, pushd, popd};
/*-----------------------------------------------------------------*/

int executer_cmd(Expression * e);

int ecrire_history(char ** arguments){
  int argc = 10; //LongueurListe(arguments);
  int fichier = open("history.tmp", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if(fichier == -1){
    perror("");
    return 1;
  }
  int i;
  char * espace = " ";
  for(i = 0; i < argc; i++){
    write(fichier, arguments[i], strlen(arguments[i]) * sizeof(char));
    write(fichier, espace, sizeof(char));
  }  
  char * back = "\n";
  write(fichier, back, sizeof(char));
  close(fichier);  
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
  close(fd);
}

void sequence(Expression * e){
  analyse_cmd(e->gauche);
  wait(NULL);
  analyse_cmd(e->droite);
  wait(NULL);
}

/*void redirection_stdout_stderr(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_TRUNC, 0664);
  dup2(fd, 0);
  dup2(fd, 2);
  analyse_cmd(e->gauche);
  close(fd);
  }*/

int executer_cmd(Expression * e){
  bool trouver = false;
  char * nom_commande = e->arguments[0];
  for(int i=0; i < NB_FONCTION; i++){
    if( strcmp(nom_fonction[i], nom_commande) == 0 ){
      trouver = true;
      fonction f = tableau_fonction[i];
      (f)(e->arguments); 
      break;
    }
  }
  if(! trouver){
    if(fork() == 0){
      execvp(e->arguments[0], e->arguments );
      fprintf(stderr, "%s : command not found\n", e->arguments[0]);
    }
  }
  return 0;
}

void analyse_cmd(Expression * e){
  //ecrire_history(e->arguments);
  switch(e->type){
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
  default:
    printf("Seules les commandes SIMPLES sont executées.\n");
    break;
  }
}

void afficher_prompt(int retour){
  char * user = NULL;
  user = getenv("USER");
  if(user == NULL)
    user = "unknown_user";
  long size;
  char *buf;
  char *ptr;
  size = pathconf(".", _PC_PATH_MAX);
  if ((buf = (char *)malloc((size_t)size)) != NULL)
    {
      ptr = getcwd(buf, (size_t)size);
    }
  if(retour == 1)
    retour = 1;
  else retour = 0;
  
  printf("%s [%s] %d -$ ", user, ptr, retour);
}



void arbre_2(Expression * racine){ // parcours infixe
  if(racine != NULL){
    arbre_2(racine->gauche); //gauche
    switch(racine->type){ // racine
    case VIDE:
      //printf("vide ");
      break;
    case SIMPLE:
      longueur += strlen(racine->arguments[0]);
      commande = realloc(commande, (longueur+1)*sizeof(*commande));
      commande = strcat(commande,racine->arguments[0]);
      commande = strcat(commande, " ");
      //printf("%s \n", commande);
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
      longueur += 5;
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "< %s ");
      break;
    case REDIRECTION_O:
      longueur += 5;
      commande = realloc(commande, longueur * sizeof(*commande));
      commande = strcat(commande, "> %s ");
      break;
    case REDIRECTION_A:
      printf(">> %s ",racine->arguments[0]);
      break;
    case REDIRECTION_E:
      printf("2> %s ",racine->arguments[0]);
      break;
    case REDIRECTION_EO:
      printf("&> %s ",racine->arguments[0]);
      break;
    default:
      printf("inconnue ");
      break;
    }
    

    arbre_2(racine->droite); // droite
  }
}

void arbre(Expression * e){
  longueur = 0;
  commande = NULL;
  arbre_2(e);
  printf("%s \n", commande);
  free(commande);
}

void interpreter(Expression * e){
  //  ecrire_history();
  //  afficher_prompt();

  if(e->type == SIMPLE)
    analyse_cmd(e);
  else if(fork() == 0)
    analyse_cmd(e);
}
