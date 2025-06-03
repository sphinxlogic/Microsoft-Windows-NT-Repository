/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: acplook.c,v 1.9 1995/10/21 18:45:20 tom Exp $";
#endif

/*
 * Title:	acplook.c
 * Author:	Thomas E. Dickey
 * Created:	08 Dec 1984 (from test code)
 * Last update:
 *		18 Mar 1995, prototypes
 *		18 Feb 1995, port to AXP (DATENT changes).
 *		04 Nov 1988, added '.fexpr' data
 *		22 Mar 1985, added file-id, record-length
 *		27 Jan 1985, fix file-size (if negative result, assume we use
 *			     the high-block).
 *		20 Dec 1984, keep file-org with format (for FLIST).
 *		12 Dec 1984, mask file-org from file-format
 *		11 Dec 1984, fixes if privilege-violation found.
 *
 * Function:	This procedure uses the VMS ancillary control processor (ACP)
 *		to obtain all directory information which will be useful for
 *		FLIST except the filename.  When successful, a lookup using
 *		ACP is up to twice as fast as the equivalent using RMS (which
 *		must itself call ACP).
 *
 * Arguments:	z	=> FILENT structure to load.  We write over default
 *			   values supplied by the caller.
 *		filespec=> unique, null-ended filename specification string.
 *		nam_	=> NAM block (if a SYS$SEARCH was used before calling
 *			   this procedure).
 *
 * Returns:	The worst error status encountered in doing I/O for the lookup.
 */

#include	<starlet.h>
#include	<rms.h>
#include	<descrip.h>
#include	<iodef.h>
#include	<ssdef.h>
#include	<stsdef.h>
#include	<string.h>

#include	"acp.h"

#include	"flist.h"
#include	"dirent.h"

unsigned acplook (
	FILENT	*z,
	char	*filespec,		/* specifies files to lookup	*/
	struct	NAM	*nam_)
{
	unsigned status;
	IOSB	iosb;
	short	chnl;
	int	j;
	FIB	fib;
	ATR	atr[14];
	short	uic_vec[2];
	FAT	recattr;
	unsigned uchar;
#define	SWAP(x)	((x >> 16) + (x & 0xffff))

	static $DESCRIPTOR(DSC_name,"");
	struct	dsc$descriptor	fibDSC;

	/* patch: if 'nam_' is zero, do local parse, search */
	DSC_name.dsc$a_pointer = filespec;
	DSC_name.dsc$w_length = strlen(filespec);
	status = sys$assign (&DSC_name, &chnl, 0, 0);

	fibDSC.dsc$w_length = sizeof(FIB);
	fibDSC.dsc$a_pointer = (char *)&fib;
	memset (&fib, 0, sizeof(fib));
	memcpy (fib.fib$w_fid, nam_->nam$w_fid, 6);

#define	atrSET(type,size,addr)\
		atr[j].atr$w_type = type;\
		atr[j].atr$w_size = size;\
		atr[j++].atr$l_addr = (char *)addr

	j = 0;
	atrSET(ATR$C_CREDATE, ATR$S_CREDATE, &z->fdate);
	atrSET(ATR$C_REVDATE, ATR$S_REVDATE, &z->frevi);
	atrSET(ATR$C_BAKDATE, ATR$S_BAKDATE, &z->fback);
	atrSET(ATR$C_EXPDATE, ATR$S_EXPDATE, &z->fexpr);
	atrSET(ATR$C_UIC,     ATR$S_UIC,     uic_vec);
	atrSET(ATR$C_FPRO,    ATR$S_FPRO,    &z->fprot);
	atrSET(ATR$C_RECATTR, ATR$S_RECATTR, &recattr);
	atrSET(ATR$C_UCHAR,   ATR$S_UCHAR,   &uchar);
	atr[j].atr$w_size = atr[j].atr$w_type = 0;

	z->fstat =
	status = sys$qiow (0, chnl, IO$_ACCESS, &iosb, 0, 0,
			&fibDSC, 0,0,0, &atr[0],0);

	if ($VMS_STATUS_SUCCESS(status))
	{
		/*
		 * Store the file-status in quasi-RMS form.  We are mostly
		 * interested in LOCKED, NOPRIV or NORMAL.  (The LOCKED-state
		 * shown by DIRECTORY corresponds to the deaccess-lock known
		 * to ACP; the RMS-lock-by-other-user is detected only after
		 * a SYS$OPEN call).
		 */
		z->fstat = RMS$_NORMAL;
		if (iosb.sts != SS$_NORMAL)	z->fstat = iosb.sts;
		if (uchar & FCH$M_LOCKED)	z->fstat = RMS$_FLK;
		if (iosb.sts == SS$_NOPRIV)	z->fstat = RMS$_PRV;

		if (! zNOPRIV(z))
		{
			z->fsize = SWAP(recattr.fat$l_efblk);
			if (!recattr.fat$w_ffbyte)	z->fsize--;
			z->fallc = SWAP(recattr.fat$l_hiblk);
			if (z->fsize < 0)		z->fsize = z->fallc;
			z->f_grp = uic_vec[1];
			z->f_mbm = uic_vec[0];
			memcpy (z->fidnum, nam_->nam$w_fid, sizeof(z->fidnum));

			/*
			 * Note: The '.f_rfm' field contains both the file-
			 *	 organization (in bits <7:4>) and the format
			 *	 (in bits <3:0>).
			 */
			z->f_rfm = recattr.fat$b_rtype;
			z->f_rat = recattr.fat$b_rattrib;
			z->f_recl= recattr.fat$w_rsize;

			if (!isOkDate(&(z->fback)))
				makeBigDate(&(z->fback)); /* (big num) */
		}
	}
	status = sys$dassgn (chnl);
	return (z->fstat);
}
