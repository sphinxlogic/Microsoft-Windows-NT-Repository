#ifndef NO_IDENT
static char *Id = "$Id: tabexp.c,v 1.2 1985/01/12 15:38:32 tom Exp $";
#endif

/*
 * Title:	tabexp.c
 * Author:	Thomas E. Dickey
 * Created:	12 Jan 1985
 * Last update:	12 Jan 1985
 *
 * Function:	Expand all of the tabs in a string into spaces.
 *
 * Arguments:	obuf	=> output buffer
 *		ibuf	=> input buffer
 */

#include	<ctype.h>

#include	"bool.h"

tabexp (obuf, ibuf)
char	*obuf, *ibuf;
{
register
char	c,
	*i_ = ibuf,
	*o_ = obuf;
register
int	column = 0, next;

	while (*i_)
	{
		if ((c = *i_++) == '\t')
		{
			c = ' ';
			next = column | 7;
			while (column < next)
			{
				*o_++ = c;
				column++;
			}
		}
		else if (c == '\b')
			column = (column > 0) ? column-1 : 0;
		else if (c == '\r')
			column = 0;
		if (isprint(c))	column++;
		*o_++ = c;
	}
	*o_ = EOS;
}
