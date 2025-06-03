/*	xcport.c -- modem interface routines for XC
	This file uses 4-character tabstops
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <termio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "xc.h"

/* define this if you need to send SIGUSR1/SIGUSR2 to
   handle an active getty process, or use ungetty.
*/
/*#define GETTY_HANDLER /**/

# if DIDO == 2			/* SCO Xenix 2.2 uses ungetty */
#	define UNGETTY "/usr/lib/uucp/ungetty"
#	define UG_NOTENAB	0
#	define UG_ENAB		1
#	define UG_FAIL		2
#	define LOCKDIR "/usr/spool/uucp"
#	define SIZEOFLOCKFILE sizeof(short)
	static	int	code, retcode, errflag;
#	ifndef GETTY_HANDLER
#		define GETTY_HANDLER
#	endif
# endif /*DIDO==2*/

# if DIDO == 3			/* SCO Xenix 2.3, SCO Unix */
#	include <utmp.h>
#	define LOCKDIR "/usr/spool/uucp"
#	ifndef ASCII_PID
#	 define ASCII_PID
#	 define PIDSIZE 10
#	endif
	static pid_t gettypid = -1;
# endif /*DIDO==3*/

# if DIDO == 4			/* System V Release 4 */
#	include <utmp.h>
#	include <sys/stat.h>
#	include <sys/mkdev.h>
#	define LOCKDIR "/var/spool/locks"
#	ifndef ASCII_PID
#	 define ASCII_PID
#	 define PIDSIZE 10
#	endif
	static pid_t gettypid = -1;
# endif /*DIDO==4*/

#ifndef SIZEOFLOCKFILE
#define SIZEOFLOCKFILE sizeof(int)
#endif

static pid_t pid;
int cbaud = B2400;			/* default bps */
short flowflag;				/* modem port i/o data mask */
static int mfd = -1;		/* modem port file descriptor */
static struct termio pmode;	/* modem device control string */
static char port[SM_BUFF],	/* modem port device file string */
		lckname[SM_BUFF];	/* lockfile string */
unsigned mrate ();
char protocol[] = "8N1";	/* default modem protocol */
extern int errno;
struct stat stat_buf;

struct {
	char *proto;
	int  clear;
	int  set;
} prot_tbl[] = {
	"8N1",		~(CSIZE | PARENB | CSTOPB),	CS8,
	"7E2",		~(CSIZE | PARODD),			CS7 | PARENB | CSTOPB,
	"7O2",		~CSIZE,						CS7 | PARENB | PARODD | CSTOPB,
	NIL(char),	0,							0
};

struct {
	char	 *bps;
	unsigned rate;
	int		 cbaud;
} bps_tbl[] = {
	"300",	300,	B300,
	"600",	600,	B600,
	"1200",	1200,	B1200,
	"2400",	2400,	B2400,
	"4800",	4800,	B4800,
	"9600",	9600,	B9600,
#ifdef B19200
	"19200",19200,	B19200,
#endif
#ifdef B38400
	"38400",38400,	B38400,
	"57600",57600,	B50,
#endif
	"0",	0,		B0
};

void
xc_setflow(flow)
short flow;
{
	if (flow)
		pmode.c_iflag |= IXON | IXOFF | IXANY;
	else
		pmode.c_iflag &= ~(IXON | IXOFF | IXANY);

	ioctl(mfd, TCSETAF, &pmode);
}

/* get/set character size and parity on the port */
char *
xc_setproto(p)
char *p;
{
	register i;

	if (!p)
		return protocol;

	for (i=0; prot_tbl[i].proto; i++){
		if (!strcmp(p,prot_tbl[i].proto)){
			pmode.c_cflag &= prot_tbl[i].clear;
			pmode.c_cflag |= prot_tbl[i].set;
			ioctl(mfd, TCSETAF, &pmode);
			strcpy(protocol,p);
			return protocol;
		}
	}
	return NIL(char);
}

