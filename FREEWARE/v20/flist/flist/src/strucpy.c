/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef	NO_IDENT
static	char	*Id = "$Id: strucpy.c,v 1.3 1995/02/19 02:20:24 tom Exp $";
#endif

/*
 * Title:	strucpy.c
 * Author:	Thomas E. Dickey
 * Created:	22 Aug 1983
 * Last update:
 *		18 Feb 1995, prototyped
 *		15 May 1985, use ctype-include
 *		11 Nov 1983
 *
 * Function:	Copy a string, converting it to uppercase.
 *
 * Parameters:	optr -	output string pointer
 * 		iptr -	input string pointer
 *
 * Returns:	Pointer to final null in output buffer.
 */

#include	<ctype.h>

#include	"strutils.h"

char *
strucpy (
	char	*optr,			/* => output string		*/
	char	*iptr)			/* => input string		*/
{
	if (!iptr)	iptr = optr;

	while (*iptr)
	{
		*optr++ = _toupper(*iptr);
		iptr++;
	}
	*optr = '\0';			/* Copy a trailing null		*/
	return (optr);
}
