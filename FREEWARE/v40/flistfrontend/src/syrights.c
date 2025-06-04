#ifndef NO_IDENT
static char *Id = "$Id: syrights.c,v 1.5 1995/10/19 10:35:36 tom Exp $";
#endif

/*
 * Title:	sysrights.c
 * Author:	T.E.Dickey
 * Created:	01 Aug 1984
 * Last update:
 *		19 Oct 1995, casts needed for DEC-C on AXP.
 *		19 Feb 1995, sys utils prototypes
 *		05 Dec 1989, pass mask,word_num as args rather than vshift,
 *			     to accommodate VMS 5.1
 *		30 Sep 1985, use SYS$GETJPIw in VMS 4.x
 *		01 Aug 1984
 *
 * Function:	Test for current-process privilege, returning TRUE if the
 *		specified privilege is enabled.
 *
 * Parameters:	mask	= privilege-mask (e.g., PRV$M_BYPASS) from <prvdef.h>.
 *		word_num= 0 (for the one-bit masks!)
 */

#include	<starlet.h>
#include	<jpidef.h>

#include	"sysutils.h"

#define	ITEM(code,len)	((code<<16)+len)

typedef	int	QUAD[2];

static
QUAD	iosb,	priv_buf;

static
int	ret_len;

static
unsigned	itmlst[] = {	/* list of 'longword' */
		ITEM(JPI$_CURPRIV,sizeof(QUAD)),
		(unsigned) &priv_buf,	(unsigned) &ret_len,
		JPI$C_LISTEND
		};

int
sysrights (int mask, int word_num)
{
	sys$getjpiw (	0,	/* efn		*/
			0,	/* pidadr	*/
			0,	/* prcnam	*/
			&itmlst,
			&iosb,
			0,	/* astadr	*/
			0);	/* astprm	*/

	return ((priv_buf[word_num] & (mask)) != 0);
}
