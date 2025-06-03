
/*  @(#)mon.c 1.34 91/11/19
 *
 *  Monitoring routines used by the faces program.
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
#include <ctype.h>
#if SYSV32 || hpux
#include <string.h>
#else
#include <strings.h>
#endif /* SYSV32 || hpux */
#include <sys/types.h>
#include <sys/stat.h>
#if ( !defined(mips) && !defined(TOPIX) ) || defined(ultrix)
#include <sys/time.h>
#else
#ifdef SYSTYPE_BSD43
#include <sys/time.h>
#else
#ifndef TOPIX
#include <bsd/sys/time.h>
#else
#include <sys/bsd_time.h>
#endif /*TOPIX*/
#endif /*SYSTYPE_BSD43*/
#endif /*( !defined(mips) && !defined(TOPIX) ) || defined(ultrix)*/
#include <sys/file.h>
#ifndef NOUTIME
#ifndef TOPIX
#ifndef	mips
#include <utime.h>
#endif
#else
struct utimbuf {
  time_t actime ;
  time_t modtime ;
} ;
#endif /*TOPIX*/
#endif /*!NOUTIME*/

#include "faces.h"
#include "extern.h"

static time_t lastmtime = 0 ; /* Only look at mailfile when it's changed. */

static char *fgetheader P((int, FILE *)) ;


void
add_face(dtype, itype, name)
enum disp_type dtype ;
enum icon_type itype ;
char *name ;
{
  unsigned short buf[256] ;  /* Ikon/icon image. */

  if (itype == XFACE)
    {
      itype = ORDINARY ;
      destroy_image(itype) ;
      get_xface(face_buf, buf) ;
      load_icon(itype, buf, 0) ;
    }
  else if (itype == ORDINARY)
    {
      destroy_image(itype) ;
      if (get_icon(name, buf) == -1) itype = NOFACE ;
      else load_icon(itype, buf, 0) ;
    }
 
  switch (dtype)
    {
      case DISP_ALL   : adjust_image(DISP_NAME,  itype, row, column) ;
                        adjust_image(DISP_OTHER, itype, row, column) ;
                        adjust_image(DISP_ICON,  itype, 0, 0) ;
                        break ;
      case DISP_BOTH  : adjust_image(DISP_NAME,  itype, row, column) ;
                        adjust_image(DISP_OTHER, itype, row, column) ;
                        break ;
      case DISP_ICON  : adjust_image(dtype, itype, 0, 0) ;
                        break ;
      case DISP_NAME  :
      case DISP_OTHER : adjust_image(dtype, itype, row, column) ;
    }
  if (itype == NOMAIL) drop_back() ;
}


void
adjust()          /* Adjust the row and column position. */
{
  struct psinfo *this ;

  if (mtype != MONNEW)
    {
      if (++column == maxcols)
        {
          column = 0 ;
          row++ ;
        }
      return ;
    }
  if (psrecs != NULL)           /* Adjust animation positions for MONNEW */
    {
      this = psrecs ;           /* Point to beginning of chain. */
      while (this != NULL)
        {
          this->column++ ;      /* Adjust column position. */
          if (facetype == NEWSTYPE && this->next == NULL)
            this->column-- ;    /* Reset for brand new animate record. */
          this = this->next ;
        }
    }
}


#ifdef AUDIO_SUPPORT
void
check_audio(iconname)     /* Check if there is an audio file for this user. */
char *iconname ;
{
  struct stat sbuf ;
  char filename[MAXLINE], *ptr ;

  if (do_audio && !firsttime)
    if ((ptr = rindex(iconname, '/')) != NULL)
      {
        STRNCPY(filename, iconname, ptr - iconname) ;
        filename[ptr - iconname] = '\0' ;
        STRCAT(filename, "/face.au") ;
        if (stat(filename, &sbuf) != -1) play_sound(filename) ;
      }
}
#endif /*AUDIO_SUPPORT*/


void
do_check()        /* Perform another check of the appropriate type. */
{
  switch (mtype)
    {
      case MONALL     : do_mail(MONALL) ;  /* Monitor all of the mail file. */
                        break ;
      case MONNEW     : do_mail(MONNEW) ;  /* Monitor new mail only. */
                        break ;
      case MONPRINTER : do_printer() ;     /* Monitor the print queue. */
                        break ;
      case MONPROG    : do_prog() ;        /* Run user supplied program. */
                        break ;
      case MONUSERS   : do_users() ;       /* Monitor users on a machine. */
    }
  firsttime = 0 ;
}


