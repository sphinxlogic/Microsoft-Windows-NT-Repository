/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: dirhigh.c,v 1.6 1995/06/04 22:42:18 tom Exp $";
#endif

/*
 * Title:	dirhigh.c
 * Author:	Thomas E. Dickey
 * Created:	18 Jul 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		24 Aug 1985, use 'dds_add2' instead of 'dirdata_one'
 *		20 Jul 1985, use 'dirent_nul' to make dummy entry in 'filelink'.
 *		16 Jul 1985, use 'filelist' as pointers to 'filelink'.
 *		03 Jul 1985, cleanup of 'filelist' definition
 *		14 Dec 1984, added nam-argument to 'dirent_chop'
 *		25 Aug 1984, cleanup buffer sizes
 *
 * Function:	This module is called from FLIST when a file-modification
 *		operation is used with the (FLIST option) /NOVERSION.  The
 *		intent of NOVERSION is to show on the display only the highest
 *		version of any particular file.  This module determines the
 *		highest version of the particular file, locates in 'filelist[]'
 *		(or adds) the entry to be updated, and displays the updated
 *		line.
 *
 * Parameters:	filespec[] = file specification to use.
 *
 * Returns:	TRUE if any version of the given filename is found; otherwise
 *		assume that the file does not exist.
 */

#include	<rms.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dds.h"

import(filelist); import(numfiles); import(numdlets);

int	dirhigh (char *filespec)
{
	FILENT	zold, znew;
	char	highspec[MAX_PATH];
	int	j,
		lower	= -1,		/* Set if version to "delete"	*/
		same	= -1;		/* Set if version to update	*/

	dirent_chop (&zold, filespec, 0);
	znew	   = zold;
	znew.fvers = 0;			/* Force current-version	*/
	dirent_glue (highspec, &znew);

	if (dirent_chk (&znew, highspec))
	{
		/*
		 * Find any occurrence of this PATH+NAME+TYPE in 'filelist[]'.
		 */
		for (j = 0; j < numfiles; j++)
		{
#define	SAMEP(p) (znew.p == FK(j).p)
			if (! DELETED(j)
			&&  SAMEP(fpath_) && SAMEP(fname) && SAMEP(ftype))
			{
				if (znew.fvers > FK(j).fvers)
					lower	= j;
				else if (znew.fvers <= FK(j).fvers)
					same	= j;
			}
		}

		if (same >= 0)
		{
			if (lower >= 0)		/* RENAME to lower version ? */
			{
				dirent_nul (lower);
				dds_line(same);
			}
			if (znew.fvers == FK(same).fvers)
				dds_add2 (&znew, same);
		}
		else if (lower >= 0)
			dds_add2 (&znew, lower);
		else
			dds_add (&znew);
		return (TRUE);
	}
	else
		return (FALSE);	/* file not found */
}
