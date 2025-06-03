
static char rcsid[] = "@(#)$Id: mailmsg2.c,v 5.12 1992/12/11 01:58:50 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.12 $   $State: Exp $
 *
 * 			Copyright (c) 1988-1992 USENET Community Trust
 * 			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: mailmsg2.c,v $
 * Revision 5.12  1992/12/11  01:58:50  syd
 * Allow for use from restricted shell by putting SHELL=/bin/sh in the
 * environment of spawned mail transport program.
 * From: chip@tct.com (Chip Salzenberg)
 *
 * Revision 5.11  1992/12/07  04:26:26  syd
 * add missing err declare
 * From: Syd
 *
 * Revision 5.10  1992/12/07  03:20:30  syd
 * Change $HOME/Cancelled.mail in message to /fullpath/Cancelled.mail
 * as AFS uses different meaning for $HOME than Elm does and it was
 * confusing to some users.
 * From: Syd
 *
 * Revision 5.9  1992/11/26  01:46:26  syd
 * add Decode option to copy_message, convert copy_message to
 * use bit or for options.
 * From: Syd and bjoerns@stud.cs.uit.no (Bjoern Stabell)
 *
 * Revision 5.8  1992/11/26  00:46:13  syd
 * changes to first change screen back (Raw off) and then issue final
 * error message.
 * From: Syd
 *
 * Revision 5.7  1992/11/22  01:14:20  syd
 * Allow SCO MMDF to use the mmdf library for mailer via execmail.
 * From: Larry Philps <larryp@sco.com>
 *
 * Revision 5.6  1992/11/07  20:29:28  syd
 * These small typos stopped me from making forms, so here is a patch.
 * From: andrew.mcparland@rd.eng.bbc.co.uk (Andrew McParland)
 *
 * Revision 5.5  1992/11/07  16:21:56  syd
 * There is no need to write out the MIME-Version header in subparts
 * From: Klaus Steinberger <Klaus.Steinberger@Physik.Uni-Muenchen.DE>
 *
 * Revision 5.4  1992/10/30  21:04:04  syd
 * fix a bug in the forms reply caused by the Cc: buffer not being
 * cleared.  If one has sent mail just prior to replying to the form and
 * specified a Cc:  address, the form reply is also sent to those Cc:
 * address(es).
 * From: dwolfe@pffft.sps.mot.com (Dave Wolfe)
 *
 * Revision 5.3  1992/10/25  01:47:45  syd
 * fixed a bug were elm didn't call metamail on messages with a characterset,
 * which could be displayed by elm itself, but message is encoded with QP
 * or BASE64
 * From: Klaus Steinberger <Klaus.Steinberger@Physik.Uni-Muenchen.DE>
 *
 * Revision 5.2  1992/10/24  13:44:41  syd
 * There is now an additional elmrc option "displaycharset", which
 * sets the charset supported on your terminal. This is to prevent
 * elm from calling out to metamail too often.
 * Plus a slight documentation update for MIME composition (added examples)
 * From: Klaus Steinberger <Klaus.Steinberger@Physik.Uni-Muenchen.DE>
 *
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** Interface to allow mail to be sent to users.  Part of ELM  **/

#include "headers.h"
#include "s_elm.h"
#include <errno.h>
#include <ctype.h>

#ifdef BSD 
#undef tolower
#undef toupper
#endif

extern int errno;
extern char version_buff[];

char *error_description(), *strip_parens();
char *strcat(), *strcpy(), *index();
char *format_long(), *strip_commas(), *tail_of_string(); 
long ftell();

unsigned long sleep();

#ifdef SITE_HIDING 
 char *get_ctime_date();
#endif
FILE *write_header_info();

/* these are all defined in the mailmsg1.c file! */

extern char subject[SLEN], in_reply_to[SLEN], expires[SLEN],
            action[SLEN], priority[SLEN], reply_to[SLEN], to[VERY_LONG_STRING], 
	    cc[VERY_LONG_STRING], expanded_to[VERY_LONG_STRING], 
	    expanded_reply_to[LONG_STRING],
	    expanded_cc[VERY_LONG_STRING], user_defined_header[SLEN],
	    bcc[VERY_LONG_STRING], expanded_bcc[VERY_LONG_STRING],
	    precedence[SLEN], expires_days[SLEN];


#ifdef	MIME
int msg_is_multipart;
#endif /* MIME */
long C_L_Position[2];   /*To Remember position of the Content-Length*/
long C_StartData[2];    /*To Remember length of Header Area */
long C_EndData[2];	/* To Remember the end of the BodyPart */

int  gotten_key;
char *bounce_off_remote();

/*
 * remove_hostbang - Given an expanded list of addresses, remove all
 * occurrences of "thishost!" at the beginning of addresses.
 * This hack is useful in itself, but it is required now because of the
 * kludge disallowing alias expansion on return addresses.
 */

void
remove_hostbang(addrs)
char *addrs;
{
	int i, j, hlen, flen;

	if ((hlen = strlen(hostname)) < 1)
	  return;

	flen = strlen(hostfullname);
	i = j = 0;

	while (addrs[i]) {
	  if (i == 0 || isspace(addrs[i - 1])) {
	    if (strncmp(&addrs[i], hostname, hlen) == 0 &&
	      addrs[i + hlen] == '!') {
	        i += hlen + 1;
	        continue;
	    }
	    if (strncmp(&addrs[i], hostfullname, flen) == 0 &&
	      addrs[i + flen] == '!') {
	        i += flen + 1;
	        continue;
	    }
	  }
	  addrs[j++] = addrs[i++];
	}
	addrs[j] = 0;
}

