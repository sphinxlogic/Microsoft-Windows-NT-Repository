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

/* gxstroke.c */
/* Path stroking procedures for Ghostscript library */
#include "math_.h"
#include "gx.h"
#include "gpcheck.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxarith.h"
#include "gxmatrix.h"
#include "gscoord.h"
#include "gzstate.h"
#include "gzdevice.h"
#include "gzcolor.h"			/* requires gxdevice.h */
#include "gzline.h"
#include "gzpath.h"

/* Define the filling adjustment that actually produces no adjustment. */
#define fill_no_adjust ((fixed)1)

/*
 * Structure for a partial line (passed to the drawing routine).
 * Two of these are required to do joins right.
 * Each endpoint includes the two ends of the cap as well,
 * and the deltas for square and round cap computation.
 *
 * The deltas (co, cdelta, ce) are in clockwise order in device space
 * around the endpoint p: they are one-half the line width (suitably
 * transformed) at 90 degrees counter-clockwise, straight ahead,
 * and 90 degrees clockwise from the oriented line o->e,
 * where "90 degrees" is measured in *user* coordinates.
 * Note that the values at o are the negatives of the values at e.
 *
 * Initially, only o.p, e.p, e.cdelta, width, and thin are set.
 * compute_caps fills in the rest.
 */
typedef gs_fixed_point _ss *p_ptr;
typedef struct endpoint_s {
	gs_fixed_point p;		/* the end of the line */
	gs_fixed_point co, ce;		/* ends of the cap, p +/- width */
	gs_fixed_point cdelta;		/* +/- cap length */
} endpoint;
typedef endpoint _ss *ep_ptr;
typedef struct partial_line_s {
	endpoint o;			/* starting coordinate */
	endpoint e;			/* ending coordinate */
	gs_fixed_point width;		/* one-half line width, see above */
	int thin;			/* true if minimum-width line */
} partial_line;
typedef partial_line _ss *pl_ptr;

/* Procedures that stroke a partial_line (the first pl_ptr argument). */
/* If both partial_lines are non-null, the procedure creates */
/* an appropriate join; otherwise, the procedure creates an */
/* end cap.  If the first int is 0, the procedure also starts with */
/* an appropriate cap. */
private int near stroke_add(P5(gx_path *, int, pl_ptr, pl_ptr, gs_state *));
private int near stroke_fill(P5(gx_path *, int, pl_ptr, pl_ptr, gs_state *));

/* Other forward declarations */
private int near stroke(P4(const gx_path *, gx_path *,
  int near (*)(P5(gx_path *, int, pl_ptr, pl_ptr, gs_state *)),
  gs_state *));
private void near adjust_stroke(P2(pl_ptr, gs_state *));
private int near expand_dashes(P3(const subpath *, gx_path *, gs_state *));
private void near compute_caps(P1(pl_ptr));
private int near add_capped(P4(gx_path *, gs_line_cap,
  int (*)(P3(gx_path *, fixed, fixed)),
  ep_ptr));

/* Stroke a path for drawing or saving */
int
gx_stroke_fill(const gx_path *ppath, gs_state *pgs)
{	return stroke(ppath, (gx_path *)0, stroke_fill, pgs);
}
int
gx_stroke_add(const gx_path *ppath, gx_path *to_path, gs_state *pgs)
{	int code = stroke(ppath, to_path, stroke_add, pgs);
	if ( code < 0 ) return code;
	if ( !ppath->subpath_open && ppath->position_valid )
		code = gx_path_add_point(to_path, ppath->position.x,
					 ppath->position.y);
	return code;
}

/* Fill a partial stroked path. */
/* Free variables: code, to_path, ppath, stroke_path_body, pgs, exit (label). */
#define fill_stroke_path()\
if(to_path==&stroke_path_body && !gx_path_is_void_inline(&stroke_path_body))\
{ code = gx_fill_path(to_path, pgs->dev_color, pgs, gx_rule_winding_number,\
    fill_no_adjust);\
  gx_path_release(to_path);\
  if ( code < 0 ) goto exit;\
  gx_path_init(to_path, ppath->memory_procs);\
}

