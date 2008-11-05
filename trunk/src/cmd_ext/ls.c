#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

void ls(char * rep){
  DIR * repertoire = opendir(rep);
  struct dirent * ent;
  while((ent = readdir(repertoire)) != NULL)
    printf("%s \n", ent->d_name);
  closedir(repertoire);
}

int main(int argc, char* argv[]){

  if(argc == 1)
    ls(".");
  else if(argc == 2)
    ls(argv[1]);
  else {
    fprintf(stderr, "Usage : %s [repertoire]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
