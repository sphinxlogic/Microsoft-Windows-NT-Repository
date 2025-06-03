/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: rabrfa.c,v 1.4 1995/10/21 18:54:03 tom Exp $";
#endif

/*
 * Title:	rabrfa.c
 * Author:	Thomas E. Dickey
 * Created:	15 Jun 1985
 * Last update:	15 Jun 1985
 *
 * Function:	This module hides the source-code details of the RAB's
 *		RFA (record-file-address) field.  It's representation was
 *		altered in CC2.0: CC2.0 declares RAB's rfa as short[3],
 *		rather than long,short
 */

#include	"rabrfa.h"

#define	RFA0	((z->rab$w_rfa[1]<<16) + z->rab$w_rfa[0])
#define	RFA4	z->rab$w_rfa[2]

/*
 * Return the record-file-address translated into an offset value,
 * compatible with the 'ftell' usage.
 */
int	rabrfa_get (struct RAB *z)
{
	return (((RFA0-1) << 9) + RFA4);
}

/*
 * Translate a single-word record-address to the 48-bit RMS representation:
 */
void	rabrfa_put (struct RAB *z, int offset)
{
	unsigned rfa0;

	rfa0 = (offset >> 9) + 1;
	z->rab$w_rfa[0] = rfa0 & 0xffff;
	z->rab$w_rfa[1] = rfa0 >> 16;
	RFA4 = offset & ((1 << 9) - 1);
}
