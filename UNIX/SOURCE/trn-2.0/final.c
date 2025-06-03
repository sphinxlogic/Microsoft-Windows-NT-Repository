/* $Id: final.c,v 1.2 92/01/11 16:28:08 usenet Exp $
 *
 * $Log:	final.c,v $
 * Revision 1.2  92/01/11  16:28:08  usenet
 * Lots of bug fixes:
 * 
 *    o    More upgrade notes for trn 1.x users (see ** LOOK ** in NEW).
 *    o    Enhanced the article-reading code to not remember our side-trip
 *         to the end of the group between thread selections (to fix '-').
 *    o    Extended trrn's handling of new articles (we fetch the active file
 *         more consistently, and listen to what the GROUP command tells us).
 *    o    Enhanced the thread selector to mention when new articles have
 *         cropped up since the last visit to the selector.
 *    o    Changed strftime to use size_t and added a check for size_t in
 *         Configure to make sure it is defined.  Also made it a bit more
 *         portable by using gettimeofday() and timezone() in some instances.
 *    o    Fixed a problem with the "total" structure not getting zero'ed in
 *         mthreads (causing bogus 'E'rrors on null groups).
 *    o    Fixed a reference to tmpbuf in intrp.c that was bogus.
 *    o    Fixed a problem with using N and Y with the newgroup code and then
 *         trying to use the 'a' command.
 *    o    Fixed an instance where having trrn get ahead of the active file
 *         might declare a group as being reset when it wasn't.
 *    o    Reorganized the checks for Apollo's C library to make sure it doesn't
 *         find the Domain OS version.
 *    o    Added a check for Xenix 386's C library.
 *    o    Made the Configure PATH more portable.
 *    o    Fixed the arguments prototypes to int_catcher() and swinch_catcher().
 *    o    Fixed the insert-my-subject-before-my-sibling code to not do this
 *         when the sibling's subject is the same as the parent.
 *    o    Fixed a bug in the RELAY code (which I'm suprised is still being used).
 *    o    Twiddled the mthreads.8 manpage.
 *    o    mthreads.8 is now installed and the destination is prompted for in
 *         Configure, since it might be different from the .1 destination.
 *    o    Fixed a typo in newsnews.SH and the README.
 * 
 * Revision 4.4.2.1  1991/12/01  18:05:42  sob
 * Patchlevel 2 changes
 *
 * Revision 4.4  1991/09/09  20:18:23  sob
 * release 4.4
 *
 *
 * 
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

#include "EXTERN.h"
#include "common.h"
#include "util.h"
#include "term.h"
#include "ng.h"
#include "init.h"
#include "bits.h"
#include "last.h"
#include "rcstuff.h"
#include "ngdata.h"
#include "artio.h"
#ifdef SERVER
#include "server.h"
#endif
#include "INTERN.h"
#include "final.h"

void
final_init()
{
#ifdef SIGTSTP
    sigset(SIGTSTP, stop_catcher);	/* job control signals */
    sigset(SIGTTOU, stop_catcher);	/* job control signals */
    sigset(SIGTTIN, stop_catcher);	/* job control signals */
#endif

    sigset(SIGINT, int_catcher);	/* always catch interrupts */
#ifdef SIGHUP
    sigset(SIGHUP, sig_catcher);	/* and hangups */
#endif
    sigset(SIGILL, sig_catcher);
#ifdef SIGTRAP
    sigset(SIGTRAP, sig_catcher);
#endif
    sigset(SIGFPE, sig_catcher);
#ifdef SIGBUS
    sigset(SIGBUS, sig_catcher);
#endif
    sigset(SIGSEGV, sig_catcher);
#ifdef SIGSYS
    sigset(SIGSYS, sig_catcher);
#endif
    sigset(SIGTERM, sig_catcher);
#ifdef SIGXCPU
    sigset(SIGXCPU, sig_catcher);
#endif
#ifdef SIGXFSZ
    sigset(SIGXFSZ, sig_catcher);
#endif
#ifdef SIGWINCH
    sigset(SIGWINCH, winch_catcher);
#endif

#ifndef lint
#ifdef SIGEMT
    sigignore(SIGEMT);
#endif
#endif /* lint */
}

