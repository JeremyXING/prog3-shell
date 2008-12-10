#ifndef COMMANDE_H
#define COMMANDE_H



extern int pwd(char ** arguments);
extern int cd(char ** arguments);
extern int history(char ** arguments);
extern int builtins(char ** arguments);
extern int kill_(char ** arguments);
extern int times_(char ** arguments);
extern int toexit(char ** arguments);
extern int pushd(char ** arguments);
extern int popd(char ** arguments);
extern int dirs(char ** arguments);
extern int echo(char ** arguments);
extern int source(char ** arguments);
extern int alias_(char ** arguments);

#endif /* COMMANDE_H */
