/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: dirarg.c,v 1.8 1995/10/21 18:17:55 tom Exp $";
#endif

/*
 * Title:	dirarg.c
 * Author:	T.E.Dickey
 * Created:	23 Jun 1984
 * Last update:
 *		19 Feb 1995, prototypes
 *		11 Oct 1988, corrected place where "/o" alone breaks code.
 *		21 Sep 1985, translate SET/SHOW to '/', '?' prefix
 *		18 Sep 1985, process non-DCL commands here also
 *		09 Sep 1985, must trim trailing '.' from FNAME-field
 *		05 Jul 1985, corrected release of dcl-list if error.
 *		03 Jul 1985, cleanup of 'filelist' definition
 *		14 May 1985, added log-file entry for implicit arguments
 *		15 Dec 1984, added 'cpy_dft' argument to 'dclarg'.
 *		17 Oct 1984, omit extra space after 0th token
 *		25 Aug 1984, cleanup of buffer limits
 *		03 Aug 1984, added call to 'sysgetsym'
 *		25 Jul 1984, added 'cmd_arg' parameter to 'dclarg()'
 *		15 Jul 1984, use PATHOF()
 *		13 Jul 1984, protect 'warn' by "%s" format
 *		04 Jul 1984
 *
 * Function:	This module performs syntax checking on the command text
 *		which is received by 'dircmd' for FLIST.  The command-name
 *		may be translated if it is a symbol (to permit FLIST to
 *		associate it with one of its "known" commands).
 *
 *		The argument portion of the string is checked.  The prefix
 *		"/o" and the command name are already parsed off, and leading
 *		blanks removed.  This module is called before executing the
 *		interpretation routines.
 *
 * Parameters:	dcl__ =>  pointer to load with linked-list address of parsed
 *			  DCL tokens.  The 0th entry of this list will be the
 *			  resulting command name.  Subsequent entries are the
 *			  options and filespecs, parsed by 'dclarg'.  All
 *			  '.dcl_text' items are uppercased by the parsed.
 *			  The '?' or '/' prefix is stripped from the command
 *			  verb (the first entry).
 *		curfile	= index to nominal current file index in 'filelist[]'.
 *			  It is used directly when the "/" substitution is
 *			  performed.
 *		argstr	= buffer containing the argument text.  It may be a
 *			  null pointer, in which case the string "" or "/"
 *			  is assumed, according to whether 'minarg' is 0 or 1.
 *		command	= string defining full name of CLI command to execute.
 *			  Since it may be a logical symbol, this procedure
 *			  must first test that, then perform a substitution
 *			  and parse.  Because 'dclarg' only knows about
 *			  defaulting items in argument lists, the command
 *			  entry must be re-adjusted.
 *		omit	= TRUE if user gave "/o" token.
 *
 * Returns:	A pointer to a character string representing the simple
 *		("/-token and logical symbol) substitution of the command
 *		and argument string.  If a process must be spawned by the
 *		command interpreter, this string is used as the actual
 *		command string, rather than the concatenated DCLARG list
 *		(due to the length, and to the level of substitution performed).
 *
 *		The command-verb is fully-substituted in this string.  The
 *		string is in lowercase.  The SET/SHOW ('/', '?') prefix is
 *		retained for use in error-checking.
 *
 *		The return value is NULL if an error condition was detected
 *		(an appropriate message is emitted via 'warn').  This procedure
 *		uses 'dclarg/dclchk' to check for syntax errors.  Further
 *		tests must be made for proper number of arguments, etc.
 *
 *		If no error is detected, garbage collection should be done
 *		on the returned string, and on the DCLARG list '*dcl__',
 *		after use.  (If an error is detected here, they are already
 *		released.)
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	<rms.h>

#include	"flist.h"
#include	"dclarg.h"
#include	"dircmd.h"
#include	"freelist.h"

#include	"strutils.h"
#include	"sysutils.h"

import(filelist);

char	*dirarg (
	DCLARG	**dcl__,
	int	curfile,
	char	*argstr,
	char	*command,
	int	omit)
{
FILENT	*z	= FK_(curfile);
DCLARG	*dcl_;
VCMD2	*vcmd_	= 0;
int	cmdnum,			/* Index into 'vcmd2[]' table	*/
	cmdlen,			/* length of command-verb	*/
	cpy_dft	= FALSE,	/* Assume command is not COPY	*/
	non_DCL	= FALSE,	/* Assume command uses DCL-syntax */
	len,
	alias	= FALSE,	/* TRUE iff 'command' is symbol	*/
	subs	= FALSE,	/* TRUE iff '/' token found	*/
	havelen,		/* Current length of 'cmd[]'	*/
	twoparm	= FALSE,	/* TRUE if COPY, APPEND, etc.	*/
	used	= FALSE;	/* Used "/" current-file token	*/
