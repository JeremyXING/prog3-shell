#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include "commande.h"
#include "analyse_expression.h"

int  pwd(char ** arguments){
  long size;
  char *buf;
  char *ptr;
  
  size = pathconf(".", _PC_PATH_MAX);
  if ((buf = (char *)malloc((size_t)size)) != NULL)
    {
      ptr = getcwd(buf, (size_t)size);
    }
  
  printf("%s\n", ptr);

  return 0;

}

int  cd(char ** arguments){
  
  int retour = chdir(arguments[1]);//c'est ce que j'ai trouver comme appel systeme,
                      //ça marche
  //printf("Commande cd\n");

  if(retour == -1){
    fprintf(stderr, "%s : Aucun dossier de ce type\n", arguments[1]);
    return 1;
  }
  else return 0;
}

int history(char ** arguments){

  FILE * fichier;
  fichier = fopen("history.tmp", "r");
  if(fichier == NULL){
    fprintf(stderr, "Aucun historique sauvegardé\n");
    return 1;
  }

  int c;
  
  while ((c = getc(fichier)) != EOF)
    putc(c, stdout);
  fclose(fichier);
  fflush(stdout);
  
  return 0;
}

int builtins(char ** arguments){
  
  printf("Commande builtins : ");
  int i;
  for(i = 0; i < NB_FONCTION; i++)
    printf("%s ", nom_fonction[i]);
  printf("\n");
  return 0;
}

int killer(char ** arguments){
  return 0;
}

int toexit(char ** arguments){
  return 0;
}

int times(char ** arguments){
  return 0;
}

