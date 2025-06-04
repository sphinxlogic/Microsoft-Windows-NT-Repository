/*
# GENERIC X-BASED HEXTRIS
#
#	utils.c
#
###
#
#  Copyright (c) 1993 - 95	David A. Bagley, bagleyd@perry.njit.edu
#
#  Taken from GENERIC X-BASED TETRIS
#
#  Copyright (c) 1992 - 95		Q. Alex Zhao, azhao@cc.gatech.edu
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

#include	"hextris.h"

#include	"hicon.xbm"

Atom            delw;

static GC       revGC, bigGC, tinyGC, xorGC;
static GC       thingGCs[MAX_START_POLYHEXES];

static char    *winName = "GENERIC ALHEXTRIS";
static char    *iconName = "ALHEXTRIS";

static int      titleLen, titleWidth, authorLen, authorWidth;
static int      titleX, titleY, authorX, authorY;
static int      sX;
static int      sLevelY, sRowsY, sScoreY;

static int      topRWidth, topWidth, topHeight, topMidX, topMidY;
static int      frameX, frameY, frameW, frameH;

typedef struct {
    int             hexagons, polyhex_number;
    int             xpos, ypos;
    int             size, color_number, direction;
    int             bonus, random_rotation, random_reflection;
    long            random_number;
}               thing_t;
typedef struct {
    int             rotation;
    int             reflection;
    int             start_height;
    int             shape[HEX_ROW_COL(HEX_SIZE)][HEX_ROW_COL(HEX_SIZE)];
    int             size;
}               Polyhexes;
typedef struct {
    int             start[MAX_START_POLYHEXES][MAX_TYPES];
    int             turn_style;
}               Mode;
typedef struct {
    Polyhexes       polyhex[MAX_POLYHEXES];
    Mode            mode[MAX_MODES];
    int             number[MAX_MODES][MAX_TYPES];
    int             corners_switch;
}               Polytris;
static Polytris tris[MAX_HEXAGONS - MIN_HEXAGONS + 1];

static char    *thingFGs[MAX_START_POLYHEXES] = {
    "Red", "Green", "Blue", "Yellow", 
    "Magenta", "Cyan", "FireBrick", "GreenYellow",
    "SlateBlue", "Khaki", "Plum", "Violet",
    "DarkTurquoise", "Gold", "Orchid", "Turquoise",
    "Orange", "OrangeRed", "VioletRed", "BlueViolet",
    "SeaGreen", "Pink", "ForestGreen", "SkyBlue",
    "Coral", "Wheat", "GoldenRod", "IndianRed",
    "SpringGreen", "DarkViolet", "MediumSeaGreen", "DarkSlateGray",
    "CadetBlue", "LightSteelBlue", "NavyBlue", "SteelBlue",
    "YellowGreen", "DarkViolet", "MediumSeaGreen", "DarkSlateGray",
    "LightGray", "MediumVioletRed", "Sienna", "MediumAquamarine",
    "MediumBlue", "Navy", "DarkOliveGreen", "DarkGreen",
    "DimGray", "Tan", "MediumTurquoise", "DarkSlateBlue",
    "Maroon", "MediumSlateBlue", "PaleGoldenrod", "MediumSpringGreen",
    "Gray", "Black"
};

static thing_t  curThing, nextThing;
static struct {
    int         pmid, cid;
}               field[ROWS][COLS];

static XPoint hexagon_unit[MAX_ORIENT][6] = {
  { {0, 0}, {2, 0}, {1, 1}, {-1, 1}, {-2, 0}, {-1, -1} },
  { {0, 0}, {1, 1}, {0, 2}, {-1, 1}, {-1, -1}, {0, -2} }
};
static XPoint hexagon_list[MAX_ORIENT][6];
static XPoint hexagon_border[7];

static Bool     overlapped();
static Bool     collision();
static void     sizeHexs();
static Bool     inHexagon();
static void     readPolyhexes();

/* ------------------------------------------------------------------ */

