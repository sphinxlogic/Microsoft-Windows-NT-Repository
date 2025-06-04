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
static char *_dbprog_c_ident_ = "@(#)dbprog.c	6.154 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/patchlevel.h"
#include "common_d/util.h"
#include "xmcd_d/xmcd.h"
#include "xmcd_d/widget.h"
#include "xmcd_d/cdfunc.h"
#include "xmcd_d/dbprog.h"
#include "libdi_d/libdi.h"
#include "cddb_d/cddb.h"


extern widgets_t	widgets;
extern appdata_t	app_data;
extern char		*cdisplay;
extern FILE		*errfp;
extern srcheng_ent_t	*srcheng_list_head;

STATIC char		*auth_passwd,		/* Proxy-auth password */
			timemode;		/* Time display mode flag */
STATIC int		sel_pos = -1,		/* Track list select position */
			ind_pos = -1,		/* Track list highlight pos */
			linksel_pos = -1,	/* Link select position */
			extt_pos = -1,		/* Ext track info position */
			dirsel_mode = DIRSEL_SAVE,
						/* Directory selector mode */
			website_idx;		/* Web site select */
STATIC bool_t		title_edited,		/* Track title edited flag */
			extt_setup,		/* Ext track info setup */
			wwwwarp_manage,		/* Whether to manage wwwWarp */
			extd_manage,		/* Whether to manage extd */
			extt_manage,		/* Whether to manage extt */
			dbmatch_mode,		/* CDDB match query mode */
			auth_initted,		/* User entered auth info */
			srcheng_list_enabled,	/* srch eng list enabled */
			auto_trk;		/* Extt auto-track */
STATIC cddb_incore_t	cur_db;			/* Database entry of CD */
STATIC srcheng_ent_t	*cur_srcheng;		/* Current search engine */
STATIC XmString		xs_extd_lbl,		/* Extd info lbl */
			xs_extd_lblx,		/* Extd info lbl asterisk */
			xs_extt_lbl,		/* Extt info lbl */
			xs_extt_lblx;		/* Extt info lbl asterisk */



/***********************
 *  internal routines  *
 ***********************/


/*
 * dbprog_dpytottime
 *	Display the disc total time in the total time indicator
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_dpytottime(curstat_t *s)
{
	XmString	xs;
	char		str[STR_BUF_SZ];
	static char	prev[STR_BUF_SZ];

	if (s->mode == MOD_BUSY || s->mode == MOD_NODISC)
		(void) strcpy(str, "--:--");
	else
		(void) sprintf(str, "%02d:%02d", s->tot_min, s->tot_sec);

	if (strcmp(str, prev) == 0)
		return;

	xs = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);

	XtVaSetValues(
		widgets.dbprog.tottime_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strcpy(prev, str);
}


/*
 * dbprog_dpyid
 *	Display the disc ID in the Disc ID Indicator.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_dpyid(void)
{
	XmString	xs;
	char		str[FILE_BASE_SZ + 16];
	static char	prev[FILE_BASE_SZ + 16];

	if (cur_db.discid != 0)
		(void) sprintf(str, "%s\n%08x", cur_db.category, cur_db.discid);
	else
		(void) sprintf(str, "\n   --   ");

	if (strcmp(str, prev) == 0)
		/* No change */
		return;

	xs = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);

	XtVaSetValues(
		widgets.dbprog.discid_ind,
		XmNlabelString,
		xs,
		NULL
	);

	XmStringFree(xs);

	(void) strcpy(prev, str);
}


/*
 * dbprog_wwwwarp_sel_cfg
 *	Set the sensitivity of the wwwWarp search selection button
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 * 
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_wwwwarp_sel_cfg(curstat_t *s)
{
	if (!srcheng_list_enabled)
		return;

	switch (s->cddb) {
	case CDDB_LOCAL:
	case CDDB_REMOTE_CDDBP:
	case CDDB_REMOTE_HTTP:
		if (cur_db.dtitle != NULL && cur_db.dtitle[0] != '\0') {
			/* There is dtitle information */
			XtSetSensitive(widgets.wwwwarp.srchweb_btn, True);
			break;
		}
		/*FALLTHROUGH*/
	default:
		if (website_idx == WWWWARP_SRCHENG) {
			website_idx = WWWWARP_1;
			XmToggleButtonSetState(
				widgets.wwwwarp.srchweb_btn, False, False
			);
			XmToggleButtonSetState(
				widgets.wwwwarp.site1_btn, True, False
			);
			XtVaSetValues(widgets.wwwwarp.sel_radbox,
				XmNmenuHistory, widgets.wwwwarp.site1_btn,
				NULL
			);
			DBGPRN(errfp,
				"wwwWarp: Reset to \"%s\"\n",
				XtName(widgets.wwwwarp.site1_btn));
		}
		XtSetSensitive(widgets.wwwwarp.srchweb_btn, False);
		XtSetSensitive(widgets.wwwwarp.srchsite_opt, False);
		break;
	}
}


/*
 * dbprog_wwwwarp_site
 *	Invoke the web browser to the selected web site.
 *
 * Args:
 *	url - The destination URL string
 *
 * Return:
 *	Error code
 */
STATIC void
dbprog_wwwwarp_site(char *url)
{
	int		a,
			b,
			n,
			ret;
	char		*r,
			*s,
			*env,
			*cmd;
	static bool_t	first = TRUE;

	/* If user specified the -display option when starting xmcd,
	 * force this display specification into the environment so
	 * that we will manage the browser on the same display as
	 * xmcd.
	 */
	if (first && cdisplay != NULL) {
		first = FALSE;
		env = (char *) MEM_ALLOC(
			"wwwwarp_site_env",
			strlen(cdisplay) + 12
		);
		if (env == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		(void) sprintf(env, "DISPLAY=%s", cdisplay);
		if (putenv(env) != 0) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		/* Do not MEM_FREE(env)! */
	}

	a = strlen(app_data.browser_rmt);
	b = strlen(app_data.browser_dir);
	n = ((a > b) ? a : b) + strlen(url) + 12;

	if ((cmd = (char *) MEM_ALLOC("wwwwarp_site_cmd", n)) == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	/* Set up remote control of web browser */
	r = cmd;
	for (s = app_data.browser_rmt; s != NULL && *s != '\0'; s++) {
		switch (*s) {
		case '%':
			switch (*(++s)) {
			case 'U':
				strcpy(r, url);
				r += strlen(url);
				break;
			case '%':
				*r++ = '%';
				break;
			default:
				*r++ = '%';
				*r++ = *s;
				break;
			}
			break;
		default:
			*r++ = *s;
			break;
		}
	}
	*r = '\0';

	/* Attempt to remote control an existing browser running on
	 * this display.
	 */
	if ((ret = util_runcmd(cmd, event_loop, 0)) == 0) {
		/* Success */
		MEM_FREE(cmd);
		return;
	}

	if (ret == EXE_ERR) {
		/* There is a configuration error: cannot invoke browser */
		CD_INFO(app_data.str_browser_fail);
		MEM_FREE(cmd);
		return;
	}

	/*
	 * Remote control of running browser failed.
	 * Try starting a new session.
	 */

	/* Set up direct browser command */
	r = cmd;
	for (s = app_data.browser_dir; s != NULL && *s != '\0'; s++) {
		switch (*s) {
		case '%':
			switch (*(++s)) {
			case 'U':
				strcpy(r, url);
				r += strlen(url);
				break;
			case '%':
				*r++ = '%';
				break;
			default:
				*r++ = '%';
				*r++ = *s;
				break;
			}
			break;
		default:
			*r++ = *s;
			break;
		}
	}
	*r = '\0';

	CD_INFO_AUTO(app_data.str_browser_start);

	/* Do the direct browser invocation */
	if ((ret = util_runcmd(cmd, event_loop, 0)) != 0)
		CD_INFO(app_data.str_browser_fail);

	MEM_FREE(cmd);
}


/*
 * dbprog_txtreduce
 *	Transform a text string into a CGI search string
 *
 * Args:
 *	text - Input text string
 *      reduce - Whether to reduce text (remove punctuations,
 *		 exclude words  from the excludeWords list).
 *
 * Return:
 *	Output string.  The storage is internally allocated per call,
 *	and should be freed by the caller with MEM_FREE() after the
 *	buffer is no longer needed.  If there is no sufficient memory
 *	to allocate the buffer, NULL is returned.
 */
STATIC char *
dbprog_txtreduce(char *text, bool_t reduce)
{
	char	*p,
		*q;

	if (reduce) {
		if ((p = util_text_reduce(text)) == NULL)
		return NULL;
	}
	else
		p = text;

	if ((q = util_cgi_xlate(p)) == NULL)
		return NULL;

	if (reduce)
		MEM_FREE(p);

	return (q);
}


/*
 * dbprog_wwwwarp_srcheng
 *	Invoke the web browser to the selected search engine.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_wwwwarp_srcheng(curstat_t *s)
{
	int		n,
			trkpos;
	char		*p,
			*q,
			*r,
			*a,
			*url;
	char		sav;

	n = strlen(cur_srcheng->action) +
	    ((cur_srcheng->acnt + cur_srcheng->dcnt) * strlen(cur_db.dtitle));

	if (sel_pos > 0)
		/* A track is selected: use it for the track title */
		trkpos = sel_pos - 1;
	else
		/* Use current playing track for the track title, if playing */
		trkpos = di_curtrk_pos(s);

	if (trkpos < 0 || cur_db.trklist[trkpos] == NULL) {
		trkpos = -1;
		n += (cur_srcheng->tcnt * strlen(cur_db.dtitle));
	}
	else
		n += (cur_srcheng->tcnt * strlen(cur_db.trklist[trkpos]));

	if ((url = (char *) MEM_ALLOC("wwwwarp_srcheng_url", n)) == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	/* Set up URL */
	r = url;
	for (a = cur_srcheng->action; *a != '\0'; a++) {
		switch (*a) {
		case '%':
			switch (*(++a)) {
			case 'A':
			case 'a':
				sav = '\0';

				/* Translate disk artist into a
				 * keyword string in CGI form
				 */
				p = cur_db.dtitle;
				while ((p = strchr(p, '/')) != NULL)
				{
					if (p > cur_db.dtitle &&
					    *(p-1) == ' ' && *(p+1) == ' ')
						break;
					else
						p++;
				}
				if (p != NULL) {
					/* Check for '/' at beginning of
					 * string.
					 */
					n = 0;
					for (q = cur_db.dtitle; q < p; q++) {
						if (!isspace(*q)) {
							n++;
							break;
						}
					}
					if (n > 0) {
						--p;
						sav = *p;
						*p = '\0';
					}
				}

				q = dbprog_txtreduce(
					cur_db.dtitle,
					(bool_t) (*a == 'a')
				);
				if (q == NULL) {
					CD_FATAL(app_data.str_nomemory);
					return;
				}

				if (sav != '\0')
					*p = sav;

				strcpy(r, q);
				r += strlen(q);
				MEM_FREE(q);
				break;

			case 'T':
			case 't':
				/* Translate track title into a
				 * keyword string in CGI form
				 */
				if (trkpos >= 0) {
					q = dbprog_txtreduce(
						cur_db.trklist[trkpos],
						(bool_t) (*a == 't')
					);
					if (q == NULL) {
					    CD_FATAL(app_data.str_nomemory);
					    return;
					}
					strcpy(r, q);
					r += strlen(q);
					MEM_FREE(q);
					break;
				}
				/*FALLTHROUGH*/

			case 'D':
			case 'd':
				/* Translate disc title into a
				 * keyword string in CGI form
				 */
				p = cur_db.dtitle;
				while ((p = strchr(p, '/')) != NULL)
				{
					if (p > cur_db.dtitle &&
					    *(p-1) == ' ' && *(p+1) == ' ')
						break;
					else
						p++;
				}
				if (p == NULL)
					/* '/' not found, just use the
					 * whole string.
					 */
					p = cur_db.dtitle;
				else {
					q = ++p;
					while (*q != '\0' && isspace(*q))
						q++;

					if (*q == '\0') {
						/* '/' at end-of-string: just
						 * use the whole string
						 */
						p = cur_db.dtitle;
					}
				}

				q = dbprog_txtreduce(p, (bool_t) (*a == 'd'));
				if (q == NULL) {
					CD_FATAL(app_data.str_nomemory);
					return;
				}

				strcpy(r, q);
				r += strlen(q);
				MEM_FREE(q);
				break;

			case 'B':
			case 'b':
				/* Translate artist and disc title into a
				 * keyword string in CGI form
				 */
				q = dbprog_txtreduce(
					cur_db.dtitle,
					(bool_t) (*a == 'b')
				);
				if (q == NULL) {
					CD_FATAL(app_data.str_nomemory);
					return;
				}

				strcpy(r, q);
				r += strlen(q);
				MEM_FREE(q);
				break;

			case '%':
				*r++ = '%';
				break;

			default:
				*r++ = '%';
				*r++ = *a;
				break;
			}
			break;
		default:
			*r++ = *a;
			break;
		}
	}
	*r = '\0';

	/* Go to the URL */
	dbprog_wwwwarp_site(url);

	MEM_FREE(url);
}


/*
 * dbprog_listupd_ent
 *	Update a track entry in the track list.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	pos - Track position to update.
 *	title - Track title string.
 *	newent - Whether this is a new entry or a replacement entry.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_listupd_ent(curstat_t *s, int pos, char *title, bool_t newent)
{
	int		n,
			secs,
			min,
			sec;
	char		*str;
	XmString	xs;

	if (s->trkinfo[pos].trkno < 0)
		return;

	if (timemode == TIME_TOTAL) {
		min = s->trkinfo[pos].min;
		sec = s->trkinfo[pos].sec;
	}
	else {
		secs = ((s->trkinfo[pos+1].min * 60 + s->trkinfo[pos+1].sec) - 
			(s->trkinfo[pos].min * 60 + s->trkinfo[pos].sec));
		min = (byte_t) (secs / 60);
		sec = (byte_t) (secs % 60);
	}

	n = strlen(title) + strlen(TRKLIST_FMT) + 1;
	if ((str = (char *) MEM_ALLOC("listupd_ent_str", n)) == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	(void) sprintf(str, TRKLIST_FMT,
		       s->trkinfo[pos].trkno,
		       min, sec, title,
		       (cur_db.extt[pos] != NULL) ? EXTT_FLGSTR : "");

	if (s->mode != MOD_BUSY && s->mode != MOD_NODISC &&
	    s->cur_trk >= 0 && di_curtrk_pos(s) == pos)
		xs = XmStringCreate(str, CHSET2);
	else
		xs = XmStringCreate(str, CHSET1);

	if (newent)
		XmListAddItemUnselected(widgets.dbprog.trk_list, xs, pos+1);
	else
		XmListReplaceItemsPos(widgets.dbprog.trk_list, &xs, 1, pos+1);

	XmStringFree(xs);
	MEM_FREE(str);
}


/*
 * dbprog_listupd_all
 *	Update the track list display to reflect the contents of
 *	the trkinfo table in the curstat_t structure.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	newent - Whether this is a new entry or a replacement entry.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_listupd_all(curstat_t *s, bool_t newent)
{
	int	i;

	if (newent)
		XmListDeleteAllItems(widgets.dbprog.trk_list);

	for (i = 0; i < (int) s->tot_trks; i++) {
		/* Update track list entry */
		dbprog_listupd_ent(
			s, i,
			(cur_db.trklist[i] != NULL) ?
				cur_db.trklist[i] : UNDEF_STR,
			newent
		);
	}

	/* If this item is previously selected, re-select it */
	if (sel_pos > 0)
		XmListSelectPos(widgets.dbprog.trk_list, sel_pos, False);
	else if (ind_pos > 0)
		XmListSelectPos(widgets.dbprog.trk_list, ind_pos, False);
}


