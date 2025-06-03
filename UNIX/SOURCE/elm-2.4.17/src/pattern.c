
static char rcsid[] = "@(#)$Id: pattern.c,v 5.2 1992/11/26 00:46:50 syd Exp $";

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
 * $Log: pattern.c,v $
 * Revision 5.2  1992/11/26  00:46:50  syd
 * Fix how errno is used so err is inited and used instead
 * as errno gets overwritten by print system call
 * From: Syd
 *
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/**    General pattern matching for the ELM mailer.     

**/

#include <errno.h>

#include "headers.h"
#include "s_elm.h"

static char pattern[SLEN] = { "" };
static char alt_pattern[SLEN] = { "" };

extern int errno;

char *error_description(), *shift_lower(), *strcpy();
static char *tag_word = NULL;
static char *tagged_word = NULL;
static char *Tagged_word = NULL;
static char *delete_word = NULL;
static char *mark_delete_word = NULL;
static char *Mark_delete_word = NULL;
static char *undelete_word = NULL;
static char *undeleted_word = NULL;
static char *Undeleted_word = NULL;
static char *enter_pattern = NULL;
static char *match_pattern = NULL;
static char *entire_match_pattern = NULL;
static char *match_anywhere = NULL;

int
meta_match(function)
int function;
{
	char	ch;

	/** Perform specific function based on whether an entered string 
	    matches either the From or Subject lines.. 
	    Return TRUE if the current message was matched, else FALSE.
	**/

	register int i, tagged=0, count=0, curtag = 0;
	char		msg[SLEN];
	static char     meta_pattern[SLEN];

	if (delete_word == NULL) {
	     tag_word = catgets(elm_msg_cat, ElmSet, ElmTag, "Tag");
	     tagged_word = catgets(elm_msg_cat, ElmSet, ElmTagged, "tagged");
	     Tagged_word = catgets(elm_msg_cat, ElmSet, ElmCapTagged, "Tagged");
	     delete_word = catgets(elm_msg_cat, ElmSet, ElmDelete, "Delete");
	     mark_delete_word = catgets(elm_msg_cat, ElmSet, ElmMarkDelete, "marked for deletion");
	     Mark_delete_word = catgets(elm_msg_cat, ElmSet, ElmCapMarkDelete, "Marked for deletion");
	     undelete_word = catgets(elm_msg_cat, ElmSet, ElmUndelete, "Undelete");
	     undeleted_word = catgets(elm_msg_cat, ElmSet, ElmUndeleted, "undeleted");
	     Undeleted_word = catgets(elm_msg_cat, ElmSet, ElmCapUndeleted, "Undeleted");
	     enter_pattern = catgets(elm_msg_cat, ElmSet, ElmEnterPattern, "Enter pattern: ");
	}

	PutLine2(LINES-3, strlen(Prompt), catgets(elm_msg_cat, ElmSet, ElmMessagesMatchPattern,
	     "%s %s that match pattern..."), 
	     function==TAGGED?tag_word: function==DELETED?delete_word:undelete_word, items);

	if (function == TAGGED) {	/* are messages already tagged??? */
	  for (i=0; i < message_count; i++)
	    if (ison(ifmain(headers[i]->status,
	                    aliases[i]->status),TAGGED))
	      tagged++;

	  if (tagged) {
	    if (tagged > 1) {
	      PutLine1(LINES-2,0, catgets(elm_msg_cat, ElmSet, ElmSomeMessagesATagged,
		"Some %s are already tagged."), items);
	      MCsprintf(msg, catgets(elm_msg_cat, ElmSet, ElmRemoveTags,
		"Remove Tags? (%c/%c) %c%c"),
		*def_ans_yes, *def_ans_no, *def_ans_yes, BACKSPACE);
	    } else {
	      PutLine1(LINES-2,0, catgets(elm_msg_cat, ElmSet, ElmAMessageATagged,
		"One %s is already tagged."), item);
	      MCsprintf(msg, catgets(elm_msg_cat, ElmSet, ElmRemoveTag,
		"Remove Tag? (%c/%c) %c%c"),
		*def_ans_yes, *def_ans_no, *def_ans_yes, BACKSPACE);
	    }
	
	    Write_to_screen(msg, 0);

	    ch = ReadCh();
	    if (tolower(ch) != *def_ans_no) {	/* remove tags... */
	      Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmYes, "Yes."), 0);
	      for (i=0; i < message_count; i++) {
	        if (inalias)
	          clearit(aliases[i]->status,TAGGED);
	        else
	          clearit(headers[i]->status,TAGGED);
		show_new_status(i);
	      }
	    } else
	      Write_to_screen(catgets(elm_msg_cat, ElmSet, ElmNo, "No."), 0);
	  }
	}
	
	PutLine0(LINES-2,0, enter_pattern);
	CleartoEOLN();

	optionally_enter(meta_pattern, LINES-2, strlen(enter_pattern),
	  FALSE, FALSE);

	if (strlen(meta_pattern) == 0) {
	  ClearLine(LINES-2);
	  return(curtag);
	}

	strcpy(meta_pattern, shift_lower(meta_pattern));   /* lowercase it */

	if (inalias) {
	  for (i = 0; i < message_count; i++) {
	    if (name_matches(i, meta_pattern) ||
	        alias_matches(i, meta_pattern)) {
	      if (! selected || (selected && (aliases[i]->status & VISIBLE))) {
	        if (function == UNDELETE)
	          clearit(aliases[i]->status, DELETED);
	        else
	          if ((function == DELETED) && (aliases[i]->type & SYSTEM)) {
	            if(i == current - 1) curtag--;
	            count--;
	          }
	          else
	            setit(aliases[i]->status, function);
	        show_new_status(i);
	        if(i == current - 1) curtag++;
	        count++;
	      }
	    }
	  }
	}
	else {
	  for (i = 0; i < message_count; i++) {
	    if (from_matches(i, meta_pattern) ||
	        subject_matches(i, meta_pattern)) {
	      if (! selected || (selected && headers[i]->status & VISIBLE)) {
	        if (function == UNDELETE)
	          clearit(headers[i]->status, DELETED);
	        else
	          setit(headers[i]->status, function);
	        show_new_status(i);
	        if(i == current - 1) curtag++;
	        count++;
	      }
	    }
	  }
	}

	ClearLine(LINES-2);	/* remove "pattern: " prompt */
	
	if (count > 1) {
	  error3(catgets(elm_msg_cat, ElmSet, ElmTaggedMessages,
		"%s %d %s."), 
	         function==TAGGED? Tagged_word : 
		   function==DELETED? Mark_delete_word : Undeleted_word,
		 count, items);
	} else if (count == 1) {
	  error2(catgets(elm_msg_cat, ElmSet, ElmTaggedMessage,
		"%s 1 %s."), 
	         function==TAGGED? Tagged_word : 
		   function==DELETED? Mark_delete_word : Undeleted_word, item);
	} else {
	  error2(catgets(elm_msg_cat, ElmSet, ElmNoMatchesNoTags,
		"No matches. No %s %s."), items,
		 function==TAGGED? tagged_word : 
		   function==DELETED? mark_delete_word: undeleted_word);
	}

	return(curtag);
}
	  
