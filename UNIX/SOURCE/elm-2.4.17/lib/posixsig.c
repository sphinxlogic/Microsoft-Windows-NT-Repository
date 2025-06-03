
static char rcsid[] = "@(#)$Id: posixsig.c,v 5.4 1992/12/07 03:13:08 syd Exp syd $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.4 $   $State: Exp $
 *
 *			Copyright (c) 1988-1992 USENET Community Trust
 *			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: posixsig.c,v $
 * Revision 5.4  1992/12/07  03:13:08  syd
 * Add code to work around SunOS and sigalrm not returning EINTR
 * From: Chip, Tom, Steve, Et. Al.
 *
 * Revision 5.3  1992/11/07  19:30:01  syd
 * Fix redefinition complaint by SCO 3.2v2.0.
 * From: chip@chinacat.unicom.com (Chip Rosenthal)
 *
 * Revision 5.2  1992/10/27  16:08:32  syd
 * Fix non ansi declaration
 * From: tom@osf.org
 *
 * Revision 5.1  1992/10/27  01:43:56  syd
 * Initial Checkin
 * Moved from src/signals.c
 *
 *
 ******************************************************************************/

/** Duplicate the old signal() call with POSIX sigaction

**/

#include "headers.h"

#ifdef POSIX_SIGNALS
/*
 * This routine used to duplicate the old signal() calls
 */
SIGHAND_TYPE
#if (defined(__STDC__) && !defined(apollo))
(*posix_signal(signo, fun))(int)
	int signo;
	SIGHAND_TYPE (*fun)(int);
#else
(*posix_signal(signo, fun))()
	int signo;
	SIGHAND_TYPE (*fun)();
#endif
{
	struct sigaction act;	/* new signal action structure */
	struct sigaction oact;  /* returned signal action structure */ 

	/*   Setup a sigaction struct */

 	act.sa_handler = fun;        /* Handler is function passed */
	sigemptyset(&(act.sa_mask)); /* No signal to mask while in handler */
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
                act.sa_flags |= SA_INTERRUPT;           /* SunOS */
#endif
        }
	
	/* use the sigaction() system call to set new and get old action */

	sigemptyset(&oact.sa_mask);
	if(sigaction(signo, &act, &oact))
		/* If sigaction failed return -1 */
	    return(SIG_ERR);
	else
        	/* use the previous signal handler as a return value */
	    return(oact.sa_handler);
}
#endif /* POSIX_SIGNALS */
