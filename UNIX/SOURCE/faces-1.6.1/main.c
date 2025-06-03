
/*  @(#)main.c 1.32 91/11/19
 *
 *  Icon face server for monitoring mail and print jobs.
 *  This program is based on the AT&T v8 mail face server,
 *  vismon, but is not derived from vismon source.
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
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#if !defined(mips) || defined(ultrix)
#include <netdb.h>
#else
#ifdef SYSTYPE_BSD43
#include <netdb.h>
#else
#include <bsd/netdb.h>
#endif
#endif
#include <ctype.h>

#ifdef DNSLOOKUP
#include <sys/socket.h>
#endif /*DNSLOOKUP*/

#ifdef NISLOOKUP
#include <rpcsvc/ypclnt.h>
#endif /*NISLOOKUP*/

#include "faces.h"

#include "noface.xbm"
#include "nomail.xbm"
#include "nopaper.xbm"
#include "noprint.xbm"
#include "nothing.xbm"
#include "nousers.xbm"

char *resources[] = {      /* Faces X resources read. */
  "geometry",              /* String:  open window geometry. */
  "iconGeometry",          /* String:  icon geometry. */
  "foreground",            /* String:  foreground color. */
  "background",            /* String:  background color. */
  "font",                  /* String:  font to use for all text. */
  "backgroundPixmap",      /* String:  open window background pixmap image. */
  "period",                /* Integer: period in seconds between checks. */
  "bell",                  /* Integer: number of beeps for new arrival. */
  "flash",                 /* Integer: number of flashes for new arrival. */
  "lower",                 /* Boolean: lower window if no mail. */
  "raise",                 /* Boolean: raise window on update? */
  "button1clear",          /* Boolean: mouse button 1 clear window? */
  "displayHostname",       /* Boolean: display hostname rather than user. */
#ifdef AUDIO_SUPPORT
  "audioSupport",          /* Boolean: machine has audio support? */
  "audioCommand",          /* String:  command for playing sounds. */
  "bellAudioFile"          /* String: "Bell" audio filename. */
#endif /*AUDIO_SUPPORT*/
} ;

struct comminfo *communities = NULL ;  /* Community alias/username chain. */
struct comminfo *clast = NULL ;     /* End of chain of community records. */
struct hostent *hp = NULL ;
struct machinfo *machines = NULL ;  /* Known machine/communities. */
struct machinfo *mlast = NULL ;     /* End of chain of machine records. */
struct psinfo *psrecs = NULL ;      /* List of news.ps animation files. */
struct psinfo *plast = NULL ;       /* End of chain of NeWS animation files. */
struct recinfo *recs = NULL ;       /* Mail messages being monitored. */
struct recinfo *last = NULL ;       /* End of the chain of mail records. */
struct stat buf ;                   /* Buffer for file existence stat call. */
long lastsize = 0L ;                /* Last known size of the mail folder. */

enum disp_type wdtype = DISP_NAME ; /* Current window display option. */
enum gr_type gtype ;                /* Which graphics system is being used. */
enum min_type found ;               /* Result from make_iconname(). */
enum mon_type mtype = MONNEW ;      /* Type of monitoring operation to do. */

#ifdef AUDIO_SUPPORT
char *audio_cmd  = NULL ;        /* Command to play sound files. */
char *bell_sound = NULL ;        /* "Bell" audio filename. */
int do_audio = 0 ;               /* Set if machine has audio support. */
#endif /*AUDIO_SUPPORT*/

