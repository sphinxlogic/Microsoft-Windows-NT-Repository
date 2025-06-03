static char rcsid[] = "@(#)$Id: striparens.c,v 5.1 1992/10/03 22:41:36 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
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
 * $Log: striparens.c,v $
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** 

**/

#include "headers.h"


char *strip_parens(string)
char *string;
{
	/**
	    Remove parenthesized information from a string.  More specifically,
	    comments as defined in RFC822 are removed.  This procedure is
	    non-destructive - a pointer to static data is returned.
	**/
	static char  buffer[VERY_LONG_STRING];
	register char *bufp;
	register int depth, l;

	bufp = buffer;
	depth = 0;
	while (*string != '\0') {
	  l = len_next_part(string);
	  if (l == 1) {
	    switch ( *string ) {
	    case '(':			/* begin comment on '('		*/
	      ++depth;
	      break;
	    case ')':			/* decr nesting level on ')'	*/
	      --depth;
	      break;
	    case '\\':			/* treat next char literally	*/
	      if ( *++string == '\0' ) {		/* gracefully handle	*/
		*bufp++ = '\\';			/* '\' at end of string	*/
		--string;				/* even tho it's wrong	*/
	      } else if ( depth == 0 ) {
		*bufp++ = '\\';
		*bufp++ = *string;
	      }
	      break;
	    default:			/* a regular char		*/
	      if ( depth == 0 )
		*bufp++ = *string;
	      break;
	    }
	    string++;
	  } else {
	    if (depth == 0) {
	      while (--l >= 0)
		*bufp++ = *string++;
	    } else
	      string += l;
	  }
	}
	*bufp = '\0';
	return( (char *) buffer);
}

/*
 * Added by RLH.  This could be combined w/ above if willing to pass
 * a TRUE/FALSE for whether we are stripping or getting...
 */
char *get_parens(string)
char *string;
{
	/**
	    Find and return parenthesized information in a string.  More
	    specifically, comments as defined in RFC822 are retrieved.
	    This procedure is non-destructive - a pointer to static data
	    is returned.
	**/
	static char  buffer[VERY_LONG_STRING];
	register char *bufp;
	register int depth, l;

	bufp = buffer;
	depth = 0;
	while (*string != '\0') {
	  l = len_next_part(string);
	  if (l == 1) {
	    switch ( *string ) {
	    case '(':			/* begin comment on '('		*/
	      ++depth;
	      break;
	    case ')':			/* decr nesting level on ')'	*/
	      --depth;
	      break;
	    case '\\':			/* treat next char literally	*/
	      if ( *++string == '\0' ) {		/* gracefully handle	*/
		*bufp++ = '\\';			/* '\' at end of string	*/
		--string;				/* even tho it's wrong	*/
	      } else if ( depth > 0 ) {
		*bufp++ = '\\';
		*bufp++ = *string;
	      }
	      break;
	    default:			/* a regular char		*/
	      if ( depth > 0 )
		*bufp++ = *string;
	      break;
	    }
	    string++;
	  } else {
	    if (depth > 0) {
	      while (--l >= 0)
		*bufp++ = *string++;
	    } else
	      string += l;
	  }
	}
	*bufp = '\0';
	return( (char *) buffer);
}
