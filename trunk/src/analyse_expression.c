#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "analyse_expression.h"
#include "Commande.h"



/*----------- Tableau des fonctions (Commande.h) ---------- */
#define NB_FONCTION 2
char * nom_fonction[NB_FONCTION] = { "ls", "cd" };
fonction  tableau_fonction[NB_FONCTION] = { ls, cd };


/*Recherche la commande dans le tableau des fonctions et l'execute si elle existe*/
void executer_cmd(Expression * e){
  bool trouver = false;
  char * nom_commande = e->arguments[0];

  for(int i=0; i < NB_FONCTION; i++){
    if( strcmp(nom_fonction[i], nom_commande) == 0 ){
      trouver = true;
      fonction f = tableau_fonction[i];
      (f)(e->arguments); // appel de la fonction
      break;
    }
  }

  if(! trouver)
    printf("Commande inconnue.\n");
}

/* Analyse de l'expression */
void analyse_cmd(Expression * e){
  switch(e->type){
  case SIMPLE:
    executer_cmd(e); // Dans le cas d'une commande simple, on l'execute.
    break;
  default:
    printf("Seules les commandes SIMPLES sont executées.\n");
    break;
  }
}


