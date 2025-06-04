#include <stdio.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "struct.h"

#ifdef VMS
#include <lib$routines.h>
unsigned long int statvms;
float seconds;
#endif

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
Pixmap mapw, mapb, omapw, omapb, wblock, bblock, owblock, obblock;
int oldw, oldb;
Cursor upc, downc, oupc, odownc;
Bool perfection;

int waitferkey();

msec_wait(millsecs)
int millsecs;
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = millsecs;
#ifndef VMS
	select(0, 0, 0, 0, &tv);
#else
      seconds = ((float) tv.tv_usec)/1000000.0;
      statvms = lib$wait(&seconds);
#endif
}

waitferkey(tm) /* returns nonzero on timeout, zero on key or click */
int tm;
{
  fd_set readbits, rb2;
  int sock1, sock2, sockh, tsec=0;
  long t1, t2;
  XEvent event;
  Bool ploop = True;
  char c;
  struct timeval timeout;

  timeout.tv_sec = (long) tm;
  timeout.tv_usec = 0;
  FD_ZERO(&readbits);
  sock1 = ConnectionNumber(dpy);
  if (botmode)
    sock2 = ConnectionNumber(odpy);
  if (sock1 > sock2)
    sockh = sock1;
  else
    sockh = sock2;
  if (!botmode)
    sockh = sock1;
  FD_SET(sock1, &readbits);
  if (botmode)
    FD_SET(sock2, &readbits);
  sockh++;
  rb2 = readbits;
 while (ploop) {
  readbits = rb2;
  timeout.tv_sec = ((long) tm - tsec);
  t1 = (long) time((long *) 0);
#ifndef VMS
  if (select (sockh, &readbits, NULL, NULL, &timeout)) {
#endif
  t2 = (long) time((long *) 0);
  tsec += (t2-t1);
  if(XCheckWindowEvent(dpy, win, ButtonPressMask | KeyPressMask | ButtonReleaseMask |
  	    ExposureMask, &event)) {
	switch(event.type) {
		case ButtonPress:
		break;
		case ButtonRelease:
			ploop = False;
		break;
		case Expose:
		break;
		case KeyPress:
			XLookupString(&event, &c, 1, NULL, NULL);
			if (c == 'q')
				quit();
			if (isalpha(c) || isspace(c))
			   ploop = False;
		break;
                default:
		break;
	}
  }
  if(botmode)
    if(XCheckWindowEvent(odpy, owin, ButtonPressMask | KeyPressMask | ButtonReleaseMask |
  	    ExposureMask, &event)) {
	switch(event.type) {
		case ButtonPress:
		break;
		case ButtonRelease:
			ploop = False;
		break;
		case Expose:
		break;
		case KeyPress:
			XLookupString(&event, &c, 1, NULL, NULL);
			if (c == 'q')
				quit();
			if (isalpha(c) || isspace(c))
			   ploop = False;
		break;
                default:
		break;
	}
   }
#ifndef VMS
  }
  else { /* we have timedout */
    return(1);
  }
#endif
 }
 return(0);
}

quit()
{
	XCloseDisplay(dpy);
	if (botmode)
		XCloseDisplay(odpy);
	exit(0);
}

draw_circle(x,y,color)
     int x,y,color;
{
  Pixmap wmap, bmap;

  if (currdpy == dpy) {
	wmap = mapw;
	bmap = mapb;
  }
  else {
	wmap = omapw;
	bmap = omapb;
  }

  if (color==WHITE)
    XCopyArea(currdpy, wmap, currbamp, currgc,0,0,X_PIECE_SIZE,
	     Y_PIECE_SIZE, x*X_PIECE_SIZE,y*Y_PIECE_SIZE);
  else
    XCopyArea(currdpy, bmap, currbamp, currgc,0,0,X_PIECE_SIZE,
	     Y_PIECE_SIZE, x*X_PIECE_SIZE,y*Y_PIECE_SIZE);
}

draw_obstacle(x,y)
     int x,y;
{
  Pixmap bbl, wbl;

  if (currdpy == dpy) {
	bbl = wblock;
	wbl = bblock;
  }
  else {
	bbl = owblock;
	wbl = obblock;
  }

  if (obstacle_color==WHITE)
    XCopyArea(currdpy, wbl, currbamp, currgc,0,0,X_PIECE_SIZE,
	     Y_PIECE_SIZE, x*X_PIECE_SIZE,y*Y_PIECE_SIZE);
  else
    XCopyArea(currdpy, bbl, currbamp, currgc,0,0,X_PIECE_SIZE,
	     Y_PIECE_SIZE, x*X_PIECE_SIZE,y*Y_PIECE_SIZE);
}

grid(cdpy, cmap, cgc)
Display *cdpy;
Pixmap cmap;
GC cgc;
{
  short x;

  XDrawRectangle(cdpy, cmap, cgc, 0, 0, xwin-1, ywin-1);
  for (x=0;x<return_x_size();x++)
	XDrawLine(cdpy, cmap, cgc, x*X_PIECE_SIZE, 0, x*X_PIECE_SIZE, ywin-1);
  for(x=0;x<return_y_size();x++)
	XDrawLine(cdpy, cmap, cgc, 0, x*Y_PIECE_SIZE, xwin-1, x*Y_PIECE_SIZE);
}

redraw_win(cdpy, cwin, cmap, cgc)
Display *cdpy;
Window cwin;
Pixmap cmap;
GC cgc;
{

  XCopyArea(cdpy, cmap, cwin, cgc, 0, 0, xwin, ywin, 0, 0);
  XFlush(cdpy);
}
