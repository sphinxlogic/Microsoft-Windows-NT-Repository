
#ifndef lint
static char sccsid[] = "@(#)spiral.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * spiral.c - low cpu screen design for xlock, the X Window System lockscreen.
 *
 * Idea based on a graphics demo I
 * saw a *LONG* time ago.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 24-Jul-95: Fix to allow cycles not to have an arbitrary value by
 *            Peter Schmitzberger (schmitz@coma.sbg.ac.at).
 * 06-Mar-95: Finished cleaning up and final testing.
 * Copyright (c) 1994 by Darrick Brown.
 *
 * 03-Mar-95: Cleaned up code.
 * 12-Jul-94: Written.
 */

#include "xlock.h"
#include <math.h>

#define MAXTRAIL 512		/* The length of the trail */
#define MAXDOTS 40
#define MINDOTS 1
#define TWOPI (2*M_PI)		/* for convienence */
#define JAGGINESS 4		/* This sets the "Craziness" of the spiral (I like 4) */
#define SPEED 2.0

ModeSpecOpt spiral_opts =
{0, NULL, NULL, NULL};

typedef struct {
	float       hx, hy, ha, hr;
} Traildots;

typedef struct {
	Traildots  *traildots;
	float       cx;
	float       cy;
	float       angle;
	float       radius;
	float       dr;
	float       da;
	float       dx;
	float       dy;
	int         erase;
	int         inc;
	float       colors;
	int         width;
	int         height;
	float       top, bottom, left, right;
	int         dots, nlength;
} spiralstruct;

static spiralstruct *spirals = NULL;

static void draw_dots(ModeInfo * mi, int in);

#define TFX(sp,x) ((int)((x/sp->right)*(float)sp->width))
#define TFY(sp,y) ((int)((y/sp->top)*(float)sp->height))

static void
draw_dots(ModeInfo * mi, int in)
{

	float       i, inc;
	float       x, y;

	spiralstruct *sp = &spirals[MI_SCREEN(mi)];

	inc = TWOPI / (float) sp->dots;
	for (i = 0.0; i < TWOPI; i += inc) {
		x = sp->traildots[in].hx + COSF(i + sp->traildots[in].ha) *
			sp->traildots[in].hr;
		y = sp->traildots[in].hy + SINF(i + sp->traildots[in].ha) *
			sp->traildots[in].hr;
		XDrawPoint(MI_DISPLAY(mi), MI_WINDOW(mi), MI_GC(mi),
			   TFX(sp, x), TFY(sp, y));
	}

}

void
init_spiral(ModeInfo * mi)
{
	spiralstruct *sp;
	int         i;

	if (spirals == NULL) {
		if ((spirals = (spiralstruct *) calloc(MI_NUM_SCREENS(mi),
					     sizeof (spiralstruct))) == NULL)
			return;
	}
	sp = &spirals[MI_SCREEN(mi)];

	sp->width = MI_WIN_WIDTH(mi);
	sp->height = MI_WIN_HEIGHT(mi);

	XClearWindow(MI_DISPLAY(mi), MI_WINDOW(mi));

	/* Init */
	sp->nlength = MI_CYCLES(mi);

	if (!sp->traildots)
		sp->traildots = (Traildots *) malloc(sp->nlength * sizeof (Traildots));

	/* initialize the allocated array */
	for (i = 0; i < sp->nlength; i++) {
		sp->traildots[i].hx = 0.0;
		sp->traildots[i].hy = 0.0;
		sp->traildots[i].ha = 0.0;
		sp->traildots[i].hr = 0.0;
	}

	/* keep the window parameters proportional */
	sp->top = 10000.0;
	sp->bottom = 0;
	sp->right = (float) (sp->width) / (float) (sp->height) * (10000.0);
	sp->left = 0;

	/* assign the initial values */
	sp->cx = (float) (5000.0 - NRAND(2000)) / 10000.0 * sp->right;
	sp->cy = (float) (5000.0 - NRAND(2000));
	sp->radius = (float) (NRAND(200) + 200);
	sp->angle = 0.0;
	sp->dx = (float) (10 - NRAND(20)) * SPEED;
	sp->dy = (float) (10 - NRAND(20)) * SPEED;
	sp->dr = (float) ((NRAND(10) + 4) * (1 - (LRAND() & 1) * 2));
	sp->da = (float) NRAND(360) / 7200.0 + 0.01;
	sp->colors = 0.0;
	sp->erase = 0;
	sp->inc = 0;
	sp->traildots[sp->inc].hx = sp->cx;
	sp->traildots[sp->inc].hy = sp->cy;
	sp->traildots[sp->inc].ha = sp->angle;
	sp->traildots[sp->inc].hr = sp->radius;
	sp->inc++;

	sp->dots = MI_BATCHCOUNT(mi);
	if (sp->dots < -MINDOTS)
		sp->dots = NRAND(sp->dots - MINDOTS + 1) + MINDOTS;
	/* Absolute minimum */
	if (sp->dots < MINDOTS)
		sp->dots = MINDOTS;
}

