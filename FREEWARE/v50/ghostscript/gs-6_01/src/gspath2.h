/* Copyright (C) 1994 Aladdin Enterprises.  All rights reserved.

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

/*$Id: gspath2.h,v 1.1 2000/03/09 08:40:42 lpd Exp $ */
/* Level 2 graphics state path procedures */
/* Requires gsmatrix.h */

#ifndef gspath2_INCLUDED
#  define gspath2_INCLUDED

/* Miscellaneous */
int gs_setbbox(P5(gs_state *, floatp, floatp, floatp, floatp));

/* Rectangles */
int gs_rectappend(P3(gs_state *, const gs_rect *, uint));
int gs_rectclip(P3(gs_state *, const gs_rect *, uint));
int gs_rectfill(P3(gs_state *, const gs_rect *, uint));
int gs_rectstroke(P4(gs_state *, const gs_rect *, uint, const gs_matrix *));

#endif /* gspath2_INCLUDED */
