/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: browse.c,v 1.22 1995/10/27 13:31:04 tom Exp $";
#endif

/*
 * Title:	BROWSE.c (BROWSE main program)
 * Author:	T. Dickey
 *		(This is a completely re-written version of a program
 *		titled BROWSE, which was written by L.Seeton, 06-Nov-1983).
 * Created:	16 Apr 1984
 * Last update:
 *		27 Oct 1995, release-version
 *		22 Oct 1995, DEC-C clean-compile
 *		28 May 1995, use stdarg instead of VARARGS hack.
 *		27 May 1995, prototyped
 *		18 Feb 1995	port to AXP (renamed 'alarm').
 *		24 Feb 1989	disable /COMMAND when invoked from FLIST.
 *				added LOGFILE/LOGARGS hooks to integrate with
 *				FLIST's /COMMAND and /LOG options.
 *		22 Aug 1988	Forgot to check for no file at all!
 *		07 Oct 1985	Added 80/132-column switch 'G'.
 *		17 Aug 1985	Corrected case of overstrike of text on space.
 *		15 Aug 1985	Permit value on /SQUEEZE
 *		29 Jul 1985	Interpret VT100 keypad.
 *		18 Jul 1985	Gave up on using 'rmscc' (file attributes) to
 *				set implicit value for /JOIN, since we have
 *				often files from tape w/o carriage-control.
 *		03 Jul 1985	Use 'scanint' instead of 'sscanf' to bypass bug
 *				in CC2.0
 *		15 Jun 1985	Added '&' in 'opts[]' declaration to make
 *				CC2.0 happy.
 *		27 Apr 1985	Broke out read-command with 'edtcmd' stuff.
 *		12 Apr 1985	Added 'gotraw' call to make buffer-input faster.
 *				Use VARARGS in 'warn'.
 *		10 Apr 1985	Added 'clrwarn()' call so that if we get an
 *				error (e.g., in /COMMAND) which does not normally
 *				halt operation, then FLIST won't have its
 *				refresh-disable set.
 *		06 Apr 1985	redid search-abort as CTRL/X-ast.  Added
 *				/COMMAND-option (given support in 'getraw').
 *		28 Mar 1985	added 'status' argument to 'error'.
 *		26 Mar 1985	RELEASE-2: v923
 *		20 Mar 1985	Changed to a conventional (VMS-style) help-display.
 *		14 Mar 1985	renamed the release-version of this to BROWSE
 *		02 Mar 1985	changed /OVER to 2-state toggle, with an
 *				argument which permits any state to be set.
 *				Permit highlighting of space overstruck by a
 *				nonnull/nonblank.
 *		18 Feb 1985	fixed 'offset[0]' latching; indexed files do
 *				not necessarily start with rfa=0.
 *		15 Feb 1985	alter /JOIN to look for trailing LF (not CR).
 *		13 Feb 1985	use 'calloc' for static buffers.  Moved crt-init
 *				so that if we can't look at file, then we won't
 *				clear the screen.
 *		12 Feb 1985	added 'erstat' call to check on record-errors.
 *				Added 'more_name', to restrict length of name
 *				in summary-line.
 *		02 Feb 1985	use scrolling-margins
 *		26 Jan 1985	cleanup use of sound_alarm in out-of-bounds scroll.
 *		20 Jan 1985	corrected for zero-length records in /SQUEEZE.
 *		19 Jan 1985	make repeat-count work with U,D,F,B commands.
 *				Added I- and J-commands, also /TRIM option.
 *		17 Jan 1985	make "_" command something we can use with the
 *				other commands.  Also, recognize CTRL/K, CTRL/W
 *				in the read-buffer routine.
 *		02 Jan 1985	Use 'rmscc' to set default on '/JOIN'.  Added
 *				'Z' as synonym for QUIT.
 *		26 Dec 1984	Changed call on 'dclwild'.
 *		04 Dec 1984	Added 'X', to abort search/skip.  Use 'putraw',
 *				'whoami'.  Added '/JOIN', '/SQUEEZE' options.
 *		06 Nov 1984	Re-wrote search-compare, using 'strvcmp'.
 *				This permits BROWSE to find overlapping search-
 *				targets (as well as fix a reset-bug).
 *		23 Oct 1984	(had stepped on trailing-spaces)
 *		21 Oct 1984	pad the record-size allocation
 *		20 Oct 1984	cleanup of '^' transition, record-length.
 *		15 Oct 1984	added 'snapshot', version
 *		13 Oct 1984	completed multi-line record coding.
 *		29 Sep 1984	began re-code to permit record to span line
 *		27 Sep 1984	added 'col_base' logic to fix ^L display bug.
 *				Corrected combination-display of FIND_?.
 *		21 Sep 1984	permit 3-state O-flag, added M-flag toggle
 *		20 Sep 1984	add logic to do space-independent search
 *		17 Sep 1984	added O-flag toggle
 *		15 Sep 1984	eliminated dependency on 'stderr'
 *		11 Sep 1984	corrected position of FIND_R marker, broke out
 *				'fgetr' code while setting up RMSIO package,
 *				use macro-form of 'toupper'.
 *		23 Aug 1984	Added numeric argument (for fractional L/R)
 *		25 Jul 1984	Added 'cmd_arg' to 'argvdcl()' call.  Show
 *				filename w/o pathname if current default.
 *		13 Jul 1984	Use DCLOPT for options-parsing
 *		10 Jul 1984	No ^C command.  Show filename in blurb.
 *		04 Jul 1984	Use 'argvdcl()' to better parse argv.
 *				Fixed bug in backward search.
 *		27 Jun 1984
 *		20 Jun 1984	Cannot mix Bitgraph with FLIST (main_incl)
 *		18 Jun 1984	completed crt modification
 *		31 May 1984	(TED) Began re-code to use 'crt.c' module
 *		29 May 1984	(TED) Optimized user-buf read.
 *		27 May 1984	(TED) Adjustments to "/" search.
 *		22 May 1984	(TED) Use 'getpad' name to combine with FLIST.
 *		12 May 1984	(TED) Further altered getraw, error-return.
 *		07 May 1984	(TED) Altered (using MAIN-define) this source
 *				to permit it to be used as a subroutine, or
 *				as standalone source.
 *		26 Apr 1984	(TED) trim lines before printing them.  Allow
 *				more than 1 file to be inspected.
 *		18 Apr 1984	(TED) altered search command to show progress
 *		17 Apr 1984	(TED) added help message, "o" option, search cmd.
 *		16 Apr 1984	(TED), added L/R scrolling, Bitgraph option,
 *				rewrote to use DEC library routines to obtain
 *				the actual terminal characteristics, and to do
 *				correct VT52/VT100 cursor movement.
 *
 * Notes:	BROWSE does both left/right and up/down scrolling.  The total
 *		number of lines on the CRT screen is assumed even, so that
 *		when the status line is deducted, the remaining part is odd.
 *		This logic is implicit in the up/down scrolling, which overlaps
 *		each marked section by one line (e.g., scrolling forward one
 *		screen keeps one line the same).
 */

#include	<stdio.h>	/* for 'sprintf()' */
#include	<signal.h>	/* for 'sleep()' */

#include	<stdlib.h>
#include	<stdarg.h>
#include	<string.h>

#include	"rmsio.h"
#include	<ctype.h>
#include	<stsdef.h>

#include	"bool.h"
#include	"cmdstk.h"
#include	"edtcmd.h"
#include	"getpad.h"
#include	"getraw.h"

#include	"dclarg.h"
#include	"dclopt.h"
#include	"freelist.h"
#include	"names.h"
#include	"dspc.h"
#include	"whoami.h"

#include	"strutils.h"

#ifdef main_incl
#include	"flist.h"
#else
#define	CTL(c)	(037 & c)
#include	"warning.h"
#endif

