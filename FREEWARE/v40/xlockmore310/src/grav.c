
#ifndef lint
static char sccsid[] = "@(#)grav.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * grav.c - for xlock, the X Window System lockscreen.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 11-Jul-94: color version
 * 6-Oct-93: by Greg Bowering <greg@smug.student.adelaide.edu.au>
 */

#include "xlock.h"
#include <math.h>

#define GRAV			-0.02	/* Gravitational constant */
#define DIST			16.0
#define COLLIDE			0.0001
#define ALMOST			15.99
#define HALF			0.5
/* #define INTRINSIC_RADIUS     200.0 */
#define INTRINSIC_RADIUS	((float) (gp->height/5))
#define STARRADIUS		(unsigned int)(gp->height/(2*DIST))
#define AVG_RADIUS		(INTRINSIC_RADIUS/DIST)
#define RADIUS			(unsigned int)(INTRINSIC_RADIUS/(POS(Z)+DIST))

#define XR			HALF*ALMOST
#define YR			HALF*ALMOST
#define ZR			HALF*ALMOST

#define VR			0.04

#define DIMENSIONS		3
#define X			0
#define Y			1
#define Z			2

#if 0
#define TRAIL 1			/* For trails (works good in mono only) */
#define DAMPING 1		/* For decaying orbits */
#endif
#if DAMPING
#define DAMP			0.999999
#define MaxA			0.1	/* Maximum acceleration */
#endif

#define POS(c) planet->P[c]
#define VEL(c) planet->V[c]
#define ACC(c) planet->A[c]

#define Planet(x,y)\
  if ((x) >= 0 && (y) >= 0 && (x) <= gp->width && (y) <= gp->height) {\
    if (planet->ri < 2)\
     XDrawPoint(display, window, gc, (x), (y));\
    else\
     XFillArc(display, window, gc,\
      (x) - planet->ri / 2, (y) - planet->ri / 2, planet->ri, planet->ri,\
      0, 23040);\
   }

#define FLOATRAND(min,max)	((min)+(LRAND()/MAXRAND)*((max)-(min)))

ModeSpecOpt grav_opts =
{0, NULL, NULL, NULL};

typedef struct {
	double
	            P[DIMENSIONS], V[DIMENSIONS], A[DIMENSIONS];
	int
	            xi, yi, ri;
	unsigned long colors;
} planetstruct;

typedef struct {
	int         width, height;
	int
	            x, y, sr, nplanets;
	unsigned long starcolor;
	planetstruct *planets;
} gravstruct;

static gravstruct *gravs = NULL;

static void
init_planet(ModeInfo * mi, planetstruct * planet)
{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);
	gravstruct *gp = &gravs[MI_SCREEN(mi)];

	if (MI_NPIXELS(mi) > 2)
		planet->colors = MI_PIXEL(mi, NRAND(MI_NPIXELS(mi)));
	else
		planet->colors = MI_WIN_WHITE_PIXEL(mi);
	/* Initialize positions */
	POS(X) = FLOATRAND(-XR, XR);
	POS(Y) = FLOATRAND(-YR, YR);
	POS(Z) = FLOATRAND(-ZR, ZR);

	if (POS(Z) > -ALMOST) {
		planet->xi = (unsigned int)
			((double) gp->width * (HALF + POS(X) / (POS(Z) + DIST)));
		planet->yi = (unsigned int)
			((double) gp->height * (HALF + POS(Y) / (POS(Z) + DIST)));
	} else
		planet->xi = planet->yi = -1;
	planet->ri = RADIUS;

	/* Initialize velocities */
	VEL(X) = FLOATRAND(-VR, VR);
	VEL(Y) = FLOATRAND(-VR, VR);
	VEL(Z) = FLOATRAND(-VR, VR);

	/* Draw planets */
	Planet(planet->xi, planet->yi);
}

