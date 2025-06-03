/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: flrnam.c,v 1.10 1995/10/21 18:42:11 tom Exp $";
#endif

/*
 * Title:	flrnam.c
 * Author:	T.E.Dickey
 * Created:	15 May 1984
 * Last update:
 *		18 Mar 1995, dircmd prototypes
 *		19 Feb 1995, sys utils prototypes
 *		09 Feb 1989, for vms5.0, may have to alter protection to rename
 *		24 Aug 1985, use 'dds_add2' instead of 'dirdata_one'
 *		31 Jul 1985, added log-file tracking
 *		27 Jul 1985, provide wildcard-rename via 'dirseek_spec2' loop.
 *		19 Jul 1985, use 'dirdata_ren'.
 *		16 Jul 1985, corrected use of 'filelist' (=> filelink).
 *		03 Jul 1985, cleanup 'filelist' definition.
 *		09 Mar 1985, add code for special case of renamed-directory.
 *		21 Dec 1984, use 'flist_sysmsg'.
 *		10 Dec 1984, use 'dirent_chk' instead of 'dirent_one'.
 *		06 Sep 1984, use "sysrename"
 *		03 Sep 1984, use "import"
 *		25 Aug 1984, cleanup buffer sizes
 *		15 Jul 1984, use PATHOF()
 *		14 Jul 1984, use $GETMSG
 *		10 Jul 1984, re-coded to DCLARG-list
 *		22 May 1984
 *
 * Function:	This module performs the RENAME function for "FLIST".  The
 *		first argument may be a name outside the file-list.
 *
 * Notes:	If the "versions" option is selected, a rename may uncover
 *		an older version of the same name.  When this is found, must
 *		create a new 'filelist[]' entry at the end of the list.
 *		Even more subtly, a rename may put the target file below
 *		the version-level of an existing file, causing it to vanish.
 *
 *		If the old-file is a directory (e.g., having ".DIR" filetype),
 *		we require that the new-file be also.  FLIST will not permit
 *		the user to accidentally zap himself by renaming a directory
 *		out of existence (or by making a file into a directory).
 *
 *		On the other hand, if the user renames a directory to
 *		another directory, then we must update the display-list
 *		to reflect the altered paths in dependent files.
 *
 * Patch:	Does VMS properly test for a user accidentally creating
 *		a maximum tree higher than 8 levels, e.g., by renaming a
 *		level-2 directory which has dependents down to level-8
 *		into a level-8 directory?
 */

#include	<rms.h>
#include	<string.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dircmd.h"
#include	"dirdata.h"
#include	"dirpath.h"
#include	"dirseek.h"
#include	"dds.h"
#include	"getprot.h"
#include	"chprot.h"

#include	"sysutils.h"

import(filelist);
import(V_opt);
import(conv_list);

static	void	flrnam_all (char *spec, int len, unsigned status);

static	int	curfile;
static	char	*newspec;

/*
 * Perform the actual rename.  For ease of use under vms5.0, check to see if we
 * have delete-privilege for the file.  If not, try to set it.  Returns 0 iff
 * no error occurred; otherwise the corresponding RMS status code.
 */
static
doit(char *newspec, char *oldspec, GETPROT *prot_)
{
	unsigned status;
	int	modified = FALSE;
#define	DELETE_MASK	 ((XAB$M_NODEL << XAB$V_SYS)\
			| (XAB$M_NODEL << XAB$V_OWN)\
			| (XAB$M_NODEL << XAB$V_GRP)\
			| (XAB$M_NODEL << XAB$V_WLD))

#define	UNMASK(name,mask) if ((status = chprot(name, mask, 0)) == RMS$_NORMAL)\
				status = 0

	if (!cmpprot(prot_, "d")) {
		UNMASK(oldspec, prot_->p_mask & ~DELETE_MASK);
		if (status != 0)
			return (status);
		modified = TRUE;
	}
	if (!(status = sysrename(newspec, oldspec))) {
		if (modified)
			UNMASK(newspec, prot_->p_mask);
	}
	return (status);
}

/*
 * Initiate a search for files matching the old-specification.  Whenever we
 * find one, call 'flrnam_all' to perform the actual RENAME-operation.
 */
tDIRCMD(flrnam)
{
	char	*oldspec = dclinx (xdcl_, 1, 0);

	curfile	= *curfile_;		/* save, in case we do refresh	*/
	newspec = dclinx (xdcl_, 2, 0);	/* save, for use in 'flrnam_all'*/
	dirseek_spec2 (oldspec, flrnam_all);
}

/*
 * This procedure is called from 'dirseek_spec' after each file-spec is found.
 */
static
void	flrnam_all (char *spec, int len, unsigned status)
{
	FILENT	ztmp,	*z = &ztmp, zold;
	char	oldspec	[MAX_PATH],
		tmpspec	[MAX_PATH],
		fixspec	[MAX_PATH],
		msg	[CRT_COLS];
	int	inx,
		wasdir;

	if (status != RMS$_NORMAL)	/* (file not found)	*/
		return;

	strncpy (oldspec, spec, len);
	oldspec[len] = EOS;

	inx = dirent_old_any (z, oldspec, 1);

	if (!diropen (zPATHOF(z)))
		return;

	wasdir	= dirent_isdir (z);
	zold	= *z;			/* Save, in case this was a directory */

	strcpy (tmpspec, newspec);	/* "sysrename" alters output arg */

	flist_log ("! rename %s to %s", oldspec, tmpspec);
	if ((status = doit(tmpspec, oldspec, &z->f_getprot)) == 0)
	{
	    if (status = dirent_chk (z, tmpspec))
	    {
		if (wasdir ^ dirent_isdir(z))
		{
		    warn ("Illegal directory-rename");
		    (void)doit(strcpy(fixspec, oldspec), tmpspec, &z->f_getprot);
		    return;
		}
		if (wasdir)	dirpath_rename (z, &zold);
	    }
	    else
		return;		/* ? (Probably a bug)		*/

	    dirdata_ren (z, &zold); /* rename the data-object	*/

	    if (inx < 0)	/* renamed new (?) entry into list	*/
		inx = dds_add (z);
	    else if (! (V_opt || wasdir))
	    {
		if (dirhigh (oldspec)) /* Check for uncovering	*/
		{
		    /*
		     * There was a lower version of the 'oldspec' around.
		     * Check the new name against the resulting list, using
		     * DIRHIGH to allocate a new entry, if needed.
		     */
		    dirhigh (tmpspec);
		    inx = -1;
		}
	    }

	    if (inx >= 0)
	    {
		dds_add2 (z, inx);
		if (wasdir && strchr(conv_list, 'p'))
		    dds_all (crt_top(), curfile);
		if (! V_opt)	dirhigh (tmpspec);
	    }
	}
	else
	{
	    flist_sysmsg (status);
	}
}