static unsigned long
getColor(name)
    char            name[];
{
    XColor          tmp;

    if (!useColor)
	return fg;
    if (XParseColor(display, colormap, name, &tmp) == 0) {
	(void) fprintf(stderr, "Hextris: invalid color '%s'.\n", name);
	return fg;
    }
    if (XAllocColor(display, colormap, &tmp) == 0) {
	(void) fprintf(stderr, "Hextris: can't allocate color '%s'.\n", name);
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
    int             i, j, col, turn_style, number_polyhexes; 

    (void) SRAND(time(NULL));
    readPolyhexes();

    turn_style = tris[hexagons - MIN_HEXAGONS].mode[corners].turn_style;
    if (turn_style < 0 || turn_style >= MAX_TYPES)
    {
      (void) fprintf(stderr, "Hextris: corrupted input file %s\n",
               POLYHEXFILE);
      (void) fprintf(stderr, "\tturn_style = %d.\n", turn_style);
(void) fprintf(stderr, "%d %d\n",hexagons, corners);
      exit(1);
    }
    number_polyhexes = tris[hexagons - MIN_HEXAGONS].number[mixed][turn_style];
    if (number_polyhexes <= 0 || number_polyhexes > MAX_START_POLYHEXES)
    {
      (void) fprintf(stderr, "Hextris: corrupted input file %s\n",
               POLYHEXFILE);
      (void) fprintf(stderr, "\tnumber_polyhexes = %d.\n", number_polyhexes);
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
                field[j][i].cid = NRAND(number_polyhexes);
            } else {
                field[j][i].pmid = -1;
                field[j][i].cid = 0;
            }
    else
      /* Second Algorithm randomly picks HEXAGONS_PER_ROW boxes per row 
         This is more in keeping with the original */
      for (j = 0; j < ROWS; j++) {
         for (i = 0; i < COLS; i++) {
                field[j][i].pmid = -1;
                field[j][i].cid = 0;
	 }
	 if (j >= ROWS - prefilled)
           for (i = 0; i < HEXAGONS_PER_ROW; i++) {
                do
                    col = NRAND(COLS);
                while (field[j][col].pmid != -1);
                field[j][col].pmid = 0;
                field[j][col].cid = NRAND(number_polyhexes);
	   }
      }

    titleLen = strlen(MSG_TITLE);
    titleWidth = XTextWidth(bigFont, MSG_TITLE, titleLen);
    authorLen = strlen(MSG_AUTHOR);
    authorWidth = XTextWidth(tinyFont, MSG_AUTHOR, authorLen);

    frameW = GAME_WIDTH - 1;
    frameH = GAME_HEIGHT - 1;
    topRWidth = HEXWIDTH * MAX_HEXAGONS + OFFSET * 2;
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

    sizeHexs();

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
	mainWin, (char *) hicon_bits, hicon_width, hicon_height);
    classhints.res_name = "hextris";
    classhints.res_class = "Hextris";

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
    for (i = 0; i < MAX_START_POLYHEXES; i++) {
	gcv.foreground = getColor(thingFGs[i]);
	if (gcv.foreground == bg)
	    gcv.foreground = fg;
	thingGCs[i] = XCreateGC(display, blockWin, gcvm, &gcv);
    }

    gcv.foreground = fg;
    gcv.function = GXxor;
    xorGC = XCreateGC(display, blockWin, gcvm, &gcv);

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

void
clearScreen()
{
    XFillRectangle(display, blockWin, revGC,
       0, 0, GAME_WIDTH - 1, GAME_HEIGHT - 1);
}

/* ------------------------------------------------------------------ */

static void
sizeHexs()
{
  int i;
  for (i = 0; i < MAX_SIDES; i++)
  { 
    hexagon_list[CORNERS][i].x = (HEXWIDTH - 2 * DELTAX) / 2 *
      hexagon_unit[CORNERS][i].x;
    hexagon_list[CORNERS][i].y = (2 * HEXHEIGHT / 3 - DELTAY) / 2 *
      hexagon_unit[CORNERS][i].y;      
    hexagon_list[NOCORN][i].x = (HEXWIDTH - DELTAX) / 4 *
      hexagon_unit[NOCORN][i].x;
    hexagon_list[NOCORN][i].y = 3 * (2 * HEXHEIGHT / 3 - DELTAY) / 4 *
      hexagon_unit[NOCORN][i].y;      
    hexagon_border[i].x = HEXWIDTH / 2 * hexagon_unit[CORNERS][i].x;
    hexagon_border[i].y = HEXHEIGHT / 3 * hexagon_unit[CORNERS][i].y;  
  }
  hexagon_border[MAX_SIDES].x = -hexagon_border[MAX_SIDES / 2].x;
  hexagon_border[MAX_SIDES].y = -hexagon_border[MAX_SIDES / 2].y;
}

/* ------------------------------------------------------------------ */

static void
drawHex(win, gc, i, j)
    Window          win;
    GC              gc;
    int             i, j;
{
    if (j < 0)
      return;
    hexagon_list[corners][0].x = i * HEXWIDTH +
      (((j + invert) % 2) ? (HEXWIDTH + DELTAX) / 2 : HEXWIDTH + 1) +
      ((corners) ? 0 : -HEXWIDTH / 4);
    hexagon_list[corners][0].y = j * HEXHEIGHT + DELTAY - 1 +
        ((corners) ? 2 : HEXHEIGHT / 4 - 1);
    XFillPolygon(display, win, gc, hexagon_list[corners], MAX_SIDES,
        Convex, CoordModePrevious);
}

/* ------------------------------------------------------------------ */

static void
drawHexBorder(win, gc, i, j)
    Window          win;
    GC              gc;
    int             i, j;
{
    hexagon_border[0].x = i * HEXWIDTH +
      (((j + invert) % 2) ? (HEXWIDTH + DELTAX) / 2 : HEXWIDTH + 1);
    hexagon_border[0].y = j * HEXHEIGHT + DELTAY / 2 - 1;
    XDrawLines(display, win, gc, hexagon_border, MAX_SIDES + 1,
      CoordModePrevious);
}

/* ------------------------------------------------------------------ */

void
drawField()
{
    int             i, j;

    for (i = 0; i < COLS; i++)
	for (j = 0; j < ROWS; j++)
	    if (field[j][i].pmid >= 0)
		drawHex(blockWin, thingGCs[field[j][i].cid], i, j);
}

/* ------------------------------------------------------------------ */

void
drawThing()
{
    int             i, j;
    
    for (i = 0; i < HEX_ROW_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW_COL(curThing.size); j++)
        if (tris[curThing.hexagons].polyhex
                [curThing.polyhex_number].shape[j][i])
	  drawHex(blockWin, thingGCs[curThing.color_number],
	    curThing.xpos + i + XPOS(curThing.size, curThing.ypos, j, invert),
            curThing.ypos + j);
}

/* ------------------------------------------------------------------ */

static void
drawThingDiff(old)
    thing_t        *old;
{
    int             i, j, ox, oy;
    int             s = HEX_ROW_COL(curThing.size);

    for (i = 0; i < s; i++)
      for (j = 0; j < s; j++)
        if ((curThing.ypos + j >= 0) && tris[curThing.hexagons].polyhex
              [curThing.polyhex_number].shape[j][i])
	  drawHex(blockWin, thingGCs[curThing.color_number],
            curThing.xpos + i + XPOS(curThing.size, curThing.ypos, j, invert),
            curThing.ypos + j);

    for (i = 0; i < s; i++)
      for (j = 0; j < s; j++)
      {
        ox = old->xpos + i - curThing.xpos;
        if ((curThing.ypos - old->ypos) % 2)
          ox += (((curThing.size + j) % 2) ?
            (((old->ypos + invert) % 2) ? -1 : 0) :
            (((old->ypos + !invert) % 2) ? 1 : 0));
        oy = old->ypos + j - curThing.ypos;
	if (tris[old->hexagons].polyhex[old->polyhex_number].shape[j][i] &&
	    ((ox < 0) || (ox >= s) || (oy < 0) || (oy >= s) ||
	     !tris[curThing.hexagons].polyhex
                  [curThing.polyhex_number].shape[oy][ox]))
              drawHex(blockWin, revGC,
                old->xpos + i + XPOS(curThing.size, old->ypos, j, invert),
                old->ypos + j);
      }
}

/* ------------------------------------------------------------------ */

void
drawNext()
{
    int             x, y;
    int             i, j;
    int             len = HEX_ROW_COL(nextThing.size);

    x = topMidX + (topRWidth - len * HEXWIDTH) / 2 + 1;
    y = topMidY - len * HEXHEIGHT / 2 + DELTAY;
    for (i = 0; i < len; i++)
      for (j = 0; j < len; j++)
	if (tris[nextThing.hexagons].polyhex
                [nextThing.polyhex_number].shape[j][i]) {
          hexagon_list[corners][0].x = x + i * HEXWIDTH + 
            (((j + nextThing.size) % 2) ? (HEXWIDTH + DELTAX) / 2 :
                                          HEXWIDTH + 1);
          hexagon_list[corners][0].y = y + j * HEXHEIGHT + DELTAY - 1;
          XFillPolygon(display, mainWin, thingGCs[nextThing.color_number],
            hexagon_list[corners], MAX_SIDES, Convex, CoordModePrevious);
        }
}

/* ------------------------------------------------------------------ */

void
clearNext()
{
    XFillRectangle(display, mainWin, revGC,
	topMidX,
	topMidY - (HEXHEIGHT * HEX_ROW_COL(HEX_SIZE) + HEXHEIGHT / 3) / 2 + 3,
	topRWidth, HEXHEIGHT * HEX_ROW_COL(HEX_SIZE) + HEXHEIGHT / 3);
}

/* ------------------------------------------------------------------ */

void
redoNext()
{
    int turn_style = tris[hexagons - MIN_HEXAGONS].mode[corners].turn_style;
    int next_start, i;

    nextThing.hexagons = hexagons - MIN_HEXAGONS + nextThing.bonus;
    next_start = nextThing.random_number % tris[nextThing.hexagons].
      number[(nextThing.bonus) ? NOMIX : mixed][turn_style];
    nextThing.color_number = next_start;

    if (mixed && !nextThing.bonus)
    {
      nextThing.hexagons = 0;
      while (next_start >=
             tris[nextThing.hexagons].number[NOMIX][turn_style])
      {
        next_start -=
          tris[nextThing.hexagons].number[NOMIX][turn_style];
        nextThing.hexagons++;
      }
    }
    nextThing.polyhex_number =
      tris[nextThing.hexagons].mode[corners].start[next_start][turn_style];
    if (tris[nextThing.hexagons].mode[corners].turn_style > NONE)
      for (i = 0; i < nextThing.random_rotation; i++)
        nextThing.polyhex_number = tris[nextThing.hexagons].polyhex
          [nextThing.polyhex_number].rotation;
    if (tris[nextThing.hexagons].mode[corners].turn_style == ALL)
      for (i = 0; i < nextThing.random_reflection; i++)
        nextThing.polyhex_number = tris[nextThing.hexagons].polyhex
          [nextThing.polyhex_number].reflection;
    nextThing.size = tris[nextThing.hexagons].polyhex
      [nextThing.polyhex_number].size;
    /* nextThing.xpos = NRAND(COLS - nextThing.size + 1); */
    nextThing.xpos = (COLS - nextThing.hexagons) / 2;
    nextThing.ypos = -tris[nextThing.hexagons].polyhex
      [nextThing.polyhex_number].start_height;
    nextThing.direction = (NRAND(2) % 2) ? 1 : -1;
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
putHex()
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < HEX_ROW_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW_COL(curThing.size); j++)
        if ((y + j >= 0) && tris[curThing.hexagons].polyhex
              [curThing.polyhex_number].shape[j][i]) {
          field[y + j][x + i + XPOS(curThing.size, y, j, invert)].pmid =
	    tris[curThing.hexagons].polyhex
                [curThing.polyhex_number].shape[j][i];
	  field[y + j][x + i + XPOS(curThing.size, y, j, invert)].cid =
            curThing.color_number;
	}
}

