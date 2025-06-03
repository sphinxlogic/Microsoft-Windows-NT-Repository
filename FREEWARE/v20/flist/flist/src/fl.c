/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: fl.c,v 1.24 1995/10/27 21:30:44 tom Exp $";
#endif

/*
 * Title:	flist.c (FLIST main program)
 * Author:	T.E.Dickey
 * Created:	31 Apr 1984
 * Last update:
 *		26 Oct 1995, err in stdarg conversion
 *		22 Oct 1995, DEC-C clean-compile.  Added seconds to "/dlong".
 *		28 May 1995, use stdarg instead of VARARGS hack.
 *		18 Mar 1995, prototypes
 *		18 Feb 1995, port to AXP (DATENT mods, renamed 'alarm')
 *		28 Feb 1989, log 132-wide messages instead of 80.
 *		23 Feb 1989, use 'flist_chdir()'
 *		04 Nov 1988, added expired-date
 *		05 Oct 1985, added key-argument to 'flist_help'
 *		17 Aug 1985, make 'flist_log' a write-thru from 'flist_lis'
 *		14 Aug 1985, added code to make '?FIND' nested.
 *		31 Jul 1985, added call on 'fledit_pack'.  Restrict subset-options
 *			     to exclude anything modifying XAB's in 'dirent'.
 *		28 Jul 1985, 'getenv' does not appear to return a value when
 *			     running from DEC/shell.
 *		25 Jul 1985, added 'flist_lis' to open/write/close to listing
 *			     file used for '?READ'.
 *		24 Jul 1985, show invocation on /LOG.  Make D_opt inherit properly
 *			     when we do recursion.
 *		23 Jul 1985, separated out options-code for use by EDIT command.
 *		05 Jul 1985, modified calls on 'dirpath_init/free' to make a
 *			     single structured character heap via 'nameheap'.
 *		03 Jul 1985, cleanup 'filelist' definitions.
 *		23 Jun 1985, if cannot enter level, set 'multi_quit'.
 *		15 Jun 1985, use '&' in 'opts[]' declaration for CC2.0,
 *			     typed 'dclarea', 'ropen2'.
 *		14 Jun 1985, call it '/SINCE' instead of '/AFTER'.  Provide
 *			     alternate names /BACKUP, /CREATED, /MODIFIED
 *			     for /DB, /DC, /DR.
 *		13 Jun 1985, added '/AFTER', '/BEFORE' options
 *		24 May 1985, corrected 'flset_mark', 'flset_hold' 'NO' tests
 *		12 May 1985, corrected 'flset_date' switch
 *		05 May 1985, chopped out the no-argument commands to leave them
 *			     solely under control of 'dircmd' module.  The
 *			     'flset_' stuff here is merely a remnant.
 *		13 Apr 1985, set default-for /COMMAND, added '/LOG'.
 *		08 Apr 1985, added /COMMAND option via 'getraw'
 *		30 Mar 1985, added '?MEMORY' display command.
 *		28 Mar 1985, provided conventional error-return point
 *		21 Mar 1985, moved help-code into 'crt_help'.  Also, added "-1"
 *			     interpretation to 'D_mode' (/DS, /DL) to allow a
 *			     very-short date format.
 *		14 Mar 1985, modified help-file search to account for system
 *			     installation.
 *		03 Mar 1985, do flcols-init only once (hierarchical-inherit)
 *		08 Feb 1985, check for limit to nesting-level
 *		02 Feb 1985, use scrolling-margins
 *		01 Feb 1985, broke 'dirread' from 'dirpath'.
 *		14 Jan 1985, altered 'DCLOPT' init-data
 *		10 Jan 1985, had wrong default for '/OWNER'
 *		08 Jan 1985, renamed '/BACKUP' to '/DBACKUP', etc.
 *		02 Jan 1985, use ^Z for quit-command.
 *		30 Dec 1984, make clean call to 'flmore' for current-entry.
 *		29 Dec 1984, added '/mark', '/nomark' to avoid keypad dependency.
 *		24 Dec 1984, added 'D_mode' to drive day-of-week display.
 *		22 Dec 1984, show allocation in "?SIZE"
 *		21 Dec 1984, added 'flist_sysmsg'.
 *		19 Dec 1984, moved 'cmdstk' init here, for hierarchy.
 *		11 Dec 1984, added "?QUOTA".  No "HOURS" option (implicit).
 *		03 Dec 1984, use 'whoami'.
 *		22 Oct 1984, added figure-of-merit (sysfom)
 *		17 Oct 1984, do refresh in 'dircmd'
 *		10 Sep 1984, got rid of <stdio>
 *		03 Sep 1984, use "import"
 *		30 Aug 1984, need at least 6 args in VARARGS
 *		28 Aug 1984, cleanup buffer sizes, added '?'-prefix commands
 *		15 Aug 1984, added date-display toggles (/dcreated, etc.)
 *		14 Aug 1984, added 'warn2' entry
 *		06 Aug 1984, added options BACKUP, REVISED and HOURS
 *		27 Jul 1984, use 'dirpath_free'
 *		25 Jul 1984, added 'cmd_arg' to 'dclarg()', added 'verify'
 *		17 Jul 1984, fixed default on VERSIONS option
 *		14 Jul 1984, use DCLOPT for option parsing
 *		09 Jul 1984, fix kludgey 'more' interface.
 *		30 Jun 1984, removed EDIT/VIEW function entirely to 'fledit'
 *		26 Jun 1984
 *
 * Function:	This program provides screen-oriented directory editing
 *		functions (rename, delete, change protection), combined
 *		with wildcard selection, and sorting on all visible
 *		attributes.  The program design is influenced by the EDT
 *		editor, the DIRED program on Unix, and the FLIST program
 *		on IBM CMS.
 */