char *bgcolor = NULL ;           /* X11 background color. */
char *bgicon  = NULL ;           /* Alternate background pattern. */
char community[MAXLINE] ;        /* Community name ("real" host name). */
char defdir[MAXLINE] ;           /* The default face directory. */
char *display = NULL ;           /* X11 display information. */
char *envmail ;             /* Pointer to MAIL environment variable value */
char face_buf[2048] ;       /* Buffer for "X-Face" face images. */
char face_host[MAXLINE] ;   /* Pointer to host name from the "From" line. */
char face_ts[MAXLINE] ;     /* Pointer to time stamp from the "From" line. */
char face_user[MAXLINE] ;   /* Pointer to user name from the "From" line. */
char *facedirs ;            /* Colon separated list of face directories. */
char *facepath[MAXPATHS+1] ;     /* Array of face image directories. */
char *faces_label ;              /* Label for the faces window. */
char *fgcolor = NULL ;           /* X11 foreground color. */
char fname[MAXTYPES][MAXLINE] ;  /* Array of various face name types. */
char *fontname = NULL ;          /* X11 font name. */
char *geometry = NULL ;          /* X11 geometry information. */
char hostcommunity[MAXLINE] ;    /* The community of the host machine */
char *hostname ;                 /* Machine name to monitor. */
char *icon_geometry ;       /* X11 geometry information. */
char iconname[MAXLINE] ;    /* Name of the icon file for this person. */
char *iwd ;                 /* Current working directory upon invocation. */
char *line = NULL ;         /* Next RFC822 folded line from user spool file. */
char machfile[MAXLINE] ;    /* Name of the machine/community file. */
char nextline[MAXLINE] ;    /* Input/conversion buffer for various routines. */
char peopfile[MAXLINE] ;    /* Name of the people/username file. */
char *printer ;             /* Printer name to monitor. */
char *progname ;            /* Name of this program. */
char realcomm[MAXLINE] ;    /* Real community for this user. */
char realname[MAXLINE] ;    /* Real username for this user. */
char *rhostname ;           /* Remote host name to monitor (-H option). */
char *spoolfile ;           /* Full pathname of users current mail. */
char *update_alias ;        /* Name of mail alias for database updates. */
char *username ;            /* This users name. */
char *userprog ;            /* User supplied program to run. */

#ifdef NISLOOKUP
char *hostdomain ;               /* NIS domain of host machine */
char domaincommunity[MAXLINE] ;  /* Community for the current domain */
#endif /*NISLOOKUP*/

int beeps = 0 ;         /* Number of beeps for arrival of new mail. */
int button1clear = 0 ;  /* Set if button 1 should clear the window. */
int column ;            /* Column number for next icon. */
int debug = 0 ;         /* Set with the -D command line option. */
int disphost = 0 ;      /* Display hostname if icon represents user. */
int dontshowno = 0 ;    /* Set if number of messages shouldn't be displayed. */
int dontshowtime = 0 ;  /* Set if timestamp for new mail shouldn't be shown. */
int dontshowuser = 0 ;  /* Set if username for new mail shouldn't be shown. */

int facetype ;          /* Type of face file found. */
int firsttime = 1 ;     /* Zeroised after first mail/printer check. */
int flashes = 0 ;       /* Number of flashes for arrival of new mail. */
int fromc_found = 0 ;   /* Set if "From:" line found during processing. */
int froms_found = 0 ;   /* Set if "From " line found during processing. */
int height ;            /* Height in pixels of faces display. */

int iconheight = ICONHEIGHT ;  /* Height of an individual icon. */
int iconpos = 0 ;              /* Set if icon position given (-WP option). */
int iconwidth = ICONWIDTH ;    /* Width of an individual icon. */
int iconic = 0 ;               /* Faces window is closed if set. */
int imageheight = ICONHEIGHT ;  /* Height of an individual face image. */
int imagewidth = ICONWIDTH ;    /* Width of an individual face image. */
int invert = 0 ;               /* Set if to use reverse video. */
int ix = 0 ;                   /* Initial X position of the icon. */
int iy = 0 ;                   /* Initial Y position of the icon. */
int lowerwindow = 0 ;          /* Set to lower window when no mail. */
int maxcols = NO_PER_ROW ;     /* Maximum number of columns to display. */