/* Stroke a path.  Call line_proc (stroke_add or stroke_fill) */
/* for each line segment. */
private int near
stroke(const gx_path *ppath, gx_path *to_path,
  int near (*line_proc)(P5(gx_path *, int, pl_ptr, pl_ptr, gs_state *)),
  gs_state *pgs)
{	const subpath *psub;
	const subpath *save_psub = 0;
	int code = 0;
	const line_params *lp = pgs->line_params;
	int dash_count = lp->dash.pattern_size;
	gx_path fpath, dpath;
	gx_path stroke_path_body;
	float xx = pgs->ctm.xx, xy = pgs->ctm.xy;
	float yx = pgs->ctm.yx, yy = pgs->ctm.yy;
	int skewed = !is_fzero2(xy, yx);
	int uniform = (skewed ? 0 : xx == yy ? 1 : xx == -yy ? -1 : 0);
	/*
	 * We are dealing with a reflected coordinate system
	 * if (1,0) is counter-clockwise from (0,1).
	 * See the note in stroke_add for the algorithm.
	 */	
	int reflected =
	  (uniform ? uniform > 0 :
	   skewed ? xy * yx < xx * yy :
	   (xx < 0) == (yy < 0));
	float line_width = lp->width;	/* this is *half* the line width! */
	int always_thin;
	double line_width_and_scale, line_width_scale_xx;
#ifdef DEBUG
if ( gs_debug['o'] )
   {	int count = lp->dash.pattern_size;
	int i;
	dprintf3("[o]half_width=%f, cap=%d, join=%d,\n",
		 lp->width, (int)lp->cap, (int)lp->join);
	dprintf2("   miter_limit=%f, miter_check=%f,\n",
		 lp->miter_limit, lp->miter_check);
	dprintf1("   dash pattern=%d", count);
	for ( i = 0; i < count; i++ )
		dprintf1(",%f", lp->dash.pattern[i]);
	dprintf4(",\n   offset=%f, init(ink_on=%d, index=%d, dist_left=%f)\n",
		 lp->dash.offset, lp->dash.init_ink_on, lp->dash.init_index,
		 lp->dash.init_dist_left);
   }
#endif
	if ( line_width < 0 ) line_width = -line_width;
	if ( is_fzero(line_width) )
		always_thin = 1;
	else if ( !skewed )
	   {	float xxa = xx, yya = yy;
		if ( xxa < 0 ) xxa = -xxa;
		if ( yya < 0 ) yya = -yya;
		always_thin = (max(xxa, yya) * line_width < 0.5);
	   }
	else
	   {	/* The check is more complicated, but it's worth it. */
		float xsq = xx * xx + xy * xy;
		float ysq = yx * yx + yy * yy;
		float cross = xx * yx + xy * yy;
		if ( cross < 0 ) cross = 0;
		always_thin =
		  ((max(xsq, ysq) + cross) * line_width * line_width < 0.5);
	   }
	line_width_and_scale = line_width * (double)int2fixed(1);
	if ( !always_thin && uniform )
	{	/* Precompute a value we'll need later. */
		line_width_scale_xx = line_width_and_scale * xx;
		if ( line_width_scale_xx < 0 )
		  line_width_scale_xx = -line_width_scale_xx;
	}
	if_debug5('o', "[o]ctm=(%g,%g,%g,%g) thin=%d\n",
		  xx, xy, yx, yy, always_thin);
	/* Start by flattening the path.  We should do this on-the-fly.... */
	if ( !ppath->curve_count )	/* don't need to flatten */
	   {	psub = ppath->first_subpath;
		if ( !psub ) return 0;
	   }
	else
	   {	if ( (code = gx_path_flatten(ppath, &fpath, pgs->flatness, (int)pgs->in_cachedevice)) < 0 )
		   return code;
		psub = fpath.first_subpath;
	   }
	if ( dash_count )
		gx_path_init(&dpath, ppath->memory_procs);
	if ( to_path == 0 )
	{	/* We might try to defer this if it's expensive.... */
		to_path = &stroke_path_body;
		gx_path_init(to_path, ppath->memory_procs);
	}
	for ( ; ; )
	 { const line_segment *pline;
	   fixed x, y;
	   partial_line pl, pl_prev, pl_first;
	   int first = 0;
	   int index = 0;
	   if ( !psub )
	    {	/* Might just be the end of a dash expansion. */
		if ( save_psub )
		   {	gx_path_release(&dpath);
			psub = (const subpath *)save_psub->last->next;
			if ( !psub ) break;
			gx_path_init(&dpath, ppath->memory_procs);
			save_psub = 0;
		   }
		else		/* all done */
			break;
	    }
	   if ( dash_count && !save_psub )
	    {	code = expand_dashes(psub, &dpath, pgs);
		if ( code < 0 ) goto exit;
		save_psub = (subpath *)psub;
		psub = dpath.first_subpath;
		continue;		/* psub might be null */
	    }
	   pline = (const line_segment *)(psub->next);
	   x = psub->pt.x;
	   y = psub->pt.y;
	   while ( pline != 0 && pline->type != s_start )
	    {	fixed sx = pline->pt.x;
		fixed sy = pline->pt.y;
		/* Compute the width parameters in device space. */
		/* We work with unscaled values, for speed. */
		pl.o.p.x = x, pl.o.p.y = y;
		pl.e.p.x = sx, pl.e.p.y = sy;
		if ( !always_thin )
		   {	fixed udx = sx - x, udy = sy - y;
			if ( !(udx | udy) )	/* degenerate */
			 { /* Only consider a degenerate segment */
			   /* if the entire subpath is degenerate and */
			   /* we are using round caps or joins. */
			   if ( index != 0 || (pline->next != 0 &&
				 pline->next->type != s_start) ||
				(lp->cap != gs_cap_round &&
				 lp->join != gs_join_round)
			      )
			     goto nd;
			   /* Pick an arbitrary orientation. */
			   udx = int2fixed(1);
			 }
			if ( uniform != 0 )
			   {	/* We can save a lot of work in this case. */
				float dpx = udx, dpy = udy;
 				float wl = line_width_scale_xx /
					hypot(dpx, dpy);
				pl.e.cdelta.x = (fixed)(dpx * wl);
				pl.e.cdelta.y = (fixed)(dpy * wl);
				pl.width.x = -pl.e.cdelta.y;
				pl.width.y = pl.e.cdelta.x;
				pl.thin = 0;	/* if not always_thin, */
						/* then never thin. */
			   }
			else
			   {	gs_point dpt;	/* unscaled */
				float wl;
				gs_idtransform_inline(pgs,
				          (float)udx, (float)udy, &dpt);
				wl = line_width_and_scale /
					hypot(dpt.x, dpt.y);
				/* Construct the width vector in */
				/* user space, still unscaled. */
				dpt.x *= wl;
				dpt.y *= wl;
				/*
				 * We now compute both perpendicular
				 * and (optionally) parallel half-widths,
				 * as deltas in device space.  We use
				 * a fixed-point, unscaled version of
				 * gs_dtransform.  The second computation
				 * folds in a 90-degree rotation (in user
				 * space, before transforming) in the
				 * direction that corresponds to clockwise
				 * in device space.
				 */
				pl.e.cdelta.x = (fixed)(dpt.x * xx);
				pl.e.cdelta.y = (fixed)(dpt.y * yy);
				if ( skewed )
				  pl.e.cdelta.x += (fixed)(dpt.y * yx),
				  pl.e.cdelta.y += (fixed)(dpt.x * xy);
				if ( reflected )
				  dpt.x = -dpt.x, dpt.y = -dpt.y;
				pl.width.x = (fixed)(dpt.y * xx),
				pl.width.y = -(fixed)(dpt.x * yy);
				if ( skewed )
				  pl.width.x -= (fixed)(dpt.x * yx),
				  pl.width.y += (fixed)(dpt.y * xy);
				pl.thin =
				  any_abs(pl.width.x) + any_abs(pl.width.y) <
				    float2fixed(0.75);
			   }
			if ( !pl.thin )
			{	adjust_stroke(&pl, pgs);
				compute_caps(&pl);
			}
		   }
		else			/* always_thin */
			pl.e.cdelta.x = pl.e.cdelta.y = 0,
			pl.width.x = pl.width.y = 0,
			pl.thin = 1;
		if ( first++ == 0 ) pl_first = pl;
		if ( index++ )
		{	code = (*line_proc)(to_path,
					(psub->closed ? 1 : index - 2),
					&pl_prev, &pl, pgs);
			if ( code < 0 ) goto exit;
			fill_stroke_path();
		}
		pl_prev = pl;
		x = sx, y = sy;
nd:		pline = (const line_segment *)(pline->next);
	    }
	   if ( index )
	    {	/* If closed, join back to start, else cap */
		code = (*line_proc)(to_path, index - 1, &pl_prev,
			     (psub->closed ? &pl_first : (pl_ptr)0), pgs);
		if ( code < 0 ) goto exit;
		fill_stroke_path();
	    }
	   psub = (const subpath *)pline;
	 }
exit:	if ( to_path == &stroke_path_body )
		gx_path_release(to_path);	/* (only needed if error) */
	if ( dash_count ) gx_path_release(&dpath);
	if ( ppath->curve_count ) gx_path_release(&fpath);
	return code;
}

