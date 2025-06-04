#ifndef NO_IDENT
static char *Id = "$Id: fledit.c,v 1.12 1995/06/06 10:02:34 tom Exp $";
#endif

/*
 * Title:	fledit.c
 * Author:	T.E.Dickey
 * Created:	15 May 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		01 Jul 1988- added hack to permit us to run LSEDIT
 *		24 Aug 1985, use 'dds_add2' instead of 'dirdata_one'
 *		20 Aug 1985, do 'dirent_width' scan on return from directory
 *		31 Jul 1985, added 'dateflag', 'datechek' to stack.
 *		30 Jul 1985, do 'dds_pack' on exit from edit-directory.
 *		24 Jul 1985, pass-thru argument string to 'flist_opts' in case
 *			     we get to provide it for /LOG.
 *		23 Jul 1985, recompute 'numdlets' after directory-edit, permit
 *			     options in EDIT-directory.
 *		20 Jul 1985, corrected update-after-edit (use 'dirdata_one')
 *		05 Jul 1985, make 'namelist' a global, structured heap.  Let
 *			     'dirpath' do this for 'namelist' and 'pathlist'.
 *		03 Jul 1985, cleanup 'filelist' definition.  Use 'scanint' to
 *			     bypass 'sscanf' bug in CC2.0
 *		11 Jun 1985, added cli-argument to dds_spawn
 *		18 May 1985, added 'pcolumns[]'
 *		14 Apr 1985, use 'flist_info' for log-routing
 *		08 Mar 1985, use 'dirent_isdir'
 *		08 Feb 1985, altered message shown on entry to directory.
 *		01 Feb 1985, added 'readllen'
 *		30 Dec 1984, make "/mark" re-entrant ('dircmd_select' calls).
 *		24 Dec 1984, added 'D_mode', 'D_opt' save/restore
 *		15 Dec 1984, added 'cpy_dft' argument to 'dclarg'
 *		05 Dec 1984, added "namelist"
 *		18 Sep 1984, use "canopen"
 *		08 Sep 1984, use "highver"
 *		03 Sep 1984, use "import", added 'flquit'
 *		25 Aug 1984, cleanup of buffer sizes
 *		27 Jul 1984, added 'readlist'
 *		25 Jul 1984, added 'cmd_arg' to 'dclarg()'
 *		20 Jul 1984, added 'dirchk' logic, added 'numreads'
 *		18 Jul 1984, changed sense of 'V_opt'
 *		15 Jul 1984, use PATHOF()
 *		10 Jul 1984, changed calling convention
 *		03 Jul 1984, changed 'faccess' to 'dirent_acc'
 *		30 Jun 1984, to combine with recurring-call on 'flist' main
 *		28 May 1984
 *
 * Function:	This module supports an edit-function for "FLIST".  It tests
 *		the file-access to see if the file's directory is writeable.
 *		If not, it restricts the file to readonly.  Also, since we
 *		use EDT, it tests for the presence of the init-file in the
 *		user's home directory to generate the "/COMMAND" option.
 *
 *		If the file happens to be a directory, 'fledit' calls the main
 *		program of FLIST with a new argument: the new directory.
 *		(If the directory-name is NOT in 'filelist[], however, FLIST
 *		will simply open a new level on the single entry.  This is
 *		done to permit FLIST to alter protection, etc., of a root-
 *		level directory without exiting from the current level.)
 *
 *		If any sort of wildcard, or multiple-file argument is given,
 *		a new level of FLIST is entered, like the directory-file
 *		treatment.
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	<signal.h>	/* for 'sleep()' */
#include	<string.h>

#include	"bool.h"
#include	"dclarg.h"
#include	"flist.h"

#include	"canopen.h"
#include	"dds.h"
#include	"dircmd.h"
#include	"dirent.h"
#include	"dds.h"
#include	"pathup.h"
#include	"scanver.h"

#include	"strutils.h"

import(filelist);	import(numfiles);	import(numdlets);
import(D_mode);		import(datechek);	import(dateflag);
import(D_opt);
import(V_opt);
import(pcolumns);
import(conv_list);
import(maxfiles);
import(readlist);	import(readllen);

