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
#ifndef LINT
static char *_cda_c_ident_ = "@(#)cda.c	6.110 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "common_d/patchlevel.h"
#include "libdi_d/libdi.h"
#include "cddb_d/cddb.h"
#include "cda_d/cda.h"
#include "cda_d/visual.h"

#if !defined(USE_TERMIOS) && !defined(USE_TERMIO) && !defined(USE_SGTTY)
#define USE_TERMIOS	/* Make USE_TERMIOS the default if not specified */
#endif

#ifdef USE_TERMIOS
#include <termios.h>
#endif
#ifdef USE_TERMIO
#include <termio.h>
#endif
#ifdef USE_SGTTY
#include <sgtty.h>
#endif

#define PGM_SEPCHAR	','			/* Program seq separator */
#define MAX_PKT_RETRIES	200			/* Max retries reading pkt */
#define PIPE_TIMEOUT	5			/* 5 seconds to time out */


extern char		*ttyname();

appdata_t		app_data;		/* Option settings */
cddb_incore_t		cur_db;			/* Database entry of CD */
curstat_t		status;			/* Current CD player status */
char			*errmsg,		/* Error msg for use on exit */
			emsg[ERR_BUF_SZ],	/* Error message buffer */
			spipe[FILE_PATH_SZ],	/* Send pipe path */
			rpipe[FILE_PATH_SZ];	/* Receive pipe path */
int			cda_sfd[2] = {-1,-1},	/* Send pipe file desc */
			cda_rfd[2] = {-1,-1};	/* Receive pipe file desc */
pid_t			daemon_pid;		/* CDA daemon pid */
FILE			*errfp = stderr;	/* Error message stream */

STATIC int		cont_delay = 1;		/* Status display interval */
STATIC dev_t		cd_rdev;		/* CD device number */
STATIC bool_t		visual,			/* Visual (curses) mode */
			isdaemon,		/* Am I the daemon process */
			stat_cont,		/* Continuous display status */
			cddb,			/* CDDB entry exists */
			batch;			/* Non-interactive */
STATIC FILE		*ttyfp;			/* /dev/tty */
STATIC cddb_client_t	cddb_cldata;		/* Client info for libcddb */
STATIC di_client_t	di_cldata;		/* Client info for libdi */
STATIC char		dlock[FILE_PATH_SZ];	/* Daemon lock file path */


/***********************
 *  internal routines  *
 ***********************/


/*
 * onsig0
 *	Signal handler to shut down application.
 *
 * Args:
 *	signo - The signal number.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
onsig0(int signo)
{
	cda_quit(&status);
	exit(3);
}


/*
 * onsig1
 *	SIGPIPE signal handler.
 *
 * Args:
 *	signo - The signal number.
 *
 * Return:
 *	Nothing.
 */
STATIC void
onsig1(int signo)
{
	/* Do nothing except re-arm the handler */
	(void) signal(signo, onsig1);
}


/*
 * onintr
 *	Signal handler to stop continuous status display.
 *
 * Args:
 *	signo - The signal number.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
onintr(int signo)
{
	(void) signal(signo, SIG_IGN);
	stat_cont = FALSE;
	(void) printf("\r");
}


/*
 * cda_echo
 *	Turn off/on echo mode.  It is assumed that the program starts
 *	with echo mode enabled.
 *
 * Args:
 *	doecho - Whether echo should be enabled
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_echo(FILE *fp, bool_t doecho)
{
	int			fd = fileno(fp);
#ifdef USE_TERMIOS
	struct termios		tio;

	(void) tcgetattr(fd, &tio);

	if (doecho)
		tio.c_lflag |= ECHO;
	else
		tio.c_lflag &= ~ECHO;

	(void) tcsetattr(fd, TCSADRAIN, &tio);
#endif	/* USE_TERMIOS */

#ifdef USE_TERMIO
	struct termio		tio;

	(void) ioctl(fd, TCGETA, &tio);

	if (doecho)
		tio.c_lflag |= ECHO;
	else
		tio.c_lflag &= ~ECHO;

	(void) ioctl(fd, TCSETAW, &tio);
#endif	/* USE_TERMIO */

#ifdef USE_SGTTY
	struct sgttyb		tio;

	(void) ioctl(fd, TIOCGETP, &tio);

	if (doecho)
		tio.sg_flags |= ECHO;
	else
		tio.sg_flags &= ~ECHO;

	(void) ioctl(fd, TIOCSETP, &tio);
#endif	/* USE_SGTTY */
}


/*
 * cda_pgm_parse
 *	Parse the shuffle/program mode play sequence text string, and
 *	update the playorder table in the curstat_t structure.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	TRUE=success, FALSE=error.
 */
/*ARGSUSED*/
STATIC bool_t
cda_pgm_parse(curstat_t *s)
{
	int		i,
			progtot,
			retcode;
	char		*p,
			*q,
			*tmpbuf;
	word32_t	*arg;
	bool_t		last = FALSE,
			ret = TRUE;

	if (cur_db.playorder == NULL)
		/* Nothing to do */
		return TRUE;

	arg = (word32_t *) MEM_ALLOC(
		"pgm_parse_arg",
		CDA_NARGS * sizeof(word32_t)
	);
	tmpbuf = (char *) MEM_ALLOC(
		"pgm_parse_tmpbuf",
		strlen(cur_db.playorder) + 1
	);
	if (arg == NULL || tmpbuf == NULL) {
		CDA_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	(void) strcpy(tmpbuf, cur_db.playorder);

	(void) memset(arg, 0, CDA_NARGS * sizeof(word32_t));
	progtot = 0;

	for (i = 0, p = q = tmpbuf; i < MAXTRACK; i++, p = ++q) {
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
			MEM_FREE(arg);
			MEM_FREE(tmpbuf);

			return FALSE;
		}

		if (q > p) {
			arg[i+1] = (word32_t) atoi(p);
			progtot++;
		}

		if (last)
			break;
	}

	if (progtot > 0) {
		arg[0] = -progtot;
		ret = cda_sendcmd(CDA_PROGRAM, arg, &retcode);
	}

	MEM_FREE(arg);
	MEM_FREE(tmpbuf);

	return (ret);
}


/*
 * cda_init
 *	Initialize the CD interface subsystem.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_init(curstat_t *s)
{
	char	*bdevname,
		str[FILE_PATH_SZ * 2];

	bdevname = util_basename(app_data.device);

	/* Get system-wide device-specific configuration parameters */
	(void) sprintf(str, SYS_DSCFG_PATH, app_data.libdir, bdevname);
	di_devspec_parmload(str, TRUE);

	/* Get user device-specific configuration parameters */
	(void) sprintf(str, USR_DSCFG_PATH, util_homedir(util_get_ouid()),
		       bdevname);
	di_devspec_parmload(str, FALSE);

	/* Initialize the CD hardware */
	di_cldata.curstat_addr = curstat_addr;
	di_cldata.quit = cda_quit;
	di_cldata.dbclear = cda_dbclear;
	di_cldata.fatal_msg = cda_fatal_msg;
	di_cldata.warning_msg = cda_warning_msg;
	di_init(&di_cldata);

	/* Set default modes */
	di_repeat(s, app_data.repeat_mode);
	di_shuffle(s, app_data.shuffle_mode);
}


/*
 * cda_start
 *	Secondary startup functions.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_start(curstat_t *s)
{
	/* Start up libutil */
	util_start();

	/* Start up I/O interface */
	di_start(s);

	/* Open FIFOs - daemon side */
	if ((cda_sfd[0] = open(spipe, O_RDONLY)) < 0) {
		perror("CD audio daemon: cannot open send pipe");
		cda_quit(s);
		exit(4);
	}
	if ((cda_rfd[0] = open(rpipe, O_WRONLY)) < 0) {
		perror("CD audio daemon: cannot open recv pipe");
		cda_quit(s);
		exit(5);
	}
}


/*
 * cda_daemonlock
 *	Check and set a lock to prevent multiple CD audio daemon
 *	processes.
 *
 * Args:
 *	None.
 *
 * Return:
 *	TRUE - Lock successful
 *	FALSE - Daemon already running
 */
STATIC bool_t
cda_daemonlock(void)
{
	int		fd;
	pid_t		pid,
			mypid;
	char		buf[12];

	(void) sprintf(dlock, "%s/cdad.%x", TEMP_DIR, (int) cd_rdev);
	mypid = getpid();

	for (;;) {
		fd = open(dlock, O_CREAT | O_EXCL | O_WRONLY);
		if (fd < 0) {
			if (errno == EEXIST) {
				if ((fd = open(dlock, O_RDONLY)) < 0)
					return FALSE;

				if (read(fd, buf, 12) > 0)
					pid = (pid_t) atoi(buf);
				else {
					(void) close(fd);
					return FALSE;
				}

				(void) close(fd);

				if (pid == mypid)
					/* Our own lock */
					return TRUE;

				if (pid <= 0 ||
				    (kill(pid, 0) < 0 && errno == ESRCH)) {
					/* Pid died, steal its lockfile */
					(void) UNLINK(dlock);
				}
				else {
					/* Pid still running: clash */
					return FALSE;
				}
			}
			else
				return FALSE;
		}
		else {
			(void) sprintf(buf, "%d\n", (int) mypid);
			(void) write(fd, buf, strlen(buf));

			(void) close(fd);
			(void) chmod(dlock, 0644);

			return TRUE;
		}
	}
}


