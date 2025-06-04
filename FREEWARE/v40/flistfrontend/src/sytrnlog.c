#ifndef	NO_IDENT
static	char	*Id = "$Id: sytrnlog.c,v 1.6 1995/10/19 10:39:40 tom Exp $";
#endif

/*
 * Title:	systrnlog.c
 * Author:	Thomas E. Dickey
 * Created:	11 Apr 1985
 * Last update:
 *		19 Oct 1995, clean compile with DEC C.
 *		19 Feb 1995, str/sys utils prototypes
 *		11 Apr 1985
 *
 * Function:	Translate the given name 'tofind' until no more logical
 *		symbol substitutions can be made.  The result string must
 *		be long enough for the longest VMS name (255).
 *
 * Arguments:	result	- result buffer
 *		tofind	- name to find
 */

#include	<starlet.h>
#include	<string.h>
#include	<rms.h>
#include	<iodef.h>
#include	<descrip.h>
#include	<ssdef.h>

#include	"bool.h"

#include	"strutils.h"
#include	"sysutils.h"

void
systrnlog (char	*result, char *tofind)
{
	struct
	dsc$descriptor_s	Idsc, Odsc;
	int	status	= 0;
	short	rsllen;

	strucpy (result, tofind);	/* Make sure name is in caps */
	while (status != SS$_NOTRAN)
	{
		Idsc.dsc$a_pointer= result;
		Idsc.dsc$w_length = strlen(result);
		Idsc.dsc$b_dtype  = DSC$K_DTYPE_T;
		Idsc.dsc$b_class  = DSC$K_CLASS_S;
		Odsc = Idsc;
		Odsc.dsc$w_length = NAM$C_MAXRSS;
		status = sys$trnlog (&Idsc, &rsllen, &Odsc, 0,0,0);
		result[rsllen] = EOS;
	}
}
