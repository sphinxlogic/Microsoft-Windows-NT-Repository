/*
 *   cda - Command-line CD Audio Player
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

/*
 *   Visual mode support
 *
 *   Contributing author: Philip Le Riche
 *   E-Mail: pleriche@uk03.bull.co.uk
 */

#ifndef LINT
static char *_visual_c_ident_ = "@(#)visual.c	6.60 98/10/27";
#endif

#ifndef NOVISUAL

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "common_d/patchlevel.h"
#include "libdi_d/libdi.h"
#include "cddb_d/cddb.h"

/* TRUE and FALSE is redefined in curses.h. */
#undef TRUE
#undef FALSE

/* curses.h redefines SYSV - handle with care */
#ifdef SYSV
#undef SYSV
#include <curses.h>
#define SYSV
#else
#if defined(ultrix) || defined(__ultrix)
#include <cursesX.h>
#else
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__bsdi__)
#include <ncurses.h>
#else
#include <curses.h>
#endif	/* __FreeBSD__ __NetBSD__ __bsdi__ */
#endif	/* ultrix */
#endif	/* SYSV */

#ifndef __QNX__
#include <term.h>
#endif

#include "cda_d/cda.h"
#include "cda_d/visual.h"

extern appdata_t	app_data;
extern cddb_incore_t	cur_db;
extern char		*errmsg,
			emsg[],
			spipe[],
			rpipe[];
extern int		cda_sfd[],
			cda_rfd[];
extern pid_t		daemon_pid;

STATIC int		scroll_line,		/* 1st line of info window */
			scroll_length,		/* Number of scrollable lines */
			route,			/* Stereo, Mono ... */
			old_route = 1,
			track = -2,		/* Current track no. */
			savch;			/* Saved char for cda_ungetch */
STATIC bool_t		isvisual,		/* Currently in visual mode */
			stat_on,		/* Visual: cda is "on" */
			ostat_on = TRUE,	/* Previous value */
			refresh_fkeys = TRUE,	/* Refresh funct key labels */
			help,			/* Display help in info_win? */
			old_help = TRUE,	/* Previous value */
			refresh_sts,		/* Refresh status line */
			echflag,		/* Own echo flag */
			savch_echo,		/* Control echo for savch */
			loaddb = TRUE;		/* Load CD database */
STATIC word32_t		oastat0 = (word32_t)-1,	/* Previous status value */
			oastat1 = (word32_t)-1;
STATIC WINDOW		*info_win,		/* Scrolling window for info */
			*status_win;		/* Window for status */


/* Keyboard input to command function mapping table */
typedef struct {
	int	key;
	void	(*keyfunc)(int, word32_t[]);
} keytbl_t;


/***********************
 *  internal routines  *
 ***********************/


/*
 * cda_wgetch
 *	Own version of curses wgetch. This interworks with cda_ungetch.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Input character or function key token.
 */
STATIC int
cda_wgetch(WINDOW *win)
{
	int	ch;

	if (savch) {
		/* Echo character now if echo on but not yet echoed */
		if (!savch_echo && echflag &&
		    isprint(savch) && !iscntrl(savch)) {
			waddch(win, savch);
			wrefresh(win);
		}
		ch = savch;
		savch = 0;
		return (ch);
	}

	ch = wgetch(win);

	/* Do our own echoing because switching it on and off doesn't
	 * seem to work on some platforms.
	 */
	if (echflag && isprint(ch) && !iscntrl(ch)) {
		waddch(win, ch);
		wrefresh(win);
	}

	return (ch);
}


/*
 * cda_ungetch
 *	Own version of ungetch, because some systems don't have it.
 *	Also, we need to remember the echo status of the ungotten
 *	character.
 *
 * Args:
 *	Char or function key token to return.
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_ungetch(int ch)
{
	savch = ch;
	/* Remember whether the character has been echoed */
	savch_echo = echflag;
}


/*
 * cda_wgetstr
 *	Own version of wgetstr, using cda_wgetch and cda_ungetch.
 *
 * Args:
 *	Buffer to be filled with input string.
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_wgetstr(WINDOW *win, char *inbuf, int max)
{
	int	ch,
		n,
		x,
		y;
	char	*p;
	bool_t	eos = FALSE;

	p = inbuf;
	n = 0;

	while (!eos) {
		if (n > max) {
			beep();
			break;
		}

		ch = cda_wgetch(win);

		switch (ch) {
		case KEY_BACKSPACE:
		case KEY_LEFT:
		case '\010':
			if (n > 0) {
				p--;
				n--;
				/* Echo the effect of backspace */
				getyx(win, y, x);
				wmove(win, y, x-1);
				waddch(win, ' ');
				wmove(win, y, x-1);
			}
			break;

		case KEY_UP:
		case '\036':	/* ctrl-^ */
		case KEY_DOWN:
		case '\026':	/* ctrl-v */
		case KEY_PPAGE:
		case '\002':	/* ctrl-b */
		case '\025':	/* ctrl-u */
		case KEY_NPAGE:
		case '\006':	/* ctrl-f */
		case '\004':	/* ctrl-d */
			if (n > 0) {
				n--;
				getyx(win, y, x);
				wmove(win, y, x-1);
				waddch(win, ' ');
				wmove(win, y, x-1);
			}

			switch (ch) {
			case KEY_UP:
			case '\036':
				scroll_line--;
				break;
			case KEY_DOWN:
			case '\026':
				scroll_line++;
				break;
			case KEY_PPAGE:
			case '\002':
				scroll_line -= (LINES - 8);
				break;
			case '\025':
				scroll_line -= ((LINES - 8) / 2);
				break;
			case KEY_NPAGE:
			case '\006':
				scroll_line += (LINES - 8);
				break;
			case '\004':
				scroll_line += ((LINES - 8) / 2);
				break;
			}

			if (scroll_line < 0)
				scroll_line = 0;
			else if (scroll_line > (scroll_length - 1))
				scroll_line = scroll_length - 1;

			prefresh(info_win, scroll_line, 0, 0, 0,
				 LINES-8, COLS-1);
			break;

		case '\n':
		case '\r':
			/* End-of-string */
			eos = TRUE;
			break;

		default:
			if (!isprint(ch) || iscntrl(ch))
				beep();
			else {
				*p++ = (char) ch;
				n++;
			}

			break;
		}

		wrefresh(win);
	}

	*p = '\0';
}


/*
 * cda_v_echo
 *	Own versions of curses echo function.
 *
 * Args:
 *	doecho - Whether to enable echo
 *
 * Return:
 *	Nothing.
 *
 */
STATIC void
cda_v_echo(bool_t doecho)
{
	echflag = doecho;
}


