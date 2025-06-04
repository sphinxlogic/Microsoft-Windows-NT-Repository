/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: chrcmd.c,v 1.6 1995/06/04 18:49:02 tom Exp $";
#endif

/*
 * Title:	chrcmd.c
 * Author:	Thomas E. Dickey
 * Created:	14 Apr 1985
 * Last update:
 *		19 Feb 1995, prototyped
 *
 * Function:	Return a pointer to a string which interprets a code which
 *		may be one of:
 *
 *		a) ASCII character (including a control)
 *		b) A GETPAD-code
 *		c) An FLIST sort-code
 */

#include	<rms.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"flist.h"
#include	"getpad.h"

#include	"dircmd.h"

char	*chrcmd (int command)
{
int	j;
static	char	bfr[30];

static	struct	{
	int	code;
	char	*show;
	}	table[]	= {
		'\r',		"<CR>",
		'\b',		"<BS>",
		'\n',		"<LF>",
		'\177',		"<DEL>",
		padUP,		"<up-arrow>",
		padDOWN,	"<down-arrow>",
		padLEFT,	"<left-arrow>",
		padRIGHT,	"<right-arrow>",
		padPF1,		"<PF1>",
		padPF2,		"<PF2>",
		padPF3,		"<PF3>",
		padPF4,		"<PF4>",
		padENTER,	"<ENTER>"
	};

	for (j = 0; j < SIZEOF(table); j++)
		if (table[j].code == command)	return (table[j].show);

	if (isascii (command))
	{
		if (iscntrl(command))
			sprintf (bfr, "^%c", command | 0100);
		else
			sprintf (bfr, "%c", command);
	}
	else if (is_PAD(command))
		sprintf (bfr, "<Keypad %c>", toascii(command));
#if UNUSED
	else if (is_sCMD(command))
	{
		command = toascii(command);
		sprintf (bfr, "/%c%c", islower(command) ? 's' : 'r', command);
	}
#endif
	else
		strcpy (bfr, "<??>");
	return (bfr);
}