/* ------ Internal routines ------ */

/* Adjust the endpoints and width of a stroke segment */
/* to achieve more uniform rendering. */
/* Only o.p, e.p, e.cdelta, and width have been set. */
private void near
adjust_stroke(pl_ptr plp, gs_state *pgs)
{	fixed _ss *pw;
	fixed _ss *pov;
	fixed _ss *pev;
	fixed w2;
	if ( !pgs->stroke_adjust && plp->width.x != 0 && plp->width.y != 0 )
		return;		/* don't adjust */
	if ( any_abs(plp->width.x) < any_abs(plp->width.y) )
	{	/* More horizontal stroke */
		pw = &plp->width.y, pov = &plp->o.p.y, pev = &plp->e.p.y;
	}
	else
	{	/* More vertical stroke */
		pw = &plp->width.x, pov = &plp->o.p.x, pev = &plp->e.p.x;
	}
	/* Round the larger component of the width up or down, */
	/* whichever way produces a result closer to the correct width. */
	/* Note that just rounding the larger component */
	/* may not produce the correct result. */
	w2 = fixed_rounded(*pw << 1);		/* full line width */
	if ( w2 == 0 && *pw != 0 )
	{	/* Make sure thin lines don't disappear. */
		w2 = (*pw < 0 ? -fixed_1 : fixed_1);
	}
	*pw = arith_rshift_1(w2);
	/* Only adjust the endpoints if the line is horizontal or vertical. */
	if ( *pov == *pev )
	{	if ( w2 & fixed_1 )	/* odd width, move to half-pixel */
		{	*pov = *pev = fixed_floor(*pov) + fixed_half;
		}
		else			/* even width, move to pixel */
		{	*pov = *pev = fixed_rounded(*pov);
		}
	}
}

