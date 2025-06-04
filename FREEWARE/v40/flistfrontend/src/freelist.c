#ifndef NO_IDENT
static char *Id = "$Id: freelist.c,v 1.4 1995/06/06 10:45:56 tom Exp $";
#endif

/*
 * Title:	freelist.c
 * Author:	Thomas E. Dickey
 * Created:	28 May 1984
 * Last update:	28 May 1984
 *
 * Function:	This procedure releases all elements in a linked list which
 *		were previously allocated using malloc, or calloc.  The
 *		list is linked with the next-pointer in the first location
 *		of each entry.  (The storage allocator knows how long the
 *		entries actually were.)
 */

#include <stdlib.h>

#include "freelist.h"

#define	ENTRY	struct _freelist
ENTRY {
	ENTRY	*next;
	};

void	freelist (void *list)
{
	ENTRY	*first_ = (ENTRY *)list;
	ENTRY	*next_;

	while (first_)
	{
		next_ = first_->next;
		free (first_);
		first_ = next_;
	}
}