void
do_face_update(name, buf)   /* Send mail to update faces database. */
char *name, buf[2048] ;
{
  FILE *fp ;                /* File descriptor for users mail spool file. */
  char command[MAXLINE] ;   /* Command to send mail to special alias. */

  SPRINTF(command, UPDATEDEF, update_alias) ;
  if ((fp = popen(command, "w")) == NULL)     /* Open pipe to mail process. */
    {
      FPRINTF(stderr,"%s: couldn't send mail to update database.\n", progname) ;
      return ;
    }
  FPRINTF(fp, "To: %s\n", update_alias) ;
  FPRINTF(fp, "Subject: %s\n\n", name) ;  /* Send icon name. */
  FPRINTF(fp, "%s\n", buf) ;              /* Send ikon data. */
  PCLOSE(fp) ;
}


void
do_key(val)            /* Handle various keyboard options. */
int val ;
{
  switch (val)
    {
      case KEY_CLEAR   : if (mtype == MONNEW)
                           {
                             repl_image(DISP_NAME,  CUROFF, width, height) ;
                             repl_image(DISP_OTHER, CUROFF, width, height) ;
                             add_face(DISP_ICON, NOMAIL, "") ;
                             toclear = 1 ;
                           }
                         do_check() ;
                         break ;
      case KEY_REFRESH : draw_screen(wdtype) ;
                         break ;
      case KEY_QUIT    : exit(0) ;
    }
}


int
LWSP(c)
int c;
{
    return (c == ' ') || (c == '\t');
}


static char *
fgetheader(eoh, fp)
int eoh;
FILE *fp;
{
    /* return either one line, or if eoh is FALSE, a folded RFC822
     * logical line. 
     */
    if (line == NULL)
    {
        line = Malloc(MAXLINE);
        maxline = MAXLINE;
    }
    if (eoh == TRUE)
    {
        /* just get a single line */
        return fgets(line, maxline, fp);
    }
    else
    {
        /* get an entire RFC822 folded header */
        char *p;
        int c;

        /* get first line */
        if (fgets(line, maxline, fp) == NULL)
            return NULL;
        /* check for EOH */
        if (line[0] == '\n')
            return line;
        /* check for LWSP between field-name and initial : */
        if ((p = index(line, ':')) != NULL)
        {
            /* canonicalize */
            while (p > line && LWSP(p[-1]))
            {
                p[0] = p[-1];
                p[-1] = ':';
                p--;
            }
        }
        /* process continuation lines */
        for (;;)
        {
            /* get last char */
            p = line + strlen(line) - 1;
            if (*p != '\n')
            {
                /* this is a physically long line */
                p++;
                c = ' ';
            }
            else
            {
                c = getc(fp);
                if (c != EOF)
                    ungetc(c,fp);
            }
            if (!LWSP(c))
                break;
            /* this is a header continuation line */
            if ((p-line)+MAXLINE > maxline)
            {
                /* extend line buffer */
                maxline += MAXLINE;
                if ((line = realloc(line, maxline)) == NULL)
                {
                    FPRINTF(stderr, "Out of memory!\n");
                    exit(1);
                }
                p = line + strlen(line) - 1;
                if (*p != '\n')
                    p++;
            }
            p[0] = '\0';
            if ((p = fgets(p, MAXLINE, fp)) == NULL)
                break;
        }
        return line;
    }
}


int
casecmp(c, d)
register int c;
register int d;
{
      if (isupper(c))
              c = tolower(c);
      if (isupper(d))
              d = tolower(d);
      return (c - d);
}


int
is822header(line, field)
register char *line;
register char *field;
{
      register int n;

      n = strlen(field);
      while (n > 0) {
              if (casecmp(*line++, *field++) != 0)
                      return (0);
              n--;
      }
      while (LWSP(*line))
              line++;
      if (*line != ':')
              return (0);
     
      return (1);
}


