#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>

#include "umask.h"
#include "commande.h"
#include "analyse_expression.h"
#include "alias.h"
#include "kill.h"
#include "echo.h"

char ** tab_dir = NULL;
int nb_dir = -1;


int  cd(char ** arguments){
  int retour = chdir(arguments[1]);
  if(retour == -1){
    fprintf(stderr, "%s : Aucun dossier de ce type\n", arguments[1]);
    return 1;
  }

  long size;
  char *buf;
  char *ptr;
  size = pathconf(".", _PC_PATH_MAX);
  if ((buf = (char *)malloc((size_t)size)) != NULL)
    {
      ptr = getcwd(buf, (size_t)size);
    }

  char * chemin = malloc((strlen(home) + strlen("/.profile") + 1) * sizeof(char));
  chemin = memcpy(chemin, home, (strlen(home) + 1) * sizeof(char));
  chemin = strncat(chemin, "/.profile", strlen("/.profile"));
  
  FILE * fichier = fopen(chemin, "r+");
  FILE * fichier_tmp = fopen("file.tmp", "w");
  
  
  if(fichier == NULL){
    fprintf(stderr, "Fichier \".profile\" introuvable\n");
    return 1;
  }
  if(fichier_tmp == NULL){
    perror("");
    return 1;
  }

  char * retour_ligne = "\n";  
  char ligne[256];

  char * var_pwd = "PWD=";
  while(fgets(ligne, sizeof(ligne), fichier)){
    if(strncmp(ligne, var_pwd, strlen(var_pwd)) != 0)
      fputs(ligne, fichier_tmp);
  }

  fwrite(var_pwd, strlen(var_pwd) * sizeof(char), 1, fichier_tmp);
  fwrite(ptr, strlen(ptr) * sizeof(char), 1, fichier_tmp);
  fwrite(retour_ligne, strlen(retour_ligne) * sizeof(char), 1, fichier_tmp);

  rename("file.tmp", chemin);
  
  free(buf);
  free(chemin);
  fclose(fichier);
  fclose(fichier_tmp);
  return 0;
}