/* Expand a dashed subpath into explicit segments. */
/* The subpath contains no curves. */
private int near
expand_dashes(const subpath *psub, gx_path *ppath, gs_state *pgs)
{	const dash_params *dash = &pgs->line_params->dash;
	const float *pattern = dash->pattern;
	int count, ink_on, index;
	float dist_left;
	fixed x0 = psub->pt.x, y0 = psub->pt.y;
	fixed x, y;
	const segment *pseg;
	int wrap = (dash->init_ink_on && psub->closed ? -1 : 0);
	int drawing = wrap;
	int code;
	if ( (code = gx_path_add_point(ppath, x0, y0)) < 0 )
		return code;
	/* To do the right thing at the beginning of a closed path, */
	/* we have to skip any initial line, and then redo it at */
	/* the end of the path.  Drawing = -1 while skipping, */
	/* 0 while drawing normally, and 1 on the second round. */
top:	count = dash->pattern_size;
	ink_on = dash->init_ink_on;
	index = dash->init_index;
	dist_left = dash->init_dist_left;
	x = x0, y = y0;
	pseg = (const segment *)psub;
	while ( (pseg = pseg->next) != 0 && pseg->type != s_start )
	   {	fixed sx = pseg->pt.x, sy = pseg->pt.y;
		fixed udx = sx - x, udy = sy - y;
		float length, dx, dy;
		float dist;
		if ( !(udx | udy) )	/* degenerate */
			dx = 0, dy = 0, length = 0;
		else
		   {	gs_point d;
			dx = udx, dy = udy;	/* scaled as fixed */
			gs_idtransform_inline(pgs, dx, dy, &d);
			length = hypot(d.x, d.y) * (1 / (float)int2fixed(1));
		   }
		dist = length;
		while ( dist > dist_left )
		   {	/* We are using up the dash element */
			float fraction = dist_left / length;
			fixed nx = x + (fixed)(dx * fraction);
			fixed ny = y + (fixed)(dy * fraction);
			if ( ink_on )
			   {	if ( drawing >= 0 )
				  code = gx_path_add_line(ppath, nx, ny);
			   }
			else
			   {	if ( drawing > 0 ) return 0;	/* done */
				code = gx_path_add_point(ppath, nx, ny);
				drawing = 0;
			   }
			if ( code < 0 ) return code;
			dist -= dist_left;
			ink_on = !ink_on;
			if ( ++index == count ) index = 0;
			dist_left = pattern[index];
			x = nx, y = ny;
		   }
		dist_left -= dist;
		/* Handle the last dash of a segment. */
		if ( ink_on )
		   {	if ( drawing >= 0 )
			  code =
			    (pseg->type == s_line_close && drawing > 0 ?
			     gx_path_close_subpath(ppath) :
			     gx_path_add_line(ppath, sx, sy));
		   }
		else
		   {	if ( drawing > 0 ) return 0;	/* done */
			code = gx_path_add_point(ppath, sx, sy);
			drawing = 0;
		   }
		if ( code < 0 ) return code;
		x = sx, y = sy;
	   }
	/* Check for wraparound. */
	if ( wrap && drawing <= 0 )
	   {	/* We skipped some initial lines. */
		/* Go back and do them now. */
		drawing = 1;
		goto top;
	   }
	return 0;
}

