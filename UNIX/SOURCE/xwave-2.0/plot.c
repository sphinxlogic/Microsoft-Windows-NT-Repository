/* plot.c - line drawing and other X functions for xwave.
 * Code here mainly written by Friedman and Riddle,
 * separated into functions by jimmc. Feb '91
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include "xwave.h"

#define   STARTX     -50
#define   STARTY     50
#define   STARTYVOFF 200
#define   WIDTH      400
#define   HEIGHT     500

extern double sqrt();

extern int flagstereo;
extern int flagroot;
extern int flagplotvel;
extern int flagedit;

extern int *editedRowVals;
extern int *editedColVals;
extern int *frequencies;
extern int *amplitudes;
extern int running;


Display	*dpy;
int screen;
Window	win;

Colormap cmap;
XColor red, blue, purple;

GC	gc,gc2;
int	scrnWidth = WIDTH, scrnHeight = HEIGHT;
Pixmap  buffer;

int   FD = 25, PD = 15;
int gap = 10;
int px= 0, py= 0;

int numEditedPoints=0;

/* This sets the window up. */
void InitWindow ()
{
	if (!(dpy = XOpenDisplay (""))) {
		(void) fprintf (stderr, "Error:  Can't open display\n");
		exit (1);
	}
	screen = DefaultScreen (dpy);
	if (flagroot) {
		win = DefaultRootWindow (dpy);
		scrnWidth = DisplayWidth (dpy, screen);
		scrnHeight = DisplayHeight (dpy, screen);
	}
	else {
		win = XCreateSimpleWindow (dpy, DefaultRootWindow (dpy),
		    0, 0, scrnWidth, scrnHeight, 1,
		    WhitePixel (dpy, screen),
		    BlackPixel (dpy, screen));
		XSelectInput (dpy, win, ExposureMask | StructureNotifyMask |
				KeyPressMask);
		XStoreName (dpy, win, "XWave");
		XMapWindow (dpy, win);
	}
	gc = XCreateGC (dpy, win, 0L, (XGCValues *) 0);
        gc2= XCreateGC (dpy, win, 0L, (XGCValues *) 0);
        XSetLineAttributes (dpy,gc2,20,LineSolid,CapButt,JoinMiter);
        XSetForeground (dpy, gc2, WhitePixel (dpy, screen));

	if (flagstereo)
		InitColor ();
	buffer = XCreatePixmap (dpy, DefaultRootWindow (dpy), scrnWidth,
	    scrnHeight, DefaultDepth (dpy, screen));
}

/* This loads the red, blue and purple colors into the color map. */
InitColor ()
{
	cmap = DefaultColormap (dpy, screen);
	blue.red   = 0;
	blue.blue  = 60000;
	blue.green = 0;

	red.red   = 40000;
	red.blue  = 0;
	red.green = 0;

        purple.red   = 40000;
        purple.blue  = 60000;
        purple.green = 0;


	if (!(XAllocColor (dpy, cmap, &red)) ||
	    !(XAllocColor (dpy, cmap, &blue)) ||
            !(XAllocColor (dpy, cmap, &purple))) {
		(void) fprintf (stderr, "Error:  Cannot allocate colors\n");
		exit (1);
	}
}

WaitForExpose()
{
	/*
         * Grab an expose event from the server, then we can get started
         */
	for (;;) {
		XEvent event;
		XNextEvent(dpy, &event);
		if (event.type == Expose) break;
	}
#if 0
	XWarpPointer (dpy, None, win, 0, 0, 0, 0, scrnWidth / 2,
	    scrnHeight / 2);
#endif
}

/*
 * Perspective projections of points in 3-space to screen coordinates.
 * These are implemented as macros to avoid function call overhead without
 * (well, almost without..) sacrificing readability of the code.
 */

#define xrpers(x,y,z) ((int) (PD - FD * (PD - (x)) / (FD - (z)) + 100))
#define yrpers(x,y,z) ((int) (FD * (y) / (FD - (z)) + 150))
#define xlpers(x,y,z) ((int) (-PD - FD * (-PD - (x)) / (FD - (z)) + 100))
#define ylpers(x,y,z) ((int) (FD * (y) / (FD - (z)) + 150))

/*
 * Line plotting routines.  Again, these are all macros.
 */

