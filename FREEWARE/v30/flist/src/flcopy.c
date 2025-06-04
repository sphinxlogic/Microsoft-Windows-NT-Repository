/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: flcopy.c,v 1.4 1995/06/06 00:38:10 tom Exp $";
#endif

/*
 * Title:	flcopy.c
 * Author:	Thomas E. Dickey
 * Created:	11 Jul 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		24 Aug 1985, use 'dds_add2' instead of 'dirdata_one'
 *		19 Jul 1985, use 'dirdata_one', corrected "COPY / ;" results for
 *			     /NOVERSION (file appears to have been in list already).
 *		03 Jul 1985, cleanup 'filelist' definition
 *		11 Jun 1985, added cli-argument to dds_spawn
 *		14 Dec 1984, added nam-argument to 'dirent_chop'
 *		03 Sep 1984, use "import"
 *		25 Aug 1984
 *		26 Jul 1984, modified 'dirseek()'
 *		25 Jul 1984, amended REPLACE-clause
 *		18 Jul 1984
 *
 * Function:	Spawn an external process (COPY or APPEND).  This references
 *		multiple input files, and a single (for simplicity) output
 *		file.  Update the output file on the screen on completion of
 *		the command.
 *
 *		The inputs need not be members of 'filelist[]', since they
 *		are not modified.
 *
 * Patch:	The version-defaulting of COPY differs slightly from other VMS
 *		commands.  If the files X and Y exist (same version numbers),
 *		then the commands
 *
 *			COPY X Y
 *			COPY X; Y
 *
 *		are treated differently.  The first command creates a new
 *		version (Y;"+1") with no complaint.  The second complains about
 *		conflicting version numbers and does not do the copy.
 *
 * Patch:	Another problem is equally annoying, but could be fixed:
 *		Suppose there are two files X.C;1 and Y.C;2, and that I issue
 *		the command
 *
 *			COPY / Y  (implicitly "COPY X.C;1 Y" => Y.C;)
 *
 *		Then I get an error message from COPY about a higher version,
 *		but (since I am basically checking only for new highest
 *		version), my code doesn't see the new lower version.
 */

#include	<string.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dircmd.h"
#include	"dirseek.h"
#include	"dds.h"

import(filelist);
import(V_opt);

tDIRCMD(flcopy)
{
	FILENT	znew,	*z = &znew;
	char	*out_	= dclinx (xdcl_, 2, 0),
		filespec[MAX_PATH];
	int	oldinx	= dirent_old (out_, 0),	/* is >= 0 if in 'filelist[]'	*/
		newinx,
		cp_cmd	= (strcmp ("COPY", xdcl_->dcl_text) == 0);

	/*
	 * Verify that all of the inputs exist, and that the output's
	 * directory is writeable:
	 */
	dds_while (nullC);	/* there may be a wait on the seek */
	if (! dirseek (xcmd_, xdcl_, 1, TRUE))	return;

	/*
	 * Parse the output name, to obtain a full (or maximum) specification.
	 * At the very least, this will give the name of the directory of the
	 * output file, permitting me to verify that I can write in that
	 * directory before attempting the COPY.  Note that if no version is
	 * given, the chop'd form will be 0.
	 */
	dirent_chop (&znew, out_, 0);
	if (! diropen (zPATHOF(z)))		return;

	/*
	 * If all parameters are ok, perform the copy:
	 */
	dds_spawn (0, *curfile_, xcmd_, 0, FALSE, 1);

	/*
	 * Now, update the directory display.  Unlike EDIT and RENAME, put
	 * the new entry elsewhere on the list (unless a REPLACE option was
	 * used, causing the old and new entries to be coincident).
	 */

	if (oldinx >= 0)	/* (Check for changes if COPY/REPLACE)	*/
	{
		dirent_glue (filespec, FK_(oldinx));
		dirent_chk (FK_(oldinx), filespec);
		dds_line (oldinx);

				/* Check further for new version	*/
		if ((newinx = dirent_old_any (&znew, out_, 0)) >= -1)
		{
			if (newinx == -1)
			{
				if (V_opt)	dds_add (&znew);
				else		dirhigh (filespec);
			}
			else if (newinx != oldinx)
				dds_add2 (&znew, newinx);
		}
		/* (else, COPY failed) */
	}
	/*
	 * If I can find something here, I did indeed create a new file.
	 * However, if /NOVERSION is selected, display this only if there
	 * are no higher versions around.  If there is a lower version,
	 * overwrite the corresponding 'filelist[]' entry.
	 */
	else if (dirent_chk (&znew, out_))
	{
		if ((oldinx = dirent_old (out_, 0)) < 0)
		{
			if (V_opt)	dds_add (&znew);
			else		dirhigh (out_);
		}
		else
			dds_add2 (&znew, oldinx);
	}
}
