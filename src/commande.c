#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/times.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <assert.h>

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

static int lire_contenu_variable(const char* variable_env){

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

static char * tab_signame[32] = {"SIGNAL 0", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP",
				 "SIGABRT", "SIGEMT", "SIGFPE", "SIGKILL", "SIGBUS","SIGSEGV",
				 "SIGSYS", "SIGPIPE", "SIGALRM", "SIGTERM", "SIGURG", "SIGSTOP",
				 "SIGTSTP", "SIGCONT", "SIGCHLD", "SIGTTIN", "SIGTTOU", "SIGIO",
				 "SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO",
				 "SIGUSR1", "SIGUSR2"};

#define NB_SIG  32


static int kill_no_signal(char * sig){
  if(atoi(sig) != 0)
    return atoi(sig);
  for(int i=1; i < NB_SIG; i++)
    if( strcmp(sig, tab_signame[i]) == 0 )
      return i;
  printf("Signal inexistant.\n");
  return -1;
  
}

static int kill_usage(void){
  printf("Usage: kill [-s sigspec | -n signum | -sigspec] pid\n");
  printf("       kill -l [sigspec]\n");
  return 1;
}

void kill_liste_signaux(void){
  int i,j;
  for(i=1, j=1; i < NB_SIG; i++, j++){
    printf("%2d) %s\t", i, tab_signame[i]);
    if(j == 4){
      printf("\n");
      j=0;
    }
  }
  printf("\n");
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
  else{
    char s[1000];
    while( fgets(s, 1000 ,file) != NULL)
      system(s);
    return 0;
  }
}

/**************************alias*******************/

struct alias{
  char * src;
  char * dst;
};

typedef struct alias *alias;

static alias * tab_alias = NULL;
static int nb_alias = 0;

void alias_modifierAlias(int no_alias, char *dst){
  free(tab_alias[no_alias]->dst);
  tab_alias[no_alias]->dst = dst;
}

int alias_rechercherAlias(char * src){
  for(int i=0; i<nb_alias; i++)
    if(strcmp(src, tab_alias[i]->src)==0)
      return i;
  return -1;
}

void alias_ajouterAlias(alias a){
  int pos;
  if((pos=alias_rechercherAlias(a->src)) != -1)//si l'alias existe
    alias_modifierAlias(pos, a->dst);
  else{
    nb_alias++;
    tab_alias = realloc(tab_alias, nb_alias*sizeof(*tab_alias));
    assert(tab_alias);
    tab_alias[nb_alias-1] = a;
  }
}

void alias_supprimerAlias(int pos_alias){
  free(tab_alias[pos_alias]->src);
  free(tab_alias[pos_alias]->dst);
  free(tab_alias[pos_alias]);
  tab_alias[pos_alias] = tab_alias[nb_alias-1];
  nb_alias--;
}

alias alias_newAlias(char * src, char * dst){
  alias new = malloc(sizeof(*new));
  assert(new);
  new->src = src;
  new->dst = dst;
  return new;
}

char * chaine_dup(char * src){
  if(src != NULL){
    int taille = strlen(src);
    char * dst = malloc(taille*sizeof(*dst));
    assert(dst);
    for(int i=0; i<taille; i++)
      dst[i]=src[i];
    dst[taille] = '\0';
    return dst;
  }
  else
    return NULL;
}

alias alias_expressionToAlias(char * expr){
  char * s = strchr(expr, '=');
  char *dst;
  if(s == NULL)
    return NULL;
  else
    dst = chaine_dup(s)+1;

  int taille = strlen(expr) - strlen(dst);
  char * src = malloc(taille*sizeof(*src));
  strncpy(src, expr, taille-1); src[taille-1]='\0';
  alias a = alias_newAlias(src, dst);
  return a;

}

void alias_afficherAlias(){
  for(int i=0; i<nb_alias; i++)
    printf("%s = %s \n", tab_alias[i]->src, tab_alias[i]->dst);
}

int alias_(char ** arguments){
  int argc = LongueurListe(arguments);
  alias a;
  int pos;
  if(argc == 0)
    alias_afficherAlias();
  else
    for(int i=1; i<argc; i++){
      a = alias_expressionToAlias(arguments[i]);
      if(a != NULL){
	alias_ajouterAlias(a);
      }
      else if ((pos=alias_rechercherAlias(arguments[i])) != -1)
	printf("alias %s='%s'\n", tab_alias[pos]->src, tab_alias[pos]->dst);
      else
	printf("alias %s inexistant\n", arguments[i]);

    }
  return 0;
}
