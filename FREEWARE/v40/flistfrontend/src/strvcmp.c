#ifndef NO_IDENT
static char *Id = "$Id: strvcmp.c,v 1.3 1995/02/19 18:32:14 tom Exp $";
#endif

/*
 * Title:	strvcmp.c
 * Author:	Thomas E. Dickey
 * Created:	05 Nov 1984
 * Last update:
 *		19 Feb 1995, prototyped
 *
 * Function:	This procedure performs a string-comparison, equating
 *		upper- and lowercase, and treating any sequence of "whitespace"
 *		(space and carriage control, including tab) as a single
 *		blank.
 *
 * Arguments:	ref_	=> reference template (an uppercased string, with
 *			   no more than one blank in a row, ending with
 *			   a null).  (See: 'strvcpy').
 *		tst_	=> the string to test the template against.  It need
 *			   end with a null, but a null will force the end-of-
 *			   compare.
 *		len	=  the maximum number of characters from 'tst_[]' to
 *			   match.
 *
 * Returns:	The number of characters from 'tst_[]' which resulted from the
 *		match with 'ref_[]', UNLESS a mismatch was found, in this case
 *		zero.
 */

#include	<ctype.h>

#include	"strutils.h"

int
strvcmp (char *ref_, char *tst_, int len)
{
	register
	char	*r_	= ref_,	/* force the pointer off the stack	*/
		*t_	= tst_;
	register
	int	matched	= 0, t;

	while (len > matched && *r_)
	{
		if (isspace(*r_))
		{
			if (!isspace(*t_))	return (0);
			while (isspace(*t_))	t_++, matched++;
			r_++;
		}
		else if (*r_)
		{
			t = toascii(*t_);
			t = _toupper(t);
			if (*r_++ != t)		return (0);
			t_++, matched++;
		}
	}
	if (*r_)		return (0);

	if (matched > len)	matched = len;
	return (matched);
}