/* ------------------------------------------------------------------ */

Bool
overlapping()
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < HEX_ROW_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW_COL(curThing.size); j++)
	if (tris[curThing.hexagons].polyhex
                [curThing.polyhex_number].shape[j][i]) {
          if ((y + j >= ROWS) ||
              (x + i + XPOS(curThing.size, y, j, invert) < 0) ||
              (x + i + XPOS(curThing.size, y, j, invert) >= COLS))
            return True;
	  if (gradualAppear) {
/* This method one can turn a polyhex to an area above of screen, also
   part of the polyhex may not be visible initially */          
            if ((y + j >= 0) && (field[y + j][x + i +
                 XPOS(curThing.size, y, j, invert)].pmid >= 0))
              return True;
          } else {
/* This method does not allow turning a polyhex to an area above screen */
            if ((y + j < 0) || (field[y + j][x + i +
                 XPOS(curThing.size, y, j, invert)].pmid >= 0))
	      return True;
          }
        }

    return False;
}

/* ------------------------------------------------------------------ */

static Bool
overlapped(dir, fall)
int dir, fall;
{
    int             i, j, k, l, blocked[2];
    int             x = curThing.xpos, y = curThing.ypos;
    int             oldy = curThing.ypos - fall;
    int             oldx = curThing.xpos -
			((fall) ? XINC(y, dir, invert) : dir);

    blocked[0] = blocked[1] = False;
    for (i = 0; i < HEX_ROW_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW_COL(curThing.size); j++)
        if ((y + j >= 0) && tris[curThing.hexagons].polyhex
              [curThing.polyhex_number].shape[j][i])
          if (fall)
          {
            k = oldx + i + XPOS(curThing.size, oldy, j, invert) + dir;
            if (gradualAppear) {
/* This method there is no wall on top of screen */
              if ((oldy + j >= 0) &&
                  ((k < 0) || (k >= COLS) || (field[oldy + j][k].pmid >= 0)))
                blocked[0] = True;
            } else {
/* This method there is a wall on top of screen */
              if ((oldy + j < 0) ||
                  (k < 0) || (k >= COLS) || (field[oldy + j][k].pmid >= 0))
                blocked[0] = True;
            }
            k = x + i + XPOS(curThing.size, y, j, invert) - dir;
            if ((k < 0) || (k >= COLS) || field[y + j][k].pmid >= 0)
              blocked[1] = True;
          }
          else
          {
            l = oldx + i + XINC(y + 1, dir, invert) +
              XPOS(curThing.size, y + 1, j, invert);
            k = y - 1 + j;
            if (gradualAppear) {
/* This method there is no wall on top of screen */
              if ((k >= 0) && (field[k][l].pmid >= 0))
                blocked[0] = True;
            } else {
/* This method there is a wall on top of screen */
              if ((k < 0) || (field[k][l].pmid >= 0))
                blocked[0] = True;
            }
            k = y + 1 + j;
            if ((k >= ROWS) || (field[k][l].pmid >= 0))
              blocked[1] = True;
          }
    return blocked[0] && blocked[1];
}

