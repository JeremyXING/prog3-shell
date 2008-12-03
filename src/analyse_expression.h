#ifndef ANALYSE_EXPRESSION
#define ANALYSE_EXPRESSION

#include "Shell.h"


#define NB_FONCTION 11

typedef int pid_t;
extern char * nom_fonction[NB_FONCTION]; //tableau contenant les noms des fonctions builtin
extern int status;

extern void interpreter(Expression * e);
extern void afficher_prompt(void);
extern int initialiser_fichier(void);


#endif /* ANALYSE_EXPRESSION */