/*
 * cda_poll
 *	Periodic polling function - used to manage program, shuffle,
 *	and repeat modes.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
cda_poll(curstat_t *s)
{
	static int	n = 0;

	if (++n > 100)
		n = 0;

	if (n % 3)
		return;

	if (s->mode == MOD_STOP && app_data.load_play)
		(void) di_check_disc(s);
	else if (s->mode != MOD_STOP && s->mode != MOD_PAUSE &&
		 s->mode != MOD_NODISC)
		di_status_upd(s);
}


/*
 * cda_sendpkt
 *	Write a CDA packet down the pipe.
 *
 * Args:
 *	name - The text string describing the caller module
 *	fd - Pipe file descriptor
 *	s - Pointer to the packet data
 *
 * Return:
 *	TRUE - pipe write successful
 *	FALSE - pipe write failed
 */
STATIC bool_t
cda_sendpkt(char *name, int fd, cdapkt_t *s)
{
	byte_t	*p = (byte_t *) s;
	int	i,
		ret;

	if (fd < 0)
		return FALSE;

	/* Brand packet with magic number */
	s->magic = CDA_MAGIC;

	/* Set timeout */
	(void) signal(SIGALRM, onsig1);
	(void) alarm(PIPE_TIMEOUT);

	/* Send a packet */
	i = sizeof(cdapkt_t);
	while ((ret = write(fd, p, i)) < i) {
		if (ret < 0) {
			switch (errno) {
			case EINTR:
			case EPIPE:
			case EBADF:
				(void) alarm(0);

				if (isdaemon)
					return FALSE;

				if (!cda_daemon_alive())
					return FALSE;
				
				(void) signal(SIGALRM, onsig1);
				(void) alarm(PIPE_TIMEOUT);
				break;
			default:
				(void) alarm(0);
				(void) sprintf(emsg,
					"%s: packet write error (errno=%d)\n",
					name, errno);
				CDA_WARNING(emsg);
				return FALSE;
			}
		}
		else if (ret == 0) {
			(void) alarm(0);
			(void) sleep(1);
			(void) signal(SIGALRM, onsig1);
			(void) alarm(PIPE_TIMEOUT);
		}
		else {
			i -= ret;
			p += ret;
		}
	}

	(void) alarm(0);
	return TRUE;
}


/*
 * cda_getpkt
 *	Read a CDA packet from the pipe.
 *
 * Args:
 *	name - The text string describing the caller module
 *	fd - Pipe file descriptor
 *	s - Pointer to the packet data
 *
 * Return:
 *	TRUE - pipe read successful
 *	FALSE - pipe read failed
 */
STATIC bool_t
cda_getpkt(char *name, int fd, cdapkt_t *r)
{
	byte_t	*p = (byte_t *) r;
	int	i,
		ret;

	if (fd < 0)
		return FALSE;

	/* Get a packet */
	i = sizeof(cdapkt_t);
	while ((ret = read(fd, p, i)) < i) {
		if (ret < 0) {
			switch (errno) {
			case EINTR:
			case EPIPE:
			case EBADF:
				if (!isdaemon)
					return FALSE;

				/* Use this occasion to perform
				 * polling function
				 */
				cda_poll(&status);

				(void) sleep(1);
				break;
			default:
				(void) sprintf(emsg,
					"%s: packet read error (errno=%d)\n",
					name, errno);
				CDA_WARNING(emsg);
				return FALSE;
			}
		}
		else if (ret == 0) {
			/* Use this occasion to perform polling function */
			if (isdaemon)
				cda_poll(&status);

			(void) sleep(1);
		}
		else {
			i -= ret;
			p += ret;
		}
	}

	/* Check packet for magic number */
	if (r->magic != CDA_MAGIC) {
		(void) sprintf(emsg, "%s: bad packet magic number.", name);
		CDA_WARNING(emsg);
		return FALSE;
	}

	return TRUE;
}


/*
 * cda_docmd
 *	Perform the command received.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	p - Pointer to the CDA packet structure.
 *
 * Return:
 *	TRUE - Received a CDA_OFF command: daemon should shut down
 *	FALSE - Received normal command.
 */
