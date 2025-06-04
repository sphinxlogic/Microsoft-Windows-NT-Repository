/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: flprot.c,v 1.15 1995/10/22 22:07:44 tom Exp $";
#endif

/*
 * Title:	flprot.c
 * Author:	Thomas E. Dickey
 * Created:	11 May 1984
 * Last update:
 *		22 Oct 1995, DEC-C treats chars in 128-255 range different from
 *			     VAX-C (in the latter, none are punctuation/alpha).
 *		18 Mar 1995, prototypes
 *		18 Feb 1995, port to AXP (renamed 'alarm')
 *		05 Oct 1985, added key-argument to 'flist_help'.
 *		24 Sep 1985, corrected threshold-test on 'prot_col'.
 *		04 Jul 1985, cleanup 'filelist' definition.
 *		15 May 1985, use '_toupper' macro, not routine
 *		05 May 1985, added history-flag
 *		22 Apr 1985, use 'edtcmd', rather than 'dircmd' for input,
 *			     do 'crt'-call directly for screen updates.
 *		14 Apr 1985, use 'flist_info' for log-routing
 *		08 Mar 1985, permit filespec as part of argument list.  If
 *			     this is not the current file, protection-code
 *			     must be explicit.
 *		21 Dec 1984, corrected error returns from 'setprot'.
 *		20 Dec 1984, use ^D, ^E for cursor movement.  Broke out 'isowner'.
 *		10 Dec 1984, use 'dirent_chk2' instead of 'dirent_one'.
 *			     Omit coercion to readonly since ACP does ok.
 *		20 Nov 1984, use 'setprot' instead of spawning
 *		19 Oct 1984, use 'cmdstk_chg' (broken from 'dircmd')
 *		17 Oct 1984, use 'dircmd_GET' instead of 'getpad'
 *		03 Sep 1984, use "import"
 *		27 Aug 1984, cleanup of buffer sizes
 *		14 Aug 1984, use 'dirent_ccol()'
 *		14 Jul 1984, forgot to test 'M_opt' before edit
 *		12 Jul 1984, corrected DCLARG usage, added help-call
 *		10 Jul 1984, re-coded for DCLARG-list
 *		22 May 1984
 *
 * Function:	This module supports an edit-function for the protection-
 *		code field displayed (last on each line) by "FLIST".  As
 *		each change to the original code is made, it is highlighted
 *		(and uppercased, for VT52, etc.) to show the user what he
 *		has changed.  On successful completion of the editing,
 *		a "SET PROTECTION" command is constructed, and saved for
 *		retrieval (if the procedure is entered as a "visible"
 *		command).
 *
 *		This function operates only on the current entry of the
 *		file-list.
 *
 * Commands:	left-arrow	- move left (with wrap-around)
 *		right-arrow	- move right (with wrap-around)
 *		up-arrow	- set the current bit
 *		down-arrow	- clear the current bit
 *		y		- select entire field (even if no change)
 *		n		- deselect field
 *		,		- Move to next group
 *		CTRL/U, CTRL/X	- abort edit
 *		ENTER, RETURN	- complete edit, submit command
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"cmdstk.h"
#include	"flist.h"
#include	"dirent.h"
#include	"getpad.h"
#include	"getprot.h"
#include	"dircmd.h"
#include	"edtcmd.h"
#include	"dirfind.h"
#include	"dds.h"

#include	"strutils.h"

extern	int	isowner(FILENT *z);

static	int	flprot_edit (char *set_, int curfile);
static	void	flprot_one(int, int *);	/* function via 'dirfind'	*/

import(filelist);
import(M_opt);

static	char *	savecmd = nullC;	/* Text of option-code to process */

