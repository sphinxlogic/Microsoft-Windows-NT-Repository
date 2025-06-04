/*
 *   xmcd - Motif(tm) CD Audio Player
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef LINT
static char *_geom_c_ident_ = "@(#)geom.c	6.39 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "xmcd_d/xmcd.h"
#include "xmcd_d/widget.h"
#include "xmcd_d/geom.h"


#define WLIST_SIZE	30		/* Widget list size */


extern appdata_t	app_data;
extern FILE		*errfp;

STATIC int		main_mode;	/* Main window mode */


/***********************
 *  internal routines  *
 ***********************/


/*
 * geom_normal_force
 *	Set the geometry of the widgets in the main window in normal mode.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_normal_force(widgets_t *m)
{
	XtVaSetValues(m->main.mode_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_MODE,
		XmNrightPosition, RIGHT_MODE,
		XmNtopPosition, TOP_MODE,
		XmNbottomPosition, BOTTOM_MODE,
		NULL
	);
	XtVaSetValues(m->main.chkbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_CHECKBOX,
		XmNrightPosition, RIGHT_CHECKBOX,
		XmNtopPosition, TOP_CHECKBOX,
		XmNbottomPosition, BOTTOM_CHECKBOX,
		XmNshadowType, XmSHADOW_OUT,
		NULL
	);
	XtVaSetValues(m->main.lock_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	if (!app_data.main_showfocus) {
		XtVaSetValues(m->main.lock_btn,
			XmNhighlightThickness, 0,
			NULL
		);
	}
	XtVaSetValues(m->main.repeat_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	if (!app_data.main_showfocus) {
		XtVaSetValues(m->main.repeat_btn,
			XmNhighlightThickness, 0,
			NULL
		);
	}
	XtVaSetValues(m->main.shuffle_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	if (!app_data.main_showfocus) {
		XtVaSetValues(m->main.shuffle_btn,
			XmNhighlightThickness, 0,
			NULL
		);
	}
	XtVaSetValues(m->main.eject_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EJECT,
		XmNrightPosition, RIGHT_EJECT,
		XmNtopPosition, TOP_EJECT,
		XmNbottomPosition, BOTTOM_EJECT,
		NULL
	);
	XtVaSetValues(m->main.quit_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_QUIT,
		XmNrightPosition, RIGHT_QUIT,
		XmNtopPosition, TOP_QUIT,
		XmNbottomPosition, BOTTOM_QUIT,
		NULL
	);
	XtVaSetValues(m->main.disc_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DISCIND,
		XmNrightPosition, RIGHT_DISCIND,
		XmNtopPosition, TOP_DISCIND,
		XmNbottomPosition, BOTTOM_DISCIND,
		NULL
	);
	XtVaSetValues(m->main.track_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TRACKIND,
		XmNrightPosition, RIGHT_TRACKIND,
		XmNtopPosition, TOP_TRACKIND,
		XmNbottomPosition, BOTTOM_TRACKIND,
		NULL
	);
	XtVaSetValues(m->main.index_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_INDEXIND,
		XmNrightPosition, RIGHT_INDEXIND,
		XmNtopPosition, TOP_INDEXIND,
		XmNbottomPosition, BOTTOM_INDEXIND,
		NULL
	);
	XtVaSetValues(m->main.time_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TIMEIND,
		XmNrightPosition, RIGHT_TIMEIND,
		XmNtopPosition, TOP_TIMEIND,
		XmNbottomPosition, BOTTOM_TIMEIND,
		NULL
	);
	XtVaSetValues(m->main.rptcnt_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_RPTCNTIND,
		XmNrightPosition, RIGHT_RPTCNTIND,
		XmNtopPosition, TOP_RPTCNTIND,
		XmNbottomPosition, BOTTOM_RPTCNTIND,
		NULL
	);
	XtVaSetValues(m->main.dbmode_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DBMODEIND,
		XmNrightPosition, RIGHT_DBMODEIND,
		XmNtopPosition, TOP_DBMODEIND,
		XmNbottomPosition, BOTTOM_DBMODEIND,
		NULL
	);
	XtVaSetValues(m->main.progmode_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PROGMODEIND,
		XmNrightPosition, RIGHT_PROGMODEIND,
		XmNtopPosition, TOP_PROGMODEIND,
		XmNbottomPosition, BOTTOM_PROGMODEIND,
		NULL
	);
	XtVaSetValues(m->main.timemode_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TIMEMODEIND,
		XmNrightPosition, RIGHT_TIMEMODEIND,
		XmNtopPosition, TOP_TIMEMODEIND,
		XmNbottomPosition, BOTTOM_TIMEMODEIND,
		NULL
	);
	XtVaSetValues(m->main.playmode_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PLAYMODEIND,
		XmNrightPosition, RIGHT_PLAYMODEIND,
		XmNtopPosition, TOP_PLAYMODEIND,
		XmNbottomPosition, BOTTOM_PLAYMODEIND,
		NULL
	);
	XtVaSetValues(m->main.dtitle_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DTITLEIND,
		XmNrightPosition, RIGHT_DTITLEIND,
		XmNtopPosition, TOP_DTITLEIND,
		XmNbottomPosition, BOTTOM_DTITLEIND,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL
	);
	XtVaSetValues(m->main.ttitle_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TTITLEIND,
		XmNrightPosition, RIGHT_TTITLEIND,
		XmNtopPosition, TOP_TTITLEIND,
		XmNbottomPosition, BOTTOM_TTITLEIND,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL
	);
	XtVaSetValues(m->main.dbprog_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DBPROG,
		XmNrightPosition, RIGHT_DBPROG,
		XmNtopPosition, TOP_DBPROG,
		XmNbottomPosition, BOTTOM_DBPROG,
		NULL
	);
	XtVaSetValues(m->main.options_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_OPTIONS,
		XmNrightPosition, RIGHT_OPTIONS,
		XmNtopPosition, TOP_OPTIONS,
		XmNbottomPosition, BOTTOM_OPTIONS,
		NULL
	);
	XtVaSetValues(m->main.time_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TIME,
		XmNrightPosition, RIGHT_TIME,
		XmNtopPosition, TOP_TIME,
		XmNbottomPosition, BOTTOM_TIME,
		NULL
	);
	XtVaSetValues(m->main.ab_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_AB,
		XmNrightPosition, RIGHT_AB,
		XmNtopPosition, TOP_AB,
		XmNbottomPosition, BOTTOM_AB,
		NULL
	);
	XtVaSetValues(m->main.sample_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_SAMPLE,
		XmNrightPosition, RIGHT_SAMPLE,
		XmNtopPosition, TOP_SAMPLE,
		XmNbottomPosition, BOTTOM_SAMPLE,
		NULL
	);
	XtVaSetValues(m->main.keypad_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEYPAD,
		XmNrightPosition, RIGHT_KEYPAD,
		XmNtopPosition, TOP_KEYPAD,
		XmNbottomPosition, BOTTOM_KEYPAD,
		NULL
	);
	XtVaSetValues(m->main.help_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_HELP,
		XmNrightPosition, RIGHT_HELP,
		XmNtopPosition, TOP_HELP,
		XmNbottomPosition, BOTTOM_HELP,
		NULL
	);
	XtVaSetValues(m->main.level_scale,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_LEVEL,
		XmNrightPosition, RIGHT_LEVEL,
		XmNtopPosition, TOP_LEVEL,
		XmNbottomPosition, BOTTOM_LEVEL,
		NULL
	);
	XtVaSetValues(m->main.playpause_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PLAYPAUSE,
		XmNrightPosition, RIGHT_PLAYPAUSE,
		XmNtopPosition, TOP_PLAYPAUSE,
		XmNbottomPosition, BOTTOM_PLAYPAUSE,
		NULL
	);
	XtVaSetValues(m->main.stop_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_STOP,
		XmNrightPosition, RIGHT_STOP,
		XmNtopPosition, TOP_STOP,
		XmNbottomPosition, BOTTOM_STOP,
		NULL
	);
	XtVaSetValues(m->main.prevdisc_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PREVDISC,
		XmNrightPosition, RIGHT_PREVDISC,
		XmNtopPosition, TOP_PREVDISC,
		XmNbottomPosition, BOTTOM_PREVDISC,
		NULL
	);
	XtVaSetValues(m->main.nextdisc_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_NEXTDISC,
		XmNrightPosition, RIGHT_NEXTDISC,
		XmNtopPosition, TOP_NEXTDISC,
		XmNbottomPosition, BOTTOM_NEXTDISC,
		NULL
	);
	XtVaSetValues(m->main.prevtrk_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PREVTRK,
		XmNrightPosition, RIGHT_PREVTRK,
		XmNtopPosition, TOP_PREVTRK,
		XmNbottomPosition, BOTTOM_PREVTRK,
		NULL
	);
	XtVaSetValues(m->main.nexttrk_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_NEXTTRK,
		XmNrightPosition, RIGHT_NEXTTRK,
		XmNtopPosition, TOP_NEXTTRK,
		XmNbottomPosition, BOTTOM_NEXTTRK,
		NULL
	);
	XtVaSetValues(m->main.previdx_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PREVIDX,
		XmNrightPosition, RIGHT_PREVIDX,
		XmNtopPosition, TOP_PREVIDX,
		XmNbottomPosition, BOTTOM_PREVIDX,
		NULL
	);
	XtVaSetValues(m->main.nextidx_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_NEXTIDX,
		XmNrightPosition, RIGHT_NEXTIDX,
		XmNtopPosition, TOP_NEXTIDX,
		XmNbottomPosition, BOTTOM_NEXTIDX,
		NULL
	);
	XtVaSetValues(m->main.rew_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_REW,
		XmNrightPosition, RIGHT_REW,
		XmNtopPosition, TOP_REW,
		XmNbottomPosition, BOTTOM_REW,
		NULL
	);
	XtVaSetValues(m->main.ff_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_FF,
		XmNrightPosition, RIGHT_FF,
		XmNtopPosition, TOP_FF,
		XmNbottomPosition, BOTTOM_FF,
		NULL
	);
}


/*
 * geom_basic_force
 *	Set the geometry of the widgets in the main window in basic mode.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_basic_force(widgets_t *m)
{
	XtVaSetValues(m->main.mode_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BMODE,
		XmNrightPosition, RIGHT_BMODE,
		XmNtopPosition, TOP_BMODE,
		XmNbottomPosition, BOTTOM_BMODE,
		NULL
	);
	XtVaSetValues(m->main.eject_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BEJECT,
		XmNrightPosition, RIGHT_BEJECT,
		XmNtopPosition, TOP_BEJECT,
		XmNbottomPosition, BOTTOM_BEJECT,
		NULL
	);
	XtVaSetValues(m->main.quit_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BQUIT,
		XmNrightPosition, RIGHT_BQUIT,
		XmNtopPosition, TOP_BQUIT,
		XmNbottomPosition, BOTTOM_BQUIT,
		NULL
	);
	XtVaSetValues(m->main.track_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BTRACKIND,
		XmNrightPosition, RIGHT_BTRACKIND,
		XmNtopPosition, TOP_BTRACKIND,
		XmNbottomPosition, BOTTOM_BTRACKIND,
		NULL
	);
	XtVaSetValues(m->main.time_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BTIMEIND,
		XmNrightPosition, RIGHT_BTIMEIND,
		XmNtopPosition, TOP_BTIMEIND,
		XmNbottomPosition, BOTTOM_BTIMEIND,
		NULL
	);
	XtVaSetValues(m->main.level_scale,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BLEVEL,
		XmNrightPosition, RIGHT_BLEVEL,
		XmNtopPosition, TOP_BLEVEL,
		XmNbottomPosition, BOTTOM_BLEVEL,
		NULL
	);
	XtVaSetValues(m->main.playpause_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BPLAYPAUSE,
		XmNrightPosition, RIGHT_BPLAYPAUSE,
		XmNtopPosition, TOP_BPLAYPAUSE,
		XmNbottomPosition, BOTTOM_BPLAYPAUSE,
		NULL
	);
	XtVaSetValues(m->main.stop_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BSTOP,
		XmNrightPosition, RIGHT_BSTOP,
		XmNtopPosition, TOP_BSTOP,
		XmNbottomPosition, BOTTOM_BSTOP,
		NULL
	);
	XtVaSetValues(m->main.prevtrk_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BPREVTRK,
		XmNrightPosition, RIGHT_BPREVTRK,
		XmNtopPosition, TOP_BPREVTRK,
		XmNbottomPosition, BOTTOM_BPREVTRK,
		NULL
	);
	XtVaSetValues(m->main.nexttrk_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BNEXTTRK,
		XmNrightPosition, RIGHT_BNEXTTRK,
		XmNtopPosition, TOP_BNEXTTRK,
		XmNbottomPosition, BOTTOM_BNEXTTRK,
		NULL
	);
}


/*
 * geom_main_force
 *	Set the geometry of the widgets in the main window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_main_force(widgets_t *m)
{
	Display		*display = XtDisplay(m->toplevel);
	int		screen = DefaultScreen(display),
			i,
			x_delta,
			y_delta;
	Dimension	width,
			height,
			swidth,
			sheight;
	Position	abs_x,
			abs_y,
			new_x,
			new_y,
			wmcorr_x,
			wmcorr_y;
	static int	wlist_size = 0;
	static Widget	wlist[WLIST_SIZE];
	static bool_t	first = TRUE;

	swidth = (Dimension) XDisplayWidth(display, screen);
	sheight = (Dimension) XDisplayHeight(display, screen);
	XtTranslateCoords(m->toplevel, 0, 0, &abs_x, &abs_y);

	if (first) {
		first = FALSE;

		/* Set up the list of main window widgets that gets
		 * managed and unmanaged when switching modes.
		 */
		i = 0;
		wlist[i++] = m->main.chkbox_frm;
		wlist[i++] = m->main.disc_ind;
		wlist[i++] = m->main.index_ind;
		wlist[i++] = m->main.rptcnt_ind;
		wlist[i++] = m->main.dbmode_ind;
		wlist[i++] = m->main.timemode_ind;
		wlist[i++] = m->main.playmode_ind;
		wlist[i++] = m->main.dtitle_ind;
		wlist[i++] = m->main.ttitle_ind;
		wlist[i++] = m->main.dbprog_btn;
		wlist[i++] = m->main.dbprog_btn;
		wlist[i++] = m->main.options_btn;
		wlist[i++] = m->main.help_btn;
		wlist[i++] = m->main.time_btn;
		wlist[i++] = m->main.ab_btn;
		wlist[i++] = m->main.sample_btn;
		wlist[i++] = m->main.keypad_btn;
		wlist[i++] = m->main.prevdisc_btn;
		wlist[i++] = m->main.nextdisc_btn;
		wlist[i++] = m->main.previdx_btn;
		wlist[i++] = m->main.nextidx_btn;
		wlist[i++] = m->main.rew_btn;
		wlist[i++] = m->main.ff_btn;
		wlist_size = i;

		wmcorr_x = wmcorr_y = 0;
	}
	else {
		/* Hack: Get window manager correction factor.
		 * This works around the problem with some window
		 * managers where setting the XmNx and XmNy positions
		 * of the toplevel does not take into account of the
		 * window manager decorations.
		 */
		XtVaSetValues(m->toplevel,
			XmNx, abs_x + 1,
			XmNy, abs_y + 1,
			NULL
		);
		for (i = 0; i < 10; i++)
			event_loop(0);

		XtTranslateCoords(m->toplevel, 0, 0, &new_x, &new_y);

		wmcorr_x = new_x - abs_x - 1;
		wmcorr_y = new_y - abs_y - 1;

		XtVaSetValues(m->toplevel,
			XmNx, abs_x - wmcorr_x,
			XmNy, abs_y - wmcorr_y,
			NULL
		);

		DBGPRN(errfp,
			"Window manager correction factor: (%d,%d)\n",
			wmcorr_x, wmcorr_y);
	}

	switch (main_mode) {
	case MAIN_BASIC:
		width = (Dimension) app_data.basic_width;
		height = (Dimension) app_data.basic_height;
		XtUnmanageChildren((WidgetList) wlist, wlist_size);
		geom_basic_force(m);
		break;
	case MAIN_NORMAL:
	default:
		width = (Dimension) app_data.normal_width;
		height = (Dimension) app_data.normal_height;
		XtManageChildren((WidgetList) wlist, wlist_size);
		geom_normal_force(m);
		break;
	}

	/* Set window coordinatess according to the
	 * modeChangeGravity parameter
	 */
	x_delta = app_data.normal_width - app_data.basic_width;
	y_delta = app_data.normal_height - app_data.basic_height;

	switch (app_data.modechg_grav) {
	case 1:
		/* Top edge */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x - (x_delta / 2);
			new_y = abs_y;
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x + (x_delta / 2);
			new_y = abs_y;
		}
		break;
	case 2:
		/* Upper right corner */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x - x_delta;
			new_y = abs_y;
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x + x_delta;
			new_y = abs_y;
		}
		break;
	case 3:
		/* Right edge */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x - x_delta;
			new_y = abs_y - (y_delta / 2);
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x + x_delta;
			new_y = abs_y + (y_delta / 2);
		}
		break;
	case 4:
		/* Lower right corner */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x - x_delta;
			new_y = abs_y - y_delta;
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x + x_delta;
			new_y = abs_y + y_delta;
		}
		break;
	case 5:
		/* Bottom edge */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x - (x_delta / 2);
			new_y = abs_y - y_delta;
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x + (x_delta / 2);
			new_y = abs_y + y_delta;
		}
		break;
	case 6:
		/* Lower left corner */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x;
			new_y = abs_y - y_delta;
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x;
			new_y = abs_y + y_delta;
		}
		break;
	case 7:
		/* Left edge */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x;
			new_y = abs_y - (y_delta / 2);
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x;
			new_y = abs_y + (y_delta / 2);
		}
		break;
	case 8:
		/* Center of window */
		if (main_mode == MAIN_NORMAL) {
			new_x = abs_x - (x_delta / 2);
			new_y = abs_y - (y_delta / 2);
		}
		else if (main_mode == MAIN_BASIC) {
			new_x = abs_x + (x_delta / 2);
			new_y = abs_y + (y_delta / 2);
		}
		break;
	case 0:
	default:
		/* Upper left corner */
		new_x = abs_x;
		new_y = abs_y;
		break;
	}

	/* Make sure the window will appear within
	 * the screen boundaries after resizing.
	 */
	if (new_x < wmcorr_x)
		new_x = wmcorr_x;
	else if ((Dimension) (new_x + width + wmcorr_x) > swidth)
		new_x = swidth - width - wmcorr_x;

	if (new_y < wmcorr_y)
		new_y = wmcorr_y;
	else if ((Dimension) (new_y + height + wmcorr_x) > sheight)
		new_y = sheight - height - wmcorr_x;

	/* Set new main window location and size */
	if (new_x == abs_x && new_y == abs_y) {
		/* Set size only */
		XtVaSetValues(m->toplevel,
			XmNwidth, width,
			XmNheight, height,
			NULL
		);
	}
	else {
		/* Set location and size */
		XtVaSetValues(m->toplevel,
			XmNx, new_x - wmcorr_x,
			XmNy, new_y - wmcorr_y,
			XmNwidth, width,
			XmNheight, height,
			NULL
		);
	}
}


