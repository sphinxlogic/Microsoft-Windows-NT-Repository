/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: xabproui.c,v 1.3 1995/06/06 15:38:56 tom Exp $";
#endif

/*
 * Title:	xabprouic.c
 * Author:	Thomas E. Dickey
 * Created:	15 Jun 1985
 * Last update:	15 Jun 1985
 *
 * Function:	Extract uic fields (group and member) from a XABPRO structure.
 *		This function changed with CC2.0 (from CC1.5).
 *
 * Arguments:	pro_	=> XABPRO structure to extract from
 *		grp_	=> word to load with group
 *		mbm_	=> word to load with member
 */

#include	"xabproui.h"

void
xabprouic (struct XABPRO *pro_, unsigned short *grp_, unsigned short *mbm_)
{
	*grp_	= pro_->xab$l_uic >> 16;
	*mbm_	= pro_->xab$l_uic & 0xffff;
}
