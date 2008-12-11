#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "Shell.h"
#include "analyse_expression.h"
#include "commande.h"
#include "shell_system.h"
#include "echo.h"

static char * commande = NULL;
static int longueur = 0;


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

void afficher_prompt(void){

  lire_contenu_variable("USER");
  printf("- %d > ", status);

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
    ptr = getcwd(buf, (size_t)size);

  home = malloc(strlen(ptr) * sizeof(char));
  home = memcpy(home, ptr, strlen(ptr) * sizeof(char));
  
  FILE * fichier = fopen(".profile", "r");

  if(fichier != NULL){ //Si le fichier existe deja
    fclose(fichier);
    char * arguments[2] = {"", "."};
    cd(arguments);
    return 0;
  }
  else {  //Sinon on le creer
    FILE * fichier = fopen(".profile", "w+");

    char * user = NULL;
    user = getenv("USER");
    if(user == NULL)
      strcpy(user, "unknown_user");
    
    char * retour = "\n";

    char * var_user = "USER=";
    char * var_pwd = "PWD=";
    char * var_home = "HOME=";
        

    /* On écrit dans ".profile" les variables PWD, USER et HOME */
    
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
    free(user);
    fclose(fichier);
    return 0;
  }
  return 0;
}
