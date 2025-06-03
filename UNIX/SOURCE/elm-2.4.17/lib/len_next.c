
static char rcsid[] = "@(#)$Id: len_next.c,v 5.1 1992/10/03 22:41:36 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
 *
 * 			Copyright (c) 1992 USENET Community Trust
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: len_next.c,v $
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** get the length of the next part of the address/data field

	This code returns the length of the next part of the
  string field containing address/data.  It takes into account
  quoting via " as well as \ escapes.
  Quoting via ' is not taken into account, as RFC-822 does not
  consider a ' character a valid 'quoting character'

  A 1 is returned for a single character unless:

  A 0 is returned at end of string.

  A 2 is returned for strings that start \

  The length of quoted sections is returned for quoted fields

**/

#include <ctype.h>


int
len_next_part(s)
register char *s;
{
	register char *c, quot;

	quot = *s;

	if (quot == '\0')
	  return(0);

	if (quot == '\\') {
	  if (*s++)
	    return(2);
	  else
	    return(1);
	}

	if (quot != '"')
	  return(1);

	for (c = s + 1; *c; c++) {
	  if (*c == quot)
	    return(1 + c - s);

          if (*c == '\\') {
	    if (*c++)
	      c++;
	  }
	}

	return(c - s);
}
