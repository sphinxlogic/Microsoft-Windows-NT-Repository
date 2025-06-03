/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: dircmd.c,v 1.15 1995/10/27 11:37:00 tom Exp $";
#endif

/*
 * Title:	dircmd.c
 * Author:	T.E.Dickey
 * Created:	10 May 1984
 * Last update:
 *		27 Oct 1995, added "create" command.
 *		26 Oct 1995, mods for animated "Working..." message
 *		24 Oct 1995, added commands to sort-by-username
 *		27 May 1995, prototypes
 *		04 Nov 1988- added "/dexpired" table entry, and matching sort
 *		11 Jul 1988- fix side-effect of "<number> -" from dclarg fix.
 *		01 Jul 1988- added LSEDIT command
 *		12 Nov 1985, added '?owner' command.
 *		05 Oct 1985, added '/width' command to permit 80/132-column
 *			     switch on VT100's.  Also, permit additional
 *			     keyword on HELP.
 *		21 Sep 1985, merged all VCMD2-tables by handling '/', '?'
 *			     prefixes in 'dirarg' module.  Added '/protection'
 *			     as alias for PROTECT (SET PROTECTION) and '?daytime'
 *			     as alias for ?DATE (SHOW [DAY]TIME).  Use 'debrief'
 *			     to show possible commands user may mean if he forgets
 *			     the unambiguous-abbreviation.
 *		31 Jul 1985, added 'UPDATE' command.
 *		25 Jul 1985, added '?READ' command.
 *		20 Jul 1985, added DUMP command, for debugging structured heap.
 *		05 Jul 1985, corrected release of memory in 'dircmd_doit'.
 *		03 Jul 1985, cleanup of 'filelist' definition.  Use 'scanint'
 *			     instead of 'sscanf' to bypass CC2.0 bug.
 *		15 Jun 1985, reference '(func)' as '(*func)' to make CC2.0 happy
 *		10 Jun 1985, added 'SHELL', '?FIND' commands
 *		24 May 1985, corrected treatment of '?' (for '?HELP').  Added
 *			     log-file for /MARK and other commands which imply
 *			     operations on the current file.
 *		19 May 1985, added '/CWIDTH' command.
 *		18 May 1985, added '/' alone as a scroll-to-top command.
 *		14 May 1985, thrashed for a while before finding a bug in the
 *			     cmdstk module.  Leave debug-code inline for later
 *			     use.
 *		05 May 1985, completed 'keydefs' code.  (Must still merge the
 *			     two vcmd-tables and check arguments).
 *		01 May 1985, began 'keydefs' code, restructured command invokes
 *			     to use a single (redefinable) table of key-defs,
 *			     rather than the combination return-code to main-case
 *			     with special keypad stuff, and then visible commands
 *			     treated as the third case.
 *		30 Apr 1985, broke 'dircmd_doit' from 'dircmd_read'.
 *		27 Apr 1985, corrected repeated-command test (in 'edtcmd').
 *		25 Apr 1985, re-use 'edtcmd' with BROWSE by adding delimiter-arg.
 *		18 Apr 1985, chopped out 'edtcmd' code
 *		13 Apr 1985, added call on 'flist_log'
 *		11 Apr 1985, added call to 'gotraw' to make typeahead run faster.
 *		30 Mar 1985, 'didbeep()' is a superset of 'didwarn' test.
 *			     Added '?MEMORY'.
 *		22 Mar 1985, added "/SIDENTIFIER", "/SLENGTH".
 *		14 Mar 1985, renamed 'more' to 'browse'
 *		05 Mar 1985, had stepped-on repeated-error latch.
 *		23 Feb 1985, added magic-intercept for ^D
 *		05 Jan 1985, added "NEXT" keyword.  Use 'read_dft' for FIND
 *			     and NFIND defaults.
 *		29 Dec 1984, added "/SBACKUP", "/SCREATED", "/SREVISED", and
 *			     relative (e.g., "12-") numeric scrolling.  Show
 *			     SELECT-state (via 'dds_line').
 *			     Put BROWSE on key-9.
 *		24 Dec 1984, added "/DSHORT", "/DLONG"
 *		23 Dec 1984, added "/CLEFT", "/CRIGHT".
 *		22 Dec 1984, added "NFIND", changed record-attributes to 'X'
 *			     to provide for 'allocate', 'length'.  In command-
 *			     edit, permit up/down arrows to traverse 'cmdstk'.
 *		19 Dec 1984, use ^E as ==> (go to end of command), also moved
 *			     'cmdstk' init calls to main program.
 *		18 Dec 1984, treat ^Y as ^U if we ever see it.
 *		15 Dec 1984, added 'cpy_dft' argument to 'dclarg'.
 *		11 Dec 1984, added '?QUOTA', '/SFORMAT', '/SATTRIBUTE'.
 *		17 Nov 1984, added 'INSPECT'
 *		22 Oct 1984
 *
 * Function:	This module performs the actual command input for the "FLIST"
 *		program.  Commands are of three types:
 *
 *		(a)	Set command-processor state (e.g., GOLD, FORWARD, and
 *			BACKWARD keys).
 *		(b)	Keypad commands, or
 *		(c)	Visible commands, requiring multiple keystrokes.  All
 *			visible-commands are terminated with ENTER/RETURN.
 *			This mode of operation is assumed whenever the user
 *			types a printing character (instead of an escape
 *			sequence or control character).  Visible commands
 *			which correspond to keypad commands are translated
 *			and returned to the main program.  Those which do not
 *			are executed under control of this module.
 */