#define DIRENT

#include	<stdlib.h>
#include	<stdio.h>
#include	<unixio.h>
#include	<unixlib.h>	/* for 'getcwd()' */
#include	<signal.h>	/* for 'sleep()' */
#include	<stdarg.h>
#include	<string.h>
#include	<ctype.h>
#include	<descrip.h>
#include	<stsdef.h>

#include	"cmdstk.h"
#include	"getpad.h"
#include	"dclarg.h"		/* FIXME */
#include	"flist.h"
#include	"rmsio.h"
#include	"whoami.h"

#include	"dds.h"
#include	"dirent.h"
#include	"dircmd.h"
#include	"dirpath.h"
#include	"dirread.h"
#include	"dclopt.h"

#include	"getraw.h"
#include	"freelist.h"
#include	"nameheap.h"
#include	"strutils.h"
#include	"sysutils.h"

#undef  EXIT_SUCCESS
#undef  EXIT_FAILURE

#define	EXIT_FAILURE	(STS$M_INHIB_MSG | STS$K_ERROR)
#define	EXIT_SUCCESS	(STS$M_INHIB_MSG | STS$K_SUCCESS)

/*
 * External data:
 */
import(filelist); import(numfiles);
import(AnyXAB);
import(A_opt);	import(D_opt);	import(M_opt);
import(O_opt);	import(P_opt);	import(V_opt);
import(D_mode);
import(dateflag);	import(datechek);

import(filelink);
import(pathlist);
import(readlist);
import(readllen);

static	void	FlistTellVA (char *format, va_list ap);
static	void	FlistInfoVA (char *format, va_list ap);

/*
 * Local (static) data:
 */
static
int	beep_flag,	warn_flag,
	hold_flag,	nesting_lvl,	did_crt_init = FALSE;
static
char	dftspec[] = "*.*;*";

