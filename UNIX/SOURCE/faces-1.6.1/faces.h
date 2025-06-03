
/*  @(#)faces.h 1.28 91/11/19
 *
 *  Contains all the global definitions used by faces.
 *
 *  Copyright (c) Rich Burridge - Sun Microsystems Australia.
 *                                All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 *
 *  No responsibility is taken for any errors or inaccuracies inherent
 *  either to the comments or the code of this program, but if
 *  reported to me then an attempt will be made to fix them.
 */

#ifndef FALSE
#define  FALSE  (0 == 1)
#define  TRUE   (0 == 0)
#endif /*FALSE*/

/* If your compiler can handle inline functions, define INLINE as `inline',
 * otherwise define it as `'.
 */

#ifdef __GNU_CC__
#define  INLINE  inline
#else
#define  INLINE
#endif /*__GNU_CC__*/

#ifdef DEBUG
#define  IF_DEBUG(stmt)  stmt
#else
#define  IF_DEBUG(stmt)
#endif /*DEBUG*/

/* For all function declarations, if ANSI then use a prototype. */

#if  defined(__STDC__)
#define P(args)  args
#else  /* ! __STDC__ */
#define P(args)  ()
#endif  /* STDC */

#ifndef FACEDIR
#define  FACEDIR  "/usr/local/faces"
#endif /*FACEDIR*/

#define  CLOSE         (void) close       /* To make line happy. */
#define  FCLOSE        (void) fclose
#define  FFLUSH        (void) fflush
#define  FGETS         (void) fgets
#define  FPRINTF       (void) fprintf
#define  FPUTS         (void) fputs
#define  FREE          (void) free
#define  FSEEK         (void) fseek
#define  GET_SUN_ICON  (void) get_sun_icon
#define  IOCTL         (void) ioctl
#define  PCLOSE        (void) pclose
#define  POLL          (void) poll
#define  PUTC          (void) putc
#define  SELECT        (void) select
#define  SSCANF        (void) sscanf
#define  SPRINTF       (void) sprintf
#define  STRCAT        (void) strcat
#define  STRCPY        (void) strcpy
#define  STRNCAT       (void) strncat
#define  STRNCPY       (void) strncpy
#define  SYSTEM        (void) system
#define  UTIME         (void) utime
#define  WRITE         (void) write

/* Window display types. */
enum disp_type { DISP_NAME,      /* Window type 1. */
                 DISP_OTHER,     /* Window type 2. */
                 DISP_ICON,      /* Icon display. */
                 DISP_BOTH,      /* Both window types. */
                 DISP_ALL,       /* Both window types and the icon. */
} ;

enum dir_type { FORWARD, BACKWARD } ;   /* Mail folder reading directions. */

/* Different fields in a program record. */
enum field_type { REC_USER,  REC_HOST,
                  REC_LWIN,  REC_RWIN,
                  REC_LICON, REC_RICON } ;

/* Different types of possible face images. */
enum icon_type { BACKIMAGE, NOFACE,  NOMAIL, NOPAPER, NOPRINT,
                 NOTHING,   NOUSERS, ORDINARY, XFACE } ;

enum image_type { ONSCREEN, OLDOFF, CUROFF } ;   /* Image types. */

enum just_type { LEFT, RIGHT } ;                 /* Text justification. */

/* Possible make_iconname() return codes. */
enum min_type { ICON_NOTFOUND,  ICON_MISC_UNKNOWN, ICON_USER,
                ICON_COMMUNITY, ICON_BOTH } ;

/* Different types of file monitoring performed by this program. */
enum mon_type { MONNEW, MONALL, MONPRINTER, MONPROG, MONUSERS } ;

/* X resources used by faces. */
enum res_type { R_WINGEOM, R_ICONGEOM, R_FGCOLOR, R_BGCOLOR,
                R_FONT,    R_BGICON,   R_PERIOD,  R_BELL,
                R_FLASH,   R_LOWER,    R_RAISE,   R_BUT1CLR, R_DISPHOST,
#ifdef AUDIO_SUPPORT
                R_AUDIO,   R_AUDIOCMD, R_BELLFILE
#endif /*AUDIO_SUPPORT*/
} ;

/* Different graphics systems. Appropriate one set in gtype. */
enum gr_type { NEWS, SVIEW, X11, XVIEW } ;

/* Determine order for face type lookup. */
#define  NEWSTYPE        0
#define  SUNTYPE         1
#define  BLITTYPE        2
#define  X11TYPE         3

/* NeWS return event values. */
#define  DIED            100  /* Faces has been zapped. */
#define  PAINTED         101  /* Canvas/Icon needs repainting. */
#define  LEFTDOWN        102  /* Left mouse button has been pressed. */