/* ------------------------------------------------------------------ */

Bool
atBottom()
{
  if (collision(curThing.direction))
    return collision(-curThing.direction);
  return False;
}

/* ------------------------------------------------------------------ */

static Bool collision(dir)
int dir;
{
    int             i, j, k, blocked[2];
    int             x = curThing.xpos, y = curThing.ypos;
    int             newy = curThing.ypos + 1;
    int             newx = curThing.xpos + XINC(newy, dir, invert);

    for (i = 0; i < HEX_ROW_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW_COL(curThing.size); j++)
	if ((y + j >= -1) && tris[curThing.hexagons].polyhex
              [curThing.polyhex_number].shape[j][i]) {
          k = x + i + XINC(newy, dir, invert) +
            XPOS(curThing.size, newy, j, invert);
	  if ((y + j >= ROWS - 1) || (k < 0) || (k >= COLS) ||
              (field[newy + j][k].pmid >= 0))
	    return True;
        }
    if (corners) {
      blocked[0] = blocked[1] = False;
      for (i = 0; i < HEX_ROW_COL(curThing.size); i++)
        for (j = 0; j < HEX_ROW_COL(curThing.size); j++)
	  if ((y + j >= -1) && tris[curThing.hexagons].polyhex
                [curThing.polyhex_number].shape[j][i])
          {
            k = x + i + XPOS(curThing.size, y, j, invert) + dir;
            if (gradualAppear) {
/* This method there is no wall on top of screen */
              if ((y + j >= 0) &&
                  ((k < 0) || (k >= COLS) || (field[y + j][k].pmid >= 0)))
                blocked[0] = True;
            } else {
/* This method there is a wall on top of screen */
              if ((y + j < 0) || 
                  (k < 0) || (k >= COLS) || (field[y + j][k].pmid >= 0))
                blocked[0] = True;
            }
            k = newx + i + XPOS(curThing.size, newy, j, invert) - dir;
            if ((k < 0) || (k >= COLS) || field[newy + j][k].pmid >= 0)
              blocked[1] = True;
          }
      return blocked[0] && blocked[1];
    }
    return False;
}