int maxline ;       /* current size of RFC822 folded line buffer */
int maxtypes ;      /* Maximum number of icon types for graphics target. */
int mboxshrunk = 0 ;   /* Set when the spoolfile size has shrunk. */
int mhflag = 0 ;    /* Set if this user uses MH to read mail. */
int newmail ;       /* Set if there is new mail this time around. */
int noicons ;       /* Number of faces this time around. */
int nolabel = 0 ;   /* Whether the faces window has a frame label. */
int period = 60 ;   /* Period in seconds for new mail check. */
int posspec = 0 ;   /* Set if -Wp or -g option is present (for X11) */
int raisewindow = 0 ;  /* Set if we are to raise window on update */
int row ;           /* Row number for next icon. */
int toclear = 0 ;   /* Set if faces memory area needs clearing. */
int update = 0 ;    /* If set, send mail to update faces database. */
int width ;         /* Width in pixels of faces display. */
int wx = 0 ;        /* Initial X position of the window. */
int wy = 0 ;        /* Initial Y position of the window. */
int x_face = 0 ;    /* Set if we've extracted an "on-the-fly" face. */

/*  256-byte table for quickly reversing the bits in an unsigned 8-bit char,
 *  used to convert between MSBFirst and LSBFirst image formats.
 */

char revtable[256] = {
        0, -128,   64,  -64,   32,  -96,   96,  -32,
       16, -112,   80,  -48,   48,  -80,  112,  -16,
        8, -120,   72,  -56,   40,  -88,  104,  -24,
       24, -104,   88,  -40,   56,  -72,  120,   -8,
        4, -124,   68,  -60,   36,  -92,  100,  -28,
       20, -108,   84,  -44,   52,  -76,  116,  -12,
       12, -116,   76,  -52,   44,  -84,  108,  -20,
       28, -100,   92,  -36,   60,  -68,  124,   -4,
        2, -126,   66,  -62,   34,  -94,   98,  -30,
       18, -110,   82,  -46,   50,  -78,  114,  -14,
       10, -118,   74,  -54,   42,  -86,  106,  -22,
       26, -102,   90,  -38,   58,  -70,  122,   -6,
        6, -122,   70,  -58,   38,  -90,  102,  -26,
       22, -106,   86,  -42,   54,  -74,  118,  -10,
       14, -114,   78,  -50,   46,  -82,  110,  -18,
       30,  -98,   94,  -34,   62,  -66,  126,   -2,
        1, -127,   65,  -63,   33,  -95,   97,  -31,
       17, -111,   81,  -47,   49,  -79,  113,  -15,
        9, -119,   73,  -55,   41,  -87,  105,  -23,
       25, -103,   89,  -39,   57,  -71,  121,   -7,
        5, -123,   69,  -59,   37,  -91,  101,  -27,
       21, -107,   85,  -43,   53,  -75,  117,  -11,
       13, -115,   77,  -51,   45,  -83,  109,  -19,
       29,  -99,   93,  -35,   61,  -67,  125,   -3,
        3, -125,   67,  -61,   35,  -93,   99,  -29,
       19, -109,   83,  -45,   51,  -77,  115,  -13,
       11, -117,   75,  -53,   43,  -85,  107,  -21,
       27, -101,   91,  -37,   59,  -69,  123,   -5,
        7, -121,   71,  -57,   39,  -89,  103,  -25,
       23, -105,   87,  -41,   55,  -73,  119,   -9,
       15, -113,   79,  -49,   47,  -81,  111,  -17,
       31,  -97,   95,  -33,   63,  -65,  127,   -1,
} ;


