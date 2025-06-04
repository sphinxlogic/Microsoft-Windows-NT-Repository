/*
# GENERIC X-BASED TERTRIS
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

#include	"tertris.h"

#include	"ticon.xbm"

Atom            delw;

static GC       revGC, bigGC, tinyGC, xorGC;
static GC       thingGCs[MAX_START_POLYIAMONDS];

static char    *winName = "GENERIC ALTERTRIS";
static char    *iconName = "ALTERTRIS";

static int      titleLen, titleWidth, authorLen, authorWidth;
static int      titleX, titleY, authorX, authorY;
static int      sX;
static int      sLevelY, sRowsY, sScoreY;

static int      topRWidth, topWidth, topHeight, topMidX, topMidY;
static int      frameX, frameY, frameW, frameH;

typedef struct {
    int             triangles, polyiamond_number;
    int             xpos, ypos;
    int             size, color_number, direction;
    int             bonus, random_rotation, random_reflection;
    long            random_number;
}               thing_t;
typedef struct {
    int             rotation;
    int             reflection;
    int             start_height;
    int             shape[HEX_ROW(HEX_SIZE)][HEX_COL(HEX_SIZE)];
    int             size;
}               Polyiamonds;
typedef struct {
    int             number[MAX_MODES][MAX_TYPES];
    int             start[MAX_START_POLYIAMONDS][MAX_TYPES];
    int             turn_style;
}               Mode;
typedef struct {
    Polyiamonds     polyiamond[MAX_POLYIAMONDS];
    Mode            mode[MAX_MODES];
    int             diagonal_switch;
}               Polytris;
static Polytris tris[MAX_TRIANGLES - MIN_TRIANGLES + 1];

static char    *thingFGs[MAX_START_POLYIAMONDS] = {
    "Red", "Green", "Blue", "Yellow", 
    "Magenta", "Cyan", "FireBrick", "GreenYellow",
    "SlateBlue", "Khaki", "Plum", "Violet",
    "DarkTurquoise", "Gold", "Orchid", "Turquoise",
    "Orange", "OrangeRed", "VioletRed", "BlueViolet",
    "SeaGreen", "Pink", "ForestGreen", "SkyBlue",
    "Coral", "Wheat", "Goldenrod", "IndianRed",
    "SpringGreen", "CornflowerBlue", "Thistle", "Aquamarine",
    "CadetBlue", "LightSteelBlue", "NavyBlue", "SteelBlue",
    "YellowGreen", "DarkViolet", "MediumSeaGreen", "DarkSlateGray",
    "LightGray", "MediumVioletRed", "Sienna", "MediumAquamarine",
    "MediumBlue", "Navy", "DarkOliveGreen", "DarkGreen",
    "DimGray", "Tan", "MediumTurquoise", "DarkSlateBlue",
    "Maroon", "MediumSlateBlue", "PaleGoldenrod", "MediumSpringGreen",
    "DarkOrchid", "LightBlue", "MidnightBlue", "LimeGreen",
    "PaleGreen", "DarkGoldenrod", "DarkSeaGreen", "MediumOrchid",
    "Salmon", "Brown", "SandyBrown", "SaddleBrown",
    "RoyalBlue", "RosyBrown", "PowderBlue", "PeachPuff",
    "PaleTurquoise", "PaleGoldenrod", "OliveDrab", "LawnGreen",
    "HotPink", "DodgerBlue", "DeepSkyBlue", "DeepPink",
    "DarkSalmon", "DarkOrange", "DarkKhaki", "AliceBlue",
    "MediumPurple", "bisque", "burlywood", "chartreuse",
    "chocolate", "moccasin", "peru", "purple",
    "tomato", "LightYellow4", "LightSalmon4", "LavenderBlush4",
    "LightSkyBlue4", "MistyRose4", "NavajoWhite4", "MediumOrchid4",
    "RosyBrown4", "VioletRed4", "PeachPuff4", "RoyalBlue4",
    "SeaGreen4", "OliveDrab4", "PaleTurquoise4", "PaleVioletRed4",
    "LightPink4", "DeepPink4", "DarkGoldenrod4", "DarkOliveGreen4",
    "Gray", "Black"
};

static thing_t  curThing, nextThing;
static struct {
    int         pmid, cid;
}               field[ROWS][COLS];

static XPoint triangle_unit[MAX_ORIENT][3] = {
 { {0, 0}, {-1, -1}, { 2,  0} },
 { {0, 0}, { 1,  1}, {-2,  0} }
};
static XPoint   triangle_list[MAX_ORIENT][3];
static XPoint   triangle_border[MAX_ORIENT][4];

static Bool     overlapped();
static Bool     collision();
static void     sizeTris();
static Bool     inHexagon();
static void     readPolyiamonds();

/* ------------------------------------------------------------------ */

