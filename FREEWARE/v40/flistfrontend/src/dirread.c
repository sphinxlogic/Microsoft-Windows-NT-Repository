#ifndef NO_IDENT
static char *Id = "$Id: dirread.c,v 1.5 1995/06/04 23:11:58 tom Exp $";
#endif

/*
 * Title:	dirread.c
 * Author:	Thomas E. Dickey
 * Created:	31 Jan 1985
 * Last update:
 *		19 Feb 1995, prototyped
 *		15 Jun 1985, typed 'realloc', 'calloc'.
 *		05 Feb 1985, added 'dirread_path' entry.
 *		01 Feb 1985
 *
 * Function:	This module updates the read-list for FLIST.  Each time a
 *		directory search is performed (i.e., by the initial invocation,
 *		or by the READ command), FLIST calls this module to unite
 *		the data with the read-list.  We do pruning here, because we
 *		want to make the full re-READ of the display list fast.
 *		By eliminating redundant search-patterns, we can keep the
 *		read-list from growing faster than the display list.
 *
 *		We perform both directions of pruning:
 *
 *		    a)	A pattern which is a proper subset of at least one
 *			pattern already in the read-list is ignored.
 *		    b)	A pattern which is a proper superset of one or more
 *			patterns already in the read-list causes those patterns
 *			to be replaced by the new pattern.
 */

#include	<string.h>

#include	<stdlib.h>
#include	<rms.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dirfind.h"
#include	"dirread.h"

import(readlist);	import(readllen);

/* <init>:
 * Initialize this module for a new directory-level:
 */
void
dirread_init (void)
{
	readllen = 0;
}

/* <put>:
 * Put a new entry into the read-list:
 */
void
dirread_put (struct FAB *fab_)
{
	struct	NAM	*nam_	= fab_->fab$l_nam;	/* patch: should pass NAM */
	FILENT	fz;
	register int	j, k;

	dirent_chop (&fz, nullC, nam_);

	if (readllen)
	{
	    /*
	     * First, see if the new pattern is a subset of anything already
	     * in the list:
	     */
	    for (j = 0; j < readllen; j++)
	    {
		if (dirfind_tst (&fz, &readlist[j], FALSE))	return;
	    }
	    /*
	     * If we get this far, then the new pattern is not a proper subset
	     * of any already there.  However, it may absorb others:
	     */
	    for (j = 0; j < readllen; j++)
	    {
		if (dirfind_tst (&readlist[j], &fz, FALSE))
		{
		    for (k = j; k < readllen; k++)
			readlist[k] = readlist[k+1];	/* compress list */
		    readllen--;
		    j--;		/* ...and restart comparison */
		}
	    }
	}
	j = (readllen + 2) * sizeof(FILENT);
	if (readllen)
		readlist = realloc (readlist, j);
	else
		readlist = calloc (1, j);
	readlist[readllen++] = fz;
}

/* <get>:
 * Return a string pointer to the inx'th item in the read-list.  If no
 * more items remain, return null.
 */
char	*dirread_get (char *filespec, int inx)
{
	return ((inx < readllen) ? dirent_glue (filespec, &readlist[inx]) : nullC);
}

/* <free>:
 * Release the structure on exit to a higher directory level:
 */
void	dirread_free (void)
{
	cfree (readlist);
}

#ifdef	DEBUG
void	dirread_show (void)
{
	int	j;
	char	filespec[MAX_PATH];

	trace ("READ-LIST:\n");
	for (j = 0; j < readllen; j++)
		trace ("%d: '%s'\n", j, dirent_glue(filespec, &readlist[j]));
}
#endif

/* <path>:
 * Given a (potential) index into the read-list, return a pointer to the
 * pathname portion.  If the index lies beyond the end of the list, return
 * null.  If the pathname is not unique (i.e., is a repeat of previous
 * entries in the list), skip to the next entry.
 *
 * To permit the skip, we pass the address of the index.
 */
char	*dirread_path (int *inx_)
{
	FILENT	*z;
	int	j;
	char	*c_;

	if (*inx_ >= readllen || *inx_ < 0)
		return (nullC);
	else
	{
		z  = &readlist[*inx_];
		c_ = zPATHOF(z);
		for (j = 0; j < *inx_; j++)
		{
			z = &readlist[j];
			if (strcmp(c_, zPATHOF(z)) == 0)
			{
				*inx_ = *inx_ + 1;
				return (dirread_path(inx_));
			}
		}
		return (c_);
	}
}
