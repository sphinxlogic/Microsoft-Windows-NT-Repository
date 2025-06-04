/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: fldlet.c,v 1.7 1995/10/21 18:41:45 tom Exp $";
#endif

/*
 * Title:	fldlet.c
 * Author:	T.E.Dickey
 * Created:	13 May 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		16 Feb 1989, added ".SHO", "TJL" and ".DIF$*" to temp-types.
 *		24 Oct 1988, corrected 'sprintf()' format for multiple-delete
 *		11 Oct 1988, added ".DIA" to temp-file types.
 *		16 Sep 1988, corrected 'verbfr[]' overflow check.
 *		10 Sep 1985, filename field of FILENT may have trailing '.'
 *		20 Jul 1985, had stepped on /NOVERSION re-read of deleted file.
 *		04 Jul 1985, cleanup 'filelist' definition.
 *		22 May 1985, use 'sysdelete' instead of unix-runtime
 *		14 Apr 1985, use 'flist_info' for log-routing
 *		08 Mar 1985, use 'dirent_isdir'.  Also, altered call on
 *			     'dirfind' to use direction-flag.
 *		05 Jan 1985, use DCLARG-filespec argument on 'dirfind'
 *		24 Dec 1984, added 'unfind' argument to 'dirfind'.
 *		14 Dec 1984, added nam-argument to 'dirent_chop'
 *		11 Dec 1984, do scroll-to-entry only we must do a prompt.
 *		03 Sep 1984, use "import", fix G-prompt
 *		23 Aug 1984
 *		16 Aug 1984, combined 'DELETE', 'PURGE' modules (which were
 *			     separate) to combine 'dirfind' calling logic.
 *		13 Aug 1984, add code for wildcard-deletes
 *		26 Jul 1984, use 'dirent_chk2'
 *		18 Jul 1984, changed sense of V_opt
 *		15 Jul 1984, use 'diropen2' call, PATHOF()
 *		10 Jul 1984, altered 'dirent_old' call
 *		04 Jul 1984, added checks for writeable directory
 *		03 Jul 1984, changed name 'faccess' to 'dirent_acc'
 *		02 Jul 1984, changed 'dirent_path' to 'pathdown'
 *		22 May 1984
 */

#include	<stdio.h>
#include	<rmsdef.h>
#include	<string.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dircmd.h"
#include	"dirfind.h"
#include	"dds.h"
#include	"pathup.h"

#include	"strutils.h"
#include	"sysutils.h"

static	void	fldlet_one (int inx, int *flag_);
static	void	flpurg_one (int inx, int *flag_);

import(filelist); import(numfiles);
import(V_opt);

/*
 * This list of filetypes is obtained from Table 2-2 in the "VAX/VMS Command
 * Languages User's Guide".  It lists the default filetypes for all temporary
 * files (listing, edit-backup, etc).
 */
static
char	*temptype[] = {
		"AIJ", "ANL", "BJL", "DIF", "DIS",
		"DIA",
		"DMP", "JNL", "JOU", "LIS", "LOG",
		"LST", "MAP", "OBJ",
		"SHO",
		"TJL",
		"TMP", "TM0",
		"TM1", "TM2", "TM3", "TM4", "TM5",
		"TM6", "TM7", "TM8", "TM9"
		};
static
int	max_temp = sizeof(temptype) / sizeof(temptype[0]);

static
int	curfile,		/* Index to entry of command		*/
	single,			/* TRUE iff only one name given		*/
	G_prompt,		/* TRUE if no prompt for unique-delete	*/
	tot_did;		/* Total number of files deleted	*/

/*
 * The main loop (called from 'dircmd') processes each argument, which may
 * contain wildcards.
 */
tDIRCMD(fldlet)
{
	int	j	= 0;
	DCLARG	*pattern;
	void	(*func)(int inx, int *flag_) =
			(strcmp (xdcl_->dcl_text, "DELETE")
				? flpurg_one
				: fldlet_one);

	curfile	= *curfile_;
	single	= dirchk (xcmd_, xdcl_, v_1, FALSE);
	tot_did	= 0;
	G_prompt= TRUE;

	for (pattern = xdcl_->dcl_next; pattern; pattern = pattern->dcl_next)
		dirfind (curfile, -1, pattern, func, TRUE, FALSE);

	if (!didwarn())
	{
		if (tot_did > 1)
			flist_info ("%d files deleted", tot_did);
		else if (tot_did == 0)
			flist_info ("No files deleted");
	}
}

/*
 * Title:	fldlet_one
 *
 * Function:	This module performs the DELETE function for "FLIST".  It is
 *		unlike the standard VMS DELETE function:
 *
 *		(a)	If the file is the only version of a non-temp file, it
 *			may prompt the user for verification.
 *		(b)	If the file is a directory file, the directory must be
 *			empty to delete the file.
 *		(c)	Only files present in 'filelist[]' may be deleted.
 *			(patch: may add entries ?)
 */
