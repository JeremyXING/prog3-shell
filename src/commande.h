#ifndef COMMANDE_H
#define COMMANDE_H

typedef void (*fonction)(char **); 

extern void  ls(char ** arguments);
extern void  cd(char ** arguments);

#endif /* COMMANDE_H */
