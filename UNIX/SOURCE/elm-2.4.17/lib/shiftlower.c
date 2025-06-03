static char rcsid[] = "@(#)$Id: shiftlower.c,v 5.1 1992/10/03 22:41:36 syd Exp $";

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
 * $Log: shiftlower.c,v $
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** 

**/

#include "headers.h"

#include <ctype.h>

#ifdef BSD
#undef tolower
#undef toupper
#endif

char *shift_lower(string)
char *string;
{
	/** return 'string' shifted to lower case.  Do NOT touch the
	    actual string handed to us! **/

	static char buffer[VERY_LONG_STRING];
	register char *bufptr = buffer;

	for (; *string; string++, bufptr++)
	  if (isupper(*string))
	    *bufptr = tolower(*string);
	  else
	    *bufptr = *string;
	
	*bufptr = 0;
	
	return( (char *) buffer);
}