/* get/set port string */
char *
mport(s)
char *s;
{
	if (s && mfd == -1)
		if (strncmp("/dev/", s, 5))
			strcpy(port, "/dev/"),
			strcat(port, s);
		else
			strcpy(port, s);

	return(port);
}

/*	Get/set the bps of the modem port; set the terminal rate to correspond. */
unsigned
mrate(s)
char *s;
{
	register i;

	if (s){
		for (i=0; bps_tbl[i].cbaud; i++){
			if (!strcmp(s,bps_tbl[i].bps)){
				cbaud = bps_tbl[i].cbaud;
				pmode.c_cflag &= ~CBAUD;
				pmode.c_cflag |= cbaud;
				ioctl(mfd, TCSETAF, &pmode);
				return (bps_tbl[i].rate);
			}
		}
		return FAILURE;
	}

	for (i=0; bps_tbl[i].cbaud; i++)
		if ((pmode.c_cflag & CBAUD) == bps_tbl[i].cbaud)
			return (bps_tbl[i].rate);
		
	return FAILURE;
}

/*	The following routine is used to hang up the modem. This is accomplished
	by setting bps to 0. According to my documentation on termio, setting bps
	to zero will result in DTR not being asserted. This hangs up some (most?)
	modems. If not, the second part of the routine sends the Hayes modem
	"escape" and then a hangup command.
*/
hangup()
{
	S1("<< HANGUP >>");

#if DTR_DROPS_CARRIER
	pmode.c_cflag &= ~CBAUD;
	pmode.c_cflag |= B0;		/* set cbaud 0 (drop DTR) */
	ioctl(mfd, TCSETAF, &pmode);

	sleep(1);					/* wait a second */

	pmode.c_cflag &= ~CBAUD;	/* reset bps */
	pmode.c_cflag |= cbaud;
	ioctl(mfd, TCSETAF, &pmode);
#else /* use Hayes command */
	sleep(2);					/* Allow for "escape guard time" */
	send_string(ATTEN);			/* Send modem escape command */
	sleep(3);					/* More "escape guard time" */
	send_string(HANGUP);		/* Send hangup command */
#endif
	return SUCCESS;
}

#ifdef GETTY_HANDLER
# if DIDO >= 2
/*	suspend() sends signal to a running getty
			 sets:	gettypid, process number of running getty, if DIDO > 2
					retcode, exit value of 'ungetty', if DIDO = 2
	restart(): restarts getty if it had been running before
*/
#  if DIDO >= 3
static void
suspend()
{
	struct	utmp *t, *getutent();
	char buf[12];
	void endutent();

	strcpy(buf, strrchr(port, '/') +1);
	while ((t = getutent())){
		if (t->ut_type == LOGIN_PROCESS && (!strcmp(buf, t->ut_line))){
			gettypid = t->ut_pid;	/* get getty PID */
			if (kill(gettypid, SIGUSR1) == -1 && errno != EPERM)
				S1("Can't signal getty");
		}
	}
	endutent();
}

static void
restart()
{
	if (gettypid != -1)
		kill(gettypid, SIGUSR2);
}
#  endif /*DIDO>=3*/
#  if DIDO == 2
static void
suspend()
{
	code=errflag=pid=retcode=0;
	if ((pid = fork()) == 0){
		execl(UNGETTY, "ungetty", port, NIL(char));
		S1("ungetty exec error");
		exit(8);
		}
	while (((code = wait(&errflag)) != pid) && code != -1);
	switch ((errflag>>8) & 0xff){
	case UG_NOTENAB:	/* line acquired: not enabled */
		retcode = UG_NOTENAB;
		break;
	case UG_ENAB:	/* line acquired: need ungetty -r when done */
		retcode = UG_ENAB;
		break;
	case UG_FAIL:		/* could not acquire line */
	case 255:
		exit(8);
	}
}

static void
restart()
{
	code=errflag=pid=0;
	if(retcode == UG_ENAB){
		if ((pid = fork()) == 0){
			execl(UNGETTY, "ungetty", "-r", port, NIL(char));
			exit(8);
		}
	while (((code = wait(&errflag)) != pid) && code != -1)
		;
	}
}
#  endif /*DIDO==2*/
# endif /*DIDO>=2*/
#endif /*GETTY_HANDLER*/

