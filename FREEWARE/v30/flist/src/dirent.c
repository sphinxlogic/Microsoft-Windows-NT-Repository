/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: dirent.c,v 1.19 1995/10/26 00:08:37 tom Exp $";
#endif

/*
 * Title:	dirent.c
 * Author:	T.E.Dickey
 * Created:	30 Apr 1984
 * Last update:
 *		25 Oct 1995, workaround for DEC-C bug in sprintf.
 *		23 Oct 1995, added 'u(ser)' column
 *		21 Oct 1995, /dlong repeated shows time in seconds.
 *		28 May 1995, prototypes
 *		18 Feb 1995, port to AXP (DATENT mods)
 *		05 Dec 1989, corrected typeof(llast)
 *		04 Nov 1988, took masking from group/member code displays
 *			     (should work out maximum field length).
 *			     Added code to support expiration-date display.
 *		16 Aug 1988, corrected 'dirent_conv()', which permitted a
 *			     buffer-overflow on terminals wider than 132!
 *		09 Sep 1985, corrected collating-order: VMS apparently sorts
 *			     up-to-and-including trailing '.' in filenames.
 *		03 Sep 1985, added 'update' option to 'dirent_all' and to
 *			     'dirent__read'.  Broke 'dirent__one' into chop-
 *			     and look-parts for this purpose.
 *		24 Aug 1985, use 'dirent_width' to reset entire-list too.
 *		20 Aug 1985, broke out 'dirent_width'.
 *		30 Jul 1985, 'numreads' (pruning only after first SYS$SEARCH)
 *			     is obsolete with muti-level heap.
 *		20 Jul 1985, added 'dirent_chk3', 'dirent_nul'.
 *		14 Jul 1985, added calls on 'dirdata' module.
 *		05 Jul 1985, moved 'dirpath_init' to main-loop.
 *		04 Jul 1985, if file-not-found in 'dirent_old_any', do not
 *			     assume NAM-block points to current parsed (expanded)
 *			     string: it may contain the previous esa/rsa.
 *		02 Jul 1985, cleanup of 'dirent.h' definitions
 *		23 Jun 1985, cleanup of 'dirent_look', return status in 'dirent_chop'
 *			     Use 'scanver' to (try to) bypass CC2.0 bug.  Got
 *			     rid of unnecessary 'strlen' calls in 'dirent_chop'.
 *		13 Jun 1985, added /AFTER and /BEFORE options (dateflag)
 *		18 May 1985, added 'pcolumns'
 *		26 Mar 1985, tuned column-widths for file-id, file-size
 *		22 Mar 1985, added file-id, record-length conversions.
 *		21 Mar 1985, added very-short date format.
 *		08 Mar 1985, added 'dirent_isdir'.
 *		03 Mar 1985, move flcols-init to main so we can treat this as
 *			     a pure hierarchy.  Restrict 'dirent_col' so that
 *			     we have a command field even if the name fills the
 *			     line by itself (still no attribute columns).
 *		01 Feb 1985, check for "*" device in 'dirent_glue' for 'dirfind'.
 *			     and "*" version, as well as in 'dirent_chop' (to
 *			     support read-list pruning 'dirread').
 *		30 Dec 1984, deselect file if it is deleted.
 *		24 Dec 1984, added 'D_mode' to drive day-of-week display.
 *		22 Dec 1984, added ALLOC column-type.
 *		21 Dec 1984, use 'flist_sysmsg'.
 *		20 Dec 1984, decode file-org, format from single byte, driving
 *			     'ccolumns' if non-sequential file.
 *		14 Dec 1984, use single FAB for parsing, search, open where
 *			     possible, to reduce paging.  Re-coded 'dirent_chop'
 *			     to use existing NAM-block, if available.  Use global
 *			     variable 'znam' in local routines 'dirent__one' and
 *			     'dirent__read'.
 *		12 Dec 1984, don't show error in 'dirent_chk'.
 *		10 Dec 1984, use 'acplook', 'rmslook' for directory-read.
 *			     Added 'nam' argument to 'dirent__read' for compat
 *			     with new 'dirent__one()'.  Moved 'syshour' to
 *			     'dirsrt'.
 *		06 Dec 1984, use 'nameheap' to store filename, filetype.  Do
 *			     better overflow check in 'dirent_conv'.  Added
 *			     'p' (PATH) to conv_list.  Interlock 'dds_tell'
 *			     call with 'flread'.
 *		17 Nov 1984, added record-format stuff
 *		15 Oct 1984, locked-files should have high-date, if unknown
 *		10 Sep 1984, use "rmsinit_???", fixed "?locked", blocksize
 *		03 Sep 1984, use "import"
 *		27 Aug 1984, cleanup buffer sizes
 *		15 Aug 1984, added 'dirent_dft()', multi-date support
 *		14 Aug 1984, added 'ccolumns[]', 'dirent_col()'
 *		06 Aug 1984, added '.fhour' component to FILENT
 *		02 Aug 1984, broke out 'sysasctim' procedure
 *		30 Jul 1984, added 'dirent_dlet' entry.
 *		28 Jul 1984, correction to same-file login
 *		27 Jul 1984, broke out pathlist/readlist logic to 'dirpath'.
 *		26 Jul 1984, added 'dirent_chk2' code, 'readlist'
 *		20 Jul 1984, added 'numreads' logic
 *		17 Jul 1984, added V_opt/duplicate pruning to 'dirent__read'.
 *		15 Jul 1984, make sort-keys for 'pathlist'
 *		14 Jul 1984, use 'dclopt' to process options, added OWNER
 *		10 Jul 1984, added warning-option to 'dirent_old'
 *		04 Jul 1984, must clear temp in 'dirent_old' before compare
 *		03 Jul 1984, broke out 'dirent_acc' as 'cmpprot.c', cleaned up.
 *		02 Jul 1984, broke out 'dirent_path' as 'pathdown.c'
 *		28 May 1984
 *
 * Function:	This module performs wildcard directory lookup using the
 *		argc/argv argument list passed to the main program of "FLIST".
 *		It processes options which permit "FLIST" to display less than
 *		the full directory listing (for speed).
 *
 * Entry:	dirent:		Read directory, given argument list
 *		dirent_acc:	Test a FILENT-block for needed access-mode
 *		dirent_add:	increment 'numfiles', reallocate 'filelist[]'
 *		dirent_all:	Load a series of files to 'filelist[]'
 *		dirent_ccol:	Return crt command-column start
 *		dirent_chk:	re-read a file-spec to update FILENT-block
 *		dirent_chk2:	re-read a file-spec to update 'filelist[]'
 *		dirent_chk3:	read a file-spec toupdate (?) 'filelist[]'
 *		dirent_chop:	parse a file-spec to load FILENT-block
 *		dirent_conv:	Convert a FILENT-block to CRT_COLS (max) string
 *		dirent_dft:	Return wildcard default, based on V_opt
 *		dirent_dlet:	Mark a 'filelist[]' entry deleted.
 *		dirent_glue:	form a complete file-spec from FILENT block
 *		dirent_glue2:	form a (short?) file-spec from FILENT block
 *		dirent_init:	Initialize this module
 *		dirent_isdir:	Test FILENT to see if it is a directory.
 *		dirent_look:	lookup a single file-spec, returns full name
 *		dirent_misc:	Set/clear bits in the '.fmisc' component
 *		dirent_nul:	Create a null-entry in 'filelink'.
 *		dirent_old:	find a filespec in 'filelist[]'
 *		dirent_old1:	Lookup any file, returning FILENT-block
 *		dirent_old_any:	Generalized lookup to 'filelist[]'
 *		dirent_width:	latch maximum column-widths for new entry
 *		dirent__cnv:	Convert numbers for display
 *		dirent__date:	Select date-field
 *		dirent__datechek:Test date-selection
 *		dirent__look:	read a FILENT-block w/o SYS$SEARCH
 *		dirent__one:	read a FILENT-block for all file-info
 *		dirent__read:	read a FILENT-block, pruning if duplicates
 */