/* Compute the intersection of two lines.  This is a messy algorithm */
/* that somehow ought to be useful in more places than just here.... */
/* If the lines are (nearly) parallel, return -1 without setting *pi; */
/* otherwise, return 0 if the intersection is beyond *pp1 and *pp2 in */
/* the direction determined by *pd1 and *pd2, and 1 otherwise. */
private int
line_intersect(
    p_ptr pp1,				/* point on 1st line */
    p_ptr pd1,				/* slope of 1st line (dx,dy) */
    p_ptr pp2,				/* point on 2nd line */
    p_ptr pd2,				/* slope of 2nd line */
    p_ptr pi)				/* return intersection here */
{	/* We don't have to do any scaling, the factors all work out right. */
	float u1 = pd1->x, v1 = pd1->y;
	float u2 = pd2->x, v2 = pd2->y;
	double denom = u1 * v2 - u2 * v1;
	float xdiff = pp2->x - pp1->x;
	float ydiff = pp2->y - pp1->y;
	double f1;
	double max_result = any_abs(denom) * (double)max_fixed;
#ifdef DEBUG
if ( gs_debug['o'] )
   {	dprintf4("[o]Intersect %f,%f(%f/%f)",
		 fixed2float(pp1->x), fixed2float(pp1->y),
		 fixed2float(pd1->x), fixed2float(pd1->y));
	dprintf4(" & %f,%f(%f/%f),\n",
		 fixed2float(pp2->x), fixed2float(pp2->y),
		 fixed2float(pd2->x), fixed2float(pd2->y));
	dprintf3("\txdiff=%f ydiff=%f denom=%f ->\n",
		 xdiff, ydiff, denom);
   }
#endif
	/* Check for degenerate result. */
	if ( any_abs(xdiff) >= max_result || any_abs(ydiff) >= max_result )
	   {	/* The lines are nearly parallel, */
		/* or one of them has zero length.  Punt. */
		if_debug0('o', "\tdegenerate!\n");
		return -1;
	   }
	f1 = (v2 * xdiff - u2 * ydiff) / denom;
	pi->x = pp1->x + (fixed)(f1 * u1);
	pi->y = pp1->y + (fixed)(f1 * v1);
	if_debug2('o', "\t%f,%f\n",
		  fixed2float(pi->x), fixed2float(pi->y));
	return (f1 >= 0 && (v1 * xdiff >= u1 * ydiff ? denom >= 0 : denom < 0) ? 0 : 1);
}

