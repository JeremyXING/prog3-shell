#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>


#define BUFF 1024

int
main(int argc, char *argv[])
{
  FILE *input;
  char *buffer;
  if (argc != 2)
    {
      fprintf(stderr,"%s : <input file name> \n", argv[0]);
      return EXIT_FAILURE;
    }
  input = fopen(argv[1], O_RDONLY);
  if(input< 0)
    perror("input");
  
  buffer = malloc(BUFF*sizeof(buffer));
  while (fread(buffer,sizeof(char *),sizeof(buffer),input) != 0)
    fwrite(buffer,sizeof(char *),sizeof(buffer),stdout); 
  
  return 0; 
}