#define	DEBUG
#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	<rms.h>

#include	"cmdstk.h"
#include	"flist.h"
#include	"getpad.h"

#include	"acpcopy.h"
#include	"dclarg.h"
#include	"dds.h"
#include	"freelist.h"
#include	"dircmd.h"
#include	"edtcmd.h"

#include	"strutils.h"

static	void	dircmd_doit (int *curfile_, char *cmdbfr, int history);
static	int	dircmd_getc (int *curfile_);
static	void	dircmd_kcmd (int *curfile_, int command);
static	void	dircmd_keydefs (int code, int flags, char *string);
static	void	dircmd_read (int *curfile_, int command, char *original);

/*
 * Special command-processing routines are called:
 *	(tocall)(&curfile, xcmd_, xdcl_);
 *
 * where:
 *	curfile - index in 'filelist[]' to current (working) file.  The address
 *		  is passed to support search/scrolling commands.
 *	xcmd_	=> lowercased-string, equal to substituted tokens & logicals.
 *	xdcl_	=> DCLARG-list (filespecs are uppercased).
 */

import(filelist);

/*
 * Local (static) data:
 */
#define	DO_NEXT	0		/* value if 'command' completed, do next*/

#define	RUBOUT	'\177'

/*
 * 'dir_flg' is used locally to control the interpretation of the keypad keys
 *	which control scrolling/searching.
 * 'selected' is used in the 'dirsrt.c' module to determine the resulting
 *	index into 'dirent[]' at which the cursor will be placed after a sort.
 *	This variable supports the notion of a fixed-point sort.  (If negative,
 *	it is inactive.)
 * 'read_dft' is used as the default name-string for READ, EDIT (with multiple-
 *	files), and is toggled by the FLIST option "VERSION, NOVERSION".
 */
int	multi_quit;			/* Nonzero if multi-quit	*/

static	int	dir_flg	= TRUE;		/* TRUE if "forward"		*/
static	int	selected;		/* index to "selected" file	*/

static	char	read_dft[]	= "*.*;*";

typedef	struct	{
	unsigned char flags;		/* nonzero iff we must edit it	*/
	char	*string;
	} KEYDEFS;

static	KEYDEFS	*keydefs	= 0;

/*
 * Symbol table to relate "visible" commands to actual controls.  The
 * actual-text may not be printing-ASCII, because this is trapped by the
 * visible-command-detection loop.  Commands which are also control characters
 * are mapped into these control characters.  (Those having no "actual" code
 * are decoded on the basis of the "func" component, which always overrides.)
 *
 * patch: This table could be re-structured to be initialized in 'dircmd_init'
 *	to accept a profile-file (a la FLIST), relating all visible-commands
 *	to control keys or escape sequences.
 */