/*
 * cda_clrstatus
 *	Clear the status line and position cursor at the beginning.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_clrstatus(void)
{
	int	i;

	wmove(status_win, 1, 1);
	for (i = 1; i < COLS-1; i++)
		waddch(status_win, ' ');
	wmove(status_win, 1, 1);
}


/*
 * cda_v_errhandler
 *	Handler function used when the daemon has exited.
 *
 * Args:
 *	code - status code from cda_sendcmd
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_v_errhandler(int code)
{
	if (code == CDA_OK || code == CDA_PARMERR)
		/* Ignore these codes */
		return;

	/* Daemon died */
	cda_clrstatus();

	if (code < 0) {
		wprintw(status_win, "CD audio daemon pid=%d dev=%s exited.",
			daemon_pid, app_data.device);

		ostat_on = stat_on;
		stat_on = FALSE;

		if (cda_sfd[1] >= 0) {
			(void) close(cda_sfd[1]);
			(void) close(cda_rfd[1]);
			(void) close(cda_rfd[0]);
			cda_sfd[1] = cda_rfd[1] = cda_rfd[0] = -1;
		}

		loaddb = TRUE;	/* Force reload of CDDB */
		track = -2;	/* Force redisplay of version info */
		scroll_line = 0;
	}
	else
		wprintw(status_win, "%s", errmsg);

	wrefresh(status_win);
}


/*
 * cda_v_inexact_select
 *	Ask the user to select from a list of inexact CDDB matches.
 *
 * Args:
 *	matchlist - List of inexact matches
 *
 * Return:
 *	User selection number, or 0 for no selection.
 */
STATIC int
cda_v_inexact_select(cddb_match_t *matchlist)
{
	int		i,
			n,
			x;
	cddb_match_t	*p;
	char		input[64];

	wclear(info_win);
	wmove(info_win, 0, 0);
	wprintw(info_win, "\n%s\n%s\n\n",
		"There were no exact matches in the CD database.",
		"Close matches found:");

	for (p = matchlist, i = 1; p != NULL; p = p->next, i++)
		wprintw(info_win, "  %2d. %-73s\n", i, p->dtitle);

	wprintw(info_win, "  %2d. None of the above\n\n", i);

	scroll_line = 0;
	prefresh(info_win, scroll_line, 0, 0, 0, LINES-8, COLS-1);
	getyx(info_win, scroll_length, x);
#ifdef LINT
	x = x;	/* Get rid of "set but not used" lint warning */
#endif
	--scroll_length;

	for (;;) {
		cda_clrstatus();
		wprintw(status_win, "Choose one (1-%d): ", i);
		cda_v_echo(TRUE);
		putp(cursor_normal);
		wrefresh(status_win);
		cda_wgetstr(status_win, input, 60);

		n = atoi(input);
		if (n > 0 && n <= i)
			break;

		beep();
	}

	cda_v_echo(FALSE);
	putp(cursor_invisible);

	if (n == i)
		return 0;

	cda_clrstatus();
	wprintw(status_win, "Querying CD database...");
	wrefresh(status_win);
	return (n);
}


/*
 * cda_v_auth
 *	Ask the user to enter name and password for proxy authorization.
 *
 * Args:
 *	retrycnt - Number of times the user tried
 *
 * Return:
 *	0 - failure
 *	1 - success
 */
STATIC int
cda_v_auth(int retrycnt)
{
	int	x;
	char	*name,
		*pass,
		input[64];

	wclear(info_win);
	wmove(info_win, 0, 0);

	if (retrycnt == 0)
		wprintw(info_win, "Proxy Authorization is required.\n");
	else {
		wprintw(info_win, "Proxy Authorization failed.");

		if (retrycnt < 3)
			wprintw(info_win, "  Try again.\n");
		else {
			wprintw(info_win, "  Too many tries.\n");
			scroll_line = 0;
			prefresh(info_win, scroll_line, 0, 0, 0,
				 LINES-8, COLS-1);
			getyx(info_win, scroll_length, x);
#ifdef LINT
			x = x;	/* Get rid of "set but not used" lint warning */
#endif
			--scroll_length;
			(void) sleep(2);
			return 0;
		}
	}

	wprintw(info_win, "%s\n\n",
		"Please enter your proxy user name and password.");

	scroll_line = 0;
	prefresh(info_win, scroll_line, 0, 0, 0, LINES-8, COLS-1);
	getyx(info_win, scroll_length, x);
#ifdef LINT
	x = x;	/* Get rid of "set but not used" lint warning */
#endif
	--scroll_length;

	/* Get user name */
	cda_clrstatus();
	wprintw(status_win, "Username: ");
	cda_v_echo(TRUE);
	putp(cursor_normal);
	wrefresh(status_win);
	cda_wgetstr(status_win, input, 60);
	if (input[0] == '\0')
		return 0;

	name = (char *) MEM_ALLOC("v_auth_name", strlen(input) + 1);
	if (name == NULL) {
		CDA_FATAL(app_data.str_nomemory);
		return 0;
	}
	strcpy(name, input);
	(void) memset(input, 0, 64);

	/* Get password */
	cda_clrstatus();
	wprintw(status_win, "Password: ");
	cda_v_echo(FALSE);
	wrefresh(status_win);
	cda_wgetstr(status_win, input, 60);
	if (input[0] == '\0')
		pass = NULL;
	else {
		pass = (char *) MEM_ALLOC("v_auth_pass", strlen(input) + 1);
		if (pass == NULL) {
			CDA_FATAL(app_data.str_nomemory);
			return 0;
		}
		strcpy(pass, input);
		(void) memset(input, 0, 64);
	}

	cddb_set_auth(name, pass == NULL ? "" : pass);

	MEM_FREE(name);
	MEM_FREE(pass);

	return 1;
}


