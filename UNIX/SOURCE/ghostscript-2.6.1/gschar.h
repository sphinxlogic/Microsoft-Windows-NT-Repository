/* Copyright (C) 1989, 1992, 1993 Aladdin Enterprises.  All rights reserved.


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

/* gschar.h */
/* Character operations for Ghostscript library */
#include "gsccode.h"

/* String display, like image display, uses an enumeration structure */
/* to keep track of what's going on (aka 'poor man's coroutine'). */
typedef struct gs_show_enum_s gs_show_enum;
extern const uint gs_show_enum_sizeof;

/* The routines all come in two versions, */
/* one that uses the C convention of null-terminated strings, */
/* and one that supplies a length. */
int	gs_show_init(P3(gs_show_enum *, gs_state *, const char *)),
	gs_show_n_init(P4(gs_show_enum *, gs_state *, const char *, uint)),
	gs_ashow_init(P5(gs_show_enum *, gs_state *, floatp, floatp, const char *)),
	gs_ashow_n_init(P6(gs_show_enum *, gs_state *, floatp, floatp, const char *, uint)),
	gs_widthshow_init(P6(gs_show_enum *, gs_state *, floatp, floatp, gs_char, const char *)),
	gs_widthshow_n_init(P7(gs_show_enum *, gs_state *, floatp, floatp, gs_char, const char *, uint)),
	gs_awidthshow_init(P8(gs_show_enum *, gs_state *, floatp, floatp, gs_char, floatp, floatp, const char *)),
	gs_awidthshow_n_init(P9(gs_show_enum *, gs_state *, floatp, floatp, gs_char, floatp, floatp, const char *, uint)),
	gs_kshow_init(P3(gs_show_enum *, gs_state *, const char *)),
	gs_kshow_n_init(P4(gs_show_enum *, gs_state *, const char *, uint)),
	gs_xyshow_init(P3(gs_show_enum *, gs_state *, const char *)),
	gs_xyshow_n_init(P4(gs_show_enum *, gs_state *, const char *, uint)),
	gs_glyphshow_init(P3(gs_show_enum *, gs_state *, gs_glyph)),
	gs_cshow_init(P3(gs_show_enum *, gs_state *, const char *)),
	gs_cshow_n_init(P4(gs_show_enum *, gs_state *, const char *, uint)),
	gs_stringwidth_init(P3(gs_show_enum *, gs_state *, const char *)),
	gs_stringwidth_n_init(P4(gs_show_enum *, gs_state *, const char *, uint)),
	gs_charpath_init(P4(gs_show_enum *, gs_state *, const char *, int)),
	gs_charpath_n_init(P5(gs_show_enum *, gs_state *, const char *, uint, int));

/* After setting up the enumeration, all the string-related routines */
/* work the same way.  The client calls gs_show_next until it returns */
/* a zero (successful completion) or negative (error) value. */
/* Other values indicate the following situations: */

	/* The client must render a character: obtain the code from */
	/* gs_show_current_char, do whatever is necessary, and then */
	/* call gs_show_next again. */
#define gs_show_render 1

	/* The client has asked to intervene between characters (kshow). */
	/* Obtain the previous and next codes from gs_kshow_previous_char */
	/* and gs_kshow_next_char, do whatever is necessary, and then */
	/* call gs_show_next again. */
#define gs_show_kern 2

	/* The client has asked to handle characters individually */
	/* (xshow, yshow, xyshow, cshow).  Obtain the current code */
	/* from gs_show_current_char, do whatever is necessary, and then */
	/* call gs_show_next again. */
#define gs_show_move 3

int	gs_show_next(P1(gs_show_enum *));
gs_char
	gs_show_current_char(P1(const gs_show_enum *)),
	gs_kshow_previous_char(P1(const gs_show_enum *)),
	gs_kshow_next_char(P1(const gs_show_enum *));
gs_glyph
	gs_show_current_glyph(P1(const gs_show_enum *));
int	gs_show_current_width(P2(const gs_show_enum *, gs_point *));
void	gs_show_width(P2(const gs_show_enum *, gs_point *));	/* cumulative width */
int	gs_show_in_charpath(P1(const gs_show_enum *));	/* return charpath flag */

/* Character cache and metrics operators */
struct gs_font_s;
struct gs_font_s *	gs_rootfont(P2(gs_show_enum *, gs_state *));
int	gs_setcachedevice(P8(gs_show_enum *, gs_state *, floatp, floatp, floatp, floatp, floatp, floatp));
int	gs_setcachedevice2(P12(gs_show_enum *, gs_state *, floatp, floatp, floatp, floatp, floatp, floatp, floatp, floatp, floatp, floatp));
int	gs_setcharwidth(P4(gs_show_enum *, gs_state *, floatp, floatp));