void					/* very much void */
finalize(status)
int status;
{
    termlib_reset();
    if (bizarre)
	resetty();
    if (lockname && *lockname)
 	UNLINK(lockname);
#ifdef USETHREADS
    if (tmpthread_group)
	UNLINK(tmpthread_file);
#endif
#ifdef SERVER
    if (*active_name)
	UNLINK(active_name);
    if (openart) {
 	char artname[MAXFILENAME];
	char intrpwork[MAXFILENAME];
	interp(intrpwork,MAXFILENAME,"%P");
 	sprintf(artname, "%s/rrn%ld.%d", intrpwork, (long)openart, getpid());
 	UNLINK(artname);
    }
    close_server();
#endif /* SERVER */
    if (status < 0) {
	chdir("/usr/tmp");
	sigset(SIGILL,SIG_DFL);
	abort();
    }
    exit(status);
}

/* come here on interrupt */

SIGRET
int_catcher(dummy)
int dummy;
{
    sigset(SIGINT,int_catcher);
#ifdef DEBUGGING
    if (debug)
	write(2,"int_catcher\n",12);
#endif
    if (!waiting) {
	if (int_count) {		/* was there already an interrupt? */
	    write(2,"\nBye-bye.\n",10);
	    sig_catcher(0);		/* emulate the other signals */
	}
	int_count++;
    }
}

/* come here on signal other than interrupt, stop, or cont */

SIGRET
sig_catcher(signo)
int signo;
{
#ifdef VERBOSE
    static char *signame[] = {
	"",
	"HUP",
	"INT",
	"QUIT",
	"ILL",
	"TRAP",
	"IOT",
	"EMT",
	"FPE",
	"KILL",
	"BUS",
	"SEGV",
	"SYS",
	"PIPE",
	"ALRM",
	"TERM",
	"???"
#ifdef SIGTSTP
	,"STOP",
	"TSTP",
	"CONT",
	"CHLD",
	"TTIN",
	"TTOU",
	"TINT",
	"XCPU",
	"XFSZ"
#ifdef SIGPROF
	,"VTALARM",
	"PROF"
#endif
#endif
	};
#endif

#ifdef DEBUGGING
    if (debug) {
	printf("\nSIG%s--.newsrc not restored in debug\n",signame[signo]);
	finalize(-1);
    }
#endif
    if (panic)
	abort();
    (void) sigset(SIGILL,SIG_DFL);
    panic = TRUE;			/* disable terminal I/O */
    if (doing_ng) {			/* need we reconstitute rc line? */
#ifdef DELAYMARK
	yankback();
#endif
	restore_ng();			/* then do so (hope this works) */
    }
    doing_ng = FALSE;
    if (rc_changed)			/* need we write .newsrc out? */
	write_rc();			/* then do so */
    rc_changed = FALSE;
#ifdef SIGHUP
    if (signo != SIGHUP)
#endif
#ifdef VERBOSE
	IF(verbose)
	    printf("\nCaught %s%s--.newsrc restored\n",
		signo ? "a SIG" : "an internal error", signame[signo]);
	ELSE
#endif
#ifdef TERSE
	    printf("\nSignal %d--bye bye\n",signo);
#endif
    switch (signo) {
#ifdef SIGBUS
    case SIGBUS:
#endif
    case SIGILL:
    case SIGSEGV:
	finalize(-signo);
    }
    finalize(1);				/* and blow up */
}

#ifdef SIGTSTP
/* come here on stop signal */

SIGRET
stop_catcher(signo)
int signo;
{
    if (!waiting) {
	checkpoint_rc();	/* good chance of crash while stopped */
	if (clear_on_stop) {
	    clear();
	    putchar('\n') FLUSH;
	}
	termlib_reset();
	resetty();		/* this is the point of all this */
#ifdef DEBUGGING
	if (debug)
	    write(2,"stop_catcher\n",13);
#endif
	sigset(signo,SIG_DFL);	/* enable stop */
#ifdef SIGBLOCK
	sigsetmask(sigblock(0) & ~(1 << (signo-1)));
#endif
	kill(0,signo);		/* and do the stop */
    	savetty();
#ifdef MAILCALL
    	mailcount = 0;			/* force recheck */
#endif
    	if (!panic) {
	    if (!waiting) {
		termlib_init();
	    	noecho();			/* set no echo */
	    	crmode();			/* set cbreak mode */
	    	forceme("\f");		/* cause a refresh */
					/* (defined only if TIOCSTI defined) */
		errno = 0;			/* needed for getcmd */
	    }
    	}
    }
    sigset(signo,stop_catcher);	/* unenable the stop */
}
#endif
