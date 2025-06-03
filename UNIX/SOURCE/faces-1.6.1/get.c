
/*  @(#)get.c 1.27 91/11/19
 *
 *  Extraction routines used by faces.
 *
 *  Copyright (c) Rich Burridge - Sun Microsystems Australia.
 *                                All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 *
 *  No responsibility is taken for any errors on inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  to me, then an attempt will be made to fix them.
 */

#include <stdio.h>
#if SYSV32 || hpux
#include <string.h>
#else
#include <strings.h>
#endif /* SYSV32 || hpux */
#include <pwd.h>
#include <ctype.h>
#include <sys/param.h>
#include "faces.h"
#include "extern.h"
#include "patchlevel.h"


/* Add boolean value to saved command line if TRUE. */

void
cmdbool(value, line, arg)
int value ;
char *line, *arg ;
{
  if (value == TRUE) STRCAT(line, arg) ;
}
 
 
/* Add integer argument to saved command line if not the default value. */
 
void
cmdint(value, defval, line, arg)
int value, defval ;
char *line, *arg ;
{
  char buf[MAXPATHLEN] ;
 
  if (value != defval)
    {
      SPRINTF(buf, arg, value) ;
      STRCAT(line, buf) ;
    }
}
 
 
/* Add string argument to saved command line if not NULL. */
 
void
cmdstr(value, line, arg)
char *value, *line, *arg ;
{
  char buf[MAXPATHLEN] ;
 
  if (value != NULL)
    {
      SPRINTF(buf, arg, value) ;
      STRCAT(line, buf) ;
    }
}


int
get_blit_ikon(name, buf)      /* Load blit ikon file. */
char *name ;
unsigned short buf[256] ;
{
  FILE *fin ;
  char *ptr ;
  int i, j, temp ;

  if ((fin = fopen(name, "r")) == NULL) return(-1) ;
  for (i = 0; i < BLITHEIGHT; i++)
    {
      FGETS(nextline, MAXLINE, fin) ;
      ptr = nextline ;
#ifdef REVORDER
      for (j = 2; j >= 0; j--)
#else
      for (j = 0; j < 3; j++)
#endif /*REVORDER*/
        {
          while (*ptr == ' ' || *ptr == '\t') ptr++ ;
          SSCANF(ptr, "0x%X", &temp) ;
          buf[i*4 + j] = (short) temp ;
          ptr = index(ptr, ',') ;
          ptr++ ;         
        }                 
      buf[i*4 + 3] = 0 ;    
    }
  for (i = BLITHEIGHT; i < iconheight; i++)
    for (j = 0; j < 4; j++) buf[i*4 + j] = 0 ;
  FCLOSE(fin) ;
  return(0) ;
}


unsigned short
get_hex(fp)
FILE *fp ;
{
  int c ;
  unsigned short rval = 0 ;

  while ((c = getc(fp)) != EOF)
    {
      if (c != '0') continue ;
      c = getc(fp) ;
      if (c != 'x') continue ;
      while ((c = getc(fp)) != EOF)
        {
               if (c >= '0' && c <= '9') rval = (rval << 4) + (c - '0') ;
          else if (c >= 'a' && c <= 'f') rval = (rval << 4) + (c - 'a' + 10) ;
          else if (c >= 'A' && c <= 'F') rval = (rval << 4) + (c - 'A' + 10) ;
          else break ;
        }
      break ;
    }
  return rval ;
}


int
get_icon(dirname, buf)            /* Read in icon file. */
char *dirname ;
unsigned short buf[256] ;
{

/*  Attempts to open the correct face file.
 *
 *  If the face file is face.ps, then another record is added to the list
 *  of NeWS .ps files to animate at a later time.
 *  If this is face.xbm, 48x48x1 or sun.icon, and the open is successful,
 *  then the face image is read into buf.
 *  -1 is returned on failure.
 */

  char *ptr ;

  ptr = rindex(dirname, '/') ;     /* Find last slash in iconname. */
  if (EQUAL(ptr+1, "face.ps"))
    if (get_news_icon(dirname)      == 0) return NEWSTYPE ;
  if (EQUAL(ptr+1, "sun.icon"))
    if (get_sun_icon(dirname, buf)  == 0) return SUNTYPE ;
  if (EQUAL(ptr+1, "48x48x1"))
    if (get_blit_ikon(dirname, buf) == 0) return BLITTYPE ;
  if (EQUAL(ptr+1, "face.xbm"))
    if (get_x11_icon(dirname, buf)  == 0) return X11TYPE ;
  return -1 ;
}