int
pattern_match()
{
	/** Get a pattern from the user and try to match it with the
	    from/subject lines being displayed.  If matched (ignoring
	    case), move current message pointer to that message, if
	    not, error and return ZERO **/

	register int i;

	char buffer[SLEN];
	int anywhere = FALSE;
	int matched;

	if (match_pattern == NULL) {
	  match_pattern = catgets(elm_msg_cat, ElmSet, ElmMatchPattern,
		"Match pattern:");
	  entire_match_pattern = catgets(elm_msg_cat, ElmSet,
		ElmMatchPatternInEntire, "Match pattern (in entire %s):");
	  match_anywhere = catgets(elm_msg_cat, ElmSet, ElmMatchAnywhere,
		"/ = Match anywhere in %s.");
	}

	MCsprintf(buffer, entire_match_pattern, item);

	PutLine1(LINES-3,40, match_anywhere, items);
	
	PutLine0(LINES-1,0, match_pattern);

	if (pattern_enter(pattern, alt_pattern, LINES-1,
		strlen(match_pattern)+1, buffer)) {
	  if (strlen(alt_pattern) == 0) {
	    return(1);
	  }
	  anywhere = TRUE;
	  strcpy(pattern, shift_lower(alt_pattern));
	}
	else if (strlen(pattern) == 0) {
	  return(0);
	}
	else {
	  strcpy(pattern, shift_lower(pattern));
	}

	if (inalias) {
	  for (i = current; i < message_count; i++) {
	    matched = name_matches(i, pattern) || alias_matches(i, pattern);
	    if (! matched && anywhere) {	/* Look only if no match yet */
	      matched = comment_matches(i, pattern) ||
			address_matches(i, pattern);
	    }
	    if (matched) {
	      if (!selected || (selected && aliases[i]->status & VISIBLE)) {
	        current = ++i;
	        return(1);
	      }
	    }
	  }
	}
	else {
	  if (anywhere) {
	      return(match_in_message(pattern));
	  }
	  else {
	    for (i = current; i < message_count; i++) {
	      if (from_matches(i, pattern) || subject_matches(i, pattern)) {
	        if (!selected || (selected && headers[i]->status & VISIBLE)) {
	          current = ++i;
	          return(1);
	        }
	      }
	    }
	  }
	}

	return(0);
}

