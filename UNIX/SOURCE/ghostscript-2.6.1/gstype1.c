/* Copyright (C) 1990, 1992, 1993 Aladdin Enterprises.  All rights reserved.

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

/* gstype1.c */
/* Adobe Type 1 font routines for Ghostscript library */
#include "math_.h"
#include "memory_.h"
#include "gx.h"
#include "gserrors.h"
#include "gxarith.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gzstate.h"
#include "gzdevice.h"			/* for gxchar */
#include "gxdevmem.h"			/* ditto */
#include "gzpath.h"
#include "gxchar.h"
#include "gxfont.h"
#include "gxtype1.h"
#include "gxop1.h"

/* Define whether to always do Flex segments as curves. */
/* This is only an issue because some old Adobe DPS fonts */
/* seem to violate the Flex specification in a way that requires this. */
#define ALWAYS_DO_FLEX_AS_CURVE 1

/* Encrypt a string. */
int
gs_type1_encrypt(byte *dest, const byte *src, uint len, crypt_state *pstate)
{	register crypt_state state = *pstate;
	register const byte *from = src;
	register byte *to = dest;
	register uint count = len;
	while ( count )
	   {	encrypt_next(*from, state, *to);
		from++, to++, count--;
	   }
	*pstate = state;
	return 0;
}
/* Decrypt a string. */
int
gs_type1_decrypt(byte *dest, const byte *src, uint len, crypt_state *pstate)
{	register crypt_state state = *pstate;
	register const byte *from = src;
	register byte *to = dest;
	register uint count = len;
	while ( count )
	   {	/* If from == to, we can't use the obvious */
		/*	decrypt_next(*from, state, *to);	*/
		register byte ch = *from++;
		decrypt_next(ch, state, *to);
		to++, count--;
	   }
	*pstate = state;
	return 0;
}

/* Export the size of the structure */
const uint gs_type1_state_sizeof = sizeof(gs_type1_state);

/* Imported procedures */
extern int gx_matrix_to_fixed_coeff(P3(gs_matrix *, fixed_coeff *, int));

/* Initialize a Type 1 interpreter. */
/* The caller must supply a string to the first call of gs_type1_interpret. */
int
gs_type1_init(register gs_type1_state *pis, gs_show_enum *penum,
  int charpath_flag, int paint_type, gs_type1_data *pdata)
{	gs_state *pgs = penum->pgs;
	int log2_scale = 0;	/* change eventually */
	pis->penum = penum;
	pis->pgs = pgs;
	pis->pdata = pdata;
	pis->charpath_flag = charpath_flag;
	pis->paint_type = paint_type;
	pis->os_count = 0;
	pis->ips_count = 1;
	pis->sb_set = 0;
	pis->seac_base = -1;
	pis->flex_count = flex_max;		/* not in Flex */
	pis->in_dotsection = 0;
	pis->vstem3_set = 0;
	pis->vs_offset.x = pis->vs_offset.y = 0;
	set_pixel_scale(&pis->scale, log2_scale);
	reset_stem_hints(pis);
	gx_matrix_to_fixed_coeff(&ctm_only(pgs), &pis->fc, max_coeff_bits);
	compute_font_hints(&pis->fh, &pgs->ctm, log2_scale, pdata);
	/* Set the current point of the path to the origin, */
	/* in anticipation of the initial [h]sbw. */
	{ gx_path *ppath = pgs->path;
	  ppath->position.x = pis->position.x = pgs->ctm.tx_fixed;
	  ppath->position.y = pis->position.y = pgs->ctm.ty_fixed;
	}
	/* Set the flatness to a value that is likely to produce */
	/* reasonably good-looking curves, regardless of its */
	/* current value in the graphics state. */
	   {	/* If the character is very small, set the flatness */
		/* to zero, which will produce very accurate curves. */
		float cxx = fabs(pgs->ctm.xx), cyy = fabs(pgs->ctm.yy);
		if ( cyy > cxx ) cxx = cyy;
		if ( is_skewed(&pgs->ctm) )
		   {	float cxy = fabs(pgs->ctm.xy), cyx = fabs(pgs->ctm.yx);
			if ( cxy > cxx ) cxx = cxy;
			if ( cyx > cxx ) cxx = cyx;
		   }
		pis->flatness = (cxx >= 0.2 ? cxx : 0.0);
	   }
	return 0;
}

