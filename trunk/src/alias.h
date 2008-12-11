#ifndef ALIAS
#define ALIAS

struct alias{
  char * src;
  char * dst;
};

typedef struct alias *alias;


extern void alias_ajouterAlias(alias a);
extern void alias_supprimerAlias(int pos_alias);
extern alias alias_expressionToAlias(char * expr);
extern int alias_rechercherAlias(char * src);
extern void alias_afficherAlias();
extern char * alias_getDst(int pos);

#endif /* ALIAS */