/*
 * Options:
 *	AFTER		Append to display-list only files with dates *after*
 *	BEFORE		Append to display-list only files with dates *before*
 *	COMMAND		Read commands from a file until EOF, then terminal
 *	DBACKUP		Show backup dates instead of creation dates
 *	DREVISED	Show file revision dates instead of creation dates
 *	DEXPIRED	Show file expriation dates instead of creation dates
 *	FAST		Suppress all fields other than the filename (*3 faster)
 *	LOG		Record commands and error messages
 *	NOALL		Suppress filename if no privilege for other data
 *	NODATE		Suppress creation-date+time
 *	NOOWNER		Suppress display of owner code
 *	NOPROTECTION	Suppress display of protection mask
 *	NOSIZE		Suppress allocation (size) data
 *	NOVERSIONS	Show only highest version of each file
 */
static
int	_FALSE	= FALSE,
	_TRUE	= TRUE,
	Xfast,	D_opt1, D_opt2,	D_opt3, D_opt4;

#define	COMMAND_	"command"
#define	LOG_		"log"

static	int	H_opt;

static	RFILE	*LisRAB;
static	RFILE	*LogRAB;

static	char	*LogFile;
static	char	*CmdFile;
static	char	LogDft[] = "SYS$LOGIN:FLIST.LOG",
		CmdDft[] = "SYS$LOGIN:FLIST.CMD",
		LisFile[]= "SYS$LOGIN:FLIST.LIS";

#define	SZ(n)	&n,sizeof(n)
static
DCLOPT	opts[] = {
	{"fast",	&_FALSE,0,	SZ(Xfast),	1,	00017},
	{COMMAND_,	0,	CmdDft,	0,0,		1,	00200},
	{LOG_,		0,	LogDft,	0,0,		1,	00400},
	{"date",	&D_opt,	0,	SZ(D_opt1),	1,	00001},
	{"owner",	&_TRUE,	0,	SZ(O_opt),	1,	00002},
	{"protection",	&_TRUE,	0,	SZ(M_opt),	1,	00004},
	{"size",	&_TRUE,	0,	SZ(A_opt),	3,	00010},
#define	SUBOPT	7
	/* * * * * * end-of-subset-options * * * * * */
	{"after",	0,	0,	SZ(datechek),	2,	01000},
	{"all",		&_TRUE,	0,	SZ(P_opt),	1,	00100},
	{"backup",	&_FALSE,0,	SZ(D_opt2),	2,	00001},
	{"before",	0,	0,	SZ(datechek),	2,	01000},
	{"created",	&D_opt,	0,	SZ(D_opt1),	1,	00001},
	{"dbackup",	&_FALSE,0,	SZ(D_opt2),	2,	00001},
	{"drevised",	&_FALSE,0,	SZ(D_opt3),	2,	00001},
	{"dexpired",	&_FALSE,0,	SZ(D_opt4),	2,	00001},
	{"help",	&_FALSE,0,	SZ(H_opt),	1,	07777},
	{"modified",	&_FALSE,0,	SZ(D_opt3),	1,	00001},
	{"since",	0,	0,	SZ(datechek),	3,	01000},
	{"versions",	&_TRUE,	0,	SZ(V_opt),	1,	00400}
	};

int	main (int argc, char **argv)
{
	DCLARG	*arg_	= argvdcl (argc, argv, dftspec, 0);
	char	*path_	= getenv("PATH");

	CmdFile	= dclarea (COMMAND_,	MAX_PATH, (DCLOPT *)&opts, sizeof(opts));
	LogFile	= dclarea (LOG_,	MAX_PATH, (DCLOPT *)&opts, sizeof(opts));
	D_opt	= TRUE;		/* set default so we can inherit it	*/

	if (dclchk (arg_, 0))				return(EXIT_FAILURE);
	if (flist_opts (argc, argv, arg_, FALSE))	return(EXIT_FAILURE);

	sysfom (nullC);		/* Initialize timer			*/

	nesting_lvl = 0;
	hold_flag = FALSE;
	crt_init (0);
	did_crt_init = TRUE;
	crt_margin (1, crt_lpp()-1);
	flcols_init();		/* initialize set of columns to display	*/
	dircmd_init();		/* Initialize command processor		*/
	flist (arg_);		/* Perform root-level call (wildcards)	*/
	freelist (arg_);	/* Release argument list		*/
	if (path_)
	    flist_chdir (path_);/* restore original working directory	*/
	flist_quit (EXIT_SUCCESS);
}