#include	<starlet.h>
#include	<lib$routines.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<rms.h>
#include	<descrip.h>
#include	<stsdef.h>

#include	"flist.h"
#define		DIRENT		/* local */
#include	"dirent.h"
#include	"dircmd.h"
#include	"dirdata.h"
#include	"dirpath.h"
#include	"dirread.h"
#include	"dclarg.h"
#include	"dds.h"

#include	"getprot.h"
#include	"rmsinit.h"
#include	"nameheap.h"
#include	"scanver.h"

#include	"strutils.h"
#include	"sysutils.h"

#define	MAXFILES	(4096 / sizeof(filelist[0]))

#define	LATCH(i,f)	if ((f) > ccolumns[i])	ccolumns[i] = latch = len

static	void	dirent__cnv2 (int no_priv, char *c_, unsigned number);
static	DATENT*	dirent__date (FILENT *z, int opt);
static	int	dirent__look (FILENT *z, char *filespec);
static	int	dirent__one (FILENT *z, char *filespec);
static	int	dirent__read (char *filespec, FLINK **flink, int update);
static	void	dirent_init (void);

/*
 * Public data:
 */
import(filelist); import(numfiles); import(numdlets);

import(AnyXAB);
import(A_opt);	import(D_opt);	import(M_opt);
import(O_opt);	import(P_opt);	import(V_opt);

import(D_mode);

import(ccolumns);
import(pcolumns);
import(conv_list);
import(maxfiles);
import(namelist);

import(dateflag);
import(datechek);

	int	prot_col;		/* (used by protection-edit)	*/

/*
 * Data structures used in directory lookup:
 */
static	struct	FAB	zfab;
static	struct	NAM	znam;		/* used in wildcard parsing	*/

static
char	zrsa[NAM$C_MAXRSS],		/* resultant string area	*/
	zesa[NAM$C_MAXRSS];		/* expanded string area		*/

/*
 * Data structures used to obtain info on particular file:
 */
static	struct	XABALL	xaball;		/* Allocation data		*/
static	struct	XABDAT	xabdat;		/* Date and time		*/
static	struct	XABFHC	xabfhc;		/* File header characteristics	*/
static	struct	XABPRO	xabpro;		/* Protection			*/

/*
 * The RMS copy of the file-organization is simply masked from the ACP copy.
 * Exploit this to make a mask for the format portion:
 */
#define	FAB$M_RFM	((1 << FAB$V_ORG)-1)

static
char	*known_org[] = {"SEQ", "REL", "IDX", "HSH"},
	*known_rfm[] = {" ", "FIX", "VAR", "VFC", "STM", "STMLF", "STMCR"},
	*known_rat[] = {"FTN", "CR", "PRN", "BLK"},
	*day_of_week[] = {"Wed ", "Thu ", "Fri ", "Sat ", "Sun ", "Mon ", "Tue "};