extern	int	multi_quit;
static	int	pack_exit = 0;

static	void	fledit_dir (int *curfile_, char *xcmd_, DCLARG *xdcl_);
static	void	fledit_file (int *curfile_, char *xcmd_, DCLARG *xdcl_, FILENT *z, int inlist);
static	void	fledit_tell (char *path_);

tDIRCMD(fledit)
{
	FILENT	ztmp,	*z = &ztmp;
	int	inx,
		single	= dirchk (xcmd_, xdcl_, v_1, FALSE);
	DCLARG	*dcl_;		/* arg-list pointer for built-command */
	char	path	[MAX_PATH],
		*spec,
		longname[MAX_PATH];

	/*
	 * Parse the name from 'xdcl_', test for use of wildcard.
	 * If wildcard or directory-name, do directory-call.  Otherwise,
	 * edit/view file.  If directory-call, no options are permitted.
	 * Otherwise, may include options.
	 *
	 * patch: should permit options on new level.
	 */
	if (single || (xcmd_ == nullC))
	{
		if (single)
		{
			spec	= dclinx (xdcl_, 1, 0);
			inx	= dirent_old_any (&ztmp, spec, 0);
		}
		else
		{
			ztmp	= FK(inx = *curfile_);
			dirent_glue2 (spec = longname, z, *curfile_);
		}

		/*
		 * 'inx' is set to -2 if the file wasn't found, -1 if it is
		 * not in the current 'filelist[]'.
		 */
		if (dirent_isdir(&ztmp))
		{
			if (inx >= 0)
			{
				if (!pathdown (path, zPATHOF(z), ztmp.fname))
				{
					warn ("Illegal pathname: %s", path);
					return;
				}
				fledit_tell (path);
				dcl_	= dclarg (path, dirent_dft(), 1, FALSE);
				fledit_dir (curfile_, xcmd_, dcl_);
			}
			else if (inx >= -1)
			{
				fledit_tell (zPATHOF(z));
				fledit_dir (curfile_, xcmd_, xdcl_->dcl_next);
			}
			else
				warn ("Directory not found: %s", spec);
		}
		else
			fledit_file (curfile_, xcmd_, xdcl_, z, inx);
	}
	else if (dirchk (xcmd_, xdcl_, v_M, TRUE))
		fledit_dir (curfile_, xcmd_, xdcl_->dcl_next);
}

/*
 * Enter a new directory.  The 'xdcl_' linked-list begins with the first
 * actual argument, rather than the command name.
 */