STATIC bool_t
cda_docmd(curstat_t *s, cdapkt_t *p)
{
	int		i,
			j,
			min,
			sec;
	word32_t	blkno,
			discid;
	bool_t		stopfirst,
			offsets;
	static time_t	cur_time,
			prev_time;

	/* Update CD status */
	if (p->cmd != CDA_OFF) {
		if (s->mode == MOD_NODISC || s->mode == MOD_BUSY ||
		    (s->mode == MOD_STOP && app_data.load_play))
			(void) di_check_disc(s);
		else if (s->mode != MOD_STOP && s->mode != MOD_PAUSE) {
			prev_time = cur_time;
			cur_time = time(NULL);

			if (cur_time != prev_time)
				di_status_upd(s);
		}
	}

	/* Default status */
	p->retcode = CDA_OK;

	switch (p->cmd) {
	case CDA_ON:
		p->arg[0] = getpid();
		break;

	case CDA_OFF:
		p->arg[0] = getpid();
		return TRUE;

	case CDA_DISC:
		if (s->mode == MOD_BUSY)
			p->retcode = CDA_INVALID;
		else if (p->arg[0] == 0) {
			/* Load */
			if (s->mode == MOD_NODISC)
				di_load_eject(s);
			else
				p->retcode = CDA_INVALID;
		}
		else if (p->arg[0] == 1) {
			/* Eject */
			if (s->mode != MOD_NODISC)
				di_load_eject(s);
			else
				p->retcode = CDA_INVALID;
		}
		else if (p->arg[0] == 2) {
			/* Next */
			if (s->cur_disc < s->last_disc) {
				s->prev_disc = s->cur_disc;
				s->cur_disc++;
				di_chgdisc(s);
			}
			else
				p->retcode = CDA_INVALID;
		}
		else if (p->arg[0] == 3) {
			/* Prev */
			if (s->cur_disc > s->first_disc) {
				s->prev_disc = s->cur_disc;
				s->cur_disc--;
				di_chgdisc(s);
			}
			else
				p->retcode = CDA_INVALID;
		}
		else if (p->arg[0] == 4) {
			/* disc# */
			i = p->arg[1];
			if (i >= s->first_disc && i <= s->last_disc) {
				s->prev_disc = s->cur_disc;
				s->cur_disc = i;
				di_chgdisc(s);
			}
			else
				p->retcode = CDA_INVALID;
		}
		break;

	case CDA_LOCK:
		if (s->mode == MOD_NODISC || s->mode == MOD_BUSY)
			p->retcode = CDA_INVALID;
		else if (p->arg[0] == 0) {
			/* Unlock */
			if (s->caddy_lock) {
				s->caddy_lock = FALSE;
				di_lock(s, FALSE);
			}
			else
				p->retcode = CDA_INVALID;
		}
		else {
			/* Lock */
			if (!s->caddy_lock) {
				s->caddy_lock = TRUE;
				di_lock(s, TRUE);
			}
			else
				p->retcode = CDA_INVALID;
		}

		break;

	case CDA_PLAY:
		switch (s->mode) {
		case MOD_PLAY:
			stopfirst = TRUE;
			break;
		case MOD_BUSY:
		case MOD_NODISC:
			p->retcode = CDA_INVALID;
			return FALSE;
		default:
			stopfirst = FALSE;
			break;
		}

		/* Starting track number */
		i = -1;
		if (p->arg[0] != 0) {
			if (s->shuffle || s->prog_cnt > 0) {
				p->retcode = CDA_INVALID;
				break;
			}

			for (i = 0; i < (int) s->tot_trks; i++) {
				if (s->trkinfo[i].trkno == p->arg[0])
					break;
			}

			if (i >= (int) s->tot_trks) {
				/* Invalid track specified */
				p->retcode = CDA_PARMERR;
				break;
			}

			s->cur_trk = p->arg[0];
		}

		if (stopfirst) {
			/* Stop current playback first */
			di_stop(s, FALSE);

			/*
			 * Restore s->cur_trk value because di_stop() zaps it
			 */
			if (p->arg[0] != 0)
				s->cur_trk = p->arg[0];
		}

		if (p->arg[0] != 0 &&
		    (int) p->arg[1] >= 0 && (int) p->arg[2] >= 0) {
			/* Track offset specified */
			if (p->arg[2] > 59) {
				p->retcode = CDA_PARMERR;
				break;
			}

			util_msftoblk(
				(byte_t) p->arg[1],
				(byte_t) p->arg[2],
				0,
				&blkno,
				0
			);

			if (blkno >=
			    (s->trkinfo[i+1].addr - s->trkinfo[i].addr)) {
				p->retcode = CDA_PARMERR;
				break;
			}

			s->cur_trk_addr = blkno;
			s->cur_trk_min = (byte_t) p->arg[1];
			s->cur_trk_sec = (byte_t) p->arg[2];
			s->cur_trk_frame = 0;

			s->cur_tot_addr = s->trkinfo[i].addr + s->cur_trk_addr;
			util_blktomsf(
				s->cur_tot_addr,
				&s->cur_tot_min,
				&s->cur_tot_sec,
				&s->cur_tot_frame,
				MSF_OFFSET
			);
		}

		/* Start playback */
		di_play_pause(s);

		break;

	case CDA_PAUSE:
		if (s->mode == MOD_PLAY)
			di_play_pause(s);
		else
			p->retcode = CDA_INVALID;

		break;

	case CDA_STOP:
		if (s->mode == MOD_BUSY)
			p->retcode = CDA_INVALID;
		else
			di_stop(s, TRUE);
		break;

	case CDA_TRACK:
		if (p->arg[0] == 0) {
			/* Previous track */
			if (s->mode == MOD_PLAY) {
				if ((i = di_curtrk_pos(s)) > 0)
					s->cur_tot_addr = s->trkinfo[i].addr;
				di_prevtrk(s);
			}
			else
				p->retcode = CDA_INVALID;
		}
		else {
			/* Next track */
			if (s->mode == MOD_PLAY)
				di_nexttrk(s);
			else
				p->retcode = CDA_INVALID;
		}

		break;

	case CDA_INDEX:
		if (p->arg[0] == 0) {
			/* Previous index */
			if (s->mode == MOD_PLAY && s->prog_tot <= 0) {
				if (s->cur_idx > 1)
					s->cur_tot_addr = s->sav_iaddr;
				di_previdx(s);
			}
			else
				p->retcode = CDA_INVALID;
		}
		else {
			/* Next index */
			if (s->mode == MOD_PLAY && s->prog_tot <= 0)
				di_nextidx(s);
			else
				p->retcode = CDA_INVALID;
		}

		break;

	case CDA_PROGRAM:
		if (s->mode == MOD_NODISC || s->mode == MOD_BUSY)
			p->retcode = CDA_INVALID;
		else if ((int) p->arg[0] > 0) {
			/* Query */
			p->arg[0] = (word32_t) s->prog_tot;
			p->arg[1] = (word32_t) -1;

			for (i = 0; i < (int) s->prog_tot; i++) {
				p->arg[i+1] = (word32_t)
				    s->trkinfo[s->playorder[i]].trkno;
			}
		}
		else if (p->arg[0] == 0) {
			/* Clear */
			if (s->shuffle) {
				/* program and shuffle modes are mutually-
				 * exclusive.
				 */
				p->retcode = CDA_INVALID;
			}
			else {
				p->arg[1] = 0;
				s->prog_tot = 0;
				s->program = FALSE;
			}
		}
		else if ((int) p->arg[0] < 0) {
			/* Define */
			if (s->shuffle) {
				/* program and shuffle modes are mutually-
				 * exclusive.
				 */
				p->retcode = CDA_INVALID;
				break;
			}

			s->prog_tot = -(p->arg[0]);
			s->program = TRUE;

			for (i = 0; i < (int) s->prog_tot; i++) {
				for (j = 0; j < (int) s->tot_trks; j++) {
					if (s->trkinfo[j].trkno == p->arg[i+1])
						break;
				}

				if (j >= (int) s->tot_trks) {
					s->prog_tot = 0;
					s->program = FALSE;
					p->retcode = CDA_PARMERR;
					break;
				}

				s->playorder[i] = j;
			}
		}
		else
			p->retcode = CDA_PARMERR;

		break;

	case CDA_SHUFFLE:
		if (s->program) {
			p->retcode = CDA_INVALID;
			break;
		}

		if (s->mode != MOD_NODISC && s->mode != MOD_STOP) {
			p->retcode = CDA_INVALID;
			break;
		}

		di_shuffle(s, (bool_t) (p->arg[0] == 1));
		break;

	case CDA_REPEAT:
		di_repeat(s, (bool_t) (p->arg[0] == 1));
		break;

	case CDA_VOLUME:
		if (p->arg[0] == 0)
			/* Query */
			p->arg[1] = (word32_t) s->level;
		else if ((int) p->arg[1] >= 0 && (int) p->arg[1] <= 100)
			/* Set */
			di_level(s, (byte_t) p->arg[1], FALSE);
		else
			p->retcode = CDA_PARMERR;

		break;

	case CDA_BALANCE:
		if (p->arg[0] == 0) {
			/* Query */
			p->arg[1] = (word32_t)
			    ((int) (s->level_right - s->level_left) / 2) + 50;
		}
		else if ((int) p->arg[1] == 50) {
			/* Center setting */
			s->level_left = s->level_right = 100;
			di_level(s, (byte_t) s->level, FALSE);
		}
		else if ((int) p->arg[1] < 50 && (int) p->arg[1] >= 0) {
			/* Attenuate the right channel */
			s->level_left = 100;
			s->level_right = 100 + (((int) p->arg[1] - 50) * 2);
			di_level(s, (byte_t) s->level, FALSE);
		}
		else if ((int) p->arg[1] > 50 && (int) p->arg[1] <= 100) {
			/* Attenuate the left channel */
			s->level_left = 100 - (((int) p->arg[1] - 50) * 2);
			s->level_right = 100;
			di_level(s, (byte_t) s->level, FALSE);
		}
		else
			p->retcode = CDA_PARMERR;

		break;

	case CDA_ROUTE:
		if (p->arg[0] == 0) {
			/* Query */
			p->arg[1] = (word32_t) app_data.ch_route;
		}
		else if ((int) p->arg[1] >= 0 && (int) p->arg[1] <= 4) {
			/* Set */
			app_data.ch_route = (int) p->arg[1];
			di_route(s);
		}
		else
			p->retcode = CDA_PARMERR;

		break;

	case CDA_STATUS:
		/* Initialize */
		(void) memset(p->arg, 0, CDA_NARGS * sizeof(word32_t));

		WR_ARG_MODE(p->arg[0], s->mode);

		if (s->caddy_lock)
			WR_ARG_LOCK(p->arg[0]);
		if (s->shuffle)
			WR_ARG_SHUF(p->arg[0]);
		if (s->repeat)
			WR_ARG_REPT(p->arg[0]);
		if (s->program)
			WR_ARG_PROG(p->arg[0]);

		WR_ARG_TRK(p->arg[1], s->cur_trk);
		WR_ARG_IDX(p->arg[1], s->cur_idx);
		WR_ARG_MIN(p->arg[1], s->cur_trk_min);
		WR_ARG_SEC(p->arg[1], s->cur_trk_sec);

		if (s->repeat && s->mode == MOD_PLAY)
			p->arg[2] = (word32_t) s->rptcnt;
		else
			p->arg[2] = (word32_t) -1;

		p->arg[3] = (word32_t) s->level;
		p->arg[4] = (word32_t)
			    ((int) (s->level_right - s->level_left) / 2) + 50;
		p->arg[5] = (word32_t) app_data.ch_route;
		if (s->mode == MOD_NODISC || s->mode == MOD_BUSY)
			discid = 0;
		else
			discid = cddb_discid(s);

		if (discid != cur_db.discid)
			cur_db.queryid = 0;

		cur_db.discid = discid;
		p->arg[7] = s->cur_disc;
		p->arg[6] = discid;
		p->arg[8] = cur_db.queryid;
		break;

	case CDA_TOC:
		if (s->mode == MOD_NODISC || s->mode == MOD_BUSY) {
			p->retcode = CDA_INVALID;
			break;
		}

		offsets = (p->arg[0] == 1);

		/* Initialize */
		(void) memset(p->arg, 0, CDA_NARGS * sizeof(word32_t));

		p->arg[0] = cddb_discid(s);

		if (offsets) {
			for (i = 0; i < (int) s->tot_trks; i++) {
				WR_ARG_TOC(p->arg[i+1], s->trkinfo[i].trkno,
					   s->cur_trk,
					   s->trkinfo[i].min,
					   s->trkinfo[i].sec);
			}
		}
		else {
			for (i = 0; i < (int) s->tot_trks; i++) {
				j = ((s->trkinfo[i+1].min * 60 +
				      s->trkinfo[i+1].sec) - 
				     (s->trkinfo[i].min * 60 +
				      s->trkinfo[i].sec));
				min = j / 60;
				sec = j % 60;

				WR_ARG_TOC(p->arg[i+1], s->trkinfo[i].trkno,
					   s->cur_trk, min, sec);
			}
		}

		/* Lead-out track */
		WR_ARG_TOC(p->arg[i+1], s->trkinfo[i].trkno,
			   0, s->trkinfo[i].min, s->trkinfo[i].sec);

		break;

	case CDA_TOC2:
		if (s->mode == MOD_NODISC || s->mode == MOD_BUSY) {
			p->retcode = CDA_INVALID;
			break;
		}

		/* Initialize */
		(void) memset(p->arg, 0, CDA_NARGS * sizeof(word32_t));

		p->arg[0] = cddb_discid(s);

		for (i = 0; i < (int) s->tot_trks; i++) {
			WR_ARG_TOC2(p->arg[i+1], s->trkinfo[i].trkno,
				   s->trkinfo[i].min,
				   s->trkinfo[i].sec,
				   s->trkinfo[i].frame
			);
		}

		/* Lead-out track */
		WR_ARG_TOC2(p->arg[i+1], s->trkinfo[i].trkno,
			   s->trkinfo[i].min,
			   s->trkinfo[i].sec,
			   s->trkinfo[i].frame
		);

		break;

	case CDA_SETQUERYID:
		cur_db.queryid = p->arg[0];
		break;

	case CDA_EXTINFO:
		if (s->mode == MOD_NODISC || s->mode == MOD_BUSY) {
			p->retcode = CDA_INVALID;
			break;
		}

		p->arg[0] = cddb_discid(s);
		p->arg[2] = (word32_t) -1;

		if ((int) p->arg[1] == -1) {
			if (s->mode != MOD_PLAY) {
				p->arg[1] = p->arg[2] = (word32_t) -1;
				break;
			}

			p->arg[1] = (word32_t) s->cur_trk;
			j = (int) s->cur_trk;
		}
		else
			j = (int) p->arg[1];

		for (i = 0; i < (int) s->tot_trks; i++) {
			if ((int) s->trkinfo[i].trkno == j)
				break;
		}
		if (i < (int) s->tot_trks)
			p->arg[2] = i;
		else
			p->retcode = CDA_PARMERR;

		break;

	case CDA_DEVICE:
		(void) sprintf((char *) p->arg,
			    "CD-ROM: %s %s (%s)\nMode:   %s",
			    s->vendor, s->prod, s->revnum, di_mode());
		break;

	case CDA_VERSION:
		(void) sprintf((char *) p->arg, "%s%s PL%d\n%s\n%s %s\n",
			    VERSION, VERSION_EXT, PATCHLEVEL, di_vers(),
			    "Remote database access",
			    cddb_rmt_support() ? "enabled" : "disabled");
		break;

	case CDA_DEBUG:
		if (p->arg[0] == 0) {
			/* Query */
			p->arg[1] = (word32_t) app_data.debug;
		}
		else {
			/* Set/Clear */
			app_data.debug = (bool_t) p->arg[1];
		}
		break;

	default:
		p->retcode = CDA_FAILED;
		break;
	}

	return FALSE;
}