int
main(argc, argv)
int argc ;
char *argv[] ;
{
  read_str(&progname, argv[0]) ;  /* Save this programs name. */
  iwd = (char *) getcwd(NULL, MAXPATHLEN) ;    /* Save current working dir. */
  if (iwd == NULL) iwd = "." ;
  get_display(argc, argv) ;       /* Get possible X11 display information. */
  init_graphics(&argc, argv) ;
  initialise() ;                  /* Set default values for various options. */
  load_resources() ;              /* Get resources from various places. */
  read_resources() ;              /* Read resources from merged database. */
  get_options(argc, argv) ;       /* Read and process command line options. */
  read_mailrc() ;                 /* Get number of flashes and beeps. */
  read_machines() ;               /* Setup up the machine/community chain. */
  read_aliases() ;                /* Setup the hostname alias subchains. */

  if (init_ws_type(argc, argv))   /* Determine window system type. */
    {
      FPRINTF(stderr,"Error initialising window system.\n") ;
      exit(1) ;
    }

  load_data(NOFACE,  noface_bits) ;
  load_data(NOMAIL,  nomail_bits) ;
  load_data(NOPAPER, nopaper_bits) ;
  load_data(NOPRINT, noprint_bits) ;
  load_data(NOTHING, nothing_bits) ;
  load_data(NOUSERS, nousers_bits) ;
  make_area(DISP_ICON, imagewidth, imageheight) ;

/*  Initialize the host community to the official name of the current host
 *  (or it's translation via the machines table.)
 */

  GETHOSTNAME(nextline, MAXLINE) ;
  read_str(&hostname, nextline) ;
#ifndef SYSV32
  hp = gethostbyname(hostname) ;
#endif /*SYSV32*/
  if (hp != NULL) read_str(&hostname, strlower(hp->h_name)) ;
  h_to_c(hostname, hostcommunity) ;

#ifdef NISLOOKUP
  {
    struct machinfo *temp ;

    yp_get_default_domain(&hostdomain) ;
    STRCPY(domaincommunity, hostdomain) ;

    IF_DEBUG( FPRINTF(stderr, "main: NIS community is `%s`\n",
                      domaincommunity) ) ;

/*  We can't call h_to_c() to setup the default because it expects
 *  it to be set already; so we grovel through the machine/community
 *  association list directly.
 */

    for (temp = machines; temp != NULL; temp = temp->next)
     if (!strcmp(temp->machine, domaincommunity))
       {
         char *str ;

         STRCPY(domaincommunity, temp->community) ;
         for (str = community;  *str;  str++)
           if (isascii(*str) && isupper(*str)) *str = tolower(*str) ;
         break ;
       }

    IF_DEBUG( FPRINTF(stderr, "main: domain community is `%s`\n",
                      domaincommunity) ; )
  }
#endif /*NISLOOKUP*/

  make_icon() ;                 /* Create default faces icon. */
  make_frame(argc, argv) ;      /* Create faces window/icon. */
  do_check() ;                  /* Generate initial faces display. */
  write_cmdline() ;             /* Save faces command line. */
  start_tool() ;                /* Start up and display the faces icon. */
  exit(0) ;
/*NOTREACHED*/
}


/* Turn possible alias into username. */

void
a_to_u(community, user, realcomm, realname)
char *community, *user, *realcomm, *realname ;
{
  struct comminfo *ctemp ;
  struct peopinfo *ptemp ;

  STRCPY(realname, user) ;         /* In case alias not found. */
  STRCPY(realcomm, community) ;    /* In case alias not found. */
  ctemp = communities ;            /* Point to chain of communities. */
  while (ctemp != NULL)
    if (!strcmp(ctemp->community, community))
      {
        ptemp = ctemp->people ;
        while (ptemp != NULL)
          if (!strcmp(ptemp->alias, user))
            {
              STRCPY(realname, ptemp->username) ;
              STRCPY(realcomm, ptemp->community) ;
              return ;
            }
          else ptemp = ptemp->next ;
        return ;
      }
    else ctemp = ctemp->next ;
}


int
get_bool_resource(rtype, boolval)  /* Get boolean resource from the server. */
enum res_type rtype ;
int *boolval ;
{
  char *val, tempstr[MAXLINE] ;
  int len, n ;

  if ((val = get_resource(rtype)) == NULL) return(0) ;
  STRCPY(tempstr, val) ;
  len = strlen(tempstr) ;
  for (n = 0; n < len; n++)
    if (isupper(tempstr[n])) tempstr[n] = tolower(tempstr[n]) ;
  if (EQUAL(tempstr, "true")) *boolval = TRUE ;
  else                        *boolval = FALSE ;
  return(1) ;
}


/*  Search through the command line options to see if a -d <display>
 *  has been given. We can't process the command line arguments first,
 *  (using get_options()), because the SunView and XView versions, are
 *  capable of having a lot of generic command line arguments which would
 *  be a pain to handle. The init_graphics() routine will strip those off,
 *  but the X11 version, needs the display name.
 */

