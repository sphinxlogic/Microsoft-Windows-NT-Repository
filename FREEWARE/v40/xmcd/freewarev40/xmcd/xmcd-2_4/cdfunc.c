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
static char *_cdfunc_c_ident_ = "@(#)cdfunc.c	6.141 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/patchlevel.h"
#include "common_d/util.h"
#include "xmcd_d/xmcd.h"
#include "xmcd_d/widget.h"
#include "xmcd_d/dbprog.h"
#include "xmcd_d/geom.h"
#include "xmcd_d/hotkey.h"
#include "xmcd_d/help.h"
#include "xmcd_d/cdfunc.h"
#include "cddb_d/cddb.h"
#include "libdi_d/libdi.h"


#define CHGDISC_DELAY	1500			/* Disc chg delay */

/* Confirmation dialog box callback info structure */
typedef struct {
	Widget		widget0;		/* OK button */
	Widget		widget1;		/* Cancel button */
	Widget		widget2;		/* Dialog box */
	String		type;			/* Callback type */
	XtCallbackProc	func;			/* Callback function */
	XtPointer	data;			/* Callback arg */
} cbinfo_t;

/* Structure to save callback function pointer and its args */
typedef struct {
	XtCallbackProc	func;			/* Callback function */
	Widget		w;			/* Widget */
	XtPointer	client_data;		/* Client data */
	XtPointer	call_data;		/* Call data */
} callback_sav_t;


extern widgets_t	widgets;
extern pixmaps_t	pixmaps;
extern appdata_t	app_data;
extern FILE		*errfp;

STATIC char		keystr[3];		/* Keypad number string */
STATIC int		keypad_mode;		/* Keypad mode */
STATIC long		tm_blinkid = -1,	/* Time dpy blink timer ID */
			ab_blinkid = -1,	/* A->B dpy blink timer ID */
			dbmode_blinkid = -1,	/* Dbmode dpy blink timer ID */
			chgdisc_dlyid = -1,	/* Disc chg delay timer ID */
			tooltip1_id = -1,	/* Tooltip popup timer ID */
			tooltip2_id = -1;	/* Tooltip popdown timer ID */
STATIC word32_t		warp_offset = 0;	/* Track warp block offset */
STATIC bool_t		searching,		/* Running REW or FF */
			popup_ok,		/* Are popups allowed? */
			pseudo_warp,		/* Warp slider only flag */
			warp_busy,		/* Warp function active */
			chgdelay,		/* Disc change delay */
			mode_chg,		/* Changing main window mode */
			tooltip_active,		/* Tooltip is active */
			skip_next_tooltip;	/* Skip the next tooltip */
STATIC cddb_client_t	cddb_cldata;		/* Client info for libcddb */
STATIC di_client_t	di_cldata;		/* Client info for libdi */
STATIC callback_sav_t	override_sav;		/* Mode override callback */


/* Forward declaration prototypes */
STATIC void		cd_pause_blink(curstat_t *, bool_t),
			cd_ab_blink(curstat_t *, bool_t),
			cd_dbmode_blink(curstat_t *, bool_t);


/***********************
 *  internal routines  *
 ***********************/


/*
 * disc_etime_norm
 *	Return the elapsed time of the disc in seconds.  This is
 *	used during normal playback.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	The disc elapsed time in seconds.
 */
STATIC sword32_t
disc_etime_norm(curstat_t *s)
{
	sword32_t	secs;

	secs = (s->cur_tot_min * 60 + s->cur_tot_sec) -
	       (MSF_OFFSET / FRAME_PER_SEC);
	return ((secs >= 0) ? secs : 0);
}


/*
 * disc_etime_prog
 *	Return the elapsed time of the disc in seconds.  This is
 *	used during shuffle or program mode.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	The disc elapsed time in seconds.
 */
STATIC sword32_t
disc_etime_prog(curstat_t *s)
{
	sword32_t	i,
			secs = 0;

	/* Find the time of all played tracks */
	for (i = 0; i < ((int) s->prog_cnt) - 1; i++) {
		secs += ((s->trkinfo[s->playorder[i]+1].min * 60 +
			 s->trkinfo[s->playorder[i]+1].sec) -
		         (s->trkinfo[s->playorder[i]].min * 60 +
			 s->trkinfo[s->playorder[i]].sec));
	}

	/* Find the elapsed time of the current track */
	for (i = 0; i < MAXTRACK; i++) {
		if (s->trkinfo[i].trkno == LEAD_OUT_TRACK)
			break;

		if (s->trkinfo[i].trkno == s->cur_trk) {
			secs += (s->cur_trk_min * 60 + s->cur_trk_sec);
			break;
		}
	}

	return ((secs >= 0) ? secs : 0);
}


/*
 * track_rtime
 *	Return the remaining time of the current playing track in seconds.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	The track remaining time in seconds.
 */
STATIC sword32_t
track_rtime(curstat_t *s)
{
	sword32_t	i,
			secs,
			tot_sec,
			cur_sec;

	if ((i = di_curtrk_pos(s)) < 0)
		return 0;

	tot_sec = (s->trkinfo[i+1].min * 60 + s->trkinfo[i+1].sec) -
		  (s->trkinfo[i].min * 60 + s->trkinfo[i].sec);
	cur_sec = s->cur_trk_min * 60 + s->cur_trk_sec;
	secs = tot_sec - cur_sec;

	return ((secs >= 0) ? secs : 0);
}


/*
 * disc_rtime_norm
 *	Return the remaining time of the disc in seconds.  This is
 *	used during normal playback.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	The disc remaining time in seconds.
 */
STATIC sword32_t
disc_rtime_norm(curstat_t *s)
{
	sword32_t	secs;

	secs = (s->tot_min * 60 + s->tot_sec) -
		(s->cur_tot_min * 60 + s->cur_tot_sec);

	return ((secs >= 0) ? secs : 0);
}


/*
 * disc_rtime_prog
 *	Return the remaining time of the disc in seconds.  This is
 *	used during shuffle or program mode.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	The disc remaining time in seconds.
 */
STATIC sword32_t
disc_rtime_prog(curstat_t *s)
{
	sword32_t	i,
			secs = 0;

	/* Find the time of all unplayed tracks */
	for (i = s->prog_cnt; i < (int) s->prog_tot; i++) {
		secs += ((s->trkinfo[s->playorder[i]+1].min * 60 +
			 s->trkinfo[s->playorder[i]+1].sec) -
		         (s->trkinfo[s->playorder[i]].min * 60 +
			 s->trkinfo[s->playorder[i]].sec));
	}

	/* Find the remaining time of the current track */
	for (i = 0; i < MAXTRACK; i++) {
		if (s->trkinfo[i].trkno == LEAD_OUT_TRACK)
			break;

		if (s->trkinfo[i].trkno == s->cur_trk) {
			secs += ((s->trkinfo[i+1].min * 60 +
				  s->trkinfo[i+1].sec) -
				 (s->cur_tot_min * 60 + s->cur_tot_sec));

			break;
		}
	}

	return ((secs >= 0) ? secs : 0);
}


/*
 * dpy_time_blink
 *	Make the time indicator region of the main window blink.
 *	This is used when the disc is paused.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
STATIC void
dpy_time_blink(curstat_t *s)
{
	static bool_t	bstate = TRUE;

	if (bstate) {
		tm_blinkid = cd_timeout(
			app_data.blinkoff_interval,
			dpy_time_blink,
			(byte_t *) s
		);
		dpy_time(s, TRUE);
	}
	else {
		tm_blinkid = cd_timeout(
			app_data.blinkon_interval,
			dpy_time_blink,
			(byte_t *) s
		);
		dpy_time(s, FALSE);
	}
	bstate = !bstate;
}


/*
 * dpy_ab_blink
 *	Make the a->b indicator of the main window blink.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
STATIC void
dpy_ab_blink(curstat_t *s)
{
	static bool_t	bstate = TRUE;

	if (bstate) {
		ab_blinkid = cd_timeout(
			app_data.blinkoff_interval,
			dpy_ab_blink,
			(byte_t *) s
		);
		dpy_playmode(s, TRUE);
	}
	else {
		ab_blinkid = cd_timeout(
			app_data.blinkon_interval,
			dpy_ab_blink,
			(byte_t *) s
		);
		dpy_playmode(s, FALSE);
	}
	bstate = !bstate;
}


/*
 * dpy_dbmode_blink
 *	Make the dbmode indicator of the main window blink.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
STATIC void
dpy_dbmode_blink(curstat_t *s)
{
	static bool_t	bstate = TRUE;

	if (bstate) {
		dbmode_blinkid = cd_timeout(
			app_data.blinkoff_interval,
			dpy_dbmode_blink,
			(byte_t *) s
		);
		dpy_dbmode(s, TRUE);
	}
	else {
		dbmode_blinkid = cd_timeout(
			app_data.blinkon_interval,
			dpy_dbmode_blink,
			(byte_t *) s
		);
		dpy_dbmode(s, FALSE);
	}
	bstate = !bstate;
}


/*
 * dpy_keypad_ind
 *	Update the digital indicator on the keypad window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dpy_keypad_ind(curstat_t *s)
{
	char		str[24],
			trk[16],
			time[16];
	byte_t		min,
			sec,
			frame;
	XmString	xs;
	static char	prevstr[24];

	if (!XtIsManaged(widgets.keypad.form))
		return;

	switch (keypad_mode) {
	case KPMODE_DISC:
		if (s->mode == MOD_BUSY)
			(void) strcpy(str, "disc    -");
		else if (keystr[0] == '\0')
			(void) sprintf(str, "disc  %3d", s->cur_disc);
		else
			(void) sprintf(str, "( disc  %3d )", atoi(keystr));
		break;

	case KPMODE_TRACK:
		if (keystr[0] == '\0') {
			if (warp_busy) {
				util_blktomsf(
					warp_offset,
					&min,
					&sec,
					&frame,
					0
				);

				(void) sprintf(trk, "%02u", s->cur_trk);

				(void) sprintf(time, "+%02u:%02u", min, sec);
			}
			else if (di_curtrk_pos(s) < 0) {
				(void) strcpy(trk, "--");
				(void) strcpy(time, " --:--");
			}
			else if (curtrk_type(s) == TYP_DATA) {
				(void) sprintf(trk, "%02u", s->cur_trk);
				(void) strcpy(time, " --:--");
			}
			else {
				util_blktomsf(
					s->cur_trk_addr,
					&min,
					&sec,
					&frame,
					0
				);
				(void) sprintf(trk, "%02u", s->cur_trk);
				(void) sprintf(time, "%c%02u:%02u",
				       (s->cur_idx == 0) ? '-' : '+', min, sec);
			}

			if (warp_busy)
				(void) sprintf(str, "( %s  %s )", trk, time);
			else
				(void) sprintf(str, "%s  %s", trk, time);
		}
		else {
			util_blktomsf(warp_offset, &min, &sec, &frame, 0);
			(void) sprintf(trk, "%02u", atoi(keystr));
			(void) sprintf(time, "+%02u:%02u", min, sec);

			(void) sprintf(str, "( %s  %s )", trk, time);
		}
		break;

	default:
		return;
	}

	if (strcmp(str, prevstr) == 0) {
		/* No change */
		return;
	}

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.keypad.keypad_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);
	(void) strcpy(prevstr, str);
}


/*
 * dpy_warp
 *	Update the warp slider position.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dpy_warp(curstat_t *s)
{
	int	i;

	if (!XtIsManaged(widgets.keypad.form) || warp_busy)
		return;

	if ((i = di_curtrk_pos(s)) < 0 || s->cur_idx == 0 ||
	    curtrk_type(s) == TYP_DATA)
		set_warp_slider(0, TRUE);
	else
		set_warp_slider(unscale_warp(s, i, s->cur_trk_addr), TRUE);
}


/*
 * set_btn_color
 *	Set the label color of a pushbutton widget
 *
 * Args:
 *	w - The pushbutton widget.
 *	px - The label pixmap, if applicable.
 *	color - The pixel value of the desired color.
 *
 * Return:
 *	Nothing.
 */
STATIC void
set_btn_color(Widget w, Pixmap px, Pixel color)
{
	unsigned char	labtype;

	XtVaGetValues(w, XmNlabelType, &labtype, NULL);

	if (labtype == XmPIXMAP)
		XtVaSetValues(w, XmNlabelPixmap, px, NULL);
	else
		XtVaSetValues(w, XmNforeground, color, NULL);
}


/*
 * set_scale_color
 *	Set the indicator color of a scale widget
 *
 * Args:
 *	w - The scale widget.
 *	pixmap - not used.
 *	color - The pixel value of the desired color.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
set_scale_color(Widget w, Pixmap px, Pixel color)
{
	XtVaSetValues(w, XmNforeground, color, NULL);
}


/*
 * cd_pause_blink
 *	Disable or enable the time indicator blinking.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	enable - TRUE: start blink, FALSE: stop blink
 *
 * Return:
 *	Nothing.
 */
STATIC void
cd_pause_blink(curstat_t *s, bool_t enable)
{
	static bool_t	blinking = FALSE;

	if (enable) {
		if (!blinking) {
			/* Start time display blink */
			blinking = TRUE;
			dpy_time_blink(s);
		}
	}
	else if (blinking) {
		/* Stop time display blink */
		cd_untimeout(tm_blinkid);

		tm_blinkid = -1;
		blinking = FALSE;
	}
}


/*
 * cd_ab_blink
 *	Disable or enable the a->b indicator blinking.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	enable - TRUE: start blink, FALSE: stop blink
 *
 * Return:
 *	Nothing.
 */
STATIC void
cd_ab_blink(curstat_t *s, bool_t enable)
{
	static bool_t	blinking = FALSE;

	if (enable) {
		if (!blinking) {
			/* Start A->B display blink */
			blinking = TRUE;
			dpy_ab_blink(s);
		}
	}
	else if (blinking) {
		/* Stop A->B display blink */
		cd_untimeout(ab_blinkid);

		ab_blinkid = -1;
		blinking = FALSE;
	}
}


/*
 * cd_dbmode_blink
 *	Disable or enable the dbmode indicator blinking.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	enable - TRUE: start blink, FALSE: stop blink
 *
 * Return:
 *	Nothing.
 */