static	int	more_0_bg (short *lpp_, short *width_);
static	int	more_0_vt (short *lpp_, short *width_);
static	void	more_0_line (void);
static	void	more_0_mark (void);
static	void	more_0_page (void);
static	void	more_char (char c);
static	void	more_conv (void);
static	void	more_ctlc (char *ctl_);
static	void	more_down (int frac, int rpt);
static	void	more_file (void);
static	void	more_getr (void);
static	int	more_lastp (void);
static	int	more_limit (int val, int lo, int hi);
static	void	more_line (void);
static	int	more_move (int dy, int dx);
static	int	more_msg (char *c_, int last);
static	char*	more_name (int maxlen);
static	void	more_next (int new);
static	void	more_null (int inx);
static	int	more_r_buf (char *co_, char *m1_, char *delim_);
static	int	more_r_cmd (void);
static	int	more_read (int rec);
static	void	more_right (void);
static	int	more_round (int num, int d);
static	void	more_screen (int view_size, int dirflg);
static	void	more_seek (int next);
static	int	more_size (int over);
static	int	more_skip (char *find_);
static	void	more_this (void);

#ifdef DEBUG
static	void	more_show (char *format, ...);
#endif

/*
 * Local (static) data:
 */
#define	MAX_PAGE	99999	/* maximum value for half-screen index	*/
#define	MAXREC		999999	/* maximum record # this program reads	*/
#define	MAXBFR		513	/* size of input/output buffers		*/
#define	MAXVEC		1024	/* granularity of seek-mark array	*/

#define	MAXCOL		(colmax-7)	/* threshold used in ruler-mode	*/

	/* Index to 'offset[]' for next, current previous pages */
#define	TOP_NEXT	(now)
#define	TOP_THIS	(now-2)
#define	TOP_PREV	(now-4)

	/* Record # (0,1,2,...) at 'offset[]' position:	*/
#define	LINEMARK(n)	(HalfPage*(n))

	/* Derived line-number, from 'inpage':			*/
#define	INPAGE_LINE	(inpage + BaseLine)
#define	INFILE_LINE	(crt_top() + INPAGE_LINE)

	/* Mark-index of last half-screen:			*/
#define	END_MARK	((last_page >= 0) ? end_mark : MAX_PAGE)

	/* Mark-index of last full (even) page:			*/
#define	LAST_PAGE	((last_page >= 0) ? last_page : end_mark)

#define	FIND_L	1	/* if off-screen on left		*/
#define	FIND_R	2	/* if off-screen on right		*/
#define	FIND_IN	4	/* if any in-screen			*/
#define	FIND_XX	8	/* debug: show half-screen marks	*/

#define	STRVCMP	strvcmp(find_bfr, &i_bfr[inx], lstate_end-inx)

#define	BELL	CTL('G')
#define	RUBOUT	'\177'
#define	toshift(c)	(iscntrl(c) ? ((c) | 0100) : (c))

#define	MARK_WIDTH	13	/* Columns used for 'M_OPT'	*/

#define	SHOW_OFF(n)	(n),offset[n].rfa,offset[n].cra

static	RFILE	*fp;

typedef	struct	{
	unsigned rfa;	/* Record's file-address (direct 'ftell/fseek')	*/
	short	cra;	/* Character's record-address (maintained here)	*/
	short	rec;	/* Actual record number				*/
	} OFFSET;
static
OFFSET	*offset,	/* 'ftell' positions, for retyping		*/
	rstate;		/* State of 'more_read', to extract line	*/

			/* State of current page-characters		*/
static
int			/* Terminal characteristics:			*/
	lpp,		/* maximum lines/page				*/
	width,		/* screen width					*/
	width2,		/* 1/2, for scrolling left/right		*/
	did_crt_init = FALSE,

	M_option,	/* TRUE iff "/MARK" set originally		*/
	O_option,	/* TRUE iff "/OVER" set originally		*/

	/*
	 * Miscellaneous flags:
	 */
	user_cmd,	/* (main-case): most recent command read	*/
	user_arg,	/* " " " numeric argument, if given		*/
	end_flag,	/* TRUE if current page is last in file		*/

	find_flg,	/* Set by 'STRVCMP', to number of chars to show	*/
	findmark,	/* Set iff find_bfr in current line		*/
	mark_width,	/* Actual number of columns used if /MARK	*/
	multiline,	/* TRUE iff a multi-line record was found	*/

	FullPage,	/* number of text lines in screen		*/
	HalfPage,	/* number of text lines in half-screen		*/
	BaseLine,	/* Adjustment after half-screen scroll		*/

	now,		/* Index to 'offset[]' for current page 	*/
	end_mark,	/* Highest known index to 'offset[]'		*/
	last_page,	/* ...index of last full page			*/
	last_line,	/* ...last display-line number in file		*/
	maxvec,		/* Number of locations allocated in 'offset[]'	*/

	i_recl,		/* maximum record length of input		*/
	i_size,		/* ... corresponding buffer size		*/
	i_line,		/* Line number in file (for display/read)	*/

	rstate_len,	/* length of current input record		*/
	lstate_len,	/* length of portion to be displayed		*/
	lstate_1st,	/* ...and index to portion to display		*/
	lstate_end,	/* ...and index above last char			*/

	/*
	 * State of ruler-mode:
	 */
	ruler_mode,	/* TRUE if we display a scale in status line	*/
	ruler_min,	/* First column of present ruler-text		*/
	ruler_max,	/* Last column of present ruler-text		*/
	ruler_x,	/* cursor-position: x = column			*/
	ruler_y,	/* cursor-position: y = row			*/

	/*
	 * State of output display:
	 */
	colmin, colmax,	/* limits on columns to display			*/
	column,		/* column # within line buffer			*/
	colend,		/* highest column # in page			*/
	*endcol,	/* end-column for each display line		*/

	inpage,		/* line number, within page			*/
	onpage;		/* printing-char's read in current page		*/

static
DCLARG	*WhatIsIt;	/* => name of file to display			*/

static
char	*i_bfr,		/* = input record				*/
	*o_bfr,		/* = current text				*/
	*obfr_,		/* => o_bfr, current character			*/
	*sbfr_,		/* => find_bfr, current search-try		*/
	*find_bfr,
	*ruler_text,	/* Ruler text				*/
	*fatal_msg;	/* Set iff I/O error			*/

/*
 * Define option keywords and their flags:
 */
static
int	B_opt,		/* Init for Bitgraph terminal		*/
	D_opt,		/* debug (values 1,2 or logical)	*/
	J_opt,		/* Join records ending in CR		*/
	O_opt,		/* show \r, \b codes			*/
	M_opt,		/* show ftell marks at each line	*/
	S_opt,		/* limit blank lines to double-space	*/
	T_opt,		/* ignore trailing blanks in J-command	*/
	W_opt,		/* wide (132-column) mode		*/
	_FALSE	= FALSE,
	_MAYBE	= (-1);
static
char	CmdFile[MAX_PATH],
	CmdDft[] = "SYS$LOGIN:BROWSE.CMD";

#define	SZ(n)	&n,sizeof(n)
static
DCLOPT	opts[] = {
	{"bg",		&_FALSE,	0,	SZ(B_opt), 	1, 00001},
	{"command",	0,		CmdDft,	SZ(CmdFile),	1, 00200},
	{"debug",	&_FALSE,	0,	SZ(D_opt), 	1, 00002},
	{"join",	&_FALSE,	0,	SZ(J_opt), 	1, 00020},
	{"marks",	&_FALSE,	0,	SZ(M_opt), 	1, 00004},
	{"over",	&_MAYBE,	0,	SZ(O_opt), 	1, 00010},
	{"squeeze",	&_MAYBE,	0,	SZ(S_opt), 	1, 00040},
	{"trim",	&_FALSE,	0,	SZ(T_opt), 	1, 00100},
	{"wide",	&_FALSE,	0,	SZ(W_opt), 	1, 00001}
	};

#ifdef	main_incl
#define	LOGFILE(s)	flist_log s;
#define	LOGARGS(c,n)	logfile(c,n)
static void logfile(int cmd, int count);

main_incl
#else
#define	LOGFILE(s)
#define	LOGARGS(c,n)	n
main
#endif

