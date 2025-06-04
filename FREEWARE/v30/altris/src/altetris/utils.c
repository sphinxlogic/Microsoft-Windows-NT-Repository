/*
# GENERIC X-BASED TETRIS (altered)
#
#	utils.c
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

#include	"tetris.h"

#include	"ticon.xbm"

#include        "bm-3d/rot00.xbm"
#include        "bm-3d/rot01.xbm"
#include        "bm-3d/rot02.xbm"
#include        "bm-3d/rot03.xbm"
#include        "bm-3d/rot04.xbm"
#include        "bm-3d/rot05.xbm"
#include        "bm-3d/rot06.xbm"
#include        "bm-3d/rot07.xbm"
#include        "bm-3d/rot08.xbm"
#include        "bm-3d/rot09.xbm"
#include        "bm-3d/rot10.xbm"
#include        "bm-3d/rot11.xbm"
#include        "bm-3d/rot12.xbm"
#include        "bm-3d/rot13.xbm"
#include        "bm-3d/rot14.xbm"
#include        "bm-3d/rot15.xbm"

#include        "bm-plain/rop00.xbm"
#include        "bm-plain/rop01.xbm"
#include        "bm-plain/rop02.xbm"
#include        "bm-plain/rop03.xbm"
#include        "bm-plain/rop04.xbm"
#include        "bm-plain/rop05.xbm"
#include        "bm-plain/rop06.xbm"
#include        "bm-plain/rop07.xbm"
#include        "bm-plain/rop08.xbm"
#include        "bm-plain/rop09.xbm"
#include        "bm-plain/rop10.xbm"
#include        "bm-plain/rop11.xbm"
#include        "bm-plain/rop12.xbm"
#include        "bm-plain/rop13.xbm"
#include        "bm-plain/rop14.xbm"
#include        "bm-plain/rop15.xbm"

Atom            delw;

static GC       revGC, bigGC, tinyGC, xorGC;
static GC       thingGCs[MAX_START_POLYOMINOES];

static char    *winName = "GENERIC ALTETRIS";
static char    *iconName = "ALTETRIS";

static int      titleLen, titleWidth, authorLen, authorWidth;
static int      titleX, titleY, authorX, authorY;
static int      sX;
static int      sLevelY, sRowsY, sScoreY;

static int      topRWidth, topWidth, topHeight, topMidX, topMidY;
static int      frameX, frameY, frameW, frameH;

static unsigned char    *bitmap_data_3d[NUM_BITMAPS] = {
    rot00_bits, rot01_bits, rot02_bits, rot03_bits, rot04_bits, rot05_bits,
    rot06_bits, rot07_bits, rot08_bits, rot09_bits, rot10_bits, rot11_bits,
    rot12_bits, rot13_bits, rot14_bits, rot15_bits
};

static unsigned char    *bitmap_data_plain[NUM_BITMAPS] = {
    rop00_bits, rop01_bits, rop02_bits, rop03_bits, rop04_bits, rop05_bits,
    rop06_bits, rop07_bits, rop08_bits, rop09_bits, rop10_bits, rop11_bits,
    rop12_bits, rop13_bits, rop14_bits, rop15_bits
};

typedef struct {
    int             squares, polyomino_number;
    int             xpos, ypos;
    int             size, color_number;
    int             bonus, random_rotation, random_reflection;
    long            random_number;
}               thing_t;
typedef struct {
    int             rotation;
    int             reflection;
    int             start_height;
    int             shape[MAX_SQUARES][MAX_SQUARES];
    int             size;
}               Polyominoes;
typedef struct {
    int             number[MAX_MODES][MAX_TYPES];
    int             start[MAX_START_POLYOMINOES][MAX_TYPES];
    int             turn_style;
}               Mode;
typedef struct {
    Polyominoes     polyomino[MAX_POLYOMINOES];
    Mode            mode[MAX_MODES];
    int             diagonal_switch;
}               Polytris;
static Polytris tris[MAX_SQUARES - MIN_SQUARES + 1];

static char    *thingFGs[MAX_START_POLYOMINOES] = {
    "Red", "Green", "Blue", "Yellow", 
    "Magenta", "Cyan", "FireBrick", "GreenYellow",
    "SlateBlue", "Khaki", "Plum", "Violet",
    "DarkTurquoise", "Gold", "Orchid", "Turquoise",
    "Orange", "OrangeRed", "VioletRed", "BlueViolet",
    "SeaGreen", "Pink", "ForestGreen", "SkyBlue",
    "Coral", "Wheat", "GoldenRod", "IndianRed",
    "SpringGreen", "CornflowerBlue", "Thistle", "Aquamarine",
    "CadetBlue", "LightSteelBlue", "NavyBlue", "SteelBlue",
    "YellowGreen", "DarkViolet", "MediumSeaGreen", "DarkSlateGray",
    "LightGray", "MediumVioletRed", "Sienna", "MediumAquamarine",
    "MediumBlue", "Navy", "DarkOliveGreen", "DarkGreen",
    "DimGray", "Tan", "MediumTurquoise", "DarkSlateBlue",
    "Maroon", "Gray", "Black"
};

static Pixmap   pms[NUM_BITMAPS];

static thing_t  curThing, nextThing;
static struct {
    int         pmid, cid;
}               field[ROWS][COLS];

static void     readPolyominoes();

/* ------------------------------------------------------------------ */

