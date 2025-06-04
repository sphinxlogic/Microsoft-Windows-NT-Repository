/*
# GENERIC X-BASED WELLTRIS
#
#	utils.c
#
###
#
#  Copyright (c) 1993 - 95 	David A. Bagley, bagleyd@perry.njit.edu
#
#  Taken from GENERIC X-WINDOW-BASED TETRIS
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

#include	"welltris.h"

#include	"wicon.xbm"

Atom            delw;

/*
static XPoint big_trapazoid[MAX_SIDES][MAX_SIDES] =
  {{4 * DELTA, 2 * DELTA},
    {ENDPT - 8 * DELTA, 0},
    {4 * DELTA - BASE_WIDTH, BASE_WIDTH - 4 * DELTA},
    {-BASE_WIDTH, 0},
   {ENDPT - 2 * DELTA, 3 * DELTA},
    {0, ENDPT - 7 * DELTA},
    {4 * DELTA - BASE_WIDTH, 4 * DELTA - BASE_WIDTH},
    {0, -BASE_WIDTH - DELTA},
   {ENDPT - 3 * DELTA, ENDPT - 2 * DELTA},
    {6 * DELTA - ENDPT, 0},
    {BASE_WIDTH - 4 * DELTA, 4 * DELTA - BASE_WIDTH},
    {BASE_WIDTH + 2 * DELTA, 0},
   {2 * DELTA, ENDPT - 4 * DELTA},
    {0, 7 * DELTA - ENDPT},
    {BASE_WIDTH - 4 * DELTA, BASE_WIDTH - 4 * DELTA},
    {0, BASE_WIDTH + DELTA}};
*/

static GC       revGC, bigGC, tinyGC, xorGC, freezeGC;
static GC       thingGCs[MAX_START_POLYOMINOES];

static char    *winName = "GENERIC ALWELLTRIS";
static char    *iconName = "ALWELLTRIS";

static int      titleLen, titleWidth, authorLen, authorWidth;
static int      titleX, titleY, authorX, authorY;
static int      sX;
static int      sLevelY, sRowsY, sScoreY;

static int      topRWidth, topWidth, topHeight, topMidX, topMidY;
static int      frameX, frameY, frameW, frameH;

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
static int frozen_wall[MAX_SIDES];

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

static thing_t  curThing, nextThing;
typedef struct {
    int             pmid, cid;
} Field;

static Field    wall[MAX_DEPTH+MAX_WIDTH][MAX_PERIMETER],
                base[MAX_WIDTH][MAX_WIDTH];

static Bool     atBaseFully();
static Bool     atBasePartially();
static Bool     wallChange();
static void     wall_to_base();
static void     dropWall();
static void     freezeWall();
static void     drawTrapazoid();
static void     drawSquare();
static void     XFillTrapazoid();
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
	(void) fprintf(stderr, "Welltris: invalid color '%s'.\n", name);
	return fg;
    }
    if (XAllocColor(display, colormap, &tmp) == 0) {
	(void) fprintf(stderr, "Welltris: can't allocate color '%s'.\n", name);
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
    int             i, j, turn_style, number_polyominoes; 

    (void) SRAND(time(NULL));
    readPolyominoes();

    turn_style = tris[squares - MIN_SQUARES].mode[diagonal].turn_style;
    if (turn_style < 0 || turn_style >= MAX_TYPES)
    {
      (void) fprintf(stderr, "Welltris: corrupted input file %s\n",
               POLYOMINOFILE);
      (void) fprintf(stderr, "\tturn_style = %d.\n", turn_style);
      exit(1);
    }
    number_polyominoes = tris[squares - MIN_SQUARES].mode
      [diagonal].number[mixed][turn_style];
    if (number_polyominoes <= 0 || number_polyominoes > MAX_START_POLYOMINOES)
    {
      (void) fprintf(stderr, "Welltris: corrupted input file %s\n",
               POLYOMINOFILE);
      (void) fprintf(stderr, "\tnumber_polyominoes = %d.\n",
               number_polyominoes);
      exit(1);
    }
    for (j = 0; j < MAX_DEPTH; j++)
         for (i = 0; i < MAX_PERIMETER; i++)
         {
                wall[j][i].pmid = EMPTY;
                wall[j][i].cid = 0;
         }
    for (j = 0; j < MAX_WIDTH; j++)
         for (i = 0; i < MAX_WIDTH; i++)
         {
                base[j][i].pmid = EMPTY;
                base[j][i].cid = 0;
         }
    for (i = 0; i < MAX_SIDES; i++)
      frozen_wall[i] = 0;

    titleLen = strlen(MSG_TITLE);
    titleWidth = XTextWidth(bigFont, MSG_TITLE, titleLen);
    authorLen = strlen(MSG_AUTHOR);
    authorWidth = XTextWidth(tinyFont, MSG_AUTHOR, authorLen);

    frameW = GAME_WIDTH - 2;
    frameH = GAME_WIDTH - 2;
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
	mainWin, (char *) wicon_bits, wicon_width, wicon_height);
    classhints.res_name = "welltris";
    classhints.res_class = "Welltris";

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

    gcv.foreground = BlackPixel(display, screen_num);
    gcv.background = WhitePixel(display, screen_num);
    gcv.function = GXxor;
    freezeGC = XCreateGC(display, blockWin, gcvm, &gcv);

    /* new things */
    newThing();
    checkFreeze();
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