static
int	absolute_day;	/* Current day-number, for short-date computation */

/* <dirent>:
 * Main routine, read a list of filespecs into 'filelist[]'.
 */
int	dirent (DCLARG *arg_)
{
	int	j, didsome = 0;
	char	c, *c_;

	dirread_init ();

	numfiles =		/* (no files in list yet)	*/
	numdlets = 0;		/* (of course, none deleted yet)*/

	dirent_width ((FILENT *)0);	/* Set nominal widths for entries*/

	filelist = calloc(maxfiles = MAXFILES, sizeof(filelist[0]));

	dirent_init();
	for (; arg_; arg_ = arg_->dcl_next)
	{
		c_ = arg_->dcl_text;
		if (!isopt(*c_))
		{
			dirent_all(c_, FALSE);
			didsome++;
		}
	}
	if (!(didsome || numfiles))
		dirent_all ("*", FALSE);

	return (numfiles);
}

/* <dirent_acc>:
 * Test a file-entry to see if the user may access the file:
 */
int	dirent_acc (
	FILENT	*z,		/* file to test			*/
	char	*mode)		/* List of access rights needed	*/
{
	return (cmpprot (&z->f_getprot, mode));
}

/* <dirent_add>:
 * Whenever the amount of space reserved for 'filelist[]' is filled, reallocate
 * to obtain reserve.
 */
int	dirent_add (void)
{
	if (++numfiles >= maxfiles)
	{
		maxfiles += MAXFILES;
		filelist = realloc (filelist, maxfiles * sizeof(FILENT));
	}
	return (numfiles);
}

/* <dirent_all>:
 * Use a wildcard string to (possibly) examine all files in a directory.
 *
 * In the array 'readlist', maintain a list of the file-specs used
 * as calls on this procedure, to support the READ command (re-read all
 * specs).
 */
void	dirent_all(
	char	*filespec,		/* specifies files to lookup	*/
	int	update)			/* true iff we *must* re-read	*/
{
	unsigned status;
	int	numfirst = numfiles,
		newfiles = FALSE;	/* Latch for 'flread'	*/
	FLINK	*llast	 = 0;

	/*
	 * Initialize file-access-block:
	 */
	zfab.fab$l_fna = filespec;	/* given-names		*/
	zfab.fab$b_fns = strlen(filespec);

	if (sys$parse(&zfab) != RMS$_NORMAL)
		return;

	dirread_put (&zfab);		/* Update 'readlist'	*/

	/*
	 * MAXFILES is the number of files to access before reallocating
	 * the 'filelist[]' array to enlarge it.
	 */
	for (;;)
	{
		dds_while(nullC);
		if ((status = sys$search(&zfab)) == RMS$_NMF)
			return;
		else if (status == RMS$_NORMAL)
		{
			if (numfiles > numfirst)	newfiles = TRUE;
			if (newfiles && (numfiles % 5 == 0) && numfiles)
			{
				clrbeep ();
				dds_tell (nullC, -1);
			}
			zrsa[znam.nam$b_rsl] = EOS;
			if (dirent__read (zrsa, &llast, update))
			{
				if (dirent__datechek (&llast->fk))
				{
					filelist[numfiles] = llast;
					numfiles = dirent_add ();
				}
			}
		}
		else
			flist_sysmsg (status);
	}
}

/* <dirent_cat_n>:
 */
void	dirent_cat_n (char *s, FILENT *z)
{
	register len = strlen(z->fname);

	strncpy (s += strlen(s), z->fname, len);
	s[len-NAME_DOT] = EOS;
}

/* <dirent_ccol>:
 * Compute the starting column number at which "visible" commands
 * begin.  This is made a variable to support future (14-Aug-84) releases
 * of VMS, which may use long filenames.
 *
 * Protect further against fools with long filenames on a narrow terminal
 * by permitting the command-field to overlay the name-field if no room is
 * found on the right of it.
 */
#define	PCOLUMNS(n) (pcolumns[n] ? min(pcolumns[n],ccolumns[n]) : ccolumns[n])
int	dirent_ccol (void)
{
	register width = crt_width()-10,
	ccol = PCOLUMNS(0) + PCOLUMNS(1) + ccolumns[2] + 6;
	return (min(width, ccol));
}

/* <dirent_chk>:
 * Re-read a directory entry to obtain newer information about a file.
 */
int	dirent_chk (
	FILENT	*z,		/* data block to load into	*/
	char	*filespec)	/* filename to check up on	*/
{
	char	longspec[NAM$C_MAXRSS];

	if ((z->fstat = dirent_look (longspec, filespec)) == RMS$_NORMAL)
	{
		zrsa[znam.nam$b_rsl] = EOS;
		return (dirent__one (z, zrsa));
	}
	else if (! zDELETED(z))
	{
		warn2 ("Unexpected RMS-err: '%s' -> %X", filespec, z->fstat);
		flist_sysmsg (z->fstat);
	}

	return (FALSE);		/* file was not found	*/
}

/* <dirent_chk2>:
 * Re-read an entry in 'filelist[]'.  The primary intent of this is to check
 * for deleted files, and to mark them as such in this code.  If the file
 * still exists, TRUE is returned.  In either case, the display-line is
 * refreshed.
 */
