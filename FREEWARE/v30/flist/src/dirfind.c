/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: dirfind.c,v 1.8 1995/10/25 15:40:18 tom Exp $";
#endif

/*
 * Title:	dirfind.c
 * Author:	Thomas E. Dickey
 * Created:	28 Jul 1984 (broke off search code from 'flfind.c')
 * Last update:
 *		25 Oct 1995, corrected missing check for deleted-files in
 *			     'dirfind()'.
 *		18 Mar 1995, prototypes
 *		14 Sep 1985, enhanced 'strwcmp' to process "..." in pathname.
 *			     Therefore, we need substitute a "*" into the
 *			     pathname only for the case of non-explicit path.
 *		09 Sep 1985, count trailing '.' in fname-field for VMS-collating
 *		13 Aug 1985, added 'dirfind_chop' to make clean "?FIND" display.
 *		03 Jul 1985, cleanup 'filelist' definition
 *		23 Jun 1985, use 'scanver' to isolate version-scanning.
 *		15 Jun 1985, eliminated unused 'j' argument from 'if_T'.
 *			     Reference function-'each' as '(*each) to make
 *			     CC2.0 happy.
 *		02 May 1985, changed call on 'dircmd_dirflg'
 *		10 Mar 1985, revised PATHNT structure.
 *		08 Mar 1985, defined negative code for 'forward' argument.
 *		05 Feb 1985, broke out 'dirfind_notexp' to permit reuse.
 *		30 Jan 1985, re-code so that 'dirfind_tst' receives pointers to
 *			     two FILENT-blocks (permitting re-use).
 *		05 Jan 1985, use DCLARG-structure rather than string to
 *			     represent search-spec, so that we needn't do
 *			     another parse here on the default-spec to obtain
 *			     wildcard information.
 *		29 Dec 1984, do "working..." if 'dft_spec' uses wildcard,
 *			     when 'each' is set.
 *		24 Dec 1984, added 'unfind' argument so that this can support
 *			     the "NFIND" command (search for mismatch).
 *		14 Dec 1984, added nam-argument to 'dirent_chop'
 *		10 Sep 1984, use "rmsinit"
 *		26 Aug 1984, cleanup buffer sizes
 *		16 Aug 1984, use '.fmisc' flag to hit entries once
 *		13 Aug 1984, fixed use of 'must_find', permit quit from loop.
 *			     Added (kludgy) relative version testing.
 *		24 Jul 1984, re-coded test for wildcard pathname
 *		15 Jul 1984, use PATHOF()
 *		05 Jul 1984
 *
 * Function:	The 'dirfind' procedure performs in-list searches for FLIST.
 *		This directly supports the FIND and NEXT commands, as well
 *		as the VERIFY command (which must find specific wildcard lists
 *		in 'filelist[]').
 *
 *		The FIND/NEXT commands require that the search be startable
 *		from any point, in FLIST's sorted order rather than that known
 *		to SYS$SEARCH.  Therefore I use my own wildcard compare.
 *
 *		If 'each' is set, we use a separate loop from the "once"
 *		searches.  We use the actual search loop to find all files
 *		to which the search applies before calling 'each'.  This is
 *		needed to make DELETE (especially of current versions!) work
 *		properly.
 *
 * Parameters:	curfile	- is the starting index in 'filelist[].
 *		forward - defines direction of search:
 *			  (-1): Use default direction-flag to modify 'curfile'
 *				so that the apparent search will begin at
 *				'curfile' and loop around.
 *			  (0):	Search backward from 'curfile', with wraparound.
 *			  (1):	Search forward from 'curfile', with wraparound.
 *		find_spec - is a DCLARG structure containing the parsed filespec
 *			(containing wildcards) to use as the search pattern.
 *		each - is a pointer to a function to execute on each entry
 *			which matches.  If no function is given, assume that
 *			we wish to find only the next occurrence.  If the
 *			pointer isn't null, continue executing the search
 *			loop until no more items are found.  If the function
 *			returns FALSE, do a quit after executing.
 *		must_find - is TRUE if failure to find any entry will cause
 *			an error message.
 *		unfind - TRUE if we are searching for a mismatch, rather than
 *			a match.
 *
 * Returns:	If the filespec is found, return the index into 'filelist[]'
 *		at which it was found, or a negative number if it was not.
 *		(If 'each' is set, instead return the number of entries
 *		which were found.)
 *
 * patch:	Should provide date-wise selection options to support DELETE,
 *		PURGE, FIND.
 */

#include	<string.h>

#include	<rms.h>
#include	<stsdef.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dclarg.h"
#include	"dds.h"
#include	"dircmd.h"

#include	"dirfind.h"

#include	"scanver.h"
#include	"strutils.h"

import(filelist); import(numfiles);

int	dirfind (
	int	curfile,
	int	forward,
	DCLARG	*find_spec,
	void	(*each)(int, int *),
	int	must_find,
	int	unfind)
{
#define	FIND_FNB	find_spec->dcl$l_fnb
#define	FIND_TEXT	find_spec->dcl_text

PATHNT	pz;
FILENT	fz;

int	flag	= TRUE,		/* continue-loop unless 'each' quits	*/
	found_any = 0;
int	j, k, tst;
char	Fpath[MAX_PATH],	Fname[MAX_NAME+1],	Ftype[MAX_TYPE+1];

	dirfind_chop (find_spec, &fz, &pz, Fpath, Fname, Ftype);

	/*
	 * If 'forward' is negative, obtain the proper value from the
	 * command decoder:
	 */
	if (forward < 0)
	{
		forward = dircmd_dirflg(-1);
		curfile	= dirfind_nxt (curfile, !forward);
	}

#define	XOR(a,b) ((a && !b) || (!a && b))
#define	if_T	tst = dirfind_tst(FK_(j), &fz, TRUE);\
		if (XOR(tst,unfind))
#define	LOOP	for (j = dirfind_nxt(curfile,forward), k = numfiles; k > 0;\
			j = dirfind_nxt(j,forward), k--)
