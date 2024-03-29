%{
#include "Shell.h"
extern int yylex(void);
%}

%union yystype {
  Expression *Expr;
  char       **ListeArgs;
  char	     identificateur[TAILLE_ID];
}

%token <identificateur> IDENTIFICATEUR
%nonassoc '&'
%left ';' ET OU
%left '|'
%token IN OUT OUT_APPEND ERR ERR_OUT

%type <Expr> expression
%type <ListeArgs> commande
%type <ListeArgs> fichier

%%
lignecommande	: expression '\n' 
		    {
  		      ExpressionAnalysee = $1;
  		      YYACCEPT;
		    }
		| error '\n'
		    {
  		      yyclearin;
		      YYABORT;
		    }
		;

expression	: 		    
		    {
  			$$ = ConstruireNoeud (VIDE, NULL, NULL, NULL);
		    }
		| commande
		    {
  			$$ = ConstruireNoeud (SIMPLE, NULL, NULL, $1);
		    }
		| expression ';' expression 
		    {
  		      $$ = ConstruireNoeud (SEQUENCE, $1, $3, NULL);
		    }
		| expression ET  expression 
		    {
  		      $$ = ConstruireNoeud (SEQUENCE_ET, $1, $3, NULL);
		    }
		| expression OU  expression
		    {
  		      $$ = ConstruireNoeud (SEQUENCE_OU, $1, $3, NULL);
		    }
		| expression '|'  expression
		    {
  		      $$ = ConstruireNoeud (PIPE, $1, $3, NULL);
		    }
		| expression IN  fichier
		    {
  		      $$ = ConstruireNoeud (REDIRECTION_I, $1, NULL, $3);
		    }
	  	| expression OUT fichier
		    {
  		      $$ = ConstruireNoeud (REDIRECTION_O, $1, NULL, $3);
		    }
	  	| expression ERR fichier
		    {
  		      $$ = ConstruireNoeud (REDIRECTION_E, $1, NULL, $3);
		    }
	  	| expression ERR_OUT fichier
		    {
  		      $$ = ConstruireNoeud (REDIRECTION_EO, $1, NULL, $3);
		    }
	       	| expression OUT_APPEND fichier
		    {
  		      $$ = ConstruireNoeud (REDIRECTION_A, $1, NULL, $3);
		    }
		| expression '&'
		    {
  		      $$ = ConstruireNoeud (BG, $1, NULL, NULL);
		    }
		| '(' expression ')'
		    {
  		      $$ = $2;
		    }
                ;

fichier		: IDENTIFICATEUR
		    {
  		      char **p = InitialiserListeArguments ();
  		      $$ = AjouterArg (p, yylval.identificateur);
		    }
		;

commande	: IDENTIFICATEUR
		    {
  		      char **p = InitialiserListeArguments ();
  		      $$ = AjouterArg (p, yylval.identificateur);
		    }
		| commande IDENTIFICATEUR
		    {
		      $$ = AjouterArg ($1,yylval.identificateur);
		    }
		;
%%

