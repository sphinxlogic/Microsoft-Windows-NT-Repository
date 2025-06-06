
#ifndef lint
static char sccsid[] = "@(#)laser.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * laser.c - laser for xlockmore
 *
 * Copyright (c) 1995 Pascal Pensa <pensa@aurora.unice.fr>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 */

#include "xlock.h"

#define MINREDRAW 3		/* Number of redrawn on each frame */
#define MAXREDRAW 8

#define MINLASER  1		/* Laser number */

#define MINWIDTH  2		/* Laser ray width range */
#define MAXWIDTH 40

#define MINSPEED  2		/* Speed range */
#define MAXSPEED 17

#define MINDIST  10		/* Minimal distance from edges */

#define COLORSTEP 2		/* Laser color step */

#define RANGE_RAND(min,max) ((min) + LRAND() % ((max) - (min)))

ModeSpecOpt laser_opts =
{0, NULL, NULL, NULL};

typedef enum {
	TOP, RIGHT, BOTTOM, LEFT
} border;

typedef struct {
	int         bx;		/* border x */
	int         by;		/* border y */
	border      bn;		/* active border */
	int         dir;	/* direction */
	int         speed;	/* laser velocity from MINSPEED to MAXSPEED */
	int         sx[MAXWIDTH];	/* x stack */
	int         sy[MAXWIDTH];	/* x stack */
	XGCValues   gcv;	/* for color */
} laserstruct;

typedef struct {
	int         width;
	int         height;
	int         cx;		/* center x */
	int         cy;		/* center y */
	int         lw;		/* laser width */
	int         ln;		/* laser number */
	int         lr;		/* laser redraw */
	int         sw;		/* stack width */
	int         so;		/* stack offset */
	int         time;	/* up time */
	GC          stippledGC;
	laserstruct *laser;
} lasersstruct;

static lasersstruct *lasers = NULL;

static XGCValues gcv_black;	/* for black color */

void
init_laser(ModeInfo * mi)
{
	int         i, c = 0;
	XGCValues   gcv;
	lasersstruct *lp;

	if (lasers == NULL) {
		if ((lasers = (lasersstruct *) calloc(MI_NUM_SCREENS(mi),
					     sizeof (lasersstruct))) == NULL)
			return;
	}
	lp = &lasers[MI_SCREEN(mi)];

	lp->width = MI_WIN_WIDTH(mi);
	lp->height = MI_WIN_HEIGHT(mi);
	lp->time = 0;

	lp->ln = MI_BATCHCOUNT(mi);
	if (lp->ln < -MINLASER) {
		/* if lp->ln is random ... the size can change */
		if (lp->laser != NULL) {
			(void) free((void *) lp->laser);
			lp->laser = NULL;
		}
		lp->ln = NRAND(-lp->ln - MINLASER + 1) + MINLASER;
	} else if (lp->ln < MINLASER)
		lp->ln = MINLASER;

	if (!lp->laser) {
		lp->laser = (laserstruct *) malloc(lp->ln * sizeof (laserstruct));

		gcv.foreground = MI_WIN_WHITE_PIXEL(mi);
		gcv.background = MI_WIN_BLACK_PIXEL(mi);
		gcv_black.foreground = MI_WIN_BLACK_PIXEL(mi);

		lp->stippledGC = XCreateGC(MI_DISPLAY(mi), MI_WINDOW(mi),
					   GCForeground | GCBackground, &gcv);
	}
	XClearWindow(MI_DISPLAY(mi), MI_WINDOW(mi));

	if (MINDIST < lp->width - MINDIST)
		lp->cx = RANGE_RAND(MINDIST, lp->width - MINDIST);
	else
		lp->cx = RANGE_RAND(0, lp->width);
	if (MINDIST < lp->height - MINDIST)
		lp->cy = RANGE_RAND(MINDIST, lp->height - MINDIST);
	else
		lp->cy = RANGE_RAND(0, lp->height);
	lp->lw = RANGE_RAND(MINWIDTH, MAXWIDTH);
	lp->lr = RANGE_RAND(MINREDRAW, MAXREDRAW);
	lp->sw = 0;
	lp->so = 0;

	if (MI_NPIXELS(mi) > 2)
		c = NRAND(MI_NPIXELS(mi));

	for (i = 0; i < lp->ln; i++) {
		laserstruct *l = &lp->laser[i];

		l->bn = (border) NRAND(4);

		switch (l->bn) {
			case TOP:
				l->bx = NRAND(lp->width);
				l->by = 0;
				break;
			case RIGHT:
				l->bx = lp->width;
				l->by = NRAND(lp->height);
				break;
			case BOTTOM:
				l->bx = NRAND(lp->width);
				l->by = lp->height;
				break;
			case LEFT:
				l->bx = 0;
				l->by = NRAND(lp->height);
		}

		l->dir = LRAND() & 1;
		l->speed = ((RANGE_RAND(MINSPEED, MAXSPEED) * lp->width) / 1000) + 1;
		if (MI_NPIXELS(mi) > 2) {
			l->gcv.foreground = MI_PIXEL(mi, c);
			c = (c + COLORSTEP) % MI_NPIXELS(mi);
		} else
			l->gcv.foreground = MI_WIN_WHITE_PIXEL(mi);
	}
}