#define	OZ(flag)	(v_nonDCL | flag),0
static
VCMD2	vcmd2[]	= {
	"show",		3, not_impl,	OZ(0),	/* "?"-prefix	  */
	"set",		3, not_impl,	OZ(0),	/* "/"-prefix	  */
						/* Status+display */
	"?help",	1, flshow,	OZ(v_ARGS),
	"?columns",	2, flshow,	OZ(0),
	"?date",	2, flshow,	OZ(0),
	"?daytime",	2, flshow,	OZ(0),
	"?find",	2, flshow, 	OZ(0),
	"?level",	2, flshow,	OZ(0),
	"?memory",	2, flshow,	OZ(0),
	"?owner",	2, flshow,	OZ(0),
	"?quota",	2, flshow,	OZ(0),
	"?read",	2, flshow,	OZ(0),
	"?size",	2, flshow,	OZ(0),
	"?times",	2, flshow,	OZ(0),
	"?version",	2, flshow,	OZ(0),
						/* Help-display	*/
	"/help",	4, flshow,	OZ(v_ARGS),
	"/hlp",		4, flshow,	OZ(v_ARGS),
	"help",		1, flshow,	OZ(v_ARGS),
						/* Commands	*/
	"quit", 	1, flquit,	OZ(v_ARGS),
						/* Trap messages*/
	"/hold",	4, flset_hold,	OZ(0),
	"/nohold",	4, flset_hold,	OZ(0),
						/* Selection	*/
	"/mark",	4, flset_mark,	OZ(v_NAME),
	"/nomark",	4, flset_mark,	OZ(0),
						/* Scrolling	*/
	"/top",		4, flpage,	OZ(0),
	"/end", 	4, flpage,	OZ(0),
	"/page",	2, flpage,	OZ(v_ARGS),
	"/backward",	2, flpage,	OZ(v_ARGS),
	"/home",	2, flpage,	OZ(0),
	"/middle",	2, flpage,	OZ(0),
	"/low",		2, flpage,	OZ(0),
	"/forward",	2, flpage,	OZ(v_ARGS),
						/* Display form */
	"/columns",	2, flcols,	OZ(v_ARGS),
	"/cleft",	3, flcols_left,	OZ(0),
	"/cright",	3, flcols_right,OZ(0),
	"/cwidth",	3, flcols_width,OZ(v_ARGS),
	"/times",	3, flshow,	OZ(0),
	"/width",	2, flcols_132,	OZ(v_ARGS),
						/* Date-display	*/
	"/dcreated",	2, flset_date,	OZ(0),	/* (default) */
	"/dbackup",	3, flset_date,	OZ(0),
	"/drevised",	3, flset_date,	OZ(0),
	"/dexpired",	3, flset_date,	OZ(0),
	"/dshort",	3, flset_date,	OZ(0),
	"/dlong",	3, flset_date,	OZ(0),
						/* Normal-sort	*/
	"/salloc",	3, flsort,	OZ(0),
	"/sbackup",	3, flsort,	OZ(0),
	"/screated",	3, flsort,	OZ(0),
	"/sdate",	3, flsort,	OZ(0),
	"/sexpired",	3, flsort,	OZ(0),
	"/sformat",	3, flsort,	OZ(0),
	"/shour",	3, flsort,	OZ(0),
	"/sidentifier",	3, flsort,	OZ(0),
	"/slength",	3, flsort,	OZ(0),
	"/smask",	3, flsort,	OZ(0),
	"/sname",	2, flsort,	OZ(0),	/* (default)	*/
	"/sowner",	3, flsort,	OZ(0),
	"/spath",	3, flsort,	OZ(0),
	"/srevised",	3, flsort,	OZ(0),
	"/ssize",	3, flsort,	OZ(0),
	"/stype",	3, flsort,	OZ(0),
	"/suser",	3, flsort,	OZ(0),
	"/svers",	3, flsort,	OZ(0),
	"/sweek",	3, flsort,	OZ(0),
	"/sxab",	3, flsort,	OZ(0),
						/* Reverse-sort	*/
	"/ralloc",	3, flsort,	OZ(0),
	"/rbackup",	3, flsort,	OZ(0),
	"/rcreated",	3, flsort,	OZ(0),
	"/rdate",	3, flsort,	OZ(0),
	"/rexpired",	3, flsort,	OZ(0),
	"/rformat",	3, flsort,	OZ(0),
	"/rhour",	3, flsort,	OZ(0),
	"/ridentifier",	3, flsort,	OZ(0),
	"/rlength",	3, flsort,	OZ(0),
	"/rmask",	3, flsort,	OZ(0),
	"/rname",	2, flsort,	OZ(0),	/* (default)	*/
	"/rowner",	3, flsort,	OZ(0),
	"/rpath",	3, flsort,	OZ(0),
	"/rrevised",	3, flsort,	OZ(0),
	"/rsize",	3, flsort,	OZ(0),
	"/rtype",	3, flsort,	OZ(0),
	"/ruser",	3, flsort,	OZ(0),
	"/rvers",	3, flsort,	OZ(0),
	"/rweek",	3, flsort,	OZ(0),
	"/rxab",	3, flsort,	OZ(0),

/*
 * Symbol table for commands which (may) require DCL arguments or options.
 * The command parser performs some checking, to ensure that the command
 * looks something like it is supposed to.
 */
	"append",	2, flcopy,	v_M_1,		0,
	"browse",	1, flmore,	v_1,		0,
	"copy",		2, flcopy,	v_M_1,		0,
	"create",	2, flcrea,	v_1d,		0,
	"delete",	3, fldlet,	v_M0,		0,
#ifdef	DEBUG
	"dump",		2, fldump,	v_OMIT,		0,
#endif
	"edit",		1, fledit,	v_M,		read_dft,
	"find",		1, flfind,	v_1a,		read_dft,
	"inspect",	1, flscan,	v_1c,		read_dft,
	"lsedit",	1, fledit,	v_M,		read_dft,
	"next",		1, flfind,	v_OMIT,		0,
	"nfind",	2, flfind,	v_1a,		read_dft,
	"print",	2, flnoid,	v_M,		".LIS",
	"/protection",	3, flprot,	v_1b,		0,
	"protect",	3, flprot,	v_1b,		0,
	"purge",	3, fldlet,	v_M0,		0,
	"read",		1, flread,	v_1c,		read_dft,
	"rename",	3, flrnam,	v_1_1,		0,
	"shell",	2, flescp,	v_OMIT,		0,
	"spawn",	2, flescp,	v_OMIT,		0,
	"submit",	2, flnoid,	v_1,		".COM",
	"update",	1, flread,	v_1c,		read_dft,
	"verify",	2, flscan,	v_1c,		read_dft,
	"view",		1, fledit,	v_M,		read_dft
	};

