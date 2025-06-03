
static char rcsid[] = "@(#)$Id: file.c,v 5.7 1992/12/07 02:38:45 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.7 $   $State: Exp $
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
 * $Log: file.c,v $
 * Revision 5.7  1992/12/07  02:38:45  syd
 * When ELM is reporting "appending to <foldername>" I never get to see
 * but a flash of light, before the message line has been replaced by a
 * dummy "Messages saved" string.  And if you happen to have mistyped the
 * foldername or the leading '=', you never see the foldername.
 *
 * This fix will change file.c to show either
 *
 * 	Message <what> to <foldername>
 *
 * or
 * 	<#> messages <what> to <foldername>
 * From: Bo.Asbjorn.Muldbak <bam@jutland.ColumbiaSC.NCR.COM>
 *
 * Revision 5.6  1992/11/26  01:46:26  syd
 * add Decode option to copy_message, convert copy_message to
 * use bit or for options.
 * From: Syd and bjoerns@stud.cs.uit.no (Bjoern Stabell)
 *
 * Revision 5.5  1992/10/30  21:01:49  syd
 * More changes to folder creation confirmation
 * From: Larry Philps <larryp@sco.COM>
 *
 * Revision 5.4  1992/10/24  13:25:41  syd
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
 * Revision 5.3  1992/10/19  16:44:00  syd
 * Make headers before pwd to allow include of stdio to
 * appear first to avoid prototype conflicts
 * From: Syd via news article by Don Watts
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

/** File I/O routines, mostly the save to file command...

**/

#include "headers.h"
#ifdef PWDINSYS
#  include <sys/pwd.h>
#else
#  include <pwd.h>
#endif

#include "s_elm.h"
#include <ctype.h>
#include <errno.h>

#ifdef BSD
#undef tolower
#endif

extern int errno;

char *strcpy(), *nameof();
unsigned long sleep();

void save_message();

static char copyhelp[VERY_LONG_STRING];
static char savehelp[VERY_LONG_STRING];
static char prev_fold[SLEN];           /* name of previous folder */

void
init_helpmsg( msg, action_word, allow_dot )
char	*msg , *action_word ;
int      allow_dot ;
{
	strcpy( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg1,
		"\n\r\n\rYou must specify a file or folder to "));

	strcat( msg, action_word );

	strcat( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg2,
		" to.  Several options\n\rare available:\n\r\
\n\r  '!'  will use your incoming mailbox ("));

	strcat( msg, defaultfile ) ;

	strcat( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg3,
		")\n\r  '>'  will use your \"received\" folder ("));

	strcat( msg, nameof( recvd_mail ) ) ;

	strcat( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg4,
		")\n\r  '<'  will use your \"sent\" folder ("));

	strcat( msg, nameof( sent_mail ) ) ;

	if( allow_dot ) {
	    strcat( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg5,
		    ")\n\r  '.' will use the previous folder ("));

	    strcat( msg, nameof(prev_fold));
	}


	strcat( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg6,
		")\n\r  '@alias' will use the default folder for \"alias\"\n\r\n\r\
If you enter a filename elm will use that file.  If the file name begins\n\r\
with a '=', elm will look for the file in your folder directory\n\r\
(your folder directory is "));

	strcat( msg, raw_folders ) ;

	strcat( msg, ").\n\r\n\r" ) ;

	strcat( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg7,
		"You may use wildcards in the name, exactly as you do in the shell.  If you\n\r\
do this, a list of all matching files or folders will be printed and you\n\r\
will be prompted for another name.  Thus typing '=a*' will print the names\n\r\
of all folders in your folder directory starting with 'a'.  Typing 'b*.c'\n\r\
will print all files in your local directory starting with 'b' and ending\n\r"));
	strcat( msg, catgets(elm_msg_cat, ElmSet, ElmFileHelpmsg8,
"in '.c'.  A star (*) will usually list everything.  See your local shell\n\r\
manual for more information on wildcards.\n\r\
\n\r\
Sometimes elm will help you out by suggesting a name.  If you want to use\n\r\
the suggested name, simply hit return.  If you don't like it, hit your\n\r\
erase or line erase keys.\n\r\
\n\r\
\n\r\
\n\r"));
}



