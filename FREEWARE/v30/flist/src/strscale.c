/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static	char	*Id = "$Id: strscale.c,v 1.3 1995/02/19 18:20:55 tom Exp $";
#endif

/*
 * Title:	strscale.c
 * Author:	Thomas E. Dickey
 * Created:	06 Jun 1984
 * Last update:
 *		19 Feb 1995, prototypes
 *
 * Function:	This procedure creates a column-scale, with numbers for use in
 *		editors, etc.  For example, if the initial column is 1, then
 *		a scale such as the following might be returned:
 *
 *		"....+...10....+...20....+...30"
 *
 *		If the starting column is not 1, the procedure properly starts
 *		on the substring of the global scale.
 *
 * Parameters:	co_	=> output buffer
 *		col	=  beginning column number
 *		len	=  length of output buffer
 */

#include <stdio.h>
#include <string.h>

#include "strutils.h"

static	char	ten[] = "....+....|";

void
strscale (
	char	*co_,
	int	col,
	int	len)
{
	int	first	= 1,		/* flag for leading 0's column	*/
		numlen;
	char	number[20],
		catbfr[20];

	if (col < 1)	col = 1;
	*co_ = '\0';

	while (len > 0)
	{
		int	base10	= (col / 10) * 10,
			next10	= base10 + 10,
			units	= col - base10;
		char	*s_	= catbfr;

		sprintf (number, "%d", next10);
		numlen	= strlen (number);
		if (units)
		{
			strcpy (catbfr, &ten[units-1]);
			units = strlen (catbfr);
			if (units > numlen + 2)
				strcpy (&catbfr[units-numlen], number);
		}
		else
		{
			if (first)
				*co_++ = '|', *co_ = '\0', len--;
			strcpy (catbfr, ten);
			strcpy (&catbfr[10-numlen], number);
		}
		while (*s_ && len > 0)
		{
			*co_++ = *s_++;
			*co_   = '\0';
			len--;
		}
		col	= next10;
		first	= 0;
	}
}
