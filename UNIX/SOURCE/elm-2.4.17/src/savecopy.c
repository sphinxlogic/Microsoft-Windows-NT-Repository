
static char rcsid[] = "@(#)$Id: savecopy.c,v 5.4 1992/11/26 00:46:50 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.4 $   $State: Exp $
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
 * $Log: savecopy.c,v $
 * Revision 5.4  1992/11/26  00:46:50  syd
 * Fix how errno is used so err is inited and used instead
 * as errno gets overwritten by print system call
 * From: Syd
 *
 * Revision 5.3  1992/10/30  21:01:49  syd
 * More changes to folder creation confirmation
 * From: Larry Philps <larryp@sco.COM>
 *
 * Revision 5.2  1992/10/24  13:25:41  syd
 * In our global elm.rc I keep the four options as below
 *
 * 	confirmappend = OFF	Don't confirm every append to any file.
 * 	confirmcreate = ON	Confirm creation of every new file.
 * 	confirmfiles = ON	Confirm append to non folder files though.
 * 	confirmfolders = ON	In case someone does not want to be asked
 * 				every time when creating a new file try
 * 				to confirm creation of folders though.
 * From: Jukka Ukkonen <ukkonen@csc.fi>
 *
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** Save a copy of the specified message in a folder.

**/

#include "headers.h"
#include "s_elm.h"
#ifdef I_TIME
#  include <time.h>
#endif
#ifdef I_SYSTIME
#  include <sys/time.h>
#endif

#include <errno.h>

char *format_long(), *error_description(), *ctime();

extern int errno;

char *strcat(), *strcpy();
unsigned long sleep();

#ifdef MIME
extern int msg_is_multipart;
#endif
extern long C_L_Position[2];   /*To Remember position of the Content-Length*/
extern long C_StartData[2];    /*To Remember length of Header Area */
extern long C_EndData[2];   /* To Remeber the End of the Data */