/*
 * prn_program
 *	Print current program sequence, if any.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_program(word32_t arg[])
{
	int	i;

	if ((int) arg[0] > 0) {
		(void) printf("Current program:");
		for (i = 0; i < arg[0]; i++)
			(void) printf(" %d", arg[i+1]);
		(void) printf("\n");
	}
	else if (arg[0] == 0 && (int) arg[1] == -1)
		(void) printf("No play sequence defined.\n");
}


/*
 * prn_vol
 *	Print current volume setting.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_vol(word32_t arg[])
{
	if (arg[0] == 0)
		(void) printf("Current volume: %u (range 0-100)\n", arg[1]);
}


/*
 * prn_bal
 *	Print current balance setting.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_bal(word32_t arg[])
{
	if (arg[0] == 0) {
		(void) printf("Current balance: %u (range 0-100, center:50)\n",
			      arg[1]);
	}
}


/*
 * prn_route
 *	Print current channel routing setting.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_route(word32_t arg[])
{
	if (arg[0] == 0) {
		(void) printf("Current routing: %u ", arg[1]);

		switch (arg[1]) {
		case 0:
			(void) printf("(normal stereo)\n");
			break;
		case 1:
			(void) printf("(reverse stereo)\n");
			break;
		case 2:
			(void) printf("(mono-L)\n");
			break;
		case 3:
			(void) printf("(mono-R)\n");
			break;
		case 4:
			(void) printf("(mono-L+R)\n");
			break;
		}
	}
}


/*
 * prn_stat
 *	Print current CD status.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_stat(word32_t arg[])
{
	if (stat_cont)
		(void) printf("\r");

	switch (RD_ARG_MODE(arg[0])) {
	case MOD_BUSY:
		(void) printf("CD_Busy    disc:-  -- --  --:--");
		break;
	case MOD_NODISC:
		(void) printf("No_Disc    disc:-  -- --  --:--");
		break;
	case MOD_STOP:
		(void) printf("CD_Stopped disc:%d  -- --  --:--", arg[7]);
		break;
	case MOD_PLAY:
		(void) printf("CD_Playing disc:%d  %02d %02u %s%02u:%02u",
			      arg[7],
			      RD_ARG_TRK(arg[1]), RD_ARG_IDX(arg[1]),
			      RD_ARG_IDX(arg[1]) == 0 ? "-" : "+",
			      RD_ARG_MIN(arg[1]), RD_ARG_SEC(arg[1]));
		break;
	case MOD_PAUSE:
		(void) printf("CD_Paused  disc:%d  %02u %02d %s%02u:%02u",
			      arg[7],
			      RD_ARG_TRK(arg[1]), RD_ARG_IDX(arg[1]),
			      RD_ARG_IDX(arg[1]) == 0 ? "-" : "+",
			      RD_ARG_MIN(arg[1]), RD_ARG_SEC(arg[1]));
		break;
	default:
		(void) printf("Inv_status disc:-  -- --  --:--");
		break;
	}

	(void) printf(" %slock", RD_ARG_LOCK(arg[0]) ? "+" : "-");
	(void) printf(" %sshuf", RD_ARG_SHUF(arg[0]) ? "+" : "-");
	(void) printf(" %sprog", RD_ARG_PROG(arg[0]) ? "+" : "-");
	(void) printf(" %srept", RD_ARG_REPT(arg[0]) ? "+" : "-");

	if ((int) arg[2] >= 0)
		(void) printf(" %u", arg[2]);
	else
		(void) printf(" -");

	if (!stat_cont)
		(void) printf("\n");
}


/*
 * prn_toc
 *	Print current CD Table Of Contents.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_toc(word32_t arg[])
{
	int		i;
	byte_t		ntrks,
			trkno,
			min,
			sec;
	bool_t		playing;

	ntrks = arg[0] & 0xff;

	(void) printf("Disc ID: %s %08x%s\n",
		      (cur_db.category[0] == '\0') ?
			    "(no category)" : cur_db.category,
		arg[0],
		(cur_db.extd == NULL) ? "" : " *");

	if (cddb && cur_db.dtitle != NULL)
		(void) printf("%s\n\n", cur_db.dtitle);
	else
		(void) printf("(unknown disc title)\n\n");

	for (i = 0; i < (int) ntrks; i++) {
		RD_ARG_TOC(arg[i+1], trkno, playing, min, sec);
		(void) printf("%s%02u %02u:%02u  ",
			      playing ? ">" : " ", trkno, min, sec);
		if (cddb && cur_db.trklist[i] != NULL)
			(void) printf("%s%s\n", cur_db.trklist[i],
				      (cur_db.extt[i] != NULL) ? "*" : "");
		else
			(void) printf("??%s\n",
				      (cur_db.extt[i] != NULL) ? "*" : "");
	}

	RD_ARG_TOC(arg[i+1], trkno, playing, min, sec);
	(void) printf("\nTotal Time: %02u:%02u\n", min, sec);
}


/*
 * prn_extinfo
 *	Print current Disc or Track Extended Information.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_extinfo(word32_t arg[])
{
	if (!cddb) {
		(void) printf("No CD database entry found for this CD\n");
		return;
	}

	(void) printf("-------- Disc Extended Information --------\n");

	if (cur_db.extd == NULL)
		(void) printf("(none)\n");
	else {
		if (cur_db.dtitle != NULL)
			(void) printf("%s\n\n", cur_db.dtitle);
		else
			(void) printf("(unknown disc title)\n\n");

		(void) printf("%s\n", cur_db.extd);
	}

	if ((int) arg[1] < 0)
		return;

	(void) printf("\n------ Track %02u Extended Information ------\n",
		      arg[1]);

	if (cur_db.extt[arg[2]] == NULL)
		(void) printf("(none)\n");
	else {
		if (cur_db.trklist[arg[2]] != NULL)
			(void) printf("%s\n\n", cur_db.trklist[arg[2]]);
		else
			(void) printf("(unknown track title)\n\n");

		(void) printf("%s\n", cur_db.extt[arg[2]]);
	}
}


/*
 * prn_device
 *	Print device information.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_device(word32_t arg[])
{
	(void) printf("Device: %s\n", app_data.device);
	(void) printf("%s\n", (char *) arg);
}


/*
 * prn_ver
 *	Print version number and other information.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_ver(word32_t arg[])
{
	(void) printf("CDA - Command Line CD Audio Player\n\n");
	(void) printf("CD audio        v%s%s PL%d\n",
		      VERSION, VERSION_EXT, PATCHLEVEL);
	(void) printf("CD audio daemon v%s\n", (char *) arg);
	(void) printf("%s\n%s\n%s\n\n%s\n",
		      COPYRIGHT, WWWURL, EMAIL, GNU_BANNER);
}


/*
 * prn_debug
 *	Print debug mode.
 *
 * Args:
 *	arg - Argument array from CD audio daemon response packet.
 *
 * Return:
 *	Nothing.
 */
STATIC void
prn_debug(word32_t arg[])
{
	(void) printf("Debug mode is %s%s\n",
		(arg[0] == 0) ? "" : "now ",
		(arg[1] == 0) ? "off" : "on");
}


/*
 * usage
 *	Display command line usage syntax
 *
 * Args:
 *	argc, argv
 *
 * Return:
 *	Nothing.
 */
STATIC void
usage(char *progname)
{
	(void) fprintf(errfp,
		"Usage: %s [-dev device] [-batch] [-debug] command\n",
		progname);
	(void) fprintf(errfp, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"Valid commands are:\n",
		"\ton\n",
		"\toff\n",
		"\tdisc <load | eject | next | prev | disc#>\n",
		"\tlock <on | off>\n",
		"\tplay [track# [min:sec]]\n",
		"\tpause\n",
		"\tstop\n",
		"\ttrack <prev | next>\n",
		"\tindex <prev | next>\n",
		"\tprogram [clear | track# ...]\n",
		"\tshuffle <on | off>\n",
		"\trepeat <on | off>\n",
		"\tvolume [value#]    (range 0-100)\n",
		"\tbalance [value#]   (range 0-100, center:50)\n",
		"\troute [value#]     (0:stereo 1:reverse 2:mono-L 3:mono-R 4:mono-L+R)\n",
		"\tstatus [cont [secs#]]\n",
		"\ttoc [offsets]\n",
		"\textinfo [track#]\n",
		"\tdevice\n",
		"\tversion\n",
		"\tdebug [on | off]\n",
		"\tvisual\n");
}


