#ifndef NO_IDENT
static char *Id = "$Id: syasctim.c,v 1.5 1995/10/21 18:39:50 tom Exp $";
#endif

/*
 * Title:	sysasctim.c
 * Author:	Thomas E. Dickey
 * Created:	02 Aug 1984 (broke out of 'dirent.c')
 * Last update:
 *		03 Jun 1995, prototypes
 *		02 Aug 1984
 *
 * Function:	Use VMS run-time library routine to convert a 64-bit date
 *		field to a printable string.  The string-length drives the
 *		actual format of the conversion by selecting subsets of the
 *		complete date format:
 *
 *			123456789.123456789.12345
 *			dd-mmm-yyyy hh:mm:ss.cc
 *
 * Parameters:	buf[]	= output string
 *		q_	=> quad-word (64-bit) VMS date (origin is 17-Nov-1858,
 *			   to use Smithsonian time).
 *		ilen	=  length to convert.
 */

#include	<lib$routines.h>
#include	<descrip.h>

void
sysasctim (
	char	*buf,			/* String to load into			*/
	unsigned *q_,			/* => 64-bit date structure		*/
	int	ilen)			/* Length of 'buf[]', counting null	*/
{
	short	olen;
	unsigned cvtflg = 0;
	static	$DESCRIPTOR(date_time,"");

	date_time.dsc$a_pointer = buf;
	date_time.dsc$w_length  = ilen - 1;
	lib$sys_asctim (&olen, &date_time, q_, &cvtflg);
	buf[olen] = '\0';
}