static unsigned long
getColor(name)
    char            name[];
{
    XColor          tmp;

    if (!useColor)
        return fg;
    if (XParseColor(display, colormap, name, &tmp) == 0) {
	(void) fprintf(stderr, "Tetris: invalid color '%s'.\n", name);
	return fg;
    }
    if (XAllocColor(display, colormap, &tmp) == 0) {
	(void) fprintf(stderr, "Tetris: can't allocate color '%s'.\n", name);
	return fg;
    }
    return tmp.pixel;
}

/* ------------------------------------------------------------------ */

void
inits(argc, argv)
    int             argc;
    char           *argv[];
{
    XSetWindowAttributes att;
    unsigned int    attvm;
    XTextProperty   wName, iName;
    XClassHint      classhints;
    XEvent          ev;
    XGCValues       gcv;
    unsigned long   gcvm;
    int             i, j, col, turn_style, number_polyominoes; 

    (void) SRAND(time(NULL));
    readPolyominoes();

    turn_style = tris[squares - MIN_SQUARES].mode[diagonal].turn_style;
    if (turn_style < 0 || turn_style >= MAX_TYPES)
    {
      (void) fprintf(stderr, "Tetris: corrupted input file %s\n",
               POLYOMINOFILE);
      (void) fprintf(stderr, "\tturn_style = %d.\n", turn_style);
      exit(1);
    }
    number_polyominoes = tris[squares - MIN_SQUARES].mode
      [diagonal].number[mixed][turn_style];
    if (number_polyominoes <= 0 || number_polyominoes > MAX_START_POLYOMINOES)
    {
      (void) fprintf(stderr, "Tetris: corrupted input file %s\n",
               POLYOMINOFILE);
      (void) fprintf(stderr, "\tnumber_polyominoes = %d.\n",
               number_polyominoes);
      exit(1);
    }
    /* prefilled */
    if (randomFill)
      /* First Algorithm randomly fills boxes to a certain level 
         The problem with this is that it could potentially fill up
         a whole row */
      for (i = 0; i < COLS; i++)
        for (j = 0; j < ROWS; j++)
            if ((j >= ROWS - prefilled) && (NRAND(2) == 0)) {
                field[j][i].pmid = 0;
                field[j][i].cid = NRAND(number_polyominoes);
            } else {
                field[j][i].pmid = -1;
                field[j][i].cid = 0;
            }
    else
      /* Second Algorithm randomly picks SQUARES_PER_ROW boxes per row 
         This is more in keeping with the original */
      for (j = 0; j < ROWS; j++) {
         for (i = 0; i < COLS; i++) {
                field[j][i].pmid = -1;
                field[j][i].cid = 0;
	 }
	 if (j >= ROWS - prefilled)
           for (i = 0; i < SQUARES_PER_ROW; i++) {
                do
                    col = NRAND(COLS);
                while (field[j][col].pmid != -1);
                field[j][col].pmid = 0;
                field[j][col].cid = NRAND(number_polyominoes);
	   }
      }

    titleLen = strlen(MSG_TITLE);
    titleWidth = XTextWidth(bigFont, MSG_TITLE, titleLen);
    authorLen = strlen(MSG_AUTHOR);
    authorWidth = XTextWidth(tinyFont, MSG_AUTHOR, authorLen);

    frameW = BOXSIZE * COLS;
    frameH = BOXSIZE * ROWS;
    topRWidth = BOXSIZE * MAX_SQUARES + OFFSET * 2;
    topHeight = frameH + OFFSET * 2 + 4;
    if (titleWidth > topRWidth)
	topRWidth = titleWidth;
    if (authorWidth > topRWidth)
	topRWidth = authorWidth;
    topMidX = frameW + OFFSET * 2 + 4;
    topMidY = topHeight / 2 + bigFont->ascent;
    topWidth = topMidX + topRWidth;
    frameX = frameY = OFFSET + 2;

    titleX = (topRWidth - titleWidth) / 2 + topMidX;
    titleY = OFFSET + 2 + bigFont->ascent;
    authorX = (topRWidth - authorWidth) / 2 + topMidX;
    authorY = OFFSET + 2 + bigFont->ascent + bigFont->descent +
         tinyFont->ascent;

    sX = topMidX + OFFSET;
    sScoreY = topHeight - OFFSET - 2 - tinyFont->descent;
    sRowsY = sScoreY - tinyFont->descent - tinyFont->ascent - 2;
    sLevelY = sRowsY - tinyFont->descent - tinyFont->ascent - 2;

    sizehints.width = (sizehints.min_width =
	(sizehints.max_width = topWidth));
    sizehints.height = (sizehints.min_height =
	(sizehints.max_height = topHeight));

    theCursor = XCreateFontCursor(display, XC_exchange);

    /* arrow keys or number pad */
    XRebindKeysym(display, XK_R10, NULL, 0,
        (unsigned char *) "j", sizeof(unsigned char));
    XRebindKeysym(display, XK_KP_4, NULL, 0,
	(unsigned char *) "j", sizeof(unsigned char));
    XRebindKeysym(display, XK_Left, NULL, 0,
	(unsigned char *) "j", sizeof(unsigned char));
    XRebindKeysym(display, XK_R8, NULL, 0,
        (unsigned char *) "i", sizeof(unsigned char));
    XRebindKeysym(display, XK_KP_8, NULL, 0,
	(unsigned char *) "i", sizeof(unsigned char));
#if	defined(UP_REFLECT)
      /* UP key to reflect */
      XRebindKeysym(display, XK_Up, NULL, 0,
  	(unsigned char *) "i", sizeof(unsigned char));
#else
      /* UP key to rotate */
      XRebindKeysym(display, XK_Up, NULL, 0,
	(unsigned char *) "k", sizeof(unsigned char));
#endif
    XRebindKeysym(display, XK_R11, NULL, 0,
        (unsigned char *) "k", sizeof(unsigned char));
    XRebindKeysym(display, XK_KP_5, NULL, 0,
	(unsigned char *) "k", sizeof(unsigned char));
    XRebindKeysym(display, XK_Begin, NULL, 0,
        (unsigned char *) "k", sizeof(unsigned char));
    XRebindKeysym(display, XK_R12, NULL, 0,
        (unsigned char *) "l", sizeof(unsigned char));
    XRebindKeysym(display, XK_KP_6, NULL, 0,
	(unsigned char *) "l", sizeof(unsigned char));
    XRebindKeysym(display, XK_Right, NULL, 0,
	(unsigned char *) "l", sizeof(unsigned char));
    XRebindKeysym(display, XK_R14, NULL, 0,
        (unsigned char *) " ", sizeof(unsigned char));
    XRebindKeysym(display, XK_KP_2, NULL, 0,
	(unsigned char *) " ", sizeof(unsigned char));
#if	defined(DOWN_ROTATE)
      /* DOWN key to rotate */
      XRebindKeysym(display, XK_Down, NULL, 0,
  	(unsigned char *) "k", sizeof(unsigned char));
#else
      /* DOWN key to drop */
      XRebindKeysym(display, XK_Down, NULL, 0,
	(unsigned char *) " ", sizeof(unsigned char));
#endif

    /* create windows */
    attvm = CWBackPixel | CWEventMask | CWDontPropagate | CWCursor;
    att.background_pixel = bg;
    att.event_mask = ExposureMask | KeyPressMask |
	StructureNotifyMask | FocusChangeMask;
    att.do_not_propagate_mask = KeyReleaseMask |
	ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
	ButtonMotionMask | Button1MotionMask | Button2MotionMask |
	Button3MotionMask | Button4MotionMask | Button5MotionMask;
    att.cursor = theCursor;

    mainWin = XCreateWindow(display, DefaultRootWindow(display),
	sizehints.x, sizehints.y, topWidth, topHeight, 0,
	CopyFromParent, InputOutput, CopyFromParent, attvm, &att);

    attvm = CWBackPixel | CWBorderPixel | CWEventMask;
    att.border_pixel = fg;
    att.event_mask = ExposureMask;

    blockWin = XCreateWindow(display, mainWin,
	frameX-2, frameY-2, frameW, frameH, 2,
	CopyFromParent, InputOutput, CopyFromParent, attvm, &att);

    /* WM hints */
    XStringListToTextProperty(&winName, 1, &wName);
    XStringListToTextProperty(&iconName, 1, &iName);

    wmhints.icon_pixmap = XCreateBitmapFromData(display,
	mainWin, (char *) ticon_bits, ticon_width, ticon_height);
    classhints.res_name = "tetris";
    classhints.res_class = "Tetris";

    XSetWMProperties(display, mainWin, &wName, &iName,
	argv, argc, &sizehints, &wmhints, &classhints);

    delw = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, mainWin, &delw, 1);

    /* GC's */

    gcvm = GCForeground | GCBackground | GCFunction |
	GCFont | GCGraphicsExposures;

    gcv.function = GXcopy;
    gcv.foreground = fg;
    gcv.background = bg;
    gcv.font = bigFont->fid;
    gcv.graphics_exposures = False;
    bigGC = XCreateGC(display, mainWin, gcvm, &gcv);

    gcv.font = tinyFont->fid;
    tinyGC = XCreateGC(display, mainWin, gcvm, &gcv);

    gcv.foreground = bg;
    gcv.background = fg;
    revGC = XCreateGC(display, mainWin, gcvm, &gcv);

    gcv.background = bg;
    for (i = 0; i < MAX_START_POLYOMINOES; i++) {
	gcv.foreground = getColor(thingFGs[i]);
	if (gcv.foreground == bg)
	    gcv.foreground = fg;
	thingGCs[i] = XCreateGC(display, blockWin, gcvm, &gcv);
    }

    gcv.foreground = fg;
    gcv.function = GXxor;
    xorGC = XCreateGC(display, blockWin, gcvm, &gcv);

    /* pixmaps */

    if (use3D)
        for (i = 0; i < NUM_BITMAPS; i++) {
            pms[i] = (Pixmap) XCreateBitmapFromData(display, blockWin,
                (char *) bitmap_data_3d[i], BOXSIZE, BOXSIZE);
        }
    else for (i = 0; i < NUM_BITMAPS; i++) {
            pms[i] = (Pixmap) XCreateBitmapFromData(display, blockWin,
                (char *) bitmap_data_plain[i], BOXSIZE, BOXSIZE);
        }

    /* new things */
    newThing();
    newThing();

    /* the last thing is to wait for mapped */
    XMapWindow(display, blockWin);
    XMapRaised(display, mainWin);
    XNextEvent(display, &ev);
}