/* Keyboard actions. */
#define  KEY_CLEAR       '\177'   /* Clear faces display. */
#define  KEY_REFRESH     '\014'   /* Refresh faces display. */
#define  KEY_QUIT        'q'      /* Terminate faces display. */

#define  BLITHEIGHT      48       /* Maximum number of lines in a blit icon. */

#ifdef   USE_BZERO
#define  CLEARMEM(arg, len)  bzero(arg, len)
#else
#define  CLEARMEM(arg, len)  memset(arg, '\0', len)
#endif /*USE_BZERO*/

#ifdef   NOGETHOSTNAME
#include <sys/utsname.h>
#include <sys/systeminfo.h>

#define  GETHOSTNAME(name, len)  sysinfo(SI_HOSTNAME, name, len)
#else
#define  GETHOSTNAME(name, len)  (void) gethostname(name, len)
#endif /*NOGETHOSTNAME*/

#define  EQUAL(str,val)  !strncmp(str,val,strlen(val))
#define  ICONHEIGHT      64   /* Default height of individual face icons. */
#define  ICONWIDTH       64   /* Default width of individual face icons. */
#define  INC             argc-- ; argv++ ;

#ifndef  LINT_CAST
#ifdef   lint
#define  LINT_CAST(arg)  (arg ? 0 : 0)
#else
#define  LINT_CAST(arg)  (arg)
#endif /*lint*/
#endif /*LINT_CAST*/

#define  MAXFIELDS       6    /* Maximum number of fields in program record. */
#define  MAXICONS        9    /* Maximum number of different icon types. */
#define  MAXITYPES       3    /* Maximum number of image types. */

#ifndef  MAXLINE
#define  MAXLINE         200  /* Maximum length for character strings. */
#endif /*MAXLINE*/

#ifndef  MAXPATHS
#define	 MAXPATHS        10   /* Maximum number of dirs in FACEPATH list */
#endif /*MAXPATHS*/

#define  MAXTYPES        4    /* Maximum number of different face types. */
#define  NO_PER_ROW      10   /* Default number of faces per row. */

#ifdef  NOINDEX
#define  index        strchr
#define  rindex       strrchr
#endif /*NOINDEX*/

#ifdef NOMAXPATHLEN
#define  MAXPATHLEN   1024
#endif /*NOMAXPATHLEN*/

#ifdef NOUTIME
#include <sys/types.h>

struct utimbuf {      /* Make a utime() definition for systems without it. */
  time_t actime ;
  time_t modtime ;
} ;
#endif /*NOUTIME*/

extern char *getenv P(()) ;
extern char *malloc P(()) ;
extern char *realloc P(()) ;
extern char *index P(()) ;
extern char *rindex P(()) ;
extern char *strcpy P(()) ;
extern char *strlower P(()) ;

#if !defined(SYSV32) && !defined(__DGUX__)
extern FILE *popen P(()) ;
#endif /*SYSV32*/

/*  Various programs that are used by faces.
 *  You might need to change these on some machines.
 */

#ifdef AUDIO_SUPPORT
/* Playing sound files:           %s replaced with audio filename. */
#define  AUDIODEF   "play %s" 
#endif /*AUDIO_SUPPORT*/ 

/* Monitoring the printer:        %s replaced by printer name. */
#define  PRINTDEF   "lpq -P %s"

/* Updating the faces database:   %s replaced with mail update alias name. */
#define  UPDATEDEF  "/usr/lib/sendmail -oeq -oi %s"

/* Monitoring users on a machine: %s replaced with hostname. */
#define  USERSDEF   "rusers -l %s"

/* Default definitions used by faces. They can be globally changed here. */

#define  MACHINETAB    "machine.tab"   /* Machine aliases table. */
#define  PEOPLETAB     "people.tab"    /* Username aliases table. */
#define  UPDATE_ALIAS  "facemaker"     /* Faces database update alias. */

struct machinfo                 /* Machine/community record. */
  {
    char *machine ;             /* Machine name. */
    char *community ;           /* Community it belongs to. */
    struct machinfo *next ;     /* Pointer to next record. */
  } ;

struct comminfo                 /* Community alias/username records. */
  {
    char *community ;           /* Community name. */
    struct peopinfo *people ;   /* Chain of alias/usernames. */
    struct comminfo *next ;     /* Pointer to next record. */
  } ;

struct peopinfo                 /* Username/alias record. */
  {
    char *alias ;               /* Alias for this user. */
    char *community ;           /* Real community. */
    char *username ;            /* Real username. */
    struct peopinfo *next ;     /* Pointer to next record. */
  } ;

struct psinfo                   /* News.ps animation records. */
  {
    char *name ;                /* Full pathname of news.ps file. */
    int row ;                   /* Row number where animation will occur. */
    int column ;                /* Column number where animation will occur. */
    struct psinfo *next ;       /* Pointer to next record. */
  } ;

