/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: pathup.c,v 1.7 1995/06/06 13:28:46 tom Exp $";
#endif

/*
 * Title:	pathup.c
 * Author:	Thomas E. Dickey
 * Created:	02 Jul 1984, recode from 'dirent.c' module
 * Last update:
 *		19 Feb 1995, str/sys utils prototypes
 *		11 Apr 1985, do a translate-logical before trying to scan the
 *			     pathname.  Some system stuff does logical-names for
 *			     subdirectories.
 *		09 Sep 1984, use 'rmsinit'
 *		25 Aug 1984
 *
 * Function:	Given a filespec, do a parse to obtain the pathname portion,
 *		and then determine the name of the parent directory.  For
 *		example, given
 *
 *			"DBC4:[DICKEY.EXE]"	- pathname in
 *		obtain
 *			"DBC4:[DICKEY]"
 *
 * Parameters:	co_	=> output buffer (NAM$C_MAXRSS bytes)
 *		ci_	=> input filespec/pathname
 *
 * Returns:	TRUE if no error was detected.
 */

#include	<starlet.h>
#include	<stdio.h>
#include	<string.h>
#include	<rms.h>

#include	"bool.h"
#include	"pathup.h"
#include	"rmsinit.h"

#include	"strutils.h"
#include	"sysutils.h"

int	pathup (char *co_, char *ci_)
{
	int	len,	j,	first,	rootUIC = TRUE;
	struct	FAB	fab;
	struct	NAM	nam;
	char	delim,
		esa	[NAM$C_MAXRSS],
		strn	[NAM$C_MAXRSS];

	strcpy (co_, ci_);		/* (return something, even if bad)*/

	/*
	 * We do a parse-loop to handle (possibly) two substitutions.  Most
	 * file specifications will fall through in one scan, e.g., DBC4 will
	 * immediately evaluate to a physical device name.  However, we may
	 * have logical assignments such as the ones in the system:
	 *
	 *	SYS$MANAGER: => SYS$SYSROOT:[SYSMGR] => __DRA0:[SYS0.SYSMGR]
	 *
	 * The second step is the crucial one; we must translate SYS$SYSROOT
	 * to find the actual location (__DRA0:[SYS0]) of the SYS$MANAGER
	 * directory file SYSMGR.DIR;1.  The intermediate logical is
	 *
	 *	SYS$SYSROOT => __DRA0:[SYS0.]
	 *
	 * The trailing ".]" permits use to refer to SYS$SYSROOT as a true
	 * logical device (with dependent directories), but confuses the
	 * path-up translation.
	 */
	for (;;)
	{
		rmsinit_fab (&fab, &nam, 0, co_);
		rmsinit_nam (&nam, 0, esa);

		if (RMS$_NORMAL != sys$parse(&fab))	return (FALSE);

#define	COPY(dst,f)\
		strncpy(dst, nam.nam$l_node, len = nam.f - nam.nam$l_node);\
		dst[len] = EOS

		esa[nam.nam$b_esl] = EOS;

		COPY(strn,nam$l_dir-1);	/* Extract the device name	*/
		systrnlog (co_, strn);	/* Expand it...			*/
		if (strcmp(co_, strn))	/* Substitution done ?		*/
		{
			if (co_[j=(strlen(co_)-1)] == ']')
			{
				if (co_[j-1] == '.')	j--;
				ci_ = nam.nam$l_dir;
				if (*ci_ == '[')
					sprintf (&co_[j], ".%s", ci_+1);
				else
					sprintf (&co_[j], "]%s", ci_);
			}
			else
				break;
		}
		else
			break;
	}

	/*
	 * Copy the expanded string, truncating it at the end of the directory,
	 * e.g.,
	 *	DBC4:[DICKEY]FILE.TMP
	 * becomes
	 *	DBC4:[DICKEY]
	 */
	COPY(co_,nam$l_name);

	first	= nam.nam$l_dir  - nam.nam$l_node;
	delim	= co_[len-1];

	for (j = len-2; j > first; j--)
	{
		if (co_[j] != '0' && co_[j] != ',')	rootUIC = FALSE;
		if (co_[j] == '.')	break;
	}

	strncpy (esa, &co_[j+1], len -= j+2);
	esa[len] = EOS;

	if (j == first)
	{
		strcpy (&co_[j+1], "0,0");
		if (rootUIC)
			strcpy (esa, "000000");
	}
	else
		co_[j] = EOS;
	sprintf (strnull(co_), "%c%s.DIR", delim, esa);
	return (TRUE);
}
