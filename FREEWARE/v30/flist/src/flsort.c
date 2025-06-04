/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: flsort.c,v 1.11 1995/10/25 13:39:18 tom Exp $";
#endif

/*
 * Title:	flsort.c - "FLIST" sorting
 * Author:	T.E.Dickey
 * Created:	04 May 1984
 * Last update:
 *		24 Oct 1995	added USER-sort
 *		18 Mar 1995	prototypes
 *		18 Feb 1995	port to AXP (DATENT mods).
 *		02 Jan 1989	modified to use 'qsort' instead of my selection-
 *				sort.
 *		04 Nov 1988	added sort for expired-date.
 *		24 Sep 1985	don't need to upper-case 'xdcl_' stuff
 *		29 Aug 1985	corrected handling of /SWEEK if applied to the
 *				backup date.
 *		29 Jul 1985	assume 'filelink' pointer is unique for resetting
 *				selection-entry.
 *		03 Jul 1985	cleanup 'filelist' definition.
 *		04 May 1985	renamed from 'dirsrt', made it called from
 *				'dircmd' rather than indirectly via main-code.
 *		22 Mar 1985	added IDENTIFIER, LENGTH
 *		09 Jan 1985	nailed down column-suppression, fixed ordering
 *				for TYPE-sort.
 *		29 Dec 1984	added BACKUP, CREATED, REVISED and WEEK
 *		22 Dec 1984	renamed ATTRIBUTE to XAB (for ALLOC).
 *		11 Dec 1984	added FORMAT, ATTRIBUTE sorting, moved HOUR
 *				decoding into this module.
 *		13 Sep 1984	corrected permission to sort OWNER
 *		03 Sep 1984	use "import"
 *		27 Aug 1984	corrected no-action exit
 *		15 Aug 1984	separated CREATED,BACKUP,REVISED dates
 *		06 Aug 1984	added sort-by-hour
 *		05 Aug 1984	use VMS-C structure-copy feature.
 *		15 Jul 1984	sort paths via sequence key
 *		14 Jul 1984	added OWNER-sort, enhanced warnings
 *		20 Jun 1984	Do fixed-point sort (uses 'dircmd_select()')
 *		19 Jun 1984	to add "Sorting..." message.
 *		14 May 1984
 *
 * Function:	This module performs the directory-sorting operations for
 *		the "FLIST" program.  The following fields are sortable:
 *
 *		P	path,name,type,vers
 *		N	name,type,vers,path (normal)
 *		T	type,name,vers,path
 *		V	vers,name,type,path
 *
 *		A	alloc	[,name,type,vers,path]
 *		B	backup	[,name,type,vers,path]
 *		C	created	[,name,type,vers,path]
 *		D	date	[,name,type,vers,path]
 *		E	expir	[,name,type,vers,path]
 *		F	format	[,name,type,vers,path]
 *		H	hour	[,name,type,vers,path]
 *		M	protect	[,name,type,vers,path]
 *		O	owner	[,name,type,vers,path]
 *		R	revised	[,name,type,vers,path]
 *		S	size	[,name,type,vers,path]
 *		U	user	[,name,type,vers,path]
 *		W	week	[,name,type,vers,path]
 *		X	xab	[,name,type,vers,path]
 *
 * Parameters:	opt -	  alphabetic code (see above)
 *		order -	  normal/reverse flag (TRUE = reverse).
 *		curfile - index into 'filelist[]' of current file
 *		fix -     temporarily "fix" the current location, restoring the
 *			  original file-entry which may have been selected.
 *
 * Returns:	The index into 'filelist[]' at which the cursor should be
 *		placed after the sort.  If the sort is not applicable (because
 *		no data was read for this purpose), the original index is
 *		returned.  Because a 'dds_pack' is performed for all applicable
 *		sort operations, the file-index may be adjusted even if no
 *		changes result directly from the sort.
 */

#include	<stdlib.h>	/* declares 'qsort()' */
#include	<ctype.h>
#include	<string.h>