int
save(redraw, silently, delete)
int *redraw, silently, delete;
{
	/** Save all tagged messages + current in a folder.  If no messages
	    are tagged, save the current message instead!  This routine
	    will return ZERO if the operation failed.
	    'redraw' is set to TRUE iff we use the '?' and mess up
	    the screen.  Pretty reasonable, eh?  If "silently" is set,
	    then don't output the "D" character upon marking for
	    deletion...
	    If delete is set, then delete the saved messages, else
	    we are just copying the messages without deletion.
	**/

	register int	tagged = 0, i, oldstat, appending = 0,
			is_ordinary_file, scount = 0;
	int mesgnum;	/* message whose address is used for save-by-name fn */
	char	filename[SLEN], address[LONG_STRING], buffer[LONG_STRING],
		ch;
	FILE *save_file;

	oldstat = headers[current-1]->status;	/* remember */
	*redraw = FALSE;

	for (i=0; i < message_count; i++) {
	  if (ison(headers[i]->status, TAGGED)) {
	    if(!tagged)
	      mesgnum = i;	/* first tagged msg -  use this one for
				 * save-by-name folder name */
	    tagged++;
	  }
	}

	if (tagged == 0) {
	  mesgnum = current-1;	/* use this one for save-by-name folder name */
	  tagged = 1;
	  setit(headers[current-1]->status, TAGGED);
	}

	dprint(4, (debugfile, "%d message%s tagged for saving (save)\n", tagged,
		plural(tagged)));

Retry_Save_If_Not_Confirmed:
	while (1) {

	  if (tagged == 1)
	    PutLine1(LINES-2, 0, catgets(elm_msg_cat, ElmSet, ElmSaveMessageTo,
		  "%s message to: "), (delete ? cap_save_word : cap_copy_word));
	  else
	    PutLine1(LINES-2, 0, catgets(elm_msg_cat, ElmSet, ElmSaveMessagesTo,
		  "%s messages to: "), (delete ? cap_save_word : cap_copy_word));

	  if (save_by_name) {
	    /** build default filename to save to **/
	    get_return(address, mesgnum);
	    get_return_name(address, buffer, TRUE);
	    sprintf(filename, "=%s", buffer);
	  }
	  else
	    filename[0] = '\0';

	  if (tagged > 1)
	    optionally_enter(filename, LINES-2, 19, FALSE, FALSE);
	  else	
	    optionally_enter(filename, LINES-2, 18, FALSE, FALSE);
  

	  if (strlen(filename) == 0) {  /** <return> means 'cancel', right? **/
	    headers[current-1]->status = oldstat;	/* BACK! */
	    return(0);
	  }
	 
	  if (strcmp(filename,"?") == 0) {	/* user asked for help */
	    *redraw = TRUE;	/* set the flag so we know what to do later */
	    if ( delete )
	    {
	      if ( !*savehelp )	/* format helpmsg if not yet done */
	        init_helpmsg( savehelp, save_word, TRUE ) ;
	      ClearScreen();
	      (void) printf( savehelp ) ;
	    }
	    else
	    {
	      if ( !*copyhelp )	/* format helpmsg if not yet done */
		init_helpmsg( copyhelp, copy_word, TRUE ) ;
	      ClearScreen();
	      (void) printf( copyhelp ) ;
	    }
	    continue;
	  }

	  /* was this a wildcard request? */
	  if ( has_wildcards( filename ) )
	  {
	    *redraw = TRUE ;
	    list_folders( 4, NULL, filename ) ;
	    continue ;
	  }
	  /* else - got a folder name - check it out */
	  if (! expand_filename(filename, TRUE)) {
	    dprint(2, (debugfile,
		  "Error: Failed on expansion of filename %s (save)\n", 
		  filename));
	    continue;
	  }

	  is_ordinary_file = strncmp (filename, folders, strlen(folders));
	  
	  if ((errno = can_open(filename, "a"))) {
	    error2(catgets(elm_msg_cat, ElmSet, ElmCannotSaveMessage,
		"Cannot %s message to folder %s!"),
	      delete ? save_word:copy_word, filename);
	    continue;
	  }
	  break;	/* got a valid filename */
	}

	save_file_stats(filename);

	if (access(filename,ACCESS_EXISTS)== 0) {	/* already there!! */
	    appending = 1;
	    if (confirm_append || (confirm_files && is_ordinary_file)) {
		PutLine1 (LINES-2, 0,
			  /*
			   *	Well it should be like this but...
			  catgets (elm_msg_cat, ElmSet, ElmConfirmFiles,
				   "Append to an existing file `%s'? (y/n) "),
			   */
			  is_ordinary_file
			  ? "Append to an existing file `%s'? (y/n) n"
			  : "Append to mail folder `%s'? (y/n) n",
			  filename);
		ch = ReadCh ();
		ch = tolower (ch);
		PutLine2 (LINES-2, 0,
			  is_ordinary_file
			  ? "Append to an existing file `%s'? (y/n) %s"
			  : "Append to mail folder `%s'? (y/n) %s",
			  filename,
			  (ch == *def_ans_yes) ? "Yes" : "No");
		sleep (1);
		ClearLine (LINES-2);

		if (ch != *def_ans_yes) {
		    goto Retry_Save_If_Not_Confirmed;
		}
	    }
	}
	else {
            if (confirm_create || (confirm_folders && !is_ordinary_file)) {
		PutLine1 (LINES-2, 0,
			  /*
			   *	Well it should be like this but...
			  catgets (elm_msg_cat, ElmSet, ElmConfirmFolders,
				   "Create a new mail folder `%s'? (y/n) "),
			   */
			  !is_ordinary_file
			  ? "Create a new mail folder `%s'? (y/n) n"
			  : "Create a new file `%s'? (y/n) n",
			  filename);
		ch = ReadCh ();
		ch = tolower (ch);
		PutLine2 (LINES-2, 0,
			  !is_ordinary_file
			  ? "Create a new mail folder `%s'? (y/n) %s"
			  : "Create a new file `%s'? (y/n) %s",
			  filename,
			  (ch == *def_ans_yes) ? "Yes" : "No");
		sleep (1);
		ClearLine (LINES-2);

		if (ch != *def_ans_yes) {
		    goto Retry_Save_If_Not_Confirmed;
		}
	    }
	}
	  
	dprint(4,(debugfile, "Saving mail to folder '%s'...\n", filename));

	/* save this filename for the next time */
	strncpy(prev_fold, filename, SLEN);

	if ((save_file = fopen(filename,"a")) == NULL) {
	  dprint(2, (debugfile,
		"Error: couldn't append to specified folder %s (save)\n", 
		filename));
	  error1(catgets(elm_msg_cat, ElmSet, ElmCouldntAppendFolder,
		"Couldn't append to folder %s!"), filename);
	  headers[current-1]->status = oldstat;	/* BACK! */
	  return(0); 
	}

	/* if we need a redraw that means index screen no longer present
	 * so whatever silently was, now it's true - we can't show those
	 * delete markings.
	 */
	if(*redraw) silently = TRUE;

	for (i=0; i < message_count; i++) 	/* save each tagged msg */
	  if (headers[i]->status & TAGGED) {
	    save_message(i, filename, save_file, (tagged > 1), appending++, 
			 silently, delete);
	    scount++;
	  }

	fclose(save_file);

	restore_file_stats(filename);

	if (tagged == 1)
	  error2(catgets(elm_msg_cat, ElmSet, ElmMessageSaved,
		"Message %s to %s."), delete ? saved_word: copied_word,
		filename);
	else if (tagged > 1)
	  error3(catgets(elm_msg_cat, ElmSet, ElmMessagesSaved,
		  "%d messages %s to %s."), scount, 
		  delete ? saved_word: copied_word,
		  filename);
	return(1);
}

