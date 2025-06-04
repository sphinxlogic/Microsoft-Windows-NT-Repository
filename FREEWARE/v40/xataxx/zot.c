#include <stdio.h>
#include <X11/Xlib.h>
#include <math.h>
#include "struct.h"

extern Display *dpy, *odpy, *currdpy;
extern Window win, owin, currwin;
extern GC gc, ogc, currgc;
extern int xsize, ysize, obstacle_color;
extern int botmode;
extern Pixmap mapw, mapb, omapw, omapb, wblock, bblock, owblock, obblock;
extern Bool flumping, wflump, fflump, jflump;

blat(sx, sy, dx, dy, cmaps, cdpy, cscr, cgc, cwin, cmap)
int sx, sy, dx, dy;
Pixmap cmaps;
Display *cdpy;
int cscr;
GC cgc;
Window cwin;
Pixmap cmap;
{
	GC bgc;

	if (!wflump)
		return;
	zot(dx, dy, cmaps, cdpy, cscr, cgc, cwin, cmap);
}

zot(dox, doy, cmaps, cdpy, cscr, cgc, cwin, cmap)
int dox, doy;
Pixmap cmaps;
Display *cdpy;
int cscr;
GC cgc;
Window cwin;
Pixmap cmap;
{
   int x1, x2, y1, y2, dx, dy, wid, hei;

   dx = dox * X_PIECE_SIZE;
   dy = doy * Y_PIECE_SIZE;
   x1 = dx + X_PIECE_SIZE / 2;
   x2 = x1+1;
   y1 = dy + Y_PIECE_SIZE / 2;
   y2 = x1+1;
   wid = x2 - x1;
   hei = y2 - y1;
   while (x1 > dx || y1 > dy || x2 < (dx+X_PIECE_SIZE-1) || y2 < (dy+Y_PIECE_SIZE-1)) {
	XCopyArea(cdpy, cmaps, cmap, cgc, (x1-dx), (y1-dy), wid, hei, x1, y1);
	if (x1 > dx) x1--;
	if (y1 > dy) y1--;
	if (x2 < (dx+X_PIECE_SIZE-1)) x2++;
	if (y2 < (dy+Y_PIECE_SIZE-1)) y2++;
	wid = x2 - x1;
	hei = y2 - y1;
	if (fflump)
	  msec_wait(2000);
	redraw_win(cdpy, cwin, cmap, cgc);
   }
}








