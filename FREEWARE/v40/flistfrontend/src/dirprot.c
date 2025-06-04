#ifndef NO_IDENT
static char *Id = "$Id: dirprot.c,v 1.4 1995/02/19 18:23:39 tom Exp $";
#endif

/*
 * Title:	dirprot.c
 * Author:	Thomas E. Dickey
 * Created:	10 Jul 1984
 * Last update:
 *		19 Feb 1995, prototyped
 *		25 Aug 1984, cleanup buffer sizes
 *
 * Function:	This procedure is used by FLIST to check access rights of
 *		a list of files, specified in a DCLARG-list.  For example,
 *		to print a list of files, they must all be readable.  This
 *		code uses 'getprot' to perform wildcard (worst-case) testing.
 *
 * Parameters:	cmd_	=> original command text (for nicer messages).
 *		d_	=> DCLARG-list to test. (Options are ignored.)
 *		mfld	=  parameter number to test (e.g., 1=input, 2=output)
 *		mode_	=> String of modes to test (lowercase "rwed", any order).
 *
 * Returns:	TRUE if all accesses are permitted.  If not, a warning message
 *		is generated.
 */

#include	<stdio.h>

#include	"flist.h"
#include	"dclarg.h"
#include	"getprot.h"

dirprot (cmd_, d_, mfld, mode_)
char	*cmd_;
DCLARG	*d_;
int	mfld;
char	*mode_;
{
GETPROT	tstval;
char	*failed	= 0;

static
char	sFMT1[]	= "File(s) not found: %%.%ds",
	sFMT2[]	= "No access: %%.%ds (%%s needed)";
char	format[sizeof(sFMT2)+6];

	for (; d_; d_ = d_->dcl_next)
	{
		if (isopt (d_->dcl_text[0]))	continue;
		else if (d_->dcl_mfld != mfld)	continue;

		if (getprot (&tstval, d_->dcl_text))
			failed = sFMT1;
		else if (! cmpprot (&tstval, mode_))
			failed = sFMT2;

		if (failed)
		{
			sprintf (format, failed, d_->dcl_size);
			warn (format, &cmd_[d_->dcl_from], mode_);
			return (FALSE);
		}
	}
	return (TRUE);
}
