#ifndef NO_IDENT
static char *Id = "$Id: crt.c,v 1.7 1995/06/04 18:58:12 tom Exp $";
#endif

/*
 * Title:	crt.c - "FLIST" display routines
 * Author:	Thomas E. Dickey
 * Created:	03 May 1984
 * Last update:
 *		27 May 1995, prototypes
 *		07 Oct 1985, ensure that if term-size changed, LPP is ok.
 *		05 Oct 1985, added code to support 80/132-column switch.
 *		15 Jun 1985, reference functions as '(*func)' to make CC2.0 happy
 *		12 Jun 1985, reset keypad mode before entering help-routine.
 *		09 May 1985, moved on-system test to 'whoami'.
 *		04 Apr 1985, forgot length-arg in strncmp-SYS$
 *		28 Mar 1985, added 'status' argument to 'error'
 *		20 Mar 1985, added 'crt_help' interface routine.
 *		14 Mar 1985, added CTRL/C-ast to force crt-reset.  Also, verify
 *			     that screen-height is even, at least four lines.
 *		02 Feb 1985, added 'crt_margin' entry, cleaned up margin-usage
 *			     in 'crt_quit', 'crt_reset'.
 *		27 Jan 1985, added 'crt_qsgr' entry.
 *		11 Jan 1985, added 'crt_reset' routine.
 *		03 Dec 1984, changed 'crt__put' to 'putraw', omitted 'crt__flush'
 *			     (no more Unix-I/O).  This should avoid a bug in
 *			     the VMS terminal driver (see: 'getpass').
 *		04 Nov 1984, merged 'crt_test' into 'crt_text'.  Added logic
 *			     supporting 'to_eql[]', 'to_chg[]', which permits
 *			     'crt_text' to skip over gaps which are similar.
 *		01 Nov 1984, in 'crt_test', skip leading spaces when writing
 *			     text to a null line.
 *		30 Oct 1984, alter threshold for 'crt_scroll'
 *		17 Oct 1984, added 'crt_refresh', 'crt_x', 'crt_y' entries
 *		15 Sep 1984, added 'crt__put', 'crt__flush' entries
 *		24 Aug 1984, use 'crt.h' include
 *		04 Aug 1984, protect 'crt_text' by killing tabs, etc.
 *		27 Jun 1984
 *		28 May 1984, to chop into 'crt' and 'dds' modules
 *
 *	These routines perform screen display/refresh operations for FLIST
 *	and BROWSE.
 */

#include	<starlet.h>
#include	<lib$routines.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<descrip.h>
#include	<iodef.h>
#include	<ssdef.h>
#include	<string.h>
#include	<stsdef.h>
#include	<ttdef.h>

#include	"bool.h"
#include	"canopen.h"
#include	"names.h"
#include	"whoami.h"

#include	"strutils.h"

#include	"flist.h"	/* crt.h + error + warn */

extern	void	help (char *lib, char *prg, int maxcol);
/*
 * Valid, but no-longer-documented screen package interface
 */
extern	void scr$set_scroll (int lo, int hi);
extern	void scr$set_cursor (int y, int x);
extern	void scr$erase_page (void);
extern	void scr$erase_line (void);
extern	void scr$down_scroll (void);
extern	void scr$up_scroll (void);
extern	void lib$put_screen(struct dsc$descriptor_s *p, ...);
extern	void lib$screen_info (short *term_flags, short *term_type, short *width, short *lpp);

/*
 * VAX/VMS terminal-independent routines know about the ANSI "Set Graphics
 * rendition" codes.  Those available via the VMS run-time library are:
 * (VMS coding is additive)
 */
#define	sgrOFF		0
#define	sgrBOLD		1
#define	sgrREVERSE	2
#define	sgrBLINK	4
#define	sgrUNDERL	8

/*
 * Local (static) data:
 */

#define	FLAG_DEC	1	/* Terminal is a DEC crt		*/
#define	FLAG_ANSI	2	/* Terminal is ANSI 3.64-compatible	*/

static	char	*bigvec	= nullC; /* => crtvec[0] */

/* Make the display memory public so it can be examined: */
char	*crtvec[CRT_LINES];	/* prior-state of screen		*/