save_copy(to, cc, bcc, filename, copy_file, form)
char *to, *cc, *bcc, *filename, *copy_file;
int form;
{
	/** This routine appends a copy of the outgoing message to the
	    file specified.  **/

	FILE *save,		/* file id for file to save to */
	     *message,		/* file id for file with message body */
	     *write_header_info();
	char  buffer[SLEN],	/* read buffer 		       */
	      savename[SLEN],	/* name of file saving into    */
	      ch;
	register int
	     is_ordinary_file;
	int  err;
  

	/* presume copy_file is okay as is for now */
	strcpy(savename, copy_file);

	/* if save-by-name wanted */
	if((strcmp(copy_file, "=") == 0)  || (strcmp(copy_file, "=?") == 0)) {

	  get_return_name(to, buffer, TRUE);	/* determine 'to' login */
	  if (strlen(buffer) == 0) {

	    /* can't get file name from 'to' -- use sent_mail instead */
	    dprint(3, (debugfile,
		"Warning: get_return_name couldn't break down %s\n", to));
	    error1(catgets(elm_msg_cat, ElmSet, ElmCannotDetermineToName,
"Cannot determine `to' name to save by! Saving to \"sent\" folder %s instead."),
	      sent_mail);
	    strcpy(savename, "<");
	    sleep(3);
	  } else
	    sprintf(savename, "=%s", buffer);		/* good! */
	}

	expand_filename(savename, TRUE);	/* expand special chars */

	/*
	 *  If saving conditionally by logname but folder doesn't
	 *  exist save to sent folder instead.
	 */
	if((strcmp(copy_file, "=?") == 0)
	      && (access(savename, ACCESS_EXISTS) != 0)) {
	  dprint(5, (debugfile,
	    "Conditional save by name: file %s doesn't exist - using \"<\".\n",
	    savename));
	  strcpy(savename, "<");
	  expand_filename(savename, TRUE);
	}

	/*
	 *  Allow options
	 *  confirm_files, confirm_folders,
	 *  confirm_append and confirm_create
	 *  to control where the actual copy
	 *  should be saved.
	 */
	is_ordinary_file = strncmp (savename, folders, strlen(folders));

        if (access(savename, ACCESS_EXISTS)== 0) {	/* already there!! */
	    if (confirm_append || (confirm_files && is_ordinary_file)) {
		/*
		 *  OK in batch mode it may be impossible
		 *  to ask the user to confirm. So we have
		 *  to use sent_mail anyway.
		 */
		if (batch_only) {
		    strcpy(savename, sent_mail);
		}
		else {
		    PutLine1 (LINES-2, 0,
                          /*
                           *    Well it should be like this but...
			      catgets (elm_msg_cat, ElmSet, ElmConfirmFiles,
				   "Append to an existing file `%s'? (y/n) n"),
                           */
			      is_ordinary_file
			      ? "Append to an existing file `%s'? (y/n) n"
			      : "Append to mail folder `%s'? (y/n) n",
			      savename);
		    ch = ReadCh ();
		    ch = tolower (ch);
		    PutLine2 (LINES-2, 0,
			      is_ordinary_file
			      ? "Append to an existing file `%s'? (y/n) %s"
			      : "Append to mail folder `%s'? (y/n) %s",
			      savename,
			      (ch == *def_ans_yes) ? "Yes" : "No");
		    sleep (1);
		    ClearLine (LINES-2);

		    if (ch != *def_ans_yes) {
			strcpy(savename, sent_mail);
			PutLine1 (LINES-2, 0,
				  "Alright - saving to `%s' instead",
				  savename);
			sleep (3);
			ClearLine (LINES-2);
		    }
		}
	    }
	}
        else {
            if (confirm_create || (confirm_folders && !is_ordinary_file)) {
		/*
		 *  OK in batch mode it may be impossible
		 *  to ask the user to confirm. So we have
		 *  to use sent_mail anyway.
		 */
		if (batch_only) {
		    strcpy(savename, sent_mail);
		}
		else {
		    PutLine1 (LINES-2, 0,
                          /*
                           *    Well it should be like this but...
			      catgets (elm_msg_cat, ElmSet, ElmConfirmFolders,
                                    "Create a new mail folder `%s'? (y/n) n"),
                           */
			      !is_ordinary_file
			      ? "Create a new mail folder `%s'? (y/n) n"
			      : "Create a new file `%s'? (y/n) n",
			      savename);
		    ch = ReadCh ();
		    ch = tolower (ch);
		    PutLine2 (LINES-2, 0,
			      !is_ordinary_file
			      ? "Create a new mail folder `%s'? (y/n) %s"
			      : "Create a new file `%s'? (y/n) %s",
			      savename,
			      (ch == *def_ans_yes) ? "Yes" : "No");
		    sleep (1);
		    ClearLine (LINES-2);

		    if (ch != *def_ans_yes) {
			strcpy(savename, sent_mail);
			PutLine1 (LINES-2, 0,
				  "Alright - saving to `%s' instead",
				  savename);
			sleep (3);
			ClearLine (LINES-2);
		    }
		}
	    }
	}

	if ((err = can_open(savename, "a"))) {
	  dprint(2, (debugfile,
	  "Error: attempt to autosave to a file that can't be appended to!\n"));
	  dprint(2, (debugfile, "\tfilename = \"%s\"\n", savename));
	  dprint(2, (debugfile, "** %s **\n", error_description(err)));

	  /* Lets try sent_mail before giving up */
	  if(strcmp(sent_mail, savename) == 0) {
	    /* we are ALREADY using sent_mail! */
	    error1(catgets(elm_msg_cat, ElmSet, ElmCannotSaveTo,
			"Cannot save to %s!"), savename);
	    sleep(3);
	    return(FALSE);
	  }

	  if ((err = can_open(sent_mail, "a"))) {
	    dprint(2, (debugfile,
	  "Error: attempt to autosave to a file that can't be appended to!\n"));
	    dprint(2, (debugfile, "\tfilename = \"%s\"\n", sent_mail));
	    dprint(2, (debugfile, "** %s **\n", error_description(err)));
	    error2(catgets(elm_msg_cat, ElmSet, ElmCannotSaveToNorSent,
		    "Cannot save to %s nor to \"sent\" folder %s!"),
		    savename, sent_mail);
	    sleep(3);
	    return(FALSE);
	  }
	  error2(catgets(elm_msg_cat, ElmSet, ElmCannotSaveToSavingInstead,
		"Cannot save to %s! Saving to \"sent\" folder %s instead."),
	        savename, sent_mail);
	  sleep(3);
	  strcpy(savename, sent_mail);
	}

	save_file_stats(savename);

	/* Write header */
	if ((save = write_header_info(savename, to, cc, bcc,
	      form == YES, TRUE)) == NULL)
	  return(FALSE);

	/* Now add file with message as handed to mailer */
	if ((message = fopen(filename, "r")) == NULL) {
	  err = errno;
	  fclose(save);
	  dprint(1, (debugfile,
		 "Error: Couldn't read folder %s (save_copy)\n", filename));
	  dprint(1, (debugfile, "** %s **\n", error_description(err)));
	  error1(catgets(elm_msg_cat, ElmSet, ElmCouldntReadFolder,
		"Couldn't read folder %s!"), filename);
	  sleep(3);
	  return(FALSE);
	}

	C_StartData[0] = ftell(save);

        copy_message_across(message, save, TRUE);

#ifdef MIME
	if (!form != NO && msg_is_multipart) {
	  fprintf(save, "--%%#%%record%%#%%--\n");
	  if (C_L_Position[1] != 0L) {
	    C_EndData[1] = ftell(save);
	    C_L_Position[1] = fseek(save, C_L_Position[1], 0);
	    fprintf(save, "%d", C_EndData[1] - C_StartData[1]);
	    fseek(save, C_EndData[1], 0);
	  }
	}
#endif
        C_EndData[0] = ftell(save) ;

#ifdef MMDF
	/*
	 * Actually, the C_EndData just calculated is wrong for MMDF.
	 * Because we are saving a copy instead of handing off to
	 * submit, copy_message_across will have added the trailing
	 * MMDF MSG_SEPARATOR to the end of the saved message to ensure
	 * a valid mailbox format.  We *must not* count that
	 * MSG_SEPARATOR when calculating the size of the message for
	 * Content-Length header!  In order to keep the hack for this
	 * localized to this function, we will just subtract off the
	 * length of the MSG_SEPARATOR.
	 */
	C_EndData[0] -= strlen(MSG_SEPARATOR);
#endif /* MMDF */

        fseek(save, C_L_Position[0], 0);
        fprintf(save, "%d", C_EndData[0] - C_StartData[0]);

	fclose(save);
	fclose(message);

	restore_file_stats(savename);

	return(TRUE);
}
char *
cf_english(fn)
char *fn;
{
    /** Return "English" expansion for special copy file name abbreviations
	or just the file name  **/

    if(!*fn)
      return(catgets(elm_msg_cat, ElmSet, ElmNoSave, "<no save>"));
    else if(!fn[1]) {
      if(*fn == '=')
	return(catgets(elm_msg_cat, ElmSet, ElmUncondSaveByName, "<unconditionally save by name>"));
      else if(*fn == '<')
	return(catgets(elm_msg_cat, ElmSet, ElmSentFolder, "<\"sent\" folder>"));
    } else if ((fn[0] == '=') && (fn[1] == '?'))
      return(catgets(elm_msg_cat, ElmSet, ElmCondSaveByName, "<conditionally save by name>"));

    return(fn);
}