static
void	fledit_dir (
	int	*curfile_,
	char	*xcmd_,
	DCLARG	*xdcl_)
{
	FLINK	*fixed_	  = 0;
	FLINK  **SAVEfile = filelist;
	FILENT	*SAVEread = readlist;
	DATENT	SAVEdchk  = datechek;
	int	j,
		SAVEpack  = pack_exit,
		SAVEmaxf  = maxfiles,
		SAVEnumf  = numfiles,
		SAVEnumR  = readllen,
		SAVEdmod  = D_mode,
		SAVEdopt  = D_opt,
		SAVEpcol  size_pcolumns,
		SAVEtopf  = dds_fast(DDS_U_C),
		PACKtopf  = SAVEtopf,
		SAVEmark  = dircmd_select(-2);
	char	SAVEconv  size_conv_list,
		SAVEdflg  size_dateflag;

	/* If options given are illegal, don't go anywhere	*/
	if (flist_opts (1, &xcmd_, xdcl_, TRUE))	return;

	memcpy (SAVEpcol, pcolumns, sizeof(pcolumns));
	memcpy (SAVEdflg, dateflag, sizeof(dateflag));
	strcpy (SAVEconv, conv_list);

	if (SAVEmark >= 0)	fixed_ = filelist[SAVEmark];
	dircmd_select (-1);	/* Start with cleared display-flag */
	pack_exit = FALSE;
	flist (xdcl_);		/* Do recursive call on main	*/
	dircmd_select (-1);	/* Call display-flag, if any	*/

	filelist = SAVEfile;	/* Restore caller's state	*/
	readlist = SAVEread;
	maxfiles = SAVEmaxf;
	numfiles = SAVEnumf;
	readllen = SAVEnumR;
	D_mode	 = SAVEdmod;
	D_opt	 = SAVEdopt;
	datechek = SAVEdchk;
	memcpy (pcolumns, SAVEpcol, sizeof(pcolumns));
	memcpy (dateflag, SAVEdflg, sizeof(dateflag));
	strcpy (conv_list, SAVEconv);

	/*
	 * Recompute the number of deleted-entries (in case the lower-level did
	 * it), and adjust the top-of-screen index 'PACKtopf', to account for
	 * whatever 'dds_pack' will do:
	 */
	for (j = numdlets = 0; j < numfiles; j++)
		if (DELETED(j))
		{	numdlets++;
			if (pack_exit && (j < SAVEtopf)) PACKtopf--;
		}

	/*
	 * If we are not skipping display levels, refresh the screen.  If there
	 * were any files in the lower-level, do a screen-pack.  Recompute the
	 * longest-column data, and finally reset the marker-entry.
	 */
	if (multi_quit <= 0)
	{
		if (pack_exit)	dds_pack (curfile_, TRUE);
		dirent_width ((FILENT *)0);
		dds_all (PACKtopf, *curfile_);

		if (fixed_) for (j = 0; j < numfiles; j++)
			if (fixed_ == filelist[j] && !DELETED(j))
			{
				dircmd_select (j);
				break;
			}
	}
	pack_exit= SAVEpack;
}

/*
 * Show the pathname in a form which will cause minimal refresh for very
 * short display-lists:
 */
static
void	fledit_tell (char *path_)
{
	flist_info ("Read: %s", path_);
	sleep (1);
}

/*
 * EDIT/VIEW a file.  Normally the user will use the default entry, which
 * edits the current file-entry.  In this case, if the file is updated,
 * the entry receives the data for the updated file, and the old entry
 * (if /NOVERSIONS isn't set) is moved to a free slot.
 *
 * On the other hand, if the user employs a "/o" escape to make a different
 * filename, one of two cases may happen:
 *	(a) The file exists, whether somewhere else in 'filelist[]', or as
 *	    a file not read into 'filelist[]'.  If it is not already in the
 *	    an entry is added for the file.
 *	(b) The file doesn't exist.
 *
 * In any case, the 'filelist[]' entry which corresponds to the highest
 * version of the file will receive the updated filename.
 *
 * patch: may alter this to put the updated name on the current line.
 * patch: cmd-size should be done with calloc.
 */
