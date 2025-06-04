#ifndef NO_IDENT
static char *Id = "$Id: strvcpy.c,v 1.3 1995/02/19 18:33:08 tom Exp $";
#endif

/*
 * Title:	strvcpy.c
 * Author:	Thomas E. Dickey
 * Created:	05 Nov 1984
 * Last update:
 *		19 Feb 1995, prototyped
 *
 * Function:	Copy a string, making it uppercase, with no more than one
 *		blank in a row.  All blanks are replaced by the space
 *		character.
 *
 * Arguments:	co_	=> output string
 *		ci_	=> input string.  If null, assume 'co_'.
 */

#include	<ctype.h>

#include	"strutils.h"

void
strvcpy (char *co_, char *ci_)
{
	register
	char	c = ' ', d = '?';

	if (!ci_)	ci_ = co_;
	do {
		c = toascii(*ci_);
		c = _toupper(c);
		ci_++;
		if (isspace(c))
		{
			if (!isspace(d))	*co_++ = ' ';
		}
		else
			*co_++ = c;
		d = c;
	} while (c);
}