/*
 * parse_time
 *	Parse a string of the form "min:sec" and convert to integer
 *	minute and second values.
 *
 * Args:
 *	str - Pointer to the "min:sec" string.
 *	min - pointer to where the minute value is to be written.
 *	sec - pointer to where the second value is to be written.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
parse_time(char *str, int *min, int *sec)
{
	char	*p;

	if ((p = strchr(str, ':')) == NULL)
		return FALSE;
	
	if (!isdigit(*str) || !isdigit(*(p+1)))
		return FALSE;

	*p = '\0';
	*min = atoi(str);
	*sec = atoi(p+1);
	*p = ':';

	return TRUE;
}


/*
 * cda_parse_args
 *	Parse CDA command line arguments.
 *
 * Args:
 *	argc, argv
 *	cmd - Pointer to the command code.
 *	arg - Command argument array.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
cda_parse_args(int argc, char **argv, word32_t *cmd, word32_t arg[])
{
	int	i,
		j,
		min,
		sec;

	/* Default values */
	*cmd = 0;
	(void) memset(arg, 0, CDA_NARGS * sizeof(word32_t));

	/* Command line args handling */
	for (i = 1; i < argc; i++) {
		if (*cmd != 0) {
			/* Multiple commands specified */
			usage(argv[0]);
			return FALSE;
		}

		if (strcmp(argv[i], "-dev") == 0) {
			if (++i < argc) {
				if (!di_isdemo())
					app_data.device = argv[i];
			}
			else {
				usage(argv[0]);
				return FALSE;
			}
		}
		else if (strcmp(argv[i], "-debug") == 0) {
			app_data.debug = TRUE;
		}
		else if (strcmp(argv[i], "-batch") == 0) {
			batch = TRUE;
		}
		else if (strcmp(argv[i], "on") == 0) {
			*cmd = CDA_ON;
		}
		else if (strcmp(argv[i], "off") == 0) {
			*cmd = CDA_OFF;
		}
		else if (strcmp(argv[i], "disc") == 0) {
			/* <load | eject | next | prev | disc#> */
			if (++i < argc) {
				if (strcmp(argv[i], "load") == 0)
					arg[0] = 0;
				else if (strcmp(argv[i], "eject") == 0)
					arg[0] = 1;
				else if (strcmp(argv[i], "next") == 0)
					arg[0] = 2;
				else if (strcmp(argv[i], "prev") == 0)
					arg[0] = 3;
				else if (isdigit(argv[i][0])) {
					arg[0] = 4;
					arg[1] = atoi(argv[i]);
				}
				else {
					/* Wrong arg */
					usage(argv[0]);
					return FALSE;
				}
			}
			else {
				/* Missing arg */
				usage(argv[0]);
				return FALSE;
			}
			*cmd = CDA_DISC;
		}
		else if (strcmp(argv[i], "lock") == 0) {
			/* <on | off> */
			if (++i < argc) {
				if (strcmp(argv[i], "off") == 0)
					arg[0] = 0;
				else if (strcmp(argv[i], "on") == 0)
					arg[0] = 1;
				else {
					/* Wrong arg */
					usage(argv[0]);
					return FALSE;
				}
			}
			else {
				/* Missing arg */
				usage(argv[0]);
				return FALSE;
			}
			*cmd = CDA_LOCK;
		}
		else if (strcmp(argv[i], "play") == 0) {
			/* [track# [min:sec]] */
			if ((i+1) < argc && isdigit(argv[i+1][0])) {
				/* The user specified the track number */
				if ((arg[0] = atoi(argv[++i])) == 0) {
					/* Wrong arg */
					usage(argv[0]);
					return FALSE;
				}

				if ((i+1) < argc &&
				    parse_time(argv[i+1], &min, &sec)) {
					/* The user specified a time offset */
					arg[1] = min;
					arg[2] = sec;
					i++;
				}
				else {
					arg[1] = arg[2] = (word32_t) -1;
				}
			}
			*cmd = CDA_PLAY;
		}
		else if (strcmp(argv[i], "pause") == 0) {
			*cmd = CDA_PAUSE;
		}
		else if (strcmp(argv[i], "stop") == 0) {
			*cmd = CDA_STOP;
		}
		else if (strcmp(argv[i], "track") == 0) {
			/* <prev | next> */
			if (++i < argc) {
				if (strcmp(argv[i], "prev") == 0)
					arg[0] = 0;
				else if (strcmp(argv[i], "next") == 0)
					arg[0] = 1;
				else {
					/* Wrong arg */
					usage(argv[0]);
					return FALSE;
				}
			}
			else {
				/* Missing arg */
				usage(argv[0]);
				return FALSE;
			}
			*cmd = CDA_TRACK;
		}
		else if (strcmp(argv[i], "index") == 0) {
			/* <prev | next> */
			if (++i < argc) {
				if (strcmp(argv[i], "prev") == 0)
					arg[0] = 0;
				else if (strcmp(argv[i], "next") == 0)
					arg[0] = 1;
				else {
					/* Wrong arg */
					usage(argv[0]);
					return FALSE;
				}
			}
			else {
				/* Missing arg */
				usage(argv[0]);
				return FALSE;
			}
			*cmd = CDA_INDEX;
		}
		else if (strcmp(argv[i], "program") == 0) {
			/* [clear | track# ...] */
			arg[0] = 1;

			if ((i+1) < argc) {
				if (strcmp(argv[i+1], "clear") == 0) {
					i++;
					arg[0] = 0;
				}
				else {
					j = 0;
					while ((i+1) < argc &&
					       isdigit(argv[i+1][0]) &&
					       j < (CDA_NARGS-1)) {
						arg[++j] = atoi(argv[++i]);
					}
					if (j > 0)
						arg[0] = (word32_t) -j;
				}
			}
			*cmd = CDA_PROGRAM;
		}
		else if (strcmp(argv[i], "shuffle") == 0) {
			/* <on | off> */
			if (++i < argc) {
				if (strcmp(argv[i], "off") == 0)
					arg[0] = 0;
				else if (strcmp(argv[i], "on") == 0)
					arg[0] = 1;
				else {
					/* Wrong arg */
					usage(argv[0]);
					return FALSE;
				}
			}
			else {
				/* Missing arg */
				usage(argv[0]);
				return FALSE;
			}
			*cmd = CDA_SHUFFLE;
		}
		else if (strcmp(argv[i], "repeat") == 0) {
			/* <on | off> */
			if (++i < argc) {
				if (strcmp(argv[i], "off") == 0)
					arg[0] = 0;
				else if (strcmp(argv[i], "on") == 0)
					arg[0] = 1;
				else {
					/* Wrong arg */
					usage(argv[0]);
					return FALSE;
				}
			}
			else {
				/* Missing arg */
				usage(argv[0]);
				return FALSE;
			}
			*cmd = CDA_REPEAT;
		}
		else if (strcmp(argv[i], "volume") == 0) {
			/* [value#] */
			if ((i+1) >= argc || !isdigit(argv[i+1][0]))
				/* Query */
				arg[0] = 0;
			else {
				/* Set */
				arg[0] = 1;
				arg[1] = (word32_t) atoi(argv[++i]);
			}
			*cmd = CDA_VOLUME;
		}
		else if (strcmp(argv[i], "balance") == 0) {
			/* [value#] */
			if ((i+1) >= argc || !isdigit(argv[i+1][0]))
				/* Query */
				arg[0] = 0;
			else {
				/* Set */
				arg[0] = 1;
				arg[1] = (word32_t) atoi(argv[++i]);
			}
			*cmd = CDA_BALANCE;
		}
		else if (strcmp(argv[i], "route") == 0) {
			/* [value#] */
			if ((i+1) >= argc || !isdigit(argv[i+1][0]))
				/* Query */
				arg[0] = 0;
			else {
				/* Set */
				arg[0] = 1;
				arg[1] = (word32_t) atoi(argv[++i]);
			}
			*cmd = CDA_ROUTE;
		}
		else if (strcmp(argv[i], "status") == 0) {
			/* [cont [secs#]] */
			if ((i+1) >= argc || strcmp(argv[i+1], "cont") != 0)
				stat_cont = FALSE;
			else {
				i++;
				stat_cont = TRUE;
				if ((i+1) < argc && isdigit(argv[i+1][0]))
					cont_delay = atoi(argv[++i]);
			}
			*cmd = CDA_STATUS;
		}
		else if (strcmp(argv[i], "toc") == 0) {
			/* [offsets] */
			if ((i+1) >= argc || strcmp(argv[i+1], "offsets") != 0)
				arg[0] = 0;
			else {
				i++;
				arg[0] = 1;
			}
			*cmd = CDA_TOC;
		}
		else if (strcmp(argv[i], "extinfo") == 0) {
			/* [track#] */
			arg[0] = 0;
			if ((i+1) >= argc || !isdigit(argv[i+1][0]))
				arg[1] = (word32_t) -1;
			else
				arg[1] = atoi(argv[++i]);

			*cmd = CDA_EXTINFO;
		}
		else if (strcmp(argv[i], "device") == 0) {
			*cmd = CDA_DEVICE;
		}
		else if (strcmp(argv[i], "version") == 0) {
			*cmd = CDA_VERSION;
		}
		else if (strcmp(argv[i], "debug") == 0) {
			/* [on | off] */
			if ((i+1) >= argc ||
			    (strcmp(argv[i+1], "on") != 0 &&
			     strcmp(argv[i+1], "off") != 0)) {
				/* Query */
				arg[0] = 0;
			}
			else if (strcmp(argv[i+1], "on") == 0) {
				/* Set debug on */
				arg[0] = 1;
				arg[1] = 1;
				i++;
			}
			else if (strcmp(argv[i+1], "off") == 0) {
				/* Set debug off */
				arg[0] = 1;
				arg[1] = 0;
				i++;
			}
			*cmd = CDA_DEBUG;
		}
		else if (strcmp(argv[i], "visual") == 0) {
#ifdef NOVISUAL
			(void) fprintf(errfp, "%s %s\n",
				       "Cannot start visual mode:",
				       "curses support is not compiled in!");
			return FALSE;
#else
			visual = TRUE;
			*cmd = CDA_STATUS;
			/* Make sure simulator/debug output is redirectable */
			ttyfp = stderr;
#endif
		}
		else {
			usage(argv[0]);
			return FALSE;
		}
	}

	if (*cmd == 0) {
		/* User did not specify a command */
		usage(argv[0]);
		return FALSE;
	}

	return TRUE;
}


