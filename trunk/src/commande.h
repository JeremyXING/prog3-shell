#ifndef COMMANDE_H
#define COMMANDE_H

typedef int (*fonction)(char **); 

extern int  pwd(char ** arguments);
extern int  cd(char ** arguments);


#endif /* COMMANDE_H */