/*
 * geom_keypad_force
 *	Set the geometry of the widgets in the keypad window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_keypad_force(widgets_t *m)
{
	XtVaSetValues(m->keypad.keypad_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEYPADLBL,
		XmNrightPosition, RIGHT_KEYPADLBL,
		XmNtopPosition, TOP_KEYPADLBL,
		XmNbottomPosition, BOTTOM_KEYPADLBL,
		NULL
	);

	XtVaSetValues(m->keypad.keypad_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEYPADIND,
		XmNrightPosition, RIGHT_KEYPADIND,
		XmNtopPosition, TOP_KEYPADIND,
		XmNbottomPosition, BOTTOM_KEYPADIND,
		NULL
	);

	XtVaSetValues(m->keypad.radio_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KPMODEBOX,
		XmNrightPosition, RIGHT_KPMODEBOX,
		XmNtopPosition, TOP_KPMODEBOX,
		XmNbottomPosition, BOTTOM_KPMODEBOX,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);

	XtVaSetValues(m->keypad.track_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->keypad.disc_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[0],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY0,
		XmNrightPosition, RIGHT_KEY0,
		XmNtopPosition, TOP_KEY0,
		XmNbottomPosition, BOTTOM_KEY0,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[1],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY1,
		XmNrightPosition, RIGHT_KEY1,
		XmNtopPosition, TOP_KEY1,
		XmNbottomPosition, BOTTOM_KEY1,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[2],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY2,
		XmNrightPosition, RIGHT_KEY2,
		XmNtopPosition, TOP_KEY2,
		XmNbottomPosition, BOTTOM_KEY2,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[3],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY3,
		XmNrightPosition, RIGHT_KEY3,
		XmNtopPosition, TOP_KEY3,
		XmNbottomPosition, BOTTOM_KEY3,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[4],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY4,
		XmNrightPosition, RIGHT_KEY4,
		XmNtopPosition, TOP_KEY4,
		XmNbottomPosition, BOTTOM_KEY4,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[5],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY5,
		XmNrightPosition, RIGHT_KEY5,
		XmNtopPosition, TOP_KEY5,
		XmNbottomPosition, BOTTOM_KEY5,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[6],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY6,
		XmNrightPosition, RIGHT_KEY6,
		XmNtopPosition, TOP_KEY6,
		XmNbottomPosition, BOTTOM_KEY6,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[7],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY7,
		XmNrightPosition, RIGHT_KEY7,
		XmNtopPosition, TOP_KEY7,
		XmNbottomPosition, BOTTOM_KEY7,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[8],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY8,
		XmNrightPosition, RIGHT_KEY8,
		XmNtopPosition, TOP_KEY8,
		XmNbottomPosition, BOTTOM_KEY8,
		NULL
	);

	XtVaSetValues(m->keypad.num_btn[9],
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_KEY9,
		XmNrightPosition, RIGHT_KEY9,
		XmNtopPosition, TOP_KEY9,
		XmNbottomPosition, BOTTOM_KEY9,
		NULL
	);

	XtVaSetValues(m->keypad.clear_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_CLEAR,
		XmNrightPosition, RIGHT_CLEAR,
		XmNtopPosition, TOP_CLEAR,
		XmNbottomPosition, BOTTOM_CLEAR,
		NULL
	);

	XtVaSetValues(m->keypad.enter_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_ENTER,
		XmNrightPosition, RIGHT_ENTER,
		XmNtopPosition, TOP_ENTER,
		XmNbottomPosition, BOTTOM_ENTER,
		NULL
	);

	XtVaSetValues(m->keypad.warp_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_WARPLBL,
		XmNrightPosition, RIGHT_WARPLBL,
		XmNtopPosition, TOP_WARPLBL,
		XmNbottomPosition, BOTTOM_WARPLBL,
		NULL
	);

	XtVaSetValues(m->keypad.warp_scale,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_WARPSCALE,
		XmNrightPosition, RIGHT_WARPSCALE,
		XmNtopPosition, TOP_WARPSCALE,
		XmNbottomPosition, BOTTOM_WARPSCALE,
		NULL
	);

	XtVaSetValues(m->keypad.keypad_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_KEYPADSEP,
		XmNrightPosition, RIGHT_KEYPADSEP,
		XmNtopPosition, TOP_KEYPADSEP,
		NULL
	);

	XtVaSetValues(m->keypad.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_KPCANCEL,
		XmNrightPosition, RIGHT_KPCANCEL,
		XmNtopPosition, TOP_KPCANCEL,
		NULL
	);
}


/*
 * geom_options_force
 *	Set the geometry of the widgets in the options window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_options_force(widgets_t *m)
{
	XtVaSetValues(m->options.load_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_LOAD_LBL,
		XmNrightPosition, RIGHT_LOAD_LBL,
		XmNtopPosition, TOP_LOAD_LBL,
		XmNbottomPosition, BOTTOM_LOAD_LBL,
		NULL
	);

	XtVaSetValues(m->options.load_chkbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNleftPosition, LEFT_LOAD_CHKFRM,
		XmNrightPosition, RIGHT_LOAD_CHKFRM,
		XmNtopPosition, TOP_LOAD_CHKFRM,
		XmNbottomWidget, m->options.load_radbox_frm,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.load_lock_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.load_radbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_LOAD_RADFRM,
		XmNrightPosition, RIGHT_LOAD_RADFRM,
		XmNtopPosition, TOP_LOAD_RADFRM,
		XmNbottomPosition, BOTTOM_LOAD_RADFRM,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.load_none_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.load_spdn_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.load_play_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.exit_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXIT_LBL,
		XmNrightPosition, RIGHT_EXIT_LBL,
		XmNtopPosition, TOP_EXIT_LBL,
		XmNbottomPosition, BOTTOM_EXIT_LBL,
		NULL
	);

	XtVaSetValues(m->options.exit_radbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXIT_RADFRM,
		XmNrightPosition, RIGHT_EXIT_RADFRM,
		XmNtopPosition, TOP_EXIT_RADFRM,
		XmNbottomPosition, BOTTOM_EXIT_RADFRM,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.exit_none_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.exit_stop_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.exit_eject_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.done_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DONE_LBL,
		XmNrightPosition, RIGHT_DONE_LBL,
		XmNtopPosition, TOP_DONE_LBL,
		XmNbottomPosition, BOTTOM_DONE_LBL,
		NULL
	);

	XtVaSetValues(m->options.done_chkbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DONE_CHKFRM,
		XmNrightPosition, RIGHT_DONE_CHKFRM,
		XmNtopPosition, TOP_DONE_CHKFRM,
		XmNbottomPosition, BOTTOM_DONE_CHKFRM,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.done_eject_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.eject_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EJECT_LBL,
		XmNrightPosition, RIGHT_EJECT_LBL,
		XmNtopPosition, TOP_EJECT_LBL,
		XmNbottomPosition, BOTTOM_EJECT_LBL,
		NULL
	);

	XtVaSetValues(m->options.eject_chkbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EJECT_CHKFRM,
		XmNrightPosition, RIGHT_EJECT_CHKFRM,
		XmNtopPosition, TOP_EJECT_CHKFRM,
		XmNbottomPosition, BOTTOM_EJECT_CHKFRM,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.eject_exit_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.chg_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_CHG_LBL,
		XmNrightPosition, RIGHT_CHG_LBL,
		XmNtopPosition, TOP_CHG_LBL,
		XmNbottomPosition, BOTTOM_CHG_LBL,
		NULL
	);

	XtVaSetValues(m->options.chg_chkbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_CHG_CHKFRM,
		XmNrightPosition, RIGHT_CHG_CHKFRM,
		XmNtopPosition, TOP_CHG_CHKFRM,
		XmNbottomPosition, BOTTOM_CHG_CHKFRM,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.chg_multiplay_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.chg_reverse_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.chroute_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_CHROUTE_LBL,
		XmNrightPosition, RIGHT_CHROUTE_LBL,
		XmNtopPosition, TOP_CHROUTE_LBL,
		XmNbottomPosition, BOTTOM_CHROUTE_LBL,
		NULL
	);

	XtVaSetValues(m->options.chroute_radbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_CHROUTE_RADFRM,
		XmNrightPosition, RIGHT_CHROUTE_RADFRM,
		XmNtopPosition, TOP_CHROUTE_RADFRM,
		XmNbottomPosition, BOTTOM_CHROUTE_RADFRM,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.chroute_stereo_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.chroute_rev_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.chroute_left_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.chroute_right_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.chroute_mono_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.vol_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_VOLTP_LBL,
		XmNrightPosition, RIGHT_VOLTP_LBL,
		XmNtopPosition, TOP_VOLTP_LBL,
		XmNbottomPosition, BOTTOM_VOLTP_LBL,
		NULL
	);

	XtVaSetValues(m->options.vol_radbox_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_VOLTP_RADFRM,
		XmNrightPosition, RIGHT_VOLTP_RADFRM,
		XmNtopPosition, TOP_VOLTP_RADFRM,
		XmNbottomPosition, BOTTOM_VOLTP_RADFRM,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);
	XtVaSetValues(m->options.vol_linear_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.vol_square_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->options.vol_invsqr_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->options.bal_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BAL_LBL,
		XmNrightPosition, RIGHT_BAL_LBL,
		XmNtopPosition, TOP_BAL_LBL,
		XmNbottomPosition, BOTTOM_BAL_LBL,
		NULL
	);

	XtVaSetValues(m->options.ball_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BALL_LBL,
		XmNrightPosition, RIGHT_BALL_LBL,
		XmNtopPosition, TOP_BALL_LBL,
		XmNbottomPosition, BOTTOM_BALL_LBL,
		NULL
	);

	XtVaSetValues(m->options.balr_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BALR_LBL,
		XmNrightPosition, RIGHT_BALR_LBL,
		XmNtopPosition, TOP_BALR_LBL,
		XmNbottomPosition, BOTTOM_BALR_LBL,
		NULL
	);

	XtVaSetValues(m->options.bal_scale,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BAL_SCALE,
		XmNrightPosition, RIGHT_BAL_SCALE,
		XmNtopPosition, TOP_BAL_SCALE,
		XmNbottomPosition, BOTTOM_BAL_SCALE,
		NULL
	);

	XtVaSetValues(m->options.balctr_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_BALCTR_BTN,
		XmNrightPosition, RIGHT_BALCTR_BTN,
		XmNtopPosition, TOP_BALCTR_BTN,
		XmNbottomPosition, BOTTOM_BALCTR_BTN,
		NULL
	);

	XtVaSetValues(m->options.options_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_OPTSEP,
		XmNrightPosition, RIGHT_OPTSEP,
		XmNtopPosition, TOP_OPTSEP,
		NULL
	);

	XtVaSetValues(m->options.reset_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_RESET_BTN,
		XmNrightPosition, RIGHT_RESET_BTN,
		XmNtopPosition, TOP_RESET_BTN,
		NULL
	);

	XtVaSetValues(m->options.save_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_SAVE_BTN,
		XmNrightPosition, RIGHT_SAVE_BTN,
		XmNtopPosition, TOP_SAVE_BTN,
		NULL
	);

	XtVaSetValues(m->options.ok_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_OPTOK_BTN,
		XmNrightPosition, RIGHT_OPTOK_BTN,
		XmNtopPosition, TOP_OPTOK_BTN,
		NULL
	);
}


/*
 * geom_dbprog_force
 *	Set the geometry of the widgets in the database/program window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_dbprog_force(widgets_t *m)
{
	XtVaSetValues(m->dbprog.tottime_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TOTTIMELBL,
		XmNrightPosition, RIGHT_TOTTIMELBL,
		XmNtopPosition, TOP_TOTTIMELBL,
		XmNbottomPosition, BOTTOM_TOTTIMELBL,
		NULL
	);

	XtVaSetValues(m->dbprog.tottime_ind,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TOTTIMEIND,
		XmNrightPosition, RIGHT_TOTTIMEIND,
		XmNtopPosition, TOP_TOTTIMEIND,
		XmNbottomPosition, BOTTOM_TOTTIMEIND,
		NULL
	);

	XtVaSetValues(m->dbprog.logo_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_LOGO,
		XmNrightPosition, RIGHT_LOGO,
		XmNtopPosition, TOP_LOGO,
		XmNbottomPosition, BOTTOM_LOGO,
		NULL
	);

	XtVaSetValues(m->dbprog.wwwwarp_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_WEB,
		XmNrightPosition, RIGHT_WEB,
		XmNtopPosition, TOP_WEB,
		XmNbottomPosition, BOTTOM_WEB,
		NULL
	);

	XtVaSetValues(m->dbprog.rmtdsbl_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_RMTDSBL,
		XmNrightPosition, RIGHT_RMTDSBL,
		XmNtopPosition, TOP_RMTDSBL,
		XmNbottomPosition, BOTTOM_RMTDSBL,
		NULL
	);

	XtVaSetValues(m->dbprog.dtitle_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DTITLELBL,
		XmNrightPosition, RIGHT_DTITLELBL,
		XmNtopPosition, TOP_DTITLELBL,
		XmNbottomPosition, BOTTOM_DTITLELBL,
		NULL
	);

	XtVaSetValues(m->dbprog.extd_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTDLBL,
		XmNrightPosition, RIGHT_EXTDLBL,
		XmNtopPosition, TOP_EXTDLBL,
		XmNbottomPosition, BOTTOM_EXTDLBL,
		NULL
	);

	XtVaSetValues(m->dbprog.dtitle_txt,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DTITLE,
		XmNrightPosition, RIGHT_DTITLE,
		XmNtopPosition, TOP_DTITLE,
		XmNbottomPosition, BOTTOM_DTITLE,
		NULL
	);

	XtVaSetValues(m->dbprog.extd_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTD,
		XmNrightPosition, RIGHT_EXTD,
		XmNtopPosition, TOP_EXTD,
		XmNbottomPosition, BOTTOM_EXTD,
		NULL
	);

	XtVaSetValues(m->dbprog.dbprog_sep1,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DBPROGSEP1,
		XmNrightPosition, RIGHT_DBPROGSEP1,
		XmNtopPosition, TOP_DBPROGSEP1,
		NULL
	);

	XtVaSetValues(m->dbprog.trklist_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TRKLISTLBL,
		XmNrightPosition, RIGHT_TRKLISTLBL,
		XmNtopPosition, TOP_TRKLISTLBL,
		XmNbottomPosition, BOTTOM_TRKLISTLBL,
		NULL
	);

	XtVaSetValues(XtParent(m->dbprog.trk_list),
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TRKLIST,
		XmNrightPosition, RIGHT_TRKLIST,
		XmNtopPosition, TOP_TRKLIST,
		XmNbottomPosition, BOTTOM_TRKLIST,
		NULL
	);

	XtVaSetValues(m->dbprog.radio_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_RADIOLBL,
		XmNrightPosition, RIGHT_RADIOLBL,
		XmNtopPosition, TOP_RADIOLBL,
		XmNbottomPosition, BOTTOM_RADIOLBL,
		NULL
	);

	XtVaSetValues(m->dbprog.radio_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_RADIOBOX,
		XmNrightPosition, RIGHT_RADIOBOX,
		XmNtopPosition, TOP_RADIOBOX,
		XmNbottomPosition, BOTTOM_RADIOBOX,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);

	XtVaSetValues(m->dbprog.tottime_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);
	XtVaSetValues(m->dbprog.trktime_btn,
		XmNheight, 16,
		XmNrecomputeSize, False,
		NULL
	);

	XtVaSetValues(m->dbprog.discid_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DISCIDLBL,
		XmNrightPosition, RIGHT_DISCIDLBL,
		XmNtopPosition, TOP_DISCIDLBL,
		XmNbottomPosition, BOTTOM_DISCIDLBL,
		NULL
	);

	XtVaSetValues(m->dbprog.discid_frm,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DISCIDFRM,
		XmNrightPosition, RIGHT_DISCIDFRM,
		XmNtopPosition, TOP_DISCIDFRM,
		XmNbottomPosition, BOTTOM_DISCIDFRM,
		NULL
	);

	XtVaSetValues(m->dbprog.ttitle_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TTITLELBL,
		XmNrightPosition, RIGHT_TTITLELBL,
		XmNtopPosition, TOP_TTITLELBL,
		XmNbottomPosition, BOTTOM_TTITLELBL,
		XmNshadowType, XmSHADOW_ETCHED_IN,
		NULL
	);

	XtVaSetValues(m->dbprog.extt_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTTLBL,
		XmNrightPosition, RIGHT_EXTTLBL,
		XmNtopPosition, TOP_EXTTLBL,
		XmNbottomPosition, BOTTOM_EXTTLBL,
		NULL
	);

	XtVaSetValues(m->dbprog.ttitle_txt,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TTITLE,
		XmNrightPosition, RIGHT_TTITLE,
		XmNtopPosition, TOP_TTITLE,
		XmNbottomPosition, BOTTOM_TTITLE,
		NULL
	);

	XtVaSetValues(m->dbprog.extt_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTT,
		XmNrightPosition, RIGHT_EXTT,
		XmNtopPosition, TOP_EXTT,
		XmNbottomPosition, BOTTOM_EXTT,
		NULL
	);

	XtVaSetValues(m->dbprog.pgm_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PGMLBL,
		XmNrightPosition, RIGHT_PGMLBL,
		XmNtopPosition, TOP_PGMLBL,
		XmNbottomPosition, BOTTOM_PGMLBL,
		NULL
	);

	XtVaSetValues(m->dbprog.addpgm_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_ADDPGM,
		XmNrightPosition, RIGHT_ADDPGM,
		XmNtopPosition, TOP_ADDPGM,
		XmNbottomPosition, BOTTOM_ADDPGM,
		NULL
	);

	XtVaSetValues(m->dbprog.clrpgm_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_CLRPGM,
		XmNrightPosition, RIGHT_CLRPGM,
		XmNtopPosition, TOP_CLRPGM,
		XmNbottomPosition, BOTTOM_CLRPGM,
		NULL
	);

	XtVaSetValues(m->dbprog.pgmseq_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PGMSEQLBL,
		XmNrightPosition, RIGHT_PGMSEQLBL,
		XmNtopPosition, TOP_PGMSEQLBL,
		XmNbottomPosition, BOTTOM_PGMSEQLBL,
		NULL
	);

	XtVaSetValues(m->dbprog.pgmseq_txt,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_PGMSEQ,
		XmNrightPosition, RIGHT_PGMSEQ,
		XmNtopPosition, TOP_PGMSEQ,
		XmNbottomPosition, BOTTOM_PGMSEQ,
		NULL
	);

	XtVaSetValues(m->dbprog.send_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_SEND,
		XmNrightPosition, RIGHT_SEND,
		XmNtopPosition, TOP_SEND,
		XmNbottomPosition, BOTTOM_SEND,
		NULL
	);

	XtVaSetValues(m->dbprog.dbprog_sep2,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DBPROGSEP2,
		XmNrightPosition, RIGHT_DBPROGSEP2,
		XmNtopPosition, TOP_DBPROGSEP2,
		NULL
	);

	XtVaSetValues(m->dbprog.savedb_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_SAVEDB,
		XmNrightPosition, RIGHT_SAVEDB,
		XmNtopPosition, TOP_SAVEDB,
		NULL
	);

	XtVaSetValues(m->dbprog.linkdb_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_LINK,
		XmNrightPosition, RIGHT_LINK,
		XmNtopPosition, TOP_LINK,
		NULL
	);

	XtVaSetValues(m->dbprog.loaddb_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_LOADDB,
		XmNrightPosition, RIGHT_LOADDB,
		XmNtopPosition, TOP_LOADDB,
		NULL
	);

	XtVaSetValues(m->dbprog.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DPCANCEL,
		XmNrightPosition, RIGHT_DPCANCEL,
		XmNtopPosition, TOP_DPCANCEL,
		NULL
	);
}


/*
 * geom_wwwwarp_force
 *	Set the geometry of the widgets in the wwwWarp window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_wwwwarp_force(widgets_t *m)
{
	XtVaSetValues(m->wwwwarp.wwwwarp_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_WLPIXMAP,
		XmNrightPosition, RIGHT_WLPIXMAP,
		XmNtopPosition, TOP_WLPIXMAP,
		NULL
	);

	XtVaSetValues(m->wwwwarp.sel_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_WLSELLBL,
		XmNrightPosition, RIGHT_WLSELLBL,
		XmNtopPosition, TOP_WLSELLBL,
		NULL
	);

	XtVaSetValues(m->wwwwarp.sel_radbox,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_WLRADBOX,
		XmNrightPosition, RIGHT_WLRADBOX,
		XmNtopPosition, TOP_WLRADBOX,
		NULL
	);

	XtVaSetValues(m->wwwwarp.srchsite_opt,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_NONE,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_WLSRCHSITES,
		XmNtopWidget, m->wwwwarp.sel_radbox,
		NULL
	);

	XtVaSetValues(m->wwwwarp.wwwwarp_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_WLSEP,
		XmNrightPosition, RIGHT_WLSEP,
		XmNtopPosition, TOP_WLSEP,
		NULL
	);

	XtVaSetValues(m->wwwwarp.go_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_WLGO,
		XmNrightPosition, RIGHT_WLGO,
		XmNtopPosition, TOP_WLGO,
		NULL
	);

	XtVaSetValues(m->wwwwarp.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_WLCANCEL,
		XmNrightPosition, RIGHT_WLCANCEL,
		XmNtopPosition, TOP_WLCANCEL,
		NULL
	);
}


/*
 * geom_extd_force
 *	Set the geometry of the widgets in the extended disc info
 *	window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_extd_force(widgets_t *m)
{
	XtVaSetValues(m->dbextd.discno_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DISCNUMLBL,
		XmNrightPosition, RIGHT_DISCNUMLBL,
		XmNtopPosition, TOP_DISCNUMLBL,
		XmNbottomPosition, BOTTOM_DISCNUMLBL,
		NULL
	);
	
	XtVaSetValues(m->dbextd.disc_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DISCLBL,
		XmNrightPosition, RIGHT_DISCLBL,
		XmNtopPosition, TOP_DISCLBL,
		XmNbottomPosition, BOTTOM_DISCLBL,
		NULL
	);
	
	XtVaSetValues(XtParent(m->dbextd.disc_txt),
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTDTXT,
		XmNrightPosition, RIGHT_EXTDTXT,
		XmNtopPosition, TOP_EXTDTXT,
		XmNbottomPosition, BOTTOM_EXTDTXT,
		NULL
	);
	
	XtVaSetValues(m->dbextd.dbextd_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DBEXTDSEP,
		XmNrightPosition, RIGHT_DBEXTDSEP,
		XmNtopPosition, TOP_DBEXTDSEP,
		NULL
	);
	
	XtVaSetValues(m->dbextd.ok_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DDOK,
		XmNrightPosition, RIGHT_DDOK,
		XmNtopPosition, TOP_DDOK,
		NULL
	);
	
	XtVaSetValues(m->dbextd.clear_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DDCLEAR,
		XmNrightPosition, RIGHT_DDCLEAR,
		XmNtopPosition, TOP_DDCLEAR,
		NULL
	);

	XtVaSetValues(m->dbextd.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DDCANCEL,
		XmNrightPosition, RIGHT_DDCANCEL,
		XmNtopPosition, TOP_DDCANCEL,
		NULL
	);
}


/*
 * geom_extt_force
 *	Set the geometry of the widgets in the extended track info
 *	window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_extt_force(widgets_t *m)
{
	XtVaSetValues(m->dbextt.prev_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTTPREV,
		XmNrightPosition, RIGHT_EXTTPREV,
		XmNtopPosition, TOP_EXTTPREV,
		XmNbottomPosition, BOTTOM_EXTTPREV,
		NULL
	);

	XtVaSetValues(m->dbextt.next_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTTNEXT,
		XmNrightPosition, RIGHT_EXTTNEXT,
		XmNtopPosition, TOP_EXTTNEXT,
		XmNbottomPosition, BOTTOM_EXTTNEXT,
		NULL
	);
	
	XtVaSetValues(m->dbextt.trkno_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTTTRKNUMLBL,
		XmNrightPosition, RIGHT_EXTTTRKNUMLBL,
		XmNtopPosition, TOP_EXTTTRKNUMLBL,
		XmNbottomPosition, BOTTOM_EXTTTRKNUMLBL,
		NULL
	);

	XtVaSetValues(m->dbextt.autotrk_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_NONE,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTTAUTOTRKBTN,
		XmNtopPosition, TOP_EXTTAUTOTRKBTN,
		XmNbottomPosition, BOTTOM_EXTTAUTOTRKBTN,
		NULL
	);

	XtVaSetValues(m->dbextt.trk_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_TRKLBL,
		XmNrightPosition, RIGHT_TRKLBL,
		XmNtopPosition, TOP_TRKLBL,
		XmNbottomPosition, BOTTOM_TRKLBL,
		NULL
	);

	XtVaSetValues(XtParent(m->dbextt.trk_txt),
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_EXTTTXT,
		XmNrightPosition, RIGHT_EXTTTXT,
		XmNtopPosition, TOP_EXTTTXT,
		XmNbottomPosition, BOTTOM_EXTTTXT,
		NULL
	);
	
	XtVaSetValues(m->dbextt.dbextt_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DBEXTTSEP,
		XmNrightPosition, RIGHT_DBEXTTSEP,
		XmNtopPosition, TOP_DBEXTTSEP,
		NULL
	);
	
	XtVaSetValues(m->dbextt.ok_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DTOK,
		XmNrightPosition, RIGHT_DTOK,
		XmNtopPosition, TOP_DTOK,
		NULL
	);
	
	XtVaSetValues(m->dbextt.clear_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DTCLEAR,
		XmNrightPosition, RIGHT_DTCLEAR,
		XmNtopPosition, TOP_DTCLEAR,
		NULL
	);

	XtVaSetValues(m->dbextt.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DTCANCEL,
		XmNrightPosition, RIGHT_DTCANCEL,
		XmNtopPosition, TOP_DTCANCEL,
		NULL
	);
}


/*
 * geom_help_force
 *	Set the geometry of the widgets in the help display window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_help_force(widgets_t *m)
{
	XtVaSetValues(m->help.topic_opt,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_NONE,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_TOPIC,
		XmNtopPosition, TOP_TOPIC,
		NULL
	);

	XtVaSetValues(XtParent(m->help.help_txt),
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_HELPTXT,
		XmNrightPosition, RIGHT_HELPTXT,
		XmNtopPosition, TOP_HELPTXT,
		XmNbottomPosition, BOTTOM_HELPTXT,
		NULL
	);
	
	XtVaSetValues(m->help.help_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_HELPSEP,
		XmNrightPosition, RIGHT_HELPSEP,
		XmNtopPosition, TOP_HELPSEP,
		NULL
	);
	
	XtVaSetValues(m->help.about_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_ABOUT,
		XmNrightPosition, RIGHT_ABOUT,
		XmNtopPosition, TOP_ABOUT,
		NULL
	);

	XtVaSetValues(m->help.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_HELPCANCEL,
		XmNrightPosition, RIGHT_HELPCANCEL,
		XmNtopPosition, TOP_HELPCANCEL,
		NULL
	);
}


/*
 * geom_dirsel_force
 *	Set the geometry of the widgets in the CD database directory
 *	selector window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_dirsel_force(widgets_t *m)
{
	XtVaSetValues(m->dirsel.dir_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DIRLBL,
		XmNrightPosition, RIGHT_DIRLBL,
		XmNtopPosition, TOP_DIRLBL,
		XmNbottomPosition, BOTTOM_DIRLBL,
		NULL
	);
	
	XtVaSetValues(XtParent(m->dirsel.dir_list),
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_DIRLIST,
		XmNrightPosition, RIGHT_DIRLIST,
		XmNtopPosition, TOP_DIRLIST,
		XmNbottomPosition, BOTTOM_DIRLIST,
		NULL
	);
	
	XtVaSetValues(m->dirsel.dirsel_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DIRSELSEP,
		XmNrightPosition, RIGHT_DIRSELSEP,
		XmNtopPosition, TOP_DIRSELSEP,
		NULL
	);
	
	XtVaSetValues(m->dirsel.ok_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DSOK,
		XmNrightPosition, RIGHT_DSOK,
		XmNtopPosition, TOP_DSOK,
		NULL
	);
	
	XtVaSetValues(m->dirsel.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_DSCANCEL,
		XmNrightPosition, RIGHT_DSCANCEL,
		XmNtopPosition, TOP_DSCANCEL,
		NULL
	);
}


/*
 * geom_linksel_force
 *	Set the geometry of the widgets in the search-link selector
 *	list window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_linksel_force(widgets_t *m)
{
	XtVaSetValues(m->linksel.link_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_LINKLBL,
		XmNrightPosition, RIGHT_LINKLBL,
		XmNtopPosition, TOP_LINKLBL,
		XmNbottomPosition, BOTTOM_LINKLBL,
		NULL
	);
	
	XtVaSetValues(XtParent(m->linksel.link_list),
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_LINKLIST,
		XmNrightPosition, RIGHT_LINKLIST,
		XmNtopPosition, TOP_LINKLIST,
		XmNbottomPosition, BOTTOM_LINKLIST,
		NULL
	);
	
	XtVaSetValues(m->linksel.linksel_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_LINKSELSEP,
		XmNrightPosition, RIGHT_LINKSELSEP,
		XmNtopPosition, TOP_LINKSELSEP,
		NULL
	);
	
	XtVaSetValues(m->linksel.ok_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_LSOK,
		XmNrightPosition, RIGHT_LSOK,
		XmNtopPosition, TOP_LSOK,
		NULL
	);
	
	XtVaSetValues(m->linksel.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_LSCANCEL,
		XmNrightPosition, RIGHT_LSCANCEL,
		XmNtopPosition, TOP_LSCANCEL,
		NULL
	);
}


/*
 * geom_auth_force
 *	Set the geometry of the widgets in the authorization window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
geom_auth_force(widgets_t *m)
{
	XtVaSetValues(m->auth.auth_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_AUTHLBL,
		XmNrightPosition, RIGHT_AUTHLBL,
		XmNtopPosition, TOP_AUTHLBL,
		XmNbottomPosition, BOTTOM_AUTHLBL,
		NULL
	);
	XtVaSetValues(m->auth.name_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_AUTHNAMELBL,
		XmNrightPosition, RIGHT_AUTHNAMELBL,
		XmNtopPosition, TOP_AUTHNAMELBL,
		XmNbottomPosition, BOTTOM_AUTHNAMELBL,
		NULL
	);
	XtVaSetValues(m->auth.name_txt,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_AUTHNAMETXT,
		XmNrightPosition, RIGHT_AUTHNAMETXT,
		XmNtopPosition, TOP_AUTHNAMETXT,
		XmNbottomPosition, BOTTOM_AUTHNAMETXT,
		NULL
	);
	XtVaSetValues(m->auth.pass_lbl,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_AUTHPASSLBL,
		XmNrightPosition, RIGHT_AUTHPASSLBL,
		XmNtopPosition, TOP_AUTHPASSLBL,
		XmNbottomPosition, BOTTOM_AUTHPASSLBL,
		NULL
	);
	XtVaSetValues(m->auth.pass_txt,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNleftPosition, LEFT_AUTHPASSTXT,
		XmNrightPosition, RIGHT_AUTHPASSTXT,
		XmNtopPosition, TOP_AUTHPASSTXT,
		XmNbottomPosition, BOTTOM_AUTHPASSTXT,
		NULL
	);

	XtVaSetValues(m->auth.auth_sep,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_AUTHSEP,
		XmNrightPosition, RIGHT_AUTHSEP,
		XmNtopPosition, TOP_AUTHSEP,
		NULL
	);

	XtVaSetValues(m->auth.ok_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_AUTHOK,
		XmNrightPosition, RIGHT_AUTHOK,
		XmNtopPosition, TOP_AUTHOK,
		NULL
	);
	XtVaSetValues(m->auth.cancel_btn,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNtopAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftPosition, LEFT_AUTHCANCEL,
		XmNrightPosition, RIGHT_AUTHCANCEL,
		XmNtopPosition, TOP_AUTHCANCEL,
		NULL
	);
}



/***********************
 *   public routines   *
 ***********************/


/*
 * geom_force
 *	Top level function to set the geometry of the widgets in each
 *	main and sub-window.
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
void
geom_force(widgets_t *m)
{
	main_mode = app_data.main_mode;

	geom_main_force(m);
	geom_keypad_force(m);
	geom_options_force(m);
	geom_dbprog_force(m);
	geom_wwwwarp_force(m);
	geom_extd_force(m);
	geom_extt_force(m);
	geom_help_force(m);
	geom_dirsel_force(m);
	geom_linksel_force(m);
	geom_auth_force(m);
}


/*
 * geom_main_chgmode
 *	Change the main window mode
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
void
geom_main_chgmode(widgets_t *m)
{
	if (main_mode == MAIN_NORMAL)
		main_mode = MAIN_BASIC;
	else
		main_mode = MAIN_NORMAL;

	geom_main_force(m);
}


/*
 * geom_main_getmode
 *	Return the current main window mode
 *
 * Args:
 *	m - Pointer to the main widgets structure.
 *
 * Return:
 *	Nothing.
 */
int
geom_main_getmode(void)
{
	return (main_mode);
}

