#ifndef NO_IDENT
static char *Id = "$Id: cmpprot.c,v 1.4 1995/06/04 21:46:54 tom Exp $";
#endif

/*
 * Title:	cmpprot.c
 * Author:	T.E.Dickey
 * Created:	03 Jul 1984 (broke out of 'dirent.c')
 * Last update:
 *		19 Feb 1995, sys utils prototypes
 *		04 Nov 1988, undid last patch, fix by using unsigned type.
 *		12 Nov 1985, limit uid-code to single byte for quick fix
 *		01 Aug 1984, added calls to 'sysrights'
 *
 * Function:	Test the protection information for a file to see if the
 *		current process may access the file.
 *
 * Parameters:	pr_	=> GETPROT structure which contains the 16-bit mask
 *			   defining the protection of the file, and the owner's
 *			   group and member UIC-code.
 *		mode	=> string containing "r", "w", "e", "d" (lowercase) for
 *			   the corresponding READ, WRITE, EXECUTE and DELETE
 *			   rights which are required.
 *
 * Returns:	TRUE if all of the required rights are available.
 *
 * Note:	The C run-time routine 'access' cannot be used for this,
 *		because it does not distinguish between delete-access and
 *		write-access.
 */

#include	<rms.h>
#include	<prvdef.h>
#include	<unixlib.h>	/* getuid/getgid */

#include	"bool.h"
#include	"getprot.h"

#include	"sysutils.h"

#define	ACCESS(m)	(((mask << m) & pr_->p_mask) == 0)

int	cmpprot (
	GETPROT	*pr_,		/* Data to test			*/
	char	*mode)		/* List of access rights needed	*/
{
#define	PATCH	0xffff
	unsigned
	short	mask	= 0,		/* Start assuming nothing	*/
		grp	= PATCH & getgid(),	/* Group-id		*/
		mbm	= PATCH & getuid();	/* ...and member-id	*/
	char	*c_	= mode;

	if (sysrights(PRV$M_BYPASS,0))				return (TRUE);

	while (*c_)
		switch (*c_++)
		{
		case 'r':	mask |= XAB$M_NOREAD;	break;
		case 'w':	mask |= XAB$M_NOWRITE;	break;
		case 'e':	mask |= XAB$M_NOEXE;	break;
		case 'd':	mask |= XAB$M_NODEL;	break;
		}

	if (ACCESS(XAB$V_WLD))					return (TRUE);

	/*
	 * patch: Must verify if this is correct:
	 */
	if (sysrights(PRV$M_SYSPRV,0) &&  ACCESS(XAB$V_SYS))	return (TRUE);

	if (grp == pr_->p_grp)
	{
		if (ACCESS(XAB$V_GRP))				return (TRUE);
		if (mbm == pr_->p_mbm && ACCESS(XAB$V_OWN))	return (TRUE);
	}
	return (FALSE);
}