/* <dircmd>:
 * Loop, reading characters and executing commands until we either delete the
 * last file in the list, or we receive a QUIT-command.
 */
void	dircmd (void)
{
	int	command,
		curfile	= 0;

	while (! dds_last (&curfile))
	{
		command = dircmd_getc (&curfile);

		if (	(command == RETRIEVE)
		||	(isascii(command) && isprint(command)) )
			dircmd_read (&curfile, command, "");
		else if (command != DO_NEXT)
			dircmd_kcmd (&curfile, command);
	}
}

/* <dircmd_kcmd>:
 * Given that a legal keypad or control code was input, complete the processing:
 */
static
void	dircmd_kcmd (int *curfile_, int command)
{
	register int	j;
	register int	gold	= (command == GOLDKEY);
	register char	*s_;
	char	cmdbfr[CRT_COLS];

	if (gold)
	{
		while (command == GOLDKEY)
			command = dircmd_getc (curfile_);
	}

	/*
	 * Check for a GOLD followed by a non-keypad/non-control key:
	 */
	if (! (is_PAD(command) || (command < ' ')))
		sound_alarm();		/* ...if so, ignore the key	*/

	j = toascii(command) + (gold ? 128 : 0);
	if (s_ = keydefs[j].string)
	{
		if (keydefs[j].flags & 2)
			dircmd_read (curfile_, EOS, s_);
		else
		{
			strcpy (cmdbfr, s_);
			dircmd_doit (curfile_, cmdbfr, FALSE);
		}
	}
	else
	{
		set_beep ();	/* Set flag indicating error	*/
		sound_alarm ();
	}
	if (multi_quit <= 0)	dds_index (*curfile_);
}

/*
 * Set/Query the selected-file index.  (A "-2" will not alter the value.)
 * We use 'dds_line' to alter the display to show the selected/deselected file.
 */
int	dircmd_select (int val)
{
	if (val >= -1)
	{
	int	was_sel	 = selected;
		selected = val;
		if (was_sel != selected)
		{
			if (was_sel  >= 0)	dds_line (was_sel);
			if (selected >= 0)	dds_line (selected);
		}
	}
	return (selected);
}

/* <dircmd_init>:
 * Initialize this module
 */
