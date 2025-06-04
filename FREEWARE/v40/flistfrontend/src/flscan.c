#ifndef NO_IDENT
static char *Id = "$Id: flscan.c,v 1.9 1995/10/25 23:46:00 tom Exp $";
#endif

/*
 * Title:	flscan.c
 * Author:	Thomas E. Dickey
 * Created:	24 Jul 1984
 * Last update:
 *		25 Oct 1995, use dds_while to show animated "working" message
 *		18 Mar 1995, prototypes
 *		27 Jul 1985, make 'flscan_set'..'flscan_off' common routines to
 *			     use in tests of real directory entries against our
 *			     picture in 'filelist[]'.
 *		24 Jul 1985, use 'dirent_chk2' in front of 'dirent_nul' in case
 *			     the file really should be marked "deleted".
 *		20 Jul 1985, use 'dirent_nul' instead of 'dirent_dlet' to mark
 *			     entries in 'filelist[]' "deleted" when they are
 *			     not found in the read-list.
 *		04 Jul 1985, cleanup 'filelist' definition.
 *		14 Jun 1985, prune files which are not date-selected.
 *		05 Feb 1985, altered call on 'dirseek_spec', which now can
 *			     test for implicit (full-list) pathnames.
 *		31 Jan 1985, use 'dirread' to maintain read-list
 *		05 Jan 1985, use DCLARG-filespec argument on 'dirfind'
 *		24 Dec 1984, added 'unfind' argument to 'dirfind'.
 *		14 Dec 1984, added nam-argument to 'dirent_chop'
 *		06 Dec 1984, changed 'readlist' to TEXTLINK structure
 *		17 Nov 1984, added 'INSPECT' code.
 *		02 Sep 1984, use "import"
 *		16 Aug 1984, use 'dirent_misc'
 *		31 Jul 1984
 *
 * Function:	This module performs the VERIFY function for "FLIST".  VERIFY
 *		tests each entry in 'filelist[]' to verify that it still
 *		exists.  Unlike READ, it does not re-read the date, size, etc.,
 *		so it can operate (relatively) rapidly.
 *
 *		If no arguments are given, VERIFY examines the entire list.
 *		If arguments are given, VERIFY uses the 'FIND' function to
 *		locate each element which is currently in the list, and then
 *		does a lookup on those elements.  (Note that in the latter
 *		case warning messages may be emitted via 'flfind()).
 *
 * 		If file is currently locked, we also do 'dirent_chk2(j)' to
 *		do the reverse type of update.
 *
 *		No attempt is made to append new filenames to the 'filelist[]'
 *		array.  This would be difficult to make consistent with the
 *		use of 'readlist'.
 *
 *		If no list is given to 'flscan', its global search may remove
 *		from the list certain filenames which were added dynamically
 *		(e.g., new files created with COPY and RENAME).
 *
 *		INSPECT (also coded here) is similar to VERIFY, but with two
 *		differences:
 *
 *		- It inspects each file (reads a block) to determine if it is
 *		  human-readable text.  If so, a "*" flag is appended to the
 *		  format-display field.
 *		- If no arguments are given, INSPECT assumes only the current
 *		  filename entry, rather than the entire list.
 *
 * Patch:	This code should be redone for the full-list case to avoid
 *		the N-squared effect of the search in 'flscan_all'.
 *
 *		The '.ftext' flag is not maintained in the rest of FLIST.
 *		In particular, a READ will clear this flag.
 */

#include	<string.h>

#include	<rmsdef.h>

#include	"flist.h"

#include	"dircmd.h"
#include	"dirent.h"
#include	"dirfind.h"
#include	"dirread.h"
#include	"dirseek.h"
#include	"dds.h"

extern	int	inspect (char *filespec, int toscan);

static	void	flscan_all (char *spec, int len, unsigned status);
static	void	flscan_clr (void);
static	void	flscan_off (int j, int *unused);
static	void	flscan_on  (int j);
static	void	flscan_set (void);

import(filelist); import(numfiles);
import(conv_list);

static	int	clarified,	/* Flag set if 'inspect' altered data	*/
		do_inspect;	/* TRUE if we also call 'inspect'	*/

#define	BIT_2	2		/* Use this '.fmisc' bit via dirent_misc */