int	dirent_chk2 (int j)
{
	int	retval;
	char	fullname[NAM$C_MAXRSS];

	dirent_glue (fullname, FK_(j));
	if (! (retval = dirent_chk (FK_(j), fullname)))
	{
		if (DELETED(j))	dirent_dlet(j);
	}
	dds_line (j);
	return (retval);
}

/* <dirent_chk3>:
 * Check a given filename.  If it exists, update the 'filelist[]' entry.  If
 * not, mark the given 'filelist[]' entry deleted.  This is used in DELETE
 * to uncover lower versions of a file which has been deleted.  Because the
 * name may not necessarily be the same as that in the 'filelist[j]' entry,
 * we must re-link the data-object to load altered data.
 */
int	dirent_chk3 (int j, char *fullname)
{
	FILENT	znew;		/* Don't destroy the old FILENT block	*/
	int	retval;

	if (retval = dirent_chk (&znew, fullname))
		dds_add2 (&znew, j);
	else
	{
		if (zDELETED((&znew)))
		{
			dirent_dlet(j);
			dds_line (j);
		}
		else
			dds_width (&znew, j);
	}
	return (retval);
}

/* <dirent_chop>:
 * Parse path, name, type and version for a full filename, loading it to
 * the indicated FILENT entry.  The filespec is assumed to be syntactically
 * correct, since it is obtained via RMS.
 *
 * We include the trailing '.' in the filename-string to force our sort to
 * collate in the same manner as the SYS$SEARCH.
 */
int	dirent_chop (FILENT *z, char *filespec, struct NAM *nam_)
{
	struct	FAB	fab;
	struct	NAM	nam;
	int	len, num,
		status	= RMS$_NORMAL;
	char	esa	[NAM$C_MAXRSS];

	z->fvers = 0;

	if (!nam_)
	{
		/* Set up RMS buffers to do name-parse: */
		rmsinit_fab (&fab, &nam, 0, filespec);
		rmsinit_nam (&nam, 0, esa);

		if (! $VMS_STATUS_SUCCESS(status = sys$parse(&fab)))
			return (status);
		/* esa[] <= parsed name	*/
		nam_ = &nam;
	}

#define	HEAP(n,l)	nameheap(nam_->n, nam_->l, (void *)&namelist)

	z->fvers = scanver (nam_->nam$l_ver, nam_->nam$b_ver);
	z->ftype = HEAP(nam$l_type+1,nam$b_type-1);
	z->fname = HEAP(nam$l_name,nam$b_name+NAME_DOT); /* name, with '.' */

	z->fpath_ = dirpath_sort (nam_->nam$l_node,
				  len = nam_->nam$l_name - nam_->nam$l_node);
	return (status);
}

/* <dirent_conv>:
 * Convert a FILENT entry to printing form, showing only those fields selected
 * in 'conv_list[]'.
 */