int
mail(copy_msg, edit_message, form)
int  copy_msg, edit_message, form;
{
	/** Given the addresses and various other miscellany (specifically, 
	    'copy-msg' indicates whether a copy of the current message should 
	    be included, 'edit_message' indicates whether the message should 
	    be edited) this routine will invoke an editor for the user and 
	    then actually mail off the message. 'form' can be YES, NO, or
	    MAYBE.  YES=add "Content-Type: mailform" header, MAYBE=add the
	    M)ake form option to last question, and NO=don't worry about it!
	    Also, if 'copy_msg' = FORM, then grab the form temp file and use
	    that...
	    Return TRUE if the main part of the screen has been changed
	    (useful for knowing whether a redraw is needed.
	**/

	FILE *reply, *real_reply; /* second is post-input buffer */
	char *whole_msg_file, *tempnam();
	char filename[SLEN], fname[SLEN], copy_file[SLEN],
             very_long_buffer[VERY_LONG_STRING], mailerflags[NLEN];
	int ch, sys_status, line_len;
	register int retransmit = FALSE; 
	int      already_has_text = FALSE;		/* we need an ADDRESS */
	int	 signature_done = FALSE;
	int	 need_redraw = 0;
	int	 err;

	static int cancelled_msg = 0;

	dprint(4, (debugfile, "\nMailing to \"%s\" (with%s editing)\n",
		  expanded_to, edit_message? "" : "out"));
	
	gotten_key = 0;		/* ignore previously gotten encryption key */

	/** first generate the temporary filename **/

	sprintf(filename,"%s%s%d", temp_dir, temp_file, getpid());

	/** if possible, let's try to recall the last message? **/

	if (! batch_only && copy_msg != FORM && user_level != 0)
	  retransmit = recall_last_msg(filename, copy_msg, &cancelled_msg, 
		       &already_has_text);

	/** if we're not retransmitting, create the file.. **/

	if (! retransmit)
	  if ((reply = fopen(filename,"w")) == NULL) {
	    err = errno;
	    dprint(1, (debugfile, 
               "Attempt to write to temp file %s failed with error %s (mail)\n",
		 filename, error_description(err)));
	    if(batch_only) {
	      MCprintf(catgets(elm_msg_cat, ElmSet, ElmCouldNotCreateFile,
		"Could not create file %s (%s)."),
		filename, error_description(err));
	      printf("\n");
	    } else
	      error2(catgets(elm_msg_cat, ElmSet, ElmCouldNotCreateFile,
		"Could not create file %s (%s)."),
		filename, error_description(errno));
	    return(need_redraw);
	  }

	chown (filename, userid, groupid);

	/* copy the message from standard input */
	if (batch_only) {
	  while (line_len = fread(very_long_buffer, 1, sizeof(very_long_buffer), stdin))
	    fwrite(very_long_buffer, 1, line_len, reply);
	}

	/** if there is an included file, copy it into the temp file **/
	if (*included_file) {
	  FILE *input;
	  if ((input = fopen(included_file,"r")) == NULL) {
	    dprint(1, (debugfile, 
              "Can't open included file %s.  Failed with error %s (mail)\n",
	      included_file, error_description(errno)));
	    error2(catgets(elm_msg_cat, ElmSet, ElmCouldNotOpenFile,
	      "Could not open file %s."), included_file);
	    return(need_redraw);
	  }

	  while (fgets(very_long_buffer, VERY_LONG_STRING, input) != NULL) 
	    fputs(very_long_buffer, reply);

	  fclose(input);
	  already_has_text = TRUE;
	}

	if (copy_msg == FORM) {
	  sprintf(fname, "%s%s%d", temp_dir, temp_form_file, getpid());
	  fclose(reply);	/* we can't retransmit a form! */
	  if (access(fname,ACCESS_EXISTS) != 0) {
	    if(batch_only) {
	      printf(catgets(elm_msg_cat, ElmSet, ElmCouldNotFindForm,
		"Couldn't find forms file!"));
	      printf("\n");
	    } else
	      error(catgets(elm_msg_cat, ElmSet, ElmCouldNotFindForm,
		"Couldn't find forms file!"));
	    return(need_redraw);
	  }
	  dprint(4, (debugfile, "-- renaming existing file %s to file %s --\n",
		  fname, filename));
	  rename(fname, filename);

	  /* kill leftover headers since forms reply skips regular init */
	  expires[0] = '\0';
	  expires_days[0] = '\0';
	  action[0] = '\0';
	  priority[0] = '\0';
	  reply_to[0] = '\0';
	  expanded_reply_to[0] = '\0';
	  cc[0] = '\0';
	  expanded_cc[0] = '\0';
	  user_defined_header[0] = '\0';
	  bcc[0] = '\0';
	  expanded_bcc[0] = '\0';
	  precedence[0] = '\0';
	}
	else if (copy_msg && ! retransmit) {  /* if retransmit we have it! */
	  if (attribution[0]) {
	    fprintf(reply, attribution, headers[current-1]->from);
	    fputc('\n', reply);
	  }
	  else if (forwarding) {
	    fputs("Forwarded message:\n", reply);
	  }
	  if (edit_message) {
	    copy_message(prefixchars, reply,
		( noheader ? CM_REMOVE_HEADER : 0 ) | CM_MMDF_HEAD | CM_DECODE);
	    already_has_text = TRUE;	/* we just added it, right? */
	  }
	  else
	    copy_message(prefixchars, reply,
		( noheader ? CM_REMOVE_HEADER : 0 ) | CM_MMDF_HEAD);
	}

        /* append signature now if we are going to use an external editor */
	/* Don't worry about the remote/local determination too much */

        if (already_has_text || 
           (strcmp(editor,"builtin") != 0 && strcmp(editor,"none") != 0)) {
	     signature_done = TRUE;
             if (!retransmit && copy_msg != FORM) 
	       already_has_text |= append_sig(reply);
	}

	if (! retransmit && copy_msg != FORM)
	  if (reply != NULL)
	    (void) fclose(reply);	/* on replies, it won't be open! */

	/** Edit the message **/

	ch = edit_message? 'e' : ' ';	/* drop through if needed... */

	/* calculate default save_file name */
	if(auto_cc) {
	  if(save_by_name) {
	    if(force_name) {
	      strcpy(copy_file, "=");	/* signals save by 'to' logname */
	    } else {
	      strcpy(copy_file, "=?");	/* conditional save by 'to' logname */
	    }
	  } else {
	    strcpy(copy_file, "<");	/* signals save to sentmail */
	  }
	} else *copy_file = '\0';	/* signals to not save a copy */


	if (! batch_only) {
	  do {
	    switch (ch) {
	      case 'e': need_redraw = 1;
			if (edit_the_message(filename, already_has_text)) {
			  cancelled_msg = TRUE;
			  return(need_redraw);
			}
			break;

	      case 'c': if (name_copy_file(copy_file))
			  need_redraw = 1;
			break;

	      case 'h': edit_headers();
			need_redraw = 1;
			break;

#ifdef ISPELL
	      case 'i': ClearScreen();
		        sprintf(very_long_buffer, "%s %s %s", ISPELL_PATH, ISPELL_OPTIONS, filename);
		        system_call(very_long_buffer, SY_ENAB_SIGHUP);
			need_redraw = 1;
		        break;
#endif

#ifdef ALLOW_SUBSHELL
	      case '!': if (subshell()) {
			  ClearScreen();
			  need_redraw = 1;
			}
			break;
#endif

	      default : /* do nothing */ ;
	    }

	    /** ask that silly question again... **/
  
	    if ((ch = verify_transmission(filename, &form)) == 'f') {
	      cancelled_msg = TRUE;
	      return(need_redraw);
	    }

	  } while (ch != 's');

	  if (form == YES) 
	    if (format_form(filename) < 1) {
	      cancelled_msg = TRUE;
	      return(need_redraw);
	    }

	  if ((reply = fopen(filename,"r")) == NULL) {
	      err = errno;
	      dprint(1, (debugfile,
	    "Attempt to open file %s for reading failed with error %s (mail)\n",
                filename, error_description(err)));
	      error1(catgets(elm_msg_cat, ElmSet, ElmCouldNotOpenReply,
		"Could not open reply file (%s)."), error_description(err));
	      return(need_redraw);
	  }
	}
	else if ((reply = fopen(filename,"r")) == NULL) {
	  err = errno;
	  dprint(1, (debugfile, 
	    "Attempt to open file %s for reading failed with error %s (mail)\n",
             filename, error_description(err)));
	  printf(catgets(elm_msg_cat, ElmSet, ElmCouldNotOpenReply,
		"Could not open reply file (%s)."), error_description(err));
	  printf("\n");
	  return(need_redraw);
	}

	cancelled_msg = FALSE;	/* it ain't cancelled, is it? */

	/** ask about bounceback if the user wants us to.... **/

	if (uucp_hops(to) > bounceback && bounceback > 0 && copy_msg != FORM) 
	  if (verify_bounceback() == TRUE) {
	    if (strlen(cc) > 0) strcat(expanded_cc, ", ");
	    strcat(expanded_cc, bounce_off_remote(to));
	  }

#ifdef	MIME
	/* Look if its a Multimedia/multipart Message */
	msg_is_multipart = check_for_multipart(reply);

	if (msg_is_multipart == -1) {
		return(need_redraw);
	}
#endif /* MIME */

	/** grab a copy if the user so desires... **/

	if (*copy_file) /* i.e. if copy_file contains a name */
	  save_copy(expanded_to, expanded_cc, expanded_bcc,
	       filename, copy_file, form);

	/** write all header information into whole_msg_file **/

	if((whole_msg_file=tempnam(temp_dir, "snd.")) == NULL) {
	  dprint(1, (debugfile, "couldn't make temp file nam! (mail)\n"));
	  if(batch_only) {
	    printf(catgets(elm_msg_cat, ElmSet, ElmCouldNotMakeTemp,
		"Sorry - couldn't make temp file name!"));
	    printf("\n");
	  } else if(mail_only)
	    error(catgets(elm_msg_cat, ElmSet, ElmCouldNotMakeTemp,
		"Sorry - couldn't make temp file name."));
	  else
	    set_error(catgets(elm_msg_cat, ElmSet, ElmCouldNotMakeTemp,
		"Sorry - couldn't make temp file name."));
	  return(need_redraw);
	}

	/** try to write headers to new temp file **/

	dprint(6, (debugfile, "Composition file='%s' and mail buffer='%s'\n", 
		    filename, whole_msg_file));

	dprint(2,(debugfile,"--\nTo: %s\nCc: %s\nBcc: %s\nSubject: %s\n---\n", 
		  expanded_to, expanded_cc, expanded_bcc, subject));

	if ((real_reply=
	   write_header_info(whole_msg_file, expanded_to,
	     expanded_cc, expanded_bcc, form == YES, FALSE)) == NULL) {

	  /** IT FAILED!!  MEIN GOTT!  Use a dumb mailer instead! **/

	  dprint(3, (debugfile, "** write_header failed: %s\n", 
		 error_description(errno)));

	  if (cc[0] != '\0')  		/* copies! */
	    sprintf(expanded_to,"%s %s", expanded_to, expanded_cc);

	  quote_args(very_long_buffer, strip_parens(strip_commas(expanded_to)));
	  strcpy(expanded_to, very_long_buffer);

	  sprintf(very_long_buffer, "( (%s -s \"%s\" %s ; %s %s) & ) < %s",
		  mailx, subject, expanded_to, remove_cmd, filename, filename);

	  if(batch_only) {
	    printf(catgets(elm_msg_cat, ElmSet, ElmUsedDumpMailer,
		"Message sent using dumb mailer %s."), mailx);
	    printf("\n");
	  } else
	    error1(catgets(elm_msg_cat, ElmSet, ElmUsedDumpMailer,
		"Message sent using dumb mailer %s."), mailx);
	  sleep(2);	/* ensure time to see this prompt! */

	}
	else {

	  C_StartData[0] = ftell(real_reply);

	  copy_message_across(reply, real_reply, FALSE);

          /* Append signature if not done earlier */

          if (!signature_done && !retransmit && copy_msg != FORM)
               append_sig(real_reply);
	  
#ifdef MIME
	  if (!copy_msg != FORM && msg_is_multipart) {
	    fprintf(real_reply, "--%s--\n", MIME_BOUNDARY);
	    if (C_L_Position[1] != 0L) {
	      C_EndData[1] = ftell(real_reply);
	      C_L_Position[1] = fseek(real_reply, C_L_Position[1], 0);
	      fprintf(real_reply, "%d", C_EndData[1] - C_StartData[1]);
	      fseek(real_reply, C_EndData[1], 0);
	    }
	  }
#endif

	  C_EndData[0] = ftell(real_reply);
	    
	  fseek(real_reply, C_L_Position[0], 0);
	  fprintf(real_reply, "%d", C_EndData[0] - C_StartData[0]);
	  fclose(real_reply);

	  if (cc[0] != '\0')  				         /* copies! */
	    sprintf(expanded_to,"%s %s", expanded_to, expanded_cc);

	  if (bcc[0] != '\0') {
	    strcat(expanded_to, " ");
	    strcat(expanded_to, expanded_bcc);
	  }

	  remove_hostbang(expanded_to);

	  if (strcmp(sendmail, mailer) == 0
#ifdef SITE_HIDING
	      && ! is_a_hidden_user(username)) {
#else
	     					) {
#endif
	    strcpy(mailerflags, (sendmail_verbose ? smflagsv : smflags));
	    if (metoo) strcat(mailerflags, smflagmt);
	  } else if (strcmp(submitmail, mailer) == 0)
	    strcpy(mailerflags, submitflags_s);
	  else if (strcmp(execmail, mailer) == 0) {
	    strcpy(mailerflags, (sendmail_verbose ? emflagsv : emflags));
	    if (metoo) strcat(mailerflags, emflagmt);
	  } else
	    mailerflags[0] ='\0';

	  if (strcmp(submitmail, mailer) == 0)
	    strcpy(expanded_to, " ");
	  else {
	    quote_args(very_long_buffer, strip_parens(strip_commas(expanded_to)));
	    strcpy(expanded_to, very_long_buffer);
	  }

	  sprintf(very_long_buffer,"( (%s %s %s ; %s %s) & ) < %s", 
		mailer, mailerflags, expanded_to,
		remove_cmd, whole_msg_file, whole_msg_file);
	}
	
	fclose(reply);

	if(batch_only) {
	  printf(catgets(elm_msg_cat, ElmSet, ElmSendingMail,
		"Sending mail..."));
	  printf("\n");
	} else {
	  PutLine0(LINES,0,catgets(elm_msg_cat, ElmSet, ElmSendingMail,
		"Sending mail..."));
	  CleartoEOLN();
	}

	/* Take note of mailer return value */

	if ( sys_status = system_call(very_long_buffer, SY_ENV_SHELL) ) {
		/* problem case: */
		if (mail_only || batch_only) {
		   printf("\r\n");
		   printf(catgets(elm_msg_cat, ElmSet, ElmMailerReturnedError,
			"mailer returned error status %d"), sys_status);
		   printf("\r\n");
		} else {
		   sprintf(very_long_buffer, catgets(elm_msg_cat, ElmSet, ElmCouldNotCreateFile,
			"mailer returned error status %d"), sys_status);
		   set_error(very_long_buffer);
		}
	} else {
		/* Success case: */
		if(batch_only) {
		  printf(catgets(elm_msg_cat, ElmSet, ElmMailSent, "Mail sent!"));
	          printf("\n");
		} else if(mail_only)
		  error(catgets(elm_msg_cat, ElmSet, ElmMailSent, "Mail sent!"));
		else
		  set_error(catgets(elm_msg_cat, ElmSet, ElmMailSent, "Mail sent!"));
	}

	/* Unlink temp file now.
	 * This is a precaution in case the message was encrypted.
	 * I.e. even though this file is readable by the owner only,
	 * encryption is supposed to hide things even from someone
	 * with root privelges. The best we can do is not let this
	 * file just hang after we're finished with it.
	 */
	(void)unlink(filename);

	return(need_redraw);
}