/* ------------------------------------------------------------------ */

void
newThing()
{
    curThing = nextThing;
    nextThing.random_number = LRAND();
    nextThing.random_rotation = NRAND(MAX_SIDES);
    nextThing.random_reflection = NRAND(2);
    nextThing.bonus = bonusNow;
    bonusNow = False;
    redoNext();
}

/* ------------------------------------------------------------------ */

void
drawTitle()
{
    XDrawString(display, mainWin, bigGC,
	titleX, titleY, MSG_TITLE, titleLen);
    XDrawString(display, mainWin, tinyGC,
	authorX, authorY, MSG_AUTHOR, authorLen);
}

/* ------------------------------------------------------------------ */

void
drawStatus()
{
    char            buf[30];

    (void) sprintf(buf, "Score: %d", score);
    XDrawImageString(display, mainWin, tinyGC, sX, sScoreY, buf, strlen(buf));

    (void) sprintf(buf, "Level: %d    ", level);
    XDrawImageString(display, mainWin, tinyGC, sX, sLevelY, buf, strlen(buf));

    (void) sprintf(buf, "Rows: %d", rows);
    XDrawImageString(display, mainWin, tinyGC, sX, sRowsY, buf, strlen(buf));
}

/* ------------------------------------------------------------------ */

static void
drawSquare(pmid, cid, i, j)
    int             pmid, cid, i, j;
{
    XCopyPlane(display, pms[pmid % NUM_BITMAPS], blockWin, thingGCs[cid], 0, 0,
	BOXSIZE, BOXSIZE, i * BOXSIZE, j * BOXSIZE, (unsigned long) 1);
}

