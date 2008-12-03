#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>

#include "commande.h"
#include "analyse_expression.h"

char ** tab_dir = NULL;
int nb_dir = -1;

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

    free(buf);
    return 0;
}

int  cd(char ** arguments){
  int retour = chdir(arguments[1]);
  if(retour == -1){
    fprintf(stderr, "%s : Aucun dossier de ce type\n", arguments[1]);
    return 1;
  }
  
  /*
  long size;
  char *buf;
  char *ptr;
  size = pathconf(".", _PC_PATH_MAX);
  if ((buf = (char *)malloc((size_t)size)) != NULL)
    {
      ptr = getcwd(buf, (size_t)size);
    }
  free(buf);

  char * chemin_fichier = malloc((strlen(ptr) + strlen(".profile")) * sizeof(char));
  strcpy(chemin_fichier, ptr);
  strcat(chemin_fichier, ".profile");
  printf("%s\n", chemin_fichier);
  FILE * fichier = fopen(chemin_fichier, "r+");
  
  free(chemin_fichier);

  if(fichier == NULL){
    fprintf(stderr, "Fichier \".profile\" introuvable\n");
    return 1;
   }

   char * buffer = malloc(64 * sizeof(char));
   char * save = buffer;

   while((retour = fscanf(fichier, "%s", buffer)) != EOF)
     {
       if(strncmp(buffer, "PWD=", strlen("PWD=")) == 0){
	 printf("%s\n", strrchr(buffer, '=')+1);
	 break;
       }
     }
   free(save);
   fclose(fichier);*/
   return 0;
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
  printf("Kill ca marche \n");
  return 0;
}

int toexit(char ** arguments){
  printf("Exiting...\n");
  printf("Bye bye !\n");
  sleep(1);
  system("clear");
  exit(EXIT_SUCCESS);
}

int times(char ** arguments){
  return 0;
}

int dirs(char ** arguments){
  if(nb_dir < 0){
    printf("Pile : [\n");
    printf("Pile de dossier vide\n");
  }
  else{
    printf("Pile : [");
    int i;
    for(i = 0; i <= nb_dir; i++)
      printf("%s ", tab_dir[i]);
    printf("\n");
  }
  return 0;
}

int pushd(char ** arguments){
  
  if(arguments[1] == NULL){
    fprintf(stderr, "Pas de dossier donné en argument...\n");
    return 1;
  }

  if(opendir(arguments[1]) == NULL){
    fprintf(stderr, "%s : ", arguments[1]);
    perror("");
    return 1;
  }

  nb_dir++;
  tab_dir = realloc(tab_dir, (nb_dir + 1) * sizeof(char*));
  tab_dir[nb_dir] = realloc(tab_dir[nb_dir], strlen(arguments[0]) * sizeof(char));
  strcpy(tab_dir[nb_dir], arguments[1]);
  
  return 0;
}

int popd(char ** arguments){
  if(nb_dir == -1){
    fprintf(stderr, "Pile de dossier vide\n");
    return 1;
  }
  free(tab_dir[nb_dir]);
  nb_dir--;

  return 0;
}

static int lire_contenu_variable(const char* variable_env){

   if(strcmp(variable_env, "?") == 0){
     printf("%d", status);
     return 0;
   }

   FILE * fichier = fopen(".profile", "r");

   if(fichier == NULL){
     perror(".profile");
     return 1;
   }

   char * buffer = malloc(64 * sizeof(char));
   char * save = buffer;
   size_t len = strlen(variable_env);
   int retour;

   while((retour = fscanf(fichier, "%s", buffer)) != EOF)
     {
       if(strncmp(buffer, variable_env, len) == 0){
	 printf("%s\t", strrchr(buffer, '=')+1);
	 break;
       }
     } 

   if(retour == EOF){
     fprintf(stderr, "Variable %s inexistante !", variable_env);
     free(save);
     return 1;
   }

   free(save);
   fclose(fichier);
   return 0;
 }

int echo(char ** arguments){
  int retour = 0;
  if(LongueurListe(arguments) == 1)
    return 0;
  else{
    int i;
    for(i = 1; i < LongueurListe(arguments); i++)//pour chaque arguments
      switch(arguments[i][0]){
      case '$':
	retour = lire_contenu_variable(arguments[i]+1);
	break;
      case '\"':
	printf("%s ", strndup(arguments[i]+1, strlen(arguments[i])-2));
	break;
      case '\'':
	printf("%s ", strndup(arguments[i]+1, strlen(arguments[i])-2));
	break;
      default:
	printf("%s ", arguments[i]);
	break;
      }
  }
  printf("\n");
  return retour;
}
