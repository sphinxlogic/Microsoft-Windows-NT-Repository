#ifndef NO_IDENT
static char *Id = "$Id: dspc.c,v 1.7 1995/06/06 00:35:32 tom Exp $";
#endif

/*
 * Title:	dspc.c
 * Author:	Thomas E. Dickey
 * Created:	07 Jun 1984
 * Last update:
 *		19 Feb 1995, prototyped
 *		17 Jan 1985, recode so we only do init/move functions here,
 *			     under control of caller.
 *		25 Aug 1984, cleanup buffer sizes
 *		18 Jun 1984
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"dspc.h"
#include	"crt.h"
#include	"getpad.h"
#include	"getraw.h"

#include	"strutils.h"

/*
 * Title:	dspc_init
 *
 * Function:	This procedure displays the current cursor location in-screen
 *		for the BROWSE program.
 *
 * Parameters:	scale	text buffer to load with basic scale.
 *		coladj	amount by which to right-shift the display to account
 *			for prefix-text.
 *		col	column number of home-position on screen.
 */

void
dspc_init (char *scale, int coladj, int col)
{
	int	width	= crt_width() - coladj,
		width2	= width - 6;
	char	sline	[CRT_COLS];

	if (width2 < 0)
	{
		scale[0] = '\0';
		return;
	}

	strscale (scale, col, width2);
	if (coladj)	/* If "home" is not true origin, adjust scale */
	{
		char	format[20];
		strcpy (sline, scale);
		sprintf (format, "%%%ds%%s", coladj);
		sprintf (scale, format, " ", sline);
	}
	crt_high (&scale[coladj], strlen(&scale[coladj]));
}

/*
 * Function:	Move the cursor to the specified column, updating the highlight
 *		in the scale.
 *
 * Arguments:	scale	=> text of scale, from init-call
 *		col	= column number (adjusting for home-position) of
 *			  cursor (0..width-1).
 *		line	= line number on which cursor resides (0..filesize-1)
 */
void
dspc_move (char *scale, int col, int line)
{
	char	sline[CRT_COLS];

	line++;
	if (*scale)
	{
		sprintf (sline, "%s%5d", scale, line);
		if (crt_vt52())
			sline[col] = '*';
		else
			sline[col] = toascii (sline[col]);
			/* de-highlight cursor */
		crt_text (sline, crt_lpp()-1, 2);
	}
	crt_move (line - crt_top(), ++col);
}
