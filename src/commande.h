#ifndef COMMANDE_H
#define COMMANDE_H

typedef int (*fonction)(char **); 

extern int pwd(char ** arguments);
extern int cd(char ** arguments);
extern int history(char ** arguments);
extern int builtins(char ** arguments);
extern int killer(char ** arguments);
extern int times(char ** arguments);
extern int toexit(char ** arguments);

#endif /* COMMANDE_H */