static unsigned long
getColor(name)
    char            name[];
{
    XColor          tmp;

    if (!useColor)
	return fg;
    if (XParseColor(display, colormap, name, &tmp) == 0) {
	(void) fprintf(stderr, "Tertris: invalid color '%s'.\n", name);
	return fg;
    }
    if (XAllocColor(display, colormap, &tmp) == 0) {
	(void) fprintf(stderr, "Tertris: can't allocate color '%s'.\n", name);
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
    int             i, j, col, turn_style, number_polyiamonds; 

    (void) SRAND(time(NULL));
    readPolyiamonds();

    turn_style = tris[triangles - MIN_TRIANGLES].mode[diagonal].turn_style;
    if (turn_style < 0 || turn_style >= MAX_TYPES)
    {
      (void) fprintf(stderr, "Tertris: corrupted input file %s\n",
               POLYIAMONDFILE);
      (void) fprintf(stderr, "\tturn_style = %d.\n", turn_style);
      exit(1);
    }
    number_polyiamonds = tris[triangles - MIN_TRIANGLES].mode
      [diagonal].number[mixed][turn_style];
    if (number_polyiamonds <= 0 || number_polyiamonds >= MAX_START_POLYIAMONDS)
    {
      (void) fprintf(stderr, "Tertris: corrupted input file %s\n",
               POLYIAMONDFILE);
      (void) fprintf(stderr, "\tnumber_polyominoes = %d.\n",
               number_polyiamonds);
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
                field[j][i].cid = NRAND(number_polyiamonds);
            } else {
                field[j][i].pmid = -1;
                field[j][i].cid = 0;
            }
    else
      /* Second Algorithm randomly picks TRIANGLES_PER_ROW boxes per row 
         This is more in keeping with the original */
      for (j = 0; j < ROWS; j++) {
         for (i = 0; i < COLS; i++) {
                field[j][i].pmid = -1;
                field[j][i].cid = 0;
	 }
	 if (j >= ROWS - prefilled)
           for (i = 0; i < TRIANGLES_PER_ROW; i++) {
                do
                    col = NRAND(COLS);
                while (field[j][col].pmid != -1);
                field[j][col].pmid = 0;
                field[j][col].cid = NRAND(number_polyiamonds);
	   }
      }

    titleLen = strlen(MSG_TITLE);
    titleWidth = XTextWidth(bigFont, MSG_TITLE, titleLen);
    authorLen = strlen(MSG_AUTHOR);
    authorWidth = XTextWidth(tinyFont, MSG_AUTHOR, authorLen);

    frameW = GAME_WIDTH - 2;
    frameH = GAME_HEIGHT - 2;
    topRWidth = TRIWIDTH * MAX_TRIANGLES / 2 + OFFSET * 2;
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

    sizeTris();

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
      /* DOWN key to rotation */
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
    classhints.res_name = "tertris";
    classhints.res_class = "Tertris";

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
    for (i = 0; i < MAX_START_POLYIAMONDS; i++) {
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
sizeTris()
{
  int i, j;
  for (j = 0; j < MAX_ORIENT; j++)
  {
    for (i = 0; i < 3; i++)
    { 
      triangle_list[j][i].x = (TRIWIDTH - DELTAX) / 2 * triangle_unit[j][i].x;
      triangle_list[j][i].y = (TRIHEIGHT - DELTAY) *
        triangle_unit[j][i].y;      
      triangle_border[j][i].x = TRIWIDTH / 2 * triangle_unit[j][i].x;
      triangle_border[j][i].y = TRIHEIGHT * triangle_unit[j][i].y;  
    }
    triangle_border[j][3].x = triangle_border[j][1].x;
    triangle_border[j][3].y = -triangle_border[j][1].y;
  }
}

/* ------------------------------------------------------------------ */

static void
drawTri(win, gc, i, j)
    Window          win;
    GC              gc;
    int             i, j;
{
    int orient = (i + j + !invert) % MAX_ORIENT;

    triangle_list[orient][0].x = (i + 1) * (TRIWIDTH / 2) + 1;
    triangle_list[orient][0].y = j * TRIHEIGHT + DELTAY +
        ((orient == UP) ? 0 : TRIHEIGHT - DELTAY);
    XFillPolygon(display, win, gc, triangle_list[orient], 3,
        Convex, CoordModePrevious);
}

/* ------------------------------------------------------------------ */

static void
drawTriBorder(gc, i, j)
    GC              gc;
    int             i, j;
{
    int orient = (i + j + !invert) % MAX_ORIENT;

    triangle_border[orient][0].x = (i + 1) * (TRIWIDTH / 2) + 1;
    triangle_border[orient][0].y = j * TRIHEIGHT + 1 +
      ((orient == UP) ? 0 : TRIHEIGHT);
    XDrawLines(display, blockWin, gc,
      triangle_border[orient], 4, CoordModePrevious);
}

/* ------------------------------------------------------------------ */

void
drawField()
{
    int             i, j;

    for (i = 0; i < COLS; i++)
	for (j = 0; j < ROWS; j++)
	    if (field[j][i].pmid >= 0)
		drawTri(blockWin, thingGCs[field[j][i].cid], i, j);
}

/* ------------------------------------------------------------------ */

void
drawThing()
{
    int             i, j;
    
    for (i = 0; i < HEX_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW(curThing.size); j++)
        if (tris[curThing.triangles].polyiamond
                [curThing.polyiamond_number].shape[j][i])
	  drawTri(blockWin, thingGCs[curThing.color_number],
	    curThing.xpos + i, curThing.ypos + j);
}

/* ------------------------------------------------------------------ */

static void
drawThingDiff(old)
    thing_t        *old;
{
    int             i, j, ox, oy;
    int             sx = HEX_COL(curThing.size), sy = HEX_ROW(curThing.size);

    for (i = 0; i < sx; i++)
      for (j = 0; j < sy; j++)
        if ((curThing.ypos + j >= 0) && tris[curThing.triangles].polyiamond
              [curThing.polyiamond_number].shape[j][i])
	  drawTri(blockWin, thingGCs[curThing.color_number],
            curThing.xpos + i, curThing.ypos + j);

    for (i = 0; i < sx; i++)
      for (j = 0; j < sy; j++)
      {
        ox = old->xpos + i - curThing.xpos;
        oy = old->ypos + j - curThing.ypos;
	if (tris[old->triangles].polyiamond
                [old->polyiamond_number].shape[j][i] &&
	    ((ox < 0) || (ox >= sx) || (oy < 0) || (oy >= sy) ||
	     !tris[curThing.triangles].polyiamond
                  [curThing.polyiamond_number].shape[oy][ox]))
              drawTri(blockWin, revGC, old->xpos + i, old->ypos + j);
      }
}

/* ------------------------------------------------------------------ */

void
drawNext()
{
    int             x, y;
    int             i, j, inv = (nextThing.size - 1) % 2;
    int             wid = HEX_COL(nextThing.size);
    int             ht = HEX_ROW(nextThing.size);

    x = topMidX + (topRWidth - (wid + 1) * TRIWIDTH / 2) / 2 + 1;
    y = topMidY - ht * TRIHEIGHT / 2 + DELTAY;
    for (i = 0; i < wid; i++)
      for (j = 0; j < ht; j++)
	if (tris[nextThing.triangles].polyiamond
                [nextThing.polyiamond_number].shape[j][i]) {
          int orient = (i + j + !inv) % MAX_ORIENT;
          triangle_list[orient][0].x = x + (i + 1) * (TRIWIDTH / 2);
          triangle_list[orient][0].y = y + j * TRIHEIGHT +
            ((orient == UP) ? 0 : TRIHEIGHT - DELTAY);
          XFillPolygon(display, mainWin, thingGCs[nextThing.color_number],
            triangle_list[orient], 3, Convex, CoordModePrevious);
        }
}

/* ------------------------------------------------------------------ */

void
clearNext()
{
    XFillRectangle(display, mainWin, revGC,
	topMidX, topMidY - TRIHEIGHT * HEX_ROW(HEX_SIZE) / 2,
	topRWidth, TRIHEIGHT * HEX_ROW(HEX_SIZE));
}

/* ------------------------------------------------------------------ */

void
redoNext()
{
    int turn_style = tris[triangles - MIN_TRIANGLES].mode[diagonal].turn_style;
    int next_start, i;

    nextThing.triangles = triangles - MIN_TRIANGLES + nextThing.bonus;
    next_start = nextThing.random_number % tris[nextThing.triangles].
      mode[diagonal].number[(nextThing.bonus) ? NOMIX : mixed][turn_style];
    nextThing.color_number = next_start;

    if (mixed && !nextThing.bonus)
    {
      nextThing.triangles = 0;
      while (next_start >=
             tris[nextThing.triangles].mode[diagonal].number[NOMIX][turn_style])
      {
        next_start -=
          tris[nextThing.triangles].mode[diagonal].number[NOMIX][turn_style];
        nextThing.triangles++;
      }
    }
    nextThing.polyiamond_number =
      tris[nextThing.triangles].mode[diagonal].start[next_start][turn_style];
    if (tris[nextThing.triangles].mode[diagonal].turn_style > NONE)
      for (i = 0; i < nextThing.random_rotation; i++)
        nextThing.polyiamond_number = tris[nextThing.triangles].polyiamond
          [nextThing.polyiamond_number].rotation;
    if (tris[nextThing.triangles].mode[diagonal].turn_style == ALL)
      for (i = 0; i < nextThing.random_reflection; i++)
        nextThing.polyiamond_number = tris[nextThing.triangles].polyiamond
          [nextThing.polyiamond_number].reflection;
    nextThing.size = tris[nextThing.triangles].polyiamond
                         [nextThing.polyiamond_number].size;
    /* nextThing.xpos = NRAND(COLS - nextThing.size + 1); */
    nextThing.ypos = -tris[nextThing.triangles].polyiamond
                          [nextThing.polyiamond_number].start_height;
    nextThing.xpos = (COLS - nextThing.triangles) / 2;
    if ((nextThing.xpos - nextThing.ypos + !invert + nextThing.size) % 2)
      nextThing.xpos++;
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
putTri()
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;
    for (i = 0; i < HEX_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW(curThing.size); j++)
        if ((y + j >= 0) && tris[curThing.triangles].polyiamond
              [curThing.polyiamond_number].shape[j][i]) {
          field[y + j][x + i].pmid = tris[curThing.triangles].polyiamond
            [curThing.polyiamond_number].shape[j][i];
	  field[y + j][x + i].cid = curThing.color_number;
	}
}

/* ------------------------------------------------------------------ */

Bool
overlapping()
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;
    for (i = 0; i < HEX_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW(curThing.size); j++)
	if (tris[curThing.triangles].polyiamond
                [curThing.polyiamond_number].shape[j][i]) {
          if ((y + j >= ROWS) || (x + i < 0) || (x + i >= COLS))
            return True;
	  if (gradualAppear) {
/* This method one can turn a polyiamond to an area above of screen, also
   part of the polyiamond may not be visible initially */          
            if ((y + j >= 0) && (field[y + j][x + i].pmid >= 0))
              return True;
          } else {
/* This method does not allow a turning polyiamond to an area above screen */
            if ((y + j < 0) || (field[y + j][x + i].pmid >= 0))
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
    int             i, j;
    int             oldx = curThing.xpos - (!fall + 1) * dir;
    int             oldy = curThing.ypos - fall;

    for (i = 0; i < HEX_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW(curThing.size); j++)
	if ((curThing.ypos + j >= 0) && tris[curThing.triangles].polyiamond
              [curThing.polyiamond_number].shape[j][i])
        {
          if ((oldx + i + oldy + j + invert) % 2) {
            if ((oldy + j >= 0) && (field[oldy + j][oldx + i + dir].pmid >= 0))
              return True;
          } else {
            if (field[oldy + j + fall][oldx + i + !fall * dir].pmid >= 0)
              return True;
          }
        }
    return False;
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

static Bool
collision(dir)
int dir;
{
    int             i, j;
    int             x = curThing.xpos, y = curThing.ypos;

    for (i = 0; i < HEX_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW(curThing.size); j++)
	if ((y + j >= -1) && tris[curThing.triangles].polyiamond
              [curThing.polyiamond_number].shape[j][i])
	  if ((y + j >= ROWS - 1) || (x + i + dir < 0) ||
              (x + i + dir >= COLS) ||
	      (field[y + j + 1][x + i + dir].pmid >= 0))
	    return True;
    for (i = 0; i < HEX_COL(curThing.size); i++)
      for (j = 0; j < HEX_ROW(curThing.size); j++)
	if ((y + j >= -1) && (tris[curThing.triangles].polyiamond
              [curThing.polyiamond_number].shape[j][i])) {
          if ((x + i + y + j + invert) % 2) {
            if ((y + j >= 0) && (field[y + j][x + i + dir].pmid >= 0))
              return True;
          } else {
            if (field[y + j + 1][x + i].pmid >= 0)
              return True;
          }
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
	curThing.xpos += curThing.direction;
        if (overlapping() || overlapped(curThing.direction, True)) {
            curThing = old;
            curThing.direction = -curThing.direction;
            curThing.ypos ++;
	    curThing.xpos += curThing.direction;
        }
        if (overlapping() || overlapped(curThing.direction, True))
	    curThing = old;
	else
 	    drawThingDiff(&old);
	break;

    case DROP:
	do {
            do {
                mid = curThing;
	        curThing.ypos ++;
	        curThing.xpos += curThing.direction;
	        score += level + prefilled;
            } while (!overlapping() && !overlapped(curThing.direction, True));
            curThing = mid;
            curThing.direction = -curThing.direction;
            curThing.ypos ++;
	    curThing.xpos += curThing.direction;
	} while (!overlapping() && !overlapped(curThing.direction, True));
	curThing = mid;
	drawThingDiff(&old);
	break;

    case LEFT:
	curThing.xpos -= 2;
	if (!overlapping() && !overlapped(-1, False))
	    drawThingDiff(&old);
	else
	    curThing = old;
	break;

    case RIGHT:
	curThing.xpos += 2;
	if (!overlapping() && !overlapped(1, False))
	    drawThingDiff(&old);
	else
	    curThing = old;
	break;

    case ROTATE:
        if (tris[triangles - MIN_TRIANGLES].mode[diagonal].turn_style > NONE)
        {
          if (cw)
            for (i = 0; i < MAX_SIDES - 1; i++)
              curThing.polyiamond_number = tris[curThing.triangles].polyiamond
                [curThing.polyiamond_number].rotation;
          else /* ccw */
            curThing.polyiamond_number = tris[curThing.triangles].polyiamond
              [curThing.polyiamond_number].rotation;
	  curThing.xpos = old.xpos;
	  curThing.ypos = old.ypos;
	}
	if (!overlapping())
	    drawThingDiff(&old);
	else
	    curThing = old;
	break;

    case REFLECT: /* reflect on y axis */
        if (tris[triangles - MIN_TRIANGLES].mode[diagonal].turn_style == ALL)
        {
          curThing.polyiamond_number = tris[curThing.triangles].polyiamond
            [curThing.polyiamond_number].reflection;
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
    int turn_style = tris[triangles - MIN_TRIANGLES].mode[diagonal].turn_style;
    
    XSync(display, True);
    for (j = 0; j <= ROWS / 2; j++) {
      for (i = 0; i < COLS; i++) {
	drawTri(blockWin, thingGCs[NRAND(tris[triangles - MIN_TRIANGLES].mode
            [diagonal].number[mixed][turn_style])], i, j);
	drawTri(blockWin, thingGCs[NRAND(tris[triangles - MIN_TRIANGLES].mode
            [diagonal].number[mixed][turn_style])], i, ROWS - j - 1);
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
		        drawTri(blockWin, xorGC, i, j);
	    }
	    XFlush(display);
	}

	for (j = ROWS-1; j >= 0; j--)
	    if (lSet[j] == COLS) {
		for (y = j; y > 0; y--)
		    for (i = 0; i < COLS; i++) {
			field[y][i] = field[y-1][i];
                        if (field[y][i].pmid == -1)
	                    drawTri(blockWin, revGC, i, y);
                        else
	                    drawTri(blockWin, thingGCs[field[y][i].cid], i, y);
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
        nextThing.xpos = (COLS - nextThing.triangles) / 2;
        if ((nextThing.xpos - nextThing.ypos + !invert + nextThing.size) % 2)
            nextThing.xpos++;
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
      drawTriBorder(gc, i, j);
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
  int w = HEX_COL(size), h = HEX_ROW(size);

  if (i > 2 * size - 1)
    i = w - 1 - i;
  if (j >= size)
    j = h - 1 - j;
  return (i + j >= size - 1);
}

/* ------------------------------------------------------------------ */

static void
readPolyiamonds()
{
  int c, i, j, k, tr, polyiamond, sum, start, n, size, height, diag,
    game, toss = 0;
  int counter[MAX_TRIANGLES - MIN_TRIANGLES + 1];
  int start_counter[MAX_TRIANGLES - MIN_TRIANGLES + 1][MAX_MODES][MAX_TYPES];
  FILE *fp;

  for (tr = 0; tr <= MAX_TRIANGLES - MIN_TRIANGLES; tr++)
  {
    counter[tr] = 0;
    for (polyiamond = 0; polyiamond < MAX_POLYIAMONDS; polyiamond++)
      for (j = 0; j < HEX_ROW(HEX_SIZE); j++)
        for (i = 0; i < HEX_COL(HEX_SIZE); i++)
          tris[tr].polyiamond[polyiamond].shape[j][i] = 0;
    for (j = 0; j < MAX_TYPES; j++)
    {
      for (polyiamond = 0; polyiamond < MAX_START_POLYIAMONDS; polyiamond++)
      {
        tris[tr].mode[NODIAG].start[polyiamond][j] = 0;
        tris[tr].mode[DIAGONAL].start[polyiamond][j] = 0;
      }
      start_counter[tr][NODIAG][j] = 0;
      start_counter[tr][DIAGONAL][j] = 0;
      tris[tr].mode[NODIAG].number[NOMIX][j] = 0;
      tris[tr].mode[DIAGONAL].number[NOMIX][j] = 0;
      tris[tr].mode[NODIAG].number[MIXED][j] = 0;
      tris[tr].mode[DIAGONAL].number[MIXED][j] = 0;
    }
    tris[tr].mode[NODIAG].turn_style = NONE;
    tris[tr].mode[DIAGONAL].turn_style = NONE;
  }
  if ((fp = fopen(POLYIAMONDFILE, "r")) == (FILE *)NULL)
  {
    (void) fprintf(stderr, "Can not open input file, %s.\n", POLYIAMONDFILE);
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
        tr = k - MIN_TRIANGLES;
        for (i = 0; i < MAX_TYPES; i++)
        {
          (void) fscanf(fp, "%d", &k);
          tris[tr].mode[NODIAG].number[NOMIX][i] = k;
        }
        (void) fscanf(fp, "%d", &k);
        tris[tr].mode[NODIAG].turn_style = k;
        (void) fscanf(fp, "%d", &k);
        tris[tr].diagonal_switch = k;
        if (tris[tr].diagonal_switch == True)
        {
          for (i = 0; i < MAX_TYPES; i++)
          {
            (void) fscanf(fp, "%d", &k);
            tris[tr].mode[DIAGONAL].number[NOMIX][i] = k;
          }
          (void) fscanf(fp, "%d", &k);
          tris[tr].mode[DIAGONAL].turn_style = k;
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
        tr = k - MIN_TRIANGLES;
        (void) fscanf(fp, "%d", &n);
        if (tris[tr].diagonal_switch == True)
          (void) fscanf(fp, "%d", &n);
        for (polyiamond = 0; polyiamond <= toss; polyiamond++)
          while ((c = getc(fp)) != EOF && c != '\n');
        for (polyiamond = 0; polyiamond < n - toss; polyiamond++)
        {
          sum = polyiamond + counter[tr];
            /* This is only there to "read" input file */
          (void) fscanf(fp, "%d", &k);
          (void) fscanf(fp, "%d", &k);
          tris[tr].polyiamond[sum].rotation =
            k + counter[tr] - toss;
          (void) fscanf(fp, "%d", &k);
          tris[tr].polyiamond[sum].reflection =
            k + counter[tr] - toss;
          for (game = JUMPIN; game <= GRADUAL; game++)
          {
            (void) fscanf(fp, "%d", &height);
            if (!gradualAppear && game == JUMPIN)
              tris[tr].polyiamond[sum].start_height = height;
            else if (gradualAppear && game == GRADUAL)
              tris[tr].polyiamond[sum].start_height = height;
            for (diag = NODIAG; diag <= tris[tr].diagonal_switch; diag++)
            {
              (void) fscanf(fp, "%d", &start);
              if (game == JUMPIN)
              {
                if (tr == 0 ||
                    tris[tr - 1].mode[diag].turn_style == NONE)
                {
                  i = start_counter[tr][diag][NONE];
                  tris[tr].mode[diag].start[i][NONE] = sum;
                  start_counter[tr][diag][NONE]++;
                }
                if ((tr == 0 ||
                     tris[tr - 1].mode[diag].turn_style < ALL) &&
                    start != NONE)
                {
                  i = start_counter[tr][diag][NOREFL];
                  tris[tr].mode[diag].start[i][NOREFL] = sum;
                  start_counter[tr][diag][NOREFL]++;
                }
                if (start == ALL)
                {
                  i= start_counter[tr][diag][ALL];
                  tris[tr].mode[diag].start[i][ALL] = sum;
                  start_counter[tr][diag][ALL]++;
                }
              }
            }
          }
	  tris[tr].polyiamond[sum].size = size;
          for (j = 0; j < HEX_ROW(size); j++)
            for (i = 0; i < HEX_COL(size); i++)
              if (inHexagon(size, i, j))
              {
                (void) fscanf (fp, "%d", &k);
                tris[tr].polyiamond[sum].shape[j][i] = k;
              }
        }
        counter[tr] += n - toss;
        toss = 0;
      }
    }
    (void) fclose(fp);
    for (i = 0; i <= MAX_TRIANGLES - MIN_TRIANGLES; i++)
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