void
do_mail(mtype)              /* Monitor a mail file for new or all mail. */
enum mon_type mtype ;
{        
  int eoh ;                 /* Set true, if end of mail header detected. */
  FILE *fp ;                /* File descriptor for users mail spool file. */
#ifdef mips
  time_t ubuf[2] ;
#else
  struct utimbuf ubuf ;     /* For resetting the access time on spoolfile. */
#endif
#ifdef RAND
  int ctrla = 0 ;
#endif /*RAND*/
  static int first = 1 ;    /* First time through. */

  column = row = 0 ;        /* Start in top left corner of pixrect. */
  mboxshrunk = 0 ;
  newmail = 0 ;             /* Assume no new mail. */
  noicons = 0 ;

  if (spoolfile == NULL) return ;
  if (mtype == MONNEW) make_pixrect(maxcols) ;
  if (stat(spoolfile, &buf) == -1)
    {
      lastsize = 0 ;
      add_face(DISP_ICON, NOMAIL, "") ;
      if (mtype == MONNEW) show_display(TRUE) ;   /* Show new mail. */
      else make_display() ;     /* Output icons and tidyup chain of records. */
      return ;
    }

/*  See if there is any work to do. If the file hasn't changed, then no need
 *  to do anything but exit. A size check is now done, because the Elm mailer
 *  resets the files' mtime.
 */

  if (!first && buf.st_size == lastsize)
    {
      show_display(FALSE) ;
      return ;
    }

  first = 0 ;
  lastmtime = buf.st_mtime ;

  if (buf.st_size > lastsize ||
      (mhflag && (buf.st_size < lastsize) && (buf.st_size != 0)))
    {
      if (buf.st_size < lastsize)
        {
          lastsize = 0 ;
          mboxshrunk = 1 ;
        }
      newmail = 1 ;                           /* New mail found. */
    }
  if (!buf.st_size) add_face(DISP_ICON, NOMAIL, "") ;

#ifdef mips
  ubuf[0] = buf.st_atime ;
  ubuf[1] = buf.st_mtime ;
#else
  CLEARMEM((char *) &ubuf, sizeof(ubuf)) ; /* Some systems have extra fields */
  ubuf.actime  = buf.st_atime ;              /* Save for possible reset. */
  ubuf.modtime = buf.st_mtime ;
#endif

  if (mtype == MONNEW)
    if (buf.st_size <= lastsize)   /* Is the size of mail folder bigger? */
      {
        lastsize = buf.st_size ;   /* No: save new size and return. */
        show_display(FALSE) ;
        return ;
      }

  if ((fp = fopen(spoolfile, "r")) == NULL)   /* Open spoolfile. */
    {
      if (mtype == MONNEW) show_display(FALSE) ;   /* Show new mail. */
      else make_display() ;     /* Output icons and tidyup chain of records. */
      return ;
    }
  if (mtype == MONNEW) FSEEK(fp, lastsize, 0) ;
  lastsize = buf.st_size ;
  eoh = FALSE ;
  while (fgetheader(eoh, fp) != NULL)
    {
#ifdef RAND
      if (EQUAL(line, "\001\001\001\001"))
#else
      if (EQUAL(line, "From "))
#endif /*RAND*/
        {
          if (froms_found) process_info() ;  /* Process previous mail. */
          eoh = FALSE ;
#ifdef RAND
          froms_found = ctrla = !ctrla ;
#else
          process_from() ;                   /* Save new from details. */
#endif /*RAND*/

        }
      else if (*line == '\n' && eoh == FALSE)          eoh = TRUE ;
      else if (is822header(line, "From")   && eoh == FALSE) process_from() ;
      else if (is822header(line, "X-Face") && eoh == FALSE) process_face() ;
    }
  FCLOSE(fp) ;

  if (!stat(spoolfile, &buf))    /* Reset access time if no change in size. */
    if (buf.st_size == lastsize)
#ifdef mips
      UTIME(spoolfile, ubuf) ;
#else
      UTIME(spoolfile, &ubuf) ;
#endif

  if (froms_found) process_info() ;          /* Process remaining mail item. */
  if (mtype == MONNEW) show_display(TRUE) ;  /* Show new mail. */
  else make_display() ;     /* Output icons and tidyup chain of records. */
}