void
clearScreen()
{
    XFillRectangle(display, blockWin, revGC,
      0, 0, GAME_WIDTH - 1, GAME_WIDTH - 1);
}

/* ------------------------------------------------------------------ */

static void
drawBox(win, gc, i, j)
    Window          win;
    GC              gc;
    int             i, j;
{
    if (j < MAX_DEPTH) {
        drawTrapazoid(win, gc, i, j);
    }
    else {
        int base_i, base_j;
        wall_to_base(&base_i, &base_j, i, j);
        drawSquare(win, gc, base_i, base_j);
    }
}

/* ------------------------------------------------------------------ */

void
drawField()
{
    int             i, j;

    for (i = 0; i < MAX_PERIMETER; i++)
        for (j = 0; j < MAX_DEPTH; j++) {
	    if (wall[j][i].pmid >= 0)
		drawTrapazoid(blockWin, thingGCs[wall[j][i].cid], i, j);
            if (frozen_wall[i / MAX_WIDTH])
	        drawTrapazoid(blockWin, freezeGC, i, j);
        }
    for (i = 0; i < MAX_WIDTH; i++)
        for (j = 0; j < MAX_WIDTH; j++)
	    if (base[j][i].pmid >= 0)
		drawSquare(blockWin, thingGCs[base[j][i].cid], i, j);
}

/* ------------------------------------------------------------------ */

void
drawThing()
{
    int             i, j, xi;
    
    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
        if (tris[curThing.squares].polyomino
                [curThing.polyomino_number].shape[j][i]) {
          xi = (curThing.xpos + i) % MAX_PERIMETER;
	  drawBox(blockWin, thingGCs[curThing.color_number],
	    xi, curThing.ypos + j);
        }
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
              [curThing.polyomino_number].shape[j][i]) {
	  drawBox(blockWin, thingGCs[curThing.color_number],
            (curThing.xpos + i) % MAX_PERIMETER, curThing.ypos + j);
        }

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++) {
        ox = (old->xpos + i - curThing.xpos + MAX_PERIMETER) % MAX_PERIMETER;
        oy = old->ypos + j - curThing.ypos;
	if (tris[old->squares].polyomino[old->polyomino_number].shape[j][i] &&
	    ((ox >= curThing.size) || (oy < 0) || (oy >= curThing.size) ||
	     !tris[curThing.squares].polyomino
                  [curThing.polyomino_number].shape[oy][ox])) {
              drawBox(blockWin, revGC,
                (old->xpos + i) % MAX_PERIMETER, old->ypos + j);
            }
	}
}

/* ------------------------------------------------------------------ */