void	dirent_conv (char *bfr, FILENT *z)
{
	DATENT	*date_;
	int	j, num,
		col0	= PCOLUMNS(0),
		col1	= PCOLUMNS(1),
		width	= crt_width(),
		no_priv	= zNOPRIV(z);
	static
	char	sFMT1[]	= "%%-%d.%ds %%-%d.%ds;%%.%ds",
		noDATE[] = "%-17s";

	char	tmp[CRT_COLS+256],
		tempid[80],
		*c_	= bfr,
		*ccol_	= conv_list,
		format	[sizeof(sFMT1)+20],
		filesize[20],
		version	[20],
		misc	[30];

#define	NXT	strnull(strcat(c_, "  "))
#define	AFTER_NXT (width - strlen(NXT) - 2)

	if (zDELETED(z))
	{
		*c_ = EOS;
		return;
	}

	if (z->fvers == WILD_VER)	strcpy (version, "*");
	else if (z->fvers)		sprintf (version, "%d", z->fvers);
	else				strcpy (version, "");

	sprintf  (format, sFMT1,
		col0,col0,		/* 'fname'	*/
		col1,col1,		/* 'ftype'	*/
		ccolumns[2]+1);		/* 'fvers', pad	*/
	sprintf  (bfr, format, z->fname, z->ftype, version);
	j = strlen(z->fname) - NAME_DOT;
	if (j > col0)			bfr[col0]	 = '|';
	else if (j >= 0)		bfr[j]		 = ' ';
	if (strlen(z->ftype) > col1)	bfr[col0+col1+1] = '|';

	sprintf	(filesize, "%u", z->fsize);

	/*
	 * If I have any data-columns to show, pad the name out to the
	 * beginning of the "command-column" first.
	 */
	if (AnyXAB)
	{
		num	= strlen(bfr);
		c_	= bfr + num;		/* => ending-null */
		while (num++ < dirent_ccol())
			*c_++ = ' ';
		*c_ = EOS;
	}

	/*
	 * Check for early termination of this conversion:
	 */
	if (zLOCKED(z))		/* file is locked, cannot open	*/
		strcpy (filesize, "*lock");
	else if (z->fstat != RMS$_NORMAL && !no_priv)
	{
		if (z->fstat)		/* If zero, no data	*/
			sysgetmsg (z->fstat, NXT, AFTER_NXT);
		return;
	}
	strcpy(tmp, bfr);		/* do the rest in a big-buffer	*/
	c_ = tmp + strlen(tmp);		/* ...to guard against overflow	*/

	/*
	 * Format each display-column in succession, under control of the
	 * 'conv_list'.
	 */
	prot_col = -1;			/* If visible, mask-col is reset */
	while (*ccol_ && ((c_-tmp) < width))
	{
		c_ = NXT;
		switch (*ccol_++)
		{
		case 'p':		/* PATH (node::directory) */
			sprintf (format, "%%-%ds", ccolumns[3]);
			sprintf (c_, format, zPATHOF(z));
			break;

		case 'a':		/* ALLOC (allocation)	*/
			dirent__cnv2 (no_priv, c_, z->fallc);
			break;

		case 's':		/* SIZE (usage)		*/
			if (no_priv)
				sprintf (c_, "%-5s", " ");
			else
				sprintf (c_, "%5.5s", filesize);
			break;

		case 'l':		/* LENGTH (of record)	*/
			dirent__cnv2 (no_priv, c_, z->f_recl);
			break;

		case 'f':		/* FORMAT (record)	*/
			*c_++ = (z->ftext ? '*' : ' ');
			j = z->f_rfm & FAB$M_RFM;
			if (j < SIZEOF(known_rfm))
				strcpy (misc, known_rfm[j]);
			else
				strcpy (misc, "?");
			if ((j = (z->f_rfm >> FAB$V_ORG)) > 0)
			{
				strcat (misc, ",");
				if (j < SIZEOF(known_org))
					strcat (misc, known_org[j]);
				else
					strcat (misc, "?");
			}
			sprintf (format, "%%-%d.%ds", ccolumns[4],ccolumns[4]);
			sprintf (c_, format, misc);
			break;

		case 'x':		/* XAB (record attributes)	*/
			{
				int	bit;
				char	*d_ = misc;
				*d_ = EOS;
				for (bit = 0; bit < SIZEOF(known_rat); bit++)
					if (z->f_rat & (1 << bit))
					{
						if (d_ != misc)	*d_++ = ',';
						strcpy (d_, known_rat[bit]);
						d_ = strnull(d_);
					}
				*d_ = EOS;
			}
			sprintf (format, "%%-%d.%ds", ccolumns[5],ccolumns[5]);
			sprintf (c_, format, misc);
			break;

		case 'e':	date_	= &z->fexpr;	goto date;
		case 'r':	date_	= &z->frevi;	goto date;
		case 'b':	date_	= &z->fback;	goto date;
		case 'c':	date_	= &z->fdate;	goto date;

		case 'd':		/* DATE "dd-mmm-yy hh:mm" */
			date_ = dirent__date (z, D_opt);

		date:
			if ((!no_priv)
			&&  isOkDate(date_)
			&&  !isBigDate(date_))
			{
				if (D_mode != 0)
					lib$day (&j, date_);
				if (D_mode > 0)
				{
					strncpy (c_, day_of_week[j % 7], 4);
					c_ += 4;
				}
				sysasctim (c_, date_, D_mode <= 1 ? 18 : 21);
				if (D_mode < 0)	/* "dd-mmm hh:mm" */
				{		/* "dd-mmm-yyyy " */
					if (j+180 > absolute_day)
						strcpy (&c_[6], &c_[11]);
				}
			}
			else
			{
				if (D_mode > 0)
				{
					strcpy (c_, "    ");
					if (D_mode > 1)
						strcat(c_, "   ");
					c_ = strnull(c_);
				}
				sprintf (c_, noDATE, " ");
			}
			if (D_mode < 0)	c_[12] = EOS;
			break;

		case 'm':		/* MASK (protection, len=20) */
			prot_col= 1 + (c_ - tmp);
			num	= z->fprot;
			for (j = 0; j < 4; j++)
			{
				*c_++ = (num & XAB$M_NOREAD)  ? '-' : 'r';
				*c_++ = (num & XAB$M_NOWRITE) ? '-' : 'w';
				*c_++ = (num & XAB$M_NOEXE)   ? '-' : 'e';
				*c_++ = (num & XAB$M_NODEL)   ? '-' : 'd';
				*c_++ = ' ';
				num >>= 4;
			}
			*(--c_) = EOS;
			break;

		case 'o':		/* OWNER (len=9)	*/
			if (no_priv)
				sprintf (c_, "%-9s", " ");
			else
				sprintf (c_, "%4o,%4o",
					z->f_grp, z->f_mbm);
			break;

		case 'u':		/* USER (len=18)	*/
			sprintf (c_, "%-*s", ccolumns[6], z->f_user ? z->f_user : "");
			break;

		/* Patch: Limit this to 13 columns until either we extend the
		   'ccolumns' array, or find a file-id which has a nonzero
		   [2]-entry. */
		case 'i':		/* IDENTIFIER (file-id)	*/
			if (no_priv)
				strcpy (misc, " ");
			else
				sprintf (misc, "%d,%d,%d", z->fidnum[0],
					z->fidnum[1], z->fidnum[2]);
			sprintf (c_, "%-13s", misc);
		}
	}
	strncpy(bfr, tmp, CRT_COLS-1);
	bfr[CRT_COLS-1] = '\0';
}

/* <dirent_dft>:
 * Return a pointer to wildcard, based on V_opt (VERSION).  If NOVERSION
 * is selected, all wildcard-SYS$SEARCH activity will be coerced to seek
 * only the current-version of files.  Otherwise, any version may be selected.
 */