void
save_message(number, filename, fd, pause, appending, silently, delete)
int number, pause, appending, silently, delete;
char *filename;
FILE *fd;
{
	/** Save an actual message to a folder.  This is called by 
	    "save()" only!  The parameters are the message number,
	    and the name and file descriptor of the folder to save to.
	    If 'pause' is true, a sleep(2) will be done after the
	    saved message appears on the screen...
	    'appending' is only true if the folder already exists 
	    If 'delete' is true, mark the message for deletion.
	**/

	register int save_current, is_new;
	
	dprint(4, (debugfile, "\tSaving message %d to folder...\n", number));

	save_current = current;
	current = number+1;

	/* change status from NEW before copy and reset to what it was
	 * so that copy doesn't look new, but we can preserve new status
	 * of message in this mailfile. This is important because if
	 * user does a resync, we don't want NEW status to be lost.
	 * I.e. NEW becomes UNREAD when we "really" leave a mailfile.
	 */
	if(is_new = ison(headers[number]->status, NEW))
	  clearit(headers[number]->status, NEW);
	copy_message("", fd, CM_UPDATE_STATUS);
	if(is_new)
	  setit(headers[number]->status, NEW);
	current = save_current;

	if (delete)
	  setit(headers[number]->status, DELETED); /* deleted, but ...   */
	clearit(headers[number]->status, TAGGED);  /* not tagged anymore */

	if (appending)
	  error2(catgets(elm_msg_cat, ElmSet, ElmMessageAppendedFolder,
		"Message %d appended to folder %s."), number+1, filename);
	else
	  error3(catgets(elm_msg_cat, ElmSet, ElmMessageSavedFolder,
		"Message %d %s to folder %s."),
	     number+1, delete ? saved_word : copied_word, filename);

	if (! silently)
	  show_new_status(number);	/* update screen, if needed */

	if (pause && (!silently) && (!appending))
	  sleep(2);
}

