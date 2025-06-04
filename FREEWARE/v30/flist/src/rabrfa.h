/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: rabrfa.h,v 1.1 1995/06/06 13:01:24 tom Exp $
 *
 * interface of rabrfa.c
 */

#ifndef RABRFA_H
#define RABRFA_H

#include	<rms.h>

extern	int	rabrfa_get (struct RAB *z);
extern	void	rabrfa_put (struct RAB *z, int offset);

#endif/*RABRFA_H*/
