/* pty_unicos.c - routines to allocate ptys - for CRAY UNICOS 5.1 and 6.0 */

/*

Original by: Don Libes, NIST, 2/6/90
Hacked for Unicos 5.1 by: Frank Terhaar-Yonkers, US EPA,  1/10/91
Hacked for Unicos 6.0 by: Pete TerMaat, pete@willow.cray.com, 3/27/91

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.

*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/fcntl.h>
#if CRAY>=60
#include <sys/termios.h>
#else
#include <sys/termio.h>
#endif /* 60 */
#if CRAY>=70 && defined(_CRAY2)
#include <sys/session.h>
#endif /* 70 */
#include <sys/pty.h>
#include <pwd.h>
#include <utmp.h>
#include <signal.h>
#include "translate.h"

void debuglog();
#if CRAY<60
extern int fork(), execl(), wait();
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static char	linep[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
static char	linet[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
static int	lowpty;
static int	highpty;
static int	realuid;
static int	realgid;

static void
pty_stty(s,name)
char *s;		/* args to stty */
char *name;		/* name of pty */
{
#define MAX_ARGLIST 10240
	char buf[MAX_ARGLIST];	/* overkill is easier */

	sprintf(buf,"stty %s < %s > %s",s,name,name);
	system(buf);
}

struct	termio exp_tty_original;

int dev_tty;		/* file descriptor to /dev/tty or -1 if none */
int knew_dev_tty;	/* true if we had our hands on /dev/tty at any time */

#define GET_TTYTYPE	0
#define SET_TTYTYPE	1
static void
ttytype(request,fd,s)
int request;
int fd;
char *s;	/* stty args, used only if request == SET_TTYTYPE */
{
	if (request == GET_TTYTYPE) {
		if (-1 == ioctl(fd, TCGETA, (char *)&exp_tty_original)) {
			knew_dev_tty = FALSE;
			dev_tty = -1;
		}
	} else {	/* type == SET_TTYTYPE */
		if (knew_dev_tty) {
			(void) ioctl(fd, TCSETA, (char *)&exp_tty_original);
		} else {
			/* if running in the background, we have no access */
			/* to a a tty to copy parameters from, so use ones */
			/* supplied by original Makefile */
			debuglog("getptyslave: (default) stty %s\n",DFLT_STTY);
			pty_stty(DFLT_STTY,linet);
		}
		if (s) {
			/* give user a chance to override any terminal parms */
			debuglog("getptyslave: (user-requested) stty %s\n",s);
			pty_stty(s,linet);
		}
	}
}

void
init_pty()
{
	lowpty=0;
#ifdef _SC_CRAY_NPTY
	highpty=sysconf(_SC_CRAY_NPTY);
#else
	highpty=128;
#endif /* _SC_CRAY_NPTY */

 	realuid=getuid();	/* get REAL uid */
 	realgid=getgid();	/* get REAL uid */
 	setgid(0);		/* Set REAL gid */
 	setuid(0);		/* Set REAL uid to root also */
 	setegid(realgid); 	/* Don't run as root! */
 	seteuid(realuid); 	/* Don't run as root! */

	dev_tty = open("/dev/tty",O_RDWR);
	knew_dev_tty = (dev_tty != -1);
	if (knew_dev_tty) ttytype(GET_TTYTYPE,dev_tty,(char *)0);
}

/* returns fd of master end of pseudotty */
int
getptymaster()
{
	struct stat sb;
	int master;
	int npty;

	for (npty = lowpty; npty <= highpty; npty++) {
		seteuid(0); /* we need to be root! */
		(void) sprintf(linep, "/dev/pty/%03d", npty);
		master = open(linep, O_RDWR);

		if (master < 0)
			continue;

		(void) sprintf(linet, "/dev/ttyp%03d", npty);
		if(stat(linet, &sb) < 0) {
			(void) close(master);
			continue;
		}
		if(sb.st_uid || sb.st_gid || sb.st_mode != 0600) {
                        chown(linet, realuid, realgid);
                        chmod(linet, 0600);
                        (void)close(master);
                        master = open(linep, 2);
                        if (master < 0)
                                continue;
                }
		setegid(realgid);
		seteuid(realuid); /* back to who we are! */
		if (access(linet, R_OK|W_OK) != 0) {
			(void) close(master);
			continue;
		}
		return(master);
	}
	return(-1);
}

int
getptyslave(stty_args)
char *stty_args;
{
	int slave;

	if (0 > (slave = open(linet, O_RDWR))) return(-1);

	/* sanity check - if slave not 0, skip rest of this and return */
	/* to what will later be detected as an error in caller */
	if (0 != slave) return(slave);

	fcntl(0,F_DUPFD,1);	/* duplicate 0 onto 1 to prepare for stty */
	ttytype(SET_TTYTYPE,slave,stty_args);
	return(slave);
}
setptyutmp()
{
	struct utmp utmp;
	struct passwd *pw;

	seteuid(0);
	(void) setutent ();
	/* set up entry to search for */
	(void) strncpy(utmp.ut_id, linet + strlen(linet) - 4,
		 sizeof (utmp.ut_id));
	utmp.ut_type = DEAD_PROCESS;

	/* position to entry in utmp file */
	if( 0 == getutid(&utmp)) {
		seteuid(realuid);
		return(-1);	/* no utmp entry for this line ??? */
	}

	/* set up the new entry */
	pw = getpwuid(realuid);
	utmp.ut_type = USER_PROCESS;
	utmp.ut_exit.e_exit = 2;
	(void) strncpy(utmp.ut_user,
	    (pw && pw->pw_name) ? pw->pw_name : "????",
	    sizeof(utmp.ut_user));

	(void) strncpy(utmp.ut_id, linet + strlen(linet) - 4,
		sizeof(utmp.ut_id)); 

	gethostname(utmp.ut_host, sizeof(utmp.ut_host));
	(void) strncpy (utmp.ut_line,
		linet + strlen("/dev/"), sizeof (utmp.ut_line));
	utmp.ut_pid = getpid();
	utmp.ut_time = time ((long *) 0);

	/* write out the entry */
	pututline(&utmp);

	/* close the file */
	(void) endutent();
	seteuid(realuid);
	return(0);
}

int
system(s)
char	*s;
{
	int	status, pid, w;
	register void (*istat)(), (*qstat)();

	if((pid = fork()) == 0) {
		fixids();
		(void) execl("/bin/sh", "sh", "-c", s, 0);
		_exit(127);
	}
	if (pid == -1)
		return(-1);
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while((w = wait(&status)) != pid && w != -1)
		;
	(void) signal(SIGINT, istat);
	(void) signal(SIGQUIT, qstat);
	return((w == -1)? w: status);
}

fixids()
{
	seteuid(0);
	setgid(realgid);
	setuid(realuid);
}