void
draw_spiral(ModeInfo * mi)
{
	Display    *display = MI_DISPLAY(mi);
	GC          gc = MI_GC(mi);
	spiralstruct *sp = &spirals[MI_SCREEN(mi)];

	if (sp->erase == 1) {
		XSetForeground(display, gc, MI_WIN_BLACK_PIXEL(mi));
		draw_dots(mi, sp->inc);
	}
	sp->cx += sp->dx;
	sp->traildots[sp->inc].hx = sp->cx;

	if ((sp->cx > 9000.0) || (sp->cx < 1000.0))
		sp->dx *= -1.0;

	sp->cy += sp->dy;
	sp->traildots[sp->inc].hy = sp->cy;

	if ((sp->cy > 9000.0) || (sp->cy < 1000.0))
		sp->dy *= -1.0;

	sp->radius += sp->dr;
	sp->traildots[sp->inc].hr = sp->radius;

	if ((sp->radius > 2500.0) && (sp->dr > 0.0))
		sp->dr *= -1.0;
	else if ((sp->radius < 50.0) && (sp->radius < 0.0))
		sp->dr *= -1.0;

	/* Randomly give some variations to:  */

	/* spiral direction (if it is within the boundaries) */
	if ((NRAND(3000) < 1 * JAGGINESS) &&
	    (((sp->cx > 2000.0) && (sp->cx < 8000.0)) &&
	     ((sp->cy > 2000.0) && (sp->cy < 8000.0)))) {
		sp->dx = (float) (10 - NRAND(20)) * SPEED;
		sp->dy = (float) (10 - NRAND(20)) * SPEED;
	}
	/* The speed of the change in size of the spiral */
	if (NRAND(3000) < 1 * JAGGINESS) {
		if (LRAND() & 1)
			sp->dr += (float) (NRAND(3) + 1);
		else
			sp->dr -= (float) (NRAND(3) + 1);

		/* don't let it get too wild */
		if (sp->dr > 18.0)
			sp->dr = 18.0;
		else if (sp->dr < 4.0)
			sp->dr = 4.0;
	}
	/* The speed of rotation */
	if (NRAND(3000) < 1 * JAGGINESS)
		sp->da = (float) NRAND(360) / 7200.0 + 0.01;

	/* Reverse rotation */
	if (NRAND(3000) < 1 * JAGGINESS)
		sp->da *= -1.0;

	sp->angle += sp->da;
	sp->traildots[sp->inc].ha = sp->angle;

	if (sp->angle > TWOPI)
		sp->angle -= TWOPI;
	else if (sp->angle < 0.0)
		sp->angle += TWOPI;

	sp->colors += (float) MI_NPIXELS(mi) / ((float) (2 * sp->nlength));
	if (sp->colors >= (float) MI_NPIXELS(mi))
		sp->colors = 0.0;

	if (MI_NPIXELS(mi) > 2)
		XSetForeground(display, gc, MI_PIXEL(mi, (int) sp->colors));
	else
		XSetForeground(display, gc, MI_WIN_WHITE_PIXEL(mi));
	draw_dots(mi, sp->inc);
	sp->inc++;

	if (sp->inc > sp->nlength - 1) {
		sp->inc -= sp->nlength;
		sp->erase = 1;
	}
}

void
release_spiral(ModeInfo * mi)
{
	if (spirals != NULL) {
		int         screen;

		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
			spiralstruct *sp = &spirals[screen];

			if (sp->traildots)
				(void) free((void *) sp->traildots);
		}
		(void) free((void *) spirals);
		spirals = NULL;
	}
}