/*
 * dbprog_trklist_autoscroll
 *	Scroll track list if necessary to make the specified track
 *	visible.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	pos - The track list position to make visible
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_trklist_autoscroll(curstat_t *s, int pos)
{
	int	n,
		top_pos,
		bottom_pos,
		vis_cnt;

	/* Determine range of tracks visible in current list */
	XtVaGetValues(widgets.dbprog.trk_list,
		XmNtopItemPosition, &top_pos,
		XmNvisibleItemCount, &vis_cnt,
		NULL
	);

	bottom_pos = top_pos + vis_cnt - 1;

	/* Try to keep the desired track near the middle of the visible
	 * portion of the list.
	 */
	if (pos < top_pos) {
		/* Scroll up */
		n = pos - (vis_cnt / 2);
		if (n < 1)
			n = 1;
		XmListSetPos(widgets.dbprog.trk_list, n);
	}
	else if (pos > bottom_pos) {
		/* Scroll down */
		n = pos + (vis_cnt / 2);
		if (n > (int) s->tot_trks)
			n = (int) s->tot_trks;
		XmListSetBottomPos(widgets.dbprog.trk_list, n);
	}
}


/*
 * dbprog_extd_lblupd
 *	Update the extd button label to indicate whether there is
 *	text contained in the extd section.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_extd_lblupd(void)
{
	XmString	xs;
	static bool_t	state = FALSE;

	if (cur_db.extd != NULL) {
		if (state)
			return;		/* no change */
		xs = xs_extd_lblx;
	}
	else {
		if (!state)
			return;		/* no change */
		xs = xs_extd_lbl;
	}

	XtVaSetValues(widgets.dbprog.extd_lbl,
		XmNlabelString, xs,
		NULL
	);

	state = (bool_t) (cur_db.extd != NULL);
}


/*
 * dbprog_extt_lblupd
 *	Update the extt button label to indicate whether there is
 *	text contained in the extt section.
 *
 * Args:
 *	extt - The track extended info string
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_extt_lblupd(char *extt)
{
	XmString	xs;
	static bool_t	state = FALSE;

	if (extt != NULL) {
		if (state)
			return;		/* no change */
		xs = xs_extt_lblx;
	}
	else {
		if (!state)
			return;		/* no change */
		xs = xs_extt_lbl;
	}

	XtVaSetValues(widgets.dbprog.extt_lbl,
		XmNlabelString, xs,
		NULL
	);

	state = (bool_t) (extt != NULL);
}


/*
 * dbprog_set_extd_title
 *	Set the disc title label on the extended disc info window
 *
 * Args:
 *	discno - The disc number
 *	str - The label string
 *
 * Return:
 *	Nothing;
 */
STATIC void
dbprog_set_extd_title(int discno, char *str)
{
	XmString	xs;
	char		buf[16];

	(void) sprintf(buf, "Disc %d", discno);

	xs = XmStringCreateSimple(buf);
	XtVaSetValues(widgets.dbextd.discno_lbl,
		XmNlabelString, xs,
		NULL
	);
	XmStringFree(xs);

	xs = XmStringCreateLtoR(
		str == NULL ? "<Untitled>" : str,
		XmSTRING_DEFAULT_CHARSET
	);
	XtVaSetValues(widgets.dbextd.disc_lbl,
		XmNlabelString, xs,
		NULL
	);
	XmStringFree(xs);
}


/*
 * dbprog_set_extt_title
 *	Set the track title label on the extended track info window
 *
 * Args:
 *	trkno - The track number
 *	str - The label string
 *
 * Return:
 *	Nothing;
 */
STATIC void
dbprog_set_extt_title(int trkno, char *str)
{
	XmString	xs;
	char		buf[16];

	(void) sprintf(buf, "Track %d", trkno);

	xs = XmStringCreateSimple(buf);
	XtVaSetValues(widgets.dbextt.trkno_lbl,
		XmNlabelString, xs,
		NULL
	);
	XmStringFree(xs);

	xs = XmStringCreateLtoR(
		str == NULL ? "<Untitled>" : str,
		XmSTRING_DEFAULT_CHARSET
	);
	XtVaSetValues(widgets.dbextt.trk_lbl,
		XmNlabelString, xs,
		NULL
	);
	XmStringFree(xs);
}


/*
 * dbprog_extt_autotrk_upd
 *	If extended track information window auto-track is enabled,
 *	display the new extended track info.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	new_pos - The new track list position to go to
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_extt_autotrk_upd(curstat_t *s, int new_pos)
{
	if (auto_trk && new_pos > 0 && sel_pos != new_pos &&
	    XtIsManaged(widgets.dbextt.form)) {
		/* Make extt go to the new track */
		if (sel_pos > 0) {
			XmListDeselectPos(widgets.dbprog.trk_list, sel_pos);
			sel_pos = -1;
		}
		XmListSelectPos(widgets.dbprog.trk_list, new_pos, True);

		/* Scroll track list if necessary */
		dbprog_trklist_autoscroll(s, new_pos);
	}
}


/*
 * dbprog_structupd
 *	Update the state of the various widgets fields in the
 *	database/program window to match that of the cur_db structure.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_structupd(curstat_t *s)
{
	/* Total time */
	dbprog_dpytottime(s);

	/* Disc title */
	if (cur_db.dtitle != NULL) {
		XmTextSetString(widgets.dbprog.dtitle_txt, cur_db.dtitle);
		XmTextSetInsertionPosition(
			widgets.dbprog.dtitle_txt,
			strlen(cur_db.dtitle)
		);
	}
	else {
		XmTextSetString(widgets.dbprog.dtitle_txt, UNDEF_STR);
		XmTextSetInsertionPosition(widgets.dbprog.dtitle_txt, 2);
	}


	/* Disc extended info popup */
	if (cur_db.extd != NULL)
		XmTextSetString(widgets.dbextd.disc_txt, cur_db.extd);
	else
		XmTextSetString(widgets.dbextd.disc_txt, "");

	/* Update extd label */
	dbprog_extd_lblupd();

	/* Track title list */
	sel_pos = -1;
	dbprog_listupd_all(s, TRUE);

	/* Track extended info popup: This is loaded when the user
	 * pops it up.
	 */
	XmTextSetString(widgets.dbextt.trk_txt, "");

	/* Program sequence */
	if (cur_db.playorder != NULL && !s->onetrk_prog) {
		XmTextSetString(widgets.dbprog.pgmseq_txt, cur_db.playorder);
		XmTextSetInsertionPosition(
			widgets.dbprog.pgmseq_txt,
			strlen(cur_db.playorder)
		);
		XtSetSensitive(widgets.dbprog.clrpgm_btn, True);
	}
	else {
		XmTextSetString(widgets.dbprog.pgmseq_txt, "");
		XtSetSensitive(widgets.dbprog.clrpgm_btn, False);
	}

	cur_db.flags &= ~CDDB_CHANGED;
	XtSetSensitive(widgets.dbprog.addpgm_btn, False);
	XtSetSensitive(widgets.dbprog.savedb_btn, False);
	XtSetSensitive(widgets.dbprog.extd_btn, True);
	XtSetSensitive(widgets.dbprog.extt_btn, False);

	/* Update extt label */
	dbprog_extt_lblupd(NULL);

	/* Update display */
	dpy_dbmode(s, FALSE);
}


/*
 * dbprog_extdupd
 *	Update the Extended disc info text field in the cur_db structure
 *	to match the contents shown in the text widget.
 *
 * Args:
 *	Nothing
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_extdupd(void)
{
	char	*cp;

	/* Update in-core database structure */
	if (cur_db.extd != NULL) {
		MEM_FREE(cur_db.extd);
		cur_db.extd = NULL;
	}

	if ((cp = XmTextGetString(widgets.dbextd.disc_txt)) == NULL)
		return;

	if (*cp != '\0') {
		cur_db.extd = (char *) MEM_ALLOC(
			"cur_db.extd",
			strlen(cp) + 1
		);
		if (cur_db.extd == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		strcpy(cur_db.extd, cp);
	}
	XtFree(cp);

	/* Update extd label */
	dbprog_extd_lblupd();

	if (cur_db.flags & CDDB_CHANGED)
		XtSetSensitive(widgets.dbprog.savedb_btn, True);
}


/*
 * dbprog_exttupd
 *	Update the Extended track info text field in the cur_db structure
 *	to match the contents shown in the text widget.
 *
 * Args:
 *	Nothing
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_exttupd(void)
{
	char	*cp;

	if (extt_pos < 0)
		return;

	/* Update in-core database structure */
	if (cur_db.extt[extt_pos] != NULL) {
		MEM_FREE(cur_db.extt[extt_pos]);
		cur_db.extt[extt_pos] = NULL;
	}

	if ((cp = XmTextGetString(widgets.dbextt.trk_txt)) == NULL) {
		extt_pos = -1;
		return;
	}

	if (*cp != '\0') {
		cur_db.extt[extt_pos] = (char *) MEM_ALLOC(
			"cur_db.extt[n]",
			strlen(cp) + 1
		);
		if (cur_db.extt[extt_pos] == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		strcpy(cur_db.extt[extt_pos], cp);
	}
	XtFree(cp);

	/* Update track list entry */
	dbprog_listupd_ent(
		curstat_addr(), extt_pos, 
		(cur_db.trklist[extt_pos] != NULL) ?
			cur_db.trklist[extt_pos] : UNDEF_STR,
		FALSE
	);

	if (sel_pos == (extt_pos+1))
		/* This item is previously selected */
		XmListSelectPos(widgets.dbprog.trk_list, sel_pos, False);

	if (cur_db.flags & CDDB_CHANGED)
		XtSetSensitive(widgets.dbprog.savedb_btn, True);
}


/*
 * dbprog_dbput
 *	Write in-core CD database entry to disc file.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_dbput(curstat_t *s)
{
	cddb_ret_t	ret;
	char		errstr[ERR_BUF_SZ];

	if (cur_db.dbfile == NULL)
		/* Output file undefined */
		return;

	/* Update structures if necessary */
	if (XtIsManaged(widgets.dbextd.form))
		dbprog_extdupd();
	if (XtIsManaged(widgets.dbextt.form))
		dbprog_exttupd();

	/* Change to watch cursor */
	cd_busycurs(TRUE, CURS_ALL);

	/* Save to database */
	if ((ret = cddb_save(&cur_db, s)) != 0) {
		DBGPRN(errfp, "cddb_save: status=%d arg=%d\n",
			CDDB_GET_STAT(ret), CDDB_GET_ARG(ret));

		if (cur_db.dbfile != NULL) {
			MEM_FREE(cur_db.dbfile);
			cur_db.dbfile = NULL;
		}
		cur_db.category[0] = '\0';

		/* Update display */
		dbprog_dpyid();
	}

	/* Change to normal cursor */
	cd_busycurs(FALSE, CURS_ALL);

	switch (CDDB_GET_STAT(ret)) {
	case 0:
		break;

	case FORK_ERR:
		(void) sprintf(errstr, app_data.str_saverr_fork,
			       CDDB_GET_ARG(ret));
		CD_WARNING(errstr);
		return;

	case SETUID_ERR:
		(void) sprintf(errstr, app_data.str_saverr_suid,
			       util_get_ouid(), util_get_ogid());
		CD_WARNING(errstr);
		return;

	case OPEN_ERR:
		(void) sprintf(errstr, app_data.str_saverr_open);
		CD_WARNING(errstr);
		return;

	case CLOSE_ERR:
		(void) sprintf(errstr, app_data.str_saverr_close);
		CD_WARNING(errstr);
		return;

	case KILLED_ERR:
		(void) sprintf(errstr, app_data.str_saverr_killed,
			       CDDB_GET_ARG(ret));
		CD_WARNING(errstr);
		return;

	case WRITE_ERR:
	case MEM_ERR:
	default:
		(void) sprintf(errstr, app_data.str_saverr_write);
		CD_WARNING(errstr);
		return;
	}

	/* Set flag */
	s->cddb = cur_db.type = CDDB_LOCAL;

	/* All edits have been saved, so clear flag */
	cur_db.flags &= ~CDDB_CHANGED;

	/* Configure the wwwWarp search engine options menu */
	dbprog_wwwwarp_sel_cfg(s);

	/* Update display */
	dpy_dbmode(s, FALSE);
	dbprog_dpyid();

	/* Update curfile */
	dbprog_curfileupd();

	XtSetSensitive(widgets.dbprog.send_btn, True);
	XtSetSensitive(widgets.dbprog.linkdb_btn, False);
	XtSetSensitive(widgets.dbprog.savedb_btn, False);
	XmProcessTraversal(widgets.dbprog.cancel_btn, XmTRAVERSE_CURRENT);
}


