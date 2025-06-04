/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: flnoid.c,v 1.5 1995/06/06 10:10:06 tom Exp $";
#endif

/*
 * Title:	flnoid.c
 * Author:	Thomas E. Dickey
 * Created:	22 May 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		11 Jun 1985, added cli-argument to dds_spawn
 *		26 Jul 1984, modified 'dirseek()'
 *		16 Jul 1984
 *		10 Jul 1984, re-coded to DCLARG-list.
 *		03 Jul 1984, changed name 'faccess' to 'dirent_acc'
 *		22 May 1984
 *
 * Function:	Spawn an external process (PRINT or SUBMIT).  To avoid messing
 *		up the screen, append the NOIDENTIFY option.  These use the
 *		NOWAIT version of spawning, since their intent is only to
 *		queue a request.
 *
 *		The arguments need not be members of 'filelist[]', since they
 *		are not modified.
 *
 * Patch:	I would rather capture and display the enqueued-message.
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"flist.h"
#include	"dirent.h"
#include	"dircmd.h"
#include	"dirseek.h"
#include	"dds.h"

static	char	format[] = "%s/noidentify";

tDIRCMD(flnoid)
{
	int	pr_cmd	= (strcmp ("PRINT", xdcl_->dcl_text) == 0);
	char	*full_;

	if (! dirseek (xcmd_, xdcl_, 1, TRUE))	return;

	full_ = calloc (1, strlen(xcmd_)+sizeof(format));
	sprintf (full_, format, xcmd_);

	dds_spawn (0, *curfile_, full_, (pr_cmd ? "Printing..." : 0), TRUE, 1);
	cfree (full_);
}
