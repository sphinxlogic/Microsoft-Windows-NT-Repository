#ifndef NO_IDENT
static char *Id = "$Id: rmslook.c,v 1.8 1998/10/19 01:02:04 tom Exp $";
#endif

/*
 * Title:	rmslook.c
 * Author:	Thomas E. Dickey
 * Created:	08 Dec 1984 (from 'dirent.c')
 * Last update:
 *		18 Oct 1998, use copyBigDate macro to work with DEC C 5.3
 *		18 Feb 1995, port to AXP (DATENT mods)
 *		15 Jun 1985, use 'xabprouic' for CC2.0 change
 *		22 Mar 1985, added file-id, record-length
 *		22 Dec 1984, set '.fallc' (file-allocation) component.
 *		20 Dec 1984, return file-format, organization in the same byte.
 *		15 Dec 1984, clear internal-file-id (W_IFI).  Don't need a
 *			     filespec argument, since the sys-search before
 *			     this procedure does the proper setup.
 *
 * Function:	Lookup information about a particular file, to fill all fields
 *		in the indicated FILENT structure.  Returns TRUE iff the file
 *		was found.
 *
 * Arguments:	z	=> FILENT structure to load.  Only those fields which
 *			   are implied by the XAB-list are modified.
 *		fab_	=> pre-init FAB, XAB-list for the attributes which we
 *			   need.
 *
 * Returns:	The worst error found while reading the directory entry.
 */

#include	<starlet.h>
#include	<rms.h>
#include	<descrip.h>
#include	<stsdef.h>

#include	"flist.h"
#include	"dirent.h"
#include	"xabproui.h"

unsigned 
rmslook (FILENT *z, struct FAB *fab_)
{
	unsigned status	= RMS$_NORMAL;		/* return-status	*/
	struct	XABALL	*all_	= 0;
	struct	XABDAT	*dat_	= 0;
	struct	XABFHC	*fhc_	= 0;
	struct	XABPRO	*pro_	= 0;
	struct	XABALL	*x_;
	struct	NAM	*nam_;

	if (!(x_ = (struct XABALL *)(fab_->fab$l_xab)))	return (status);

	fab_->fab$w_ifi = 0;
	z->fstat = status = sys$open(fab_);

	if ($VMS_STATUS_SUCCESS(status))
	{
		while (x_)
		{
			switch (x_->xab$b_cod)
			{
			case XAB$C_ALL:	all_ = x_;	break;
			case XAB$C_DAT:	dat_ = (struct XABDAT *)x_;	break;
			case XAB$C_FHC:	fhc_ = (struct XABFHC *)x_;	break;
			case XAB$C_PRO:	pro_ = (struct XABPRO *)x_;	break;
			}
			x_ = (struct XABALL *)x_->xab$l_nxt;
		}

		if (fhc_ && all_)
		{
			if ((z->fsize = fhc_->xab$l_ebk) == 0)
				z->fsize = all_->xab$l_alq;
			else
			{
				if (fhc_->xab$w_ffb == 0)
					z->fsize = z->fsize - 1;
			}
			z->fallc = all_->xab$l_alq;
		}

		if (dat_)
		{
			copyBigDate(&(z->fexpr), &(dat_->xab$q_edt));
			copyBigDate(&(z->frevi), &(dat_->xab$q_rdt));
			copyBigDate(&(z->fback), &(dat_->xab$q_bdt));
			if (!isOkDate(&(z->fback)))
				makeBigDate(&(z->fback)); /* (big num)*/
			copyBigDate(&(z->fdate), &(dat_->xab$q_cdt));
		}

		z->f_rfm	= fab_->fab$b_rfm & fab_->fab$b_org;
		z->f_rat	= fab_->fab$b_rat;
		z->f_recl	= fab_->fab$w_mrs;

		if (pro_)
		{
			z->fprot	= pro_->xab$w_pro;
			xabprouic (pro_, &z->f_grp, &z->f_mbm);
		}

		nam_ = fab_->fab$l_nam;	/* => NAM-block */
		z->fidnum[0] = nam_->nam$w_fid[0];
		z->fidnum[1] = nam_->nam$w_fid[1];
		z->fidnum[2] = nam_->nam$w_fid[2];

		status = sys$close(fab_);
	}
	return (z->fstat);
}
