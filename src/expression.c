#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "Shell.h"
#include "expression.h"
#include "analyse_expression.h"

void tube(Expression * gauche, Expression * droite){
  int tube[2];
  pipe(tube);
  switch(fork()){
  case 0://fils
    close(tube[0]);
    dup2(tube[1], 1);
    close(tube[1]);
    if(gauche->type == SIMPLE){
      executer_cmd(gauche);
      exit(1);
    }
    else
      analyse_cmd(gauche);
    break;
  default://pere
    wait(NULL);
    close(tube[1]);
    dup2(tube[0],0);
    close(tube[0]);
    if(droite->type == SIMPLE){
      executer_cmd(droite);
      exit(1);
    }
    else
      analyse_cmd(droite);
    break;
  }
}

void redirection_stdout(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_TRUNC, 0664);
  if(fd == -1){
    perror(e->arguments[0]);
    exit(1);
  }
  dup2(fd, 1);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void redirection_stdin(Expression * e){
  int fd = open(e->arguments[0], O_RDONLY, 0664);
  if(fd == -1){
    perror(e->arguments[0]);
    exit(1);
  }
  dup2(fd, 0);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void redirection_stdout_append(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_APPEND, 0664);
  if(fd == -1){
    perror(e->arguments[0]);
    exit(1);
  }
  dup2(fd, 1);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void redirection_stderr(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_TRUNC, 0664);
  if(fd == -1){
    perror("");
    exit(1);
  }
  dup2(fd, 2);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void redirection_stdout_stderr(Expression * e){
  int fd = open(e->arguments[0], O_CREAT | O_WRONLY | O_TRUNC, 0664);
  dup2(fd, 1);
  dup2(fd, 2);
  analyse_cmd(e->gauche);
  analyse_cmd(e->droite);
  close(fd);
}

void sequence(Expression * e){
  analyse_cmd(e->gauche);
  wait(NULL);
  analyse_cmd(e->droite);
  wait(NULL);
}

void sequence_et(Expression * e)
{
  interpreter(e->gauche);
  wait(NULL);
  if(status == 0 ){
    interpreter(e->droite);
  }
}

void sequence_ou(Expression * e)
{
  interpreter(e->gauche);
  wait(NULL);
  if(status != 0 )
      interpreter(e->droite);
}