/* ------------------------------------------------------------------ */

static void
clearSquare(i, j)
    int             i, j;
{
    XFillRectangle(display, blockWin, revGC,
      i * BOXSIZE, j * BOXSIZE, BOXSIZE, BOXSIZE);
}

/* ------------------------------------------------------------------ */

void
drawField()
{
    int             i, j;

    for (i = 0; i < COLS; i++)
	for (j = 0; j < ROWS; j++)
	    if (field[j][i].pmid >= 0)
		drawSquare(field[j][i].pmid, field[j][i].cid, i, j);
}

/* ------------------------------------------------------------------ */

void
drawThing()
{
    int             i, j;
    
    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
        if (tris[curThing.squares].polyomino
                [curThing.polyomino_number].shape[j][i])
	  drawSquare(tris[curThing.squares].polyomino
                         [curThing.polyomino_number].shape[j][i],
            curThing.color_number, curThing.xpos + i, curThing.ypos + j);
}

/* ------------------------------------------------------------------ */

static void
drawThingDiff(old)
    thing_t        *old;
{
    int             i, j, ox, oy;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
        if ((curThing.ypos + j >= 0) && tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i])
	  drawSquare(tris[curThing.squares].polyomino
                         [curThing.polyomino_number].shape[j][i],
            curThing.color_number, curThing.xpos + i, curThing.ypos + j);

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++) {
        ox = old->xpos + i - curThing.xpos;
        oy = old->ypos + j - curThing.ypos;
	if (tris[old->squares].polyomino[old->polyomino_number].shape[j][i] &&
	    ((ox < 0) || (ox >= curThing.size) ||
	     (oy < 0) || (oy >= curThing.size) ||
	     !tris[curThing.squares].polyomino
                  [curThing.polyomino_number].shape[oy][ox]))
              clearSquare((old->xpos + i), (old->ypos + j));
	}
}