mail_form(address, subj)
char *address, *subj;
{
	/** copy the appropriate variables to the shared space... */

	strcpy(subject, subj);
	strcpy(to, address);
	strcpy(expanded_to, address);

	return(mail(FORM, NO, NO));
}

int
recall_last_msg(filename, copy_msg, cancelled_msg, already_has_text)
char *filename;
int  copy_msg, *cancelled_msg, *already_has_text;
{
	char ch;
	char msg[SLEN];

	/** If filename exists and we've recently cancelled a message,
	    the ask if the user wants to use that message instead!  This
	    routine returns TRUE if the user wants to retransmit the last
	    message, FALSE otherwise...
	**/

	register int retransmit = FALSE;

	if (access(filename, EDIT_ACCESS) == 0 && *cancelled_msg) {
	  Raw(ON);
	  CleartoEOLN();
	  if (copy_msg)
	    MCsprintf(msg, catgets(elm_msg_cat, ElmSet, ElmRecallLastInstead,
		     "Recall last kept message instead? (%c/%c) "),
		     *def_ans_yes, *def_ans_no);
	  else
	    MCsprintf(msg, catgets(elm_msg_cat, ElmSet, ElmRecallLastKept,
		     "Recall last kept message? (%c/%c) "),
		     *def_ans_yes, *def_ans_no);
	  do {
	    PutLine0(LINES-1, 0, msg);
	    CleartoEOLN();
	    fflush(stdout);
	    ch = ReadCh();
	    ch = tolower(ch);
	    if (ch == *def_ans_yes) {
	      Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmYes, "Yes."), 0);	
              retransmit++;
	      *already_has_text = TRUE;
	    } else if (ch == *def_ans_no) {
	      Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmNo, "No."), 0);	
	    } else {
	      Write_to_screen("%c??", 1, 07);	/* BEEP */
	      sleep(1);
	      ch = 0;
	    }
	  } while (ch == 0);

	  fflush(stdout);

	  *cancelled_msg = 0;
	}

	return(retransmit);
}

