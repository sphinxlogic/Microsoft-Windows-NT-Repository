#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main(int argc, char **argv)
{
  struct stat filetime1;
  struct stat filetime2;
  int i;

  if (argc < 3)
  {
    printf(
"Usage: newer ref file1 ...\nPrints all files newer than ref.\n");
    exit(0);
  }

  filetime1.st_mtime = 0;
  filetime2.st_mtime = 0;

  if (stat(argv[1], &filetime1) == -1)
    { }
/*    exit(0); */

  for (i = 2; i < argc; i++)
  {
    if (stat(argv[i], &filetime2) == 0)
    {
      if (filetime2.st_mtime > filetime1.st_mtime)
      printf("%s ", argv[i]);
    }
  }
  printf("\n");
}
