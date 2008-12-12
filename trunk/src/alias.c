#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "analyse_expression.h"
#include "Shell.h"
#include "alias.h"

static alias * tab_alias = NULL;
static int nb_alias = 0;

static void alias_modifierAlias(int no_alias, char *dst){
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

static char * chaine_dup(char * src){
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

static alias alias_newAlias(char * src, char * dst){
  alias new = malloc(sizeof(*new));
  assert(new);
  new->src = chaine_dup(src);
  new->dst = chaine_dup(dst);
  return new;
}



alias alias_expressionToAlias(char * expr){
  char * s = strchr(expr, '=');
  char *dst;
  if(s == NULL)
    return NULL;
  else
    dst = s+1;

  int taille = strlen(expr) - strlen(dst);
  char * src = malloc(taille*sizeof(*src));
  strncpy(src, expr, taille-1); src[taille-1]='\0';
  alias a = alias_newAlias(src, dst);
  free(src);
  return a;

}

void alias_afficherAlias(){
  for(int i=0; i<nb_alias; i++)
    printf("%s = %s \n", tab_alias[i]->src, tab_alias[i]->dst);
}

char * alias_getDst(int pos){
  return tab_alias[pos]->dst;
}

void
remplacer_alias_(char *argument, Expression *e)
{
  char *valeur= alias_getDst(alias_rechercherAlias(argument));
  int nb_arg =LongueurListe(e->arguments);
  char **nouv_arg=malloc(sizeof(nouv_arg)*nb_arg);
  nouv_arg[0]=chaine_dup(valeur);
  for(int i = 1; i < nb_arg; i++)
    nouv_arg[i]=chaine_dup(e->arguments[i]);
  interpreter( ConstruireNoeud(e->type, e->gauche, e->droite, nouv_arg));
}