void
drawNext()
{
    int             x, y;
    int             i, j;

    x = topMidX + (topRWidth - nextThing.size * BOXSIZE) / 2;
    y = topMidY - nextThing.size * BOXSIZE / 2;
    for (i = 0; i < nextThing.size; i++)
      for (j = 0; j < nextThing.size; j++)
	if (tris[nextThing.squares].polyomino
                [nextThing.polyomino_number].shape[j][i])
          XFillRectangle (display, mainWin, thingGCs[nextThing.color_number],
            i * BOXSIZE + x + 2 * DELTA,
            j * BOXSIZE + y + 2 * DELTA,
            BOXSIZE - 2 * DELTA - 1, BOXSIZE - 2 * DELTA - 1);
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
    int             i, j, xi, yj;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
        if ((y + j >= 0) && tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i]) {
          xi = (x + i) % MAX_PERIMETER;
          yj = y + j;
          if (yj < MAX_DEPTH) {
            wall[yj][xi].pmid = tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i];
	    wall[yj][xi].cid = curThing.color_number;
            frozen_wall[xi / MAX_WIDTH] = MAX_SIDES;
          } else {
            int base_i, base_j;
            wall_to_base(&base_i, &base_j, xi, yj);
            base[base_j][base_i].pmid = tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i];
	    base[base_j][base_i].cid = curThing.color_number;
          }
	}
}

/* ------------------------------------------------------------------ */

Bool
overlapping()
{
    int             i, j, xi, yj;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
	if (tris[curThing.squares].polyomino
                [curThing.polyomino_number].shape[j][i]) {
          xi = (x + i) % MAX_PERIMETER;
          yj = y + j;
          if (yj < MAX_DEPTH) {
            if (frozen_wall[xi / MAX_WIDTH])
	      return True; 
	    if (gradualAppear) {
/* This method one can turn polyomino to an area above of screen, also
   part of the polyomino may not be visible initially */          
              if ((yj >= 0) && (wall[yj][xi].pmid >= 0))
                return True;
            } else {
/* This method does not allow turning polyomino to an area above screen */
              if ((yj < 0) || (wall[yj][xi].pmid >= 0))
	        return True;
            }
          }
          else if (yj < MAX_DEPTH + MAX_WIDTH) {
            int base_i, base_j;
            wall_to_base(&base_i, &base_j, xi, yj);
            if (base[base_j][base_i].pmid >= 0)
	      return True;
          }
          else
            return True;
        }

    return False;
}

/* ------------------------------------------------------------------ */

Bool
atBottom()
{
    int             i, j, xi, yj;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
	if (tris[curThing.squares].polyomino
                [curThing.polyomino_number].shape[j][i]) {
          xi = (x + i) % MAX_PERIMETER;
          yj = y + j;
    	  if (yj < -1)
            return False;
          if (yj < MAX_DEPTH - 1) {
            if (frozen_wall[xi / MAX_WIDTH])
	      return True; 
            if (wall[yj + 1][xi].pmid >= 0)
	      return True;
          }
          else if (yj < MAX_DEPTH + MAX_WIDTH - 1) {
            int base_i, base_j;
            wall_to_base(&base_i, &base_j, xi, yj + 1);
            if (base[base_j][base_i].pmid >= 0)
	      return True;
          }
          else
            return True;
        }

    return False;
}

/* ------------------------------------------------------------------ */

static Bool
atBaseFully()
{
    int             i, j;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
	if (curThing.ypos + j < MAX_DEPTH && tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i])
 
          return False;
     return True;
}            

/* ------------------------------------------------------------------ */

static Bool
atBasePartially()
{
    int             i, j;

    for (i = 0; i < curThing.size; i++)
      for (j = 0; j < curThing.size; j++)
        if (curThing.ypos + j >= MAX_DEPTH && tris[curThing.squares].polyomino
              [curThing.polyomino_number].shape[j][i])
            
          return True;
     return False;
}
         
/* ------------------------------------------------------------------ */

static Bool
wallChange(old, new)
thing_t old, new;
{
    int             w = -1, i, j;
    int             x = curThing.xpos;

    for (i = 0; i < old.size; i++)
      for (j = 0; j < old.size; j++)
	if (tris[old.squares].polyomino[old.polyomino_number].shape[j][i]) {
          if (w == -1)
            w = ((x + i) % MAX_PERIMETER) / MAX_WIDTH;
          else if (w != ((x + i) % MAX_PERIMETER) / MAX_WIDTH)
            return True;
        }
    for (i = 0; i < new.size; i++)
      for (j = 0; j < new.size; j++)
	if (tris[new.squares].polyomino[new.polyomino_number].shape[j][i])
          if (w != ((x + i) % MAX_PERIMETER) / MAX_WIDTH)
            return True;
    return False;
}