int history(char ** arguments){
  
  
  char * chemin = malloc((strlen(home) + strlen("/history.tmp") + 1) * sizeof(char));
  chemin = memcpy(chemin, home, (strlen(home) + 1) * sizeof(char));
  chemin = strncat(chemin, "/history.tmp", strlen("/history.tmp"));
  
  
  FILE * fichier;
  fichier = fopen(chemin, "r");
  free(chemin);
  
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

int toexit(char ** arguments){
  printf("Exiting...\n");
  printf("Bye bye !\n");
  sleep(1);
  system("clear");
  exit(EXIT_SUCCESS);
}

int times_(char ** arguments){

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

int echo(char ** arguments){
  int retour = 0;
  if(LongueurListe(arguments) == 1)
    return 0;
  else{
    int i;
    char * buf = NULL;
    for(i = 1; i < LongueurListe(arguments); i++)//pour chaque arguments
      switch(arguments[i][0]){
      case '$':
	retour = lire_contenu_variable(arguments[i]+1);
	break;
      case '\"':
	buf = malloc((strlen(arguments[i])-2) * sizeof(char));
	buf = strncpy(buf, arguments[i]+2, strlen(arguments[i])-3);
	retour = lire_contenu_variable(buf);
	free(buf);
	break;
      case '\'':
	buf = malloc((strlen(arguments[i])-2) * sizeof(char));
	buf = strncpy(buf, arguments[i]+1, strlen(arguments[i])-2);
	printf("%s ", buf);
	free(buf);
	break;
      default:
	printf("%s ", arguments[i]);
	break;
      }
  }
  printf("\n");
  return retour;
  return 0;
}

int  pwd(char ** arguments){
  int retour = lire_contenu_variable("PWD");
  printf("\n");
  return retour;
}

int kill_(char ** arguments){
  int retour = 0;
  int signal_default = 15;
  int argc = LongueurListe(arguments);
  if(argc < 2)
    retour = kill_usage();
  switch(argc){
  case 2:
    if(strcmp(arguments[1], "-l") == 0)
      kill_liste_signaux();
    else
      if(atoi(arguments[argc-1]) != 0)
	retour = kill(atoi(arguments[argc-1]), signal_default);
      else
	retour = kill_usage();
    break;
  case 3:
    if(arguments[1][0] == '-')
      retour = kill(atoi(arguments[argc-1]), kill_no_signal(arguments[1]+1));
    else
      retour = kill_usage();
    break;
  case 4:
    if(strcmp(arguments[1], "-s") == 0 || strcmp(arguments[1], "-n") == 0)
      retour = kill(atoi(arguments[argc-1]), kill_no_signal(arguments[2]));
    else
      retour = kill_usage();
    break;
  }
  return abs(retour);
}

int source(char ** arguments){
 int argc = LongueurListe(arguments);
 if(argc < 2){
   fprintf(stderr, "usage: source filename \n");
   return 1;
 }
  FILE * file = fopen(arguments[1], "r");
  if (file == NULL){
    perror(arguments[1]);
    return 1;
  }
  char s[1000];
  while( fgets(s, 1000 ,file) != NULL)
    system(s);
  return 0;
}

int printenv(char ** arguments){
  char * chemin = malloc((strlen(home) + strlen("/.profile") + 1) * sizeof(char));
  chemin = memcpy(chemin, home, (strlen(home) + 1) * sizeof(char));
  chemin = strncat(chemin, "/.profile", strlen("/.profile"));
  
  FILE * fichier = fopen(chemin, "r");

  if (fichier == NULL){
    perror(arguments[1]);
    return 1;
  }
  char s[1000];
  while( fgets(s, 1000 ,fichier) != NULL)
    printf("%s", s);

  fclose(fichier);
  return 0;
}

/**************************alias*******************/


int alias_(char ** arguments){
  int pid;
  int argc = LongueurListe(arguments);
  alias a;
  int pos;
  if(argc == 1)
    if((pos = alias_rechercherAlias(arguments[0])) != -1){
      if((pid = fork()) == 0){
	execlp(alias_getDst(pos), alias_getDst(pos), NULL);
	fprintf(stderr, "%s : command not found\n",alias_getDst(pos));
	exit(1);
      }
      waitpid(pid,&status, 0);
    }
    else
      alias_afficherAlias();    
  else
    for(int i=1; i<argc; i++){
      a = alias_expressionToAlias(arguments[i]);
      if(a != NULL)
	alias_ajouterAlias(a);
      else if ((pos=alias_rechercherAlias(arguments[i])) != -1)
	printf("alias %s='%s'\n", arguments[i], alias_getDst(pos));
      else{
	printf("alias %s inexistant\n", arguments[i]);
      }
    }
  return 0;
}

int unalias_(char ** arguments){
  int pos;
  int argc = LongueurListe(arguments);
  if(argc == 1)
    fprintf(stderr,"usage: unalias name [name ...]\n");
  for(int i=1; i<argc; i++){
    pos=alias_rechercherAlias(arguments[i]);
    if(pos != -1)
      alias_supprimerAlias(pos);
    else
      printf("unalias: %s non trouv�\n",arguments[i]);
  }
  return 0;
}

int
umask_(char **arguments)
{
  int argc=LongueurListe(arguments);
  if(argc == 1)
    {
      um = umask(0022); //avoir le vieux umask
      umask(um);
      printf("%04o \n", um);
    }
  else
    {
      if(!strcmp(arguments[1],"-S"))
        {
          print_symbolic_umask();
        }
      else if(isalpha(arguments[1][1]))
        printf("N'est pas implemente\n");
      else if(isdigit(arguments[1][1]))
        verifier_et_appliquer_umask(arguments[1]);
      

      else
        umask(0777);
    }
  return 0;
}

