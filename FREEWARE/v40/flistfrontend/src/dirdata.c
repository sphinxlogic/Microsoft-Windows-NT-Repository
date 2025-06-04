#ifndef NO_IDENT
static char *Id = "$Id: dirdata.c,v 1.10 1995/06/04 21:18:02 tom Exp $";
#endif

/*
 * Title:	dirdata.c
 * Author:	Thomas E. Dickey
 * Created:	13 Jul 1985
 * Last update:
 *		28 May 1995, prototypes
 *		22 Jul 1985, if rename, make sure path has same references as
 *			     the data objects, use 'dirpath_add'
 *		16 Jul 1985, added 'dirdata_one', 'dirdata_ren'.
 *
 * Function:	This module contains the principal routines which maintain the
 *		'filelink' linked-list of file information for FLIST.  We
 *		keep a single list of all files known to all levels of FLIST,
 *		distinguishing ownership by a given level with a bit-mask
 *		'.file_refs' (see NAMEHEAP).  The list is sorted in the order
 *		which a SYS$SEARCH would yield the entire list of files:
 *
 *		* increasing by pathname,
 *		* increasing by filename,
 *		* increasing by filetype, and
 *		* decreasing by version.
 *
 *		Each list entry contains a FILENT block (.fk) which records
 *		everything which we know about a particular file.
 *
 *		The linked-list routines use an initial-pointer argument so
 *		that successive calls (as within a SYS$SEARCH-loop) will tend
 *		to reduce search time: we need only search from the last file
 *		known.
 *
 * Entrypoint:	dirdata_add:	Add the given FILENT block to this level.
 *		dirdata_chg:	Add FILENT block, given prior link-pointer.
 *		dirdata_find:	Find a given FILENT block in the list.
 *		dirdata_high:	Search list for higher-version than FILENT block.
 *		dirdata_one:	Add FILENT block, searching from beginning.
 *		dirdata_ren:	Rename file, relinking block.
 */

#include	<stdlib.h>
#include	<string.h>

#include	<rms.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dirdata.h"
#include	"dirpath.h"
#include	"nameheap.h"

import(filelink);

/* <dirdata_find>:
 * Returns:
 *	1, iff we find the entry already in the list or
 *	-1, 0, iff the entry is not in the list.
 *
 * If we do not find the entry in the list, we leave 'lastp' set to the entry
 * which would precede the new data.  Thus, iff the list is empty, 'lastp' will
 * be zero.  The "-1" return value is used to distinguish the list-beginning
 * when the entry is not found in the list.
 */
int	dirdata_find (FILENT *z, FLINK **lastp)
{
FLINK	*now	= filelink,
	*old	= filelink;
int	found	= 0,
	first,
	cmp;

	if (lastp)	if (*lastp)	now	= *lastp;
	first	= (now == filelink);
#ifdef	DEBUG
	trace ("Find: %s%s.%s;%d\n", zPATHOF(z), z->fname, z->ftype, z->fvers);
	trace ("%08X ", now);
#endif

#define	BINCMP(q)	(cmp = (z->q - now->fk.q))
#define	STRCMP(q)	(cmp = strcmp(z->q, now->fk.q))
#define	DIFFP(q)	(z->q != now->fk.q)
#define	SAMEP(q)	(z->q == now->fk.q)

	while (now)
	{
		if (BINCMP(fpath_->path_sort) > 0)	goto after;
		else if (cmp < 0)			goto before;

		if (DIFFP(fname))
		{
			if (STRCMP(fname) > 0)		goto after;
			else if (cmp < 0)		goto before;
		}

		if (DIFFP(ftype))
		{
			if (STRCMP(ftype) > 0)		goto after;
			else if (cmp < 0)		goto before;
		}

		if (BINCMP(fvers) < 0)			goto after;
		else if (cmp >  0)			goto before;
		else if (cmp == 0)
		{
			found = 1;
			goto exit;
		}

		/*
		 * The new entry should go after the 'now' entry.  Link to the
		 * next one, to test.
		 */
after:		old = now;
		now = now->file_next;
		first = FALSE;
	}

	/*
	 * The new entry should go before the 'now' entry.  Assume that 'old'
	 * points to the proper position.
	 */
before:	if (first)
	{
		now	= filelink;	/* Insert at list-beginning	*/
		found	= -1;
	}
	else
		now	= old;

exit:	if (lastp)			*lastp	= now;
#ifdef	DEBUG
	trace ("=> %08X (%d):%d\n", now, found, first);
#endif
	return (found);
}

/* <dirdata_add>:
 * Unconditionally add/replace data in the linked-list.  We use this entry, for
 * example, when showing the result of reading a directory (without suppressing
 * lower versions).
 */
void	dirdata_add (FILENT *z, FLINK **lastp)
{
	dirdata_chg (z, lastp, dirdata_find (z, lastp));
}

/* <dirdata_chg>:
 * Add/replace data in the linked-list, using the return value from 'dirdata_find'
 * to guide the link-placement.  We use this entry, for example, when reading
 * a directory subject to pruning lower versions (i.e., first, '_find', then
 * '_high', then '_chg'.
 */
