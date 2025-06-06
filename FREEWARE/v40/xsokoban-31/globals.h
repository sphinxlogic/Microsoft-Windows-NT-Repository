#include "config.h"
/*****************************************************************************\
 *  Stuff in this file shouldn't ever need to be changed.                    *
\*****************************************************************************/

#ifdef VICE
extern int PlayerUID;
#define getuid() PlayerUID
#define geteuid() PlayerUID
#endif

#define BUFSIZE 256

/* internal object representation */
#define   player	'@'
#define   playerstore	'+'
#define   store		'.'
#define   packet	'$'
#define   save		'*'
#define   ground	' '
#define   wall		'#'

/* maximum possible size of a board */
#define MAXROW		20
#define MAXCOL		20

/* player position for movement */
typedef struct {
   short x, y;
} POS;

/* list of possible errors */
#define E_FOPENSCREEN	1
#define E_PLAYPOS1	2
#define E_ILLCHAR	3
#define E_PLAYPOS2	4
#define E_TOMUCHROWS	5
#define E_TOMUCHCOLS	6
#define E_ENDGAME	7
#define E_NOUSER	8
#define E_FOPENSAVE	9
#define E_WRITESAVE	10
#define E_STATSAVE	11
#define E_READSAVE	12
#define E_ALTERSAVE	13
#define E_SAVED		14
#define E_TOMUCHSE	15
#define E_FOPENSCORE	16
#define E_READSCORE	17
#define E_WRITESCORE	18
#define E_USAGE		19
#define E_ILLPASSWORD	20
#define E_LEVELTOOHIGH	21
#define E_NOSUPER	22
#define E_NOSAVEFILE	23
#define E_NOBITMAP	24
#define E_NODISPLAY	25
#define E_NOFONT	26
#define E_NOMEM		27
#define E_NOCOLOR	28

/* classname for silly X stuff */
#define CLASSNAME "XSokoban"

/* macros translating game coords to window coords */
#define cX(x) (bit_width * (((MAXCOL - cols) / 2) + (x)))
#define cY(x) (bit_height * (((MAXROW - rows) / 2) + (x)))

/* macros translating window coords to game coords */
#define wX(x) (((x)/bit_width) - ((MAXCOL - cols)/2))
#define wY(x) (((x)/bit_height) - ((MAXROW - rows)/2))


/*** Global state ***/
typedef char Map[MAXROW+1][MAXCOL+1];

extern Map map;

extern short rows, cols, level, moves, pushes, savepack, packets;
extern unsigned short scorelevel, scoremoves, scorepushes;
extern POS ppos;
extern Display *dpy;
extern unsigned bit_width, bit_height; /* for macros wX, wY */
extern Atom wm_delete_window, wm_protocols;
extern XrmDatabase rdb;
extern Colormap cmap;
extern char *progname, *bitpath, *username;
extern Boolean display_alloc;

extern short scoreentries;
extern struct st_entry {
  char user[MAXUSERNAME];
  unsigned short lv, pad1, mv, pad2, ps, pad3;
} scoretable[MAXSCOREENTRIES];