int
verify_transmission(filename, form_letter)
char *filename;
int  *form_letter;
{
	/** Ensure the user wants to send this.  This routine returns
	    the character entered.  Modified compliments of Steve Wolf 
	    to add the'dead.letter' feature.
	    Also added form letter support... 
	**/

	FILE *deadfd, *messagefd;
	char ch, buffer[SLEN], fname[SLEN];
	int x_coord, y_coord, line_len;

	while(1) {
	  /* clear bottom part of screen */
	  MoveCursor(LINES-2,0);
	  CleartoEOS();

	  /* display prompt and menu according to
	   * user level and what's available on the menu */
	  if (user_level == 0) {
	    PutLine0(LINES-2,0, catgets(elm_msg_cat, ElmSet, ElmPleaseChooseSend,
      "Please choose one of the following options by parenthesized letter: s"));
	    GetXYLocation(&x_coord, &y_coord);
	    y_coord--;	/* backspace over default answer */
	    Centerline(LINES-1,
	      "e)dit message, edit h)eaders, s)end it, or f)orget it.");
	  } else {
	    PutLine0(LINES-2, 0, catgets(elm_msg_cat, ElmSet, ElmAndNowSend, "And now: s"));
	    GetXYLocation(&x_coord, &y_coord);
	    y_coord--;	/* backspace over default answer */
	    if (*form_letter == PREFORMATTED) {
              strcpy(buffer, catgets(elm_msg_cat, ElmSet, ElmChoose, "Choose "));
	    } else if (*form_letter == YES) {
	        strcat(buffer, catgets(elm_msg_cat, ElmSet, ElmEditForm, "Choose e)dit form, "));
	    } else if (*form_letter == MAYBE)  {
	       strcpy(buffer, catgets(elm_msg_cat, ElmSet, ElmEditMsg, "Choose e)dit msg, m)ake form, "));
	    } else {
	       strcpy(buffer, catgets(elm_msg_cat, ElmSet, ElmEditMessage, "Choose e)dit message, "));
	    }
#ifdef ALLOW_SUBSHELL
	    strcat(buffer, catgets(elm_msg_cat, ElmSet, ElmShell, "!)shell, "));
#endif
	    strcat(buffer, catgets(elm_msg_cat, ElmSet, ElmHeadersCopy, "h)eaders, c)opy file, "));
#ifdef ISPELL
	    strcat(buffer, catgets(elm_msg_cat, ElmSet, ElmIspell, "i)spell, "));
#endif
	    strcat(buffer, catgets(elm_msg_cat, ElmSet, ElmSendorForget, "s)end, or f)orget."));
	    Centerline(LINES-1, buffer);
	  }

	  /* wait for answer */
	  fflush(stdin);
	  fflush(stdout);
	  Raw(ON);	/* double check... testing only... */
	  MoveCursor(x_coord, y_coord);
	  ch = ReadCh();
	  ch = tolower(ch);

	  /* process answer */
	  switch (ch) {
	     case 'f': Write_to_screen("Forget",0);
		       if (mail_only) {
			 /** try to save it as a dead letter file **/
			 save_file_stats(fname);
			 sprintf(fname, "%s/%s", home, dead_letter);
			 if ((deadfd = fopen(fname,"a")) == NULL) {
			   dprint(1, (debugfile,
		   "\nAttempt to append to deadletter file '%s' failed: %s\n\r",
			       fname, error_description(errno)));
			   error(catgets(elm_msg_cat, ElmSet, ElmMessageNotSaved,
				"Message not saved, Sorry."));
			 }
			 else if ((messagefd = fopen(filename, "r")) == NULL) {
			   dprint(1, (debugfile,
			     "\nAttempt to read reply file '%s' failed: %s\n\r",
				   filename, error_description(errno)));
			   error(catgets(elm_msg_cat, ElmSet, ElmMessageNotSaved,
				"Message not saved, Sorry."));
			 } else {
			   /* if we get here we're okay for everything */
			   while (line_len = fread(buffer, 1, sizeof(buffer), messagefd))
			     fwrite(buffer, 1, line_len, deadfd);

			   fclose(messagefd);
			   fclose(deadfd);
			   restore_file_stats(fname);

			   error2(catgets(elm_msg_cat, ElmSet, ElmMessageSavedIn,
				"Message saved in file \"%s/%s\"."), home, dead_letter);

			}
		       } else if (user_level != 0)
			set_error(catgets(elm_msg_cat, ElmSet, ElmMessageKept,
				"Message kept. Can be restored at next f)orward, m)ail or r)eply."));
		       break;

	     case '\n' :
	     case '\r' :
	     case 's'  : Write_to_screen("Send",0);
			 ch = 's';		/* make sure! */
			 break;

	     case 'm'  : if (*form_letter == MAYBE) {
			   *form_letter = YES;
		           switch (check_form_file(filename)) {
			     case -1 : return('f');
			     case 0  : *form_letter = MAYBE;  /* check later!*/
				       error(catgets(elm_msg_cat, ElmSet, ElmNoFieldsInForm,
					 "No fields in form!"));
				       return('e');
			     default : continue;
	                   }
			 }
			 else {
	                    Write_to_screen("%c??", 1, 07);	/* BEEP */
			    sleep(1);
		            continue;
	                 }
	     case 'e'  :  if (*form_letter != PREFORMATTED) {
			    Write_to_screen("Edit",0);
	 	            if (*form_letter == YES) 
			      *form_letter = MAYBE;
	                  }
			  else {
	                    Write_to_screen("%c??", 1, 07);	/* BEEP */
			    sleep(1);
		            continue;
	                 }
			 break;

	     case 'h'  : Write_to_screen("Headers",0);
			 break;

#ifdef ISPELL
	     case 'i'  : if (*form_letter != PREFORMATTED) {
			    Write_to_screen("Ispell",0);
	 	            if (*form_letter == YES) 
			      *form_letter = MAYBE;
	                  }
			  else {
	                    Write_to_screen("%c??", 1, 07);	/* BEEP */
			    sleep(1);
		            continue;
	                 }
			 break;
#endif

	     case 'c'  : Write_to_screen("Copy file",0);
			 break;

#ifdef ALLOW_SUBSHELL
	     case '!'  : break;
#endif

	     default   : Write_to_screen("%c??", 1, 07);	/* BEEP */
			 sleep(1);
		         continue;
	   }

	   return(ch);
	}
}

