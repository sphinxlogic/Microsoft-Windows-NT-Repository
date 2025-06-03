
static char rcsid[] = "@(#)$Id: aliaslib.c,v 5.3 1992/12/12 01:28:24 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.3 $   $State: Exp $
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
 * $Log: aliaslib.c,v $
 * Revision 5.3  1992/12/12  01:28:24  syd
 * in do_get_alias().  abuf[] was under dimensioned.
 * From: "Robert L. Howard" <robert.howard@matd.gatech.edu>
 *
 * Revision 5.2  1992/10/11  01:21:17  syd
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
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** Library of functions dealing with the alias system...

 **/

#include "headers.h"
#include "s_elm.h"
#include <ctype.h>

char *get_alias_address(), *qstrpbrk(), *index();
extern int current_mail_message;
long lseek();

/*
 * Expand "name" as an alias and return a pointer to static data containing
 * the expansion.  If "name" is not an alias, then NULL is returned.
 */
char *get_alias_address(name, mailing)
char *name;	/* name to expand as an alias				*/
int mailing;	/* TRUE to fully expand group names & recursive aliases	*/
{
	static char buffer[VERY_LONG_STRING];
	char *bufptr;
	int bufsize, tmpflag = FALSE;

	if (!inalias) {
	    main_state();
	    tmpflag = TRUE;
	}

	/* reopens files iff changed since last read */
	open_alias_files();

	/* if name is an alias then return its expansion */
	bufptr = buffer;
	bufsize = sizeof(buffer);
	if ( do_get_alias(name, &bufptr, &bufsize, mailing, FALSE, 0) ) {
	  if (tmpflag) main_state();
	  return buffer+2;	/* skip comma/space from add_name_to_list() */
	}

	/* nope...not an alias */
	if (tmpflag) main_state();
	return (char *) NULL;
}


/*
 * Determine if "name" is an alias, and if so expand it and store the result in
 * "*bufptr".  TRUE returned if any expansion occurs, else FALSE is returned.
 */
int do_get_alias(name, bufptr, bufsizep, mailing, sysalias, depth)
char *name;	/* name to expand as an alias				*/
char **bufptr;	/* place to store result of expansion			*/
int *bufsizep;	/* available space in the buffer			*/
int mailing;	/* TRUE to fully expand group names & recursive aliases	*/
int sysalias;	/* TRUE to suppress checks of the user's aliases	*/
int depth;	/* recursion depth - initially call at depth=0		*/
{
	struct alias_rec *match;
	char abuf[VERY_LONG_STRING];
	int loc;

	/* update the recursion depth counter */
	++depth;

	dprint(6, (debugfile, "%*s->attempting alias expansion on \"%s\"\n",
		(depth*2), "", name));

	/* strip out (comments) and leading/trailing whitespace */
	remove_possible_trailing_spaces( name = strip_parens(name) );
	for ( ; isspace(*name)  ; ++name ) ;

	/* throw back empty addresses */
	if ( *name == '\0' )
	  return FALSE;

/* The next two blocks could be merged somewhat */
	/* check for a user alias, unless in the midst of sys alias expansion */
	if ( !sysalias ) {
	  if ( (loc = find_alias(name, USER)) >= 0 ) {
	    match = aliases[loc];
	    strcpy(abuf, match->address);
	    if ( match->type & PERSON ) {
	      if (strlen(match->name) > 0) {
                sprintf(abuf+strlen(abuf), " (%s)", match->name);
	      }
	    }
	    goto do_expand;
	  }
	}

	/* check for a system alias */
	  if ( (loc = find_alias(name, SYSTEM)) >= 0 ) {
	    match = aliases[loc];
	    strcpy(abuf, match->address);
	    if ( match->type & PERSON ) {
	      if (strlen(match->name) > 0) {
                sprintf(abuf+strlen(abuf), " (%s)", match->name);
	      }
	    }
	    sysalias = TRUE;
	    goto do_expand;
	  }

	/* nope...this name wasn't an alias */
	return FALSE;

do_expand:

	/* at this point, alias is expanded into "abuf" - now what to do... */

	dprint(7, (debugfile, "%*s  ->expanded alias to \"%s\"\n",
	    (depth*2), "", abuf));

	/* check for an exact match */
	loc = strlen(name);
	if (strncmp(name, abuf, loc) == 0 && (isspace(abuf[loc]) || abuf[loc] == '\0'))
	  return add_name_to_list(abuf, bufptr, bufsizep);

	/* see if we are stuck in a loop */
	if ( depth > 12 ) {
	  dprint(2, (debugfile,
	      "alias expansion loop detected at \"%s\" - bailing out\n", name));
	    error1(catgets(elm_msg_cat, ElmSet, ElmErrorExpanding,
		"Error expanding \"%s\" - probable alias definition loop."),
		name);
	    return FALSE;
	}

	/* see if the alias equivalence is a group name */
	if ( mailing && match->type & GROUP )
	  return do_expand_group(abuf, bufptr, bufsizep, sysalias, depth);

	/* see if the alias equivalence is an email address */
	if ( qstrpbrk(abuf,"!@:") != NULL ) {
	  return add_name_to_list(abuf, bufptr, bufsizep);
	}

	/* see if the alias equivalence is itself an alias */
	if ( mailing && do_get_alias(abuf,bufptr,bufsizep,TRUE,sysalias,depth) )
	  return TRUE;

	/* the alias equivalence must just be a local address */
	return add_name_to_list(abuf, bufptr, bufsizep);
}