/*
 * cda_screen
 *	Paints the screen in visual mode, geting status and extinfo
 *	as required.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_screen(int signo)
{
	word32_t	arg[CDA_NARGS],
			discid,
			queryid,
			discnum = 0,
			astat0 = 0,
			astat1 = 0;
	int		x,
			y,
			i,
			retcode,
			trkno,
			min,
			sec,
			vol,
			bal,
			rptcnt;
	char		*p;
	curstat_t	*s = curstat_addr();
	bool_t		playing;
	static bool_t	cddb = FALSE;
	static word32_t	odiscnum;

	/* Need to refresh function key labels? */
	if (refresh_fkeys) {
		refresh_fkeys = FALSE;
		wmove(status_win, 3, 0);
		waddstr(status_win, STATUS_LINE0);
		wmove(status_win, 4, 0);
		waddstr(status_win, STATUS_LINE1);
		wmove(status_win, 5, 0);
		waddstr(status_win, STATUS_LINE2);
		wrefresh(status_win);

		box(status_win, 0, 0);
	}

	/* If daemon running, get status and update display */
	switch ((int) stat_on) {
	case FALSE:
		/* Daemon not running - just update display to "off" */
		if (stat_on != ostat_on) {
			wmove(status_win, ON_Y, ON_X);
			waddstr(status_win, "On");

			wmove(status_win, OFF_Y, OFF_X);
			wattron(status_win, A_STANDOUT);
			waddstr(status_win, "Off");
			wattroff(status_win, A_STANDOUT);

			wmove(status_win, LOAD_Y, LOAD_X);
			waddstr(status_win, "Load");

			wmove(status_win, EJECT_Y, EJECT_X);
			waddstr(status_win, "Eject");

			wmove(status_win, PLAY_Y, PLAY_X);
			waddstr(status_win, "Play");

			wmove(status_win, PAUSE_Y, PAUSE_X);
			waddstr(status_win, "Pause");

			wmove(status_win, STOP_Y, STOP_X);
			waddstr(status_win, "Stop");

			wmove(status_win, LOCK_Y, LOCK_X);
			waddstr(status_win, "Lock");

			wmove(status_win, UNLOCK_Y, UNLOCK_X);
			waddstr(status_win, "Unlock");

			wmove(status_win, SHUFFLE_Y, SHUFFLE_X);
			waddstr(status_win, "Shuffle");

			wmove(status_win, PROGRAM_Y, PROGRAM_X);
			waddstr(status_win, "Program");

			wmove(status_win, REPEAT_ON_Y, REPEAT_ON_X);
			waddstr(status_win, "On");

			wmove(status_win, REPEAT_OFF_Y, REPEAT_OFF_X);
			waddstr(status_win, "Off");
		}

		/* Check to see if daemon started */
		if (cda_daemon_alive()) {
			/* Daemon is alive: open FIFOs - command side */
			if (cda_sfd[1] < 0)
				cda_sfd[1] = open(spipe, O_WRONLY);

			if (cda_sfd[1] >= 0) {
				stat_on = TRUE;

				cda_rfd[1] = open(rpipe, O_RDONLY);
				if (cda_rfd[1] < 0) {
					CDA_FATAL(
					    "cannot open recv pipe for reading"
					);
				}
				cda_rfd[0] = open(rpipe, O_WRONLY);
				if (cda_rfd[0] < 0) {
					CDA_FATAL(
					    "cannot open recv pipe for writing"
					);
				}
			}
		}

		if (!stat_on)
			break;

		arg[0] = 0;
		if (!cda_sendcmd(CDA_ON, arg, &retcode))
			cda_v_errhandler(retcode);
		else {
			daemon_pid = (pid_t) arg[0];
			cda_clrstatus();
			wprintw(status_win,
				"CD audio daemon pid=%d dev=%s started.",
				daemon_pid, app_data.device);
			wrefresh(status_win);
		}

		/*FALLTHROUGH*/

	case TRUE:
		/* Daemon running - get status and update display */
		(void) memset(arg, 0, CDA_NARGS * sizeof(word32_t));
		if (!cda_sendcmd(CDA_STATUS, arg, &retcode)) {
			cda_v_errhandler(retcode);

			oastat0 = astat0;
			oastat1 = astat1;

			/* Come back in 1 sec */
			(void) signal(SIGALRM, cda_screen);
			(void) alarm(1);
			return;
		}

		wmove(status_win, ON_Y, ON_X);
		wattron(status_win, A_STANDOUT);
		waddstr(status_win, "On");
		wattroff(status_win, A_STANDOUT);

		wmove(status_win, OFF_Y, OFF_X);
		waddstr(status_win, "Off");

		astat0 = arg[0];
		astat1 = arg[1];
		rptcnt = (int) arg[2];
		vol = (int) arg[3];
		bal = (int) arg[4];
		route = (int) arg[5];
		discid = arg[6];
		discnum = arg[7];

		if (RD_ARG_MODE(astat0) == MOD_NODISC || odiscnum != discnum)
			loaddb = TRUE;

		if (discid != 0 && loaddb) {
			/* Load CD database entry */
			loaddb = FALSE;
			cda_clrstatus();
			wprintw(status_win, "Querying CD database...");
			wrefresh(status_win);

			cda_dbclear(s, TRUE);
			cur_db.discid = discid;
			queryid = (arg[8] != 0) ? arg[8] : arg[6];
			cddb = cda_dbload(
				queryid,
				cda_v_inexact_select,
				cda_v_auth,
				0
			);
			cda_clrstatus();
		}

		if (astat0 != oastat0 || astat1 != oastat1) {
			if (RD_ARG_MODE(astat0) == MOD_STOP &&
			    RD_ARG_MODE(oastat0) != MOD_NODISC) {
				/* CD no longer busy or playing:
				 * clear status line.
				 */
				cda_clrstatus();
				refresh_sts = TRUE;
			}

			switch (RD_ARG_MODE(astat0)) {
			case MOD_BUSY:
			case MOD_NODISC:
				wmove(status_win, LOAD_Y, LOAD_X);
				waddstr(status_win, "Load");
				 
				wmove(status_win, EJECT_Y, EJECT_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Eject");
				wattroff(status_win, A_STANDOUT);

				wmove(status_win, PLAY_Y, PLAY_X);
				waddstr(status_win, "Play");

				wmove(status_win, PAUSE_Y, PAUSE_X);
				waddstr(status_win, "Pause");

				wmove(status_win, STOP_Y, STOP_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Stop");
				wattroff(status_win, A_STANDOUT);

				break;

			case MOD_STOP:
				wmove(status_win, LOAD_Y, LOAD_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Load");
				wattroff(status_win, A_STANDOUT);

				wmove(status_win, EJECT_Y, EJECT_X);
				waddstr(status_win, "Eject");

				wmove(status_win, PLAY_Y, PLAY_X);
				waddstr(status_win, "Play");

				wmove(status_win, PAUSE_Y, PAUSE_X);
				waddstr(status_win, "Pause");

				wmove(status_win, STOP_Y, STOP_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Stop");
				wattroff(status_win, A_STANDOUT);

				break;

			case MOD_PLAY:
				wmove(status_win, LOAD_Y, LOAD_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Load");
				wattroff(status_win, A_STANDOUT);

				wmove(status_win, EJECT_Y, EJECT_X);
				waddstr(status_win, "Eject");

				wmove(status_win, PLAY_Y, PLAY_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Play");
				wattroff(status_win, A_STANDOUT);

				wmove(status_win, PAUSE_Y, PAUSE_X);
				waddstr(status_win, "Pause");

				wmove(status_win, STOP_Y, STOP_X);
				waddstr(status_win, "Stop");

				break;

			case MOD_PAUSE:
				wmove(status_win, LOAD_Y, LOAD_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Load");
				wattroff(status_win, A_STANDOUT);

				wmove(status_win, EJECT_Y, EJECT_X);
				waddstr(status_win, "Eject");

				wmove(status_win, PLAY_Y, PLAY_X);
				waddstr(status_win, "Play");

				wmove(status_win, PAUSE_Y, PAUSE_X);
				wattron(status_win, A_STANDOUT);
				waddstr(status_win, "Pause");
				wattroff(status_win, A_STANDOUT);

				wmove(status_win, STOP_Y, STOP_X);
				waddstr(status_win, "Stop");

				break;
			}

			wmove(status_win, LOCK_Y, LOCK_X);
			if (RD_ARG_LOCK(astat0))
				wattron(status_win, A_STANDOUT);
			waddstr(status_win, "Lock");
			wattroff(status_win, A_STANDOUT);

			wmove(status_win, UNLOCK_Y, UNLOCK_X);
			if (!RD_ARG_LOCK(astat0))
				wattron(status_win, A_STANDOUT);
			waddstr(status_win, "Unlock");
			wattroff(status_win, A_STANDOUT);

			wmove(status_win, SHUFFLE_Y, SHUFFLE_X);
			if (RD_ARG_SHUF(astat0))
				wattron(status_win, A_STANDOUT);
			waddstr(status_win, "Shuffle");
			wattroff(status_win, A_STANDOUT);

			wmove(status_win, PROGRAM_Y, PROGRAM_X);
			if (stat_on &&
			    RD_ARG_MODE(astat0) != MOD_BUSY &&
			    RD_ARG_MODE(astat0) != MOD_NODISC &&
			    !RD_ARG_SHUF(astat0)) {
				arg[0] = 1;
				if (!cda_sendcmd(CDA_PROGRAM, arg, &retcode)) {
					cda_v_errhandler(retcode);

					oastat0 = astat0;
					oastat1 = astat1;

					/* Come back in 1 sec */
					(void) signal(SIGALRM, cda_screen);
					(void) alarm(1);
					return;
				}

				if (arg[0] != 0)
					wattron(status_win, A_STANDOUT);
			}
			waddstr(status_win, "Program");
			wattroff(status_win, A_STANDOUT);

			wmove(status_win, REPEAT_ON_Y, REPEAT_ON_X);
			if (RD_ARG_REPT(astat0))
				wattron(status_win, A_STANDOUT);
			waddstr(status_win, "On");
			wattroff(status_win, A_STANDOUT);

			wmove(status_win, REPEAT_OFF_Y, REPEAT_OFF_X);
			if (!RD_ARG_REPT(astat0))
				wattron(status_win, A_STANDOUT);
			waddstr(status_win, "Off");
			wattroff(status_win, A_STANDOUT);
		}

		wmove(status_win, 1, 1);
		if (RD_ARG_MODE(astat0) == MOD_BUSY) {
			cda_clrstatus();
			wprintw(status_win, "CD Busy");
		}
		else if (RD_ARG_MODE(astat0) == MOD_PLAY ||
			 RD_ARG_MODE(astat0) == MOD_PAUSE ||
			 RD_ARG_MODE(astat0) == MOD_NODISC) {

			if (RD_ARG_MODE(astat0) == MOD_NODISC) {
				track = -2;	/* Force redisplay of
						 * version/device
						 */
				wprintw(status_win, "No Disc   ");
			}
			else {
				wprintw(status_win,
				"Disc %d Track %02u Index %02u %s%02u:%02u  ",
					discnum,
					RD_ARG_TRK(astat1),
					RD_ARG_IDX(astat1),
					(RD_ARG_IDX(astat1) == 0) ? "-" : "+",
					RD_ARG_MIN(astat1),
					RD_ARG_SEC(astat1));
			}

			wprintw(status_win, "Volume %3u%% Balance %3u%% ",
				vol, bal);

			switch (route) {
			case 0:
				wprintw(status_win, "Stereo    ");
				break;
			case 1:
				wprintw(status_win, "Reverse   ");
				break;
			case 2:
				wprintw(status_win, "Mono-L    ");
				break;
			case 3:
				wprintw(status_win, "Mono-R    ");
				break;
			case 4:
				wprintw(status_win, "Mono-L+R  ");
				break;
			}

			if (rptcnt >= 0)
				wprintw(status_win, "Count %u", rptcnt);

			getyx(status_win, y, x);
#ifdef LINT
			y = y;	/* Get rid of "set but not used" lint warning */
#endif
			for (i = x; i < COLS-1; i++)
				waddch(status_win, ' ');

			wmove(status_win, 1, 1);
		}
		else if (refresh_sts) {
			refresh_sts = FALSE;
			cda_clrstatus();

			if (stat_on &&
			    RD_ARG_MODE(astat0) != MOD_BUSY &&
			    RD_ARG_MODE(astat0) != MOD_NODISC &&
			    !RD_ARG_SHUF(astat0)) {
				arg[0] = 1;
				if (!cda_sendcmd(CDA_PROGRAM, arg, &retcode)) {
					cda_v_errhandler(retcode);

					oastat0 = astat0;
					oastat1 = astat1;

					/* Come back in 1 sec */
					(void) signal(SIGALRM, cda_screen);
					(void) alarm(1);
					return;
				}

				if (arg[0] != 0) {
					wprintw(status_win, "Program: ");
					for (i = 0; i < arg[0]; i++) {
						wprintw(status_win, " %02u",
							arg[i+1]);
					}
				}
			}
		}
		break;
	}

	wrefresh(status_win);

	/* See if we want to display help info */
	if (help) {
		if (!old_help) {
			wclear(info_win);
			wmove(info_win, 0, 0);
			wprintw(info_win, HELP_INFO);
			scroll_line = 0;
			prefresh(info_win, scroll_line, 0, 0, 0,
				 LINES-8, COLS-1);
			old_help = help;
		}
		odiscnum = discnum;
		(void) signal(SIGALRM, cda_screen);
		(void) alarm(1);
		return;
	}
	else if (old_help) {
		wclear(info_win);
		scroll_line = 0;
		track = -2;	/* Force display of version/device */
	}

	/* Disc changed: redisplay */
	if (odiscnum != discnum) {
		wclear(info_win);
		scroll_line = 0;
		track = -2;
	}

	/* If state is unchanged since last time, no more to do */
	if (stat_on == ostat_on && old_help == help && old_route == route &&
	    RD_ARG_MODE(astat0) == RD_ARG_MODE(oastat0) &&
	    RD_ARG_TRK(astat1) == RD_ARG_TRK(oastat1) &&
	    RD_ARG_IDX(astat1) == RD_ARG_IDX(oastat1) &&
	    odiscnum == discnum) {
		oastat0 = astat0;
		oastat1 = astat1;

		/* Call us again - nothing is too much trouble! */
		(void) signal(SIGALRM, cda_screen);
		(void) alarm(1);
		return;
	}

	old_help = help;
	old_route = route;
	odiscnum = discnum;
	ostat_on = stat_on;
	oastat0 = astat0;
	oastat1 = astat1;

	/* Now display data, according to our state: */

	/* Off, busy or no disc, display version and device */
	if (!stat_on ||
	    RD_ARG_MODE(astat0) == MOD_BUSY ||
	    RD_ARG_MODE(astat0) == MOD_NODISC) {
		if (track != -1) {
			track = -1;
			wclear(info_win);
			wmove(info_win, 0,0);
			wprintw(info_win,
				"CDA - Command Line CD Audio Player");
			wmove(info_win, 0, COLS-18);
			wprintw(info_win, "Press ");
			wattron(info_win, A_STANDOUT);
			wprintw(info_win, "?");
			wattroff(info_win, A_STANDOUT);
			wprintw(info_win, " for help.\n\n");

			wprintw(info_win, "CD audio        v%s%s PL%d\n",
				VERSION, VERSION_EXT, PATCHLEVEL);

			switch ((int) stat_on) {
			case TRUE:
				if (cda_sendcmd(CDA_VERSION, arg, &retcode)) {
					wprintw(info_win,
						"CD audio daemon v%s\n",
						(char *) arg);
					break;
				}
				cda_v_errhandler(retcode);
				/*FALLTHROUGH*/

			case FALSE:
				(void) sprintf((char *) arg, "%s%s PL%d\n%s",
					       VERSION, VERSION_EXT,
					       PATCHLEVEL, di_vers());
				break;
			}
			wprintw(info_win, "%s\n%s\n%s\n\n%s\n",
				COPYRIGHT, EMAIL, WWWURL, GNU_BANNER);

			wprintw(info_win, "\nDevice: %s\n", app_data.device);
			if (stat_on) {
				if (!cda_sendcmd(CDA_DEVICE, arg, &retcode)) {
					cda_v_errhandler(retcode);

					oastat0 = astat0;
					oastat1 = astat1;

					/* Come back in 1 sec */
					(void) signal(SIGALRM, cda_screen);
					(void) alarm(1);
					return;
				}
				wprintw(info_win, "%s\n", (char *) arg);
			}

			prefresh(info_win, scroll_line, 0, 0, 0,
				 LINES-8, COLS-1);
			getyx(info_win, scroll_length, i);
			--scroll_length;
		}
	}
	else if (track != RD_ARG_TRK(astat1)) {
		/* If disc loaded, display extinfo */

		wclear(info_win);
		wmove(info_win, 0, 0);

		/* Get database entry */
		if (RD_ARG_MODE(astat0) == MOD_PLAY ||
		    RD_ARG_MODE(astat0) == MOD_PAUSE) {
			track = RD_ARG_TRK(astat1);
		}
		else
			track = -1;

		arg[0] = 0;
		arg[1] = track;
		if (!cda_sendcmd(CDA_EXTINFO, arg, &retcode)) {
			cda_v_errhandler(retcode);

			oastat0 = astat0;
			oastat1 = astat1;

			/* Come back in 1 sec */
			(void) signal(SIGALRM, cda_screen);
			(void) alarm(1);
			return;
		}

		if (!cddb || RD_ARG_MODE(astat0) == MOD_STOP) {
			int	ntrks;

			/* No database entry */
			arg[0] = 0;
			if (!cda_sendcmd(CDA_TOC, arg, &retcode)) {
				cda_v_errhandler(retcode);

				oastat0 = astat0;
				oastat1 = astat1;

				/* Come back in 1 sec */
				(void) signal(SIGALRM, cda_screen);
				(void) alarm(1);
				return;
			}

			cur_db.discid = arg[0];
			ntrks = (int) (arg[0] & 0xff);

			wprintw(info_win, "Disc %d  ID: %s %08x%s",
				discnum,
				(cur_db.category[0] == '\0') ?
					"(no category)" : cur_db.category,
				arg[0], (cur_db.extd == NULL) ? "" : " *");

			wmove(info_win, 0, COLS-18);
			wprintw(info_win, "Press ");
			wattron(info_win, A_STANDOUT);
			wprintw(info_win, "?");
			wattroff(info_win, A_STANDOUT);
			wprintw(info_win, " for help.\n\n");

			if (cddb && cur_db.dtitle != NULL)
				wprintw(info_win, "%s\n\n", cur_db.dtitle);
			else
				wprintw(info_win, "(unknown disc title)\n\n");

			for (i = 0; i < (int) ntrks; i++) {
				RD_ARG_TOC(arg[i+1], trkno, playing, min, sec);
				wprintw(info_win, "%s%02u %02u:%02u  ",
					playing ? ">" : " ",
					trkno, min, sec);
				if (cddb && cur_db.trklist[i] != NULL) {
					wprintw(info_win, "%s%s\n",
						cur_db.trklist[i],
						(cur_db.extt[i] != NULL) ?
							"*" : "");
				}
				else {
					wprintw(info_win, "??%s\n",
						(cur_db.extt[i] != NULL) ?
							"*" : "");
				}
			}

			RD_ARG_TOC(arg[i+1], trkno, playing, min, sec);
			wprintw(info_win, "\nTotal Time: %02u:%02u\n",
				min, sec);
		}
		else {
			/* Have database entry */
			if (cur_db.extd == NULL) {
				wprintw(info_win,
				    "No Extended Information for this CD.\n");
			}
			else {
				if (cur_db.dtitle != NULL)
					wprintw(info_win, "%s\n\n",
						cur_db.dtitle);
				else
					wprintw(info_win,
						"(unknown disc title)\n\n");

				/* Not using wprintw here to avoid a bug
				 * with very long strings
				 */
				p = cur_db.extd;
				for (; *p != '\0'; p++)
				       waddch(info_win, *p);
				waddch(info_win, '\n');
			}

			for (i = 0; i < COLS-1; i++)
				waddch(info_win, ACS_HLINE);
			waddch(info_win, '\n');

			/* If Play or Pause, display track info */
			if (RD_ARG_MODE(astat0) == MOD_PLAY ||
			    RD_ARG_MODE(astat0) == MOD_PAUSE) {
				if (cur_db.trklist[arg[2]] == NULL) {
					wprintw(info_win,
						"(No title for track %02u.)\n",
						arg[1]);
				}
				else {
					wprintw(info_win, "%s\n",
						cur_db.trklist[arg[2]]);

					if (cur_db.extt[arg[2]] != NULL) {
						/* Not using wprintw here
						 * to avoid a bug with very
						 * long strings
						 */
						p = cur_db.extt[arg[2]];
						waddch(info_win, '\n');
						for (; *p != '\0'; p++)
						       waddch(info_win, *p);
						waddch(info_win, '\n');
					}
				}
			}
		}
		scroll_line = 0;
		getyx(info_win, scroll_length, i);
		prefresh(info_win, scroll_line, 0, 0, 0, LINES-8, COLS-1);
	}
	oastat0 = astat0;
	oastat1 = astat1;

	/* Come back in 1 sec */
	(void) signal(SIGALRM, cda_screen);
	(void) alarm(1);
}


/*
 * cda_v_on_off
 *	Handler function for the visual mode on/off control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_on_off(int inp, word32_t arg[])
{
	int		retcode;
	curstat_t	*s = curstat_addr();

	cda_clrstatus();

	if (!stat_on) {
		if (cda_daemon_alive()) {
			wprintw(status_win,
				"CD audio daemon already running.");
			wrefresh(status_win);
			beep();
		}
		else {
			/* Start CDA daemon */
			loaddb = TRUE;
			wprintw(status_win, "Initializing...");
			wrefresh(status_win);

			if (cda_daemon(s)) {
				cda_clrstatus();
				wprintw(status_win,
					"Cannot start CD audio daemon.");
				wrefresh(status_win);
			}
		}
	}
	else {
		if (cda_sendcmd(CDA_OFF, arg, &retcode)) {
			wprintw(status_win, "Stopping CD audio daemon...");
			wrefresh(status_win);

			/* Wait for daemon to die */
			do {
				(void) sleep(1);
			} while (cda_daemon_alive());
		}

		cda_v_errhandler(retcode);
	}
}


/*
 * cda_v_load_eject
 *	Handler function for the visual mode load/eject control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_load_eject(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on || RD_ARG_MODE(arg[0]) == MOD_BUSY) {
		beep();
		return;
	}

	arg[0] = (RD_ARG_MODE(arg[0]) == MOD_NODISC) ? 0 : 1;

	if (!cda_sendcmd(CDA_DISC, arg, &retcode))
		cda_v_errhandler(retcode);

	loaddb = TRUE;	/* Force reload of CDDB */
	refresh_sts = TRUE;
}


/*
 * cda_v_play_pause
 *	Handler function for the visual mode play/pause control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_play_pause(int inp, word32_t arg[])
{
	word32_t	cmd;
	int		retcode;

	if (!stat_on ||
	    RD_ARG_MODE(arg[0]) == MOD_BUSY ||
	    RD_ARG_MODE(arg[0]) == MOD_NODISC) {
		beep();
		return;
	}

	if (RD_ARG_MODE(arg[0]) == MOD_PLAY)
		cmd = CDA_PAUSE;
	else
		cmd = CDA_PLAY;

	arg[0] = 0;
	if (!cda_sendcmd(cmd, arg, &retcode))
		cda_v_errhandler(retcode);
}


/*
 * cda_v_stop
 *	Handler function for the visual mode stop control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_stop(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on ||
	    RD_ARG_MODE(arg[0]) == MOD_BUSY ||
	    RD_ARG_MODE(arg[0]) == MOD_NODISC) {
		beep();
		return;
	}

	if (RD_ARG_MODE(arg[0]) != MOD_PLAY &&
	    RD_ARG_MODE(arg[0]) != MOD_PAUSE) {
		beep();
		return;
	}

	if (!cda_sendcmd(CDA_STOP, arg, &retcode))
		cda_v_errhandler(retcode);

	refresh_sts = TRUE;
}


/*
 * cda_v_lock
 *	Handler function for the visual mode lock/unlock control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_lock(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on ||
	    RD_ARG_MODE(arg[0]) == MOD_BUSY ||
	    RD_ARG_MODE(arg[0]) == MOD_NODISC) {
		beep();
		return;
	}

	arg[0] = RD_ARG_LOCK(arg[0]) ? 0 : 1;

	if (!cda_sendcmd(CDA_LOCK, arg, &retcode))
		cda_v_errhandler(retcode);
}


/*
 * cda_v_shufprog
 *	Handler function for the visual mode shuffle/program control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_shufprog(int inp, word32_t arg[])
{
	word32_t	astat0 = arg[0];
	int		i,
			j,
			retcode;
	char		inbuf[80],
			*p;

	if (!stat_on ||
	    RD_ARG_MODE(astat0) == MOD_BUSY ||
	    RD_ARG_MODE(astat0) == MOD_NODISC) {
		beep();
		return;
	}

	/* Not allowed if play or pause */
	if (RD_ARG_MODE(astat0) == MOD_PLAY ||
	    RD_ARG_MODE(astat0) == MOD_PAUSE) {
		beep();
		return;
	}

	/* See if program on */
	arg[0] = 1;
	if (!cda_sendcmd(CDA_PROGRAM, arg, &retcode)) {
		cda_v_errhandler(retcode);
		return;
	}

	/* If neither program nor shuffle, set shuffle */
	if (!RD_ARG_SHUF(astat0) && arg[0] == 0) {
		arg[0] = 1;
		if (!cda_sendcmd(CDA_SHUFFLE, arg, &retcode)) {
			cda_v_errhandler(retcode);
			return;
		}
	}
	else if (RD_ARG_SHUF(astat0)) {
		/* If shuffle, turn it off and prompt for program */
		arg[0] = 0;
		if (!cda_sendcmd(CDA_SHUFFLE, arg, &retcode)) {
			cda_v_errhandler(retcode);
			return;
		}

		wmove(status_win, SHUFFLE_Y, SHUFFLE_X);
		waddstr(status_win, "Shuffle");

		wmove(status_win, PROGRAM_Y, PROGRAM_X);
		wattron(status_win, A_STANDOUT);
		waddstr(status_win, "Program");
		wattroff(status_win, A_STANDOUT);

		cda_clrstatus();
		wprintw(status_win, "Program: ");
		cda_v_echo(TRUE);
		putp(cursor_normal);
		wrefresh(status_win);

		/* Before reading the program list, check for
		 * F6 or "u", and dismiss prog mode if found
		 */
		i = cda_wgetch(status_win);
		if (i != KEY_F(6) && i != 'u') {
			cda_ungetch(i);

			cda_wgetstr(status_win, inbuf, COLS-12);

			/* Is the string just read was
			 * terminated by F6, it will
			 * have been ungotten, but must be
			 * thrown away or it will cause
			 * return to shuffle mode.
			 */
			if (savch == KEY_F(6))
				savch = 0;

			j = 0;
			p = inbuf;
			while ((i = strtol(p, &p, 10)) != 0) {
				arg[++j] = i;

				if (p == NULL)
					break;

				while (*p != '\0' && !isdigit(*p))
					p++;
			}

			arg[0] = (word32_t) -j;
			if (!cda_sendcmd(CDA_PROGRAM, arg, &retcode)) {
				errmsg = NULL;
				beep();
			}
		}

		cda_v_echo(FALSE);
		putp(cursor_invisible);

		refresh_sts = TRUE;
	}
	else {
		/* Program is on - reset it */
		arg[0] = 0;
		if (!cda_sendcmd(CDA_PROGRAM, arg, &retcode)) {
			cda_v_errhandler(retcode);
			return;
		}
		refresh_sts = TRUE;
	}
}


/*
 * cda_v_repeat
 *	Handler function for the visual mode repeat control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_repeat(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on) {
		beep();
		return;
	}

	arg[0] = RD_ARG_REPT(arg[0]) ? 0 : 1;
	if (!cda_sendcmd(CDA_REPEAT, arg, &retcode))
		cda_v_errhandler(retcode);
}


/*
 * cda_v_disc
 *	Handler function for the visual mode disc change control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_v_disc(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on) {
		beep();
		return;
	}

	arg[0] = (inp == 'd') ? 2 : 3;
	if (!cda_sendcmd(CDA_DISC, arg, &retcode))
		cda_v_errhandler(retcode);

	loaddb = TRUE;
}


/*
 * cda_v_track
 *	Handler function for the visual mode track change control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_v_track(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on ||
	    RD_ARG_MODE(arg[0]) == MOD_BUSY ||
	    RD_ARG_MODE(arg[0]) == MOD_NODISC ||
	    RD_ARG_MODE(arg[0]) != MOD_PLAY) {
		beep();
		return;
	}

	arg[0] = (inp == KEY_LEFT || inp == 'C') ? 0 : 1;
	if (!cda_sendcmd(CDA_TRACK, arg, &retcode))
		cda_v_errhandler(retcode);
}


/*
 * cda_v_index
 *	Handler function for the visual mode index change control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_v_index(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on ||
	    RD_ARG_MODE(arg[0]) == MOD_BUSY ||
	    RD_ARG_MODE(arg[0]) == MOD_NODISC) {
		beep();
		return;
	}

	arg[0] = (inp == '<') ? 0 : 1;
	if (!cda_sendcmd(CDA_INDEX, arg, &retcode))
		cda_v_errhandler(retcode);
}


/*
 * cda_v_volume
 *	Handler function for the visual mode volume control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_v_volume(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on) {
		beep();
		return;
	}

	arg[0] = 0;
	if (!cda_sendcmd(CDA_VOLUME, arg, &retcode)) {
		cda_v_errhandler(retcode);
		return;
	}

	if (inp == '+') {
		if (arg[1] <= 95)
			arg[1] += 5;
		else
			arg[1] = 100;
	}
	else {
		if (arg[1] >= 5)
			arg[1] -= 5;
		else
			arg[1] = 0;
	}

	arg[0] = 1;
	if (!cda_sendcmd(CDA_VOLUME, arg, &retcode)) {
		cda_v_errhandler(retcode);
		return;
	}
}


/*
 * cda_v_balance
 *	Handler function for the visual mode balance control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_v_balance(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on) {
		beep();
		return;
	}

	arg[0] = 0;
	if (!cda_sendcmd(CDA_BALANCE, arg, &retcode)) {
		cda_v_errhandler(retcode);
		return;
	}

	if (inp == 'r' || inp == 'R') {
		if (arg[1] <= 95)
			arg[1] += 5;
		else
			arg[1] = 100;
	}
	else {
		if (arg[1] >= 5)
			arg[1] -= 5;
		else
			arg[1] = 0;
	}

	arg[0] = 1;
	if (!cda_sendcmd(CDA_BALANCE, arg, &retcode))
		cda_v_errhandler(retcode);
}


/*
 * cda_v_route
 *	Handler function for the visual mode channel routing control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_route(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on) {
		beep();
		return;
	}

	if (--route < 0)
		route = 4;

	arg[0] = 1;
	arg[1] = route;
	if (!cda_sendcmd(CDA_ROUTE, arg, &retcode))
		cda_v_errhandler(retcode);
}


/*
 * cda_v_help
 *	Handler function for the visual mode help control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_help(int inp, word32_t arg[])
{
	if (inp == '?') {
		help = TRUE;
		scroll_length = HELP_SCROLL_LENGTH;
	}
	else
		help = FALSE;
}


/*
 * cda_v_repaint
 *	Handler function for the visual mode repaint control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_repaint(int inp, word32_t arg[])
{
	wclear(info_win);
	wclear(status_win);
	oastat0 = oastat1 = (word32_t) -1;
	ostat_on = !stat_on;
	old_help = !help;
	old_route = !route;
	refresh_fkeys = TRUE;
}


/*
 * cda_v_scroll
 *	Handler function for the visual mode scroll control.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_scroll(int inp, word32_t arg[])
{
	switch (inp) {
	case KEY_UP:
	case '^':
	case '\036':	/* ctrl-^ */
		scroll_line--;
		break;
	case KEY_DOWN:
	case 'v':
	case 'V':
	case '\026':	/* ctrl-v */
		scroll_line++;
		break;
	case KEY_PPAGE:
	case '\002':	/* ctrl-b */
		scroll_line -= (LINES - 8);
		break;
	case '\025':	/* ctrl-u */
		scroll_line -= ((LINES - 8) / 2);
		break;
	case KEY_NPAGE:
	case '\006':	/* ctrl-f */
		scroll_line += (LINES - 8);
		break;
	case '\004':	/* ctrl-d */
		scroll_line += ((LINES - 8) / 2);
		break;
	default:
		return;
	}

	if (scroll_line < 0)
		scroll_line = 0;
	else if (scroll_line > (scroll_length - 1))
		scroll_line = scroll_length - 1;

	prefresh(info_win, scroll_line, 0, 0, 0, LINES-8, COLS-1);
}


