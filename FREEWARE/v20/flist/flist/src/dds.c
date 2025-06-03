/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: dds.c,v 1.16 1995/10/26 23:48:56 tom Exp $";
#endif

/*
 * Title:	dds.c - "FLIST" display routines
 * Author:	T.E.Dickey
 * Created:	03 May 1984
 * Last update:
 *		26 Oct 1995, mods to make 'dds_while()' animated
 *		18 Mar 1995, prototyped
 *		18 Feb 1995, renamed 'beep' to avoid conflict with curses.
 *		23 Feb 1989, use 'flist_chdir()'
 *		25 Aug 1985, added 'dds_width' entrypoint.
 *		24 Aug 1985, added 'dds_add2' entrypoint.
 *		20 Aug 1985, call 'dirent_width' in 'dds_add'
 *		30 Jul 1985, make 'dds_pack' adjust fixed-point if it lies off
 *			     screen.
 *		16 Jul 1985, use 'dirdata_add' in 'dds_add'.
 *		03 Jul 1985, cleanup of 'filelist' usage.
 *		11 Jun 1985, added cli-argument to dds_spawn
 *		22 Apr 1985, 'dds_cmd' code is now done in 'edtcmd'.
 *		02 Feb 1985, do 'crt_quit' before spawning to reset scroll-margin
 *		10 Jan 1985, added call to 'crt_reset' to fix up after spawning.
 *		29 Dec 1984, added 'dds_inx1', 'dds_inx2'.  Highlight name if
 *			     line is selected.
 *		21 Dec 1984, use 'flist_sysmsg'
 *		10 Dec 1984, use 'dirent_chk2' instead of 'dirent_one'
 *		09 Sep 1984, use 'multi_quit' in 'dds_last'
 *		30 Aug 1984, added 'dds_last'
 *		29 Aug 1984, plugged hole in 'dds_index' (numfile==0)
 *		25 Aug 1984, cleanup buffer sizes
 *		14 Aug 1984, use 'dirent_ccol()'
 *		07 Aug 1984, cleanup of scrolling, declarations
 *		14 Jul 1984, added completion-routine to dds_spawn
 *		11 Jul 1984, added 'dds_hold' to spawn-process
 *		03 Jul 1984, changed 'conv_filent' to 'dirent_conv'
 *		19 Jun 1984
 *		29 May 1984, to chop into 'crt' and 'dds' modules
 *
 * Function:	These routines perform screen display/refresh operations for
 *		"FLIST".
 *
 * Entry:
 *		dds_add:	Add a new FILENT-block to 'filelist[]' (show it)
 *		dds_add2:	Add a new FILENT block at specific index
 *		dds_all:	Display all lines in the current screen
 *		dds_ast1:	Obtain completion status of spawned tasks
 *		dds_fast:	Returns index to top, middle or bottom of screen
 *		dds_hold:	Prompt for CR before continuing
 *		dds_index:	Set cursor to given 'filelist[]' entry
 *		dds_inx1:	Given 'filelist[]' index, return nth-file number
 *		dds_inx2:	Given nth-file number, return 'filelist[]' index
 *		dds_last:	Cleanup after command execution
 *		dds_line:	Display a given entry of 'filelist[]'
 *		dds_move:	Move to new index in 'filelist[]', may scroll
 *		dds_pack:	Pack deleted entries out of 'filelist[]'
 *		dds_scroll:	Do scrolling to given 'filelist[]' index
 *		dds_spawn:	Spawn an external process, refreshing screen
 *		dds_tell:	Update message/summary line (bottom)
 *		dds_while:	Show "Working..." messages
 *		dds_width:	Update display if (possible) column-width change
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<types.h>
#include	<time.h>
#include	<string.h>

#include	<lib$routines.h>
#include	<rms.h>
#include	<ssdef.h>
#include	<stsdef.h>
#include	<descrip.h>

#include	"flist.h"
#include	"canopen.h"
#include	"dds.h"
#include	"dirent.h"
#include	"dirdata.h"
#include	"dircmd.h"
#include	"getpad.h"

#include	"strutils.h"

import(filelist); import(numfiles); import(numdlets);
import(ccolumns); import(pcolumns); import(conv_list);

extern	int	multi_quit;	/* >0 iff multi-level quit in progress	*/

