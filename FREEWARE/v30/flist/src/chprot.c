/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: chprot.c,v 1.7 1995/10/21 19:00:38 tom Exp $";
#endif

/*
 * Title:	chprot.c
 * Author:	Thomas E. Dickey
 * Created:	19 Nov 1984
 * Last update:
 *		18 Mar 1995, standardized memory copy/set.
 *		21 Dec 1984, return status-code from I/O status-block.
 *
 * Function:	This procedure uses the ACP to alter the protection code of
 *		one or more VAX/VMS disk files.
 *
 * Arguments:	filespec - address of a file specification, optionally with
 *			wildcards.  Each matching file is altered.
 *		code	- 16-bit VMS file protection code (see XAB.H).  It is
 *			  arranged from LSB to MSB: SYSTEM, OWNER, GROUP,
 *			  WORLD; within fields NOREAD, NOWRITE, NOEXEC, and
 *			  NODELETE.
 *		mask	- 16-bit mask, selecting fields in 'code' to disable.
 *			  If zero, we need not read the file's protection
 *			  before altering it.
 *
 * Returns:	The first error status found.  (The procedure halts on the
 *		first error.)
 */

#include	<starlet.h>
#include	<rms.h>
#include	<descrip.h>
#include	<iodef.h>
#include	<stsdef.h>
#include	<string.h>

#include	"bool.h"
#include	"acp.h"
#include	"rmsinit.h"

#include	"chprot.h"

#define	sys(f)	status = f; if (! $VMS_STATUS_SUCCESS(status))
#define	QIO(func) sys$qiow (0, chnl, func, &iosb, 0, 0,\
			&fibDSC, 0,0,0, &atr[0],0)

int
chprot (
	char	*filespec,		/* specifies files to lookup	*/
	int	code,
	int	mask)
{
	struct	FAB	fab;
	struct	NAM	nam;		/* used in wildcard parsing	*/

	char	rsa[NAM$C_MAXRSS],	/* resultant string area	*/
		esa[NAM$C_MAXRSS];	/* expanded string area		*/

	unsigned status;
	IOSB	iosb;
	short	chnl;
	FIB	fib;
	ATR	atr[2];
	short	short_fpro;

	$DESCRIPTOR(DSC_name,"");
	struct	dsc$descriptor	fibDSC;

	rmsinit_fab (&fab, &nam, nullC, filespec);
	rmsinit_nam (&nam, esa, rsa);

	sys(sys$parse(&fab))			return (status);

	for (;;)
	{
		sys(sys$search(&fab))
		{
			if (status == RMS$_NMF)
				status = 0;
			return (status);
		}

		DSC_name.dsc$a_pointer = nam.nam$l_rsa;
		DSC_name.dsc$w_length = nam.nam$b_rsl;

		fibDSC.dsc$w_length = sizeof(FIB);
		fibDSC.dsc$a_pointer = (char *)&fib;
		memset (&fib, 0, sizeof(fib));
		fib.fib$l_acctl = FIB$M_WRITECK;
		memcpy (fib.fib$w_fid, nam.nam$w_fid, 6);

		atr[0].atr$w_type = ATR$C_FPRO;
		atr[0].atr$w_size = ATR$S_FPRO;
		atr[0].atr$l_addr = (char *)&short_fpro;
		atr[1].atr$w_size = atr[1].atr$w_type = 0;

		sys(sys$assign(&DSC_name, &chnl, 0, 0))	return (status);

		if (mask)
		{
			sys(QIO(IO$_ACCESS))		goto no_access;
			sys(iosb.sts)			goto no_access;
			short_fpro = (short_fpro & mask)
				   | (code & ~mask);
		}
		else
		{
			short_fpro = code;
		}

		sys(QIO(IO$_MODIFY))			goto no_access;
		sys(iosb.sts)				goto no_access;
		sys(sys$dassgn (chnl))			return (status);
	}

no_access:
	sys$dassgn (chnl);	/* Try to cleanup if error found */
	return (status);
}