tDIRCMD(flprot)
{
	FILENT	*z	= FK_(*curfile_);
	DCLARG	*name_	= 0,		/* => filename specification	*/
		*code_	= 0;		/* => protection-code		*/
	int	no_edit	= FALSE,	/* TRUE iff code is specified	*/
		flag;
	char	msg	[CRT_COLS],
		fullname[MAX_PATH];

	if (!M_opt)
	{
		warn ("EDIT-PROTECTION is not applicable");
		return;
	}
	dirent_glue (fullname, z);

	if (!isowner(z))	return;

	/*
	 * Analyze the argument list, if one was given.  It may fall into
	 * one of the following cases:
	 *
	 *	(a) No list: edit current file
	 *	(b) No code: name must be that of current file (edit it, if so).
	 *	(c) Code: apply this to the specified file(s).
	 */
	if (xdcl_)
	{
		while (xdcl_ = xdcl_->dcl_next)
		{
			if (isopt(xdcl_->dcl_text[0]))
				code_ = xdcl_;
			else
				name_ = xdcl_;
		}
		no_edit = (code_ != 0);
		if ((code_ == 0) && (name_ != 0))
		{
			if (strcmp(fullname, name_->dcl_text))
			{
				warn ("You must specify a protection code");
				return;
			}
		}
	}

	/*
	 * If no code given, enter cursor-only edit of the protection code.
	 * Note that if a field is unspecified, it is not altered.  When
	 * complete, save this command for retrieval.
	 */
	if (!savecmd)
		savecmd = calloc(1, CRT_COLS);
	if (no_edit)
		sprintf (savecmd, "PROTECT %s", code_->dcl_text);
	else
	{
		if (!flprot_edit (savecmd, *curfile_))
			return;
	}

	if (name_)
		dirfind (*curfile_, -1, name_, flprot_one, TRUE, FALSE);
	else
		flprot_one (*curfile_, &flag);

	if (!code_ && history)
		cmdstk_chg (savecmd);
}

/* <flprot_one>:
 * Process the protection code for a single file.
 */
static
void	flprot_one (int j, int *flag_)
{
	int	status;
	char	fullname[MAX_PATH];

	dirent_glue (fullname, FK_(j));
	flist_log ("%s %s", savecmd, fullname);
	if ((status = setprot (fullname, savecmd)) == -1)
	{
		warn2 ("Syntax error");
		*flag_ = FALSE;		/* Don't continue after first error */
	}
	else
		flist_sysmsg(status);
	dirent_chk2 (j);
}

/* <flprot_edit>:
 * If no arguments were supplied to "flprot", enter cursor-oriented edit
 * of the protection code.
 */

extern	int	prot_col;

#define	NEXT	if (++edit > 15) edit = 0
#define	RUBOUT	'\177'

typedef	struct	{
	char	SET,		/* Character-if-set	*/
		old;		/* Original character	*/
	int	BIT,		/* mask-bit (1=disable)	*/
		col;		/* Column of this entry	*/
	} PTBL;

static
PTBL	ptbl[16];		/* protection-state-bits	*/

