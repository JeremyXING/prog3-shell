/* Construction des arbres représentant des commandes */

#include <signal.h>


#include "Shell.h"
#include "analyse_expression.h"

/*
 * Construit une expression à partir de sous-expressions
 */
Expression *ConstruireNoeud (expr_t type, Expression *g, Expression *d, char **args)
{
  Expression *e;
     
  if ((e = (Expression *)malloc(sizeof(Expression))) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  e->type   = type;
  e->gauche = g;
  e->droite = d;
  e->arguments = args;
  return e;
} /* ConstruireNoeud */



/*
 * Renvoie la longueur d'une liste d'arguments
 */
int LongueurListe(char **l)
{
  char **p;
  for (p=l; *p != NULL; p++)
    ;
  return p-l;
} /* LongueurListe */



/*
 * Renvoie une liste d'arguments, la première case étant initialisée à NULL, la
 * liste pouvant contenir NB_ARGS arguments (plus le pointeur NULL de fin de
 * liste)
 */
char **InitialiserListeArguments (void)
{
  char **l;
  
  l = (char **) (calloc (NB_ARGS+1, sizeof (char *)));
  *l = NULL;
  return l;
} /* InitialiserListeArguments */



/*
 * Ajoute en fin de liste le nouvel argument et renvoie la liste résultante
 */
char **AjouterArg (char **Liste, char *Arg)
{
  char **l;
  
  l = Liste + LongueurListe (Liste);
  *l = (char *) (malloc (1+strlen (Arg)));
  strcpy (*l++, Arg);
  *l = NULL;
  return Liste;
} /* AjouterArg */



/*
 * Fonction appelée lorsque l'utilisateur tape "".
 */
void EndOfFile (void)
{
  exit (0);
} /* EndOfFile */

/*
 * Appelée par yyparse() sur erreur syntaxique
 */
void yyerror (char *s)
{
   fprintf(stderr, "%s\n", s);
}

/*void
f_sigint(int s)
{
  // kill(getpid(), SIGINT);
  printf("Ctrl-c \n");
}
*/
int
main (int argc, char **argv) 
{
  system("clear");
  initialiser_fichier();
  afficher_prompt();

  /*struct sigaction s;
  s.sa_handler = f_sigint;
  sigemptyset(&s.sa_mask);     
  s.sa_flags = 0;
  sigaction(SIGINT, &s, NULL);*/


  while (1){
    if (yyparse () == 0){
      Expression * e = ExpressionAnalysee;
      interpreter(e);
    }
    else {
      status = 1;
      afficher_prompt();
    }
  }
  return 0;
}