/*
 * dbprog_dbsend
 *	Send current CD database entry to archive site via e-mail.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
dbprog_dbsend(curstat_t *s)
{
	cddb_ret_t	ret;

	/* Change to the watch cursor */
	cd_busycurs(TRUE, CURS_ALL);

	/* Send the CDDB entry */
	if ((ret = cddb_send(&cur_db, s)) != 0) {
		DBGPRN(errfp, "cddb_send: status=%d arg=%d\n",
			CDDB_GET_STAT(ret), CDDB_GET_ARG(ret));
	}

	/* Change to the normal cursor */
	cd_busycurs(FALSE, CURS_ALL);

	switch (CDDB_GET_STAT(ret)) {
	case 0:
		/* Make the send button insensitive */
		XtSetSensitive(widgets.dbprog.send_btn, False);
		break;
	case INCMPL_ERR:
		/* Make the send button insensitive */
		XtSetSensitive(widgets.dbprog.send_btn, False);
		CD_WARNING(app_data.str_dbincmplerr);
		break;
	case CMD_ERR:
	case MEM_ERR:
	default:
		CD_WARNING(app_data.str_mailerr);
		break;
	}
}


/*
 * dbprog_dblink
 *	Pop up the search-link popup window, to let the user pick
 *	an existing CD database file entry to link the current disc to.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
dbprog_dblink(curstat_t *s)
{
	int		i;
	cddb_linkopts_t	*p;
	XmString	xs;
	byte_t		min,
			sec,
			frame;
	char		buf[STR_BUF_SZ * 2];

	if (cur_db.dbfile == NULL)
		/* Output file undefined */
		return;

	/* Pop up message */
	CD_INFO(app_data.str_srchdb);

	/* Change to the watch cursor */
	cd_busycurs(TRUE, CURS_DBPROG | CURS_DBEXTT | CURS_DBEXTD);

	/* Search directory for possible alternatives, and allow
	 * user to select a file to link to.
	 */

	XmListDeleteAllItems(widgets.linksel.link_list);

	/* Grey out some buttons */
	XtSetSensitive(widgets.dbprog.linkdb_btn, False);
	XtSetSensitive(widgets.dbprog.loaddb_btn, False);

	/* Build linkopts list */
	if (!cddb_init_linkopts(&cur_db, s)) {
		/* Error: shouldn't get here */
		if (XtIsManaged(widgets.dialog.info))
			XtUnmanageChild(widgets.dialog.info);

		/* Restore buttons */
		XtSetSensitive(widgets.dbprog.linkdb_btn, True);
		XtSetSensitive(widgets.dbprog.loaddb_btn, True);

		/* Change to normal cursor */
		cd_busycurs(FALSE, CURS_DBPROG | CURS_DBEXTT | CURS_DBEXTD);

		CD_INFO(app_data.str_nolink);
		return;
	}

	/* Traverse the list and add to list widget */
	for (i = 0, p = cddb_linkopts(); p != NULL; i++, p = p->next) {
		if (p->offset == OFFSET_UNKN) {
			(void) sprintf(buf, "??:??  %.120s", p->dtitle);
			xs = XmStringCreate(buf, CHSET1);
		}
		else {
			util_blktomsf(p->offset, &min, &sec, &frame, 0);
			(void) sprintf(buf, "%02u:%02u  %.120s",
				       min, sec, p->dtitle);
			if (p->offset < OFFSET_THRESH)
				xs = XmStringCreate(buf, CHSET2);
			else
				xs = XmStringCreate(buf, CHSET1);
		}

		XmListAddItemUnselected(widgets.linksel.link_list, xs, i + 1);
		XmStringFree(xs);
	}

	/* Pop down info window */
	if (XtIsManaged(widgets.dialog.info))
		XtUnmanageChild(widgets.dialog.info);

	/* Restore buttons */
	XtSetSensitive(widgets.dbprog.linkdb_btn, True);
	XtSetSensitive(widgets.dbprog.loaddb_btn, True);

	/* Change to the normal cursor */
	cd_busycurs(FALSE, CURS_DBPROG | CURS_DBEXTT | CURS_DBEXTD);

	if (i == 0) {
		CD_INFO(app_data.str_nolink);
	}
	else if (!XtIsManaged(widgets.linksel.form)) {
		linksel_pos = 0;

		/* Pop up the link selector window */
		if (!XtIsManaged(widgets.dbprog.form))
			XtManageChild(widgets.dbprog.form);
		XtManageChild(widgets.linksel.form);
	}
}


/*
 * dbprog_pgm_active
 *	Indicate whether a play program is currently defined.
 *
 * Args:
 *	None.
 *
 * Return:
 *	TRUE = program is active,
 *	FALSE = program is not active.
 */
STATIC bool_t
dbprog_pgm_active(void)
{
	return (cur_db.playorder != NULL && cur_db.playorder[0] != '\0');
}


/***********************
 *   public routines   *
 ***********************/


/*
 * dbprog_curfileupd
 *	Update the curr.XXX file to show the current disc status.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
void
dbprog_curfileupd(void)
{
	cddb_curfileupd(&cur_db);
}


/*
 * dbprog_curtrkupd
 *	Update the track list display to show the current playing
 *	track entry in bold font.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
dbprog_curtrkupd(curstat_t *s)
{
	int		pos,
			list_pos;
	static int	sav_pos = -1;

	/* Update curfile */
	dbprog_curfileupd();

	if (sav_pos >= 0) {
		/* Update track list entry: un-highlight previous track */
		dbprog_listupd_ent(
			s, sav_pos, 
			(cur_db.trklist[sav_pos] != NULL) ?
				cur_db.trklist[sav_pos] : UNDEF_STR,
			FALSE
		);

		if (sel_pos == sav_pos + 1)
			/* This item is previously selected */
			XmListSelectPos(widgets.dbprog.trk_list,
					sel_pos, False);
	}

	if (s->cur_trk <= 0 || s->mode == MOD_BUSY || s->mode == MOD_NODISC) {
		sav_pos = -1;
		return;
	}

	sav_pos = pos = di_curtrk_pos(s);
	list_pos = pos + 1;

	/* Update track list entry: highlight current track */
	dbprog_listupd_ent(
		s, sav_pos, 
		(cur_db.trklist[sav_pos] != NULL) ?
			cur_db.trklist[sav_pos] : UNDEF_STR,
		FALSE
	);

	/* If this item is previously selected, re-select it */
	if (sel_pos == list_pos)
		XmListSelectPos(widgets.dbprog.trk_list, sel_pos, False);
	else if (ind_pos == list_pos)
		XmListSelectPos(widgets.dbprog.trk_list, ind_pos, False);

	/* Auto-scroll the track list if the current track is not visible.
	 * No scrolling is done while a track list entry is selected
	 * or if a track title is being edited.
	 */
	if (sel_pos < 0 && ind_pos < 0)
		dbprog_trklist_autoscroll(s, list_pos);

	dbprog_extt_autotrk_upd(s, list_pos);
}


/*
 * dbprog_progclear
 *	Clear program sequence.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
dbprog_progclear(curstat_t *s)
{
	if (cur_db.playorder == NULL)
		/* Nothing to do */
		return;

	MEM_FREE(cur_db.playorder);
	cur_db.playorder = NULL;

	s->prog_tot = 0;
	s->prog_cnt = 0;
	s->program = FALSE;

	/* Update display */
	dpy_progmode(s);
}


/*
 * dbprog_dbclear
 *	Clear in-core CD database entry.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	reload - Whether we are going to be re-loading the CDDB entry.
 *
 * Return:
 *	Nothing.
 */
void
dbprog_dbclear(curstat_t *s, bool_t reload)
{
	word16_t	flags;
	bool_t		upd_display = FALSE;
	static bool_t	first_time = TRUE;

	/* Pop down the extd and extt popups if necessary */
	if (XtIsManaged(widgets.dbextd.form)) {
		dbprog_extd_cancel(
			widgets.dbextd.cancel_btn,
			(XtPointer) s,
			(XtPointer) NULL
		);
	}
	if (XtIsManaged(widgets.dbextt.form)) {
		dbprog_extt_cancel(
			widgets.dbextt.cancel_btn,
			(XtPointer) s,
			(XtPointer) NULL
		);
	}

	/* Save in-core database to file, if so designated */
	if (s->flags & STAT_SAVE) {
		s->flags |= STAT_CLEAR;
		dbprog_save(widgets.dbprog.savedb_btn, (XtPointer) s, NULL);
		return;
	}

	if (first_time || s->mode == MOD_NODISC) {
		first_time = FALSE;
		upd_display = TRUE;
	}

	/* Save flags */
	flags = s->flags;

	/* Clear database entry structure */
	cddb_clear(&cur_db, s, reload);

	/* Clear flags */
	s->flags &= ~(STAT_SAVE | STAT_CLEAR |
		      STAT_EJECT | STAT_EXIT | STAT_CHGDISC);

	/* Set flag */
	s->cddb = CDDB_INVALID;

	/* Configure the wwwWarp search engine options menu */
	dbprog_wwwwarp_sel_cfg(s);

	if (upd_display) {
		/* Update display */
		dbprog_dpytottime(s);
		dbprog_dpyid();
		dpy_dtitle(s);
		dpy_ttitle(s);

		/* Update curfile */
		dbprog_curfileupd();

		/* Update database/program display */
		XmTextSetString(widgets.dbprog.dtitle_txt, "");
		XmListDeleteAllItems(widgets.dbprog.trk_list);
		XmTextSetString(widgets.dbprog.ttitle_txt, "");
		XmTextSetString(widgets.dbprog.pgmseq_txt, "");
		XmTextSetString(widgets.dbextd.disc_txt, "");
		XmTextSetString(widgets.dbextt.trk_txt, "");

		/* Make some buttons insensitive */
		XtSetSensitive(widgets.dbprog.addpgm_btn, False);
		XtSetSensitive(widgets.dbprog.clrpgm_btn, False);
		XtSetSensitive(widgets.dbprog.send_btn, False);
		XtSetSensitive(widgets.dbprog.savedb_btn, False);
		XtSetSensitive(widgets.dbprog.linkdb_btn, False);
		XtSetSensitive(widgets.dbprog.extd_btn, False);
		XtSetSensitive(widgets.dbprog.extt_btn, False);

		/* Update extt label */
		dbprog_extt_lblupd(NULL);
	}

	/* Clear changed flag */
	cur_db.flags &= ~CDDB_CHANGED;

	/* Eject the CD, if so specified */
	if (flags & STAT_EJECT) {
		cd_busycurs(TRUE, CURS_ALL);

		di_load_eject(s);

		cd_busycurs(FALSE, CURS_ALL);
	}

	/* Change disc, if so specified */
	if (flags & STAT_CHGDISC) {
		cd_busycurs(TRUE, CURS_ALL);

		di_chgdisc(s);

		cd_busycurs(FALSE, CURS_ALL);
	}

	/* Quit the application, if so specified */
	if (flags & STAT_EXIT)
		cd_quit(s);
}