static void
draw_planet(ModeInfo * mi, planetstruct * planet)
{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);
	gravstruct *gp = &gravs[MI_SCREEN(mi)];
	double      D;		/* A distance variable to work with */
	register unsigned char cmpt;

	D = POS(X) * POS(X) + POS(Y) * POS(Y) + POS(Z) * POS(Z);
	if (D < COLLIDE)
		D = COLLIDE;
	D = sqrt(D);
	D = D * D * D;
	for (cmpt = X; cmpt < DIMENSIONS; cmpt++) {
		ACC(cmpt) = POS(cmpt) * GRAV / D;
#ifdef DAMPING
		if (ACC(cmpt) > MaxA)
			ACC(cmpt) = MaxA;
		else if (ACC(cmpt) < -MaxA)
			ACC(cmpt) = -MaxA;
		VEL(cmpt) = VEL(cmpt) + ACC(cmpt);
		VEL(cmpt) *= DAMP;
#else
		/* update velocity */
		VEL(cmpt) = VEL(cmpt) + ACC(cmpt);
#endif
		/* update position */
		POS(cmpt) = POS(cmpt) + VEL(cmpt);
	}

	gp->x = planet->xi;
	gp->y = planet->yi;

	if (POS(Z) > -ALMOST) {
		planet->xi = (unsigned int)
			((double) gp->width * (HALF + POS(X) / (POS(Z) + DIST)));
		planet->yi = (unsigned int)
			((double) gp->height * (HALF + POS(Y) / (POS(Z) + DIST)));
	} else
		planet->xi = planet->yi = -1;

	/* Mask */
	XSetForeground(display, gc, MI_WIN_BLACK_PIXEL(mi));
	Planet(gp->x, gp->y);
#ifdef TRAIL
	XSetForeground(display, gc, planet->colors);
	XDrawPoint(display, MI_WINDOW(mi), gc, gp->x, gp->y);
#endif
	/* Move */
	gp->x = planet->xi;
	gp->y = planet->yi;
	planet->ri = RADIUS;

	/* Redraw */
	XSetForeground(display, gc, planet->colors);
	Planet(gp->x, gp->y);
}

void
init_grav(ModeInfo * mi)
{
	Display    *display = MI_DISPLAY(mi);
	GC          gc = MI_GC(mi);
	gravstruct *gp;
	unsigned char ball;

	if (gravs == NULL) {
		if ((gravs = (gravstruct *) calloc(MI_NUM_SCREENS(mi),
					       sizeof (gravstruct))) == NULL)
			return;
	}
	gp = &gravs[MI_SCREEN(mi)];

	gp->width = MI_WIN_WIDTH(mi);
	gp->height = MI_WIN_HEIGHT(mi);

	gp->sr = STARRADIUS;

	gp->nplanets = MI_BATCHCOUNT(mi);
	if (gp->nplanets < 0) {
		if (gp->planets) {
			(void) free((void *) gp->planets);
			gp->planets = NULL;
		}
		gp->nplanets = NRAND(-gp->nplanets) + 1;	/* Add 1 so its not too boring */
	}
	if (!gp->planets)
		gp->planets = (planetstruct *) calloc(gp->nplanets, sizeof (planetstruct));

	XClearWindow(display, MI_WINDOW(mi));

	if (MI_NPIXELS(mi) > 2)
		gp->starcolor = MI_PIXEL(mi, NRAND(MI_NPIXELS(mi)));
	else
		gp->starcolor = MI_WIN_WHITE_PIXEL(mi);
	for (ball = 0; ball < (unsigned char) gp->nplanets; ball++)
		init_planet(mi, &gp->planets[ball]);

	/* Draw centrepoint */
	XDrawArc(display, MI_WINDOW(mi), gc,
		 gp->width / 2 - gp->sr / 2, gp->height / 2 - gp->sr / 2, gp->sr, gp->sr,
		 0, 23040);
}

void
draw_grav(ModeInfo * mi)
{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);
	gravstruct *gp = &gravs[MI_SCREEN(mi)];
	register unsigned char ball;

	/* Mask centrepoint */
	XSetForeground(display, gc, MI_WIN_BLACK_PIXEL(mi));
	XDrawArc(display, window, gc,
		 gp->width / 2 - gp->sr / 2, gp->height / 2 - gp->sr / 2, gp->sr, gp->sr,
		 0, 23040);

	/* Resize centrepoint */
	switch (NRAND(4)) {
		case 0:
			if (gp->sr < STARRADIUS)
				gp->sr++;
			break;
		case 1:
			if (gp->sr > 2)
				gp->sr--;
	}

	/* Draw centrepoint */
	XSetForeground(display, gc, gp->starcolor);
	XDrawArc(display, window, gc,
		 gp->width / 2 - gp->sr / 2, gp->height / 2 - gp->sr / 2, gp->sr, gp->sr,
		 0, 23040);

	for (ball = 0; ball < (unsigned char) gp->nplanets; ball++)
		draw_planet(mi, &gp->planets[ball]);
}

void
release_grav(ModeInfo * mi)
{
	if (gravs != NULL) {
		int         screen;

		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
			gravstruct *gp = &gravs[screen];

			if (gp->planets)
				(void) free((void *) gp->planets);
		}
		(void) free((void *) gravs);
		gravs = NULL;
	}
}