static
void	fledit_file (
	int	*curfile_,
	char	*xcmd_,
	DCLARG	*xdcl_,
	FILENT	*z,		/* => data for file to edit		*/
	int	inlist)		/* >= 0 iff 'z' is found in 'filelist[]'*/
{
	FILENT	zold	= *z;		/* original old-file data	*/
	int	status,	curvers, j,
		oldver	= z->fvers,
		rmode	= (xdcl_->dcl_text[0] == 'V'),	/* "EDIT" or "VIEW" */
		width	= crt_width() - 6;	/* nominally 74	*/
	static	char	sFMT1[]	= "%%s %%-%d.%ds";
	char	fullname[MAX_PATH],
		tstspec	[MAX_PATH],
		*c_,
		format	[sizeof(sFMT1)+6],
		cmd	[(3*MAX_PATH)+CRT_COLS]; /* filenames, plus options */

	static	char	*bad_type[] = {"EXE", "OBJ", "OLB"};
	static	int	max_bad_type = sizeof (bad_type) / sizeof(bad_type[0]);

	dirent_glue (fullname, z);

	for (j = 0; j < max_bad_type; j++)
	{
		if (strcmp (z->ftype, bad_type[j]) == 0)
		{
			warn ("You cannot edit this file-type: %s", z->ftype);
			return;
		}
	}

	if (inlist == -1)	/* Add it to the list to edit it	*/
		inlist = dds_add (z);

	if (inlist >= 0)	/* If an old-file, check readability	*/
	{
		if (! dirent_acc (z, "r"))
		{
			flist_info ("You cannot view this file");
			return;
		}
	}

	/*
	 * If the user has defaulted to EDIT mode, test to ensure that the
	 * directory is writeable (i.e., that we can make a new version via
	 * EDIT).  If not, 'diropen' emits a warning message.  In this case,
	 * coerce to readonly, and proceed.
	 */
	if (! rmode)
	{
		if (! diropen (fullname))
		{
			rmode = TRUE;
			sleep(1);
			clrwarn ();
		}
	}

	/*
	 * Another reason for forcing readonly mode is if the edited-version
	 * is not the highest (current) version.  By selecting readonly edit,
	 * the editor warning message is suppressed.
	 */
	strcpy (tstspec, fullname);
	curvers = highver (tstspec);	/* tstspec <= name w/o version */
	if (z->fvers < curvers)
		rmode = TRUE;

	if (xdcl_->dcl_text[0] == 'L') {
		strcpy (cmd, "LSEDIT");
	} else {
		strcpy (cmd, "EDIT");
		if (strcmp (getenv("HOME"), zPATHOF(z)))
		{
			char	edtini[MAX_PATH];
			sprintf (edtini, "%s%s", getenv("HOME"), "EDTINI.EDT");
			if (!canopen (edtini))
				sprintf (strnull(cmd), "/COMMAND=%s", edtini);
		}
	}

	/*
	 * Provide for passing through options:
	 * (patch: should do better for storage allocation
	 */
	if (xcmd_)
	{
		for (xdcl_ = xdcl_->dcl_next; xdcl_; xdcl_ = xdcl_->dcl_next)
		{
			strcat (cmd, " ");
			strcat (cmd, xdcl_->dcl_text);
		}
	}
	else
	{
		strcat (cmd, " ");
		strcat (cmd, fullname);
	}
	if (rmode) strcat (cmd, "/READ_ONLY");	/* (override "/NOREAD") */

	sprintf (format, sFMT1, width, width);
	flist_info (format,  (rmode ? "VIEW" : "EDIT"), fullname);

	dds_spawn (0, *curfile_, cmd, 0, FALSE, 2);

	/*
	 * On exit, the refresh-option may have altered the current entry,
	 * but the prior entry probably still exists.  If altered, do a scan
	 * to pick up the prior entry, and insert it on the end of the list.
	 */
	if (!rmode)
	{
		dirent_chk (z, tstspec);
		if (inlist >= 0)
		{
			if (memcmp (z, &zold, sizeof(FILENT)))
				dds_add2 (z, inlist);
		}
		else
			dds_add (z);
		if (V_opt && oldver && (oldver != z->fvers))
			dds_add (&zold);
	}
}

/*
 * Title:	flquit.c
 *
 * Function:	Process a "quit" command for FLIST.  If an argument is given,
 *		it must be the number of levels to quit from (default: 1).
 *		An asterisk quits all levels (actually 999).
 */
tDIRCMD(flquit)
{
	int	num;
	register char	*c_, *d_;

	if (xdcl_ = xdcl_->dcl_next)
	{
		c_ = scanint (d_ = xdcl_->dcl_text, &num);
		if (!strcmp("*", d_))
			multi_quit = 999;
		else if (c_ != d_ && *c_ == '\0')
		{
			if (num >= 1)
				multi_quit = num;
			else
			{
				warn ("Argument for QUIT must be positive");
				return;
			}
		}
		else
		{
			warn ("Argument for QUIT must be '*' or number");
			return;
		}
	}
	numfiles = 0;	/* Force quit by clearing file count	*/
}

/*
 * This entrypoint is called from the main-loop immediately after succeeding
 * in reading a directory.  We know that we have a new screen to display, and
 * thus, on exit from the screen, can start over again by a 'dds_pack'.
 */
void	fledit_pack (void)
{
	pack_exit = (numfiles > 0);
}
