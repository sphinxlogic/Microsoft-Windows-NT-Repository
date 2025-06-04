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
static char *_widget_c_ident_ = "@(#)widget.c	6.68 98/10/09";
#endif

#include "common_d/appenv.h"
#include "common_d/patchlevel.h"
#include "xmcd_d/xmcd.h"
#include "xmcd_d/dbprog.h"
#include "xmcd_d/cdfunc.h"
#include "xmcd_d/widget.h"
#include "xmcd_d/geom.h"
#include "xmcd_d/help.h"
#include "xmcd_d/hotkey.h"
#include "libdi_d/libdi.h"

#include "bitmaps/mode.xbm"
#include "bitmaps/lock.xbm"
#include "bitmaps/repeat.xbm"
#include "bitmaps/shuffle.xbm"
#include "bitmaps/eject.xbm"
#include "bitmaps/quit.xbm"
#include "bitmaps/dbprog.xbm"
#include "bitmaps/help.xbm"
#include "bitmaps/options.xbm"
#include "bitmaps/time.xbm"
#include "bitmaps/ab.xbm"
#include "bitmaps/sample.xbm"
#include "bitmaps/keypad.xbm"
#include "bitmaps/playpaus.xbm"
#include "bitmaps/stop.xbm"
#include "bitmaps/prevdisc.xbm"
#include "bitmaps/nextdisc.xbm"
#include "bitmaps/prevtrk.xbm"
#include "bitmaps/nexttrk.xbm"
#include "bitmaps/previdx.xbm"
#include "bitmaps/nextidx.xbm"
#include "bitmaps/rew.xbm"
#include "bitmaps/ff.xbm"
#include "bitmaps/logo.xbm"
#include "bitmaps/wwwwarp.xbm"
#include "bitmaps/xmcd.xbm"


extern appdata_t	app_data;
extern widgets_t	widgets;

STATIC Atom		delw;
STATIC XtCallbackRec	main_chkbox_cblist[] = {
	{ (XtCallbackProc) cd_checkbox,		NULL },
	{ (XtCallbackProc) NULL,		NULL },
};
STATIC XtCallbackRec	keypad_radbox_cblist[] = {
	{ (XtCallbackProc) cd_keypad_mode,	NULL },
	{ (XtCallbackProc) NULL,		NULL },
};
STATIC XtCallbackRec	options_cblist[] = {
	{ (XtCallbackProc) cd_options,		NULL },
	{ (XtCallbackProc) NULL,		NULL },
};
STATIC XtCallbackRec	dbprog_radbox_cblist[] = {
	{ (XtCallbackProc) dbprog_timedpy,	NULL },
	{ (XtCallbackProc) NULL,		NULL },
};
STATIC XtCallbackRec	wwwwarp_radbox_cblist[] = {
	{ (XtCallbackProc) dbprog_wwwwarp_select, NULL },
	{ (XtCallbackProc) NULL,		NULL },
};
STATIC XtCallbackRec	help_cblist[] = {
	{ (XtCallbackProc) cd_help_popup,	NULL },
	{ (XtCallbackProc) NULL,		NULL },
};


/***********************
 *  internal routines  *
 ***********************/


/*
 * Action routines
 */

/*
 * focuschg
 *	Widget action routine to handle keyboard focus change events
 *	This is used to change the label color of widgets in the
 *	main window.
 */
/*ARGSUSED*/
STATIC void
focuschg(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	Widget	action_widget;

	if ((int) *num_args != 1)
		return;	/* Error: should have one arg */

	action_widget = XtNameToWidget(widgets.main.form, args[0]);
	if (action_widget == NULL)
		return;	/* Can't find widget */

	cd_focus_chg(action_widget, NULL, ev);
}


/*
 * mainmap
 *	Widget action routine to handle the map and unmap events
 *	on the main window.  This is used to perform certain
 *	optimizations when the user iconifies the application.
 */
/*ARGSUSED*/
STATIC void
mainmap(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	curstat_t	*s = curstat_addr();

	if (w != widgets.toplevel)
		return;

	if (ev->type == MapNotify)
		cd_icon(s, FALSE);
	else if (ev->type == UnmapNotify)
		cd_icon(s, TRUE);
}


/*
 * Widget-related functions
 */


/*
 * create_main_widgets
 *	Create all widgets in the main window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_main_widgets(widgets_t *m)
{
	int		i;
	Arg		arg[10];
	curstat_t	*s = curstat_addr();
	XmString	dash = XmStringCreateSimple("-"),
			blank = XmStringCreateSimple("");

	main_chkbox_cblist[0].closure = (XtPointer) s;

	/* Create form widget as container */
	m->main.form = XmCreateForm(
		m->toplevel,
		"mainForm",
		NULL,
		0
	);

	/* Create pushbutton widget for Mode button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.mode_btn = XmCreatePushButton(
		m->main.form,
		"modeButton",
		arg,
		i
	);

	/* Create frame for check box */
	m->main.chkbox_frm = XmCreateFrame(
		m->main.form,
		"checkBoxFrame",
		NULL,
		0
	);

	/* Create check box
	 * The SimpleCheckBox widget in Motif 2.0 doesn't let one set
	 * a pixmap for the toggle button labels, so we use a RowColumn
	 * widget with individual toggle button children.
	 */
	i = 0;
	XtSetArg(arg[i], XmNrowColumnType, XmWORK_AREA); i++;
	XtSetArg(arg[i], XmNnumColumns, 1); i++;
	XtSetArg(arg[i], XmNspacing, 2); i++;
	XtSetArg(arg[i], XmNmarginHeight, 4); i++;
	XtSetArg(arg[i], XmNorientation, XmVERTICAL); i++;
	XtSetArg(arg[i], XmNisHomogeneous, True); i++;
	XtSetArg(arg[i], XmNentryClass, xmToggleButtonWidgetClass); i++;
	XtSetArg(arg[i], XmNentryCallback, main_chkbox_cblist); i++;
	m->main.check_box = XmCreateRowColumn(
		m->main.chkbox_frm,
		"checkBox",
		arg,
		i
	);

	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.lock_btn = XmCreateToggleButton(
		m->main.check_box,
		"button_0",
		arg,
		i
	);
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.repeat_btn = XmCreateToggleButton(
		m->main.check_box,
		"button_1",
		arg,
		i
	);
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.shuffle_btn = XmCreateToggleButton(
		m->main.check_box,
		"button_2",
		arg,
		i
	);

	/* Create pushbutton widget for Eject button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.eject_btn = XmCreatePushButton(
		m->main.form,
		"ejectButton",
		arg,
		i
	);

	/* Create pushbutton widget for Quit button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.quit_btn = XmCreatePushButton(
		m->main.form,
		"quitButton",
		arg,
		i
	);

	/* Create label widget as disc indicator */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, dash); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.disc_ind = XmCreateLabel(
		m->main.form,
		"discIndicator",
		arg,
		i
	);

	/* Create label widget as track indicator */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, dash); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.track_ind = XmCreateLabel(
		m->main.form,
		"trackIndicator",
		arg,
		i
	);

	/* Create label widget as index indicator */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, dash); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.index_ind = XmCreateLabel(
		m->main.form,
		"indexIndicator",
		arg,
		i
	);

	/* Create label widget as time indicator */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, dash); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.time_ind = XmCreateLabel(
		m->main.form,
		"timeIndicator",
		arg,
		i
	);

	/* Create label widget as Repeat count indicator */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, dash); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.rptcnt_ind = XmCreateLabel(
		m->main.form,
		"repeatCountIndicator",
		arg,
		i
	);

	/* Create label widget as CDDB indicator label */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, blank); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.dbmode_ind = XmCreateLabel(
		m->main.form,
		"dbModeIndicator",
		arg,
		i
	);

	/* Create label widget as Program Mode indicator label */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, blank); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.progmode_ind = XmCreateLabel(
		m->main.form,
		"progModeIndicator",
		arg,
		i
	);

	/* Create label widget as Time Mode indicator label */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, blank); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.timemode_ind = XmCreateLabel(
		m->main.form,
		"timeModeIndicator",
		arg,
		i
	);

	/* Create label widget as Play Mode indicator label */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, blank); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.playmode_ind = XmCreateLabel(
		m->main.form,
		"playModeIndicator",
		arg,
		i
	);

	/* Create label widget as disc title indicator */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, blank); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.dtitle_ind = XmCreateLabel(
		m->main.form,
		"discTitleIndicator",
		arg,
		i
	);

	/* Create label widget as track title indicator */
	i = 0;
	XtSetArg(arg[i], XmNlabelString, blank); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.ttitle_ind = XmCreateLabel(
		m->main.form,
		"trackTitleIndicator",
		arg,
		i
	);

	/* Create pushbutton widget for CDDB/Program button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.dbprog_btn = XmCreatePushButton(
		m->main.form,
		"dbprogButton",
		arg,
		i
	);

	/* Create pushbutton widget as Options button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.options_btn = XmCreatePushButton(
		m->main.form,
		"optionsButton",
		arg,
		i
	);

	/* Create pushbutton widget for Time button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.time_btn = XmCreatePushButton(
		m->main.form,
		"timeButton",
		arg,
		i
	);

	/* Create pushbutton widgets for A->B button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.ab_btn = XmCreatePushButton(
		m->main.form,
		"abButton",
		arg,
		i
	);

	/* Create pushbutton widget for Sample button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.sample_btn = XmCreatePushButton(
		m->main.form,
		"sampleButton",
		arg,
		i
	);

	/* Create pushbutton widget for Sample button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.keypad_btn = XmCreatePushButton(
		m->main.form,
		"keypadButton",
		arg,
		i
	);

	/* Create pushbutton widget for Help button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.help_btn = XmCreatePushButton(
		m->main.form,
		"helpButton",
		arg,
		i
	);

	/* Create scale widget for level slider */
	i = 0;
	XtSetArg(arg[i], XmNshowValue, True); i++;
	XtSetArg(arg[i], XmNminimum, 0); i++;
	XtSetArg(arg[i], XmNmaximum, 100); i++;
	XtSetArg(arg[i], XmNorientation, XmHORIZONTAL); i++;
	XtSetArg(arg[i], XmNprocessingDirection, XmMAX_ON_RIGHT); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.level_scale = XmCreateScale(
		m->main.form,
		"levelScale",
		arg,
		i
	);

	/* Create pushbutton widget for Play-Pause button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.playpause_btn = XmCreatePushButton(
		m->main.form,
		"playPauseButton",
		arg,
		i
	);

	/* Create pushbutton widget for Stop button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.stop_btn = XmCreatePushButton(
		m->main.form,
		"stopButton",
		arg,
		i
	);

	/* Create pushbutton widget for Prev-Disc button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.prevdisc_btn = XmCreatePushButton(
		m->main.form,
		"prevDiscButton",
		arg,
		i
	);

	/* Create pushbutton widget for Next-Disc button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.nextdisc_btn = XmCreatePushButton(
		m->main.form,
		"nextDiscButton",
		arg,
		i
	);

	/* Create pushbutton widget for Prev Track button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.prevtrk_btn = XmCreatePushButton(
		m->main.form,
		"prevTrackButton",
		arg,
		i
	);

	/* Create pushbutton widget for Next Track button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.nexttrk_btn = XmCreatePushButton(
		m->main.form,
		"nextTrackButton",
		arg,
		i
	);

	/* Create pushbutton widget for Prev Index button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.previdx_btn = XmCreatePushButton(
		m->main.form,
		"prevIndexButton",
		arg,
		i
	);

	/* Create pushbutton widget for Next Index button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.nextidx_btn = XmCreatePushButton(
		m->main.form,
		"nextIndexButton",
		arg,
		i
	);

	/* Create pushbutton widget for REW button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.rew_btn = XmCreatePushButton(
		m->main.form,
		"rewButton",
		arg,
		i
	);

	/* Create pushbutton widget for FF button */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->main.ff_btn = XmCreatePushButton(
		m->main.form,
		"ffButton",
		arg,
		i
	);

	/* Manage the widgets */
	XtManageChild(m->main.form);
	XtManageChild(m->main.mode_btn);
	XtManageChild(m->main.chkbox_frm);
	XtManageChild(m->main.check_box);
	XtManageChild(m->main.lock_btn);
	XtManageChild(m->main.repeat_btn);
	XtManageChild(m->main.shuffle_btn);
	XtManageChild(m->main.eject_btn);
	XtManageChild(m->main.quit_btn);
	XtManageChild(m->main.disc_ind);
	XtManageChild(m->main.track_ind);
	XtManageChild(m->main.index_ind);
	XtManageChild(m->main.time_ind);
	XtManageChild(m->main.rptcnt_ind);
	XtManageChild(m->main.dbmode_ind);
	XtManageChild(m->main.progmode_ind);
	XtManageChild(m->main.timemode_ind);
	XtManageChild(m->main.playmode_ind);
	XtManageChild(m->main.dtitle_ind);
	XtManageChild(m->main.ttitle_ind);
	XtManageChild(m->main.dbprog_btn);
	XtManageChild(m->main.options_btn);
	XtManageChild(m->main.time_btn);
	XtManageChild(m->main.ab_btn);
	XtManageChild(m->main.sample_btn);
	XtManageChild(m->main.keypad_btn);
	XtManageChild(m->main.help_btn);
	XtManageChild(m->main.level_scale);
	XtManageChild(m->main.playpause_btn);
	XtManageChild(m->main.stop_btn);
	XtManageChild(m->main.prevdisc_btn);
	XtManageChild(m->main.nextdisc_btn);
	XtManageChild(m->main.prevtrk_btn);
	XtManageChild(m->main.nexttrk_btn);
	XtManageChild(m->main.previdx_btn);
	XtManageChild(m->main.nextidx_btn);
	XtManageChild(m->main.rew_btn);
	XtManageChild(m->main.ff_btn);

	XmStringFree(dash);
	XmStringFree(blank);
}