/* ------------------------------------------------------------------ */

void
drawNext()
{
    int             x, y;
    int             i, j, pmid;

    x = topMidX + (topRWidth - nextThing.size * BOXSIZE) / 2;
    y = topMidY - nextThing.size * BOXSIZE / 2;
    for (i = 0; i < nextThing.size; i++)
      for (j = 0; j < nextThing.size; j++) {
        pmid = tris[nextThing.squares].polyomino
                   [nextThing.polyomino_number].shape[j][i];
	if (pmid)
          XCopyPlane(display, pms[pmid % NUM_BITMAPS], mainWin,
	    thingGCs[nextThing.color_number], 0, 0,
 	    BOXSIZE, BOXSIZE, x + i * BOXSIZE, y + j * BOXSIZE,
            (unsigned long) 1);
      }
}

/* ------------------------------------------------------------------ */

void
clearNext()
{
    XFillRectangle(display, mainWin, revGC,
	topMidX, topMidY - BOXSIZE * MAX_SQUARES / 2,
	topRWidth, BOXSIZE * MAX_SQUARES);
}

/* ------------------------------------------------------------------ */

void
redoNext()
{
    int turn_style = tris[squares - MIN_SQUARES].mode[diagonal].turn_style;
    int next_start, i;

    nextThing.squares = squares - MIN_SQUARES + nextThing.bonus;
    next_start = nextThing.random_number % tris[nextThing.squares].
      mode[diagonal].number[(nextThing.bonus) ? NOMIX : mixed][turn_style];
    nextThing.color_number = next_start;

    if (mixed && !nextThing.bonus)
    {
      nextThing.squares = 0;
      while (next_start >=
             tris[nextThing.squares].mode[diagonal].number[NOMIX][turn_style])
      {
        next_start -=
          tris[nextThing.squares].mode[diagonal].number[NOMIX][turn_style];
        nextThing.squares++;
      }
    }
    nextThing.polyomino_number =
      tris[nextThing.squares].mode[diagonal].start[next_start][turn_style];
    if (tris[nextThing.squares].mode[diagonal].turn_style > NONE)
      for (i = 0; i < nextThing.random_rotation; i++)
        nextThing.polyomino_number = tris[nextThing.squares].polyomino
          [nextThing.polyomino_number].rotation;
    if (tris[nextThing.squares].mode[diagonal].turn_style == ALL)
      for (i = 0; i < nextThing.random_reflection; i++)
        nextThing.polyomino_number = tris[nextThing.squares].polyomino
          [nextThing.polyomino_number].reflection;
    nextThing.size =
      tris[nextThing.squares].polyomino[nextThing.polyomino_number].size;
    nextThing.xpos = NRAND(COLS - nextThing.size + 1);
    /* nextThing.xpos = (COLS - nextThing.size) / 2; */
    nextThing.ypos = -tris[nextThing.squares].polyomino
      [nextThing.polyomino_number].start_height;
}

