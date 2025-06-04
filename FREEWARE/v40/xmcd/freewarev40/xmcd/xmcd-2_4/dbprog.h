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
#ifndef __DBPROG_H__
#define __DBPROG_H__

#ifndef LINT
static char *_dbprog_h_ident_ = "@(#)dbprog.h	6.24 98/10/09";
#endif


#define TRKLIST_FMT	" %02d  %02u:%02u  %s%s "
#define UNDEF_STR	"??"
#define PGM_SEPCHAR	','
#define EXTT_FLGSTR	"*"
#define TIME_TOTAL	1
#define TIME_TRACK	2
#define DIRSEL_SAVE	1
#define DIRSEL_LINK	2

#define WWWWARP_1	0
#define WWWWARP_2	1
#define WWWWARP_3	2
#define WWWWARP_SRCHENG	3


/* Public functions */
extern void	dbprog_curfileupd(void);
extern void	dbprog_curtrkupd(curstat_t *);
extern void	dbprog_progclear(curstat_t *);
extern void	dbprog_dbclear(curstat_t *, bool_t);
extern void	dbprog_dbget(curstat_t *);
extern void	dbprog_init(curstat_t *);
extern bool_t	dbprog_chgsave(curstat_t *);
extern char	*dbprog_curdtitle(curstat_t *);
extern char	*dbprog_curttitle(curstat_t *);
extern bool_t	dbprog_pgm_parse(curstat_t *);

/* Callback functions */
extern void	dbprog_popup(Widget, XtPointer, XtPointer);
extern void	dbprog_rmtdsbl(Widget, XtPointer, XtPointer);
extern void	dbprog_txtline_verify(Widget, XtPointer, XtPointer);
extern void	dbprog_dtitle_new(Widget, XtPointer, XtPointer);
extern void	dbprog_trklist_play(Widget, XtPointer, XtPointer);
extern void	dbprog_trklist_select(Widget, XtPointer, XtPointer);
extern void	dbprog_ttitle_focuschg(Widget, XtPointer, XtPointer);
extern void	dbprog_ttitle_new(Widget, XtPointer, XtPointer);
extern void	dbprog_pgmseq_verify(Widget, XtPointer, XtPointer);
extern void	dbprog_pgmseq_txtchg(Widget, XtPointer, XtPointer);
extern void	dbprog_addpgm(Widget, XtPointer, XtPointer);
extern void	dbprog_clrpgm(Widget, XtPointer, XtPointer);
extern void	dbprog_send(Widget, XtPointer, XtPointer);
extern void	dbprog_save(Widget, XtPointer, XtPointer);
extern void	dbprog_load(Widget, XtPointer, XtPointer);
extern void	dbprog_link(Widget, XtPointer, XtPointer);
extern void	dbprog_cancel(Widget, XtPointer, XtPointer);
extern void	dbprog_do_clear(Widget, XtPointer, XtPointer);
extern void	dbprog_timedpy(Widget, XtPointer, XtPointer);
extern void	dbprog_extd(Widget, XtPointer, XtPointer);
extern void	dbprog_extt(Widget, XtPointer, XtPointer);
extern void	dbprog_set_changed(Widget, XtPointer, XtPointer);
extern void	dbprog_extd_ok(Widget, XtPointer, XtPointer);
extern void	dbprog_extd_clear(Widget, XtPointer, XtPointer);
extern void	dbprog_extd_cancel(Widget, XtPointer, XtPointer);
extern void	dbprog_extt_prev(Widget, XtPointer, XtPointer);
extern void	dbprog_extt_next(Widget, XtPointer, XtPointer);
extern void	dbprog_extt_autotrk(Widget, XtPointer, XtPointer);
extern void	dbprog_extt_ok(Widget, XtPointer, XtPointer);
extern void	dbprog_extt_clear(Widget, XtPointer, XtPointer);
extern void	dbprog_extt_cancel(Widget, XtPointer, XtPointer);
extern void	dbprog_dirsel_select(Widget, XtPointer, XtPointer);
extern void	dbprog_dirsel_ok(Widget, XtPointer, XtPointer);
extern void	dbprog_dirsel_cancel(Widget, XtPointer, XtPointer);
extern void	dbprog_linksel_select(Widget, XtPointer, XtPointer);
extern void	dbprog_linksel_ok(Widget, XtPointer, XtPointer);
extern void	dbprog_linksel_cancel(Widget, XtPointer, XtPointer);
extern void	dbprog_rmtmatch_yes(Widget, XtPointer, XtPointer);
extern void	dbprog_rmtmatch_no(Widget, XtPointer, XtPointer);
extern void	dbprog_rmtmatch_stop(Widget, XtPointer, XtPointer);
extern void	dbprog_auth_retry(Widget, XtPointer, XtPointer);
extern void	dbprog_auth_name(Widget, XtPointer, XtPointer);
extern void	dbprog_auth_password_vfy(Widget, XtPointer, XtPointer);
extern void	dbprog_auth_ok(Widget, XtPointer, XtPointer);
extern void	dbprog_auth_cancel(Widget, XtPointer, XtPointer);
extern void	dbprog_wwwwarp_select(Widget, XtPointer, XtPointer);
extern void	dbprog_wwwwarp_cancel(Widget, XtPointer, XtPointer);
extern void	dbprog_wwwwarp_go(Widget, XtPointer, XtPointer);
extern void	dbprog_wwwwarp(Widget, XtPointer, XtPointer);
extern void	dbprog_srcheng_sel(Widget, XtPointer, XtPointer);

#endif	/* __DBPROG_H__ */
