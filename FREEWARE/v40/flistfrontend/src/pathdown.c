#ifndef NO_IDENT
static char *Id = "$Id: pathdown.c,v 1.6 1995/06/06 12:58:22 tom Exp $";
#endif

/*
 * Title:	pathdown.c
 * Author:	Thomas E. Dickey
 * Created:	02 Jul 1984, recode from 'dirent.c' module
 * Last update:
 *		19 Feb 1995, prototypes
 *		10 Sep 1985, calls using FILENT-arguments may have a trailing
 *			     '.' in 'name_' field.
 *		09 Sep 1984, use "rmsinit"
 *		25 Aug 1984
 *
 * Function:	Given a filespec, do a parse to obtain the pathname portion,
 *		and then create a character string containing the pathname of a
 *		subdirectory whose name is given.  For example, given
 *
 *			"DBC4:[DICKEY]"	- pathname in
 *			"XX" - directory name
 *		obtain
 *			"DBC4:[DICKEY.XX]"
 *
 *		This procedure tests for the "[0,0]" UIC-code, but no others,
 *		since I cannot experiment much on numeric UIC's.
 *
 * Parameters:	co_	=> output buffer (NAM$C_MAXRSS bytes)
 *		ci_	=> input filespec/pathname
 *		name_	=> name of dependent directory
 *
 * Returns:	TRUE if no error was detected.  Errors include parse-errors,
 *		and attempts to recur on the "[0,0]" name "000000".
 */

#include	<stdio.h>
#include	<string.h>

#include	<starlet.h>
#include	<rms.h>

#include	"bool.h"
#include	"pathup.h"
#include	"rmsinit.h"

int	pathdown (char	*co_, char *ci_, char *name_)
{
int	len;
int	j,
	rootUIC = TRUE;
char	delim,	delim2;
struct	FAB	fab;
struct	NAM	nam;
char	tmp	[NAM$C_MAXRSS],
	esa	[NAM$C_MAXRSS];

	strcpy (co_, ci_);		/* (return something, even if bad)*/

	rmsinit_fab (&fab, &nam, 0, ci_);
	rmsinit_nam (&nam, 0, esa);

	if (RMS$_NORMAL != sys$parse(&fab))	return (FALSE);

	strncpy (co_, nam.nam$l_node, len = nam.nam$b_esl);
	co_[len] = '\0';

	/* cf: NAME_DOT in DIRENT.H */
	strcpy (tmp, name_);
	len	= strlen (name_ = tmp);
	if (tmp[len-1] == '.')	tmp[--len] = '\0';

	len	= nam.nam$l_name - nam.nam$l_node;
	delim	= co_[len-1];
	delim2	= (delim == ']') ? '[' : '<';

	for (j = len-2; co_[j] != delim2 && j >= 0; j--)
	{
		if (co_[j] != '0' && co_[j] != ',')
		{
			rootUIC = FALSE;
			break;
		}
	}

	if (rootUIC && co_[j] == delim2)
	{
		if (strcmp (name_, "000000") == 0)
			return (FALSE);
		sprintf (&co_[j+1], "%s%c", name_, delim);
	}
	else
		sprintf (&co_[len-1], ".%s%c", name_, delim);
	return (TRUE);
}
