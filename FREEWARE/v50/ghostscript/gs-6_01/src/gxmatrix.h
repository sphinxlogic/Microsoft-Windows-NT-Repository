/* Copyright (C) 1989, 1995, 1996, 1997, 1999 Aladdin Enterprises.  All rights reserved.

   This file is part of Aladdin Ghostscript.

   Aladdin Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author
   or distributor accepts any responsibility for the consequences of using it,
   or for whether it serves any particular purpose or works at all, unless he
   or she says so in writing.  Refer to the Aladdin Ghostscript Free Public
   License (the "License") for full details.

   Every copy of Aladdin Ghostscript must include a copy of the License,
   normally in a plain ASCII text file named PUBLIC.  The License grants you
   the right to copy, modify and redistribute Aladdin Ghostscript, but only
   under certain conditions described in the License.  Among other things, the
   License requires that the copyright notice and this notice be preserved on
   all copies.
 */

/*$Id: gxmatrix.h,v 1.1 2000/03/09 08:40:43 lpd Exp $ */
/* Internal matrix routines for Ghostscript library */

#ifndef gxmatrix_INCLUDED
#  define gxmatrix_INCLUDED

#include "gsmatrix.h"

/*
 * Define a matrix with a cached fixed-point copy of the translation.
 * This is only used by a few routines in gscoord.c; they are responsible
 * for ensuring the validity of the cache.  Note that the floating point
 * tx/ty values may be too large to fit in a fixed values; txy_fixed_valid
 * is false if this is the case, and true otherwise.
 */
typedef struct gs_matrix_fixed_s {
    _matrix_body;
    fixed tx_fixed, ty_fixed;
    bool txy_fixed_valid;
} gs_matrix_fixed;

/* Make a gs_matrix_fixed from a gs_matrix. */
int gs_matrix_fixed_from_matrix(P2(gs_matrix_fixed *, const gs_matrix *));

/* Coordinate transformations to fixed point. */
int gs_point_transform2fixed(P4(const gs_matrix_fixed *, floatp, floatp,
				gs_fixed_point *));
int gs_distance_transform2fixed(P4(const gs_matrix_fixed *, floatp, floatp,
				   gs_fixed_point *));

/*
 * Define the fixed-point coefficient structure for avoiding
 * floating point in coordinate transformations.
 * Currently this is used only by the Type 1 font interpreter.
 * The setup is in gscoord.c.
 */
typedef struct {
    long l;
    fixed f;
} coeff1;
typedef struct {
    coeff1 xx, xy, yx, yy;
    int skewed;
    int shift;			/* see m_fixed */
    int max_bits;		/* max bits of coefficient */
    fixed round;		/* ditto */
} fixed_coeff;

/*
 * Multiply a fixed whose integer part usually does not exceed max_bits
 * in magnitude by a coefficient from a fixed_coeff.
 * We can use a faster algorithm if the fixed is an integer within
 * a range that doesn't cause the multiplication to overflow an int.
 */
#define m_fixed(v, c, fc, maxb)\
  (((v) + (fixed_1 << (maxb - 1))) &\
    ((-fixed_1 << maxb) | _fixed_fraction_v) ?	/* out of range, or has fraction */\
   (fixed2int_var(v) * (fc).c.f +\
    arith_rshift(fixed_fraction(v) * (fc).c.f + fixed_half, _fixed_shift)) :\
   arith_rshift(fixed2int_var(v) * (fc).c.l + (fc).round, (fc).shift))

#endif /* gxmatrix_INCLUDED */