/* ------------------------------------------------------------------ */

void
tryMove(move)
    move_t          move;
{
    thing_t         old, mid;
    int             i;

    old = curThing;

    switch (move) {
    case FALL:
	curThing.ypos ++;
        curThing.xpos += XINC(curThing.ypos, curThing.direction, invert);
        if (overlapping() || (corners &&
            overlapped(curThing.direction, True))) {
            curThing = old;
            curThing.direction = -curThing.direction;
            curThing.ypos ++;
            curThing.xpos += XINC(curThing.ypos, curThing.direction, invert);
        }
        if (overlapping() || (corners &&
            overlapped(curThing.direction, True)))
	  curThing = old;
	else
 	  drawThingDiff(&old);
	break;

    case DROP:
	do {
            do {
                mid = curThing;
	        curThing.ypos ++;
                curThing.xpos += XINC(curThing.ypos, curThing.direction,
                     invert);
	        score += level + prefilled;
            } while (!overlapping() && !(corners &&
                     overlapped(curThing.direction, True)));
            curThing = mid;
            curThing.direction = -curThing.direction;
            curThing.ypos ++;
            curThing.xpos += XINC(curThing.ypos, curThing.direction, invert);
	} while (!overlapping() && !(corners && 
                 overlapped(curThing.direction, True)));
	curThing = mid;
	drawThingDiff(&old);
	break;

    case LEFT:
	curThing.xpos --;
	if (overlapping() || (corners && overlapped(-1, False)))
	    curThing = old;
	else
	    drawThingDiff(&old);
	break;

    case RIGHT:
	curThing.xpos ++;
	if (overlapping() || (corners && overlapped(1, False)))
	    curThing = old;
	else
	    drawThingDiff(&old);
	break;

    case ROTATE:
        if (tris[hexagons - MIN_HEXAGONS].mode[corners].turn_style > NONE)
        {
          if (cw)
            for (i = 0; i < MAX_SIDES - 1; i++)
              curThing.polyhex_number = tris[curThing.hexagons].polyhex
                [curThing.polyhex_number].rotation;
          else /* ccw */
            curThing.polyhex_number = tris[curThing.hexagons].polyhex
              [curThing.polyhex_number].rotation;
	  curThing.xpos = old.xpos;
	  curThing.ypos = old.ypos;
	}
	if (!overlapping())
	    drawThingDiff(&old);
	else
	    curThing = old;
	break;

    case REFLECT: /* reflect on y axis */
        if (tris[hexagons - MIN_HEXAGONS].mode[corners].turn_style == ALL)
        {
          curThing.polyhex_number = tris[curThing.hexagons].polyhex
            [curThing.polyhex_number].reflection;
	  curThing.xpos = old.xpos;
	  curThing.ypos = old.ypos;
	}
	if (!overlapping())
	    drawThingDiff(&old);
	else
	    curThing = old;
	break;
    }
}

