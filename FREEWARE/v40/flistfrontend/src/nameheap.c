#ifndef NO_IDENT
static char *Id = "$Id: nameheap.c,v 1.7 1995/06/06 13:02:56 tom Exp $";
#endif

/*
 * Title:	nameheap.c
 * Author:	Thomas E. Dickey
 * Created:	04 Dec 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		06 Jul 1985, added 'nameheap_ref' and 'nameheap_add'.
 *		04 Jul 1985, added 'nameheap_set' and 'nameheap_clr'
 *			     entrypoints to support refs-mask.
 *		15 Jun 1985, typed 'calloc'
 *		14 Dec 1984, allow for 0-length non-null-ended strings
 *		06 Dec 1984
 *
 * Function:	This module maintains a table of character strings.  It is
 *		used by FLIST to maintain the set of filename (path, name or
 *		type) strings).  The strings are stored in an alphabetically-
 *		sorted (no repeats) linked list.
 *
 *		To make symbols identical at all levels of hierarchy, we
 *		allocate space for a given unique string once, and keep it
 *		until it is obsolete.  The '.refs' mask keeps track of which
 *		levels of hierarchy reference a given string.  When a string
 *		has no more references, we may purge it with 'nameheap_clr'.
 *
 * Entry points:
 *	nameheap	Do allocation/lookup of strings in a heap
 *	nameheap_set	Set current reference-mask level
 *	nameheap_ref	Return current reference-mask
 *	nameheap_clr	Deallocate entries in a heap at a given reference level
 *	nameheap_add	Mask additional levels for a given string
 */

#include	<stdlib.h>
#include	<stddef.h>	/* 'offsetof()' */
#include	<stdio.h>
#include	<string.h>

#include	"nameheap.h"
#include	"textlink.h"

#define	CHR(link)	link->text[0]
#define	ADR(link)	&CHR(link)

static	int	last_refs = 0;

/* <nameheap>:
 * Function:	Do lookup/allocation of strings in a heap.
 *
 * Arguments:	s_	- pointer to string to lookup/store
 *		len	- length of string (if -1, assume null-ended).
 *		heap	- address of pointer to list to maintain.
 *
 * Returns:	A pointer to the string component of the entry (the pointer
 *		component will be invisible to the user).
 */
char	*nameheap (char *s_, int len, void **heap)
{
	int	cmp, cnt;
	TEXTLINK *new, *old, *tmp;
	char	*ref_,	*tst_;

	if (len < 0)		len = strlen (s_);
	if (len <= 0)		return ("");

#ifdef	DEBUG
	{
		char	format[20];
		sprintf (format, "'%%.%ds'\n", len);
		trace (format, s_);
	}
#endif

	for (new = *heap, old = 0; new; old = new, new = new->next)
	{
		/*
		 * Do an inline string compare to make this run fast.  Note
		 * that the input string need not be ended with a null.
		 */
		for (tst_ = s_, ref_ = ADR(new), cnt = len;
			*ref_;
				tst_++, ref_++, cnt--)
		{
			if (cmp = ((cnt > 0) ? *tst_ : 0) - *ref_)	break;
		}
		if (!*ref_ && cnt)	cmp = *tst_;
#ifdef	DEBUG
		trace ("\t'%s' => %d (%d)\n", ADR(new), cmp, len-cnt);
#endif
		if (cmp == 0)		goto exit;
		else if (cmp < 0)	break;
	}

	tmp = calloc (1, sizeof(TEXTLINK) + len);/* Allocate pointer + text */

	if (old)	old->next = tmp;
	else		*heap	  = tmp;	/* Entry is first in list */

	tmp->next = new;
	strncpy (ADR(tmp), s_, len);
	tmp->text[len] = '\0';
	new = tmp;

#ifdef	DEBUG
	trace ("\t--> new\n");
#endif

exit:	new->refs |= last_refs;
	return (ADR(new));
}

/* <nameheap_set>:
 * Set the current refs-level.  We expect an integer in the range 1-8, since
 * we shift it as a bit-mask ourselves.  Note that this level-mask is shared
 * by all lists maintained by this module.
 */
void	nameheap_set (int new_refs)
{
	if (new_refs < 1)	new_refs = 1;
	last_refs = 1 << (new_refs-1);
#ifdef	DEBUG1
	trace ("SET: %d => %d\n", new_refs, last_refs);
#endif
}

/* <nameheap_ref>:
 * Return the mask to permit caller to maintain a compatible tagged list.
 */
int	nameheap_ref (void)
{
	return (last_refs);
}

/* <nameheap_clr>:
 * Clear the specified reference-level from all items in the linked-list.
 * If a string has no more references, unlink it and deallocate the entry.
 */
void	nameheap_clr (int old_refs, void **heap)
{
	TEXTLINK *old = 0;
	TEXTLINK *new = *heap;
	TEXTLINK *nxt;

	if (old_refs < 1 || old_refs > 8)	return;
	old_refs = ~(1 << (old_refs-1));

	while (new)
	{
		nxt = new->next;
		if (new->refs &= old_refs)
			old = new;
		else
		{
			if (old)	old->next = nxt;
			else		*heap	  = nxt;
			cfree (new);
		}
		new = nxt;
	}
}

/* <nameheap_add>:
 * Given a string and a reference mask, OR the mask against the string's
 * heap-descriptor.  This is used, for example, when renaming an object to
 * ensure that higher levels will refer to all parts of the object.
 */
void	nameheap_add (int refs, char *text)
{
	TEXTLINK *P = (TEXTLINK *) 0;
	void	*C = text - offsetof(TEXTLINK, text);
	P = (TEXTLINK *) C;
	P->refs |= refs;
}

#ifdef	DEBUG1
void	nameheap_dump (char *tag, void **heap)
{
	TEXTLINK *new = *heap;
	register int	j;

	trace("Names: %s %08X\n", tag, new);
	while (new)
	{
		for (j = 1; j < 256; j <<= 1)
			trace ("%c", new->refs & j ? '*' : '-');
		trace (" '%s'\n", new->text);
		new = new->next;
	}
	trace ("----\n");
}
#endif	/* DEBUG1 */