/*
 * Set default directory.
 */
void	flist_chdir(char *path)
{
	static	char	old_path[MAX_PATH];
	auto	char	new_path[MAX_PATH];

	chdir(path);
	getcwd(new_path, sizeof(new_path));
	if (strcmp(old_path, new_path)) {
		if (strcmp(new_path, path))
			flist_log("! asked %s", path);
		flist_log("! chdir %s", strcpy(old_path, new_path));
	}
}

/*
 * Common error/normal exit point:
 */
void	flist_quit (int status)
{
	if (did_crt_init)
		crt_quit (TRUE);	/* Make a clean exit		*/
	exit (status);
}

void	flist (DCLARG *dcl_)
{
	int	curfile = 0;
	int	Quit	= FALSE;
	CMDSTK	*cmdstk_ = cmdstk_init();

	flfind_init (nesting_lvl);
	clrwarn();
	filelist = 0;
	if (Quit = (++nesting_lvl > 8))
		warn ("Too many display levels");
	else
	{
		dirpath_init (nesting_lvl);
		dirent (dcl_);		/* Read directory	*/
		if (Quit = (numfiles <= 0))
			warn ("No files found");
		else
		{
			fledit_pack ();	/* We got to a new level	*/
			dds_all(0, curfile);
		}
	}
	if (Quit)	sleep (3);	/* Make sure we see fatal message */
	else		dircmd();	/* Loop, executing all commands	*/

	/*
	 * Release subdirectory storage and miscellaneous lists.  'filelist'
	 * is initialized only if we have successfully executed a directory
	 * read via 'dirent'.
	 *
	 * Note: 'dcl_' is free'd on return to 'dircmd' or main program.
	 */
	if (filelist)
	{
		cfree (filelist);
		dirread_free ();
	}

	cmdstk_free(cmdstk_);	/* ...and command history	*/
	dirpath_free (nesting_lvl);
	nameheap_set (--nesting_lvl);
}

/*
 * Move the cursor to a new line.  If no actual movement results, flag an
 * error message so that the user will know what happened.
 */
void	flist_move (int *curfile_, int ref, int code)
{
	int	old = *curfile_;
	*curfile_ = dds_move (ref, code);
	if ((ref == old) && (old == *curfile_) && (code /* 2's comp */ & 1))
		warn ((code < 0) ? "Already at top" : "No more files");
}

/*
 * Make the 'hold'-flag visible to external world (see: 'dds_hold()').
 */
int	flist_hold(void)
{
	return (hold_flag);
}

tDIRCMD(flset_hold)
{
	hold_flag = (xcmd_[1] != 'n');
}

/*
 * Set/clear selection-mark:
 */
tDIRCMD(flset_mark)
{
	dircmd_select (xcmd_[1] == 'n' ? -1 : *curfile_);
}

/*
 * Set flags controlling data-display:
 */
tDIRCMD(flset_date)
{
	switch (xcmd_[2])
	{
	case 's':	D_mode--; flist_date2 (*curfile_);	break;
	case 'l':	D_mode++; flist_date2 (*curfile_);	break;
	case 'b':	flist_date (*curfile_, 2);	break;
	case 'r':	flist_date (*curfile_, 3);	break;
	case 'e':	flist_date (*curfile_, 4);	break;
	default:	flist_date (*curfile_, 1);
	}
}

/*
 * Set flag 'D_mode', which controls the width of the data-field in the display.
 * Values:
 *	-1, very short, minimal 12-character format
 *	 0, nominal (date to minutes)
 *	 1, long (day of week plus date to minutes)
 *	 2, very long (day of week plus date to seconds)
 */