/*
 * dbprog_dbget
 *	Read in the CD database file entry pertaining to the
 *	currently loaded disc, if available.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
dbprog_dbget(curstat_t *s)
{
	cddb_ret_t	ret;
	cddb_match_t	*p;
	char		*msg;
	XmString	xs;

	if (XtIsManaged(widgets.dialog.dbmatch))
		XtUnmanageChild(widgets.dialog.dbmatch);

	if (cur_db.discid == 0) {
		/* Get magic disc identifier */
		if ((cur_db.discid = cddb_discid(s)) == 0)
			/* Invalid identifier */
			return;

		cur_db.queryid = cur_db.discid;
	}

	if (s->onetrk_prog && s->mode != MOD_STOP) {
		/* Currently playing a single-track program: clear it first */
		s->onetrk_prog = FALSE;
		dbprog_progclear(s);
	}

	/* Set the cddb indicator */
	s->cddb = CDDB_WAIT;

	/* Update widgets */
	dbprog_structupd(s);
	dpy_dtitle(s);
	dpy_ttitle(s);

	/* Configure the wwwWarp search engine options menu */
	dbprog_wwwwarp_sel_cfg(s);

	/* Update curfile */
	dbprog_curfileupd();

	XtSetSensitive(widgets.dbprog.loaddb_btn, False);

	/* If in synchronous mode, change to watch cursor, otherwise,
	 * force normal cursor.
	 */
	cd_busycurs(cddb_issync(), CURS_ALL);

	/* Load CD database entry */
	if ((ret = cddb_load(&cur_db, s)) != 0) {
		DBGPRN(errfp, "cddb_load: status=%d arg=%d\n",
			CDDB_GET_STAT(ret), CDDB_GET_ARG(ret));
	}

	/* Change to normal cursor */
	if (cddb_issync())
		cd_busycurs(FALSE, CURS_ALL);

	switch (CDDB_GET_STAT(ret)) {
	case 0:
		/* Success */
		break;

	case AUTH_ERR:
		/* Authorization failed */
		if (auth_initted) {
			cd_confirm_popup(
				app_data.str_confirm, app_data.str_authfail,
				dbprog_auth_retry, (XtPointer) 1,
				dbprog_auth_retry, NULL 
			);
		}
		else {
			/* Clear out user name and password */
			XmTextSetString(widgets.auth.name_txt, "");
			XmTextSetString(widgets.auth.pass_txt, "");
			if (auth_passwd != NULL) {
				(void) memset(auth_passwd, 0,
					      strlen(auth_passwd));
				MEM_FREE(auth_passwd);
				auth_passwd = NULL;
			}

			/* Pop up authorization dialog */
			if (!XtIsManaged(widgets.auth.form))
				XtManageChild(widgets.auth.form);

			/* Set keyboard focus to the user name field */
			XmProcessTraversal(
				widgets.auth.name_txt,
				XmTRAVERSE_CURRENT
			);
		}
		return;
		/*NOTREACHED*/

	case MATCH_ERR:
		/* Inexact match found - ask user */
		p = cur_db.matchlist;
		if (p != NULL && !dbmatch_mode) {
			/* Set flag to prevent recursion */
			dbmatch_mode = TRUE;

			msg = (char *) MEM_ALLOC(
				"msg",
				strlen(app_data.str_rmtmatch) +
				strlen(p->category) +
				strlen(p->dtitle)
			);
			if (msg == NULL) {
				CD_FATAL(app_data.str_nomemory);
				return;
			}

			/* Set up dialog box message string */
			(void) sprintf(msg, app_data.str_rmtmatch,
				       p->category, p->dtitle);

			xs = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);

			XtVaSetValues(widgets.dialog.dbmatch,
				XmNmessageString, xs,
				NULL
			);

			XmStringFree(xs);
			MEM_FREE(msg);

			/* Set pointer */
			cur_db.match_cur = p;

			/* Pop up dialog box */
			XtManageChild(widgets.dialog.dbmatch);
			return;
		}

		cur_db.type = CDDB_INVALID;
		/*FALLTHRU*/

	default:
		/* Cannot find CDDB entry, or other error */

		/* Set the cddb indicator */
		s->cddb = (byte_t) cur_db.type;

		/* Update widgets */
		dbprog_structupd(s);

		/* Configure the wwwWarp search engine options menu */
		dbprog_wwwwarp_sel_cfg(s);

		XtSetSensitive(widgets.dbprog.linkdb_btn, True);
		XtSetSensitive(widgets.dbprog.loaddb_btn, True);

		/* Update display */
		dbprog_dpyid();
		dpy_dtitle(s);
		dpy_ttitle(s);

		/* Update curfile */
		dbprog_curfileupd();

		return;
	}

	/* Set the cddb indicator */
	s->cddb = (byte_t) cur_db.type;

	/* Update widgets */
	dbprog_structupd(s);

	/* Configure the wwwWarp search engine options menu */
	dbprog_wwwwarp_sel_cfg(s);

	if (cur_db.type == CDDB_LOCAL) {
		XtSetSensitive(widgets.dbprog.send_btn, True);
		XtSetSensitive(widgets.dbprog.savedb_btn, False);
	}
	else {
		XtSetSensitive(widgets.dbprog.send_btn, False);
		if (cur_db.flags & CDDB_AUTOSAVE)
			XtSetSensitive(widgets.dbprog.savedb_btn, False);
		else
			XtSetSensitive(widgets.dbprog.savedb_btn, True);
	}

	XtSetSensitive(widgets.dbprog.linkdb_btn, False);
	XtSetSensitive(widgets.dbprog.loaddb_btn, True);

	s->program = dbprog_pgm_active();

	/* Update display */
	dbprog_dpyid();
	dpy_progmode(s);
	dpy_dtitle(s);
	dpy_ttitle(s);

	/* Update curfile */
	dbprog_curfileupd();
}


/*
 * dbprog_init
 *	Initialize the CD database/program subsystem.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
dbprog_init(curstat_t *s)
{
	int		i;
	Widget		w;
	XmString	xs,
			xs2,
			xs3;
	srcheng_ent_t	*p;
	Arg		arg[10];
	char		str[12];
	bool_t		first;

	/* Check various error message strings to ensure we don't
	 * overflow our error message buffer later.
	 */
	if (((int) (strlen(app_data.str_saverr_fork) + 16) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_saverr_suid) + 32) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_saverr_open) + 1) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_saverr_close) + 1) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_saverr_killed) + 16) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_saverr_write) + 1) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_lnkerr_suid) + 32) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_lnkerr_link) + 1) >= ERR_BUF_SZ)) {
		CD_FATAL(app_data.str_longpatherr);
		return;
	}

	timemode = TIME_TRACK;

	/* Clear the in-core structure */
	dbprog_dbclear(s, FALSE);

	/* Save extd and extt labels, create versions with asterisk */
	XtVaGetValues(widgets.dbprog.extd_lbl,
		XmNlabelString, &xs_extd_lbl,
		NULL
	);
	XtVaGetValues(widgets.dbprog.extt_lbl,
		XmNlabelString, &xs_extt_lbl,
		NULL
	);
	xs = XmStringCreateSimple("*");
	xs_extd_lblx = XmStringConcat(xs_extd_lbl, xs);
	xs_extt_lblx = XmStringConcat(xs_extt_lbl, xs);
	XmStringFree(xs);

	/* Set up wwwWarp */
	first = TRUE;
	for (p = srcheng_list_head; p != NULL; p = p->next) {
		if (strcmp(p->name, "-") == 0) {
			/* Separator line */
			i = 0;
			XtSetArg(arg[i], XmNseparatorType,
				 XmSINGLE_DASHED_LINE); i++;
			w = XmCreateSeparator(
				widgets.wwwwarp.srchsite_menu,
				"menuSeparator",
				arg,
				i
			);
			XtManageChild(w);
			continue;
		}

		/* Real menu entry */
		(void) sprintf(str, "[%s%s%s]   ",
			(p->acnt > 0) ? "a" : "-",
			(p->dcnt > 0) ? "d" : "-",
			(p->tcnt > 0) ? "t" : "-");
		xs2 = XmStringCreateLtoR(str, CHSET3);
		xs3 = XmStringCreateLtoR(p->name, CHSET2);
		xs = XmStringConcat(xs2, xs3);

		i = 0;
		XtSetArg(arg[i], XmNlabelString, xs); i++;
		w = XmCreatePushButton(
			widgets.wwwwarp.srchsite_menu,
			p->name,
			arg,
			i
		);
		XmStringFree(xs);
		XmStringFree(xs2);
		XmStringFree(xs3);
		XtManageChild(w);
		p->aux = (void *) w;

		if (first) {
			XtVaSetValues(widgets.wwwwarp.srchsite_opt,
				XmNmenuHistory, w,
				NULL
			);

			/* Set current selection to the first entry */
			cur_srcheng = p;
			srcheng_list_enabled = TRUE;
			first = FALSE;
		}

		XtAddCallback(w,
			XmNactivateCallback,
			(XtCallbackProc) dbprog_srcheng_sel,
			(XtPointer) p
		);
	}
}


/*
 * dbprog_chgsave
 *	If in-core CDDB info has been changed, ask user if it should
 *	be saved.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Whether the application should proceed to shut down
 */
/*ARGSUSED*/
bool_t
dbprog_chgsave(curstat_t *s)
{
	char	*msg;

	if (cur_db.discid != 0 && (cur_db.flags & CDDB_CHANGED)) {
		if (!XtIsManaged(widgets.dialog.confirm)) {
			if (cur_db.dtitle == NULL) {
				msg = (char *) MEM_ALLOC(
					"msg",
					strlen(app_data.str_chgsave) +
					strlen(app_data.str_unkndisc)
				);
			}
			else {
				msg = (char *) MEM_ALLOC(
					"msg",
					strlen(app_data.str_chgsave) +
					strlen(cur_db.dtitle)
				);
			}

			if (msg == NULL) {
				CD_FATAL(app_data.str_nomemory);
				return FALSE;
			}

			(void) sprintf(msg, app_data.str_chgsave,
				       (cur_db.dtitle == NULL) ?
					app_data.str_unkndisc : cur_db.dtitle);

			/* Pop-up the dialog box */
			cd_confirm_popup(
				app_data.str_confirm, msg,
				dbprog_do_clear, (XtPointer) STAT_SAVE,
				dbprog_do_clear, (XtPointer) 0
			);

			MEM_FREE(msg);
		}
		return FALSE;
	}

	return TRUE;
}


/*
 * dbprog_curdtitle
 *	Return the current disc title string.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Disc title text string, or the null string if there
 *	is no title available.
 */
char *
dbprog_curdtitle(curstat_t *s)
{
	if (s->mode == MOD_BUSY || s->mode == MOD_NODISC ||
	    s->cddb == CDDB_WAIT)
		return ("");

	return (cur_db.dtitle == NULL ? app_data.str_unkndisc : cur_db.dtitle);
}


/*
 * dbprog_curttitle
 *	Return the current track title string.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Track title text string, or the null string if there
 *	is no title available.
 */
char *
dbprog_curttitle(curstat_t *s)
{
	int	n = di_curtrk_pos(s);

	if (s->mode == MOD_BUSY || s->mode == MOD_NODISC ||
	    (int) s->cur_trk < 0 || s->cddb == CDDB_WAIT)
		return ("");

	if (n < 0 || cur_db.trklist[n] == NULL)
		return (app_data.str_unkntrk);

	return (cur_db.trklist[n]);
}


/*
 * dbprog_pgm_parse
 *	Parse the program mode play sequence text string, and
 *	update the playorder table in the curstat_t structure.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	TRUE=success, FALSE=error.
 */
bool_t
dbprog_pgm_parse(curstat_t *s)
{
	int	i,
		j,
		n;
	char	*p,
		*q,
		*tmpbuf;
	bool_t	last = FALSE,
		skipped = FALSE;

	if (cur_db.playorder == NULL)
		/* Nothing to do */
		return TRUE;

	n = strlen(cur_db.playorder) + 1;
	if ((tmpbuf = (char *) MEM_ALLOC("pgm_parse_tmpbuf", n)) == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	(void) strcpy(tmpbuf, cur_db.playorder);

	s->prog_tot = 0;

	for (i = 0, p = q = tmpbuf; i < MAXTRACK; p = ++q) {
		/* Skip p to the next digit */
		for (; !isdigit(*p) && *p != '\0'; p++)
			;

		if (*p == '\0')
			/* No more to do */
			break;

		/* Skip q to the next non-digit */
		for (q = p; isdigit(*q); q++)
			;

		if (*q == PGM_SEPCHAR)
			*q = '\0';
		else if (*q == '\0')
			last = TRUE;
		else {
			MEM_FREE(tmpbuf);
			CD_WARNING(app_data.str_seqfmterr);
			return FALSE;
		}

		if (q > p) {
			/* Update play sequence */
			for (j = 0; j < MAXTRACK; j++) {
				if (s->trkinfo[j].trkno == atoi(p)) {
					s->playorder[i] = j;
					s->prog_tot++;
					i++;
					break;
				}
			}

			if (j >= MAXTRACK)
				skipped = TRUE;
		}

		if (last)
			break;
	}

	if (skipped) {
		/* Delete invalid tracks from list */

		tmpbuf[0] = '\0';
		for (i = 0; i < (int) s->prog_tot; i++) {
			if (i == 0)
				(void) sprintf(tmpbuf, "%u",
					    s->trkinfo[s->playorder[i]].trkno);
			else
				(void) sprintf(tmpbuf, "%s,%u",
					    tmpbuf,
					    s->trkinfo[s->playorder[i]].trkno);
		}

		XmTextSetString(widgets.dbprog.pgmseq_txt, tmpbuf);

		CD_WARNING(app_data.str_invpgmtrk);
	}

	MEM_FREE(tmpbuf);

	return TRUE;
}


/**************** vv Callback routines vv ****************/

/*
 * dbprog_popup
 *	Pop up the database/program subsystem window.
 */
void
dbprog_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (!XtIsManaged(widgets.dbprog.form)) {
		/* Pop up the dbprog window */
		XtManageChild(widgets.dbprog.form);

		/* Pop up the wwwWarp windows if necessary */
		if (wwwwarp_manage)
			dbprog_wwwwarp(w, client_data, call_data);

		/* Pop up the extd/extt windows if necessary */
		if (extd_manage)
			dbprog_extd(w, client_data, call_data);

		if (extt_manage)
			dbprog_extt(w, (XtPointer) FALSE, call_data);
	}
}


/*
 * dbprog_rmtdsbl
 *	The remote disable button callback.
 */
/*ARGSUSED*/
void
dbprog_rmtdsbl(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmToggleButtonCallbackStruct	*p =
		(XmToggleButtonCallbackStruct *)(void *) call_data;

	if (p->reason != XmCR_VALUE_CHANGED)
		return;

	DBGPRN(errfp, "\n* DISABLE-REMOTE: %s\n", p->set ? "On" : "Off");

	app_data.cddb_rmtdsbl = (bool_t) p->set;
}


/*
 * dbprog_txtline_verify
 *	Single-line text widget user-input verification callback.
 */