char	*dirent_dft (void)
{
	return (V_opt ? "*.*;*" : "*.*;");
}

/* <dirent_dlet>:
 * Mark an entry in 'filelist[]' deleted.
 */
void	dirent_dlet (int j)
{
	FK(j).fstat = RMS$_FNF;
	numdlets++;
	if (j == dircmd_select(-2))	dircmd_select(-1);
	dds_line (j);
}

/* <dirent_glue>:
 * Concatenate all substrings to make a complete file specification, for
 * the indicated FILENT entry.
 */
char	*dirent_glue (char *filespec, FILENT *z)
{
	return (dirent_glue2 (filespec, z, -1));
}

/* <dirent_glue2>:
 * Concatenate all substrings to make a complete file specification, for
 * the indicated FILENT entry.  The 'curfile' argument is provided so that
 * warning messages and other similar functions can make the inclusion of
 * the pathname dependent on whether it is the same as that of the current
 * entry on the screen.  (The path of the current entry is also the default
 * directory.)
 */
char	*dirent_glue2 (char *filespec, FILENT *z, int curfile)
{
	static	char	fmt[] =
#if	NAME_DOT
			"%s%s;";
#else
			"%s.%s;";
#endif
	if (curfile >= 0)
	{
		strcpy (filespec, (zPATHOF(z) != PATHOF(curfile))
			? zPATHOF(z) : "");
	}
	else
		strcpy (filespec, zPATHOF(z));
	if (strcmp(filespec, "*") == 0)	strcat (filespec, ":");

	sprintf (strnull(filespec), fmt, z->fname, z->ftype);
	if (z->fvers == WILD_VER)
		strcat (filespec, "*");
	else if (z->fvers)
		sprintf (strnull(filespec), "%d", z->fvers);
	return (filespec);
}

/* <dirent_init>:
 * Initialize data structures used in this module (used only via main routine).
 */
static
void	dirent_init(void)
{
	/*
	 * Chain pointers for XAB (the "nxt" is the same offset in all variations)
	 */
	struct	XABALL	*first_ = 0, *last_ = 0;
	int	today_date[2];

	/* dirent_all state: */

	rmsinit_fab (&zfab, &znam, dirent_dft(), 0);
	rmsinit_nam (&znam, zrsa, zesa); /* Initialize name-block	*/

	/* dirent__one state: */

	xaball	= cc$rms_xaball;
	xabdat	= cc$rms_xabdat;
	xabfhc	= cc$rms_xabfhc;
	xabpro	= cc$rms_xabpro;

	/*
	 * Initialize extended-attribute-blocks.
	 *	For each file found, will retrieve the size "-a", the last
	 *	modification date "-d", and the protection codes "-m".
	 */
	if (A_opt)
	{
		if (first_ == 0) first_           = (struct XABALL *)&xaball;
		if (last_  != 0) last_->xab$l_nxt = (char *)&xaball;
		xaball.xab$l_nxt = (char *)&xabfhc;
		last_	= (struct XABALL *)&xabfhc;
	}
	if (D_opt)
	{
		if (first_ == 0) first_           = (struct XABALL *)&xabdat;
		if (last_  != 0) last_->xab$l_nxt = (char *)&xabdat;
		last_	= (struct XABALL *)&xabdat;
	}
	if (M_opt || O_opt)
	{
		if (first_ == 0) first_           = (struct XABALL *)&xabpro;
		if (last_  != 0) last_->xab$l_nxt = (char *)&xabpro;
		last_	= (struct XABALL *)&xabpro;
	}
	if (first_)
	{
		AnyXAB = TRUE;
		zfab.fab$l_xab = (char *)first_; /* ...and => XAB chain	*/
	}

	pcolumns[0] = pcolumns[1] = 0;	/* Show name,type as-is	*/
	prot_col = -1;		/* assume no mask-column until displayed */
	sys$gettim (today_date);
	lib$day (&absolute_day, today_date);
}

/* <dirent_isdir>:
 * Test a FILENT block to see if it is a directory.  While ACP can do additional
 * tests, we will assume that a ".DIR" filetype is enough indication.  Note
 * that VMS insists that the version number of a directory must be "1".
 */
int	dirent_isdir (FILENT *z)
{
	return ((strcmp ("DIR", z->ftype) == 0) && (z->fvers <= 1));
}

/* <dirent_look>:
 * Lookup a single file, returning (via pointer) its fully-resolved name,
 * and the lookup status.
 */
unsigned dirent_look (char *longspec, char *filespec)
{
	size_t	len;
	unsigned status;

	zfab.fab$l_fna = filespec;
	zfab.fab$b_fns = strlen(filespec);

	if ($VMS_STATUS_SUCCESS(status = sys$parse(&zfab)))
	{
		if ($VMS_STATUS_SUCCESS(status = sys$search(&zfab)))
		{
			strncpy (longspec, zrsa, len = znam.nam$b_rsl);
			longspec[len] = EOS;
		}
	}
	return (status);
}

/* <dirent_misc>:
 * Set/clear bits in the '.fmisc' component.  This is used to keep track of
 * wildcard searches (limited to entries in 'filelist[]').  By flagging
 * entries, we need examine each once, making it faster, and (in the case of
 * DELETE) more precise.
 *
 * Patch: This doesn't clear-specific, or do testing.
 */