void	dircmd_init (void)
{
	edtcmd_init();
	dircmd_dirflg (TRUE);		/* forward-direction	*/
	selected  = -1;			/* Index of fixed-point	*/
	multi_quit= 0;			/* No "quit" in progress*/

	keydefs	  = calloc (256, sizeof(KEYDEFS));
	dircmd_keydefs (CTL('B'),	0,	"/backward");
	dircmd_keydefs (CTL('E'),	0,	"/end");
	dircmd_keydefs (CTL('F'),	0,	"/forward");
	dircmd_keydefs (CTL('G'), 	0,	"/width");
	dircmd_keydefs (CTL('H'),	0,	"1-");
	dircmd_keydefs (CTL('M'),	0,	"1+");
	dircmd_keydefs (CTL('N'),	0,	"next");
	dircmd_keydefs (CTL('P'),	0,	"protect");
	dircmd_keydefs (CTL('R'),	0,	"update /n./t;/v");
	dircmd_keydefs (CTL('T'),	0,	"/top");
	dircmd_keydefs (CTL('V'),	0,	"verify");
	dircmd_keydefs (CTL('Z'),	0,	"quit");

	dircmd_keydefs (padUP,		0,	"1-");
	dircmd_keydefs (padUP,		1,	"/backward 1");
	dircmd_keydefs (padDOWN,	0,	"1+");
	dircmd_keydefs (padDOWN,	1,	"/forward 1");

	dircmd_keydefs (HELPKEY,	0,	"?help");
	dircmd_keydefs (HELPKEY,	1,	"?help");
	dircmd_keydefs (padPF3, 	0,	"next");
	dircmd_keydefs (padPF3, 	1+2,	"find ");
	dircmd_keydefs (padPF4, 	0,	"/cleft");
	dircmd_keydefs (padPF4, 	1,	"/cright");
	dircmd_keydefs (pad0,		0,	"/sname");
	dircmd_keydefs (pad0,		1,	"/rname");
	dircmd_keydefs (pad1,		0,	"/stype");
	dircmd_keydefs (pad1,		1,	"/rtype");
	dircmd_keydefs (pad2,		0,	"/sdate");
	dircmd_keydefs (pad2,		1,	"/rdate");
	dircmd_keydefs (pad3,		0,	"/ssize");
	dircmd_keydefs (pad3,		1,	"/rsize");
	dircmd_keydefs (pad4,		0,	"/forward 0");
	dircmd_keydefs (pad4,		1,	"/end");
	dircmd_keydefs (pad5,		0,	"/backward 0");
	dircmd_keydefs (pad5,		1,	"/top");
	dircmd_keydefs (pad6,		0,	"?time");
	dircmd_keydefs (pad6,		1,	"/time");
	dircmd_keydefs (pad7,		0,	"edit");
	dircmd_keydefs (pad7,		1,	"view");
	dircmd_keydefs (pad8,		0,	"/page");
	dircmd_keydefs (pad9,		0,	"browse");
	dircmd_keydefs (pad9,		1,	"browse/mark/over:3");
	dircmd_keydefs (padMINUS,	0+2,	"-");
	dircmd_keydefs (padMINUS,	1+2,	"-");
	dircmd_keydefs (padCOMMA,	0,	"?memory");
	dircmd_keydefs (padDOT,		0,	"/mark");
	dircmd_keydefs (padDOT,		1,	"/nomark");
	dircmd_keydefs (padENTER,	0,	"1+");
	dircmd_keydefs (padENTER,	1,	"1-");

	strcpy (read_dft, dirent_dft());
}

/* <dircmd_keydefs>:
 * Copy a new key-definition into the 'keydefs[]' table.  We map the control
 * characters to themselves, and the keypad codes down to ASCII.  Use the
 * gold-key flag to drive a shift into 128-255.
 */
static
void	dircmd_keydefs (int code, int flags, char *string)
{
	register int c = toascii(code) + ((flags & 1) ? 128 : 0);
	keydefs[c].flags = flags & ~1;
	keydefs[c].string = string;
}

/* <dircmd_getc>:
 * Read a single command (keystroke-level), suppressing characters such
 * as control/S and control/Q which may be passed back by 'getpad()',
 * which are not used as single-key commands by "FLIST".
 *
 * In particular, absorb all incoming nulls, since this code is used (DO_NEXT)
 * to tell the higher-level routines to continue reading command-input.
 */