static
void	fldlet_one (int inx, int *flag_)
{
	FILENT	*z, ztmp;
	int	j, prompt, status;
	char	*c_,
		fullname[MAX_PATH],
		tempspec[MAX_PATH];

	z	= FK_(inx);
	if (! diropen2 (z))
		return;

	dirent_glue2 (fullname, z, curfile);
	if (! dirent_acc(z, "d"))
	{
		warn2 ("You may not delete %s", fullname);
		return;
	}

	/*
	 * If this is a directory-file, verify that the directory hasn't at
	 * least one file in it before deleting the directory.
	 */
	if (dirent_isdir(z))
	{
		pathdown (tempspec, zPATHOF(z), z->fname);
						/* Expand into path-name */
		strcat (tempspec, "*.*");	/* ...and ask for any file */
		if (dirent_chk (&ztmp, tempspec))
		{
			warn2 ("Directory is not empty");
			return;
		}
	}
	/*
	 * If this is a "temporary" file, delete it without any qualms.
	 * Otherwise, verify that it isn't the only version.  If it is,
	 * prompt before deleting it.
	 */
	else
	{
		prompt	= G_prompt;

		for (j = 0; j < max_temp; j++)
		{
			if (strcmp (temptype[j], z->ftype) == 0)
			{
				prompt = FALSE;
				break;
			}
		}
		if (prompt) {
			if (!strncmp(z->ftype, "DIF$", 4))
				prompt = FALSE;
		}
		if (prompt)
		{
			ztmp	= *z;
			ztmp.fvers = -1;
			dirent_glue (tempspec, &ztmp);
			if (!dirent_chk (&ztmp, tempspec))
			{
				if (!single) dds_index (inx);	/* scroll-to-entry */
				prompt = dircmd_ask (
						(single ? curfile : inx),
						"DELETE only version");
				*flag_ = (prompt >= 0);
				if (prompt <= 0)
					return;
				G_prompt = (prompt <= 1); /* Reset iff 2 */
			}
		}
	}

	if (status = sysdelete (fullname))
	{
		dirent_glue2 (fullname, z, curfile);
		sysgetmsg (status, tempspec, sizeof(tempspec));
		warn2 ("%s: %.80s", tempspec, fullname);
	}
	else				/* Refresh the display	*/
	{
		/*
		 * If highest-version option, trim off explicit-version part
		 * of filename.  Then, the re-read will pick up any remaining
		 * version of the file.
		 */
		tot_did++;		/* Count deletions	*/
		if (! V_opt)
		{
			c_ = strnull(fullname);
			while (*(--c_) != ';');
			*(++c_) = '\0';
		}
		/*
		 * Re-read directory entry to show effect of deletion.
		 */
		dirent_chk3 (inx, fullname);
	}
}

/*
 * Title:	flpurg_one
 *
 * Created:	22 May 1984
 * Last update:	16 Aug 1984, merged into 'fldlet'.
 *		26 Jul 1984, use 'dirent_chk2'
 *		25 Jul 1984, do separate loop if /NOVERSION
 *		15 Jul 1984, use PATHOF()
 *		10 Jul 1984, re-coded to use DCLARG-list
 *		22 May 1984
 *
 * Function:	This module performs a PURGE function for "FLIST".  It is
 *		unlike the standard VMS PURGE function; only lower versions
 *		of the current-file are deleted.
 */

static void
flpurg_one (int inx, int *flag_)
{
#define	VERBFR	5
	FILENT	*z, zprev, ztry;
	unsigned status;
	int	j,
		vercnt	= 0,
		verbfr	[VERBFR],	/* record versions which couldn't*/
		num_try	= 0,		/* # of files found for possible */
		num_err	= 0;		/* # which were protected, etc.	 */
	char	msg	[CRT_COLS],
		fullname[MAX_PATH], longname[MAX_PATH];
	static	char	fmt[] =
#if	NAME_DOT
			" (%s%s)";
#else
			" (%s.%s)";
#endif
	z = FK_(inx);
	strcpy (msg, "Version(s)");
	if (!single)
		sprintf (strnull(msg), " %s%s", z->fname, z->ftype);

	if (dirent_isdir(z))
	{
		if (single)
		{
			warn ("file should be a directory?");
			*flag_ = FALSE;	/* Stupid error, if no wildcard	*/
		}
		return;
	}

	if (V_opt)
	{
		for (j = 0; j < numfiles; j++)
		{
			if (DELETED(j))				continue;
			if (memcmp (z, FK_(j), FILENT_vers_size)) continue;
			if (z->fvers <= FK(j).fvers)		continue;

			num_try++;
			dirent_glue (fullname, FK_(j));
			if (sysdelete (fullname))
			{
				num_err++;
				if (vercnt < VERBFR)
					verbfr[vercnt++] = FK(j).fvers;
			}
			else
				tot_did++;
			dirent_chk2 (j);
		}
	}
	/*
	 * If "/NOVERSION" option is selected in the FLIST call, a PURGE
	 * will delete only "invisible" files (i.e., the previous versions
	 * of files which are presently in the 'filelist[]' array.
	 */
	else
	{
		zprev	= FK(inx);
		zprev.fvers = -1;
		dirent_glue (fullname, &zprev);
		while ((status = dirent_look (longname, fullname)) != RMS$_FNF)
		{
			if (status == RMS$_NORMAL)
			{
				num_try++;
				dirent_chop (&ztry, longname, 0);
				if (sysdelete (longname))
				{
					num_err++;
					if (vercnt < VERBFR)
						verbfr[vercnt++] = ztry.fvers;
					zprev.fvers--;
				}
				else
					tot_did++;
			}
			else
				zprev.fvers--;

			dirent_glue (fullname, &zprev);
		}
	}

	if (num_err)
	{
		for (j = 0; j < vercnt; j++)
			sprintf (strnull(msg), " %d", verbfr[j]);
		if (num_err > vercnt)
			strcat (msg, " ...");
		strcat (msg, " were protected");
		warn2 (msg);
	}
}