FILE *
write_header_info(filename, long_to, long_cc, long_bcc, form, copy)
char *filename, *long_to, *long_cc, *long_bcc;
int   form, copy;
{
	/** Try to open filedesc as the specified filename.  If we can,
	    then write all the headers into the file.  The routine returns
	    'filedesc' if it succeeded, NULL otherwise.  Added the ability
	    to have backquoted stuff in the users .elmheaders file!
	    If copy is TRUE, then treat this as the saved copy of outbound
	    mail.
	**/

	char opentype[3];
	long time(), thetime;
	char *ctime();
	static FILE *filedesc;		/* our friendly file descriptor  */
        char to_buf[VERY_LONG_STRING];
	int err;

#ifdef SITE_HIDING
	char  buffer[SLEN];
	int   is_hidden_user;		/* someone we should know about?  */
#endif
#ifdef MMDF
	int   is_submit_mailer;		/* using submit means change From: */
#endif /* MMDF */

	char  *get_arpa_date();

	if(copy) 
	    strcpy(opentype, "r+");
	else
	    strcpy(opentype, "w+");

	save_file_stats(filename);

	filedesc = fopen(filename, opentype);
	if (copy && filedesc == NULL)
	  filedesc = fopen(filename, "w+");

	if (filedesc == NULL) {
	  err = errno;
	  dprint(1, (debugfile,
	    "Attempt to open file %s for writing failed! (write_header_info)\n",
	     filename));
	  dprint(1, (debugfile, "** %s **\n\n", error_description(err)));
	  error2(catgets(elm_msg_cat, ElmSet, ElmErrorTryingToWrite,
		"Error %s encountered trying to write to %s."), 
		error_description(err), filename);
	  sleep(2);
	  return(NULL);		/* couldn't open it!! */
	}

	if (copy) {
		/* Position to the end of the file */
		fseek(filedesc, 0L, 2);
	}

	restore_file_stats(filename);

	if(copy) {	/* Add top line that mailer would add */
#ifdef MMDF
	  fprintf(filedesc, MSG_SEPARATOR);
#endif /* MMDF */
	  thetime = time((long *) 0);
	  fprintf(filedesc,"From %s %s", username, ctime(&thetime));
#ifdef MMDF
	} else if (strcmp(submitmail,mailer) == 0) {
	  sprintf(to_buf, "%s %s %s", long_to, long_cc, long_bcc);
	  do_mmdf_addresses(filedesc, strip_parens(strip_commas(to_buf)));
#endif /* MMDF */
	}

#ifdef SITE_HIDING
	if ( !copy && (is_hidden_user = is_a_hidden_user(username))) {
	  /** this is the interesting part of this trick... **/
	  sprintf(buffer, "From %s!%s %s\n",  HIDDEN_SITE_NAME,
		  username, get_ctime_date());
	  fprintf(filedesc, "%s", buffer);
	  dprint(1,(debugfile, "\nadded: %s", buffer));
	  /** so is this perverted or what? **/
	}
#endif


	/** Subject moved to top of headers for mail because the
	    pure System V.3 mailer, in its infinite wisdom, now
	    assumes that anything the user sends is part of the 
	    message body unless either:
		1. the "-s" flag is used (although it doesn't seem
		   to be supported on all implementations?? )
		2. the first line is "Subject:".  If so, then it'll
		   read until a blank line and assume all are meant
		   to be headers.
	    So the gory solution here is to move the Subject: line
	    up to the top.  I assume it won't break anyone elses program
	    or anything anyway (besides, RFC-822 specifies that the *order*
	    of headers is irrelevant).  Gahhhhh....
	**/

	fprintf(filedesc, "Subject: %s\n", subject);

	  fprintf(filedesc, "To: %s\n", format_long(long_to, strlen("To:")));

	fprintf(filedesc,"Date: %s\n", get_arpa_date());

#ifndef DONT_ADD_FROM
#ifdef MMDF
	is_submit_mailer = (strcmp(submitmail,mailer) == 0);
#endif /* MMDF */
# ifdef SITE_HIDING
#    ifdef MMDF
	if (is_submit_mailer)
	  fprintf(filedesc,"From: %s <%s>\n", full_username, username);
	else
#    endif /* MMDF */
	if (is_hidden_user)
	  fprintf(filedesc,"From: %s <%s!%s!%s>\n", full_username,
		  hostname, HIDDEN_SITE_NAME, username);
	else
	  fprintf(filedesc,"From: %s <%s!%s>\n", full_username,
		  hostname, username);
# else
#  ifdef  INTERNET
#   ifdef  USE_DOMAIN
#    ifdef MMDF
	if (is_submit_mailer)
	  fprintf(filedesc,"From: %s <%s>\n", full_username, username);
	else
#    endif /* MMDF */
	  fprintf(filedesc,"From: %s <%s@%s>\n", full_username, 
		username, hostfullname);
#   else
#    ifdef MMDF
	if (is_submit_mailer)
	  fprintf(filedesc,"From: %s <%s>\n", full_username, username);
	else
#    endif /* MMDF */
	fprintf(filedesc,"From: %s <%s@%s>\n", full_username,
		username, hostname);
#   endif
#  else
#    ifdef MMDF
	if (is_submit_mailer)
	  fprintf(filedesc,"From: %s <%s>\n", full_username, username);
	else
#    endif /* MMDF */
	fprintf(filedesc,"From: %s <%s!%s>\n", full_username,
		hostname, username);
#  endif
# endif
#endif

	if (cc[0] != '\0')
	    fprintf(filedesc, "Cc: %s\n", format_long(long_cc, strlen("Cc: ")));

	if (copy && (bcc[0] != '\0'))
	    fprintf(filedesc, "Bcc: %s\n", format_long(long_bcc, strlen("Bcc: ")));

	if (strlen(action) > 0)
	    fprintf(filedesc, "Action: %s\n", action);
	
	if (strlen(priority) > 0)
	    fprintf(filedesc, "Priority: %s\n", priority);

	if (strlen(precedence) > 0)
	    fprintf(filedesc, "Precedence: %s\n", precedence);
	
	if (strlen(expires) > 0)
	    fprintf(filedesc, "Expires: %s\n", expires);
	
	if (strlen(expanded_reply_to) > 0)
	    fprintf(filedesc, "Reply-To: %s\n", expanded_reply_to);

	if (strlen(in_reply_to) > 0)
	    fprintf(filedesc, "In-Reply-To: %s\n", in_reply_to);

	if (strlen(user_defined_header) > 0)
	    fprintf(filedesc, "%s\n", user_defined_header);

	add_mailheaders(filedesc);

#ifndef NO_XHEADER
	fprintf(filedesc, "X-Mailer: ELM [version %s]\n", version_buff);
#endif /* !NO_XHEADER */

	if (form)
	  fprintf(filedesc, "Content-Type: mailform\n");
#ifdef MIME
	else if (msg_is_multipart) {
	  fprintf(filedesc, "%s\n", MIME_HEADER);
	  fprintf(filedesc,
		"%s multipart/mixed; boundary=%s\n", MIME_CONTENTTYPE, MIME_BOUNDARY);
	}
#endif /* MIME */
	else {
#ifdef	MIME
	  fprintf(filedesc, "%s\n", MIME_HEADER);
	  fprintf(filedesc, "%s text/plain; charset=%s\n",MIME_CONTENTTYPE, charset);
	  fprintf(filedesc, "Content-Transfer-Encoding: %s\n", text_encoding);
#else
	  fprintf(filedesc, "Content-Type: text\n");
#endif /* MIME */
	}
	fprintf(filedesc, "Content-Length: ");
	C_L_Position[0] = ftell(filedesc);
	fprintf(filedesc, "          \n"); /* Print Blanks as Placeholders */

	putc('\n', filedesc);

	return((FILE *) filedesc);
}