char	filespec[MAX_PATH],
	symbol1	[MAX_PATH],	/* expansion via 'sysgetsym()'	*/
	symbol2	[MAX_PATH],	/* command-part of expansion	*/
	symbol3	[MAX_PATH],	/* SET/SHOW translation		*/
	*dft_	= 0,
	*cmd,
	*i_,	*o_;

	/*
	 * Numbers are a special case: they are used solely for positioning.
	 * The '/' command is also used only for positioning.  These are
	 * parsed elsewhere.
	 */
	if (isdigit(*command) || !strcmp(command, "/"))	return (nullC);

	dirent_glue (filespec, z);

	cmd	= calloc (1, havelen = MAX_PATH);

	/*
	 * If the given keyword is a FLIST-recognized command, or if the user
	 * has assigned a symbol which equates to it, set 'cmdnum' to the
	 * corresponding VCMD2-table index.  Otherwise, let the command fall-
	 * through to be treated as a non-FLIST command
	 */
#define	MODE(m)	(vcmd_->v_mode & (m))
#define	CMDNUM(t)  (cmdnum = dircmd_vcmd2 (t, strlen(t)))

	if (CMDNUM(command) < 0)
	{
		if (sysgetsym (symbol1, command, sizeof(symbol1)))
		{
			strlcpy (symbol1, nullC); /* lowercase for parse */
			len = dclarg_spec (symbol1, "@") - symbol1;
			strncpy (symbol2, symbol1, len);
			symbol2[len] = EOS;
			if (CMDNUM(symbol2) >= 0)
			{
				alias	= TRUE;
				o_	= calloc (1, strlen(argstr)
						   + strlen(symbol1));
				sprintf (o_, "%s%s", &symbol1[len], argstr);
				argstr	= o_;
				command	= symbol2;
			}
		}
	}

	/*
	 * FLIST's '/' and '?' prefixes are analogous to VMS 'SET' and 'SHOW'
	 * commands.  Translate 'SET' and 'SHOW' verbs into FLIST-format.
	 */
	if (cmdnum == 0 || cmdnum == 1)
	{
		strcpy (symbol3, (cmdnum ? "/" : "?"));
		strcpy (&symbol3[1], strskps(argstr));
		len = dclarg_spec (symbol3+1, "@") - symbol3;
		if (!alias)	argstr = calloc(1, strlen(symbol3));
		strcpy (argstr, &symbol3[len]);
		alias = TRUE;
		symbol3[len] = EOS;
		command = symbol3;
		CMDNUM(command);
	}

	/*
	 * If the command-verb is recognizable, save its state:
	 */
	if (cmdnum >= 0)
	{
		vcmd_	= dircmd_full (cmdnum);
		command	= vcmd_->v_full;
		omit	= omit || MODE(v_OMIT) || MODE(v_nonDCL);
		twoparm	= !omit && MODE(v_1_OUT + v_M_OUT);
		dft_	= vcmd_->v_dfts;
	}
	else
		omit	= TRUE;

	strcpy (cmd, command);		/* Start with 0th entry */
	i_	= argstr;
	o_	= strnull (cmd);
	cmdlen	= strlen (cmd);

	/*
	 * Substitute path,name,type,version tokens as required.
	 */
	while (*i_)
	{
#define	C	i_[1]
		subs	= FALSE;
		if ((i_[0] == '/')
		&&  isalnum(i_[1]) && !isalnum(i_[2]))
		{
			subs	= TRUE;
			switch (C)
			{
			case 'p':	strcat (o_, zPATHOF(z));	break;
			case 'n':	dirent_cat_n (o_, z);		break;
			case 't':	strcat (o_, z->ftype);		break;
			case 'v':	sprintf (o_, "%d", z->fvers);	break;
			default:	subs	= FALSE;
					strcat (o_, "/");
			}
			i_++;
			if (subs)	i_++;
		}
		/*
		 * Permit current-file substitution anywhere a '/' is found
		 * next to a character which would make it an error (or at
		 * least unlikely) for VMS syntax:
		 */
		else if (*i_ == '/'
		&& (isspace(C) || C == ',' || C == ')' || C == '/' || !C))
		{
			subs	= TRUE;
			i_++;
			if (!used)
			{
				strcpy (o_, filespec);
				used	= TRUE;
			}
		}
		else
		{
			*o_++	= *i_++;
			*o_	= EOS;
		}

		/*
		 * Re-check the allocated length of the substituted string.
		 * It may be much longer than the input string.  Reallocate
		 * if necessary, so that the next substitution will have
		 * enough space for a full filename.
		 */
		o_	= strnull (o_);
		if (subs)
		{
			if ((strlen (i_) + strlen (cmd) + MAX_PATH) > havelen)
			{
				cmd	= realloc (cmd, havelen += MAX_PATH);
				o_	= strnull (cmd);
			}
		}
	}
	*o_	= EOS;

	/*
	 * If the current entry hasn't been used, and the command mode implies
	 * its inclusion, put it on the end of the command string.  If this is
	 * a two-parameter command (e.g., COPY), at the same time trim the
	 * version number from the text so that the user will be able to run
	 * COPY in a natural manner (i.e., create a new version).
	 */
	if (!omit)
	{
		if (! used)
		{
			if (twoparm)
			{
				FILENT	ztmp	= *z;
				ztmp.fvers	= 0;
				dirent_glue (filespec, &ztmp);
			}
			sprintf (o_, " %s", filespec);
		}
		flist_log ("! %.80s", filespec);
	}
	else if (subs)
		flist_log ("! %.80s", &cmd[cmdlen]);

	if (alias)	cfree (argstr);	/* release locally-acquired bfr	*/

	/*
	 * All "normal" commands fall through this point, including things
	 * like COPY and APPEND (which do not propagate the pathname to the
	 * output specification).  Do a quick check to see if we have one
	 * of these special cases.
	 */
	o_ = cmd;
	if (*o_ == '/' || *o_ == '?')
		o_++;

	if ((cmdnum = dircmd_vcmd2(cmd, dclarg_spec(o_, "@") - cmd)) >= 0)
	{
		vcmd_	= dircmd_full(cmdnum);
		cpy_dft	= MODE(v_COPY);
		non_DCL	= MODE(v_nonDCL);
	}

	/*
	 * Parse the resulting command:
	 */
	dcl_	= dclarg (o_, dft_, non_DCL ? strlen(o_) : 1, cpy_dft);
	if (dclchk (dcl_, cmd))		/* 'cmd' is altered iff an error */
	{
		warn ("%s", cmd);
		cfree (cmd);
		freelist (dcl_);
		return (nullC);		/* error was found	*/
	}

	/*
	 * Restore '?' (normally illegal-DCL), and set command-name in
	 * uppercase to ease later comparisons.
	 */
	if (dcl_ && dcl_->dcl_text)
		strucpy (dcl_->dcl_text, nullC);
	else {
		cfree (cmd);
		return (nullC);
	}
	/*
	 * Return pointers to DCLARG-list, and to substituted commands:
	 */
	*dcl__	= dcl_;
	return (cmd);			/* No error found	*/
}
