#define _GNU_SOURCE


#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>


void 
perror_and_exit_whenever(int assertion)
{
  if (assertion)
    {
      perror("");
      exit(EXIT_FAILURE);
    }
}

int 
main(int argc, char **argv)
{
  int input, output, power;
  size_t buffer_size;
  char *buffer;
  ssize_t wcount, rcount;

  if (argc != 4)
    {
      fprintf(stderr,"%s : <input file name>"
	      " <output filename> <log2 of buffer length> \n", argv[0]);
      return EXIT_FAILURE;
    }
  
  input = open(argv[1], O_RDONLY);
  perror_and_exit_whenever(input < 0);
  //    posix_fadvise(input,0,0,POSIX_FADV_NOREUSE); // pour le fun



  output = open(argv[2], O_WRONLY | O_CREAT);
  perror_and_exit_whenever(output < 0);
  ftruncate(output,0);  

  power = strtoul(argv[3], NULL, 10);
  buffer_size = 1 << power;
  buffer = malloc(buffer_size);

  do{
    rcount = read(input, buffer, buffer_size);
    wcount = write(output,buffer,rcount);      
    perror_and_exit_whenever(rcount != wcount);
  } while (rcount == buffer_size);
  
  close(input);
  close(output);

  return EXIT_SUCCESS;
}
