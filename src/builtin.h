#ifndef BUILTIN_H
#define BUILTIN_H

#include <stdbool.h>

extern bool check_builtin(char * cmd);
extern void cd(char * directory);
extern void pwd(void);

#endif /* BUILTIN */