(
int	argc,
char	**argv)
{
#ifdef	main_incl
	DCLARG	*dcl_	= argvdcl (argc, argv, "", 2);
#else
	DCLARG	*dcl_	= argvdcl (argc, argv, "", 0);
#endif

	DCLARG	*opt_;
	int	(*if_bg)(short *lpp, short *width) = more_0_vt;
	char	*c_,
		msg	[MAXBFR];

	WhatIsIt = 0;

	if (dclchk (dcl_, msg))
	{
		warn (msg);
		return;
	}
#ifdef	main_incl
	dcl_ = dcl_->dcl_next;	/* Skip over program-name token	*/
#endif

	if (dclopt (msg, dcl_, (DCLOPT *)&opts, sizeof(opts)))
	{
		warn (msg);
		return;
	}
#ifndef	main_incl
	getraw_init (CmdFile, CmdDft);
#endif

	if (B_opt)	if_bg	= more_0_bg;
	if (D_opt)	M_opt	= TRUE;
	M_option = M_opt;	/* Save initial "/MARK" */

	/*
	 * Restrict /OVER to legal values, saving initial value in 'O_option':
	 *	0 - no control-character interpretation
	 *	1 - show \r as ^M, \b as ^H
	 *	2 - show \r as <CR>, \b as <BS>
	 *	3 - show all control characters as '.'
	 */
	if (O_opt > 3)	O_opt = 3;
	O_option = O_opt;
	if (O_option < 0)
	{
	    O_option = 1;
	    O_opt = 0;
	}

	for (opt_ = dcl_; opt_; opt_ = opt_->dcl_next)
	{
		if (! isopt(opt_->dcl_text[0]))
		{
			if (! (WhatIsIt || dclwild (opt_)))
			    WhatIsIt = opt_;
			else
			{
			    warn ("You must specify a single file");
			    freelist (dcl_);
			    return;
			}
		}
	}

	fatal_msg  = calloc(1, CRT_COLS);

	if (!WhatIsIt) {
		warn ("No file specified");
		freelist (dcl_);
		return;
	}

	if ((fp = ropen (WhatIsIt->dcl_text, "r")))
	{
	CMDSTK	*oldcmd_ = cmdstk_init ();
#ifdef main_incl
	int	oldtop = crt_top(),
		oldend = crt_end();

	    crt_set (TRUE, 0);
	    crt_set (FALSE, 0);
#else
	    crt_init (if_bg);
	    edtcmd_init ();
	    did_crt_init = TRUE;
	    crt_margin (1, crt_lpp()-1);
#endif
	    lpp	= crt_lpp ();
	    multiline = FALSE;
	    more_0_mark ();

	    i_recl	= rsize(fp);
	    i_size	= i_recl + 4;

	    if ((J_opt > 0) || (S_opt >= 0)) i_size <<= 2;
    			/* Allow extra if merge */

	    i_bfr	= calloc(1, i_size);
	    obfr_	=
	    o_bfr	= calloc(1, CRT_COLS);
	    find_bfr	= calloc(1, CRT_COLS);
	    ruler_text	= calloc(1, CRT_COLS);
	    offset	= calloc(maxvec = MAXVEC, sizeof(OFFSET));
	    endcol	= calloc(crt_lpp(), sizeof(endcol[0]));

	    more_file ();
	    rclose (fp);

	    cfree (i_bfr);
	    cfree (o_bfr);
	    cfree (find_bfr);
	    cfree (ruler_text);
	    cfree (offset);
	    cfree (endcol);

#ifdef	main_incl
	    crt_set (TRUE,  oldtop);
	    crt_set (FALSE, oldend);
	    clrwarn ();		/* Reset FLIST's screen-latch */
#else
	    crt_quit (TRUE);
#endif
	    cmdstk_free (oldcmd_);
	}
	else
	{
	    erstat (fp, fatal_msg, CRT_COLS);
	    warn (fatal_msg);
	}
	cfree (fatal_msg);
	freelist (dcl_);
}

/* <name>:
 * Given a display-buffer limit, attempt to show as much as possible of the
 * VMS filename.
 */
static
char	*more_name (int maxlen)
{
	register char	*c_ = WhatIsIt->dcl_text;
	register int	len = strlen (c_);
	static	char	bfr[CRT_COLS];

	if (len <= maxlen)	return (c_);
#define	ADJ(x)	c_ += WhatIsIt->x; len -= WhatIsIt->x;\
	if (len <= maxlen) return (c_);

	ADJ(dcl$b_node);
	ADJ(dcl$b_dev);
	ADJ(dcl$b_dir);
	strncpy (bfr, c_, len-4);
	strcpy (&bfr[len-4], " ...");
	return (c_);
}

/* <file>:
 * Do all processing for a single file:
 */
