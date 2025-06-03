/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: scanver.h,v 1.2 1995/06/04 22:23:34 tom Exp $
 *
 * interface of scanver/highver
 */
extern	int	highver (char *name_);
extern	int	scanver (char *vers_, int len);
