/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: alarm.c,v 1.6 1995/06/04 17:08:44 tom Exp $";
#endif

/*
 * Title:	alarm.c
 * Author:	Thomas E. Dickey
 * Created:	10 Sep 1984
 * Last update:	10 Sep 1984
 *
 * Function:	Use VAX/VMS run-time routine to make an audible tone (BELL)
 *		on the user's terminal.  This is used rather than 'putchar'
 *		because we wish to avoid using the VMS-C run-time support.
 */

#include	<descrip.h>

#include	"crt.h"

extern	void	lib$put_screen(struct dsc$descriptor_s *p);

void
sound_alarm (void)
{
	static $DESCRIPTOR(DSC_bell,"\007");

	lib$put_screen (&DSC_bell);
}
