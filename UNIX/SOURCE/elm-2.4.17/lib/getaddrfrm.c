
static char rcsid[] = "@(#)$Id: getaddrfrm.c,v 5.2 1992/11/07 20:05:52 syd Exp $";

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
 * $Log: getaddrfrm.c,v $
 * Revision 5.2  1992/11/07  20:05:52  syd
 * change to use header_cmp to allow for linear white space around the colon
 * From: Syd
 *
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** 

**/

#include "headers.h"
#include <ctype.h>

#ifdef USE_EMBEDDED_ADDRESSES

get_address_from(line, buffer)
char *line, *buffer;
{
	/** This routine extracts the address from either a 'From:' line
	    or a 'Reply-To:' line.  The strategy is as follows:  if the
	    line contains a '<', then the stuff enclosed is returned.
	    Otherwise we go through the line and strip out comments
	    and return that.  White space will be elided from the result.
	**/

    register char *s;
    register int l;

    /**  Skip start of line over prefix, e.g. "From:".  **/
    if ((s = index(line, ':')) != NULL)
	line = s + 1;

    /**  If there is a '<' then copy from it to '>' into the buffer.  **/
    if ( (s = index(line,'<')) != NULL ) {
	s++;
	while (*s != '\0' && *s != '>') {
	    l = len_next_part(s);
	    if (l == 1) {
	      if ( !isspace(*s) )
		*buffer++ = *s;
	      s++;
	    } else {
	      while (--l >= 0)
		*buffer++ = *s++;
	    }
	}
	*buffer = '\0';
	return;
    }

    /**  Otherwise, strip comments and get address with whitespace elided.  **/
    s = strip_parens(line);
    while (*s != '\0') {
	l = len_next_part(s);
	if (l == 1) {
	  if ( !isspace(*s) )
	    *buffer++ = *s;
	  s++;
	} else {
	  while (--l >= 0)
	    *buffer++ = *s++;
	}
    }
    *buffer = '\0';

}

#endif