void	flist_date2 (int curfile)
{
	if (D_opt)
	{
		D_mode = max(-1, min(2, D_mode));
		dds_all (dds_fast(DDS_U_C), curfile);
	}
	else
		flist_date (curfile, 0);
}

/*
 * Toggle the date-display mode.  To make FLIST faster, normally only one
 * type of filedate (CREATED,BACKUP,REVISED) is shown on the screen at a time.
 */
void	flist_date (int curfile, int opt)
{
	if (D_opt)		/* Can I toggle it ?	*/
	{
		if (opt != D_opt)
		{
			D_opt = opt;
			dds_all (dds_fast(DDS_U_C), curfile);
		}
	}
	else
		warn ("Date-toggle is invalid since NODATE was used");
}

/*
 * FLIST sounds an audible alarm whenever a warning is sent to the status line.
 * This, and other messages are normally cleared from the status line to show
 * the current default directory, and the index into the file-list.  We use
 * flags (beep_flag, warn_flag) to avoid having the status line alternate too
 * often between its default (pathname) and informational states:
 *
 *	beep_flag - tested in 'dds_tell', prevents new data from being typed
 *		on the status line.  This flag is normally reset on the first
 *		character of a new command, except when the character is the
 *		same as the last (single-character) command, which generated
 *		a message.
 *
 *	warn_flag - (maintained here, tested in 'dircmd') is set by any
 *		warning message.  If a single command generates multiple
 *		warnings, we do not really care to see more than one (e.g.,
 *		the case of a repeated, erroneous command).  It is reset in
 *		'dircmd' when we receive the first character of a new command
 *		(so that we can tell if we have the repeated single-character
 *		command).
 */
void	set_beep(void)	{	beep_flag = TRUE;	}
void	clrbeep(void)	{	beep_flag = FALSE;	}
int	didbeep(void)	{	return (beep_flag);	}

void	clrwarn(void)	{	clrbeep(); warn_flag = FALSE;	}
int	didwarn(void)	{	return (warn_flag);	}

/*
 * Display a warning message at the bottom of the screen.
 */
static
void	WarnVA (char *format, va_list ap)
{
	if (!didbeep())
	{
		warn_flag = TRUE;
		sound_alarm();
		FlistTellVA (format, ap);
	}
	else if (++beep_flag < 99)	/* Continue beeping, but limit it */
		sound_alarm();
}

void	warn (char *format, ...)
{
	va_list	ap;

	va_start(ap, format);
	WarnVA(format, ap);
	va_end(ap);
}

/*
 * This entry is used for the (rare) case in which a series of operations
 * is requested, any of which may generate a warning message.  If a warning
 * has been posted, wait before setting a new message.
 */
void	warn2 (char *format, ...)
{
	va_list	ap;

	if (didbeep())
	{
		clrbeep();
		sleep(1);	/* patch: should use some type of hold */
	}
	va_start(ap, format);
	WarnVA (format, ap);
	va_end(ap);
}

/* <flist_tell>:
 * Unconditionally send a warning message, overwriting any pending text:
 */
static
void	FlistTellVA (char *format, va_list ap)
{
	clrbeep();
	FlistInfoVA (format, ap);
	warn_flag = TRUE;
}

void	flist_tell (char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	FlistTellVA (format, ap);
	va_end(ap);
}

/* <flist_info>:
 * Display informational message at the bottom of the screen.  We expect no
 * more than 2 filespecs, usually abbreviated.  If a message is already
 * pending (cf: 'didbeep'), then 'dds_tell' will not update the summary line.
 *
 * Route all error- and informational-messages through this point so that we
 * can send them to a log-file.
 */
