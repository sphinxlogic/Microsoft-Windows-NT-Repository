#ifndef NO_IDENT
static char *Id = "$Id: whoami.c,v 1.6 1995/10/19 10:39:40 tom Exp $";
#endif

/*
 * Title:	whoami.c
 * Author:	Thomas E. Dickey
 * Created:	29 Nov 1984
 * Last update:
 *		19 Oct 1995, DEC-C clean-compile.
 *		18 Mar 1995, prototypes
 *		30 Sep 1985, use SYS$GETJPIw in VMS 4.x
 *		09 May 1985, return nonzero code if from system
 *		01 Dec 1984
 *
 * Function:	Return to the caller all-or-part of the pathname by which
 *		the main program was called.
 *
 * Arguments:	name	=> buffer into which to return result (a null-ended
 *			  string, maximum length of 128).
 *		opt	= option controlling use of $PARSE after the $GETJPI
 *			  which determines the image name:
 *
 *			  0 - return full pathname.
 *			  1 - return name, less version
 *			  2 - return name, less type.
 *			  3 - return only the filename (for error reporting).
 *			  4 - return only the device+directory name
 *
 * Returns:	TRUE iff the image is run from a system directory
 */

#include	<starlet.h>
#include	<rms.h>
#include	<jpidef.h>
#include	<string.h>

#include	"rmsinit.h"
#include	"whoami.h"

int	whoami (char *name, int opt)
{
	int	len;
	char	buffer[NAM$C_MAXRSS];
	/* Safe: $GETJPI returns at most 128 bytes */

	struct	{
		short	buf_len,
			code;
		char	*buf_adr;
		unsigned end_flag;
		} itmlst;
	struct	FAB	fab;
	struct	NAM	nam;
	char	esa[NAM$C_MAXRSS], *lo_, *hi_;

	memset (buffer, 0, sizeof(buffer));	/* clear buffer	*/

	itmlst.buf_len = sizeof(buffer) - 1;
	itmlst.code    = JPI$_IMAGNAME;
	itmlst.buf_adr = buffer;
	itmlst.end_flag = 0;

	sys$getjpiw (0,0,0, &itmlst, 0,0,0);

	rmsinit_fab (&fab, &nam, 0, buffer);
	rmsinit_nam (&nam, 0, esa);
	sys$parse (&fab);
	lo_ = nam.nam$l_node;
	if (opt == 1)
		hi_ = nam.nam$l_ver;
	else if (opt == 2)
		hi_ = nam.nam$l_type;
	else if (opt == 3)
	{
		lo_ = nam.nam$l_name;
		hi_ = nam.nam$l_type;
	}
	else if (opt == 4)
		hi_ = nam.nam$l_name;
	else
		hi_ = lo_ + nam.nam$b_esl;
	strncpy (name, lo_, len = (hi_ - lo_));
	name[len] = '\0';

	return (   (strncmp (nam.nam$l_dir, "[SYS", 4) == 0)	/* VMS 4.x */
		|| (strncmp (nam.nam$l_dev, "SYS$", 4) == 0));	/* VMS 3.x */
}