/* Tracing for type 1 interpreter */
#ifdef DEBUG
#  define dc(str) if ( gs_debug['1'] ) type1_trace(cip, c, str);
private void near
type1_trace(const byte *cip, byte c, const char _ds *str)
{	dprintf3("[1]%lx: %02x %s\n", (ulong)(cip - 1), c, (char *)str);
}
#else
#  define dc(str)
#endif

/* Define the state used by operator procedures. */
/* These macros refer to a current instance (s) of gs_op1_state. */
#define sppath s.ppath
#define sfc s.fc
#define ptx s.px
#define pty s.py

/* Accumulate relative coordinates */
/****** THESE ARE NOT ACCURATE FOR NON-INTEGER DELTAS. ******/
/* This probably doesn't make any difference in practice. */
#define c_fixed(d, c) m_fixed(d, c, sfc, max_coeff_bits)
#define accum_x(dx)\
    ptx += c_fixed(dx, xx);\
    if ( sfc.skewed ) pty += c_fixed(dx, xy)
#define accum_y(dy)\
    pty += c_fixed(dy, yy);\
    if ( sfc.skewed ) ptx += c_fixed(dy, yx)
#define accum_xy(dx,dy)\
    accum_xy_proc(&s, dx, dy)

#define s (*ps)

private void near
accum_xy_proc(register is_ptr ps, fixed dx, fixed dy)
{	ptx += m_fixed(dx, xx, sfc, max_coeff_bits),
	pty += m_fixed(dy, yy, sfc, max_coeff_bits);
	if ( sfc.skewed )
		ptx += m_fixed(dy, yx, sfc, max_coeff_bits),
		pty += m_fixed(dx, xy, sfc, max_coeff_bits);
}

/* We round all endpoints of lines or curves */
/* to the nearest quarter-pixel, and suppress null lines. */
/* (Rounding to the half-pixel causes too many dropouts.) */
/* This saves a lot of rendering work for small characters. */
#define pixel_rounded(fx)\
  (((fx) + float2fixed(0.125)) & float2fixed(-0.25))
#define must_draw_to(lpx, lpy, px, py)\
  ((lpx = pixel_rounded(px)), (lpy = pixel_rounded(py)),\
   (psub = sppath->current_subpath) == 0 ||\
   (pseg = psub->last)->type == s_line_close ||\
   lpx != pseg->pt.x || lpy != pseg->pt.y)

/* ------ Operator procedures ------ */

/* We put these before the interpreter to save having to write */
/* prototypes for all of them. */

int
gs_op1_closepath(register is_ptr ps)
{	/* Note that this does NOT reset the current point! */
	int code = gx_path_close_subpath(sppath);
	if ( code < 0 ) return code;
	return gx_path_add_point(sppath, ptx, pty);	/* put the point where it was */
}

int
gs_op1_sbw(register is_ptr ps, fixed sbx, fixed sby, fixed wx, fixed wy)
{	register gs_type1_state *pis = ps->pis;
	if ( !pis->sb_set )
		pis->lsb.x = sbx, pis->lsb.y = sby;
	pis->width.x = wx, pis->width.y = wy;
	if_debug4('1',"[1]sb=(%g,%g) w=(%g,%g)\n",
		  fixed2float(pis->lsb.x), fixed2float(pis->lsb.y),
		  fixed2float(pis->width.x), fixed2float(pis->width.y));
	accum_xy(pis->lsb.x, pis->lsb.y);
	return 0;
}

int
gs_op1_hsbw(register is_ptr ps, fixed sbx, fixed wx)
{	return gs_op1_sbw(ps, sbx, fixed_0, wx, fixed_0);
}

