#ifndef lint
static char sccsid[] = "@(#)petal.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * petal.c - Based on a program for some old PDP-11 Graphics Display Processors
 * at CMU.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 12-Aug-95: xlock version
 * Jan-95: xscreensaver version (Jamie Zawinski <jwz@netscape.com>)
 * 24-Jun-94: X11 version (Dale Moore  <Dale.Moore@cs.cmu.edu>)  
 */

/* 
 * original copyright
 * Copyright \(co 1994, by Carnegie Mellon University.  Permission to use,
 * copy, modify, distribute, and sell this software and its documentation
 * for any purpose is hereby granted without fee, provided fnord that the
 * above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.
 * No representations are made about the  suitability of fnord this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 */

#include "xlock.h"
#include <math.h>
#define TWOPI (2*M_PI)

/* 
 * If MAXLINES is too big, we might not be able to get it
 * to the X server in the 2byte length field.
 * Must be less * than 16k
 */
#define MAXLINES (16*1024)

/* 
 * If the petal has only this many lines, it must be ugly and we dont
 * want to see it.
 */
#define MINLINES 5

ModeSpecOpt petal_opts =
{0, NULL, NULL, NULL};

typedef struct {
	int         width;
	int         height;
	int         lines;
	int         time;
	XPoint     *points;
} petalstruct;

static petalstruct *petals = NULL;

/* 
 * Macro:
 *   sine
 *   cosine
 * Description:
 *   Assume that degrees is .. oh 360... meaning that
 *   there are 360 degress in a circle.  Then this function
 *   would return the sin of the angle in degrees.  But lets
 *   say that they are really big degrees, with 4 big degrees
 *   the same as one regular degree.  Then this routine
 *   would be called mysin(t, 90) and would return sin(t degrees * 4)
 */
#define sine(t, degrees) sin(t * TWOPI / (degrees))
#define cosine(t, degrees) cos(t * TWOPI / (degrees))

/* 
 * Macro:
 *   rand_range
 * Description:
 *   Return a random number between a inclusive  and b exclusive.
 *    rand (3, 6) returns 3 or 4 or 5, but not 6.
 */
#define rand_range(a, b) (a + NRAND(b - a))

static int
gcd(int m, int n)
/* 
 * Greatest Common Divisor (also Greatest common factor).
 */
{
	int         r;

	for (;;) {
		r = m % n;
		if (r == 0)
			return (n);
		m = n;
		n = r;
	}
}

static int
numlines(int a, int b, int d)
/* 
 * Description:
 *
 *      Given parameters a and b, how many lines will we have to draw?
 *
 * Algorithm:
 *
 *      This algorithm assumes that r = sin (theta * a), where we
 *      evaluate theta on multiples of b.
 *
 *      LCM (i, j) = i * j / GCD (i, j);
 *
 *      So, at LCM (b, 360) we start over again.  But since we
 *      got to LCM (b, 360) by steps of b, the number of lines is
 *      LCM (b, 360) / b.
 *
 *      If a is odd, then at 180 we cross over and start the
 *      negative.  Someone should write up an elegant way of proving
 *      this.  Why?  Because I'm not convinced of it myself.
 *
 */
{
#define odd(x) (x & 1)
#define even(x) (!odd(x))
	if (odd(a) && odd(b) && even(d))
		d /= 2;
	return (d / gcd(d, b));
#undef odd
}

static int
compute_petal(XPoint * points, int lines, int sizex, int sizey)
/* 
 * Description:
 *
 *    Basically, it's combination spirograph and string art.
 *    Instead of doing lines, we just use a complex polygon,
 *    and use an even/odd rule for filling in between.
 *
 *    The spirograph, in mathematical terms is a polar
 *    plot of the form r = sin (theta * c);
 *    The string art of this is that we evaluate that
 *    function only on certain multiples of theta.  That is
 *    we let theta advance in some random increment.  And then
 *    we draw a straight line between those two adjacent points.
 *
 *    Eventually, the lines will start repeating themselves
 *    if we've evaluated theta on some rational portion of the
 *    whole.
 *
 *    The number of lines generated is limited to the
 *    ratio of the increment we put on theta to the whole.
 *    If we say that there are 360 degrees in a circle, then we
 *    will never have more than 360 lines.
 *
 * Return:
 *
 *    The number of points.
 *
 */
{
	int         a, b, d;	/* These describe a unique petal */

	double      r;
	int         theta = 0;
	XPoint     *p = points;
	int         count;
	int         numpoints;

	for (;;) {
		d = lines;

		a = rand_range(1, d);
		b = rand_range(1, d);
		numpoints = numlines(a, b, d);
		if (numpoints >= MINLINES)
			break;
	}

	/* it might be nice to try to move as much sin and cos computing
	 * (or at least the argument computing) out of the loop.
	 */
	for (count = numpoints; count--;) {
		r = sine(theta * a, d);

		/* Convert from polar to cartesian coordinates */
		/* We could round the results, but coercing seems just fine */
		p->x = sine(theta, d) * r * sizex + sizex;
		p->y = cosine(theta, d) * r * sizey + sizey;

		/* Advance index into array */
		p++;

		/* Advance theta */
		theta += b;
		theta %= d;
	}

	return (numpoints);
}

static void
random_petal(ModeInfo * mi)
{
	petalstruct *pp = &petals[MI_SCREEN(mi)];
	int         numpoints;

	numpoints = compute_petal(pp->points, pp->lines,
				  pp->width / 2, pp->height / 2);

	if (MI_NPIXELS(mi) > 2)
		XSetForeground(MI_DISPLAY(mi), MI_GC(mi),
			       MI_PIXEL(mi, NRAND(MI_NPIXELS(mi))));
	XFillPolygon(MI_DISPLAY(mi), MI_WINDOW(mi), MI_GC(mi),
		     pp->points, numpoints, Complex, CoordModeOrigin);
}

void
init_petal(ModeInfo * mi)
{
	petalstruct *pp;

	if (petals == NULL) {
		if ((petals = (petalstruct *) calloc(MI_NUM_SCREENS(mi),
					      sizeof (petalstruct))) == NULL)
			return;
	}
	pp = &petals[MI_SCREEN(mi)];

	pp->lines = MI_BATCHCOUNT(mi);
	if (pp->lines > MAXLINES)
		pp->lines = MAXLINES;
	else if (pp->lines < -MINLINES) {
		if (pp->points) {
			(void) free((void *) pp->points);
			pp->points = NULL;
		}
		pp->lines = NRAND(-pp->lines - MINLINES + 1) + MINLINES;
	} else if (pp->lines < MINLINES)
		pp->lines = MINLINES;
	if (!pp->points)
		pp->points = (XPoint *) malloc(pp->lines * sizeof (XPoint));
	pp->width = MI_WIN_WIDTH(mi);
	pp->height = MI_WIN_HEIGHT(mi);

	pp->time = 0;
	XSetForeground(MI_DISPLAY(mi), MI_GC(mi), MI_WIN_WHITE_PIXEL(mi));

	XClearWindow(MI_DISPLAY(mi), MI_WINDOW(mi));

	random_petal(mi);
}

void
draw_petal(ModeInfo * mi)
{
	if (++petals[MI_SCREEN(mi)].time > MI_CYCLES(mi))
		init_petal(mi);
}

void
release_petal(ModeInfo * mi)
{
	if (petals != NULL) {
		int         screen;

		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
			petalstruct *pp = &petals[screen];

			if (pp->points)
				(void) free((void *) pp->points);
		}
		(void) free((void *) petals);
		petals = NULL;
	}
}
