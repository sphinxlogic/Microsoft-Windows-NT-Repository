
static char rcsid[] = "@(#)$Id: string2.c,v 5.1 1992/10/03 22:58:40 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
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
 * $Log: string2.c,v $
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** This file contains string functions that are shared throughout the
    various ELM utilities...

**/

#include "headers.h"
#include <ctype.h>

#ifdef BSD
#undef tolower
#undef toupper
#endif


int
occurances_of(ch, string)
char ch, *string;
{
	/** returns the number of occurances of 'ch' in string 'string' **/

	register int count = 0;

	for (; *string; string++)
	  if (*string == ch) count++;

	return(count);
}

int
qoccurances_of(ch, string)
char ch, *string;
{
	/** returns the number of occurances of 'ch' in string 'string' **/

	register int count = 0, len;

	while(*string) {
	  len = len_next_part(string);
	  if (len == 1 && *string == ch) count++;
	  string += len;
	}

	return(count);
}

remove_possible_trailing_spaces(string)
char *string;
{
	/** an incredibly simple routine that will read backwards through
	    a string and remove all trailing whitespace.
	**/

	register int i, j;

	for ( j = i = strlen(string); --i >= 0 && whitespace(string[i]); )
		/** spin backwards, semicolon intented **/ ;

	if (string[i-1] == '\\') /* allow for line to end with \blank  */
		i++;

	if (i < j)
	  string[i+1] = '\0';	/* note that even in the worst case when there
				   are no trailing spaces at all, we'll simply
				   end up replacing the existing '\0' with
				   another one!  No worries, as M.G. would say
				*/
}