/*
 * cda_v_debug
 *	Handler function for the debug command - toggle debug mode.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
cda_v_debug(int inp, word32_t arg[])
{
	int	retcode;

	if (!stat_on) {
		beep();
		return;
	}

	/* Query current debug mode */
	arg[0] = 0;
	if (!cda_sendcmd(CDA_DEBUG, arg, &retcode)) {
		beep();
		cda_v_errhandler(retcode);
		return;
	}

	/* Toggle debug mode */
	arg[0] = 1;
	if (arg[1] == 0)
		arg[1] = 1;
	else
		arg[1] = 0;

	if (!cda_sendcmd(CDA_DEBUG, arg, &retcode)) {
		beep();
		cda_v_errhandler(retcode);
		return;
	}

	cda_clrstatus();
	wprintw(status_win, "Debug mode is now %s.",
		(arg[1] == 0) ? "off" : "on");
	wrefresh(status_win);
}


/*
 * cda_v_dirtrk
 *	Handler function for the visual mode direct track access.
 *
 * Args:
 *	inp - command character
 *	arg - command arguments
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_v_dirtrk(int inp, word32_t arg[])
{
	int	i,
		mins,
		secs,
		retcode;
	char	inbuf[80],
		*p;

	if (!isdigit(inp))
		return;

	if (!stat_on) {
		beep();
		return;
	}

	cda_ungetch(inp);

	cda_clrstatus();
	wprintw(status_win, "Track n [mins secs] : ");
	cda_v_echo(TRUE);
	putp(cursor_normal);
	wrefresh(status_win);

	cda_wgetstr(status_win, inbuf, 20);
	if ((i = strtol(inbuf, &p, 10)) == 0)
		beep();
	else {
		mins = secs = 0;
		if (p != NULL) {
			while (*p != '\0' && !isdigit(*p))
				p++;

			mins = strtol(p, &p, 10);

			if (p != NULL) {
				while (*p != '\0' && !isdigit(*p))
					p++;

				secs = strtol(p, &p, 10);
			}
		}

		arg[0] = i;
		arg[1] = mins;
		arg[2] = secs;
		if (!cda_sendcmd(CDA_PLAY, arg, &retcode))
			cda_v_errhandler(retcode);
	}

	cda_v_echo(FALSE);
	putp(cursor_invisible);
	refresh_sts = TRUE;
}


#if defined(SIGTSTP) && defined(SIGCONT)
/*
 * ontstp
 *	Handler for job control stop signal
 *
 * Args:
 *	signo - The signal number
 *
 * Return:
 *	Nothing.
 */
