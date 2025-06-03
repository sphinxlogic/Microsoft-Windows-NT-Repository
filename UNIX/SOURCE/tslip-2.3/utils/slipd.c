/* @(#)slipd.c	1.4 (13 Oct 1993) */
/*..........................................................................
 . Copyright 1993, Jim Avera.  All Rights Reserved.
 . 
 . This program contains new material owned by the above copyright holder,
 . and may also contain portions derived from existing works used according
 . to permission granted by the owners of those works.
 .
 . You are prohibited from copying, distributing, modifying, or using this
 . file (or the portions owned by the above copyright holder) except as 
 . described in the file "COPYRIGHT" which accompanies this program.
 ...........................................................................*/

/*
 * ***PLEASE send jima@netcom.com notice of any problems or bugs you find.
 *    I'd also like to hear about successful deployment of this code.  Thanks!
 */

/*
 * slipd -- slip auto-dial daemon
 *
 * USAGE:
 *	slipd [-v] [-s /dev/tslip] [-c ifcname]
 *
 * With -c, the program exits immediately with the following status:
 *   0 - A dial-up link is currently established for slip interface "ifcname".
 *   1 - No link exists for interface "ifcname" (auto-dial would occur if used)
 *   2 - An error occurred (e.g., "ifcname" is not configured, or the tslip
 *       driver can't be opened).
 *
 * Without -c, the process becomes the slip auto-dial daemon, unless one
 * is already registered.
 *
 * The slip driver (which is interposed between ip and serial line drivers)
 * sends messages to the daemon to perform auto-dial functions.
 *
 * An auto-dial parameter string is stored in the slip driver (by slattach),
 * and sent to slipd in the connection-request message.  The parameter string
 * has the following format:
 *
 *	[ :bg-cmd-list: ] dialing-args...
 *
 * bg-cmd-list is a optional :-separated list of commands to be run in 
 *   background after the link is established.  These commands may do things
 *   which should occur whenever the link is established for some other purpose,
 *   but which aren't important enough by themselves to cause a dial-out.
 *   Typically bg-cmd-list is "slipcmd -r ifcname", which runs any commands
 *   queued by "slipcmd -q cmd...".  See the slipcmd description for details.
 *
 * dialing-args are used to make the connection, and have the same form as
 *   areguments to "cu" (e.g., a system name).  No ':' characters are allowed.
 *
 * As of 5/93 there are two alaternate dialing modules available (selected by 
 * Makefile):
 *
 *	sldial_tuucp.c	- calls Taylor UUCP dialing routines ala "cu"
 *	sldial_debug.c	- opens a hard-coded line for debug testing
 *
 * -Jim Avera (jima@netcom.com)
 */

/* NOTE: Slip support currently (5/93) only works for SVR4. */

#include "../sysdep.h"
#include "../patchlevel.h"

#include <setjmp.h>
#include <assert.h>
#include <pwd.h>

#include <termios.h>
#include <stropts.h>
#include <sys/mkdev.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/stream.h>
/* #include <sys/conf.h> */
#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include SLIP_HEADER

#include "slipd.h"

static void check_inuse(int fd_slip, char *ifcname);
static void slioctl(int fd, int cmd, char *errstring, void *buf, unsigned blen);
static void sllog(char *fmt, ...);
static void slerror (char *fmt, ...);
static void usage ();
static void slproc_msg( tslipd_t *p, char *s, unsigned slen );
static void *allocmem( void *oldp, unsigned n );
static void terminate( pid_t pid, unsigned grace );
static RETSIGTYPE sigchld_handler( int sig );
static RETSIGTYPE sigterm_handler( int sig );

static char *shellpath = "/bin/sh";
static char whitechars[] = " \t\f\n\r";

static char *slipname = SLIP_DEV_NAME_STR;
static int verbose = 0;
static int debug = 0;
static char *check_ifc = NULL;
static int fd_slip = -1;

static char *program;
static pid_t parent_pid;
static sigset_t SIGCHLD_set;

#define TERM_GRACE_SECS 3