void
do_printer()                 /* Monitor printer queue. */
{
  struct recinfo *this, *next ;
  FILE *fp ;                 /* File descriptor for users mail spool file. */
  char command[MAXLINE] ;    /* Print status command for this printer. */
  char owner[MAXLINE] ;      /* Owner of job in the print queue. */
  char *ptr ;
  int size ;                 /* Size of this print job in bytes. */

  noicons = 0 ;
  if (printer == NULL) read_str(&printer, "lp") ;
  SPRINTF(command, PRINTDEF, printer) ;
  if ((fp = popen(command, "r")) == NULL)     /* Open spoolfile. */
    {
      FPRINTF(stderr,"%s: couldn't get printer stats.\n", progname) ;
      return ;
    }
  column = row = 0 ;          /* Start in top left corner of pixrect. */

#ifndef PLP
  FGETS(nextline, MAXLINE, fp) ;

  ptr = rindex(nextline, ':') ;            /* Is this a valid printer? */
  if (ptr != NULL && EQUAL(ptr, ": unknown printer"))
    {
      FPRINTF(stderr, "%s: %s is an unknown printer.\n", progname, printer) ;
      exit(1) ;
    }

  if (EQUAL(nextline, "no entries"))
    {
      make_pixrect(1) ;                    /* Just the "no print" icon. */
      add_face(DISP_ALL, NOPRINT, "") ;    /* Set to "no print" icon. */
      text(DISP_ALL, LEFT, printer) ;      /* Output printer name. */
    }
  else if (EQUAL(nextline, "Printer Error: may need attention!"))
    {
      make_pixrect(1) ;                    /* Just the "no paper" icon. */
      add_face(DISP_ALL, NOPAPER, "") ;    /* Set to "no paper" icon. */
      text(DISP_ALL, LEFT, printer) ;      /* Output printer name. */
    }
  else
#endif /*PLP*/
    {
      FGETS(nextline, MAXLINE, fp) ;    /* Skip the next line. */
      while (fgets(nextline, MAXLINE, fp) != NULL)
        {
#ifdef PLP
          if ((nextline[0] == '\n')             ||
              (EQUAL(nextline, "  work done"))  ||
              (EQUAL(nextline, "  processing")) ||
              (EQUAL(nextline, "Printer "))     ||
              (EQUAL(nextline, "Remote printer "))) continue ;

          if ((EQUAL(nextline, "  Printer Error: may need attention!")) ||
              (EQUAL(nextline, "Remote connect to "))) /* for the time being */
            {
              make_pixrect(1) ;                 /* Just the "no paper" icon. */
              add_face(DISP_ALL, NOPAPER, "") ; /* Set to "no paper" icon. */
              text(DISP_ALL, LEFT, printer) ;   /* Output printer name. */
              PCLOSE(fp) ;
              show_display(TRUE) ;
              return ;
	    }
#endif /*PLP*/
          SSCANF(&nextline[7], "%s", owner) ;
#ifdef PLP
          if (EQUAL(owner, "Owner")) continue ;
          SSCANF(&nextline[59], "%d", &size) ;
#else
          SSCANF(&nextline[60], "%d", &size) ;
#endif /*PLP*/
          h_to_c("", community) ;
          found = make_iconname(facepath, community, owner) ;
          add_record("", owner, "", size) ;
        }
#ifdef PLP
      if (recs == NULL)
        {
          make_pixrect(1) ;                    /* Just the "no print" icon. */
          add_face(DISP_ALL, NOPRINT, "") ;    /* Set to "no print" icon. */
          text(DISP_ALL, LEFT, printer) ;      /* Output printer name. */
          PCLOSE(fp) ;
          show_display(TRUE) ;
          return ;
	}
#endif /*PLP*/
      if (noicons == 0) noicons++ ;
      make_pixrect(noicons) ;
      this = recs ;
      while (this != NULL)
        {
          next = this->next ;
          add_face(DISP_BOTH, ORDINARY, this->iconname) ;
          SPRINTF(nextline, "%1d", this->size) ;
          if (!dontshowuser)
            text(DISP_NAME, LEFT, this->username) ;  /* Owner. */
          text(DISP_OTHER, LEFT, nextline) ;         /* Size. */
          if (this == recs)
            {
              add_face(DISP_ICON, ORDINARY, this->iconname) ;
              text(DISP_ICON, LEFT, printer) ;
              SPRINTF(nextline, "%1d %s", noicons,
                                         (noicons == 1 ? "job" : "jobs")) ;
              text(DISP_ICON, RIGHT, nextline) ;   /* Number of jobs. */
            }
          adjust() ;    /* Adjust column and row. */
          remove_record(this) ;
          this = next ;
        }
      recs = last = NULL ;
    }
  PCLOSE(fp) ;
  show_display(TRUE) ;
}


