
static char rcsid[] = "@(#)$Id: remail.c,v 5.6 1992/11/26 01:46:26 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.6 $   $State: Exp $
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
 * $Log: remail.c,v $
 * Revision 5.6  1992/11/26  01:46:26  syd
 * add Decode option to copy_message, convert copy_message to
 * use bit or for options.
 * From: Syd and bjoerns@stud.cs.uit.no (Bjoern Stabell)
 *
 * Revision 5.5  1992/11/26  00:49:04  syd
 * fix use of errno
 * From: Syd
 *
 * Revision 5.4  1992/11/22  01:14:20  syd
 * Allow SCO MMDF to use the mmdf library for mailer via execmail.
 * From: Larry Philps <larryp@sco.com>
 *
 * Revision 5.3  1992/10/24  13:35:39  syd
 * changes found by using codecenter on Elm 2.4.3
 * From: Graham Hudspith <gwh@inmos.co.uk>
 *
 * Revision 5.2  1992/10/11  00:59:39  syd
 * Fix some compiler warnings that I receive compiling Elm on my SVR4
 * machine.
 * From: Tom Moore <tmoore@fievel.DaytonOH.NCR.COM>
 *
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** For those cases when you want to have a message continue along
    to another person in such a way as they end up receiving it with
    the return address the person YOU received the mail from (does
    this comment make any sense yet?)...

**/

#include "headers.h"
#include "s_elm.h"
#include <errno.h>

extern int errno;

char *error_description();
extern void display_to();

int
remail()
{
	/** remail a message... returns TRUE if new foot needed ... **/
	
	FILE *mailfd;
	char entered[VERY_LONG_STRING], expanded[VERY_LONG_STRING];
	char *filename, buffer[VERY_LONG_STRING], ch;
	char mailerflags[NLEN];
	int  err;
	extern char *tempnam();

	entered[0] = '\0';

	get_to(entered, expanded);
	if (strlen(entered) == 0)
	  return(0);

	display_to(expanded);

	if((filename=tempnam(temp_dir, "snd.")) == NULL) {
	  dprint(1, (debugfile, "couldn't make temp file nam! (remail)\n"));
	  sprintf(buffer, catgets(elm_msg_cat, ElmSet, ElmCouldntMakeTempFileName,
		"Sorry - couldn't make file temp file name."));
	  set_error(buffer);
	  return(1);
	}

	if ((mailfd = fopen(filename, "w")) == NULL) {
	  err = errno;
	  dprint(1, (debugfile, "couldn't open temp file %s! (remail)\n", 
		  filename));
	  dprint(1, (debugfile, "** %s **\n", error_description(err)));
	  sprintf(buffer, catgets(elm_msg_cat, ElmSet, ElmCouldntOpenForWriting,
			"Sorry - couldn't open file %s for writing (%s)."),
		  error_description(err));
	  set_error(buffer);
	  return(1);
	}

	/** now let's copy the message into the newly opened
	    buffer... **/

	chown (filename, userid, groupid);

#ifdef MMDF
	if (strcmp(submitmail, mailer) == 0)
	  do_mmdf_addresses(mailfd, strip_parens(strip_commas(expanded)));
#endif /* MMDF */

	copy_message("", mailfd, CM_REMOTE | CM_MMDF_HEAD | CM_REMAIL);

	fclose(mailfd);

	/** Got the messsage, now let's ensure the person really wants to 
	    remail it... **/

	ClearLine(LINES-1);
	ClearLine(LINES);
	MCsprintf(buffer, catgets(elm_msg_cat, ElmSet, ElmSureYouWantToRemail,
	    "Are you sure you want to remail this message (%c/%c)? %c%c"),
	    *def_ans_yes, *def_ans_no, *def_ans_yes, BACKSPACE);
	PutLine0(LINES-1, 0, buffer);
	fflush(stdin);
	fflush(stdout);
	ch = ReadCh();
	if (tolower(ch) == *def_ans_no) { /* another day, another No... */
	  Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmNo, "No."), 0);
	  set_error(catgets(elm_msg_cat, ElmSet, ElmBounceCancelled,
		"Bounce of message cancelled."));
	  (void) unlink(filename);
	  return(1);
	}
	Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmYes, "Yes."), 0);

	if (strcmp(sendmail, mailer) == 0
#ifdef SITE_HIDING
	    && ! is_a_hidden_user(username))
#else
					 )
#endif
	  strcpy(mailerflags, (sendmail_verbose ? smflagsv : smflags));
	else if (strcmp(submitmail, mailer) == 0) {
	  strcpy(mailerflags, submitflags_s);
	  strcpy(expanded, " ");
	} else if (strcmp(execmail, mailer) == 0) {
	  strcpy(mailerflags, (sendmail_verbose ? emflagsv : emflags));
	} else
	  mailerflags[0] = '\0';
	
	sprintf(buffer,"( (%s %s %s ; %s %s) & ) < %s", 
	      mailer, mailerflags, strip_parens(strip_commas(expanded)), 
	      remove_cmd, filename, filename);

	PutLine0(LINES, 0, catgets(elm_msg_cat, ElmSet, ElmResendingMail,
		"Resending mail..."));
	(void) system_call(buffer, 0);
	set_error(catgets(elm_msg_cat, ElmSet, ElmMailResent, "Mail resent."));

	return(1);
}
#ifdef MMDF
do_mmdf_addresses(dest_file,buffer)
FILE *dest_file;
char *buffer;
{
	char old[VERY_LONG_STRING], first[VERY_LONG_STRING], 
		rest[VERY_LONG_STRING];

	strcpy(old,buffer);
	split_word(old, first, rest);
	while (strcmp(first, "") != 0) {
	  fprintf(dest_file, "%s\n", first);
	  strcpy(old, rest);
	  split_word(old, first, rest);
	}
	fprintf(dest_file, "\n");
}
#endif /* MMDF */