/* ------------------------------------------------------------------ */

void
fillLines()
{
    int             i, j;
    int turn_style = tris[hexagons - MIN_HEXAGONS].mode[corners].turn_style;
    
    XSync(display, True);
    for (j = 0; j <= ROWS / 2; j++) {
      for (i = 0; i < COLS; i++) {
	drawHex(blockWin, thingGCs[NRAND(tris[hexagons - MIN_HEXAGONS].number
            [mixed][turn_style])], i, j);
	drawHex(blockWin, thingGCs[NRAND(tris[hexagons - MIN_HEXAGONS].number
            [mixed][turn_style])], i, ROWS - j - 1);
        }
	XSync(display, True);
    }

}
							
/* ------------------------------------------------------------------ */

int
checkLines()
{
    int             lSet[ROWS], nset = 0;
    int             i, j, k, y;

    for (j = 0; j < ROWS; j++) {
	lSet[j] = 0;
	for (i = 0; i < COLS; i++)
	    if (field[j][i].pmid >= 0)
		lSet[j] ++;
	if (lSet[j] == COLS)
	    nset ++;
    }

    if (nset) {
	for (k = 0; k < ((NUM_FLASHES / nset) % 2) * 2; k++) {
	    for (j = 0; j < ROWS; j++) {
		if (lSet[j] == COLS)
  	            for (i = 0; i < COLS; i++)
		        drawHex(blockWin, xorGC, i, j);
	    }
	    XFlush(display);
	}

	for (j = ROWS-1; j >= 0; j--)
	    if (lSet[j] == COLS) {
		for (y = j; y > 0; y--)
		    for (i = 0; i < COLS; i++) {
			field[y][i] = field[y-1][i];
                        if (field[y][i].pmid == -1)
	                    drawHex(blockWin, revGC, i, y);
                        else
	                    drawHex(blockWin, thingGCs[field[y][i].cid], i, y);
                    }
		XFlush(display);
	    }

	if (beep) XBell(display, BVOLUME);
	XSync(display, False);
    }
    if (nset % 2) {
        clearScreen();
        invert = !invert;
        drawField();
        if (grid)
            drawGrid();
        XFlush(display);
    }
    return nset;
}

