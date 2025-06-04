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
static char *_help_c_ident_ = "@(#)help.c	6.47 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "common_d/patchlevel.h"
#include "xmcd_d/xmcd.h"
#include "xmcd_d/widget.h"
#include "xmcd_d/cdfunc.h"
#include "xmcd_d/help.h"


/* This structure is used to map widgets to associated help files.
 * Instead of using XtName(), this mechanism allows us to map multiple
 * widgets to a common help file.  Also, we can use arbitrary lengths
 * for the widget name and still have help files with less than 14 chars
 * in its name (necessary for compatibility with some systems).
 */
typedef struct {
	Widget	widget;
	char	*hlpname;
} wname_t;


extern appdata_t	app_data;
extern widgets_t	widgets;
extern FILE		*errfp;

STATIC wname_t		wname[MAX_HELP_WIDGETS];
STATIC doc_topic_t	*dochead,
			*doctail;


/***********************
 *  internal routines  *
 ***********************/

/*
 * help_mapinit
 *	Initialize the widget-to-helpfile_name mapping table.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
STATIC void
help_mapinit(void)
{
	int	i = 0;

	wname[i].widget = widgets.main.mode_btn;
	wname[i++].hlpname = "Mode.btn";
	wname[i].widget = widgets.main.lock_btn;
	wname[i++].hlpname = "Main.cbx";
	wname[i].widget = widgets.main.repeat_btn;
	wname[i++].hlpname = "Main.cbx";
	wname[i].widget = widgets.main.shuffle_btn;
	wname[i++].hlpname = "Main.cbx";
	wname[i].widget = widgets.main.eject_btn;
	wname[i++].hlpname = "Eject.btn";
	wname[i].widget = widgets.main.quit_btn;
	wname[i++].hlpname = "Quit.btn";
	wname[i].widget = widgets.main.dbprog_btn;
	wname[i++].hlpname = "DbProg.btn";
	wname[i].widget = widgets.main.options_btn;
	wname[i++].hlpname = "Options.btn";
	wname[i].widget = widgets.main.time_btn;
	wname[i++].hlpname = "Time.btn";
	wname[i].widget = widgets.main.ab_btn;
	wname[i++].hlpname = "Ab.btn";
	wname[i].widget = widgets.main.sample_btn;
	wname[i++].hlpname = "Sample.btn";
	wname[i].widget = widgets.main.keypad_btn;
	wname[i++].hlpname = "Keypad.btn";
	wname[i].widget = widgets.main.help_btn;
	wname[i++].hlpname = "Help.btn";
	wname[i].widget = widgets.main.level_scale;
	wname[i++].hlpname = "Level.scl";
	wname[i].widget = widgets.main.playpause_btn;
	wname[i++].hlpname = "PlayPaus.btn";
	wname[i].widget = widgets.main.stop_btn;
	wname[i++].hlpname = "Stop.btn";
	wname[i].widget = widgets.main.prevdisc_btn;
	wname[i++].hlpname = "PrevDisc.btn";
	wname[i].widget = widgets.main.nextdisc_btn;
	wname[i++].hlpname = "NextDisc.btn";
	wname[i].widget = widgets.main.prevtrk_btn;
	wname[i++].hlpname = "PrevTrk.btn";
	wname[i].widget = widgets.main.nexttrk_btn;
	wname[i++].hlpname = "NextTrk.btn";
	wname[i].widget = widgets.main.previdx_btn;
	wname[i++].hlpname = "PrevIdx.btn";
	wname[i].widget = widgets.main.nextidx_btn;
	wname[i++].hlpname = "NextIdx.btn";
	wname[i].widget = widgets.main.rew_btn;
	wname[i++].hlpname = "Rew.btn";
	wname[i].widget = widgets.main.ff_btn;
	wname[i++].hlpname = "Ff.btn";
	wname[i].widget = widgets.main.disc_ind;
	wname[i++].hlpname = "Disc.lbl";
	wname[i].widget = widgets.main.track_ind;
	wname[i++].hlpname = "Track.lbl";
	wname[i].widget = widgets.main.index_ind;
	wname[i++].hlpname = "Index.lbl";
	wname[i].widget = widgets.main.time_ind;
	wname[i++].hlpname = "Time.lbl";
	wname[i].widget = widgets.main.rptcnt_ind;
	wname[i++].hlpname = "RptCnt.lbl";
	wname[i].widget = widgets.main.dbmode_ind;
	wname[i++].hlpname = "DbMode.lbl";
	wname[i].widget = widgets.main.progmode_ind;
	wname[i++].hlpname = "ProgMode.lbl";
	wname[i].widget = widgets.main.timemode_ind;
	wname[i++].hlpname = "TimeMode.lbl";
	wname[i].widget = widgets.main.playmode_ind;
	wname[i++].hlpname = "PlayMode.lbl";
	wname[i].widget = widgets.main.dtitle_ind;
	wname[i++].hlpname = "DiscTitl.lbl";
	wname[i].widget = widgets.main.ttitle_ind;
	wname[i++].hlpname = "TrkTitle.lbl";
	wname[i].widget = widgets.keypad.keypad_ind;
	wname[i++].hlpname = "Keypad.lbl";
	wname[i].widget = widgets.keypad.radio_box;
	wname[i++].hlpname = "KpSel.btn";
	wname[i].widget = widgets.keypad.num_btn[0];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[1];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[2];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[3];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[4];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[5];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[6];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[7];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[8];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.num_btn[9];
	wname[i++].hlpname = "KpNum.btn";
	wname[i].widget = widgets.keypad.clear_btn;
	wname[i++].hlpname = "KpClear.btn";
	wname[i].widget = widgets.keypad.enter_btn;
	wname[i++].hlpname = "KpEnter.btn";
	wname[i].widget = widgets.keypad.warp_lbl;
	wname[i++].hlpname = "KpWarp.scl";
	wname[i].widget = widgets.keypad.warp_scale;
	wname[i++].hlpname = "KpWarp.scl";
	wname[i].widget = widgets.keypad.cancel_btn;
	wname[i++].hlpname = "KpCancel.btn";
	wname[i].widget = widgets.options.load_chkbox;
	wname[i++].hlpname = "OpLoad.cbx";
	wname[i].widget = widgets.options.load_radbox;
	wname[i++].hlpname = "OpLoad.rbx";
	wname[i].widget = widgets.options.exit_radbox;
	wname[i++].hlpname = "OpExit.rbx";
	wname[i].widget = widgets.options.done_chkbox;
	wname[i++].hlpname = "OpDone.cbx";
	wname[i].widget = widgets.options.eject_chkbox;
	wname[i++].hlpname = "OpEject.cbx";
	wname[i].widget = widgets.options.chg_chkbox;
	wname[i++].hlpname = "OpChgr.cbx";
	wname[i].widget = widgets.options.chroute_radbox;
	wname[i++].hlpname = "OpChRt.rbx";
	wname[i].widget = widgets.options.vol_radbox;
	wname[i++].hlpname = "OpVolTpr.rbx";
	wname[i].widget = widgets.options.bal_lbl;
	wname[i++].hlpname = "OpBal.scl";
	wname[i].widget = widgets.options.bal_scale;
	wname[i++].hlpname = "OpBal.scl";
	wname[i].widget = widgets.options.ball_lbl;
	wname[i++].hlpname = "OpBal.scl";
	wname[i].widget = widgets.options.balr_lbl;
	wname[i++].hlpname = "OpBal.scl";
	wname[i].widget = widgets.options.balctr_btn;
	wname[i++].hlpname = "OpBalCtr.btn";
	wname[i].widget = widgets.options.reset_btn;
	wname[i++].hlpname = "OpReset.btn";
	wname[i].widget = widgets.options.save_btn;
	wname[i++].hlpname = "OpSave.btn";
	wname[i].widget = widgets.options.ok_btn;
	wname[i++].hlpname = "OpOk.btn";
	wname[i].widget = widgets.dbprog.tottime_lbl;
	wname[i++].hlpname = "DpTotTim.lbl";
	wname[i].widget = widgets.dbprog.tottime_ind;
	wname[i++].hlpname = "DpTotTim.lbl";
	wname[i].widget = widgets.dbprog.wwwwarp_btn;
	wname[i++].hlpname = "DpWeb.btn";
	wname[i].widget = widgets.dbprog.rmtdsbl_btn;
	wname[i++].hlpname = "DpRmDsbl.btn";
	wname[i].widget = widgets.dbprog.dtitle_txt;
	wname[i++].hlpname = "DpDTitle.txw";
	wname[i].widget = widgets.dbprog.extd_btn;
	wname[i++].hlpname = "DpDExt.btn";
	wname[i].widget = widgets.dbprog.trk_list;
	wname[i++].hlpname = "DpTrk.lsw";
	wname[i].widget = widgets.dbprog.addpgm_btn;
	wname[i++].hlpname = "DpAddPgm.btn";
	wname[i].widget = widgets.dbprog.clrpgm_btn;
	wname[i++].hlpname = "DpClrPgm.btn";
	wname[i].widget = widgets.dbprog.radio_box;
	wname[i++].hlpname = "DpTimSel.btn";
	wname[i].widget = widgets.dbprog.discid_ind;
	wname[i++].hlpname = "DiscId.lbl";
	wname[i].widget = widgets.dbprog.ttitle_txt;
	wname[i++].hlpname = "DpTTitle.txw";
	wname[i].widget = widgets.dbprog.extt_btn;
	wname[i++].hlpname = "DpTExt.btn";
	wname[i].widget = widgets.dbprog.pgmseq_txt;
	wname[i++].hlpname = "DpPgmSeq.txw";
	wname[i].widget = widgets.dbprog.send_btn;
	wname[i++].hlpname = "DpSend.btn";
	wname[i].widget = widgets.dbprog.savedb_btn;
	wname[i++].hlpname = "DpSave.btn";
	wname[i].widget = widgets.dbprog.linkdb_btn;
	wname[i++].hlpname = "DpLink.btn";
	wname[i].widget = widgets.dbprog.loaddb_btn;
	wname[i++].hlpname = "DpLoad.btn";
	wname[i].widget = widgets.dbprog.cancel_btn;
	wname[i++].hlpname = "DpCancel.btn";
	wname[i].widget = widgets.wwwwarp.sel_radbox;
	wname[i++].hlpname = "WwSelect.rbx";
	wname[i].widget = widgets.wwwwarp.srchsite_opt;
	wname[i++].hlpname = "WwSearch.opt";
	wname[i].widget = widgets.wwwwarp.go_btn;
	wname[i++].hlpname = "WwGo.btn";
	wname[i].widget = widgets.wwwwarp.cancel_btn;
	wname[i++].hlpname = "WwCancel.btn";
	wname[i].widget = widgets.dbextd.disc_txt;
	wname[i++].hlpname = "DdDisc.txw";
	wname[i].widget = widgets.dbextd.ok_btn;
	wname[i++].hlpname = "DdOk.btn";
	wname[i].widget = widgets.dbextd.clear_btn;
	wname[i++].hlpname = "DdClr.btn";
	wname[i].widget = widgets.dbextd.cancel_btn;
	wname[i++].hlpname = "DdCancel.btn";
	wname[i].widget = widgets.dbextt.prev_btn;
	wname[i++].hlpname = "DtDir.btn";
	wname[i].widget = widgets.dbextt.next_btn;
	wname[i++].hlpname = "DtDir.btn";
	wname[i].widget = widgets.dbextt.autotrk_btn;
	wname[i++].hlpname = "DtAutoTr.btn";
	wname[i].widget = widgets.dbextt.trk_txt;
	wname[i++].hlpname = "DtTrack.txw";
	wname[i].widget = widgets.dbextt.ok_btn;
	wname[i++].hlpname = "DtOk.btn";
	wname[i].widget = widgets.dbextt.clear_btn;
	wname[i++].hlpname = "DtClr.btn";
	wname[i].widget = widgets.dbextt.cancel_btn;
	wname[i++].hlpname = "DtCancel.btn";
	wname[i].widget = widgets.dirsel.dir_list;
	wname[i++].hlpname = "DsDir.lsw";
	wname[i].widget = widgets.dirsel.ok_btn;
	wname[i++].hlpname = "DsOk.btn";
	wname[i].widget = widgets.dirsel.cancel_btn;
	wname[i++].hlpname = "DsCancel.btn";
	wname[i].widget = widgets.linksel.link_list;
	wname[i++].hlpname = "LsLink.lsw";
	wname[i].widget = widgets.linksel.ok_btn;
	wname[i++].hlpname = "LsOk.btn";
	wname[i].widget = widgets.linksel.cancel_btn;
	wname[i++].hlpname = "LsCancel.btn";
	wname[i].widget = widgets.help.topic_opt;
	wname[i++].hlpname = "HpTopic.opt";
	wname[i].widget = widgets.help.help_txt;
	wname[i++].hlpname = "HpText.txw";
	wname[i].widget = widgets.help.about_btn;
	wname[i++].hlpname = "HpAbout.btn";
	wname[i].widget = widgets.help.cancel_btn;
	wname[i++].hlpname = "HpCancel.btn";
	wname[i].widget = (Widget) NULL;
	wname[i].hlpname = NULL;
}


/*
 * help_docinit
 *	Initialize the documentation topics list.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
STATIC void
help_docinit(void)
{
	DIR		*dp;
	struct dirent	*de;
	doc_topic_t	*p;
	struct stat	stbuf;
	char		docdir[FILE_PATH_SZ],
			fname[FILE_PATH_SZ];

	if (app_data.libdir == NULL)
		/* shrug */
		return;

	(void) sprintf(docdir, DOCFILE_PATH, app_data.libdir);
	if ((dp = OPENDIR(docdir)) == NULL) {
		DBGPRN(errfp, "Cannot open %s, no topics added.\n", docdir);
		return;
	}

	while ((de = READDIR(dp)) != NULL) {
		/* Skip ".", ".." and dot files */
		if (de->d_name[0] == '.')
			continue;

		(void) sprintf(fname, "%s%s", docdir, de->d_name);
		if (stat(fname, &stbuf) < 0)
			continue;

		/* Skip non-regular files */
		if (!S_ISREG(stbuf.st_mode))
			continue;

		p = (doc_topic_t *) MEM_ALLOC(
			"doc_topic_t",
			sizeof(doc_topic_t)
		);
		if (p == NULL) {
			(void) CLOSEDIR(dp);
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		p->next = NULL;
		p->name = (char *) MEM_ALLOC(
			"p->name",
			strlen(de->d_name) + 1
		);
		if (p->name == NULL) {
			(void) CLOSEDIR(dp);
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		p->path = (char *) MEM_ALLOC("p->path", strlen(fname) + 1);
		if (p->path == NULL) {
			(void) CLOSEDIR(dp);
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		(void) strcpy(p->name, de->d_name);
		(void) strcpy(p->path, fname);

		if (dochead == NULL)
			dochead = doctail = p;
		else if (strcmp(p->name, "README") == 0) {
			/* Put README file on top */
			p->next = dochead;
			dochead = p;
		}
		else {
			doctail->next = p;
			doctail = p;
		}
	}

	for (p = dochead; p != NULL; p = p->next) {
		p->actbtn = XmCreatePushButton(
			widgets.help.topic_menu,
			p->name,
			NULL,
			0
		);
		XtManageChild(p->actbtn);

		register_activate_cb(p->actbtn, help_topic_sel, p);
	}

	(void) CLOSEDIR(dp);
}


/*
 * help_getname
 *	Given a widget, return the associated help file name.
 *
 * Args:
 *	w - The widget
 *
 * Return:
 *	The help file name text string.
 */
STATIC char *
help_getname(Widget w)
{
	int	i;

	for (i = 0; wname[i].widget != NULL; i++) {
		if (w == wname[i].widget)
			return (wname[i].hlpname);
	}
	return NULL;
}


/*
 * help_loadfile
 *	Pop up the help window and display text from the specified file.
 *
 * Args:
 *	path - The file path
 *
 * Return:
 *	Nothing.
 */
STATIC void
help_loadfile(char *path)
{
	int		bufsz = STR_BUF_SZ * 2;
	char		*tmpbuf,
			*helptext;
	FILE		*fp;
#ifndef __VMS
	pid_t		cpid;
	waitret_t	stat_val;
	int		ret,
			pfd[2];
	FILE		*wfp;

	if (PIPE(pfd) < 0) {
		DBGPRN(errfp, "help_loadfile: pipe failed (errno=%d)\n",
			errno);
		cd_beep();
		return;
	}

	switch (cpid = FORK()) {
	case 0:
		/* Child */

		/* Close un-needed pipe descriptor */
		(void) close(pfd[0]);

		DBGPRN(errfp, "Setting uid to %d, gid to %d\n",
			(int) util_get_ouid(), (int) util_get_ogid());

		/* Force uid and gid to original setting */
		if (setuid(util_get_ouid()) < 0 ||
		    setgid(util_get_ogid()) < 0) {
			(void) close(pfd[1]);
			exit(1);
		}

		DBGPRN(errfp, "Help: loading %s\n", path);

		if ((fp = fopen(path, "r")) == NULL)
			DBGPRN(errfp, "Cannot open %s\n", path);

		if ((wfp = fdopen(pfd[1], "w")) == NULL) {
			DBGPRN(errfp,
				"help_loadfile: write pipe fdopen failed\n");
			(void) close(pfd[1]);
			exit(4);
		}

		if (fp == NULL) {
			(void) fprintf(wfp, "%s", app_data.str_nohelp);
			(void) fclose(wfp);
			exit(0);
		}

		/* Allocate temporary buffer */
		tmpbuf = (char *) MEM_ALLOC("help_loadfile_tmpbuf", bufsz);
		if (tmpbuf == NULL) {
			(void) close(pfd[1]);
			exit(2);
		}
		
		while (fgets(tmpbuf, bufsz, fp) != NULL)
			(void) fprintf(wfp, "%s", tmpbuf);

		(void) fclose(fp);
		(void) fclose(wfp);

		exit(0);
		/*NOTREACHED*/

	case -1:
		DBGPRN(errfp, "help_loadfile: fork failed (errno=%d)\n",
			errno);
		(void) close(pfd[0]);
		(void) close(pfd[1]);

		cd_beep();
		return;

	default:
		/* Parent */

		/* Close un-needed pipe descriptor */
		(void) close(pfd[1]);

		if ((fp = fdopen(pfd[0], "r")) == NULL) {
			DBGPRN(errfp,
			    "help_loadfile: read pipe fdopen failed\n");
			cd_beep();
			return;
		}
		break;
	}
#else
	DBGPRN(errfp, "Help: loading %s\n", path);

	if ((fp = fopen(path, "r")) == NULL)
		DBGPRN(errfp, "Cannot open %s\n", path);
#endif	/* __VMS */

	if (fp == NULL) {
		helptext = (char *) MEM_ALLOC(
			"helptext",
			strlen(app_data.str_nohelp) + 1
		);
		if (helptext == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}
		strcpy(helptext, app_data.str_nohelp);
	}
	else {
		/* Allocate temporary buffer */
		tmpbuf = (char *) MEM_ALLOC("help_tmpbuf", bufsz);
		if (tmpbuf == NULL) {
			CD_FATAL(app_data.str_nomemory);
			return;
		}

		helptext = NULL;

		while (fgets(tmpbuf, bufsz, fp) != NULL) {
			if (tmpbuf[0] == '#')
				/* Comment */
				continue;
			if (strncmp(tmpbuf, "@(#)", 4) == 0)
				/* SCCS ident */
				continue;

			if (helptext == NULL) {
				helptext = (char *) MEM_ALLOC(
					"helptext",
					strlen(tmpbuf) + 1
				);

				if (helptext != NULL)
					*helptext = '\0';
			}
			else {
				helptext = (char *) MEM_REALLOC(
					"helptext",
					helptext,
					strlen(helptext) + strlen(tmpbuf) + 1
				);
			}

			if (helptext == NULL) {
				CD_FATAL(app_data.str_nomemory);
				(void) fclose(fp);
				MEM_FREE(tmpbuf);
				return;
			}

			(void) strcat(helptext, tmpbuf);
		}

		(void) fclose(fp);
		MEM_FREE(tmpbuf);
	}

#ifndef __VMS
	/* Wait for child to finish */
	while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
		if (ret < 0)
			break;

		/* Service some events */
		event_loop(0);
	}
#endif

	/* Set new help text */
	XmTextSetString(widgets.help.help_txt, helptext);
	MEM_FREE(helptext);

	/* Pop it up */
	if (!XtIsManaged(widgets.help.form))
		XtManageChild(widgets.help.form);

	/* Set keyboard focus on the Cancel button */
	XmProcessTraversal(
		widgets.help.cancel_btn,
		XmTRAVERSE_CURRENT
	);
}


/***********************
 *   public routines   *
 ***********************/


/*
 * help_init
 *	Top level function to set up the help subsystem.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
help_init(void)
{
	int		i;
	XtTranslations	xtab1,
			xtab2;
	widgets_t	*m;

	m = &widgets;

	xtab1 = XtParseTranslationTable(
		"<Btn3Down>,<Btn3Up>: Help()\n"
	);
	xtab2 = XtParseTranslationTable(
		"<Btn3Down>,<Btn3Up>: PrimitiveHelp()\n"
	);

	/* Translations for the main window widgets */
	XtOverrideTranslations(m->main.mode_btn, xtab1);
	XtOverrideTranslations(m->main.lock_btn, xtab1);
	XtOverrideTranslations(m->main.repeat_btn, xtab1);
	XtOverrideTranslations(m->main.shuffle_btn, xtab1);
	XtOverrideTranslations(m->main.eject_btn, xtab1);
	XtOverrideTranslations(m->main.quit_btn, xtab1);
	XtOverrideTranslations(m->main.disc_ind, xtab1);
	XtOverrideTranslations(m->main.track_ind, xtab1);
	XtOverrideTranslations(m->main.index_ind, xtab1);
	XtOverrideTranslations(m->main.time_ind, xtab1);
	XtOverrideTranslations(m->main.rptcnt_ind, xtab1);
	XtOverrideTranslations(m->main.dbmode_ind, xtab1);
	XtOverrideTranslations(m->main.progmode_ind, xtab1);
	XtOverrideTranslations(m->main.timemode_ind, xtab1);
	XtOverrideTranslations(m->main.playmode_ind, xtab1);
	XtOverrideTranslations(m->main.dtitle_ind, xtab1);
	XtOverrideTranslations(m->main.ttitle_ind, xtab1);
	XtOverrideTranslations(m->main.dbprog_btn, xtab1);
	XtOverrideTranslations(m->main.options_btn, xtab1);
	XtOverrideTranslations(m->main.time_btn, xtab1);
	XtOverrideTranslations(m->main.ab_btn, xtab1);
	XtOverrideTranslations(m->main.sample_btn, xtab1);
	XtOverrideTranslations(m->main.keypad_btn, xtab1);
	XtOverrideTranslations(m->main.help_btn, xtab1);
	XtOverrideTranslations(m->main.level_scale, xtab1);
	XtOverrideTranslations(m->main.playpause_btn, xtab1);
	XtOverrideTranslations(m->main.stop_btn, xtab1);
	XtOverrideTranslations(m->main.prevdisc_btn, xtab1);
	XtOverrideTranslations(m->main.nextdisc_btn, xtab1);
	XtOverrideTranslations(m->main.previdx_btn, xtab1);
	XtOverrideTranslations(m->main.nextidx_btn, xtab1);
	XtOverrideTranslations(m->main.prevtrk_btn, xtab1);
	XtOverrideTranslations(m->main.nexttrk_btn, xtab1);
	XtOverrideTranslations(m->main.rew_btn, xtab1);
	XtOverrideTranslations(m->main.ff_btn, xtab1);

	/* Translations for the keypad window widgets */
	XtOverrideTranslations(m->keypad.keypad_ind, xtab1);
	XtOverrideTranslations(m->keypad.radio_box, xtab1);
	for (i = 0; i < 10; i++)
		XtOverrideTranslations(m->keypad.num_btn[i], xtab1);
	XtOverrideTranslations(m->keypad.clear_btn, xtab1);
	XtOverrideTranslations(m->keypad.enter_btn, xtab1);
	XtOverrideTranslations(m->keypad.warp_lbl, xtab1);
	XtOverrideTranslations(m->keypad.warp_scale, xtab1);
	XtOverrideTranslations(m->keypad.cancel_btn, xtab1);

	/* Translations for the options window widgets */
	XtOverrideTranslations(m->options.load_chkbox, xtab1);
	XtOverrideTranslations(m->options.load_radbox, xtab1);
	XtOverrideTranslations(m->options.exit_radbox, xtab1);
	XtOverrideTranslations(m->options.done_chkbox, xtab1);
	XtOverrideTranslations(m->options.eject_chkbox, xtab1);
	XtOverrideTranslations(m->options.chg_chkbox, xtab1);
	XtOverrideTranslations(m->options.chroute_radbox, xtab1);
	XtOverrideTranslations(m->options.vol_radbox, xtab1);
	XtOverrideTranslations(m->options.bal_lbl, xtab1);
	XtOverrideTranslations(m->options.bal_scale, xtab1);
	XtOverrideTranslations(m->options.ball_lbl, xtab1);
	XtOverrideTranslations(m->options.balr_lbl, xtab1);
	XtOverrideTranslations(m->options.balctr_btn, xtab1);
	XtOverrideTranslations(m->options.reset_btn, xtab1);
	XtOverrideTranslations(m->options.save_btn, xtab1);
	XtOverrideTranslations(m->options.ok_btn, xtab1);

	/* Translations for the dbprog window widgets */
	XtOverrideTranslations(m->dbprog.tottime_lbl, xtab1);
	XtOverrideTranslations(m->dbprog.tottime_ind, xtab1);
	XtOverrideTranslations(m->dbprog.rmtdsbl_btn, xtab1);
	XtOverrideTranslations(m->dbprog.wwwwarp_btn, xtab1);
	XtOverrideTranslations(m->dbprog.dtitle_txt, xtab1);
	XtOverrideTranslations(m->dbprog.extd_btn, xtab1);
	XtOverrideTranslations(m->dbprog.trk_list, xtab2);
	XtOverrideTranslations(m->dbprog.radio_box, xtab1);
	XtOverrideTranslations(m->dbprog.discid_ind, xtab1);
	XtOverrideTranslations(m->dbprog.ttitle_txt, xtab1);
	XtOverrideTranslations(m->dbprog.extt_btn, xtab1);
	XtOverrideTranslations(m->dbprog.addpgm_btn, xtab1);
	XtOverrideTranslations(m->dbprog.clrpgm_btn, xtab1);
	XtOverrideTranslations(m->dbprog.pgmseq_txt, xtab1);
	XtOverrideTranslations(m->dbprog.send_btn, xtab1);
	XtOverrideTranslations(m->dbprog.savedb_btn, xtab1);
	XtOverrideTranslations(m->dbprog.linkdb_btn, xtab1);
	XtOverrideTranslations(m->dbprog.loaddb_btn, xtab1);
	XtOverrideTranslations(m->dbprog.cancel_btn, xtab1);

	/* Translations for the wwwWarp window widgets */
	XtOverrideTranslations(m->wwwwarp.sel_radbox, xtab1);
	XtOverrideTranslations(m->wwwwarp.srchsite_opt, xtab1);
	XtOverrideTranslations(m->wwwwarp.go_btn, xtab1);
	XtOverrideTranslations(m->wwwwarp.cancel_btn, xtab1);

	/* Translations for the extd window widgets */
	XtOverrideTranslations(m->dbextd.disc_txt, xtab1);
	XtOverrideTranslations(m->dbextd.ok_btn, xtab1);
	XtOverrideTranslations(m->dbextd.clear_btn, xtab1);
	XtOverrideTranslations(m->dbextd.cancel_btn, xtab1);

	/* Translations for the extt window widgets */
	XtOverrideTranslations(m->dbextt.prev_btn, xtab1);
	XtOverrideTranslations(m->dbextt.next_btn, xtab1);
	XtOverrideTranslations(m->dbextt.autotrk_btn, xtab1);
	XtOverrideTranslations(m->dbextt.trk_txt, xtab1);
	XtOverrideTranslations(m->dbextt.ok_btn, xtab1);
	XtOverrideTranslations(m->dbextt.clear_btn, xtab1);
	XtOverrideTranslations(m->dbextt.cancel_btn, xtab1);

	/* Translations for the dirsel window widgets */
	XtOverrideTranslations(m->dirsel.dir_list, xtab2);
	XtOverrideTranslations(m->dirsel.ok_btn, xtab1);
	XtOverrideTranslations(m->dirsel.cancel_btn, xtab1);

	/* Translations for the linksel window widgets */
	XtOverrideTranslations(m->linksel.link_list, xtab2);
	XtOverrideTranslations(m->linksel.ok_btn, xtab1);
	XtOverrideTranslations(m->linksel.cancel_btn, xtab1);

	/* Translations for the help window widgets */
	XtOverrideTranslations(m->help.topic_opt, xtab1);
	XtOverrideTranslations(m->help.help_txt, xtab1);
	XtOverrideTranslations(m->help.about_btn, xtab1);
	XtOverrideTranslations(m->help.cancel_btn, xtab1);

	/* Initialize helpfile mappings */
	help_mapinit();

	/* Initialize documentation topics list */
	help_docinit();
}


