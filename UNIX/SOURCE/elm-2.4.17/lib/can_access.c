
static char rcsid[] = "@(#)$Id: can_access.c,v 5.2 1992/12/12 01:29:26 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.2 $   $State: Exp $
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
 * $Log: can_access.c,v $
 * Revision 5.2  1992/12/12  01:29:26  syd
 * Fix double inclusion of sys/types.h
 * From: Tom Moore <tmoore@wnas.DaytonOH.NCR.COM>
 *
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** can_access - can this user access this file using their normal uid/gid

**/

#include "headers.h"
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>

#ifdef BSD
# include <sys/wait.h>
#endif

extern int errno;		/* system error number */

int
can_access(file, mode)
char *file; 
int   mode;
{
	/** returns ZERO iff user can access file or "errno" otherwise **/

	int the_stat = 0, pid, w; 
	struct stat stat_buf;
	void _exit();
#if defined(BSD) && !defined(WEXITSTATUS)
	union wait status;
#else
	int status;
#endif
	register SIGHAND_TYPE (*istat)(), (*qstat)();
	
#ifdef VFORK
	if ((pid = vfork()) == 0) {
#else
	if ((pid = fork()) == 0) {
#endif
	  setgid(groupid);
	  setuid(userid);		/** back to normal userid **/

	  errno = 0;

	  if (access(file, mode) == 0) 
	    _exit(0);
	  else 
	    _exit(errno != 0? errno : 1);	/* never return zero! */
	  _exit(127);
	}

	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);

	while ((w = wait(&status)) != pid && w != -1)
		;

#if	defined(WEXITSTATUS)
	/* Use POSIX macro if defined */
	the_stat = WEXITSTATUS(status);
#else
#ifdef BSD
	the_stat = status.w_retcode;
#else
	the_stat = status >> 8;
#endif
#endif	/*WEXITSTATUS*/

	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	if (the_stat == 0) {
	  if (stat(file, &stat_buf) == 0) {
	    w = stat_buf.st_mode & S_IFMT;
#ifdef S_IFLNK
	    if (w != S_IFREG && w != S_IFLNK)
#else
	    if (w != S_IFREG)
#endif
	      the_stat = 1;
	  }
	}

	return(the_stat);
}
