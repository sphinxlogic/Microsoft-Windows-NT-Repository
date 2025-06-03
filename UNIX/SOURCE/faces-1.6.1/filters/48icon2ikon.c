
/*  @(#)icon2ikon.c 1.6 91/10/24
 *
 *  Small program to take a Sun icon on standard input and convert
 *  the top-leftmost 48x48 pixels to a blit ikon on standard output.
 *
 *  Copyright (c) Rich Burridge - Sun Microsystems Australia.
 * 
 *  NOTE: Currently only supports 48x48x1 Sun icons.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 *
 *  No responsibility is taken for any errors on inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  to me, then an attempt will be made to fix them.
 */

#include <stdio.h>
#if defined(SYSV32) || defined(hpux)
#include <string.h>
#else
#include <strings.h>
#endif /* SYSV32 || hpux */

#define  PATCHLEVEL  2

#define  FPRINTF     (void) fprintf     /* To make lint happy. */
#define  PRINTF      (void) printf
#define  SSCANF      (void) sscanf
#define  STRCPY      (void) strcpy

#define  BLITHEIGHT  48     /* Maximum number of lines in a blit icon. */
#define  MAXLINE     80     /* Maximum length for character strings. */

char progname[MAXLINE] ;    /* The name of this program. */
unsigned short buf[192] ;   /* Buffer for Sun icon info. */


/*ARGSUSED*/
main(argc, argv)
int argc ;
char *argv[] ;
{
  STRCPY(progname, argv[0]) ;   /* Save for later use. */
  if (!get_sun_icon(buf))       /* Read Sun icon into internal buffer. */
    make_blit_ikon() ;          /* Turn into Blit ikon format. */
}


get_sun_icon(buf)     /* Load Sun icon file. */
unsigned short buf[192] ;
{
  char htype[MAXLINE] ;     /* Current header comment parameter. */
  int c ;                   /* Count of items found from fscanf call. */
  int comment ;             /* Set if more initial comment to read. */
  int count ;               /* Number of items to read from icon file. */
  int idepth ;              /* Depth of this icon. */
  int iheight ;             /* Height of this icon. */
  int iwidth ;              /* Width of this icon. */
  int ivbpi ;               /* Number of valid bits per item. */
  int temp ;                /* Temporary location for latest 16 bits. */
 
  comment = 1 ;
  while (comment)
    {
      if (scanf("%*[^WHDV*]%s", htype) == EOF) break ;
      switch (htype[0])
        {
          case 'W' : SSCANF(htype, "Width=%d", &iwidth) ;
                     if (iwidth != 48)
                       {
                         FPRINTF(stderr, "%s: icon has width %d\n",
                                          progname, iwidth) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;                         return -1 ;
                       }
                     break ;
          case 'H' : SSCANF(htype, "Height=%d", &iheight) ;
                     if (iheight != 48)
                       {
                         FPRINTF(stderr, "%s: icon has height %d\n",
                                          progname, iheight) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;                         return -1 ;
                       }
                     break ;
          case 'D' : SSCANF(htype, "Depth=%d", &idepth) ;
                     if (idepth != 1)
                       {
                         FPRINTF(stderr, "%s: icon has depth %d\n",
                                          progname, idepth) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;                         return -1 ;
                       }
                     break ;
          case 'V' : SSCANF(htype, "Valid_bits_per_item=%d", &ivbpi) ;
                     if (ivbpi != 16)
                       {
                         FPRINTF(stderr, "%s: icon has %d bits per item\n",
                                          progname, ivbpi) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;                         return -1 ;
                       }
                     break ;
          case '*' : if (htype[1] == '/') comment = 0 ;
        }
    }    

  count = 0 ;
  while (count < ((iheight * iwidth) / 16))
    {
      c = scanf(" 0x%X,", &temp) ;
      if (c == 0 || c == EOF) break ;
      buf[count++] = (short) temp ;
    }
  return(0) ;
}


make_blit_ikon()        /* Create Blit ikon on standard output. */
{
  int i ;

  for (i = 0; i < BLITHEIGHT; i++)
    PRINTF("0x%04x,0x%04x,0x%04x,\n", buf[i*3], buf[i*3+1], buf[i*3+2]) ;
}