static
BYTE	sgrvec[CRT_LINES];	/* last code used via 'crt_text'	*/

static
short	term_flags,
	term_type,		/* terminal-type (see 'dcdef.h')	*/
	width,			/* Current logical-width of screen	*/
	v_width,		/* Maximum width reserved in 'crtvec[]'	*/
	lpp,			/* Current logical-length of screen	*/
	v_lpp;			/* Maximum length reserved in 'crtvec[]'*/
#define	lpp1	(lpp-1)
#define	lpp2	(lpp-2)

static
int	top_line,		/* index number of first line displayed	*/
	end_line,		/* " " " last line in screen		*/
	top_margin,		/* [1..lpp] top scrolling margin	*/
	end_margin,		/* [1..lpp] bottom scrolling margin	*/
	NLflag,			/* TRUE iff '\n'-adjust needed		*/
	lasty, lastx;		/* Last cursor position, for filtering	*/

static	int	crt__lpp0 (int length);
static	void	crt_ctl_c_ast (short *ttchan_);

/* <crt_ctl_c_ast>:
 * CTRL/C AST routine.  Reset terminal characteristics, cancel pending I/O.
 */
static
void	crt_ctl_c_ast (short *ttchan_)
{
	unsigned status;

	crt_quit (FALSE);
	crt_move (lpp-1, 1);
	status = sys$cancel(*ttchan_);
	if (! $VMS_STATUS_SUCCESS(status)) lib$stop(status);
}

/* <crt_init>:
 * Initialize the screen and this module:
 */
void	crt_init (int (*func)(short *lpp_, short *width_))
{
	register int	j;
	static	short	ttchan;
	unsigned status;
	static	$DESCRIPTOR(terminal,"SYS$COMMAND");

	/* Assign channel to terminal	*/
	status = sys$assign(&terminal, &ttchan, 0, 0);
	if (! $VMS_STATUS_SUCCESS(status)) lib$stop(status);

	/* Enable control-C AST	*/
	status = sys$qiow (0,
			ttchan,
			IO$_SETMODE | IO$M_CTRLCAST, 0, 0, 0,
			&crt_ctl_c_ast, &ttchan, 0, 0, 0, 0);
	if (! $VMS_STATUS_SUCCESS(status)) error(status, 0);

	/* Get screen size, terminal type: */
	lib$screen_info (&term_flags, &term_type, &width, &lpp);
	if ((term_flags & FLAG_DEC) == 0)
		error (0, "Terminal must be VT52, VT100 or ANSI\n");

	lpp = crt__lpp0 (lpp);

	crt__NL0 (TRUE);	/* Normally must absorb 1st '\n'	*/
	if (func)	(*func)(&lpp, &width);

	top_line = end_line = 0;
	lpp	= min(CRT_LINES-1, lpp);
	width	= min(CRT_COLS-1,  width);
	v_width	= max(width, 132);

	lastx	= lasty	= 0;

	top_margin = 1;
	end_margin = lpp;

	bigvec = calloc(lpp, 1+v_width);
	for (j = 0; j <= lpp; j++)
	{
		sgrvec[j] = 0;
		crtvec[j] = &bigvec[(1+v_width) * j];
	}
	crt_clear();
}

/*
 * "TT$_" codes for terminals encode specific types in the second hex digit.
 * To return "generic" type, mask this digit first:
 */
#define	GEN_TYPE ((-1) << 4)

int	crt_vt100 (void)   { return ((term_type & GEN_TYPE) == TT$_VT100); }
int	crt_vt52 (void)	   { return ((term_type & GEN_TYPE) == TT$_VT52); }
int	crt_ansi (void)	   { return (term_flags & FLAG_ANSI); }

int	crt_top (void)	   { return (top_line);	}
int	crt_end (void)	   { return (end_line);	}

int	crt_lpp (void)	   { return (lpp);	}
int	crt_width (void)   { return (width);	}

int	crt_topm (void)	   { return (top_margin); }
int	crt_endm (void)	   { return (end_margin); }

int	crt_x (void)	   { return (lastx);	}
int	crt_y (void)	   { return (lasty);	}

int	crt_qsgr(int inx)  { return (sgrvec[inx]); }

