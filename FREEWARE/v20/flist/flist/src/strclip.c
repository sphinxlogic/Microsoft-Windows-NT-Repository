/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: strclip.c,v 1.1 1995/10/22 21:48:56 tom Exp $";
#endif

/*
 * Title:	strclip.c
 * Author:	Thomas E. Dickey
 * Created:	22 Oct 1995
 * Last update:
 *
 * Function:	Remove the first occurrence of the given substring from the
 *		argument, returning true if found.
 *
 * Parameters:	inout	= the string to remove from
 *		clip	= the substring to remove
 */

#include <string.h>

#include "bool.h"
#include "strutils.h"

int
strclip (
	char	*inout,
	char	*clip)
{
	char	*found = strstr(inout, clip);

	if (found != 0) {
		size_t len = strlen(clip);
		while ((found[0] = found[len]) != EOS)
			found++;
		return TRUE;
	}
	return FALSE;
}
