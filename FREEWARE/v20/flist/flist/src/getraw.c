/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: getraw.c,v 1.9 1995/10/21 18:55:13 tom Exp $";
#endif

/*
 * Title:	getraw.c
 * Author:	T.E.Dickey
 * Created:	03 May 1984
 * Last update:
 *		18 Feb 1995, removed syi-testing.
 *		24 Feb 1989, when reading from a command-file, suppress lines
 *			     which do not begin with '$' (with optional blanks)
 *		17 Aug 1988, use SYS$COMMAND instead of SYS$INPUT.
 *		10 Jun 1985, ensure type-ahead is killed if CTRL/X
 *		08 Jun 1985, added code for interactive type-ahead
 *		10 Apr 1985, made command-files nest, some better error checks.
 *		06 Apr 1985, added file-input, CTRL/X-ast
 *		29 Mar 1985, added 'status' to 'error'.
 *		24 Feb 1985, removed ^C^C.
 *		02 Dec 1984, added entry 'gotraw' to do QIO (no wait)
 *		09 Jul 1984, use virtual-block instead on TTY_READALL
 *		04 Jul 1984, make debug more readable
 *		27 Jun 1984, do debug-print to look for lockup
 *		22 Jun 1984, test status from 'iosb'
 *		20 Jun 1984, added timeout test
 *		18 Jun 1984, do 'fflush' to attempt fix for spurious lockups
 *		31 May 1984, double ^C causes walkback-exit
 *		10 May 1984, provide passall mode if no echo
 *		07 May 1984, to make echo optional, other arguments.
 *
 * Function:	Obtain a single character from the terminal without echo.
 *
 * References:	VAX/VMS I/O User's Guide
 *		(1-16 to 1-19) - sys$qiow
 *		(2-3) - Discussion of escapes vs read-terminator
 */

#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>

#include	<starlet.h>
#include	<lib$routines.h>
#include	<descrip.h>
#include	<iodef.h>
#include	<ssdef.h>
#include	<stsdef.h>

#include	"bool.h"
#include	"flist.h"
#include	"getraw.h"
#include	"rmsio.h"
#include	"rmscc.h"

/*
 * Local definitions:
 */
#define	OK(f)	$VMS_STATUS_SUCCESS(status=f)
#define	SYS(f)	if (!OK(f)) lib$stop(status)

/* patch: Should I use 'lib$get_ef' ? */
#define	ef2	2

#define	QA(efn,func,b,len,t)\
	efn,		/* Event flag number		*/\
	tty_chan,\
	func,\
	&iosb,0,0,\
	b,		/* buffer address		*/\
	len,		/* buffer size			*/\
	t,		/* timeout count		*/\
	0,		/* read terminator desc-blk addr */\
	0,		/* prompt-string buffer address */\
	0		/* prompt-string buffer size	*/

typedef	struct	{ short	sts, count; unsigned device; } IOSB;

#undef  CFP
#define	CFP	struct _cmd_fp

CFP	{
	CFP	*nest;		/* linked-list to nest	*/
	RFILE	*file;		/* File-pointer			*/
	char	*text;		/* => I/O buffer		*/
	unsigned mark;		/* = record-address		*/
	short	size;		/* I/O buffer size		*/
	short	rlen;		/* = length of current record	*/
	short	used;		/* number of chars used so far	*/
	short	iscc;		/* TRUE if format is carriage-control */
	};

#define	CFP_TEXT(n)	cfp->text[cfp->n]

/*
 * Local (static) data:
 */
$DESCRIPTOR(tty_name,"SYS$COMMAND");

static	CFP	*cfp	= nullS(CFP);	/* command-file-pointer	*/
static	unsigned
	short	tty_chan = 0,
		ast_chan = 0,
		init	= FALSE;	/* Force auto-init */
static	unsigned ctlx_flag = FALSE;
static	char	typeahead;		/* input-buffer	*/

static	void	getraw_free (void);

/*
 * Provide a publicly-testable CTRL/X-flag, which is set only if the AST-routine
 * is called:
 */
void
ctlx_ast (void)
{
	unsigned status;

	SYS(sys$cancel(tty_chan));
	ctlx_flag = TRUE;
	typeahead = EOS;
}

void
ctlx_clr (void)
{
	ctlx_flag = FALSE;
}

int
ctlx_tst (void)
{
	return(ctlx_flag);
}

/* <getraw_init>:
 * The initialization routine for this module may be either called explicitly
 * (to call in a new command file), or implicitly (when the module is first
 * invoked).
 *
 * Arguments:
 *	cmd_	=> command-file name
 *	dft_	=> file-specification default
 */