struct recinfo                  /* Mail/print information record. */
  {
    char *community ;           /* Community name for this person. */
    unsigned char *faceimage ;  /* X-Face image or NULL. */
    char *iconname ;            /* Name of iconfile for this person. */
    char *username ;            /* User name for this person. */
    int total ;                 /* Total number of messages/print jobs. */
    int size ;                  /* Total size in bytes of print job. */
    int update ;                /* If set, the database has been updated. */
    char ts[6] ;                /* Latest timestamp for this user. */
    struct recinfo *next ;      /* Pointer to next record. */
  } ;

enum min_type make_iconname P((char *[MAXPATHS+1], char *, char *)) ;

unsigned short get_hex P((FILE *)) ;

struct recinfo *rec_exists P((char *, char *)) ;

char *Malloc P((int)) ;
char *get_resource P((enum res_type)) ;
char *get_token P((char *)) ;
char *getname P(()) ;
char *strlower P((char *)) ;

int casecmp P((int, int)) ;
int get_blit_ikon P((char *, unsigned short [256])) ;
int get_bool_resource P((enum res_type, int *)) ;
int get_int_resource P((enum res_type, int *)) ;
int get_icon P((char *, unsigned short [256])) ;
int get_news_icon P((char *)) ;
int get_str_resource P((enum res_type, char *)) ;
int get_sun_icon P((char *, unsigned short [256])) ;
int get_x11_icon P((char *, unsigned short [256])) ;
int init_ws_type P((int, char *[])) ;
int is822header P((char *, char *)) ;
int LWSP P((int)) ;
int main P((int, char *[])) ;
int parsefrom P((char *, char **, char **)) ;
int searchfacedb P((char *, char *[], char *, char *)) ;
int soft_match P((char *, char *)) ;

#ifdef NOUTIME
int utime P((char *, struct utimbuf *)) ;
#endif /*NOUTIME*/

#ifdef USE_GETWD
char *getcwd P((char *, int)) ;
#endif /*USE_GETWD*/

#ifdef AUDIO_SUPPORT
void check_audio P((char *)) ;
void play_sound P((char *)) ;
#endif /*AUDIO_SUPPORT*/

void a_to_u P((char *, char *, char *, char *)) ;
void add_alias P((struct comminfo *, char *, char *, char *)) ;
void add_face P((enum disp_type, enum icon_type, char *)) ;
void add_machine P((char *, char *)) ;
void add_ps_rec P((int, int, char *)) ;
void add_record P((char *, char *, char *, int)) ;
void adjust P(()) ;
void adjust_image P((enum disp_type, enum icon_type, int, int)) ;
void beep_flash P((int, int)) ;
void check_comm P((char *, char *, char *, char *)) ;
void cmdbool P((int, char *, char *)) ;
void cmdint P((int, int, char *, char *)) ;
void cmdstr P((char *, char *, char *)) ;
void create_pixrects P((int, int)) ;
void destroy_image P(()) ;
void do_check P(()) ;
void do_face_update P((char *, char [2048])) ;
void do_key P((int)) ;
void do_mail P((enum mon_type)) ;
void do_printer P(()) ;
void do_prog P(()) ;
void do_prop_check P(()) ;
void do_users P(()) ;
void draw_screen P(()) ;
void drop_back P(()) ;
void garbage_collect P(()) ;
void get_display P((int, char *[])) ;
void get_options P((int, char *[])) ;
void get_userrec P((char *, enum icon_type)) ;
void get_xface P((char *, unsigned short [])) ;
void getparam P((char **, char *[], char *)) ;
void h_to_c P((char *, char *)) ;
void initialise P(()) ;
void init_font P(()) ;
void init_graphics P((int *, char *[])) ;
void load_data P((enum icon_type, unsigned char *)) ;
void load_icon P((enum icon_type, unsigned short [256], int)) ;
void load_resources P(()) ;
void make_area P((enum disp_type, int, int)) ;
void make_display P(()) ;
void make_frame P((int, char *[])) ;
void make_icon P(()) ;
void make_pixrect P((int)) ;
void process_face P(()) ;
void process_from P(()) ;
void process_info P(()) ;
void read_aliases P(()) ;
void read_machines P(()) ;
void read_mailrc P(()) ;
void read_resources P(()) ;
void read_str P((char **, char *)) ;
void remove_record P((struct recinfo *)) ;
void repl_image P((enum disp_type, enum image_type, int, int)) ;
void show_display P((int)) ;
void start_tool P(()) ;
void text P((enum disp_type, enum just_type, char *)) ;
void usage P(()) ;
void write_cmdline P(()) ;