static
int	dircmd_getc (int *curfile_)
{
	int	command;

	for (;;)
	{
		switch (command = edtcmd_get())
		{
		case CTL('U'):
		case CTL('X'):
		case CTL('C'):			/* Equate ^C, ^Y to ^U	*/
		case CTL('Y'):
		case ' ':
		case '\t':
		case '\n':
		case RUBOUT:
		case padLEFT:
		case padRIGHT:
			sound_alarm ();		/* Illegal context	*/
		case CTL('Q'):
		case CTL('S'):
		case 0:				/* == DO_NEXT		*/
			break;
		case '-':			/* Gives 1 level w/o keypad */
			command = RETRIEVE;
		default:
			goto got_command;
		}
	}
got_command:
	/*
	 * If a message was pending, wipe it off with the first character
	 * of a new command, except:
	 *	a) if we had an error message, and the current character is
	 *	   a repeat of the offending one, or
	 *	b) if we had an informational message (triggered by a keypad
	 *	   or control-key) and it is repeated.
	 *
	 * patch: should suppress wipeoff if the key is GOLD.
	 */
	if ((command == RETRIEVE) || didbeep() || didwarn())
	{
		register
		int	repeated = edtcmd_last(command) &&
					(iscntrl(command) ||
					!isascii(command) ),
/********/
			isel	= dircmd_select(-2);

		if (repeated
		&&  (isel >= 0)
		&&  (isel != *curfile_)
		&&  (command == CTL('D'))) /* PATCH !! */
		{
			char	iname[MAX_PATH],
				oname[MAX_PATH];
			dirent_glue (iname, FK_(isel));
			dirent_glue (oname, FK_(*curfile_));
			acpcopy (3, iname, oname);
			dirent_chk2 (*curfile_); /* Re-read this entry */
			command = DO_NEXT;
		}
		else
/********/
		if (!repeated)
		{
			clrwarn();		/* ...clear it off screen */
			dds_index (*curfile_);
		}
	}
	return (command);
}

/* <dircmd_read>:
 * Read/edit a "visible" command.  After editing it, execute it:
 */
static
void	dircmd_read (int *curfile_, int command, char *original)
{
	int	line	= *curfile_ - crt_top();

	clrwarn();
	dds_index (*curfile_);		/* clear out any prior messages */

	if (command == '-')	command = RETRIEVE;
	dircmd_doit (
		curfile_,
		edtcmd (command,	/* Initial command-char		*/
			nullC,		/* No special delimiter-list	*/
			7,		/* Highlight+lowercase+trim	*/
			line,		/* ...on which to receive	*/
			dirent_ccol(),	/* beginning command-column	*/
			"FLIST",	/* patch:'whoami': help-entry	*/
			original,
			crtvec[line]),
		TRUE);			/* Save resulting command	*/
}

/* <dircmd_doit>:
 * Execute a command string.  Return a continuation command-code.
 */
