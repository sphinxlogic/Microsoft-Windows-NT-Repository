
static char rcsid[] = "@(#)$Id: qstrings.c,v 5.1 1992/10/03 22:41:36 syd Exp $";

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
 * $Log: qstrings.c,v $
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** This file contains equivalent routines to the string routines, but
     modifed to handle quoted strings.

**/

#include "headers.h"
#include <ctype.h>

#ifdef BSD
#undef tolower
#undef toupper
#endif

char *qstrpbrk(source, keys)
char *source, *keys;
{
	/** Returns a pointer to the first character of source that is any
	    of the specified keys, or NULL if none of the keys are present
	    in the source string. 
	**/

	register char *s, *k;
	register int len;

	s = source;
	while (*s != '\0') {
	  len = len_next_part(s);
	  if (len == 1) {
	    for (k = keys; *k; k++)
	      if (*k == *s)
		return(s);
	  }
	  s += len;
	}
	
	return(NULL);
}

int
qstrspn(source, keys)
char *source, *keys;
{
	/** This function returns the length of the substring of
	    'source' (starting at zero) that consists ENTIRELY of
	    characters from 'keys'.  This is used to skip over a
	    defined set of characters with parsing, usually. 
	**/

	register int loc = 0, key_index = 0, len;

	while (source[loc] != '\0') {
	  key_index = 0;
	  len = len_next_part(&source[loc]);
	  if (len > 1)
	    return(loc);

	  while (keys[key_index] != source[loc])
	    if (keys[key_index++] == '\0')
	      return(loc);
	  loc++;
	}

	return(loc);
}

int
qstrcspn(source, keys)
char *source, *keys;
{
	/** This function returns the length of the substring of
	    'source' (starting at zero) that consists entirely of
	    characters NOT from 'keys'.  This is used to skip to a
	    defined set of characters with parsing, usually. 
	    NOTE that this is the opposite of strspn() above
	**/

	register int loc = 0, key_index = 0, len;

	while (source[loc] != '\0') {
	  key_index = 0;
	  len = len_next_part(&source[loc]);
	  if (len > 1) {
	    loc += len;
	    continue;
	  }

	  while (keys[key_index] != '\0')
	    if (keys[key_index++] == source[loc])
	      return(loc);
	  loc++;
	}

	return(loc);
}