/*	Attach standard input and output to the modem port. This only gets called
	after a fork by the child process, which then exec's a program that uses
	standard i/o for some data transfer protocol. (To put this here is actually
	a kludge, but I wanted to keep the modem-specific stuff in a black box.)
*/
void
mattach()
{
	dup2(mfd, 0);	/* close local stdin and connect to port */
	dup2(mfd, 1);	/* close local stdout and connect to port */

	close(mfd);		/* close the old port descriptor */
}

static void
alrm(junk)
int junk;
{ /* do nothing */
}

/*	Get a byte from the modem port within 'seconds' or return -1.
	All data read from the modem are input through this routine.
*/
readbyte(seconds)
unsigned seconds;
{
	static int count = 0;
	static char *p, rxbuf[SM_BUFF];
	unsigned alarm();

	if (count > 0){
		count--;
		return(*p++ & 0xff);
	}
	if (seconds){
		signal(SIGALRM, alrm);
		alarm(seconds);
	}
	if ((count = read(mfd, p = rxbuf, SM_BUFF)) < 1)
		return(-1);
	if (seconds)
		alarm(0);

	count--;
	return(*p++ & 0xff);
}

/*	Output a byte to the modem port.
	All data sent to the modem are output through this routine.
*/
void
sendbyte(ch)
int ch;
{
	char c = ch & 0xff;

	if(write(mfd, &c, 1)<0)
		S1("sendbyte: write error!");
}

void
send_string(s)
char *s;
{
	while (*s){
		sendbyte(*s++);
		/* msecs(35); 		/* season to taste ... */
	}
}

/* send a modem break */
xmitbrk()
{
	S1("<< BREAK >>");
	ioctl(mfd, TCSBRK, 0);
	return SUCCESS;
}

/*	lock_tty() returns FAILURE if the lock file exists (and XC will not run).

	unlock_tty() deletes the lock file.

	SCOXENIX 2.3 mods: Steve Manes
	Check for active LCK file and try to delete it

	SCOXENIX 2.2 mods: Jean-Pierre Radley
	As above, using 'ungetty'

	Tandy 6000 mods: Fred Buck
  	SVR4 mods: Larry Rosenman
*/

static
lock_tty()
{
#if DIDO >= 2
	int lckfd;
	char *s, buf[12];
#ifdef ASCII_PID
	static char apid[PIDSIZE+2] = { '\0' };
#else
	pid = -1;
#endif

	strcpy(buf, strrchr(port, '/') +1);
	s = buf + strlen(buf) - 1;

#if DIDO == 2
	*s = toupper(*s);
#endif
#if DIDO >= 3
	*s = tolower(*s);
#endif

#if DIDO == 4
	if(stat(port,&stat_buf)==0){
		sprintf(lckname,"%s/LK.%03d.%03d.%03d",LOCKDIR,
			major(stat_buf.st_dev),
			major(stat_buf.st_rdev),
			minor(stat_buf.st_rdev));
	}
#else
	sprintf(lckname, "%s/LCK..%s", LOCKDIR, buf);
#endif  /*DIDO==4*/

	if (!checkLCK())	/* check LCK file */
		return FAILURE;	/* can't unlock it */

	if ((lckfd = creat(lckname, 0666)) < 0){
		sprintf(Msg,"Can't create '%s'", lckname);
		S;
		return FAILURE;
	}

#ifdef ASCII_PID
	sprintf(apid, "%*d\n", PIDSIZE, getpid());
	write(lckfd, apid, PIDSIZE+1);
#else
	pid = getpid();
	write(lckfd, (char *)&pid, SIZEOFLOCKFILE);
#endif

	close(lckfd);
#endif /*DIDO*/
	return SUCCESS;
}