#include	<lib$routines.h>
#include	<rmsdef.h>

#include	"flist.h"
#include	"dircmd.h"
#include	"dds.h"

#include	"sysutils.h"

import(filelist); import(numfiles);
import(A_opt);	import(D_opt);	import(M_opt);	import(O_opt);

static
char	*full_opt[] = {
		"PATH",		"NAME",		"TYPE",		"VERSION",
		"ALLOC",	"BACKUP",	"CREATED",	"DATE",
		"EXPIRED",
		"FORMAT",	"HOUR",		"MASK",		"OWNER",
		"REVISED",	"SIZE",		"USER",
		"XAB"};
static
int	by_hour = 0;		/* Last HOUR-mode used	*/

static	int	flsort_fix (FLINK *f_);

/*
 * The following sort-definitions return -1 if the first argument is lower,
 * 0 if equal, +1 if greater.  Definitions are used rather than procedures
 * to try to make this code run as rapidly as possible (though larger).
 *
 * Notes:
 *	1. Strings are sorted in the opposite sense (increasing order)
 *	   to the sense of the size, date, protection codes.
 *	2. Even though the filetype is only three bytes, I must use 'strcmp'
 *	   to compare it, because the ordering of bytes on VAX puts the left
 *	   character in the least significant position.
 */

#define	LSS	-1
#define	GTR	1
#define	EQL	0

#define	hi(z)		(z.date64[1])
#define	lo(z)		(z.date64[0])

#define	sort_FWD(n1,n2)	(cmp = ((n1 < n2) ? LSS : ((n1 > n2) ? GTR : EQL)))
#define	sort_NUM(n1,n2)	(cmp = ((n1 < n2) ? GTR : ((n1 > n2) ? LSS : EQL)))

#define	sort_STR(s1,s2) (cmp = strcmp(s1, s2))

#define	sort_P(z1,z2)	sort_FWD(z1->fpath_->path_sort, z2->fpath_->path_sort)
#define	sort_N(z1,z2)	sort_STR(z1->fname,  z2->fname)
#define	sort_T(z1,z2)	sort_STR(z1->ftype,  z2->ftype)
#define	sort_U(z1,z2)	sort_STR(z1->f_user, z2->f_user)

#define	sort_V(z1,z2)	sort_NUM(z1->fvers,  z2->fvers)

#ifdef __alpha
#define	sort_D(z1,z2)	sort_NUM(z1,z2)
#else
#define	sort_D(z1,z2)	(sort_NUM(hi(z1),hi(z2)) ? cmp :\
			 sort_NUM(lo(z1),lo(z2)))
#endif

#define	sort_H(z1,z2)	sort_NUM(z1->fhour,  z2->fhour)

#define	sort_S(z1,z2)	sort_NUM(z1->fsize,  z2->fsize)
#define	sort_A(z1,z2)	sort_NUM(z1->fallc,  z2->fallc)
#define	sort_M(z1,z2)	sort_NUM(z1->fprot,  z2->fprot)
#define	sort_O(z1,z2)	(sort_FWD(z1->f_grp, z2->f_grp) ? cmp :\
			 sort_FWD(z1->f_mbm, z2->f_mbm))
#define	sort_F(z1,z2)	sort_NUM(z1->f_rfm,  z2->f_rfm)
#define	sort_X(z1,z2)	sort_NUM(z1->f_rat,  z2->f_rat)

/*
 * Define sorting for file entries which cannot be fully displayed, due to
 * privilege violation (SIZE, DATE, MASK and OWNER are affected):
 */
#define	sortPRV(z1,z2)	(cmp = (zNOPRIV(z1) ? (zNOPRIV(z2) ? EQL : GTR)\
					    : (zNOPRIV(z2) ? LSS : EQL) ))

static	int	sortopt,	/* copy of 'opt' */
		sortord;	/* copy of 'order' */

