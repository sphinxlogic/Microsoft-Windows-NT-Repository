#ifndef	NO_IDENT
static	char	*Id = "$Id: strtrim.c,v 1.4 1995/06/06 13:41:18 tom Exp $";
#endif

/*
 * Title:	strtrim
 * Author:	Thomas E. Dickey
 * Created:	11 Aug 1983
 * Last update:
 *		18 Feb 1995, prototypes
 *
 * Function:	This procedure trims trailing "blank" characters from a null-
 *		ended ASCII string, by replacing the blanks with nulls.
 *
 * Parameters:	c_ 	=> string to trim
 *
 * Returns:	The length of the string after trimming it.
 */

#include	<ctype.h>
#include	<string.h>

#include	"strutils.h"

int
strtrim (char *c_)
{
	int	len;

	for (len = strlen(c_--);
		(len > 0) && isspace(*(c_+len));
			*(c_+len--) = '\0');
	return (len);
}