static
void	dircmd_doit (int *curfile_, char *cmdbfr, int history)
{
DCLARG	*xdcl_	= nullS(DCLARG);	/* 'dirarg()' parsed list	*/
tDIRCMD((*DO_func)) = 0;		/* external function to execute	*/
int	cmdlen	= 0,			/* length of parsed keyword	*/
	cmdnum	= EXTRN_CMD,		/* index to 'vcmd2', if used	*/
	j,
	prefix	= 0,			/* nonzero iff prefix operator	*/
	k1st	= 0,			/* != 'prefix' if SET/SHOW	*/
	omit	= FALSE,		/* TRUE iff "/o" prefix used	*/
	non_DCL	= FALSE,		/* TRUE iff non-DCL arguments	*/
	temp_fix= FALSE,		/* TRUE iff '//sort'		*/
	sort_cmd= FALSE,		/* '/' prefix only on sort-cmd	*/
	known_cmd = FALSE;
char	*fullname = "",
	*xcmd_	= nullC,		/* 'dirarg()' substituted string*/
	tmp_bfr	[MAX_PATH],		/* temporary buffer		*/
	err_msg	[MAX_PATH];

	clrwarn();			/* Permit current command to continue */
	flist_log ("$ %.80s", cmdbfr);

	if (!*cmdbfr)
		return;		/* Ignore ^U, etc.	*/

	/*
	 * Translate & execute the command.
	 *
	 * The first "token" on the line is the command keyword.  It may
	 * contain a leading "/" for punctuation, but otherwise must be a
	 * legal VMS filespec (i.e., the most general form of a command).
	 * (This rule is used to find the beginning of the argument
	 * substitution list).
	 *
	 * A "/" before a sort command (e.g., "//sn") will cause a flag to
	 * be set selecting a fixed-point sort for that command only.  This
	 * is an extension provided only for sort-commands.
	 *
	 * A "/o" before any non-sorting command sets a flag which disables
	 * the normal practice of using the current file-entry as an implicit
	 * argument.
	 */
	cmdlen	= strtrim (cmdbfr);
	prefix	= 0;
	omit	= FALSE;
	if (cmdbfr[0] == '/')
	{
		if (cmdbfr[1] == '/')
			prefix++;		/* Allow "//sort"	*/
		else if (isdigit(cmdbfr[1]) || isspace(cmdbfr[1]))
			prefix++;		/* Allow "/ number"	*/
		else if (cmdbfr[1] == 'o' && !isalpha(cmdbfr[2]))
		{
			prefix	+= 2;
			omit	= TRUE;
		}
		if (prefix)	/* Allow blanks after prefix-text	*/
			prefix += strskps(&cmdbfr[prefix]) - &cmdbfr[prefix];
	}

	k1st	= prefix;
	if (strchr ("/@?", cmdbfr[k1st]))		k1st++;
	j	= (dclarg_spec (&cmdbfr[k1st], nullC) - &cmdbfr[k1st]) + k1st;
	cmdlen	= j - prefix;

	/*
	 * Lookup the command-verb.
	 */
	strncpy (tmp_bfr, &cmdbfr[prefix], cmdlen);
	tmp_bfr[cmdlen] = EOS;
	err_msg[0] = EOS;

	if (xcmd_ = dirarg (&xdcl_, *curfile_,
			&cmdbfr[prefix + cmdlen], tmp_bfr, omit) )
	{
		/*
		 * Check 'xcmd_', since it is lowercased, with the complete
		 * command-verb.  We must get an index of at least 2, to skip
		 * over the SET/SHOW verbs.
		 */
		j = strlen(xdcl_->dcl_text);
		if (strchr ("/?", *xcmd_))	j++;
		cmdnum = debrief (err_msg, vcmd2, sizeof(VCMD2), SIZEOF(vcmd2),
				xcmd_, j);
		if (cmdnum >= 2)
		{
			if (!(DO_func = vcmd2[cmdnum].v_func))
				DO_func = not_impl;
			fullname  = vcmd2[cmdnum].v_full;
			non_DCL	  = vcmd2[cmdnum].v_mode & v_nonDCL;
			sort_cmd  = (*fullname == '/')
				 && strchr("rs", fullname[1]);
			if ((sort_cmd && prefix) || (vcmd2[cmdnum].v_mode & v_NAME))
			{
				dirent_glue (tmp_bfr, FK_(*curfile_));
				flist_log ("! %.80s", tmp_bfr);
			}
			if (dirchk (xcmd_, xdcl_, vcmd2[cmdnum].v_mode, TRUE))
				known_cmd = TRUE;
			else
				cmdnum	= ERROR_CMD;
		}
		else
			cmdnum	= EXTRN_CMD;
	}

	cmdlen	+= prefix;	/* count the first '/' as part of cmd	*/
	if (prefix && !omit)	/* Did I have a leading '/'?		*/
	{
		if (non_DCL && sort_cmd)
			temp_fix = TRUE;
		else if (!isdigit(cmdbfr[prefix]))
		{
			warn ("'/'-prefix is legal only on sort-command");
			cmdbfr[0] = EOS;
		}
	}

	/*
	 * Common exit point for all completed command-input:
	 */
	dds_line (*curfile_);	/* refresh the screen after receipt	*/

	/*
	 * A number directs "FLIST" to move the cursor to the specified
	 * file-index a la FLIST.  We permit a prefix '/' (actually ignore
	 * all prefixes), and a trailing sign, for relative movement.
	 */
	if (isdigit(cmdbfr[prefix]))
	{
	register char	*d_ = &cmdbfr[cmdlen];
	register int	sign = 0;
	int	inx = dds_inx1 (*curfile_),
		val;

		if ((cmdlen >= 1) && (d_[-1] == '-')) {
			d_--;
			cmdlen--;
		} else
			d_ = strskps(d_);
		if (*d_)
		{
			if (! strcmp (d_, "+"))		sign = 1;
			else if (! strcmp (d_, "-"))	sign = -1;
			else	warn ("<number> uses no arguments: %s", d_);
			cmdbfr[cmdlen] = EOS;
		}
		if (! didwarn ())
		{
			if (scanint(&cmdbfr[prefix], &val) == &cmdbfr[cmdlen])
			{
				if (val == 1 && sign)
					flist_move (curfile_, *curfile_,
						sign > 0 ? DDS_D_1 : DDS_U_1);
				else
				{
					if (sign)	inx += (sign * val);
					else		inx = val;
					*curfile_ = dds_inx2 (inx);
				}
			}
			else
				warn ("Illegal number: %s\n", &cmdbfr[prefix]);
		}
		cmdbfr[0] = EOS;	/* Don't save for retrieval */
		cmdlen	  = 0;
	}
	else if (!strcmp(cmdbfr, "/"))	/* Force current entry to top-of-screen */
	{
		cmdbfr[0] = EOS;
		dds_scroll (crt_end() + (*curfile_ - crt_top()));
	}
	if (history) cmdstk_put (cmdbfr);

	/*
	 * If 'known_cmd', then we found the command name in one of the tables.
	 */
	if (known_cmd && !didwarn())
	{
		if (cmdnum >= 2)
			(*DO_func)(curfile_, xcmd_, xdcl_, temp_fix, history);
	}
	/* patch:
	 * If not found in symbol table, simply make a nice error message.
	 * Later, should consider trying to spawn an appropriate process to
	 * execute non-screenable commands.
	 */
	else if (cmdbfr[0] && !didwarn())
	{
		if (*err_msg)
			warn ("%s", err_msg);
		else if (cmdlen)
		{
			cmdbfr[cmdlen] = EOS;
			warn ("Unknown command '%.30s'.  Type '/help'.",
				(cmdbfr[0] == '/' ? &cmdbfr[prefix]
						  : cmdbfr) );
		}
		else
			warn ("Illegal command: '%.30s'", cmdbfr);
	}

	dds_done();	/* clear the "Working..." message, if any */

	if (xdcl_)	freelist (xdcl_);
	if (xcmd_)	cfree (xcmd_);
}