#define plotLineRed(x0,y0,z0,x1,y1,z1) \
   XSetForeground (dpy, gc, red.pixel); \
   XDrawLine (dpy, buffer, gc, \
	      xrpers ((float) (x0), (float) (y0), (float) (z0)), \
	      yrpers ((float) (x0), (float) (y0), (float) (z0)), \
	      xrpers ((float) (x1), (float) (y1), (float) (z1)), \
	      yrpers ((float) (x1), (float) (y1), (float) (z1)))

#define plotLineBlue(x0,y0,z0,x1,y1,z1) \
   XSetForeground (dpy, gc, blue.pixel); \
   XDrawLine (dpy, buffer, gc, \
	      xlpers ((float) (x0), (float) (y0), (float) (z0)), \
	      ylpers ((float) (x0), (float) (y0), (float) (z0)), \
	      xlpers ((float) (x1), (float) (y1), (float) (z1)), \
	      ylpers ((float) (x1), (float) (y1), (float) (z1)))

#define plotLine(x0,y0,z0,x1,y1,z1) \
   if (flagstereo) { \
      plotLineRed((x0),(y0),(z0),(x1),(y1),(z1)); \
      plotLineBlue((x0),(y0),(z0),(x1),(y1),(z1)); \
   } else {\
      XSetForeground (dpy, gc, WhitePixel (dpy, screen)); \
      XDrawLine (dpy, buffer, gc, \
		 xlpers ((float) (x0), (float) (y0), (float) (z0)), \
		 ylpers ((float) (x0), (float) (y0), (float) (z0)), \
		 xlpers ((float) (x1), (float) (y1), (float) (z1)), \
		 ylpers ((float) (x1), (float) (y1), (float) (z1))); \
   }

/* Plot the current generation in our background pixmap */
PlotCur(which)
int which;		/* 0 means plot position, 1 means plot vel */
{
	int r, c;
	struct pinfo *pi, *pir;

	/* get transformed values for all points in grid */
	if (which) {	/* velocity */
		/* velocity stuff added by jimmc */
		int kscale = (int)(SCALE*sqrt(springconst));
			/* low spring constant gives smaller velocities, so
			 * scale up for smaller spring constansts */
		int startyv = STARTY + STARTYVOFF;
		for (r = 0; r < maxrows; r++) {
			for (c = 0; c < maxcols; c++) {
				pi = CurGen+IX(r,c);
				pi->tx = gap * c + STARTX + px;
				pi->ty = pi->vel/kscale + gap * r + startyv+ py;
			}
		}
	} else {	/* position */
		for (r = 0; r < maxrows; r++) {
			for (c = 0; c < maxcols; c++) {
				pi = CurGen+IX(r,c);
				pi->tx = gap * c + STARTX + px;
				pi->ty = pi->pos/SCALE + gap * r + STARTY + py;
			}
		}
	}

	/* now plot all the line segments */
	for (r = 0; r < maxrows - 1; r++) {
		for (c = 0; c < maxcols - 1; c++) {
			pi = CurGen+IX(r,c);
			pir = pi+maxcols;	/* access CurGen[IX(r+1,c)] */
			/* pi[1] accesses CurGen[IX(r,c+1)] */
			plotLine (pi->tx, pi->ty, r, pi[1].tx, pi[1].ty, r);
			plotLine (pi->tx, pi->ty, r, pir->tx, pir->ty, r+1);
		}
		/* do the last column (c==maxcols-1) */
		pi = CurGen+IX(r,c);
		pir = pi+maxcols;	/* access CurGen[IX(r+1,c)] */
		plotLine (pi->tx, pi->ty, r, pir->tx, pir->ty, r+1);

	}
	/* now do the last row (r==maxrows-1) */
	for (c = 0; c < maxcols - 1; c++) {
		pi = CurGen+IX(r,c);
		/* pi[1] accesses CurGen[IX(r,c+1)] */
		plotLine (pi->tx, pi->ty, r, pi[1].tx, pi[1].ty, r);
	}
}

ShowData()
{
	PlotCur(0);	/* plot the image of the next time point into buffer */
	if (flagplotvel)
		PlotCur(1);	/* plot velocity also */

	/* display the buffer we just drew */
	XCopyArea (dpy, buffer, win, gc, 0, 0, scrnWidth, scrnHeight, 0, 0);
        if (flagstereo)
           XSetForeground (dpy, gc, purple.pixel);
        else
  	   XSetForeground (dpy, gc, BlackPixel (dpy, screen));
	XFillRectangle (dpy, buffer, gc, 0, 0, scrnWidth, scrnHeight);
}