STATIC void
cd_dbmode_blink(curstat_t *s, bool_t enable)
{
	static bool_t	blinking = FALSE;

	if (enable) {
		if (!blinking) {
			/* Start dbmode display blink */
			blinking = TRUE;
			dpy_dbmode_blink(s);
		}
	}
	else if (blinking) {
		/* Stop A->B display blink */
		cd_untimeout(dbmode_blinkid);

		dbmode_blinkid = -1;
		blinking = FALSE;
	}
}


/*
 * do_chgdisc
 *	Timer function to change discs on a multi-CD changer.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
do_chgdisc(curstat_t *s)
{
	/* Reset timer ID */
	chgdisc_dlyid = -1;
	chgdelay = FALSE;

	/* Change to watch cursor */
	cd_busycurs(TRUE, CURS_ALL);

	/* Do the disc change */
	di_chgdisc(s);

	/* Change to normal cursor */
	cd_busycurs(FALSE, CURS_ALL);
}


/*
 * cd_tooltip_popdown
 *	Pop-down the tool-tip.
 *
 * Args:
 *	w - The tooltip shell.
 *
 * Return:
 *	Nothing.
 */
STATIC void
cd_tooltip_popdown(Widget w)
{
	/* Cancel pending timers */
	if (tooltip1_id >= 0) {
		cd_untimeout(tooltip1_id);
		tooltip1_id = -1;
	}
	if (tooltip2_id >= 0) {
		cd_untimeout(tooltip2_id);
		tooltip2_id = -1;
	}

	/* Pop down the tooltip */
	if (tooltip_active) {
		tooltip_active = FALSE;
		XtPopdown(w);
	}
}


/*
 * cd_tooltip_sphandler
 *	Special widget-specific handler for tool-tips
 *
 * Args:
 *	w - The associated control widget
 *	ret_tlbl - the return XmString label that will be displayed on the
 *		   tool-tip.  The caller should XmStringFree() this when
 *		   done.
 *
 * Return:
 *	0: Don't pop-up tool-tip
 *	1: Pop-up tooltip with the returned ret_tlbl
 *	2: This is not a special widget: use the default handler.
 */
STATIC int
cd_tooltip_sphandler(Widget w, XmString *ret_tlbl)
{
	char		*ttitle,
			*dtitle;
	XmString	xs1,
			xs2,
			xs3;
	curstat_t	*s = curstat_addr();

	if (w == widgets.main.disc_ind) {
		/* Disc display */
		xs1 = XmStringCreateSimple("Disc ");
		XtVaGetValues(w, XmNlabelString, &xs2, NULL);
		*ret_tlbl = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);
	}
	else if (w == widgets.main.track_ind) {
		/* Track display */
		xs1 = XmStringCreateSimple("Track ");
		XtVaGetValues(w, XmNlabelString, &xs2, NULL);
		*ret_tlbl = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);
	}
	else if (w == widgets.main.index_ind) {
		/* Index display */
		xs1 = XmStringCreateSimple("Index ");
		XtVaGetValues(w, XmNlabelString, &xs2, NULL);
		*ret_tlbl = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);
	}
	else if (w == widgets.main.time_ind) {
		/* Time display */
		xs1 = XmStringCreateSimple("Time ");
		XtVaGetValues(widgets.main.timemode_ind,
			XmNlabelString, &xs2,
			NULL
		);
		xs3 = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);

		switch (geom_main_getmode()) {
		case MAIN_NORMAL:
			/* Normal mode */
			*ret_tlbl = xs3;
			break;

		case MAIN_BASIC:
			/* Basic mode */

			/* Add disc title */
			dtitle = dbprog_curdtitle(s);
			if (dtitle[0] == '\0') {
				/* No disc title */
				*ret_tlbl = xs3;
				break;
			}

			xs1 = XmStringSeparatorCreate();
			xs2 = XmStringConcat(xs3, xs1);
			XmStringFree(xs1);
			XmStringFree(xs3);

			xs1 = XmStringCreateLtoR(
				dtitle,
				XmSTRING_DEFAULT_CHARSET
			);
			xs3 = XmStringConcat(xs2, xs1);
			XmStringFree(xs1);
			XmStringFree(xs2);

			/* Add track title */
			ttitle = dbprog_curttitle(s);
			if (ttitle[0] == '\0') {
				/* No track title */
				*ret_tlbl = xs3;
			}
			else {
				xs1 = XmStringSeparatorCreate();
				xs2 = XmStringConcat(xs3, xs1);
				XmStringFree(xs1);
				XmStringFree(xs3);

				xs1 = XmStringCreateLtoR(
					ttitle,
					XmSTRING_DEFAULT_CHARSET
				);
				xs3 = XmStringConcat(xs2, xs1);
				XmStringFree(xs1);
				XmStringFree(xs2);

				*ret_tlbl = xs3;
			}
			break;
		}
	}
	else if (w == widgets.main.rptcnt_ind) {
		xs1 = XmStringCreateSimple("Repeat count: ");
		XtVaGetValues(w, XmNlabelString, &xs2, NULL);
		*ret_tlbl = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);
	}
	else if (w == widgets.main.dbmode_ind) {
		xs1 = XmStringCreateSimple("Database mode: ");
		XtVaGetValues(w, XmNlabelString, &xs2, NULL);
		*ret_tlbl = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);
	}
	else if (w == widgets.main.progmode_ind) {
		*ret_tlbl = XmStringCreateSimple(
			(s->program && !s->onetrk_prog && !s->shuffle) ?
			"Program mode: on" : "Program mode: off"
		);
	}
	else if (w == widgets.main.timemode_ind) {
		xs1 = XmStringCreateSimple("Time display mode: ");
		XtVaGetValues(w, XmNlabelString, &xs2, NULL);
		*ret_tlbl = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);
	}
	else if (w == widgets.main.playmode_ind) {
		xs1 = XmStringCreateSimple("Playback mode: ");
		XtVaGetValues(w, XmNlabelString, &xs2, NULL);
		*ret_tlbl = XmStringConcat(xs1, xs2);
		XmStringFree(xs1);
		XmStringFree(xs2);
	}
	else if (w == widgets.main.dtitle_ind) {
		dtitle = dbprog_curdtitle(s);
		if (dtitle[0] == '\0') {
			/* No disc title: don't popup tooltip */
			return 0;
		}
		/* Use label string */
		XtVaGetValues(w, XmNlabelString, ret_tlbl, NULL);
	}
	else if (w == widgets.main.ttitle_ind) {
		ttitle = dbprog_curttitle(s);
		if (ttitle[0] == '\0') {
			/* No track title: don't popup tooltip */
			return 0;
		}
		/* Use label string */
		XtVaGetValues(w, XmNlabelString, ret_tlbl, NULL);
	}
	else
		return 2;

	return 1;
}


/*
 * cd_tooltip_popup
 *	Timer function to pop-up the tool-tip.
 *
 * Args:
 *	w - The associated control widget
 *
 * Return:
 *	Nothing.
 */
STATIC void
cd_tooltip_popup(Widget w)
{
	Display			*display = XtDisplay(widgets.toplevel);
	int			screen = DefaultScreen(display);
	Position		end_x,
				end_y,
				abs_x,
				abs_y,
				x,
				y;
	Dimension		width,
				height,
				off_x,
				off_y,
				swidth,
				sheight;
	WidgetClass		wc;
	XtWidgetGeometry	geom;
	XmString		tlbl;

	tooltip1_id = -1;
	tooltip_active = TRUE;
	swidth = (Dimension) XDisplayWidth(display, screen);
	sheight = (Dimension) XDisplayHeight(display, screen);

	XtVaGetValues(w,
		XmNwidth, &width,
		XmNheight, &height,
		NULL
	);

	/* Perform widget-specific handling */
	switch (cd_tooltip_sphandler(w, &tlbl)) {
	case 0:
		/* tooltip popup refused by cd_tooltip_sphandler */
		tooltip_active = FALSE;
		return;
	case 1:
		/* cd_tooltip_sphandler handled it */
		break;
	default:
		wc = XtClass(w);

		if (wc == xmPushButtonWidgetClass ||
			 wc == xmToggleButtonWidgetClass) {
			/* Use label string */
			XtVaGetValues(w, XmNlabelString, &tlbl, NULL);
		}
		else if (wc == xmScaleWidgetClass) {
			/* Use title string */
			XtVaGetValues(w, XmNtitleString, &tlbl, NULL);
		}
		else {
			/* Use widget name */
			tlbl = XmStringCreateSimple(XtName(w));
		}
		break;
	}

	/* Set the tooltip label string and hotkey mnemonic, if any. */
	XtVaSetValues(widgets.tooltip.tooltip_lbl, XmNlabelString, tlbl, NULL);
	hotkey_tooltip_mnemonic(w);

	/* Translate to screen absolute coordinates, and add desired offsets */
	XtTranslateCoords(w, 0, 0, &abs_x, &abs_y);

	/* Make sure that the tooltip window doesn't go beyond
	 * screen boundaries.
	 */
	(void) XtQueryGeometry(widgets.tooltip.tooltip_lbl, NULL, &geom);

	off_x = width / 2;
	end_x = abs_x + geom.width + off_x + 2;
	if (end_x > (Position) swidth)
		off_x = -(end_x - swidth - off_x);
	x = abs_x + off_x;
	if (x < 0)
		x = 2;

	off_y = height + 5;
	end_y = abs_y + geom.height + off_y + 2;
	if (end_y > (Position) sheight)
		off_y = -(geom.height + 5);
	y = abs_y + off_y;

	/* Move tooltip widget to desired location and pop it up */
	XtVaSetValues(widgets.tooltip.shell, XmNx, x, XmNy, y, NULL);
	XtPopup(widgets.tooltip.shell, XtGrabNone);

	/* Set timer for auto-popdown */
	if (app_data.tooltip_time > 0) {
		int	n;

		/* If the tooltip label is longer than 40 characters,
		 * add 50mS to the tooltip time for each additional
		 * character.
		 */
		n = XmStringLength(tlbl);
		if (n <= 40)
			n = 0;
		else
			n = ((n - 40) + 1) * 50;

		tooltip2_id = cd_timeout(
			app_data.tooltip_time + n,
			cd_tooltip_popdown,
			(byte_t *) widgets.tooltip.shell
		);
	}

	XmStringFree(tlbl);
}


/*
 * cd_keypad_ask_dsbl
 *	Prompt the user whether to disable the shuffle or program modes
 *	if the user tries to use the keypad to change the track/disc.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	func - The callback function to call if the user answers yes, and
 *             after the shuffle or program mode is disabled.
 *	w - The widget that normally activates the specified callback function
 *	call_data - The callback structure pointer
 *	call_data_len - The callback structure size
 *
 * Return:
 *	Nothing.
 */
STATIC void
cd_keypad_ask_dsbl(
	curstat_t	*s,
	XtCallbackProc	func,
	Widget		w,
	XtPointer	call_data,
	int		call_data_len
)
{
	char	*str;

	override_sav.func = func;
	override_sav.w = w;
	override_sav.client_data = (XtPointer) s;
	override_sav.call_data = (XtPointer) MEM_ALLOC(
		"override_sav.call_data",
		call_data_len
	);
	if (override_sav.call_data == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}
	memcpy(override_sav.call_data, call_data, call_data_len);

	str = (char *) MEM_ALLOC(
		"ask_dsbl_str",
		strlen(app_data.str_kpmodedsbl) + 20
	);
	if (str == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}
	(void) sprintf(str, app_data.str_kpmodedsbl,
		       s->shuffle ? "shuffle" : "program");

	cd_confirm_popup(
		app_data.str_confirm,
		str,
		(XtCallbackProc) cd_keypad_dsbl_modes_yes,
		(XtPointer) s,
		(XtCallbackProc) cd_keypad_dsbl_modes_no,
		(XtPointer) s
	);

	MEM_FREE(str);
}


/***********************
 *   public routines   *
 ***********************/


/*
 * curtrk_type
 *	Return the track type of the currently playing track.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	TYP_AUDIO or TYP_DATA.
 */
byte_t
curtrk_type(curstat_t *s)
{
	sword32_t	i;

	if ((i = di_curtrk_pos(s)) >= 0)
		return (s->trkinfo[i].type);

	return TYP_AUDIO;
}


/*
 * dpy_disc
 *	Update the disc number display region of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_disc(curstat_t *s)
{
	XmString	xs;
	char		str[8];
	static char	prev[8] = { '\0' };

	(void) sprintf(str, "%u", s->cur_disc);

	if (strcmp(str, prev) == 0)
		/* No change, just return */
		return;

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.disc_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	/* Update the keypad indicator */
	dpy_keypad_ind(s);

	(void) strcpy(prev, str);
}


/*
 * dpy_track
 *	Update the track number display region of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_track(curstat_t *s)
{
	XmString	xs;
	char		str[4];
	static char	prev[4] = { '\0' };
	static int	sav_trk = -1;


	if (s->cur_trk != sav_trk) {
		/* Update database/program window current track display */
		dbprog_curtrkupd(s);
		/* Update main window track title display */
		dpy_ttitle(s);
		/* Update the keypad indicator */
		dpy_keypad_ind(s);
		/* Update warp slider */
		dpy_warp(s);
	}

	sav_trk = s->cur_trk;

	if (s->cur_trk < 0 || s->mode == MOD_BUSY ||
		 s->mode == MOD_NODISC)
		(void) strcpy(str, "--");
	else if (s->time_dpy == T_REMAIN_DISC) {
		if (s->shuffle || s->program) {
			if (s->prog_tot >= s->prog_cnt)
				(void) sprintf(str, "-%u",
					       s->prog_tot - s->prog_cnt);
			else
				(void) strcpy(str, "-0");
		}
		else
			(void) sprintf(str, "-%u",
				       s->tot_trks - di_curtrk_pos(s) - 1);
	}
	else
		(void) sprintf(str, "%02u", s->cur_trk);

	if (strcmp(str, prev) == 0 && !mode_chg)
		/* No change, just return */
		return;

	xs = XmStringCreateLtoR(
		str,
		(geom_main_getmode() == MAIN_NORMAL) ? CHSET1 : CHSET2
	);

	XtVaSetValues(
		widgets.main.track_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strcpy(prev, str);
}