/* ------------------------------------------------------------------ */

void
tryMove(move)
    move_t          move;
{
    thing_t         old;
    int             i, skip = False;

    old = curThing;

    switch (move) {
    case FALL:
	curThing.ypos ++;
	break;

    case DROP:
	do {
	    curThing.ypos ++;
	    score += level;
	} while (!overlapping());
	curThing.ypos --;
	break;

    case LEFT:
	curThing.xpos = (curThing.xpos + MAX_PERIMETER - 1) % MAX_PERIMETER;
        if (atBaseFully() || (atBasePartially() && wallChange(old, curThing)))
          skip = True;
	break;

    case RIGHT:
	curThing.xpos = (curThing.xpos + 1) % MAX_PERIMETER;
        if (atBaseFully() || (atBasePartially() && wallChange(old, curThing)))
          skip = True;
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
          if (atBasePartially() && wallChange(old, curThing))
            skip = True;
	}
	break;

    case REFLECT: /* reflect on y axis */
        if (tris[squares - MIN_SQUARES].mode[diagonal].turn_style == ALL)
        {
          curThing.polyomino_number = tris[curThing.squares].polyomino
            [curThing.polyomino_number].reflection;
	  curThing.xpos = old.xpos;
	  curThing.ypos = old.ypos;
          if (atBasePartially() && wallChange(old, curThing))
            skip = True;
	}
	break;
    }

    if (!skip && !overlapping())
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
    for (j = 0; j < MAX_DEPTH + MAX_WIDTH; j++)
        for (i = 0; i < MAX_PERIMETER; i++) {
                drawBox(blockWin, 
                        thingGCs[NRAND(tris[squares - MIN_SQUARES].mode
                          [diagonal].number[mixed][turn_style])],
                        i, j);
	        XSync(display, True);
	    }
}
							
/* ------------------------------------------------------------------ */

static void
freezeWall(w)
int w;
{
  int i, j;

  for (j = 0; j < MAX_DEPTH; j++)
    for (i = 0; i < MAX_WIDTH; i++)
      drawTrapazoid (blockWin, freezeGC, w * MAX_WIDTH + i, j);
  if (beep)
    XBell(display, BVOLUME);
  XSync(display, False);
}

/* ------------------------------------------------------------------ */

static void
wall_to_base(base_x, base_y, wall_x, wall_y)
int *base_x, *base_y, wall_x, wall_y;
{
  switch (wall_x / MAX_WIDTH)
  {
    case 0:
      *base_x = wall_x;
      *base_y = wall_y - MAX_DEPTH;
      break;
    case 1:
      *base_x = MAX_WIDTH - 1 + MAX_DEPTH - wall_y;
      *base_y = wall_x - MAX_WIDTH;
      break;
    case 2:
      *base_x = MAX_PERIMETER - 1 - MAX_WIDTH - wall_x;
      *base_y = MAX_WIDTH - 1 + MAX_DEPTH - wall_y;
      break;
    case 3:
      *base_x = wall_y - MAX_DEPTH;
      *base_y = MAX_PERIMETER - 1 - wall_x;
      break;
    default:
     (void) fprintf (stderr, "wall_to_base kinds range 0-3, got %d.\n",
               wall_x / MAX_WIDTH);
      exit(-1);
  }
}

/* ------------------------------------------------------------------ */

