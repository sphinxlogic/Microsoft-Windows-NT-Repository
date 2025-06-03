/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: rmsinit.c,v 1.4 1995/06/06 13:08:10 tom Exp $";
#endif

/*
 * Title:	rmsinit.c
 * Author:	Thomas E. Dickey
 * Created:	05 Sep 1984 (from 'mv', 17 May 1984)
 * Last update:
 *		03 Jun 1994, prototyped
 *
 * Function:	These procedures do the most common type of RMS initialization,
 *		that of FAB- and NAM-block initialization for filename parsing
 *		and directory lookup.
 */

#include	<string.h>

#include	"rmsinit.h"

/*
 * Initialize a FAB block:
 *
 *	fab_	=> FAB block to initialize
 *	nam_	=> NAM block to use in parse
 *	dna_	=> string to use (if nonnull) in parse-default
 *	fna_	=> string to use (if nonnull) for input filename
 */
void
rmsinit_fab(struct FAB *fab_, struct NAM *nam_, char *dna_, char *fna_)
{
	*fab_ = cc$rms_fab;

	fab_->fab$l_fop = FAB$M_NAM;
	fab_->fab$l_nam = nam_;			/* FAB => NAM block	*/
	if (dna_)
	{
		fab_->fab$l_dna = dna_;		/* Default-selection	*/
		fab_->fab$b_dns = strlen(dna_);
	}
	if (fna_)
	{
		fab_->fab$l_fna = fna_;
		fab_->fab$b_fns = strlen(fna_);
	}
}

/*
 * Initialize a NAM-block:
 *
 *	nam_	=> NAM-block to initialize
 *	rsa_	=> "resultant string" address (loaded by SYS$SEARCH)
 *	esa_	=> "expanded string" address (loaded by SYS$PARSE)
 */
void
rmsinit_nam (struct NAM *nam_, char *rsa_, char *esa_)
{
	*nam_ = cc$rms_nam;

	if (esa_)
	{
		nam_->nam$b_ess = NAM$C_MAXRSS;
		nam_->nam$l_esa = esa_;
	}
	if (rsa_)
	{
		nam_->nam$b_rss = NAM$C_MAXRSS;
		nam_->nam$l_rsa = rsa_;
	}
}