/*
 * cda_inexact_select
 *	Ask the user to select from a list of inexact CDDB matches.
 *
 * Args:
 *	matchlist - List of inexact matches
 *
 * Return:
 *	User selection number, or 0 for no selection.
 */
STATIC int
cda_inexact_select(cddb_match_t *matchlist)
{
	int		i,
			n;
	cddb_match_t	*p;
	char		input[64];

	(void) fprintf(ttyfp, "\n%s\n%s\n\n",
		"There were no exact matches in the CD database.",
		"Close matches found:");

	/* Display list */
	for (p = matchlist, i = 1; p != NULL; p = p->next, i++)
		(void) fprintf(ttyfp, "  %2d. %-73s\n", i, p->dtitle);

	(void) fprintf(ttyfp, "  %2d. None of the above\n\n", i);

	for (;;) {
		(void) fprintf(ttyfp, "Choose one (1-%d): ", i);
		if (fgets(input, 63, stdin) == NULL) {
			(void) fprintf(ttyfp, "\n");
			(void) fflush(ttyfp);
			return 0;
		}

		input[strlen(input)-1] = '\0';

		n = atoi(input);
		if (n > 0 && n <= i)
			break;
	}

	(void) fprintf(ttyfp, "\n");
	(void) fflush(ttyfp);
	return ((n == i) ? 0 : n);
}

/*
 * cda_auth
 *	Ask the user to enter user and password for proxy authorization.
 *
 * Args:
 *	retrycnt - How many times has the user retried this
 *
 * Return:
 *	0 for failure
 *	1 for success
 */
STATIC int
cda_auth(int retrycnt)
{
	char	*name,
		*pass,
		input[64];

	if (retrycnt == 0)
		(void) fprintf(ttyfp, "Proxy Authorization is required.\n");
	else {
		(void) fprintf(ttyfp, "Proxy Authorization failed.");

		if (retrycnt < 3)
			(void) fprintf(ttyfp, "  Try again.\n");
		else {
			(void) fprintf(ttyfp, "  Too many tries.\n\n");
			(void) fflush(ttyfp);
			return 0;
		}
	}

	(void) fprintf(ttyfp, "%s\n\n",
		"Please enter your proxy user name and password.");

	/* Get user name */
	(void) fprintf(ttyfp, "Username: ");
	if (fgets(input, 63, stdin) == NULL) {
		(void) fprintf(ttyfp, "\n");
		(void) fflush(ttyfp);
		return 0;
	}
	input[strlen(input)-1] = '\0';
	if (input[0] == '\0')
		return 0;

	name = (char *) MEM_ALLOC("auth_name", strlen(input) + 1);
	if (name == NULL) {
		CDA_FATAL(app_data.str_nomemory);
		return 0;
	}
	(void) strcpy(name, input);
	(void) memset(input, 0, 64);

	/* Get password */
	(void) fprintf(ttyfp, "Password: ");
	cda_echo(ttyfp, FALSE);
	if (fgets(input, 63, stdin) == NULL) {
		cda_echo(ttyfp, TRUE);
		(void) fprintf(ttyfp, "\n");
		(void) fflush(ttyfp);
		return 0;
	}
	cda_echo(ttyfp, TRUE);
	input[strlen(input)-1] = '\0';
	if (input[0] == '\0')
		pass = NULL;
	else {
		pass = (char *) MEM_ALLOC("auth_pass", strlen(input) + 1);
		if (pass == NULL) {
			CDA_FATAL(app_data.str_nomemory);
			return 0;
		}
		(void) strcpy(pass, input);
		(void) memset(input, 0, 64);
	}

	cddb_set_auth(name, pass == NULL ? "" : pass);

	MEM_FREE(name);
	MEM_FREE(pass);

	(void) fprintf(ttyfp, "\n");
	(void) fflush(ttyfp);
	return 1;
}


/***********************
 *   public routines   *
 ***********************/


/*
 * curstat_addr
 *	Return the address of the curstat_t structure.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
curstat_t *
curstat_addr(void)
{
	return (&status);
}


/*
 * cda_quit
 *      Shut down CD audio
 *
 * Args:
 *      s - Pointer to the curstat_t structure.
 *
 * Return:
 *      Nothing.
 */
void
cda_quit(curstat_t *s)
{
	if (isdaemon) {
		/* Shut down CD interface subsystem */
		di_halt(s);

		/* Close FIFOs - daemon side */
		if (cda_sfd[0] >= 0)
			(void) close(cda_sfd[0]);
		if (cda_rfd[0] >= 0)
			(void) close(cda_rfd[0]);

		/* Remove FIFOs */
		if (spipe[0] != '\0')
			(void) UNLINK(spipe);
		if (rpipe[0] != '\0')
			(void) UNLINK(rpipe);

		/* Remove lock */
		if (dlock[0] != '\0')
			(void) UNLINK(dlock);
	}
#ifndef NOVISUAL
	else {
		cda_vtidy();
	}
#endif
}


/*
 * cda_warning_msg
 *	Print warning message.
 *
 * Args:
 *	title - Not used.
 *	msg - The warning message text string.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
void
cda_warning_msg(char *title, char *msg)
{
	(void) fprintf(errfp, "CD audio Warning: %s\n", msg);
}


/*
 * cda_fatal_msg
 *	Print fatal error message.
 *
 * Args:
 *	title - Not used..
 *	msg - The fatal error message text string.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
void
cda_fatal_msg(char *title, char *msg)
{
	(void) fprintf(errfp, "CD audio Fatal Error: %s\n", msg);
	cda_quit(&status);
	exit(6);
}


/*
 * cda_dbclear
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
cda_dbclear(curstat_t *s, bool_t reload)
{
	if (cur_db.discid == 0)
		/* Already cleared */
		return;

	/* Clear database entry structure */
	cddb_clear(&cur_db, s, reload);
}


/*
 * cda_dbload
 *	Read in the CD database file entry pertaining to the
 *	currently loaded disc, if available.
 *
 * Args:
 *	id - The disc identifier to be used for querying
 *	ifunc - Function pointer to a inexact CDDB match handling function.
 *	pfunc - Function pointer to a proxy authorization handling function.
 *	depth - recursion depth
 *
 * Return:
 *	Nothing.
 */
bool_t
cda_dbload(
	word32_t	id,
	int		(*ifunc)(cddb_match_t *),
	int		(*pfunc)(int),
	int		depth
)
{
	int		i,
			n,
			retcode,
			ret;
	word32_t	queryid;
	curstat_t	*s;
	cddb_match_t	*p,
			*q;
	word32_t	arg[CDA_NARGS];

	s = &status;
	(void) memset(arg, 0, CDA_NARGS * sizeof(word32_t));

	if (!cda_sendcmd(CDA_TOC2, arg, &retcode)) {
		(void) printf("%s\n", errmsg);
		return FALSE;
	}

	cur_db.queryid = id;
	s->tot_trks = id & 0xff;

	/* Update curstat with essential info */
	for (i = 0; i < (int) s->tot_trks; i++) {
		RD_ARG_TOC2(arg[i+1], s->trkinfo[i].trkno,
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame
		);

		util_msftoblk(
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame,
			&s->trkinfo[i].addr,
			MSF_OFFSET
		);
	}

	/* Lead-out track */
	RD_ARG_TOC2(arg[i+1], s->trkinfo[i].trkno,
		s->trkinfo[i].min,
		s->trkinfo[i].sec,
		s->trkinfo[i].frame
	);
	util_msftoblk(
		s->trkinfo[i].min,
		s->trkinfo[i].sec,
		s->trkinfo[i].frame,
		&s->trkinfo[i].addr,
		MSF_OFFSET
	);

	/* Load CD database entry */
	if ((ret = cddb_load(&cur_db, s)) != 0) {
		/* Failed to load database entry */
		DBGPRN(errfp, "cddb_load: status=%d arg=%d\n",
			CDDB_GET_STAT(ret), CDDB_GET_ARG(ret));

		switch (CDDB_GET_STAT(ret)) {
		case MATCH_ERR:
			/* Got a list of "inexact matches": Prompt user
			 * for a selection.
			 */
			if (ifunc == NULL ||
			    (n = (*ifunc)(cur_db.matchlist)) == 0) {
				/* Clear match list */
				for (p = q = cur_db.matchlist; p != NULL;
				     p = q) {
					q = p->next;
					MEM_FREE(p->dtitle);
					MEM_FREE(p);
				}
				cur_db.matchlist = NULL;
				return FALSE;
			}

			/* Go find the match */
			for (p = cur_db.matchlist, i=1; i < n; p = p->next, i++)
				;
			queryid = p->discid;

			/* Clear match list */
		        for (p = q = cur_db.matchlist; p != NULL; p = q) {
				q = p->next;
				MEM_FREE(p->dtitle);
				MEM_FREE(p);
			}
			cur_db.matchlist = NULL;

			/* Make daemon remember this ID */
			(void) memset(arg, 0, CDA_NARGS * sizeof(word32_t));
			arg[0] = queryid;
			if (!cda_sendcmd(CDA_SETQUERYID, arg, &retcode)) {
				(void) printf("%s\n", errmsg);
				return FALSE;
			}

			/* Recursion */
			return (cda_dbload(queryid, NULL, pfunc, depth + 1));

		case AUTH_ERR:
			if (pfunc == NULL || (n = (*pfunc)(depth)) == 0)
				return FALSE;

			/* Recursion */
			return (cda_dbload(id, ifunc, pfunc, depth + 1));

		default:
			return FALSE;
		}
	}

	/* Parse play order string and set the play order */
	if (!cda_pgm_parse(s))
		CDA_WARNING(app_data.str_seqfmterr);

	return TRUE;
}