copy_message_across(source, dest, copy)
FILE *source, *dest;
int copy;
{
	/** Copy the message in the file pointed to by source to the
	    file pointed to by dest.
	    If copy is TRUE, treat as a saved copy of outbound mail. **/

	int  crypted = FALSE;			/* are we encrypting?  */
	int  encoded_lines = 0;			/* # lines encoded     */
	int  line_len;
	char buffer[SLEN];			/* file reading buffer */
#ifdef MIME
	int	text_lines = 0;
	int	at_boundary = FALSE;

	C_L_Position[1] = 0L;
	C_StartData[1] = 0L;
	C_EndData[1] = 0L;

#endif /* MIME */

	while (line_len = mail_gets(buffer, SLEN, source)) {
	  if (buffer[0] == '[') {
	    if (strncmp(buffer, START_ENCODE, strlen(START_ENCODE))==0)
	      crypted = TRUE;
	    else if (strncmp(buffer, END_ENCODE, strlen(END_ENCODE))==0)
	      crypted = FALSE;
	    else if ((strncmp(buffer, DONT_SAVE, strlen(DONT_SAVE)) == 0)
	          || (strncmp(buffer, DONT_SAVE2, strlen(DONT_SAVE2)) == 0)) {
	      if(copy) break;  /* saved copy doesn't want anything after this */
	      else continue;   /* next line? */
#ifdef MIME
	    } else if (strncmp(buffer, MIME_INCLUDE, strlen(MIME_INCLUDE))==0) {
	      text_lines = 0;
	      if (!at_boundary) {
		if (C_L_Position[1] != 0L) {
		  C_EndData[1] = ftell(dest);
		  C_L_Position[1] = fseek(dest, C_L_Position[1], 0);
		  fprintf(dest, "%d", C_EndData[1] - C_StartData[1]);
		  fseek(dest, C_EndData[1], 0);
		}
		fprintf(dest, "\n--%s\n", MIME_BOUNDARY);
	      }
	      Include_Part(dest, buffer, FALSE);
	      fprintf(dest, "--%s\n", MIME_BOUNDARY);
	      at_boundary = TRUE;
	      continue;
#endif /* MIME */
	    }

	    if (crypted) {
	      if (! gotten_key++)
	        getkey(ON);
	      else if (! encoded_lines)
	        get_key_no_prompt();		/* reinitialize.. */
	      if (strncmp(buffer, START_ENCODE, strlen(START_ENCODE))) {
	        encode(buffer);
	        encoded_lines++;
	      }
	    }
	  }
	  else if (crypted) {
	    if (batch_only) {
	      printf(catgets(elm_msg_cat, ElmSet, ElmNoEncryptInBatch,
		"Sorry. Cannot send encrypted mail in \"batch mode\".\n"));
	      leave(0);
	    } else if (! gotten_key++)
	      getkey(ON);
	    else if (! encoded_lines)
	      get_key_no_prompt();		/* reinitialize.. */
	    if (strncmp(buffer, START_ENCODE, strlen(START_ENCODE))) {
	      encode(buffer);
	      encoded_lines++;
	    }
#ifdef MIME
	  } else {
	    if (text_lines == 0) {
	      if (msg_is_multipart) {
		if (!at_boundary) {
	 	  fprintf(dest,"--%s\n",MIME_BOUNDARY);
		}
	        fprintf(dest, "%s text/plain; charset=%s\n",
			MIME_CONTENTTYPE, charset);
		fprintf(dest, "Content-Transfer-Encoding: %s\n", text_encoding);
		fprintf(dest, "Content-Length: ");
		C_L_Position[1] = ftell(dest);
		fprintf(dest, "          \n"); /* Print Placeholders */
	        fprintf(dest, "\n");
		C_StartData[1] = ftell(dest);
		at_boundary = FALSE;
	      }
	    }	
	    text_lines++;
#endif /* MIME */
          }

#ifndef DONT_ESCAPE_MESSAGES
	  if (copy && (strncmp(buffer, "From ", 5) == 0)) {
	    /* Add in the > to a From on our copy */
	    fprintf(dest, ">");
	    if (fwrite(buffer, 1, line_len, dest) != line_len) {
		MoveCursor(LINES, 0);
		Raw(OFF);
		Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmWriteFailedCopyAcross,
			"\nWrite failed in copy_message_across\n"), 0);
		emergency_exit();
	    }
	  }