int
gs_op1_rrcurveto(register is_ptr ps, fixed dx1, fixed dy1,
  fixed dx2, fixed dy2, fixed dx3, fixed dy3)
{	gs_fixed_point pt1, pt2, pt3;
	gs_type1_state *pis = ps->pis;
	fixed ax0 = sppath->position.x - ptx;
	fixed ay0 = sppath->position.y - pty;
	/* Following declarations are only for must_draw_to */
	fixed lpx, lpy;
	subpath *psub;
	segment *pseg;
	accum_xy(dx1, dy1);
	pt1.x = ptx + ax0, pt1.y = pty + ay0;
	accum_xy(dx2, dy2);
	pt2.x = ptx, pt2.y = pty;
	accum_xy(dx3, dy3);
	find_stem_hints(pis, ptx, pty, dx3, dy3, &pt3);
	if ( must_draw_to(lpx, lpy, pt3.x, pt3.y) )
	{	/* Adjust second control point for endpoint hint */
		/* and rounding. */
		pt2.x += lpx - ptx, pt2.y += lpy - pty;
		return gx_path_add_flattened_curve(sppath, pt1.x, pt1.y,
				pt2.x, pt2.y, lpx, lpy, pis->flatness);
	}
	return 0;
}

#undef s

/* ------ Main interpreter ------ */