/*
 * create_keypad_widgets
 *	Create all widgets in the keypad window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_keypad_widgets(widgets_t *m)
{
	int		i, j;
	Arg		arg[10];
	char		btn_name[20];
	curstat_t	*s = curstat_addr();

	keypad_radbox_cblist[0].closure = (XtPointer) s;

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->keypad.form = XmCreateFormDialog(
		m->toplevel,
		"keypadForm",
		arg,
		i
	);

	/* Create label widget as keypad label */
	m->keypad.keypad_lbl = XmCreateLabel(
		m->keypad.form,
		"keypadLabel",
		NULL,
		0
	);

	/* Create label widget as keypad indicator */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->keypad.keypad_ind = XmCreateLabel(
		m->keypad.form,
		"keypadIndicator",
		arg,
		i
	);

	/* Create frame for radio box */
	m->keypad.radio_frm = XmCreateFrame(
		m->keypad.form,
		"keypadSelectFrame",
		NULL,
		0
	);

	/* Create radio box widget as keypad mode selector */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 2); i++;
	XtSetArg(arg[i], XmNbuttonSet, 1); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNradioAlwaysOne, True); i++;
	XtSetArg(arg[i], XmNentryCallback, keypad_radbox_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->keypad.radio_box = XmCreateSimpleRadioBox(
		m->keypad.radio_frm,
		"keypadSelectBox",
		arg,
		i
	);
	m->keypad.disc_btn = XtNameToWidget(m->keypad.radio_box, "button_0");
	m->keypad.track_btn = XtNameToWidget(m->keypad.radio_box, "button_1");

	/* Create pushbutton widgets as number keys */
	for (j = 0; j < 10; j++) {
		(void) sprintf(btn_name, "keypadNumButton%u", j);

		i = 0;
		XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
		m->keypad.num_btn[j] = XmCreatePushButton(
			m->keypad.form,
			btn_name,
			arg,
			i
		);
	}

	/* Create pushbutton widget as clear button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->keypad.clear_btn = XmCreatePushButton(
		m->keypad.form,
		"keypadClearButton",
		arg,
		i
	);

	/* Create pushbutton widget as enter button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->keypad.enter_btn = XmCreatePushButton(
		m->keypad.form,
		"keypadEnterButton",
		arg,
		i
	);

	/* Create label widget as track warp label */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->keypad.warp_lbl = XmCreateLabel(
		m->keypad.form,
		"trackWarpLabel",
		arg,
		i
	);

	/* Create scale widget for track warp slider */
	i = 0;
	XtSetArg(arg[i], XmNshowValue, False); i++;
	XtSetArg(arg[i], XmNminimum, 0); i++;
	XtSetArg(arg[i], XmNmaximum, 255); i++;
	XtSetArg(arg[i], XmNorientation, XmHORIZONTAL); i++;
	XtSetArg(arg[i], XmNprocessingDirection, XmMAX_ON_RIGHT); i++;
	XtSetArg(arg[i], XmNhighlightOnEnter, True); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->keypad.warp_scale = XmCreateScale(
		m->keypad.form,
		"trackWarpScale",
		arg,
		i
	);

	/* Create separator bar widget */
	m->keypad.keypad_sep = XmCreateSeparator(
		m->keypad.form,
		"keypadSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->keypad.cancel_btn = XmCreatePushButton(
		m->keypad.form,
		"keypadCancelButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->keypad.keypad_lbl);
	XtManageChild(m->keypad.keypad_ind);
	XtManageChild(m->keypad.radio_frm);
	XtManageChild(m->keypad.radio_box);
	for (i = 0; i < 10; i++)
		XtManageChild(m->keypad.num_btn[i]);
	XtManageChild(m->keypad.clear_btn);
	XtManageChild(m->keypad.enter_btn);
	XtManageChild(m->keypad.warp_lbl);
	XtManageChild(m->keypad.warp_scale);
	XtManageChild(m->keypad.keypad_sep);
	XtManageChild(m->keypad.cancel_btn);
}


/*
 * create_options_widgets
 *	Create all widgets in the options window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_options_widgets(widgets_t *m)
{
	int		i;
	Arg		arg[10];
	curstat_t	*s = curstat_addr();

	options_cblist[0].closure = (XtPointer) s;

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->options.form = XmCreateFormDialog(
		m->toplevel,
		"optionsForm",
		arg,
		i
	);

	/* Create label widget as load options label */
	m->options.load_lbl = XmCreateLabel(
		m->options.form,
		"onLoadLabel",
		NULL,
		0
	);

	/* Create frame for load options check box */
	m->options.load_chkbox_frm = XmCreateFrame(
		m->options.form,
		"onLoadCheckBoxFrame",
		NULL,
		0
	);

	/* Create check box widget as load options checkbox */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 1); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.load_chkbox = XmCreateSimpleCheckBox(
		m->options.load_chkbox_frm,
		"onLoadCheckBox",
		arg,
		i
	);
	m->options.load_lock_btn =
		XtNameToWidget(m->options.load_chkbox, "button_0");

	/* Create frame for load options radio box */
	m->options.load_radbox_frm = XmCreateFrame(
		m->options.form,
		"onLoadRadioBoxFrame",
		NULL,
		0
	);

	/* Create radio box widget as load options selector */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 3); i++;
	XtSetArg(arg[i], XmNbuttonSet, 0); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNradioAlwaysOne, True); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.load_radbox = XmCreateSimpleRadioBox(
		m->options.load_radbox_frm,
		"onLoadRadioBox",
		arg,
		i
	);
	m->options.load_none_btn =
		XtNameToWidget(m->options.load_radbox, "button_0");
	m->options.load_spdn_btn =
		XtNameToWidget(m->options.load_radbox, "button_1");
	m->options.load_play_btn =
		XtNameToWidget(m->options.load_radbox, "button_2");

	/* Create label widget as exit options label */
	m->options.exit_lbl = XmCreateLabel(
		m->options.form,
		"onExitLabel",
		NULL,
		0
	);

	/* Create frame for exit options radio box */
	m->options.exit_radbox_frm = XmCreateFrame(
		m->options.form,
		"onExitRadioBoxFrame",
		NULL,
		0
	);

	/* Create radio box widget as exit options selector */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 3); i++;
	XtSetArg(arg[i], XmNbuttonSet, 0); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNradioAlwaysOne, True); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.exit_radbox = XmCreateSimpleRadioBox(
		m->options.exit_radbox_frm,
		"onExitRadioBox",
		arg,
		i
	);
	m->options.exit_none_btn =
		XtNameToWidget(m->options.exit_radbox, "button_0");
	m->options.exit_stop_btn =
		XtNameToWidget(m->options.exit_radbox, "button_1");
	m->options.exit_eject_btn =
		XtNameToWidget(m->options.exit_radbox, "button_2");

	/* Create label widget as done options label */
	m->options.done_lbl = XmCreateLabel(
		m->options.form,
		"onDoneLabel",
		NULL,
		0
	);

	/* Create frame for done options radio box */
	m->options.done_chkbox_frm = XmCreateFrame(
		m->options.form,
		"onDoneCheckBoxFrame",
		NULL,
		0
	);

	/* Create check box widget as done options checkbox */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 1); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.done_chkbox = XmCreateSimpleCheckBox(
		m->options.done_chkbox_frm,
		"onDoneCheckBox",
		arg,
		i
	);
	m->options.done_eject_btn =
		XtNameToWidget(m->options.done_chkbox, "button_0");

	/* Create label widget as eject options label */
	m->options.eject_lbl = XmCreateLabel(
		m->options.form,
		"onEjectLabel",
		NULL,
		0
	);

	/* Create frame for done options radio box */
	m->options.eject_chkbox_frm = XmCreateFrame(
		m->options.form,
		"onEjectCheckBoxFrame",
		NULL,
		0
	);

	/* Create check box widget as eject options checkbox */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 1); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.eject_chkbox = XmCreateSimpleCheckBox(
		m->options.eject_chkbox_frm,
		"onEjectCheckBox",
		arg,
		i
	);
	m->options.eject_exit_btn =
		XtNameToWidget(m->options.eject_chkbox, "button_0");

	/* Create label widget as channel route options label */
	m->options.chroute_lbl = XmCreateLabel(
		m->options.form,
		"channelRouteLabel",
		NULL,
		0
	);

	/* Create frame for channel route options radio box */
	m->options.chroute_radbox_frm = XmCreateFrame(
		m->options.form,
		"channelRouteRadioBoxFrame",
		NULL,
		0
	);

	/* Create label widget as changer options label */
	m->options.chg_lbl = XmCreateLabel(
		m->options.form,
		"changerLabel",
		NULL,
		0
	);

	/* Create frame for changer options radio box */
	m->options.chg_chkbox_frm = XmCreateFrame(
		m->options.form,
		"changerCheckBoxFrame",
		NULL,
		0
	);

	/* Create check box widget as changer options checkbox */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 2); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.chg_chkbox = XmCreateSimpleCheckBox(
		m->options.chg_chkbox_frm,
		"changerCheckBox",
		arg,
		i
	);
	m->options.chg_multiplay_btn =
		XtNameToWidget(m->options.chg_chkbox, "button_0");
	m->options.chg_reverse_btn =
		XtNameToWidget(m->options.chg_chkbox, "button_1");

	/* Create radio box widget as channel route options selector */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 5); i++;
	XtSetArg(arg[i], XmNbuttonSet, 0); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNradioAlwaysOne, True); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.chroute_radbox = XmCreateSimpleRadioBox(
		m->options.chroute_radbox_frm,
		"channelRouteRadioBox",
		arg,
		i
	);
	m->options.chroute_stereo_btn =
		XtNameToWidget(m->options.chroute_radbox, "button_0");
	m->options.chroute_rev_btn =
		XtNameToWidget(m->options.chroute_radbox, "button_1");
	m->options.chroute_left_btn =
		XtNameToWidget(m->options.chroute_radbox, "button_2");
	m->options.chroute_right_btn =
		XtNameToWidget(m->options.chroute_radbox, "button_3");
	m->options.chroute_mono_btn =
		XtNameToWidget(m->options.chroute_radbox, "button_4");

	/* Create label widget as vol taper options label */
	m->options.vol_lbl = XmCreateLabel(
		m->options.form,
		"volTaperLabel",
		NULL,
		0
	);

	/* Create frame for vol taper options radio box */
	m->options.vol_radbox_frm = XmCreateFrame(
		m->options.form,
		"volTaperRadioBoxFrame",
		NULL,
		0
	);

	/* Create radio box widget as vol taper options selector */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 3); i++;
	XtSetArg(arg[i], XmNbuttonSet, 0); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNradioAlwaysOne, True); i++;
	XtSetArg(arg[i], XmNentryCallback, options_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.vol_radbox = XmCreateSimpleRadioBox(
		m->options.vol_radbox_frm,
		"volTaperRadioBox",
		arg,
		i
	);
	m->options.vol_linear_btn =
		XtNameToWidget(m->options.vol_radbox, "button_0");
	m->options.vol_square_btn =
		XtNameToWidget(m->options.vol_radbox, "button_1");
	m->options.vol_invsqr_btn =
		XtNameToWidget(m->options.vol_radbox, "button_2");

	/* Create label widget as balance control label */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.bal_lbl = XmCreateLabel(
		m->options.form,
		"balanceLabel",
		arg,
		i
	);

	/* Create label widget as balance control L label */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.ball_lbl = XmCreateLabel(
		m->options.form,
		"balanceLeftLabel",
		arg,
		i
	);

	/* Create label widget as balance control R label */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.balr_lbl = XmCreateLabel(
		m->options.form,
		"balanceRightLabel",
		arg,
		i
	);

	/* Create scale widget for balance control slider */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	XtSetArg(arg[i], XmNshowValue, False); i++;
	XtSetArg(arg[i], XmNminimum, -50); i++;
	XtSetArg(arg[i], XmNmaximum, 50); i++;
	XtSetArg(arg[i], XmNvalue, 0); i++;
	XtSetArg(arg[i], XmNorientation, XmHORIZONTAL); i++;
	XtSetArg(arg[i], XmNprocessingDirection, XmMAX_ON_RIGHT); i++;
	XtSetArg(arg[i], XmNhighlightOnEnter, True); i++;
	m->options.bal_scale = XmCreateScale(
		m->options.form,
		"balanceScale",
		arg,
		i
	);

	/* Create pushbutton widget as balance center button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.balctr_btn = XmCreatePushButton(
		m->options.form,
		"balanceCenterButton",
		arg,
		i
	);

	/* Create separator bar widget */
	m->options.options_sep = XmCreateSeparator(
		m->options.form,
		"optionsSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as reset button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.reset_btn = XmCreatePushButton(
		m->options.form,
		"resetButton",
		arg,
		i
	);

	/* Create pushbutton widget as save button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.save_btn = XmCreatePushButton(
		m->options.form,
		"saveButton",
		arg,
		i
	);

	/* Create pushbutton widget as OK button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->options.ok_btn = XmCreatePushButton(
		m->options.form,
		"okButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->options.load_lbl);
	XtManageChild(m->options.load_chkbox_frm);
	XtManageChild(m->options.load_chkbox);
	XtManageChild(m->options.load_radbox_frm);
	XtManageChild(m->options.load_radbox);
	XtManageChild(m->options.exit_lbl);
	XtManageChild(m->options.exit_radbox_frm);
	XtManageChild(m->options.exit_radbox);
	XtManageChild(m->options.done_lbl);
	XtManageChild(m->options.done_chkbox_frm);
	XtManageChild(m->options.done_chkbox);
	XtManageChild(m->options.eject_lbl);
	XtManageChild(m->options.eject_chkbox_frm);
	XtManageChild(m->options.eject_chkbox);
	XtManageChild(m->options.chg_lbl);
	XtManageChild(m->options.chg_chkbox_frm);
	XtManageChild(m->options.chg_chkbox);
	XtManageChild(m->options.chroute_lbl);
	XtManageChild(m->options.chroute_radbox_frm);
	XtManageChild(m->options.chroute_radbox);
	XtManageChild(m->options.vol_lbl);
	XtManageChild(m->options.vol_radbox_frm);
	XtManageChild(m->options.vol_radbox);
	XtManageChild(m->options.bal_lbl);
	XtManageChild(m->options.ball_lbl);
	XtManageChild(m->options.balr_lbl);
	XtManageChild(m->options.bal_scale);
	XtManageChild(m->options.balctr_btn);
	XtManageChild(m->options.options_sep);
	XtManageChild(m->options.reset_btn);
	XtManageChild(m->options.save_btn);
	XtManageChild(m->options.ok_btn);
}


/*
 * create_dbprog_widgets
 *	Create all widgets in the database/program window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_dbprog_widgets(widgets_t *m)
{
	int		i;
	Arg		arg[10];
	curstat_t	*s = curstat_addr();

	dbprog_radbox_cblist[0].closure = (XtPointer) s;

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->dbprog.form = XmCreateFormDialog(
		m->toplevel,
		"dbprogForm",
		arg,
		i
	);

	/* Create label widget as total time label */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.tottime_lbl = XmCreateLabel(
		m->dbprog.form,
		"totalTimeLabel",
		arg,
		i
	);

	/* Create label widget as total time indicator */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.tottime_ind = XmCreateLabel(
		m->dbprog.form,
		"totalTimeIndicator",
		arg,
		i
	);

	/* Create label widget as logo */
	m->dbprog.logo_lbl = XmCreateLabel(
		m->dbprog.form,
		"logoLabel",
		NULL,
		0
	);

	/* Create pushbutton widget as web access button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.wwwwarp_btn = XmCreatePushButton(
		m->dbprog.form,
		"wwwWarpButton",
		arg,
		i
	);

	/* Create toggle button for CDDB remote disable */
	i = 0;
	XtSetArg(arg[i], XmNshadowThickness, 0); i++;
	XtSetArg(arg[i], XmNalignment, XmALIGNMENT_BEGINNING); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.rmtdsbl_btn = XmCreateToggleButton(
		m->dbprog.form,
		"cddbRemoteDisableButton",
		arg,
		i
	);

	/* Create label widget as disc title display/editor label */
	m->dbprog.dtitle_lbl = XmCreateLabel(
		m->dbprog.form,
		"discTitleLabel",
		NULL,
		0
	);

	/* Create label widget as disc ext descr label */
	m->dbprog.extd_lbl = XmCreateLabel(
		m->dbprog.form,
		"discLabel",
		NULL,
		0
	);

	/* Create text widget as disc title display/editor */
	i = 0;
	XtSetArg(arg[i], XmNeditable, True); i++;
	XtSetArg(arg[i], XmNeditMode, XmSINGLE_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, True); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.dtitle_txt = XmCreateText(
		m->dbprog.form,
		"discTitleText",
		arg,
		i
	);

	/* Create pushbutton widget as disc title ext descr popup button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.extd_btn = XmCreatePushButton(
		m->dbprog.form,
		"extDiscInfoButton",
		arg,
		i
	);

	/* Create separator bar widget */
	m->dbprog.dbprog_sep1 = XmCreateSeparator(
		m->dbprog.form,
		"dbprogSeparator1",
		NULL,
		0
	);

	/* Create label widget as track list label */
	m->dbprog.trklist_lbl = XmCreateLabel(
		m->dbprog.form,
		"trackListLabel",
		NULL,
		0
	);

	/* Create scrolled list widget as track list */
	i = 0;
	XtSetArg(arg[i], XmNautomaticSelection, False); i++;
	XtSetArg(arg[i], XmNselectionPolicy, XmBROWSE_SELECT); i++;
	XtSetArg(arg[i], XmNlistSizePolicy, XmCONSTANT); i++;
	XtSetArg(arg[i], XmNscrollBarDisplayPolicy, XmSTATIC); i++;
	XtSetArg(arg[i], XmNscrolledWindowMarginWidth, 2); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.trk_list = XmCreateScrolledList(
		m->dbprog.form,
		"trackList",
		arg,
		i
	);

	/* Create label widget as time mode selector label */
	m->dbprog.radio_lbl = XmCreateLabel(
		m->dbprog.form,
		"timeSelectLabel",
		NULL,
		0
	);

	/* Create frame for radio box */
	m->dbprog.radio_frm = XmCreateFrame(
		m->dbprog.form,
		"timeSelectFrame",
		NULL,
		0
	);

	/* Create radio box widget as time mode selector */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 2); i++;
	XtSetArg(arg[i], XmNbuttonSet, 1); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNradioAlwaysOne, True); i++;
	XtSetArg(arg[i], XmNentryCallback, dbprog_radbox_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.radio_box = XmCreateSimpleRadioBox(
		m->dbprog.radio_frm,
		"timeSelectBox",
		arg,
		i
	);
	m->dbprog.tottime_btn = XtNameToWidget(m->dbprog.radio_box, "button_0");
	m->dbprog.trktime_btn = XtNameToWidget(m->dbprog.radio_box, "button_1");

	/* Create label widget as disc ID indicator label */
	m->dbprog.discid_lbl = XmCreateLabel(
		m->dbprog.form,
		"discIdLabel",
		NULL,
		0
	);

	/* Create frame for disc ID indicator */
	m->dbprog.discid_frm = XmCreateFrame(
		m->dbprog.form,
		"discIdFrame",
		NULL,
		0
	);

	/* Create label widget as disc ID indicator */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.discid_ind = XmCreateLabel(
		m->dbprog.discid_frm,
		"discIdIndicator",
		arg,
		i
	);

	/* Create label widget as track title display/editor label */
	m->dbprog.ttitle_lbl = XmCreateLabel(
		m->dbprog.form,
		"trackTitleLabel",
		NULL,
		0
	);

	/* Create label widget as disc ext descr label */
	m->dbprog.extt_lbl = XmCreateLabel(
		m->dbprog.form,
		"trackLabel",
		NULL,
		0
	);

	/* Create text widget as track title display/editor */
	i = 0;
	XtSetArg(arg[i], XmNeditable, True); i++;
	XtSetArg(arg[i], XmNeditMode, XmSINGLE_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, True); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.ttitle_txt = XmCreateText(
		m->dbprog.form,
		"trackTitleText",
		arg,
		i
	);

	/* Create pushbutton widget as track title ext descr popup button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.extt_btn = XmCreatePushButton(
		m->dbprog.form,
		"extTrackInfoButton",
		arg,
		i
	);

	/* Create label widget as program pushbuttons label */
	m->dbprog.pgm_lbl = XmCreateLabel(
		m->dbprog.form,
		"programLabel",
		NULL,
		0
	);

	/* Create pushbutton widget as Add PGM button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.addpgm_btn = XmCreatePushButton(
		m->dbprog.form,
		"addProgramButton",
		arg,
		i
	);

	/* Create pushbutton widget as Clear PGM button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.clrpgm_btn = XmCreatePushButton(
		m->dbprog.form,
		"clearProgramButton",
		arg,
		i
	);

	/* Create label widget as program sequence display/editor label */
	m->dbprog.pgmseq_lbl = XmCreateLabel(
		m->dbprog.form,
		"programSequenceLabel",
		NULL,
		0
	);

	/* Create text widget as program sequence display/editor */
	i = 0;
	XtSetArg(arg[i], XmNeditable, True); i++;
	XtSetArg(arg[i], XmNeditMode, XmSINGLE_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, True); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.pgmseq_txt = XmCreateText(
		m->dbprog.form,
		"programSequenceText",
		arg,
		i
	);

	/* Create pushbutton widget as Send CDDB button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.send_btn = XmCreatePushButton(
		m->dbprog.form,
		"sendButton",
		arg,
		i
	);

	/* Create separator bar widget */
	m->dbprog.dbprog_sep2 = XmCreateSeparator(
		m->dbprog.form,
		"dbprogSeparator2",
		NULL,
		0
	);

	/* Create pushbutton widget as Save DB button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.savedb_btn = XmCreatePushButton(
		m->dbprog.form,
		"saveDatabaseButton",
		arg,
		i
	);

	/* Create pushbutton widget as Link DB button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.linkdb_btn = XmCreatePushButton(
		m->dbprog.form,
		"linkDatabaseButton",
		arg,
		i
	);

	/* Create pushbutton widget as Load DB button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.loaddb_btn = XmCreatePushButton(
		m->dbprog.form,
		"loadDatabaseButton",
		arg,
		i
	);

	/* Create pushbutton widget as Cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbprog.cancel_btn = XmCreatePushButton(
		m->dbprog.form,
		"dbprogCancelButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->dbprog.wwwwarp_btn);
	XtManageChild(m->dbprog.logo_lbl);
	XtManageChild(m->dbprog.tottime_lbl);
	XtManageChild(m->dbprog.tottime_ind);
	XtManageChild(m->dbprog.rmtdsbl_btn);
	XtManageChild(m->dbprog.dtitle_lbl);
	XtManageChild(m->dbprog.dtitle_txt);
	XtManageChild(m->dbprog.extd_lbl);
	XtManageChild(m->dbprog.extd_btn);
	XtManageChild(m->dbprog.dbprog_sep1);
	XtManageChild(m->dbprog.trklist_lbl);
	XtManageChild(m->dbprog.trk_list);
	XtManageChild(m->dbprog.radio_lbl);
	XtManageChild(m->dbprog.radio_frm);
	XtManageChild(m->dbprog.radio_box);
	XtManageChild(m->dbprog.discid_lbl);
	XtManageChild(m->dbprog.discid_frm);
	XtManageChild(m->dbprog.discid_ind);
	XtManageChild(m->dbprog.ttitle_lbl);
	XtManageChild(m->dbprog.ttitle_txt);
	XtManageChild(m->dbprog.extt_lbl);
	XtManageChild(m->dbprog.extt_btn);
	XtManageChild(m->dbprog.pgm_lbl);
	XtManageChild(m->dbprog.addpgm_btn);
	XtManageChild(m->dbprog.clrpgm_btn);
	XtManageChild(m->dbprog.pgmseq_lbl);
	XtManageChild(m->dbprog.pgmseq_txt);
	XtManageChild(m->dbprog.send_btn);
	XtManageChild(m->dbprog.dbprog_sep2);
	XtManageChild(m->dbprog.savedb_btn);
	XtManageChild(m->dbprog.linkdb_btn);
	XtManageChild(m->dbprog.loaddb_btn);
	XtManageChild(m->dbprog.cancel_btn);
}


/*
 * create_wwwwarp_widgets
 *	Create all widgets in the wwwWarp window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_wwwwarp_widgets(widgets_t *m)
{
	int		i;
	Arg		arg[10];
	curstat_t	*s = curstat_addr();

	wwwwarp_radbox_cblist[0].closure = (XtPointer) s;

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->wwwwarp.form = XmCreateFormDialog(
		m->dbprog.wwwwarp_btn,
		"wwwWarpForm",
		arg,
		i
	);

	/* Create label widget as wwwwarp logo */
	i = 0;
	XtSetArg(arg[i], XmNlabelType, XmPIXMAP); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->wwwwarp.wwwwarp_lbl = XmCreateLabel(
		m->wwwwarp.form,
		"wwwWarpLogoLabel",
		arg,
		i
	);

	/* Create label widget as "Select one" label */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->wwwwarp.sel_lbl = XmCreateLabel(
		m->wwwwarp.form,
		"selectOneLabel",
		arg,
		i
	);

	/* Create radio box widget as sites selector */
	i = 0;
	XtSetArg(arg[i], XmNbuttonCount, 4); i++;
	XtSetArg(arg[i], XmNbuttonSet, 0); i++;
	XtSetArg(arg[i], XmNspacing, 1); i++;
	XtSetArg(arg[i], XmNmarginHeight, 0); i++;
	XtSetArg(arg[i], XmNshadowThickness, 0); i++;
	XtSetArg(arg[i], XmNradioAlwaysOne, True); i++;
	XtSetArg(arg[i], XmNentryCallback, wwwwarp_radbox_cblist); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->wwwwarp.sel_radbox = XmCreateSimpleRadioBox(
		m->wwwwarp.form,
		"siteSelectBox",
		arg,
		i
	);
	m->wwwwarp.site1_btn = XtNameToWidget(
		m->wwwwarp.sel_radbox, "button_0"
	);
	m->wwwwarp.site2_btn = XtNameToWidget(
		m->wwwwarp.sel_radbox, "button_1"
	);
	m->wwwwarp.site3_btn = XtNameToWidget(
		m->wwwwarp.sel_radbox, "button_2"
	);
	m->wwwwarp.srchweb_btn = XtNameToWidget(
		m->wwwwarp.sel_radbox, "button_3"
	);
	/* Make the 4th button insensitive until we add some
	 * items to the Search engines menu.
	 */
	XtSetSensitive(m->wwwwarp.srchweb_btn, False);

	/* Create pulldown menu widget for search site selector */
	m->wwwwarp.srchsite_menu = XmCreatePulldownMenu(
		m->wwwwarp.form,
		"searchSitePulldownMenu",
		NULL,
		0
	);

	/* Create a label for the menu */
	m->wwwwarp.srchsite_lbl = XmCreateLabel(
		m->wwwwarp.srchsite_menu,
		"searchSiteMenuLabel",
		NULL,
		0
	);

	/* Create separator bar widget */
	i = 0;
	XtSetArg(arg[i], XmNseparatorType, XmDOUBLE_LINE); i++;
	m->wwwwarp.srchsite_sep = XmCreateSeparator(
		m->wwwwarp.srchsite_menu,
		"searchSiteMenuSeparator",
		arg,
		i
	);

	/* Create option menu widget for search site selector */
	i = 0;
	XtSetArg(arg[i], XmNsubMenuId, m->wwwwarp.srchsite_menu); i++;
	XtSetArg(arg[i], XmNsensitive, False); i++;
	XtSetArg(arg[i], XmNshadowThickness, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->wwwwarp.srchsite_opt = XmCreateOptionMenu(
		m->wwwwarp.form,
		"searchSiteOptionMenu",
		arg,
		i
	);

	/* Create separator bar widget */
	m->wwwwarp.wwwwarp_sep = XmCreateSeparator(
		m->wwwwarp.form,
		"wwwWarpSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as OK button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->wwwwarp.go_btn = XmCreatePushButton(
		m->wwwwarp.form,
		"wwwWarpGoButton",
		arg,
		i
	);

	/* Create pushbutton widget as Cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->wwwwarp.cancel_btn = XmCreatePushButton(
		m->wwwwarp.form,
		"wwwWarpCancelButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->wwwwarp.wwwwarp_lbl);
	XtManageChild(m->wwwwarp.sel_lbl);
	XtManageChild(m->wwwwarp.sel_radbox);
	XtManageChild(m->wwwwarp.srchsite_lbl);
	XtManageChild(m->wwwwarp.srchsite_sep);
	XtManageChild(m->wwwwarp.srchsite_opt);
	XtManageChild(m->wwwwarp.wwwwarp_sep);
	XtManageChild(m->wwwwarp.go_btn);
	XtManageChild(m->wwwwarp.cancel_btn);
}


/*
 * create_exttxt_widgets
 *	Create all widgets in the extended disc information and
 *	extended track information windows.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_exttxt_widgets(widgets_t *m)
{
	int		i;
	Arg		arg[10];

	/* Extended disc info window */

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->dbextd.form = XmCreateFormDialog(
		m->dbprog.extd_btn,
		"extDiscInfoForm",
		arg,
		i
	);

	/* Create label widget as extended disc number label */
	m->dbextd.discno_lbl = XmCreateLabel(
		m->dbextd.form,
		"extDiscNumberLabel",
		NULL,
		0
	);

	/* Create label widget as extended disc info label */
	m->dbextd.disc_lbl = XmCreateLabel(
		m->dbextd.form,
		"extDiscInfoLabel",
		NULL,
		0
	);

	/* Create text widget as extended disc info editor/viewer */
	i = 0;
	XtSetArg(arg[i], XmNeditable, True); i++;
	XtSetArg(arg[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, True); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextd.disc_txt = XmCreateScrolledText(
		m->dbextd.form,
		"extDiscInfoText",
		arg,
		i
	);

	/* Create separator bar widget */
	m->dbextd.dbextd_sep = XmCreateSeparator(
		m->dbextd.form,
		"extDiscInfoSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as OK button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextd.ok_btn = XmCreatePushButton(
		m->dbextd.form,
		"extDiscInfoOkButton",
		arg,
		i
	);

	/* Create pushbutton widget as Clear button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextd.clear_btn = XmCreatePushButton(
		m->dbextd.form,
		"extDiscInfoClearButton",
		arg,
		i
	);

	/* Create pushbutton widget as Cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextd.cancel_btn = XmCreatePushButton(
		m->dbextd.form,
		"extDiscInfoCancelButton",
		arg,
		i
	);

	/* Extended track info window */

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->dbextt.form = XmCreateFormDialog(
		m->dbprog.extt_btn,
		"extTrackInfoForm",
		arg,
		i
	);

	/* Create arrow button widget as prev track button */
	i = 0;
	XtSetArg(arg[i], XmNtraversalOn, True); i++;
	XtSetArg(arg[i], XmNarrowDirection, XmARROW_LEFT); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextt.prev_btn = XmCreateArrowButton(
		m->dbextt.form,
		"extTrackPrevButton",
		arg,
		i
	);

	/* Create arrow button widget as next track button */
	i = 0;
	XtSetArg(arg[i], XmNtraversalOn, True); i++;
	XtSetArg(arg[i], XmNarrowDirection, XmARROW_RIGHT); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextt.next_btn = XmCreateArrowButton(
		m->dbextt.form,
		"extTrackNextButton",
		arg,
		i
	);

	/* Create label widget as extended track number label */
	m->dbextt.trkno_lbl = XmCreateLabel(
		m->dbextt.form,
		"extTrackNumberLabel",
		NULL,
		0
	);

	/* Create toggle button for auto-track */
	i = 0;
	XtSetArg(arg[i], XmNshadowThickness, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextt.autotrk_btn = XmCreateToggleButton(
		m->dbextt.form,
		"extTrackAutoTrackButton",
		arg,
		i
	);

	/* Create label widget as extended track info label */
	m->dbextt.trk_lbl = XmCreateLabel(
		m->dbextt.form,
		"extTrackInfoLabel",
		NULL,
		0
	);

	/* Create text widget as extended track info editor/viewer */
	i = 0;
	XtSetArg(arg[i], XmNeditable, True); i++;
	XtSetArg(arg[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, True); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextt.trk_txt = XmCreateScrolledText(
		m->dbextt.form,
		"extTrackInfoText",
		arg,
		i
	);

	/* Create separator bar widget */
	m->dbextt.dbextt_sep = XmCreateSeparator(
		m->dbextt.form,
		"extTrackInfoSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as OK button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextt.ok_btn = XmCreatePushButton(
		m->dbextt.form,
		"extTrackInfoOkButton",
		arg,
		i
	);

	/* Create pushbutton widget as Clear button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextt.clear_btn = XmCreatePushButton(
		m->dbextt.form,
		"extTrackInfoClearButton",
		arg,
		i
	);

	/* Create pushbutton widget as Cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dbextt.cancel_btn = XmCreatePushButton(
		m->dbextt.form,
		"extTrackInfoCancelButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->dbextd.discno_lbl);
	XtManageChild(m->dbextd.disc_lbl);
	XtManageChild(m->dbextd.disc_txt);
	XtManageChild(m->dbextd.dbextd_sep);
	XtManageChild(m->dbextd.ok_btn);
	XtManageChild(m->dbextd.clear_btn);
	XtManageChild(m->dbextd.cancel_btn);

	XtManageChild(m->dbextt.prev_btn);
	XtManageChild(m->dbextt.next_btn);
	XtManageChild(m->dbextt.trkno_lbl);
	XtManageChild(m->dbextt.autotrk_btn);
	XtManageChild(m->dbextt.trk_lbl);
	XtManageChild(m->dbextt.trk_txt);
	XtManageChild(m->dbextt.dbextt_sep);
	XtManageChild(m->dbextt.ok_btn);
	XtManageChild(m->dbextt.clear_btn);
	XtManageChild(m->dbextt.cancel_btn);
}


/*
 * create_dirsel_widgets
 *	Create all widgets in the CD database directory selector window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_dirsel_widgets(widgets_t *m)
{
	int	i;
	Arg	arg[10];

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	m->dirsel.form = XmCreateFormDialog(
		m->toplevel,
		"dirSelectForm",
		arg,
		i
	);

	/* Create label widget as database directory selector label */
	m->dirsel.dir_lbl = XmCreateLabel(
		m->dirsel.form,
		"dirSelectLabel",
		NULL,
		0
	);

	/* Create scrolled list widget as directory list */
	i = 0;
	XtSetArg(arg[i], XmNautomaticSelection, False); i++;
	XtSetArg(arg[i], XmNselectionPolicy, XmBROWSE_SELECT); i++;
	XtSetArg(arg[i], XmNlistSizePolicy, XmCONSTANT); i++;
	XtSetArg(arg[i], XmNscrollBarDisplayPolicy, XmSTATIC); i++;
	XtSetArg(arg[i], XmNscrolledWindowMarginWidth, 2); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dirsel.dir_list = XmCreateScrolledList(
		m->dirsel.form,
		"dirSelectList",
		arg,
		i
	);

	/* Create separator bar widget */
	m->dirsel.dirsel_sep = XmCreateSeparator(
		m->dirsel.form,
		"dirSelectSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as OK button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dirsel.ok_btn = XmCreatePushButton(
		m->dirsel.form,
		"dirSelectOkButton",
		arg,
		i
	);

	/* Create pushbutton widget as Cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->dirsel.cancel_btn = XmCreatePushButton(
		m->dirsel.form,
		"dirSelectCancelButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->dirsel.dir_lbl);
	XtManageChild(m->dirsel.dir_list);
	XtManageChild(m->dirsel.dirsel_sep);
	XtManageChild(m->dirsel.ok_btn);
	XtManageChild(m->dirsel.cancel_btn);
}


/*
 * create_linksel_widgets
 *	Create all widgets in the CD database search-link window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_linksel_widgets(widgets_t *m)
{
	int	i;
	Arg	arg[10];

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	m->linksel.form = XmCreateFormDialog(
		m->dbprog.linkdb_btn,
		"linkSelectForm",
		arg,
		i
	);

	/* Create label widget as cddb link selector label */
	m->linksel.link_lbl = XmCreateLabel(
		m->linksel.form,
		"linkSelectLabel",
		NULL,
		0
	);

	/* Create scrolled list widget as disc titles list */
	i = 0;
	XtSetArg(arg[i], XmNautomaticSelection, False); i++;
	XtSetArg(arg[i], XmNselectionPolicy, XmBROWSE_SELECT); i++;
	XtSetArg(arg[i], XmNlistSizePolicy, XmCONSTANT); i++;
	XtSetArg(arg[i], XmNscrollBarDisplayPolicy, XmSTATIC); i++;
	XtSetArg(arg[i], XmNscrolledWindowMarginWidth, 2); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->linksel.link_list = XmCreateScrolledList(
		m->linksel.form,
		"linkSelectList",
		arg,
		i
	);

	/* Create separator bar widget */
	m->linksel.linksel_sep = XmCreateSeparator(
		m->linksel.form,
		"linkSelectSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as OK button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->linksel.ok_btn = XmCreatePushButton(
		m->linksel.form,
		"linkSelectOkButton",
		arg,
		i
	);

	/* Create pushbutton widget as Cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->linksel.cancel_btn = XmCreatePushButton(
		m->linksel.form,
		"linkSelectCancelButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->linksel.link_lbl);
	XtManageChild(m->linksel.link_list);
	XtManageChild(m->linksel.linksel_sep);
	XtManageChild(m->linksel.ok_btn);
	XtManageChild(m->linksel.cancel_btn);
}


/*
 * create_help_widgets
 *	Create all widgets in the help text display window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_help_widgets(widgets_t *m)
{
	int	i;
	Arg	arg[10];

	/* Help popup window */

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->help.form = XmCreateFormDialog(
		m->toplevel,
		"helpForm",
		arg,
		i
	);

	/* Create pulldown menu widget for topic selector */
	m->help.topic_menu = XmCreatePulldownMenu(
		m->help.form,
		"topicPulldownMenu",
		NULL,
		0
	);

	/* Create pushbutton widget for online help menu entry */
	m->help.online_btn = XmCreatePushButton(
		m->help.topic_menu,
		"onlineHelpButton",
		NULL,
		0
	);

	/* Create separator bar widget as menu separator */
	m->help.menu_sep = XmCreateSeparator(
		m->help.topic_menu,
		"topicSeparator",
		NULL,
		0
	);

	/* Create label widget for xmcd docs menu title */
	m->help.docs_lbl = XmCreateLabel(
		m->help.topic_menu,
		"docsLabel",
		NULL,
		0
	);

	/* Create separator bar widget as menu separator */
	i = 0;
	XtSetArg(arg[i], XmNseparatorType, XmDOUBLE_LINE); i++;
	m->help.menu_sep2 = XmCreateSeparator(
		m->help.topic_menu,
		"topicSeparator",
		arg,
		i
	);

	/* Create option menu widget for topic selector */
	i = 0;
	XtSetArg(arg[i], XmNsubMenuId, m->help.topic_menu); i++;
	XtSetArg(arg[i], XmNshadowThickness, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->help.topic_opt = XmCreateOptionMenu(
		m->help.form,
		"topicOptionMenu",
		arg,
		i
	);

	/* Create text widget as help text viewer */
	i = 0;
	XtSetArg(arg[i], XmNeditable, False); i++;
	XtSetArg(arg[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, False); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->help.help_txt = XmCreateScrolledText(
		m->help.form,
		"helpText",
		arg,
		i
	);

	/* Create separator bar widget */
	m->help.help_sep = XmCreateSeparator(
		m->help.form,
		"helpSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as about button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->help.about_btn = XmCreatePushButton(
		m->help.form,
		"aboutButton",
		arg,
		i
	);

	/* Create pushbutton widget as Cancel button */
	i = 0;
	XtSetArg(arg[i], XmNhelpCallback, help_cblist); i++;
	m->help.cancel_btn = XmCreatePushButton(
		m->help.form,
		"helpCancelButton",
		arg,
		i
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->help.online_btn);
	XtManageChild(m->help.menu_sep);
	XtManageChild(m->help.docs_lbl);
	XtManageChild(m->help.menu_sep2);
	XtManageChild(m->help.help_txt);
	XtManageChild(m->help.help_sep);
	XtManageChild(m->help.topic_opt);
	XtManageChild(m->help.about_btn);
	XtManageChild(m->help.cancel_btn);
}


/*
 * create_auth_widgets
 *	Create all widgets in the authorization window.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_auth_widgets(widgets_t *m)
{
	int	i;
	Arg	arg[10];

	/* Authorization popup window */

	/* Create form widget as container */
	i = 0;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	XtSetArg(arg[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	m->auth.form = XmCreateFormDialog(
		m->toplevel,
		"authForm",
		arg,
		i
	);

	/* Create label widget as auth label */
	m->auth.auth_lbl = XmCreateLabel(
		m->auth.form,
		"authLabel",
		NULL,
		0
	);

	/* Create label widget as name label */
	m->auth.name_lbl = XmCreateLabel(
		m->auth.form,
		"nameLabel",
		NULL,
		0
	);

	/* Create text widget as name input text field */
	XtSetArg(arg[i], XmNeditable, True); i++;
	XtSetArg(arg[i], XmNeditMode, XmSINGLE_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, True); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	m->auth.name_txt = XmCreateText(
		m->auth.form,
		"nameText",
		arg,
		i
	);

	/* Create label widget as password label */
	m->auth.pass_lbl = XmCreateLabel(
		m->auth.form,
		"passwordLabel",
		NULL,
		0
	);

	/* Create text widget as password input text field */
	XtSetArg(arg[i], XmNeditable, True); i++;
	XtSetArg(arg[i], XmNeditMode, XmSINGLE_LINE_EDIT); i++;
	XtSetArg(arg[i], XmNcursorPositionVisible, True); i++;
	XtSetArg(arg[i], XmNcursorPosition, 0); i++;
	m->auth.pass_txt = XmCreateText(
		m->auth.form,
		"passwordText",
		arg,
		i
	);

	/* Create separator bar widget */
	m->auth.auth_sep = XmCreateSeparator(
		m->auth.form,
		"authSeparator",
		NULL,
		0
	);

	/* Create pushbutton widget as OK button */
	m->auth.ok_btn = XmCreatePushButton(
		m->auth.form,
		"authOkButton",
		NULL,
		0
	);

	/* Create pushbutton widget as Cancel button */
	m->auth.cancel_btn = XmCreatePushButton(
		m->auth.form,
		"authCancelButton",
		NULL,
		0
	);

	/* Manage the widgets (except the form) */
	XtManageChild(m->auth.auth_lbl);
	XtManageChild(m->auth.name_lbl);
	XtManageChild(m->auth.name_txt);
	XtManageChild(m->auth.pass_lbl);
	XtManageChild(m->auth.pass_txt);
	XtManageChild(m->auth.auth_sep);
	XtManageChild(m->auth.ok_btn);
	XtManageChild(m->auth.cancel_btn);
}


/*
 * create_dialog_widgets
 *	Create all widgets in the dialog box windows.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_dialog_widgets(widgets_t *m)
{
	int	i;
	Arg	arg[10];
	Widget	help_btn,
		cancel_btn;

	/* Create info dialog widget for information messages */
	i = 0;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_MODELESS); i++;
	XtSetArg(arg[i], XmNautoUnmanage, True); i++;
	m->dialog.info = XmCreateInformationDialog(
		m->toplevel,
		"infoPopup",
		arg,
		i
	);

	/* Remove unused buttons in the info dialog widget */
	help_btn = XmMessageBoxGetChild(
		m->dialog.info,
		XmDIALOG_HELP_BUTTON
	);
	cancel_btn = XmMessageBoxGetChild(
		m->dialog.info,
		XmDIALOG_CANCEL_BUTTON
	);

	XtUnmanageChild(help_btn);
	XtUnmanageChild(cancel_btn);

	/* Create warning dialog widget for warning messages */
	i = 0;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	XtSetArg(arg[i], XmNautoUnmanage, True); i++;
	m->dialog.warning = XmCreateWarningDialog(
		m->toplevel,
		"warningPopup",
		arg,
		i
	);

	/* Remove unused buttons in the warning dialog widget */
	help_btn = XmMessageBoxGetChild(
		m->dialog.warning,
		XmDIALOG_HELP_BUTTON
	);
	cancel_btn = XmMessageBoxGetChild(
		m->dialog.warning,
		XmDIALOG_CANCEL_BUTTON
	);

	XtUnmanageChild(help_btn);
	XtUnmanageChild(cancel_btn);

	/* Create error dialog widget for fatal error messages */
	i = 0;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	XtSetArg(arg[i], XmNautoUnmanage, True); i++;
	m->dialog.fatal = XmCreateErrorDialog(
		m->toplevel,
		"fatalPopup",
		arg,
		i
	);

	/* Remove unused buttons in the error dialog widget */
	help_btn = XmMessageBoxGetChild(
		m->dialog.fatal,
		XmDIALOG_HELP_BUTTON
	);
	cancel_btn = XmMessageBoxGetChild(
		m->dialog.fatal,
		XmDIALOG_CANCEL_BUTTON
	);

	XtUnmanageChild(help_btn);
	XtUnmanageChild(cancel_btn);

	/* Create question dialog widget for confirm messages */
	i = 0;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	XtSetArg(arg[i], XmNautoUnmanage, True); i++;
	m->dialog.confirm = XmCreateQuestionDialog(
		m->toplevel,
		"questionPopup",
		arg,
		i
	);

	/* Remove unused buttons in the question dialog widget */
	help_btn = XmMessageBoxGetChild(
		m->dialog.confirm,
		XmDIALOG_HELP_BUTTON
	);

	XtUnmanageChild(help_btn);

	/* Create CDDB match query dialog box widget */
	i = 0;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_MODELESS); i++;
	XtSetArg(arg[i], XmNautoUnmanage, False); i++;
	m->dialog.dbmatch = XmCreateQuestionDialog(
		m->toplevel,
		"cddbMatchPopup",
		arg,
		i
	);

	/* Create info dialog widget for the About popup */
	i = 0;
	XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_MODELESS); i++;
	XtSetArg(arg[i], XmNautoUnmanage, True); i++;
	m->dialog.about = XmCreateInformationDialog(
		m->help.form,
		"aboutPopup",
		arg,
		i
	);

	/* Remove unused buttons in the about popup */
	help_btn = XmMessageBoxGetChild(
		m->dialog.about,
		XmDIALOG_HELP_BUTTON
	);
	cancel_btn = XmMessageBoxGetChild(
		m->dialog.about,
		XmDIALOG_CANCEL_BUTTON
	);

	XtUnmanageChild(help_btn);
	XtUnmanageChild(cancel_btn);
}


/*
 * create_tooltip_widgets
 *	Create all widgets for the tooltip popup feature.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
create_tooltip_widgets(widgets_t *m)
{
	int	i;
	Arg	arg[10];

	if (!app_data.tooltip_enable)
		return;

	/* Create shell widget */
	i = 0;
	XtSetArg(arg[i], XmNborderWidth, 1); i++;
	XtSetArg(arg[i], XmNallowShellResize, True); i++;
	XtSetArg(arg[i], XmNoverrideRedirect, True); i++;
	m->tooltip.shell = XtCreatePopupShell(
		"tooltipShell",
		/* Some versions of LessTif will crash if we use
		 * transientShell.  But overrideShell seems to work
		 * fine.  However, the reverse is true for some
		 * versions of Motif on X11R6.  Sigh.
		 */
#ifdef LesstifVersion
		overrideShellWidgetClass,
#else
		transientShellWidgetClass,
#endif
		m->toplevel,
		arg,
		i
	);

	/* Create label widget for tooltip label string */
	i = 0;
	XtSetArg(arg[i], XmNrecomputeSize, True); i++;
	XtSetArg(arg[i], XmNmarginWidth, 4); i++;
	XtSetArg(arg[i], XmNmarginHeight, 3); i++;
	XtSetArg(arg[i], XmNlabelType, XmSTRING); i++;
	m->tooltip.tooltip_lbl = XmCreateLabel(
		m->tooltip.shell,
		"tooltipLabel",
		arg,
		i
	);

	/* Manage the widgets */
	XtManageChild(m->tooltip.tooltip_lbl);
}


/*
 * make_pixmaps
 *	Create pixmaps from bitmap data and set up various widgets to
 *	use them.
 *
 * Args:
 *	m - Pointer to the main widgets placeholder structure
 *	p - The main pixmaps placeholder structure
 *	depth - The desired depth of the pixmap
 *
 * Return:
 *	Nothing.
 */
STATIC void
make_pixmaps(widgets_t *m, pixmaps_t *p, int depth)
{
	/* Set icon pixmap */
	p->main.icon_pixmap = bm_to_px(
		m->toplevel,
		logo_bits,
		logo_width,
		logo_height,
		1,
		BM_PX_NORMAL
	);
	XtVaSetValues(m->toplevel, XmNiconPixmap, p->main.icon_pixmap, NULL);
					
	/*
	 * The following puts proper pixmaps on button faces
	 */

	p->main.mode_pixmap = bm_to_px(
		m->main.mode_btn,
		mode_bits,
		mode_width,
		mode_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.mode_hlpixmap = bm_to_px(
		m->main.mode_btn,
		mode_bits,
		mode_width,
		mode_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.mode_btn,
		XmNlabelPixmap, p->main.mode_pixmap,
		NULL
	);

	p->main.lock_pixmap = bm_to_px(
		m->main.check_box,
		lock_bits,
		lock_width,
		lock_height,
		depth,
		BM_PX_NORMAL
	);
	XtVaSetValues(m->main.lock_btn,
		XmNlabelPixmap, p->main.lock_pixmap,
		NULL
	);

	p->main.repeat_pixmap = bm_to_px(
		m->main.check_box,
		repeat_bits,
		repeat_width,
		repeat_height,
		depth,
		BM_PX_NORMAL
	);
	XtVaSetValues(m->main.repeat_btn,
		XmNlabelPixmap, p->main.repeat_pixmap,
		NULL
	);

	p->main.shuffle_pixmap = bm_to_px(
		m->main.check_box,
		shuffle_bits,
		shuffle_width,
		shuffle_height,
		depth,
		BM_PX_NORMAL
	);
	XtVaSetValues(m->main.shuffle_btn,
		XmNlabelPixmap, p->main.shuffle_pixmap,
		NULL
	);

	p->main.eject_pixmap = bm_to_px(
		m->main.eject_btn,
		eject_bits,
		eject_width,
		eject_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.eject_hlpixmap = bm_to_px(
		m->main.eject_btn,
		eject_bits,
		eject_width,
		eject_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.eject_btn,
		XmNlabelPixmap, p->main.eject_pixmap,
		NULL
	);

	p->main.quit_pixmap = bm_to_px(
		m->main.quit_btn,
		quit_bits,
		quit_width,
		quit_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.quit_hlpixmap = bm_to_px(
		m->main.quit_btn,
		quit_bits,
		quit_width,
		quit_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.quit_btn,
		XmNlabelPixmap, p->main.quit_pixmap,
		NULL
	);

	p->main.dbprog_pixmap = bm_to_px(
		m->main.dbprog_btn,
		dbprog_bits,
		dbprog_width,
		dbprog_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.dbprog_hlpixmap = bm_to_px(
		m->main.dbprog_btn,
		dbprog_bits,
		dbprog_width,
		dbprog_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.dbprog_btn,
		XmNlabelPixmap, p->main.dbprog_pixmap,
		NULL
	);

	p->main.options_pixmap = bm_to_px(
		m->main.options_btn,
		options_bits,
		options_width,
		options_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.options_hlpixmap = bm_to_px(
		m->main.options_btn,
		options_bits,
		options_width,
		options_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.options_btn,
		XmNlabelPixmap, p->main.options_pixmap,
		NULL
	);

	p->main.time_pixmap = bm_to_px(
		m->main.time_btn,
		time_bits,
		time_width,
		time_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.time_hlpixmap = bm_to_px(
		m->main.time_btn,
		time_bits,
		time_width,
		time_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.time_btn,
		XmNlabelPixmap, p->main.time_pixmap,
		NULL
	);

	p->main.ab_pixmap = bm_to_px(
		m->main.ab_btn,
		ab_bits,
		ab_width,
		ab_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.ab_hlpixmap = bm_to_px(
		m->main.ab_btn,
		ab_bits,
		ab_width,
		ab_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.ab_btn,
		XmNlabelPixmap, p->main.ab_pixmap,
		NULL
	);

	p->main.sample_pixmap = bm_to_px(
		m->main.sample_btn,
		sample_bits,
		sample_width,
		sample_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.sample_hlpixmap = bm_to_px(
		m->main.sample_btn,
		sample_bits,
		sample_width,
		sample_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.sample_btn,
		XmNlabelPixmap, p->main.sample_pixmap,
		NULL
	);

	p->main.keypad_pixmap = bm_to_px(
		m->main.keypad_btn,
		keypad_bits,
		keypad_width,
		keypad_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.keypad_hlpixmap = bm_to_px(
		m->main.keypad_btn,
		keypad_bits,
		keypad_width,
		keypad_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.keypad_btn,
		XmNlabelPixmap, p->main.keypad_pixmap,
		NULL
	);

	p->main.help_pixmap = bm_to_px(
		m->main.help_btn,
		help_bits,
		help_width,
		help_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.help_hlpixmap = bm_to_px(
		m->main.help_btn,
		help_bits,
		help_width,
		help_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.help_btn,
		XmNlabelPixmap, p->main.help_pixmap,
		NULL
	);

	p->main.playpause_pixmap = bm_to_px(
		m->main.playpause_btn,
		playpause_bits,
		playpause_width,
		playpause_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.playpause_hlpixmap = bm_to_px(
		m->main.playpause_btn,
		playpause_bits,
		playpause_width,
		playpause_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.playpause_btn,
		XmNlabelPixmap, p->main.playpause_pixmap,
		NULL
	);

	p->main.stop_pixmap = bm_to_px(
		m->main.stop_btn,
		stop_bits,
		stop_width,
		stop_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.stop_hlpixmap = bm_to_px(
		m->main.stop_btn,
		stop_bits,
		stop_width,
		stop_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.stop_btn,
		XmNlabelPixmap, p->main.stop_pixmap,
		NULL
	);

	p->main.prevdisc_pixmap = bm_to_px(
		m->main.prevdisc_btn,
		prevdisc_bits,
		prevdisc_width,
		prevdisc_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.prevdisc_hlpixmap = bm_to_px(
		m->main.prevdisc_btn,
		prevdisc_bits,
		prevdisc_width,
		prevdisc_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.prevdisc_btn,
		XmNlabelPixmap, p->main.prevdisc_pixmap,
		NULL
	);

	p->main.nextdisc_pixmap = bm_to_px(
		m->main.nextdisc_btn,
		nextdisc_bits,
		nextdisc_width,
		nextdisc_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.nextdisc_hlpixmap = bm_to_px(
		m->main.nextdisc_btn,
		nextdisc_bits,
		nextdisc_width,
		nextdisc_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.nextdisc_btn,
		XmNlabelPixmap, p->main.nextdisc_pixmap,
		NULL
	);

	p->main.prevtrk_pixmap = bm_to_px(
		m->main.prevtrk_btn,
		prevtrk_bits,
		prevtrk_width,
		prevtrk_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.prevtrk_hlpixmap = bm_to_px(
		m->main.prevtrk_btn,
		prevtrk_bits,
		prevtrk_width,
		prevtrk_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.prevtrk_btn,
		XmNlabelPixmap, p->main.prevtrk_pixmap,
		NULL
	);

	p->main.nexttrk_pixmap = bm_to_px(
		m->main.nexttrk_btn,
		nexttrk_bits,
		nexttrk_width,
		nexttrk_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.nexttrk_hlpixmap = bm_to_px(
		m->main.nexttrk_btn,
		nexttrk_bits,
		nexttrk_width,
		nexttrk_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.nexttrk_btn,
		XmNlabelPixmap, p->main.nexttrk_pixmap,
		NULL
	);

	p->main.previdx_pixmap = bm_to_px(
		m->main.previdx_btn,
		previdx_bits,
		previdx_width,
		previdx_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.previdx_hlpixmap = bm_to_px(
		m->main.previdx_btn,
		previdx_bits,
		previdx_width,
		previdx_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.previdx_btn,
		XmNlabelPixmap, p->main.previdx_pixmap,
		NULL
	);

	p->main.nextidx_pixmap = bm_to_px(
		m->main.nextidx_btn,
		nextidx_bits,
		nextidx_width,
		nextidx_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.nextidx_hlpixmap = bm_to_px(
		m->main.nextidx_btn,
		nextidx_bits,
		nextidx_width,
		nextidx_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.nextidx_btn,
		XmNlabelPixmap, p->main.nextidx_pixmap,
		NULL
	);

	p->main.rew_pixmap = bm_to_px(
		m->main.rew_btn,
		rew_bits,
		rew_width,
		rew_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.rew_hlpixmap = bm_to_px(
		m->main.rew_btn,
		rew_bits,
		rew_width,
		rew_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.rew_btn,
		XmNlabelPixmap, p->main.rew_pixmap,
		NULL
	);

	p->main.ff_pixmap = bm_to_px(
		m->main.ff_btn,
		ff_bits,
		ff_width,
		ff_height,
		depth,
		BM_PX_NORMAL
	);
	p->main.ff_hlpixmap = bm_to_px(
		m->main.ff_btn,
		ff_bits,
		ff_width,
		ff_height,
		depth,
		BM_PX_HIGHLIGHT
	);
	XtVaSetValues(m->main.ff_btn,
		XmNlabelPixmap, p->main.ff_pixmap,
		NULL
	);

	p->dbprog.logo_pixmap = bm_to_px(
		m->main.dbprog_btn,
		logo_bits,
		logo_width,
		logo_height,
		depth,
		BM_PX_NORMAL
	);
	XtVaSetValues(m->dbprog.logo_lbl,
		XmNlabelType, XmPIXMAP,
		XmNlabelPixmap, p->dbprog.logo_pixmap,
		XmNlabelInsensitivePixmap, p->dbprog.logo_pixmap,
		NULL
	);

	p->wwwwarp.wwwwarp_pixmap = bm_to_px(
		m->main.dtitle_ind,
		wwwwarp_bits,
		wwwwarp_width,
		wwwwarp_height,
		depth,
		BM_PX_NORMAL
	);
	XtVaSetValues(m->wwwwarp.wwwwarp_lbl,
		XmNlabelPixmap, p->wwwwarp.wwwwarp_pixmap,
		NULL
	);

	p->dialog.xmcd_pixmap = bm_to_px(
		m->main.dtitle_ind,
		xmcd_bits,
		xmcd_width,
		xmcd_height,
		depth,
		BM_PX_NORMAL
	);
	XtVaSetValues(m->dialog.about,
		XmNsymbolPixmap, p->dialog.xmcd_pixmap,
		NULL
	);
}


/*
 * register_main_callbacks
 *	Register all callback routines for widgets in the main window
 *
 * Args:
 *	m - Pointer to the main widgets structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
register_main_callbacks(widgets_t *m, curstat_t *s)
{
	/* Main window callbacks */
	register_focus_cb(m->main.form, cd_shell_focus_chg, m->main.form);

	register_arm_cb(m->main.mode_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.mode_btn, cd_mode, s);
	register_focuschg_ev(m->main.mode_btn);
	register_xingchg_ev(m->main.mode_btn);

	register_arm_cb(m->main.lock_btn, cd_tooltip_cancel, s);
	register_xingchg_ev(m->main.lock_btn);

	register_arm_cb(m->main.repeat_btn, cd_tooltip_cancel, s);
	register_xingchg_ev(m->main.repeat_btn);

	register_arm_cb(m->main.shuffle_btn, cd_tooltip_cancel, s);
	register_xingchg_ev(m->main.shuffle_btn);

	register_arm_cb(m->main.eject_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.eject_btn, cd_load_eject, s);
	register_focuschg_ev(m->main.eject_btn);
	register_xingchg_ev(m->main.eject_btn);

	register_arm_cb(m->main.quit_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.quit_btn, cd_quit_btn, s);
	register_focuschg_ev(m->main.quit_btn);
	register_xingchg_ev(m->main.quit_btn);

	register_xingchg_ev(m->main.disc_ind);
	register_xingchg_ev(m->main.track_ind);
	register_xingchg_ev(m->main.index_ind);
	register_xingchg_ev(m->main.time_ind);

	register_xingchg_ev(m->main.rptcnt_ind);
	register_xingchg_ev(m->main.dbmode_ind);
	register_xingchg_ev(m->main.progmode_ind);
	register_xingchg_ev(m->main.timemode_ind);
	register_xingchg_ev(m->main.playmode_ind);

	register_xingchg_ev(m->main.dtitle_ind);
	register_xingchg_ev(m->main.ttitle_ind);

	register_arm_cb(m->main.dbprog_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.dbprog_btn, cd_dbprog, s);
	register_focuschg_ev(m->main.dbprog_btn);
	register_xingchg_ev(m->main.dbprog_btn);

	register_arm_cb(m->main.options_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.options_btn, cd_options_popup, s);
	register_focuschg_ev(m->main.options_btn);
	register_xingchg_ev(m->main.options_btn);

	register_arm_cb(m->main.time_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.time_btn, cd_time, s);
	register_focuschg_ev(m->main.time_btn);
	register_xingchg_ev(m->main.time_btn);

	register_arm_cb(m->main.ab_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.ab_btn, cd_ab, s);
	register_focuschg_ev(m->main.ab_btn);
	register_xingchg_ev(m->main.ab_btn);

	register_arm_cb(m->main.sample_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.sample_btn, cd_sample, s);
	register_focuschg_ev(m->main.sample_btn);
	register_xingchg_ev(m->main.sample_btn);

	register_arm_cb(m->main.keypad_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.keypad_btn, cd_keypad_popup, s);
	register_focuschg_ev(m->main.keypad_btn);
	register_xingchg_ev(m->main.keypad_btn);

	register_arm_cb(m->main.help_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.help_btn, cd_help_popup, s);
	register_focuschg_ev(m->main.help_btn);
	register_xingchg_ev(m->main.help_btn);

	register_valchg_cb(m->main.level_scale, cd_tooltip_cancel, s);
	register_valchg_cb(m->main.level_scale, cd_level, s);
	register_drag_cb(m->main.level_scale, cd_tooltip_cancel, s);
	register_drag_cb(m->main.level_scale, cd_level, s);
	register_xingchg_ev(m->main.level_scale);

	register_arm_cb(m->main.playpause_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.playpause_btn, cd_play_pause, s);
	register_focuschg_ev(m->main.playpause_btn);
	register_xingchg_ev(m->main.playpause_btn);

	register_arm_cb(m->main.stop_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.stop_btn, cd_stop, s);
	register_focuschg_ev(m->main.stop_btn);
	register_xingchg_ev(m->main.stop_btn);

	register_arm_cb(m->main.prevdisc_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.prevdisc_btn, cd_chgdisc, s);
	register_focuschg_ev(m->main.prevdisc_btn);
	register_xingchg_ev(m->main.prevdisc_btn);

	register_arm_cb(m->main.nextdisc_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.nextdisc_btn, cd_chgdisc, s);
	register_focuschg_ev(m->main.nextdisc_btn);
	register_xingchg_ev(m->main.nextdisc_btn);

	register_arm_cb(m->main.prevtrk_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.prevtrk_btn, cd_prevtrk, s);
	register_focuschg_ev(m->main.prevtrk_btn);
	register_xingchg_ev(m->main.prevtrk_btn);

	register_arm_cb(m->main.nexttrk_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.nexttrk_btn, cd_nexttrk, s);
	register_focuschg_ev(m->main.nexttrk_btn);
	register_xingchg_ev(m->main.nexttrk_btn);

	register_arm_cb(m->main.previdx_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.previdx_btn, cd_previdx, s);
	register_focuschg_ev(m->main.previdx_btn);
	register_xingchg_ev(m->main.previdx_btn);

	register_arm_cb(m->main.nextidx_btn, cd_tooltip_cancel, s);
	register_activate_cb(m->main.nextidx_btn, cd_nextidx, s);
	register_focuschg_ev(m->main.nextidx_btn);
	register_xingchg_ev(m->main.nextidx_btn);

	register_arm_cb(m->main.rew_btn, cd_tooltip_cancel, s);
	register_arm_cb(m->main.rew_btn, cd_rew, s);
	register_disarm_cb(m->main.rew_btn, cd_rew, s);
	register_activate_cb(m->main.rew_btn, cd_rew, s);
	register_focuschg_ev(m->main.rew_btn);
	register_xingchg_ev(m->main.rew_btn);

	register_arm_cb(m->main.ff_btn, cd_tooltip_cancel, s);
	register_arm_cb(m->main.ff_btn, cd_ff, s);
	register_disarm_cb(m->main.ff_btn, cd_ff, s);
	register_activate_cb(m->main.ff_btn, cd_ff, s);
	register_focuschg_ev(m->main.ff_btn);
	register_xingchg_ev(m->main.ff_btn);

	/* Install WM_DELETE_WINDOW handler */
	add_delw_callback(
		m->toplevel,
		(XtCallbackProc) cd_exit,
		(XtPointer) s
	);
}


/*
 * register_keypad_callbacks
 *	Register all callback routines for widgets in the keypad window
 *
 * Args:
 *	m - Pointer to the main widgets structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
register_keypad_callbacks(widgets_t *m, curstat_t *s)
{
	int	i;

	register_focus_cb(m->keypad.form, cd_shell_focus_chg, m->keypad.form);

	/* Keypad popup callbacks */
	for (i = 0; i < 10; i++)
		register_activate_cb(m->keypad.num_btn[i], cd_keypad_num, i);

	register_activate_cb(m->keypad.clear_btn, cd_keypad_clear, s);
	register_activate_cb(m->keypad.enter_btn, cd_keypad_enter, s);

	register_valchg_cb(m->keypad.warp_scale, cd_warp, s);
	register_drag_cb(m->keypad.warp_scale, cd_warp, s);

	register_activate_cb(m->keypad.cancel_btn, cd_keypad_popdown, s);

	/* Install WM_DELETE_WINDOW handler */
	add_delw_callback(
		XtParent(m->keypad.form),
		(XtCallbackProc) cd_keypad_popdown,
		(XtPointer) s
	);
}


/*
 * register_options_c                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
