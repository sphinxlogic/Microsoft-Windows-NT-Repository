
static char rcsid[] = "@(#)$Id: can_open.c,v 5.2 1992/12/12 01:29:26 syd Exp $";

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
 * $Log: can_open.c,v $
 * Revision 5.2  1992/12/12  01:29:26  syd
 * Fix double inclusion of sys/types.h
 * From: Tom Moore <tmoore@wnas.DaytonOH.NCR.COM>
 *
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** can_open - can this user open this file using their normal uid/gid

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
can_open(file, mode)
char *file, *mode;
{
	/** Returns 0 iff user can open the file.  This is not
	    the same as can_access - it's used for when the file might
	    not exist... **/

	FILE *fd;
	int the_stat = 0, pid, w, preexisted = 0; 
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
	  if (access(file, ACCESS_EXISTS) == 0)
	    preexisted = 1;
	  if ((fd = fopen(file, mode)) == NULL)
	    _exit(errno);
	  else {
	    fclose(fd);		/* don't just leave it open! */
	    if(!preexisted)	/* don't leave it if this test created it! */
	      unlink(file);
	    _exit(0);
	  }
	  _exit(127);
	}

	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);

	while ((w = wait(&status)) != pid && w != -1)
		;

#ifdef WEXITSTATUS
	the_stat = WEXITSTATUS(status);
#else
#ifdef BSD
	the_stat = status.w_retcode;
#else
	the_stat = status >> 8;
#endif
#endif /*WEXITSTATUS*/
	
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);

	return(the_stat);
}
