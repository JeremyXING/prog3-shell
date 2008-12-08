/* Construction des arbres repr�sentant des commandes */

#include "Shell.h"
#include "analyse_expression.h"

/*
 * Construit une expression � partir de sous-expressions
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
 * Renvoie une liste d'arguments, la premi�re case �tant initialis�e � NULL, la
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
 * Ajoute en fin de liste le nouvel argument et renvoie la liste r�sultante
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
 * Fonction appel�e lorsque l'utilisateur tape "".
 */
void EndOfFile (void)
{
  exit (0);
} /* EndOfFile */

/*
 * Appel�e par yyparse() sur erreur syntaxique
 */
void yyerror (char *s)
{
   fprintf(stderr, "%s\n", s);
}


int
main (int argc, char **argv) 
{
  system("clear");
  initialiser_fichier();
  while (1){
    if (yyparse () == 0){
      Expression * e = ExpressionAnalysee;
      //      if (fork()==0) => voir fichier analyse_expression.c ligne 146
      interpreter(e);
      //analyse_cmd(e);
      //printf("arbre : \n");
      //arbre(e);
      
      /*--------------------------------------------------------------------------------------.
      | L'analyse de la ligne de commande est effectu�e sans erreur.  La variable globale     |
      |       ExpressionAnalysee pointe sur un arbre repr�sentant l'expression.  Le type      |
      |       "Expression" de l'arbre est d�crit dans le fichier Shell.h. Il contient 4       |
      |       champs. Si e est du type Expression :					      |
      | 										      |
      | - e.type est un type d'expression, contenant une valeur d�finie par �num�ration dans  |
      |   Shell.h. Cette valeur peut �tre :					      	      |
      | 										      |
      |   - VIDE, commande vide								      |
      |   - SIMPLE, commande simple et ses arguments					      |
      |   - SEQUENCE, s�quence (;) d'instructions					      |
      |   - SEQUENCE_ET, s�quence conditionnelle (&&) d'instructions			      |
      |   - SEQUENCE_OU, s�quence conditionnelle (||) d'instructions			      |
      |   - BG, t�che en arri�re plan (&)						      |
      |   - PIPE, pipe (|).								      |
      |   - REDIRECTION_I, redirection de l'entr�e (<)					      |
      |   - REDIRECTION_O, redirection de la sortie (>)					      |
      |   - REDIRECTION_A, redirection de la sortie en mode APPEND (>>).		      |
      |   - REDIRECTION_E, redirection de la sortie erreur,  	   			      |
      |   - REDIRECTION_EO, redirection des sorties erreur et standard.			      |
      | 										      |
      | - e.gauche et e.droite, de type Expression *, repr�sentent une sous-expression gauche |
      |       et une sous-expression droite. Ces deux champs ne sont pas utilis�s pour les    |
      |       types VIDE et SIMPLE. Pour les expressions r�clamant deux sous-expressions      |
      |       (SEQUENCE, SEQUENCE_ET, SEQUENCE_OU, et PIPE) ces deux champs sont utilis�s     |
      |       simultann�ment.  Pour les autres champs, seule l'expression gauche est	      |
      |       utilis�e.									      |
      | 										      |
      | - e.arguments, de type char **, a deux interpretations :			      |
      | 										      |
      |      - si le type de la commande est simple, e.arguments pointe sur un tableau � la   |
      |       argv. (e.arguments)[0] est le nom de la commande, (e.arguments)[1] est le	      |
      |       premier argument, etc.							      |
      | 										      |
      |      - si le type de la commande est une redirection, (e.arguments)[0] est le nom du  |
      |       fichier vers lequel on redirige.						      |
      `--------------------------------------------------------------------------------------*/
    }
    else {
      /* L'analyse de la ligne de commande a donn� une erreur */
    }
  }
  return 0;
}