/* <crt_margin>:
 * If this is an ANSI terminal, we may set scrolling margins.  Both FLIST
 * and BROWSE use the special case of a status line at the end of the page.
 */
void	crt_margin (int lo, int hi)
{
	if (crt_ansi())
		scr$set_scroll (top_margin = lo, end_margin = hi);
	else
		top_margin = 1, end_margin = lpp;
}

/* <crt_set>:
 * Set top_line/end_line w/o using global variables:
 */
int	crt_set (int top, int val)
{
	if (top)	top_line = val;
	else		end_line = val;
	return (val);
}

/* <crt_quit>:
 * On exit, clear the path-mark line, to set the cursor there.  We also
 * reset the scrolling margins to their default value (without keeping this
 * in our state) so that if this is called from a FLIST-SPAWN command, then
 * we can use 'crt_reset' to restore our scrolling margins.
 *
 * The 'erase' option causes the last line of the screen to be cleared.
 * If the last input was a newline, then VMS will echo this (scrolling up
 * one line on exit).  If not (e.g., CTRL/Z), then the program will exit
 * without scrolling.
 */
void	crt_quit(int erase)
{
	putraw ("\033>");		/* Exit alternate keypad mode */
	scr$set_scroll (1, lpp);
	if (erase)			/* Make a clean exit to command line */
		crt__ED (lpp, 1);
}

/* <crt_clear>:
 * Force screen contents to be cleared.  VT100 info-call does a clear-screen
 * for VT100, but not for VT52.
 */
void	crt_clear(void)
{
	register int	j;

	crt_reset ();
	crt__ED (1,1);
	for (j = 0; j < lpp; *crtvec[j++] = EOS);
}

/* <crt_reset>:
 * Reset scrolling margins and graphics-rendition for ANSI terminals (which
 * may be left in funny modes by other programs).  This code is called on
 * entry to FLIST/BROWSE, and also on return from spawned subprocesses.
 */
void	crt_reset (void)
{
	int	new_w, new_l,
		adj_l, j;

	lastx = lasty = 999;	/* We don't really know where cursor is !! */
	termsize (0, &new_w, &new_l);	/* Interrogate current screen size */
	new_w	= min(v_width, new_w);
	new_l	= crt__lpp0 (min(new_l, CRT_LINES));
	if (new_w != width || new_l != lpp)
	{
		if (new_w < width)
			for (j = 0; j < lpp; crtvec[j++][new_w] = EOS);

		adj_l	= new_l - lpp;
		if (new_l > v_lpp)
		{
			bigvec = realloc(bigvec, new_l * (1+v_width));
			for (j = 0; j <= new_l; j++)
			{
				if (j >= lpp) sgrvec[j] = 0;
				crtvec[j] = &bigvec[(1+v_width) * j];
			}
		}
		for (j = lpp1; j < new_l; j++)
		{
			sgrvec[j] = 0;
			*crtvec[j] = EOS;
		}
		width	= new_w;
		lpp	= new_l;
		end_margin += adj_l;  end_margin = max(top_margin, end_margin);
		end_line   += adj_l;  end_line   = max(top_line, end_line);
	}

	crt_margin (top_margin, end_margin);	/* Scrolling margins	*/
	if (crt_ansi())
		putraw ("\033[0m\033[20l");	/* SGR, new line */
	if (crt_vt100())
		putraw ("\033[?4l");		/* smooth-scroll */
	putraw ("\033=");		/* Enter alternate Keypad mode */
}

/* <crt_refresh>:
 * Refresh the contents of the screen without requiring that the caller code
 * re-generate the data to drive the display.
 */
void	crt_refresh (void)
{
	register
	int	j,
		save_x	= lastx,	/* This is where we think cursor was */
		save_y	= lasty;
	char	sline[CRT_COLS];

	lastx = lasty = 999;	/* We don't really know where cursor is !! */
	crt_reset ();		/* Another process may have redefined TERM */
	crt__ED (1,1);		/* clear the physical screen	*/
	for (j = 0; j < lpp; j++)
	{
		strcpy (sline, crtvec[j]);
		*crtvec[j] = EOS;
		crt_text (sline, j, sgrvec[j]);
	}
	crt_move (save_y, save_x);
}