static void
dropWall(w)
int w;
{
  int i, j, k, l, fits, lines, base_x, base_y;
  lines = 0;
  for (j = MAX_DEPTH - 1; j >= 0 && lines == 0; j--)
    for (i = 0; i < MAX_WIDTH; i++)
      if (wall[j][w * MAX_WIDTH + i].pmid != EMPTY)
        lines = MAX_DEPTH - j;
  if (lines > 0) {
    fits = True;
    j = 0;
    while (j < (MAX_WIDTH / 2 + lines - 1) && fits) {
      j++;
      for (l = MAX_DEPTH - j; l < MAX_DEPTH; l++)
        for (i = 0; i < MAX_WIDTH; i++)
          if (wall[l][w * MAX_WIDTH + i].pmid != EMPTY) {
            wall_to_base(&base_x, &base_y,
              w * MAX_WIDTH + i, l + j);
            if (base[base_y][base_x].pmid != EMPTY)
              fits = False;
          }
    }
    if (!fits)
      j--;
    if (j > 0)
    {
      for (l = MAX_DEPTH - 1; l >= 0; l--)
        for (i = 0; i < MAX_WIDTH; i++)
          if (wall[l][w * MAX_WIDTH + i].pmid != EMPTY) {
            k = w * MAX_WIDTH + i;
            if (l + j >= MAX_DEPTH) {
              wall_to_base(&base_x, &base_y, k, l + j);
              base[base_y][base_x] = wall[l][k];
              wall[l][k].pmid = EMPTY;
              drawTrapazoid(blockWin, revGC, k, l);
              drawSquare(blockWin, thingGCs[wall[l][k].cid], base_x, base_y);
            } else {
              wall[l + j][k] = wall[l][k];
              wall[l][k].pmid = EMPTY;
              drawTrapazoid(blockWin, revGC, k, l);
              drawTrapazoid(blockWin, thingGCs[wall[l][k].cid], k, l + j);
            }
            XFlush(display);
          }
      if (beep)
        XBell(display, BVOLUME);
      XSync(display, False);
    }
  }
}

/* ------------------------------------------------------------------ */

Bool
allFrozen()
{
  int w;

  for (w = 0; w < MAX_SIDES; w++)
    if (frozen_wall[w] == MAX_SIDES) {
      freezeWall(w);
      XSync(display, False);
    }
  for (w = 0; w < MAX_SIDES; w++)
    if (frozen_wall[w] == 0)
      return False;
  return True;
}

/* ------------------------------------------------------------------ */

void
checkFreeze()
{
  int w;

  for (w = 0; w < MAX_SIDES; w++) {
    if (frozen_wall[w] == 0)
      dropWall(w);
    else {
      frozen_wall[w]--;
      if (frozen_wall[w] == 0) {
        freezeWall(w);
        dropWall(w);
      }
    }
  }
  /* not all frozen else this is an infinite loop */
  /* curThing.xpos = (MAX_WIDTH - curThing.size) / 2; */
  curThing.xpos = NRAND(MAX_WIDTH - curThing.size + 1);
  do {
    w = NRAND(MAX_SIDES);
  } while (frozen_wall[w]);
  curThing.xpos += w * MAX_WIDTH;
}

/* ------------------------------------------------------------------ */