/*ARGSUSED*/
void
dbprog_txtline_verify(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmTextVerifyCallbackStruct
		*p = (XmTextVerifyCallbackStruct *)(void *) call_data;
	int	i;

	if (p->reason != XmCR_MODIFYING_TEXT_VALUE)
		return;

	p->doit = True;

	if (p->startPos != p->endPos)
		/* Deleting text, no verification needed */
		return;

	switch (p->text->format) {
	case XmFMT_8_BIT:
		for (i = 0; i < p->text->length; i++) {
			/* This is a single-line text widget, so a
			 * newline is not allowed.
			 */
			if (p->text->ptr[i] == '\n' ||
			    p->text->ptr[i] == '\r') {
				p->doit = False;
				break;
			}
		}
		break;

	case XmFMT_16_BIT:
	default:
		/* Nothing to do here */
		break;
	}
}


/*
 * dbprog_dtitle_new
 *	Disc title editor text widget callback function.
 */
/*ARGSUSED*/
void
dbprog_dtitle_new(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmAnyCallbackStruct	*p = (XmAnyCallbackStruct *)(void *) call_data;
	curstat_t		*s = (curstat_t *)(void *) client_data;
	char			*str;

	switch (p->reason) {
	case XmCR_VALUE_CHANGED:
		if ((str = XmTextGetString(w)) == NULL)
			return;

		if (strcmp(str, UNDEF_STR) == 0) {
			if (cur_db.dtitle != NULL) {
				MEM_FREE(cur_db.dtitle);
				cur_db.dtitle = NULL;
			}
			XtFree(str);
			return;
		}

		cur_db.flags |= CDDB_CHANGED;
		XtSetSensitive(widgets.dbprog.savedb_btn, True);

		if (cur_db.dtitle != NULL)
			MEM_FREE(cur_db.dtitle);

		if (str[0] == '\0') {
			XtFree(str);
			cur_db.dtitle = NULL;
		}
		else {
			cur_db.dtitle = (char *) MEM_ALLOC(
				"cur_db.dtitle",
				strlen(str) + 1
			);
			if (cur_db.dtitle == NULL) {
				CD_FATAL(app_data.str_nomemory);
				return;
			}
			strcpy(cur_db.dtitle, str);
			XtFree(str);
		}

		break;

	case XmCR_ACTIVATE:
	case XmCR_LOSING_FOCUS:
		/* Update the extd window if necessary */
		if (XtIsManaged(widgets.dbextd.form))
			/* Update extd disc title */
			dbprog_set_extd_title(s->cur_disc, cur_db.dtitle);

		/* Update main window title display */
		dpy_dtitle(s);

		/* Update curfile */
		dbprog_curfileupd();

		break;

	default:
		break;
	}
}


/*
 * dbprog_trklist_play
 *	Track list entry selection default action callback.
 */
void
dbprog_trklist_play(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmListCallbackStruct	*p = (XmListCallbackStruct *)(void *) call_data;
	curstat_t		*s = (curstat_t *)(void *) client_data;

	if (p->reason != XmCR_DEFAULT_ACTION)
		return;

	/* Stop current playback */
	if (s->mode != MOD_STOP)
		di_stop(s, FALSE);

	sel_pos = p->item_position;

	/* Clear previous program */
	XmTextSetString(widgets.dbprog.pgmseq_txt, "");
	XtSetSensitive(widgets.dbprog.addpgm_btn, False);
	XtSetSensitive(widgets.dbprog.clrpgm_btn, False);
	dbprog_progclear(s);

	/* This is a single-track program as a result of double clicking
	 * on a track (or pressing return).
	 */
	s->onetrk_prog = TRUE;

	/* Add selected track to program */
	dbprog_addpgm(w, client_data, call_data);

	/* Play selected track */
	cd_play_pause(w, client_data, call_data);

	XmListDeselectPos(w, sel_pos);
	sel_pos = -1;
	XmTextSetString(widgets.dbprog.ttitle_txt, "");

	XtSetSensitive(widgets.dbprog.addpgm_btn, False);
	XtSetSensitive(widgets.dbprog.extt_btn, False);

	/* Update extt label */
	dbprog_extt_lblupd(NULL);
}


/*
 * dbprog_trklist_select
 *	Track list entry selection callback.
 */