/* Continue interpreting a Type 1 CharString. */
/* If str != 0, it is taken as the byte string to interpret. */
/* Return 0 on successful completion, <0 on error, */
/* or >0 when client intervention is required. */
/* The int * argument is where the character is stored for seac, */
/* or the othersubr # for callothersubr. */
private int near type1_endchar(P3(gs_type1_state *, gs_state *, gx_path *));
int
gs_type1_interpret(register gs_type1_state *pis, const byte *str,
  const gs_point *psbpt, int *pindex)
{	gs_state *pgs = pis->pgs;
	gs_type1_data *pdata = pis->pdata;
	gs_op1_state s;
	fixed cstack[ostack_size];
#define cs0 cstack[0]
#define ics0 fixed2int_var(cs0)
#define cs1 cstack[1]
#define ics1 fixed2int_var(cs1)
#define cs2 cstack[2]
#define ics2 fixed2int_var(cs2)
#define cs3 cstack[3]
#define ics3 fixed2int_var(cs3)
#define cs4 cstack[4]
#define ics4 fixed2int_var(cs4)
#define cs5 cstack[5]
#define ics5 fixed2int_var(cs5)
	register fixed _ss *csp;
#define clear csp = cstack - 1
	ip_state *ipsp = &pis->ipstack[pis->ips_count - 1];
	register const byte *cip;
	register crypt_state state;
	register int c;
	int code = 0;
	fixed ftx = pgs->ctm.tx_fixed, fty = pgs->ctm.ty_fixed;
	gs_fixed_point hpt;
	/* Following are only for must_draw_to */
	fixed lpx, lpy;
	subpath *psub;
	segment *pseg;

	sppath = pgs->path;
	s.pis = pis;
	sfc = pis->fc;
	ptx = pis->position.x;
	pty = pis->position.y;

	/* Check for a side bearing override. */
	if ( psbpt != 0 )
	{	gs_point_transform2fixed(&pgs->ctm, psbpt->x, psbpt->y,
					 &pis->lsb);
		pis->sb_set = 1;
	}

	/* Copy the operand stack out of the saved state. */
	if ( pis->os_count == 0 )
	   {	clear;
	   }
	else
	   {	memcpy(cstack, pis->ostack, pis->os_count * sizeof(fixed));
		csp = &cstack[pis->os_count - 1];
	   }

	if ( str == 0 ) goto cont;
	cip = str;
call:	state = crypt_charstring_seed;
	   {	int skip = pdata->lenIV;
		/* Skip initial random bytes */
		for ( ; skip > 0; --skip )
		   {	decrypt_skip_next(*cip, state); ++cip;
		   }
	   }
	goto top;
cont:	cip = ipsp->ip;
	state = ipsp->dstate;
top:	while ( 1 )
	 { uint c0;
	   c = decrypt_this((c0 = *cip++), state);
	   decrypt_skip_next(c0, state);
	   switch ( (char_command)c )
	   {
#define cnext clear; goto top
#define inext goto top
	case c_hstem: dc("hstem")
		type1_hstem(pis, cs0, cs1);
		cnext;
	case c_vstem: dc("vstem")
		type1_vstem(pis, cs0, cs1);
		cnext;
	case c_vmoveto: dc("vmoveto")
		cs1 = cs0;
		cs0 = 0;
		accum_y(cs1);
move:		/* cs0 = dx, cs1 = dy for hint checking. */
		find_stem_hints(pis, ptx, pty, cs0, cs1, &hpt);
		/* Round to the nearest center of a quarter-pixel. */
		if ( must_draw_to(lpx, lpy, hpt.x, hpt.y) )
			code = gx_path_add_point(sppath, lpx, lpy);
		goto cc;
	case c_rlineto: dc("rlineto")
		accum_xy(cs0, cs1);
line:		/* cs0 = dx, cs1 = dy for hint checking. */
		find_stem_hints(pis, ptx, pty, cs0, cs1, &hpt);
		/* Round to the nearest center of a quarter-pixel. */
		if ( must_draw_to(lpx, lpy, hpt.x, hpt.y) )
			code = gx_path_add_line(sppath, lpx, lpy);
cc:		if ( code < 0 ) return code;
pp:		if_debug2('1', "[1]pt=(%g,%g)\n",
			  fixed2float(ptx), fixed2float(pty));
		cnext;
	case c_hlineto: dc("hlineto")
		accum_x(cs0);
		cs1 = 0;
		goto line;
	case c_vlineto: dc("vlineto")
		cs1 = cs0;
		cs0 = 0;
		accum_y(cs1);
		goto line;
	case c_rrcurveto: dc("rrcurveto")
		code = gs_op1_rrcurveto(&s, cs0, cs1, cs2, cs3, cs4, cs5);
		goto cc;
	case c_closepath: dc("closepath")
		code = gs_op1_closepath(&s);
		goto cc;
	case c_callsubr: dc("callsubr")
	   {	int index = fixed2int_var(*csp);
		const byte *nip;
		code = (*pdata->subr_proc)(pdata, index, &nip);
		if ( code < 0 ) return_error(code);
		--csp;
		ipsp->ip = cip, ipsp->dstate = state;
		++ipsp;
		cip = nip;
	   }
		goto call;
	case c_return: dc("return")
		--ipsp;
		goto cont;
	case c_escape: dc("escape:")
		decrypt_next(*cip, state, c); ++cip;
		switch ( (char_extended_command)c )
		   {
		case ce_dotsection: dc("  dotsection")
			pis->in_dotsection = !pis->in_dotsection;
			cnext;
		case ce_vstem3: dc("  vstem3")
			if ( !pis->vstem3_set && pis->fh.use_x_hints )
			{	center_vstem(pis, pis->lsb.x + cs2, cs3);
				/* Adjust the current point */
				/* (center_vstem handles everything else). */
				ptx += pis->vs_offset.x;
				pty += pis->vs_offset.y;
				pis->vstem3_set = 1;
			}
			type1_vstem(pis, cs0, cs1);
			type1_vstem(pis, cs2, cs3);
			type1_vstem(pis, cs4, cs5);
			cnext;
		case ce_hstem3: dc("  hstem3")
			type1_hstem(pis, cs0, cs1);
			type1_hstem(pis, cs2, cs3);
			type1_hstem(pis, cs4, cs5);
			cnext;
		case ce_seac: dc("  seac")
			/* Do the accent now.  When it finishes */
			/* (detected in endchar), do the base character. */
			pis->seac_base = ics3;
			/* Adjust the origin of the coordinate system */
			/* for the accent (endchar puts it back). */
			ptx = ftx, pty = fty;
			/* The Adobe documentation says that adx/ady are */
			/* the offset for the *origin*, but they're */
			/* actually the offset for the *lsb*: */
			cs1 += pis->lsb.x - cs0;
			cs2 += pis->lsb.y;
			accum_xy(cs1, cs2);
			sppath->position.x = pis->position.x = ptx;
			sppath->position.y = pis->position.y = pty;
			pis->os_count = 0;	/* clear */
			/* Give control back to the caller, who must */
			/* re-invoke the interpreter with the seac string. */
			*pindex = ics4;
			return type1_result_seac;
		case ce_sbw: dc("  sbw")
			code = gs_op1_sbw(&s, cs0, cs1, cs2, cs3);
			goto cc;
		case ce_div: dc("  div")
			csp[-1] = float2fixed((float)csp[-1] / (float)*csp);
			--csp; goto pushed;
		case ce_undoc15: dc("  undoc15")
			/* See gstype1.h for information on this opcode. */
			cnext;
		case ce_callothersubr: dc("  callothersubr")
#define fpts pis->flex_points
/* Check that the next opcode is a `pop'. */
#define check_next_pop()\
  decrypt_next(*cip, state, c);\
  if ( c != c_escape ) return_error(gs_error_invalidfont);\
  decrypt_next(cip[1], state, c);\
  if ( c != ce_pop ) return_error(gs_error_invalidfont);\
  cip += 2
			/* We must remember to pop both the othersubr # */
			/* and the argument count off the stack. */
			switch ( *pindex = fixed2int_var(*csp) )
			{
			case 0:
			{	/* We have to do something really sleazy */
				/* here, namely, make it look as though */
				/* the rmovetos never really happened, */
				/* because we don't want to interrupt */
				/* the current subpath. */
				gs_fixed_point ept;
				fixed fheight = csp[-4];
				gs_fixed_point hpt;
				if ( pis->flex_count != 8 )
					return_error(gs_error_invalidfont);
				/* Make sure the next two opcodes */
				/* are `pop' `pop'. */
				check_next_pop();
				check_next_pop();
				csp[-4] = csp[-3];
				csp[-3] = csp[-2];
				csp -= 3;
				gx_path_current_point(sppath, &ept);
				gx_path_add_point(sppath, fpts[0].x, fpts[0].y);
				sppath->subpath_open =	/* <--- sleaze */
					pis->flex_path_was_open;
				/* Decide whether to do the flex as a curve. */
				hpt.x = fpts[1].x - fpts[4].x;
				hpt.y = fpts[1].y - fpts[4].y;
				if_debug3('1',
					  "[1]flex: d=(%g,%g), height=%g\n",
					  fixed2float(hpt.x), fixed2float(hpt.y),
					  fixed2float(fheight) / 100);
#if !ALWAYS_DO_FLEX_AS_CURVE			/* See beginning of file. */
				if ( any_abs(hpt.x) + any_abs(hpt.y) <
				     fheight / 100
				   )
				{	/* Do the flex as a line. */
					code = gx_path_add_line(sppath,
								ept.x, ept.y);
				}
				else
#endif
				{	/* Do the flex as a curve. */
					code = gx_path_add_flattened_curve(sppath,
						fpts[2].x, fpts[2].y,
						fpts[3].x, fpts[3].y,
						fpts[4].x, fpts[4].y,
						pis->flatness);
					if ( code < 0 ) return code;
					code = gx_path_add_flattened_curve(sppath,
						fpts[5].x, fpts[5].y,
						fpts[6].x, fpts[6].y,
						fpts[7].x, fpts[7].y,
						pis->flatness);
				}
			}
				if ( code < 0 ) return code;
				pis->flex_count = flex_max;	/* not inside flex */
				inext;
			case 1:
				gx_path_current_point(sppath, &fpts[0]);
				pis->flex_path_was_open = /* <--- more sleaze */
					sppath->subpath_open;
				pis->flex_count = 1;
				csp -= 2;
				inext;
			case 2:
				if ( pis->flex_count >= flex_max )
					return_error(gs_error_invalidfont);
				gx_path_current_point(sppath,
						&fpts[pis->flex_count++]);
				csp -= 2;
				inext;
			case 3:
				/* Make sure the next opcode is a `pop'. */
				check_next_pop();
				reset_stem_hints(pis);
				csp -= 2;
				inext;
			}
#undef fpts
			/* Not a recognized othersubr, */
			/* let the client handle it. */
		   {	int scount = csp - cstack;
			pis->position.x = ptx;
			pis->position.y = pty;
			/* Exit to caller */
			ipsp->ip = cip, ipsp->dstate = state;
			pis->os_count = scount;
			pis->ips_count = ipsp - &pis->ipstack[0] + 1;
			if ( scount )
				memcpy(pis->ostack, cstack,
				       scount * sizeof(fixed));
			return type1_result_callothersubr;
		   }
		case ce_pop: dc("  pop")
			++csp;
			code = (*pdata->pop_proc)(pdata, csp);
			if ( code < 0 ) return_error(code);
			goto pushed;
		case ce_setcurrentpoint: dc("  setcurrentpoint")
			ptx = ftx, pty = fty;
			accum_xy(cs0, cs1);
			goto pp;
		default:
			return_error(gs_error_invalidfont);
		   }
		break;
	case c_hsbw: dc("hsbw")
		code = gs_op1_hsbw(&s, cs0, cs1);
		goto cc;
	case c_endchar: dc("endchar")
		if ( pis->seac_base >= 0 )
		   {	/* We just finished the accent of a seac. */
			/* Do the base character. */
			*pindex = pis->seac_base;
			pis->seac_base = -1;
			/* Restore the coordinate system origin */
			sppath->position.x = pis->position.x = ftx;
			sppath->position.y = pis->position.y = fty;
			pis->os_count = 0;	/* clear */
			/* Clear the ipstack, in case the accent ended */
			/* inside a subroutine. */
			pis->ips_count = 1;
			/* Give control back to the caller, who must */
			/* re-invoke the interpreter with the */
			/* base character string. */
			return type1_result_seac;
		   }
		/* This is a real endchar.  Handle it below. */
		return type1_endchar(pis, pgs, sppath);
	case c_undoc15: dc("  undoc15")
		/* See gstype1.h for information on this opcode. */
		cnext;
	case c_rmoveto: dc("rmoveto")
		accum_xy(cs0, cs1);
		goto move;
	case c_hmoveto: dc("hmoveto")
		accum_x(cs0);
		cs1 = 0;
		goto move;
	case c_vhcurveto: dc("vhcurveto")
		code = gs_op1_rrcurveto(&s, fixed_0, cs0, cs1, cs2, cs3, fixed_0);
		goto cc;
	case c_hvcurveto: dc("hvcurveto")
		code = gs_op1_rrcurveto(&s, cs0, fixed_0, cs1, cs2, fixed_0, cs3);
		goto cc;
	/* Fill up the dispatch up to 32. */
	case c_undef0: case c_undef2:
	case c_undef16: case c_undef17: case c_undef18: case c_undef19:
	case c_undef20: case c_undef23:
	case c_undef24: case c_undef25: case c_undef26: case c_undef27:
	case c_undef28: case c_undef29:
		return_error(gs_error_invalidfont);
	/* Fill up the dispatch for 1-byte numbers. */
#define icase(n) case n:
#define ncase(n) case n: *++csp = int2fixed(c_value_num1(n)); goto pushed;
#define icase10(n)\
  icase(n) icase(n+1) icase(n+2) icase(n+3) icase(n+4)\
  icase(n+5) icase(n+6) icase(n+7) icase(n+8) icase(n+9)
#define ncase10(n)\
  ncase(n) ncase(n+1) ncase(n+2) ncase(n+3) ncase(n+4)\
  ncase(n+5) ncase(n+6) ncase(n+7) ncase(n+8) ncase(n+9)
	icase(32) icase(33) icase(34)
	icase(35) icase(36) icase(37) icase(38) icase(39)
	icase10(40)
	icase10(50) icase10(60) icase10(70) icase10(80) icase10(90)
	icase10(100) icase10(110) goto pi; ncase10(120) ncase10(130) ncase10(140)
	ncase10(150) icase10(160) icase10(170) icase10(180) icase10(190)
	icase10(200) icase10(210) icase10(220) icase10(230)
	icase(240) icase(241) icase(242) icase(243) icase(244)
	icase(245) icase(246)
pi:		*++csp = int2fixed(c_value_num1(c));
pushed:		if_debug3('1', "[1]%d: (%d) %f\n",
			  (int)(csp - cstack), c, fixed2float(*csp));
		break;
	/* Handle 2-byte positive numbers. */
#define case_num2(n)\
  case c_num2+n: c = c_value_num2(c_num2+n, 0)
	case_num2(0); goto pos2;
	case_num2(1); goto pos2;
	case_num2(2); goto pos2;
	case_num2(3);
#undef case_num2
pos2:	   {	c0 = *cip++;
		if_debug2('1', "[1] (%d)+%d\n",
			  c, decrypt_this(c0, state));
		*++csp = int2fixed((int)decrypt_this(c0, state) + c);
		decrypt_skip_next(c0, state);
	   }	goto pushed;
	/* Handle 2-byte negative numbers. */
#define case_num3(n)\
  case c_num3+n: c = c_value_num3(c_num3+n, 0)
	case_num3(0); goto neg2;
	case_num3(1); goto neg2;
	case_num3(2); goto neg2;
	case_num3(3);
#undef case_num3
neg2:	   {	c0 = *cip++;
		if_debug2('1', "[1] (%d)-%d\n",
			  c, decrypt_this(c0, state));
		*++csp = int2fixed(c - (int)decrypt_this(c0, state));
		decrypt_skip_next(c0, state);
	   }	goto pushed;
	/* Handle 5-byte numbers. */
	case c_num4:
	   {	long lw = 0;
		int i;
		for ( i = 4; --i >= 0; )
		{	decrypt_next(*cip, state, c0);
			lw = (lw << 8) + c0;
			cip++;
		}
		*++csp = int2fixed(lw);
		if ( lw != fixed2long(*csp) )
			return_error(gs_error_rangecheck);
	   }	goto pushed;
	   }
	 }
}

