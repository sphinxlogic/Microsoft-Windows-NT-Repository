/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: chprot.h,v 1.1 1995/03/18 20:51:28 tom Exp $
 *
 * Public interface of 'chprot.c'
 */

extern	int chprot (
	char	*filespec,	/* specifies files to lookup	*/
	int	code,
	int	mask);