#define	BIT_1	1		/* Use this bit via 'dirent_misc'	*/

	if (each)		/* Multiple action ?	*/
	{
		dirent_misc (-1, BIT_1);	/* Reset all '.fmisc'	*/
		if (strchr(FIND_TEXT,'*') || strchr(FIND_TEXT,'%')
		||  strchr(Fpath, '*'))
			dds_while (nullC);

		for (j = 0; j < numfiles; j++)
		{
			if_T
				dirent_misc (j, BIT_1);
		}
		LOOP
		{
			if (DELETED(j))		continue;
			dds_while(nullC);
			if (FK(j).fmisc & BIT_1)
			{
				(*each)(j, &flag);
				found_any++;
				if (!flag)		break;
			}
		}
		dirent_misc (-1, BIT_1);	/* cleanup after use	*/
	}
	else
	{
		LOOP
		{
			if (DELETED(j))		continue;
			if_T			return (j);
		}
	}

	if (must_find && !found_any)
	{
		char	fullname[MAX_PATH];
		warn ("Not found: %s", dirent_glue (fullname, &fz));
	}

	if (found_any)	return (found_any);
	else		return (-2);
}

/*
 * Split the given DCLARG-pattern into a FILENT-block, so that the pathname,
 * filename, type and version will be readily accessible for use in the
 * directory-search.  This routine is also used to partly-parse the search
 * target when "?FIND" is executed.
 */
void	dirfind_chop (
	DCLARG	*find_spec,
	FILENT	*fz_,
	PATHNT	*pz_,
	char	*Fpath, char *Fname, char *Ftype)
{
	int	len	= 0,
		j,
		version;	/* value if version null or "*"		*/

#define	CPY(to)	if (len > 0) strncpy(to, &FIND_TEXT[j], len);\
		to[len] = EOS; j += len

	j	= 0;
	len	= find_spec->dcl$b_node
		+ find_spec->dcl$b_dev
		+ find_spec->dcl$b_dir;			/* Directory-path*/
	CPY(Fpath);

	len	= find_spec->dcl$b_name + NAME_DOT;	/* name with trailing '.' */
	CPY(Fname);
	j	-= NAME_DOT;		/* '.' is normally counted in type */

	len	= find_spec->dcl$b_type - 1;	j++;	/* Don't use "." */
	CPY(Ftype);

	version	= scanver (&FIND_TEXT[j], find_spec->dcl$b_ver);

	/*
	 * If pathname was not explicitly given, make the entire path wild.
	 */
	if (dirfind_notexp(find_spec))
		strcpy (Fpath, "*");

	/*
	 * Set up structure so we can use 'dirfind_tst':
	 */
	fz_->fpath_ = pz_;	pz_->path_text = Fpath;
	fz_->fname  = Fname;
	fz_->ftype  = Ftype;
	fz_->fvers  = version;
}

/*
 * Make an index to the next item to test:
 */
int	dirfind_nxt (int j, int forward)
{
	if (forward)
	{
		if ((++j) >= numfiles)	j = 0;
	}
	else
	{
		if ((--j) <  0)		j = numfiles-1;
	}
	return (j);
}

/*
 * Return true iff the pathname of the indicated DCLARG-structure was
 * not explicit.  Inexplicit pathnames within FLIST are treated as a special
 * type of wildcard.
 */
int	dirfind_notexp (DCLARG *find_spec)
{
	return (!(FIND_FNB & (NAM$M_EXP_DIR + NAM$M_EXP_DEV)));
}

/*
 * Test the entry 'z' against the wildcard and exact matches in 'zref':
 */
int	dirfind_tst (
	FILENT	*z,
	FILENT	*zref,
	int	lookup)	/* TRUE if we permit lookup for relative versions */
{
	FILENT	ztmp;
	char	relnam[NAM$C_MAXRSS];
	char	absnam[NAM$C_MAXRSS];

	if (strwcmp(zPATHOF(zref), zPATHOF(z)))	return (FALSE);
	if (strwcmp(zref->fname,   z->fname))	return (FALSE);
	if (strwcmp(zref->ftype,   z->ftype))	return (FALSE);

	/*
	 * If the reference version is a wild-card, or if there is an exact
	 * match (only possible for versions greater than zero), return
	 * true immediately.
	 */
	if (zref->fvers == WILD_VER
	||  zref->fvers == z->fvers)
		return (TRUE);
	/*
	 * Otherwise, if the reference version is negative (i.e., relative),
	 * construct a temporary copy of the test-name, and see if the given
	 * relative version is identical with the test-name.  To permit re-use
	 * (e.g., for read-list pruning), we permit the argument 'lookup' to
	 * suppress this clause.
	 *
	 * patch: This is slow, but works !!
	 */
	else if (zref->fvers <= 0 && lookup)
	{
		ztmp	= *z;
		ztmp.fvers = zref->fvers;
		dirent_glue (relnam, &ztmp);
		if (! $VMS_STATUS_SUCCESS (dirent_look (absnam, relnam)))
			return (FALSE);
		dirent_chop (&ztmp, absnam, 0);
		return (ztmp.fvers == z->fvers);
	}
	else
		return (FALSE);
}
