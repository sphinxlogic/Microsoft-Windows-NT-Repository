#ifndef NO_IDENT
static char *Id = "$Id: dclchk.c,v 1.4 1995/06/04 19:06:22 tom Exp $";
#endif

/*
 * Title:	dclchk.c
 * Author:	Thomas E. Dickey
 * Created:	28 May 1984
 * Last update:
 *		19 Feb 1995, prototypes
 *		26 Aug 1984, cleanup buffer sizes
 *		14 Jul 1984, use $GETMSG for most messages.
 *		28 Jun 1984
 *
 * Function:	Test the DCLARG-list given for error conditions, print an
 *		appropriate error message if found.
 *
 * Parameters:	dcl_	=> DCLARG list to check
 *		co_	=> buffer into which to return error message (suppress
 *			   actual print).
 *
 * Returns:	TRUE if an error is found, else FALSE (0).
 */

#include	<stdio.h>
#include	<string.h>
#include	<rms.h>

#include	"bool.h"
#include	"crt.h"
#include	"dclarg.h"

#include	"strutils.h"
#include	"sysutils.h"

int
dclchk (DCLARG *dcl_, char *co_)
{
	unsigned status;
	int	j;
	char	*form_	= 0,
		gotmsg	[CRT_COLS],
		bfr	[CRT_COLS];

	for (; dcl_; dcl_ = dcl_->dcl_next)
	{
		if (status = dcl_->dcl_stat)
		{
			if (status == -1)
				form_ = dcl_->dcl_text;
			else
			{
				sysgetmsg (status, gotmsg, sizeof(gotmsg));
				strform2 (bfr, sizeof(bfr),
					gotmsg, 0, dcl_->dcl_text, 0);
				form_	= bfr;
			}
			if (co_)
				strcpy (co_, form_);
			else
				printf ("%s\n", form_);
			return (TRUE);	/* TRUE, found an error	*/
		}
	}
	return (FALSE);
}