void	dirent_misc (
	int	inx,		/* index into 'filelist[]', or negative	*/
	int	bitv)		/* value with 1's where setting is needed*/
{
	int	j;

	if (inx >= 0)	/* If legal index, set specific bit	*/
	{
		FK(inx).fmisc |= bitv;
	}
	else			/* Do a global change		*/
	{
		if (inx == -1)	/* Clear this bit everywhere ?	*/
		{
			for (j = 0; j < numfiles; j++)
				FK(j).fmisc &= (~bitv);
		}
		else		/* Set this bit everywhere	*/
		{
			for (j = 0; j < numfiles; j++)
				FK(j).fmisc |= bitv;
		}
	}
}

/* <dirent_nul>:
 * Create a dummy entry in 'filelink' so that we can mark an entry in 'filelist'
 * deleted without clobbering our database entry for the file which used to be
 * there.  For instance, we make this when we do a VERIFY and purge lots of
 * renamed entries from 'filelist[]'.
 */
void	dirent_nul (int inx)
{
	FILENT	ztmp = FK(inx);		/* make sure path-name is legal	*/
	ztmp.fname = ztmp.ftype = "";	/* dummy-out the name, type	*/
	ztmp.fvers = 0;			/* ...and version		*/
	ztmp.fstat = RMS$_FNF;		/* Show a DELETED() code	*/
	dirdata_one (&ztmp, &filelist[inx]);
	numdlets++;
	dds_line(inx);
}

/* <dirent_old>:
 * Given a filespec, convert it to a FILENT structure, and attempt to find
 * it in the current 'filelist[]' array.  If found, return its index.  Else,
 * return a "-1" or "-2".
 */
int	dirent_old (
	char	*filespec,
	int	need)	/* 0: no warn, 1: any file, 2: in filelist[]	*/
{
	FILENT	ztmp;

	return (dirent_old_any (&ztmp, filespec, need));
}

/* <dirent_old1>:
 * Lookup filename, returning TRUE if there is any file available by that name.
 * If not, flag a warning message.  If the lookup is successful, will return
 * the FILENT entry in the output argument.
 */
int	dirent_old1 (FILENT *z, char *spec)
{
	int	inx	= dirent_old_any (z, spec, 1);

	return (! didwarn());
}

/* <dirent_old_any>:
 * Given a filespec, convert it to a FILENT structure, and attempt to find
 * it in the current 'filelist[]' array.  If found, return its index.  Else,
 * return a "-1" or "-2".  If the file exists, the FILENT structure '*z' is
 * loaded with the data.
 */
int	dirent_old_any (
	FILENT	*z,		/* => buffer to load with file data	*/
	char	*filespec,	/* => file specification to lookup	*/
	int	need)		/* 0: no warn, 1: any file, 2: in filelist[] */
{
	char	longspec[NAM$C_MAXRSS];

	if (dirent_look (longspec, filespec) == RMS$_FNF)
		dirent_chop (z, filespec, (struct NAM *)0);
	else if (dirent__one (z, longspec))
	{
		int	j;
		for (j = 0; j < numfiles; j++)
		{
			if (DELETED(j))				continue;
			if (memcmp (z, FK_(j), FILENT_name_size) == 0)
				return (j);
		}
		if (need >= 2)
			warn ("File '%s' is not a member of file-list", filespec);
		return (-1);
	}

	if (need >= 1)
	{
		if (zDELETED(z))
			warn ("File not found: %s", filespec);
		else
			warn ("File isn't readable: %s", filespec);
	}
	return (-2);
}

/* <dirent_width>:
 * Latch the widest field of each type which is actually used in the display
 * list.  We call this from points at which we are actually loading a FILENT
 * block to the display list; other points may read a FILENT block which will
 * be included in the 'dirdata' list, but not displayed.
 */
int	dirent_width (FILENT *z)
{
	register int	len, num, latch;
	static	int	tens[6] = {1,10,100,1000,10000,100000};

	if (z)
	{
		LATCH(0,len = strlen(z->fname)-1);
		LATCH(1,len = strlen(z->ftype));
		for (len = 1; len < SIZEOF(tens) && z->fvers >= tens[len]; len++)
			/*EMPTY*/;
		LATCH(2,len);
		LATCH(3,len = strlen(zPATHOF(z)));

		/*
		 * Latch the widest FORMAT description, so that we can do this
		 * in minimum width:
		 */
		num = z->f_rfm & FAB$M_RFM;
		if (num >= SIZEOF(known_rfm))	len = 1;
		else				len = strlen(known_rfm[num]);
		if (z->f_rfm > FAB$M_RFM)	len += 4;
		LATCH(4,len);

		len = 0;
		for (num = 1; num < (1 << SIZEOF(known_rat)); num <<= 1)
			if (num & z->f_rat)
			{
				if (len)	len++;
				len += 3;
			}
		LATCH(5,len);

		len = strlen(z->f_user);
		LATCH(6,len);
	}
	else		/* Reset entire list	*/
	{
		ccolumns[0] = 9;	/* filename	*/
		ccolumns[1] = 3;	/* filetype	*/
		ccolumns[2] = 4;	/* version	*/
		ccolumns[3] = 1;	/* pathname	*/
		ccolumns[4] = 3;	/* format + (organization)	*/
		ccolumns[5] = 1;	/* attributes	*/
		ccolumns[6] = 8;	/* user-identifier */
		for (num = 0; num < numfiles; num++)
			if (!DELETED(num))
				latch |= dirent_width (FK_(num));
	}
	return (latch);
}

