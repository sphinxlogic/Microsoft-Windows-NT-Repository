
static char rcsid[] = "@(#)$Id: alias.c,v 5.17 1992/12/13 17:59:18 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.17 $   $State: Exp $
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
 * $Log: alias.c,v $
 * Revision 5.17  1992/12/13  17:59:18  syd
 * Please write on the blackboard 500 times `NULL != 0.'
 * From: chip@chinacat.unicom.com (Chip Rosenthal)
 *
 * Revision 5.16  1992/12/11  02:09:06  syd
 * Fix where the user creates a first new alias, then deletes it, the
 * alias stays on screen, but the file really will be empty if it was the
 * last alias, so the retry to delete gives 'cannot open ...file' messages
 * From: "Robert L. Howard" <robert.howard@matd.gatech.edu>
 *
 * Revision 5.15  1992/12/11  01:58:22  syd
 * Anytime elm wants to re-run newalias, selected is set to 0.
 * (removing any limit in effect)
 * From: "Robert L. Howard" <robert.howard@matd.gatech.edu>
 *
 * Revision 5.14  1992/12/11  01:45:04  syd
 * remove sys/types.h include, it is now included by defs.h
 * and this routine includes defs.h or indirectly includes defs.h
 * From: Syd
 *
 * Revision 5.13  1992/12/07  03:02:03  syd
 * On machines with 64 bit pointers (and 64 bit longs) using int
 * for newmax causes pointer truncation.
 * From: Jim Brown
 *
 * Revision 5.12  1992/11/26  00:46:50  syd
 * Fix how errno is used so err is inited and used instead
 * as errno gets overwritten by print system call
 * From: Syd
 *
 * Revision 5.11  1992/11/15  01:24:34  syd
 * The situation is that the .elm/aliases file is missing, but
 * .elm/aliases.dir and .elm/aliases.pag exist (isn't serendipity
 * wonderful?).  The ndbz functions tolerate this and just put a NULL
 * pointer in the db structure for the data file FILE pointer.  However,
 * get_one_alias() in listalias and elm doesn't account for the db_open()
 * succeeding but the dbz_basef field being NULL, so it passes the NULL
 * pointer to fread().  Detect null and return 0
 * From: dwolfe@pffft.sps.mot.com (Dave Wolfe)
 *
 * Revision 5.10  1992/11/15  01:15:28  syd
 * The alias message_count isn't set to zero if the last alias has
 * been deleted from the alias table. As no aliases are reread from
 * the aliases database the message_count is left as it was before.
 *
 * Fixed that the function do_newalias() sometimes returns without freeing
 * the buffer allocated before. The patch adds these free calls.
 *
 * When you erroneously type a number in your folder elm asks you for
 * a new current message number. But now if you erase this one number
 * and leave the string empty elm will set the new current message to
 * the second message on our sun4! The patch adds a check for an empty
 * string and returns the current number if no number was entered.
 * From: vogt@isa.de (Gerald Vogt)
 *
 * Revision 5.9  1992/10/24  13:35:39  syd
 * changes found by using codecenter on Elm 2.4.3
 * From: Graham Hudspith <gwh@inmos.co.uk>
 *
 * Revision 5.8  1992/10/19  16:58:18  syd
 * more on the update of compiler warnings
 * From: Syd
 *
 * Revision 5.7  1992/10/19  16:50:41  syd
 * Fix a couple more compiler gripes from SYSVR4
 * From: Tom Moore <tmoore@wnas.DaytonOH.NCR.COM>
 *
 * Revision 5.6  1992/10/17  22:47:09  syd
 * adds the function bytemap() and the macros MAPIN and MAPOUT from the file
 * lib/ndbz.c in the file src/alias.c.
 *
 * prevent elm from exiting when resyncing the empty incoming mailbox.
 * From: vogt@isa.de (Gerald Vogt)
 *
 * Revision 5.5  1992/10/11  01:46:35  syd
 * change dbm name to dbz to avoid conflicts with partial call
 * ins from shared librarys, and from mixing code with yp code.
 * From: Syd via prompt from Jess Anderson
 *
 * Revision 5.4  1992/10/11  01:21:17  syd
 * 1. If firstname && lastname is null then copy aliasname into the
 * personal name field (inside the ()'s) when creating an alias
 * from the menu using the 'n' command.
 *
 * 2. Now if for some reason and alias has a null personal name field
 * (the person hand edited aliases.text) the blank () is not printed
 * as part of the address.  This actually cured another problem, where
 * the To: field on the screen (when you hit 'm' on the alias menu)
 * used to be blank, now the address shows up....
 * From: "Robert L. Howard" <robert.howard@matd.gatech.edu>
 *
 * Revision 5.3  1992/10/11  01:07:52  syd
 * get_return() assumes that message_count reflects the number of
 * messages in the current folder, but the message_count it's seeing
 * from the alias subsystem is actually the *alias* count.
 * toggle the main state before and after calling get_return().
 * From: cliff@sfn.ORG (R. Cliff Young)
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

/** This file contains alias stuff

**/

#include "headers.h"
#include "s_elm.h"
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include "s_aliases.h"
#include "ndbz.h"

#ifdef BSD
#undef        tolower
#endif

#define	ECHOIT	1 	/* echo on for prompting */

/*
 * A simple macro to make it easier to remember how to do a simple
 * resync and not screw up whether or not to prompt on deletions.
 */

#define resync_aliases(newaliases)	delete_aliases(newaliases,TRUE)

extern char *alias_type(), *get_alias_address();
char *error_description(), *get_parens();
void get_realnames();

int  is_system=0;		/* system file updating?     */

extern int errno;

int current_mail_message;
int num_duplicates;
DBZ *system_hash = NULL, *user_hash = NULL;
char *a_rev_alias_pad, *a_rev_alias_abr, *a_rev_alias_name,
	*a_rev_full_pad, *a_full_abr, *a_rev_full_name,
	*a_rev_text_pad, *a_text_abr, *a_rev_text_file,
	*a_alias_pad, *a_alias_abr, *a_alias_name,
	*a_full_pad, *a_full_name,
	*a_text_pad, *a_text_file,
	*a_group_name, *a_person_name, *a_system_flag;
	
int
ok_alias_name(name)
char *name;
{
	while ( *name != '\0' && ok_alias_char(*name) )
	  ++name;
	return ( *name == '\0' );
}


open_alias_files()
{
	static int first_time = TRUE;
/*
 *	Close and re-open the system and user alias files, if present,
 *	and if they have changed since last we opened them.
 *
 *	Also, parse the data files into memory if needed
 */
	if (first_time) {
	    a_group_name = catgets(elm_msg_cat, AliasesSet, AliasesGroup,
		" Group");
	    a_person_name = catgets(elm_msg_cat, AliasesSet, AliasesPerson,
		"Person");
	    a_system_flag = catgets(elm_msg_cat, AliasesSet, AliasesSystemFlag,
		"(S)");
	    a_rev_alias_pad = catgets(elm_msg_cat, AliasesSet, AliasesRevAliasPad,
		"Reverse Alias Name      ");
	    a_rev_alias_abr = catgets(elm_msg_cat, AliasesSet, AliasesRevAliasAbr,
		"Reverse-Alias");
	    a_rev_alias_name = catgets(elm_msg_cat, AliasesSet, AliasesRevAliasName,
		"Reverse Alias Name");
	    a_rev_full_pad = catgets(elm_msg_cat, AliasesSet, AliasesRevFullPad,
		"Reverse Full (Real) Name");
	    a_full_abr = catgets(elm_msg_cat, AliasesSet, AliasesRevFullAbr,
		"Reverse-Name");
	    a_rev_full_name = catgets(elm_msg_cat, AliasesSet, AliasesRevFullName,
		"Reverse Full (Real) Name");
	    a_rev_text_pad = catgets(elm_msg_cat, AliasesSet, AliasesRevTextPad,
		"Reverse Text File       ");
	    a_text_abr = catgets(elm_msg_cat, AliasesSet, AliasesRevTextAbr,
		"Reverse-Text");
	    a_rev_text_file = catgets(elm_msg_cat, AliasesSet, AliasesRevTextFile,
		"Reverse Text File");
	    a_alias_pad = catgets(elm_msg_cat, AliasesSet, AliasesAliasPad,
		"Alias Name              ");
	    a_alias_abr = catgets(elm_msg_cat, AliasesSet, AliasesAliasAbr,
		"Alias");
	    a_alias_name = catgets(elm_msg_cat, AliasesSet, AliasesAliasName,
		"Alias Name");
	    a_full_pad = catgets(elm_msg_cat, AliasesSet, AliasesFullPad,
		"Full (Real) Name        ");
	    a_full_abr = catgets(elm_msg_cat, AliasesSet, AliasesFullAbr,
		"Name");
	    a_full_name = catgets(elm_msg_cat, AliasesSet, AliasesFullName,
		"Full (Real) Name");
	    a_text_pad = catgets(elm_msg_cat, AliasesSet, AliasesTextPad,
		"Text File               ");
	    a_text_abr = catgets(elm_msg_cat, AliasesSet, AliasesTextAbr,
		"Text");
	    a_text_file = catgets(elm_msg_cat, AliasesSet, AliasesTextFile,
		"Text File");
	    first_time = FALSE;
	}

	if(open_system_aliases() | open_user_aliases()) {
	    dprint(5, (debugfile,
		      "Reading alias data files...\n"));
	    get_aliases();
	}
}

int
open_system_aliases()
{
/*
 *	Open the system alias file, if present,
 *	and if it has changed since last we read it.
 *
 *	Return 0 if hash file wasn't opened, otherwise 1
 */

	struct stat hst;
	static time_t system_ctime = 0, system_mtime = 0;

	/* If file hasn't changed, don't bother re-opening. */

	if (stat(system_data_file, &hst) == 0) {	/* File exists */
	    if (hst.st_ctime == system_ctime &&
	        hst.st_mtime == system_mtime) {		/* No changes */
	        return(0);
	    }

	   /*
	    * Re-open system hash table.  If we can't, just return.
	    */

	    if (system_hash != NULL)
	        dbz_close(system_hash);

	    if ((system_hash = dbz_open(system_data_file, O_RDONLY, 0)) == NULL)
	        return(0);

	    /* Remember hash file times. */

	    system_ctime = hst.st_ctime;
	    system_mtime = hst.st_mtime;

            return(1);
	}
	else {					/* File does not exist */
	    if (system_ctime == 0 && system_mtime == 0) {
	        return(0);			/* File never existed */
	    }
	    else {			/* Once existed, better re-read */

	       /*
	        * Since we no longer exist, we pretend we never existed.
	        */

	        system_ctime = 0;
	        system_mtime = 0;

	        return(1);
	    }
	}

}

int
open_user_aliases()
{
/*
 *	Open the user alias file, if present,
 *	and if it has changed since last we read it.
 *
 *	Return 0 if hash file wasn't opened, otherwise 1
 */

	struct stat hst;
	char fname[SLEN];
	static time_t user_ctime = 0, user_mtime = 0;

	/* If hash file hasn't changed, don't bother re-reading. */

	sprintf(fname, "%s/%s", home, ALIAS_DATA);

	if (stat(fname, &hst) == 0) {			/* File exists */
	    if (hst.st_ctime == user_ctime &&
	        hst.st_mtime == user_mtime) {		/* No changes */
	        return(0);
	    }

	   /*
	    * Open user hash table.  If we can't, just return.
	    */

	    if (user_hash != NULL)
	        dbz_close(user_hash);

	    if ((user_hash = dbz_open(fname, O_RDONLY, 0)) == NULL)
	        return(0);

	    /* Remember hash file times. */

	    user_ctime = hst.st_ctime;
	    user_mtime = hst.st_mtime;

            return(1);
	}
	else {					/* File does not exist */
	    if (user_ctime == 0 && user_mtime == 0) {
	        return(0);			/* File never existed */
	    }
	    else {			/* Once existed, better re-read */

	       /*
	        * Since we no longer exist, we pretend we never existed.
	        */

	        user_ctime = 0;
	        user_mtime = 0;

	        return(1);
	    }
	}

}

int
add_alias()
{
/*
 *	Add an alias to the user alias text file.  If there
 *	are aliases tagged, the user is asked if he wants to
 *	create a group alias from the tagged files.
 *
 *	Return zero if alias not added in actuality.
 */

	int i, leftoff, tagged = 0;

	char aliasname[SLEN], firstname[SLEN], lastname[SLEN];
	char address1[LONG_STRING], buffer[SLEN];
	char comment[LONG_STRING], ch;

/*
 *	See if there are any tagged aliases.
 */
	for (i=0; i < message_count; i++) {
	    if (ison(aliases[i]->status, TAGGED)) {
		if (tagged == 0) leftoff = i;
	        tagged++;
	    }
	}

	if (tagged == 1) {
	 /*
	  * There is only on alias tagged.  Ask the question
	  * but the default response is NO.
	  */
	    PutLine0(LINES-2,0, catgets(elm_msg_cat,
	            AliasesSet, AliasesOneTagged,
	            "There is 1 alias tagged..."));
	    CleartoEOLN();
	    MCsprintf(buffer, catgets(elm_msg_cat,
	            AliasesSet, AliasesCreateGroup,
	            "Create group alias? (%c/%c) "),
	        *def_ans_yes, *def_ans_no);
	    ch = want_to(buffer, *def_ans_no);
	}
	if (tagged > 1) {
	 /*
	  * If multiple tagged aliases then we assume the user
	  * wants to create a group alias.  The default response
	  * is YES.
	  */
	    PutLine1(LINES-2,0, catgets(elm_msg_cat,
	            AliasesSet, AliasesManyTagged,
	            "There are %d aliases tagged..."), tagged);
	    CleartoEOLN();
	    MCsprintf(buffer, catgets(elm_msg_cat,
	            AliasesSet, AliasesCreateGroup,
	            "Create group alias? (%c/%c) "),
	        *def_ans_yes, *def_ans_no);
	    ch = want_to(buffer, *def_ans_yes);
	}

/*
 *	Create the group alias.  This is (hopefully) only
 *	done if one of the above want_to() questions were
 *	answered YES (and thus there *were* tagged messages
 *	and the user responded correctly).
 */
	if (ch == *def_ans_yes) {
	    strcpy(address1, aliases[leftoff]->alias);
	    clearit(aliases[leftoff]->status, TAGGED);
	    show_msg_tag(leftoff);
	    for (i=leftoff+1; i < message_count; i++) {
	        if (ison(aliases[i]->status, TAGGED)) {
	            strcat(address1, ",");
	            strcat(address1, aliases[i]->alias);
	            clearit(aliases[i]->status, TAGGED);
	            show_msg_tag(i);
	        }
	    }
	}

	strcpy(buffer, catgets(elm_msg_cat,
	        AliasesSet, AliasesEnterAliasName, "Enter alias name: "));
	PutLine0(LINES-2,0, buffer);
	CleartoEOLN();
	*aliasname = '\0';
	if (get_aliasname(aliasname, buffer) != 0) {
	    dprint(3, (debugfile, 
	        "Aliasname [%s] was rejected in add_alias\n", aliasname));
	    ClearLine(LINES-2);
	    return(0);
	}

	*lastname = '\0';
	*firstname = '\0';
	*comment = '\0';
	get_realnames(aliasname, firstname, lastname, comment, buffer);

	if ((tagged == 0) || (ch == *def_ans_no)) {
	    sprintf(buffer, catgets(elm_msg_cat,
	            AliasesSet, AliasesEnterAddress,
	            "Enter address for %s: "), aliasname);
	    PutLine0(LINES-2,0, buffer);
	    CleartoEOLN();
	    *address1 = '\0';
	    optionally_enter(address1, LINES-2, strlen(buffer), FALSE, FALSE);
	    Raw(ON);
	    if (strlen(address1) == 0) {
	        error(catgets(elm_msg_cat, AliasesSet, AliasesNoAddressSpec,
	                "No address specified!"));
	        return(0);
	    }
	}

	return(ask_accept(aliasname, firstname, lastname, comment, address1,
	        buffer));

}

int
add_current_alias()
{
/*
 *	Alias the current message to the specified name and
 *	add it to the alias text file, for processing as
 *	the user leaves the program.
 *
 *	Returns non-zero iff alias actually added to file.
 */

	char aliasname[SLEN], firstname[SLEN], lastname[SLEN];
	char comment[SLEN], address1[LONG_STRING], buffer[SLEN];
	char comment_buff[LONG_STRING];
	char *chspace, *bufptr;
	struct header_rec *current_header;

	static char bad_punc[] = ",.:;";
	char *punc_ptr;
	int i, match;

	if (current_mail_message == 0) {
	 dprint(4, (debugfile, 
		"Add current alias called without any current message!\n"));
	 error(catgets(elm_msg_cat, AliasesSet, AliasesNoMessage,
		"No message to alias to!"));
	 return(0);
	}
	current_header = headers[current_mail_message - 1];
	
	strcpy(buffer, catgets(elm_msg_cat, AliasesSet, AliasesCurrentMessage,
		"Current message address aliased to: "));
	PutLine0(LINES-2,0, buffer);
	CleartoEOLN();
	*aliasname = '\0';
	if (get_aliasname(aliasname, buffer) != 0) {
	    dprint(3, (debugfile, 
	        "Aliasname [%s] was rejected in add_current_alias\n",
	        aliasname));
	    return(0);
	}

	/* use full name in current message for default comment */
	tail_of(current_header->from, comment_buff, current_header->to);
	if(index(comment_buff, (int)'!') || index(comment_buff, (int)'@'))
	  /* never mind - it's an address not a full name */
	  *comment_buff = '\0';

/*
 *	Try to break up the From: comment into firstname, lastname, and
 *	any other text.  This is based on the fact that many address
 *	comments are pretty straightforward.  This will break on many
 *	situations.  Should handle:
 *		(Robert Howard)
 *		(Robert L. Howard)
 *		(Robert Howard, Georgia Tech)
 *	pretty well.  Will break on:
 *		(The Voice of Reason)
 *		and others....
 */

	*firstname = '\0';
	*lastname = '\0';
	*comment = '\0';
	if (strlen(comment_buff) != 0) {	/* There is something. */
	    bufptr = comment_buff;
	    while (*bufptr == SPACE) bufptr++;	/* Always strip leading WS */
	    if ((chspace = index(bufptr, (int) SPACE)) != NULL) {
	   /*
	    *   A space means that there is at least (firstname lastname)
	    *   Get firstname and move bufptr.
	    */
	        *chspace = '\0';
	        strcpy(firstname, bufptr);
	        bufptr = chspace + 1;			/* Move the pointer */
	        while (*bufptr == SPACE) bufptr++;
	    }

above:	    if ((chspace = index(bufptr, (int) SPACE)) != NULL) {
	   /*
	    *   Another space means a third+ word.  We either have:
	    *       1. Word 3+ is a comment, or
	    *       2. Word 2 is a middle initial (word 3 is lastname).
	    *   Check and see.
	    */
	        *chspace = '\0';
	        if ((strlen(bufptr) == 1) ||
	            (strlen(bufptr) == 2  && *(bufptr+1) == '.')) {
	       /*
	        *   If the second word is either a single
	        *   character or a character followed by '.' it was
	        *   probably a middle initial.  Add it to firstname
	        *   and shift.
	        */
	            strcat(firstname, " ");
	            strcat(firstname, bufptr);
	            bufptr = chspace + 1;		/* Move the pointer */
	            while (*bufptr == SPACE) bufptr++;
	            goto above;
	        }
	        strcpy(lastname, bufptr);
	        bufptr = chspace + 1;			/* Move the pointer */
	        while (*bufptr == SPACE) bufptr++;
	        strcpy(comment, bufptr);
	    }
	    else {
	   /*
	    *   Only a lastname left.
	    */
	        strcpy(lastname, bufptr);
	    }

	/*
	 *  Finally, get any puctuation characters off the end of
	 *  lastname.
	 */
	    match = TRUE;
	    for (i = strlen(lastname) - 1; match && i>0; i--) {
	        match = FALSE;
	        for (punc_ptr = bad_punc; *punc_ptr != '\0'; punc_ptr++) {
	            if (lastname[i] == *punc_ptr) {
	                lastname[i] = '\0';
	                match = TRUE;
	                break;
	            }
	        }
	    }
	}

	get_realnames(aliasname, firstname, lastname, comment, buffer);

	/* grab the return address of this message */
	main_state(); /* toggle main state so that message_count is right */
	get_return(address1, current_mail_message-1);
	main_state(); /* toggle main state back to alias mode */
	strcpy(address1, strip_parens(address1));	/* remove parens! */

	return(ask_accept(aliasname, firstname, lastname, comment, address1,
	        buffer));

}

add_to_alias_text(aliasname, firstname, lastname, comment, address)
char *aliasname, *firstname, *lastname, *comment, *address;
{
/*
 *	Add the data to the user alias text file.
 *
 *	Return zero if we succeeded, 1 if not.
 */
	
	FILE *file;
	char fname[SLEN];
	char buffer[SLEN];
	int  err;
	
	sprintf(fname,"%s/%s", home, ALIAS_TEXT);
	
	save_file_stats(fname);
	if ((file = fopen(fname, "a")) == NULL) {
	  err = errno;
	  dprint(2, (debugfile, 
		 "Failure attempting to add alias to file %s within %s",
		   fname, "add_to_alias_text"));
	  dprint(2, (debugfile, "** %s **\n", error_description(err)));
	  error1(catgets(elm_msg_cat, AliasesSet, AliasesCouldntOpenAdd,
		 "Couldn't open %s to add new alias!"), fname);
	  return(1);
	}

	if (strlen(firstname) == 0) {
	    strcpy(buffer, lastname);  
	}
	else {
	    sprintf(buffer, "%s; %s", lastname, firstname);
	}
	if (strlen(comment) != 0) {
	    strcat(buffer, ", ");
	    strcat(buffer, comment);
	}
	if (fprintf(file,"%s = %s = %s\n", aliasname, buffer, address) == EOF) {
	    err = errno;
	    dprint(2, (debugfile,
		       "Failure attempting to write alias to file within %s",
		       fname, "add_to_alias_text"));
	    dprint(2, (debugfile, "** %s **\n", error_description(err)));
	    error1(catgets(elm_msg_cat, AliasesSet, AliasesCouldntWrite,
		   "Couldn't write alias to file %s!"), fname);
	    fclose(file);
	    return(1);
	}

	fclose(file);

	restore_file_stats(fname);

	return(0);
}

delete_from_alias_text(name, num_to_delete)
char **name;
int num_to_delete;
{
/*
 *	Delete the data from the user alias text file.
 *
 *	Return zero if we succeeded, 1 if not.
 */
	
	FILE *file, *tmp_file;

	char fname[SLEN], tmpfname[SLEN];
	char line_in_file[LONG_STRING];
	char rest_of_line[LONG_STRING];
	char *s, *rest;

	register int i;
	int num_aliases;
	int delete_continues;
	int err;

	delete_continues = FALSE;

	for (i=0; i < num_to_delete; i++)
	  strcat(name[i], ","); 

	sprintf(fname,"%s/%s", home, ALIAS_TEXT);
	sprintf(tmpfname,"%s/%s.t", home, ALIAS_TEXT);
	
	save_file_stats(fname);

	if ((file = fopen(fname, "r")) == NULL) {
	  err = errno;
	  dprint(2, (debugfile, 
		 "Failure attempting to delete alias from file %s within %s",
		   fname, "delete_from_alias_text"));
	  dprint(2, (debugfile, "** %s **\n", error_description(err)));
	  error1(catgets(elm_msg_cat, AliasesSet, AliasesCouldntOpenDelete,
		 "Couldn't open %s to delete alias!"), fname);
	  return(1);
	}

	if ((tmp_file = fopen(tmpfname, "w")) == NULL) {
	  err = errno;
	  dprint(2, (debugfile, 
		 "Failure attempting to open temp file %s within %s",
		   tmpfname, "delete_from_alias_text"));
	  dprint(2, (debugfile, "** %s **\n", error_description(err)));
	  error1(catgets(elm_msg_cat, AliasesSet, AliasesCouldntOpenTemp,
	  	 "Couldn't open tempfile %s to delete alias!"), tmpfname);
	  return(1);
	}

	while (mail_gets(line_in_file, sizeof(line_in_file), file) != 0)
	{
	  if (! whitespace(line_in_file[0])) {
	    delete_continues = FALSE;
	    if (line_in_file[0] != '#') {
	      if (num_aliases = parse_aliases(line_in_file, rest_of_line)) {
	        for (i=0; i < num_to_delete && num_aliases; i++) {
	          if ((s = strstr(line_in_file, name[i])) != NULL) {
/*
 *	Collapse the to be deleted alias out of line_in_file
 */
	            rest = index(s, (int)',');
	            for (++rest; *rest; rest++)
	              *s++ = *rest;
	            *s = '\0';
	            num_aliases--;
	          }
	        }
	        if (num_aliases) {
	          *(line_in_file + strlen(line_in_file) - 1) = ' ';
	          strcat(line_in_file, rest_of_line);
	        }
	        else {
	          delete_continues = TRUE;
	        }
	      }
	    }
	  }
	  if (! delete_continues) {
	    if (fprintf(tmp_file,"%s", line_in_file) == EOF) {
	      err = errno;
	      dprint(2, (debugfile,
		"Failure attempting to write to temp file %s within %s",
		tmpfname, "delete_from_alias_text"));
	      dprint(2, (debugfile, "** %s **\n", error_description(err)));
	      error1(catgets(elm_msg_cat, AliasesSet, AliasesCouldntWriteTemp,
		"Couldn't write to tempfile %s!"), tmpfname);
	      fclose(file);
	      fclose(tmp_file);
	      unlink(tmpfname);
	      return(1);
	    }
	  }
	}
	fclose(file);
	fclose(tmp_file);
	if (rename(tmpfname, fname) != 0)
	{
		error1(catgets(elm_msg_cat, AliasesSet, AliasesCouldntRenameTemp,
			"Couldn't rename tempfile %s after deleting alias!"), tmpfname);
		return(1);
	}

	restore_file_stats(fname);

	return(0);
}

alias()
{
/*
 *	Work with alias commands...
 */

	char name[NLEN], *address, ch, buffer[SLEN];
	char *commap;
	int  key_offset;        /** Position offset within keyboard string   **/
	static int  newaliases = 0;
	int  i, j;

/*
 *	We're going to try to match the way elm does it at
 * 	he main menu.  I probably won't be able to use any
 *	main menu routines, but I will "borrow" from them. RLH
 */

	main_state();		/* Save globals for return to main menu */

	open_alias_files();	/* First, read the alias files. RLH */

	alias_screen();
	define_softkeys(ALIAS);

	while (1) {

#ifdef SIGWINCH
	  if (resize_screen) {
	    int	newLINES, newCOLUMNS;

	    ScreenSize(&newLINES, &newCOLUMNS);
	    resize_screen = 0;
	    if (newLINES != LINES || newCOLUMNS != COLUMNS) {
	      LINES = newLINES, COLUMNS = newCOLUMNS;
#define max(a,b)	       ((a) < (b) ? (b) : (a))
	      if (mini_menu)
		headers_per_page = max (LINES - 13, 1);
	      else
		headers_per_page = max (LINES -	 8, 1);	  /* 5 more headers! */
#undef max
	      redraw++;
	    }
	  }
	  else redraw = 0;
#else
    	  redraw = 0;
#endif
	  nucurr = 0;
	  nufoot = 0;

	  prompt(Prompt);
	  CleartoEOLN();
	  ch = ReadCh();
	  MoveCursor(LINES-3,strlen(Prompt)); CleartoEOS();
	  
	  dprint(3, (debugfile, "\n-- Alias command: %c\n\n", ch));

	  switch (ch) {
	    case '?': redraw += alias_help();			break;

	    case '$': PutLine0(LINES-3, strlen(Prompt),
			     catgets(elm_msg_cat, AliasesSet, AliasesResync,
					"Resynchronize aliases..."));
	           /*
	            * Process deletions and then see if we need to
	            * re-run the "newalias" routine.
	            */
		      if (resync_aliases(newaliases)) {
		        install_aliases();
	                newaliases = 0;
		        redraw++;
		      }
		      break;

	    case 'a': PutLine0(LINES-3, strlen(Prompt),
			     catgets(elm_msg_cat, AliasesSet, AliasesAddCurrent,
					"Add address from current message..."));
		      clear_error();
		      newaliases += add_current_alias();	break;

	    case 'e': PutLine1(LINES-3, strlen(Prompt),
	                  catgets(elm_msg_cat, AliasesSet, AliasesEdit,
	                      "Edit %s..."), ALIAS_TEXT);
	           /*
	            * Process aliases.text for deletions, etc.  You
	            * have to do this *before* checking current because
	            * all aliases could be marked for deletion.
	            */
	              (void) resync_aliases(newaliases);
	              if (current > 0) {
	                  edit_aliases_text();
	                  if (cursor_control) {
	                      transmit_functions(ON);	/* insurance */
	                  }
	                  newaliases = 0;
		          redraw++;
	              }
	              else {
	             /*
	              *   Here we don't allow editing when there are no
	              *   aliases (and thus no aliases.text file).  I
	              *   suppose we could create one from scratch but
	              *   you just can't do everything...
	              */
	                  error(catgets(elm_msg_cat, AliasesSet,
	                          AliasesNoneToEdit, "No aliases to edit!"));
	                  fflush(stdin);
	              }
		      break;

	    case 'm':
	              if (current > 0) {
	                  PutLine0(LINES-3, strlen(Prompt),
	                      catgets(elm_msg_cat, AliasesSet, AliasesMail,
	                          "Mail..."));
	                  redraw += a_sendmsg(TRUE,allow_forms);
	              }
	              else {
		          error(catgets(elm_msg_cat,
	                          AliasesSet, AliasesNoneToMail,
				  "Warning: no aliases to send mail to!"));
	              }
		      break;

	    case 'n': PutLine0(LINES-3, strlen(Prompt),
			     catgets(elm_msg_cat, AliasesSet, AliasesAddNew,
					     "Add a new alias to database..."));
		      clear_error();
		      newaliases += add_alias(); 		break;

	    case 'q':
	    case 'Q':
	    case 'i':
	    case 'r':
	    case 'R': PutLine0(LINES-3, strlen(Prompt),
			     catgets(elm_msg_cat, AliasesSet, AliasesAddReturn,
					"Return to main menu..."));
	           /*
	            * leaving the alias system.  Must check for
	            * pending deletes, etc.  prompt is set to FALSE
	            * on uppercase letters so that deletions are
	            * NOT queried.
	            */
	              if (delete_aliases(newaliases, islower(ch))) {
	                install_aliases();
	                newaliases = 0;
	              }
		      clear_error();
		      main_state();		/* Done with aliases. */
		      return;

	    case RETURN:
	    case LINE_FEED:
	    case SPACE:
	    case 'v':
		      if (newaliases) {		/* Need this ?? */
		          error(catgets(elm_msg_cat,
	                          AliasesSet, AliasesNotInstalled,
				  "Warning: new aliases not installed yet!"));
	              }

	              if (current > 0) {
	                  if (aliases[current-1]->type & GROUP) {
	                      PutLine1(LINES-1, 0, catgets(elm_msg_cat,
	                              AliasesSet, AliasesGroupAlias,
				      "Group alias: %-60.60s"),
	                          aliases[current-1]->address);
		          }
		          else {
	                      PutLine1(LINES-1, 0, catgets(elm_msg_cat,
	                              AliasesSet, AliasesAliasedAddress,
				      "Aliased address: %-60.60s"), 
	                          aliases[current-1]->address);
		          }
		      }
	              else {
		          error(catgets(elm_msg_cat,
	                          AliasesSet, AliasesNoneToView,
				  "Warning: no aliases to view!"));
		      }
		      break;

	    case 'x':
	    case 'X': PutLine0(LINES-3, strlen(Prompt),
			     catgets(elm_msg_cat, AliasesSet, AliasesAddReturn,
					"Return to main menu..."));
	              exit_alias();
		      clear_error();
		      main_state();		/* Done with aliases. */
		      return;

	    case 'f':
	    case 'F':
	              if (current > 0) {
		          strcpy(name, aliases[current-1]->alias);
		          if (ch == 'F') {
		              strcpy(buffer, catgets(elm_msg_cat,
	                              AliasesSet, AliasesFullyExpanded,
				      "Fully expand alias: "));
		              PutLine0(LINES-2,0, buffer);
		              CleartoEOS();
		              optionally_enter(name, LINES-2, strlen(buffer),
			          FALSE, FALSE);
		          }
		          address = get_alias_address(name, TRUE);
		          if (address != NULL) {
		              while (TRUE) {
	                          ClearScreen();
			          PutLine1(2,0, catgets(elm_msg_cat,
	                                  AliasesSet, AliasesAliasedFull,
					  "Aliased address for:\t%s\n\r"),
	                              name);
		                  i = 4;
		                  while (i < LINES-2) {
		                      if ((commap = index(address, (int)','))
	                                          == NULL) {
		                          PutLine0(i, 4, address);
		                          break;
		                      }
		                      *commap = '\0';
		                      PutLine0(i++, 4, address);
		                      address = commap+2;
		                  }
	                          PutLine0(LINES-1, 0, catgets(elm_msg_cat,
	                                  AliasesSet, AliasesPressReturn,
					  "Press <return> to continue."));
			          (void) getchar();
		                  if (commap == NULL) {
			              redraw++;
		                      break;
		                  }
		              }
		          }
	                  else {
			      error(catgets(elm_msg_cat,
	                              AliasesSet, AliasesNotFound,
				      "Not found."));
		          }
		      }
	              else {
		          error(catgets(elm_msg_cat,
	                          AliasesSet, AliasesNoneToView,
				  "Warning: no aliases to view!"));
		      }
		      break;

	 /*
	  * None of the menu specific commands were chosen, therefore
	  * it must be a "motion" command (or an error).
	  */
	    default	: motion(ch);

	  }

	  if (redraw)			/* Redraw screen if necessary */
	    alias_screen();

	  check_range();

	  if (nucurr == NEW_PAGE)
	    show_headers();
	  else if (nucurr == SAME_PAGE)
	    show_current();
	  else if (nufoot) {
	    if (mini_menu) {
	      MoveCursor(LINES-7, 0);  
              CleartoEOS();
	      show_alias_menu();
	    }
	    else {
	      MoveCursor(LINES-4, 0);
	      CleartoEOS();
	    }
	    show_last_error();	/* for those operations that have to
				 * clear the footer except for a message.
				 */
	  }
	}			/* BIG while loop... */
}

install_aliases()
{
/*
 *	Run the 'newalias' program and update the
 *	aliases before going back to the main program! 
 *
 *	No return value.....
 */

	int na;
	char itextfile[SLEN], odatafile[SLEN];
	char buffer[SLEN];


	error(catgets(elm_msg_cat, AliasesSet, AliasesUpdating,
		"Updating aliases..."));
	sleep(2);

	sprintf(itextfile, "%s/%s", home, ALIAS_TEXT);
	sprintf(odatafile, "%s/%s", home, ALIAS_DATA);

/*
 *	We need to unlimit everything since aliases are 
 * 	eing read in from scratch.
 */
	selected = 0;

	na = do_newalias(itextfile, odatafile, TRUE, FALSE);
	if (na >= 0) {
	    error1(catgets(elm_msg_cat, AliasesSet, AliasesReReading,
		  "Processed %d aliases.  Re-reading the database..."), na);
	    sleep(2);
	    open_alias_files();
	    set_error(catgets(elm_msg_cat, AliasesSet, AliasesUpdatedOK,
		  "Aliases updated successfully."));
	}
	else {
	/*
	 *  All we need to do is sleep so the user can read the
	 *  message that was put to the screen by the do_newalias()
	 *  call.
	 */
	    message_count = 0;
	    sleep(2);
	}
}

alias_help()
{
/*
 *	Help section for the alias menu...
 *
 *	Return non-0 if main part of screen overwritten, else 0
 */
	
	char ch;
	int  redraw=0;
	char *alias_prompt;
	
	
	if (mini_menu)
		alias_prompt = catgets(elm_msg_cat, AliasesSet, AliasesShortKey,
			"Key: ");
	else
		alias_prompt = catgets(elm_msg_cat, AliasesSet, AliasesLongKey,
			"Key you want help for: ");

	MoveCursor(LINES-3, 0);	CleartoEOS();

	if (mini_menu) {
	  Centerline(LINES-3, catgets(elm_msg_cat, AliasesSet, AliasesKeyMenu,
 "Press the key you want help for, '?' for a key list, or '.' to exit help"));
	}

	lower_prompt(alias_prompt);

	while ((ch = ReadCh()) != '.') {
	  ch = tolower(ch);
	  switch(ch) {
	    case '?' : display_helpfile(ALIAS_HELP);	
		       redraw++;
		       return(redraw);

	    case '$': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpDollar,
"$ = Force resynchronization of aliases, processing additions and deletions."));
		      break;

	    case '/': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpSlash,
			"/ = Search for specified name or alias in list."));
		      break;

	    case RETURN:
	    case LINE_FEED:
	    case SPACE:
	    case 'v': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpv,
	    "v = View the address for the currently selected alias."));
		      break;
	
	    case 'a': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpa,
	    "a = Add (return) address of current message to alias database."));
		      break;

	    case 'd': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpd,
	    "d = Mark the current alias for deletion from alias database."));
		      break;

	    case ctrl('D'): error(catgets(elm_msg_cat, AliasesSet, AliasesHelpCtrlD,
	    "^D = Mark for deletion user aliases matching specified pattern."));
		      break;

	    case 'e': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpe,
	    "e = Edit the alias text file directly (will run newalias)."));
		      break;

	    case 'f': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpf,
		"f = Display fully expanded address of current alias."));
		      break;

	    case 'l': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpl,
	    "l = Limit displayed aliases on the specified criteria."));
		      break;

	    case ctrl('L'): error(catgets(elm_msg_cat, AliasesSet, AliasesHelpCtrlL,
		      "^L = Rewrite the screen."));
	    	      break;

	    case 'm': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpm,
	    "m = Send mail to the current or tagged aliases."));
		      break;

	    case 'n': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpn,
"n = Add a new user alias, adding to alias database at next resync."));
		      break;

	    case 'r':
	    case 'q':
	    case 'i': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpi,
		      "i = Return from alias menu (with prompting)."));
	   	      break;
		      
	    case 'R':
	    case 'Q': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpQ,
		      "R = Return from alias menu (no prompting)."));
	   	      break;
		      
	    case 't': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpt,
		      "t = Tag current alias for further operations."));
		      break;
	
	    case ctrl('T'): error(catgets(elm_msg_cat, AliasesSet, AliasesHelpCtrlT,
	    "^T = Tag aliases matching specified pattern."));
		      break;

	    case 'u': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpu,
	    "u = Unmark the current alias for deletion from alias database."));
		      break;

	    case ctrl('U'): error(catgets(elm_msg_cat, AliasesSet, AliasesHelpCtrlU,
"^U = Mark for undeletion user aliases matching specified pattern."));
		      break;

	    case 'x':
	    case 'X': error(catgets(elm_msg_cat, AliasesSet, AliasesHelpX,
	    "x = Exit from alias menu, abandoning any pending deletions."));
	   	      break;

	    default : error(catgets(elm_msg_cat, AliasesSet, AliasesHelpNoHelp,
			"That key isn't used in this section."));
	   	      break;
	  }
	  lower_prompt(alias_prompt);
	}

	/* Remove help lines */
	MoveCursor(LINES-3, 0);	CleartoEOS();
	return(redraw);
}

get_aliases()
{
/*
 *	Get all the system and user alias info
 *
 *	If we get this far, we must be needing to re-read from
 *	at least one data file.  Unfortunately that means we 
 *	really need to read both since the aliases may be sorted
 *	and all mixed up...  :-(
 */

	char fname[SLEN];
	register int i = -1;
	int dups = 0;

	current = 0;
	num_duplicates = 0;

	/* Read from user data file if it is open. */
	if (user_hash != NULL) {
	    dprint(6, (debugfile,
		      "About to read user data file = %d.\n", user_hash->dbz_basef));
	    lseek(user_hash->dbz_basef, 0L, 0);
	    while (get_one_alias(user_hash, current)) {
		dprint(8, (debugfile, "%d\t%s\t%s\n", current+1,
				       aliases[current]->alias,
				       aliases[current]->address));

		current++;
	    }
	}
	message_count = current;	/* Needed for find_alias() */

	/* Read from system data file if it is open. */
	if (system_hash != NULL) {
	    dprint(6, (debugfile,
		      "About to read system data file = %d.\n", system_hash->dbz_basef));
	    fseek(system_hash->dbz_basef, 0L, 0);
	    while (get_one_alias(system_hash, current)) {
		/* If an identical user alias is found, we may
		 * not want to display it, so we had better mark it.
		 */
		if (find_alias(aliases[current]->alias, USER) >= 0) {
		    setit(aliases[current]->type, DUPLICATE);
		    dups++;
		    setit(aliases[current]->status, URGENT);
				    /* Not really, I want the U for User */
		    dprint(6, (debugfile,
			       "System alias %s is same as user alias.\n",
			       aliases[current]->alias));
		}
		dprint(8, (debugfile, "%d\t%s\t%s\n", current+1,
				       aliases[current]->alias,
				       aliases[current]->address));

		current++;
	    }
	    num_duplicates = dups;
	}
	message_count = current - num_duplicates;

	if (!mail_only && !check_only && message_count) {
	    current = 0;
	    sort_aliases((message_count+num_duplicates), FALSE);
	    current = 1;
	    (void) get_page(current);
	}

}

/* byte-ordering stuff */
#define	MAPIN(o)	((db->dbz_bytesame) ? (of_t) (o) : bytemap((of_t)(o), db->dbz_conf.bytemap, db->dbz_mybmap))
#define	MAPOUT(o)	((db->dbz_bytesame) ? (of_t) (o) : bytemap((of_t)(o), db->dbz_mybmap, db->dbz_conf.bytemap))

static of_t			/* transformed result */
bytemap(ino, map1, map2)
of_t ino;
int *map1;
int *map2;
{
	union oc {
		of_t o;
		char c[SOF];
	};
	union oc in;
	union oc out;
	register int i;

	in.o = ino;
	for (i = 0; i < SOF; i++)
		out.c[map2[i]] = in.c[map1[i]];
	return(out.o);
}

get_one_alias(db, current)
DBZ *db;
int current;
{
/*
 *	Get an alias (name, address, etc.) from the data file
 */

	long new_max;
	register struct alias_rec	**new_aliases, *a;
	struct alias_rec	ar;
	FILE *data_file = db->dbz_basef;

	if (data_file == NULL)
	    return(0);	/* no alias file, but hash exists, error condition */

	if (fread((char *) &ar, sizeof(ar), 1, data_file) <= 0)
	    return(0);

	if (current >= max_aliases) {
	    new_max = max_aliases + KLICK;
	    if (max_aliases == 0)
		new_aliases = (struct alias_rec **)
		    malloc(new_max * sizeof(struct alias_rec *));
	    else
		new_aliases = (struct alias_rec **)
		    realloc((char *) aliases,
			new_max * sizeof(struct alias_rec *));

	    if (new_aliases == NULL) {
		error1(catgets(elm_msg_cat, AliasesSet, AliasesErrorMemory,
      "\n\r\n\rCouldn't allocate enough memory! Alias #%d.\n\r\n\r"),
			current);
		return(0);
	    }
	    aliases = new_aliases;
	    while (max_aliases < new_max)
		aliases[max_aliases++] = NULL;
	}


	if (aliases[current] != NULL) {
	    free((char *) aliases[current]);
	    aliases[current] = NULL;
	}

	ar.status = (int) MAPIN(ar.status);
	ar.alias = (char *) MAPIN(ar.alias);
	ar.last_name = (char *) MAPIN(ar.last_name);
	ar.name = (char *) MAPIN(ar.name);
	ar.comment = (char *) MAPIN(ar.comment);
	ar.address = (char *) MAPIN(ar.address);
	ar.type = (int) MAPIN(ar.type);
	ar.length = (long) MAPIN(ar.length);

	if ((a = (struct alias_rec *)
		malloc(sizeof(ar) + ar.length)) == NULL) {
		error1(catgets(elm_msg_cat, AliasesSet, AliasesErrorMemory,
      "\n\r\n\rCouldn't allocate enough memory! Alias #%d.\n\r\n\r"),
			current);
		return(0);
	    }

	aliases[current] = a;
	fread((char *) (a + 1), ar.length, 1, data_file);
	*a = ar;
	new_max = (long) (a + 1);
	a->alias += new_max;
	a->last_name += new_max;
	a->name += new_max;
	a->comment += new_max;
	a->address += new_max;
	a->length = current;

	return(1);
}

main_state()
{
/*	Save the globals that are shared for both menus
 *	so that we can return to the main menu without
 *	"tragedy".
 */

	static int alias_count = 0,    alias_current = 0, alias_last = -1,
	           alias_selected = 0, alias_page = 0;
	static int main_count = 0,     main_current = 0, main_last = -1,
	           main_selected = 0,  main_page = 0;

	if (inalias) {			/* Restore the settings */
	    alias_count = message_count;
	    alias_current = current;
	    alias_last = last_current;
	    alias_selected = selected;
	    alias_page = header_page;

	    message_count = main_count;
	    current = main_current;
	    last_current = main_last;
	    selected = main_selected;
	    header_page = main_page;

	    strcpy(item, catgets(elm_msg_cat, ElmSet, Elmitem, "message"));
	    strcpy(items, catgets(elm_msg_cat, ElmSet, Elmitems, "messages"));
	    strcpy(Item, catgets(elm_msg_cat, ElmSet, ElmItem, "Message"));
	    strcpy(Items, catgets(elm_msg_cat, ElmSet, ElmItems, "Messages"));
	    strcpy(Prompt, catgets(elm_msg_cat, ElmSet, ElmPrompt, "Command: "));

	    dprint(3, (debugfile, "Leaving alias mode\n"));
	    inalias = FALSE;
	}
	else {
	    main_count = message_count;
	    main_current = current;
	    current_mail_message = current;
	    main_last = last_current;
	    main_selected = selected;
	    main_page = header_page;

	    message_count = alias_count;
	    current = alias_current;
	    last_current = alias_last;
	    selected = alias_selected;
	    header_page = alias_page;

	    strcpy(item, catgets(elm_msg_cat, AliasesSet, Aliasesitem, "alias"));
	    strcpy(items, catgets(elm_msg_cat, AliasesSet, Aliasesitems, "aliases"));
	    strcpy(Item, catgets(elm_msg_cat, AliasesSet, AliasesItem, "Alias"));
	    strcpy(Items, catgets(elm_msg_cat, AliasesSet, AliasesItems, "Aliases"));
	    strcpy(Prompt, catgets(elm_msg_cat, AliasesSet, AliasesPrompt, "Alias: "));

	    dprint(3, (debugfile, "Entered alias mode\n"));
	    inalias = TRUE;
	}
}

int
parse_aliases(buffer, remainder)
char *buffer, *remainder;
{
/*
 *	This routine will parse out the individual aliases present
 *	on the line passed in buffer.  This involves:
 *
 *	1. Testing for an '=' to make sure this is an alias entry.
 *
 *	2. Setting remainder to point to the rest of the line starting
 *	   at the '=' (for later rewriting if needed).
 *
 *	3. Parsing the aliases into an string padded with ',' at 
 *	   the end.
 *
 *	4. Returning the number of aliases found (0 if test #1 fails).
 */

	char *s;
	int number;

/*	Check to see if an alias */

	if ((s = index(buffer, (int)'=')) == NULL)
	  return (0);

	strcpy(remainder, s);		/* Save the remainder of the line */

/*	Terminate the list of aliases with a ',' */

	while (--s >= buffer && whitespace(*s)) ;
	*++s = ',';
	*++s = '\0';

/*	Now, count the aliases */

	number = 0;
	for (s = buffer; *s; s++)
	  if (*s == ',')
	    number++;

	return (number);
}

int
get_aliasname(aliasname, buffer)
char *aliasname, *buffer;
{

	int loc;
	char ch;

	optionally_enter(aliasname, LINES-2, strlen(buffer), FALSE, FALSE);
	if (strlen(aliasname) == 0) 
	    return(-1);
        if ( !ok_alias_name(aliasname) ) {
	    error1(catgets(elm_msg_cat, AliasesSet, AliasesBadChars,
	            "Bad character(s) in alias name %s."), aliasname);
	    return(-1);
	}
	if ((loc = find_alias(aliasname, USER)) >= 0) {
	    dprint(3, (debugfile, 
	         "Attempt to add a duplicate alias [%s] in get_aliasname\n",
	         aliases[loc]->address)); 
	    if (aliases[loc]->type & GROUP )
	        error1(catgets(elm_msg_cat, AliasesSet, AliasesAlreadyGroup,
	                "Already a group with name %s."), aliases[loc]->alias);
	    else
	        error1(catgets(elm_msg_cat, AliasesSet, AliasesAlreadyAlias,
	                "Already an alias for %s."), aliases[loc]->alias);
	    return(-1);
	}
	if ((loc = find_alias(aliasname, SYSTEM)) >= 0) {
	    dprint(3, (debugfile, 
	      "Attempt to add a duplicate system alias [%s] in get_aliasname\n",
	      aliases[loc]->address)); 
	  
	    PutLine2(LINES-2, 0, catgets(elm_msg_cat,
	            AliasesSet, AliasesSystemAlias,
		    "System (%6s) alias for %s."),
	        alias_type(aliases[loc]->type), aliases[loc]->alias);
	    MCsprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesSuperceed,
	            "Superceed? (%c/%c)"), *def_ans_yes, *def_ans_no);
	    if ((ch = want_to(buffer, *def_ans_yes)) != *def_ans_yes)
	        return(-1);
	}
	return(0);

}

void
get_realnames(aliasname, firstname, lastname, comment, buffer)
char *aliasname, *firstname, *lastname, *comment, *buffer;
{

	sprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesEnterLastName,
		"Enter last name for %s: "), aliasname);
	PutLine0(LINES-2,0, buffer);
	CleartoEOLN();
	optionally_enter(lastname, LINES-2, strlen(buffer), FALSE, FALSE);

	sprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesEnterFirstName,
		"Enter first name for %s: "), aliasname);
	PutLine0(LINES-2,0, buffer);
	CleartoEOLN();
	optionally_enter(firstname, LINES-2, strlen(buffer), FALSE, FALSE);

	if ((strlen(lastname) == 0) && (strlen(firstname) == 0)) {
	    strcpy(lastname, aliasname);  
	}

	sprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesEnterComment,
		"Enter optional comment for %s: "), aliasname);
	PutLine0(LINES-2,0, buffer);
	optionally_enter(comment, LINES-2, strlen(buffer), FALSE, FALSE);

}

int
ask_accept(aliasname, firstname, lastname, comment, address, buffer)
char *aliasname, *firstname, *lastname, *comment, *address, *buffer;
{

	char ch;

	if (strlen(firstname) == 0) {
	    strcpy(buffer, lastname);  
	}
	else {
	    sprintf(buffer, "%s %s", firstname, lastname);
	}
	PutLine3(LINES-1,0, catgets(elm_msg_cat, AliasesSet, AliasesAddressAs,
	        "Messages addressed as: %s (%s)"), address, buffer);
	if (strlen(comment) != 0) {
	    strcat(buffer, ", ");
	    strcat(buffer, comment);
	}

	PutLine3(LINES-2,0, catgets(elm_msg_cat, AliasesSet, AliasesAddressTo,
	        "New alias: %s is '%s'."), aliasname, buffer);
	CleartoEOLN();
	MCsprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesAcceptNew,
		"  Accept new alias? (%c/%c) "), *def_ans_yes, *def_ans_no);
	if((ch = want_to(buffer, *def_ans_yes)) == *def_ans_yes)
	  add_to_alias_text(aliasname, firstname, lastname, comment, address);
	ClearLine(LINES-2);
	ClearLine(LINES-1);
	return(ch == *def_ans_yes ? 1 : 0);

}