#define	TOP_END	top_line = crt_top(), end_line = crt_end(), lpp = crt_lpp()
#define	lpp2	(lpp-2)
#define	lpp1	(lpp-1)

/* <dds_inx1>:
 * Given an index into 'filelist[]', return the number in the range
 * [1..(numfiles-numdlets)] which shows its real position in the display
 * list.  This is used at the conclusion of each command to show the
 * current position in the display list.
 */
int	dds_inx1 (int ifile)
{
	register
	int	j,k;

	ifile++;		/* put into range [1..numfiles]	*/
	ifile = max(1,min(ifile,numfiles));
	j = k = ifile;

	if (numdlets > 0)
	{
		while (--k >= 0)
			if (DELETED(k))	j--;
	}
	return (j);
}

/* <dds_inx2>:
 * Given a position-index in the display list, return the corresponding
 * index to 'filelist[]'.  This is used in scrolling commands by index
 * number.
 */
int	dds_inx2 (int inx)
{
	register
	int	j = numfiles-numdlets;

	inx = max(1,min(inx,j));

	if (numdlets)
	{
		j = 0;
		while (j < inx)
		{
			if (DELETED(j))	inx++;
			j++;
		}
		j--;
	}
	else
		j = inx - 1;

	return (j);
}

/* <dds_line>:
 * Display the indicated file-index on the screen, at the indicated line
 * (numbered from 0).
 */
void	dds_line(int inx)
{
	int	TOP_END,
		line	= inx - top_line;
	char	bfr	[CRT_COLS];

	if (line >= 0 && line < lpp1)
	{
		dirent_conv (bfr, FK_(inx));
		if (dircmd_select(-2) == inx)
		{
			int	col = dirent_ccol()-1;

			bfr[col-1] = '*';	/* both marker		*/
			crt_high (bfr, col);	/* and highlighting	*/
		}
		crt_text (bfr, line, 0);
	}
}

/* <dds_scroll>:
 */
int	dds_scroll (int ifile)
{
	return (crt_scroll(ifile, numfiles, dds_line));
}

/* <dds_index>:
 * Set the cursor to the indicated file-index, updating the path-summary
 * as needed.  The current working-directory is always set to that of the
 * current file.  Return the actual constrained value of 'ifile'.
 */
int	dds_index (int ifile)
{
	int	iline;

	if ((ifile = min(numfiles-1, max(0, ifile))) >= 0)
	{
		iline = dds_scroll(ifile);
		dds_tell (nullC, ifile);
		flist_chdir (PATHOF(ifile));
		crt_move (iline, dirent_ccol());
	}
	return (ifile);
}

/* <dds_all>:
 * Display all lines in the current screen, beginning with the 'crt_top()'
 * index.  Use "-1" argument for 'top_set' to permit screen refresh,
 * and the -2 argument to do this while setting the top_line-marker to origin.
 */
void	dds_all(int top_set, int now_set)
{
	int	j, last, TOP_END;

	if (top_set >= 0)
		crt_set (TRUE, top_set);
	else
	{
		if (top_set == -2)	crt_set (TRUE, 0);
		crt_clear();
	}
	top_line = crt_top();
	end_line = crt_set (FALSE, min(numfiles-1, top_line + lpp2));

	last = end_line - top_line;
	for (j = 0; j <= last; j++)
		dds_line (j+top_line);

	/* Clear unused part of screen in case I have just shifted to
	   displaying a new directory. */
	for (j = last+1; j < lpp1; j++)
	{
		if (*crtvec[j])
		{
			crt__EL (j+1, 1);
			*crtvec[j] = '\0';
		}
	}
	dds_index (now_set);
}

/* <dds_tell>:
 * The last line of the screen is used for two purposes:
 *
 *	Display the current file's pathname, and location in directory-list.
 *	Display error messages.
 *
 * This routine is used to maintain both items.  The "set_beep" routine both
 * sounds an audible alarm, as well as sets a flag "didbeep" which is
 * cleared only after the user next types a character for command-input.
 */