/* <crt_move>:
 * Set the cursor to the next position from which to manipulate text.
 * Note that the VMS screen routines do not (cannot really) do filtering
 * to reduce the number of cursor movement sequences emitted.  This
 * routine MUST be used for ALL cursor movement to permit it to do the
 * optimization.
 */
void	crt_move (
	int	y,			/* 1 <= y <= lpp	*/
	int	x)			/* 1 <= x <= width	*/
{
	x = max(1, min(width, x));
	y = max(1, min(lpp,   y));
	if (lasty != y || lastx != x)
	{
		register
		int	len,
			jumpx = lastx - x,
			jumpy = y - (lasty ? lasty : (y + 1)),
			crlfs = (NLflag + jumpy) << 1;
		char	sequence[20];

		/*
		 * The VMS-library routines are very dumb, not even
		 * defaulting parameters.  Use as my threshold an
		 * ANSI x3.64 cursor movement:
		 */
		if ((y >  end_margin && lasty >  end_margin)
		||  (y <= end_margin && lasty <= end_margin))
		{
			sprintf (sequence, "?[%d;%d?", y-1, x-1);
			len = strlen (sequence);
		}
		else	/* Disable CRLF if past scrolling margins */
			len = crlfs;

		if ((lasty == y) && (jumpx > 0) && (jumpx < len))
		{
			while (jumpx-- > 0)
				putraw ("\b");
		}
		else if ((1 == x) && (jumpy > 0) && (crlfs < len))
		{
			crt__NL0 (FALSE);
			while (jumpy-- > 0)
				putraw ("\r\n");
		}
		else
			scr$set_cursor (y, x);
	}
	lasty = y;
	lastx = x;
}

/* <crt__lpp0>:
 * We need at least four lines on the screen to run BROWSE.  For the same
 * reason, the screen-length must be even.
 */
static
int	crt__lpp0 (int length)
{
	if (length < 4)
		error (0, "TERMINAL/PAGE must be at least four lines\n");
	if (length & 1)	length--;
	return (length);
}

/*
 * One peculiarity (which I work around) is that if I use '\n' to substitute
 * for cursor movement, the first '\n' after a LIB$SCREEN_INFO() is absorbed.
 * The flag 'NLflag' is intended to track this, so I can provide an extra
 * '\n'.
 *
 * This entry point is needed because the Bitgraph init (see BROWSE) cancels
 * the absorption.
 */
void	crt__NL0 (int flg)
{
	NLflag = flg;
}

/*
 * ANSI x3.64 defines erase functions:
 */
void	crt__ED (int y, int x)		/* Erase to end of Display	*/
{
	crt_move (y, x);
	scr$erase_page ();
}

void	crt__EL (int y, int x)		/* Erase to end of Line	*/
{
	crt_move (y, x);
	scr$erase_line ();
}

/* <crt_help>:
 * We assume that the calling program uses a HELP-library!!
 * This procedure interfaces to the HELP-dialog by clearing the screen,
 * calling HELP, and restoring the screen on exit.
 *
 * If 'library' is null, this program ultimately will be installed, and use the
 * standard VMS help-library.  However, if it is not currently run from the
 * system-directory, then we must assume this is a test-run, from a private
 * directory.  In this case, assume the help-library is in the same directory
 * as the program is run from.
 */
void	crt_help (char *library, char *program)
{
	char	pathname[MAX_PATH], *c_;

	if (! library)
	{
		if (! whoami (pathname, 4))
		{
			strcpy (strnull(pathname), "HELPLIB.HLB");
			if (canopen (library = pathname))
			{
				warn ("Cannot open %s", pathname);
				return;
			}
		}
	}

	crt_quit (FALSE);
	crt__ED (1,1);
	help (library, program, crt_width());
	crt_refresh();
}

/* <crt_text>:
 * Display the given text 'bfr', at the specified line (range 0..lpp-1),
 * with the 'mark' code for graphics-rendition (1=highlight, 0=normal).
 *
 * Use the 'crtvec[]' array to store the previous state of the given line,
 * to trade string copy/compare against screen I/O.
 *
 * 'crtvec[]' uses bit-7 as the flag for highlighting, allowing the display
 * driver to highlight part of any line.
 *
 * Do the blanking based on 'col_x' after all other changes have been made
 * to the line, to avoid an extra cursor movement.
 */
