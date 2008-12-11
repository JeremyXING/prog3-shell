#ifndef UMASK_H
#define UMASK_H


#include <sys/types.h>
#include <sys/stat.h>

extern mode_t um;


extern void verifier_et_appliquer_umask(char *argument);
extern void print_symbolic_umask (void);

#endif /* UMASK */
