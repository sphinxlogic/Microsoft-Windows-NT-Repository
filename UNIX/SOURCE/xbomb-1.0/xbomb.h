
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string.h>

#include <errno.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <sys/time.h>

#ifndef _PATH_SCORE
error ("_PATH_SCORE must be defined in the Makefile")
#endif

#ifndef _PATH_SOLVERS
error ("_PATH_SOLVERS must be defined in the Makefile")
#endif

#ifndef _PATH_BITMAPS
error ("_PATH_BITMAPS must be defined in the Makefile")
#endif

#define NUM_SCORES 100
#define MAX_PER_PLAYER 5
#define TOP_PRINT 5

/* typedef */ struct score {
        char    name[10];
        int     size;
        int     bombs;
        int     defused;
	int	won;
	int	score;
	time_t	time;
	int	mine;
};

	
	
#define  MAXWIDTH	100
#define  MAXHEIGHT	100
#define  DEFWIDTH	20
#define  DEFHEIGHT	20
#define  WIDTH		32
#define  HEIGHT		32

#define XTOCOLUMN(X)	((X)/(WIDTH))
#define YTOROW(Y)	((Y)/(HEIGHT) - 1)
	
#define  EMPTY		10
#define  FLAG		11
#define  BOMB		12
#define  KEMPTY		13
#define  KBOMB		14
#define  EBOMB		15
#define  BORDER		16
#define  CHECK 20
#define  BLANK          17
#define  THINK          18

void move_somewhere();
void place_flag();
void bail();


extern int dead,
	numflags,
	emptylen,
	numempty,
	button,
	shifted,
	numbombs,
	width,
	height,
	ted,
	onlyprint,
	randomE,
	randomB,
	seed,
	showscore,
	betterfollow,
	startwithzero,
	playagain;

extern char *name;
extern char *bitdir;
	
extern char *geom;
extern int borderWidth;

#define MAX_SOLVERS 10
extern char *solvers[MAX_SOLVERS];
extern FILE *solver_rfps[MAX_SOLVERS], *solver_wfps[MAX_SOLVERS];
extern int solver_pids[MAX_SOLVERS];


struct cell {
	unsigned status;
	unsigned neighbors;
	unsigned flag;
	int neighbor_kbombs;
	int neighbor_unknowns;  /* not maintained */
};
	
#define is_bomb(s)  ((s == BOMB)?1:0)
#define is_bomb_or_flag(s) ((s==BOMB || s==FLAG)?1:0)
#define is_unknown(s) ((s==BOMB || s==EMPTY)?1:0)

extern struct cell map[MAXWIDTH+2][MAXHEIGHT+2];
	
extern Display     *dpy;
extern int         screen;
extern GC          gc;
	
extern Window      rootWindow, puzzleWindow;
extern long        FgPixel, BgPixel, BdPixel;
	
extern Pixmap pixmaps[20];
	
extern int apply_inc_if_bomb(),
	apply_inc_if_unknown(),
	apply_flag_if_unknown(),
	apply_add_neighbor_kbombs();
