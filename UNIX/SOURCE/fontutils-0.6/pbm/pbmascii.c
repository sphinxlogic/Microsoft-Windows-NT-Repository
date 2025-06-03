/* pbmascii -- dump a PBM file (from stdin) on stdout.  */

#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char *argv[])
{
  int width, height, format;
  unsigned row;
  unsigned char *image_row;
  
  pbm_readpbminit (stdin, &width, &height, &format);
  
  image_row = malloc (width);
  
  for (row = 0; row < height; row++)
    {
      int c;

      pbm_readpbmrow (stdin, image_row, width, format);
      
      printf ("%5d:", row);
      for (c = 0; c < width; c++)
        putchar (image_row[c] ? '*' : ' ');
      putchar ('\n');
    }
  
  return 0;
}


/*
Local variables:
compile-command: "gcc -g -posix -o pbmascii pbmascii.c pbm.a"
End:
*/