void
get_display(argc, argv)
int argc ;
char *argv[] ;
{
  INC ;
  while (argc > 0)
    {
      if (argv[0][0] == '-' && argv[0][1] == 'd')
        {
          INC ;               /* X11 display information. */
          getparam(&display, argv, "-d needs display information") ;
          return ;
        }
      INC ;
    }
}


int
get_int_resource(rtype, intval)   /* Get integer resource from the server. */
enum res_type rtype ;
int *intval ;
{  
  char *val ;
   
  if ((val = get_resource(rtype)) == NULL) return(0) ;
  *intval = atoi(val) ;
  return(1) ;
}


int
get_str_resource(rtype, strval)   /* Get a string resource from the server. */
enum res_type rtype ;
char *strval ;
{
  char *val ;

  if ((val = get_resource(rtype)) == NULL) return(0) ;
  STRCPY(strval, val) ;
  return(1) ;
}


#ifdef USE_GETWD
char *
getcwd(buf, size)
char *buf ;
int size ;
{
  if (buf == NULL) buf = Malloc(size) ;
  return(getwd(buf)) ;
}
#endif /*USE_GETWD*/


void
initialise()
{
  char sfile[MAXLINE] ;
  int i ;

  read_str(&faces_label, "faces") ;     /* Default title line label. */

  facepath[0] = FACEDIR ;    /* Default path is single built-in directory. */
  for (i = 1;  i <= MAXPATHS;  i++) facepath[i] = NULL ;

#ifdef AUDIO_SUPPORT
#ifdef AUDIO_CMD
  read_str(&audio_cmd, AUDIO_CMD) ;     /* Alternate audio command. */
#else
  read_str(&audio_cmd, AUDIODEF) ;      /* Default audio command. */
#endif /*AUDIO_CMD*/
#endif /*AUDIO_SUPPORT*/

#ifdef FMONTYPE
  mtype = FMONTYPE ;                    /* Type of monitoring to do. */
#endif /*FMONTYPE*/

#ifdef BACKGROUND
  read_str(&bgicon, BACKGROUND) ;       /* Alternate background pattern. */
#endif /*BACKGROUND*/

#ifdef INVERT
  invert = 1 ;               /* Display in reverse video. */
#endif /*INVERT*/

#ifdef PERIOD
  period = PERIOD ;          /* Period for new mail/print check. */
#endif /*PERIOD*/

#ifdef DONTSHOWNO
  dontshowno = 1 ;           /* Don't show number of messages on images. */
#endif /*DONTSHOWNO*/

#ifdef SPOOLFILE
  read_str(&spoolfile, SPOOLFILE) ;   /* Alternative spoolfile to monitor. */
#else
  if ((envmail = getenv("MAIL")) != NULL) read_str(&spoolfile, envmail) ;
  else
    {
      username = getname() ;     /* Get users name from passwd entry. */
#ifdef SPOOLDIR
      SPRINTF(sfile, "%s/%s", SPOOLDIR, username) ;
#else
      SPRINTF(sfile, "/usr/spool/mail/%s", username) ;
#endif /*SPOOLDIR*/
      read_str(&spoolfile, sfile) ;
    }
#endif /*SPOOLFILE*/

#ifdef UPDATE
  read_str(&update_alias, UPDATE) ;   /* Alt. faces database update alias. */
#else
  read_str(&update_alias, UPDATE_ALIAS) ;
#endif /*UPDATE*/

#ifdef DONTSHOWTIME
  dontshowtime = 1 ;           /* Don't show timestamp on images. */
#endif /*DONTSHOWTIME*/

#ifdef DONTSHOWUSER
  dontshowuser = 1 ;           /* Don't show username on images. */
#endif /*DONTSHOWUSER*/
}


