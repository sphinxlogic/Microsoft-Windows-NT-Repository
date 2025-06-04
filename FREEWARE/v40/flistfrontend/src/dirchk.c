#ifndef NO_IDENT
static char *Id = "$Id: dirchk.c,v 1.6 1995/06/04 19:44:26 tom Exp $";
#endif

/*
 * Title:	dirchk.c
 * Author:	Thomas E. Dickey
 * Created:	28 Jun 1984
 * Last update:
 *		19 Feb 1995, prototyped
 *		19 Sep 1985, added tests for non-DCL syntax
 *		26 Dec 1984, changed call on 'dclwild'.
 *		25 Aug 1984, cleanup of buffer sizes
 *		20 Jul 1984, added flag to make warning optional
 *		30 Jun 1984
 *
 * Function:	This module checks the parsed command line (from 'dirarg')
 *		for FLIST.  It checks for the proper number of filespecs,
 *		and the absence (or presence) of options.  A status word is
 *		used to check for permissible min/max argument lists (see
 *		symbols "v_???").  This check routine can handle the following
 *		cases:
 *
 *		(a)	one file (assumed both input, output)
 *		(b)	one file in, one file out
 *		(c)	one file in, many files out
 *		(d)	many files in, many files out
 *		(e)	many files in, one file out
 *
 * Parameters:	cmd_	=> string which was parsed to make DCLARG-list,
 *		dcl_	=> parsed form of command (DCLARG-list).
 *		state	=  (DIRCMD2.h) bit-vector defining command requirements
 *		flg	=  TRUE iff errors should be reported via 'warn'.
 *
 * Returns:	TRUE if no errors are found.  If an error is found, an
 *		appropriate message is emitted via 'warn'.
 */

#include	<stdio.h>
#include	<ctype.h>

#include	<rms.h>

#include	"flist.h"
#include	"names.h"
#include	"dircmd.h"

int	dirchk (char *cmd_, DCLARG *dcl_, int state, int flg)
{
DCLARG	*d_;
int	numinp	= 0,		/* number of items in subfield-1	*/
	numout	= 0,		/* number of items in subfield-2	*/
	numopt	= 0,		/* number of option-fields found	*/

	wldinp	= 0,		/* number of inputs with wildcards	*/
	wldout	= 0,		/* number of outputs with wildcards	*/

	nummax	= 0,		/* number of parmeters past output	*/
	numcmd	= 0;		/* should be 1, if larger have extra ","*/
char	*msg_	= nullC,	/* set iff warning message formed	*/
	msgbfr	[MAX_PATH];

	/*
	 * See what is in the argument list, and then check the result
	 * for adherence to the desired state.
	 */
	for (d_ = dcl_; d_; d_ = d_->dcl_next)
	{
		char	*s_ = d_->dcl_text;
		if (isopt(*s_))
			numopt++;
		else switch (d_->dcl_mfld)
		{
		case 0:	numcmd++;		/* have a comma in command? */
			break;
		case 1:	numinp++;
			if (dclwild(d_))	wldinp++;
			break;
		case 2:	numout++;
			if (dclwild(d_))	wldout++;
			break;
		default:nummax++;		/* too many parameters */
		}
	}

#define	MODE(m)	(state & (m))
#define	ONE_IN	MODE(v_1_IN)
#define	ONE_OUT	MODE(v_1_OUT)

#define	ANY_IN	MODE(v_1_IN  + v_M_IN)
#define	ANY_OUT	MODE(v_1_OUT + v_M_OUT)

#define	FREE	MODE(v_FREE)

#define	NONE(t)	sprintf (msg_ = msgbfr, "%s uses no t", dcl_->dcl_text);

	if (MODE(v_nonDCL))
	{
		if (isopt(dcl_->dcl_text[0]))	numopt--, numcmd++;
		if (!MODE(v_ARGS)
		&&  ((numout + numinp) > 0 || numcmd > 1 || numopt > 0))
			NONE(arguments);
	}
	else if (numcmd > 1)
		msg_ = "extraneous ',' in command field";
	else if (numinp > 1 && ONE_IN)
		msg_ = "too many input fields";
	else if (numinp <= 0 && ANY_IN && !FREE)
		msg_ = "need a filespec";
	else if (nummax > 0
	||	 (numout > 0 && !ANY_OUT)
	||	 (numinp > 0 && !ANY_IN) )
		msg_ = "too many parameters";
	else if (numout > 1 && ONE_OUT)
		msg_ = "too many output fields";
	else if (numout <= 0 && ANY_OUT && !FREE)
		msg_ = "need an output filespec";
	else if (wldinp && !(state & v_W_IN))
		msg_ = "no wildcard permitted in input spec";
	else if (wldout && !(state & v_W_OUT))
		msg_ = "no wildcard permitted in output spec";
	else if (numopt > 0 && !(state & v_OPTS))
		NONE(options);

	if (msg_ && flg)		warn (msg_);
	return (msg_ == nullC);
}