/*
 * cda_sendcmd
 *	Send command down the pipe and handle response.
 *
 * Args:
 *	cmd - The command code
 *	arg - Command arguments
 *	retcode - Return code
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
cda_sendcmd(word32_t cmd, word32_t arg[], int *retcode)
{
	int		i;
	cdapkt_t	p,
			r;

	/* Fill in command packet */
	(void) memset(&p, 0, sizeof(cdapkt_t));
	p.pktid = getpid();
	p.cmd = cmd;
	(void) memcpy(p.arg, arg, CDA_NARGS * sizeof(word32_t));

	/* Send command packet */
	if (!cda_sendpkt("CD audio", cda_sfd[1], &p)) {
		errmsg = "\nCannot send packet to CD audio daemon.";
		*retcode = -1;
		return FALSE;
	}

	for (i = 0; ; i++) {
		/* Get response packet */
		if (!cda_getpkt("CD audio", cda_rfd[1], &r)) {
			errmsg = "Cannot get packet from CD audio daemon.";
			*retcode = -1;
			return FALSE;
		}

		/* Sanity check */
		if (p.pktid == r.pktid)
			break;

		/* This is not our packet */

		if (i >= MAX_PKT_RETRIES) {
			errmsg =
			"Retry overflow reading packet from CD audio daemon.";
			*retcode = -1;
			return FALSE;
		}

		/* Increment packet reject count */
		r.rejcnt++;

		/* If packet has not reached reject limit, put it back
		 * into the pipe.
		 */
		if (r.rejcnt < MAX_PKT_RETRIES &&
		    !cda_sendpkt("CD audio", cda_rfd[0], &r)) {
			errmsg = "Cannot send packet to CD audio daemon.";
			*retcode = -1;
			return FALSE;
		}

		/* Introduce some randomness to shuffle
		 * the order of packets in the pipe
		 */
		if ((rand() & 0x80) != 0)
			util_delayms(100);
	}

	/* Return args */
	(void) memcpy(arg, r.arg, CDA_NARGS * sizeof(word32_t));

	*retcode = r.retcode;

	/* Check return code */
	switch (r.retcode) {
	case CDA_OK:
		return TRUE;
	case CDA_INVALID:
		errmsg = "This command is not valid in the current state.";
		return FALSE;
	case CDA_PARMERR:
		errmsg = "Command argument error.";
		return FALSE;
	case CDA_FAILED:
		errmsg = "The CD audio daemon does not support this command.";
		return FALSE;
	default:
		errmsg = "The CD audio daemon returned an invalid status.";
		return FALSE;
	}
	/*NOTREACHED*/
}


/*
 * cda_daemon_alive
 *	Check if the cda daemon is running.
 *
 * Args:
 *	None.
 *
 * Return:
 *	TRUE - daemon is alive
 *	FALSE - daemon is dead
 */
bool_t
cda_daemon_alive(void)
{
	int		fd;
	pid_t		pid;
	char		dlock[FILE_PATH_SZ],
			buf[12];

	(void) sprintf(dlock, "%s/cdad.%x", TEMP_DIR, (int) cd_rdev);

	if ((fd = open(dlock, O_RDONLY)) < 0)
		return FALSE;

	if (read(fd, buf, 12) < 0) {
		(void) close(fd);
		return FALSE;
	}
	(void) close(fd);

	pid = (pid_t) atoi(buf);
	
	if (kill(pid, 0) < 0)
		return FALSE;

	daemon_pid = pid;

	return TRUE;
}


/*
 * cda_daemon
 *	CD audio daemon main loop function.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	The CD audio daemon exit status.
 */
