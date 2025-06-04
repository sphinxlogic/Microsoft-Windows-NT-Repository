/* spl-array.c: Spline interpolation support routines for matrices
**
** Written and Copyright (C) 1993, 1994, 1995 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "spline.h"
#include "spl-array.h"
#include "my_malloc.h"

/* --------------------------------------------------------------- */

#define NEAR(x1, x2) (((x2)!=0.0) && (((x1)/(x2)) >= 0.999) && (((x1)/(x2))<1.001))

#ifndef FALSE
#define FALSE 0
#endif

/* --------------------------------------------------------------- */
/* derivative_hack: compute 1st derivative of x,y data (len entries)
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
**
** Mathematically, it's a hack to prevent overshooting knots, but
** maintain smoothness and it works more intuitively.  Besides, we all
** know that mathematicians are too worried about rigor, and the
** physicists end up creating the right math. -- MJG
*/
static void derivative_hack(double *x, double *y, double *yd, int len)
{
  int indx;

  if(x[0]==x[1]) {
    yd[0] = 0.0; /* avoid division by zero */
  } else {
    yd[0] = (y[1]-y[0])/(x[1]-x[0]);
  }
  if(x[len-2] == x[len-1]) {
    yd[len-1] = 0.0; /* avoid division by zero */
  } else {
    yd[len-1] = (y[len-1]-y[len-2])/(x[len-1]-x[len-2]);
  }
  for(indx=1; indx<(len-1); indx++) {
    if(x[indx-1]==x[indx] || x[indx]==x[indx+1]) {
      yd[indx] = 0.0; /* avoid division by zero */
    } else {
      if( (y[indx-1] - y[indx]) * (y[indx+1] - y[indx]) >= 0.0) {
        /* There was a change in the sign of yd so force it zero */
        /* This will prevent the spline from overshooting this knot */
        yd[indx] = 0.0;
      } else {
        /* Set slope at this knot to slope between two adjacent knots */
        yd[indx] = (y[indx-1]-y[indx+1]) / (x[indx-1]-x[indx+1]);
      }
    }
  }
}

/* --------------------------------------------------------------- */
/* hermite3_array : cubic hermite interpolation for an array of points
** Uses derivative_hack to find derivatives.
** input : kx, ky, nk, sx, sy, ns (sy only need be allocated; contents ignored)
** output : sy (spline value at evaluation points sx)
*/
int
hermite3_array(kx, ky, nk, sx, sy, ns)
double *kx, *ky; /* arrays of knots */
long nk;         /* number of knots */
double *sx;      /* evaluation points array (input) */
double *sy;      /* evaluation values array (output) */
long   ns;       /* number of evaluation points */
{
  register long xi;
  double *kyd;

  if((kyd=MY_CALLOC(nk, double, "hermite3_array"))==NULL)
    return(1);

  /* Test bounds. */
  /* As of 18jul94, this test was triggering for cases
  ** where the bounds were nearly equal, but slightly out of range, in
  ** which case the spline should work anyway, which is why I let it run
  ** even if the spline abcissas are out of range.
  */
  if((sx[0] < kx[0]) || (sx[ns-1] > kx[nk-1])) {
    if(!NEAR(sx[ns-1], kx[nk-1])) {
      fprintf(stderr, "hermite3_array: out of range.\n");
      fprintf(stderr, "hermite3_array: %.20lg<%.20lg | %.20lg>%.20lg\n",
        sx[0], kx[0], sx[ns-1], kx[nk-1]);
    }
  }

  /* Find array of derivatives */
  derivative_hack(kx, ky, kyd, nk);

  /* Evaluate the spline */
  for(xi=0; xi<ns; xi++) {
    sy[xi]=hermite3_interp(sx[xi], kx, ky, kyd, nk, NULL, FALSE, NULL, NULL);
  }

  /* hmmm.. I don't think the derivatives are needed any longer so just
  ** let's get rid of them... (AT 03feb95)
  */
  my_free(kyd, "hermite3_array");

  return(0);
}
