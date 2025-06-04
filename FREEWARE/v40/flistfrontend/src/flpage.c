#ifndef NO_IDENT
static char *Id = "$Id: flpage.c,v 1.4 1995/06/06 10:10:06 tom Exp $";
#endif

/*
 * Title:	flpage.c
 * Author:	Thomas E. Dickey
 * Created:	02 May 1985 (from code in main program)
 * Last update:
 *		18 Mar 1995, prototypes
 *		03 Jul 1985, added import-definition.  Use 'scanint' to bypass
 *			     bug in 'sscanf' in CC2.0
 *		04 May 1985
 *
 * Function:	This module performs scrolling/cursor-movement operations
 *		for FLIST.
 */

#include	<ctype.h>
#include	<string.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dds.h"
#include	"dircmd.h"
#include	"strutils.h"

import(numfiles);

tDIRCMD(flpage)
{
	DCLARG	*next_	= xdcl_->dcl_next;
	int	lines	= 0, st;
	char	command = _toupper (xdcl_->dcl_text[0]),
		*c_;

	/*
	 * Interpret the argument, if given, as the number of lines by which
	 * to scroll.  We accept only a positive argument.  A zero is used to
	 * set the direction-flag only.
	 */
	if (next_)
	{
		if (strchr ("FBP", command))
		{
			c_ = scanint (next_->dcl_text, &lines);
			if (*c_ || lines < 0)
				warn ("Illegal line count");
			else
			{
				if (command == 'B')
					dircmd_dirflg (FALSE);
				else if (command == 'F')
					dircmd_dirflg (TRUE);
				if (dircmd_dirflg (-1))
					command = DDS_D_1;
				else
					command = DDS_U_1;
				while (lines-- > 0 && !didwarn())
					flist_move (curfile_, *curfile_, command);
			}
		}
		else
			warn ("Command '%s' uses no arguments", xdcl_->dcl_text);
	}
	else switch (command)
	{
	case 'B':		/* Scroll backward on screen	*/
		dircmd_dirflg (-1);
		flist_move (curfile_, *curfile_, DDS_U_S);
		break;
	case 'E':		/* Scroll to end of list	*/
		flist_move (curfile_, numfiles-1, DDS_D_1);
		break;
	case 'F':		/* Scroll forward one screen	*/
		dircmd_dirflg (1);
		flist_move (curfile_, *curfile_, DDS_D_S);
		break;
	case 'H':		/* Move to top-of-screen	*/
		flist_move (curfile_, *curfile_, DDS_U_C);
		break;
	case 'L':		/* Move to end-of-screen	*/
		flist_move (curfile_, *curfile_, DDS_D_C);
		break;
	case 'M':		/* Move to middle-of-screen	*/
		flist_move (curfile_, *curfile_, DDS_0);
		break;
	case 'P':		/* Scroll by one page	*/
		flist_move (curfile_, *curfile_, dircmd_dirflg(-1) ? DDS_D_S
								   : DDS_U_S);
		break;
	case 'T':		/* Scroll to top of list	*/
		flist_move (curfile_, 0, DDS_U_1);
	}
}
