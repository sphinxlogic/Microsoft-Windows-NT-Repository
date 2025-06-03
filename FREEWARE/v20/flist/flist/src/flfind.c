/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: flfind.c,v 1.8 1995/06/06 10:18:44 tom Exp $";
#endif

/*
 * Title:	flfind.c
 * Author:	Thomas E. Dickey
 * Created:	21 May 1984
 * Last update:
 *		18 Mar 1995, prototyped
 *		14 Aug 1985, added code to make search-state nested.
 *		13 Aug 1985, corrected pointer-references to '->dcl_text' (had
 *			     '.dcl_text', which gave strange result).  Added
 *			     'flfind_show' entrypoint.
 *		16 Jun 1985, typed 'calloc' to shut up CC2.0; corrected arg
 *			     in memory-copy which broke NEXT-status.
 *		02 May 1985, changed call on 'dircmd_dirflg'.
 *		05 Jan 1985, permit "NEXT" keyword.  Altered interface with
 *			     'dirfind'.
 *		24 Dec 1984, added "NFIND" command.
 *		28 Jul 1984, broke off 'dirfind.c' code for common searches.
 *		24 Jul 1984, re-coded test for wildcard pathname
 *		15 Jul 1984, use PATHOF()
 *		10 Jul 1984, changed calling convention
 *		05 Jul 1984
 *
 * Function:	This module performs the FIND, NFIND, and NEXT functions
 *		for FLIST:
 *
 *		FIND -	initiates a search in the current direction, which is
 *			halted when a file entry is found matching the pattern.
 *		NFIND -	is the complement of FIND; halts when a mismatch is
 *			found.
 *		NEXT -	continues the last search.
 *
 * Patch:	Should provide a re-entry point to 'dirarg' so that we can
 *		re-substitute /P/N/T/V stuff if it is needed in NEXT.
 */

#include	<stdlib.h>
#include	<string.h>

#include	"flist.h"
#include	"dircmd.h"
#include	"dirfind.h"
#include	"dds.h"

typedef	struct	{
	int	findFLG;
	DCLARG	*findDCL;
	char	*findTXT;
	}	STK;

static	STK	*stk_	= 0;
static	int	deepest	= -1;

#define	FindFLG	stk_[lvl].findFLG
#define	FindDCL	stk_[lvl].findDCL
#define	FindTXT	stk_[lvl].findTXT

tDIRCMD(flfind)
{
	register int next,
		lvl	= flist_nest() - 1;	/* 0,1,2,...	*/
	DCLARG	*pattern;

	/*
	 * If we have a file-specification, this must be either FIND or
	 * NFIND.
	 */
	if (dclinx (xdcl_, 1, 0))
	{
		pattern	= xdcl_->dcl_next;	/* => file specification*/
		FindFLG	= (xdcl_->dcl_text[0] == 'N');
	}
	else
		pattern = FindDCL;		/* "NEXT" usage		*/

	/*
	 * Check if the user entered this code with CTRL/N first without
	 * initializing the search.
	 */
	if (!pattern->dcl_text[0])
	{
		warn ("FIND requires a filename template");
		return;
	}

	/*
	 * Save a copy of the specification for "NEXT":
	 */
	memcpy (FindDCL, pattern, sizeof(DCLARG));
	strcpy (FindTXT, pattern->dcl_text);
	FindDCL->dcl_text = FindTXT;

	next = dirfind (*curfile_, dircmd_dirflg(-1), pattern,
			(void(*)(int,int *))0,
			TRUE, /* Need exactly one file */
			FindFLG);

	if (next == *curfile_)
		warn ("Current entry is the only match");
	else if (next >= 0)
		dds_index (*curfile_ = next);
}

/* <flfind_init>:
 * The argument 'lvl' is numbered 0,1,2,..., for the actual number of the "prior"
 * nesting level of the directory editor.
 */
void	flfind_init (int lvl)
{
	if (lvl == 0)
	{
		stk_	= calloc (deepest = 1, sizeof(STK));
		FindTXT = calloc (1, MAX_PATH);
		FindDCL = calloc (1, sizeof(DCLARG));
	}
	else
	{
		if (lvl > deepest-1)
		{
			stk_	= realloc (stk_, (++deepest) * sizeof(STK));
			FindTXT = calloc (1, MAX_PATH);
			FindDCL = calloc (1, sizeof(DCLARG));
		}
		memcpy (FindDCL, stk_[lvl-1].findDCL, sizeof(DCLARG));
		strcpy (FindTXT, stk_[lvl-1].findTXT);
		FindFLG = stk_[lvl-1].findFLG;
	}
	FindDCL->dcl_text = FindTXT;
}

/* <flfind_show>:
 * Display current search-target: "?FIND".  Use 'dirfind_chop' to parse it
 * to show the actual interpretation of wildcards and implicit pathnames.
 */
void	flfind_show (void)
{
	register lvl	= flist_nest() -1;

	if (FindDCL)
	{
		if (*FindDCL->dcl_text)
		{
			FILENT	z;
			PATHNT	pz;
			char	p[MAX_PATH],	n[MAX_NAME],	t[MAX_TYPE],
				fullname[MAX_PATH];
			dirfind_chop (FindDCL, &z, &pz, p, n, t);
			dirent_glue (fullname, &z);
			flist_tell ("%sFind: %.132s", FindFLG ? "N" : "",
						fullname);
			return;
		}
	}
	warn ("No search target in effect");
}
