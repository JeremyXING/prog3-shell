#include <stdio.h>

int
main()
{
  char c;


  while (fread(&c,1,1,stdin) == 1)
    fwrite(&c,1,1,stdout); 

  return 1; 
}