#define lix plp->o.p.x
#define liy plp->o.p.y
#define litox plp->e.p.x
#define litoy plp->e.p.y
#define trsign(v, c) ((v) >= 0 ? (c) : -(c))

/* Set up the width and delta parameters for a thin line. */
/* We only approximate the width and height. */
private void near
set_thin_widths(register pl_ptr plp)
{	fixed dx = litox - lix, dy = litoy - liy;
	if ( any_abs(dx) > any_abs(dy) )
	{	plp->width.x = plp->e.cdelta.y = 0;
		plp->width.y = -(plp->e.cdelta.x =
			trsign(dx, -fixed_half));
	}
	else
	{	plp->width.y = plp->e.cdelta.x = 0;
		plp->width.x = -(plp->e.cdelta.y =
			trsign(dy, -fixed_half));
	}
}

/* Draw a line on the device. */
private int near
stroke_fill(gx_path *ppath, int first, register pl_ptr plp, pl_ptr nplp,
  gs_state *pgs)
{	if ( plp->thin )
	   {	/* Minimum-width line, don't have to be careful. */
		/* We do have to check for the entire line being */
		/* within the clipping rectangle, allowing for some */
		/* slop at the ends. */
		fixed dx = litox - lix, dy = litoy - liy;
#define slop int2fixed(2)
		fixed xslop = trsign(dx, slop);
		fixed yslop = trsign(dy, slop);
		if ( gx_cpath_includes_rectangle(pgs->clip_path,
				lix - xslop, liy - yslop,
				litox + xslop, litoy + yslop) )
			return gz_draw_line_fixed(lix, liy, litox, litoy,
				pgs->dev_color, pgs);
#undef slop
#undef trsign
		/* We didn't set up the endpoint parameters before, */
		/* because the line was thin.  stroke_add will do this. */
	   }
	/* General case: construct a path for the fill algorithm. */
	return stroke_add(ppath, first, plp, nplp, pgs);
}

#undef lix
#undef liy
#undef litox
#undef litoy