/*
 * help_start
 *	Start up xmcd help system.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
void
help_start(void)
{
	int		ret;
	pid_t		cpid;
	waitret_t	stat_val;
	char		*dirpath,
			vfile[16],
			path[FILE_PATH_SZ + 16];
	struct stat	stbuf;

	/* Popup the help window if this version of xmcd is being run
	 * by the user for the first time.
	 */

	dirpath = util_homedir(util_get_ouid());
	if (dirpath == NULL) {
		/* No home directory: shrug */
		return;
	}
	if ((int) strlen(dirpath) >= FILE_PATH_SZ) {
		CD_FATAL(app_data.str_longpatherr);
		return;
	}
	(void) sprintf(vfile, "%s-%s", PROGNAME, VERSION);
	(void) sprintf(path, USR_VINIT_PATH, dirpath, vfile);

	if (stat(path, &stbuf) < 0) {
		switch (cpid = FORK()) {
		case 0:
			/* Child process */
			DBGPRN(errfp, "\nSetting uid to %d, gid to %d\n",
				(int) util_get_ouid(), (int) util_get_ogid());

			/* Force uid and gid to original setting */
			if (setuid(util_get_ouid()) < 0 ||
			    setgid(util_get_ogid()) < 0)
				exit(1);

			dirpath = util_dirname(path);
			if (!util_mkdir(dirpath, 0755)) {
				DBGPRN(errfp, "help_setup: cannot mkdir %s.\n",
					dirpath);
				exit(2);
			}

			/* Create version file */
			if (creat(path, 0666) < 0) {
				DBGPRN(errfp, "help_setup: cannot creat %s.\n",
					path);
				exit(3);
			}
			exit(0);
			/*NOTREACHED*/

		case -1:
			/* fork failed */
			break;

		default:
			/* Parent: wait for child to finish */
			while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
				if (ret < 0)
					break;
				/* Handle some events */
				event_loop(0);
			}
			break;
		}

		/* Pop up help */
		help_popup(widgets.main.help_btn);
	}
}


