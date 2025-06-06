#ifndef lint
static char sccsid[] = "@(#)triangle.c	3.9 96/05/25 xlockmore";

#endif

/*-
 * triangle.c - create a triangle-mountain
 *
 * Copyright (c) 1995 by Tobias Gloth
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 10-Mar-96: re-arranged and re-formatted the code for appearance and
 *	to make common subroutines.  Simplified.
 *	Ron Hitchens <ron@utw.com>
 * 07-Mar-96: Removed internal delay code, set MI_PAUSE(mi) for inter-scene
 *	delays.  No other delays are needed here.
 *	Made pause time sensitive to value of cycles (in 10ths of a second)
 *	Removed (hopefully) all references to globals.
 *	Ron Hitchens <ron@utw.com>
 * 27-Feb-96 Undid the changes listed below.  Added ModeInfo argument.
 *	Implemented delay between scenes using the MI_PAUSE(mi) scheme.
 *	Ron Hitchens <ron@utw.com>
 * 27-Dec-95 Ron Hitchens <ron@utw.com>
 *      Modified logic of draw_triangle() to provide a delay (sensitive
 *      to the value of cycles) between each iteration.  Because this
 *      mode is so compute intensive, when the new event loop adjusted
 *      the delay to compensate, this mode had almost no delay time left.
 *      This change pauses between each new landscape, but could still
 *      be done better (it is not sensitive to input events while drawing,
 *      for example).
 * 03-Nov-95 Many changes (hopefully some good ones) by David Bagley
 * 01-Oct-95 Written by Tobias Gloth
 */

#include <math.h>
#include "xlock.h"

#ifndef max
#define max(X,Y) ((X)>(Y)?(X):(Y))
#endif

#ifndef min
#define min(X,Y) ((X)<(Y)?(X):(Y))
#endif

#define MAX_STEPS 8
#define MAX_SIZE  (1<<MAX_STEPS)
#define MAX_LEVELS 1000

#define DELTA  0.4
#define LEFT   (-0.25)
#define RIGHT  1.25
#define TOP    0.3
#define BOTTOM 1.0
#define BLUE   45		/* Just the right shade of blue */

#define BACKFACE_REMOVAL

#define DISPLACE(h,d) ((h)/2+LRAND()/(MAXRAND/(2*(d)+1))-d)

typedef struct {
	int         win_height;
	int         win_width;
	int         size;
	int         steps;
	int         stage;
	int         xpos[2 * MAX_SIZE + 1];
	int         ypos[MAX_SIZE + 1];
	int         init_now;
	int         fast;
	int         i;
	int         j;
	int         d;
	short       level[MAX_LEVELS];
	short       H[(MAX_SIZE + 1) * (MAX_SIZE + 2) / 2];
	short      *h[MAX_SIZE + 1];
	short       delta[MAX_STEPS];
} trianglestruct;

static trianglestruct triangles[MAXSCREENS];

ModeSpecOpt triangle_opts =
{0, NULL, NULL, NULL};


static
void
draw_atriangle(ModeInfo * mi, XPoint * p, int y_0, int y_1, int y_2, double dinv)
{
	Display    *dpy = MI_DISPLAY(mi);
	Window      win = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);

	if (!MI_WIN_IS_MONO(mi) && MI_NPIXELS(mi) > 2) {	/* color */
		int         dmax, dmin;
		long        color;

		dmin = min(y_0, y_1);
		dmin = min(dmin, y_2);
		dmax = max(y_0, y_1);
		dmax = max(dmax, y_2);

		if (dmax == 0) {
			color = BLUE;
		} else {
			color = NUMCOLORS - ((double) NUMCOLORS / M_PI_2 * atan(dinv * (dmax - dmin)));
		}

		XSetForeground(dpy, gc, MI_PIXEL(mi, color));
		XFillPolygon(dpy, win, gc, p, 3, Convex, CoordModeOrigin);
	} else {
		/* mono */
#ifdef BACKFACE_REMOVAL
		XSetForeground(dpy, gc, MI_WIN_BLACK_PIXEL(mi));
		XFillPolygon(dpy, win, gc, p, 3, Convex, CoordModeOrigin);
#endif
		XSetForeground(dpy, gc, MI_WIN_WHITE_PIXEL(mi));
		XDrawLine(dpy, win, gc, p[0].x, p[0].y, p[1].x, p[1].y);
		XDrawLine(dpy, win, gc, p[1].x, p[1].y, p[2].x, p[2].y);
		XDrawLine(dpy, win, gc, p[2].x, p[2].y, p[0].x, p[0].y);
	}
}

static
void
calc_points1(trianglestruct * tp, int d, int *y0_p, int *y1_p, int *y2_p, XPoint * p)
{
	*y0_p = tp->level[max(tp->h[tp->i][tp->j], 0)];
	*y1_p = tp->level[max(tp->h[tp->i + d][tp->j], 0)];
	*y2_p = tp->level[max(tp->h[tp->i][tp->j + d], 0)];

	p[0].x = tp->xpos[2 * tp->i + tp->j];
	p[1].x = tp->xpos[2 * (tp->i + d) + tp->j];
	p[2].x = tp->xpos[2 * tp->i + (tp->j + d)];

	p[0].y = tp->ypos[tp->j] - *y0_p;
	p[1].y = tp->ypos[tp->j] - *y1_p;
	p[2].y = tp->ypos[tp->j + d] - *y2_p;
}