void	dds_tell (char *msg_, int ifile)
{
	int	lpp	= crt_lpp(),
		width	= crt_width()-1;	/* nominally 79	*/
	static
	char	sFMT1[] = "%%-%d.%ds",
		sFMT2[]	= "Path: %%-%d.%ds %%3d of %%3d ";
	char	bfr[CRT_COLS+3],
		format[sizeof(sFMT2)+6];

	if (!didbeep())
	{
		if (msg_)
		{
			set_beep();
			sprintf (format, sFMT1, width, width);
			sprintf (bfr, format, msg_);
		}
		else
		{
			if (ifile >= 0)
			{
				sprintf (format, sFMT2, width-19, width-19);
				sprintf (bfr, format,
					PATHOF(ifile),
					dds_inx1(ifile), numfiles-numdlets);
			}
			else
			{
				sprintf (bfr, "Reading: %3d files", numfiles);
			}
		}
		if (msg_ == 0 && ifile < 0)
			dds_while(bfr);
		else
		{
			crt_high (bfr, strlen(bfr));
			crt_text (bfr, lpp1, 0);
		}
	}
}

#define	DDS_WORKING	14	/* leaves nominal 66 columns */

static	int	dds_working;	/* true iff we've displayed "Working..." */
static	char	fmt_while[]	= "%%-%d.%ds %%-10.10s";

/* <dds_while>:
 * Borrow the end of the status-line to show a short (10-char max) message
 * while executing a long process, such as spawn or sort.
 */
void	dds_while (char *msg_)
{
	static	int	toggle = -1;
	int	lpp	= crt_lpp(),
		width	= crt_width() - DDS_WORKING;
	static
	char	msgbfr[CRT_COLS];
	char	format[sizeof(fmt_while)+6];
	char	*message;

	static	time_t last;
	time_t	temp = time((time_t *)0);

	dds_working = TRUE;
	if (temp != last)
	{
		toggle = !toggle;
		last = temp;
	}
	if (toggle == 0)
		message = "...Working";
	else
		message = "Working...";

	sprintf (format, fmt_while, width, width);
	sprintf (msgbfr, format, msg_ ? msg_ : crtvec[lpp1], message);
	crt_high (msgbfr, strlen(msgbfr));
	crt_text (msgbfr, lpp1, 0);
	crt_move (lpp, 1);	/* Leave cursor in known position */
}

/* <dds_done>:
 * Repaint the message-line without the "Working..." text when a command is
 * completed.
 */
void	dds_done (void)
{
	if (dds_working)
	{
		int	lpp	= crt_lpp(),
			width	= crt_width() - DDS_WORKING;
		char	msgbfr[CRT_COLS];
		char	format[sizeof(fmt_while)+6];

		dds_working = FALSE;
		clrbeep();

		sprintf (format, fmt_while, width, width);
		sprintf (msgbfr, format, crtvec[lpp1], " ");
		dds_tell(msgbfr, -1);
	}
}

static	unsigned completion = 0;

/* <dds_ast1>:
 * This procedure is called on completion of a spawned subprocess.  It tests
 * the completion status for abnormality, printing an error message if so.
 */
void	dds_ast1 (void)
{
	flist_sysmsg (completion);
}

/* <dds_spawn>:
 * Spawn an external process, refreshing the screen as needed.  Spawning
 * a process in VMS takes some time, so we must put out a message to the
 * user (so he doesn't think the program died).  Then, some things (such
 * as EDIT) will totally alter the screen.  For this reason, a hook is
 * provided to refresh the screen when this process wakes up again.
 */