/*
 * dpy_index
 *	Update the index number display region of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_index(curstat_t *s)
{
	XmString	xs;
	char		str[4];
	static char	prev[4] = { '\0' };

	if (s->cur_idx <= 0 || s->mode == MOD_BUSY ||
	    s->mode == MOD_NODISC || s->mode == MOD_STOP)
		(void) strcpy(str, "--");
	else
		(void) sprintf(str, "%02u", s->cur_idx);

	if (strcmp(str, prev) == 0)
		/* No change, just return */
		return;

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.index_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strcpy(prev, str);
}


/*
 * dpy_time
 *	Update the time display region of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	blank - Whether the display region should be blanked.
 *
 * Return:
 *	Nothing
 */
void
dpy_time(curstat_t *s, bool_t blank)
{
	sword32_t	time_sec;
	XmString	xs;
	bool_t		playing;
	char		str[12];
	static char	prev[12] = { 'j', 'u', 'n', 'k', '\0' };

	playing = FALSE;

	if (blank)
		str[0] = '\0';
	else if (s->mode == MOD_BUSY)
		(void) strcpy(str, app_data.str_busy);
	else if (s->mode == MOD_NODISC)
		(void) strcpy(str, app_data.str_nodisc);
	else if (curtrk_type(s) == TYP_DATA)
		(void) strcpy(str, app_data.str_data);
	else if (s->mode == MOD_STOP)
		(void) strcpy(str, " --:--");
	else {
		playing = TRUE;

		switch (s->time_dpy) {
		case T_ELAPSED:
			(void) sprintf(str, "%s%02u:%02u",
				       (s->cur_idx == 0) ? "-" : "+",
				       s->cur_trk_min,
				       s->cur_trk_sec);
			break;

		case T_ELAPSED_DISC:
			if (s->shuffle || s->program) {
				if (s->cur_idx == 0) {
					(void) strcpy(str, "   :  ");
					break;
				}
				else
					time_sec = disc_etime_prog(s);
			}
			else
				time_sec = disc_etime_norm(s);

			(void) sprintf(str, "+%02u:%02u",
				       time_sec / 60, time_sec % 60);
			break;

		case T_REMAIN_TRACK:
			if (s->cur_idx == 0)
				(void) strcpy(str, "   :  ");
			else {
				time_sec = track_rtime(s);
				(void) sprintf(str, "-%02u:%02u",
					       time_sec / 60, time_sec % 60);
			}
			break;

		case T_REMAIN_DISC:
			if (s->shuffle || s->program) {
				if (s->cur_idx == 0) {
					(void) strcpy(str, "   :  ");
					break;
				}
				else
					time_sec = disc_rtime_prog(s);
			}
			else
				time_sec = disc_rtime_norm(s);

			(void) sprintf(str, "-%02u:%02u",
				       time_sec / 60, time_sec % 60);
			break;

		default:
			(void) strcpy(str, "??:??");
			break;
		}
	}

	if (s->mode == MOD_PAUSE)
		cd_pause_blink(s, TRUE);
	else
		cd_pause_blink(s, FALSE);

	if (strcmp(str, prev) == 0 && !mode_chg) {
		if (s->cur_idx == 0 && playing)
			/* Update the keypad indicator */
			dpy_keypad_ind(s);

		/* No change: just return */
		return;
	}

	xs = XmStringCreateLtoR(
		str,
		(geom_main_getmode() == MAIN_NORMAL) ? CHSET1 : CHSET2
	);

	XtVaSetValues(
		widgets.main.time_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);
	(void) strcpy(prev, str);

	/* Update the keypad indicator */
	dpy_keypad_ind(s);

	/* Update warp slider */
	dpy_warp(s);
}


/*
 * dpy_dtitle
 *	Update the disc title display region of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_dtitle(curstat_t *s)
{
	XmString	xs;
	char		str[TITLEIND_LEN];
	static char	prev[TITLEIND_LEN];

	if (chgdelay)
		str[0] = '\0';
	else {
		(void) strncpy(str, dbprog_curdtitle(s), TITLEIND_LEN);
		str[TITLEIND_LEN - 1] = '\0';
	}

	if (strcmp(str, prev) == 0)
		/* No change: just return */
		return;

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.dtitle_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strcpy(prev, str);
}


/*
 * dpy_ttitle
 *	Update the track title display region of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_ttitle(curstat_t *s)
{
	XmString	xs;
	char		str[TITLEIND_LEN];
	static char	prev[TITLEIND_LEN];

	if (chgdelay)
		str[0] = '\0';
	else {
		(void) strncpy(str, dbprog_curttitle(s), TITLEIND_LEN);
		str[TITLEIND_LEN - 1] = '\0';
	}

	if (strcmp(str, prev) == 0)
		/* No change: just return */
		return;

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.ttitle_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strcpy(prev, str);
}


/*
 * dpy_rptcnt
 *	Update the repeat count indicator of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_rptcnt(curstat_t *s)
{
	XmString		xs;
	char			str[12];
	static char		prevstr[12];

	if (s->repeat && (s->mode == MOD_PLAY || s->mode == MOD_PAUSE))
		(void) sprintf(str, "%u", s->rptcnt);
	else
		(void) strcpy(str, "-");

	if (strcmp(str, prevstr) == 0)
		/* No change */
		return;

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.rptcnt_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strcpy(prevstr, str);
}


/*
 * dpy_dbmode
 *	Update the cddb indicator of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	blank - Whether the indicator should be blanked
 *
 * Return:
 *	Nothing
 */
void
dpy_dbmode(curstat_t *s, bool_t blank)
{
	char		*str;
	XmString	xs;
	static char	prev[12] = { 'j', 'u', 'n', 'k', '\0' };

	if (blank)
		str = "";
	else {
		switch (s->cddb) {
		case CDDB_LOCAL:
			str = app_data.str_cddblocal;
			break;
		case CDDB_REMOTE_CDDBP:
		case CDDB_REMOTE_HTTP:
			str = app_data.str_cddbremote;
			break;
		case CDDB_WAIT:
			str = app_data.str_cddbquery;
			break;
		case CDDB_INVALID:
		default:
			str = "";
			break;
		}
	}

	if (strcmp(prev, str) == 0)
		/* No change: just return */
		return;

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.dbmode_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strncpy(prev, str, 12 - 1);
	prev[12 - 1] = '\0';

	if (s->cddb == CDDB_WAIT)
		cd_dbmode_blink(s, TRUE);
	else
		cd_dbmode_blink(s, FALSE);
}


/*
 * dpy_progmode
 *	Update the prog indicator of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_progmode(curstat_t *s)
{
	XmString	xs;
	bool_t		state;
	static bool_t	first = TRUE,
			prev = FALSE;

	state = (bool_t) (s->program && !s->onetrk_prog && !s->shuffle);

	if (!first && state == prev)
		/* No change: just return */
		return;

	first = FALSE;

	xs = XmStringCreateSimple(state ? app_data.str_progmode : "");

	XtVaSetValues(
		widgets.main.progmode_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	prev = state;
}


/*
 * dpy_timemode
 *	Update the time mode indicator of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_timemode(curstat_t *s)
{
	String		str;
	XmString	xs;
	static byte_t	prev = 0xff;

	if (prev == s->time_dpy)
		/* No change: just return */
		return;

	switch (s->time_dpy) {
	case T_ELAPSED:
		str = app_data.str_elapse;
		break;

	case T_ELAPSED_DISC:
		str = app_data.str_elapsedisc;
		break;

	case T_REMAIN_TRACK:
		str = app_data.str_remaintrk;
		break;

	case T_REMAIN_DISC:
		str = app_data.str_remaindisc;
		break;
	default:
		/* Invalid mode */
		return;
	}

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.timemode_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	prev = s->time_dpy;
}


/*
 * dpy_playmode
 *	Update the play mode indicator of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	blank - Whether the indicator should be blanked
 *
 * Return:
 *	Nothing
 */
void
dpy_playmode(curstat_t *s, bool_t blank)
{
	char		*str;
	XmString	xs;
	static char	prev[12] = { 'j', 'u', 'n', 'k', '\0' };

	if (blank)
		str = "";
	else {
		switch (s->mode) {
		case MOD_PLAY:
			str = app_data.str_play;
			break;
		case MOD_PAUSE:
			str = app_data.str_pause;
			break;
		case MOD_STOP:
			str = app_data.str_ready;
			break;
		case MOD_A:
			str = "a->?";
			break;
		case MOD_AB:
			str = "a->b";
			break;
		case MOD_SAMPLE:
			str = app_data.str_sample;
			break;
		default:
			str = "";
			break;
		}
	}

	if (strcmp(prev, str) == 0)
		/* No change: just return */
		return;

	xs = XmStringCreateSimple(str);

	XtVaSetValues(
		widgets.main.playmode_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strncpy(prev, str, 12 - 1);
	prev[12 - 1] = '\0';

	if (s->mode == MOD_A)
		cd_ab_blink(s, TRUE);
	else
		cd_ab_blink(s, FALSE);
}


/*
 * dpy_all
 *	Update all indicator of the main window.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing
 */
void
dpy_all(curstat_t *s)
{
	dpy_disc(s);
	dpy_track(s);
	dpy_index(s);
	dpy_time(s, FALSE);
	dpy_dtitle(s);
	dpy_rptcnt(s);
	dpy_dbmode(s, FALSE);
	dpy_progmode(s);
	dpy_timemode(s);
	dpy_playmode(s, FALSE);
}


/*
 * set_lock_btn
 *	Set the lock button state
 *
 * Args:
 *	state - TRUE=in, FALSE=out
 *
 * Return:
 *	Nothing.
 */
void
set_lock_btn(bool_t state)
{
	XmToggleButtonSetState(
		widgets.main.lock_btn, (Boolean) state, False
	);
}


/*
 * set_repeat_btn
 *	Set the repeat button state
 *
 * Args:
 *	state - TRUE=in, FALSE=out
 *
 * Return:
 *	Nothing.
 */
void
set_repeat_btn(bool_t state)
{
	XmToggleButtonSetState(
		widgets.main.repeat_btn, (Boolean) state, False
	);
}


/*
 * set_shuffle_btn
 *	Set the shuffle button state
 *
 * Args:
 *	state - TRUE=in, FALSE=out
 *
 * Return:
 *	Nothing.
 */
void
set_shuffle_btn(bool_t state)
{
	XmToggleButtonSetState(
		widgets.main.shuffle_btn, (Boolean) state, False
	);
}


/*
 * set_vol_slider
 *	Set the volume control slider position
 *
 * Args:
 *	val - The value setting.
 *
 * Return:
 *	Nothing.
 */
void
set_vol_slider(int val)
{
	/* Check bounds */
	if (val > 100)
		val = 100;
	if (val < 0)
		val = 0;

	XmScaleSetValue(widgets.main.level_scale, val);
}


/*
 * set_warp_slider
 *	Set the track warp slider position
 *
 * Args:
 *	val - The value setting.
 *	autoupd - This is an auto-update.
 *
 * Return:
 *	Nothing.
 */
void
set_warp_slider(int val, bool_t autoupd)
{
	if (autoupd && (keypad_mode != KPMODE_TRACK || keystr[0] != '\0')) {
		/* User using keypad: no updates */
		return;
	}

	/* Check bounds */
	if (val > 255)
		val = 255;
	if (val < 0)
		val = 0;

	pseudo_warp = TRUE;
	XmScaleSetValue(widgets.keypad.warp_scale, val);
	pseudo_warp = FALSE;
}


/*
 * set_bal_slider
 *	Set the balance control slider position
 *
 * Args:
 *	val - The value setting.
 *
 * Return:
 *	Nothing.
 */
void
set_bal_slider(int val)
{
	/* Check bounds */
	if (val > 50)
		val = 50;
	if (val < -50)
		val = -50;

	XmScaleSetValue(widgets.options.bal_scale, val);
}


/*
 * scale_warp
 *	Scale track warp value (0-255) to the number of CD logical audio
 *	blocks.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	pos - Track position.
 *	val - Warp value.
 *
 * Return:
 *	The number of CD logical audio blocks
 */
int
scale_warp(curstat_t *s, int pos, int val)
{
	int	n;

	n = val * (s->trkinfo[pos+1].addr - s->trkinfo[pos].addr) / 0xff;
	return ((n > 0) ? (n - 1) : n);
}


/*
 * unscale_warp
 *	Scale the number of CD logical audio blocks to track warp
 *	value (0-255).
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	pos - Track position.
 *	val - Number of logical audio blocks.
 *
 * Return:
 *	The warp value
 */
int
unscale_warp(curstat_t *s, int pos, int val)
{
	return (val * 0xff / (s->trkinfo[pos+1].addr - s->trkinfo[pos].addr));
}


/*
 * cd_timeout
 *	Alarm clock callback facility
 *
 * Args:
 *	msec - When msec milliseconds has elapsed, the callback
 *		occurs.
 *	handler - Pointer to the callback function.
 *	arg - An argument passed to the callback function.
 *
 * Return:
 *	Timeout ID.
 */
long
cd_timeout(word32_t msec, void (*handler)(), byte_t *arg)
{
	/* Note: This code assumes that sizeof(XtIntervalId) <= sizeof(long)
	 * If this is not true then cd_timeout/cd_untimeout will not work
	 * correctly.
	 */
	return ((long)
		XtAppAddTimeOut(
			XtWidgetToApplicationContext(widgets.toplevel),
			(unsigned long) msec,
			(XtTimerCallbackProc) handler,
			(XtPointer) arg
		)
	);
}


/*
 * cd_untimeout
 *	Cancel a pending alarm configured with cd_timeout.
 *
 * Args:
 *	id - The timeout ID
 *
 * Return:
 *	Nothing.
 */
void
cd_untimeout(long id)
{
	/* Note: This code assumes that sizeof(XtIntervalId) <= sizeof(long)
	 * If this is not true then cd_timeout/cd_untimeout will not work
	 * correctly.
	 */
	XtRemoveTimeOut((XtIntervalId) id);
}


/*
 * cd_beep
 *	Beep the workstation speaker.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
cd_beep(void)
{
	XBell(XtDisplay(widgets.toplevel), 50);
}


/*
 * cd_info_popup
 *	Pop up the information message dialog box.
 *
 * Args:
 *	title - The title bar text string.
 *	msg - The information message text string.
 *
 * Return:
 *	Nothing.
 */
void
cd_info_popup(char *title, char *msg)
{
	XmString	xs;

	if (!popup_ok) {
		(void) fprintf(errfp, "%s %s:\n%s\n", PROGNAME, title, msg);
		return;
	}

	/* Set the dialog box title */
	xs = XmStringCreateSimple(title);
	XtVaSetValues(widgets.dialog.info, XmNdialogTitle, xs, NULL);
	XmStringFree(xs);

	/* Set the dialog box message */
	xs = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues(widgets.dialog.info, XmNmessageString, xs, NULL);
	XmStringFree(xs);

	/* Pop up the info dialog */
	if (!XtIsManaged(widgets.dialog.info))
		XtManageChild(widgets.dialog.info);
}


/*
 * cd_info_popup_auto
 *	Pop up the information message dialog box, which will auto-popdown
 *	in 5 seconds.
 *
 * Args:
 *	title - The title bar text string.
 *	msg - The information message text string.
 *
 * Return:
 *	Nothing.
 */
void
cd_info_popup_auto(char *title, char *msg)
{
	cd_info_popup(title, msg);

	(void) cd_timeout(
		5000,	/* popup interval */
		XtUnmanageChild,
		(byte_t *) widgets.dialog.info
	);
}


/*
 * cd_warning_popup
 *	Pop up the warning message dialog box.
 *
 * Args:
 *	title - The title bar text string.
 *	msg - The warning message text string.
 *
 * Return:
 *	Nothing.
 */
void
cd_warning_popup(char *title, char *msg)
{
	XmString	xs;

	if (!popup_ok) {
		(void) fprintf(errfp, "%s %s:\n%s\n", PROGNAME, title, msg);
		return;
	}

	/* Set the dialog box title */
	xs = XmStringCreateSimple(title);
	XtVaSetValues(widgets.dialog.warning, XmNdialogTitle, xs, NULL);
	XmStringFree(xs);

	/* Set the dialog box message */
	xs = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues(widgets.dialog.warning, XmNmessageString, xs, NULL);
	XmStringFree(xs);

	/* Pop up the warning dialog */
	if (!XtIsManaged(widgets.dialog.warning))
		XtManageChild(widgets.dialog.warning);
}


/*
 * cd_fatal_popup
 *	Pop up the fatal error message dialog box.
 *
 * Args:
 *	title - The title bar text string.
 *	msg - The fatal error message text string.
 *
 * Return:
 *	Nothing.
 */
void
cd_fatal_popup(char *title, char *msg)
{
	XmString	xs;

	if (!popup_ok) {
		(void) fprintf(errfp, "%s %s:\n%s\n", PROGNAME, title, msg);
		exit(1);
	}

	/* Make sure that the cursor is normal */
	cd_busycurs(FALSE, CURS_ALL);

	if (!XtIsManaged(widgets.dialog.fatal)) {
		/* Set the dialog box title */
		xs = XmStringCreateSimple(title);
		XtVaSetValues(widgets.dialog.fatal, XmNdialogTitle, xs, NULL);
		XmStringFree(xs);

		/* Set the dialog box message */
		xs = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);
		XtVaSetValues(widgets.dialog.fatal, XmNmessageString, xs, NULL);
		XmStringFree(xs);

		/* Pop up the error dialog */
		XtManageChild(widgets.dialog.fatal);
	}
}