void
get_userrec(s, itype)
char *s ;
enum icon_type itype ;
{
  enum icon_type ftype ;       /* Type of the current face. */
  char *fields[MAXFIELDS] ;    /* Pointers to program record fields. */
  char line[MAXLINE] ;
  char *fptr, *lptr ;
  int i ;

  STRCPY(line, s) ;
  for (i = 0; i < MAXFIELDS; i++) fields[i] = NULL ;
  lptr = line ;
  for (i = 0; i < MAXFIELDS; i++)
    {
      if ((fptr = index(lptr, '\t')) == NULL)
        if ((fptr = index(lptr, '\n')) == NULL)
          break ;
      fields[i] = lptr ;
      lptr = fptr + 1 ;
      *fptr = '\0' ;
    }
  for (i = (int) REC_LWIN; i <= (int) REC_RICON; i++)
    if (fields[i] != NULL && fields[i][0] == '\0') fields[i] = NULL ;

  STRCPY(community, "") ;
  if (!(EQUAL(fields[(int) REC_USER], "NOMAIL")  ||
        EQUAL(fields[(int) REC_USER], "NOPAPER") ||
        EQUAL(fields[(int) REC_USER], "NOPRINT") ||
        EQUAL(fields[(int) REC_USER], "NOTHING") ||
        EQUAL(fields[(int) REC_USER], "NOUSERS")))
    {

/* Turn hostname into community name. */

      h_to_c(fields[(int) REC_HOST], community) ;
      a_to_u(community, fields[(int) REC_USER], realcomm, realname) ;
      found = make_iconname(facepath, realcomm, realname) ;
    }
       if (EQUAL(fields[(int) REC_USER], "NOMAIL"))  ftype = NOMAIL ;
  else if (EQUAL(fields[(int) REC_USER], "NOPAPER")) ftype = NOPAPER ;
  else if (EQUAL(fields[(int) REC_USER], "NOPRINT")) ftype = NOPRINT ;
  else if (EQUAL(fields[(int) REC_USER], "NOTHING")) ftype = NOTHING ;
  else if (EQUAL(fields[(int) REC_USER], "NOUSERS")) ftype = NOUSERS ;
  else                                               ftype = ORDINARY ;
  add_face(itype, ftype, iconname) ;
  if (itype == DISP_ICON || itype == DISP_ALL)
    {
      if (fields[(int) REC_LWIN] != NULL)
        text(DISP_ICON,  LEFT,  fields[(int) REC_LWIN]) ;
      if (fields[(int) REC_RWIN] != NULL)
        text(DISP_ICON,  RIGHT, fields[(int) REC_RWIN]) ;
    }
  if (itype != DISP_ICON)
    {
      if (fields[(int) REC_LICON] != NULL)
        text(DISP_OTHER, LEFT,  fields[(int) REC_LICON]) ;
      if (fields[(int) REC_RICON] != NULL)
        text(DISP_OTHER, RIGHT, fields[(int) REC_RICON]) ;
      if (fields[(int) REC_LWIN] != NULL)
        text(DISP_NAME,  LEFT,  fields[(int) REC_LWIN]) ;
      if (fields[(int) REC_RWIN] != NULL)
        text(DISP_NAME,  RIGHT, fields[(int) REC_RWIN]) ;
    }
}


