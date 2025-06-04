#include <stdio.h>
#ifdef VMS
#include <string.h>
#else
#include <strings.h>
#endif /* VMS */
#include "struct.h"

#define return_fail(str) { \
  fprintf(stderr,"%s\n",str); \
  return(1); \
}

load_board(file)
     char *file;
{
  FILE *fp;
  char line[1024],*tok;
  int x_board_size,y_board_size,xpos,ypos;

  if (!(fp = fopen(file,"r"))) {
    fprintf(stderr,"Could not open board file %s\n",file);
    return(1);
  }

  /* Read board size */
  fgets(line,1024,fp);
  if (!(tok= (char *) strtok(line,TOKS)))
    return_fail("Could not read board size");
  x_board_size = atoi(tok);
  if (!(tok=(char *) strtok(NULL,TOKS)))
    return_fail("Could not read board size");
  y_board_size = atoi(tok);

  if (allocate_board(x_board_size,y_board_size))
    return_fail("Invalid board size");

  /* Now for the obstructions */
  for (fgets(line,1024,fp),fgets(line,1024,fp);
       (tok= (char *) strtok(line,TOKS));fgets(line,1024,fp)) {
    xpos = atoi(tok);
    if (!(tok=(char *) strtok(NULL,TOKS)))
      return_fail("Invalid pair in obstructions");
    ypos = atoi(tok);
    if (add_obstruction(xpos,ypos))
      return_fail("Obstruction out of range");
  }

  return(0);
}
