/* misc handy functions.
 *
 * these are the same as used by xephem but I didn't want to make a strict
 *   dependence on its source files (I might if the common list gets too big).
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "gscdbd.h"

/* solve a spherical triangle:
 *           A
 *          /  \
 *         /    \
 *      c /      \ b
 *       /        \
 *      /          \
 *    B ____________ C
 *           a
 *
 * given A, b, c find B and a in range 0..B..2PI, 0..a..PI
 * N.B. we pass in cos(c) and sin(c) because in many problems one of the sides
 *   remains constant for many values of A and b.
 */
void
solve_sphere (A, b, cc, sc, cap, Bp)
double A, b;
double cc, sc;
double *cap, *Bp;
{
	double cA = cos(A), sA = sin(A);
	double cb = cos(b), sb = sin(b);
	double ca;
	double B;
	double x, y;

	ca = cb*cc + sb*sc*cA;
	if (ca >  1.0) ca =  1.0;
	if (ca < -1.0) ca = -1.0;

	if (cc > .99999) {
	    /* as c approaches 0, B approaches pi - A */
	    B = PI - A;
	} else if (cc < -.99999) {
	    /* as c approaches PI, B approaches A */
	    B = A;
	} else {
	    /* compute cB and sB and remove common factor of sa from quotient.
	     * be careful where B causes atan to blow.
	     */
	    y = sA*sb*sc;
	    x = cb - ca*cc;
	
	    if (fabs(x) < 1e-5)
		B = y < 0 ? 3*PI/2 : PI/2;
	    else
		B = atan2 (y, x);
	}

	*cap = ca;
	*Bp = B;
	range (Bp, 2*PI);
}

/* insure 0 <= *v < r.
 */
void
range (v, r)
double *v, r;
{
	*v -= r*floor(*v/r);
}

/* sprint the variable a in sexagesimal format into out[].
 * w is the number of spaces for the whole part.
 * fracbase is the number of pieces a whole is to broken into; valid options:
 *	360000:	<w>:mm:ss.ss
 *	36000:	<w>:mm:ss.s
 *	3600:	<w>:mm:ss
 *	600:	<w>:mm.m
 *	60:	<w>:mm
 */
void
fs_sexa (out, a, w, fracbase)
char *out;
double a;
int w;
int fracbase;
{
	unsigned long n;
	int d;
	int f;
	int m;
	int s;
	int isneg;

	/* save whether it's negative but do all the rest with a positive */
	isneg = (a < 0);
	if (isneg)
	    a = -a;

	/* convert to an integral number of whole portions */
	n = (unsigned long)(a * fracbase + 0.5);
	d = n/fracbase;
	f = n%fracbase;

	/* form the whole part; "negative 0" is a special case */
	if (isneg && d == 0)
	    (void) sprintf (out, "%*s-0", w-2, "");
	else
	    (void) sprintf (out, "%*d", w, isneg ? -d : d);
	out += w;

	/* do the rest */
	switch (fracbase) {
	case 60:	/* dd:mm */
	    m = f/(fracbase/60);
	    (void) sprintf (out, ":%02d", m);
	    break;
	case 600:	/* dd:mm.m */
	    (void) sprintf (out, ":%02d.%1d", f/10, f%10);
	    break;
	case 3600:	/* dd:mm:ss */
	    m = f/(fracbase/60);
	    s = f%(fracbase/60);
	    (void) sprintf (out, ":%02d:%02d", m, s);
	    break;
	case 36000:	/* dd:mm:ss.s*/
	    m = f/(fracbase/60);
	    s = f%(fracbase/60);
	    (void) sprintf (out, ":%02d:%02d.%1d", m, s/10, s%10);
	    break;
	case 360000:	/* dd:mm:ss.ss */
	    m = f/(fracbase/60);
	    s = f%(fracbase/60);
	    (void) sprintf (out, ":%02d:%02d.%02d", m, s/100, s%100);
	    break;
	default:
	    printf ("fs_sexa: unknown fracbase: %d\n", fracbase);
	    exit(1);
	}
}