/* ------------------------------------------------------------------ */

void
drawGrid()
{
  GC gc;
  int i, j;

  if (grid)
    gc = tinyGC;
  else
    gc = revGC;
  for (j = 0; j < ROWS; j++)
    for (i = 0; i < COLS; i++)
      drawHexBorder (blockWin, gc, i, j);
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

static Bool 
inHexagon(size, i, j)
int size, i, j;
{
  int l = HEX_ROW_COL(size);

  if (j >= size)
    j = l - 1 - j;
  if (i >= size - !((size + j) % 2))
    i = l - 1 - i - !((size + j) % 2);
  return (2 * i + j >= size - 2);
}

/* ------------------------------------------------------------------ */

static void
readPolyhexes()
{
  int c, i, j, k, hx, polyhex, sum, start, n, size, height, corn,
    game, toss = 0;
  int counter[MAX_HEXAGONS - MIN_HEXAGONS + 1];
  int start_counter[MAX_HEXAGONS - MIN_HEXAGONS + 1][MAX_MODES][MAX_TYPES];
  FILE *fp;

  for (hx = 0; hx <= MAX_HEXAGONS - MIN_HEXAGONS; hx++)
  {
    counter[hx] = 0;
    for (polyhex = 0; polyhex < MAX_POLYHEXES; polyhex++)
      for (j = 0; j < HEX_ROW_COL(HEX_SIZE); j++)
        for (i = 0; i < HEX_ROW_COL(HEX_SIZE); i++)
          tris[hx].polyhex[polyhex].shape[j][i] = 0;
    for (j = 0; j < MAX_TYPES; j++)
    {
      for (polyhex = 0; polyhex < MAX_START_POLYHEXES; polyhex++)
      {
        tris[hx].mode[NOCORN].start[polyhex][j] = 0;
        tris[hx].mode[CORNERS].start[polyhex][j] = 0;
      }
      start_counter[hx][NOCORN][j] = 0;
      start_counter[hx][CORNERS][j] = 0;
      tris[hx].number[NOMIX][j] = 0;
      tris[hx].number[MIXED][j] = 0;
    }
    tris[hx].mode[NOCORN].turn_style = NONE;
    tris[hx].mode[CORNERS].turn_style = NONE;
  }
  if ((fp = fopen(POLYHEXFILE, "r")) == (FILE *)NULL)
  {
    (void) fprintf(stderr, "Can not open input file, %s.\n", POLYHEXFILE);
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
        hx = k - MIN_HEXAGONS;
        for (i = 0; i < MAX_TYPES; i++)
        {
          (void) fscanf(fp, "%d", &k);
          tris[hx].number[NOMIX][i] = k;
        }
        (void) fscanf(fp, "%d", &k);
        tris[hx].mode[NOCORN].turn_style = k;
        (void) fscanf(fp, "%d", &k);
        tris[hx].corners_switch = k;
        if (tris[hx].corners_switch == True)
        {
          (void) fscanf(fp, "%d", &k);
          tris[hx].mode[CORNERS].turn_style = k;
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
        hx = k - MIN_HEXAGONS;
        (void) fscanf(fp, "%d", &n);
        for (polyhex = 0; polyhex <= toss; polyhex++)
          while ((c = getc(fp)) != EOF && c != '\n');
        for (polyhex = 0; polyhex < n - toss; polyhex++)
        {
          sum = polyhex + counter[hx];
            /* This is only there to "read" input file */
          (void) fscanf(fp, "%d", &k);
          (void) fscanf(fp, "%d", &k);
          tris[hx].polyhex[sum].rotation =
            k + counter[hx] - toss;
          (void) fscanf(fp, "%d", &k);
          tris[hx].polyhex[sum].reflection =
            k + counter[hx] - toss;
          for (game = JUMPIN; game <= GRADUAL; game++)
          {
            (void) fscanf(fp, "%d", &height);
            if (!gradualAppear && game == JUMPIN)
              tris[hx].polyhex[sum].start_height = height;
            else if (gradualAppear && game == GRADUAL)
              tris[hx].polyhex[sum].start_height = height;
            (void) fscanf(fp, "%d", &start);
            for (corn = NOCORN; corn <= tris[hx].corners_switch; corn++)
            {
              if (game == JUMPIN)
              {
                if (hx == 0 ||
                    tris[hx - 1].mode[corn].turn_style == NONE)
                {
                  i = start_counter[hx][corn][NONE];
                  tris[hx].mode[corn].start[i][NONE] = sum;
                  start_counter[hx][corn][NONE]++;
                }
                if ((hx == 0 ||
                     tris[hx - 1].mode[corn].turn_style < ALL) &&
                    start != NONE)
                {
                  i = start_counter[hx][corn][NOREFL];
                  tris[hx].mode[corn].start[i][NOREFL] = sum;
                  start_counter[hx][corn][NOREFL]++;
                }
                if (start == ALL)
                {
                  i= start_counter[hx][corn][ALL];
                  tris[hx].mode[corn].start[i][ALL] = sum;
                  start_counter[hx][corn][ALL]++;
                }
              }
            }
          }
	  tris[hx].polyhex[sum].size = size;
          for (j = 0; j < HEX_ROW_COL(size); j++)
            for (i = 0; i < HEX_ROW_COL(size); i++)
              if (inHexagon(size, i, j))
              {
                (void) fscanf (fp, "%d", &k);
                tris[hx].polyhex[sum].shape[j][i] = k;
              }
        }
        counter[hx] += n - toss;
        toss = 0;
      }
    }
    (void) fclose(fp);
    for (i = 0; i <= MAX_HEXAGONS - MIN_HEXAGONS; i++)
      for (j = 0; j < MAX_TYPES; j++)
        for (k = 0; k <= i; k++)
          tris[i].number[MIXED][j] += tris[k].number[NOMIX][j];
  }
}