static
void	FlistInfoVA (char *format, va_list ap)
{
	char	bfr[(3*MAX_PATH) + CRT_COLS];

	vsprintf (bfr, format, ap);

	if (did_crt_init)
		dds_tell (bfr,-1);
	else
	{
		putraw (bfr);
		putraw ("\r\n");
	}
	flist_log ("%.132s", bfr);
}

void	flist_info (char *format, ...)
{
	va_list	ap;

	va_start(ap, format);
	FlistInfoVA(format, ap);
	va_end(ap);
}

/*
 * Call 'flist_tell', when the argument is a VMS descriptor-string:
 */
int	flist_tell_dsc (struct	dsc$descriptor_s *ds_)
{
	char	bfr[CRT_COLS];
	int	len = ds_->dsc$w_length;

	if (len > sizeof(bfr))	len = sizeof(bfr);
	strncpy (bfr, ds_->dsc$a_pointer, len);
	bfr[len] = EOS;
	flist_tell ("%s", bfr);
	return (EXIT_SUCCESS);
}

/*
 * Display a system message at the bottom of the screen.  We ignore success-
 * and info-messages.
 *
 * Return TRUE on success so that the caller can use this routine to test
 * completion as well.  We assume that legal VMS status codes are nonzero
 * unsigned integers.
 */
int	flist_sysmsg (unsigned status)
{
	char	buffer[CRT_COLS];

	if (!status || $VMS_STATUS_SUCCESS(status))
		return (TRUE);
	else
	{
		sysgetmsg (status, buffer, sizeof(buffer));
		warn2 ("%s", buffer);
		return (FALSE);
	}
}

/*
 * Display fatal-error message, exit:
 */
void	error (int status, char *msg_)
{
	auto	char	who[MAX_PATH];

	if (did_crt_init)
	{
		crt_quit (TRUE);	/* Make a clean exit		*/
		did_crt_init = FALSE;
	}

	whoami (who, 3);
	strcat (who, "-f-");
	if (msg_)	/* patch: We should send this to SYS$ERROR */
		printf ("\n%s%s\n", who, msg_);
	else
		perror (who);
	flist_quit (status ? status : EXIT_FAILURE);
}

/* <flist_help>:
 * Provide help for this program.  Route through this one point so we can
 * use a common-name for the program.
 */
void	flist_help (int curfile, char *key)
{
	char	bfr[CRT_COLS];
	int	len;

	strcpy (bfr, "FLIST ");
	if (key)
	{
		len = strlen (bfr);
		strncat (&bfr[len], key, sizeof(bfr)-len-1);
		bfr[sizeof(bfr)-1] = EOS;
	}
	crt_help (0, bfr);
}

/* <flist_log>:
 * Write the specified information to the LOG-file, if it is open:
 */
static
void	FlistLogVA (char *format, va_list ap)
{
	if (LogRAB != 0)
	{
		register int j,k,next;
		char	bfr[CRT_COLS+(2*MAX_PATH)],
			bfr2[CRT_COLS];

		sprintf (bfr2, "%*s", nesting_lvl, " ");
		vsprintf (bfr, format, ap);

		/*
		 * Split the buffer if it contains embedded newlines.  Ignore
		 * completely-null buffers.
		 */
		j = k = 0;
		while (bfr[j])
		{
			next = nesting_lvl;
			for (k = j; bfr[k] && bfr[k] != '\n'; k++)
				bfr2[next++] = bfr[k];
			rputr (LogRAB, bfr2, next);
			j = bfr[k] ? k + 1: k;
		}
	}
}

void	flist_log (char *format, ...)
{
	va_list	ap;
	va_start(ap, format);
	FlistLogVA (format, ap);
	va_end(ap);
}

/*
 * Make available the nesting-level:
 */
int	flist_nest (void)
{
	return (nesting_lvl);
}

/* <flist_opts>:
 * Process options for either the main entry, or for EDIT-directory (subset).
 * In the latter case, some options are unavailable because of a conflict of
 * resources which would result.  However, we generally inherit options from
 * the main-level.
 *
 * Return TRUE if an error is found.
 *
 * patch: should resolve the ability to do COMMAND, LOG options within levels.
 */