/* ------------------------------------------------------------------ */

void
banner(msg)
    char            msg[];
{
    int             mlen = strlen(msg);
    int             w = XTextWidth(bigFont, msg, mlen);
    int             x = (topRWidth - w)/2 + topMidX;

    XFillRectangle(display, mainWin, revGC,
	x - 60, topMidY - bigFont->ascent - 5,
	w + 120, bigFont->ascent + bigFont->descent + 10);
    XDrawString(display, mainWin, bigGC, x, topMidY, msg, mlen);
}

/* ------------------------------------------------------------------ */

void
putBox()
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
        if ((y + j >= 0) && tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i]) {
          field[y + j][x + i].pmid = tris[curThing.squares].polyomino
            [curThing.polyomino_number].shape[j][i] % NUM_BITMAPS;
	  field[y + j][x + i].cid = curThing.color_number;
	}
}

/* ------------------------------------------------------------------ */

Bool
overlapping()
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
	if (tris[curThing.squares].polyomino
                [curThing.polyomino_number].shape[j][i]) {
          if ((y + j >= ROWS) || (x + i < 0) || (x + i >= COLS))
            return True;
	  if (gradualAppear) {
/* This method one can turn polyomino to an area above of screen, also
   part of the polyomino may not be visible initially */          
            if ((y + j >= 0) && (field[y + j][x + i].pmid >= 0))
              return True;
          } else {
/* This method does not allow turning polyomino to an area above screen */
            if ((y + j < 0) || (field[y + j][x + i].pmid >= 0))
	      return True;
          }
        }

    return False;
}

/* ------------------------------------------------------------------ */

Bool
atBottom()
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
	if ((y + j >= -1) && tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i])
	  if ((y + j >= ROWS - 1) || (x + i < 0) || (x + i >= COLS) ||
	      (field[y + j + 1][x + i].pmid >= 0))
	    return True;

    return False;
}

/* ------------------------------------------------------------------ */

void
tryMove(move)
    move_t          move;
{
    thing_t         old;
    int             i;

    old = curThing;

    switch (move) {
    case FALL:
	curThing.ypos ++;
	break;

    case DROP:
	do {
	    curThing.ypos ++;
	    score += level + prefilled;
	} while (!overlapping());
	curThing.ypos --;
	break;

    case LEFT:
	curThing.xpos --;
	break;

    case RIGHT:
	curThing.xpos ++;
	break;

    case ROTATE:
        if (tris[squares - MIN_SQUARES].mode[diagonal].turn_style > NONE)
        {
          if (cw)
            for (i = 0; i < MAX_SIDES - 1; i++)
              curThing.polyomino_number = tris[curThing.squares].polyomino
                [curThing.polyomino_number].rotation;
          else /* ccw */
            curThing.polyomino_number = tris[curThing.squares].polyomino
              [curThing.polyomino_number].rotation;
	  curThing.xpos = old.xpos;
	  curThing.ypos = old.ypos;
	}
	break;

    case REFLECT: /* reflect on y axis */
        if (tris[squares - MIN_SQUARES].mode[diagonal].turn_style == ALL)
        {
          curThing.polyomino_number = tris[curThing.squares].polyomino
            [curThing.polyomino_number].reflection;
	  curThing.xpos = old.xpos;
	  curThing.ypos = old.ypos;
	}
	break;
    }

    if (!overlapping())
	drawThingDiff(&old);
    else
	curThing = old;
}

/* ------------------------------------------------------------------ */

void
fillLines()
{
    int             i, j;
    int turn_style = tris[squares - MIN_SQUARES].mode[diagonal].turn_style;
    
    XSync(display, True);
    for (j = 0; j <= ROWS / 2; j++) {
      for (i = 0; i < COLS; i++) {
	drawSquare(0, NRAND(tris[squares - MIN_SQUARES].mode
            [diagonal].number[mixed][turn_style]), i, j);
	drawSquare(0, NRAND(tris[squares - MIN_SQUARES].mode
            [diagonal].number[mixed][turn_style]), i, ROWS - j - 1);
        }
	XSync(display, True);
    }

}
							
/* ------------------------------------------------------------------ */

