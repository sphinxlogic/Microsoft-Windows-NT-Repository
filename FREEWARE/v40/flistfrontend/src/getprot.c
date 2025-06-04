#ifndef NO_IDENT
static char *Id = "$Id: getprot.c,v 1.7 1995/10/21 19:00:38 tom Exp $";
#endif

/*
 * Title:	getprot.c
 * Author:	Thomas E. Dickey
 * Created:	02 Jul 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		15 Jun 1985, CC2.0 changes declaration of uic-code.
 *		14 Dec 1984, use single FAB for search, open.
 *		11 Dec 1984, use ACP if on local node
 *		09 Sep 1984, use "rmsinit"
 *		03 Jul 1984
 *
 * Function:	Obtain protection code for a single file, or for a wildcarded
 *		file specification.  If multiple files are selected, return
 *		the worst-case (i.e., show the least common access provided
 *		in the group).
 *
 * Parameters:	ret_	=> the GETPROT structure to load.  GETPROT includes
 *			   a 16-bit mask representing the RWED modes in SYSTEM,
 *			   OWNER, GROUP and WORLD.  See SYS$LIBRARY:XAB.H for
 *			   a layout.  If any bit is set, the mode is disabled.
 *			   If an error is found in the lookup, "-1" is returned,
 *			   indicating no access.  The group and member codes
 *			   are returned in the high-order part of the return
 *			   value.  See "getprot.h" for the layout.
 *		name_	=> the specification to lookup and test.
 *
 * Returns:	If nonzero, the RMS-status indicating the first failure.
 */

#include	<string.h>

#include	<starlet.h>
#include	<rms.h>
#include	<descrip.h>
#include	<iodef.h>
#include	<ssdef.h>
#include	<stsdef.h>

#include	"acp.h"

#include	"bool.h"
#include	"getprot.h"
#include	"rmsinit.h"
#include	"xabproui.h"

int	getprot (
	GETPROT	*ret_,
	char	*name_)			/* specifies files to lookup	*/
{
	struct	FAB	fab;
	struct	NAM	nam;			/* used in wildcard parsing	*/

	struct	XABPRO	xabpro;			/* Protection attribute block	*/

	char	rsa[NAM$C_MAXRSS],		/* resultant string area	*/
		esa[NAM$C_MAXRSS];		/* expanded string area (search)*/

	unsigned status;

	IOSB	iosb;
	short	chnl;
	FIB	fib;
	ATR	atr[4];

	unsigned
	short	mask,
		uic_vec[2];
#define	group	uic_vec[1]
#define	member	uic_vec[0]

	static $DESCRIPTOR(DSC_name,"");
	struct	dsc$descriptor	fibDSC;

	rmsinit_fab (&fab, &nam, 0, name_);
	rmsinit_nam (&nam, rsa, esa);
	xabpro		= cc$rms_xabpro;
	fab.fab$l_xab	= (char *)&xabpro;

	mask = group = member = 0;	/* If 0, mask is ok	*/

	if ((status = sys$parse(&fab)) == RMS$_NORMAL)
	for (;;)
	{
		if ((status = sys$search(&fab)) == RMS$_NMF)
		{
			status	= 0;	/* normal return	*/
			break;
		}
		else if (status == RMS$_NORMAL)
		{
			if (nam.nam$l_fnb & NAM$M_NODE)	/* Via DECNET ?	*/
			{
				fab.fab$w_ifi = 0;
				if ((status = sys$open(&fab)) == RMS$_NORMAL)
				{
					mask	|= xabpro.xab$w_pro;
					xabprouic (&xabpro, &group, &member);
					sys$close(&fab);
				}
				else
					break;	/* ...no need to look more */
			}
			else
			{
				DSC_name.dsc$a_pointer = rsa;
				DSC_name.dsc$w_length = nam.nam$b_rsl;
				status = sys$assign (&DSC_name, &chnl, 0, 0);

				fibDSC.dsc$w_length = sizeof(FIB);
				fibDSC.dsc$a_pointer = (char *)&fib;
				memset (&fib, 0, sizeof(FIB));
				memcpy (fib.fib$w_fid, nam.nam$w_fid, 6);

				memset (atr, 0, sizeof(atr));
				atr[0].atr$w_type = ATR$C_UIC;
				atr[0].atr$w_size = ATR$S_UIC;
				atr[0].atr$l_addr = (char *)uic_vec;

				atr[1].atr$w_type = ATR$C_FPRO;
				atr[1].atr$w_size = ATR$S_FPRO;
				atr[1].atr$l_addr = (char *)&mask;

				status = sys$qiow (
					0, chnl, IO$_ACCESS, &iosb, 0, 0,
					&fibDSC, 0,0,0, &atr[0],0);
				sys$dassgn (chnl);

				if (! $VMS_STATUS_SUCCESS(status)
				||    iosb.sts == SS$_NOPRIV)
					break;	/* Access will fail	*/
			}
		}
		else
			break;	/* Any other error, give up	*/
	}

	if (status)
	{
		mask	= -1;	/* No access	*/
		group	=
		member	= 0;	/* ...except by SYSTEM	*/
	}
	ret_->p_mask = mask;
	ret_->p_grp  = group;
	ret_->p_mbm  = member;

	return (status);
}