void
dbprog_trklist_select(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmListCallbackStruct	*p = (XmListCallbackStruct *)(void *) call_data;
	curstat_t		*s = (curstat_t *)(void *) client_data;
	int			pos;
	char			*cp;

	if (p->reason != XmCR_BROWSE_SELECT)
		return;

	if (!di_check_disc(s) || s->mode == MOD_BUSY || s->mode == MOD_NODISC)
		return;

	pos = p->item_position - 1;

	if (title_edited) {
		title_edited = FALSE;

		if ((cp = XmTextGetString(widgets.dbprog.ttitle_txt)) == NULL)
			return;

		/* Update track list entry */
		dbprog_listupd_ent(s, pos, cp, FALSE);

		XmListDeselectPos(w, sel_pos);
		sel_pos = -1;
		XmTextSetString(widgets.dbprog.ttitle_txt, "");

		if (cur_db.trklist[pos] != NULL)
			MEM_FREE(cur_db.trklist[pos]);

		cur_db.trklist[pos] = (char *) MEM_ALLOC(
			"cur_db.trklist[n]",
			strlen(cp) + 1
		);
		if (cur_db.trklist[pos] == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		strcpy(cur_db.trklist[pos], cp);
		XtFree(cp);

		cur_db.flags |= CDDB_CHANGED;
		XtSetSensitive(widgets.dbprog.savedb_btn, True);
		XtSetSensitive(widgets.dbprog.addpgm_btn, False);
		XtSetSensitive(widgets.dbprog.extt_btn, False);

		/* Update extt label */
		dbprog_extt_lblupd(NULL);

		/* Update the extt window if necessary */
		if (extt_pos == pos && XtIsManaged(widgets.dbextt.form)) {
			dbprog_set_extt_title(
				s->trkinfo[pos].trkno,
				cur_db.trklist[pos]
			);
		}

		/* Update the main window if necessary */
		if (di_curtrk_pos(s) == pos) {
			dpy_ttitle(s);

			/* Update curfile */
			dbprog_curfileupd();
		}

		/* Return the input focus to the track title editor */
		XmProcessTraversal(
			widgets.dbprog.ttitle_txt,
			XmTRAVERSE_CURRENT
		);
	}
	else if (sel_pos == p->item_position) {
		/* This item is already selected: deselect it */

		XmListDeselectPos(w, p->item_position);
		sel_pos = ind_pos = -1;
		XmTextSetString(widgets.dbprog.ttitle_txt, "");

		XtSetSensitive(widgets.dbprog.addpgm_btn, False);
		XtSetSensitive(widgets.dbprog.extt_btn, False);

		/* Update extt label */
		dbprog_extt_lblupd(NULL);
	}
	else {
		sel_pos = p->item_position;

		if (cur_db.trklist[pos] == NULL) {
			XmTextSetString(widgets.dbprog.ttitle_txt, UNDEF_STR);
			XmTextSetInsertionPosition(
				widgets.dbprog.ttitle_txt,
				strlen(UNDEF_STR)
			);
		}
		else {
			XmTextSetString(widgets.dbprog.ttitle_txt,
					cur_db.trklist[pos]);
			XmTextSetInsertionPosition(
				widgets.dbprog.ttitle_txt,
				strlen(cur_db.trklist[pos])
			);
		}

		XtSetSensitive(widgets.dbprog.addpgm_btn, True);
		XtSetSensitive(widgets.dbprog.extt_btn, True);

		/* Update extt label */
		dbprog_extt_lblupd(cur_db.extt[pos]);

		/* Warp the extt window to the new selected track, if
		 * it is popped up.
		 */
		if (XtIsManaged(widgets.dbextt.form)) {
			dbprog_extt(w, (XtPointer) FALSE, call_data);
			XmListSelectPos(w, sel_pos, False);
		}
	}
}


/*
 * dbprog_ttitle_focuschg
 *	Track title editor text widget keyboard focus change callback.
 */
/*ARGSUSED*/
void
dbprog_ttitle_focuschg(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmAnyCallbackStruct	*p = (XmAnyCallbackStruct *)(void *) call_data;
	curstat_t		*s = (curstat_t *)(void *) client_data;
	int			i;

	switch (p->reason) {
	case XmCR_FOCUS:
		if (sel_pos < 0) {
			for (i = 0; i < (int) s->tot_trks; i++) {
				if (cur_db.trklist[i] == NULL) {
					ind_pos = i + 1;

					XmListSelectPos(
						widgets.dbprog.trk_list,
						ind_pos,
						False
					);
					XmListSetBottomPos(
						widgets.dbprog.trk_list,
						ind_pos
					);
					break;
				}
			}
		}
		break;

	case XmCR_LOSING_FOCUS:
		if (ind_pos > 0) {
			XmListDeselectPos(widgets.dbprog.trk_list, ind_pos);
			ind_pos = -1;
		}
		break;

	default:
		break;
	}
}


/*
 * dbprog_ttitle_new
 *	Track title editor text widget callback function.
 */
void
dbprog_ttitle_new(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmAnyCallbackStruct	*p = (XmAnyCallbackStruct *)(void *) call_data;
	curstat_t		*s = (curstat_t *)(void *) client_data;
	char			*cp;
	int			*pos,
				i;
	XmListCallbackStruct	cb;

	if (p->reason == XmCR_VALUE_CHANGED) {
		if ((cp = XmTextGetString(w)) == NULL)
			return;

		if (*cp == '\0')
			title_edited = FALSE;
		else if (sel_pos < 0)
			title_edited = TRUE;

		XtFree(cp);
		return;
	}
	else if (p->reason != XmCR_ACTIVATE)
		return;

	if (sel_pos >= 0 &&
	    XmListGetSelectedPos(widgets.dbprog.trk_list, &pos, &i)) {
		if ((cp = XmTextGetString(w)) == NULL)
			return;

		if (pos == NULL) {
			XtFree(cp);
			return;
		}

		if (i != 1)
			XtFree(cp);
		else {
			/* Update track list entry */
			dbprog_listupd_ent(s, (*pos)-1, cp, FALSE);

			XmListDeselectPos(widgets.dbprog.trk_list, sel_pos);
			sel_pos = -1;

			if (cur_db.trklist[(*pos)-1] != NULL)
				MEM_FREE(cur_db.trklist[(*pos)-1]);

			cur_db.trklist[(*pos)-1] = (char *) MEM_ALLOC(
				"cur_db.trklist[n]",
				strlen(cp) + 1
			);
			if (cur_db.trklist[(*pos)-1] == NULL) {
				CD_FATAL(app_data.str_nomemory);
				return;
			}
			strcpy(cur_db.trklist[(*pos)-1], cp);
			XtFree(cp);

			cur_db.flags |= CDDB_CHANGED;
			XtSetSensitive(widgets.dbprog.savedb_btn, True);
			XtSetSensitive(widgets.dbprog.addpgm_btn, False);
			XtSetSensitive(widgets.dbprog.extt_btn, False);

			/* Update extt label */
			dbprog_extt_lblupd(NULL);

			/* Update the extt window if necessary */
			if (extt_pos == (*pos)-1 &&
			    XtIsManaged(widgets.dbextt.form))
				dbprog_set_extt_title(
					s->trkinfo[extt_pos].trkno,
					cur_db.trklist[extt_pos]
				);

			/* Update the main window if necessary */
			if (di_curtrk_pos(s) == (*pos)-1) {
				dpy_ttitle(s);

				/* Update curfile */
				dbprog_curfileupd();
			}
		}

		XmTextSetString(w, "");

		XtFree((XtPointer) pos);
	}
	else {
		/* Pressing Return in this case is equivalent to clicking
		 * on the first title-less track on the track list.
		 */
		for (i = 0; i < (int) s->tot_trks; i++) {
			if (cur_db.trklist[i] == NULL) {
				cb.item_position = i + 1;
				cb.reason = XmCR_BROWSE_SELECT;
				cb.event = p->event;

				dbprog_trklist_select(
					widgets.dbprog.trk_list,
					(XtPointer) s,
					(XtPointer) &cb
				);
				break;
			}
		}
	}

	for (i = 0; i < (int) s->tot_trks; i++) {
		if (cur_db.trklist[i] == NULL) {
			ind_pos = i + 1;

			XmListSelectPos(
				widgets.dbprog.trk_list,
				ind_pos,
				False
			);
			XmListSetBottomPos(
				widgets.dbprog.trk_list,
				ind_pos
			);
			break;
		}
	}
}


/*
 * dbprog_pgmseq_verify
 *	Play sequence editor text widget user-input verification callback.
 */
/*ARGSUSED*/
void
dbprog_pgmseq_verify(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmTextVerifyCallbackStruct
		*p = (XmTextVerifyCallbackStruct *)(void *) call_data;
	int	i;
	char	prev,
		*currstr;

	if (p->reason != XmCR_MODIFYING_TEXT_VALUE)
		return;

	p->doit = True;

	if (p->startPos != p->endPos)
		/* Deleting text, no verification needed */
		return;

	currstr = XmTextGetString(w);

	switch (p->text->format) {
	case XmFMT_8_BIT:
		if (p->currInsert > 0 && currstr != NULL)
			prev = currstr[p->currInsert - 1];
		else
			prev = ',';

		for (i = 0; i < p->text->length; i++) {
			/* Only allowed input is digits or PGM_SEPCHAR */
			if (p->text->ptr[i] == PGM_SEPCHAR) {
				if (prev == PGM_SEPCHAR) {
					p->doit = False;
					break;
				}
			}
			else if (!isdigit(p->text->ptr[i])) {
				p->doit = False;
				break;
			}
			prev = p->text->ptr[i];
		}
		break;

	case XmFMT_16_BIT:
	default:
		/* Don't know how to handle other character sets yet */
		p->doit = False;
		break;
	}

	if (currstr != NULL)
		XtFree(currstr);
}


/*
 * dbprog_pgmseq_txtchg
 *	Play sequence editor text widget text changed callback.
 */
/*ARGSUSED*/
void
dbprog_pgmseq_txtchg(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmAnyCallbackStruct	*p = (XmAnyCallbackStruct *)(void *) call_data;
	curstat_t		*s = (curstat_t *)(void *) client_data;
	char			*cp;

	if (p->reason != XmCR_VALUE_CHANGED)
		return;

	if (s->onetrk_prog && s->mode != MOD_STOP) {
		/* Currently playing a single-track program: clear it first */
		s->onetrk_prog = FALSE;
		dbprog_progclear(s);
	}

	/* Disable shuffle mode */
	if (s->shuffle) {
		di_shuffle(s, FALSE);
		set_shuffle_btn(FALSE);
	}

	if (cur_db.playorder != NULL)
		MEM_FREE(cur_db.playorder);
	cp = XmTextGetString(w);
	cur_db.playorder = (char *) MEM_ALLOC(
		"cur_db.playorder",
		strlen(cp) + 1
	);
	if (cur_db.playorder == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}
	strcpy(cur_db.playorder, cp);
	XtFree(cp);

	if ((s->program = dbprog_pgm_active()) == FALSE)
		dbprog_progclear(s);

	/* Update display */
	dpy_progmode(s);

	cur_db.flags |= CDDB_CHANGED;
	XtSetSensitive(widgets.dbprog.savedb_btn, True);
	XtSetSensitive(widgets.dbprog.clrpgm_btn, (Boolean) s->program);
}


/*
 * dbprog_addpgm
 *	Program Add button callback.
 */
/*ARGSUSED*/
void
dbprog_addpgm(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	char		*cp,
			tmpbuf[6];

	if (sel_pos < 0 || !di_check_disc(s) || s->mode == MOD_BUSY ||
	    s->mode == MOD_NODISC) {
		cd_beep();
		return;
	}

	if (s->onetrk_prog && s->mode != MOD_STOP) {
		/* Currently playing a single-track program: clear it first */
		s->onetrk_prog = FALSE;
		dbprog_progclear(s);
	}

	/* Disable shuffle mode */
	if (s->shuffle) {
		di_shuffle(s, FALSE);
		set_shuffle_btn(FALSE);
	}

	if (cur_db.playorder != NULL && cur_db.playorder[0] == '\0') {
		MEM_FREE(cur_db.playorder);
		cur_db.playorder = NULL;
	}

	if (cur_db.playorder == NULL) {
		(void) sprintf(tmpbuf, "%u", s->trkinfo[sel_pos-1].trkno);
		cp = (char *) MEM_ALLOC("addpgm_cp", strlen(tmpbuf) + 1);
	}
	else {
		(void) sprintf(tmpbuf, "%c%u",
			       PGM_SEPCHAR, s->trkinfo[sel_pos-1].trkno);
		cp = (char *) MEM_ALLOC(
			"addpgm_cp",
			strlen(cur_db.playorder) + strlen(tmpbuf) + 1
		);
	}

	if (cp == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	(void) sprintf(cp, "%s%s",
		       (cur_db.playorder == NULL) ? "" : cur_db.playorder,
		       tmpbuf);

	if (cur_db.playorder != NULL)
		MEM_FREE(cur_db.playorder);
	cur_db.playorder = cp;

	if (!s->onetrk_prog) {
		XmTextSetString(widgets.dbprog.pgmseq_txt, cur_db.playorder);
		XmTextSetInsertionPosition(
			widgets.dbprog.pgmseq_txt,
			strlen(cur_db.playorder)
		);
	}

	s->program = TRUE;

	/* Update display */
	dpy_progmode(s);

	if (!s->onetrk_prog) {
		cur_db.flags |= CDDB_CHANGED;
		XtSetSensitive(widgets.dbprog.savedb_btn, True);
		XtSetSensitive(widgets.dbprog.clrpgm_btn, True);
	}

	XtSetSensitive(widgets.dbprog.addpgm_btn, False);
	XtSetSensitive(widgets.dbprog.extt_btn, False);

	/* Update extt label */
	dbprog_extt_lblupd(NULL);

	XmListDeselectPos(widgets.dbprog.trk_list, sel_pos);
	sel_pos = -1;

	XmTextSetString(widgets.dbprog.ttitle_txt, "");
}


/*
 * dbprog_clrpgm
 *	Program Clear button callback.
 */
/*ARGSUSED*/
void
dbprog_clrpgm(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmListDeselectPos(widgets.dbprog.trk_list, sel_pos);
	sel_pos = -1;
	XmTextSetString(widgets.dbprog.ttitle_txt, "");

	XmTextSetString(widgets.dbprog.pgmseq_txt, "");

	cur_db.flags |= CDDB_CHANGED;
	XtSetSensitive(widgets.dbprog.savedb_btn, True);
	XtSetSensitive(widgets.dbprog.addpgm_btn, False);
	XtSetSensitive(widgets.dbprog.clrpgm_btn, False);
	XtSetSensitive(widgets.dbprog.extt_btn, False);

	/* Update extt label */
	dbprog_extt_lblupd(NULL);

	dbprog_progclear((curstat_t *)(void *) client_data);
}


/*
 * dbprog_send
 *	CDDB Send pushbutton callback.
 */
/*ARGSUSED*/
void
dbprog_send(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	static char	*msg = NULL;

	if (s->mode == MOD_BUSY || s->mode == MOD_NODISC ||
	    cur_db.discid == 0 || cur_db.dbfile == NULL) {
		/* Nothing to send */
		cd_beep();
		return;
	}

	if (app_data.cddb_mailsite == NULL ||
	    app_data.cddb_mailsite[0] == '\0' ||
	    app_data.cddb_mailcmd == NULL ||
	    app_data.cddb_mailcmd[0] == '\0') {
		/* No mail site or mail command specified */
		cd_beep();
		return;
	}

	if (msg == NULL) {
		msg = (char *) MEM_ALLOC(
			"msg",
			strlen(app_data.str_send) +
			strlen(app_data.cddb_mailsite)
		);
		if (msg == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
	}

	(void) sprintf(msg, app_data.str_send, app_data.cddb_mailsite);

	cd_confirm_popup(
		app_data.str_confirm, msg,
		(XtCallbackProc) dbprog_dbsend, client_data,
		(XtCallbackProc) NULL, NULL
	);
}


/*
 * dbprog_save
 *	In-core CD database SAVE button callback.
 */
/*ARGSUSED*/
void
dbprog_save(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	cddb_path_t	*pp;
	bool_t		found_local;

	if (cur_db.discid == 0) {
		cd_beep();

		/* Clear flag */
		s->flags &= ~STAT_SAVE;

		if (s->flags & STAT_CLEAR)
			dbprog_dbclear(s, FALSE);

		return;
	}

	/* Look for local database directories */
	found_local = FALSE;
	for (pp = cddb_pathlist(); pp != NULL; pp = pp->next) {
		if (pp->type == CDDB_LOCAL) {
			found_local = TRUE;
			break;
		}
	}

	if (!found_local) {
		/* No database directory */
		CD_INFO(app_data.str_nodb);

		/* Clear flag */
		s->flags &= ~STAT_SAVE;

		if (s->flags & STAT_CLEAR)
			dbprog_dbclear(s, FALSE);

		return;
	}

	dirsel_mode = DIRSEL_SAVE;

	if (cur_db.dbfile == NULL) {
		/* Pop up the database directory selection popup */
		XmListDeselectAllItems(widgets.dirsel.dir_list);
		XmTextSetString(widgets.dbprog.ttitle_txt, "");

		XtManageChild(widgets.dirsel.form);

		XmProcessTraversal(
			widgets.dirsel.ok_btn,
			XmTRAVERSE_CURRENT
		);
	}
	else {
		/* Save to file directly */
		dbprog_dbput(s);

		/* Clear flag */
		s->flags &= ~STAT_SAVE;

		if (s->flags & STAT_CLEAR)
			dbprog_dbclear(s, FALSE);
	}
}


/*
 * dbprog_load
 *	CD database file LOAD button callback.
 */
/*ARGSUSED*/
void
dbprog_load(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	if (!di_check_disc(s) || s->mode == MOD_BUSY || s->mode == MOD_NODISC) {
		cd_beep();
		return;
	}

	/* Clear the in-core entry */
	dbprog_dbclear(s, TRUE);

	/* Re-load from database */
	dbprog_dbget(s);
}


/*
 * dbprog_link
 *	CD Database file search-link button callback.
 */
/*ARGSUSED*/
void
dbprog_link(Widget w, XtPointer client_data, XtPointer call_data)
{
	cddb_path_t	*pp;
	bool_t		found_local;

	if (cur_db.discid == 0) {
		cd_beep();
		return;
	}

	dirsel_mode = DIRSEL_LINK;

	if (cur_db.dbfile != NULL) {
		MEM_FREE(cur_db.dbfile);
		cur_db.dbfile = NULL;
	}

	/* Look for local database directories */
	found_local = FALSE;
	for (pp = cddb_pathlist(); pp != NULL; pp = pp->next) {
		if (pp->type == CDDB_LOCAL) {
			found_local = TRUE;
			break;
		}
	}

	if (!found_local) {
		/* No database directory */
		CD_INFO(app_data.str_nodb);
		return;
	}

	/* Pop up the database directory selection popup */
	XmListDeselectAllItems(widgets.dirsel.dir_list);
	XmTextSetString(widgets.dbprog.ttitle_txt, "");

	XmProcessTraversal(
		widgets.dirsel.ok_btn,
		XmTRAVERSE_CURRENT
	);

	XtManageChild(widgets.dirsel.form);
}


/*
 * dbprog_cancel
 *	Pop down CD database/program window.
 */
/*ARGSUSED*/
void
dbprog_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmPushButtonCallbackStruct
			*p = (XmPushButtonCallbackStruct *)(void *) call_data;
	curstat_t	*s = (curstat_t *)(void *) client_data;

	if (XtIsManaged(widgets.wwwwarp.form)) {
		/* Force a popdown of the wwwWarp window */
		dbprog_wwwwarp_cancel(
			widgets.wwwwarp.cancel_btn,
			(XtPointer) s,
			(XtPointer) p
		);

		wwwwarp_manage = TRUE;
	}
	if (XtIsManaged(widgets.dbextd.form)) {
		/* Force a popdown of the extd window */
		dbprog_extd_cancel(
			widgets.dbextd.cancel_btn,
			(XtPointer) s,
			(XtPointer) p
		);

		extd_manage = TRUE;
	}
	if (XtIsManaged(widgets.dbextt.form)) {
		/* Force a popdown of the extt window */
		dbprog_extt_cancel(
			widgets.dbextt.cancel_btn,
			(XtPointer) s,
			(XtPointer) p
		);

		extt_manage = TRUE;
	}

	/* Pop down the database/program dialog */
	XtUnmanageChild(widgets.dbprog.form);
}


/*
 * dbprog_do_clear
 *	Changed-save dialog box pushbuttons callback.
 */
/*ARGSUSED*/
void
dbprog_do_clear(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = curstat_addr();
	word32_t	flags = (word32_t) client_data;

	cur_db.flags &= ~CDDB_CHANGED;
	s->flags |= (flags & 0xffff);

	/* Clear in-core database */
	dbprog_dbclear(s, FALSE);
}


/*
 * dbprog_timedpy
 *	Toggle the time display mode in the track list.
 */
/*ARGSUSED*/
void
dbprog_timedpy(Widget w, XtPointer client_data, XtPointer call_data)
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

	/* Overload the function of these buttons to also
	 * dump the contents of the cddb_incore_t structure
	 * in debug mode
	 */
	if (app_data.debug)
		cddb_dump_incore(&cur_db, s);

	if (p->widget == widgets.dbprog.tottime_btn) {
		if (timemode == TIME_TOTAL)
			return;	/* No change */

		timemode = TIME_TOTAL;
	}
	else if (p->widget == widgets.dbprog.trktime_btn) {
		if (timemode == TIME_TRACK)
			return;	/* No change */

		timemode = TIME_TRACK;
	}
	else
		return;	/* Invalid widget */

	if (di_check_disc(s) && s->mode != MOD_BUSY && s->mode != MOD_NODISC)
		/* Update track list with new time display mode */
		dbprog_listupd_all(s, FALSE);
}


/*
 * dbprog_extd
 *	Pop up/down the disc extended info window.
 */
void
dbprog_extd(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	if (XtIsManaged(widgets.dbextd.form)) {
		/* Pop down the Disc Extended Info window */
		dbprog_extd_ok(w, client_data, call_data);
		return;
	}

	/* Update the extd disc title */
	dbprog_set_extd_title(s->cur_disc, cur_db.dtitle);

	/* Pop up the Disc Extended Info window */
	XtManageChild(widgets.dbextd.form);

	/* Put input focus on the cancel button */
	XmProcessTraversal(widgets.dbextd.cancel_btn, XmTRAVERSE_CURRENT);

	extd_manage = FALSE;
}


/*
 * dbprog_extt
 *	Pop up/down the track extended info window.
 */
void
dbprog_extt(Widget w, XtPointer client_data, XtPointer call_data)
{
	int		i,
			j;
	bool_t		from_main = (bool_t)(int) client_data,
			managed;
	curstat_t	*s = curstat_addr();

	managed = (bool_t) XtIsManaged(widgets.dbextt.form);
	if (managed) {
		if (from_main) {
			/* Pop down the Track Extended Info window */
			dbprog_extt_ok(w, client_data, call_data);

			return;
		}
		else {
			/* Update structures */
			dbprog_exttupd();
			extt_pos = -1;
		}
	}

	if (sel_pos > 0) {
		j = sel_pos - 1;

		/* Enter extt setup mode */
		extt_setup = TRUE;

		/* Set extt track title */
		dbprog_set_extt_title(
			s->trkinfo[j].trkno,
			cur_db.trklist[j]
		);

		/* Track extended info text */
		if (cur_db.extt[j] != NULL)
			XmTextSetString(widgets.dbextt.trk_txt, cur_db.extt[j]);
		else
			XmTextSetString(widgets.dbextt.trk_txt, "");

		extt_pos = j;

		if (from_main) {
			/* Save a backup copy of the text in case the user
			 * wants to abort.  This code will ensure that the
			 * data is not saved twice while the extended track
			 * info window is popped up.
			 */
			for (i = 0; i < MAXTRACK; i++) {
				if (cur_db.sav_extt[i] == NULL &&
				    cur_db.extt[i] != NULL) {
					cur_db.sav_extt[i] = (char *)
						MEM_ALLOC(
						    "cur_db.sav_extt[n]",
						    strlen(cur_db.extt[i]) + 1
						);
					if (cur_db.sav_extt[i] == NULL) {
						CD_FATAL(app_data.str_nomemory);
						return;
					}
					(void) strcpy(
						cur_db.sav_extt[i],
						cur_db.extt[i]
					);
				}
			}
		}

		if (!managed) {
			/* Pop up the Track Extended Info popup */
			XtManageChild(widgets.dbextt.form);

			/* Put input focus on the cancel button */
			XmProcessTraversal(
				widgets.dbextt.cancel_btn,
				XmTRAVERSE_CURRENT
			);
		}

		extt_manage = FALSE;

		/* Exit extt setup mode */
		extt_setup = FALSE;
	}
}


/*
 * dbprog_set_changed
 *	Set the flag indicating that the user has made changes to the
 *	in-core CD database entry.
 */
/*ARGSUSED*/
void
dbprog_set_changed(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmAnyCallbackStruct	*p = (XmAnyCallbackStruct *)(void *) call_data;

	if (p->reason != XmCR_VALUE_CHANGED)
		return;

	/* Setup of the extt window is not a user change */
	if (!extt_setup) {
		cur_db.flags |= CDDB_CHANGED;
		XtSetSensitive(widgets.dbprog.savedb_btn, True);
	}
}


/*
 * dbprog_extd_ok
 *	Extended disc info window OK button callback.
 */
/*ARGSUSED*/
void
dbprog_extd_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Pop down the Disc Extended Info popup */
	XtUnmanageChild(widgets.dbextd.form);

	/* Update structures */
	dbprog_extdupd();
}


