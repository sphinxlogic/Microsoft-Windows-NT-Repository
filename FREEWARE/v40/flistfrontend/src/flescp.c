#ifndef NO_IDENT
static char *Id = "$Id: flescp.c,v 1.4 1995/03/18 23:50:22 tom Exp $";
#endif

/*
 * Title:	flescp.c
 * Author:	Thomas E. Dickey
 * Created:	11 Jul 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		11 Jun 1985, added cli-argument to dds_spawn
 *		11 Jul 1984
 *
 * Function:	Spawn an external process to permit command entry outside
 *		FLIST.
 */

#include	<ctype.h>

#include	"flist.h"
#include	"dircmd.h"
#include	"dds.h"

tDIRCMD(flescp)
{
	dds_spawn ((_toupper(xcmd_[1]) == 'H') ? "shell" : nullC,
		*curfile_, 0, 0, FALSE, 2);
}
