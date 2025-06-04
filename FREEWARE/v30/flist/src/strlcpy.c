/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: strlcpy.c,v 1.3 1995/02/19 02:23:25 tom Exp $";
#endif

/*
 * Title:	strlcpy.c
 * Author:	Thomas E. Dickey
 * Created:	28 Jun 1984
 * Last update: 15 May 1985, use macro, not routine for convert
 *		28 Jun 1984
 *
 * Function:	Copy a string, converting it to lowercase.
 *
 * Parameters:	optr -	output string pointer
 * 		iptr -	input string pointer
 *
 * Returns:	Pointer to final null in output buffer.
 */

#include	<ctype.h>

#include	"strutils.h"

char *
strlcpy (
	char	*optr,			/* => output string		*/
	char	*iptr)			/* => input string		*/
{
	if (iptr == 0)
		iptr = optr;

	while (*iptr)
	{
		*optr++ = _tolower(*iptr);
		iptr++;
	}
	*optr = '\0';			/* Copy a trailing null		*/
	return (optr);
}