/* Add a segment to the path.  This handles all the complex cases. */
private int near add_capped(P4(gx_path *, gs_line_cap, int (*)(P3(gx_path *, fixed, fixed)), ep_ptr));
private int near
stroke_add(gx_path *ppath, int first, register pl_ptr plp, pl_ptr nplp,
  gs_state *pgs)
{	int code;
	if ( plp->thin )
	   {	/* We didn't set up the endpoint parameters before, */
		/* because the line was thin.  Do it now. */
		set_thin_widths(plp);
		adjust_stroke(plp, pgs);
		compute_caps(plp);
	   }
	if ( (code = add_capped(ppath, (first == 0 ? pgs->line_params->cap : gs_cap_butt), gx_path_add_point, &plp->o)) < 0 )
		return code;
	if ( nplp == 0 )
	   {	code = add_capped(ppath, pgs->line_params->cap, gx_path_add_line, &plp->e);
	   }
	else if ( pgs->line_params->join == gs_join_round )
	   {	code = add_capped(ppath, gs_cap_round, gx_path_add_line, &plp->e);
	   }
	else if ( nplp->thin )		/* no join */
	  {	code = add_capped(ppath, gs_cap_butt, gx_path_add_line, &plp->e);
	  }
	else				/* join_bevel or join_miter */
	   {	gs_fixed_point jp1, jp2;
		/*
		 * Set np to whichever of nplp->o.co or .ce
		 * is outside the current line.  We observe that
		 * point (x2,y2) is counter-clockwise from (x1,y1)
		 * relative to the origin iff x1*y2 < x2*y1.
		 * In this case x1,y1 are plp->width,
		 * x2,y2 are nplp->width, and the origin is
		 * their common point (plp->e.p, nplp->o.p).
		 */
		float wx1 = plp->width.x, wy1 = plp->width.y;
		float wx2 = nplp->width.x, wy2 = nplp->width.y;
		int ccw = wx1 * wy2 < wx2 * wy1;
		p_ptr outp, np, np1, np2;
		/* Initialize for a bevel join. */
		jp1.x = plp->e.co.x, jp1.y = plp->e.co.y;
		jp2.x = plp->e.ce.x, jp2.y = plp->e.ce.y;
		if ( ccw )
			outp = &jp2, np2 = np = &nplp->o.co, np1 = &plp->e.p;
		else
			outp = &jp1, np1 = np = &nplp->o.ce, np2 = &plp->e.p;
		if_debug1('o', "[o]use %s\n", (ccw ? "co (ccw)" : "ce (cw)"));
		/* Don't bother with the miter check if the two */
		/* points to be joined are very close together, */
		/* namely, in the same square half-pixel. */
		if ( pgs->line_params->join == gs_join_miter &&
		     !(fixed2long(outp->x << 1) == fixed2long(np->x << 1) &&
		       fixed2long(outp->y << 1) == fixed2long(np->y << 1))
		   )
		  { /*
		     * Check whether a miter join is appropriate.
		     * Let a, b be the angles of the two lines.
		     * We check tan(a-b) against the miter_check
		     * by using the following formula:
		     * If tan(a)=u1/v1 and tan(b)=u2/v2, then
		     * tan(a-b) = (u1*v2 - u2*v1) / (u1*u2 + v1*v2).
		     * We can do all the computations unscaled,
		     * because we're only concerned with ratios.
		     */
		    float u1 = plp->e.cdelta.x, v1 = plp->e.cdelta.y;
		    float u2 = nplp->o.cdelta.x, v2 = nplp->o.cdelta.y;
		    double num = u1 * v2 - u2 * v1;
		    double denom = u1 * u2 + v1 * v2;
		    float check = pgs->line_params->miter_check;
		    /*
		     * We will want either tan(a-b) or tan(b-a)
		     * depending on the orientations of the lines.
		     * Fortunately we know the relative orientations already.
		     */
		    if ( !ccw )		/* have plp - nplp, want vice versa */
			num = -num;
#ifdef DEBUG
if ( gs_debug['o'] )
                   {    dprintf4("[o]Miter check: u1/v1=%f/%f, u2/v2=%f/%f,\n",
				 u1, v1, u2, v2);
                        dprintf3("        num=%f, denom=%f, check=%f\n",
				 num, denom, check);
                   }
#endif
		    /* Use a miter if num / denom >= check. */
		    /* If check > 0, num < 0 always passes; */
		    /* if check < 0, num >= 0 always fails. */
		    if ( denom < 0 ) num = -num, denom = -denom;
		    if ( check > 0 ?
			(num < 0 || num >= denom * check) :
			(num < 0 && num >= denom * check)
		       )
		    {	/* OK to use a miter join. */
			gs_fixed_point mpt;
			if_debug0('o', "        ... passes.\n");
			/* Compute the intersection of */
			/* the extended edge lines. */
			if ( line_intersect(outp, &plp->e.cdelta, np,
					    &nplp->o.cdelta, &mpt) == 0
			   )
				outp->x = mpt.x,
				outp->y = mpt.y;
		    }
		   }
		if ( (code = gx_path_add_line(ppath, jp1.x, jp1.y)) < 0 ||
		     (code = gx_path_add_line(ppath, np1->x, np1->y)) < 0 ||
		     (code = gx_path_add_line(ppath, np2->x, np2->y)) < 0 ||
		     (code = gx_path_add_line(ppath, jp2.x, jp2.y)) < 0
		   )
			return code;
	   }
	if ( code < 0 || (code = gx_path_close_subpath(ppath)) < 0 )
		return code;
	return 0;
}

