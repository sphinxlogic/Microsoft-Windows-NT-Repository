/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: datent.h,v 1.2 1995/10/21 18:26:19 tom Exp $
 *
 * VMS date+time is stored as a 64-bit integer.  Use my own mode to avoid
 * long sequence of include-files for RMS.  Cover up diffs between VAX and AXP
 * by macros that use the address of the date.
 */
#ifndef DATENT_H
#define DATENT_H

#ifdef __alpha
#define isOkDate(p) ((*p) != 0)
#define isBigDate(p) ((*p) == -1)
#define makeBigDate(p) (*p) = -1
typedef __int64 DATENT;
#else
typedef struct	my_datent {
	unsigned date64[2];
	} DATENT;
#define isOkDate(p) ((p)->date64[1] != 0)
#define isBigDate(p) ((p)->date64[1] == -1)
#define makeBigDate(p) (p)->date64[1] = -1
#endif

#endif /* DATENT_H */
