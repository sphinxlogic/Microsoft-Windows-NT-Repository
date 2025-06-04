#ifndef NO_IDENT
static char *Id = "$Id: sybintim.c,v 1.7 1995/10/19 15:42:44 tom Exp $";
#endif

/*
 * Title:	sysbintim.c
 * Author:	Thomas E. Dickey
 * Created:	13 Jul 1984
 * Last update:
 *		19 Oct 1995, DEC-C clean-compile on AXP
 *		03 Jun 1995, prototypes
 *		26 Jun 1985, make this recognize the DCL-style VMS date format
 *			     (keywords, fill-in-gaps).
 *		13 Jul 1984
 *
 * Function:	Use the VMS procedure SYS$BINTIM to convert a null-ended
 *		string into a 64-bit date value.
 *
 * Parameters:	ci_	=> string to convert
 *		bfr_	=> 2-word buffer to load with date.
 *
 * Returns:	0 if no error is detected in the parse, otherwise the $BINTIM
 *		code.
 *
 * Patch:	Should consider adding things like "YESTERDAY:6:30"
 * Bugs:	If user gives a string like "6::.4", then the current minute
 *		and second are filled in, rather than using zero.
 */

#include	<starlet.h>
#include	<lib$routines.h>
#include	<ctype.h>
#include	<ssdef.h>
#include	<descrip.h>
#include	<string.h>

#include	"strutils.h"
#include	"sysutils.h"

#define	MAXDAY	24
#define	MAXBFR	80

/* FIXME: AXP */
static	int	day[2] = {0x2a69c000, 0xc9};
static	char	zeros[] = " 00:00:00.00";
			/* 0123456789ab */

int
sysbintim (char *ci_, DATENT *obfr)
{
	static	char	bigbfr[MAXBFR];	/* FIXME */
	static	char	midbfr[MAXBFR];
	$DESCRIPTOR(DSCx,bigbfr);
	$DESCRIPTOR(midnite,midbfr);
	DATENT	base;
	int	j;
	int	num	= strlen(ci_);
	char	*c_, *d_, *e_;

	if (num >= (MAXBFR-1))
		num = (MAXBFR-1);
	for (j = 0; j < num; j++)
		bigbfr[j] = _toupper(ci_[j]);
	bigbfr[num] = '\0';

	/*
	 * Compute the base of the current day, to use in keyword-dates:
	 */
	sys$gettim (obfr);
	sysasctim (midbfr, obfr, MAXDAY);
	strcpy (&midbfr[11], zeros);
	midnite.dsc$a_pointer = midbfr;
	midnite.dsc$w_length  = strlen(midbfr);
	sys$bintim (&midnite, obfr);
	base = *obfr;

	if (strabbr (bigbfr, "TODAY", num, 3))
		num = 0;
	else if (strabbr (bigbfr, "YESTERDAY", num, 1))
	{
		lib$subx (base, day, obfr, 0);
		num = 0;
	}
	else if (strabbr (bigbfr, "TOMORROW", num, 3))
	{
		lib$addx (base, day, obfr, 0);
		num = 0;
	}
	/*
	 * $BINTIM cannot parse the ':' used in DCL to separate date, time.
	 * Convert this to a trailing space and supply trailing zeros.  Also,
	 * if the year and/or month is not given, supply these:
	 */
	else
	{
		register int dash = 0;
		d_ = zeros;
		for (c_ = bigbfr; *c_; c_++)
		{
			if (*c_ == '-')
				dash++;
			else
			{
				if (*c_ == ':' && dash)		*c_ = ' ';
				if (*c_ == ' ')
				{
					if (*d_)		d_ += 3;
					break;
				}
				if (ispunct(*c_) && *d_)	d_ += 3;
			}
		}

		if (*d_ && !dash)	d_ += 3;
		e_ = dash ? c_ : bigbfr;
		if (*c_ || !dash)
		{
			if (*c_ == ' ')		c_++;
			while (*c_ && *d_)
			{
				if (ispunct(*c_))	d_ += 3;
				c_++;
			}
			if (*d_) strcpy (c_, d_);
		}
		else		/* Default to midnight for time	*/
			strcpy (c_, zeros);

		/*
		 * Insert month (mmm) and year (yyyy) if omitted, since
		 * $BINTIM does not permit *gaps*, but only loss of significance.
		 */
		if (dash < 2)
		{
			char	time[sizeof(zeros)+2];
			strcpy (time, e_);
			*e_ = '\0';
			if (dash == 1)	/* dd-mmm given	*/
			{
				strncpy (e_, midbfr+6, 5);
				strcpy (e_+5, time);
			}
			else		/* dash==0, assume 'hh' of 'hh:mm' */
			{
				strcpy (bigbfr, midbfr);
				strcpy (bigbfr+12, time);
			}
		}

		DSCx.dsc$w_length  = strlen(bigbfr);

		if ((num = sys$bintim (&DSCx, obfr)) == SS$_NORMAL)	num = 0;
	}
	return (num);
}