static void
draw_laser_once(ModeInfo * mi)
{
	Display    *display = MI_DISPLAY(mi);
	lasersstruct *lp = &lasers[MI_SCREEN(mi)];
	int         i;

	for (i = 0; i < lp->ln; i++) {
		laserstruct *l = &lp->laser[i];

		if (lp->sw >= lp->lw) {
			XChangeGC(display, lp->stippledGC, GCForeground, &gcv_black);
			XDrawLine(display, MI_WINDOW(mi), lp->stippledGC,
				  lp->cx, lp->cy,
				  l->sx[lp->so], l->sy[lp->so]);
		}
		if (l->dir) {
			switch (l->bn) {
				case TOP:
					l->bx -= l->speed;
					if (l->bx < 0) {
						l->by = -l->bx;
						l->bx = 0;
						l->bn = LEFT;
					}
					break;
				case RIGHT:
					l->by -= l->speed;
					if (l->by < 0) {
						l->bx = lp->width + l->by;
						l->by = 0;
						l->bn = TOP;
					}
					break;
				case BOTTOM:
					l->bx += l->speed;
					if (l->bx >= lp->width) {
						l->by = lp->height - l->bx % lp->width;
						l->bx = lp->width;
						l->bn = RIGHT;
					}
					break;
				case LEFT:
					l->by += l->speed;
					if (l->by >= lp->height) {
						l->bx = l->by % lp->height;
						l->by = lp->height;
						l->bn = BOTTOM;
					}
			}
		} else {
			switch (l->bn) {
				case TOP:
					l->bx += l->speed;
					if (l->bx >= lp->width) {
						l->by = l->bx % lp->width;
						l->bx = lp->width;
						l->bn = RIGHT;
					}
					break;
				case RIGHT:
					l->by += l->speed;
					if (l->by >= lp->height) {
						l->bx = lp->width - l->by % lp->height;
						l->by = lp->height;
						l->bn = BOTTOM;
					}
					break;
				case BOTTOM:
					l->bx -= l->speed;
					if (l->bx < 0) {
						l->by = lp->height + l->bx;
						l->bx = 0;
						l->bn = LEFT;
					}
					break;
				case LEFT:
					l->by -= l->speed;
					if (l->by < 0) {
						l->bx = -l->bx;
						l->by = 0;
						l->bn = TOP;
					}
			}
		}

		XChangeGC(display, lp->stippledGC, GCForeground, &l->gcv);
		XDrawLine(display, MI_WINDOW(mi), lp->stippledGC,
			  lp->cx, lp->cy, l->bx, l->by);

		l->sx[lp->so] = l->bx;
		l->sy[lp->so] = l->by;

	}

	if (lp->sw < lp->lw)
		++lp->sw;

	lp->so = (lp->so + 1) % lp->lw;
}

void
draw_laser(ModeInfo * mi)
{
	lasersstruct *lp = &lasers[MI_SCREEN(mi)];
	int         i;

	for (i = 0; i < lp->lr; i++)
		draw_laser_once(mi);

	if (++lp->time > MI_CYCLES(mi))
		init_laser(mi);
}

void
release_laser(ModeInfo * mi)
{
	if (lasers != NULL) {
		int         screen;

		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
			lasersstruct *lp = &lasers[screen];

			if (lp->laser != NULL)
				(void) free((void *) lp->laser);
			if (lp->stippledGC != NULL)
				XFreeGC(MI_DISPLAY(mi), lp->stippledGC);
		}
		(void) free((void *) lasers);
		lasers = NULL;
	}
}
