/* Copyright (C) 1989, 1992, 1993 Aladdin Enterprises.  All rights reserved.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* gscoord.h */
/* Coordinate system operators for Ghostscript library */
/* Requires gsmatrix.h and gsstate.h */

/* Coordinate system modification */
int	gs_initmatrix(P1(gs_state *)),
	gs_defaultmatrix(P2(const gs_state *, gs_matrix *)),
	gs_currentmatrix(P2(const gs_state *, gs_matrix *)),
	gs_currentcharmatrix(P3(gs_state *, gs_matrix *, int)),
	gs_setmatrix(P2(gs_state *, const gs_matrix *)),
	gs_translate(P3(gs_state *, floatp, floatp)),
	gs_scale(P3(gs_state *, floatp, floatp)),
	gs_rotate(P2(gs_state *, floatp)),
	gs_concat(P2(gs_state *, const gs_matrix *));

/* Coordinate transformation */
int	gs_transform(P4(gs_state *, floatp, floatp, gs_point *)),
	gs_dtransform(P4(gs_state *, floatp, floatp, gs_point *)),
	gs_itransform(P4(gs_state *, floatp, floatp, gs_point *)),
	gs_idtransform(P4(gs_state *, floatp, floatp, gs_point *));

/* Inline versions of some of the transformations */
#define gs_idtransform_inline(pgs, dx, dy, pt)\
  (is_skewed(&(pgs)->ctm) ? gs_idtransform(pgs, dx, dy, pt) :\
   is_fzero((pgs)->ctm.xx) || is_fzero((pgs)->ctm.yy) ?\
     gs_note_error(gs_error_undefinedresult) :\
   ((pt)->x = (dx) / (pgs)->ctm.xx, (pt)->y = (dy) / (pgs)->ctm.yy, 0))