void
h_to_c(host, community)        /* Turn hostname into community name. */
char *host, *community ;
{
#ifdef NISLOOKUP
  char *hostinfo ;             /* Info returned from yp_match */
  int len ;                    /* Length of returned hostinfo */
  int yp_err ;                 /* Result from yp_match() */
#endif /*NISLOOKUP*/
  struct machinfo *temp ;      /* Pointer to next machine record. */
  char *str ;                  /* Temp ptr used for uppercasing string */

  if (host[0] == '\0')
    {

/* Host not specified means use current host - return cached community. */

      STRCPY(community, hostcommunity) ;
      return ;
    }
  else STRCPY(community, host) ;   /* Copied in case machine name not found. */

/* Grovel through the machine/community association list. */

  for (temp = machines; temp != NULL; temp = temp->next)
    if (soft_match(temp->machine, community) == 0)
      {
        STRCPY(community, temp->community) ;
        IF_DEBUG( FPRINTF(stderr,
                          "h_to_c(): machine `%s` found in community '%s'\n",
                          temp->machine, community) ; )
        return ;
      }

#ifdef DNSLOOKUP
  {
    char commbuff[256] ;
    struct hostent *found ;

    if (index(community, '.'))
      {
        found = gethostbyname(community) ;
      }
    else
      {
        STRCPY(commbuff, community) ;
        STRCAT(commbuff, ".") ;
        if (!(found = gethostbyname(commbuff)))
          found = gethostbyname(community) ;
      }
 
    if (found)
      {
         IF_DEBUG( FPRINTF(stderr, "DNSLOOKUP found %s as %s\n",
                           community, found->h_name) ) ;
         STRCPY(community, found->h_name) ;

         for (str = community;  *str;  str++)
           if (isascii(*str) && isupper(*str)) *str = tolower(*str) ;
       
/* Grovel through the machine/community association list again
 * to allow domains to be aliases.
 */

         for (temp = machines; temp != NULL; temp = temp->next)
           if (!strcmp(temp->machine, community))
             {
               STRCPY(community, temp->community) ;
               IF_DEBUG( FPRINTF(stderr,
                         "h_to_c(): machine `%s` found in community '%s'\n",
                         temp->machine, community) ; )
               return ;
             }
         return ;
      }
  }
#endif /*DNSLOOKUP*/

#ifdef NISLOOKUP

/*  yp_match() returns zero on success. We will treat any failure as
 *  a simple `no match' condition.
 */

  yp_err = yp_match(hostdomain, "hosts.byname",
                    host, strlen(host), &hostinfo, &len) ;

/* We aren't interested in the value returned, only whether the key
 * was found...
 */

  if (yp_err == 0)
    {
      STRCPY(community, domaincommunity) ;
      for (str = community;  *str;  str++)
        if (isascii(*str) && isupper(*str)) *str = tolower(*str) ;
 
/* Don't bother checking for domaincommunity in the machines
 * alias list - that should have been done when the variable
 * was initialized.
 */
 
      IF_DEBUG( FPRINTF(stderr, "NISlookup: found %s\n", host) ) ;
      return ;
    }
  IF_DEBUG( else FPRINTF(stderr, "NISlookup: Error %d\n", yp_err) ) ;
  IF_DEBUG( FPRINTF(stderr, "h_to_c(): community = '%s'\n", community) ) ;
#endif /*NISLOOKUP*/

  for (str = community; *str; str++)
    if (isascii(*str) && isupper(*str)) *str = tolower(*str) ;
}


