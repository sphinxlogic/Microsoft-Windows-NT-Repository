#ifndef NO_IDENT
static char *Id = "$Id: highver.c,v 1.4 1995/10/21 18:53:22 tom Exp $";
#endif

/*
 * Title:	highver.c
 * Author:	Thomas E. Dickey
 * Created:	08 Sep 1984 (from 'mv$current', 17-May-1984)
 * Last update:
 *		03 Jun 1995, prototyped
 *		23 Jun 1985, use 'scanver' to decode version number
 *		08 Sep 1984
 *
 * Function:	Given a filename (no wildcards are assumed), return the
 *		highest version number which it has in the directory.
 *
 * Parameters:	name_	=> Filename string.  This is returned without a version
 *			   on the end.
 *
 * Returns:	the actual version, if the file is found, else 0.
 */

#include	<starlet.h>
#include	<rms.h>
#include	<stsdef.h>
#include	<string.h>

#include	"rmsinit.h"
#include	"scanver.h"

int
highver (char *name_)
{
	struct	FAB	tmpFAB;
	struct	NAM	tmpNAM;
	unsigned status;
	int	len;			/* length to ending ";" */
	char	tmpRSA	[NAM$C_MAXRSS];
	char	tmpESA	[NAM$C_MAXRSS];

#define	ok(x)	status = x; if (!$VMS_STATUS_SUCCESS(status))	return (0)

	rmsinit_fab (&tmpFAB, &tmpNAM, 0, name_);
	rmsinit_nam (&tmpNAM, tmpRSA, tmpESA);

	ok(sys$parse(&tmpFAB));		/* parse the name to find version */

	strncpy (name_, tmpESA, len = (tmpNAM.nam$l_ver - tmpNAM.nam$l_node));
	name_[len] = '\0';		/* strip off version code	*/

	rmsinit_fab (&tmpFAB, &tmpNAM, 0, name_);
	rmsinit_nam (&tmpNAM, tmpRSA, tmpESA);

	ok(sys$parse(&tmpFAB));		/* parse the name	*/
	ok(sys$search(&tmpFAB));	/* ...find the file	*/

	return (scanver (tmpNAM.nam$l_ver, tmpNAM.nam$b_ver));
}
