/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: setctrl.c,v 1.4 1995/06/04 01:19:32 tom Exp $";
#endif

/*
 * Title:	setctrl.c
 * Author:	Thomas E. Dickey
 * Created:	18 Dec 1984
 * Last update:	18 Dec 1984
 *
 * Function:	Set/clear the VMS control functions for ^T, ^Y.  This is used
 *		by FLIST.
 */

#include	<lib$routines.h>

#define	LIB$M_CLI_CTRLT	0x00100000
#define	LIB$M_CLI_CTRLY	0x02000000

void
setctrl (int turnon)
{
static
int	newmask	= (LIB$M_CLI_CTRLT | LIB$M_CLI_CTRLY),
	oldmask;

	oldmask |= LIB$M_CLI_CTRLY;	/* Should be set always	*/
	if (turnon)
		lib$enable_ctrl (&oldmask);
	else
		lib$disable_ctrl (&newmask, &oldmask);
}
