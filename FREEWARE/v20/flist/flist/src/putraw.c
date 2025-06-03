/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: putraw.c,v 1.4 1995/06/04 17:08:54 tom Exp $";
#endif

/*
 * Title:	putraw.c
 * Author:	Thomas E. Dickey
 * Created:	12 Nov 1984
 * Last update:
 *		03 Jun 1995, prototyped
 *
 * Function:	Print a null-ended string on the terminal.  Unlike 'printf',
 *		this VMS function will not be confused by backspaces.  (The
 *		problem with 'printf' is that it does not track backspaces
 *		when determining the wraparound column.)
 */

#include	<descrip.h>
#include	<string.h>

#include	"crt.h"

extern	void	lib$put_screen(struct dsc$descriptor_s *p);

void
putraw (char *s_)
{
	static $DESCRIPTOR(DSC_bell," ");

	DSC_bell.dsc$a_pointer = s_;
	DSC_bell.dsc$w_length = strlen(s_);
	lib$put_screen (&DSC_bell);
}