int	flist_opts (int argc, char **argv, DCLARG *arg_, int subset)
{
	char	msg[CRT_COLS + 8 * MAX_PATH];

	if (subset)	subset = SUBOPT; /* Number of entries to ignore	*/
	if (dclopt (msg, arg_, &opts[subset],
		sizeof(opts) - subset * sizeof(DCLOPT)))
	{
		if (subset)		warn ("%s", msg);
		else			error (0, msg);
		return (TRUE);
	}

	if (H_opt) {
		int	n;
		printf("FLIST -- File List -- built %s\n", __DATE__);
		printf("\nOptions:\n");
		for (n = 0; n < sizeof(opts) / sizeof(opts[0]); n++)
		{
			printf("\t/%s\n", opts[n].opt_name);
		}
		exit(EXIT_SUCCESS);
	}

	if (D_opt1)	D_opt = 1;
	if (D_opt2)	D_opt = 2;
	if (D_opt3)	D_opt = 3;
	if (D_opt4)	D_opt = 4;
	if (! subset)
	{
		if (Xfast)	A_opt = D_opt = M_opt = FALSE;
		AnyXAB = (!Xfast || A_opt || D_opt || M_opt || O_opt);
		D_mode = 0;	/* Normally don't show day-of-week	*/
	}

	/*
	 * If a date-selection (/AFTER or /BEFORE) option was made, find the
	 * side-effect flags 'dateflag[]'.
	 */
	dateflag[0] = dateflag[1] = 0;
	if (isOkDate(&datechek))
	{
		if (dateflag[1] = D_opt)
		{
		DCLARG	*d_ = arg_;
			while (d_ && ! dateflag[0])
			{
				strucpy (msg, d_->dcl_text);
				if (!strncmp (msg, "/AF", 3)
				||  !strncmp (msg, "/SI", 3))
					dateflag[0] = 1;
				else if (!strncmp (msg, "/BE", 3))
					dateflag[0] = -1;
				d_ = d_->dcl_next;
			}
		}
		else
			error (0, "Date-selection requires date-lookup");
	}

	if (! subset)
	{
		if (*LogFile)
		{
			if (!(LogRAB = ropen2 (LogFile, LogDft, "w")))
			{
				rerror();
				return (TRUE);
			}
			else if (argc > 0)
			{
				int	j;
				size_t	len = 0;

				msg[0]	= EOS;
				for (j = 0; j < argc; j++)
				{
					char	*parm = argv[j];
					len += (strlen(parm) + 1);
					if (len >= sizeof(msg) - 1)
						break;
					strcat (msg, " ");
					strcat (msg, parm);
				}
				flist_log ("!%s", msg);
			}
		}
		getraw_init(CmdFile,CmdDft);
		/* Init this first, in case of error!!	*/
	}

	return (FALSE);			/* no errors found	*/
}

/* <flist_lis>:
 * Open/write-to/close listing-file used for miscellaneous displays which do
 * not fit into either the status-line, or into the normal file-display list.
 *
 * If the operation is successful, return the actual-name for use by the caller
 * (i.e., to browse it).
 */
char	*flist_lis (char *format, ...)
{
	char	bfr[CRT_COLS+(2*MAX_PATH)];

	if (format)	/* If arguments given, open/write-to	*/
	{
		va_list ap;

		if (! LisRAB)
		{
			if (!(LisRAB = ropen (LisFile, "w")))
			{
				rerror();
				return (nullC);
			}
		}

		va_start(ap, format);
		FlistLogVA (format, ap);
		vsprintf (bfr, format, ap);
		va_end(ap);

		rputr (LisRAB, bfr, strlen(bfr));
	}
	else if (LisRAB)
	{
		rclose (LisRAB);
		LisRAB	= 0;
	}
	return (LisFile);
}