/*
 * dbprog_extd_clear
 *	Extended disc info window Clear button callback.
 */
/*ARGSUSED*/
void
dbprog_extd_clear(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmTextSetString(widgets.dbextd.disc_txt, "");
}


/*
 * dbprog_extd_cancel
 *	Extended disc info window Cancel button callback.
 */
/*ARGSUSED*/
void
dbprog_extd_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	bool_t	sav_chg;

	/* Pop down the Disc Extended Info popup */
	XtUnmanageChild(widgets.dbextd.form);

	sav_chg = (cur_db.flags & CDDB_CHANGED);

	/* Restore original text */
	if (cur_db.extd == NULL)
		XmTextSetString(widgets.dbextd.disc_txt, "");
	else
		XmTextSetString(widgets.dbextd.disc_txt, cur_db.extd);

	/* Update extd label */
	dbprog_extd_lblupd();

	if (sav_chg)
		cur_db.flags |= CDDB_CHANGED;
	else
		cur_db.flags &= ~CDDB_CHANGED;

	if (((cur_db.type == CDDB_LOCAL) || (cur_db.flags & CDDB_AUTOSAVE)) &&
	    !(cur_db.flags & CDDB_CHANGED))
		XtSetSensitive(widgets.dbprog.savedb_btn, False);
}


/*
 * dbprog_extt_prev
 *	Extended track info window Previous button callback.
 */
/*ARGSUSED*/
void
dbprog_extt_prev(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	/* Put input focus on the cancel button */
	XmProcessTraversal(widgets.dbextt.cancel_btn, XmTRAVERSE_CURRENT);

	if (extt_pos <= 0) {
		cd_beep();
		return;
	}

	/* Select the previous track */
	if (sel_pos > 0) {
		XmListDeselectPos(widgets.dbprog.trk_list, sel_pos);
		sel_pos = -1;
	}
	XmListSelectPos(widgets.dbprog.trk_list, extt_pos, True);

	/* Scroll track list if necessary */
	dbprog_trklist_autoscroll(s, extt_pos + 1);
}


/*
 * dbprog_extt_next
 *	Extended track info window Next button callback.
 */
/*ARGSUSED*/
void
dbprog_extt_next(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	int		n;

	/* Put keyboard focus on the cancel button */
	XmProcessTraversal(widgets.dbextt.cancel_btn, XmTRAVERSE_CURRENT);

	if (extt_pos < 0) {
		cd_beep();
		return;
	}

	n = extt_pos + 2;
	if (n > (int) s->tot_trks) {
		cd_beep();
		return;
	}

	/* Select the next track */
	if (sel_pos > 0) {
		XmListDeselectPos(widgets.dbprog.trk_list, sel_pos);
		sel_pos = -1;
	}
	XmListSelectPos(widgets.dbprog.trk_list, n, True);

	/* Scroll track list if necessary */
	dbprog_trklist_autoscroll(s, n);
}


/*
 * dbprog_extt_autotrk
 *	Extended track info window Auto-track button callback.
 */
/*ARGSUSED*/
void
dbprog_extt_autotrk(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmToggleButtonCallbackStruct	*p =
		(XmToggleButtonCallbackStruct *)(void *) call_data;
	curstat_t			*s =
		(curstat_t *)(void *) client_data;

	if (p->reason != XmCR_VALUE_CHANGED)
		return;

	DBGPRN(errfp, "\n* AUTO-TRACK: %s\n", p->set ? "On" : "Off");

	auto_trk = (bool_t) p->set;

	dbprog_extt_autotrk_upd(s, di_curtrk_pos(s) + 1);
}


/*
 * dbprog_extt_ok
 *	Extended track info window OK button callback.
 */
/*ARGSUSED*/
void
dbprog_extt_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
	int	i;

	/* Pop down the Track Extended Info popup */
	XtUnmanageChild(widgets.dbextt.form);

	/* Update structures */
	dbprog_exttupd();
	extt_pos = -1;

	/* Update extt label */
	dbprog_extt_lblupd((sel_pos > 0) ? cur_db.extt[sel_pos - 1] : NULL);

	/* Delete backup text */
	for (i = 0; i < MAXTRACK; i++) {
		if (cur_db.sav_extt[i] != NULL) {
			MEM_FREE(cur_db.sav_extt[i]);
			cur_db.sav_extt[i] = NULL;
		}
	}
}


/*
 * dbprog_extt_clear
 *	Extended track info window Clear button callback.
 */
/*ARGSUSED*/
void
dbprog_extt_clear(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmTextSetString(widgets.dbextt.trk_txt, "");
}


/*
 * dbprog_extt_cancel
 *	Extended track info window Cancel button callback.
 */
/*ARGSUSED*/
void
dbprog_extt_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	int	i;

	/* Pop down the Track Extended Info popup */
	XtUnmanageChild(widgets.dbextt.form);

	/* Restore backup text */
	for (i = 0; i < MAXTRACK; i++) {
		if (cur_db.extt[i] != NULL)
			MEM_FREE(cur_db.extt[i]);

		cur_db.extt[i] = cur_db.sav_extt[i];
		cur_db.sav_extt[i] = NULL;
	}
}


/*
 * dbprog_dirsel_select
 *	CD Database directory selection list callback.
 */
/*ARGSUSED*/
void
dbprog_dirsel_select(Widget w, XtPointer client_data, XtPointer call_data)
{
	int			i;
	XmListCallbackStruct	*p = (XmListCallbackStruct *)(void *) call_data;
	cddb_path_t		*pp;

	if (p->reason != XmCR_BROWSE_SELECT)
		return;

	if (cur_db.dbfile != NULL) {
		MEM_FREE(cur_db.dbfile);
		cur_db.dbfile = NULL;
	}

	/* Look for matching CDDB path entry */
	i = 0;
	for (pp = cddb_pathlist(); pp != NULL; pp = pp->next) {
		if (pp->type != CDDB_LOCAL)
			continue;

		if (++i == p->item_position)
			break;
	}

	if (i != p->item_position) {
		/* Error: this shouldn't happen */
		return;
	}

	cur_db.dbfile = (char *) MEM_ALLOC(
		"cur_db.dbfile",
		strlen(pp->path) + 10
	);
	if (cur_db.dbfile == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	(void) sprintf(cur_db.dbfile, CDDBFILE_PATH, pp->path, cur_db.queryid);
	(void) strncpy(cur_db.category, cddb_category(pp->path),
		       FILE_BASE_SZ - 1);
	cur_db.category[FILE_BASE_SZ - 1] = '\0';
}


/*
 * dbprog_dirsel_ok
 *	CD Database directory selection window OK button callback.
 */
/*ARGSUSED*/
void
dbprog_dirsel_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	if (cur_db.dbfile == NULL) {
		/* User has not selected a directory yet */
		cd_beep();
		return;
	}

	/* Pop down the database directory selector popup dialog */
	XtUnmanageChild(widgets.dirsel.form);

	switch (dirsel_mode) {
	case DIRSEL_SAVE:
		/* Save the database entry to output file */
		dbprog_dbput(s);
		break;

	case DIRSEL_LINK:
		/* Link the database entry to another file */
		dbprog_dblink(s);
		break;

	default:
		/* Shouldn't get here */
		break;
	}

	/* Clear flag */
	s->flags &= ~STAT_SAVE;

	if (s->flags & STAT_CLEAR)
		dbprog_dbclear(s, FALSE);
}


/*
 * dbprog_dirsel_cancel
 *	CD Database directory selection window Cancel button callback.
 */
/*ARGSUSED*/
void
dbprog_dirsel_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	/* Pop down the database directory selector popup dialog */
	XtUnmanageChild(widgets.dirsel.form);

	/* Clear database file path */
	if (cur_db.dbfile != NULL) {
		MEM_FREE(cur_db.dbfile);
		cur_db.dbfile = NULL;
	}

	/* Clear flag */
	s->flags &= ~STAT_SAVE;

	if (s->flags & STAT_CLEAR)
		dbprog_dbclear(s, FALSE);
}


/*
 * dbprog_linksel_select
 *	Search-link selector list user-selection callback.
 */
/*ARGSUSED*/
void
dbprog_linksel_select(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmListCallbackStruct	*p = (XmListCallbackStruct *)(void *) call_data;

	if (p->reason != XmCR_BROWSE_SELECT)
		return;

	linksel_pos = p->item_position;
}


/*
 * dbprog_linksel_ok
 *	Search-link selector window OK button callback.
 */
void
dbprog_linksel_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	char		errstr[ERR_BUF_SZ],
			ltarget[FILE_PATH_SZ];
	int		i;
	cddb_ret_t	ret;
	cddb_linkopts_t	*q;

	if (linksel_pos <= 0) {
		/* User has not selected a link target yet */
		cd_beep();
		return;
	}

	/* Pop down the link selector popup dialog */
	XtUnmanageChild(widgets.linksel.form);

	for (i = 0, q = cddb_linkopts(); q != NULL; i++, q = q->next)
		if (i == linksel_pos - 1)
			break;

	if (q == NULL)
		/* This should not happen */
		return;

	(void) sprintf(ltarget, CONCAT_PATH, util_dirname(cur_db.dbfile),
		       q->idstr);

	/* Change to watch cursor */
	cd_busycurs(TRUE, CURS_ALL);

	/* Do the link */
	if ((ret = cddb_link(&cur_db, ltarget, cur_db.dbfile)) != 0) {
		DBGPRN(errfp, "cddb_link: status=%d arg=%d\n",
			CDDB_GET_STAT(ret), CDDB_GET_ARG(ret));
	}

	/* Change to normal cursor */
	cd_busycurs(FALSE, CURS_ALL);

	/* Free link options list */
	cddb_free_linkopts();

	switch (CDDB_GET_STAT(ret)) {
	case 0:
		break;

	case SETUID_ERR:
		(void) sprintf(errstr, app_data.str_lnkerr_suid,
			       util_get_ouid(), util_get_ogid());
		CD_WARNING(errstr);
		return;

	case KILLED_ERR:
		(void) sprintf(errstr, app_data.str_saverr_killed,
			       CDDB_GET_ARG(ret));
		CD_WARNING(errstr);
		return;

	case OPEN_ERR:
		(void) sprintf(errstr, app_data.str_saverr_open);
		CD_WARNING(errstr);
		return;

	case LINK_ERR:
	case MEM_ERR:
	default:
		(void) sprintf(errstr, app_data.str_lnkerr_link);
		CD_WARNING(errstr);
		return;
	}

	/* Load new database entry */
	dbprog_load(w, client_data, call_data);

	/* All edits have been saved, so clear flag */
	cur_db.flags &= ~CDDB_CHANGED;

	/* Update display */
	dbprog_dpyid();
	dpy_dbmode(s, FALSE);
	dpy_ttitle(s);

	/* Update curfile */
	dbprog_curfileupd();

	XtSetSensitive(widgets.dbprog.send_btn, True);
	XtSetSensitive(widgets.dbprog.linkdb_btn, False);
	XtSetSensitive(widgets.dbprog.savedb_btn, False);
	XmProcessTraversal(
		widgets.dbprog.cancel_btn,
		XmTRAVERSE_CURRENT
	);
}


/*
 * dbprog_linksel_cancel
 *	Search-link selector window Cancel button callback.
 */