static
void	more_file (void)
{
	int	j,
		forward	= TRUE;

	find_bfr[0] = EOS;

	FullPage  = lpp - 1;		/* Expect 23 lines, normal display */
	HalfPage  = FullPage / 2;	/* ...line #11 is middle of 0..22 */
	width2	  = width / 2;

	colmin	  = 0;
	TOP_NEXT  = -1;			/* First read will be '0'	*/
	multiline = FALSE;		/* Assume one line per record	*/

	i_line	  = 0;			/* ...line #0			*/
	last_page = -1;			/* If minus, haven't found EOF	*/
	last_line = MAXREC;		/* Allow a large number of rec's*/
	end_mark  = 0;
	end_flag  = FALSE;

	rstate.rec= 0;			/* ...record #0			*/
	rstate.cra= -1;			/* Force first record-read	*/
	rstate.rfa= 0;
	rstate_len = lstate_len = lstate_end = 0;
	lstate_1st = -1;

	ruler_mode = ruler_x = ruler_y = 0;
	ruler_min = ruler_max = -1;

	crt_set (FALSE, crt_top() + FullPage-1); /* Init the scroller	*/
	more_screen (FullPage, 0);

	while (TRUE)
	{
	int	gold	= FALSE;
	    do
	    {
		user_cmd = more_r_cmd();
		gold	= gold || (user_cmd == GOLDKEY);
	    }
	    while (user_cmd == GOLDKEY);

	    if (! isascii(user_cmd))
	    {
	    int	j;
	    struct {
		int	input,	normal,	shifted;
		} KEYPAD[] = {
			{padENTER,	'M',	'M'},
			{HELPKEY,	'?',	'?'},
			{padPF3,	'N',	'/'},
			{padPF4,	'P',	'\\'},
			{pad0,		'H',	'H'},
			{pad1,		'L',	'L'},
			{pad2,		'R',	'R'},
			{pad3,		'J',	'J'},
			{pad4,		pad4,	'E'},
			{pad5,		pad5,	'T'},
			{pad6,		'I',	'I'},
			{pad9,		'_',	'_'}
			};

		if (user_cmd == pad7)
		    user_cmd = forward ? 'F' : 'B';
		else if (user_cmd == pad8)
		    user_cmd = forward ? 'D' : 'U';
		else for (j = 0; j < SIZEOF(KEYPAD); j++)
		{
		    if (KEYPAD[j].input == user_cmd)
		    {
			user_cmd = gold ? KEYPAD[j].shifted : KEYPAD[j].normal;
			break;
		    }
		}
	    }

	    switch (user_cmd)
	    {
	    case pad4:	forward = TRUE;		break;
	    case pad5:	forward = FALSE;	break;

	    case 'Z':
	    case 'Q':			/* Quit				*/
		LOGARGS('Z',0);
		getraw_flush();
		return;

	    case padUP:
		if (more_move (-1, 0))	break;
	    case 'U':			/* Display previous 1/2 screen	*/
		LOGARGS('U', user_arg = max(1, user_arg));
		if (TOP_THIS > 0)
		    more_next (TOP_THIS - user_arg);
		else
		    sound_alarm ();
		break;

	    case ' ':			/* (to look like Unix-more)	*/
	    case 'F':			/* Scroll forward 1 screen	*/
	    	LOGARGS('F', user_arg = max(1, user_arg));
	    	more_down (2, user_arg);
		break;

	    case padDOWN:
		if (more_move(1,0))	break;
	    case 'D':			/* Display another 1/2 screen	*/
		LOGARGS('D', user_arg = max(1, user_arg));
		more_down (1, user_arg);
		break;

	    case 'T':			/* Return to top-of-file	*/
		LOGARGS('T',0);
		if (TOP_THIS > 0)
		    more_next (0);
		else if (!ruler_y)
		    sound_alarm ();
		ruler_y = 0;
		break;

	    case 'B':			/* Back up one page		*/
		LOGARGS('B', user_arg = max(1, user_arg));
		if (TOP_THIS > 0)
		    more_next (TOP_THIS - (2 * user_arg));
		else
		    sound_alarm();
		break;

	    case 'E':			/* Go to end of file		*/
		LOGARGS('E',0);
		if (last_page < 0)
		{
		    int	old = TOP_NEXT;
		    while (last_page < 0 && ! ctlx_tst()) more_skip(nullC);
		    more_seek (old);	/* fix scroll */
		}
		if ((j = more_lastp()) != TOP_THIS)
		    more_next (j);
		else
		    sound_alarm();
		ruler_y = min(last_line-1, crt_end());
		break;

		/* patch: user_arg not used! */
	    case '\\':			/* Search backward for a string	*/
	    case 'P':			/* ...search for prev occurrence*/

	    case '/':			/* Search for a string		*/
	    case 'N':			/* ...search for next occurrence */
		if (ispunct(user_cmd))
		{
		    find_bfr[0] = EOS;
		    more_r_buf (find_bfr, "Search for:", nullC);
		    LOGFILE(("  $ %c%s", user_cmd, find_bfr))
		    strvcpy (find_bfr, nullC);
		    if (*find_bfr)	cmdstk_put (find_bfr);
		}
#ifdef	main_incl
		else {
			LOGARGS(user_cmd,user_arg);
		}
#endif/*main_incl*/
		if (find_bfr[0] == EOS)
		{
		    more_this ();	/* (clear old markers) */
		    sound_alarm ();
		}
		else if (user_cmd == 'P' && TOP_THIS <= 0)
		    sound_alarm();
		else
		{
		int	fwd = (user_cmd == '/' || user_cmd == 'N'),
			new, next,
			old = TOP_NEXT,
			got = FALSE;
		char	*c_ = find_bfr;

		    /*
		     * If first, start on current page.
		     */
		    switch (user_cmd)
		    {
		    case '/':
			more_seek (max(0,TOP_THIS));
			break;
		    case '\\':
			more_seek (new = TOP_NEXT-1);
			break;
		    case 'P':
			more_seek (new = TOP_THIS-1);
		    }

		    while (! (end_flag || ctlx_tst()))
		    {
			if (got = more_skip (find_bfr))
			    break;
			if (! fwd)
			{
			    if (--new >= 0)
				more_seek (new);
			    else
				break;
			}
		    }

		    /*
		     * If found, get pointer to top of half-page containing
		     * string, scroll to that screen.  The index 'now' is set
		     * to the mark at the bottom of the half-screen containing
		     * the string, if 'N', else top.
		     */
		    if (got)
		    {
			new = TOP_NEXT;
			if (user_cmd == 'N')	new--;
			more_seek (old); /* fix scroll */
			next = new;
				/* Prefer full-screen scrolling: */
			if ((old-1) == next)	next = old - 2;
			if ((old-3) == next)	next = old - 4;
				/* Restrict to legal limits: */
			next = max(0, min(next, more_lastp()));
#ifdef	DEBUG
			more_show ("(...)got:%d old:%d new:%d =>%d",
					got, old, new, next);
#endif
			more_next (next);
		    }
		    else
		    {
			TOP_NEXT = old;
			more_this ();	/* refresh old	*/
			sound_alarm ();
		    }
		}
		break;

	    case '^':			/* Toggle debug-mode		*/
		LOGARGS('^',0);
		if (!D_opt && !M_opt)
		{
		    M_opt = 1;
		    more_0_mark ();
		    more_this ();
		}
#ifdef	DEBUG
		D_opt = ! D_opt;
#endif
		break;

	    case 'O':			/* Toggle overlay mode		*/
		LOGARGS('O', user_arg);
		if (user_arg <= 3)	/* (maximum code permitted)	*/
		{
		    if (user_arg)
			O_opt = user_arg;
		    else
		    {
			if (O_opt)	O_opt = 0;
			else		O_opt = O_option;
		    }
		    more_this ();
		}
		else
		    sound_alarm();
		break;

	    case 'M':			/* Toggle mark-display mode	*/
		LOGARGS('M', user_arg);
		if (M_option || M_opt)
		{
		    if (M_opt == 0)		M_opt = 1; /* mark   */
		    else if (M_opt == 1)	M_opt = 2; /* length */
		    else			M_opt = 0; /* off    */
		    more_0_mark ();
		    more_this ();
		}
		else
		    sound_alarm ();
		break;

	    case 'K':			/* Make a snapshot of screen	*/
		LOGARGS('K',0);
		snapshot ();
		break;

	    case 'V':			/* Display current version	*/
		LOGARGS('V',0);
		more_msg ("BROWSE (v951027)", TRUE);
		sleep (3);
		break;

	    case 'G':			/* Toggle 80/132 columns	*/
		LOGARGS('G',0);
		{
		    int width, length;
		    termsize (FALSE, &width, &length);
		    if (width <= 80)	width = 132;
		    else		width = 80;
		    if (! termsize (TRUE, &width, &length))	break;
		}
	    case 'W':			/* Refresh current window	*/
		crt_clear ();
		more_this ();
		break;

	    case 'H':			/* scroll left to home-column	*/
		LOGARGS('H', user_arg = max(0, user_arg-1));
		if (colmin != user_arg && more_size(O_opt) > width)
		{
		    colmin = user_arg;
		    ruler_x = 0;
		    more_this ();
		}
		else if (ruler_x)	/* Force ruler-position if we	*/
		    ruler_x = 0;	/* ...are alreay in that screen	*/
		else if (ruler_mode)	/* Try to move up one row	*/
		{
		    if (!more_move (-1, 0))
		    {
			if (TOP_THIS > 0)	more_next (TOP_THIS-1);
			else			sound_alarm();
		    }
		}
		else
		    sound_alarm();
		break;

	    case 'I':			/* tab right (ruler only)	*/
		LOGARGS('I', j = max(0, user_arg-1));
		user_arg = 0;		/* kill the repeat-factor	*/
		if (ruler_mode)
		{
		    if (!more_move (0, (ruler_x | 7) + 1 + (8 * j) - ruler_x))
			more_right();
		}
		else
		    sound_alarm();
		break;

	    case 'J':			/* tab to end-column		*/
		LOGARGS('J', user_arg);
		if (ruler_mode)
		{
		    j = endcol[ruler_y - crt_top()];
		    j = max(0, j-1);
		    if (ruler_x == j)	/* repeated-action scrolls down	*/
		    {
			if (!more_move (1, 0))
			    more_down(1, user_arg = max(1, user_arg));
			user_arg = 0;
		    }
		    j = endcol[ruler_y - crt_top()];
		    j = max(0, j-1);
		    if (!more_move (0, j - ruler_x))
			more_right();
		}
		else if (colend < colmin || colend > colmax-1
		||	(colmin && colend < colmin + width2))
		{
		    colmin = width2 * ((colend - width2 - 1) / width2);
		    if (colmin < 0)	colmin = 0;
		    more_this();	/* Scroll to screen containing end
					   of longest record in display	*/
		}
		else
		    sound_alarm ();		/* No action performed		*/
		break;

	    case padLEFT:
		if (more_move(0, -1))	break;
		user_arg = -user_arg;
	    case 'L':			/* scroll left (1/2) screen	*/
		if (user_arg == 0) user_arg = width2;
		LOGARGS('L', user_arg);
		if (colmin > 0)
		{
		    colmin -= user_arg;
		    if (colmin < 0)
		    {
			colmin = 0;	/* Limit shift	*/
			sound_alarm ();	/* ...but yell	*/
		    }
		    more_this ();
		}
		else
		    sound_alarm();
		break;

	    case padRIGHT:
		if (more_move (0, 1))	break;
	    case 'R':			/* Scroll right (1/2) screen	*/
		LOGARGS('R', user_arg);
		more_right();
		break;

	    case '_':		/* Display a scale in the prompt-line */
		LOGARGS('_',0);
		ruler_mode = ! ruler_mode;
		break;

	    case '?':
		LOGARGS('?',0);
		crt_help (0, "BROWSE");
		break;

	    default:
		sound_alarm();
	    }
	}
}

