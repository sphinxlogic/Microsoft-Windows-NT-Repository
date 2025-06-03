/*
 * The ultimative banner.
 *
 * Written in 1987 by Wolfgang Ocker, reccoware systems, Puchheim
 *
 * It's *not* fast, but nice ...
 *
 * Tested on OS-9/68000, SunOS
 */

#include <stdio.h>
#ifdef OSK
#include <modes.h>
#else
#define S_IREAD 0
#endif


#define TRUE  1
#define FALSE 0

extern unsigned char charset;      /* Zeichensatz */
extern int           errno;

#ifdef OSK
/*
 * Signal Handler (Dummy)
 */
int sighandler()
{
}
#endif

/*
 * m a i n
 */
main(argc, argv)
  int  argc;
  char *argv[];
{
  char          *str, strbuf[200];
  int           linenum, chnum, i, j, k, count, strnum;
  unsigned char ch;
  int           ch_off;
  unsigned char *ch_addr;
  unsigned char *font;
  int           italic, dblsize, samechar;
  char          bannerchar;
  FILE          *infp;  
  char          *from, *plife;

  italic     = dblsize = samechar = FALSE;
  strnum     = 0;
  from       = NULL;
  bannerchar = '*';
  plife      = NULL;

  /*
   * Get the arguments. I *hate* getopt(:-). This works nice (and is
   * simpler).
   */
  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-')
      for (j = 1; j < strlen(argv[i]); j++)
        switch(tolower(argv[i][j])) {
          case '?':
            usage();
            exit(1);
            
          case 'i':     /* italic printing */
            italic = TRUE;
            break;          

          case 'd':     /* double sized characters */
            dblsize = TRUE;
            break;
  
          case 's':     /* use character to build large char, e.g.
                         * c becomes
                         *
                         *  ccc
                         * c   
                         * c          (or so ...)
                         *  ccc
                         */
            samechar = TRUE;
            break;

#ifdef OSK          
          case 'l':
            plife = argv[i] + j + (argv[i][j+1] == '=' ? 2 : 1);
            j = strlen(argv[i]);
            break;
#endif
  
          case 'c':       /* character for banner */
            j += argv[i][j+1] == '=' ? 2 : 1;
            bannerchar = argv[i][j];
            break;
 
          case 'z':       /* get text from ... */
            if (from) {
              usage();
              fputs("multiple 'z' option not allowed\n", stderr);
              exit(1);
            }

            from = argv[i] + j + (argv[i][j+1] == '=' ? 2 : 1);
            j = strlen(argv[i]);
            break;

          default:
            usage();
            fprintf(stderr, "banner: unknown option '%c'\n", argv[i][j]);
            exit(1);
        }
    else
      strnum++;     /* count number of strings */

  if (strnum == 0 && from == NULL) {
    usage();
#ifdef OSK
    exit(_errmsg(1, "no string given\n"));
#else
    exit(fputs("no string given\n", stderr), 1);
#endif
  }

  if (strnum && from) {
    usage();
#ifdef OSK
    exit(_errmsg(1, "'z' option not allowed if string(s) given\n"));
#else
  exit(fputs("'z' option not allowed if string(s) given\n", stderr), 1);
#endif
  }

  if (from) {
    if (from[0]) {
      if ((infp = fopen(from, "r")) == NULL)
#ifdef OSK
        exit(_errmsg(errno, "can't open '%s'\n", from));
#else
        exit(fprintf(stderr, "can't open '%s'\n", from), errno);
#endif
    }
    else
      infp = stdin;

  }

#ifdef OSK
  if (plife && !strcmp(plife, "dont_kill_me"))
    intercept(sighandler);
#endif

  font = &charset;
  str  = strbuf;

  i = 1;
  while (TRUE) {
    if (from) {     /* read strings from file/stdin */
      if (fgets(strbuf, sizeof(strbuf)-1, infp) == NULL)
        break;
      strbuf[strlen(strbuf)-1] = '\0';
    }
    else {        /* get strings from argument line */
      if ((str = argv[i++]) == NULL)
        break;
      if (str[0] == '-')
        continue;
    }
       
    for (linenum = 0; linenum < 8; linenum++) {     /* 8 lines per char */
      for (j = 0; j < (dblsize ? 2 : 1); j++) {
        putchar('\n');

        if (italic)     /* shift for italics */
          for (k = linenum; k < 7; k++)
            fputs(dblsize ? "  " : " ", stdout);

        for (chnum = 0; chnum < strlen(str); chnum++) {
          ch      = str[chnum];
          ch_off  = (int) ch * 8;
          ch_addr = font + ch_off + linenum;
          outline(ch, *ch_addr, dblsize, bannerchar, samechar);
        }
      } 
    }
  }

  putchar('\n');
}

/*
 * o u t l i n e
 */
outline(outchar, outbyte, dblsize, bannerchar, samechar)
  char outchar, outbyte, bannerchar;
  int  dblsize, samechar;

{
  int bc, j;

  for (bc = 7; bc >= 0; bc--)
    for (j = 0; j < (dblsize ? 2 : 1); j++)
      if (outbyte & (0x01 << bc))
        putchar((samechar) ? outchar : bannerchar);
      else
        putchar(' ');
}

/*
 * u s a g e
 */
usage()
{
  fputs("Syntax: banner [<opts>] {<string>} [<opts>]\n", stderr);
  fputs("Function: prints a banner to stdout\n", stderr);
  fputs("Options:\n", stderr);
  fputs("     -i          prints italic\n", stderr);
  fputs("     -d          double size\n", stderr);
  fputs("     -c=<char>   character\n", stderr);
  fputs("     -s          use same character\n", stderr);
  fputs("     -z          read strings from standard input\n", stderr);
  fputs("     -z=<file>   read strings from <file>\n", stderr);
}
