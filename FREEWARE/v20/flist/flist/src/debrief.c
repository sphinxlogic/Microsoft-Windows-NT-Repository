/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: debrief.c,v 1.4 1995/06/04 19:27:42 tom Exp $";
#endif

/*
 * Title:	debrief.c
 * Author:	Thomas E. Dickey
 * Created:	16 Sep 1985
 * Last update:
 *		19 Feb 1995, prototypes
 *		21 Sep 1985	The 'vec[]' array may not be sorted.  Allow
 *				for this in generating a list of ambiguous
 *				matches.
 *		17 Sep 1985
 *
 * Function:	Perform a table-lookup for a command-keyword.  If the keyword
 *		is not found, determine if there is a partial match, generating
 *		an "ambiguous" message for this case, or "unknown" for the
 *		default.
 *
 * Arguments:	msg	=> buffer in which to return error message
 *		vec	=> array of structures a la VEC (no repeated entries!!)
 *		vsize	= actual size of *vec
 *		vlast	= number of entries in array *vec
 *		find 	- string to find (must be in the same case as the table).
 *		fsize	= length of 'find' (need not be null-terminated).
 *
 * Returns:	0..vlast-1 if legal match found
 *		-1, if partial match found (ambiguous)
 *		-2, if no match at all
 */

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>

#include	"strutils.h"

typedef	struct	{
	char	*match;
	int	brief;
	}	VEC;

#define	LOOP	for (v_ = vec, k = 0;\
			 k < vlast;\
			 k++, v_ = (VEC *)((char *)v_ + vsize))
#define	RANGE	4

int
debrief (
	char	*msg,
	VEC	*vec,
	int	vsize,		/* actual number of bytes/entry	*/
	int	vlast,
	char	*find,
	int	fsize)
{
	register VEC	*v_;
	int	vlong	= 0,
		i,
		j,
		k,
		range,
    		toomany	= 0,
		flen[RANGE];
	char	key[255],
		*s_,
		*d_,
		*fuse[RANGE];

	strncpy (key, find, fsize);
	key[fsize] = '\0';
	if (msg) *msg = '\0';

	LOOP
	{
	    if ((fsize >= v_->brief) && !strncmp (key, v_->match, fsize))
		return (k);
	    /*
	     * If the search-string is not as long as the minimum
	     * abbreviation, latch the length to which it matches.
	     */
	    else
	    {
		for (s_ = key, d_ = v_->match, j = 0;
		    *s_ && *d_ && *s_ == *d_;
			s_++, d_++, j++);
		if (j > vlong)	vlong = j;
	    }
	}

	/*
	 * If we have only partial-matches, make up a message showing the
	 * range:
	 */
	if (vlong)
	{
	    if (msg)
	    {
		range = 0;
		for (j = 0; j < RANGE; j++) {
			fuse[j] = 0;
			flen[j] = 0;
		}
		LOOP
		{
#define	CMP(j)	strcmp(v_->match, fuse[j])

		    if (!strncmp (v_->match, key, vlong))
		    {
			for (j = 0; j < range; j++) /* Find insertion-point */
			{
			    if (CMP(j) < 0) break;
			}

			if (j >= range)	/* New entry is higher than table */
			{
			    if (++range > RANGE)
			    {
				j--, range--, toomany++;
				/* Overwrite last entry in table */
				if (CMP(j) > 0)
				{
				    flen[j] = v_->brief;
				    fuse[j] = v_->match;
				}
			    }
			    else
			    {
				flen[j] = v_->brief;
				fuse[j] = v_->match;
			    }
			}
			else		/* Insert within the pointer-list */
			{
			    i = RANGE;	/* Assume we up-shift pointers */
			    if (toomany)
				i--;
			    else
			    {
				if (++range >= RANGE)
				    i--, range--, toomany++;
			    }
			    while (--i > j)
			    {
				flen[i] = flen[i-1];
				fuse[i] = fuse[i-1];
			    }
			    if ((j < RANGE-1) || (CMP(j) > 0))
			    {
				flen[j] = v_->brief;
				fuse[j] = v_->match;
			    }
			}
		    }
		}
		strcpy (msg, "Ambiguous keyword:");
		for (j = 0; j < range; j++)
		{
		    strcat (msg, " ");
		    if (toomany && (j == range-1))	strcat (msg, "... ");
		    for (d_ = strnull(msg), s_ = fuse[j]; *s_; s_++)
		    {
			*d_++ = (s_ > fuse[j]+flen[j]-1)
				? _tolower(*s_)
				: _toupper(*s_);
		    }
		    *d_ = '\0';
		}
	    }
	    return (-1);
	}
	else
	{
	    if (msg) sprintf (msg, "Unknown keyword: '%.80s'", key);
	    return (-2);
	}
}