int
expand_filename(filename, use_cursor_control)
char *filename;
int use_cursor_control;
{
	/** Expands	~/	to the current user's home directory
			~user/	to the home directory of "user"
			=,+,%	to the user's folder's directory
			!	to the user's incoming mailbox
			>	to the user's received folder
			<	to the user's sent folder
			.	to the last folder used 
			@alias	to the default folder directory for "alias"
			shell variables (begun with $)

	    Side effect: strips off trailing blanks

	    Returns 	1	upon proper expansions
			0	upon failed expansions
	 **/

	char temp_filename[SLEN], varname[SLEN], env_value[SLEN],
	     logname[SLEN], *ptr, *address, *get_alias_address(),
	     buffer[LONG_STRING];
	register int iindex;
	struct passwd *pass, *getpwnam();
	char *getenv();

	ptr = filename;
	while (*ptr == ' ') ptr++;	/* leading spaces GONE! */
	strcpy(temp_filename, ptr);

	/** New stuff - make sure no illegal char as last **/
	/** Strip off any trailing backslashes or blanks **/

	ptr = temp_filename + strlen(temp_filename) - 1;
	while (*ptr == '\n' || *ptr == '\r'
	       || *ptr == '\\' || *ptr == ' ' || *ptr == '\t') {
	    *ptr-- = '\0';
	}

	if (temp_filename[0] == '~') {
	  if(temp_filename[1] == '/')
	    sprintf(filename, "%s%s%s",
		  home, (lastch(home) != '/' ? "/" : ""), &temp_filename[2]);
	  else {
	    for(ptr = &temp_filename[1], iindex = 0; *ptr && *ptr != '/'; ptr++, iindex++)
	      logname[iindex] = *ptr;
	    logname[iindex] = '\0';
	    if((pass = getpwnam(logname)) == NULL) {
	      dprint(3,(debugfile, 
		      "Error: Can't get home directory for %s (%s)\n",
		      logname, "expand_filename"));
	      if(use_cursor_control)
		error1(catgets(elm_msg_cat, ElmSet, ElmDontKnowHomeCursor,
			"Don't know what the home directory of \"%s\" is!"),
			logname);
	      else
		printf(catgets(elm_msg_cat, ElmSet, ElmDontKnowHomePrintf,
		    "\n\rDon't know what the home directory of \"%s\" is!\n\r"),
		    logname);
	      return(0);
	    }
	    sprintf(filename, "%s%s", pass->pw_dir, ptr);
	  }

	}
	else if (temp_filename[0] == '=' || temp_filename[0] == '+' || 
	 	 temp_filename[0] == '%') {
	  sprintf(filename, "%s%s%s", folders, 
		(temp_filename[1] != '/' && lastch(folders) != '/')? "/" : "",
	  	&temp_filename[1]);
	}
	else if  ( temp_filename[0] == '@' ) {

	  /* try to expand alias */

	  if ((address = get_alias_address(&(temp_filename[1]), FALSE))!=NULL) {

            if (address[0] == '!') {
	      /* Refuse group aliases? */
              address[0] = ' ';
	    }

	    /* get filename from address */
	    get_return_name(address, buffer, TRUE);

	    sprintf(filename, "%s%s%s",
	      folders, 
	      (temp_filename[1] != '/' && lastch(folders) != '/') ? "/" : "", 
	      buffer);
	  }
          else {
	    if ( use_cursor_control )
	      error1(catgets(elm_msg_cat, ElmSet, ElmCannotExpand,
		     "Cannot expand alias '%s'!\n\r"), &(temp_filename[1]) );
	    else
	      printf(catgets(elm_msg_cat, ElmSet, ElmCannotExpand,
		     "Cannot expand alias '%s'!\n\r"), &(temp_filename[1]) );
	    return(0);
	  }
	}
	else if (temp_filename[0] == '$') {	/* env variable! */
	  for(ptr = &temp_filename[1], iindex = 0; isalnum(*ptr); ptr++, iindex++)
	    varname[iindex] = *ptr;
	  varname[iindex] = '\0';

	  env_value[0] = '\0';			/* null string for strlen! */
	  if (getenv(varname) != NULL)
	    strcpy(env_value, getenv(varname));

	  if (strlen(env_value) == 0) {
	    dprint(3,(debugfile, 
		    "Error: Can't expand environment variable $%s (%s)\n",
		    varname, "expand_filename"));
	    if(use_cursor_control)
	      error1(catgets(elm_msg_cat, ElmSet, ElmDontKnowValueCursor,
		    "Don't know what the value of $%s is!"), varname);
	    else
	      printf(catgets(elm_msg_cat, ElmSet, ElmDontKnowValuePrintf,
		    "\n\rDon't know what the value of $%s is!\n\r"), varname);
	    return(0);
	  }

	  sprintf(filename, "%s%s%s", env_value, 
		(*ptr && *ptr != '/' && lastch(env_value) != '/')? "/" : "", ptr);

	} else if (strcmp(temp_filename, "!") == 0) {
	  strcpy(filename, defaultfile);
	} else if (strcmp(temp_filename, ">") == 0) {
	  strcpy(filename, recvd_mail);
	} else if (strcmp(temp_filename, "<") == 0) {
	  strcpy(filename, sent_mail);
	} else if (strcmp(temp_filename, ".") == 0) {
	  strcpy(filename, prev_fold);
	} else
	  strcpy(filename, temp_filename);
	  
	return(1);
}
