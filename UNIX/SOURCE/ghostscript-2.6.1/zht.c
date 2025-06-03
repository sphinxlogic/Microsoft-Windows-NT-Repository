/* Copyright (C) 1989, 1991 Aladdin Enterprises.  All rights reserved.

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

/* zht.c */
/* Halftone operators and rendering for Ghostscript */
#include "ghost.h"
#include "memory_.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "estack.h"
#include "gsmatrix.h"
#include "gsstate.h"
#include "state.h"
#include "store.h"

/* Forward references */
private int screen_sample(P1(os_ptr));
private int set_screen_continue(P1(os_ptr));
private int screen_cleanup(P1(os_ptr));

/* - currentscreen <frequency> <angle> <proc> */
/* - currentscreen 60 0 <dict> */
int
zcurrentscreen(register os_ptr op)
{	if ( r_has_type(&istate.halftone, t_null) )
	{	/* Screen was set by setscreen. */
		float freq, angle;
		float (*proc)(P2(floatp, floatp));
		gs_currentscreen(igs, &freq, &angle, &proc);
		push(3);
		make_real(op - 2, freq);
		make_real(op - 1, angle);
		*op = istate.screen_procs.gray;
	}
	else
	{	/* Screen was set by sethalftone. */
		push(3);
		make_real(op - 2, 60);
		make_real(op - 1, 0);
		*op = istate.halftone;
	}
	return 0;
}

/* The setscreen operator is complex because it has to sample */
/* each pixel in the pattern cell, calling a procedure, and then */
/* sort the result into a whitening order. */

/* Layout of stuff pushed on estack: */
/*	Control mark, */
/*	spot procedure, */
/*	enumeration structure (as bytes). */
#define snumpush 3
#define sproc esp[-1]
#define senum (gs_screen_enum *)esp->value.bytes

/* <frequency> <angle> <proc> setscreen - */
int
zsetscreen(register os_ptr op)
{	float fa[2];
	int code = num_params(op - 1, 2, fa);
	gs_screen_enum *penum;
	if ( code < 0 ) return code;
	check_proc(*op);
	check_estack(snumpush);
	penum = (gs_screen_enum *)alloc(1, gs_screen_enum_sizeof, "setscreen");
	if ( penum == 0 )
		return_error(e_VMerror);
	/* Push everything on the estack */
	make_mark_estack(esp + 1, es_other, screen_cleanup);
	esp[2] = *op;			/* sproc = proc */
	make_tasv(esp + 3, t_string, 0, gs_screen_enum_sizeof, bytes, (byte *)penum);
	code = gs_screen_init(penum, igs, fa[0], fa[1]);
	if ( code < 0 )
	   {	screen_cleanup(op);
		return code;
	   }
	esp += snumpush;
	pop(3);  op -= 3;
	return screen_sample(op);
}
/* Set up the next sample */
private int
screen_sample(register os_ptr op)
{	gs_screen_enum *penum = senum;
	gs_point pt;
	int code = gs_screen_currentpoint(penum, &pt);
	ref proc;
	if ( code < 0 ) return code;
	if ( code != 0 )
	   {	/* All done */
		istate.screen_procs.red = sproc;
		istate.screen_procs.green = sproc;
		istate.screen_procs.blue = sproc;
		istate.screen_procs.gray = sproc;
		make_null(&istate.halftone);
		esp -= snumpush;
		screen_cleanup(op);
		return o_pop_estack;
	   }
	push(2);
	make_real(op - 1, pt.x);
	make_real(op, pt.y);
	proc = sproc;
	push_op_estack(set_screen_continue);
	*++esp = proc;
	return o_push_estack;
}
/* Continuation procedure for processing sampled pixels. */
private int
set_screen_continue(register os_ptr op)
{	float value;
	int code = num_params(op, 1, &value);
	if ( code < 0 ) return code;
	code = gs_screen_next(senum, value);
	if ( code < 0 ) return code;
	pop(1);  op--;
	return screen_sample(op);
}
/* Clean up after screen enumeration */
private int
screen_cleanup(os_ptr op)
{	alloc_free((char *)esp[snumpush].value.bytes,
		   1, gs_screen_enum_sizeof, "screen_cleanup");
	return 0;
}

/* ------ Initialization procedure ------ */

op_def zht_op_defs[] = {
	{"0currentscreen", zcurrentscreen},
	{"3setscreen", zsetscreen},
		/* Internal operators */
	{"1%set_screen_continue", set_screen_continue},
	op_def_end(0)
};
