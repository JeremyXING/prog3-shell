#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "umask.h"


mode_t um;

void
print_symbolic_umask ()
 
{
  char ubits[4], gbits[4], obits[4];		/* u=rwx,g=rwx,o=rwx */
  int i;

  i = 0;
  if ((um & S_IRUSR) == 0)
    ubits[i++] = 'r';
  if ((um & S_IWUSR) == 0)
    ubits[i++] = 'w';
  if ((um & S_IXUSR) == 0)
    ubits[i++] = 'x';
  ubits[i] = '\0';

  i = 0;
  if ((um & S_IRGRP) == 0)
    gbits[i++] = 'r';
  if ((um & S_IWGRP) == 0)
    gbits[i++] = 'w';
  if ((um & S_IXGRP) == 0)
    gbits[i++] = 'x';
  gbits[i] = '\0';

  i = 0;
  if ((um & S_IROTH) == 0)
    obits[i++] = 'r';
  if ((um & S_IWOTH) == 0)
    obits[i++] = 'w';
  if ((um & S_IXOTH) == 0)
    obits[i++] = 'x';
  obits[i] = '\0';

  printf ("u=%s,g=%s,o=%s\n", ubits, gbits, obits);
}


void
verifier_et_appliquer_umask(char *argument)
{
  int um_ =0;
  int i=0;
  while(argument[i] && argument[i]>='0' && argument[i] < '8')
    {
      um_=(um_*8) + argument[i]-'0';
      i++;
    }
  if(!i || um_ > 0777 || argument[i])
    fprintf(stderr," %s : Nombre octal hors plage \n",argument);
  else
    umask(um_);
}
