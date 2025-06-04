#ifndef NO_IDENT
static char *Id = "$Id: edtcmd.c,v 1.12 1995/06/06 09:44:14 tom Exp $";
#endif

/*
 * Title:	edtcmd.c
 * Author:	Thomas E. Dickey
 * Created:	16 Apr 1985 (from DIRCMD, 10 May 1984)
 * Last update:
 *		19 Feb 1995, prototypes
 *		18 Feb 1995, port to AXP (renamed 'alarm').
 *		27 Apr 1985
 *
 * Entry:	edtcmd:		Accept & edit a command
 *		edtcmd_init:	Initialize this module
 *		edtcmd_get:	Get a single character
 *		edtcmd_last:	Compare argument with last from 'edtcmd_get'.
 *		edtcmd_crt:	Merge input+reference buffers to display.
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"bool.h"
#include	"cmdstk.h"
#include	"getpad.h"
#include	"getraw.h"
#include	"edtcmd.h"

#include	"strutils.h"

#define	RUBOUT	'\177'
#define	CTL(c)	(037 & (c))

/*
 * 'lastcommand' is used as a 1-level stack to suppress repeated error messages
 *	due to repeated (or stuck) key.  The logic is tested in the main program
 *	of 'FLIST', where all commands are reduced to a single code.
 */
static	int	lastcommand;
static	int	thiscommand;	/* 1-level stack for repeat test */
static	char	*cmdbfr	= nullC;

/* <edtcmd_get>:
 * Read a character, saving the previous one, for repeated-key testing.
 * We do screen-dump and refresh here as well:
 */
int
edtcmd_get (void)
{
	lastcommand = thiscommand;
	for (;;)
	{
		switch (thiscommand = getpad())
		{
		case CTL('W'):	crt_refresh ();		break;
		case CTL('K'):	snapshot ();		break;
		default:	return (thiscommand);
		}
	}
}

/* <edtcmd_last>:
 * Test if the argument is the same as the previous character.  This is used
 * to latch multiple-keystroke errors (such as from a stuck key).
 */
int
edtcmd_last (int c)
{
	return ((c > 0) && (c == lastcommand));
}

/* <edtcmd>:
 * Read/edit a "visible" command
 */

#define	doARROW	(flags & 8)	/* Return arrow keys in 128-255 code	*/
#define	doHIGH	(flags & 4)	/* Highlight command as it is entered	*/
#define	doLOWER	(flags & 2)	/* Lowercase command as it is entered	*/
#define	doTRIM	(flags & 1)	/* Trim leading/trailing blanks		*/

#define	THIS	cmdbfr[col]
#define	PREV	cmdbfr[col-1]
#define	DELETE	{strcpy (&PREV, &THIS); col--; }
#define	SHOWIT	edtcmd_crt (cmdbfr, refbfr, doHIGH, do_col, do_line, col)
#define	COMMAND	(doLOWER ? _tolower(command) : _toupper(command))