/* <dircmd_ask>:
 * This procedure may be called by a visible-command to verify an action.
 * Prompt for:
 *	G - (2) go, selecting Y (if loop) for all successive prompts
 *	Y - (1) yes
 *	N - (0) disallow this particular selection
 *	Q - (-1) same as N, but exit loop, if any
 */
int	dircmd_ask (int curfile, char *msg_)
{
	int	reply	= 256,
		rline	= curfile - crt_top();
	char	*s_,
		tell	[80];
	static	char	ok[]	= "QNYG";

	sprintf (tell, "%.60s (G,Y/N,Q): ", msg_);
	edtcmd_crt (tell, crtvec[rline], TRUE, dirent_ccol(), rline, strlen(tell)-1);
	for (;;)
	{
		for (reply = 256; (!isascii(reply) || !isprint(reply));)
			reply = edtcmd_get ();
		if (s_ = strchr (ok, _toupper(reply)))	break;
		sound_alarm();		/* warn if incorrect reply	*/
	}

	dds_line (curfile);		/* refresh display */

	return ((s_-ok)-1);
}

/* <dircmd_vcmd2>:
 * Do table lookup to find FLIST's full-name for a "visible" command.  This
 * lookup is used only for the VCMD2-entries (which can specify arguments,
 * hence must be parsed).  Because the command name may be found via lookup
 * in the logical-symbol table, this procedure may be called more than once
 * to interpret a command.
 *
 * If the name is not found in FLIST's table, assume it is the name of an
 * external command.
 */
int	dircmd_vcmd2 (char *cmd_, int len)
{
	int	j;

	for (j = 0; j < SIZEOF(vcmd2); j++)
	{
		if ((len >= vcmd2[j].v_len)
		&&  !strncmp (cmd_, vcmd2[j].v_full, len))
			return (j);
	}
	return (EXTRN_CMD);	/* not found, return an illegal index	*/
}

/* <dircmd_full>:
 * Return a pointer to a specified component of 'vcmd2[]'.  This is needed
 * in 'dirarg', which does a lookup into 'vcmd2[]' before testing for a
 * logical-symbol.
 */
VCMD2*	dircmd_full (int cmdnum)
{
	return (&vcmd2[cmdnum]);	/* patch: limits on 'cmdnum' ?	*/
}

/* <dircmd_dirflg>:
 * Return the direction-flag to make it public:
 */
int	dircmd_dirflg (int flag)
{
	if (flag > 0)		dir_flg = TRUE;
	else if (flag == 0)	dir_flg = FALSE;
	return (dir_flg);
}

/* <not_impl>:
 * Print an appropriate error message for unimplemented routines:
 */
tDIRCMD(not_impl)
{
	warn ("?NOT implemented: %.60s", xcmd_ ? xcmd_ : xdcl_->dcl_text);
}