void
unlock_tty()
{
	static char byettyxx[50], *byeptr;
	extern char *ttyname();

	sprintf(byettyxx,"BYE%s", strrchr(ttyname(mfd),'/')+1);
	byeptr = getenv(byettyxx);
	if (byeptr && *byeptr)
		S1("Sending BYE string to modem"),
		send_string("\r"),
		send_string(byeptr),
		send_string("\r");

	pmode.c_cflag &= ~CLOCAL;
	pmode.c_cflag |= B0 | HUPCL;
	ioctl(mfd, TCSETAF, &pmode);
	close(mfd);

#if DIDO >= 2
	setuid(geteuid());
	setgid(getegid());
	unlink(lckname);
# ifdef GETTY_HANDLER
	restart();
# endif
#endif
	S1("Exiting XC");
}

/*	check to see if lock file exists and is still active.
	kill(pid, 0) only works on ATTSV, some BSDs and Xenix
	returns: SUCCESS, or
			FAILURE if lock file active
	added: Steve Manes 7/29/88
*/
checkLCK()
{
	int rc, fd;
#ifdef ASCII_PID
	char alckpid[PIDSIZE+2];
#endif
#if DIDO == 2
	short lckpid = -1;
#else
	pid_t lckpid = -1;
#endif

	if ((fd = open(lckname, O_RDONLY)) == -1){
		if (errno == ENOENT)
			return SUCCESS;	/* lock file doesn't exist */
		goto unlock;
	}
#ifdef ASCII_PID
	rc = read(fd, (char *)alckpid, PIDSIZE+1);
	close(fd);
	lckpid = atoi(alckpid);
	if (rc != 11)
#else
	rc = read(fd, (char *)&lckpid, SIZEOFLOCKFILE);
	close(fd);
	if (rc != SIZEOFLOCKFILE)
#endif
	{
		S1("Lock file has bad format");
		goto unlock;
	}

	/* now, send a bogus 'kill' and check the results */
	if (kill(lckpid, 0) == 0 || errno == EPERM){
		sprintf(Msg,"Lock file process %d on %s is still active - try later",
			lckpid, port);
		S;
		return FAILURE;
	}

unlock:
	if (unlink(lckname) != 0){
		sprintf(Msg,"Can't unlink %s file", lckname);
		S;
		return FAILURE;
	}
	return SUCCESS;
}

/*	Opens the modem port and configures it. If the port string is
	already defined it will use that as the modem port; otherwise it
	gets the environment variable MODEM. Returns SUCCESS or FAILURE.
*/
mopen()
{
	int c;
	char *p;

	if (port[0] == '\0'){
		if (!(p = getenv("MODEM"))){
			S1("Exiting: no modem port specified or present in environment");
			exit(3);
		}
		mport(p);
	}
	if (!lock_tty())
		exit(4);

#if DIDO
	p = port +strlen(port) -1;
	*p = toupper(*p);
# ifdef GETTY_HANDLER
	suspend();
# endif
#endif

	if ((mfd = open(port, O_RDWR | O_NDELAY)) < 0){
		sprintf(Msg,"Can't open modem port %s",port);
		S;
		exit(5);
	}

	ioctl(mfd, TCGETA, &pmode);

	pmode.c_cflag &= ~(CBAUD | HUPCL);
	pmode.c_cflag |= CLOCAL | cbaud;
#if DIDO >= 3 & defined(CTSFLOW) & defined(RTSFLOW)
	pmode.c_cflag |= CTSFLOW | RTSFLOW ;
	/* pmode.c_cflag |= CRTSFL ; */
#endif
	pmode.c_iflag = IGNBRK ;
	pmode.c_oflag = pmode.c_lflag = 0;
	pmode.c_cc[VMIN] = 1; 	/* This many chars satisfies reads */
	pmode.c_cc[VTIME] = 0;	/* or in this many tenths of seconds */

	xc_setflow(flowflag);

	c = mfd;
	if ((mfd = open(port, O_RDWR)) < 0){	/* Reopen line with CLOCAL */
		sprintf(Msg,"Can't re-open modem port %s",port);
		S;
		return FAILURE;
	}
	close(c);

	return SUCCESS;
}
