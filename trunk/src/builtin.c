#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

#define SIZE_MAX_CURRENT_DIR 50
#define NB_CMD_BUILTIN 2

char * cmd_builtin[NB_CMD_BUILTIN] = {"pwd", "cat"};

bool check_builtin(char * cmd){
  for(int i = 0; i < NB_CMD_BUILTIN; i++)
    if((strcmp(cmd, cmd_builtin[i])) == 0)
      return true;
  return false;
}
  

void cd(char * directory){

}

void pwd(void){

  size_t size_path = sizeof(SIZE_MAX_CURRENT_DIR);
  char * path = malloc(size_path * sizeof(char));
  getcwd(path, size_path);
  printf("\n%s\n", path);
  free(path);
}
