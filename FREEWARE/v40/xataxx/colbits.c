#include <stdio.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "struct.h"

#define TITLESIZE (7*X_PIECE_SIZE)

#ifndef FD_SET

#define MAXSELFD	64
typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask (power of 2!)*/
#define NFDSHIFT 5				/* Shift based on above */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif howmany

#define	NBBY	8		/* number of bits in a byte */


#define	FD_SETSIZE	64


#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))

#endif

extern Display *dpy, *odpy, *currdpy;
extern Window win, owin, currwin;
extern GC gc, ogc, currgc;
extern int xsize, ysize, obstacle_color, scr, oscr;
extern int botmode;
extern Pixmap bamp, obamp, currbamp;
extern int xwin,ywin;
extern Pixmap mapw, mapb, omapw, omapb, wblock, bblock, owblock, obblock;
extern int oldw, oldb;
extern Cursor upc, downc, oupc, odownc;
extern Bool perfection;

int waitferkey();

readbitmaps()
{
  unsigned int x, y, xh, yh;
  Pixmap curs, cmask, loader;
  XColor fg,bg,garbage;

  XSync(dpy, True);
  XReadBitmapFile(dpy, win, WHITESTONE, &x, &y, &loader, &xh, &yh);
  mapw = XCreatePixmap(dpy, win, x, y, DefaultDepth(dpy,scr));
  XCopyPlane(dpy, loader, mapw, gc, 0, 0, x, y, 0, 0, 1);
  XFreePixmap(dpy, loader);
  XReadBitmapFile(dpy, win, BLACKSTONE, &x, &y, &loader, &xh, &yh);
  mapb = XCreatePixmap(dpy, win, x, y, DefaultDepth(dpy,scr));
  XCopyPlane(dpy, loader, mapb, gc, 0, 0, x, y, 0, 0, 1);
  XFreePixmap(dpy, loader);
  XReadBitmapFile(dpy, win, BLACKBLOCK, &x, &y, &loader, &xh, &yh);
  bblock = XCreatePixmap(dpy, win, x, y, DefaultDepth(dpy,scr));
  XCopyPlane(dpy, loader, bblock, gc, 0, 0, x, y, 0, 0, 1);
  XFreePixmap(dpy, loader);
  XReadBitmapFile(dpy, win, WHITEBLOCK, &x, &y, &loader, &xh, &yh);
  wblock = XCreatePixmap(dpy, win, x, y, DefaultDepth(dpy,scr));
  XCopyPlane(dpy, loader, wblock, gc, 0, 0, x, y, 0, 0, 1);
  XFreePixmap(dpy, loader);

  fg.pixel = WhitePixel(dpy,scr);
  bg.pixel = BlackPixel(dpy,scr);
  XQueryColor(dpy, DefaultColormap(dpy,scr), &fg);
  XQueryColor(dpy, DefaultColormap(dpy,scr), &bg);
  XAllocNamedColor(dpy, DefaultColormap(dpy,scr), "lime green", &fg, &garbage);
  XAllocNamedColor(dpy, DefaultColormap(dpy,scr), "magenta", &bg, &garbage);

  XReadBitmapFile(dpy, win, UPCURSOR, &x, &y, &curs, &xh, &yh);
  XReadBitmapFile(dpy, win, UPMASK, &x, &y, &cmask, &xh, &yh);

  upc = XCreatePixmapCursor(dpy, curs, cmask, &fg, &bg, xh, yh);
  XFreePixmap(dpy,curs);
  XFreePixmap(dpy,cmask);

  XReadBitmapFile(dpy, win, DOWNCURSOR, &x, &y, &curs, &xh, &yh);
  XReadBitmapFile(dpy, win, DOWNMASK, &x, &y, &cmask, &xh, &yh);
  downc = XCreatePixmapCursor(dpy, curs, cmask, &fg, &bg, xh, yh);
  XFreePixmap(dpy,curs);
  XFreePixmap(dpy,cmask);
  XDefineCursor(dpy,win,upc);
  if (botmode) {
	XSync(odpy, True);
  	fg.pixel = WhitePixel(odpy,oscr);
  	bg.pixel = BlackPixel(odpy,oscr);
  	XQueryColor(odpy, DefaultColormap(odpy,oscr), &fg);
  	XQueryColor(odpy, DefaultColormap(odpy,oscr), &bg);
    XAllocNamedColor(odpy, DefaultColormap(odpy,oscr), "lime green", &fg, &garbage);
    XAllocNamedColor(odpy, DefaultColormap(odpy,oscr), "magenta", &bg, &garbage);
	XReadBitmapFile(odpy, owin, BLACKSTONE, &x, &y, &loader, &x, &y);
  	omapb = XCreatePixmap(odpy, owin, x, y, DefaultDepth(odpy,oscr));
  	XCopyPlane(odpy, loader, omapb, ogc, 0, 0, x, y, 0, 0, 1);
  	XFreePixmap(odpy, loader);
	XReadBitmapFile(odpy, owin, WHITESTONE, &x, &y, &loader, &x, &y);
  	omapw = XCreatePixmap(odpy, owin, x, y, DefaultDepth(odpy,oscr));
  	XCopyPlane(odpy, loader, omapw, ogc, 0, 0, x, y, 0, 0, 1);
  	XFreePixmap(odpy, loader);
  	XReadBitmapFile(odpy, owin, BLACKBLOCK, &x, &y, &loader, &x, &y);
  	obblock = XCreatePixmap(odpy, owin, x, y, DefaultDepth(odpy,oscr));
  	XCopyPlane(odpy, loader, obblock, ogc, 0, 0, x, y, 0, 0, 1);
  	XFreePixmap(odpy, loader);
  	XReadBitmapFile(odpy, owin, WHITEBLOCK, &x, &y, &loader, &x, &y);
  	owblock = XCreatePixmap(odpy, owin, x, y, DefaultDepth(odpy,oscr));
  	XCopyPlane(odpy, loader, owblock, ogc, 0, 0, x, y, 0, 0, 1);
  	XFreePixmap(odpy, loader);

  	XReadBitmapFile(odpy, owin, UPCURSOR, &x, &y, &curs, &xh, &yh);
        XReadBitmapFile(odpy, owin, UPMASK, &x, &y, &cmask, &x, &y);
  	oupc = XCreatePixmapCursor(odpy, curs, cmask, &fg, &bg, xh, yh);
  	XFreePixmap(odpy,curs);
        XFreePixmap(odpy,cmask);
  	XReadBitmapFile(odpy, owin, DOWNCURSOR, &x, &y, &curs, &xh, &yh);
        XReadBitmapFile(odpy, owin, DOWNMASK, &x, &y, &cmask, &x, &y);
  	odownc = XCreatePixmapCursor(odpy, curs, cmask, &fg, &bg, xh, yh);
	XFreePixmap(odpy,curs);
        XFreePixmap(odpy,cmask);
 	XDefineCursor(odpy,owin,oupc);
	XFlush(odpy);
  }
}

show_title()
{
  Pixmap title, otitle, loader;
  int ax, ay, x, y, hx, hy;

  XReadBitmapFile(dpy, win, TITLE, &x, &y, &loader, &hx, &hy);
  title = XCreatePixmap(dpy, win, x, y, DefaultDepth(dpy,scr));
  XCopyPlane(dpy, loader, title, gc, 0, 0, x, y, 0, 0, 1);
  XFreePixmap(dpy, loader);
  if (botmode)
    XReadBitmapFile(odpy, owin, TITLE, &x, &y, &otitle, &hx, &hy);
  ax = ((return_x_size()*X_PIECE_SIZE)-x) / 2;
  ay = ax;
  XCopyArea(dpy, title, win, gc, 0,0, x, y, ax, ay);
  if (botmode)
    XCopyArea(odpy, otitle, owin, ogc, 0,0, x, y, ax, ay);
  show_scores(dpy, win, gc, oldw, oldb);
  if (botmode)
    show_scores(odpy, owin, ogc, oldw, oldb);
}

