/*
 * cd_confirm_popup
 *	Pop up the user-confirmation message dialog box.
 *
 * Args:
 *	title - The title bar text string.
 *	msg - The fatal error message text string.
 *	f_yes - Pointer to the callback function if user selects OK
 *	a_yes - Argument passed to f_yes
 *	f_no - Pointer to the callback function if user selects Cancel
 *	a_no - Argument passed to f_no
 *
 * Return:
 *	Nothing.
 */
void
cd_confirm_popup(
	char *title,
	char *msg,
	XtCallbackProc f_yes,
	XtPointer a_yes,
	XtCallbackProc f_no,
	XtPointer a_no
)
{
	XmString	xs;
	Widget		yes_btn,
			no_btn;
	cbinfo_t	*yes_cbinfo,
			*no_cbinfo;

	if (!popup_ok)
		/* Not allowed */
		return;

	/* Set the dialog box title */
	xs = XmStringCreateSimple(title);
	XtVaSetValues(widgets.dialog.confirm, XmNdialogTitle, xs, NULL);
	XmStringFree(xs);

	/* Set the dialog box message */
	xs = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues(widgets.dialog.confirm, XmNmessageString, xs, NULL);
	XmStringFree(xs);

	/* Add callbacks */
	yes_btn = XmMessageBoxGetChild(
		widgets.dialog.confirm,
		XmDIALOG_OK_BUTTON
	);
	no_btn = XmMessageBoxGetChild(
		widgets.dialog.confirm,
		XmDIALOG_CANCEL_BUTTON
	);

	if (f_yes != NULL) {
		yes_cbinfo = (cbinfo_t *)(void *) MEM_ALLOC(
			"yes_cbinfo",
			sizeof(cbinfo_t)
		);
		if (yes_cbinfo == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}

		yes_cbinfo->widget0 = yes_btn;
		yes_cbinfo->widget1 = no_btn;
		yes_cbinfo->widget2 = (Widget) NULL;
		yes_cbinfo->type = XmNactivateCallback;
		yes_cbinfo->func = f_yes;
		yes_cbinfo->data = a_yes;

		XtAddCallback(
			yes_btn,
			XmNactivateCallback,
			f_yes,
			a_yes
		);

		XtAddCallback(
			yes_btn,
			XmNactivateCallback,
			(XtCallbackProc) cd_rmcallback,
			(XtPointer) yes_cbinfo
		);

		XtAddCallback(
			no_btn,
			XmNactivateCallback,
			(XtCallbackProc) cd_rmcallback,
			(XtPointer) yes_cbinfo
		);
	}

	if (f_no != NULL) {
		no_cbinfo = (cbinfo_t *)(void *) MEM_ALLOC(
			"no_cbinfo",
			sizeof(cbinfo_t)
		);
		if (no_cbinfo == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		no_cbinfo->widget0 = no_btn;
		no_cbinfo->widget1 = yes_btn;
		no_cbinfo->widget2 = XtParent(widgets.dialog.confirm);
		no_cbinfo->type = XmNactivateCallback;
		no_cbinfo->func = f_no;
		no_cbinfo->data = a_no;

		XtAddCallback(
			no_btn,
			XmNactivateCallback,
			f_no,
			a_no
		);

		XtAddCallback(
			no_btn,
			XmNactivateCallback,
			(XtCallbackProc) cd_rmcallback,
			(XtPointer) no_cbinfo
		);

		XtAddCallback(
			yes_btn,
			XmNactivateCallback,
			(XtCallbackProc) cd_rmcallback,
			(XtPointer) no_cbinfo
		);

		/* Install WM_DELETE_WINDOW handler */
		add_delw_callback(
			XtParent(widgets.dialog.confirm),
			f_no,
			a_no
		);
	}

	/* Pop up the confirm dialog */
	if (!XtIsManaged(widgets.dialog.confirm))
		XtManageChild(widgets.dialog.confirm);
}


/*
 * cd_init
 *	Top level function that initializes all subsystems.  Used on
 *	program startup.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
cd_init(curstat_t *s)
{
	int		i;
	char		*cp,
			*bdevname,
			*hd;
	cddb_path_t	*pathp;
	XmString	xs;
	char		titlestr[STR_BUF_SZ],
			str[FILE_PATH_SZ * 2],
			*errmsg;
	struct stat	stbuf;


	DBGPRN(errfp, "XMCD v%s%s PL%d DEBUG MODE\n\n",
	       VERSION, VERSION_EXT, PATCHLEVEL);

	/* app-defaults file check */
	if (app_data.version == NULL || 
	    strncmp(VERSION, app_data.version, strlen(VERSION)) != 0) {
		CD_FATAL(app_data.str_appdef);
		return;
	}

#ifndef NOMKTMPDIR
	errmsg = (char *) MEM_ALLOC(
		"errmsg",
		strlen(app_data.str_tmpdirerr) + strlen(TEMP_DIR)
	);
	if (errmsg == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	/* Make temporary directory, if needed */
	(void) sprintf(errmsg, app_data.str_tmpdirerr, TEMP_DIR);
	if (LSTAT(TEMP_DIR, &stbuf) < 0) {
		if (!util_mkdir(TEMP_DIR, 0777)) {
			CD_FATAL(errmsg);
			return;
		}
	}
	else if (!S_ISDIR(stbuf.st_mode)) {
		CD_FATAL(errmsg);
		return;
	}

	MEM_FREE(errmsg);
#endif	/* NOMKTMPDIR */

	/* Initialize libutil */
	util_init();

	if ((cp = (char *) getenv("XMCD_LIBDIR")) != NULL) {
		app_data.libdir = (char *) MEM_ALLOC(
			"app_data.libdir",
			strlen(cp) + 1
		);
		if (app_data.libdir == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}

		(void) strcpy(app_data.libdir, cp);
	}

	if (app_data.libdir == NULL || app_data.libdir[0] == '\0') {
		/* No library directory specified */
		if (di_isdemo()) {
			/* Demo mode: just fake it */
			app_data.libdir = ".";
		}
		else {
			/* Real application: this is a fatal error */
			CD_FATAL(app_data.str_libdirerr);
			return;
		}
	}

	/* Paranoia: avoid overflowing buffers */
	if ((int) strlen(app_data.libdir) >= FILE_PATH_SZ) {
		CD_FATAL(app_data.str_longpatherr);
		return;
	}
	if (app_data.device != NULL) {
		if ((int) strlen(app_data.device) >= FILE_PATH_SZ ||
		    (int) strlen(util_basename(app_data.device)) >=
		    FILE_BASE_SZ) {
			CD_FATAL(app_data.str_longpatherr);
			return;
		}
	}
	hd = util_homedir(util_get_ouid());
	if ((int) strlen(hd) >= FILE_PATH_SZ) {
		CD_FATAL(app_data.str_longpatherr);
		return;
	}

	/* Get system common configuration parameters */
	(void) sprintf(str, SYS_CMCFG_PATH, app_data.libdir);
	di_common_parmload(str, TRUE);

	/* Get user common configuration parameters */
	(void) sprintf(str, USR_CMCFG_PATH, hd);
	di_common_parmload(str, FALSE);

	/* Initialize CD database services */
	(void) strcpy(cddb_cldata.prog, PROGNAME);
	(void) strcpy(cddb_cldata.user, util_loginname());
	cddb_cldata.isdemo = di_isdemo;
	cddb_cldata.curstat_addr = curstat_addr;
	cddb_cldata.fatal_msg = cd_fatal_popup;
	cddb_cldata.warning_msg = cd_warning_popup;
	cddb_cldata.info_msg = cd_info_popup;
	cddb_cldata.workproc = event_loop;
	cddb_init(&cddb_cldata);

	i = 0;
	for (pathp = cddb_pathlist(); pathp != NULL; pathp = pathp->next) {
		if (pathp->type != CDDB_LOCAL)
			continue;

		/* Add path to list in directory selector popup */
		(void) strcpy(str, "  ");
		(void) strncat(str, pathp->path, FILE_PATH_SZ);
		str[FILE_PATH_SZ] = '\0';
		xs = XmStringCreateSimple(str);
		XmListAddItemUnselected(widgets.dirsel.dir_list, xs, ++i);
		XmStringFree(xs);
	}

	bdevname = util_basename(app_data.device);

	/* Get system-wide device-specific configuration parameters */
	(void) sprintf(str, SYS_DSCFG_PATH, app_data.libdir, bdevname);
	di_devspec_parmload(str, TRUE);

	/* Get user device-specific configuration parameters */
	(void) sprintf(str, USR_DSCFG_PATH, hd, bdevname);
	di_devspec_parmload(str, FALSE);

	/* Initialize help system */
	help_init();

	/* Initialize the database/program subsystem */
	dbprog_init(s);

	/* Initialize the CD interface subsystem */
	di_cldata.curstat_addr = curstat_addr;
	di_cldata.quit = cd_quit;
	di_cldata.timeout = cd_timeout;
	di_cldata.untimeout = cd_untimeout;
	di_cldata.dbclear = dbprog_dbclear;
	di_cldata.dbget = dbprog_dbget;
	di_cldata.progclear = dbprog_progclear;
	di_cldata.fatal_msg = cd_fatal_popup;
	di_cldata.warning_msg = cd_warning_popup;
	di_cldata.info_msg = cd_info_popup;
	di_cldata.beep = cd_beep;
	di_cldata.set_lock_btn = set_lock_btn;
	di_cldata.set_shuffle_btn = set_shuffle_btn;
	di_cldata.set_vol_slider = set_vol_slider;
	di_cldata.set_bal_slider = set_bal_slider;
	di_cldata.dpy_all = dpy_all;
	di_cldata.dpy_disc = dpy_disc;
	di_cldata.dpy_track = dpy_track;
	di_cldata.dpy_index = dpy_index;
	di_cldata.dpy_time = dpy_time;
	di_cldata.dpy_playmode = dpy_playmode;
	di_cldata.dpy_rptcnt = dpy_rptcnt;
	di_init(&di_cldata);

	/* Set default modes */
	di_repeat(s, app_data.repeat_mode);
	set_repeat_btn(s->repeat);
	di_shuffle(s, app_data.shuffle_mode);
	set_shuffle_btn(s->shuffle);
	keypad_mode = KPMODE_TRACK;
	s->time_dpy = (byte_t) app_data.timedpy_mode;

	/* Set default options */
	cd_options_reset(widgets.options.reset_btn, (XtPointer) s, NULL);

	/* Set the main window and icon titles */
	(void) strncpy(titlestr, app_data.main_title, STR_BUF_SZ - 12);
	titlestr[STR_BUF_SZ - 12] = '\0';
	(void) sprintf(titlestr, "%s %d", titlestr, app_data.devnum);
	XtVaSetValues(widgets.toplevel,
		XmNtitle, titlestr,
		XmNiconName, PROGNAME,
		NULL
	);
}


/*
 * cd_start
 *	Secondary startup functions
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
cd_start(curstat_t *s)
{
	/* Allow popup dialogs from here on */
	popup_ok = TRUE;

	/* Start up libutil */
	util_start();

	/* Start up I/O interface */
	di_start(s);

	/* Start up help */
	help_start();
}