void	crt_text(
	char*	bfr,		/* "new" text to display		*/
	int	line,		/* index (0 to lpp-1) of line on screen	*/
	int	mode)		/* sgr-code if "highlighting"		*/
{
	static $DESCRIPTOR(DSC_line,"");

	register int	col_l;
	register int	col_r;
	register int	col_x;
	register char	*c1_	= bfr;
	register char	*c2_;
	register int	column = 0;
	register int	eql;
	register int	chg;

	char	bfr1	[CRT_COLS];
	char	bfr2	[CRT_COLS];
	static
	BYTE	to_chg[CRT_COLS],	/* distance til changed-column	*/
		to_eql[CRT_COLS];	/* distance til difference	*/

	line = max(0, min(lpp1, line));
	sgrvec[line] = mode;

	/*
	 * Copy the input string to a temporary buffer, suppressing nongraphic
	 * characters, and then trimming trailing blanks.  Explicit test for
	 * space is used to avoid problems with highlighting.
	 */
	for (c1_ = bfr1, c2_ = bfr; *c2_; c2_++)
	{
		if (isprint(toascii(*c2_)))	*c1_++ = *c2_;
		else if (toascii(*c2_) == '\t')	*c1_++ = *c2_ + (' ' - '\t');
		if ((c1_-bfr1) >= (width-1))	break;
	}
	*c1_ = EOS;
	for (c1_ = bfr1 + strlen(bfr1) - 1;
		(c1_ > bfr1) && (*c1_ == ' '); *c1_-- = EOS);

	/*
	 * Set highlighting to default, if caller uses defaults:
	 */
	if (mode == 0)
		mode	= (line >= lpp1) ? sgrBOLD : sgrREVERSE;

	/*
	 * Do string comparison to determine the limits of difference between
	 * the latest line and the prior contents.  If the older text is longer
	 * than the newer, also set index 'col_x' to show the erase-point.
	 *
	 * If the new text is longer than the old, skip leading spaces in the
	 * special case that no difference has been encountered (e.g., writing
	 * a word in the right margin on a blank line).
	 */

	col_l = col_r = col_x = 0;
	for (c1_ = bfr1, c2_ = crtvec[line]; *c1_ && *c2_; c1_++, c2_++)
	{
		if (*c1_ != *c2_)
		{
			column = c1_ - bfr1 + 1;
			if (col_l)	col_r = column;
			else		col_l = column;
		}
	}

	if (*c2_)		/* Was the old line longer than new ?	*/
		col_x = c1_ - bfr1 + 1;
	else if (*c1_)		/* If new is longer, extend right-col	*/
	{
		if (col_l == 0)	/* If no difference yet, skip leading spaces */
		{
			while (*c1_ == ' ')	c1_++;
			col_l = c1_ - bfr1 + 1;
		}
		col_r = strlen(bfr1);
	}
	if (col_l && (col_r == 0))
		col_r = col_l;

	/*
	 * If we have differences, compute a distance-vector, so that if
	 * there are long gaps of similarity, then we can set up a jump.
	 */
	if (col_l | col_r)
	{
		int	oldlen = strlen(c2_ = crtvec[line]);

		/*
		 * Pad the old string out to the length of the new part,
		 * so that the difference loop below will work.  (The
		 * column-index ranges from 1 to the length of the string.)
		 */
		c1_ = bfr1 - 1;
		c2_--;
		while (oldlen < col_r)	c2_[++oldlen] = ' ';

		/*
		 * First pass: find the length of gaps in the difference,
		 * (to_chg) and the length of the difference-sections (to_eql).
		 *
		 * Example:
		 *	'xxx...yyy' - new
		 *	'xxx!!!yyy' - old
		 *	 321000321  - chg
		 *	 000321000  - eql
		 */
		for (column = col_r, chg = eql = 0; column >= col_l; column--)
		{
			if (c1_[column] == c2_[column])
			{
				to_chg[column] = ++chg;
				to_eql[column] = eql = 0;
			}
			else
			{
				to_chg[column] = chg = 0;
				to_eql[column] = ++eql;
			}
		}

		/*
		 * Second pass: consolidate difference-sections which are
		 * too close together to jump economically.
		 * (patch: should make JUMP bigger still if there is a change
		 * in graphic-rendition on the seams.)
		 */
#define	JUMP	10
		for (column = col_l; column <= col_r;)
		{
loop:			if ((eql = column + to_eql[column]) > col_r)
				break;
			if ((chg = to_chg[eql]) < JUMP)
			{
				chg += eql;
				to_eql[column] += to_chg[eql] + to_eql[chg];
				goto loop;
			}
			else
				column = eql + to_chg[eql];
		}

		/*
		 * Finally, update the actual screen, where it has changed.
		 */
		strcpy (crtvec[line], bfr1);	/* Update internal buffer */
		strcpy (bfr2, bfr1);		/* strip parity bit */
		{ register char *s = bfr2; while (*s) *s++ &= 0x7f; }
		while (col_l <= col_r)	/* Update partial highlighting */
		{
			int	inx	= col_l - 1,
				right	= to_eql[col_l] + inx,
				mark;

			while (col_l <= right)
			{
#define	MARK	ishigh(bfr1[inx])
				inx	= col_l - 1;
				DSC_line.dsc$a_pointer = &bfr2[inx];
				for (mark = MARK;
					(inx < right) && (mark == MARK);
						inx++);
				DSC_line.dsc$w_length  = inx - col_l + 1;
				crt_move (line+1, col_l);
				lastx	+= DSC_line.dsc$w_length;
				mark	= (mark ? mode : 0);
				lib$put_screen (&DSC_line, 0, 0, &mark);
				col_l	= inx + 1;
			}
			col_l	= right + 1;
			if (col_l < col_r)	col_l += to_chg[col_l];
		}
	}
	else if (col_x)		/* (entire line was blanked) */
		strcpy (crtvec[line], bfr1);

	if (col_x)	crt__EL (line+1, col_x);
}

