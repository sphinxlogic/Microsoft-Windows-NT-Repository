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
#ifndef __CDFUNC_H__
#define __CDFUNC_H__

#ifndef LINT
static char *_cdfunc_h_ident_ = "@(#)cdfunc.h	6.21 98/09/27";
#endif


/* Fatal error, warning and information message macros */
#define CD_FATAL(msg)		cd_fatal_popup(app_data.str_fatal, (msg))
#define CD_WARNING(msg)		cd_warning_popup(app_data.str_warning, (msg))
#define CD_INFO(msg)		cd_info_popup(app_data.str_info, (msg))
#define CD_INFO_AUTO(msg)	cd_info_popup_auto(app_data.str_info, (msg))

/* Maximum main window disc title string length */
#define TITLEIND_LEN	80

/* Widget map bits for cd_busycurs() */
#define CURS_MAIN	0x0001
#define CURS_KEYPAD	0x0002
#define CURS_OPTIONS	0x0004
#define CURS_DBPROG	0x0008
#define CURS_WWWWARP	0x0010
#define CURS_DBEXTD	0x0020
#define CURS_DBEXTT	0x0040
#define CURS_DIRSEL	0x0080
#define CURS_LINKSEL	0x0100
#define CURS_HELP	0x0200
#define CURS_ALL	( CURS_MAIN   | CURS_KEYPAD  | CURS_OPTIONS |	\
			  CURS_DBPROG | CURS_WWWWARP | CURS_DBEXTD  |	\
			  CURS_DBEXTT |	CURS_DIRSEL  | CURS_LINKSEL |	\
			  CURS_HELP )

/* Keypad modes */
#define KPMODE_DISC	0
#define KPMODE_TRACK	1


/* Public functions */
extern byte_t	curtrk_type(curstat_t *);
extern void	dpy_disc(curstat_t *);
extern void	dpy_track(curstat_t *);
extern void	dpy_index(curstat_t *);
extern void	dpy_time(curstat_t *, bool_t);
extern void	dpy_dtitle(curstat_t *);
extern void	dpy_ttitle(curstat_t *);
extern void	dpy_rptcnt(curstat_t *);
extern void	dpy_dbmode(curstat_t *, bool_t);
extern void	dpy_progmode(curstat_t *);
extern void	dpy_timemode(curstat_t *);
extern void	dpy_playmode(curstat_t *, bool_t);
extern void	dpy_all(curstat_t *);

extern void	set_lock_btn(bool_t);
extern void	set_repeat_btn(bool_t);
extern void	set_shuffle_btn(bool_t);
extern void	set_vol_slider(int);
extern void	set_warp_slider(int, bool_t);
extern void	set_bal_slider(int);
extern void	set_btn_lbltype(byte_t);

extern int	scale_warp(curstat_t *, int, int);
extern int	unscale_warp(curstat_t *, int, int);

extern long	cd_timeout(word32_t, void (*)(), byte_t *);
extern void	cd_untimeout(long);
extern void	cd_beep(void);
extern void	cd_info_popup(char *, char *);
extern void	cd_info_popup_auto(char *, char *);
extern void	cd_warning_popup(char *, char *);
extern void	cd_fatal_popup(char *, char *);
extern void	cd_confirm_popup(char *, char *, XtCallbackProc, XtPointer,
			XtCallbackProc, XtPointer);

extern void	cd_playprog(curstat_t *);
extern void	cd_init(curstat_t *);
extern void	cd_start(curstat_t *);
extern void	cd_icon(curstat_t *, bool_t);
extern void	cd_halt(curstat_t *);
extern void	cd_quit(curstat_t *);

extern void	cd_busycurs(bool_t, int);
extern void	onsig(int);


/* Callback functions */
extern void	cd_checkbox(Widget, XtPointer, XtPointer);
extern void	cd_mode(Widget, XtPointer, XtPointer);
extern void	cd_load_eject(Widget, XtPointer, XtPointer);
extern void	cd_quit_btn(Widget, XtPointer, XtPointer);
extern void	cd_dbprog(Widget, XtPointer, XtPointer);
extern void	cd_time(Widget, XtPointer, XtPointer);
extern void	cd_ab(Widget, XtPointer, XtPointer);
extern void	cd_sample(Widget, XtPointer, XtPointer);
extern void	cd_level(Widget, XtPointer, XtPointer);
extern void	cd_play_pause(Widget, XtPointer, XtPointer);
extern void	cd_stop(Widget, XtPointer, XtPointer);
extern void	cd_chgdisc(Widget, XtPointer, XtPointer);
extern void	cd_prevtrk(Widget, XtPointer, XtPointer);
extern void	cd_nexttrk(Widget, XtPointer, XtPointer);
extern void	cd_previdx(Widget, XtPointer, XtPointer);
extern void	cd_nextidx(Widget, XtPointer, XtPointer);
extern void	cd_rew(Widget, XtPointer, XtPointer);
extern void	cd_ff(Widget, XtPointer, XtPointer);
extern void	cd_keypad_popup(Widget, XtPointer, XtPointer);
extern void	cd_keypad_popdown(Widget, XtPointer, XtPointer);
extern void	cd_keypad_mode(Widget, XtPointer, XtPointer);
extern void	cd_keypad_num(Widget, XtPointer, XtPointer);
extern void	cd_keypad_clear(Widget, XtPointer, XtPointer);
extern void	cd_keypad_dsbl_modes_yes(Widget, XtPointer, XtPointer);
extern void	cd_keypad_dsbl_modes_no(Widget, XtPointer, XtPointer);
extern void	cd_keypad_enter(Widget, XtPointer, XtPointer);
extern void	cd_warp(Widget, XtPointer, XtPointer);
extern void	cd_options_popup(Widget, XtPointer, XtPointer);
extern void	cd_options_popdown(Widget, XtPointer, XtPointer);
extern void	cd_options_reset(Widget, XtPointer, XtPointer);
extern void	cd_options_save(Widget, XtPointer, XtPointer);
extern void	cd_options(Widget, XtPointer, XtPointer);
extern void	cd_balance(Widget, XtPointer, XtPointer);
extern void	cd_balance_center(Widget, XtPointer, XtPointer);
extern void	cd_about(Widget, XtPointer, XtPointer);
extern void	cd_help_popup(Widget, XtPointer, XtPointer);
extern void	cd_help_popdown(Widget, XtPointer, XtPointer);
extern void	cd_warning_popdown(Widget, XtPointer, XtPointer);
extern void	cd_fatal_popdown(Widget, XtPointer, XtPointer);
extern void	cd_rmcallback(Widget, XtPointer, XtPointer);
extern void	cd_shell_focus_chg(Widget, XtPointer, XtPointer);
extern void	cd_exit(Widget, XtPointer, XtPointer);
extern void	cd_tooltip_cancel(Widget, XtPointer, XtPointer);

/* Event handlers */
extern void	cd_focus_chg(Widget, XtPointer, XEvent *);
extern void	cd_xing_chg(Widget, XtPointer, XEvent *);

#endif	/* __CDFUNC_H__ */