int
checkLines()
{
    int             lSet[ROWS], nset = 0;
    int             i, j, y;

    for (j = 0; j < ROWS; j++) {
	lSet[j] = 0;
	for (i = 0; i < COLS; i++)
	    if (field[j][i].pmid >= 0)
		lSet[j] ++;
	if (lSet[j] == COLS)
	    nset ++;
    }

    if (nset) {
	for (i = 0; i < ((NUM_FLASHES / nset) % 2) * 2; i++) {
	    for (j = 0; j < ROWS; j++) {
		if (lSet[j] == COLS)
		    XFillRectangle(display, blockWin, xorGC,
			0, j * BOXSIZE, frameW, BOXSIZE);
	    }
	    XFlush(display);
	}

	for (j = ROWS-1; j >= 0; j--) {
	    if (lSet[j] == COLS) {
		for (y = j; y > 0; y--)
		    for (i = 0; i < COLS; i++)
			field[y][i] = field[y-1][i];
		for (i = 0; i < COLS; i++)
		    field[0][i].pmid = -1;

		XCopyArea(display, blockWin, blockWin, tinyGC,
			0, 0, frameW, j * BOXSIZE, 0, BOXSIZE);
		
		XFillRectangle(display, blockWin, revGC,
			0, 0, frameW, BOXSIZE);

		for (i = j; i > 0; i--)
		    lSet[i] = lSet[i-1];
		lSet[0] = 0;

		if (j > 0)
		    for (i = 0; i < COLS; i++) {
			int             tmp = field[j][i].pmid;
			if ((tmp >= 0) && (tmp != CHECKDOWN(tmp))) {
			    field[j][i].pmid = CHECKDOWN(tmp);
			    drawSquare(field[j][i].pmid, field[j][i].cid,
                                       i, j);
			}
		    }

		j++;

		if (j < ROWS)
		    for (i = 0; i < COLS; i++) {
			int             tmp = field[j][i].pmid;
			if ((tmp >= 0) && (tmp != CHECKUP(tmp))) {
			    field[j][i].pmid = CHECKUP(tmp);
			    drawSquare(field[j][i].pmid, field[j][i].cid,
                                       i, j);
			}
		    }

		XFlush(display);
	    }
	}

	if (beep) XBell(display, BVOLUME);
	XSync(display, False);
    }

    return nset;
}

/* ------------------------------------------------------------------ */

void
realTime(tv)
    struct timeval *tv;
{
    while (tv->tv_usec < 0) {
	tv->tv_sec --;
	tv->tv_usec += MILLION;
    }
    while (tv->tv_usec >= MILLION) {
	tv->tv_sec ++;
	tv->tv_usec -= MILLION;
    }
}

/* ------------------------------------------------------------------ */

