/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: canopen.c,v 1.6 1995/10/21 18:25:53 tom Exp $";
#endif

/*
 * Title:	canopen.c
 * Author:	Thomas E. Dickey
 * Created:	18 Sep 1984
 * Last update:	18 Sep 1984
 *
 * Function:	Verify that a given file can be opened (for input).  This is
 *		a more stringent test-for-existence, which is used in FLIST to
 *		verify that a command-file is present and usable.
 *
 * Parameters:	name_	=> name-string to use
 *
 * Returns:	Zero iff we can open file.
 *
 * Patch:	Should consider extending this function to testing other types
 *		of file-open.
 */

#include	<starlet.h>
#include	<rms.h>
#include	<stsdef.h>

#include	"rmsinit.h"
#include	"canopen.h"

#define	check(f)	status = (f);\
			if (!$VMS_STATUS_SUCCESS(status)) goto failed;

int	canopen (char *name_)
{
	struct	FAB	fab;
	struct	NAM	nam;
	char	esa[NAM$C_MAXRSS],	/* expanded by SYS$PARSE	*/
		rsa[NAM$C_MAXRSS];	/* result from SYS$SEARCH	*/
	unsigned status;

	rmsinit_fab (&fab, &nam, 0, name_);
	rmsinit_nam (&nam, rsa, esa);

	fab.fab$b_fac |= FAB$M_GET;

	check(sys$parse(&fab));
	check(sys$search(&fab));
	check(sys$open(&fab));
	check(sys$close(&fab));
	status = 0;

failed:
	return (status);
}