enum min_type
make_iconname(facepath, community, user)    /* Construct the icon name. */
char *facepath[MAXPATHS+1], *community, *user ;
{

/*  Sets up community and user based on the first successful
 *  open from the following list of files:
 *
 *  $(FACEDIR)/ity/un/comm/user/[face.ps, sun.icon, 48x48x1, face.xbm]
 *  $(FACEDIR)/community/user/[face.ps, sun.icon, 48x48x1, face.xbm]
 *  $(FACEDIR)/MISC/user/[face.ps, sun.icon, 48x48x1, face.xbm]
 *  $(FACEDIR)/community/unknown/[face.ps, sun.icon, 48x48x1, face.xbm]
 *  $(FACEDIR)/MISC/unknown/[face.ps, sun.icon, 48x48x1, face.xbm]
 *
 *  Firstly a check is made for the "old" style. This is where the username
 *  is not a directory but the ikon in 48x48x1 format. $(FACEDIR) is
 *  replaced by all the face directories given.
 *
 *  If none of these are found, the "blank face" is returned.
 *  
 *  Return code is 0 if not found              - ICON_NOTFOUND.
 *                 1 unknown in misc.          - ICON_MISC_UNKNOWN.
 *                 2 user found, not community - ICON_USER.
 *                 3 community found, not user - ICON_COMMUNITY.
 *                 4 both.                     - ICON_BOTH.
 */

  char *cptr, *icomm[3], *iuser[3], *path ;
  char s[MAXPATHLEN], *t ;
  int i, ic, id, iu ;
  int savecode = ICON_NOTFOUND ;

#ifndef  LOWER_NAMES
  icomm[0] = community ;
#else  /* LOWER_NAMES */
  icomm[1] = strlower(community) ;
#endif  /* LOWER_NAMES */
  icomm[1] = "MISC" ;
  icomm[2] = NULL ;

  iuser[0] = user ;
  iuser[1] = "unknown" ;
  iuser[2] = NULL ;

  IF_DEBUG( FPRINTF(stderr, "make_iconname: %s[%s]\n", community, user) ; )

  for (iu = 0; iuser[iu] != NULL; iu++)
    for (ic = 0; icomm[ic] != NULL; ic++)
      {
        s[0]='\0' ;
        if (! searchfacedb(s, facepath, icomm[ic], iuser[iu])) continue ;

        IF_DEBUG( FPRINTF(stderr, "\t<searchdb succeeded>\n", iconname) ; )

#ifdef  REGULAR_FILE_BUT_BROKEN
        if ((buf.st_mode & S_IFMT) == S_IFREG)
          {
            IF_DEBUG( FPRINTF(stderr, "\tfound as %s\n", iconname) ; )
            return((enum min_type) (4 - (((ic & 1) << 1) + (iu & 1)))) ;
          }
#endif  REGULAR_FILE_BUT_BROKEN
        t = iconname + strlen(iconname) ;
        *t++ = '/' ;
        for (i = 0; i < maxtypes; i++)
          {
            STRCPY(t, fname[i]) ;
            IF_DEBUG( FPRINTF(stderr, "\ttrying %s\n", iconname) ; )
            if (stat(iconname, &buf) != -1)
              {
                IF_DEBUG( FPRINTF(stderr, "\tfound as %s\n", iconname) ; )
#ifdef NAMEUNKNOWN
                if (EQUAL(iuser[iu], "unknown")) STRCPY(user, "unknown") ;
#endif /*NAMEUNKNOWN*/
                return((enum min_type) (4 - (((ic & 1) << 1) + (iu & 1)))) ;
              }
          }
      }

/* Face icon not found, so the "blank face" should be used. */

  *iconname = '\0' ;
  return(ICON_NOTFOUND) ;
}


void
read_mailrc()                /* Get number of flashes and beeps. */
{
  char mrcname[MAXLINE] ;    /* Full pathname of the .mailrc file. */
  char *ptr ;
  FILE *fd ;

  if (getenv("HOME"))
    {
      SPRINTF(mrcname, "%s/.facesrc", getenv("HOME")) ;
      if ((fd = fopen(mrcname, "r")) == NULL)
        {
          SPRINTF(mrcname, "%s/.mailrc", getenv("HOME")) ;
          if ((fd = fopen(mrcname, "r")) == NULL) return ;
        }
      while (fgets(nextline, MAXLINE, fd) != NULL)
        if (EQUAL(nextline, "set"))
          {
            if ((ptr = index(nextline, ' ')) == NULL) continue ;
            if (EQUAL(ptr+1, "flash"))
              {
                if ((ptr = index(nextline, '=')) == NULL) continue ;
                SSCANF(ptr+1, "%d", &flashes) ;
              }
            else if (EQUAL(ptr+1, "bell"))
              {
                if ((ptr = index(nextline, '=')) == NULL) continue ;
                SSCANF(ptr+1, "%d", &beeps) ;
              }
            else if (EQUAL(ptr+1, "raise"))        raisewindow  = 1 ;
            else if (EQUAL(ptr+1, "button1clear")) button1clear = 1 ;
          }
    }
  FCLOSE(fd) ;
}