static
void
calc_points2(trianglestruct * tp, int d, int *y0_p, int *y1_p, int *y2_p, XPoint * p)
{
	*y0_p = tp->level[max(tp->h[tp->i + d][tp->j], 0)];
	*y1_p = tp->level[max(tp->h[tp->i + d][tp->j + d], 0)];
	*y2_p = tp->level[max(tp->h[tp->i][tp->j + d], 0)];

	p[0].x = tp->xpos[2 * (tp->i + d) + tp->j];
	p[1].x = tp->xpos[2 * (tp->i + d) + (tp->j + d)];
	p[2].x = tp->xpos[2 * tp->i + (tp->j + d)];

	p[0].y = tp->ypos[tp->j] - *y0_p;
	p[1].y = tp->ypos[tp->j + d] - *y1_p;
	p[2].y = tp->ypos[tp->j + d] - *y2_p;
}


static
void
draw_mesh(ModeInfo * mi, trianglestruct * tp, int d, int count)
{
	Display    *dpy = MI_DISPLAY(mi);
	Window      win = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);
	XPoint      p[3];
	int         new = 1;
	int         y_0, y_1, y_2;
	double      dinv = 0.2 / d;

	if ((tp->j == 0) && (tp->i == 0)) {
		XSetForeground(dpy, gc, MI_WIN_BLACK_PIXEL(mi));
		XFillRectangle(dpy, win, gc, 0, 0,
			       tp->win_width, tp->win_height);
	}
	for (; (tp->j < tp->size) && (count > 0); tp->j += ((count) ? d : 0)) {
		for (tp->i = (new) ? tp->i : 0, new = 0;
		     (tp->i < MAX_SIZE - tp->j) && (count > 0);
		     tp->i += d, count--) {
			if (tp->i + tp->j < tp->size) {
				calc_points1(tp, d, &y_0, &y_1, &y_2, p);
				draw_atriangle(mi, p, y_0, y_1, y_2, dinv);
			}
			if (tp->i + tp->j + d < tp->size) {
				calc_points2(tp, d, &y_0, &y_1, &y_2, p);
				draw_atriangle(mi, p, y_0, y_1, y_2, dinv);
			}
		}
	}

	if (tp->j == tp->size) {
		tp->init_now = 1;
	}
}

void
init_triangle(ModeInfo * mi)
{
	Display    *display = MI_DISPLAY(mi);
	Window      win = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);
	int         scrn = MI_SCREEN(mi);
	trianglestruct *tp = &triangles[scrn];
	short      *tmp;
	int         i, dim, one;

	tp->win_width = MI_WIN_WIDTH(mi);
	tp->win_height = MI_WIN_HEIGHT(mi);
	tp->init_now = 1;
	tp->fast = 2;

	XSetForeground(display, gc, MI_WIN_BLACK_PIXEL(mi));
	XFillRectangle(display, win, gc, 0, 0, tp->win_width, tp->win_height);

	tp->steps = MAX_STEPS;

	do {
		tp->size = 1 << --tp->steps;
	} while (tp->size * 5 > tp->win_width);

	tmp = tp->H;
	for (i = 0; i < tp->size + 1; i++) {
		tp->h[i] = tmp;
		tmp += (tp->size) + 1 - i;
	}

	tp->stage = -1;
	dim = min(tp->win_width, tp->win_height);

	for (i = 0; i < 2 * tp->size + 1; i++) {
		tp->xpos[i] = (short) ((((double) i)
			 / ((double) (2 * tp->size)) * (RIGHT - LEFT) + LEFT)
				       * dim) + (tp->win_width - dim) / 2;
	}

	for (i = 0; i < (tp->size + 1); i++) {
		tp->ypos[i] = (short) ((((double) i)
			 / ((double) tp->size) * (BOTTOM - TOP) + TOP) * dim)
			+ (tp->win_height - dim) / 2;
	}

	for (i = 0; i < tp->steps; i++) {
		tp->delta[i] = ((short) (DELTA * dim)) >> i;
	}

	one = tp->delta[0];

	for (i = 0; i < MAX_LEVELS; i++) {
		tp->level[i] = (i * i) / one;
	}
}

void
draw_triangle(ModeInfo * mi)
{
	trianglestruct *tp = &triangles[MI_SCREEN(mi)];
	int         d, d2, i, j, delta;

	if (!tp->init_now) {
		draw_mesh(mi, tp, tp->d / 2, MAX_SIZE / tp->d);

		/* The init_now flag will pop up when the scene is complete.
		 * Cycles specifies how long to wait, in 1/10 secs.
		 TODO: This is wrong for multi-screens ***
		 */
		if (tp->init_now) {
			MI_PAUSE(mi) = MI_CYCLES(mi) * 100000;
		}
		return;
	}
	if (!(++tp->stage)) {
		tp->h[0][0] = max(0, DISPLACE(0, tp->delta[0]));
		tp->h[tp->size][0] = max(0, DISPLACE(0, tp->delta[0]));
		tp->h[0][tp->size] = max(0, DISPLACE(0, tp->delta[0]));
	} else {
		d = 2 << (tp->steps - tp->stage);
		d2 = d / 2;
		delta = tp->delta[tp->stage - 1];

		for (i = 0; i < tp->size; i += d) {
			for (j = 0; j < (tp->size - i); j += d) {
				tp->h[i + d2][j] = DISPLACE(tp->h[i][j]
						   + tp->h[i + d][j], delta);
				tp->h[i][j + d2] = DISPLACE(tp->h[i][j]
						   + tp->h[i][j + d], delta);
				tp->h[i + d2][j + d2] =
					DISPLACE(tp->h[i + d][j]
						 + tp->h[i][j + d], delta);
			}

			tp->init_now = 0;
			tp->i = 0;
			tp->j = 0;
			tp->d = d;
		}
	}

	if (tp->stage == tp->steps) {
		tp->stage = -1;
	}
}
