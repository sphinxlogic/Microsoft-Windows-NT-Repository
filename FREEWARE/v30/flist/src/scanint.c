/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef	NO_IDENT
static	char	*Id = "$Id: scanint.c,v 1.4 1995/10/22 21:17:33 tom Exp $";
#endif

/*
 * Title:	scanint.c
 * Author:	Thomas E. Dickey
 * Created:	17 Jun 1985
 * Last update:
 *		19 Feb 1995, prototypes
 *		04 Jul 1985, stepped on my good copy!!
 *
 * Function:	Translate an unsigned decimal integer, returning a pointer past
 *		the decoded part.
 *
 * Arguments:	string	=> string to decode
 *		int_	= address of number to load, iff a number is found
 *
 * Returns:	Address of first character past the string of decimal digits.
 */

#include	<ctype.h>

#include	"strutils.h"

char *
scanint (char *string, int *int_)
{
	register int first = 1;

	while (isdigit(*string))
	{
		if (first)	first = *int_ = 0;
		*int_	= (*int_ * 10) + (*string++ - '0');
	}
	return (string);
}