/* Pop a (fixed) number off the internal stack. */
/* The client uses this to get the arguments for an OtherSubr. */
int
gs_type1_pop(gs_type1_state *pis, fixed *pf)
{	*pf = pis->ostack[--(pis->os_count)];
	return 0;
}

/* ------ Termination ------ */

/* Handle the end of a character. */
private int near
type1_endchar(gs_type1_state *pis, gs_state *pgs, gx_path *ppath)
{	/* Set the current point to the character origin */
	/* plus the width. */
	gs_moveto(pgs, fixed2float(pis->width.x), fixed2float(pis->width.y));
	/* If there are no hints, ... */
	if ( pis->fh.snap_h.count || pis->fh.snap_v.count ||
	     pis->fh.a_zone_count
	   )
	{	pgs->fill_adjust = fixed_0;
		return 0;
	}
	/* ... tweak the fill adjustment. */
	/* This is based on experience, not theory! */
	{	gs_fixed_rect bbox;
		int dx, dy, dmax;
		gx_path_bbox(pgs->path, &bbox);
		dx = fixed2int_ceiling(bbox.q.x - bbox.p.x);
		dy = fixed2int_ceiling(bbox.q.y - bbox.p.y);
		dmax = max(dx, dy);
		pgs->fill_adjust =
			(dmax < 10 ? float2fixed(0.2) :
			 dmax < 25 ? float2fixed(0.1) :
			 float2fixed(0.05));
	}
	return 0;
}