STATIC void
ontstp(int signo)
{
	if (signo != SIGTSTP)
		return;

	/* Cancel alarms */
	(void) alarm(0);

	/* Put screen in sane state */
	move(LINES-1, 0);
	printw("\r\n\n");
	putp(cursor_normal);
	refresh();
	reset_shell_mode();

	/* Now stop the process */
	(void) signal(SIGTSTP, SIG_DFL);
	(void) kill((pid_t) getpid(), SIGTSTP);
}


/*
 * oncont
 *	Handler for job control continue signal
 *
 * Args:
 *	signo - The signal number
 *
 * Return:
 *	Nothing.
 */
STATIC void
oncont(int signo)
{
	if (signo != SIGCONT)
		return;

	(void) signal(SIGTSTP, ontstp);
	(void) signal(SIGCONT, oncont);

	/* Restore visual attributes */
	reset_prog_mode();
	putp(cursor_invisible);

	/* Set up for auto refresh */
	wclear(info_win);
	wclear(status_win);
	oastat0 = oastat1 = (word32_t) -1;
	ostat_on = !stat_on;
	old_help = !help;
	old_route = !route;
	refresh_fkeys = TRUE;
}
#endif	/* SIGTSTP SIGCONT */


/***********************
 *   public routines   *
 ***********************/