/*
 * cd_icon
 *	Main window iconification/deiconification handler.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	iconified - Whether the main window is iconified.
 *
 * Return:
 *	Nothing.
 */
void
cd_icon(curstat_t *s, bool_t iconified)
{
	di_icon(s, iconified);
}


/*
 * cd_halt
 *	Top level function to shut down all subsystems.  Used when
 *	closing the application.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
cd_halt(curstat_t *s)
{
	di_halt(s);
	cddb_halt(s);
}


/*
 * cd_quit
 *	Close the application.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
cd_quit(curstat_t *s)
{
	XmAnyCallbackStruct	p;

	if (XtIsRealized(widgets.toplevel))
		XtUnmapWidget(widgets.toplevel);

	/* Cancel asynchronous CDDB load operation, if active */
	cddb_load_cancel();

	/* Shut down all xmcd subsystems */
	cd_halt(s);

	/* Uninstall current keyboard grabs */
	p.reason = XmCR_FOCUS;
	cd_shell_focus_chg(
		widgets.toplevel,
		(XtPointer) widgets.toplevel,
		(XtPointer) &p
	);

	/* Let X events drain */
	event_loop(0);

	exit(0);
}


/*
 * cd_busycurs
 *	Enable/disable the watch cursor.
 *
 * Args:
 *	busy   - Boolean value indicating whether to enable or disable the
 *		 watch cursor.
 *	winmap - Bitmap of form widgets in which the cursor should be
 *		 affected
 *
 * Return:
 *	Nothing.
 */