static void
readPolyominoes()
{
  int c, i, j, k, sq, polyomino, sum, start, n, size, height, diag,
    game, toss = 0;
  int counter[MAX_SQUARES - MIN_SQUARES + 1];
  int start_counter[MAX_SQUARES - MIN_SQUARES + 1][MAX_MODES][MAX_TYPES];
  FILE *fp;

  for (sq = 0; sq <= MAX_SQUARES - MIN_SQUARES; sq++)
  {
    counter[sq] = 0;
    for (polyomino = 0; polyomino < MAX_POLYOMINOES; polyomino++)
      for (j = 0; j < MAX_SQUARES; j++)
        for (i = 0; i < MAX_SQUARES; i++)
          tris[sq].polyomino[polyomino].shape[j][i] = 0;
    for (j = 0; j < MAX_TYPES; j++)
    {
      for (polyomino = 0; polyomino < MAX_START_POLYOMINOES; polyomino++)
      {
        tris[sq].mode[NODIAG].start[polyomino][j] = 0;
        tris[sq].mode[DIAGONAL].start[polyomino][j] = 0;
      }
      start_counter[sq][NODIAG][j] = 0;
      start_counter[sq][DIAGONAL][j] = 0;
      tris[sq].mode[NODIAG].number[NOMIX][j] = 0;
      tris[sq].mode[DIAGONAL].number[NOMIX][j] = 0;
      tris[sq].mode[NODIAG].number[MIXED][j] = 0;
      tris[sq].mode[DIAGONAL].number[MIXED][j] = 0;
    }
    tris[sq].mode[NODIAG].turn_style = NONE;
    tris[sq].mode[DIAGONAL].turn_style = NONE;
  }
  if ((fp = fopen(POLYOMINOFILE, "r")) == (FILE *)NULL)
  {
    (void) fprintf(stderr, "Can not open input file, %s.\n", POLYOMINOFILE);
    exit(1);
  }
  else
  {
    while ((c = getc(fp)) != EOF)
    {
      if (c == '/')
      {
        while ((c = getc(fp)) != '/') {}
        c = getc(fp);
      }
      if (c == '+')
      {
        (void) fscanf(fp, "%d", &k);
        sq = k - MIN_SQUARES;
        for (i = 0; i < MAX_TYPES; i++)
        {
          (void) fscanf(fp, "%d", &k);
          tris[sq].mode[NODIAG].number[NOMIX][i] = k;
        }
        (void) fscanf(fp, "%d", &k);
        tris[sq].mode[NODIAG].turn_style = k;
        (void) fscanf(fp, "%d", &k);
        tris[sq].diagonal_switch = k;
        if (tris[sq].diagonal_switch == True)
        {
          for (i = 0; i < MAX_TYPES; i++)
          {
            (void) fscanf(fp, "%d", &k);
            tris[sq].mode[DIAGONAL].number[NOMIX][i] = k;
          }
          (void) fscanf(fp, "%d", &k);
          tris[sq].mode[DIAGONAL].turn_style = k;
        }
        c = getc(fp);
      }
      if (c == '*')
      {
        (void) fscanf (fp, "%d", &size);
        c = getc(fp);
      }
      if (c == '~') /* Useful for debugging Things */
      {
        (void) fscanf (fp, "%d", &toss);
        c = getc(fp);
      }
      if (c == '#')
      {
        (void) fscanf(fp, "%d", &k);
        sq = k - MIN_SQUARES;
        (void) fscanf(fp, "%d", &n);
        if (tris[sq].diagonal_switch == True)
          (void) fscanf(fp, "%d", &n);
        for (polyomino = 0; polyomino <= toss; polyomino++)
          while ((c = getc(fp)) != EOF && c != '\n');
        for (polyomino = 0; polyomino < n - toss; polyomino++)
        {
          sum = polyomino + counter[sq];
            /* This is only there to "read" input file */
          (void) fscanf(fp, "%d", &k);
          (void) fscanf(fp, "%d", &k);
          tris[sq].polyomino[sum].rotation =
            k + counter[sq] - toss;
          (void) fscanf(fp, "%d", &k);
          tris[sq].polyomino[sum].reflection =
            k + counter[sq] - toss;
          for (game = JUMPIN; game <= GRADUAL; game++)
          {
            (void) fscanf(fp, "%d", &height);
            if (!gradualAppear && game == JUMPIN)
              tris[sq].polyomino[sum].start_height = height;
            else if (gradualAppear && game == GRADUAL)
              tris[sq].polyomino[sum].start_height = height;
            for (diag = NODIAG; diag <= tris[sq].diagonal_switch; diag++)
            {
              (void) fscanf(fp, "%d", &start);
              if (game == JUMPIN)
              {
                if (sq == 0 ||
                    tris[sq - 1].mode[diag].turn_style == NONE)
                {
                  i = start_counter[sq][diag][NONE];
                  tris[sq].mode[diag].start[i][NONE] = sum;
                  start_counter[sq][diag][NONE]++;
                }
                if ((sq == 0 ||
                     tris[sq - 1].mode[diag].turn_style < ALL) &&
                    start != NONE)
                {
                  i = start_counter[sq][diag][NOREFL];
                  tris[sq].mode[diag].start[i][NOREFL] = sum;
                  start_counter[sq][diag][NOREFL]++;
                }
                if (start == ALL)
                {
                  i= start_counter[sq][diag][ALL];
                  tris[sq].mode[diag].start[i][ALL] = sum;
                  start_counter[sq][diag][ALL]++;
                }
              }
            }
          }
	  tris[sq].polyomino[sum].size = size;
          for (j = 0; j < size; j++)
            for (i = 0; i < size; i++)
            {
              (void) fscanf(fp, "%d", &k);
              tris[sq].polyomino[sum].shape[j][i] = k;
            }
        }
        counter[sq] += n - toss;
        toss = 0;
      }
    }
    (void) fclose(fp);
    for (i = 0; i <= MAX_SQUARES - MIN_SQUARES; i++)
      for (j = 0; j < MAX_TYPES; j++)
        for (k = 0; k <= i; k++)
        {
          tris[i].mode[NODIAG].number[MIXED][j] +=
            tris[k].mode[NODIAG].number[NOMIX][j];
          if (tris[i].diagonal_switch == True)
             /*since k & i are not independent*/
            tris[i].mode[DIAGONAL].number[MIXED][j] +=
              tris[k].mode[DIAGONAL].number[NOMIX][j];
        }
  }
}