/*
 * cda_vtidy
 *	Tidy up and go home for visual mode.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
void
cda_vtidy(void)
{
	if (isvisual) {
		keypad(stdscr, FALSE);
		putp(cursor_normal);
		clear();

		move(LINES-1, 0);
		refresh();
		echo();
		nocbreak();
		endwin();
	}

	(void) printf("%s\n", errmsg != NULL ? errmsg : "Goodbye!");
}


/* Keyboard input to command function mapping table */
STATIC keytbl_t	ktbl[] = {
	{ KEY_F(1),	cda_v_on_off		},
	{ 'o',		cda_v_on_off		},
	{ 'O',		cda_v_on_off		},
	{ KEY_F(2),	cda_v_load_eject	},
	{ 'j',		cda_v_load_eject	},
	{ 'J',		cda_v_load_eject	},
	{ KEY_F(3),	cda_v_play_pause	},
	{ 'p',		cda_v_play_pause	},
	{ 'P',		cda_v_play_pause	},
	{ KEY_F(4),	cda_v_stop		},
	{ 's',		cda_v_stop		},
	{ 'S',		cda_v_stop		},
	{ KEY_F(5),	cda_v_lock		},
	{ 'k',		cda_v_lock		},
	{ 'K',		cda_v_lock		},
	{ KEY_F(6),	cda_v_shufprog		},
	{ 'u',		cda_v_shufprog		},
	{ 'U',		cda_v_shufprog		},
	{ KEY_F(7),	cda_v_repeat		},
	{ 'e',		cda_v_repeat		},
	{ 'E',		cda_v_repeat		},
	{ 'd',		cda_v_disc		},
	{ 'D',		cda_v_disc		},
	{ KEY_LEFT,	cda_v_track		},
	{ KEY_RIGHT,	cda_v_track		},
	{ 'C',		cda_v_track		},
	{ 'c',		cda_v_track		},
	{ '<',		cda_v_index		},
	{ '>',		cda_v_index		},
	{ '+',		cda_v_volume		},
	{ '-',		cda_v_volume		},
	{ 'l',		cda_v_balance		},
	{ 'L',		cda_v_balance		},
	{ 'r',		cda_v_balance		},
	{ 'R',		cda_v_balance		},
	{ '\011',	cda_v_route		},
	{ '?',		cda_v_help		},
	{ ' ',		cda_v_help		},
	{ '\014',	cda_v_repaint		},
	{ '\022',	cda_v_repaint		},
	{ KEY_UP,	cda_v_scroll		},
	{ KEY_DOWN,	cda_v_scroll		},
	{ '^',		cda_v_scroll		},
	{ 'v',		cda_v_scroll		},
	{ '\036',	cda_v_scroll		},
	{ '\026',	cda_v_scroll		},
	{ 'V',		cda_v_scroll		},
	{ KEY_PPAGE,	cda_v_scroll		},
	{ KEY_NPAGE,	cda_v_scroll		},
	{ '\002',	cda_v_scroll		},
	{ '\025',	cda_v_scroll		},
	{ '\006',	cda_v_scroll		},
	{ '\004',	cda_v_scroll		},
	{ 'd',		cda_v_debug		},
	{ '\0',		cda_v_dirtrk		},
	{ '\0',		NULL			}
};