int
from_matches(message_number, pat)
int message_number;
char *pat;
{
	/** Returns true iff the pattern occurs in it's entirety
	    in the from line of the indicated message **/

	return( in_string(shift_lower(headers[message_number]->from), 
		pat) );
}

int
subject_matches(message_number, pat)
int message_number;
char *pat;
{
	/** Returns true iff the pattern occurs in it's entirety
	    in the subject line of the indicated message **/

	return( in_string(shift_lower(headers[message_number]->subject), 
		pat) );
}

int
name_matches(message_number, pat)
int message_number;
char *pat;
{
	/** Returns true iff the pattern occurs in it's entirety
	    in the proper name of the indicated alias **/

	return( in_string(shift_lower(aliases[message_number]->name),
		pat) );
}

int
alias_matches(message_number, pat)
int message_number;
char *pat;
{
	/** Returns true iff the pattern occurs in it's entirety
	    in the alias name of the indicated alias **/

	return( in_string(shift_lower(aliases[message_number]->alias),
		pat) );
}

int
comment_matches(message_number, pat)
int message_number;
char *pat;
{
	/** Returns true iff the pattern occurs in it's entirety
	    in the comment field of the indicated alias **/

	return( in_string(shift_lower(aliases[message_number]->comment),
		pat) );
}

int
address_matches(message_number, pat)
int message_number;
char *pat;
{
	/** Returns true iff the pattern occurs in it's entirety
	    in the fully expanded address of the indicated alias **/

	return( in_string(shift_lower(
		get_alias_address(aliases[message_number]->alias, TRUE)),
		pat) );
}

match_in_message(pat)
char *pat;
{
	/** Match a string INSIDE a message...starting at the current 
	    message read each line and try to find the pattern.  As
	    soon as we do, set current and leave! 
	    Returns 1 if found, 0 if not
	**/

	char buffer[LONG_STRING];
	int  message_number, lines, line, line_len, err;

	message_number = current-1;

	error(catgets(elm_msg_cat, ElmSet, ElmSearchingFolderPattern,
		"Searching folder for pattern..."));

	while (message_number < message_count) {

	  if (fseek(mailfile, headers[message_number]->offset, 0L) == -1) {

	    err = errno;
	    dprint(1, (debugfile,
		"Error: seek %ld bytes into file failed. errno %d (%s)\n",
		headers[message_number]->offset, err, 
		"match_in_message"));
	    error2(catgets(elm_msg_cat, ElmSet, ElmMatchSeekFailed,
		   "ELM [match] failed looking %ld bytes into file (%s)."),
		   headers[message_number]->offset, error_description(err));
	    return(1);	/* fake it out to avoid replacing error message */
	  }

	  line = 0;
	  lines = headers[message_number]->lines;

	  while ((line_len = mail_gets(buffer, LONG_STRING, mailfile)) && line < lines) {
	
	    if(buffer[line_len - 1] == '\n') line++;

	    if (in_string(shift_lower(buffer), pat)) {
	      current = message_number+1; 
	      clear_error();
	      return(1);
	    }
	  }

	  /** now we've passed the end of THIS message...increment and 
	      continue the search with the next message! **/

	  message_number++;
	}

	return(0);
}