static
int	compare(const void *p1, const void *p2)
{
	register FILENT	*f1	= &((*(FLINK **)p1)->fk),
			*f2	= &((*(FLINK **)p2)->fk);
	register int	cmp	= 0;

	switch (sortopt)
	{
	case 'P':
		if (sort_P(f1,f2))	break;
		if (sort_N(f1,f2))	break;
		if (sort_T(f1,f2))	break;
		sort_V(f1,f2);
		break;
	case 'T':
		if (sort_T(f1,f2))	break;
		if (sort_N(f1,f2))	break;
		if (sort_P(f1,f2))	break;
		sort_V(f1,f2);
		break;
	case 'V':
		if (sort_V(f1,f2))	break;
		goto by_name;
	case 'S':
		if (sortPRV(f1,f2))	break;
		if (sort_S(f1,f2))	break;
		goto by_name;

	case 'A':
		if (sortPRV(f1,f2))	break;
		if (sort_A(f1,f2))	break;
		goto by_name;

	case 'C':			/* CREATED	*/
		if (sortPRV(f1,f2))	break;
		if (sort_D(f1->fdate,f2->fdate))	break;
		goto by_name;
	case 'B':			/* BACKUP	*/
		if (sortPRV(f1,f2))	break;
		if (sort_D(f1->fback,f2->fback))	break;
		goto by_name;
	case 'R':			/* REVISED	*/
		if (sortPRV(f1,f2))	break;
		if (sort_D(f1->frevi,f2->frevi))	break;
		goto by_name;
	case 'E':			/* EXPIRED	*/
		if (sortPRV(f1,f2))	break;
		if (sort_D(f1->fexpr,f2->fexpr))	break;
		goto by_name;

	case 'H':
		if (sortPRV(f1,f2))	break;
		if (sort_H(f1,f2))	break;
		goto by_name;
	case 'M':
		if (sortPRV(f1,f2))	break;
		if (sort_M(f1,f2))	break;
		goto by_name;
	case 'O':
		if (sortPRV(f1,f2))	break;
		if (sort_O(f1,f2))	break;
		goto by_name;
	case 'U':
		if (sortPRV(f1,f2))	break;
		if (sort_U(f1,f2))	break;
		goto by_name;
	case 'I':
#define	sort_ID(n)	sort_NUM(f1->fidnum[n],f2->fidnum[n])
		if (sort_ID(0))		break;
		if (sort_ID(1))		break;
		if (sort_ID(2))		break;
		goto by_name;

	case 'L':
		if (sort_NUM(f1->f_recl,f2->f_recl))	break;
		goto by_name;

	case 'F':
		if (sortPRV(f1,f2))	break;
		if (sort_F(f1,f2))	break;
		goto by_name;
	/* Sort XAB within FORMAT for better ordering	*/
	case 'X':
		if (sortPRV(f1,f2))	break;
		if (sort_F(f1,f2))	break;
		if (sort_X(f1,f2))	break;
	/* Continue into NAME after numeric sorts:	*/
	by_name:
	default:
		if (sort_N(f1,f2))	break;
		if (sort_T(f1,f2))	break;
		if (sort_P(f1,f2))	break;
		if (sortopt != 'V')	sort_V(f1,f2);
	}
	return ((sortord > 0) ? cmp : -cmp);
}

