#ifndef NO_IDENT
static char *Id = "$Id: flcols.c,v 1.10 1995/10/24 10:42:50 tom Exp $";
#endif

/*
 * Title:	flcols.c
 * Author:	T.E.Dickey
 * Created:	01 Sep 1984
 * Last update:
 *		24 Oct 1995, added "USER" column
 *		18 Mar 1995, prototypes
 *		04 Nov 1988, added expired-date
 *		05 Oct 1985, added 'flcols_132' entrypoint.
 *		24 Aug 1985, if no argument for /CWIDTH, reset entire list
 *		01 Jul 1985, recoded /CWIDTH to use 'scanint' rather than
 *			     'sscanf' to bypass bug in CC2.0
 *		19 May 1985, do nn.nn decoding as /CWIDTH command
 *		18 May 1985, added nn.nn decoding to set name.type widths
 *		22 Mar 1985, added IDENTIFIER, LENGTH (of record)
 *		10 Jan 1985, had wrong default for '/OWNER' qualifier
 *		08 Jan 1985, nailed down column-permission better
 *		22 Dec 1984, added 'ALLOC' (renamed 'ATTRIBUTE' to 'XAB').
 *			     If "*" given, copy old list at that point.
 *		05 Dec 1984, added 'PATH' type.
 *		17 Nov 1984, added 'ATTRIBUTE', 'FORMAT' types.
 *
 * Function:	Alter the FLIST display format by modifying the 'conv_list'
 *		string.  DIRCMD passes the DCL-list as a series of lower-
 *		cased tokens which we must match against the table of
 *		permissible keys.
 */

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>

#include	"bool.h"
#include	"flist.h"
#include	"dds.h"
#include	"dircmd.h"
#include	"dirent.h"

#include	"strutils.h"

import(A_opt);	import(D_opt);	import(M_opt);	import(O_opt);
import(pcolumns);
import(conv_list);

/*
 * Table of full-length keywords used for display-columns.  They are all in
 * uppercase to make messages with them more readable.  The order of the
 * date subtypes corresponds to the permissible values of 'D_opt' (1:4 if set).
 */
static
char	*keys[]	= {
		"DATE",
		"CREATED",	"BACKUP",	"REVISED",	"EXPIRED",
		/* end subtype */
		"ALLOC",	"FORMAT",	"IDENTIFIER",	"LENGTH",
		"MASK",
		"OWNER",	"PATH",		"SIZE",		"USER",
		"XAB"};

static	int	max_keys = sizeof(keys) / sizeof(keys[0]);

static	int	flcols__add (char *out_, char c);
static	char *	flcols__key (char c);

/*
 * Decode a list of column-keywords to set/reset the display format:
 */
tDIRCMD(flcols)
{
	register
	int	j;
	char	new_list[sizeof(conv_list)];
	register
	char	*k_,
		*text_;

	new_list[0] = '\0';
	if (xdcl_ = xdcl_->dcl_next)	/* Point to first argument	*/
	{
	    for (; xdcl_; xdcl_ = xdcl_->dcl_next)
	    {
		text_ = xdcl_->dcl_text;
		if (strcmp(text_, "*") == 0)
		{
		    for (k_ = conv_list; *k_; k_++)
			if (flcols__add (new_list, *k_))	return;
		}
		else
		{
		    for (j = 0, k_ = nullC; j < max_keys; j++)
		    {
			if (strabbr (text_, keys[j], strlen(text_),1))
			{
			    k_ = keys[j];
			    break;
			}
		    }
		    if (k_)
		    {
			if (flcols__add (new_list, _tolower(*text_)))
			    return;
		    }
		    else
		    {
			warn ("Unknown keyword: %s", text_);
			return;
		    }
		}
	    }
	    strcpy (conv_list, new_list);
	}
	else
		flcols_init ();

	dds_all (dds_fast(DDS_U_C), *curfile_);
}

/* <__key>:
 * Return a pointer to a specific key-word, given the first character.
 */
static
char	*flcols__key (char c)
{
	int	j;
	for (j = 0, c = _toupper(c); j < max_keys; j++)
	{
		if (c == *keys[j])		return (keys[j]);
	}
	return ("?");
}

/* <__add>:
 * Add a new character to the (temporary) conversion list.  Return TRUE if
 * an error was discovered.
 */
static
int	flcols__add (char *out_, char c)
{
	if (	(strchr("bcdre", c) && !D_opt)
	||	(strchr("m", c)    && !M_opt)
	||	(strchr("o", c)    && !O_opt)
	||	(strchr("as", c)   && !A_opt) )
	{
		warn ("Display type %s is not applicable", flcols__key(c));
		return (TRUE);
	}
	if (strchr(out_, c))
	{
		warn ("Repeated display-field: %s", flcols__key(c));
		return (TRUE);
	}
	sprintf (out_ + strlen(out_), "%c", c);
	return (FALSE);
}