void	dds_spawn (
	char	*cli_,		/* null iff DCL (default)		*/
	int	ifile,
	char	*cmd_,		/* command (if null, simple SPAWN)	*/
	char	*msg_,
	int	nowait,		/* 1=nowait, 0=wait (til process done)	*/
	int	refresh)	/* 0=norefresh, 1=line, 2=screen	*/
{
	static	$DESCRIPTOR(DSC_line,"");
	static	$DESCRIPTOR(DSC_prompt,"% ");
	static	$DESCRIPTOR(DSC_cli,"");
	int	status,
		lpp	= crt_lpp(),
		flags	= (nowait ? 1 : 0);
	FILENT	*z = FK_(ifile);
	char	fullname[MAX_PATH];

	/*
	 * If a CLI is specified, verify that it is accessible.  The file-spec
	 * must be all in uppercase.  (Note: to run a VMS CLI, we must be
	 * installed.)
	 */
	if (cli_)
	{
		sprintf (fullname, "SYS$SYSTEM:%s.EXE;", cli_);
		strucpy (fullname, nullC);
		DSC_cli.dsc$a_pointer = fullname;
		DSC_cli.dsc$w_length  = strlen(DSC_cli.dsc$a_pointer);
		if (canopen(fullname))
		{
			warn ("CLI-%s not found", cli_);
			return;
		}
	}

	if (cmd_)
	{
		DSC_line.dsc$a_pointer = cmd_;
		DSC_line.dsc$w_length  = strlen(cmd_);
	}
	dds_while (msg_);
	if (refresh & 2)
		crt_quit (!cmd_);	/* I'll be gone a while	*/

	status = lib$spawn (
		(cmd_ ? &DSC_line : 0),
		0, 0, &flags,
		0, 0, &completion, 0,
		nowait ? dds_ast1  : 0, 0,
		cli_ ? &DSC_prompt : 0,
		cli_ ? &DSC_cli    : 0);

	if (status != SS$_NORMAL)
		warn ("Spawn failed: %X", status);
	else
	{
		dds_hold (FALSE);	/* Do conditional hold	*/
		if (refresh)
		{
			if (refresh & 1)
				dirent_chk2 (ifile);
			if (refresh & 2)
			{
				crt_reset ();
				dds_all (-1, ifile);
			}
		}
	}
	if (! nowait)
		dds_ast1 ();	/* Use completion routine to show message */
	dds_index (ifile);
}

/* <dds_hold>:
 * Prompt the user for a carriage-return before continuing.  This procedure is
 * called from 'dds_spawn' to conditionally (if "/hold" in effect) hold, or
 * from other commands (e.g., "difference") to perform an unconditional hold.
 *
 * The hold function is used only for commands which are expected to wholly
 * alter the screen.  A hold-point may be needed to trap system messages for
 * the user.
 *
 * patch: We may implement an intermediate level of "/HOLD" where we will
 *	force it if the completion-status is in error.
 */
void	dds_hold (int hold)
{
	int	got,
		ok	= $VMS_STATUS_SUCCESS(completion);

	if (hold || flist_hold())
	{
		crt_move (1,1);		/* Force it to known position	*/
		if (!ok)		/* Try not to obscure message!!	*/
		{
			crt_move (crt_lpp(),1);
			putraw ("\r\n\n");
		}
		else
			dds_tell (" ",-1);/* Make it clear first		*/
		clrbeep();		/* ...permit the next text:	*/
		dds_tell ("Hit RETURN to continue",-1);
		clrbeep();
		for (;;)
		{
			if ((got = toascii(getpad())) == '\n')
				break;
			else if (got == '\r')
				break;
		}
	}
}

/* <dds_fast>:
 * Compute & return index to top (-1), bottom (+1) or middle (0) of display,
 * ignoring possibly deleted files.
 */
int	dds_fast(int opt)
{
	int	TOP_END;

	switch (opt)
	{
	case DDS_U_S:	return (max(0, top_line - lpp1));
	case DDS_U_C:	return (top_line);
	case DDS_D_C:	return (end_line);
	case DDS_D_S:	return (min(numfiles-1, end_line+lpp1));
	default:	return ((top_line+end_line)/2);
	}
}

