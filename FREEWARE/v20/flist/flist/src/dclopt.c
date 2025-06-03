/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: dclopt.c,v 1.9 1995/10/21 17:03:40 tom Exp $";
#endif

/*
 * Title:	dclopt.c
 * Author:	Thomas E. Dickey
 * Created:	11 Jul 1984
 * Last update:
 *		21 Oct 1995, prototypes
 *		03 Jul 1985, use 'scanint' instead of 'sscanf' to bypass bug
 *			     in CC2.0
 *		26 Jun 1985, moved fancy date-parsing to 'sysbintim'.
 *		15 Jun 1985, typed 'strchr'
 *		14 Jun 1985, had stepped on date-parsing.  fixed it.
 *		28 Mar 1985, added 'status' argument to 'error'.
 *		20 Jan 1985, fixed bug in 'dcl_ini2' code.
 *		15 Jan 1985, removed Unix-I/O call.
 *		14 Jan 1985, added 'dcl_ini2' component
 *		14 Oct 1984, use 'isopt2' for delimiter test
 *		02 Aug 1984, fixed 'sysbintim' call to get absolute dates.
 *		23 Jul 1984, fixed usage of 'sscanf' return code
 *		13 Jul 1984
 *
 * Function:	Do simple lookup/setting of options using the DCLARG-list.
 *		The DCLOPT structure provides for three types of option values:
 *
 *		(a)	1-byte to 4-byte, an option flag, or numeric value.
 *		(b)	8-byte, a date value
 *		(c)	a character string.
 *
 *		Area-sizes no longer than 8 bytes are considered to be binary;
 *		longer ones are considered character strings.
 *
 *		DCLOPT strips a "NO" prefix from any option keyword.  Its
 *		validity is then tested (strings are nulled, flag or value
 *		set to zero, illegal for date).
 *
 * Parameters:	msg_	=> area into which (if nonnull) any error message is
 *			   written.  If null, error message is sent to 'stderr'.
 *		d_	=> DCLARG linked-list.  Only options beginning with '/'
 *			   are legal.
 *		opt[]	=> DCLOPT array of permissible options.
 *		size_opt=  sizeof(opt[])
 *
 * Returns:	TRUE if any errors were found.
 *
 * Patch:	The 'abstim' string adjustment is necessary to prevent $BINTIM
 *		from storing a date whose hours and minutes are the same as
 *		that of the current date (??).  The form shown in HELP SPECIFY
 *		DATE_TIME doesn't work (it specifies a ":" between date and
 *		time).  For this reason, I patch the first colon to a space:
 *			"17-NOV-1858:00:00:00.00"
 *		becomes
 *			"17-NOV-1858 00:00:00.00"
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"bool.h"
#include	"flist.h"
#include	"dclarg.h"
#include	"dclopt.h"

#include	"strutils.h"
#include	"sysutils.h"

#define	OJ	opt[j]

static
char	e_confl[] =	"repeated or conflicting option",
	e_equal[] =	"illegal option delimiter",
	e_dateq[] =	"illegal date value",
	e_datev[] =	"DATE-option requires value",
	e_known[] =	"unknown option keyword",
	e_lvals[] =	"'NO' prefix cannot be combined with value",
	e_nonum[] =	"expected a numeric value",
	e_table[] =	"(DCLOPT internal error)",
	e_wantv[] =	"value needed",
	e_2long[] =	"string too long";

/*
 * Do the actual copy of a 'from' object to 'to', given the size.  If the
 * length is no greater than 8, it won't be a string.
 */
static
void	dclopt_cpy (void *to, void *from, int size)
{
	if (from != 0) {
		if (size <= 8) {
			memcpy (to, from, size);
		} else {
	    		strncpy (to, from, size);
		}
	} else {
		if (size <= 8) {
			memset(to, 0, size);
		} else {
	    		*(char *)to = EOS;
		}
	}
}

int	dclopt (
	char	*msg_,
	DCLARG	*d_,
	DCLOPT	opt[],
	int	size_opt)
{
	int	j,	size,	slen,	got,	flag,
		Any_Errors = FALSE,
		confl	= 0,
		maxopt	= size_opt / sizeof(DCLOPT);
	char	*area,	*init,	msg[80+CRT_COLS],
		*v_, *v2_,
		*r_, *s_, *t_,
		*err_	= 0;

	/*
	 * Initialize all options by copying the init component to the area.
	 */
	for (j = 0; j < maxopt; j++)
	    dclopt_cpy (OJ.opt_area, OJ.opt_init, OJ.opt_size);

	/*
	 * Search DCLARG-list for options; lookup these in the table 'opt[]'.
	 */
	for (; d_; d_ = d_->dcl_next)
	{
	    s_ = r_ = &d_->dcl_text[0];
	    if (*s_ == '/')
	    {
		got	= FALSE;
		flag	= TRUE;
		t_	= dclarg_keyw(r_ = ++s_);
		slen	= t_ - s_;
		if (slen > 2)	/* If longer than 2, may have "NO" prefix */
		{
		    if (strabbr (s_, "no", 2, 2))
		    {
			flag	= FALSE;
			s_	+= 2;
			slen	-= 2;
		    }
		}

		for (j = 0; j < maxopt; j++)
		{
		    if (got = strabbr(s_, OJ.opt_name, slen, OJ.opt_abbr))
		    {
			if (confl & OJ.opt_flag)
			    err_ = e_confl;
			else
			    confl |= OJ.opt_flag;
			break;
		    }
		}

		v_   = OJ.opt_area;
		size = OJ.opt_size;
		v2_  = OJ.opt_ini2;

		if (! got)
		    err_ = e_known;
		else if (*t_)	/* Decode qualifier value	*/
		{
		    if (! flag)
			err_ = e_lvals;
		    else if (! isopt2(*t_))
			err_ = e_wantv;
		    else
		    {
			t_++;	/* Skip past punctuation	*/
			if (size == 8)
			{
			    if (sysbintim (t_, (DATENT *)v_) != 0)
				err_ = e_dateq;
			}
			else if (size < 8)
			{
			    register char *e_ = scanint(t_, &flag);
			    if (*e_ || t_ == e_)
				err_ = e_nonum;
			}
			else if (size <= strlen(t_))
			    err_ = e_2long;
			else
			    strcpy (v_, t_);
		    }
		}
		else		/* No qualifier value given	*/
		{
		    if (flag && v2_)	/* Default if-set ?	*/
			memcpy (((size <= sizeof(flag)) ? &flag : (int *)v_), v2_, size);
		    else if (size > 8)
			err_ = e_wantv;
		    else if (size == 8)
			err_ = e_datev;
		}

		if (got && (size < 8))
		{
		    if (size <= sizeof(flag))
			memcpy (v_, &flag, size);
		    else
			err_ = e_table;
		}
	    }
	    else if (isopt(*r_))
		err_ = e_equal;

	    if (err_)
	    {
		Any_Errors++;
		if (msg_)
		    sprintf (msg_, "%s: %s", err_, r_);
		else
		{
		    sprintf (msg, "%s: %s\n", err_, r_);
		    error (0, msg);
		}
		break;		/* quit on the first error	*/
	    }
	}
	return (Any_Errors);
}