tDIRCMD(flscan)
{
	int	j,
		k	= 0,
		do_args	= FALSE;
	DCLARG	*d_, *spec_;
	char	fbfr[MAX_PATH];

	clarified = FALSE;
	do_inspect = FALSE;	/* Assume VERIFY	*/
	if (xdcl_)
	{
		do_args = (dclinx(xdcl_, 1, 0) != 0);
		do_inspect = (xdcl_->dcl_text[0] == 'I');
	}

	/*
	 * Process the entries which the user selects in 'filelist[]' for
	 * verification:
	 */

	/*
	 * If the user gave a specific list of files, initialize '.fmisc'
	 * to FALSE only where the list does intersect 'filelist[]'.  Else,
	 * there is nothing to initialize.
	 */
	dds_while (nullC);
	if (do_args)
	{
		flscan_set ();
		for (d_ = xdcl_->dcl_next; d_; d_ = d_->dcl_next)
			dirfind (0, TRUE, d_, flscan_off, FALSE, FALSE);
		for (k = 0; spec_ = dclinx2 (xdcl_, 1, k); k++)
			dirseek_spec (spec_, TRUE, flscan_all);
	}
	else if (do_inspect)
	{
		flscan_set ();
		dirent_glue (fbfr, FK_(*curfile_));
		dirseek_spec2 (fbfr, flscan_all);
	}
	else	/* Leave only files found in 'read-list'	*/
	{
		flscan_clr ();
		for (k = 0; dirread_get(fbfr, k); k++)
			dirseek_spec2 (fbfr, flscan_all);
	}

	/*
	 * Now, go back over the list of files in 'filelist[]', checking off
	 * those which were not found (and must be assumed to have been
	 * deleted).  We must be careful not to mark a file deleted more than
	 * once, since 'dirent_nul' counts the total number of deletions to
	 * update the display.
	 *
	 * Re-check the file data for any files which were locked.
	 */
	for (j = 0; j < numfiles; j++)
	{
		dds_while(nullC);
		if (FK(j).fmisc & BIT_2)	/* File found ?	*/
		{
			if (LOCKED(j))		/* ...if so, check locked-files */
				dirent_chk2 (j);
		}
		else if (! DELETED(j))		/* Make it look deleted	*/
		{
			if (dirent_chk2(j))	dirent_nul(j);
		}
	}

	/*
	 * If 'inspect' altered any of the normal-text (.ftext) flags, then
	 * we may have to repaint the screen anyway.  Do this only if the
	 * FORMAT-display is active:
	 */
	if (strchr(conv_list, 'f') && clarified)
		dds_all (crt_top(), *curfile_);
}

static
void	flscan_set (void)	/* Set bit everywhere	*/
{
	dirent_misc (-2, BIT_2);
}

static
void	flscan_clr (void)	/* Clear bit everywhere	*/
{
	dirent_misc (-1, BIT_2);
}

static
void	flscan_on  (int j)	/* Set particular bit	*/
{
	dirent_misc (j,  BIT_2);
}

/*
 * Reset a flag to indicate intersection between the user's argument list and
 * the contents of 'filelist[]'.  If not subsequently verified by finding the
 * file in the real directory, 'flscan' will remove it from 'filelist[]'.
 */
static
void	flscan_off (int j, int *unused)
{
	FK(j).fmisc &= (~BIT_2);
}

/*
 * This procedure is called from 'dirseek_spec' after each file-spec is found.
 * Reset the corresponding bit in the '.fmisc' component (which are set TRUE
 * only where a specific file-spec was found).
 */
static
void	flscan_all (char *spec, int len, unsigned status)
{
	FILENT	z2;
	int	j;
	char	spec2[MAX_PATH];

	dds_while(nullC);
	if (status == RMS$_NORMAL)
	{
		strncpy (spec2, spec, len);
		spec2[len] = EOS;
		dirent_chop (&z2, spec2, 0);

		for (j = 0; j < numfiles; j++)
		{
			dds_while(nullC);
			if (DELETED(j))
				continue;
			if (! dirent__datechek (FK_(j)))
				continue;
			if (memcmp (&z2, FK_(j), FILENT_name_size) == 0)
			{
				flscan_on (j);
				if (do_inspect)
				{
				int	old = FK(j).ftext,
					new = inspect (spec2, 512);
					if (old != new)
					{
						FK(j).ftext = new;
						clarified = TRUE;
					}
				}
				break;
			}
		}
	}
}