#ifdef NEED_LONE_PERIOD_ESCAPE
	  else if (!copy && strcmp(buffer, ".\n") == 0)
	    /* Because some mail transport agents take a lone period to
	     * mean EOF, we add a blank space on outbound message.
	     */
	    fputs(". \n", dest);
#endif /* NEED_LONE_PERIOD_ESCAPE */
  	  else
#endif /* DONT_ESCAPE_MESSAGES */
  	    if (fwrite(buffer, 1, line_len, dest) != line_len) {
		MoveCursor(LINES, 0);
		Raw(OFF);
		Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmWriteFailedCopyAcross,
			"\nWrite failed in copy_message_across\n"), 0);
		emergency_exit();
	    }
	} 
#ifdef MMDF
	if (copy) fputs(MSG_SEPARATOR, dest);
#else
	if (copy) fputs("\n", dest);	/* ensure a blank line at the end */
#endif /* MMDF */
}

int
verify_bounceback()
{
	char	ch;
	char	msg[SLEN];

	/** Ensure the user wants to have a bounceback copy too.  (This is
	    only called on messages that are greater than the specified 
	    threshold hops and NEVER for non-uucp addresses.... Returns
	    TRUE iff the user wants to bounce a copy back.... 
	 **/

	MoveCursor(LINES,0);
	CleartoEOLN();
	MCsprintf(msg, catgets(elm_msg_cat, ElmSet, ElmBounceOffRemote,
	      "\"Bounce\" a copy off the remote machine? (%c/%c) %c%c"),
	      *def_ans_yes, *def_ans_no, *def_ans_yes, BACKSPACE);
	PutLine0(LINES,0, msg);
	fflush(stdin);	/* wait for answer! */
	fflush(stdout);
	ch = ReadCh();
	if (tolower(ch) != *def_ans_yes) { 
	  Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmNo, "No."), 0);
	  fflush(stdout);
	  return(FALSE);
	}
	Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmYes, "Yes."), 0);
	fflush(stdout);

	return(TRUE);
}


