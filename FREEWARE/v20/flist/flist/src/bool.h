/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: bool.h,v 1.4 1995/05/28 00:02:54 tom Exp $
 *
 * Title:	bool.h
 * Author:	Thomas E. Dickey
 * Created:	08 Aug 1983
 * Last update:
 *		27 May 1995, ifdef-guard
 *		11 Dec 1984, added SIZEOF, EOS
 *		11 Apr 1984, removed 'struct' from nullS
 *		05 Apr 1984, to added null-pointer def's
 */

#ifndef BOOL_H
#define BOOL_H

#ifndef TRUE
#define	TRUE	1
#define	FALSE	0
#endif

typedef	int	bool;

#define	nullP(t)	((t *)0)
#define	nullS(t)	((t *)0)	/* K&R says 'struct', Unix not	*/

/* The most common null pointers: */
#define	nullC		nullP(char)
#define	nullINT		nullP(int)

#define	SIZEOF(array)	(sizeof(array)/sizeof(array[0]))

#define	EOS	'\0'

#endif /* BOOL_H */