/*ARGSUSED*/
void
dbprog_linksel_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Pop down the link selector popup dialog */
	XtUnmanageChild(widgets.linksel.form);

	/* Free link options list */
	cddb_free_linkopts();

	/* Clear database file path */
	if (cur_db.dbfile != NULL) {
		MEM_FREE(cur_db.dbfile);
		cur_db.dbfile = NULL;
	}
}


/*
 * dbprog_rmtmatch_yes
 *	Remote CDDB inexact match confirmation window Yes button callback.
 */
/*ARGSUSED*/
void
dbprog_rmtmatch_yes(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	cddb_match_t	*p,
			*q;

	if (XtIsManaged(widgets.dialog.dbmatch))
		XtUnmanageChild(widgets.dialog.dbmatch);

	if ((p = cur_db.match_cur) == NULL)
		/* Shouldn't get here */
		return;

	/* Set new query id */
	cur_db.queryid = p->discid;
	(void) strncpy(cur_db.category, p->category, FILE_BASE_SZ - 1);
	cur_db.category[FILE_BASE_SZ - 1 ] = '\0';

	/* Clear match list */
        for (p = q = cur_db.matchlist; p != NULL; p = q) {
		q = p->next;
		MEM_FREE(p->dtitle);
		MEM_FREE(p);
	}
	cur_db.matchlist = cur_db.match_cur = NULL;

	/* Reload from database */
	dbprog_dbget(s);

	if (cur_db.type == CDDB_INVALID)
		cur_db.category[0] = '\0';

	/* Update display */
	dbprog_dpyid();
	dpy_dbmode(s, FALSE);

	/* Update curfile */
	dbprog_curfileupd();

	/* Clear flag */
	dbmatch_mode = FALSE;
}


/*
 * dbprog_rmtmatch_no
 *	Remote CDDB inexact match confirmation window No button callback.
 */
/*ARGSUSED*/
void
dbprog_rmtmatch_no(Widget w, XtPointer client_data, XtPointer call_data)
{
	cddb_match_t	*p;
	char		*msg;
	XmString	xs;

	if (XtIsManaged(widgets.dialog.dbmatch))
		XtUnmanageChild(widgets.dialog.dbmatch);

	if ((p = cur_db.match_cur) == NULL)
		/* Shouldn't get here */
		return;

	if (p->next == NULL) {
		/* This is the last entry */
		dbprog_rmtmatch_stop(w, client_data, call_data);
	}
	else {
		p = p->next;

		msg = (char *) MEM_ALLOC(
			"msg",
			strlen(app_data.str_rmtmatch) +
			strlen(p->category) +
			strlen(p->dtitle)
		);
		if (msg == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}

		/* Set up dialog box message string */
		(void) sprintf(msg, app_data.str_rmtmatch,
			       p->category, p->dtitle);

		xs = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);

		XtVaSetValues(widgets.dialog.dbmatch,
			XmNmessageString, xs,
			NULL
		);

		XmStringFree(xs);
		MEM_FREE(msg);

		/* Set pointer */
		cur_db.match_cur = p;

		/* Pop up dialog box */
		XtManageChild(widgets.dialog.dbmatch);
	}
}


/*
 * dbprog_rmtmatch_stop
 *	Remote CDDB inexact match confirmation window Stop button callback.
 */
/*ARGSUSED*/
void
dbprog_rmtmatch_stop(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	cddb_match_t	*p,
			*q;

	if (XtIsManaged(widgets.dialog.dbmatch))
		XtUnmanageChild(widgets.dialog.dbmatch);

	if ((p = cur_db.match_cur) == NULL)
		/* Shouldn't get here */
		return;

	/* Set flag */
	s->cddb = cur_db.type = CDDB_INVALID;

	/* Configure the wwwWarp search engine options menu */
	dbprog_wwwwarp_sel_cfg(s);

	/* Clear match list */
        for (p = q = cur_db.matchlist; p != NULL; p = q) {
		q = p->next;
		MEM_FREE(p->dtitle);
		MEM_FREE(p);
	}
	cur_db.matchlist = cur_db.match_cur = NULL;

	/* Update widgets */
	dbprog_structupd(s);

	XtSetSensitive(widgets.dbprog.linkdb_btn, True);
	XtSetSensitive(widgets.dbprog.loaddb_btn, True);

	/* Update display */
	dbprog_dpyid();
	dpy_dtitle(s);
	dpy_ttitle(s);

	/* Update curfile */
	dbprog_curfileupd();

	/* Clear flag */
	dbmatch_mode = FALSE;
}


/*
 * dbprog_auth_retry
 *	Let the user have the option of retrying the proxy-authorization.
 */
/*ARGSUSED*/
void
dbprog_auth_retry(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = curstat_addr();

	if (client_data) {
		/* Clear out previously entered password */
		XmTextSetString(widgets.auth.pass_txt, "");
		if (auth_passwd != NULL) {
			(void) memset(auth_passwd, 0, strlen(auth_passwd));
			MEM_FREE(auth_passwd);
			auth_passwd = NULL;
		}

		/* Pop up authorization dialog */
		if (!XtIsManaged(widgets.auth.form))
			XtManageChild(widgets.auth.form);

		/* Set keyboard focus to password field */
		XmProcessTraversal(
			widgets.auth.pass_txt,
			XmTRAVERSE_CURRENT
		);
	}
	else {
		/* Auth failed: cannot get CDDB info */
		if (auth_passwd != NULL) {
			(void) memset(auth_passwd, 0, strlen(auth_passwd));
			MEM_FREE(auth_passwd);
			auth_passwd = NULL;
		}

		/* Set the cddb indicator */
		cur_db.type = CDDB_INVALID;
		s->cddb = (byte_t) cur_db.type;

		/* Update widgets */
		dbprog_structupd(s);

		/* Configure the wwwWarp search engine options menu */
		dbprog_wwwwarp_sel_cfg(s);

		XtSetSensitive(widgets.dbprog.linkdb_btn, True);
		XtSetSensitive(widgets.dbprog.loaddb_btn, True);

		/* Update display */
		dbprog_dpyid();
		dpy_dtitle(s);
		dpy_ttitle(s);

		/* Update curfile */
		dbprog_curfileupd();
	}
}


/*
 * dbprog_auth_name
 *	Callback for the proxy-authorization user name text widget
 */
/*ARGSUSED*/
void
dbprog_auth_name(Widget w, XtPointer client_data, XtPointer call_data)
{
	/* Change keyboard focus to the password field */
	XmProcessTraversal(widgets.auth.pass_txt, XmTRAVERSE_CURRENT);
}


/*
 * dbprog_auth_password_vfy
 *	Verify Callback for the proxy-authorization password text widget
 */
/*ARGSUSED*/
void
dbprog_auth_password_vfy(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmTextVerifyCallbackStruct
		*p = (XmTextVerifyCallbackStruct *)(void *) call_data;
	char	*str,
		*tp,
		tc;
	int	i,
		len,
		start;

	if (p->reason != XmCR_MODIFYING_TEXT_VALUE)
		return;

	if (p->text->length >= 1 && p->text->ptr != NULL) {
		if (auth_passwd == NULL) {
			auth_passwd = (char *) MEM_ALLOC(
				"auth_passwd",
				p->text->length + 1
			);
			if (auth_passwd == NULL) {
				CD_FATAL(app_data.str_nomemory);
				return;
			}

			(void) strncpy(auth_passwd, p->text->ptr,
				       p->text->length);
			auth_passwd[p->text->length] = '\0';
		}
		else {
			len = strlen(auth_passwd);
			start = (p->startPos < len) ? p->startPos : len;
			tp = &auth_passwd[start];
			tc = *tp;
			*tp = '\0';

			str = (char *) MEM_ALLOC(
				"auth_password_vfy_str",
				len + p->text->length + 1
			);
			if (str == NULL) {
				CD_FATAL(app_data.str_nomemory);
				return;
			}

			(void) strcpy(str, auth_passwd);
			(void) strncat(str, p->text->ptr, p->text->length);
			str[start + p->text->length] = '\0';
			*tp = tc;
			(void) strcat(str, tp);

			(void) memset(auth_passwd, 0, strlen(auth_passwd));
			MEM_FREE(auth_passwd);
			auth_passwd = str;
		}

		/* Display '*' instead of what the user typed */
		for (i = 0; i < p->text->length; i++)
			p->text->ptr[i] = '*';

		p->doit = True;
	}
	else if (p->text->length == 0) {
		/* backspace */
		if (auth_passwd != NULL && *auth_passwd != '\0') {
			len = strlen(auth_passwd);

			start = (p->startPos < len) ? p->startPos : (len - 1);
			tp = &auth_passwd[(p->endPos > len) ? len : p->endPos];

			auth_passwd[start] = '\0';
			(void) strcat(auth_passwd, tp);
		}

		p->doit = True;
	}
}


/*
 * dbprog_auth_ok
 *	Callback for the proxy-authorization OK button
 */
/*ARGSUSED*/
void
dbprog_auth_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;
	char		*name;

	if (XtIsManaged(widgets.auth.form))
		XtUnmanageChild(widgets.auth.form);

	auth_initted = TRUE;

	/* Set proxy auth user and password */
	name = XmTextGetString(widgets.auth.name_txt);

	cddb_set_auth(name, auth_passwd == NULL ? "" : auth_passwd);

	(void) memset(name, 0, strlen(name));
	XtFree(name);
	if (auth_passwd != NULL) {
		(void) memset(auth_passwd, 0, strlen(auth_passwd));
		MEM_FREE(auth_passwd);
		auth_passwd = NULL;
	}

	/* Re-load from database */
	dbprog_dbget(s);
}


/*
 * dbprog_auth_cancel
 *	Callback for the proxy-authorization Cancel button
 */
/*ARGSUSED*/
void
dbprog_auth_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (XtIsManaged(widgets.auth.form))
		XtUnmanageChild(widgets.auth.form);

	dbprog_auth_retry(w, NULL, call_data);
}


/*
 * dbprog_wwwwarp_select
 *	wwwWarp selection callback function
 */
/*ARGSUSED*/
void
dbprog_wwwwarp_select(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmRowColumnCallbackStruct	*p =
		(XmRowColumnCallbackStruct *)(void *) call_data;
	XmToggleButtonCallbackStruct	*q;

	if (p == NULL)
		return;

	q = (XmToggleButtonCallbackStruct *)(void *) p->callbackstruct;

	if (!q->set)
		return;

	DBGPRN(errfp, "wwwWarp: %s selected.\n", XtName(p->widget));

	if (p->widget == widgets.wwwwarp.site1_btn)
		website_idx = WWWWARP_1;
	else if (p->widget == widgets.wwwwarp.site2_btn)
		website_idx = WWWWARP_2;
	else if (p->widget == widgets.wwwwarp.site3_btn)
		website_idx = WWWWARP_3;
	else if (p->widget == widgets.wwwwarp.srchweb_btn)
		website_idx = WWWWARP_SRCHENG;

	if (website_idx == WWWWARP_SRCHENG)
		XtSetSensitive(widgets.wwwwarp.srchsite_opt, True);
	else
		XtSetSensitive(widgets.wwwwarp.srchsite_opt, False);
}


/*
 * dbprog_wwwwarp_cancel
 *	wwwWarp cancel button callback function
 */
/*ARGSUSED*/
void
dbprog_wwwwarp_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (XtIsManaged(widgets.wwwwarp.form))
		XtUnmanageChild(widgets.wwwwarp.form);
}


/*
 * dbprog_wwwwarp_go
 *	wwwWarp OK button callback function
 */
/*ARGSUSED*/
void
dbprog_wwwwarp_go(Widget w, XtPointer client_data, XtPointer call_data)
{
	curstat_t	*s = (curstat_t *)(void *) client_data;

	/* Change to watch cursor */
	cd_busycurs(TRUE, CURS_WWWWARP);

	switch (website_idx) {
	case WWWWARP_1:
		dbprog_wwwwarp_site(app_data.site1_url);
		break;
	case WWWWARP_2:
		dbprog_wwwwarp_site(app_data.site2_url);
		break;
	case WWWWARP_3:
		dbprog_wwwwarp_site(app_data.site3_url);
		break;
	case WWWWARP_SRCHENG:
		if (cur_srcheng == NULL || cur_srcheng->action == NULL) {
			cd_beep();
			break;
		}

		dbprog_wwwwarp_srcheng(s);
		break;
	default:
		cd_beep();
		break;
	}

	/* Change to normal cursor */
	cd_busycurs(FALSE, CURS_WWWWARP);
}


/*
 * dbprog_wwwwarp
 *	wwwWarp button callback function
 */
void
dbprog_wwwwarp(Widget w, XtPointer client_data, XtPointer call_data)
{
	if (XtIsManaged(widgets.wwwwarp.form)) {
		/* Pop wwwwarp window down */
		dbprog_wwwwarp_cancel(w, client_data, call_data);
		return;
	}

	/* Pop wwwWarp window up */
	XtManageChild(widgets.wwwwarp.form);

	/* Set keyboard focus on the Go button */
	XmProcessTraversal(
		widgets.wwwwarp.go_btn,
		XmTRAVERSE_CURRENT
	);

	wwwwarp_manage = FALSE;
}


/*
 * dbprog_srcheng_sel
 *	wwwWarp search engine selector menu callback function
 */
/*ARGSUSED*/
void
dbprog_srcheng_sel(Widget w, XtPointer client_data, XtPointer call_data)
{
	srcheng_ent_t	*p = (srcheng_ent_t *)(void *) client_data;

	if (w != (Widget) p->aux)
		return;

	cur_srcheng = p;
	DBGPRN(errfp, "wwwWarp: search engine set to \"%s\"\n", p->name);
}


/**************** ^^ Callback routines ^^ ****************/

