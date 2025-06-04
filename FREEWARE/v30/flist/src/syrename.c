/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: syrename.c,v 1.6 1995/10/21 18:40:13 tom Exp $";
#endif

/*
 * Title:	sysrename.c
 * Author:	Thomas E. Dickey
 * Created:	17 May 1984
 * Last update:
 *		19 Oct 1995, DEC C clean-compile
 *		19 Feb 1995, sys utils prototypes
 *		06 Sep 1984, permit wildcards in output-filename
 *		05 Sep 1984, fixed fatal error
 *
 * Function:	Rename a file, given its file-specification, and the (maybe
 *		defaultable) new file-specification.
 *
 * Parameters:	newspec	= new file-specification string.  This is replaced by
 *			  the resulting filename after the rename operation.
 *		oldspec	= old file-specification string
 *
 * Returns:	nonzero VMS status value iff an error is detected.
 *
 * Patch:	Some things in VMS are unkosher, but we must live with them:
 *
 *			RENAME X X;-1   yields ok status, with the next version
 *			RENAME X X;+1	yields an error
 */

#include	<starlet.h>
#include	<string.h>
#include	<rms.h>
#include	<stsdef.h>

#include	"rmsinit.h"
#include	"sysutils.h"

#define	ok(x) status = x; if (!$VMS_STATUS_SUCCESS(status)) return(status)

unsigned sysrename (char *newspec, char *oldspec)
{
	struct	FAB	oldFAB, newFAB,	tmpFAB;
	struct	NAM	oldNAM, newNAM,	tmpNAM;
	unsigned status;
	int	len;
	char	oldRSA	[NAM$C_MAXRSS],	oldESA	[NAM$C_MAXRSS],
		tmpESA	[NAM$C_MAXRSS],	 /* buffer to get rid of wildcard */
		newRSA	[NAM$C_MAXRSS],	newESA	[NAM$C_MAXRSS];

	/*
	 * Check for the old-name:
	 */
	rmsinit_fab (&oldFAB, &oldNAM, 0, oldspec);
	rmsinit_nam (&oldNAM, oldRSA, oldESA);

	ok(sys$parse(&oldFAB));
	ok(sys$search(&oldFAB));

	/*
	 * Do an intermediate (related-file) parse to get rid of wildcards
	 * in the new-name.  By using an "output-file-parse", these wildcards
	 * will be replaced by the corresponding tokens from the old-spec.
	 */
	rmsinit_fab (&tmpFAB, &tmpNAM, 0, newspec);
	rmsinit_nam (&tmpNAM, 0, tmpESA);
	tmpNAM.nam$l_rlf = &oldNAM;

	tmpFAB.fab$l_fop |= FAB$M_OFP;	/* output-file parse	*/
	ok(sys$parse(&tmpFAB));

	/*
	 * Finally, do the actual rename-operation:
	 */
	tmpESA[tmpNAM.nam$b_esl] = '\0';
	rmsinit_fab (&newFAB, &newNAM, 0, tmpESA);
	rmsinit_nam (&newNAM, newRSA, newESA);

	ok((sys$rename (&oldFAB, 0,0, &newFAB)));

	/*
	 * If the operation is completed normally, put the (parsed and fully
	 * resolved) name into the output argument:
	 */
	strncpy (newspec, newRSA, len = newNAM.nam$b_rsl)[len] = '\0';

	return (0);
}
