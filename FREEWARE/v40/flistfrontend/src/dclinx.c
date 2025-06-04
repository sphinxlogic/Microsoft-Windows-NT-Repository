#ifndef NO_IDENT
static char *Id = "$Id: dclinx.c,v 1.4 1995/06/04 19:12:26 tom Exp $";
#endif

/*
 * Title:	dclinx.c
 * Author:	Thomas E. Dickey
 * Created:	30 Jun 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		05 Feb 1985, added 'dclinx2' entry to get actual DCLARG-pointer
 *		04 Jul 1984
 *
 * Function:	This module performs simple indexing for DCLARG lists.  It
 *		is used to locate the character-string for a particular
 *		filename in the argument list, indexed by parameter number.
 *
 * Parameters:	dcl_	=> pointer to DCLARG-list to search.
 *		mfld	=  parameter number, starting from 0.
 *		sfld	=  subfield number, incremented after each "," in a
 *			   list.  This also starts from 0.
 *
 * Returns:	A pointer to the required DCLARG-data.  If it does not
 *		exist, NULL is returned.
 */

#include	"bool.h"
#include	"dclarg.h"

DCLARG	*dclinx2 (DCLARG *dcl_, int mfld, int sfld)
{
	for (; dcl_; dcl_ = dcl_->dcl_next)
	{
		if (mfld < dcl_->dcl_mfld)	/* give up	*/
			break;
		else if (mfld == dcl_->dcl_mfld && !isopt(dcl_->dcl_text[0]))
		{
			if (sfld == dcl_->dcl_sfld)
				return (dcl_);
		}
	}
	return (0);
}

/*
 * Return pointer to text-field only:
 */
char	*dclinx (DCLARG *dcl_, int mfld, int sfld)
{
	register
	DCLARG	*d_	= dclinx2 (dcl_, mfld, sfld);

	return (d_ ? d_->dcl_text : nullC);
}
