#ifndef NO_IDENT
static char *Id = "$Id: flread.c,v 1.6 1995/06/06 10:23:28 tom Exp $";
#endif

/*
 * Title:	flread.c
 * Author:	Thomas E. Dickey
 * Created:	24 Jul 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		03 Sep 1985, pass-thru 'UPDATE' flag to 'dirent_all'
 *		05 Feb 1985, had coded for-loop wrong.
 *		01 Feb 1985, added another "working..." call (dds_while).
 *		31 Jan 1985, use 'dirread' module to maintain read-list.
 *		06 Dec 1984, use 'nameheap' to store 'readlist'
 *		02 Sep 1984, use "import"
 *		27 Jul 1984
 *
 * Function:	This module performs the READ function for "FLIST".  READ
 *		reads new (or re-reads old) entries into the 'filelist[]'
 *		array.
 *
 * 		In 'dirent', we set up and maintain 'readlist', to see which
 *		arguments of READ are new.  Then, if no arguments are given,
 *		simply re-read the entire list.  (Note that this in itself
 *		will not get rid of files added by EDIT, which may have been
 *		deleted by other processes.)
 *
 *		There are actually two commands which call this module:
 *
 *		UPDATE	- directs the program to re-read all directory entries
 *			  which match the argument specification.
 *		READ	- directs the program to read only those entries which
 *			  were not previously in the database.
 */

#include	<signal.h>	/* for 'sleep()' */

#include	"flist.h"

#include	"dircmd.h"
#include	"dds.h"
#include	"dirent.h"
#include	"dirread.h"

tDIRCMD(flread)
{
	FILENT	ztmp,	*z = &ztmp;
	register
	int	j,
		update	= xdcl_->dcl_text[0] == 'U';
	char	filespec[MAX_PATH];

	if (xdcl_ = xdcl_->dcl_next)
	{
		for (; xdcl_; xdcl_ = xdcl_->dcl_next)
		{
			dds_while (nullC);
			dirent_all (xdcl_->dcl_text, update);
		}
	}
	else
	{
		for (j = 0; dirread_get (filespec, j); j++)
		{
			flist_tell ("READ: %s", filespec);
			dds_while (nullC);
			sleep(1);
			dirent_all (filespec, update);
		}
	}

	dds_all (dds_fast(DDS_U_C), *curfile_);	/* update the screen	*/
}
