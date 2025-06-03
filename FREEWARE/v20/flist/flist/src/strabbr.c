/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: strabbr.c,v 1.3 1995/02/19 02:15:01 tom Exp $";
#endif

/*
 * Title:	strabbr.c
 * Author:	Thomas E. Dickey
 * Created:	11 Jul 1984
 * Last update:
 *		18 Feb 1995, prototyped
 *		01 Sep 1984, use '_toupper'
 *
 * Function:	Compare two strings, ignoring case, to see if the first is
 *		an allowable abbreviation of the second.
 *
 * Parameters:	tst_	=> string to test
 *		ref_	=> string to use as a reference
 *		cmplen	=  length of 'tst_' to compare (should be determined
 *			   by either 'strlen', or delimiter).
 *		abbr	=  minimum match length needed for legal abbreviation
 *
 * Returns:	TRUE if the strings are matched.
 */

#include	<ctype.h>

#include	"strutils.h"

int
strabbr (
	char	*tst_,
	char	*ref_,
	int	cmplen,
	int	abbr)
{
	while (*tst_ && *ref_ && (cmplen > 0))
	{
		if (_toupper(*tst_) != _toupper(*ref_))	break;
		tst_++;
		ref_++;
		cmplen--;
		abbr--;
	}
	return ((abbr <= 0) && (cmplen <= 0));
}