/* <dds_move>:
 * Do short-distance (one-line), or within screen cursor-movement, checking
 * limits:
 *	DDS_U_S	= top of prior screen
 *	DDS_U_C	= screen top
 *	DDS_U_1	= up one line
 *	DDS_0	= screen middle
 *	DDS_D_1	= down one line
 *	DDS_D_C	= screen bottom
 *	DDS_D_S	= top of next screen
 *
 * The movement is complicated if there are deleted files within the current
 * screen.  These are forbidden zones, since no operations may be performed
 * on them.
 *
 * If the scrolling is out of the current screen, the appropriate scrolling
 * action is first performed.  The returned value from this procedure can
 * be used with the call
 *	dds_line (ret);
 * to properly reposition the cursor.  Out-of-screen scrolling assumes that
 * the screen has already been packed.
 */
int	dds_move (int curfile, int opt)
{
	int	nxt, j, jm, jp, TOP_END;

	if (numfiles <= numdlets)
		return (0);		/* Ensure a limit to recursion */

	switch (opt)
	{
	case DDS_U_S:
		if (top_line)		/* Can I go upward ?		*/
			dds_scroll (top_line - lpp1);
		return (dds_move(curfile, DDS_U_C));
	case DDS_U_C:
		for (nxt = top_line; nxt <= end_line; nxt++)
			if (!DELETED(nxt))	return (nxt);
		return (dds_move(curfile, DDS_U_1));
	case DDS_U_1:
		for (nxt = curfile-1; nxt >= 0; nxt--)
			if (!DELETED(nxt))	return (nxt);
		return (dds_move(curfile-1, DDS_D_1));
	case  DDS_0:
		nxt = dds_fast(DDS_0);
		if (!DELETED(nxt))		return (nxt);
		for (j = 0;
			(jp = nxt+j) <= end_line && (jm = nxt-j) >= top_line;
				j++)
		{
			if (jp <= end_line && !DELETED(jp))	return (jp);
			if (jm >= top_line && !DELETED(jm))	return (jm);
		}
		return (dds_move(nxt, DDS_D_1));
	case  DDS_D_1:
		for (nxt = curfile+1; nxt < numfiles; nxt++)
			if (!DELETED(nxt))	return (nxt);
		return (dds_move (curfile+1, DDS_U_1));
	case  DDS_D_C:
		for (nxt = end_line; nxt >= top_line; nxt--)
			if (!DELETED(nxt))	return (nxt);
		return (dds_move (curfile, DDS_D_1));
	case DDS_D_S:
		if (end_line < numfiles-1)
		{
			dds_scroll (dds_fast(DDS_D_S));	/* Do down-scroll */
			return (dds_move (curfile, DDS_U_C));
		}
		else
			return (dds_move (curfile, DDS_D_C));
	}
}

/* <dds_pack>:
 * Scan the 'filelist[]' array for deleted files (usually outside the current
 * screen).  When found, compress them out of the array, returning TRUE (the
 * number of files compressed).  If no compression is done, return FALSE
 * (no files).
 */
int	dds_pack (
	int	*curfile_,	/* => current-file index, to adjust	*/
	int	inscreen)	/* != 0 if current screen can compress	*/
				/* <= 0 if we adjust fixed-point	*/
{
	int	j,
		k,
		oldtop	= crt_top(),
		oldbot	= crt_end(),
		select	= dircmd_select(-2),
		adj	= 0,		/* amount to adjust indices	*/
		to_trim	= 0,		/* # of blank lines on end of list */
		to_pack	= numdlets;	/* # of deleted files to scan	*/

	if (!numdlets)
		return (0);

	/*
	 * If any deleted files are at the end of the list, trim these first,
	 * since they involve no copying.  Note that this search will not go
	 * back before the current screen.
	 */
	for (j = numfiles-1; j >= oldtop; j--)
	{
		if (DELETED(j))
		{
			numdlets--;
			numfiles--;
		}
		else
			break;
	}
	crt_set (FALSE, min(crt_end(), numfiles-1));
	*curfile_ = min(*curfile_, numfiles-1);

	if (!(to_pack = numdlets))
		return (0);

	/*
	 * If I have gotten this far, the deleted-file entries may be in the
	 * list anywhere, but not clustered at the end of the list.  For
	 * normal operation, only those entries which lie off the current
	 * screen may be collected.  (Operations such as the sorting require
	 * that all entries be compressed; sorting doesn't work on null entries.
	 */

	if (inscreen == 0)	/* subtract the number of files in screen */
	{
	int	TOP_END;
		for (j = top_line; j <= end_line; j++)
			if (DELETED(j))	to_pack--;
	}

	if (to_pack <= 0)	return (FALSE);

	for (j = 0; j < numfiles; j++)
	{
		if (adj)
		{
			k = j - adj;
			if (j == *curfile_)	*curfile_ = k;
			if (j == crt_top())	crt_set (TRUE,  k);
			if (j == crt_end())	crt_set (FALSE, k);
		}
		else
			k = j;
		if ((inscreen == 0) && (j >= oldtop) && (j <= oldbot))
		{
			if (adj)
				filelist[k] = filelist[j];
		}
		else
		{
			if (DELETED(j))
			{
				adj++;
				if (j < select)	select--;
			}
			else if (adj)
				filelist[k] = filelist[j];
		}
	}

	numdlets -= adj;
	numfiles -= adj;

	if (select >= 0 && (inscreen <= 0))	dircmd_select (select);

	return (adj);			/* Return # of files packed out */
}