tDIRCMD(flsort)
{
	FLINK	*cmpFIX, *cmpTMP;
	int	opt	= xdcl_->dcl_text[1],
		order	= (xdcl_->dcl_text[0] == 'S') ? GTR : LSS,
		fixpoint = dircmd_select(-2),	/* Save original selection-index */
		j0, j1,
		k0	= numfiles-1, k1,
		cmp, got,
		didsome	= 0,
		lpp2	= crt_lpp() - 1;
	DATENT	*c_;

	/* Verify that the option is legal one: */
	if (! isprint (opt))
		opt = 'N';

	for (j0 = 0; j0 < SIZEOF(full_opt); j0++)
	{
		if (opt == *full_opt[j0])
			break;
	}
	if (j0 == SIZEOF(full_opt))	j0 = 1;	/* Coerce to NAME	*/

	if ((strchr("AS",	opt)	&& !A_opt)
	||  (strchr("BCDEHRW",	opt)	&& !D_opt)
	||  (strchr("M",	opt)	&& !M_opt)
	||  (strchr("OU",	opt)	&& !O_opt) )
	{
		warn ("Sort (%s) is not applicable", full_opt[j0]);
		return;
	}

	if (opt == 'D')
	{
		switch (D_opt)
		{
		case 3:		opt = 'R';	break;
		case 2:		opt = 'B';	break;
		default:	opt = 'C';
		}
	}

	if (numfiles > 24)	dds_while (nullC);

	/*
	 * If we do a sort-by-hour, update the '.fhour' components to match
	 * the current date-selection option.  We use the same field for
	 * sort-by-day-of-week, since the two can't be combined anyway.
	 */
	if (strchr("HW", opt))
	{
	    got = (opt == 'H') ? D_opt : -D_opt;
	    cmp = (by_hour != got);
	    for (j0 = 0; j0 < numfiles; j0++)
	    {
		if (!DELETED(j0) && !NOPRIV(j0))
		{
		    switch (D_opt)
		    {
		    case 4:	c_ = &(FK(j0).fexpr);	break;
		    case 3:	c_ = &(FK(j0).frevi);	break;
		    case 2:	c_ = &(FK(j0).fback);	break;
		    default:	c_ = &(FK(j0).fdate);
		    }
		    if (cmp || ! FK(j0).fhour)
		    {
			if (isBigDate(c_))
			    FK(j0).fhour = 0xffff; /* biggest possible */
			else
			{
			    if (opt == 'H')
				FK(j0).fhour = syshour(c_) >> 16;
			    else
			    {
				lib$day (&j1, c_);
				FK(j0).fhour = ((j1 + 3) % 7) + 1;
			    }
			}
		    }
		}
	    }
	    by_hour	= got;
	    opt	= 'H';
	}

	if (fixpoint >= 0)	cmpFIX	= filelist[fixpoint];
	if (fix)		cmpTMP	= filelist[*curfile_];

	didsome = dds_pack (curfile_, TRUE); /* Get rid of all deleted files */
	sortopt = opt;		/* set global needed in 'compare()' */
	sortord = order;

	qsort(filelist, numfiles, sizeof(filelist[0]), compare);
	didsome++;	/* for this sort, always scroll to beginning */

	/*
	 * If I did a fixed-point search, find the new position of the entry,
	 * and:
	 *	(a) Reset the index to the new position, and
	 *	(b) Reset (if required) the top/bottom file indices to permit
	 *	    me to show this screen next.  The screen refresh is made
	 *	    complicated because of the compaction which may have been
	 *	    made with 'dds_pack()'.
	 *
	 * If I didn't do a fixed-sort, reset the current-file to the top-of
	 * list.
	 */
	if (didsome)
	{
		if (fixpoint >= 0 || fix)
		{
			if (fixpoint >= 0)
				dircmd_select (*curfile_ = flsort_fix (cmpFIX));
			if (fix)
				*curfile_ = flsort_fix (cmpTMP);

			k0 = lpp2 * (*curfile_ / lpp2); /* Nominal top-screen  */
			k1 = numfiles - lpp2;     /* Try a better estimate   */
			if (k1 >= 0 && k1 <= *curfile_)	k0 = k1;
			dds_all (k0, *curfile_);
		}
		else
			dds_all (0, *curfile_ = 0);
	}
	else
		warn ("No change in sort-order");
}

/*
 * Return the present index of a particular FILENT entry in the 'filelist[]'
 * array.
 */
static
int	flsort_fix (FLINK *f_)
{
	int	j;

	for (j = 0; j < numfiles; j++)
		if (f_ == filelist[j])		return (j);
	return (-1);
}
