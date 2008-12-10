#ifndef ANALYSE_EXPRESSION
#define ANALYSE_EXPRESSION

#include "Shell.h"


#define NB_FONCTION 13

typedef int (*fonction)(char **); 

typedef int pid_t;
extern char * nom_fonction[NB_FONCTION]; //tableau contenant les noms des fonctions builtin
extern fonction tableau_fonction[NB_FONCTION];
extern int status;
extern char* home;

extern void interpreter(Expression * e);
extern void afficher_prompt(void);
extern int initialiser_fichier(void);
extern int executer_cmd(Expression * e);


#endif /* ANALYSE_EXPRESSION */

