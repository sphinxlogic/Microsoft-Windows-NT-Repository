#ifndef NO_IDENT
static char *Id = "$Id: syhour.c,v 1.5 1995/10/21 17:20:40 tom Exp $";
#endif

/*
 * Title:	syshour.c
 * Author:	Thomas E. Dickey
 * Created:	02 Aug 1984
 * Last update:
 *		21 Oct 1995, DEC-C clean-compile on AXP
 *		19 Feb 1995, prototypes
 *		25 Jun 1985, corrected comments.
 *		21 Dec 1984, test for special cases 0, -1 which cause the system
 *			     routines to supriously generate current-time-of-day.
 *
 * Function:	Given a 64-bit VMS date+time, return a corresponding
 *		single-word value for hours and minutes within the day.
 *
 * Parameters:	q_	=> 64-bit (quad-word) system date
 *
 * Returns:	A single (32-bit) unsigned integer.  This function is used
 *		to sort filedates by time-of-day.  Since the actual number
 *		of 10MHz clock ticks would not really fit within 32 bits,
 *		the entire result must be right-shifted by at least 3 bits.
 *		(A full day is 0xC9.2A69C000.)
 */

#include <string.h>

#include "sysutils.h"

#define	SHR	8
#define	WORD	32

/*
 * On VAX we've got to shift the date down so we'll have the hour portion
 * of the date in the low-order 32-bits that we use for sorting.  AXP doesn't
 * have to do this.
 */
#ifdef __alpha
#define	lo_temp temp
#else	/* vax */
#define	lo_temp temp.date64[0]
#define	hi_temp temp.date64[1]
#endif

static	char	origin[] = "17-NOV-1858";

unsigned
syshour (DATENT *q_)
{
	DATENT temp;
	char	bfr[80];

	if (!isOkDate(q_))
		lo_temp = 0;
	else if (isBigDate(q_))
		lo_temp = -1;
	else
	{
		sysasctim (bfr, q_, sizeof(bfr));
		strncpy (bfr, origin, sizeof(origin)-1);
		sysbintim (bfr, &temp);
		lo_temp >>= SHR;
#ifndef __alpha
		lo_temp += (hi_temp << (WORD-SHR));
#endif
	}
	return (lo_temp);
}

/*
 * Function:	Convert the integer returned by 'syshour' to a string (hours
 *		and minutes only).
 *
 * Parameters:	co_	=> output string
 *		q	=  single-word integer to convert
 *		len	=  maximum length of output string
 */
void
syshours (char *co_, unsigned q, int len)
{
	DATENT  temp;
	int	max;
	char	bfr[80], *ci_;

#ifndef __alpha
	hi_temp = q >> (WORD-SHR);		/* Reconstruct 64-bit time */
#endif
	lo_temp = q << SHR;
	sysasctim (bfr, &temp, sizeof(bfr)); 	/* Get a string (from origin) */
	ci_ = &bfr[sizeof(origin)];		/* => hh:mm:ss.cc	*/
	if (len < (max = strlen(ci_)))	max = len;
	strncpy (co_, ci_, max);
	co_[max] = '\0';
}
