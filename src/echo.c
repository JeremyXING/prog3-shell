#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include "echo.h"
#include "analyse_expression.h"

int lire_contenu_variable(const char* variable_env){

   if(strcmp(variable_env, "?") == 0){
     printf("%d", status);
     return 0;
   }
   
   char * chemin = malloc((strlen(home) + strlen("/.profile") + 1) * sizeof(char));
   chemin = memcpy(chemin, home, (strlen(home) + 1) * sizeof(char));
   chemin = strncat(chemin, "/.profile", strlen("/.profile"));

   FILE * fichier = fopen(chemin, "r");
   free(chemin);

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
	 printf("%s ", strrchr(buffer, '=')+1);
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
