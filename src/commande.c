#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commande.h"

void  pwd(char ** arguments){
  long size;
  char *buf;
  char *ptr;
  
  size = pathconf(".", _PC_PATH_MAX);
  if ((buf = (char *)malloc((size_t)size)) != NULL)
    {
      ptr = getcwd(buf, (size_t)size);
    }
  
  printf("%s\n", ptr);

}

void  cd(char ** arguments){
  printf("Commande cd\n");
}