int
cda_daemon(curstat_t *s)
{
	int		i;
	cdapkt_t	p;
	char		*errmsg;
	struct stat	stbuf;
	bool_t		done = FALSE;

#ifndef NOMKTMPDIR
	errmsg = (char *) MEM_ALLOC(
		"errmsg",
		strlen(app_data.str_tmpdirerr) + strlen(TEMP_DIR)
	);
	if (errmsg == NULL) {
		CDA_FATAL(app_data.str_nomemory);
		return 1;
	}

	/* Make temporary directory, if needed */
	(void) sprintf(errmsg, app_data.str_tmpdirerr, TEMP_DIR);
	if (LSTAT(TEMP_DIR, &stbuf) < 0) {
		if (!util_mkdir(TEMP_DIR, 0777)) {
			(void) fprintf(errfp, "%s\n", errmsg);
			return 1;
		}
	}
	else if (!S_ISDIR(stbuf.st_mode)) {
		(void) fprintf(errfp, "%s\n", errmsg);
		return 1;
	}

	MEM_FREE(errmsg);
#endif	/* NOMKTMPDIR */

	(void) signal(SIGCHLD, SIG_IGN);

	/* Become a daemon process */
	switch (FORK()) {
	case -1:
		perror("Cannot fork CD audio daemon");
		cda_quit(s);
		return 1;
	case 0:
		/* Child process */
		if (ttyfp != stderr) {
			errfp = ttyfp;
			(void) fclose(stdin);
			(void) fclose(stdout);
			(void) fclose(stderr);
		}

		break;
	default:
		/* Parent process */
		/* Make sure the daemon is running */
		for (i = 0; i < 5; i++) {
			(void) sleep(1);
			if (cda_daemon_alive())
				break;
		}
		if (i >= 5) {
			CDA_FATAL("CD audio daemon not started.");
			return 1;
		}

		return 0;
	}

	/* Check for duplicate daemon processes */
	if (!cda_daemonlock()) {
		CDA_FATAL("CD audio daemon already running.");
		return 1;
	}

	/* Set daemon flag */
	isdaemon = TRUE;

	/* Create FIFOs */
	if (MKFIFO(spipe, 0600) < 0) {
		if (errno != EEXIST) {
			perror("CD audio: Cannot create send pipe");
			cda_quit(s);
			return 1;
		}

		/* Try removing and re-creating the FIFO */
		if (UNLINK(spipe) < 0) {
			perror("CD audio: Cannot unlink old send pipe");
			cda_quit(s);
			return 1;
		}
		if (MKFIFO(spipe, 0600) < 0) {
			perror("CD audio: Cannot create send pipe");
			cda_quit(s);
			return 1;
		}
	}
	if (MKFIFO(rpipe, 0600) < 0) {
		if (errno != EEXIST) {
			perror("CD audio: Cannot create recv pipe");
			cda_quit(s);
			return 1;
		}

		/* Try removing and re-creating the FIFO */
		if (UNLINK(rpipe) < 0) {
			perror("CD audio: Cannot unlink old recv pipe");
			cda_quit(s);
			return 1;
		}
		if (MKFIFO(rpipe, 0600) < 0) {
			perror("CD audio: Cannot create recv pipe");
			cda_quit(s);
			return 1;
		}
	}

	/* Initialize and start drive interfaces */
	cda_init(s);
	cda_start(s);

	/* Handle some signals */
	if (signal(SIGHUP, onsig0) == SIG_IGN)
		(void) signal(SIGHUP, SIG_IGN);
	(void) signal(SIGTERM, SIG_IGN);
	(void) signal(SIGINT, SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
#if defined(SIGTSTP) && defined(SIGCONT)
	(void) signal(SIGTSTP, SIG_IGN);
	(void) signal(SIGCONT, SIG_IGN);
#endif

	/* Main command handling loop */
	while (!done) {
		/* Get command packet */
		if (!cda_getpkt("CD audio daemon", cda_sfd[0], &p)) {
			cda_quit(s);
			return 1;
		}

		/* Interpret and carry out command */
		done = cda_docmd(s, &p);

		/* Send response packet */
		if (!cda_sendpkt("CD audio daemon", cda_rfd[0], &p)) {
			cda_quit(s);
			return 1;
		}
	}

	/* Stop the drive */
	cda_quit(s);

	exit(0);

	/*NOTREACHED*/
}


/*
 * main
 *	The main function
 */
int
main(int argc, char **argv)
{
	int		i,
			retcode,
			ret;
	word32_t	cmd,
			queryid,
			*arg1,
			*arg2;
	struct stat	stbuf;
	char		*ttypath,
			*cp,
			*hd,
			str[FILE_PATH_SZ * 2];
	bool_t		loaddb = TRUE;


	/* Hack: Aside from stdin, stdout, stderr, there shouldn't
	 * be any other files open, so force-close them.  This is
	 * necessary in case xmcd was inheriting any open file
	 * descriptors from a parent process which is for the
	 * CD-ROM device, and violating exclusive-open requirements
	 * on some platforms.
	 */
	for (i = 3; i < 10; i++)
		(void) close(i);

	/* Initialize */
	if ((ttypath = ttyname(2)) == NULL)
		ttypath = "/dev/tty";
	if ((ttyfp = fopen(ttypath, "w")) != NULL)
		setbuf(ttyfp, NULL);
	else
		ttyfp = stderr;

	/* Initialize error messages */
	app_data.str_nomethod = STR_NOMETHOD;
	app_data.str_novu = STR_NOVU;
	app_data.str_nomemory = STR_NOMEMORY;
	app_data.str_nocfg = STR_NOCFG;
	app_data.str_notrom = STR_NOTROM;
	app_data.str_notscsi2 = STR_NOTSCSI2;
	app_data.str_moderr = STR_MODERR;
	app_data.str_staterr = STR_STATERR;
	app_data.str_noderr = STR_NODERR;
	app_data.str_recoverr = STR_RECOVERR;
	app_data.str_maxerr = STR_MAXERR;
	app_data.str_tmpdirerr = STR_TMPDIRERR;
	app_data.str_seqfmterr = STR_SEQFMTERR;
	app_data.str_longpatherr = STR_LONGPATHERR;
	app_data.str_devlist_undef = STR_DEVLIST_UNDEF;
	app_data.str_devlist_count = STR_DEVLIST_COUNT;
	app_data.str_medchg_noinit = STR_MEDCHG_NOINIT;
	app_data.str_proxyerr = STR_PROXYERR;

	/* Initialize other app_data parameters */
	app_data.cddb_mailcmd = "";
	app_data.cddb_mailsite = "";
	app_data.cddb_port = CDDB_PORT;
	app_data.cddb_timeout = CDDB_TIMEOUT;
	app_data.cddb_rmtautosave = FALSE;
	app_data.stat_interval = 260;
	app_data.ins_interval = 4000;
	app_data.startup_vol = -1;

	/* Allocate arg arrays */
	arg1 = (word32_t *) MEM_ALLOC("arg1", CDA_NARGS * sizeof(word32_t));
	arg2 = (word32_t *) MEM_ALLOC("arg2", CDA_NARGS * sizeof(word32_t));
	if (arg1 == NULL || arg2 == NULL)
		CDA_FATAL(app_data.str_nomemory);

	/* Parse command line args */
	if (!cda_parse_args(argc, argv, &cmd, arg1))
		exit(1);

	/* Seed random number generator */
	srand((unsigned) time(NULL));

	/* Debug mode start-up banner */
	DBGPRN(errfp, "CDA v%s%s PL%d DEBUG MODE\n\n",
		VERSION, VERSION_EXT, PATCHLEVEL);

	/* Initialize libutil */
	util_init();

	/* Set library directory path */
	if ((cp = (char *) getenv("XMCD_LIBDIR")) == NULL)
		CDA_FATAL("XMCD_LIBDIR environment not defined.");

	app_data.libdir = (char *) MEM_ALLOC("app_data.libdir", strlen(cp) + 1);
	if (app_data.libdir == NULL)
		CDA_FATAL(app_data.str_nomemory);
	(void) strcpy(app_data.libdir, cp);

	/* Paranoia: avoid overflowing buffers */
	if ((int) strlen(app_data.libdir) >= FILE_PATH_SZ)
		CDA_FATAL(app_data.str_longpatherr);

	if (app_data.device != NULL) {
		if ((int) strlen(app_data.device) >= FILE_PATH_SZ ||
		    (int) strlen(util_basename(app_data.device)) >=
		    FILE_BASE_SZ) {
			CDA_FATAL(app_data.str_longpatherr);
		}
	}

	hd = util_homedir(util_get_ouid());
	if ((int) strlen(hd) >= FILE_PATH_SZ)
		CDA_FATAL(app_data.str_longpatherr);

	/* Get system common configuration parameters */
	(void) sprintf(str, SYS_CMCFG_PATH, app_data.libdir);
	di_common_parmload(str, TRUE);

	/* Get user common configuration parameters */
	(void) sprintf(str, USR_CMCFG_PATH, hd);
	di_common_parmload(str, FALSE);

	/* Check validity of device */
	if (di_isdemo())
		cd_rdev = 0;
	else {
		if (stat(app_data.device, &stbuf) < 0) {
			(void) sprintf(emsg, "Cannot stat %s.",
				       app_data.device);
			CDA_FATAL(emsg);
		}
		cd_rdev = stbuf.st_rdev;
	}

	/* FIFO paths */
	(void) sprintf(spipe, "%s/send.%x", TEMP_DIR, (int) cd_rdev);
	(void) sprintf(rpipe, "%s/recv.%x", TEMP_DIR, (int) cd_rdev);

	/* Initialize CD database services */
	(void) strcpy(cddb_cldata.prog, PROGNAME);
	(void) strcpy(cddb_cldata.user, util_loginname());
	cddb_cldata.isdemo = di_isdemo;
	cddb_cldata.curstat_addr = curstat_addr;
	cddb_cldata.fatal_msg = cda_fatal_msg;
	cddb_cldata.warning_msg = cda_warning_msg;
	cddb_init(&cddb_cldata);

#ifndef NOVISUAL
	if (visual) {
		/* Handle some signals */
		if (signal(SIGINT, onsig0) == SIG_IGN)
			(void) signal(SIGINT, SIG_IGN);
		if (signal(SIGQUIT, onsig0) == SIG_IGN)
			(void) signal(SIGQUIT, SIG_IGN);
		if (signal(SIGTERM, onsig0) == SIG_IGN)
			(void) signal(SIGTERM, SIG_IGN);
		(void) signal(SIGPIPE, onsig1);

		/* Start visual mode */
		cda_visual();
	}
#endif

	if (cmd == CDA_ON) {
		/* Start CDA daemon */
		(void) printf("Initializing...\n");
		if ((ret = cda_daemon(&status)))
			exit(ret);
	}

	/* Open FIFOs - command side */
	if ((cda_sfd[1] = open(spipe, O_WRONLY)) < 0) {
		perror("CD audio: cannot open send pipe");
		CDA_FATAL(
			"Run \"cda on\" first to initialize CD audio daemon."
		);
	}
	if ((cda_rfd[1] = open(rpipe, O_RDONLY)) < 0) {
		perror("CD audio: cannot open recv pipe for reading");
		CDA_FATAL(
			"Run \"cda on\" first to initialize CD audio daemon."
		);
	}
	if ((cda_rfd[0] = open(rpipe, O_WRONLY)) < 0) {
		perror("CD audio: cannot open recv pipe for writing");
		CDA_FATAL(
			"Run \"cda on\" first to initialize CD audio daemon."
		);
	}

	/* Handle some signals */
	(void) signal(SIGINT, onintr);
	(void) signal(SIGQUIT, onintr);
	(void) signal(SIGTERM, onintr);

	for (;;) {
		/* Send the command */
		switch (cmd) {
		case CDA_ON:
			/* Send command to cda daemon */
			if (!cda_sendcmd(cmd, arg1, &retcode)) {
				(void) printf("%s\n", errmsg);
				exit(2);
			}

			daemon_pid = (pid_t) arg1[0];

			(void) fprintf(errfp,
				    "CD audio daemon pid=%d dev=%s started.\n",
				    (int) daemon_pid, app_data.device);
			break;

		case CDA_OFF:
			/* Send command to cda daemon */
			if (!cda_sendcmd(cmd, arg1, &retcode)) {
				(void) printf("%s\n", errmsg);
				exit(2);
			}

			daemon_pid = (pid_t) arg1[0];

			(void) fprintf(errfp,
				    "CD audio daemon pid=%d dev=%s exited.\n",
				    (int) daemon_pid, app_data.device);
			break;

		case CDA_STATUS:
			/* Send command to cda daemon */
			if (!cda_sendcmd(cmd, arg1, &retcode)) {
				(void) printf("%s\n", errmsg);
				exit(2);
			}

			if (RD_ARG_MODE(arg1[0]) == MOD_NODISC)
				loaddb = TRUE;

			cur_db.discid = arg1[6];

			/* Load CD database entry */
			if (loaddb &&
			    RD_ARG_MODE(arg1[0]) != MOD_NODISC &&
			    cur_db.discid != 0) {
				loaddb = FALSE;
				queryid = (arg1[8] != 0) ? arg1[8] : arg1[6];
				cddb = cda_dbload(queryid, NULL, NULL, 0);
			}

			/* Print status */
			prn_stat(arg1);
			break;

		default:	/* All other commands */
			/* Check current status */
			if (!cda_sendcmd(CDA_STATUS, arg2, &retcode)) {
				(void) printf("%s\n", errmsg);
				exit(2);
			}

			if (RD_ARG_MODE(arg2[0]) == MOD_NODISC)
				loaddb = TRUE;

			cur_db.discid = arg2[6];

			/* Load CD database entry */
			if (loaddb && RD_ARG_MODE(arg2[0]) != MOD_NODISC &&
			    cur_db.discid != 0) {
				loaddb = FALSE;
				queryid = (arg2[8] != 0) ? arg2[8] : arg2[6];
				if (cmd == CDA_TOC && !batch)
					cddb = cda_dbload(
						queryid,
						cda_inexact_select,
						cda_auth,
						0
					);
				else
					cddb = cda_dbload(queryid, NULL,
							  NULL, 0);
			}

			/* Send command to cda daemon */
			if (!cda_sendcmd(cmd, arg1, &retcode)) {
				(void) printf("%s\n", errmsg);
				exit(2);
			}

			/* Perform post-command tasks */
			switch (cmd) {
			case CDA_PROGRAM:
				prn_program(arg1);
				break;
			case CDA_VOLUME:
				prn_vol(arg1);
				break;
			case CDA_BALANCE:
				prn_bal(arg1);
				break;
			case CDA_ROUTE:
				prn_route(arg1);
				break;
			case CDA_TOC:
				prn_toc(arg1);
				break;
			case CDA_EXTINFO:
				prn_extinfo(arg1);
				break;
			case CDA_DEVICE:
				prn_device(arg1);
				break;
			case CDA_VERSION:
				prn_ver(arg1);
				break;
			case CDA_DEBUG:
				prn_debug(arg1);
				break;
			default:
				/* Unknown command */
				break;
			}

			break;
		}

		(void) fflush(stdout);

		if (!stat_cont)
			break;

		if (cont_delay > 0)
			(void) sleep(cont_delay);
	}

	/* Close FIFOs - command side */
	if (cda_sfd[1] >= 0)
		(void) close(cda_sfd[1]);
	if (cda_rfd[1] >= 0)
		(void) close(cda_rfd[1]);
	if (cda_rfd[0] >= 0)
		(void) close(cda_rfd[0]);

	exit(0);

	/*NOTREACHED*/
}