int
checkLines()
{
    int             lSet[2][MAX_WIDTH], nset[2];
    int             i, j, k;

    nset[0] = nset[1] = 0;

    for (j = 0; j < MAX_WIDTH; j++) {
        lSet[0][j] = 0;
        for (i = 0; i < MAX_WIDTH; i++)
            if (base[j][i].pmid >= 0)
                lSet[0][j] ++;
        if (lSet[0][j] == MAX_WIDTH)
            nset[0] ++;
    }
    for (i = 0; i < MAX_WIDTH; i++) {
        lSet[1][i] = 0;
        for (j = 0; j < MAX_WIDTH; j++)
            if (base[j][i].pmid >= 0)
                lSet[1][i] ++;
        if (lSet[1][i] == MAX_WIDTH)
            nset[1] ++;
    }

    if (nset[0] || nset[1]) {
        Field temp_base[MAX_WIDTH][MAX_WIDTH];
        for (k = 0; k < ((NUM_FLASHES / (nset[0] + nset[1])) % 2) * 2; k++) {
            for (j = 0; j < MAX_WIDTH; j++) {
                if (lSet[0][j] == MAX_WIDTH)
                  for (i = 0; i < MAX_WIDTH; i++)
                    drawSquare(blockWin, xorGC, i, j);
                if (lSet[1][i] == MAX_WIDTH)
                  for (i = 0; i < MAX_WIDTH; i++)
                    drawSquare(blockWin, xorGC, j, i);
            }
            XFlush(display);
        }

        for (j = 0; j < MAX_WIDTH; j++)
          for (i = 0; i < MAX_WIDTH; i++)
            temp_base[j][i] = base[j][i];
        if (nset[0] > 0)
          for (j = 0; j < MAX_WIDTH; j++)
            if (lSet[0][j] == MAX_WIDTH) {
              if (j < MAX_WIDTH / 2)
                for (i = 0; i < MAX_WIDTH; i++) {
                  for (k = j; k > 0; k--)
                    temp_base[k][i] = temp_base[k - 1][i];
                  temp_base[0][i].pmid = EMPTY;
                }
              else
                for (i = 0; i < MAX_WIDTH; i++) {
                  for (k = j; k < MAX_WIDTH - 1; k++)
                    temp_base[k][i] = temp_base[k + 1][i];
                  temp_base[MAX_WIDTH - 1][i].pmid = EMPTY;
                }
            }
        if (nset[1] > 0)
          for (i = 0; i < MAX_WIDTH; i++)
            if (lSet[1][i] == MAX_WIDTH) {
              if (i < MAX_WIDTH / 2)
                for (j = 0; j < MAX_WIDTH; j++) {
                  for (k = i; k > 0; k--)
                    temp_base[j][k] = temp_base[j][k - 1];
                  temp_base[j][0].pmid = EMPTY;
                }
              else
                for (j = 0; j < MAX_WIDTH; j++) {
                  for (k = i; k < MAX_WIDTH - 1; k++)
                    temp_base[j][k] = temp_base[j][k + 1];
                  temp_base[j][MAX_WIDTH - 1].pmid = EMPTY;
                }
            }
       for (j = 0; j < MAX_WIDTH; j++)
         for (i = 0; i < MAX_WIDTH; i++)
           if ((temp_base[j][i].cid != base[j][i].cid) ||
               (temp_base[j][i].pmid != base[j][i].pmid))
           {
             base[j][i] = temp_base[j][i];
             if (base[j][i].pmid >= 0)
               drawSquare(blockWin, thingGCs[base[j][i].cid], i, j);
             else
               drawSquare(blockWin, revGC, i, j);
             XFlush(display);
           }

      if (beep)
        XBell(display, BVOLUME);
      XSync(display, False);
    }

    return (nset[0] + nset[1]);
}

/* ------------------------------------------------------------------ */

void
drawGrid()
{
  GC gc;
  int i, k, square_x, square_y, sum, sum_var;

  if (grid)
    gc = tinyGC;
  else
    gc = revGC;
  sum = sum_var = MAX_WIDTH * BOXSIZE + DELTA;
  square_x = 0;
  for (i = 0; i < MAX_WIDTH; i++)
  {
    XDrawLine(display, blockWin, gc, square_x, 0, sum_var, sum);
    XDrawLine(display, blockWin, gc,
      ENDPT - 1, square_x, ENDPT - 1 - sum, sum_var);
    XDrawLine(display, blockWin, gc, ENDPT - 1 - square_x, ENDPT - 1,
      ENDPT - 1 - sum_var, ENDPT - 1 - sum);
    XDrawLine(display, blockWin, gc,
      0, ENDPT - 1 - square_x, sum, ENDPT - 1 - sum_var);
    square_x += 3 * BOXSIZE;
    sum_var += BOXSIZE;
    if (2 * (i + 1) == MAX_WIDTH)
      square_x++;
  }
  sum = ENDPT;
  square_x = 0;
  k = (MAX_WIDTH * BOXSIZE) / MAX_DEPTH;
  for (i= 0; i <= MAX_DEPTH; i++)
  {
    XFillRectangle(display, blockWin, gc,
      square_x, square_x, 1, sum);
    XFillRectangle(display, blockWin, gc,
      square_x, square_x, sum, 1);
    XFillRectangle(display, blockWin, gc,
      square_x + sum - 1, square_x, 1, sum);
    XFillRectangle(display, blockWin, gc,
      square_x, square_x + sum - 1, sum, 1);
    square_x += k;
    sum -= (2 * k);
  }
  sum = MAX_WIDTH * BOXSIZE + DELTA;
  square_x = square_y = sum;
  for (i= 0; i <= MAX_WIDTH ; i++)
  {
    XFillRectangle(display, blockWin, gc, square_x, square_y, 1, sum);
    square_x += BOXSIZE;
  }
  square_x = square_y = sum;
  for (i= 0; i <= MAX_WIDTH ; i++)
  {
    XFillRectangle(display, blockWin, gc, square_x, square_y, sum, 1);
    square_y += BOXSIZE;
  }
  XSync(display, False);
}