/* <crt_high>:
 * Mark the indicated character(s) with the highlighting bit (assume all ANSI
 * devices do some form of graphic-rendition):
 */
void	crt_high (char *s_, int len)
{
	if (!crt_vt52())
	{
		while (len > 0)
		{
			if (!s_[--len])	break;
			s_[len] |= 0x80;
		}
	}
}

/* <crt_scroll>:
 * Scroll the screen up (down) to permit the index to lie within the screen.
 * If the amount to scroll is less than the screen size, do the scrolling by
 * single-line indexing.  This code works on VT52 and VT100(-compatible) by
 * using a prevailing set of scrolling margins.
 *
 * Return the resulting line in the screen (1..endscroll).
 */
static
void	crt__null(int inx)
{
	/*nothing*/
}

int	crt_scroll (int iline, int numlines, void (*func)(int))
{
	register
	int	dif, j;
	int	close	= end_margin - top_margin - 1;

	iline = min(numlines-1, max(0, iline));

	if (func == 0)	func = crt__null;

	if (iline < top_line)
	{
		if ((dif = (top_line - iline)) < close)
		{
			while (dif-- > 0)
			{
				crt_move (top_margin, 1);
				top_line--;
				end_line = min(top_line+lpp2, end_line);
				scr$down_scroll ();
				for (j = end_margin-1; j >= top_margin; j--)
					strcpy (crtvec[j], crtvec[j-1]);
				*crtvec[0]    = EOS;
				(*func)(top_line);
			}
		}
		else
		{
			top_line -= dif;
			end_line = min(top_line+lpp2, numlines-1);
			for (j = top_margin-1; j < end_margin; j++)
				(*func)(top_line+j);
		}
	}
	else if (iline > end_line)
	{
		if ((dif = (iline - end_line)) < close)
		{
			while (dif-- > 0)
			{
				top_line++, end_line++;
				crt_move (end_margin, 1);
				scr$up_scroll ();
				for (j = top_margin-1; j < end_margin-1; j++)
					strcpy (crtvec[j], crtvec[j+1]);
				*crtvec[end_margin-1] = EOS;
				(*func)(end_line);
			}
		}
		else
		{
			top_line += dif;
			end_line += dif;
			for (j = top_margin-1; j < end_margin; j++)
				(*func)(top_line+j);
		}
	}
	return (iline-top_line+1);
}
