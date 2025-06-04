/* spline.c: Hermite spline interpolation
** Spring 1993
**
** Written and Copyright (C) 1993, 1994, 1995 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "spline.h"
#include "my_malloc.h"

/* --------------------------------------------------------------- */

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a)   ((a)>=0?(a):(-(a)))

/* --------------------------------------------------------------- */
/* spline3_setup: set parameters for natural cubic spline
** input: x, y, n
** output: c, h
*/
void spline3_setup(x, y, n, c, h)
  double *x, *y; /* knots */
  long   n;      /* number of knots */
  double *c;     /* spline parameters */
  double *h;     /* intervals: h[i]=x[i+1]-x[i] */
{
  double *u, *v;
  long i;

  /* only need u and v to index from 1 to n-1 */
  u=MY_CALLOC(n, double, "spline3_setup");
  v=MY_CALLOC(n, double, "spline3_setup");

  for(i=0; i<n; i++) {
    h[i]=x[i+1]-x[i];
  }

  for(i=1; i<n; i++) {
    v[i]=3.0/h[i]*(y[i+1]-y[i]) - 3.0/h[i-1]*(y[i]-y[i-1]);
    u[i]=2.0*(h[i]+h[i-1]);
  }

  c[0]=c[n]=0;
  for(i=n-1; i>0; i--) {
    c[i]=(v[i]-h[i]*c[i+1])/u[i];
  }

  my_free(u, "spline3_setup"); /* my_ added 13sep95 AT, MJG */
  my_free(v, "spline3_setup");
}

/* --------------------------------------------------------------- */
/* spline3_eval -- evaluate the natural cubic spline
** input : w, x, y, n, c, h
** output : s1, s2
** return: spline evaluation
**  if s1==NULL then s1 is not evaluated
**  if s2==NULL then s2 is not evaluated
*/
double spline3_eval(w, x, y, n, c, h, s1, s2)
  double w;   /* argument, point at which spline is evaluated */
  double *x;  /* array of knot x values, in increasing order */
  double *y;  /* array of knot y values */
  long n;     /* number of x's and y's */
  double *c;  /* array of parameters from spline3_setup */
  double *h;  /* array of intervals between x's */
  double *s1; /* spline first derivative */
  double *s2; /* spline second derivative */
{
  double diff=0.0;
  double b, d;
  long i;

  /* find interval of spline to evaluate */
  for(i=n-1; (i>=0) && ((diff=(w-x[i]))<0.0); i--)
   ;

  /* calculate other spline parameters */
  /* here a=y[i] so it is not explicitly named a */
  b = (y[i+1]-y[i])/h[i] - h[i]/3.0*(2.0*c[i] + c[i+1]);
  d = (c[i+1]-c[i])/h[i];

  /* evaluate derivatives of spline */
  if(s1!=NULL) *s1 = b + diff*(2.0*c[i] + 3.0*d*diff);
  if(s2!=NULL) *s2 = 2.0*(c[i] + 3.0*d*diff);

  /* return spline value */
  return(y[i] + diff*(b + diff*(c[i] + diff*d)));
}


/* --------------------------------------------------------------- */
/* d_parabola -- returns the derivative of a parabola fit through 3 points */
double d_parabola(x, xp0, xp1, xp2, yp0, yp1, yp2)
  double x;          /* place on parabola where derivative is taken */
  double xp0, xp1, xp2; /* points through which parabola passes */
  double yp0, yp1, yp2;
{
  double dP=(  xp0*(yp1-yp2)*(2*x - xp0)
             + xp1*(yp2-yp0)*(2*x - xp1)
             + xp2*(yp0-yp1)*(2*x - xp2)) / ((xp0-xp1)*(xp0-xp2)*(xp2-xp1));

  return(dP);
}

/* --------------------------------------------------------------- */
/* hermite3_interp : cubic hermite interpolation
** input : w, x, y, d, n, f, find_d
** output : h1, h2
** return : evaluation of interpolation
*/
double hermite3_interp(w, x, y, d, n, f, find_d, h1, h2)
  double w;      /* evaluation point */
  double *x, *y; /* arrays of knots */
  double *d;     /* derivatives */
  long n;        /* number of knots */
  double (*f)(); /* derivative function, NULL=>approximate from knots */
  int find_d;    /* find_derivative flag, 0=>use d, 1=>find d */
  double *h1;    /* first derivative of spline, NULL=>ignore */
  double *h2;    /* second derivative of spline, NULL=>ignore */
{
  double A, B;
  double h, h_2;
  double diff=0.0;
  double H;
  long si;

  /* find interval of spline to evaluate */
  /* MJG 14jul95 -- do not start at si=n-1.  See "h =" line below. */
  /* MJG 18jul94 -- was reading beyond bounds at last knot. (maybe) */
  for(si=n-2; (si>=0) && ((diff=(w-x[si]))<0.0); si--)
    ;

  /* h is the interval between knots */
  h = x[si+1]-x[si];
  h_2 = h*h;

  /* either the derivatives were provided or must be found */
  if(find_d) {
    /* must calculate derivatives */
    if(f!=NULL) {
      /* calculate the derivative */
      d[si]=(*f)(x[si]);
      d[si+1]=(*f)(x[si+1]);
    } else {
      /* approximate derivative using parabola fit */
      if(si==0) { /* at first knot */
        d[si]=d_parabola(x[si], x[si], x[si+1], x[si+2],
                              y[si], y[si+1], y[si+2]);
        d[si+1]=d_parabola(x[si+1], x[si], x[si+1], x[si+2],
                                  y[si], y[si+1], y[si+2]);
      } else if(si>=(n-2)) { /* at or near last knot */
        d[si]=d_parabola(x[si], x[si-1], x[si], x[si+1],
                              y[si-1], y[si], y[si+1]);
        d[si+1]=d_parabola(x[si+1], x[si-1], x[si], x[si+1],
                                  y[si-1], y[si], y[si+1]);
      } else { /* between first and 2nd to last knot */
        d[si]=d_parabola(x[si], x[si-1], x[si], x[si+1],
                              y[si-1], y[si], y[si+1]);
        d[si+1]=d_parabola(x[si+1], x[si], x[si+1], x[si+2],
                                  y[si], y[si+1], y[si+2]);
      }
    }
  }

  /* calculate interpolant parameters */
  A = (y[si+1] - y[si] - h*d[si])/h_2;
  B = (d[si+1] - d[si] -2.0*h*A)/h_2;

  /* evaluate spline derivatives */
  if(h1!=NULL) *h1 = d[si] + diff*(2.0*A + B*(diff + 2.0*(w-x[si+1])));
  if(h2!=NULL) *h2 = 2.0*A + 2.0*B*(2.0*diff + (w-x[si+1]));

  /* return the spline evaluation */
  H  =  y[si] + diff*(d[si] + diff*(A + (w-x[si+1])*B));

  return(H);
}