/* <0_mark>:
 * Initialize display-width, based on terminal size and on use of '/MARK'.
 */
static
void	more_0_mark (void)
{
	width	= crt_width ();
	if (M_opt)
	{
		width -= (mark_width = MARK_WIDTH);
		if (multiline)
		{
#define	MULT_WIDTH	8
			width -= MULT_WIDTH;
			mark_width += MULT_WIDTH;
		}
	}
	width2 = width / 2;
}

/* <0_line>:
 * Re-init flags for current line (or record).
 */
static
void	more_0_line(void)
{
	obfr_	 = o_bfr; *obfr_ = EOS;
	findmark = 0;
	column   = 0;
}

/* <0_page>:
 * Re-init flags for current page.
 */
static
void	more_0_page(void)
{
	onpage	= inpage = 0;
	colmax	= colmin + width;
	more_0_line();
}

/* <move>:
 * Move the cursor in the specified direction.
 */
static
int	more_move (int dy, int dx)
{
	register
	int	rpt	= max(1, user_arg),
		top	= crt_top(),
		end	= crt_end();

	if (ruler_mode)
	{
	    user_arg = 0;
	    if (dx *= rpt)
	    {
		ruler_x += dx;
		ruler_x = more_limit(ruler_x, 0, more_size(O_opt));
		if (ruler_x < colmin)
		    user_arg = ruler_x - (colmin + 1);
		else if (ruler_x > MAXCOL)
		    user_arg = ruler_x - MAXCOL;
		user_arg = width2 * more_round (user_arg, width2);
	    }
	    else if (dy *= rpt)
	    {
		ruler_y += dy;
		ruler_y = more_limit (ruler_y, 0, last_line-1);
		if (ruler_y < top)
			user_arg = top - ruler_y;
		else if (ruler_y > end)
			user_arg = ruler_y - end;
		user_arg = more_round (user_arg, HalfPage);
	    }
	    return (user_arg == 0);
	}
	return (FALSE);
}

/* <limit>:
 * Limit a value to a specified range, sounding alarm if it falls out.
 */
static
int	more_limit (int val, int lo, int hi)
{
	if (val < lo)
	    sound_alarm(),	val = lo;
	else if (val > hi)
	    sound_alarm(),	val = hi;
	return (val);
}

/* <round>:
 * Given a number 'num', and the granularity 'd', return the closest multiple
 * of 'd'.
 */
static
int	more_round (int num, int d)
{
	if (num >= 0)
	    return ((num + d - 1) / d);
	else
	    return (-more_round(-num, d));
}

/* <lastp>:
 * Return the last seek-mark index which we may use to initiate a screen.
 */
static
int	more_lastp (void)
{
	return (max(0, LAST_PAGE));
}

/* <size>:
 * Return the maximum number of characters wide, given a value of /OVER:
 */
static
int	more_size (int over)
{
	int	j = i_recl;

	if (over == 0)		j <<= 3;
	else if (over <= 2)	j <<= 1;
	return ((j | 7) + 1);	/* Allow at least one tab stop	*/
}

/* <right>:
 * Shift screen right by either 1/2 screen, or by the number of columns
 * specified in 'user_arg'.
 */
static
void	more_right(void)
{
	int	j,
		endcol	= more_size(O_opt);

	if (user_arg == 0)	user_arg = width2;
	if (endcol > width)
	{
	    if ((j = more_limit (colmin + user_arg, 0, endcol)) != colmin)
	    {
		colmin = j;
		more_this ();
	    }
	}
	else
	    sound_alarm();
}

/* <down>:
 * Combined forward-2, down-1 commands taking into account repeat-factor.
 * We permit an "X" to abort the forward-search.
 */
static
void	more_down (int frac, int rpt)
{
	register
	int	old	= TOP_NEXT,
		target	= TOP_NEXT + ((--rpt) * frac),
		actual;

	/*
	 * If we haven't reached the end of the file before, we must skip
	 * along, setting pointers first, until we either reach the end of
	 * the file, or we reach the desired target-mark.
	 */
	if ((last_page < 0) && (target > end_mark))
	{
	    while ((last_page < 0) && (target > end_mark) && ! ctlx_tst())
		more_skip(nullC);
	    more_seek (old);	/* fix crt-scroll */
	}

	actual = max(0, min(target, more_lastp()));

	if (actual != TOP_THIS)
	{
	    more_seek (actual);
	    more_screen ((frac == 1 && rpt == 0 && actual == target)
			? HalfPage : FullPage, 1);
	}
	else		/* Yell if I didn't go anywhere	*/
	    sound_alarm();
}

/* <skip>:
 * Read through current half-page.  A page is ended when
 *	(a) The number of lines for the display screen have been passed, or
 *	(b) An end-of-file is found.
 *
 * If the 'find_' argument is nonnull, test the character stream as it is
 * read.  If a match is found within the page, return true, otherwise false.
 *
 * Because no fseek is done before beginning to read the half-screen, this
 * will not search overlapping half-screens:
 *
 *	'/' search begins on a mark (screen top), reads up to (but ends
 *	    BEFORE) the mark ending a half-screen.  Backward searches must
 *	    also do a seek-to-mark, and consequently do not read down to the
 *	    mark either.
 *	'N' search begins AFTER the present mark (screen bottom) and reads
 *	    up to (including) the mark ending the page.
 *
 * Thus, on exit from a search, the value of 'now' will be that of the TOP of
 * the half-screen containing the search target, or the BOTTOM, depending on
 * the search state.
 */
static
int	more_skip(
	char	*find_)		/* => 'find_bfr', or null	*/
{
	int	inx,
		found	= FALSE;
	char	msgbuf[CRT_COLS];

	if (find_)
		sprintf (msgbuf, "%3.3d  Find: '%s'", TOP_NEXT, find_);
	else
		sprintf (msgbuf, "%3.3d  Skipping...", TOP_NEXT);
	more_msg (msgbuf, FALSE);

	for (more_0_page(); inpage < HalfPage;)
	{
		if (more_read(i_line) < 0)	/* End of file ? */
			break;
		else
		{
			onpage += lstate_len;
			inpage++;
			if (find_ && !found)
			{
				for (inx = lstate_1st; inx < lstate_end; inx++)
				{
					if (STRVCMP)
					{
						found = i_line + 1;
						break;
					}
				}
			}
		}
	}

	/*
	 * The present position should be one display-line above the next
	 * to be displayed:
	 */
end_of_page:
#ifdef	DEBUG
	more_show ("skip(end=%d): [%d:%d,%d] => %d  after  [?:%d,%d] => %d",
		i_line,
		SHOW_OFF(now), LINEMARK(now),
		rstate.rfa, lstate_1st, inpage+LINEMARK(now-1));
#endif
	return (found);
}

/* <line>:
 * Put the portion of the output line which lies within the current column
 * limits onto the screen.
 */
