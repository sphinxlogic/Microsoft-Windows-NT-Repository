/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: pathup.h,v 1.1 1995/06/06 00:41:16 tom Exp $
 *
 * interface of path up/down
 */
extern	int	pathdown (char	*co_, char *ci_, char *name_);
extern	int	pathup (char *co_, char *ci_);