/* Routines for cap computations */

/* Compute the endpoints of the two caps of a segment. */
private void near
compute_caps(register pl_ptr plp)
{	fixed wx2 = plp->width.x;
	fixed wy2 = plp->width.y;
	plp->o.co.x = plp->o.p.x + wx2, plp->o.co.y = plp->o.p.y + wy2;
	plp->o.cdelta.x = -plp->e.cdelta.x,
	  plp->o.cdelta.y = -plp->e.cdelta.y;
	plp->o.ce.x = plp->o.p.x - wx2, plp->o.ce.y = plp->o.p.y - wy2;
	plp->e.co.x = plp->e.p.x - wx2, plp->e.co.y = plp->e.p.y - wy2;
	plp->e.ce.x = plp->e.p.x + wx2, plp->e.ce.y = plp->e.p.y + wy2;
#ifdef DEBUG
if ( gs_debug['o'] )
	dprintf4("[o]Stroke o=(%f,%f) e=(%f,%f)\n",
		 fixed2float(plp->o.p.x), fixed2float(plp->o.p.y),
		 fixed2float(plp->e.p.x), fixed2float(plp->e.p.y)),
	dprintf4("\twxy=(%f,%f) lxy=(%f,%f)\n",
		 fixed2float(wx2), fixed2float(wy2),
		 fixed2float(plp->e.cdelta.x), fixed2float(plp->e.cdelta.y));
#endif
}

/* Add a properly capped line endpoint to the path. */
/* The first point may require either moveto or lineto. */
private int near
add_capped(gx_path *ppath, gs_line_cap type,
  int (*add_proc)(P3(gx_path *, fixed, fixed)), /* gx_path_add_point/line */
  register ep_ptr endp)
{	int code;
#define px endp->p.x
#define py endp->p.y
#define xo endp->co.x
#define yo endp->co.y
#define xe endp->ce.x
#define ye endp->ce.y
#define cdx endp->cdelta.x
#define cdy endp->cdelta.y
#ifdef DEBUG
if ( gs_debug['o'] )
	dprintf4("[o]cap: p=(%g,%g), co=(%g,%g),\n",
		 fixed2float(px), fixed2float(py),
		 fixed2float(xo), fixed2float(yo)),
	dprintf4("[o]\tce=(%g,%g), cd=(%g,%g)\n",
		 fixed2float(xe), fixed2float(ye),
		 fixed2float(cdx), fixed2float(cdy));
#endif
	switch ( type )
	   {
	case gs_cap_round:
	   {	fixed xm = px + cdx;
		fixed ym = py + cdy;
		if (	(code = (*add_proc)(ppath, xo, yo)) < 0 ||
			(code = gx_path_add_arc(ppath, xo, yo, xm, ym,
				xo + cdx, yo + cdy, quarter_arc_fraction)) < 0 ||
			(code = gx_path_add_arc(ppath, xm, ym, xe, ye,
				xe + cdx, ye + cdy, quarter_arc_fraction)) < 0
		   ) return code;
	   }
		break;
	case gs_cap_square:
		if (	(code = (*add_proc)(ppath, xo + cdx, yo + cdy)) < 0 ||
			(code = gx_path_add_line(ppath, xe + cdx, ye + cdy)) < 0
		   ) return code;
		break;
	case gs_cap_butt:
		if (	(code = (*add_proc)(ppath, xo, yo)) < 0 ||
			(code = gx_path_add_line(ppath, xe, ye)) < 0
		   ) return code;
	   }
	return code;
}