char *
edtcmd (
	int	command,	/* Initial and scratch value		*/
	char	*delim,		/* Delimiters (if used, for repeat-count) */
	int	flags,		/* Bits: highlight, lowercase, trim	*/
	int	do_line,	/* Line on which to enter command 	*/
	int	do_col,		/* Column at which to begin command	*/
	char	*do_hlp,	/* Help-interface string		*/
	char	*do_1st,	/* Starting contents of command-result	*/
	char	*do_ref)	/* Reference copy of command-line	*/
{
	int	len,			/* Current command-length	*/
		col,			/* Edit-index into 'cmdbfr[]'	*/
		maxcols	= crt_width()-do_col-2,	/* Max length of cmd-string */
		complete= FALSE,
		j,
		get_dir	= 1;		/* Normal sense of RETRIEVE	*/
	char	refbfr[CRT_COLS],
		tmp[CRT_COLS];


	cmdstk_tos ();
	strcpy (cmdbfr, do_1st);
	strcpy (refbfr, do_ref);

retrieve:
	if (command == RETRIEVE)
	{
		if (cmdstk_get (cmdbfr, get_dir))
		{
			if (! doLOWER)	strucpy (cmdbfr, nullC);
		}
		else
		{
			sound_alarm ();
			goto did_abort;
		}
	}
	else if (isprint(command) && isascii(command))
		sprintf (cmdbfr, "%c", COMMAND);

	col	= strlen (cmdbfr);
	get_dir	= 1;			/* Normal sense of RETRIEVE	*/

	SHOWIT;				/* show the new command	*/
	/*
	 * Perform restricted command editing a la EDT, by recognizing
	 * backspace, linefeed, delete, left/right arrows for editing.
	 * Normal printing characters (including space) are inserted at
	 * cursor position.
	 *
	 * If 'doARROW' is set, we do not use arrow-keys for editing/retrieval,
	 * but instead pass them back mapped into the range 128-255 on the end
	 * of the input string.
	 */
	while (!complete)
	{
		ctlx_clr();		/* Reset CTRL/X flag	*/
		switch (command = edtcmd_get())
		{
		case padUP:
			if (doARROW)	goto do_arrow;
		case RETRIEVE:
			command	= RETRIEVE;
			get_dir	= 1;
			goto retrieve;
		case padDOWN:
			if (doARROW)	goto do_arrow;
			command	= RETRIEVE;
			get_dir = -1;
			goto retrieve;
		case RUBOUT:
			if (col)	DELETE
			else		sound_alarm ();
			break;
		case CTL('X'):
		case CTL('U'):
		case CTL('C'):
		case CTL('Y'):
			cmdbfr[0] = cmdbfr[1] = col = 0;
			break;
		case padENTER:
		case '\r':
			if (delim)
			{
				if (strchr(delim, command))
					complete = TRUE;
				else
					sound_alarm();
			}
			else
				complete = TRUE;
			break;
		case '\n':
			if (col)
			{
				if (isspace(PREV))
				{
					while (col && isspace(PREV))
						DELETE
				}
				while (col && !isspace(PREV))
					DELETE
			}
			else		sound_alarm();
			break;
		case padLEFT:		/* left-arrow	*/
			if (doARROW)	goto do_arrow;
		case CTL('D'):		/* (cf: VMS 4.0)*/
			if (col > 0)	col--;
			else		sound_alarm();
			break;

		case padRIGHT:		/* right-arrow	*/
			if (doARROW)	goto do_arrow;
		case CTL('F'):		/* (cf: VMS 4.0)*/
			if (col < maxcols-1)
			{
				if (++col >= strlen(cmdbfr))
					strcat (cmdbfr, " ");
			}
			else
				sound_alarm();
			break;		

		case '\b':
		case pad0:
			col = 0;
			break;
		case pad2:
		case CTL('E'):		/* (a la VMS 4.0)	*/
			col = strlen(cmdbfr);
			break;
		case HELPKEY:
			crt_help (nullC, do_hlp);
			break;
		case ' ':
			if (doTRIM && col == 0)
			{
				sound_alarm ();
				break;
			}
		default:
			if (isascii(command)
			&&  isprint(command)
			&&  strlen(cmdbfr) < maxcols)
			{
			register int use_it = TRUE;
				/*
				 * If we have a delimiter, it will be returned
				 * as the last character on the command-string
				 * (unless it happens to be '\n').
				 */
				command = COMMAND;
				if (delim)
				{
					if (strchr(delim, command))
						complete = TRUE;
					else if (!isdigit(command))
						use_it = FALSE;	
				}
				if (use_it)
				{
					strcpy (tmp, &THIS);
					strtrim (tmp);
					sprintf (&THIS, "%c%s", command, tmp);
					col++;
				}
				else
					sound_alarm ();
			}
			else
				sound_alarm ();
		}	/* end:switch(command) */
		/*
		 * Cursor movement may temporarily lengthen the buffer.  Trim
		 * trailing blanks:
		 */
		len	= doTRIM ? strtrim(cmdbfr) : strlen(cmdbfr);
		j	= max(len, col) - len;
		while (j-- > 0)
			strcat  (cmdbfr, " ");

		if (!gotraw() || complete) SHOWIT;
		if (strlen(cmdbfr) <= 0)
		{
			cmdbfr[0] = EOS;
			goto did_abort;
		}

		/*
		 * Enter this block to return an arrow-key (or other keypad
		 * control, as required), mapped into the range 128-255.
		 */
		command	= EOS;
do_arrow:	if (command > 255)	/* 'pad' codes are 256-511	*/
		{
			SHOWIT;
			sprintf (tmp, "%c", command | 128);
			strcat (cmdbfr, tmp);
			complete = TRUE;
		}

	}	/* end:while (! complete) */
did_abort:
	lastcommand = EOS;
	ctlx_clr ();		/* Reset CTRL/X flag	*/
	return (cmdbfr);
}

/* <edtcmd_init>:
 * Initialize this module.  We need a buffer in which to build commands,
 * 'cmdbfr', as well as a buffer in which to store the prior-state of a
 * line which we are receiving upon.
 */
void
edtcmd_init (void)
{
	cmdbfr = calloc(1, CRT_COLS+1);
	lastcommand = EOS;
}

/* <edtcmd_crt>:
 * Display the text for the current "visible-command".  The command-text is
 * displayed *highlighted*, with a trailing space (so that if we are overstriking
 * a display-line, there is a gap).  The cursor is positioned to the current-
 * column.
 *
 * This procedure is self-contained (does not directly reference any static
 * data).
 */
void
edtcmd_crt (
	char	*cmd_,	/* Text to overlay 'ref_[]'		(asciz)	*/
	char	*ref_,	/* Reference, to restore/update			*/
	int	highlight, /* Do highlighting of new text if true	*/
	int	rcol,	/* Column at which 'cmd_[]' is put	 1..n	*/
	int	rline,	/* Reference: line in screen		 1..n	*/
	int	ccol)	/* Reference: cursor-column in 'cmd_[]', 0..n	*/
{
	int	j	= strlen(ref_),
		len	= strlen(cmd_);
	char	*c_,
		tocmp	[CRT_COLS],
		toshow	[CRT_COLS];

	for (c_ = toshow; *cmd_; c_++, cmd_++)
	{
		if (highlight)
			*c_ = tohigh(*cmd_);
		else
			*c_ = *cmd_;
	}

	strcpy (tocmp, ref_);
	while (j < rcol)	strcpy (&tocmp[j++], " ");
	strncpy (&tocmp[rcol-1], toshow, len);
	j	 = rcol + len - 1;
	tocmp[j] = ' ';			/* add a space on the end */
	if (strlen(ref_) <= j)
		tocmp[j+1] = '\0';	/* ...but keep trailing null */

	crt_text (tocmp, rline, 0);
	crt_move (rline+1, rcol+ccol);
}