/* ------------------------------------------------------------------ */

static void
drawTrapazoid (win, gc, i, j)
Window win;
GC gc;
int i, j;
{
  int ulx, uly, lrx, lry, base_x, plateau_x, w;

  w = i / MAX_WIDTH;
  i -= (w * MAX_WIDTH);
  ulx = 3 * BASE_WIDTH / 2 + (i - (MAX_WIDTH / 2)) *
    (2 * BOXSIZE * (MAX_DEPTH - j) / MAX_DEPTH + BOXSIZE) + 2 * DELTA;
  uly = j * MAX_WIDTH * BOXSIZE/MAX_DEPTH + 2*DELTA;
  lrx = 3 * BASE_WIDTH / 2 + (i + 1 - (MAX_WIDTH / 2)) *
    (2 * BOXSIZE * (MAX_DEPTH - j - 1) / MAX_DEPTH + BOXSIZE) +
    DELTA * (i / 4) - 3 * DELTA;
  lry = (j + 1) * MAX_WIDTH * BOXSIZE / MAX_DEPTH - DELTA;
  base_x = BOXSIZE + (MAX_DEPTH - j) * 2 * BOXSIZE / MAX_DEPTH -
    5 * DELTA;
  plateau_x = BOXSIZE + (MAX_DEPTH - j - 1) * 2 *
    BOXSIZE / MAX_DEPTH - 5 * DELTA;
  switch (w)
  {
    case 0:
      XFillTrapazoid (display, win, gc,
        ulx, uly, base_x, 0, lrx, lry, plateau_x, 0);
      break;
    case 1:
      XFillTrapazoid (display, win, gc,
        ENDPT - uly, ulx, 0, base_x, ENDPT - lry, lrx, 0, plateau_x);
      break;
    case 2:
      XFillTrapazoid (display, win, gc,
        ENDPT - 1 - ulx, ENDPT - uly, -base_x, 0,
        ENDPT - 1 - lrx, ENDPT - lry, -plateau_x, 0);
      break;
    case 3:
      XFillTrapazoid (display, win, gc,
         uly, ENDPT - 1 - ulx, 0, -base_x,
         lry, ENDPT - 1 - lrx, 0, -plateau_x);
      break;
    default:
      (void) fprintf (stderr, "trapazoid kinds range 0-3, got %d.\n", w);
  }
}

/* ------------------------------------------------------------------ */

static void
drawSquare (win, gc, i, j)
Window win;
GC gc;
int i, j;
{
  XFillRectangle (display, win, gc,
    i * BOXSIZE + BASE_WIDTH + 2 * DELTA,
    j * BOXSIZE + BASE_WIDTH + 2 * DELTA,
    BOXSIZE - 2 * DELTA - 1, BOXSIZE - 2 * DELTA - 1);
}

/* ------------------------------------------------------------------ */

static void
XFillTrapazoid (display, window, gc,
  ulx, uly, base_x, base_y, lrx, lry, plateau_x, plateau_y)
Display *display;
Window window;
GC gc;
int ulx, uly, base_x, base_y, lrx, lry, plateau_x, plateau_y;
{
  static XPoint trapazoid_list[MAX_SIDES];
  trapazoid_list[0].x = ulx;
  trapazoid_list[0].y = uly;
  trapazoid_list[1].x = base_x;
  trapazoid_list[1].y = base_y;
  trapazoid_list[2].x = lrx - base_x - ulx;
  trapazoid_list[2].y = lry - base_y - uly;
  trapazoid_list[3].x = -plateau_x;
  trapazoid_list[3].y = -plateau_y;
  XFillPolygon (display, window, gc, trapazoid_list, MAX_SIDES,
    Convex, CoordModePrevious);
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
    (void) fprintf(stderr, "Can not open input file, %s\n.", POLYOMINOFILE);
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
