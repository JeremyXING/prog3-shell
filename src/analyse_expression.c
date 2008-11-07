#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "analyse_expression.h"
#include "commande.h"



/*----------- Tableau des fonctions internes(commande.h) ---------- */
#define NB_FONCTION 2
char * nom_fonction[NB_FONCTION] = { "pwd", "cd" };
fonction  tableau_fonction[NB_FONCTION] = { pwd, cd };


/*Recherche la commande dans le tableau des fonctions internes et l'execute si elle existe*/
void executer_cmd(Expression * e){
  bool trouver = false;
  char * nom_commande = e->arguments[0];

  for(int i=0; i < NB_FONCTION; i++){
    if( strcmp(nom_fonction[i], nom_commande) == 0 ){
      trouver = true;
      fonction f = tableau_fonction[i];
      (f)(e->arguments); // appel de la fonction interne
      break;
    }
  }

  if(! trouver){
    if(fork() == 0){
      execlp(e->arguments[0], NULL, NULL);
      perror("");
    }
  }
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


