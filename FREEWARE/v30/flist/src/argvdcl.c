/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: argvdcl.c,v 1.5 1995/05/29 00:26:38 tom Exp $";
#endif

/*
 * Title:	argvdcl.c
 * Author:	Thomas E. Dickey
 * Created:	28 May 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		15 Dec 1984, added 'cpy_dft' argument to DCLARG.  Corrected
 *			     use of argc-count.
 *		24 Jul 1984, added 'cmd_arg' parameter to DCLARG.
 *		26 Jun 1984
 *
 * Function:	This procedure translates C-type argc,argv module-arguments
 *		into a character string, calls 'dclarg' to process the
 *		resulting list, and returns the DCLARG-pointer to the caller.
 *
 * Parameters:	argc, argv - Passed from module via VMS call.
 *		dft_	=> string specifying defaults for file specifications.
 *		cmd_arg	=  TRUE if command-token appears (first) on the line.
 *
 * Note:	This should not be used as a general replacement for the
 *		VAX/VMS DCL interface.
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"bool.h"
#include	"dclarg.h"

#include	"strutils.h"

DCLARG * argvdcl (int argc, char *argv[], char *dft_, int cmd_arg)
{
	DCLARG	*arg_	= 0;
	int	len	= 0;
	int	j;
	char	*c_;

	if (argc > 1)
	{
		for (j = 1; j < argc; j++)
			len += 3 + strlen(argv[j]);
		c_ = calloc(1, len);

		for (j = 1; j < argc; j++)
			sprintf (strnull(c_), " %s", argv[j]);
		arg_ = dclarg(c_, dft_, cmd_arg, FALSE);
		cfree (c_);
	}
	return (arg_);
}