/*
 * help_popup
 *	Pop up the help window and display help text based on the
 *	specified widget.
 *
 * Args:
 *	w - The widget which the help info is being displayed about.
 *
 * Return:
 *	Nothing.
 */
void
help_popup(Widget w)
{
	char	hlpfile[FILE_PATH_SZ * 2],
		*hlpname;

	if ((hlpname = help_getname(w)) == NULL)
		return;

	(void) sprintf(hlpfile, HELPFILE_PATH, app_data.libdir, hlpname);

	XtVaSetValues(widgets.help.topic_opt,
		XmNmenuHistory, widgets.help.online_btn,
		NULL
	);

	/* Change to watch cursor */
	cd_busycurs(TRUE, CURS_HELP);

	/* Load the help file */
	help_loadfile(hlpfile);

	/* Change to normal cursor */
	cd_busycurs(FALSE, CURS_HELP);
}


/*
 * help_popdown
 *	Pop down the help window.
 *
 * Args:
 *	None
 *
 * Return:
 *	Nothing.
 */
void
help_popdown(void)
{
	if (XtIsManaged(widgets.help.form))
		XtUnmanageChild(widgets.help.form);
}


/* help_isactive
 *	Check if the help window is currently popped up.
 *
 * Args:
 *	None
 *
 * Return:
 *	TRUE: help currently popped up
 *	FALSE: help not currently popped up
 */
bool_t
help_isactive(void)
{
	return ((bool_t) XtIsManaged(widgets.help.form));
}


/**************** vv Callback routines vv ****************/

/*
 * help_topic_sel
 *	Help topic selector callback
 */
/*ARGSUSED*/
void
help_topic_sel(Widget w, XtPointer client_data, XtPointer call_data)
{
	doc_topic_t	*p = (doc_topic_t *)(void *) client_data;

	if (p == NULL) {
		/* Display generic help text */
		help_popup(widgets.main.help_btn);
	}
	else if (p->actbtn == w) {
		/* Display appropriate doc file */
		help_loadfile(p->path);
	}
}


