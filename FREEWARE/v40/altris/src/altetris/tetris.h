/*
# GENERIC X-BASED TETRIS (altered)
#
#	tetris.h
#
###
#
#  Copyright (c) 1993 - 95	David A. Bagley, bagleyd@perry.njit.edu
#
#  Taken from GENERIC X-BASED TETRIS
#
#  Copyright (c) 1992 - 95	 	Q. Alex Zhao, azhao@cc.gatech.edu
#
#			All Rights Reserved
#
#  Permission to use, copy, modify, and distribute this software and
#  its documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and
#  that both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.
#
#  This program is distributed in the hope that it will be "playable",
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
*/

/*** Common headers ***/

#include	<stdio.h>
#include	<math.h>
#include	<pwd.h>
#include	<errno.h>

#ifdef VMS
#include        "unix_time.h"
#include        "unix_types.h"
#endif

#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/Xresource.h>
#include	<X11/cursorfont.h>
#include	<X11/keysym.h>
#include	<X11/Xos.h>
#include	<X11/Xfuncs.h>
#include	<X11/Xfuncproto.h>

#ifndef	X_NOT_STDC_ENV
#include	<stdlib.h>
#else
extern char    *getenv(
#if	NeedFunctionPrototypes
    char *
#endif
);
#endif

#ifdef	X_NOT_POSIX
extern int      getuid(
#if	NeedFunctionPrototypes
    void
#endif
);
extern struct passwd *getpwuid(
#if	NeedFunctionPrototypes
    int
#endif
);
#endif

#ifdef	_AIX
#include	<sys/select.h>
#endif

/*** for lseek ***/

#ifndef	SEEK_SET
#define	SEEK_SET	L_SET
#endif

#ifndef	SEEK_CUR
#define	SEEK_CUR	L_INCR
#endif

#ifndef	SEEK_END
#define	SEEK_END	L_XTND
#endif

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()

#define NRAND(X) ((int)(LRAND()%(X)))

/*** macros ***/

#define	ZLIM(X, Y)	(((int) X) < (Y) ? ((int) X) : (Y))

/*** constants ***/

#ifndef	SCOREFILE
#ifndef VMS
#define	SCOREFILE	"/usr/games/lib/tetris.scores"
#else
#define	SCOREFILE	"SYS$LOGIN:ALTETRIS.SCORE"
#endif /* VMS */
#endif
#ifndef	POLYOMINOFILE
#ifndef VMS
#define	POLYOMINOFILE	"/usr/games/lib/polyomino.dat"
#else
#define	POLYOMINOFILE	"ALTETRIS_DIR:polyomino.dat"
#endif
#endif
#ifndef VMS
#define	BIGFONT		"12x24"
#define	TINYFONT	"6x12"
#else
#define BIGFONT        "-Sony-Fixed-Medium-R-Normal--24-*-*-*-C-120-ISO8859-1"
#define TINYFONT       "-Misc-Fixed-Medium-R-SemiCondensed--12-*-*-*-C-60-ISO8859-1"
#endif
#define	BVOLUME		-90
#define NUM_FLASHES	16

#define SAMPLERATE  5000000

#define	MILLION		1000000
#define	MAXSCORES	3
#define	SHOWSCORES	15
#define	NAMELEN		12
#define	FILENAMELEN	1024

#define	MSG_PAUSED	"PAUSED"
#define	MSG_END		"GAME OVER"
#define	MSG_TITLE	"ALTETRIS"
#define	MSG_AUTHOR	"by David Bagley"
#define	NUM_LEVELS	19
#define	ROWS		20
#define	COLS		10
#define MAX_SIDES	4

#define	BOXSIZE		30
#define	OFFSET		20

/*
#define	THINGSIZE	4
#define	NUM_THINGS	7
*/
#define NUM_BITMAPS	16
#define NODIAG		0 /* Normal, ie no diagonals */
#define DIAGONAL	1 /* Normal and diagonal */
#define NOMIX		0 /* Do not mix lowered number squares with current */
#define MIXED		1
#define MAX_MODES	2
#define NONE		0 /* No rotation or reflection */
#define NOREFL		1 /* Normal, no reflection allow rotation */
#define ALL		2 /* Allow reflection or rotation */
#define MAX_TYPES       3
#define DEFAULT_SQUARES	4
#define MIN_SQUARES	2 /* Why not 1 you say? */
#define MAX_SQUARES	6
#define DEFAULT_DIAGONAL	3
#define MIN_DIAGONAL	2
#define MAX_DIAGONAL	3
#define MAX_POLYOMINOES	216
#define MAX_START_POLYOMINOES	55
#define SQUARES_PER_ROW	4 /* Also less than ROWS - 1 */
#define JUMPIN		0
#define GRADUAL		1
#define LEGAL(n,g,m,o)	((n>=MIN_SQUARES+m)&&\
			(n<=((g)?MAX_DIAGONAL:MAX_SQUARES)-o))
#define THRESHOLD(x)	((x+1)*10)
#define CHECKUP(x)	((x)%8)
#define CHECKDOWN(x)	(((x)/4)*4+(x)%2)

/*** types ***/

typedef enum {FALL, DROP, LEFT, RIGHT, ROTATE, REFLECT} move_t;

typedef struct {
    char            myname[NAMELEN], myhost[NAMELEN], mydate[27];
    char            score[10];
    char            level[4];
    char            rows[5];
}               score_t;
#define	SCORESIZE	sizeof(score_t)

/*** variables defined in "tetris.c" ***/

extern Display *display;
extern int      screen_num;
extern Bool     useColor;
extern Bool	use3D, cw, randomFill, gradualAppear;
extern Colormap colormap;
extern Window   mainWin, blockWin;
extern Cursor   theCursor;
extern XFontStruct *bigFont, *tinyFont;
extern unsigned long fg, bg;

extern XSizeHints sizehints;
extern XWMHints wmhints;

extern char     scorefile[FILENAMELEN];
extern int      level, prefilled, score, rows, squares;
extern Bool     showNext, beep, diagonal, mixed, bonus, bonusNow;
extern score_t  myscore;

/*** variables defined in "utils.c" ***/

extern Atom     delw;

/*** variables defined in "playing.c" ***/

/*** functions ***/

extern void     showScores();
extern void     inits();
extern void     playing();
extern void     realTime();
extern void     newThing();
extern void     drawTitle();
extern void     drawStatus();
extern void     drawField();
extern void     drawThing();
extern void     drawNext();
extern void     banner();
extern void     clearNext();
extern void     putBox();
extern void     tryMove();
extern Bool     atBottom();
extern Bool     overlapping();
extern int      checkLines();
extern void     fillLines();
extern void     redoNext();
