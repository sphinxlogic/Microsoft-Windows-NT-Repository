#ifndef NO_IDENT
static char *Id = "$Id: dclarea.c,v 1.4 1995/06/04 01:47:57 tom Exp $";
#endif

/*
 * Title:	dclarea.c
 * Author:	Thomas E. Dickey
 * Created:	13 Apr 1985
 * Last update:
 *		19 Feb 1995, prototyped
 *		15 Jun 1985, typed 'calloc'
 *		13 Apr 1985
 *
 * Function:	Allocate a string-area for a DCLOPT-table entry so we needn't
 *		have a lot of large static buffers lying around.
 *
 * Arguments:	name	- name of entry to find in table 'opt[]'.
 *		size	- number of bytes to allocate
 *		opt[]	- DCLOPT array of permissible options
 *		size_opt - sizeof(opt[]).
 *
 * Returns:	Pointer to the allocated area.  The DCLOPT entry also points
 *		to this area.
 */

#include	<stdlib.h>
#include	<string.h>

#include	"dclopt.h"

char	*dclarea (char *name, int size, DCLOPT *opt, int size_opt)
{
	register int	j;
	register int	maxopt	= size_opt / sizeof(DCLOPT);
	register char	*area;

	area = calloc(1,size);
	for (j = 0; j < maxopt; j++)
	{
		if (strcmp (opt[j].opt_name, name) == 0)
		{
			opt[j].opt_area = area;
			opt[j].opt_size = size;
			break;
		}
	}
	return (area);
}