int
append_sig(file)
FILE *file;
{
	/* Append the correct signature file to file.  Return TRUE if
           we append anything.  */

        /* Look at the to and cc list to determine which one to use */

	/* We could check the bcc list too, but we don't want people to
           know about bcc, even indirectly */

	/* Some people claim that  user@anything.same_domain should be 
	   considered local.  Since it's not the same machine, better be 
           safe and use the remote sig (presumably it has more complete
           information).  You can't necessarily finger someone in the
           same domain. */

	  if (!batch_only && (local_signature[0] || remote_signature[0])) {

            char filename2[SLEN];
	    char *sig;

  	    if (index(expanded_to, '!') || index(expanded_cc,'!'))
              sig = remote_signature;		/* ! always means remote */
            else {
	      /* check each @ for @thissite.domain */
	      /* if any one is different than this, then use remote sig */
	      int len;
	      char *ptr;
	      char sitename[SLEN];
	      sprintf(sitename,"@%s",hostfullname);
	      len = strlen(sitename);
              sig = local_signature;
              for (ptr = index(expanded_to,'@'); ptr;  /* check To: list */
	          ptr = index(ptr+1,'@')) {
		if (strincmp(ptr,sitename,len) != 0
		    || (*(ptr+len) != ',' && *(ptr+len) != 0
		    && *(ptr+len) != ' ')) {
	          sig = remote_signature;
                  break;
                }
              }
              if (sig == local_signature)		   /* still local? */ 
                for (ptr = index(expanded_cc,'@'); ptr;   /* check Cc: */
		    ptr = index(ptr+1,'@')) {
		  if (strincmp(ptr,sitename,len) != 0
		      || (*(ptr+len) != ',' && *(ptr+len) != 0 
		      && *(ptr+len) != ' ')) {
	            sig = remote_signature;
                    break;
                  }
                }
            }

            if (sig[0]) {  /* if there is a signature file */
	      if (sig[0] != '/')
	        sprintf(filename2, "%s/%s", home, sig);
	      else
	        strcpy(filename2, sig);
	      /* append the file - with a news 2.11 compatible */
	      /* seperator if "sig_dashes" is enabled */
	      (void) append(file, filename2, (sig_dashes ? "\n-- \n" : NULL));

              return TRUE;
            }
          }

return FALSE;

}