void
getraw_init (char *cmd_, char *dft_)
{
	int	bmask[2] = {0, 0x01000000};
	unsigned status;

	/*
	 * If a command-file argument is provided, open the file for input.
	 * We will read from this file until we get an end-of-file.
	 */
	if (cmd_)
	{
		if (*cmd_)
		{
		CFP	*old = cfp;
			cfp = calloc(1, sizeof(CFP));
			cfp->nest = old;
			if (cfp->file = ropen2 (cmd_, dft_, "r"))
			{
				cfp->size = 4 + rsize(cfp->file);
				cfp->text = calloc(1, cfp->size);
				cfp->iscc = rmscc(cfp->file) > 0;
			}
			else
				getraw_free ();
		}
	}

	if (! tty_chan)
	{
		SYS(sys$assign(&tty_name, &tty_chan, 0,0));
	}

	if (! ast_chan)
	{
		SYS(sys$assign(&tty_name,&ast_chan,0,0));

		/* Establish CTRL/X AST routine */
		SYS(sys$qiow(ef2,ast_chan,IO$_SETMODE | IO$M_OUTBAND,0,0,0,
			&ctlx_ast,&bmask,0,0,0,0));
	}
	init = TRUE;
}

/*
 * Wait for, read one or more characters for commands:
 */
int
getraw (void)
{
	int	gotc	= FALSE,
		status;
	char	newc;
	IOSB	iosb;

	if (!init)	getraw_init(0,0);

	if (cfp)
	{
		if (cfp->used >= cfp->rlen)
		{
get_cmd:
			if ((cfp->rlen = rgetr(cfp->file, cfp->text,
					       cfp->size, &cfp->mark)) >= 0)
			{
				strcpy (&CFP_TEXT(rlen++),
					cfp->iscc ? "\r" : "");
				cfp->used = 0;
				while (isspace(CFP_TEXT(used)))
					cfp->used++;
				if (CFP_TEXT(used++) != '$') goto get_cmd;
				while (	(CFP_TEXT(used) == ' ')
				||	(CFP_TEXT(used) == '\t'))
					cfp->used++;
			}
			else
				getraw_free();
		}
		if (cfp)	/* Still here if no end-of-file	*/
		{
			if (gotc = (cfp->used < cfp->rlen))
				newc = CFP_TEXT(used++);
		}
	}

	/*
	 * Use data from call on 'gotraw':
	 */
	if (! gotc && typeahead)
	{
		gotc	= TRUE;
		newc	= typeahead;
	}

	/*
	 * If neither command-file, nor type-ahead, wait for input from the
	 * terminal:
	 */
	while (! gotc)
	{
		if (ctlx_tst())
		{
			newc = CTL('X');
			ctlx_clr();		/* Reset flag after use !! */
			break;
		}
		if (! OK(sys$qiow (QA(0,
			IO$_READVBLK | IO$M_NOFILTR | IO$M_NOECHO,
			&typeahead,1,0))))
			error (status, 0);

		gotc = (iosb.sts == SS$_NORMAL);
		newc = typeahead;
		/* CTRL/X yields SS$_ABORT */
	}

	typeahead = EOS;
	return (newc);
}

/* <gotraw>:
 * Return TRUE iff we have already another character in the input buffer,
 * following the one which we have (just) read.  This test lets us optimize
 * screen outputs by combining several refreshes together (e.g., postponing
 * updates of an input line until we have a carriage-return).
 */
int
gotraw (void)
{
	int	status;
	IOSB	iosb;

	if (cfp)
		return (cfp->used < cfp->rlen);
	else if (! typeahead)
	{
		if (! OK(sys$qio (QA(ef2,
			IO$M_TIMED | IO$_READVBLK | IO$M_NOFILTR | IO$M_NOECHO,
			&typeahead,1,0))))
			error (status, 0);

		sys$waitfr (ef2);
		if (iosb.sts != SS$_NORMAL)	typeahead = EOS;
	}
	return (isprint(typeahead));
}

/* <getraw_free>:
 * Close the current command-file, returning buffer space.  If there is an
 * error, print the message.
 */
static void
getraw_free (void)
{
	char	msg[CRT_COLS];

	if (cfp)
	{
	CFP	*old = cfp->nest;
		if (cfp->text)	cfree (cfp->text);
		if (cfp->file)
		{
			if (erstat (cfp->file, msg, sizeof(msg)))
				warn (msg);
			rclose (cfp->file);
		}
		else
			rerror();	/* Show file-not-found message	*/
		cfree (cfp);
		cfp = old;
	}
	rclear();	/* Reset I/O-error latch	*/
}

/*
 * <getraw_flush>:
 * Discard the remainder of the current command-file line.
 */
void
getraw_flush(void)
{
	if (cfp)
		cfp->used = cfp->rlen + 1;
}