void
do_prog()                   /* Run user supplied program or script. */
{
  FILE *fp ;                /* File descriptor for users command output. */
  int cols = maxcols ;      /* Maximum number of columns for display. */
  int isicon = 0 ;          /* Set if there is an initial icon record. */
  int rows = 1 ;            /* Maximum number of rows for display. */
  char *reply ;

  if (userprog == NULL ||
      (fp = popen(userprog, "r")) == NULL)   /* Connect to user program. */
    {
      FPRINTF(stderr,"%s: couldn't get program (%s) information.\n",
              progname, userprog) ;
      exit(1) ;
    }
  if ((reply = fgets(nextline, MAXLINE, fp)) == NULL || !reply)
    {
      FPRINTF(stderr,"%s: couldn't get program (%s) information.\n",
              progname, userprog) ;
      exit(1) ;
    }
  if (!EQUAL(nextline, "Cols="))
    {
      isicon = 1 ;
      get_userrec(nextline, DISP_ICON) ;
      FGETS(nextline, MAXLINE, fp) ;
    }
  SSCANF(nextline, "Cols=%d Rows=%d", &cols, &rows) ;
  if (cols < 1) cols = maxcols ;
  if (rows < 1) rows = 1 ;

  width  = cols * imagewidth ;
  height = rows * imageheight ;
  create_pixrects(width, height) ;

  maxcols = cols ;          /* Maximum width in columns. */
  column = row = 0 ;        /* Start in top left corner of pixrect. */
  while (fgets(nextline, MAXLINE, fp) != NULL)
    {
      get_userrec(nextline, (isicon ? DISP_BOTH : DISP_ALL)) ;
      adjust() ;            /* Adjust column and row. */
    }
  PCLOSE(fp) ;
  show_display(TRUE) ;
}


void
do_prop_check()
{
  struct recinfo *this, *next ;

  lastmtime = (time_t) 0 ;
  lastsize = 0L ;
  this = recs ;
  while (this != NULL)       /* Force chain of records to be removed. */
    {
      next = this->next ;
      this->total = 0 ;
      this = next ;
    }
  garbage_collect() ;
  write_cmdline() ;
  if (mtype == MONNEW) do_key(KEY_CLEAR) ;
  else                 do_check() ;
}


void
do_users()                   /* Monitor users on a machine. */
{
  struct recinfo *this, *next ;
  FILE *fp ;                 /* File descriptor for users mail spool file. */
  char command[MAXLINE] ;    /* Rusers system call for this machine. */
  char ts[MAXLINE] ;         /* Pointer to login time from rusers line. */
  char username[MAXLINE] ;   /* Name of user logged in. */
 
  noicons = 0 ;
  if (rhostname == NULL) read_str(&rhostname, "localhost") ;
  SPRINTF(command, USERSDEF, rhostname) ;
  if ((fp = popen(command, "r")) == NULL)    /* Connect to rusers command. */
    {
      FPRINTF(stderr,"%s: couldn't get user stats.\n", progname) ;
      return ;
    }
  column = row = 0 ;          /* Start in top left corner of pixrect. */
  while (fgets(nextline, MAXLINE, fp) != NULL)
    {
      SSCANF(&nextline[0], "%s", username) ;
      SSCANF(&nextline[38], "%s", ts) ;
      h_to_c(rhostname, community) ;
      found = make_iconname(facepath, community, username) ;
      add_record("", username, ts, 0) ;
    }
  if (!noicons)
    {
      make_pixrect(1) ;                    /* Just the "no users" icon. */
      add_face(DISP_ALL, NOUSERS, "") ;    /* Set to "no users" icon. */
    }
  else
    {
      make_pixrect(noicons) ;
      add_face(DISP_ICON, NOFACE, "") ;
      this = recs ;
      while (this != NULL)
        {
          next = this->next ;
          add_face(DISP_BOTH, ORDINARY, this->iconname) ;
          if (!dontshowtime)
            text(DISP_OTHER, LEFT, this->ts) ;         /* Timestamp. */
          if (!dontshowuser)
            text(DISP_NAME, LEFT, this->username) ;    /* Username. */
          adjust() ;    /* Adjust column and row. */
          remove_record(this) ;
          this = next ;
        }
      SPRINTF(nextline, "%1d %s", noicons, (noicons == 1 ? "user" : "users")) ;
      text(DISP_ICON, RIGHT, nextline) ;   /* Number of jobs. */
      recs = last = NULL ;
    }
  PCLOSE(fp) ;
  show_display(TRUE) ;
}