static
void	more_line (void)
{
	int	len;
	char	msg	[MAXBFR],
		bfr2	[MAXBFR],
		field_0	[4],	field_1	[8],	field_2	[8],	field_r_l[8],
		*c_;

	/*
	 * If a search target was found on the current line, but not within
	 * the column limits, set visible indicators on the closest end of the
	 * line to the target.
	 */
	if (findmark & FIND_L)
	{
		if (o_bfr[0] == EOS)
			strcpy (o_bfr, " ");
		crt_high (o_bfr, 1);
	}
	if (findmark & FIND_R)
	{
		c_	= strnull(o_bfr);
		len	= strlen(o_bfr);
		while (len++ < width)
			strcpy (c_++, " ");
		crt_high (&o_bfr[width-2], 1);
	}
	obfr_ = strnull (c_ = o_bfr);

	/*
	 * M-option: If set, display one of two formats.  The 'findmark'
	 *	pattern (3 cols) and the line-number (low 2 digits) are always
	 *	shown.  The other two fields depend on the value of M_opt:
	 *
	 *	(1) the ftell-mark, offset of line-within-record
	 *	(2) the record length, length of line-within-record
	 *
	 * If 'multiline' is set, we display both the record number and the
	 * line number.
	 */
	if (M_opt)
	{
	static	char	cent[]	= ".%02d",
			thou[]	= ".%04d";

		sprintf (field_0, "%c%c%c",
			(findmark & FIND_L)  ? '<' : ' ',
			(findmark & FIND_IN) ? '=' : ' ',
			(findmark & FIND_R)  ? '>' : ' ');

		field_2[0] = field_r_l[0] = EOS;

		if (lstate_1st)	/* Are we doing continuation lines?*/
		{
			sprintf (field_1, "%6s", " ");
			if (multiline)
			{
				sprintf (field_2, thou,
					(M_opt == 1	? lstate_1st
							: lstate_len));
				strcpy (field_r_l, "...");
			}
		}
		else
		{
			sprintf (field_1, "%6d",
				(M_opt == 1 ? rstate.rfa : rstate_len));
			if (multiline)
			{
				if (M_opt == 1 || (lstate_len == rstate_len))
					strcpy (field_2, ".....");
				else
					sprintf (field_2, thou, lstate_len);
				sprintf (field_r_l, cent, (rstate.rec % 100));
			}
		}
		sprintf (strnull(field_r_l), cent,
			((INFILE_LINE+1) % 100));
		sprintf (msg, "%s%s%s%s:%s",
			field_0, field_1, field_2, field_r_l, o_bfr);
		c_ = msg;
	}

	if(D_opt && ((inpage % HalfPage) == 0))	/* FIND_XX	*/
	{
		strcpy (bfr2, c_);
		c_ = bfr2;
		crt_high (c_, strlen(c_));
	}
	crt_text (c_, INPAGE_LINE, 0);

	inpage++;
}

/* <char>:
 * Route a character into the output-display buffer, highlighting overstrikes
 * or search-targets.
 *
 * Track the current column to avoid putting characters into the buffer unless
 * within the selected column limits.
 */
static
void	more_char(char c)
{
	int	on_screen;

	if (isprint(c))
		column++;

	if ((T_opt && isgraph(c))	/* /TRIM: all but space		*/
	|| (!T_opt && isprint(c)))	/* /NOTRIM: all printing chars	*/
	{
	    if (column > endcol[INPAGE_LINE])
		endcol[INPAGE_LINE] = column;
	}

	on_screen = (column == 0 || ((column > colmin) && (column < colmax)));

	if (on_screen)
	{
		int	oldc, newc;
		/*
		 * Buffer latest character.  If I am writing onto the end of
		 * the buffer, 'oldc' will be null.  However, if I have gotten
		 * a backspace or carriage return, I may be overstriking a
		 * character which isn't blank.
		 *
		 * Make merge-logic test to overstrike alphas over punctuation;
		 * highlight overstrikes when no search is active.
		 */
		if (isprint(c))
		{
			obfr_	= o_bfr + (column - colmin - 1);
			oldc	= toascii(*obfr_),
			newc	= c;
			if (isgraph(oldc))
			{
				if ((newc == ' ')
				||  (isalnum(oldc))
				||  (newc == '_') )
					newc = oldc;
			}
			if (find_flg)	newc |= 0200;
			*obfr_++ = newc; /* Buffer latest character */
			if (!oldc)
				*obfr_	 = EOS;
			else if (!*find_bfr && (c != ' ') && (oldc != ' '))
				obfr_[-1] |= 0200;
		}
		/* else, BS, CR or LF */
		onpage++;		/* Count anything I buffer */
	}

	/*
	 * When a search is matched, set up visible indication for the display:
	 * If the string is at least partly visible, highlight those characters. 
	 * If it is not visible at all, set a flag for use in 'more_line' to
	 * highlight the end of the line closest to the string.
	 */
	if (find_flg)		/* Have I completed a search ?	*/
	{
		if (on_screen)
			findmark |= FIND_IN;
		else if (column <= colmin)
			findmark |= FIND_L;/* out-of-sight on left	*/
		else
			findmark |= FIND_R;/* out-of-sight on right	*/
	}
}

/* <ctlc>:
 * Send a string of characters per input character, according to the setting
 * of the /OVER option:
 */
static
void	more_ctlc (
	char	*ctl_)
{
	if (O_opt == 3)		more_char('.');
	else
	{
	    while (*ctl_)	more_char(*ctl_++);
	}
}

/* <conv>:
 * Given an input record, translate control characters as per options.
 */
static
void	more_conv (void)
{
int	inx,
	old_ok	= 0,
	col_base,		/* column to set on CR (or embedded FF)  */
	on_end;			/* TRUE when on last character of record */
char	c,
	*ctl_,
	showctl[4];

	col_base = 0;
	find_flg = 0;
	endcol[INPAGE_LINE] = 0;
	for (inx = lstate_1st; inx < lstate_end; inx++)
	{
		old_ok	= max((find_flg-1),0);
		if (*find_bfr)
		{
			find_flg = STRVCMP;
			find_flg = max(find_flg, old_ok);
		}
		on_end	= (inx >= (lstate_end-1));
		switch (c = i_bfr[inx])
		{
		/*
		 * Rubout is considered a control character, but does not map
		 * in the usual fashion.
		 */
		case 0x7f:
		    if (O_opt >= 2)	more_ctlc("<DEL>");
		    else		more_ctlc("^?");
		    break;
		/*
		 * Newline resets the column to zero because it begins a new
		 * display-line.  It always ends the display line (because it
		 * is the only reasonable way to show it).
		 */
		case '\n':
		    break;
		/*
		 * Carriage-return resets the column to the beginning of the
		 * current overstrike-region (marked by 'col_base').
		 *
		 * To see better binary records (and simplify the seek-logic)
		 * show carriage control in '^' form if it does not end the
		 * record, but is instead embedded.
		 */
		case '\r':
		    if ((!O_opt) || on_end)
		    {
			column	= col_base;
			obfr_	= o_bfr + strlen (o_bfr);
			more_char (c);
			break;
		    }
		/*
		 * Convert tabs to spaces to simplify left/right scrolling.
		 */
		case '\t':
		    if (!O_opt)
		    {
			register int j = 1 + (column | 7);
			while (column < j)
			    more_char (' ');
			break;
		    }
		/*
		 * Account for backspace:
		 */
		case '\b':
		    if (!O_opt)
		    {
			if (column > 0) column --;
			more_char(c);
			break;
		    }
		/*
		 * Convert control characters to a pair, display iff within
		 * column bounds.
		 */
		default:
		    if (iscntrl(c))
		    {
			ctl_ = &showctl[2];
			*ctl_-- = EOS;
			*ctl_-- = c | 0100;
			*ctl_   = '^';
			if (O_opt == 2) switch (c)
			{
			case CTL('H'):	ctl_ = "<BS>";	break;
			case CTL('I'):	ctl_ = "<TAB>";	break;
			case CTL('K'):	ctl_ = "<VT>";	break;
			case CTL('L'):	ctl_ = "<FF>";	break;
			case CTL('M'):	ctl_ = "<CR>";	break;
			case '\033':	ctl_ = "<ESC>";	break;
			}
			more_ctlc (ctl_);

			if (c == '\r' || c == '\n' || c == '\f')
			    col_base = column;
		    }
		    else
			more_char(c);
		}
	}
	more_line ();
}

/* <screen>:
 * Read and display either a full screen (one line less than actual screen
 * size), or a half screen (on either top or bottom, with appropriate
 * pre-scrolling).
 */
static
void	more_null (int inx)
{
	crt_text ("", inx-crt_top(), 0);
}

