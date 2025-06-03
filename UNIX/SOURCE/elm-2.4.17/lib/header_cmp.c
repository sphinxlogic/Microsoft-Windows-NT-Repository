static char rcsid[] = "@(#)$Id: header_cmp.c,v 5.2 1992/11/07 20:59:49 syd Exp $";

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
 * $Log: header_cmp.c,v $
 * Revision 5.2  1992/11/07  20:59:49  syd
 * fix typo
 *
 * Revision 5.1  1992/11/07  20:16:29  syd
 * Initial Checkin
 *
 *
 ******************************************************************************/

/** 
	compare a header, ignoring case and allowing linear white space
	around the :.  Header must be anchored to the start of the line.

	returns NULL if no match, or first character after trailing linear
	white space of the :.

**/

#include "headers.h"
#include <ctype.h>

#ifdef BSD
#undef tolower
#endif


char *
header_cmp(header, prefix, suffix)
register char *header, *prefix, *suffix;
{
	int len;

	len = strlen(prefix);
	if (strincmp(header, prefix, len))
		return(NULL);

	/* skip over while space if any */
	header += len;

	if (*header != ':')	/* headers must end in a : */
		return(NULL);

	/* skip over while space if any */
	header++;

	while (*header) {
		if (!whitespace(*header))
			break;
		header++;
	}

	if (suffix != NULL) {
		len = strlen(suffix);
		if (len > 0)
			if (strincmp(header, suffix, len))
				return(NULL);
	}

	return(header);
}