/*
 * cda_visual
 *	Visual (curses mode) interface.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
void
cda_visual(void)
{
	word32_t	arg[CDA_NARGS];
	int		inp,
			i,
			retcode;
	curstat_t	*s = curstat_addr();

#if defined(SIGTSTP) && defined(SIGCONT)
	/* Handle job control */
	(void) signal(SIGTSTP, ontstp);
	(void) signal(SIGCONT, oncont);
#endif

	/* Initialize curses and paint initial screen */
	initscr();
	isvisual = TRUE;

	noecho();
	cbreak();
	putp(cursor_invisible);

	if ((info_win = newpad(MAXTRACK * 2, COLS)) == (WINDOW *) NULL) {
		cda_quit(s);
		exit(2);
	}

	keypad(info_win, TRUE);

	if ((status_win = newwin(7, COLS, LINES-7, 0)) == (WINDOW *) NULL) {
		cda_quit(s);
		exit(2);
	}

	keypad(status_win, TRUE);

	wmove(status_win, 3, 0);
	waddstr(status_win, STATUS_LINE0);
	wmove(status_win, 4, 0);
	waddstr(status_win, STATUS_LINE1);
	wmove(status_win, 5, 0);
	waddstr(status_win, STATUS_LINE2);
	wrefresh(status_win);

	box(status_win, 0, 0);

	/* Paint the screen every second */
	cda_screen(0);

	/* Main processing loop */
	for (;;) {
		inp = cda_wgetch(status_win);

		if (inp == KEY_F(8) || inp == 'q' || inp == 'Q')
			break;

		/* Cancel alarm so we don't nest */
		(void) alarm(0);

		/* Get current status */
		if (stat_on && !cda_sendcmd(CDA_STATUS, arg, &retcode))
			cda_v_errhandler(retcode);

		/* Execute command function */
		for (i = 0; ktbl[i].keyfunc != NULL; i++) {
			if (ktbl[i].key == inp || ktbl[i].key == '\0') {
				ktbl[i].keyfunc(inp, arg);
				break;
			}
		}

		/* Repaint screen */
		cda_screen(0);
	}

	/* Tidy up and go home */
	(void) alarm(0);
	errmsg = NULL;

	if (cda_daemon_alive()) {
		(void) sprintf(emsg, "CD audio daemon pid=%d still running.",
			       (int) daemon_pid);
		errmsg = emsg;
	}

	cda_quit(s);
	exit(0);
}


#endif	/* NOVISUAL */