/* <dirent__cnv2>:
 * Convert (for 'dirent_conv') 16-bit numbers.
 */
static
void	dirent__cnv2 (int no_priv, char *c_, unsigned number)
{
	if (no_priv)
		sprintf (c_, "%-5s", " ");
	else
		sprintf (c_, "%5u", number);
}

/* <dirent__date>:
 * Given a date-selection option (/DC, /DB, /DR) return the appropriate
 * pointer to a date-field.  Note that this is used to control the
 * interpretation of the /AFTER and /BEFORE options.
 */
static
DATENT	*dirent__date (FILENT *z, int opt)
{
	register DATENT	*date_;

	if (opt == 4)		date_	= &z->fexpr;
	else if (opt == 3)	date_	= &z->frevi;
	else if (opt == 2)	date_	= &z->fback;
	else			date_	= &z->fdate;
	return (date_);
}

/* <dirent__datechek>:
 * Test a FILENT structure to see if it is permitted by the current date-
 * selection option (/AFTER or /BEFORE).
 */
int	dirent__datechek (FILENT *z)
{
	register int ok = TRUE;

	if (dateflag[0])
	{
	register
	DATENT	*date_	= dirent__date (z, dateflag[1]);
#ifdef __alpha
	int	gt	= *date_ > datechek;
#else
	register
	int	gt	=   (date_->date64[1]  > datechek.date64[1])
			|| ((date_->date64[0]  > datechek.date64[0]) &&
			    (date_->date64[1] == datechek.date64[1]));
#endif
		ok = (dateflag[0] > 0) ? gt : ! gt;
	}
	return (ok);
}

/* <dirent__look>:
 * Lookup information about a particular file, to fill all fields in the
 * indicated FILENT structure, except those set in 'dirent_chop'.  Returns
 * TRUE iff the file was found.
 *
 * To make the RMS-lookup go faster, we assume that the global 'znam' block
 * is set from the SYS$SEARCH for this file.
 */
static
int	dirent__look (
	FILENT	*z,			/* => Data block to load	*/
	char	*filespec)		/* Full, unique filename	*/
{
	unsigned status	= RMS$_NORMAL;		/* return-status	*/
	int	ok	= TRUE;
	char	temp[80];

	/*
	 * Fill the FILENT block with the name, and null-data:
	 */
	memset (((char *)z) + FILENT_name_size, 0,
		sizeof(FILENT) - FILENT_name_size);
	z->f_grp = z->f_mbm = z->fprot = -1;

	/*
	 * ACP is generally faster than RMS (less overhead), but cannot
	 * (under VMS 3.7) go across DECNET.
	 */
	if (znam.nam$l_fnb & NAM$M_NODE)
		status = rmslook (z, &zfab);
	else
		status = acplook (z, filespec, &znam);

	if (!P_opt && zNOPRIV(z))	ok = FALSE;

	vms_uid2s(temp, z->f_mbm, z->f_grp);
	if (!strcmp(temp, "[]"))
		temp[0] = '\0';
	/* FIXME: memory leak */
	z->f_user = strcpy(malloc(strlen(temp)+1), temp);

	return (ok);
}

/* <dirent__one>:
 * Lookup information about a particular file, to fill all fields in the
 * indicated FILENT structure.  Returns TRUE iff the file was found.
 * In all cases, the filename is parsed.
 */
static
int	dirent__one (
	FILENT	*z,			/* => Data block to load	*/
	char	*filespec)		/* Full, unique filename	*/
{
	dirent_chop (z, filespec, &znam);
	return (dirent__look(z, filespec));
}

/* <dirent__read>:
 * Lookup information about a particular file, to fill all fields in the
 * indicated FILENT structure.  Returns TRUE iff:
 *	(a) the file was found,
 *	(b) the file did not appear elsewhere in the list, and
 *	(c) the file does not conflict with /NOVERSION
 * If the name (counting version) is an exact duplicate, load the newer file
 * data, to support re-reads.
 */
static
int	dirent__read (
	char	*filespec,
	FLINK	**flink,
	int	update)		/* true if we *must* re-read directory	*/
{
	FILENT	filent;
	int	j,
		ok	= TRUE;

	if (update)
	{
		if (! dirent__one (&filent, filespec))	return (FALSE);
	}
	else		/* Look in database before the physical directory */
	{
		dirent_chop (&filent, filespec, &znam);
		if ((j = dirdata_find (&filent, flink)) <= 0)
		{
			if (! dirent__look (&filent, filespec))
				return (FALSE);
		}
		else
			filent	= (*flink)->fk;
	}

	if (! V_opt)	/* Suppress if a higher version is in 'filelist[]' */
	{
		if (update) j = dirdata_find (&filent, flink);
		/* Position the link-pointer if we have not already */
		if (dirdata_high (&filent, flink))
			ok = FALSE;
		else
		{
			dirdata_chg (&filent, flink, j);
			dirent_width (&filent);
		}
	}
	else		/* Create/replace (if duplicate) database entry */
	{
		dirdata_add (&filent, flink);	/* Get data's list-pointer */
		dirent_width (&filent);
	}

	if (ok)
	{
		for (j = 0; j < numfiles; j++)
		{
			if (filelist[j] == *flink)
			{
				ok = FALSE;
				break;
			}
		}
	}
	return (ok);
}
