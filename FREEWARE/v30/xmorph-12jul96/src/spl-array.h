/* spl-array.h: Spline interpolation support routines for matrices
**
** Written and Copyright (C) 1993, 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#ifndef _SPL_ARRAY_H__INCLUDED_
#define _SPL_ARRAY_H__INCLUDED_

/* Prototypes */
#ifdef ANSI
#define DECL(s)  s
#else
#define DECL(s)  ()
#endif

extern int hermite3_array DECL((double *kx, double *ky, long nk, double *sx, double *sy, long ns));

#endif