main(argc, argv)
	char argc;
	char **argv;
{
	extern	char *optarg;
	extern	int  optind;

	int	i;
	int	flags;
	struct	strbuf	ctlbuf;
	tslipd_t	slbuf;
	struct	strbuf	dtabuf;
	char	dta[BUFSIZ];
	char 	*cp;

	if ((cp = strrchr(argv[0],'/')) == NULL) 
	    program=argv[0];
	else
	    program = cp+1;

        sigemptyset(&SIGCHLD_set); sigaddset(&SIGCHLD_set, SIGCHLD);
	while ((i = getopt (argc, argv, "c:ds:v")) != EOF)
		switch (i) {
		case 'c': check_ifc = optarg; break;
		case 'd': debug = verbose = 1; break;
		case 's': slipname = optarg; break;
		case 'v': verbose = 1; break;
		default:  sllog("Bad arg %c\n",i); usage ();
		}
	if (optind != argc) usage();

	/* Open communication to the slip driver */
	if ((fd_slip = open(slipname, O_RDWR)) < 0)
	    slerror("Can't open %s", slipname);
	if (verbose) 
	    sllog("Opened %s as fd %d\n", slipname, fd_slip);

	if (check_ifc) {
	    check_inuse(fd_slip, check_ifc);
	    /*NOTREACHED*/
	}

	/* Demonize ourself */
	if (!debug) {
	    fflush(stdout); fflush(stderr);
	    setpgrp();
	    setsid();
	    switch (fork()) {
	    case 0:  break;
	    case -1: perror ("fork failed"); exit (2);
	    default: exit(0);
	    }
  	}

	parent_pid = getpid();

	signal(SIGINT,SIG_DFL);
	signal(SIGTERM, sigterm_handler);
	signal(SIGCHLD, sigchld_handler);

	/* register ourself as the daemon */
	slioctl(fd_slip, SLP_REG_SLIPD, "SLP_REG_SLIPD", NULL, 0);

	if (verbose) 
	    sllog("Daemon (pid %d) is now registered\n", getpid());

	/* Wait for messages */
	ctlbuf.buf = (char *) &slbuf;
	ctlbuf.maxlen = sizeof(slbuf);
	dtabuf.buf = dta;
	dtabuf.maxlen = sizeof(dta);

	for (;;) {
		ctlbuf.len = 0;
		flags = 0;
		if ((i=getmsg (fd_slip, &ctlbuf, &dtabuf, &flags)) < 0) {
		    if (errno!=EINTR) {
		        perror("getmsg");
		        exit(2);
		    }
		    continue;
		}
		if (ctlbuf.len != sizeof(tslipd_t) || i != 0) {
		    sllog("Got wrong-length msg (%d)\n", ctlbuf.len);
		    continue;
		}
		slproc_msg( &slbuf, dta, dtabuf.len );
	}
	
}

static void
usage ()
{
	fprintf (stderr, "%s version %s:%d\n", program, VERSION, PATCHLEVEL);
	fprintf (stderr,"\n\
Usage: %s [-v] [-s %s] [-c ifc]\n\
   -c check if a dial-up link is active for interface \"ifc\"\n\
      (exit 0 if so, 1 if not, 2 if error)\n",
		 
		 program, slipname);
	exit (2);
}

/*
 * Attempt to determine if a dial-up slip interface is currently established.
 * Exit(0) if so, (1) if not, exit(> 1) if error.
 */
static void 
check_inuse(int fd_slip, char *ifcname)
{
	minor_t	minor;
	struct	strioctl iocb;
	tsl_t	*p_tsl;
	int	exitval = 1;
	union {
	    int	minor;
	    tsl_info_t tslinfo;
	} iocdata;

	for (minor=0; ;minor++) {
	    iocb.ic_cmd = SLP_GETINFO;
	    iocb.ic_timout = 15;
	    iocb.ic_dp = (char *)&iocdata;
	    iocb.ic_len = sizeof(iocdata);
	    iocdata.minor = minor;
	    if (ioctl (fd_slip, I_STR, &iocb) < 0) {
		if (minor != 0) {
		    sllog("Slip interface \"%s\" not found (configured?)\n",
			  ifcname);
		    exit(2);	
		}
	        slerror("SLP_GETINFO");
	    }
	    if (0==strncmp((char *)iocdata.tslinfo.origifn,
		           ifcname, sizeof(iocdata.tslinfo.origifn))) {
	        if (iocdata.tslinfo.lindex >= 0) {
	            if (verbose) sllog("%s is active\n", ifcname);
	            exitval = 0;
	        } else {
	            if (verbose) sllog("%s is NOT active\n", ifcname);
	        }
	        break;
	    }
	}
	exit(exitval);
}

/* Child pids associated with each slip minor# */
static pid_t	*pidlist = NULL;
static unsigned pidlist_cnt = 0;

