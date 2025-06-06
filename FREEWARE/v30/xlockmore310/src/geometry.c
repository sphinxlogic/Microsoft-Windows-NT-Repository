
#ifndef lint
static char sccsid[] = "@(#)geometry.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * geometry.c - low cpu screen design for xlock, the X Window System lockscreen.
 *
 * This screen design has n (2 <= n <= MAXINT) number of points that
 * randomly move around the screen.  Each point is connected to every
 * other point by a line.  Gives the sensation of abstract 3D morphing.
 *
 * Copyright (c) 1994 by Darrick Brown.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 06-Mar-95: Cleaned up code.
 * 11-Jul-94: Written.
 */

#include "xlock.h"
#include <math.h>

/* remember: the number of lines to be drawn is 1+2+3+...+PTS */
#define MINPTS 2

ModeSpecOpt geometry_opts =
{0, NULL, NULL, NULL};

typedef struct {
	float       sx, sy;
	float       ex, ey;
	float       dx, dy;
	float       px, py;
	float       oldx, oldy;
} geompoint;
typedef struct {
	geompoint  *point;
	float       colors;
	int         width;
	int         height;
	float       top, bottom, left, right;
	int         num;
} movepoint;

static movepoint *pts = NULL;

#define SQR(v) ((v) * (v))
#define TFX(x) ((int) ((x / pt->right) * (float) pt->width))
#define TFY(y) (pt->height - (int) ((y / pt->top) * (float) pt->height))

static void
eraselines(ModeInfo * mi)
{
	int         i, j;
	movepoint  *pt = &pts[MI_SCREEN(mi)];

	XSetForeground(MI_DISPLAY(mi), MI_GC(mi), MI_WIN_BLACK_PIXEL(mi));
	for (i = 0; i < pt->num - 1; i++) {
		for (j = i + 1; j < pt->num; j++) {
			XDrawLine(MI_DISPLAY(mi), MI_WINDOW(mi), MI_GC(mi),
			      TFX(pt->point[i].oldx), TFY(pt->point[i].oldy),
			     TFX(pt->point[j].oldx), TFY(pt->point[j].oldy));
		}
	}
}


static void
drawlines(ModeInfo * mi)
{
	int         i, j;
	movepoint  *pt = &pts[MI_SCREEN(mi)];

	for (i = 0; i < pt->num - 1; i++) {
		for (j = i + 1; j < pt->num; j++) {
			XDrawLine(MI_DISPLAY(mi), MI_WINDOW(mi), MI_GC(mi),
				  TFX(pt->point[i].px), TFY(pt->point[i].py),
				  TFX(pt->point[j].px), TFY(pt->point[j].py));
		}
	}
}

void
init_geometry(ModeInfo * mi)
{
	int         i;
	float       dist, speed;
	movepoint  *pt;

	if (pts == NULL) {
		if ((pts = (movepoint *) calloc(MI_NUM_SCREENS(mi),
						sizeof (movepoint))) == NULL)
			return;
	}
	pt = &pts[MI_SCREEN(mi)];

	pt->width = MI_WIN_WIDTH(mi);
	pt->height = MI_WIN_HEIGHT(mi);

	XClearWindow(MI_DISPLAY(mi), MI_WINDOW(mi));

	/* keep the window parameters proportional */
	pt->top = 10000.0;
	pt->bottom = 0;
	pt->right = (float) (pt->width) / (float) (pt->height) * (10000.0);
	pt->left = 0;

	pt->num = MI_BATCHCOUNT(mi);
	if (pt->num < -MINPTS) {
		if (pt->point) {
			(void) free((void *) pt->point);
			pt->point = NULL;
		}
		pt->num = NRAND(-pt->num - MINPTS + 1) + MINPTS;
	}
	/* Absolute minimum */
	if (pt->num < MINPTS)
		pt->num = MINPTS;
	if (!pt->point)
		pt->point = (geompoint *) malloc(pt->num * sizeof (geompoint));

	for (i = 0; i < pt->num; i++) {
		pt->point[i].sx = (float) (NRAND((int) pt->right));
		pt->point[i].sy = (float) (NRAND(10000));
		pt->point[i].px = pt->point[i].sx;
		pt->point[i].py = pt->point[i].sy;
		pt->point[i].ex = (float) (NRAND((int) pt->right));
		pt->point[i].ey = (float) (NRAND(10000));
		pt->point[i].dx = pt->point[i].ex - pt->point[i].sx;
		pt->point[i].dy = pt->point[i].ey - pt->point[i].sy;
		dist = sqrt((pt->point[i].dx * pt->point[i].dx) +
			    (pt->point[i].dy * pt->point[i].dy));
		pt->point[i].dx /= dist;
		pt->point[i].dy /= dist;
		speed = (float) (NRAND(15) + 21);	/* a float 15.0-36.0 */
		pt->point[i].dx *= speed;
		pt->point[i].dy *= speed;
	}

	pt->colors = 0.0;

}

void
draw_geometry(ModeInfo * mi)
{
	int         i;
	float       dist, speed;
	movepoint  *pt = &pts[MI_SCREEN(mi)];

	for (i = 0; i < pt->num; i++) {
		pt->point[i].oldx = pt->point[i].px;
		pt->point[i].oldy = pt->point[i].py;
		pt->point[i].px += pt->point[i].dx;
		pt->point[i].py += pt->point[i].dy;
	}

	eraselines(mi);

	pt->colors += (float) MI_NPIXELS(mi) / 200.0;
	if (pt->colors >= MI_NPIXELS(mi))
		pt->colors = 0.0;
	if (MI_NPIXELS(mi) > 2)
		XSetForeground(MI_DISPLAY(mi), MI_GC(mi), MI_PIXEL(mi, (int) pt->colors));
	else
		XSetForeground(MI_DISPLAY(mi), MI_GC(mi), MI_WIN_WHITE_PIXEL(mi));
	drawlines(mi);

	for (i = 0; i < pt->num; i++)
		if (sqrt(SQR(pt->point[i].px - pt->point[i].ex) +
			 SQR(pt->point[i].py - pt->point[i].ey)) <= 50.0) {

			pt->point[i].sx = pt->point[i].px;	/* Set it to the current point */
			pt->point[i].sy = pt->point[i].py;
			pt->point[i].ex = (float) (NRAND((int) pt->right));
			pt->point[i].ey = (float) (NRAND(10000));
			pt->point[i].dx = pt->point[i].ex - pt->point[i].sx;
			pt->point[i].dy = pt->point[i].ey - pt->point[i].sy;
			dist = sqrt((pt->point[i].dx * pt->point[i].dx) +
				    (pt->point[i].dy * pt->point[i].dy));
			pt->point[i].dx /= dist;
			pt->point[i].dy /= dist;
			speed = (float) (NRAND(15) + 36);	/* a float 1.5-3.6 */
			pt->point[i].dx *= speed;
			pt->point[i].dy *= speed;
		}
}

void
release_geometry(ModeInfo * mi)
{
	if (pts != NULL) {
		int         screen;

		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
			movepoint  *pt = &pts[screen];

			if (pt->point)
				(void) free((void *) pt->point);
		}
		(void) free((void *) pts);
		pts = NULL;
	}
}
