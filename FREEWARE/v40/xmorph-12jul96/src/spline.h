/* spline.h: Interpolation
**
** Written and Copyright (C) 1993, 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#ifndef _SPLINE_H__INCLUDED_
#define _SPLINE_H__INCLUDED_

/* Prototypes */
#ifdef ANSI
#define DECL(s)  s
#else
#define DECL(s)  ()
#endif

extern void spline3_setup DECL((double *, double *, long, double *, double *));

extern double spline3_eval DECL((double, double *, double *, long, double *, double *, double *, double *));

extern double hermite3_interp DECL((double, double *, double *, double *, long, double (*f)(), int, double *, double *));

#endif