void
make_pixrect(count)   /* Make window pixrect the correct size. */
int count ;
{
  int c, r ;          /* Size in columns and rows of window display. */

  r = ((count-1) / maxcols) + 1 ;   /* Number of rows of faces. */
  c = maxcols ;                     /* Full width display. */
  if (count <= maxcols)
    {
      r = 1 ;         /* One row. */
      c = count ;     /* Of 'count' columns. */
    }

  height = r * imageheight ;    /* Height of the icon display. */
  width = c * imagewidth ;      /* Width of the icon display. */
  create_pixrects(width, height) ;
}


void
make_display()              /* Output icons and tidyup chain of records. */
{
  int count ;               /* Name of faces in icon display. */
  struct recinfo *this, *next ;

  count = noicons ;         /* Number of faces to display. */
  if (!count) count = 1 ;   /* Always one "no mail" icon. */
  make_pixrect(count) ;

  count = 0 ;
  if (!noicons) add_face(DISP_ALL, NOMAIL, "") ;
  else
    {
      this = recs ;
      while (this != NULL)
        {
          next = this->next ;
          if (!this->total)
            {
              this = next ;
              continue ;
            }
          if (this->faceimage != NULL)
            {
              STRCPY(face_buf, (char *) this->faceimage) ;
              add_face(DISP_ALL, XFACE, this->iconname) ;
              if (!this->update && update)               /* Update database */
                {
                  do_face_update(this->iconname, face_buf) ;
                  this->update = 1 ;
                }
            }
          else add_face(DISP_ALL, ORDINARY, this->iconname) ;
          count += this->total ;
          if (!dontshowno)
            {
              SPRINTF(nextline, "%1d", this->total) ;
              text(DISP_OTHER, RIGHT, nextline) ;
            }
          if (!dontshowtime) text(DISP_OTHER, LEFT, this->ts) ;
          if (!dontshowuser)
            {
              text(DISP_NAME, LEFT, this->username) ;
              text(DISP_ICON, LEFT, this->username) ;
            }
          adjust() ;
          this = next ;
        }
      SPRINTF(nextline, "%1d", count) ;
      text(DISP_ICON, RIGHT, nextline) ;
      garbage_collect() ;                 /* Remove zero length records. */
    }
  show_display(TRUE) ;         /* Display the latest set of faces. */
}


#ifdef AUDIO_SUPPORT
void
play_sound(filename)       /* Play an audio sound file. */
char *filename ;
{
  char cmd[MAXLINE] ;

  if (do_audio)
    {
      SPRINTF(cmd, "%s %s", audio_cmd, filename) ;
      SYSTEM(cmd) ;
    }
}
#endif /*AUDIO_SUPPORT*/


void
process_face()        /* Extract next line of face image information. */
{

/*  Read in the X-Face: data.
 */

  char *ptr ;

/*  Only take the first occurrence of X-Face: in any given mail item.
 *  This solves several of the possible problems in this area, and
 *  prevents core dumps when an X-Face: line terminates the mail file.
 */

  if (x_face) return ;

  CLEARMEM(face_buf, 2048) ;

/* XXX: This &line[7] stuff is dangerous, and relies on the fact that
 *      fgetheader() pushed the colon back against the field name, which
 *      may well change. This should be fixed.
 */

  ptr = &line[7] ;
  while (LWSP(*ptr)) ptr++ ;
  STRNCPY(face_buf, ptr, strlen(ptr)-1) ;
  face_buf[strlen(ptr)-1] = '\0' ;
  if (uncompface(face_buf) < 0) return ;
  x_face = 1 ;                       /* We have an "on-the-fly" face. */
}


/*  Both process_from() and process_face() need to be fixed to take into
 *  account the possibility that one piece of mail is from multiple people.
 *  This, however, can wait until after we change over to a different method
 *  of face lookup (full name?).
 */
 
/* XXX: The RAND stuff breaks face_ts, since in RAND there is no From_ line.
 *      Timestamps displayed if RAND is true will be garbage.  I don't want
 *      to fix this.  -John.  (If anyone does, the right way to do so is to
 *      parse all the Received: headers, looking for one with "by"
 *      <local-domain>, then locate and parse the 822 date in there and use
 *      that for the timestamp. A potential pitfall is that there may well
 *      be more than one Received: header with "by" <local-domain>; the way
 *      to handle this is to parse_date all of them and only retain the one
 *      with the greatest date. If you don't want to write parse_date, I can
 *      supply it for you.)
 */