static
void	more_screen (
	int	view_size,
	int	dirflg)
{
	void	(*ffunc)(int) = (dirflg > 0) ? more_null : 0;
	int	old_top = crt_top(),
		old_end = crt_end(),
		new_top = max(0, (old_top + 1 - view_size)),
		new_end	= i_line + view_size - 1;

	more_0_page();
#ifdef	DEBUG
	more_show ("(screen)(%d,%d) old(%d,%d), new(%d,%d) rec:%d",
		view_size, dirflg, old_top, old_end, new_top, new_end, i_line);
#endif

	if (view_size == HalfPage && dirflg > 0 && old_end > new_top)
	    memcpy (&endcol[0], &endcol[HalfPage], HalfPage*sizeof(endcol[0]));

	/*
	 * Note: 'crt_scroll' is given a null display routine to simplify
	 * the logic of this program.  Thus, the end-line of the screen is
	 * reset in forward 1/2-screen scrolling.  MAXREC is used in controlling
	 * the scrolling to coerce 'crt_scroll' to be able to show a partly
	 * blank screen.
	 */
	if (view_size == HalfPage)
	{
		crt_scroll (new_end+1, MAXREC, ffunc);
		BaseLine = HalfPage;
		view_size++;	/* half-pages overlap by one line	*/
	}
	/*
	 * The view-size is not changed from full-page to half-page in
	 * backward scrolling, even if only half a page can be displayed.
	 * The 'crt' module will simply not update the screen, even though
	 * this program will read it.
	 */
	else
	{
		int next = (dirflg > 0 ? new_end : i_line);
		BaseLine = 0;
		if (dirflg)
		{
#ifdef	DEBUG
			more_show ("(...)scroll to:%d", next);
#endif
			crt_scroll (next, MAXREC, 0);
#ifdef	DEBUG
			more_show ("(...)old(%d,%d) new(%d,%d), mark %d",
				old_top, old_end, crt_top(), crt_end(), now);
#endif
		}
#ifdef	DEBUG
		else
			more_show ("(...) no scrolling");
#endif
	}

	while (inpage < view_size)
	{
	    if (more_read(inpage) < 0)
	    {
		last_line = INFILE_LINE;
		break;
	    }
	    more_conv ();
	}
	while (inpage < view_size)	/* (If EOF, clear remainder)	*/
	{
	    crt_text ("", INPAGE_LINE, 0);
	    endcol[INPAGE_LINE] = 0;
	    inpage++;
	}

	/*
	 * Obtain maximum line-length within the current screen:
	 */
	for (colend = inpage = 0; inpage < FullPage; inpage++)
		colend = max(colend, endcol[inpage]);
}

/* <this>:
 * Refresh the current page.  Note that if the file is shorter than one page,
 * I must restrict the backup to the 0-mark.
 */
static
void	more_this (void)
{
	more_next (max(0,TOP_THIS));
}

/* <next>:
 * This routine is used to display a page when jumping around in the file,
 * refreshing the current page, etc.
 */
static
void	more_next (
	int	new)
{
	int	old = now;

#ifdef	DEBUG
	more_show ("(next)(%d), was %d", new, old);
#endif
	if (new < 0)	new = 0;
	more_seek (new);
	more_screen (FullPage, 1 + (new - old));
}

/* <seek>:
 * Re-position using 'fseek' so that the next record-read will begin at the
 * top of a half-page screen section.
 */
static
void	more_seek (
	int	next)		/* next index in 'offset[]' to use	*/
{
	if (next < 0 || next > end_mark)
	{
		char bfr[CRT_COLS];
		sprintf (bfr, "fseek index(%d) out-of-range (0:%d)",
			next, end_mark);
		error (0, bfr);
	}
#ifdef	DEBUG
	more_show ("(seek)  [%d:%d,%d] => %d,  was [%d:%d,%d] => %d",
		SHOW_OFF(next), LINEMARK(next),
		SHOW_OFF(now),  LINEMARK(now));
#endif
	rseek(fp, offset[now=next].rfa, 0);

	/*
	 * Force the next call on 'more_read' to return the line which I
	 * am selecting, by adjusting 'lstate_???'.
	 */
	if (offset[now].cra)	more_getr ();

	rstate.rec = offset[now].rec;
	lstate_1st = offset[now].cra - 1;
	lstate_len = 1;
	lstate_end = lstate_1st + 1;

	if (lstate_1st >= 0)	rstate.rec = rstate.rec + 1;

	i_line	 = LINEMARK(now);	/* Reset actual-line-number	*/
	end_flag = FALSE;
	now--;		/* first record-read will re-increment */
}

/* <read>:
 * Read a new line from the input file (extracting, as needed, from records).
 * Do all 'ftell' calls to obtain points to repeat or restart the sequential
 * reads.
 *
 * State:
 *	rstate_len <= length of current record
 *	lstate_len <= length of line to display
 *	lstate_1st <= index into 'i_bfr[]' of line to display.
 *	lstate_end <= index past end of line
 */
static
int	more_read (int rec)		/* Either 'i_line' or 'inpage'	*/
{
	int	j;

	more_0_line();

	lstate_1st += lstate_len;
	lstate_len = 0;
	if ((lstate_1st >= rstate_len) || (lstate_1st <= 0))
	{
		lstate_1st = 0;
		more_getr ();
	}

	/*
	 * Find the newline which ends the newest line, compute the line-length.
	 */
	if (rstate_len >= 0)
	{
		++i_line;		/* Found a new record		*/
		for (j = lstate_1st;
			(j < rstate_len) && (i_bfr[j] != '\n');
				j++, lstate_len++);
		if (j < rstate_len && i_bfr[j] == '\n')	lstate_len++;
		if (rstate_len != lstate_len && !multiline)
		{
			multiline = TRUE;
			more_0_mark ();
		}
	}
	else
		lstate_len = -1;	/* eof-length is negative	*/

	lstate_end = lstate_1st + lstate_len;

	/*
	 * If this line falls on a display mark (top,mid,bot), or an end of
	 * file occurred, latch the filemarks:
	 */
	if (!(rec % HalfPage) || end_flag)
	{
		if (++now >= maxvec)
		{
			maxvec += MAXVEC;
			offset = realloc (offset, maxvec * sizeof(OFFSET));
		}
		if (now > end_mark || end_mark == 0)
		{
			end_mark = now;
			offset[now].rec = rstate.rec - 1;
			offset[now].rfa = rstate.rfa;
			offset[now].cra = lstate_1st;
		}

#ifdef	DEBUG
		more_show ("read(%d): [%d:%d.%d] => %d",
			end_mark, SHOW_OFF(now), LINEMARK(now));
#endif
	}

	return (lstate_len);
}

/* <getr>:
 * Read a record from the input file, setting end-of-file flag, record-length,
 * and stripping parity.  If "/FEED", then read/append as long as CR ends the
 * input record.
 */