static int
flprot_edit (
	char	*set_,
	int	curfile)
{
	FILENT	*z	= FK_(curfile);
	int	rline	= curfile - crt_top(),
		j, k, jk, edit, command, level,
		abort	= FALSE,
		complete= FALSE,
		cmdcol	= dirent_ccol(),
		select	[4];		/* Override-flags for code-selection */
	char	c, d, *c_, *d_,
		cmdbfr	[CRT_COLS];

	if (!M_opt
	||  (prot_col < 0)
	||  ((prot_col+19) > crt_width()) )
	{
		warn2 ("No protection-mask to edit");
		return (FALSE);
	}

	/*
	 * The protection-mask is the last field of the "FLIST" display
	 * line.  The display corresponds (with blanks between groups of
	 * four bits) to the bits in the protection mask, left-to-right
	 * to LSB-to-MSB.  Use dead reckoning to build an index table:
	 */
	strcpy (cmdbfr, crtvec[rline]);
	for (j = 0, k = prot_col; j < 16; j++, k++)
	{
		ptbl[j].old = cmdbfr[k-1];
		ptbl[j].col = k;
		switch (j % 4)
		{
		case 0:	ptbl[j].SET = 'R';	break;
		case 1:	ptbl[j].SET = 'W';	break;
		case 2:	ptbl[j].SET = 'E';	break;
		case 3:	ptbl[j].SET = 'D';	k++;	/* skip a space */
		}
		ptbl[j].BIT = 1 << j;
	}
	for (level = 0; level < 4; select[level++] = FALSE);

	/*
	 * Now that I know what the codes are now, do the edit:
	 */
	edit = 4;			/* Start on OWNER field	*/

	while (!abort && !complete)
	{
		switch (level = edit/4)
		{
		case 0:	d_ = "SYS";	break;
		case 1:	d_ = "OWN";	break;
		case 2:	d_ = "GRP";	break;
		case 3: d_ = "WLD";
		}
		for (c_ = &cmdbfr[cmdcol-1]; *d_; *c_++ = tohigh(*d_++))
			/*EMPTY*/;

		crt_text (cmdbfr, rline, 0);
		crt_move (rline+1, k = ptbl[edit].col);
		j = level * 4;		/* 0, 4, 8, 12 = field index	*/
		k--;			/* index into cmdbfr		*/
		c_ = &cmdbfr[k];

		switch (command = edtcmd_get())
		{
		case padENTER:
		case '\r':
			complete = TRUE;
			break;
		case CTL('C'):
		case CTL('U'):
		case CTL('X'):
		case CTL('Y'):
			abort = TRUE;
			sound_alarm ();
			break;
		case CTL('D'):
		case padLEFT:
			if (--edit < 0) edit = 15;	break;
		case CTL('F'):
		case padRIGHT:
			NEXT;				break;
		case padUP:
			if (ptbl[edit].old != _tolower(ptbl[edit].SET))
				*c_ = tohigh(ptbl[edit].SET);
			else
				*c_ = ptbl[edit].old;
			if (select[level])	*c_ = tohigh(*c_);
			NEXT;
			break;
		case padDOWN:
			if (ptbl[edit].old != '-')
				*c_ = tohigh('.');
			else
				*c_ = '-';
			if (select[level])	*c_ = tohigh(*c_);
			NEXT;
			break;
		case '?':
		case HELPKEY:
			flist_help (curfile, "PROTECTION");
			break;
		case 'y':
		case 'Y':	/* do group select */
			select[level] = TRUE;
			for (jk = j; jk < j+4; jk++)
			{
				c_ = &cmdbfr[ptbl[jk].col-1];
				if (isalpha(*c_))
					*c_ = tohigh(_toupper(*c_));
				else
					*c_ = tohigh('.');
			}
			break;
		case 'n':
		case 'N':	/* do group deselect */
			select[level] = FALSE;
			for (jk = j; jk < j+4; jk++)
			{
				c_ = &cmdbfr[ptbl[jk].col-1];
				c  = ptbl[jk].old;
				if ((isalpha(*c_) && isalpha(c))
				||  (ispunct(*c_) && ispunct(c)) )
					*c_ = c;
			}
			break;
		case ',':
		case padCOMMA:
			if (++level >= 4)	level = 0;
			edit = level * 4;
			break;
		case RUBOUT:	/* patch: would be nice to backtrack */
		default:
			sound_alarm ();
		}
	}

	/*
	 * Now, if normal exit, go back over the list of protection bits
	 * to make up the actual composite SET PROTECTION command.
	 */
	if (!abort)
	{
		int	colon, comma, didselect = FALSE;

		for (j = 0; j < 16; j++)
		{
			level = j/4;
			c = ptbl[j].old;
			d = toascii(cmdbfr[ptbl[j].col-1]); /* cf: 'tohigh()' */
			if ((isalpha(d) && ispunct(c))
			||  (ispunct(d) && isalpha(c)) )
				select[level] = TRUE;
			if (select[level])
				didselect = TRUE;
		}

		if (!didselect)
		{
			flist_info ("No changes made");
			abort = TRUE;
			goto cleanup;
		}

		strcpy (set_, "PROTECT=(");	/* Abbreviate to match FLIST */
		set_ = strnull(set_);
		comma = FALSE;

		for (j = 0; j < 16; j++)
		{
			if (select[level = j/4])
			{
				if ((j % 4) == 0)
				{
					switch (level)
					{
					case 0:	c_ = "SYSTEM";	break;
					case 1:	c_ = "OWNER";	break;
					case 2:	c_ = "GROUP";	break;
					case 3:	c_ = "WORLD";
					}
					if (comma) strcat (set_, ",");
					strcat (set_, c_);
					colon = FALSE;
					comma = TRUE;
				}
				set_ = strnull(set_);
				c_   = &cmdbfr[ptbl[j].col-1];
				if (isalpha(c = toascii(*c_)))
				{
					if (!colon)
					{
						strcat (set_, ":");
						set_ = strnull(set_);
						colon = TRUE;
					}
					sprintf (set_, "%c", _toupper(c));
				}
			}
		}
		strcat (set_, ") ");
	}

cleanup:
	dds_line (curfile);
	return (!abort);
}