void
read_resources()            /* Read all possible resources from database. */
{
  int boolval, intval ;
  char str[MAXLINE] ;

  if (get_str_resource(R_WINGEOM,   str))      read_str(&geometry,      str) ;
  if (get_str_resource(R_ICONGEOM,  str))      read_str(&icon_geometry, str) ;
  if (get_str_resource(R_FGCOLOR,   str))      read_str(&fgcolor,       str) ;
  if (get_str_resource(R_BGCOLOR,   str))      read_str(&bgcolor,       str) ;
  if (get_str_resource(R_FONT,      str))      read_str(&fontname,      str) ;
  if (get_str_resource(R_BGICON,    str))      read_str(&bgicon,        str) ;

  if (get_int_resource(R_PERIOD,    &intval))  period       = intval ;
  if (get_int_resource(R_BELL,      &intval))  beeps        = intval ;
  if (get_int_resource(R_FLASH,     &intval))  flashes      = intval ;

  if (get_bool_resource(R_LOWER,    &boolval)) lowerwindow  = boolval ;
  if (get_bool_resource(R_RAISE,    &boolval)) raisewindow  = boolval ;
  if (get_bool_resource(R_BUT1CLR,  &boolval)) button1clear = boolval ;
  if (get_bool_resource(R_DISPHOST, &boolval)) disphost     = boolval ;

#ifdef AUDIO_SUPPORT
  if (get_str_resource(R_AUDIOCMD,  str))      read_str(&audio_cmd,  str) ;
  if (get_str_resource(R_BELLFILE,  str))      read_str(&bell_sound, str) ;
  if (get_bool_resource(R_AUDIO,    &boolval)) do_audio = boolval ;
#endif /*AUDIO_SUPPORT*/
}


void
read_str(str, value)
char **str, *value ;
{
  if (*str != NULL) (void) FREE(*str) ;
  if (value != NULL && strlen(value))
    {
      *str = (char *) malloc((unsigned) (strlen(value) + 1)) ;
      STRCPY(*str, value) ;
    }
  else *str = NULL ;
}


int
searchfacedb(b, facepath, community, user)
char *b, *facepath[MAXPATHS+1], *community, *user ;
{
  char *s, *t ;
  int i, id ;

  i = FALSE ;
  t = b + strlen(b) ;
  if (*community != (char) NULL)
    {
      s = rindex(community, '.') ;
      if (s == (char *) NULL)
        {
          STRCAT(strcat(t, "/"), community) ;
          community = "" ;
          s = "." ;
        }
      else
        {   
          *s = '\0' ;
          STRCAT(strcat(t, "/"), s+1) ;
        }
      i = searchfacedb(b, facepath, community, user) ;
      *t = '\0' ;
      *s = '.' ;
      if (i) return(i) ;
    }

  STRCAT(strcat(t, "/"), user) ;
  for (id = 0; !i && facepath[id] != NULL; id++)
    {
      char *temp[MAXPATHLEN] ;

      STRCAT(strcpy(temp, facepath[id]), b) ;
      IF_DEBUG( FPRINTF(stderr, "\ttrying %s\n", temp) ; )
      if (!i && (i = (stat(temp, &buf) != -1)))
        {
          IF_DEBUG( FPRINTF(stderr, "\tfound %s\n", temp) ; )
          STRCPY(iconname, temp) ;
        }
    }
  *t = '\0' ;
  return(i) ;
}


int
soft_match(d_ptr, c_ptr)
register char *c_ptr, *d_ptr ;
{
  if (*d_ptr == '*')
    {
      d_ptr++ ;
      c_ptr = c_ptr + strlen(c_ptr) - strlen(d_ptr) ;
    }
  return(!EQUAL(c_ptr, d_ptr)) ;
}

