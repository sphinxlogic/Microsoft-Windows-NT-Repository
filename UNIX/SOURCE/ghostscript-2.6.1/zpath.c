/* Copyright (C) 1989, 1990, 1991, 1993 Aladdin Enterprises.  All rights reserved.

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

/* zpath.c */
/* Path operators for Ghostscript */
#include "math_.h"
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "gsmatrix.h"
#include "gspath.h"
#include "state.h"
#include "store.h"

/* Forward references */
private int near common_to(P2(os_ptr,
  int (*)(P3(gs_state *, floatp, floatp))));
private int near common_arc(P2(os_ptr,
  int (*)(P6(gs_state *, floatp, floatp, floatp, floatp, floatp))));
private int near common_arct(P2(os_ptr, float *));
private int near common_curve(P2(os_ptr,
  int (*)(P7(gs_state *, floatp, floatp, floatp, floatp, floatp, floatp))));

/* - newpath - */
int
znewpath(register os_ptr op)
{	return gs_newpath(igs);
}

/* - currentpoint <x> <y> */
int
zcurrentpoint(register os_ptr op)
{	gs_point pt;
	int code = gs_currentpoint(igs, &pt);
	if ( code < 0 ) return code;
	push(2);
	make_real(op - 1, pt.x);
	make_real(op, pt.y);
	return 0;
}

/* <x> <y> moveto - */
int
zmoveto(os_ptr op)
{	return common_to(op, gs_moveto);
}

/* <dx> <dy> rmoveto - */
int
zrmoveto(os_ptr op)
{	return common_to(op, gs_rmoveto);
}

/* <x> <y> lineto - */
int
zlineto(os_ptr op)
{	return common_to(op, gs_lineto);
}

/* <dx> <dy> rlineto - */
int
zrlineto(os_ptr op)
{	return common_to(op, gs_rlineto);
}

/* Common code for [r](move/line)to */
private int near
common_to(os_ptr op, int (*add_proc)(P3(gs_state *, floatp, floatp)))
{	float opxy[2];
	int code;
	if (	(code = num_params(op, 2, opxy)) < 0 ||
		(code = (*add_proc)(igs, opxy[0], opxy[1])) < 0
	   ) return code;
	pop(2);
	return 0;
}

/* <x> <y> <r> <ang1> <ang2> arc - */
int
zarc(os_ptr op)
{	return common_arc(op, gs_arc);
}

/* <x> <y> <r> <ang1> <ang2> arcn - */
int
zarcn(os_ptr op)
{	return common_arc(op, gs_arcn);
}

/* Common code for arc[n] */
private int near
common_arc(os_ptr op,
  int (*aproc)(P6(gs_state *, floatp, floatp, floatp, floatp, floatp)))
{	float xyra[5];			/* x, y, r, ang1, ang2 */
	int code;
	if ( (code = num_params(op, 5, xyra)) < 0 ) return code;
	code = (*aproc)(igs, xyra[0], xyra[1], xyra[2], xyra[3], xyra[4]);
	if ( code >= 0 ) pop(5);
	return code;
}

/* <x1> <y1> <x2> <y2> <r> arct - */
int
zarct(register os_ptr op)
{	int code = common_arct(op, (float *)0);
	if ( code < 0 ) return code;
	pop(5);
	return 0;
}

/* <x1> <y1> <x2> <y2> <r> arcto <xt1> <yt1> <xt2> <yt2> */
int
zarcto(register os_ptr op)
{	float tanxy[4];			/* xt1, yt1, xt2, yt2 */
	int code = common_arct(op, tanxy);
	if ( code < 0 ) return code;
	make_real(op - 4, tanxy[0]);
	make_real(op - 3, tanxy[1]);
	make_real(op - 2, tanxy[2]);
	make_real(op - 1, tanxy[3]);
	pop(1);
	return 0;
}

/* Common code for arct[o] */
private int near
common_arct(os_ptr op, float *tanxy)
{	float args[5];			/* x1, y1, x2, y2, r */
	int code;
	if ( (code = num_params(op, 5, args)) < 0 ) return code;
	return gs_arcto(igs, args[0], args[1], args[2], args[3], args[4], tanxy);
}

/* <x1> <y1> <x2> <y2> <x3> <y3> curveto - */
int
zcurveto(register os_ptr op)
{	return common_curve(op, gs_curveto);
}

/* <dx1> <dy1> <dx2> <dy2> <dx3> <dy3> rcurveto - */
int
zrcurveto(register os_ptr op)
{	return common_curve(op, gs_rcurveto);
}

/* Common code for [r]curveto */
private int near
common_curve(os_ptr op,
  int (*add_proc)(P7(gs_state *, floatp, floatp, floatp, floatp, floatp, floatp)))
{	float opxy[6];
	int code;
	if ( (code = num_params(op, 6, opxy)) < 0 ) return code;
	code = (*add_proc)(igs, opxy[0], opxy[1], opxy[2], opxy[3], opxy[4], opxy[5]);
	if ( code >= 0 ) pop(6);
	return code;
}

/* - closepath - */
int
zclosepath(register os_ptr op)
{	return gs_closepath(igs);
}

/* ------ Initialization procedure ------ */

op_def zpath_op_defs[] = {
	{"5arc", zarc},
	{"5arcn", zarcn},
	{"5arct", zarct},
	{"5arcto", zarcto},
	{"0closepath", zclosepath},
	{"0currentpoint", zcurrentpoint},
	{"6curveto", zcurveto},
	{"2lineto", zlineto},
	{"2moveto", zmoveto},
	{"0newpath", znewpath},
	{"6rcurveto", zrcurveto},
	{"2rlineto", zrlineto},
	{"2rmoveto", zrmoveto},
	op_def_end(0)
};