int
get_news_icon(name)    /* Create record for news.ps file. */
char *name ;
{
  FILE *fin ;

  if (gtype != NEWS) return -1 ;
  if ((fin = fopen(name, "r")) == NULL) return -1 ;
  FCLOSE(fin) ;
  add_ps_rec(row, column, name) ;
  return 0 ;
}


void
get_options(argc, argv)    /* Read and process command line options. */
int argc ;
char *argv[] ;
{
  char *faceenv ;
  char *next = NULL ;      /* The next command line parameter. */
  char *pptr ;
  int i ;

  faceenv = getenv("FACEPATH") ;
  if (faceenv) read_str(&facedirs, faceenv) ;
  else read_str(&facedirs, "/opt/lib/faces");

  INC ;
  while (argc > 0)
    {
      if (argv[0][0] == '-')
        switch (argv[0][1])
          {
#ifdef AUDIO_SUPPORT
            case 'A' : do_audio = TRUE ;   /* Enable audio support. */
                       break ;
            case 'B' : INC ;
                       getparam(&bell_sound, argv, "-B needs bell audio file") ;
                       break ;
            case 'C' : INC ;
                       getparam(&audio_cmd, argv, "-C needs audio command") ;
                       break ;
#endif /*AUDIO_SUPPORT*/
            case 'D' : debug = TRUE ;
                       break ;
            case 'H' : mtype = MONUSERS ;  /* Monitor users on a machine. */
                       INC ;
                       getparam(&rhostname, argv, "-H needs hostname") ;
                       break ;
            case 'M' : mhflag++ ;            /* Mail box can shrink. */
                       break ;
            case 'P' : mtype = MONPRINTER ;  /* Monitor printer queue. */
                       INC ;
                       getparam(&printer, argv, "-P needs printer name") ;
                       break ;
            case 'S' : INC ;               /* Alternative spoolfile. */
                       getparam(&next, argv, "-s needs spool directory") ;
                       SPRINTF(spoolfile, "%s/%s", next, username) ;
                       break ;
            case 'U' : update++ ;          /* Update faces database. */
                       break ;
            case 'a' : mtype = MONALL ;    /* Monitor all of the spoolfile. */
                       break ;
            case 'b' : if (argv[0][2] == 'g')
                         {
                           INC ;           /* Background color. */
                           getparam(&bgcolor, argv,
                                    "-bg specifies background color") ;
                           break ;
                         }
                       INC ;               /* Alternate background pattern. */
                       getparam(&bgicon, argv, "-b needs background icon") ;
                       break ;
            case 'c' : INC ;               /* Number of columns. */
                       getparam(&next, argv,
                                      "-c needs number of columns value") ;
                       maxcols = atoi(next) ;
                       if (maxcols <= 0 || maxcols > (1152 / ICONWIDTH))
                         maxcols = NO_PER_ROW ;
                       break ;
            case 'd' : INC ;       /* Already extracted; ignore here. */
                       break ;
            case 'e' : mtype = MONPROG ;   /* User specified program. */
                       INC ;
                       getparam(&userprog, argv, "-e needs user program") ;
                       break ;
            case 'f' : if (argv[0][2] == 'g')
                         {
                           INC ;           /* Background color. */
                           getparam(&fgcolor, argv,
                                    "-fg specifies foreground color") ;
                           break ;
                         }
                       else if (argv[0][2] == 'n')
                         {
                           INC ;           /* font name. */
                           getparam(&fontname, argv,
                                    "-fn specifies font name") ;
                           break ;
                         }
                       INC ;               /* New directory for face icons. */
                       getparam(&facedirs, argv,
                                "-f needs face directory path") ;
                       break ;
            case 'g' : INC ;               /* X11 geometry information. */
                       getparam(&geometry, argv,
                                          "-g needs geometry information") ;
                       posspec = 1 ;
                       break ;
            case 'h' : INC ;               /* Face image height. */
                       getparam(&next, argv, "-h needs height value") ;
                       imageheight = atoi(next) ;
                       if (imageheight < ICONHEIGHT || imageheight > 250)
                         {
                           FPRINTF(stderr, "Illegal -h value; resetting.\n") ;
                           imageheight = ICONHEIGHT ;
                         }
                       break ;
            case 'i' : if (!strncmp(&argv[0][1], "iconic", 6)) iconic = 1 ;
                       else if (argv[0][2] == '\0')            invert = 1 ;
                       break ;
            case 'l' : INC ;               /* Faces window title line label. */
                       getparam(&faces_label, argv, "-l needs window label") ;
                       break ;
            case 'n' : dontshowno = 1 ;    /* Don't show number of messages. */
                       break ;
            case 'p' : INC ;               /* No. of seconds between checks. */
                       getparam(&next, argv, "-p needs period time") ;
                       period = atoi(next) ;
                       break ;
            case 'r' : INC ;               /* Same as -i, but for X11 */
                       invert = 1 ;
                       break ;
            case 's' : INC ;               /* Alternative spoolfile. */
                       getparam(&spoolfile, argv, "-s needs spool file") ;
                       break ;
            case 't' : dontshowtime = 1 ;  /* Do not show timestamps. */
                       break ;
            case 'u' : dontshowuser = 1 ;  /* Do not show usernames. */
                       break ;
            case '?' :
            case 'v' : usage() ;
            case 'w' : INC ;               /* Face image width. */
                       getparam(&next, argv, "-w needs width value") ;
                       imagewidth = atoi(next) ;
                       if (imagewidth < ICONWIDTH || imagewidth > 250)
                         {
                           FPRINTF(stderr, "Illegal -w value; resetting.\n") ;
                           imagewidth = ICONWIDTH ;
                         }
                       break ; 

/*  SunView windowing arguments. -Wp, -WP and -Wi are used in the NeWS
 *  implementation to initially position the window and icon.
 */

            case 'W' : switch (argv[0][2])
                         {
                           case 'H' : break ;   /* -WH, no sub-args follow */
                           case 'i' : iconic = 1 ;
                                      break ;   /* -Wi, start as an icon. */
                           case 'g' :           /* -Wg, set default color. */
                           case 'n' : nolabel = 1 ; /* -Wn, no label at all */
                                      break ;
                           case 'h' :           /* -Wh, height */
                           case 'I' :           /* -WI "icon filename" */
                           case 'l' :           /* -Wl "some window label" */
                           case 'L' :           /* -Wl "some icon label" */
                           case 't' :           /* Font filename */
                           case 'T' :           /* Icon font filename */
                           case 'w' : INC ;     /* Width, in columns. */
                                      break ;
                           case 'p' : INC ;     /* -Wp xnum ynum */
                                      getparam(&next, argv,
                                               "-Wp needs x coordinate") ;
                                      wx = atoi(next) ;
                                      INC ;
                                      getparam(&next, argv,
                                               "-Wp needs y coordinate") ;
                                      wy = atoi(next) ;
                                      posspec = 1 ;
                                      break ;
                           case 'P' : INC ;      /* -WP xnum ynum */
                                      getparam(&next, argv,
                                               "-WP needs x coordinate") ;
                                      ix = atoi(next) ;
                                      INC ;
                                      getparam(&next, argv,
                                               "-WP needs y coordinate") ;
                                      iy = atoi(next) ;
                                      iconpos = 1 ;
                                      break ;
                           case 's' : INC ; INC ;  /* -Ws xnum ynum */
                                      break ;
                           case 'b' :         /* -Wb r g b (bg color spec) */
                           case 'f' : INC ; INC ; INC ;  /* Same, fg color */
                                      break ;
                           default :  FPRINTF(stderr,
                                              "%s: -W%c unknown argument\n",
                                              progname, argv[0][2]) ;
                                      break ;
                         }
                       break ;
            error    :
            default  : usage() ;
                       exit(1) ;
          }
      INC ;
    }

/* Parse the face path from the parameter line... */

  for (i = 0, pptr = facedirs; pptr && (i <= MAXPATHS); i++)
    {
      if ((*pptr == (char) NULL) || (*pptr == ':'))
        {

/* Null entry in face path means insert compiled-in default. */

          facepath[i] = FACEDIR ;
          if (*(pptr++) == (char) NULL) break ;
        }
      else
        {
          facepath[i] = pptr ;
          pptr = index(pptr, ':') ;
          if (pptr) *(pptr++) = (char) NULL ;
        }
    }
}