void
cd_busycurs(bool_t busy, int winmap)
{
	Display		*dpy = XtDisplay(widgets.toplevel);
	Window		win;
	static Cursor	wcur = (Cursor) 0;

	if (wcur == (Cursor) 0)
		wcur = XCreateFontCursor(dpy, XC_watch);

	if (winmap == 0)
		return;

	if (busy) {
		if ((winmap & CURS_MAIN) &&
		    (win = XtWindow(widgets.main.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_KEYPAD) &&
		    (win = XtWindow(widgets.keypad.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_OPTIONS) &&
		    (win = XtWindow(widgets.options.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_DBPROG) &&
		    (win = XtWindow(widgets.dbprog.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_WWWWARP) &&
		    (win = XtWindow(widgets.wwwwarp.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_DBEXTD) &&
		    (win = XtWindow(widgets.dbextd.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_DBEXTT) &&
		    (win = XtWindow(widgets.dbextt.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_DIRSEL) &&
		    (win = XtWindow(widgets.dirsel.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_LINKSEL) &&
		    (win = XtWindow(widgets.linksel.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
		if ((winmap & CURS_HELP) &&
		    (win = XtWindow(widgets.help.form)) != (Window) 0)
			XDefineCursor(dpy, win, wcur);
	}
	else {
		if ((winmap & CURS_MAIN) &&
		    (win = XtWindow(widgets.main.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_KEYPAD) &&
		    (win = XtWindow(widgets.keypad.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_OPTIONS) &&
		    (win = XtWindow(widgets.options.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_DBPROG) &&
		    (win = XtWindow(widgets.dbprog.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_WWWWARP) &&
		    (win = XtWindow(widgets.wwwwarp.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_DBEXTD) &&
		    (win = XtWindow(widgets.dbextd.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_DBEXTT) &&
		    (win = XtWindow(widgets.dbextt.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_DIRSEL) &&
		    (win = XtWindow(widgets.dirsel.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_LINKSEL) &&
		    (win = XtWindow(widgets.linksel.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
		if ((winmap & CURS_HELP) &&
		    (win = XtWindow(widgets.help.form)) != (Window) 0)
			XUndefineCursor(dpy, win);
	}
	XFlush(dpy);
}


/*
 * onsig
 *	Signal handler.  Causes the application to shut down gracefully.
 *
 * Args:
 *	sig - The signal number received.
 *
 * Return:
 *	Nothing.
 */
void
onsig(int sig)
{
	(void) signal(sig, SIG_IGN);
	cd_quit(curstat_addr());
}


/**************** vv Callback routines vv ****************/

/*
 * cd_checkbox
 *	Main window checkbox callback function
 */
/*ARGSUSED*/
void
cd_checkbox(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmRowColumnCallbackStruct	*p =
		(XmRowColumnCallbackStruct *)(void *) call_data;
	curstat_t			*s =
		(curstat_t *)(void *) client_data;

	if (p->reason != XmCR_ACTIVATE)
		return;

	DBGPRN(errfp, "\n* CHKBOX: ");

	if (p->widget == widgets.main.lock_btn) {
		DBGPRN(errfp, "lock\n");

		di_lock(s, (bool_t) !s->caddy_lock);
	}
	else if (p->widget == widgets.main.repeat_btn) {
		DBGPRN(errfp, "repeat\n");

		di_repeat(s, (bool_t) !s->repeat);
	}
	else if (p->widget == widgets.main.shuffle_btn) {
		DBGPRN(errfp, "shuffle\n");

		di_shuffle(s, (bool_t) !s->shuffle);
	}
}


/*
 * cd_mode
 *	Main window mode button callback function
 */
/*ARGSUSED*/
void
cd_mode(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	DBGPRN(errfp, "\n* MODE\n");

	/* Change the main window arrangement.  Unmap the form while
	 * the change takes place so that the user does not see a
	 * bunch of jumbled widgets while they adjust to the new size.
	 */
	XtUnmapWidget(widgets.main.form);
	geom_main_chgmode(&widgets);
	XtMapWidget(widgets.main.form);

	/* Force indicator font change */
	mode_chg = TRUE;
	dpy_track(s);
	dpy_time(s, FALSE);
	mode_chg = FALSE;

	/* This is a hack to prevent the tooltip from popping up due to
	 * the main window reconfiguration.
	 */
	skip_next_tooltip = TRUE;

	/* Overload the function of this button to also
	 * dump the contents of the curstat_t structure
	 * in debug mode
	 */
	if (app_data.debug)
		di_dump_curstat(s);
}


/*
 * cd_load_eject
 *	Main window load/eject button callback function
 */
/*ARGSUSED*/
void
cd_load_eject(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	DBGPRN(errfp, "\n* LOAD_EJECT\n");

	if (searching) {
		cd_beep();
		return;
	}
	if (s->mode == MOD_PAUSE)
		dpy_time(s, FALSE);

	if (s->mode != MOD_BUSY && s->mode != MOD_NODISC) {
		s->flags |= STAT_EJECT;

		/* Ask the user if the changed CDDB entry should be
		 * saved to file.
		 */
		if (!dbprog_chgsave(s))
			return;
	}

	/* Change to watch cursor */
	cd_busycurs(TRUE, CURS_ALL);

	/* Cancel asynchronous CDDB load operation, if active */
	cddb_load_cancel();

	/* Load/Eject the CD */
	di_load_eject(s);

	/* Change to normal cursor */
	cd_busycurs(FALSE, CURS_ALL);

	s->flags &= ~STAT_EJECT;
}


/*
 * cd_quit_btn
 *	Main window quit button callback function
 */
/*ARGSUSED*/
void
cd_quit_btn(Widget w, XtPointer client_data, XtPointer call_data)
{
	DBGPRN(errfp, "\n* QUIT\n");

	cd_confirm_popup(
		app_data.str_confirm,
		app_data.str_quit,
		(XtCallbackProc) cd_exit,
		client_data,
		(XtCallbackProc) NULL,
		NULL
	);
}


/*
 * cd_dbprog
 *	Main window dbprog button callback function
 */
void
cd_dbprog(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (XtIsManaged(widgets.dbprog.form)) {
		/* Pop down the Database/Program window */
		dbprog_cancel(w, client_data, call_data);
		return;
	}

	/* Pop up the Database/Program window */
	dbprog_popup(w, client_data, call_data);

	XmProcessTraversal(
		widgets.dbprog.cancel_btn,
		XmTRAVERSE_CURRENT
	);
}


/*
 * cd_time
 *	Main window time mode button callback function
 */
/*ARGSUSED*/
void
cd_time(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	switch (s->time_dpy) {
	case T_ELAPSED:
		s->time_dpy = T_ELAPSED_DISC;
		break;

	case T_ELAPSED_DISC:
		s->time_dpy = T_REMAIN_TRACK;
		break;

	case T_REMAIN_TRACK:
		s->time_dpy = T_REMAIN_DISC;
		break;

	case T_REMAIN_DISC:
		s->time_dpy = T_ELAPSED;
		break;
	}

	dpy_timemode(s);
	dpy_track(s);
	dpy_time(s, FALSE);
}


/*
 * cd_ab
 *	Main window a->b mode button callback function
 */
/*ARGSUSED*/
void
cd_ab(Widget w, XtPointer client_data, XtPointer call_data)
{
	DBGPRN(errfp, "\n* A->B\n");

	if (searching) {
		cd_beep();
		return;
	}
	di_ab((curstat_t *)(void *) client_data);
}


/*
 * cd_sample
 *	Main window sample mode button callback function
 */
/*ARGSUSED*/
void
cd_sample(Widget w, XtPointer client_data, XtPointer call_data)
{
	DBGPRN(errfp, "\n* SAMPLE\n");

	if (searching) {
		cd_beep();
		return;
	}
	di_sample((curstat_t *)(void *) client_data);
}


/*
 * cd_level
 *	Main window volume control slider callback function
 */
/*ARGSUSED*/
void
cd_level(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmScaleCallbackStruct
			*p = (XmScaleCallbackStruct *)(void *) call_data;

	DBGPRN(errfp, "\n* VOL\n");

	di_level(
		(curstat_t *)(void *) client_data,
		(byte_t) p->value,
		(bool_t) (p->reason != XmCR_VALUE_CHANGED)
	);
}


/*
 * cd_play_pause
 *	Main window play/pause button callback function
 */
/*ARGSUSED*/
void
cd_play_pause(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	sword32_t	sav_trk;

	DBGPRN(errfp, "\n* PLAY_PAUSE\n");

	if (searching) {
		cd_beep();
		return;
	}

	if (s->mode == MOD_STOP && s->program) {
		if (!dbprog_pgm_parse(s)) {
			cd_beep();
			return;
		}
	}

	sav_trk = s->cur_trk;

	di_play_pause(s);

	if (sav_trk >= 0) {
		/* Update curfile */
		dbprog_curfileupd();
	}

	switch (s->mode) {
	case MOD_PAUSE:
		dpy_time(s, FALSE);
		break;
	case MOD_PLAY:
	case MOD_STOP:
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		dpy_time(s, FALSE);

		cd_keypad_clear(w, client_data, NULL);
		warp_busy = FALSE;
		break;
	}
}


/*
 * cd_stop
 *	Main window stop button callback function
 */
/*ARGSUSED*/
void
cd_stop(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	DBGPRN(errfp, "\n* STOP\n");

	if (searching) {
		cd_beep();
		return;
	}

	dpy_time(s, FALSE);

	di_stop(s, TRUE);
}


/*
 * cd_chgdisc
 *	Main window disc change buttons callback function
 */
/*ARGSUSED*/
void
cd_chgdisc(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	int		newdisc;

	if (s->first_disc == s->last_disc) {
		/* Single disc player */
		cd_beep();
		return;
	}

	newdisc = s->cur_disc;

	if (w == widgets.main.prevdisc_btn) {
		DBGPRN(errfp, "\n* PREV_DISC\n");
		if (newdisc > s->first_disc)
			newdisc--;
	}
	else if (w == widgets.main.nextdisc_btn) {
		DBGPRN(errfp, "\n* NEXT_DISC\n");
		if (newdisc < s->last_disc)
			newdisc++;
	}
	else
		return;

	if (newdisc == s->cur_disc)
		/* No change */
		return;

	s->prev_disc = s->cur_disc;
	s->cur_disc = newdisc;
	dpy_disc(s);

	s->flags |= STAT_CHGDISC;

	/* Ask the user if the changed CDDB entry should be
	 * saved to file.
	 */
	if (!dbprog_chgsave(s))
		return;

	s->flags &= ~STAT_CHGDISC;

	/* Update display: clear disc/track titles on the main window */
	chgdelay = TRUE;
	dpy_dtitle(s);
	dpy_ttitle(s);

	/* Use a timer callback routine to do the disc change.
	 * This allows the user to click on the disc change buttons
	 * multiple times to advance/reverse to the desired
	 * target disc without causing the changer to actually
	 * switch through all of them.
	 */
	if (chgdisc_dlyid >= 0)
		cd_untimeout(chgdisc_dlyid);

	chgdisc_dlyid = cd_timeout(CHGDISC_DELAY, do_chgdisc, (byte_t *) s);
}


/*
 * cd_prevtrk
 *	Main window prev track button callback function
 */
/*ARGSUSED*/
void
cd_prevtrk(Widget w, XtPointer client_data, XtPointer call_data)
{
	DBGPRN(errfp, "\n* PREVTRK\n");

	if (searching) {
		cd_beep();
		return;
	}
	di_prevtrk((curstat_t *)(void *) client_data);
}


/*
 * cd_nexttrk
 *	Main window next track button callback function
 */
/*ARGSUSED*/
void
cd_nexttrk(Widget w, XtPointer client_data, XtPointer call_data)
{
	DBGPRN(errfp, "\n* NEXTTRK\n");

	if (searching) {
		cd_beep();
		return;
	}
	di_nexttrk((curstat_t *)(void *) client_data);
}


/*
 * cd_previdx
 *	Main window prev index button callback function
 */
/*ARGSUSED*/
void
cd_previdx(Widget w, XtPointer client_data, XtPointer call_data)
{
	DBGPRN(errfp, "\n* PREVIDX\n");

	if (searching) {
		cd_beep();
		return;
	}
	di_previdx((curstat_t *)(void *) client_data);
}


/*
 * cd_previdx
 *	Main window next index button callback function
 */
/*ARGSUSED*/
void
cd_nextidx(Widget w, XtPointer client_data, XtPointer call_data)
{
	DBGPRN(errfp, "\n* NEXTIDX\n");

	if (searching) {
		cd_beep();
		return;
	}
	di_nextidx((curstat_t *)(void *) client_data);
}


/*
 * cd_rew
 *	Main window search rewind button callback function
 */
/*ARGSUSED*/
void
cd_rew(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmPushButtonCallbackStruct
			*p = (XmPushButtonCallbackStruct *)(void *) call_data;
	curstat_t	*s = (curstat_t *)(void *) client_data;
	bool_t		start;
	static bool_t	rew_running = FALSE;

	if (p->reason == XmCR_ARM) {
		DBGPRN(errfp, "\n* REW: down\n");

		if (!rew_running) {
			if (searching) {
				/* Release running FF */
				XtCallActionProc(
					widgets.main.ff_btn,
					"Activate",
					p->event,
					NULL,
					0
				);
				XtCallActionProc(
					widgets.main.ff_btn,
					"Disarm",
					p->event,
					NULL,
					0
				);
			}

			rew_running = TRUE;
			searching = TRUE;
			start = TRUE;
		}
		else
			/* Already running REW */
			return;
	}
	else {
		DBGPRN(errfp, "\n* REW: up\n");

		if (rew_running) {
			rew_running = FALSE;
			searching = FALSE;
			start = FALSE;
		}
		else
			/* Not running REW */
			return;
	}

	di_rew(s, start);

	dpy_time(s, FALSE);
}


/*
 * cd_ff
 *	Main window search fast-forward button callback function
 */
/*ARGSUSED*/
void
cd_ff(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmPushButtonCallbackStruct
			*p = (XmPushButtonCallbackStruct *)(void *) call_data;
	curstat_t	*s = (curstat_t *)(void *) client_data;
	bool_t		start;
	static bool_t	ff_running = FALSE;

	if (p->reason == XmCR_ARM) {
		DBGPRN(errfp, "\n* FF: down\n");

		if (!ff_running) {
			if (searching) {
				/* Release running REW */
				XtCallActionProc(
					widgets.main.rew_btn,
					"Activate",
					p->event,
					NULL,
					0
				);
				XtCallActionProc(
					widgets.main.rew_btn,
					"Disarm",
					p->event,
					NULL,
					0
				);
			}

			ff_running = TRUE;
			searching = TRUE;
			start = TRUE;
		}
		else
			/* Already running FF */
			return;
	}
	else {
		DBGPRN(errfp, "\n* FF: up\n");

		if (ff_running) {
			ff_running = FALSE;
			searching = FALSE;
			start = FALSE;
		}
		else
			/* Not running FF */
			return;
	}

	di_ff(s, start);

	dpy_time(s, FALSE);
}


/*
 * cd_keypad_popup
 *	Main window keypad button callback function
 */
void
cd_keypad_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (XtIsManaged(widgets.keypad.form)) {
		/* Pop down keypad window */
		cd_keypad_popdown(w, client_data, call_data);
		return;
	}

	/* Pop up keypad window */
	XtManageChild(widgets.keypad.form);

	/* Reset keypad */
	cd_keypad_clear(w, client_data, NULL);

	/* Update warp slider */
	dpy_warp((curstat_t *)(void *) client_data);

	XmProcessTraversal(
		widgets.keypad.cancel_btn,
		XmTRAVERSE_CURRENT
	);
}


/*
 * cd_keypad_popdown
 *	Keypad window popdown callback function
 */
/*ARGSUSED*/
void
cd_keypad_popdown(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Pop down keypad window */
	if (XtIsManaged(widgets.keypad.form))
		XtUnmanageChild(widgets.keypad.form);
}


/*
 * cd_keypad_mode
 *	Keypad window mode selector callback function
 */
void
cd_keypad_mode(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmRowColumnCallbackStruct	*p =
		(XmRowColumnCallbackStruct *)(void *) call_data;
	XmToggleButtonCallbackStruct	*q;
	curstat_t			*s = (curstat_t *)(void *) client_data;

	if (p == NULL)
		return;

	q = (XmToggleButtonCallbackStruct *)(void *) p->callbackstruct;

	if (!q->set)
		return;

	if (p->widget == widgets.keypad.disc_btn) {
		if (keypad_mode == KPMODE_DISC)
			return;	/* No change */

		keypad_mode = KPMODE_DISC;
	}
	else if (p->widget == widgets.keypad.track_btn) {
		if (keypad_mode == KPMODE_TRACK)
			return;	/* No change */

		keypad_mode = KPMODE_TRACK;
	}
	else
		return;	/* Invalid widget */

	cd_keypad_clear(w, (XtPointer) s, NULL);
	warp_busy = FALSE;
}


/*
 * cd_keypad_num
 *	Keypad window number button callback function
 */
/*ARGSUSED*/
void
cd_keypad_num(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = curstat_addr();
	int		n;
	char		tmpstr[2];

	/* The user entered a digit */
	if (strlen(keystr) >= sizeof(keystr) - 1) {
		cd_beep();
		return;
	}

	(void) sprintf(tmpstr, "%u", (unsigned int) client_data);
	(void) strcat(keystr, tmpstr);

	switch (keypad_mode) {
	case KPMODE_DISC:
		n = atoi(keystr);

		if (n < s->first_disc || n > s->last_disc) {
			/* Illegal disc entered */
			cd_keypad_clear(w, (XtPointer) s, NULL);

			cd_beep();
			return;
		}
		break;

	case KPMODE_TRACK:
		n = s->cur_trk;
		s->cur_trk = (sword32_t) atoi(keystr);

		if (di_curtrk_pos(s) < 0) {
			/* Illegal track entered */
			cd_keypad_clear(w, (XtPointer) s, NULL);
			s->cur_trk = n;

			cd_beep();
			return;
		}
		s->cur_trk = n;
		break;

	default:
		/* Illegal mode */
		return;
	}

	warp_offset = 0;
	set_warp_slider(0, FALSE);
	dpy_keypad_ind(s);
}


/*
 * cd_keypad_clear
 *	Keypad window clear button callback function
 */
/*ARGSUSED*/
void
cd_keypad_clear(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	/* Reset keypad */
	keystr[0] = '\0';

	/* Hack: if the third arg is NULL, then it's an internal
	 * call rather than a callback.  We want to set s->cur_trk
	 * to -1 only for callbacks, so that the keypad indicator
	 * display gets updated correctly.
	 */
	if (call_data != NULL)
		s->cur_trk = -1;

	warp_offset = 0;
	set_warp_slider(0, FALSE);
	dpy_keypad_ind(s);
}


/*
 * cd_keypad_dsbl_modes_yes
 *	User "yes" confirm callback to cancel shuffle or program modes after
 *	activating the keypad.
 */
/*ARGSUSED*/
void
cd_keypad_dsbl_modes_yes(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	if (override_sav.func == NULL) {
		cd_beep();
		return;
	}

	/* Note: This assumes that shuffle and program modes are
	 * mutually exclusive!
	 */
	if (s->shuffle) {
		/* Disable shuffle mode */
		di_shuffle(s, FALSE);
		set_shuffle_btn(FALSE);
	}
	else if (s->program) {
		s->onetrk_prog = FALSE;
		dbprog_clrpgm(w, client_data, call_data);
	}

	(*override_sav.func)(
		override_sav.w,
		override_sav.client_data,
		override_sav.call_data
	);

	override_sav.func = (XtCallbackProc) NULL;
	if (override_sav.call_data != NULL) {
		MEM_FREE(override_sav.call_data);
		override_sav.call_data = NULL;
	}
}


/*
 * cd_keypad_dsbl_modes_no
 *	User "no" confirm callback to cancel shuffle or program modes after
 *	activating the keypad.
 */
/*ARGSUSED*/
void
cd_keypad_dsbl_modes_no(Widget w, XtPointer client_data, XtPointer call_data)
{
	warp_busy = FALSE;
	cd_keypad_clear(w, client_data, call_data);

	override_sav.func = (XtCallbackProc) NULL;
	if (override_sav.call_data != NULL) {
		MEM_FREE(override_sav.call_data);
		override_sav.call_data = NULL;
	}
}


/*
 * cd_keypad_enter
 *	Keypad window enter button callback function
 */
void
cd_keypad_enter(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	int		i;
	sword32_t	curr,
			next,
			sav_cur_trk;
	bool_t		paused = FALSE;

	/* The user activated the Enter key */
	if (keystr[0] == '\0') {
		/* No numeric input */
		cd_beep();
		return;
	}

	switch (keypad_mode) {
	case KPMODE_DISC:
		/* Use disc number selected on keypad */
		curr = s->cur_disc;
		next = (sword32_t) atoi(keystr);

		if (s->program && curr != next) {
			if (s->onetrk_prog) {
				s->onetrk_prog = FALSE;
				dbprog_clrpgm(w, client_data, call_data);
			}
			else {
				/* Trying to use keypad while in shuffle or
				 * program mode: ask user if shuffle/program
				 * should be disabled.
				 */
				cd_keypad_ask_dsbl(
					s,
					cd_keypad_enter,
					w,
					call_data,
					sizeof(XmPushButtonCallbackStruct)
				);
				return;
			}
		}

		s->prev_disc = curr;
		s->cur_disc = next;

		cd_keypad_clear(w, client_data, NULL);

		s->flags |= STAT_CHGDISC;

		/* Ask the user if the changed CDDB entry
		 * should be saved to file.
		 */
		if (!dbprog_chgsave(s))
			return;

		s->flags &= ~STAT_CHGDISC;

		/* Change to watch cursor */
		cd_busycurs(TRUE, CURS_ALL);

		/* Do the disc change */
		di_chgdisc(s);

		/* Change to normal cursor */
		cd_busycurs(FALSE, CURS_ALL);

		break;

	case KPMODE_TRACK:
		if (!di_check_disc(s)) {
			/* Cannot go to a track when the disc is not ready */
			cd_keypad_clear(w, client_data, NULL);
			cd_beep();
			return;
		}

		if (s->shuffle || s->program) {
			if (s->onetrk_prog) {
				s->onetrk_prog = FALSE;
				dbprog_clrpgm(w, client_data, call_data);
			}
			else {
				/* Trying to use keypad while in shuffle or
				 * program mode: ask user if shuffle/program
				 * should be disabled.
				 */
				cd_keypad_ask_dsbl(
					s,
					cd_keypad_enter,
					w,
					call_data,
					sizeof(XmPushButtonCallbackStruct)
				);
				return;
			}
		}

		/* Use track number selected on keypad */
		sav_cur_trk = s->cur_trk;
		s->cur_trk = (word32_t) atoi(keystr);

		if ((i = di_curtrk_pos(s)) < 0) {
			s->cur_trk = sav_cur_trk;
			cd_beep();
			return;
		}

		switch (s->mode) {
		case MOD_PAUSE:
			/* Mute sound */
			di_mute_on(s);
			paused = TRUE;

			/*FALLTHROUGH*/
		case MOD_PLAY:
		case MOD_A:
		case MOD_AB:
		case MOD_SAMPLE:
			sav_cur_trk = s->cur_trk;

			/* Set play status to stop */
			di_stop(s, FALSE);

			/* Restore s->cur_trk because di_stop
			 * resets it
			 */
			s->cur_trk = sav_cur_trk;

			break;

		default:
			break;
		}

		s->cur_trk_addr = warp_offset;
		util_blktomsf(
			s->cur_trk_addr,
			&s->cur_trk_min,
			&s->cur_trk_sec,
			&s->cur_trk_frame,
			0
		);
		s->cur_tot_addr = s->trkinfo[i].addr + warp_offset;
		util_blktomsf(
			s->cur_tot_addr,
			&s->cur_tot_min,
			&s->cur_tot_sec,
			&s->cur_tot_frame,
			MSF_OFFSET
		);

		/* Start playback at new position */
		cd_play_pause(w, client_data, call_data);

		if (paused) {
			/* This will cause the playback to pause */
			cd_play_pause(w, client_data, call_data);

			/* Restore sound */
			di_mute_off(s);
		}

		break;

	default:
		/* Illegal mode */
		break;
	}
}


/*
 * cd_warp
 *	Track warp function
 */
void
cd_warp(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	XmScaleCallbackStruct
			*p = (XmScaleCallbackStruct *)(void *) call_data;
	int		i;
	sword32_t	sav_cur_trk;

	if (pseudo_warp) {
		warp_busy = FALSE;
		return;
	}

	if (keypad_mode != KPMODE_TRACK ||
	    s->mode == MOD_BUSY || s->mode == MOD_NODISC) {
		warp_offset = 0;
		warp_busy = FALSE;
		set_warp_slider(0, FALSE);
		return;
	}

	sav_cur_trk = s->cur_trk;
	if (keystr[0] != '\0') {
		/* Use track number selected on keypad */
		s->cur_trk = atoi(keystr);
	}

	if ((i = di_curtrk_pos(s)) < 0) {
		warp_offset = 0;
		warp_busy = FALSE;
		set_warp_slider(0, FALSE);
		s->cur_trk = sav_cur_trk;
		return;
	}

	/* Translate slider position to block offset */
	warp_offset = (word32_t) scale_warp(s, i, p->value);

	if (p->reason == XmCR_VALUE_CHANGED) {
		if ((s->shuffle || s->program) &&
		    s->mode != MOD_STOP && sav_cur_trk != s->cur_trk) {
			if (s->onetrk_prog) {
				s->onetrk_prog = FALSE;
				dbprog_clrpgm(w, client_data, call_data);
			}
			else {
				/* Trying to warp to a different track while
				 * in shuffle or program mode: ask user if
				 * shuffle/program should be disabled.
				 */
				cd_keypad_ask_dsbl(
					s,
					cd_warp,
					w,
					call_data,
					sizeof(XmScaleCallbackStruct)
				);
				return;
			}
		}

		DBGPRN(errfp, "\n* TRACK WARP\n");

		s->cur_trk_addr = warp_offset;
		util_blktomsf(
			s->cur_trk_addr,
			&s->cur_trk_min,
			&s->cur_trk_sec,
			&s->cur_trk_frame,
			0
		);
		s->cur_tot_addr = s->trkinfo[i].addr + warp_offset;
		util_blktomsf(
			s->cur_tot_addr,
			&s->cur_tot_min,
			&s->cur_tot_sec,
			&s->cur_tot_frame,
			MSF_OFFSET
		);

		if (s->mode == MOD_STOP) {
			warp_busy = TRUE;
			dpy_keypad_ind(s);
			warp_busy = FALSE;
			return;
		}

		/* Start playback at new position */
		di_warp(s);

		cd_keypad_clear(w, client_data, NULL);
		warp_offset = 0;
		warp_busy = FALSE;

		/* Update display */
		dpy_track(s);
		dpy_index(s);
		dpy_time(s, FALSE);
	}
	else {
		warp_busy = TRUE;
	}

	dpy_keypad_ind(s);

	/* Restore s->cur_trk to actual */
	s->cur_trk = sav_cur_trk;
}


/*
 * cd_options_popup
 *	Options window popup callback function
 */
/*ARGSUSED*/
void
cd_options_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (XtIsManaged(widgets.options.form)) {
		/* Pop down options window */
		cd_options_popdown(w, client_data, call_data);
		return;
	}

	/* Pop up options window */
	XtManageChild(widgets.options.form);

	XmProcessTraversal(
		widgets.options.ok_btn,
		XmTRAVERSE_CURRENT
	);
}


/*
 * cd_options_popdown
 *	Options window popdown callback function
 */
/*ARGSUSED*/
void
cd_options_popdown(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Pop down options window */
	if (XtIsManaged(widgets.options.form))
		XtUnmanageChild(widgets.options.form);
}


/*
 * cd_options_reset
 *	Options window reset button callback function
 */
/*ARGSUSED*/
void
cd_options_reset(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	char		*bdevname,
			str[FILE_PATH_SZ * 2];

	if (call_data != NULL) {
		/* Re-read defaults */

		bdevname = util_basename(app_data.device);

		/* Get system-wide device-specific configuration parameters */
		(void) sprintf(str, SYS_DSCFG_PATH, app_data.libdir, bdevname);
		di_devspec_parmload(str, FALSE);

		/* Get user device-specific configuration parameters */
		(void) sprintf(str, USR_DSCFG_PATH,
			       util_homedir(util_get_ouid()), bdevname);
		di_devspec_parmload(str, FALSE);

		/* Set the channel routing */
		di_route(s);

		/* Set the volume level */
		di_level(s, (byte_t) s->level, TRUE);
	}

	XmToggleButtonSetState(
		widgets.options.load_lock_btn,
		(Boolean) app_data.caddy_lock,
		False
	);

	XmToggleButtonSetState(
		widgets.options.load_none_btn,
		(Boolean) (!app_data.load_spindown && !app_data.load_play),
		False
	);
	XmToggleButtonSetState(
		widgets.options.load_spdn_btn,
		(Boolean) app_data.load_spindown,
		False
	);
	XmToggleButtonSetState(
		widgets.options.load_play_btn,
		(Boolean) app_data.load_play,
		False
	);

	XmToggleButtonSetState(
		widgets.options.exit_none_btn,
		(Boolean) (!app_data.exit_stop && !app_data.exit_eject),
		False
	);
	XmToggleButtonSetState(
		widgets.options.exit_stop_btn,
		(Boolean) app_data.exit_stop,
		False
	);
	XmToggleButtonSetState(
		widgets.options.exit_eject_btn,
		(Boolean) app_data.exit_eject,
		False
	);

	XmToggleButtonSetState(
		widgets.options.done_eject_btn,
		(Boolean) app_data.done_eject,
		False
	);

	XmToggleButtonSetState(
		widgets.options.eject_exit_btn,
		(Boolean) app_data.eject_exit,
		False
	);

	XmToggleButtonSetState(
		widgets.options.chg_multiplay_btn,
		(Boolean) app_data.multi_play,
		False
	);

	XmToggleButtonSetState(
		widgets.options.chg_reverse_btn,
		(Boolean) app_data.reverse,
		False
	);

	XmToggleButtonSetState(
		widgets.options.vol_linear_btn,
		(Boolean) (app_data.vol_taper == 0),
		False
	);
	XmToggleButtonSetState(
		widgets.options.vol_square_btn,
		(Boolean) (app_data.vol_taper == 1),
		False
	);
	XmToggleButtonSetState(
		widgets.options.vol_invsqr_btn,
		(Boolean) (app_data.vol_taper == 2),
		False
	);

	XmToggleButtonSetState(
		widgets.options.chroute_stereo_btn,
		(Boolean) (app_data.ch_route == 0),
		False
	);
	XmToggleButtonSetState(
		widgets.options.chroute_rev_btn,
		(Boolean) (app_data.ch_route == 1),
		False
	);
	XmToggleButtonSetState(
		widgets.options.chroute_left_btn,
		(Boolean) (app_data.ch_route == 2),
		False
	);
	XmToggleButtonSetState(
		widgets.options.chroute_right_btn,
		(Boolean) (app_data.ch_route == 3),
		False
	);
	XmToggleButtonSetState(
		widgets.options.chroute_mono_btn,
		(Boolean) (app_data.ch_route == 4),
		False
	);

	/* Make the Reset and Save buttons insensitive */
	XtSetSensitive(widgets.options.reset_btn, False);
	XtSetSensitive(widgets.options.save_btn, False);
}


/*
 * cd_options_save
 *	Options window save button callback function
 */
/*ARGSUSED*/
void
cd_options_save(Widget w, XtPointer client_data, XtPointer call_data)
{
	char		str[FILE_PATH_SZ + 2];

	/* Change to watch cursor */
	cd_busycurs(TRUE, CURS_ALL);

	/* Save user device-specific configuration parameters */
	(void) sprintf(str, USR_DSCFG_PATH,
		       util_homedir(util_get_ouid()),
		       util_basename(app_data.device));
	di_devspec_parmsave(str);

	/* Make the Reset and Save buttons insensitive */
	XtSetSensitive(widgets.options.reset_btn, False);
	XtSetSensitive(widgets.options.save_btn, False);

	/* Change to normal cursor */
	cd_busycurs(FALSE, CURS_ALL);
}


/*
 * cd_options
 *	Options window toggle button callback function
 */
void
cd_options(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmRowColumnCallbackStruct	*p =
		(XmRowColumnCallbackStruct *)(void *) call_data;
	XmToggleButtonCallbackStruct	*q;
	curstat_t			*s = (curstat_t *)(void *) client_data;

	if (p == NULL)
		return;

	q = (XmToggleButtonCallbackStruct *)(void *) p->callbackstruct;

	if (w == widgets.options.load_chkbox) {
		if (p->widget == widgets.options.load_lock_btn) {
			if (app_data.caddylock_supp) {
				DBGPRN(errfp, "\n* OPTION: caddyLock=%d\n",
				       q->set);
				app_data.caddy_lock = (bool_t) q->set;
			}
			else {
				DBGPRN(errfp, "\n* OPTION: caddyLock=0\n");
				cd_beep();
				XmToggleButtonSetState(
					p->widget,
					False,
					False
				);
				return;
			}
		}
	}
	else if (w == widgets.options.load_radbox) {
		if (p->widget == widgets.options.load_spdn_btn) {
			DBGPRN(errfp, "\n* OPTION: spinDownOnLoad=%d\n",
			       q->set);
			app_data.load_spindown = (bool_t) q->set;
		}
		else if (p->widget == widgets.options.load_play_btn) {
			DBGPRN(errfp, "\n* OPTION: playOnLoad=%d\n", q->set);
			app_data.load_play = (bool_t) q->set;
		}
	}
	else if (w == widgets.options.exit_radbox) {
		if (p->widget == widgets.options.exit_stop_btn) {
			DBGPRN(errfp, "\n* OPTION: stopOnExit=%d\n", q->set);
			app_data.exit_stop = (bool_t) q->set;
		}
		else if (p->widget == widgets.options.exit_eject_btn) {
			if (app_data.eject_supp) {
				DBGPRN(errfp, "\n* OPTION: ejectOnExit=%d\n",
				       q->set);
				app_data.exit_eject = (bool_t) q->set;
			}
			else {
				DBGPRN(errfp, "\n* OPTION: ejectOnExit=0\n");
				cd_beep();
				XmToggleButtonSetState(
					p->widget,
					False,
					False
				);
				if (app_data.exit_stop) {
					XmToggleButtonSetState(
						widgets.options.exit_stop_btn,
						True,
						False
					);
				}
				else {
					XmToggleButtonSetState(
						widgets.options.exit_none_btn,
						True,
						False
					);
				}
				return;
			}
		}
	}
	else if (w == widgets.options.done_chkbox) {
		if (p->widget == widgets.options.done_eject_btn) {
			if (app_data.eject_supp) {
				DBGPRN(errfp, "\n* OPTION: ejectOnDone=%d\n",
				       q->set);
				app_data.done_eject = (bool_t) q->set;
			}
			else {
				DBGPRN(errfp, "\n* OPTION: ejectOnDone=0\n");
				cd_beep();
				XmToggleButtonSetState(
					p->widget,
					False,
					False
				);
				return;
			}
		}
	}
	else if (w == widgets.options.eject_chkbox) {
		if (p->widget == widgets.options.eject_exit_btn) {
			if (app_data.eject_supp) {
				DBGPRN(errfp, "\n* OPTION: exitOnEject=%d\n",
				       q->set);
				app_data.eject_exit = (bool_t) q->set;
			}
			else {
				DBGPRN(errfp, "\n* OPTION: exitOnEject=0\n");
				cd_beep();
				XmToggleButtonSetState(
					p->widget,
					False,
					False
				);
				return;
			}
		}
	}
	else if (w == widgets.options.chg_chkbox) {
		if (s->first_disc == s->last_disc) {
			/* Single-disc player: inhibit any change here */
			cd_beep();
			XmToggleButtonSetState(p->widget, False, False);
			return;
		}

		if (p->widget == widgets.options.chg_multiplay_btn) {
			DBGPRN(errfp, "\n* OPTION: multiPlay=%d\n",
			       q->set);
			app_data.multi_play = (bool_t) q->set;

			if (!app_data.multi_play) {
				app_data.reverse = FALSE;
				DBGPRN(errfp, "\n* OPTION: reversePlay=0\n");
				XmToggleButtonSetState(
					widgets.options.chg_reverse_btn,
					False,
					False
				);
			}
		}
		else if (p->widget == widgets.options.chg_reverse_btn) {
			DBGPRN(errfp, "\n* OPTION: reversePlay=%d\n",
			       q->set);
			app_data.reverse = (bool_t) q->set;

			if (app_data.reverse) {
				app_data.multi_play = TRUE;
				DBGPRN(errfp, "\n* OPTION: multiPlay=0\n");
				XmToggleButtonSetState(
					widgets.options.chg_multiplay_btn,
					True,
					False
				);
			}
		}
	}
	else if (w == widgets.options.chroute_radbox) {
		if (!q->set)
			return;

		if (!app_data.chroute_supp) {
			/* Channel routing not supported: force to
			 * normal setting.
			 */
			cd_beep();
			XmToggleButtonSetState(
				p->widget,
				False,
				False
			);
			XmToggleButtonSetState(
				widgets.options.chroute_stereo_btn,
				True,
				False
			);
			return;
		}

		if (p->widget == widgets.options.chroute_stereo_btn) {
			DBGPRN(errfp, "\n* OPTION: channelRoute=0\n");
			app_data.ch_route = 0;
		}
		else if (p->widget == widgets.options.chroute_rev_btn) {
			DBGPRN(errfp, "\n* OPTION: channelRoute=1\n");
			app_data.ch_route = 1;
		}
		else if (p->widget == widgets.options.chroute_left_btn) {
			DBGPRN(errfp, "\n* OPTION: channelRoute=2\n");
			app_data.ch_route = 2;
		}
		else if (p->widget == widgets.options.chroute_right_btn) {
			DBGPRN(errfp, "\n* OPTION: channelRoute=3\n");
			app_data.ch_route = 3;
		}
		else if (p->widget == widgets.options.chroute_mono_btn) {
			DBGPRN(errfp, "\n* OPTION: channelRoute=4\n");
			app_data.ch_route = 4;
		}

		di_route(s);
	}
	else if (w == widgets.options.vol_radbox) {
		if (!q->set)
			return;

		if (!app_data.mselvol_supp) {
			/* Volume control not supported: force to
			 * linear setting.
			 */
			cd_beep();
			XmToggleButtonSetState(
				p->widget,
				False,
				False
			);
			XmToggleButtonSetState(
				widgets.options.vol_linear_btn,
				True,
				False
			);
			return;
		}

		if (p->widget == widgets.options.vol_linear_btn) {
			DBGPRN(errfp, "\n* OPTION: volumeControlTaper=0\n");
			app_data.vol_taper = 0;
		}
		else if (p->widget == widgets.options.vol_square_btn) {
			DBGPRN(errfp, "\n* OPTION: volumeControlTaper=1\n");
			app_data.vol_taper = 1;
		}
		else if (p->widget == widgets.options.vol_invsqr_btn) {
			DBGPRN(errfp, "\n* OPTION: volumeControlTaper=2\n");
			app_data.vol_taper = 2;
		}

		di_level(s, (byte_t) s->level, TRUE);
	}

	/* Make the Reset and Save buttons sensitive */
	XtSetSensitive(widgets.options.reset_btn, True);
	XtSetSensitive(widgets.options.save_btn, True);
}


/*
 * cd_balance
 *	Balance control slider callback function
 */
/*ARGSUSED*/
void
cd_balance(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmScaleCallbackStruct
			*p = (XmScaleCallbackStruct *)(void *) call_data;
	curstat_t	*s = (curstat_t *)(void *) client_data;

	if (!app_data.balance_supp) {
		if (p->reason == XmCR_VALUE_CHANGED)
			set_bal_slider(0);
		return;
	}

	if (p->value == 0) {
		/* Center setting */
		s->level_left = s->level_right = 100;
	}
	else if (p->value < 0) {
		/* Attenuate the right channel */
		s->level_left = 100;
		s->level_right = 100 + (p->value * 2);
	}
	else {
		/* Attenuate the left channel */
		s->level_left = 100 - (p->value * 2);
		s->level_right = 100;
	}

	di_level(
		s,
		(byte_t) s->level,
		(bool_t) (p->reason != XmCR_VALUE_CHANGED)
	);
}


/*
 * cd_balance_center
 *	Balance control center button callback function
 */
/*ARGSUSED*/
void
cd_balance_center(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmScaleCallbackStruct	d;

	/* Force the balance control to the center position */
	set_bal_slider(0);

	/* Force a callback */
	d.reason = XmCR_VALUE_CHANGED;
	d.value = 0;
	cd_balance(widgets.options.bal_scale, client_data, (XtPointer) &d);
}


/*
 * cd_about
 *	Program information popup callback function
 */
/*ARGSUSED*/
void
cd_about(Widget w, XtPointer client_data, XtPointer call_data)
{
	int		allocsz;
	char		*txt;
	cddb_path_t	*pp;
	XmString	xs_progname,
			xs_desc,
			xs_info,
			xs_tmp,
			xs;
	curstat_t	*s = (curstat_t *)(void *) client_data;

	if (XtIsManaged(widgets.dialog.about)) {
		/* Pop down the about dialog box */
		XtUnmanageChild(widgets.dialog.about);
		return;
	}

	allocsz = STR_BUF_SZ * 32;
	if (app_data.cddb_path != NULL)
		allocsz += strlen(app_data.cddb_path);

	if ((txt = (char *) MEM_ALLOC("about_allocsz", allocsz)) == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	xs_progname = XmStringCreateLtoR(PROGNAME, CHSET1);

	(void) sprintf(txt, "   v%s%s PL%d\n%s\n%s\n%s\n%s\n\n",
		       VERSION,
		       VERSION_EXT,
		       PATCHLEVEL,
		       "Motif(tm) CD Audio Player",
		       COPYRIGHT,
		       EMAIL,
		       WWWURL);

	xs_desc = XmStringCreateLtoR(txt, CHSET2);

	(void) sprintf(txt, "%s\n%s%s %s %s%s%s\n%s%s\n%s%s\n\n%s",
		       di_vers(),
		       "CD-ROM: ",
		       (s->vendor[0] == '\0') ? "??" : s->vendor,
		       s->prod,
		       (s->revnum[0] == '\0') ? "" : "(",
		       s->revnum,
		       (s->revnum[0] == '\0') ? "" : ")",
		       "Device: ",
		       s->curdev,
		       "Mode:   ",
		       di_mode(),
		       "CD Database paths:"
	);

	pp = cddb_pathlist();
	if (pp != NULL) {
		for (; pp != NULL; pp = pp->next) {
			switch (pp->type) {
			case CDDB_LOCAL:
				(void) sprintf(txt, "%s\n    %s",
					       txt, pp->path);
				break;

			case CDDB_REMOTE_CDDBP:
				(void) sprintf(txt,
					       "%s\n    cddbp://%s:%d",
					       txt, pp->host, pp->port);
				break;

			case CDDB_REMOTE_HTTP:
				(void) sprintf(txt,
					       "%s\n    http://%s:%d/%s",
					       txt, pp->host, pp->port,
					       pp->path);
				break;

			case CDDB_INVALID:
			default:
				break;
			}
		}
	}
	else
		(void) strcat(txt, " None");

	(void) sprintf(txt, "%s\n\nRemote database server access %s\n", txt,
		       cddb_rmt_support() ? "enabled" : "disabled");

	(void) sprintf(txt, "%s\n%s", txt, GNU_BANNER);

	xs_info = XmStringCreateLtoR(txt, CHSET3);

	/* Set the dialog box title */
	xs = XmStringCreateSimple(app_data.str_about);
	XtVaSetValues(widgets.dialog.about, XmNdialogTitle, xs, NULL);
	XmStringFree(xs);

	/* Set the dialog box message */
	xs_tmp = XmStringConcat(xs_progname, xs_desc);
	xs = XmStringConcat(xs_tmp, xs_info);
	XtVaSetValues(widgets.dialog.about, XmNmessageString, xs, NULL);
	XmStringFree(xs_progname);
	XmStringFree(xs_desc);
	XmStringFree(xs_info);
	XmStringFree(xs_tmp);
	XmStringFree(xs);

	MEM_FREE(txt);

	/* Pop up the about dialog box */
	XtManageChild(widgets.dialog.about);
}


/*
 * cd_help_popup
 *	Program help window popup callback function
 */
/*ARGSUSED*/
void
cd_help_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (w == widgets.main.help_btn && help_isactive()) {
		/* Pop down help window */
		help_popdown();
		return;
	}

	/* Pop up help window */
	help_popup(w);
}


/*
 * cd_help_popdown
 *	Program help window popdown callback function
 */
/*ARGSUSED*/
void
cd_help_popdown(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Pop down help window */
	if (help_isactive())
		help_popdown();
}


/*
 * cd_warning_popdown
 *	Warning message dialog box popdown callback function
 */
/*ARGSUSED*/
void
cd_warning_popdown(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Pop down the warning dialog */
	if (XtIsManaged(widgets.dialog.warning))
		XtUnmanageChild(widgets.dialog.warning);
}


/*
 * cd_fatal_popdown
 *	Fatal error message dialog box popdown callback function.
 *	This causes the application to terminate.
 */
/*ARGSUSED*/
void
cd_fatal_popdown(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Pop down the error dialog */
	if (XtIsManaged(widgets.dialog.fatal))
		XtUnmanageChild(widgets.dialog.fatal);

	/* Quit */
	cd_quit((curstat_t *)(void *) client_data);
}


/*
 * cd_rmcallback
 *	Remove callback function specified in cdinfo_t.
 */
/*ARGSUSED*/
void
cd_rmcallback(Widget w, XtPointer client_data, XtPointer call_data)
{
	cbinfo_t	*cb = (cbinfo_t *)(void *) client_data;

	if (cb == NULL)
		return;

	XtRemoveCallback(
		cb->widget0,
		cb->type,
		(XtCallbackProc) cb->func,
		(XtPointer) cb->data
	);

	XtRemoveCallback(
		cb->widget0,
		cb->type,
		(XtCallbackProc) cd_rmcallback,
		client_data
	);

	XtRemoveCallback(
		cb->widget1,
		cb->type,
		(XtCallbackProc) cd_rmcallback,
		client_data
	);

	/* Remove WM_DELETE_WINDOW handler */
	if (cb->widget2 != (Widget) NULL) {
		rm_delw_callback(
			cb->widget2,
			(XtCallbackProc) cb->func,
			(XtPointer) cb->data
		);
	}

	MEM_FREE(cb);
}


/*
 * cd_shell_focus_chg
 *	Focus change callback.  Used to implement keyboard grabs for
 *	hotkey handling.
 */
/*ARGSUSED*/
void
cd_shell_focus_chg(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmAnyCallbackStruct	*p = (XmAnyCallbackStruct *)(void *) call_data;
	Widget			shell = (Widget) client_data;
	static Widget		prev_shell = (Widget) NULL;

	if (p->reason != XmCR_FOCUS || shell == (Widget) NULL)
		return;

	if (prev_shell != NULL) {
		if (shell == prev_shell)
			return;
		else
			hotkey_ungrabkeys(prev_shell);
	}

	if (shell != widgets.toplevel) {
		hotkey_grabkeys(shell);
		prev_shell = shell;
	}
}


/*
 * cd_exit
 *	Shut down the application gracefully.
 */
/*ARGSUSED*/
void
cd_exit(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	/* Shut down CDDB - if there are more processing needed in the
	 * CDDB, they will call us back.
	 */
	s->flags |= STAT_EXIT;
	if (!dbprog_chgsave(s))
		return;

	cd_quit(s);
}


/*
 * cd_tooltip_cancel
 *	Cancel the tooltip window
 */
/*ARGSUSED*/
void
cd_tooltip_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	cd_tooltip_popdown(widgets.tooltip.shell);
}


/**************** ^^ Callback routines ^^ ****************/

/***************** vv Event Handlers vv ******************/


/* Mapping table for main window controls and their label color change
 * function pointer, and associated label pixmaps.
 */
struct wpix_tab {
	Widget	*wptr;
	void	(*set_func)(Widget w, Pixmap px, Pixel color);
	Pixmap	*fpx;
	Pixmap	*hpx;
} wpix_tab[] = {
    { &widgets.main.mode_btn, set_btn_color,
      &pixmaps.main.mode_pixmap, &pixmaps.main.mode_hlpixmap },
    { &widgets.main.eject_btn, set_btn_color,
      &pixmaps.main.eject_pixmap, &pixmaps.main.eject_hlpixmap },
    { &widgets.main.quit_btn, set_btn_color,
      &pixmaps.main.quit_pixmap, &pixmaps.main.quit_hlpixmap },
    { &widgets.main.dbprog_btn, set_btn_color,
      &pixmaps.main.dbprog_pixmap, &pixmaps.main.dbprog_hlpixmap },
    { &widgets.main.help_btn, set_btn_color,
      &pixmaps.main.help_pixmap, &pixmaps.main.help_hlpixmap },
    { &widgets.main.options_btn, set_btn_color,
      &pixmaps.main.options_pixmap, &pixmaps.main.options_hlpixmap },
    { &widgets.main.time_btn, set_btn_color,
      &pixmaps.main.time_pixmap, &pixmaps.main.time_hlpixmap },
    { &widgets.main.ab_btn, set_btn_color,
      &pixmaps.main.ab_pixmap, &pixmaps.main.ab_hlpixmap },
    { &widgets.main.sample_btn, set_btn_color,
      &pixmaps.main.sample_pixmap, &pixmaps.main.sample_hlpixmap },
    { &widgets.main.keypad_btn, set_btn_color,
      &pixmaps.main.keypad_pixmap, &pixmaps.main.keypad_hlpixmap },
    { &widgets.main.level_scale, set_scale_color,
      NULL, NULL },
    { &widgets.main.playpause_btn, set_btn_color,
      &pixmaps.main.playpause_pixmap, &pixmaps.main.playpause_hlpixmap },
    { &widgets.main.stop_btn, set_btn_color,
      &pixmaps.main.stop_pixmap, &pixmaps.main.stop_hlpixmap },
    { &widgets.main.prevdisc_btn, set_btn_color,
      &pixmaps.main.prevdisc_pixmap, &pixmaps.main.prevdisc_hlpixmap },
    { &widgets.main.nextdisc_btn, set_btn_color,
      &pixmaps.main.nextdisc_pixmap, &pixmaps.main.nextdisc_hlpixmap },
    { &widgets.main.prevtrk_btn, set_btn_color,
      &pixmaps.main.prevtrk_pixmap, &pixmaps.main.prevtrk_hlpixmap },
    { &widgets.main.nexttrk_btn, set_btn_color,
      &pixmaps.main.nexttrk_pixmap, &pixmaps.main.nexttrk_hlpixmap },
    { &widgets.main.previdx_btn, set_btn_color,
      &pixmaps.main.previdx_pixmap, &pixmaps.main.previdx_hlpixmap },
    { &widgets.main.nextidx_btn, set_btn_color,
      &pixmaps.main.nextidx_pixmap, &pixmaps.main.nextidx_hlpixmap },
    { &widgets.main.rew_btn, set_btn_color,
      &pixmaps.main.rew_pixmap, &pixmaps.main.rew_hlpixmap },
    { &widgets.main.ff_btn, set_btn_color,
      &pixmaps.main.ff_pixmap, &pixmaps.main.ff_hlpixmap },
    { NULL, NULL, 0, 0 }
};


/*
 * cd_focus_chg
 *	Widget keyboard focus change event handler.  Used to change
 *	the main window controls' label color.
 */
/*ARGSUSED*/
void
cd_focus_chg(Widget w, XtPointer client_data, XEvent *ev)
{
	int		i;
	unsigned char	focuspolicy;		
	static bool_t	first = TRUE;
	static int	count = 0;
	static Pixel	fg,
			hl;

	if (!app_data.main_showfocus)
		return;

	if (first) {
		first = FALSE;

		XtVaGetValues(
			widgets.toplevel,
			XmNkeyboardFocusPolicy,
			&focuspolicy,
			NULL
		);
		if (focuspolicy != XmEXPLICIT) {
			app_data.main_showfocus = FALSE;
			return;
		}

		XtVaGetValues(w, XmNforeground, &fg, NULL);
		XtVaGetValues(w, XmNhighlightColor, &hl, NULL);
	}

	if (ev->xfocus.mode != NotifyNormal ||
	    ev->xfocus.detail != NotifyAncestor)
		return;

	if (ev->type == FocusOut) {
		if (count <= 0)
			return;

		/* Restore original foreground pixmap */
		for (i = 0; wpix_tab[i].set_func != NULL; i++) {
			if (w == *(wpix_tab[i].wptr)) {
				wpix_tab[i].set_func(w,
					(wpix_tab[i].fpx == NULL) ?
					    (Pixmap) 0 : *(wpix_tab[i].fpx),
					fg
				);
				break;
			}
		}
		count--;
	}
	else if (ev->type == FocusIn) {
		if (count >= 1)
			return;

		/* Set new highlighted foreground pixmap */
		for (i = 0; wpix_tab[i].set_func != NULL; i++) {
			if (w == *(wpix_tab[i].wptr)) {
				wpix_tab[i].set_func(w,
					(wpix_tab[i].fpx == NULL) ?
					    (Pixmap) 0 : *(wpix_tab[i].hpx),
					hl
				);
				break;
			}
		}
		count++;
	}
}

/*
 * cd_xing_chg
 *	Widget enter/leave crossing event handler.  Used to manage
 *	pop-up tool-tips.
 */
/*ARGSUSED*/
void
cd_xing_chg(Widget w, XtPointer client_data, XEvent *ev)
{
	if (!app_data.tooltip_enable ||
	    ev->xcrossing.mode != NotifyNormal ||
	    ev->xcrossing.detail == NotifyInferior)
		return;

	if (ev->type == EnterNotify) {
		if (skip_next_tooltip) {
			skip_next_tooltip = FALSE;
			return;
		}

		if (tooltip1_id < 0) {
			tooltip1_id = cd_timeout(
				app_data.tooltip_delay,
				cd_tooltip_popup,
				(byte_t *) w
			);
		}
	}
	else if (ev->type == LeaveNotify) {
		cd_tooltip_popdown(widgets.tooltip.shell);
	}
}


/***************** ^^ Event Handlers ^^ ******************/