void	dirdata_chg (FILENT *z, FLINK **lastp, int found)
{
	FLINK	*p, *q, *r;

#ifdef	DEBUG
trace ("CHG: %08X %d\n", *lastp, found);
#endif
	if (found <= 0)
	{
		p = calloc (1, sizeof(FLINK));
		if (q = *lastp)
		{
			if (found < 0)	/* Insert at beginning	*/
			{
				p->file_next = filelink;
				filelink = p;
			}
			else		/* Normal insertion	*/
			{
				r = q->file_next;
				q->file_next = p;
				p->file_next = r;
			}
		}
		else			/* Only entry in list	*/
			filelink = p;
		*lastp	= p;
	}
	else
		p = *lastp;
	p->file_refs |= nameheap_ref();
	p->fk	= *z;
#ifdef	DEBUG
	trace ("FLINK: ");
	for (p = filelink; p; p = p->file_next)
		trace ("%08X ", p);
	trace ("\n");
#endif
}

/* <dirdata_high>:
 * Search the list to determine if there is an entry with a higher version
 * than the FILENT-block.  Use the 'lastp' argument as a reference; don't
 * need to search past this point.
 */
int	dirdata_high (FILENT *z, FLINK **lastp)
{
	FLINK	*now = filelink;
	int	cmp;

	while (now)
	{
		if (SAMEP(fpath_) && SAMEP(fname) && SAMEP(ftype))
			return (BINCMP(fvers) < 0);
		if (now == *lastp)	break;
		now = now->file_next;
	}
	return (FALSE);
}

/* <dirdata_one>:
 * Search from list-beginning to insert/update a FILENT block.  We use this for
 * code which is not driven from a SYS$SEARCH loop.
 */
void	dirdata_one (FILENT *z, FLINK **lastp)
{
	*lastp = filelink;
	dirdata_add (z, lastp);
}

/* <dirdata_ren>:
 * Rename a file.  Because entries in 'filelist' will point to the original
 * block, we do not simply allocate a new block, but re-use the old one.
 * This requires that we re-link the list.
 */
void	dirdata_ren (FILENT *znew, FILENT *zold)
{
	FLINK	*pnew = 0,
		*pold = 0;
	int	found = dirdata_find (znew, &pnew);

	if (!memcmp(znew, zold, sizeof(FILENT)))
		return;		/* don't waste my time !! */

	/*
	 * If the "new" name is in the list already, then this is probably
	 * (if no bug!) because the caller marked the file deleted.  To safely
	 * dispose of this, we must move it also in the list by making its
	 * version number non-conflicting with other entries.
	 */
	if (found > 0)
	{
	FLINK	*old,
		*now	= pnew->file_next;

	    /*
	     * De-link the "new" entry from the list, saving pointer-before.
	     */
	    if (pnew == filelink)
	    	old = filelink = pnew->file_next;
	    else
	    {
		for (old = filelink; old->file_next != pnew; old = old->file_next);
		old->file_next = pnew->file_next;
	    }

	    /*
	     * If the block is referenced by more than the current level, find
	     * a new insertion point (with a negative version code to make it
	     * transparent to other data).
	     */
	    if (pnew->file_refs != nameheap_ref())
	    {
	    FILENT	*z	= &(pnew->fk);
	    int		tvers	= -1;

		now = old->file_next;	/* => after de-linked "new" data */
		while (now)
		{
		    if (!(SAMEP(fpath_) && SAMEP(fname) && SAMEP(ftype)))
			break;
		    if (tvers >= now->fk.fvers) tvers = now->fk.fvers - 1;
		    old = now;
		    now = now->file_next;
		}
		pnew->fk.fvers = tvers;
		if (old == filelink)
		{
		    pnew->file_next = filelink;
		    filelink = pnew;
		}
		else
		{
		    pnew->file_next = old->file_next;
		    old->file_next  = pnew;
		}
	    }
	    else	/* simply de-allocate the entry	*/
		cfree (pnew);
	}

	/*
	 * Now, we know that the "new" name does not appear in the list.  We
	 * can re-link the entry in which we have the "old" filename stored
	 * (if it is in the list).
	 *
	 * From the code above, we now have no instance of 'znew' in the list.
	 */
	if ((found = dirdata_find (zold, &pold)) > 0)
	{
	FLINK	*now = filelink;

	    if (now == pold)	/* de-link at beginning */
		filelink = now->file_next;
	    else
	    {
		while (now->file_next != pold)
		    now = now->file_next;
		now->file_next = pold->file_next;
	    }
	    pold->fk = *znew;
	    pnew = nullS(FLINK);
	    if ((found = dirdata_find (znew, &pnew)) < 0)
	    {
		pold->file_next = filelink;
		filelink = pold;
	    }
	    else
	    {
		pold->file_next = pnew->file_next;
		pnew->file_next = pold;
	    }
	    pold->fk = *znew;
	}
	else
	{
	    pold = 0;
	    dirdata_one (znew, &pold);
	}

	/*
	 * Ensure that any strings and pathnames to which we refer have at least
	 * the same scope as the object which we have just renamed:
	 */
	dirpath_add  (pold->file_refs, pold->fk.fpath_);
	nameheap_add (pold->file_refs, pold->fk.fname);
	nameheap_add (pold->file_refs, pold->fk.ftype);
}