int
get_x11_icon(name, buf)     /* Load X11 icon file. */
char *name ;
unsigned short buf[256] ;
{
  FILE *fin ;
  int hgt, i, j, wid ;
  char c, *cptr ;
  unsigned char vbuf[512] ;
  unsigned int tmp ;

  if ((fin = fopen(name, "r")) == NULL) return -1 ;
  if (fgets(nextline, MAXLINE, fin) == NULL) goto err_end ;
  if (sscanf(nextline, "#define %s %d", vbuf, &wid) != 2) goto err_end ;
  if ((cptr = rindex((char *) vbuf, '_')) == NULL || strcmp(cptr, "_width"))
    goto err_end ;
  wid = (wid + 7) / 8 ;

  if (fgets(nextline, MAXLINE, fin) == NULL) goto err_end ;
  if (sscanf(nextline, "#define %s %d", vbuf, &hgt) != 2) goto err_end ;
  if ((cptr = rindex((char *) vbuf, '_')) == NULL || strcmp(cptr, "_height"))
    goto err_end ;
  if (hgt > iconheight) hgt = iconheight ;

  while(nextline[0] == '#')
    if (fgets(nextline, MAXLINE, fin) == NULL) goto err_end ;
  if (sscanf(nextline, "static %s %*[^{]%c", vbuf, &c) != 2) goto err_end ;
  if (strcmp((char *) vbuf, "char") == 0)
    {
      for (i = 0; i < sizeof(vbuf); vbuf[i++] = 0) continue ;
      for (i = 0; i < hgt; i++)
        for (j = 0; j < wid; j++)
          {
            tmp = get_hex(fin) & 0xFF ;
            if (j < (iconwidth / 8))
              vbuf[i*8+j] = (unsigned char) tmp ;
          }
      for (i = 0; i < 256; i++)
        buf[i] = (revtable[vbuf[(i*2)+1]] & 0xFF) +
                 ((revtable[vbuf[i*2]] & 0xFF) << 8) ;
      FCLOSE(fin) ;
      return(0) ;
    }
  else if (strcmp((char *) vbuf, "short") == 0)
    {
      wid = (wid + 1) / 2 ;
      for (i = 0; i < 256; buf[i++] = 0) continue ;
      for (i = 0; i < hgt; i++)
        for (j = 0; j < wid; j++)
          {
            tmp = get_hex(fin) ;
            if (j < (iconwidth / 16))
              buf[i*4+j] = (revtable[tmp >> 8] & 0xFF) +
                           ((revtable[tmp & 0xFF] & 0xFF) << 8) ;
          }
      FCLOSE(fin) ;
      return(0) ;
    }
err_end:

  FCLOSE(fin) ;
  return -1 ;
}