void
process_from()             /* Extract information from the From line. */
{
  char *host, *pfline, *user ;
  char temp[MAXLINE], ts[9] ;

  pfline = line ;
  if (EQUAL(line, "From "))
    {
      SSCANF(line, "%s %s %s %s %s %s", temp, temp, temp, temp, temp, ts) ;
      ts[5] = '\0' ;
      STRCPY(face_ts, ts) ;
      froms_found = 1 ;
    }
  else if (is822header(line, "From"))
    {
      if (fromc_found)               /* Only process first From: line. */
        {
          FPRINTF(stderr,
                  "%s: warning: message has more than one `From:' header\n",
                  progname) ;
          return ;
        }
      fromc_found = 1 ;

/*  parse822from() will return non-zero on OK. If it returns zero, there
 *  was an error and we fall back to using parsefrom().  If it was OK,
 *  we check the local-part for routing characters (% or !); if any are
 *  present, we run the local-part through parsefrom and use the result
 *  of that.  Otherwise, we're happy with what parse822from() returned.
 */

      if (parse822from(line, &user, &host))
        {
          if (index(user, '!') == (char *) NULL &&
              index(user, '%') == (char *) NULL)
            {
              STRCPY(face_user, strlower(user)) ;
              STRCPY(face_host, strlower(host)) ;
              return ;
            }
          else
            {
              SPRINTF(temp, "From: %s\n", user) ;
              pfline = temp ;
            }
        }
    }

  if (parsefrom(pfline, &user, &host))
    {
      STRCPY(face_user, strlower(user)) ;
      STRCPY(face_host, strlower(host)) ;
    }
  else FPRINTF(stderr, "%s: couldn't parse '%s'\n", progname, pfline) ;
}


void
process_info()          /* Process From line and face information. */
{
  struct recinfo *crec ;    /* Pointer to current mail record for updating. */
  char *lcomm = realcomm ;
  char *lname = realname ;

  h_to_c(face_host, community) ;    /* Turn hostname into community name. */
  a_to_u(community, face_user, realcomm, realname) ;

  found = make_iconname(facepath, realcomm, realname) ;
  if (x_face && found == ICON_NOTFOUND)
    SPRINTF(iconname, "%s/%s/%s/48x48x1", facepath[0], realcomm, realname) ;

  if (disphost == TRUE && found == ICON_USER)
    {
      lname = realcomm ;
      lcomm = realname ;
    }

  if (mtype == MONNEW)
    {
      if (x_face)
        {
#ifdef AUDIO_SUPPORT
          if (found == ICON_BOTH) check_audio(iconname) ;
#endif /*AUDIO_SUPPORT*/
          add_face(DISP_ALL, XFACE, iconname) ;
          if (update) do_face_update(iconname, face_buf) ;
        }
      else
        {
          if (found == ICON_NOTFOUND)
            add_face(DISP_ALL, NOFACE, (char *) NULL) ;
          else
            {
#ifdef AUDIO_SUPPORT
              check_audio(iconname) ;
#endif /*AUDIO_SUPPORT*/
              add_face(DISP_ALL, ORDINARY, iconname) ;
            }
        }

      if (!dontshowtime) text(DISP_OTHER, LEFT, face_ts) ;
      if (!dontshowuser)
        {
          text(DISP_NAME, LEFT, lname) ;
          text(DISP_ICON, LEFT, lname) ;
        }
      adjust() ;
    }
  else
    {
      if (x_face || (crec = rec_exists(lcomm, lname)) == NULL)
        {
          add_record(lcomm, lname, face_ts, 0) ;
        }
      else
        {
          STRCPY(crec->ts, face_ts) ;
          if (!crec->total) noicons++ ;
          crec->total++ ;
        }
    }
  fromc_found = froms_found = x_face = 0 ;  /* Reset for the next message. */
}


#ifdef NOUTIME
int
utime(file, ubufp)
char *file ;
struct utimbuf *ubufp ;
{
  struct timeval tvp[2] ;

  tvp[0].tv_sec  = ubufp->actime ;
  tvp[0].tv_usec = 0 ;
  tvp[1].tv_sec  = ubufp->modtime ;
  tvp[1].tv_usec = 0 ;

  return utimes(file, tvp) ;
}
#endif /*NOUTIME*/