/* <init>:
 * (Re)initialize 'conv_list', based on the current settings of FLIST's
 * option flags.
 */
void	flcols_init (void)
{
	char	*c_ = conv_list;

	if (A_opt)	*c_++ = 's';
	if (D_opt)	*c_++ = 'd';
	if (M_opt)	*c_++ = 'm';
	*c_ = EOS;
}

/* <show>:
 * Show the current contents of 'conv_list', in the summary line:
 */
void	flcols_show (void)
{
	int	j;
	char	*c_ = conv_list,
		c,
		bfr[CRT_COLS];

	bfr[0] = EOS;
	while (c = *c_++)
	{
		if (bfr[0])	strcat (bfr, ", ");
		strcat (bfr, flcols__key(c));
		if (c == 'd')
			sprintf (strnull(bfr), " (%s)", keys[D_opt]);
	}
	flist_tell ("Display columns: %s", bfr);
}

/* <left>:
 * Rotate the display-list to the left.
 */
tDIRCMD(flcols_left)
{
	char	c = conv_list[0];

	if (conv_list[1])
	{
		sprintf (conv_list, "%s%c", &conv_list[1], c);
		dds_all (dds_fast(DDS_U_C), *curfile_);
	}
}

/* <right>:
 * Rotate the display-list to the right.
 */
tDIRCMD(flcols_right)
{
	int	len	= strlen(conv_list)-1;
	char	old_list[sizeof(conv_list)],
		c	= conv_list[len];

	if (len > 0)
	{
		strcpy (old_list, conv_list);
		old_list[len] = EOS;
		sprintf (conv_list, "%c%s", c, old_list);
		dds_all (dds_fast(DDS_U_C), *curfile_);
	}
}

/* <width>:
 * Decode a token of the form "n.t" where both parts are optional; specify the
 * maximum number of columns to permit for the display of filename and filetype,
 * respectively.
 */
tDIRCMD(flcols_width)
{
	int	nlen	= -1,
		tlen	= -1;
	register
	int	set_width = 0;
	register
	char	*text_;

	if (xdcl_ = xdcl_->dcl_next)	/* Point to first argument	*/
	{
	    if (xdcl_->dcl_next)
	    {
	    	warn ("Only one argument expected");
		return;
	    }
	    else
	    {
		text_ = xdcl_->dcl_text;
		if (isdigit(*text_))	/* "nn" or "nn.nn"	*/
		{
		    text_ = scanint (text_, &nlen);
		    set_width++;
		}
		if (*text_)
		{
		    if (*text_++ == '.')
		    {
			if (isdigit(*text_))
			{
			    text_ = scanint (text_, &tlen);
			    if (*text_ && *text_ != ';' && *text_ != '.')
				set_width = -1;
			    else
				set_width++;
			}
			else if (*text_)
			    set_width = -1;
		    }
		    else
			set_width = -1;
		}
		if (set_width < 0 ||
			nlen == 0 || nlen > MAX_NAME ||
			tlen == 0 || tlen > MAX_TYPE)
		{
		    warn ("Illegal column-width: %s", xdcl_->dcl_text);
		    return;
		}
	    }
	    if (set_width)
	    {
		pcolumns[0] = (nlen > 0) ? nlen : 0;
		pcolumns[1] = (tlen > 0) ? tlen : 0;
	    }
	}
	else
	{
	    pcolumns[0] = pcolumns[1] = 0;
	    dirent_width ((FILENT *)0);
	}

	dds_all (dds_fast(DDS_U_C), *curfile_);
}

/* <132>:
 * Provide 80/132 column switch on VT100-compatible terminals with AVO (or
 * equivalent).  If an argument is given, set a specific terminal-width.
 */
#define	WIDEST	132
#define	NARROW	80

tDIRCMD(flcols_132)
{
	int	width,
		length;
	char	*text_;

	termsize (FALSE, &width, &length); /* Find current terminal-size */
	if (xdcl_ = xdcl_->dcl_next)	/* Point to first argument	*/
	{
	    if (xdcl_->dcl_next)
	    {
	    	warn ("Only one argument expected");
		return;
	    }
	    else
	    {
		text_ = scanint (xdcl_->dcl_text, &width);
		if (*text_ || width <= 0)
		{
		    warn ("Illegal argument: %d", xdcl_->dcl_text);
		    return;
		}
	    }
	}
	else
	{
	    if (width <= NARROW)	width = WIDEST;
	    else			width = NARROW;
	}

	if (termsize (TRUE, &width, &length))
	{
		crt_refresh ();		/* Readjust CRT-module, repaint screen	*/
		dds_all (dds_fast(DDS_U_C), *curfile_);	/* Fill in any new text */
	}
	else
	    warn ("SET TERMINAL failed");
}
