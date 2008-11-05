#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

void 
perror_and_exit_whenever(int assertion)
{
  if (assertion)
    {
      perror("");
      exit(EXIT_FAILURE);
    }
}


void 
mirror(char *out, char *in, size_t length)
{
  int i,j;
  
  for(i=0,j = length - 1; j >= 0 ; i++,j--)
    out[j] = in[i];
}

int 
main(int argc, char **argv)
{
  int input, output, power;
  size_t buffer_size, nbslice;
  int slice;
  char *buffer, *mirrored_buf;
  ssize_t wcount, rcount;
  int file_length;
  struct stat file_attributes;


  if (argc != 4)
    {
      fprintf(stderr,"%s : <input file name>"
	      " <output filename> <log2 of buffer's length> \n", argv[0]);
      return EXIT_FAILURE;
    }
  
  input = open(argv[1], O_RDONLY);
  perror_and_exit_whenever(input < 0);
  fstat (input, &file_attributes);

  output = open(argv[2], O_WRONLY | O_CREAT);
  perror_and_exit_whenever(output < 0);
  ftruncate(output,0);  

  power = strtoul(argv[3], NULL, 10);
  buffer_size = 1 << power;
  buffer = malloc(buffer_size);
  mirrored_buf = malloc(buffer_size);
  

  nbslice = file_attributes.st_size / buffer_size;
  
  
  file_length = file_attributes.st_size;
  
  for (slice = nbslice; slice >= 0; slice--)
    {
      lseek(input, slice * buffer_size,SEEK_SET);
      rcount = read(input, buffer, buffer_size);
      mirror(mirrored_buf, buffer,rcount);
      wcount = write(output,mirrored_buf,rcount);      
      perror_and_exit_whenever(rcount != wcount);
    }
  
  close(input);
  close(output);

  return EXIT_SUCCESS;
}