/* <dds_last>:
 * Cleanup after command execution by moving to a non-deleted position, and
 * packing entries which lie off the resulting screen.  Return TRUE if no
 * more files remain, necessitating a QUIT.  The current-file index is updated
 * to point to the proper file entry.
 */
int	dds_last (int *curfile_)
{
	if (multi_quit > 0)
	{
		multi_quit--;
		return (TRUE);
	}

	if (DELETED(*curfile_))
		*curfile_ = dds_move (*curfile_, DDS_D_1);
	if (numfiles > 0)
	{
		dds_pack (curfile_, FALSE);
		dds_index (*curfile_);
	}
	return (numfiles <= 0);
}

/* <dds_add>:
 * Add a FILENT entry to 'filelist[]', updating the display as required.
 * This code is used, for example, when an EDIT causes a new version to be
 * displayed in a location, and the old version must be moved.  If there
 * are any deleted-slots, put the entry there, otherwise put it at the end
 * of the list.
 */
int	dds_add (	/* returns resulting index into 'filelist[]'	*/
	FILENT	*z)	/* => structure to insert/append to 'filelist[]' */
{
	int	j,
		curfile = -1,
		TOP_END;

	for (j = 0; j < numfiles; j++)	/* Find the first deleted-slot */
	{
		if (DELETED(j))
		{
			curfile = j;
			numdlets--;	/* This slot is no longer deleted */
			break;
		}
	}
	if (curfile < 0)
		curfile = dirent_add() - 1;	/* create a new entry	*/

	filelist[curfile] = 0;
	dirdata_add (z, &filelist[curfile]);

	if (curfile >= top_line && curfile <= (top_line + lpp2))
		crt_set (FALSE, max(min(numfiles-1, top_line+lpp2), end_line));

	dds_width (z, curfile);			/* Latch column widths */
	return (curfile);
}

/* <dds_add2>:
 * Add a new FILENT block at an empty slot.  Display it.  This code is used,
 * for example, as a result of COPY.
 */
void	dds_add2 (FILENT *z, int curfile)
{
	dirdata_one (z, &filelist[curfile]);
	dds_width (z, curfile);
}

/* <dds_width>:
 * Update the display-column widths.  If we have a new maxima, may refresh the
 * entire display rather than simply the current line.
 */
void	dds_width (FILENT *z, int curfile)
{
	type_ccolumns	Ccolumns size_ccolumns;
	static	char	Pcolumns [SIZEOF(ccolumns)+1] = "ntvpfxu";
	register j;

	memcpy (Ccolumns, ccolumns, sizeof(ccolumns));
	if (dirent_width(z))
	{
		for (j = 0; j < SIZEOF(ccolumns); j++)
		{
			if (Ccolumns[j] != ccolumns[j])
			{
				if ((j <= 1 && !pcolumns[j == 1])
				||  (j == 2)
				||  strchr(conv_list, Pcolumns[j]))
				{
					dds_all (crt_top(), curfile);
					return;
				}
			}
		}
	}
	dds_line (curfile);
}