void
get_xface(ibuf, obuf)              /* Extract "raw" X-Face data. */
char *ibuf ;
unsigned short obuf[] ;
{
  char *ptr ;
  int i, j, temp ;

  ptr = ibuf ;
  for (i = 0; i < BLITHEIGHT; i++)
    {
#ifdef REVORDER
      for (j = 2; j >= 0; j--)
#else
      for (j = 0; j < 3; j++)
#endif /*REVORDER*/
        {
          while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') ptr++ ;
          SSCANF(ptr, "0x%X", &temp) ;
          obuf[i*4 + j] = (short) temp ;
          if ((ptr = index(ptr, ',')) == NULL) return ;
          ptr++ ;
        }
      obuf[i*4 + 3] = 0 ;
    }
  for (i = BLITHEIGHT; i < iconheight; i++)
    for (j = 0; j < 4; j++) obuf[i*4 + j] = 0 ;
}


char *
getname()       /* Get users name from passwd entry. */
{
  char *getlogin(), *username ;
  struct passwd *getpwuid(), *pwent ;

  if (!(username = getlogin()))
    {
      pwent = getpwuid(getuid()) ;
      username = pwent->pw_name ;
      endpwent() ;                   /* Close the passwd file */
    }
  return username ;
}


void
getparam(s, argv, errmes)
char **s, *argv[], *errmes ;
{
  if (*argv != NULL) read_str(s, *argv) ;
  else
    {
      FPRINTF(stderr,"%s: %s as next argument.\n", progname, errmes) ;
      exit(1) ;
    }
}


