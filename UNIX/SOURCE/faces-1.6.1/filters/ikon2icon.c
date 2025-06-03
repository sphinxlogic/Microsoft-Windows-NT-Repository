
/*  @(#)ikon2icon.c 1.6 91/04/15
 *
 *  Small program to take a blit icon on standard input and convert
 *  it to Sun icon format on standard output.
 *
 *  Copyright (c) Rich Burridge - Sun Microsystems Australia.
 * 
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 *
 *  No responsibility is taken for any errors on inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  to me, then an attempt will be made to fix them.
 */

#include <stdio.h>

#define  PATCHLEVEL  1

#define  GETS        (void) gets     /* To make lint happy. */
#define  PRINTF      (void) printf
#define  SPRINTF     (void) sprintf
#define  SSCANF      (void) sscanf

#ifdef  NOINDEX
#define  index        strchr
#endif /*NOINDEX*/

#define  BLITHEIGHT  48       /* Maximum number of lines in a blit icon. */
#define  MAXLINE     80       /* Maximum length for character strings. */

char *index() ;
unsigned char buf[6][48] ;    /* Buffer for blit icon info. */

main()
{
  read_blit() ;      /* Read blit info to internal buffer. */
  make_icon() ;      /* Turn into Sun icon format. */
}


make_icon()        /* Create Sun icon on standard output. */
{
  int i, j ;

  PRINTF("/* Format_version=1, Width=64, Height=64, Depth=1, ") ;
  PRINTF("Valid_bits_per_item=16\n */\n") ;
  for (i = 0; i < BLITHEIGHT; i++)
    {
      if (!(i % 2)) PRINTF("\t") ;
      for (j = 0; j < 6; j += 2)
        PRINTF("0x%02x%02x,", buf[j][i], buf[j+1][i]) ;
      PRINTF("0x0000,") ;
      if (i % 2) PRINTF("\n") ;
    }
  for (i = 0; i < 8; i++)
    PRINTF("\t0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,\n") ;
}

read_blit()                         /* Read rasterfile into internal buffer. */
{
  char nextline[MAXLINE], *ptr ;    /* Next line from blit icon file. */
  int i, j, v0, v1 ;

  for (i = 0; i < BLITHEIGHT; i++)
    {
      GETS(nextline) ;
      ptr = nextline ;
      for (j = 0; j < 3; j++)
        {
          while (*ptr == ' ') ptr++ ;
          SSCANF(ptr,"0x%02x%02x",&v0,&v1) ;
          buf[j*2][i] = v0 ;
          buf[j*2+1][i] = v1 ;
          ptr = index(ptr, ',') ;
          ptr++ ;
        }
    }
}