static jmp_buf jmpbuf;
static int jmpbuf_valid = 0;	/* set only in a child process! */
static int got_sigterm = 0;
static RETSIGTYPE
sigterm_handler( int sig )
{
	/* sigterm_handler is armed in the parent and inherited by child
	 * processes. 
	 * In a child, execute a longjmp if jump_valid is true, otherwise
	 * just set got_sigterm and return.
	 */
	minor_t minor;
	pid_t pid;
	static char GotSIGTERM_msg[] = "Got SIGTERM\n";
	got_sigterm = 1;
	if (getpid() == parent_pid) {
	    sllog(GotSIGTERM_msg);
	    /* Terminate all recorded children and exit */
 	    for (minor=0; minor<pidlist_cnt; minor++) {
		if ((pid = pidlist[minor]) != 0) terminate(pid, 6);
	    }
	    exit(10);
	    /* NOTREACHED */
	} else {
	    if (verbose) sllog(GotSIGTERM_msg);
	    if (jmpbuf_valid) {
		longjmp(jmpbuf,1);
		/* NOTREACHED */
	    }
	    return;
	    /* NOTREACHED */
	}
}

/*
 * Process a message.  s is the data part (the dial parameter string)
 */
static void
slproc_msg(p, s, slen)
	tslipd_t *p;
	char	*s;
	unsigned slen;
{
	char	*cp;
	int	fd_dev;
	int	i, n;
	pid_t	pid, oldpid;
    	struct termios stio;
	void	*dial_state;
	char	*bgcmdlist;
	char	*dialparam;
	char	modname[FMNAMESZ+1];
	struct passwd *pw;
	time_t	login_time, logoff_time, et;

	if (verbose) {
	    switch (p->op) {
	    case SLPD_CONNREQ: cp="CONNECT"; break;
	    case SLPD_TIMEOUT: cp="TIMEOUT"; break;
	    case SLPD_HANGUP: cp="HANGUP"; break;
	    default: cp="<bad msg op>"; break;
	    }
	    sllog("Msg %s (%s) %d:%.*s m%ld p%d l=%d\n",
	    	  cp, p->origifn, slen, slen, s,
		  (long)p->minor, p->pid, p->lindex);
	}
	switch (p->op) {

	case SLPD_CONNREQ:
	    if (!debug) {
	        sigprocmask(SIG_BLOCK, &SIGCHLD_set, NULL);
	        switch (pid=fork()) {
	        case -1: 
		    perror("fork failed");
	            slioctl(fd_slip, SLP_CONNFAIL, "SLP_CONNFAIL",
			    (char *)&p->minor, sizeof(p->minor));
		    sigprocmask(SIG_UNBLOCK, &SIGCHLD_set, NULL);
		    return;
	        default: 
		    /* PARENT - save child pid associated with this minor# */
		    if (pidlist_cnt <= p->minor) {
			n = pidlist_cnt * 2;
			if (n <= p->minor) n = p->minor + 5 ;
			pidlist = allocmem((char *)pidlist, sizeof(*pidlist)*n);
		        for (i=pidlist_cnt; i<n; i++) pidlist[i] = 0;
		        pidlist_cnt = n;
		    }
		    oldpid=pidlist[p->minor];
		    pidlist[p->minor] = pid;
		    if (verbose) 
			sllog("Child pid %ld (minor %ld)\n", 
			      (long)pid, (long)p->minor);
		    sigprocmask(SIG_UNBLOCK, &SIGCHLD_set, NULL);
		    if (oldpid) {
		        sllog("CONN REQUEST: OLD PID %d ACTIVE!\n", oldpid);
		        terminate(oldpid, 3);
		    }
		    return; /*parent*/
	        case 0:  
		    break;
	        }
	        sigprocmask(SIG_UNBLOCK, &SIGCHLD_set, NULL);
	    }

	    /* CHILD - dial out and attach the port to the slip interface.
	     * The dialing parameter string was supplied by the driver
	     * (which stored it after being set by slattach).
	     * We wait until we get a signal (hangup or terminate) before
	     * closing & unlinking the port and returning.
	     */

	    /* Become the uucp user so lock files will be created correctly */
	    pw = getpwnam(OWNER);
	    if (pw==NULL) slerror("%s (OWNER) not defined!\n", OWNER);
	    if (setuid(pw->pw_uid) < 0)
		slerror("Cant setuid %s\n",OWNER);

	    if (slen==0 || s[slen-1] != '\0' || slen != (strlen(s)+1)) {
		sllog( "Corrupt dial string sent by slip driver\n");
		exit(2);
	    }

	    /* Isolate any :bg-cmd-list: prefix from the dialing args */
	    cp = strrchr(s, ':');
	    if (cp) {
	        bgcmdlist = s;
		*cp = '\0';
		dialparam = cp + 1;
	    } else {
		bgcmdlist = NULL;
		dialparam = s;	
	    }

	    /* DIAL OUT */
	    if (!slipd_dialout( dialparam, (char *)p->origifn, 
				&fd_dev, &dial_state )) {
	        exit(2);
	    }
	    time(&login_time);

    	    /* Set 8-bit clean raw mode */
    	    if (tcgetattr(fd_dev, &stio) < 0) slerror("tcgetattr");
    	    stio.c_cflag &= (CBAUD | CLOCAL);
    	    stio.c_cflag |= CS8 | CREAD | HUPCL;
    	    stio.c_iflag = BRKINT|IGNPAR;
    	    if (tcsetattr(fd_dev, TCSANOW, &stio) < 0) slerror("tcsetattr");

  	    /* pop off all stream modules (line discipline) */
  	    while(ioctl(fd_dev, I_LOOK, modname) >= 0) {
		if (verbose) sllog( "I_POP %s\n", modname);
      		ioctl(fd_dev, I_POP, modname);
    	    }

	    /* Link the device under the supervisory slip device */
  	    if ((p->lindex=ioctl( fd_slip, I_LINK, fd_dev )) < 0)
    	        slerror("Can't I_LINK port fd %d under slip fd %d", 
			fd_dev, fd_slip);
	    if (verbose) sllog("I_LINK returned %d\n", p->lindex);
  
 	    /* Move the device to the desired minor number */
	    slioctl(fd_slip, SLP_MOVE_LOWER, "SLP_MOVE_LOWER",
		    &p->minor, sizeof(p->minor));

	    sllog("ON %s (%s) minor %ld, lindex %ld\n", 
			(char *)p->origifn, dialparam, 
			(long)p->minor, (long)p->lindex);		

	    /* Execute any background commands given in the parameter string */
	    if (bgcmdlist) {
		signal(SIGCLD, SIG_IGN);
		cp = strtok(bgcmdlist,":");
		do {
		    int pid;
		    static char *cmd = NULL;
		    cp += strspn(cp,whitechars);
		    if (*cp) {
		        cmd = allocmem(cmd, strlen(cp)+20);
		        sprintf(cmd,"exec %s",cp);
			if (verbose) sllog("sh -c \"%s\"\n",cmd);
		        switch(pid = vfork()) {
		        case 0: execl(shellpath,"sh","-c",cmd,NULL);
			    slerror("exec %s",shellpath);
			    /*NOTREACHED*/
		        case -1:
			    sllog("vfork failed - bg cmd not launched\n");
			    break;
		        }
		    }
		} while ((cp = strtok(NULL,":")) != NULL);
	    }

	    /* 
	     * The parent slipd process sends us SIGTERM when it wants us to
	     * close the connection (after it has unlinked the driver from 
	     * slip).  
	     *
	     * slipd_dialout() may arm a SIGTERM handler, in which case it can
	     * close the modem connection, and should then re-raise the signal 
	     * with the previous handler installed, which gives the signal to 
	     * us. However, an error message may be logged, so we want to catch
	     * SIGTERM directly when possible and call slipd_undial() to
	     * gracefully close the connection.
	     *
	     * In either case, we log the end of the connection and exit.
	     */
	    if (setjmp(jmpbuf) == 0) {
		jmpbuf_valid = 1;
		signal(SIGTERM, sigterm_handler);
		if (!got_sigterm) {
    	            if (verbose) sllog("SLIP linkage completed. Pausing.\n");
		    pause();
		}
		jmpbuf_valid = 0;
	    }

	    /* Close and unlock the port.  slipd_undial MAY never return
	     * if it noticed the signal (e.g., taylor cu code)
	     */
  	    slipd_undial(dial_state);

	    time(&logoff_time);
	    et = logoff_time - login_time;
	    sllog("OFF %s (elapsed %02ld:%02ld)\n", p->origifn, et/60L, et%60L);

    	    if (verbose) sllog("Child: Normal exit(0)\n");
	    exit(0);
	    /*NOTREACHED*/
    
	case SLPD_TIMEOUT:
	case SLPD_HANGUP:
	    /* Unlink the driver and then signal the cu process to release
	     * the lock file.
	     */
	    if (p->minor >= pidlist_cnt) {
		sllog("ERROR:TIMEOUT or HUP for unknown minor %ld !!\n",
			(long)p->minor);
		break;
	    }
	    pid = pidlist[p->minor];

	    if (ioctl (fd_slip, I_UNLINK, p->lindex) < 0) {
		sllog("Cant I_UNLINK lindex=%d ", p->lindex); 
		fflush(stderr);
		perror("");
	    } else {
	        if (verbose) 
		    sllog("%s [%ld] UNLINK ok\n", p->origifn, (long)p->minor);
		/* Since the unlink succeeded, presumably the child dialed
		 * out successfully.  Erase our record of the child so that
		 * we won't send CONNFAIL when the child-death signal occurs.
		 */
		assert(p->minor < pidlist_cnt && pidlist[p->minor] == pid);
		if (p->minor < pidlist_cnt) 
		    pidlist[p->minor] = 0;
	    }

	    terminate(pid, 10);
	    break;

	default:
	    sllog("Unrecognized msg op=%d from %s\n", p->op, slipname);
	}
}

