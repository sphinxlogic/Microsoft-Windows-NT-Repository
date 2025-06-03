
static char rcsid[] = "@(#)$Id: exitprog.c,v 5.1 1992/10/03 22:58:40 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
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
 * $Log: exitprog.c,v $
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

#include "headers.h"
#include "s_elm.h"

int
exit_prog()
{
	/** Exit, abandoning all changes to the mailbox (if there were
	    any, and if the user say's it's ok)
	**/

	char *msg, answer;
	register int i, changes;

	dprint(1, (debugfile, "\n\n-- exiting --\n\n"));

	/* Determine if any messages are scheduled for deletion, or if
	 * any message has changed status
	 */
	for (changes = 0, i = 0; i < message_count; i++)
	  if (ison(headers[i]->status, DELETED) || headers[i]->status_chgd)
	    changes++;
	
	if (changes) {
	  /* YES or NO on softkeys */
	  if (hp_softkeys) {
	    define_softkeys(YESNO);
	    softkeys_on();
	  }
	  if (changes == 1)
	    msg = catgets(elm_msg_cat, ElmSet, ElmAbandonChange,
		"Abandon change to mailbox? (y/n) ");
	  else
	    msg = catgets(elm_msg_cat, ElmSet, ElmAbandonChangePlural,
		"Abandon changes to mailbox? (y/n) ");
	  answer = want_to(msg, 'n');

	  if(answer != 'y') return -1;
	}

	fflush(stdout);
	return leave(0);
}
