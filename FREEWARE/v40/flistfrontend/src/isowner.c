#ifndef NO_IDENT
static char *Id = "$Id: isowner.c,v 1.7 1998/10/25 00:52:43 tom Exp $";
#endif

/*
 * Title:	isowner.c
 * Author:	T.E.Dickey
 * Created:	20 Dec 1984
 * Last update:
 *		24 Oct 1998, limit getuid/getgid compares to 16-bits
 *		25 Oct 1995, missing parenthesis
 *		19 Feb 1995, str/sys utils prototypes
 *		04 Nov 1988, removed masking from getuid/getgid calls.
 *		12 Nov 1985, patch (for v4.x) to use only low-byte of uid codes
 *		20 Dec 1984
 *
 * Function:	Test (for FLIST) a file's UIC to see if it is either owned
 *		by the current process, or if the process has sufficient
 *		privilege to modify it.
 *
 * Arguments:	z	=> FILENT structure defining file.
 */

#include	<stdio.h>
#include	<string.h>
#include	<unixlib.h>	/* for 'getuid()' */

#include	<prvdef.h>

#include	"flist.h"
#include	"dirent.h"

#include	"strutils.h"
#include	"sysutils.h"

#define	CMP(func,offset) ((func() & 0xffff) != z->offset)

int	isowner (FILENT *z)
{
	char	msg	[CRT_COLS];

	if ((CMP(getgid,f_grp) || CMP(getuid,f_mbm))
	&& ! sysrights(PRV$M_SYSPRV,0))
	{
		strcpy (msg, "You are not the file's owner");
		if (z->f_grp == -1)
			strcat (msg, "(NO Privilege)");
		else
			sprintf (strnull(msg), ": [%03o,%03o]",
				z->f_grp, z->f_mbm);
		warn2 (msg);
		return (FALSE);
	}
	return (TRUE);
}
