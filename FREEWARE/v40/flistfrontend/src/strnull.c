#ifndef	NO_IDENT
static	char	*Id = "$Id: strnull.c,v 1.3 1995/02/19 18:33:17 tom Exp $";
#endif

/*
 * Title:	strnull
 * Author:	Thomas E. Dickey
 * Created:	03 Aug 1983
 * Last update:
 *		19 Feb 1995, prototyped
 *
 * Function:	Return a pointer to the null which ends (by convention) a
 *		character string.
 */

#include	"strutils.h"

char *
strnull (char *cptr)
{
	while (*cptr)
		cptr++;
	return (cptr);
}