static
void	more_getr (void)
{
	int	j;
	int	first	= TRUE;
	int	join;
	int	skip;
	int	len	= 0;
	int	size	= i_size;
	unsigned *rfa_	= &rstate.rfa;
	static	unsigned dummy_rfa;
	char	*s_;

	rstate_len = 0;
	for (;;)
	{
	    len = rgetr (fp, &i_bfr[rstate_len], size, rfa_);
	    erstat (fp, fatal_msg, CRT_COLS);
	    if (first)		rstate_len = len;
	    else if (len >= 0)	rstate_len += len;
	    if (len < 0 || !((J_opt > 0) || (S_opt >= 0)))	break;

	    /*
	     * If JOIN option is set, and we have anything in the record
	     * buffer, test for a trailing LF.  If not found, we can merge
	     * successive records.
	     */
	    join = FALSE;
	    if (J_opt > 0)
	    {
		join = TRUE;
		if ((rstate_len > 0)
		&& (i_bfr[rstate_len-1] == '\n'))	join = FALSE;
	    }

	    /*
	     * If SQUEEZE option is set (and if we did not JOIN), test the
	     * record buffer to see if it is a simple sequence of
	     * space/carriage control.  If we find only blank line(s), force
	     * these down to a single blank line and set the continuation to
	     * permit an additional record to be appended.
	     *
	     * Normally, this will cause multiple blank lines to be
	     * compressed into a single blank line.  It will not do anything
	     * to records containing (for example) a sequence of LF's
	     * in addition to non-space characters.
	     */
	    if (!join && (S_opt >= 0) && (rstate_len >= 0))
	    {
		join = TRUE;
		skip = 1;
		i_bfr[rstate_len] = EOS;
		for (s_ = i_bfr; *s_; s_++)
		{
		    if (*s_ == '\n')
			skip++;
		    else if (! isspace(*s_))
		    {
			join = FALSE;
			break;
		    }
		}
		/*
		 * Limit the number of successively-skipped lines to the value
		 * given in '/SQUEEZE'.
		 */
		if (join)
		{
		    if (skip > S_opt)	skip = S_opt;
		    i_bfr[skip] = EOS;
		    while (skip > 0)	i_bfr[--skip] = '\n';
		    rstate_len = len = strlen(i_bfr);
		    size = i_size;
		}
	    }
	    /*
	     * If I couldn't join or squeeze, exit the loop.  Otherwise,
	     * set up for the next combining.
	     */
	    if (!join)			break;
	    first = FALSE;
	    size -= len;
	    rfa_ = &dummy_rfa;
	}

	if (rstate_len < 0)
	{
	    rstate_len = -1;
	    if (last_page < 0)
	    {
		last_page = onpage ? now-1 : now-2;
		if (last_page < 0)	last_page = 0;
	    }
#ifdef	DEBUG
	    more_show ("read(%d): EOF last-rec:%d, last-page:%d",
			now, rstate.rec, last_page);
#endif
	    end_flag = TRUE;
	}
	else
	{
	    rstate.rec = rstate.rec + 1;
	    for (j = 0; j < rstate_len; j++)
		i_bfr[j] = toascii(i_bfr[j]);
	}
}

/* <r_cmd>:
 * Prompt and read a single character for commands:
 */
static
int	more_r_cmd(void)
{
int	command,
	len,
	top	= crt_top(),	/* display-line-min (0..n-1)	*/
	end	= crt_end(),	/* display-line-max (0..n-1)	*/
	left	= colmin,	/* leftmost column (1..n)	*/
	right	= colmax-1,	/* rightmost column (1..n)	*/
	width	= crt_width();	/* display-width (<= CRT_COLS)	*/
char	*c_,
	msg	[CRT_COLS],
	format	[20],
	numbfr	[MAXBFR];

	end	= min(last_line-1, end);

	if (fatal_msg[0])
	    more_msg (fatal_msg, TRUE);
	else if (ruler_mode)
	{
	    int	ruler_col = (M_opt ? mark_width : 0);
	    ruler_x = min(MAXCOL, max(left, ruler_x));
	    ruler_y = min(end, max(top, ruler_y));
	    if (ruler_min != colmin || ruler_max != colmax)
	    {
	    	dspc_init (ruler_text, ruler_col, colmin+1);
		ruler_min = colmin;
		ruler_max = colmax;
	    }
	    dspc_move (ruler_text, ruler_col + ruler_x - colmin, ruler_y);
	}
	else
	{
	    sprintf (numbfr, "  Lines %d:%d  Cols %d:%d (%d)",
			top+1, end+1,
			colmin+1, right, colend);
	    if (M_opt)
	    {
#ifdef	RMSIO
		sprintf (strnull(numbfr), ":%d", i_recl);
#endif
		if (multiline)
			sprintf (strnull(numbfr), "  Records %d+",
				offset[TOP_THIS].rec+1);
	    }
#ifdef	DEBUG
	    if (D_opt)
		strcat (numbfr, " (debug)");
#endif

	/* Protect against obscenely-long filenames: */
	    sprintf (format, "%%.%ds %%s", len = width - (strlen(numbfr)+3));
	    sprintf (msg, format, more_name(len), numbfr);
	    more_msg (msg, TRUE);
	}

	/*
	 * Read the user's reply, translating alpha's to controls
	 * (making them equivalent).
	 */
	while (((command = getpad()) == '\r')
	||	(command == '\n'));
	if (isascii(command) && isalpha(command)) command = CTL(command);

	user_arg = 0;			/* Inactive if zero		*/
	if (isascii(command) && isdigit(command))
	{
		sprintf (numbfr, "%c", command);
		command = more_r_buf (numbfr, "Repeat:", "BDFIJHLMORU");
		if (command > 128)
			command += 128;	/* 128..255 => 256..getpad	*/
		else if (isalpha(command))
			command = CTL(command);
		c_ = scanint (numbfr, &user_arg);
		if (c_ == numbfr || c_[1])
			command = -1;	/* Force an error-return	*/
	}
	return toshift(command);
}

/* <r_buf>:
 * Read a string-buffer, for either search target, or for numeric argument.
 * If the latter, 'delim_' is set, and we must do special actions.
 */
static
int	more_r_buf (
	char	*co_,			/* => Output buffer	*/
	char	*m1_,			/* Prompt-message	*/
	char	*delim_)		/* If non-null, list of terminators */
{
	int	len	= strlen(m1_);
	char	prompt	[MAXBFR];

	strcpy (prompt, m1_);
	crt_high (prompt, len);
	strcpy (co_, edtcmd (*co_,
			delim_, (delim_ ? 8 : 0),
		    	lpp-1, len+2, "BROWSE", co_, prompt));
	len = strlen(co_)-1;
	return (0xff & co_[max(len,0)]);
}

/* <msg>:
 * Put some text on the last line of the screen (highlighted), and position
 * the cursor at the end of the string.
 */
static
int	more_msg (char *c_, int last)
{
	int	len;
	char	bfr	[MAXBFR];

	strcpy (bfr, c_);
	crt_high (bfr, len = strlen(bfr));
	crt_text (bfr, lpp-1, 1);
	if (last)
	{
		len += 2;
		crt_move (lpp, len);
	}
	return (len);
}

#ifdef	DEBUG
/* <show>:
 * debug: show "printf" on screen-end
 */
static
void	more_show (char *format, ...)
{
	va_list	ap;
	char	bfr[1024];

	if (D_opt)
	{
		sleep(1);
		va_start (ap, format);
		vsprintf (bfr, format, ap);
		va_end (ap);
		crt_text (bfr, lpp-1, 0);
		if (D_opt > 1)
			getpad();
		else
			sleep(3);
	}
}
#endif

/* <0_bg>:
 * Special terminal initialization.  These routines may be called via 'crt_init'
 * to adjust the screen on the basis of BROWSE options.
 */

/* Set Bitgraph terminal to native mode, home and clear. */
static
int	more_0_bg(short	*lpp_, short *width_)
{
	int	j;

	for (j = 0; j < 3; j++)
	{
		putraw ("\033:0e");
		putraw ("\033[H");
		putraw ("\033[J");
	}
	*lpp_	= 64;
	*width_	= 80;
	crt__NL0 (FALSE);
	return 0;
}

/* <0_vt>:
 */
static
int	more_0_vt (short *lpp_, short *width_)
{
	if (W_opt)
		*width_ = 132;
	return 0;
}

/*
 * Define routines to make this source standalone.
 */
#ifndef	main_incl
void	warn (char *format, ...)
{
	va_list	ap;
	char	msg[CRT_COLS+MAX_PATH];

	whoami (msg, 3);
	strcat (msg, "-w-");
	va_start(ap, format);
	vsprintf (strnull(msg), format, ap);
	va_end(ap);
	putraw (msg);
}

void	error (int status, char *s_)
{
	char	msg[80],
		who[80];

	whoami (who, 3);	strcat (who, "-f-");
	if (s_)
	{
		sprintf (msg, "%s-%.60s\n", who, s_);
		putraw (msg);
	}
	else
		perror (who);
	if (did_crt_init) crt_quit(FALSE);
	exit (status ? status : (STS$M_INHIB_MSG | STS$K_ERROR));
}
#endif

#ifdef	main_incl
static
void	logfile(int cmd, int count)
{
	if (count)
		LOGFILE(("  $ %d%c", count, cmd))
	else
		LOGFILE(("  $ %c", cmd))
}
#endif/*main_incl*/
