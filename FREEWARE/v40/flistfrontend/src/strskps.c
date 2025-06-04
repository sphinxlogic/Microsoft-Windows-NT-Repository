#ifndef NO_IDENT
static char *Id = "$Id: strskps.c,v 1.3 1995/02/19 02:18:17 tom Exp $";
#endif

/*
 * Title:	strskps.c
 * Author:	Thomas E. Dickey
 * Created:	14 Sep 1983
 * Last update:
 *		18 Feb 1995, prototyped
 *		29 Dec 1984, to use ctype-include
 *		05 Dec 1983
 *
 * Function:	Skip over spaces and other blank characters.
 *
 * Parameters:	c_	=> string to skip over.
 *
 * Returns:	Updated pointer, after skipping blanks.  If null is found,
 *		the procedure stops on null.
 */

#include	<ctype.h>

#include	"strutils.h"

char *
strskps (char *c_)
{
	if (c_)
	{
		while (*c_)
			if (isspace(*c_))
				c_++;
			else
				break;
	}
	return (c_);
}