int
get_sun_icon(name, buf)     /* Load Sun icon file. */
char *name ;
unsigned short buf[256] ;
{
  FILE *fin ;
  char htype[MAXLINE] ;     /* Current header comment parameter. */
  int c ;                   /* Count of items found from fscanf call. */
  int comment ;             /* Set if more initial comment to read. */
  int count ;               /* Number of items to read from icon file. */
  int idepth ;              /* Depth of this icon. */
  int iheight ;             /* Height of this icon. */
  int iwidth ;              /* Width of this icon. */
  int ivbpi ;               /* Number of valid bits per item. */
  int temp ;                /* Temporary location for latest 16 bits. */

  if ((fin = fopen(name, "r")) == NULL) return -1 ;
  comment = 1 ;
  while (comment)
    {
      if (fscanf(fin, "%*[^WHDV*]%s", htype) == EOF) break ;
      switch (htype[0])
        {
          case 'W' : SSCANF(htype, "Width=%d", &iwidth) ;
                     if (iwidth != 64)
                       {
                         FPRINTF(stderr, "%s: %s has width %d\n",
                                          progname, name, iwidth) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;                         return -1 ;
                       }
                     break ;
          case 'H' : SSCANF(htype, "Height=%d", &iheight) ;
                     if (iheight != 64)
                       {
                         FPRINTF(stderr, "%s: %s has height %d\n",
                                          progname, name, iheight) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;                         return -1 ;
                       }
                     break ;
          case 'D' : SSCANF(htype, "Depth=%d", &idepth) ;
                     if (idepth != 1)
                       {
                         FPRINTF(stderr, "%s: %s has depth %d\n",
                                          progname, name, idepth) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;
                         return -1 ;
                       }
                     break ;
          case 'V' : SSCANF(htype, "Valid_bits_per_item=%d", &ivbpi) ;
                     if (ivbpi != 16)
                       {
                         FPRINTF(stderr, "%s: %s has %d bits per item\n",
                                          progname, name, ivbpi) ;
                         FPRINTF(stderr, "This is currently not supported.\n") ;
                         return -1 ;
                       }
                     break ;
          case '*' : if (htype[1] == '/') comment = 0 ;
        }
    }

  count = 0 ;
  while (count < ((iheight * iwidth) / 16))
    {
      c = fscanf(fin, " 0x%X,", &temp) ;
      if (c == 0 || c == EOF) break ;
#ifdef REVORDER
      buf[count++] = (short) ((revtable[temp & 0xFF] << 8) +
                             ((revtable[(temp >> 8) & 0xFF]) & 0xFF)) ;
#else
      buf[count++] = (short) temp ;
#endif /*REVORDER*/
    }    
  FCLOSE(fin) ;
  return(0) ;
}


char *
strlower(str)
char *str ;
{
  char *c ;

  for (c = str; *c != '\0'; c++) 
    if (isupper(*c)) *c = tolower(*c) ;
  return(str) ;
}