/*
 * Expand the comma-delimited group of names in "group", storing the result
 * in "*bufptr".  Returns TRUE if expansion occurs OK, else FALSE in the
 * event of errors.
 */
int do_expand_group(group, bufptr, bufsizep, sysalias, depth)
char *group;	/* group list to expand					*/
char **bufptr;	/* place to store result of expansion			*/
int *bufsizep;	/* available space in the buffer			*/
int sysalias;	/* TRUE to suppress checks of the user's aliases	*/
int depth;	/* nesting depth					*/
{
	char *name;

	/* go through each comma-delimited name in the group */
	while ( group != NULL ) {

	  /* extract the next name from the list */
	  for ( name = group ; isspace(*name) ; ++name ) ;
	  if ( (group = index(name,',')) != NULL )
	      *group++ = '\0';
	  remove_possible_trailing_spaces(name);
	  if ( *name == '\0' )
	    continue;

	  /* see if this name is really an alias */
	  if ( do_get_alias(name, bufptr, bufsizep, TRUE, sysalias, depth) )
	    continue;

	  /* verify it is a valid address */
	  if ( !valid_name(name) ) {
	    dprint(3, (debugfile,
		"Illegal address %s during list expansion in %s\n",
		name, "do_get_alias"));
	    error1(catgets(elm_msg_cat, ElmSet, ElmIllegalAddress,
		"%s is an illegal address!"), name);
	    return FALSE;
	  }

	  /* add it to the list */
	  if ( !add_name_to_list(name, bufptr, bufsizep) )
	    return FALSE;

	}

	return TRUE;
}


/*
 * Append "<comma><space>name" to the list, checking to ensure the buffer
 * does not overflow.  Upon return, *bufptr and *bufsizep will be updated to
 * reflect the stuff added to the buffer.  If a buffer overflow would occur,
 * an error message is printed and FALSE is returned, else TRUE is returned.
 */
int add_name_to_list(name,bufptr,bufsizep)
register char *name;	/* name to append to buffer			*/
register char **bufptr;	/* pointer to pointer to end of buffer		*/
register int *bufsizep;	/* pointer to space remaining in buffer		*/
{
	if ( *bufsizep < 0 )
	    return FALSE;

	*bufsizep -= strlen(name)+2;
	if ( *bufsizep <= 0 ) {
	    *bufsizep = -1;
	    error(catgets(elm_msg_cat, ElmSet, ElmAliasExpTooLong,
		"Alias expansion is too long."));
	    return FALSE;
	}

	*(*bufptr)++ = ',';
	*(*bufptr)++ = ' ';
	while ( *name != '\0' )
	  *(*bufptr)++ = *name++ ;
	**bufptr = '\0';

	return TRUE;
}