static void
sllog(char *fmt, ...)
{
	va_list ap;
	time_t t;
	struct tm *pt;
	time(&t); pt = localtime(&t);
	fprintf(stderr, "%s", program);
	/*
	if (getpid() != parent_pid) fprintf(stderr, "-%ld", (long)getpid());
	*/
	fprintf(stderr, " %ld", (long)getpid());
	fprintf(stderr, " %d/%d/%d %2d:%02d:%02d ", 
		pt->tm_mon+1, pt->tm_mday, pt->tm_year,
		pt->tm_hour, pt->tm_min, pt->tm_sec);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

static void
slerror(char *fmt, ...)
{
	int save_errno = errno;
	va_list ap;
	fprintf(stderr, "%s %ld: ", program, (long)getpid());
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fflush(stderr);
	errno = save_errno;
	perror(":");
	exit (2);
}

static void
slioctl(fd, cmd, errstring, data, datalen)
	int fd;
	int cmd;
	char *errstring;
	void *data;
	unsigned datalen;
{
	struct	strioctl iocb;

	if (verbose) sllog("%s\n", errstring);

	iocb.ic_cmd = cmd;
	iocb.ic_timout = 15;
	iocb.ic_dp = (datalen ? data : "");
	iocb.ic_len = datalen;
	if (ioctl (fd, I_STR, &iocb) < 0)
  	    slerror(errstring);
}

static void *
allocmem(oldp,n)
	void *oldp;
	unsigned n;
{
	void *p;
	if (oldp)
	    p = realloc( oldp, n );
	else
	    p = malloc( n );
	if (p == NULL) slerror("Malloc/Realloc failed");
	return p;
}

static void
terminate(pid, grace)
	pid_t	pid;
	unsigned grace;
{
	if (verbose) sllog("%s: Terminating pid %d\n",program,pid);
#ifndef SLDEBUG
	if (pid<2) { sllog("ERROR: pid==%d !!\n",pid); exit(2); }
#endif
	kill(pid, SIGTERM);
	while (kill(pid,0)>=0 && grace--) sleep(1);
	if (grace==0) {
	    if (verbose) 
		sllog("Didn't die. Doing kill -%d %d\n",pid,SIGKILL);
	    kill(pid, SIGKILL);
	}
}

static RETSIGTYPE /*ARGSUSED*/
sigchld_handler(sig)
	int sig;
{
	int statbuf, pid;
	minor_t minor;

	pid = wait(&statbuf);
	if (WIFSIGNALED(statbuf))
	    sllog("Child %d: died with signal %d\n", pid, WTERMSIG(statbuf));
	else
	if (WIFEXITED(statbuf)) {
	    if (statbuf!=0 && verbose)
		sllog("Child %d: exit(%d)\n", pid, WEXITSTATUS(statbuf));
        } else
            sllog("Child %d stopped/dead!? status=%lx\n", pid, statbuf);
	
	if (pid > 1) {
 	    for (minor=0; minor<pidlist_cnt; minor++) 
		if (pidlist[minor]==pid) break;
	    if (minor < pidlist_cnt) {
		if (statbuf != 0) {
		    /* The child did not exit normally.  Send negative
		     * reply to the slip driver.
		     */
	            slioctl(fd_slip, SLP_CONNFAIL, "SLP_CONNFAIL",
			    &minor, sizeof(minor));
		}
		pidlist[minor] = 0;
	    } else {
		/* We killed the child ourself, after removing it from 
		 * pidlist[].  It can be normal behavior for silpd_undial()
		 * to exit or resend SIGTERM to itself.
		 */
		/*EMPTY*/
	    }
	}
	signal(SIGCHLD, sigchld_handler);
}
