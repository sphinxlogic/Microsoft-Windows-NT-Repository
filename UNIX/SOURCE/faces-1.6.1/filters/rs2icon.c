
/*  @(#)rs2icon.c 1.6 91/04/15
 *
 *  Small program to take a rasterfile on standard input and generate
 *  an icon on standard output. There are two options:
 *
 *  -x nnn - Displacement in the x direction before starting to extract
 *           the icon information.
 *
 *  -y nnn - Displacement in the y direction before starting to extract
 *           the icon information.
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
#ifdef SYSV32 || hpux
#include <string.h>
#else
#include <strings.h>
#endif /* SYSV32 || hpux */

#define  PATCHLEVEL  2

/* Taken from: @(#)rasterfile.h 1.11 89/08/21 SMI. */

struct rasterfile {
        int     ras_magic;              /* magic number */
        int     ras_width;              /* width (pixels) of image */
        int     ras_height;             /* height (pixels) of image */
        int     ras_depth;              /* depth (1, 8, or 24 bits) of pixel */
        int     ras_length;             /* length (bytes) of image */
        int     ras_type;               /* type of file; see RT_* below */
        int     ras_maptype;            /* type of colormap; see RMT_* below */
        int     ras_maplength;          /* length (bytes) of following map */
        /* color map follows for ras_maplength bytes, followed by image */
};

#define  FPRINTF     (void) fprintf  /* To make lint happy. */
#define  FREAD       (void) fread
#define  PRINTF      (void) printf
#define  STRCPY      (void) strcpy

#define  MAXLINE     80           /* Maximum length for character strings. */

unsigned char pic[1152/8][900] ;  /* Buffer for rasterfile. */
char progname[MAXLINE] ;          /* The name of this program. */
int x ;                           /* X displacement for start of extraction. */
int y ;                           /* Y displacement for start of extraction. */

main(argc, argv)
int argc ;
char *argv[] ;
{
  STRCPY(progname, argv[0]) ;    /* Save program name for later use. */
  get_options(argc, argv) ;      /* Extract command line options. */
  read_raster() ;                /* Read rasterfile to internal buffer. */
  make_icon() ;                  /* Turn rasterfile portion into an icon. */
}


get_options(argc, argv)          /* Extract command line options. */
int argc ;
char *argv[] ;
{
  x = 0 ;                        /* Default is no x displacement. */
  y = 0 ;                        /* Default is no y displacement. */
  argv++ ;
  argc-- ;
  while (argc > 0)
    {
      switch (argv[0][1])
        {
          case 'x' : argv++ ;    /* X displacement value given. */
                     argc-- ;
                     x = atoi(*argv) ;
                     break ;
          case 'y' : argv++ ;    /* Y displacement value given. */
                     argc-- ;
                     y = atoi(*argv) ;
                     break ;
          case 'v' : FPRINTF(stderr, "%s version 1.2.%1d\n",
                                     progname, PATCHLEVEL) ;
                     break ;
          case '?' : FPRINTF(stderr, "Usage: %s [-x] [-y] [-v] [-?]\n",
                                     progname) ;
                     exit(1) ;
        }
      argc-- ;
      argv++ ;
    }
}


make_icon()        /* Create icon from rasterfile on standard output. */
{
  unsigned char line[8] ;   /* 64 bits of the current line of the rasterfile. */
  int i, j, k, n, val ;

  PRINTF("/* Format_version=1, Width=64, Height=64, Depth=1, ") ;
  PRINTF("Valid_bits_per_item=16\n */\n") ;
  n = 0 ;
  for (i = y; i < (y+64); i++)
    {
      for (j = x; j < (x+64); j += 8)
        {
          val = 0 ;
          for (k = 0; k < 8; k++)
            val = (val << 1) + ((pic[(j+k)/8][i] >> (7 - ((j+k) % 8))) & 1) ;
          line[(j - x) / 8] = val ;
        }
      if (!(n % 2)) PRINTF("\t") ;
      for (j = 0; j < 8; j += 2) PRINTF("0x%02x%02x,", line[j], line[j+1]) ;
      if (n % 2) PRINTF("\n") ;
      n++ ;
    }
}

read_raster()                    /* Read rasterfile into internal buffer. */
{
  int i, j ;
  int height, len ;
  struct rasterfile header ;

  FREAD((char *) &header,sizeof(struct rasterfile),1,stdin) ;
  for (i = 0; i < header.ras_maplength; i++) j = getchar() ;
  len = ((header.ras_width+15) >> 4) * 2 ;
  height = header.ras_height ;
  for (i = 0; i < height; i++)
    for (j = 0; j < len; j++) pic[j][i] = getchar() ;
}
