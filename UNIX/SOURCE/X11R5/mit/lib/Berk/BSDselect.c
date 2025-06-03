#ifdef WANT_BSDSELECT
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>



/*  This is a fix for the difference between BSD's select() and
 *  some SVR4's select().  SVR4's select() can never return a value larger
 *  than the total number of file descriptors being checked.  So, if
 *  you select for read and write on one file descriptor, and both
 *  are true, SVR4 select() will only return 1.  BSD select in the
 *  same situation will return 2.
 *
 *	Additionally, BSD select() on timing out, will zero the masks,
 *	while SVR4 does not.  This is fixed here as well.
 *
 *	Set your tabstops to 4 characters to have this code nicely formatted.
 *
 *	Jerry Whelan, guru@bradley.edu, June 12th, 1993
 */

/* $XFree86: mit/lib/Berk/BSDselect.c,v 2.0 1993/09/12 07:19:21 dawes Exp $ */

int
BSDselect(nfds, readfds, writefds, exceptfds, timeout)
int nfds;
fd_set *readfds, *writefds, *exceptfds;
struct timeval *timeout;
{
	int		rval,
			i;

	rval = select(nfds, readfds, writefds, exceptfds, timeout);

	switch(rval) {
		case -1:	return(rval);
					break;

		case 0:		if(readfds != NULL)
						FD_ZERO(readfds);
					if(writefds != NULL)
						FD_ZERO(writefds);
					if(exceptfds != NULL)
						FD_ZERO(exceptfds);

					return(rval);
					break;

		default:	for(i=0, rval=0; i < nfds; i++) {
						if((readfds != NULL) && FD_ISSET(i, readfds)) rval++;
						if((writefds != NULL) && FD_ISSET(i, writefds)) rval++;
						if((writefds != NULL) && FD_ISSET(i, exceptfds)) rval++;
					}
					return(rval);
		}
/* Should never get here */
}
#endif /* WANT_BSDSELECT */