static char *ncf_prompt = NULL;
int
name_copy_file(fn)
char *fn;
{
    /** Prompt user for name of file for saving copy of outbound msg to.
	Return true if we need a redraw. **/

    int redraw = 0;	/* set when we ask for help = need redraw */
    char buffer[SLEN], origbuffer[SLEN];
    static char helpmsg[LONG_STRING];


    if (ncf_prompt == NULL) {
	ncf_prompt = catgets(elm_msg_cat, ElmSet, ElmSaveCopyInPrompt,
			"Save copy in (use '?' for help/to list folders): ");
    }
    /* expand passed copy file name into English */
    strcpy(buffer, cf_english(fn));

    /* prepare screen with instructions */
    MoveCursor(LINES-2, 0);
    CleartoEOS();
    PutLine0(LINES-2, 0, ncf_prompt);

    while(1) {

      /* get file name from user input */
      strcpy(origbuffer, buffer);
      optionally_enter(buffer, LINES-2, strlen(ncf_prompt), FALSE, FALSE);

      if(strcmp(buffer, "?") != 0) { /* got what we wanted - non-help choice */

	if(strcmp(origbuffer, buffer) != 0)
	  /* user changed from our English expansion 
	   * so we'd better copy user input to fn
	   */
	  strcpy(fn, buffer);

	/* else user presumably left our English expansion - no change in fn */

	/* display English expansion of new user input a while */
	PutLine1(LINES-2, strlen(ncf_prompt), cf_english(fn));
	MoveCursor(LINES, 0);
	sleep(1);
	MoveCursor(LINES-2, 0);
	CleartoEOS();

	return(redraw);
      }

      /* give help and list folders */
      redraw = TRUE;
      if(!*helpmsg) {	/* help message not yet formulated */
	strcpy(helpmsg, catgets(elm_msg_cat, ElmSet, ElmListFoldersHelp1,
"Enter: <nothing> to not save a copy of the message,\n\
\r       '<'       to save in your \"sent\" folder ("));
	strcat(helpmsg, sent_mail);
	strcat(helpmsg, catgets(elm_msg_cat, ElmSet, ElmListFoldersHelp2,
"),\n\
\r       '='       to save by name (the folder name depends on whom the\n\
\r                     message is to, in the end),\n\
\r       '=?'      to save by name if the folder already exists,\n\
\r                     and if not, to your \"sent\" folder,\n\
\r       or a filename (a leading '=' denotes your folder directory).\n\r\n\r"));
      }

      list_folders(4, helpmsg, NULL);
      PutLine0(LINES-2, 0, ncf_prompt);

      /* restore as default to English version of the passed copy file name */
      strcpy(buffer, cf_english(fn));

    }
}
