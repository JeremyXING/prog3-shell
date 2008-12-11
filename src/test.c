#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int status = 0;

int LongueurListe(char **l)
{
  char **p;
  for (p=l; *p != NULL; p++)
    ;
  return p-l;
}

int lire_contenu_variable(const char* variable_env){

  ssize_t taille_ligne;
  size_t len = 0;
  char * ligne = NULL;

  int longueur = strlen(variable_env);

  FILE * fichier = fopen(".profile", "r");
  if(fichier == NULL){
    perror(".profile");
    return 1;
  }
  
  int numero_ligne = 1;
  while ((taille_ligne = getline(&ligne, &len, fichier)) != -1) {

    if(strncmp(variable_env, ligne, longueur) == 0){
      printf("%s", strrchr(ligne, '=')+1);
      break;
    }
    numero_ligne++;
  }

  if(taille_ligne == -1){
    fprintf(stderr, "Variable %s inexistante !\n", variable_env);
    return 1;
  }
  
  if (ligne)
    free(ligne);

  fclose(fichier);
  return 0;
}

int echo(char ** arguments){
  if(LongueurListe(arguments) == 1)
    return 0;
  else{
    int i;
    for(i = 1; i < LongueurListe(arguments); i++)//pour chaque arguments
      switch(arguments[i][0]){
      case '$':
	lire_contenu_variable(arguments[i]+1);
	break;
      case '\"':
	printf("%s\n", strndup(arguments[i]+1, strlen(arguments[i])-2));
	break;
      case '\'':
	printf("%s\n", strndup(arguments[i]+1, strlen(arguments[i])-2));
	break;
      default:
	printf("%s\n", arguments[i]);
	break;
      }
  }
  return 0;
}

char * lire_contenu_variable_v2(const char * variable_env){

  if(strcmp(variable_env, "?") == 0){
    if(status == 0)
      return "0"; else return "1";
  }

  FILE * fichier = fopen(".profile", "r");

  if(fichier == NULL){
    perror("");
    return NULL;
  }

  char buffer[64];
  size_t len = strlen(variable_env);
  int retour;

  while((retour = fscanf(fichier, "%s", buffer)) != EOF)
    {
      if(strncmp(buffer, variable_env, len) == 0){
	return (strrchr(buffer, '=')+1);
      }
    } 

  if(retour == EOF){
    fprintf(stderr, "Variable %s inexistante !\n", variable_env);
    return NULL;
  }
  
  printf("\n");  
  free(buffer);
  fclose(fichier);
  return NULL;
}

int initialiser_fichier(void){
  
  FILE * fichier = fopen(".profile", "r");

  if(fichier != NULL)
    return 0;
  else {
    fichier = fopen(".profile", "a+");

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

    fwrite(var_user, strlen(var_user) * sizeof(char), 1, fichier);
    fwrite(user, strlen(user) * sizeof(char), 1, fichier);
    fwrite(retour, strlen(retour) * sizeof(char), 1, fichier);

    fwrite(var_pwd, strlen(var_pwd) * sizeof(char), 1, fichier);
    fwrite(ptr, strlen(ptr) * sizeof(char), 1, fichier);
    fwrite(retour, strlen(retour) * sizeof(char), 1, fichier);

    free(buf);
  }

  
  fclose(fichier);
  return 0;
}
int main(int argc, char* argv[]){

  printf("%s\n", "huhu");

  return EXIT_SUCCESS;
}