void
usage()      /* Print faces usage message. */
{
  FPRINTF(stderr, "%s version 1.6.%1d\n\n", progname, PATCHLEVEL) ;
  FPRINTF(stderr, "Usage: %s ", progname) ;
#ifdef AUDIO_SUPPORT
  FPRINTF(stderr, "\t[-A] [-B bell_sound_file] [-C audio_cmd]\n") ;
#endif /*AUDIO_SUPPORT*/
  FPRINTF(stderr, "\t[-H hostname] [-M] [-P printer]\n") ;
  FPRINTF(stderr, "\t[-S spooldir] [-U] [-Wi] [-Wp x y] [-WP x y] [-a]\n") ;
  FPRINTF(stderr, "\t[-b background] [-bg background_color] [-c columns]\n") ;
  FPRINTF(stderr, "\t[-d display] [-e progname] [-f facepath]\n") ;
  FPRINTF(stderr, "\t[-fg foreground_color] [-g geometry] [-h height]\n") ;
  FPRINTF(stderr, "\t[-i] [-iconic] [-l label] [-n] [-p period]\n") ;
  FPRINTF(stderr, "\t[-s spoolfile] [-t] [-u] [-v] [-w width] [-?]\n\n") ;
  FPRINTF(stderr, "Keyboard accelerators:\n") ;
  FPRINTF(stderr, "\t^L  - clear faces display.\n") ;
  FPRINTF(stderr, "\tdel - refresh faces display.\n") ;
  FPRINTF(stderr, "\tq   - terminate faces display.\n") ;
  exit(1) ;
}


void
write_cmdline()
{
  char line[MAXPATHLEN] ;

  line[0] = NULL ;

#ifdef AUDIO_SUPPORT
  cmdbool(do_audio,  line, " -A ") ;           /* Enable audio support. */
  cmdstr(bell_sound, line, " -B %s ") ;        /* Bell audio file. */
  cmdstr(audio_cmd,  line, " -C %s ") ;        /* Audio command */
#endif /*AUDIO_SUPPORT*/

  cmdbool(debug,        line, " -D ") ;        /* Turn on debugging. */
  cmdbool(mhflag,       line, " -M ") ;        /* Mail box can shrink. */
  cmdbool(update,       line, " -U ") ;        /* Update faces database. */
  cmdbool(iconic,       line, " -iconic ") ;   /* Start faces iconic. */
  cmdbool(invert,       line, " -i ") ;        /* Invert display. */
  cmdbool(dontshowno,   line, " -n ") ;        /* Don't show # of messages. */
  cmdbool(dontshowtime, line, " -t ") ;        /* Don't show timestamps. */
  cmdbool(dontshowuser, line, " -u ") ;        /* Don't show usernames. */

  if (mtype == MONALL)                         /* Monitor all of spoolfile. */
    STRCAT(line, " -a ") ;
  if (mtype == MONUSERS)                       /* Monitor users. */
    cmdstr(rhostname, line, " -H %s ") ;
  if (mtype == MONPRINTER)                     /* Monitor printer. */
    cmdstr(printer,   line, " -P %s ") ;
  if (mtype == MONPROG)                        /* User specified program. */
    cmdstr(userprog,  line, " -e %s ") ;

  cmdint(imageheight, ICONHEIGHT, line, " -h %d ") ;    /* Image height. */ 
  cmdint(imagewidth,  ICONWIDTH,  line, " -w %d ") ;    /* Image width. */
  cmdint(maxcols,     NO_PER_ROW, line, " -c %1d ") ;   /* # of columns. */
  cmdint(period,      60,         line, " -p %d ") ;    /* Period length. */

  cmdstr(bgcolor,     line, " -bg %s ") ;      /* Background color. */
  cmdstr(bgicon,      line, " -b %s ") ;       /* Background pattern. */
  cmdstr(display,     line, " -d %s ") ;       /* X11 display information. */
  cmdstr(fgcolor,     line, " -fg %s ") ;      /* Foreground color. */
  cmdstr(fontname,    line, " -fn %s ") ;      /* Alternate fontname. */
  cmdstr(facedirs,    line, " -f %s ") ;       /* List of face directories. */
  cmdstr(geometry,    line, " -g %s ") ;       /* X11 geometry information. */
  cmdstr(faces_label, line, " -l %s ") ;       /* Window title line. */
  cmdstr(spoolfile,   line, " -s %s ") ;       /* Spoolfile to monitor. */
  save_cmdline(line) ;
}
