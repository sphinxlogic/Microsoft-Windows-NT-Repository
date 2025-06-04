#ifndef NO_IDENT
static char *Id = "$Id: cmdstk.c,v 1.8 1995/10/21 17:49:10 tom Exp $";
#endif

/*
 * Title:	cmdstk.c
 * Author:	Thomas E. Dickey
 * Created:	17 Oct 1984 (broke out of 'dircmd')
 * Last update:
 *		27 May 1995, split-out cmdstk.h
 *		18 Feb 1995, port to AXP (renamed 'alarm')
 *		14 May 1985, was testing wrong variable in entry to '_put'.
 *		30 Apr 1985, always store commands in lower-case.
 *		27 Apr 1985, don't concat ' ' on end of stored-string!!
 *		22 Dec 1984, permit bidirectional movement in 'cmdstk_get'.
 *		18 Dec 1984, restrict stack to a page of memory.  Re-coded
 *			     from a linked-list to a ring buffer.
 *
 * Function:	This module maintains a buffer of the most-recently-used
 *		command strings of FLIST, permitting the user to retrieve
 *		commands for re-use or alteration.
 */

#include	<stdlib.h>
#include	<string.h>

#include	"bool.h"
#include	"crt.h"
#include	"cmdstk.h"
#include	"strutils.h"

/*
 * Local (static) data:
 *
 * Save FLIST "visible commands" in a stack-list in memory, to provide
 * last-command retrieval.
 */

static	CMDSTK	*cmdstk_ = 0;

#define	HEAD	cmdstk_->head
#define	STORED	cmdstk_->stored
#define	DEPTH	cmdstk_->depth
#define	TEXT(j)	cmdstk_->text[j]

#define	INDEX(j) cmdstk_index(HEAD-(j))

static	int	cmdstk_index (int n);

/*
 * Allocate a page-buffer for the command-stack.  We provide a hierarchy
 * for FLIST by copying the stack from a higher level to this level.  Thus,
 * there is no loss of continuity on entering a new level; but on exit we
 * resume with the set of stacked commands which were present on nesting.
 */
CMDSTK *
cmdstk_init (void)
{
	CMDSTK	*new_ = calloc (1, PAGESIZE),
		*old_ = cmdstk_;

	if (cmdstk_)	*new_ = *cmdstk_;
	cmdstk_ = new_;
	return (old_);
}

void
cmdstk_free (CMDSTK *old_)
{
	if (cmdstk_)		cfree (cmdstk_);
	cmdstk_ = old_;
}

void
cmdstk_tos (void)
{
	DEPTH = -1;		/* Provide index to most-recent	*/
}

/*
 * Retrieve the (next) most recent item on the command stack.  If we are at
 * the end, sound an alarm.  Return TRUE iff there is text to process.
 * Note that if no commands have been entered, we do not alter the output
 * string from whatever the caller set it to.
 */
int
cmdstk_get (char *string, int dir)
{
	int	deep	= DEPTH + dir;

	if ((deep < STORED) && (deep >= 0))
	{
		strcpy (string, TEXT(INDEX(deep)));
		DEPTH = deep;
		return (TRUE);
	}
	sound_alarm ();
	return (*string);
}

/*
 * If the current command is the same as one of the last 'RING' commands,
 * do not save it.  This permits the user to toggle between a couple
 * of commands in the retrieval state.
 *
 * A command which duplicates one in the last 'RING' levels causes this
 * item to be "popped" to the top-level of the ring.  The entire stack
 * is pushed only when no match is found in the ring.
 */
void
cmdstk_put (char *s_)
{
	register
	int	j, k;
	char	string[CRT_COLS+1];
#define	RING	3

	if (*s_)		/* Save all "visible" commands	*/
	{
		strlcpy (string, s_);
		if (STORED)
		{
			for (j = 0; j < RING; j++)
			{
				if (!strcmp (string, TEXT(INDEX(j))))
				{
					for (k = j; k > 0; k--)
						strcpy (TEXT(INDEX(k)),
							TEXT(INDEX(k-1)));
					strcpy (TEXT(HEAD), string);
					return;
				}
			}
		}
		HEAD = INDEX(-1);
		if (STORED < MAXSTK)	STORED += 1;
		strcpy (TEXT(HEAD), string);
	}
}

/*
 * This procedure is called by a "visible" command to alter to extend its
 * saved-text, for command retrieval.  In particular, it is called by the
 * protection-editor.
 */
void
cmdstk_chg (char *string)
{
	if (STORED > 0)		/* Only "change" if there is something */
	{
		HEAD  = INDEX(1);		/* Redo this entry	*/
		cmdstk_tos ();
		STORED--;
		cmdstk_put (string);
	}
	else
		sound_alarm ();
}

/*
 * Compute an index in the queue, accounting for wraparound.
 */
static int
cmdstk_index (int n)
{
	if (n < 0)		n = MAXSTK - n;
	else if (n >= MAXSTK)	n = n - MAXSTK;
	return (n);
}