HandleResize(event)
XEvent *event;
{
	if (event->xconfigure.width != scrnWidth ||
	    event->xconfigure.height != scrnHeight) {
		XFreePixmap (dpy, buffer);
		scrnWidth = event->xconfigure.width;
		scrnHeight = event->xconfigure.height;
		buffer = XCreatePixmap (dpy, DefaultRootWindow (dpy),
		    scrnWidth, scrnHeight,
		    DefaultDepth (dpy, screen));
		XSetForeground (dpy, gc, BlackPixel (dpy, screen));
		XFillRectangle (dpy, buffer, gc, 0, 0, scrnWidth, scrnHeight);
		XSetForeground (dpy, gc, WhitePixel (dpy, screen));
	}
}

DoQueryPointer(pxp,pyp)
int *pxp, *pyp;
{
	static int lastx=0, lasty=0;
	int thisx, thisy;
	int dx, dy;
	int kb;
	Window junk_win;
	int junk;

	XQueryPointer (dpy, win, &junk_win, &junk_win, &junk, &junk,
	    &thisx, &thisy, &kb);

	/*
	 * Give the user something to do :-)
	 */
	if ((kb & Button1Mask) && gap > 10) gap--;
	if ((kb & Button3Mask) && gap < 50) gap++;
	if (kb & Button2Mask) {
		dx = thisx - lastx;
		dy = thisy - lasty;
		px += dx;
		py += dy;
	}
	lastx = thisx;
	lasty = thisy;
}

void edit ()
     /* This allows the user to select the wave points and their frequencies */
{
  int px,py,junk;
  Window junkWin;
  int  x,ax,z,r,c,kb,i;
  int  xoff,yoff;
  int  row,col;

  xoff = scrnWidth/2 - 275;
  yoff = scrnHeight/2 - 350;
  xoff = 0;
  yoff = 0;


  for (;;) {
    if (QLength (dpy))
      ProcessEvent();

    XQueryPointer (dpy, win, &junkWin, &junkWin, &junk, &junk,&px, &py, &kb);
    if (px<0)
      px = 0;

    x = (px/10 % maxcols) * gap + STARTX + xoff;
    ax = px/10 % maxcols;
    z = (py/10 % (maxrows-1)) + 1;
    if (z < 1)
      z = 1;

                /* display the cursor */

    plotLine (x-5,yoff+STARTY+gap*(z-1),z-1, x+5,yoff+STARTY+gap*(z+1),z+1);
    plotLine (x-5,yoff+STARTY+gap*(z+1),z+1, x+5,yoff+STARTY+gap*(z-1),z-1);
    plotLine (x-6,yoff+STARTY+gap*(z-1),z-1, x+4,yoff+STARTY+gap*(z+1),z+1);
    plotLine (x-6,yoff+STARTY+gap*(z+1),z+1, x+4,yoff+STARTY+gap*(z-1),z-1);


    if (kb & Button3Mask) { /* right button starts the waving               */
      flagedit = !flagedit;
      return;
    }

    if (kb & Button1Mask) { /* left button held down to change amp. & freq. */
      while (1) {

        if (QLength (dpy))
          ProcessEvent();

        for (i=0;i<numEditedPoints;i++) {
          row = editedRowVals[i];
          col = editedColVals[i];
          CurGen[row*maxcols+col].pos = amplitudes[row*maxcols+col];
        }

        XQueryPointer (dpy,win,&junkWin,&junkWin, &junk, &junk,&px, &py, &kb);
        XDrawLine (dpy,buffer,gc2,0,scrnHeight-25,px,scrnHeight-25);

        frequencies[z*maxcols+ax] = (int)((float)px / (float)scrnWidth * 32);

        if (frequencies[z*maxcols+ax] < 0)
          frequencies[z*maxcols+ax] = 0;
        printf ("frequency=%d  \r",frequencies[z*maxcols+ax]);
        fflush(stdout);

        if (!(kb & Button1Mask)) {   /* release button to move edit cursor */
          numEditedPoints++;
          break;
        }
        CurGen[z*maxcols+ax].pos = (py - scrnHeight/2)*SCALE;
        amplitudes[z*maxcols+ax] = py - scrnHeight/2;
        editedRowVals[numEditedPoints] = z;
        editedColVals[numEditedPoints] = ax;

        Step();

        ShowData();

      }

    }
    Step();

    ShowData();

  }
}

/* end */

