/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: getprot.h,v 1.4 1995/06/04 21:47:28 tom Exp $
 * 
 * public interface of 'getprot.c', 'setprot.c'
 */

#ifndef GETPROT_H
#define GETPROT_H

typedef	struct	{
	unsigned short	p_mask;		/* protection mask	*/
	unsigned short	p_grp,		/* group code		*/
			p_mbm;		/* member code		*/
	} GETPROT;

extern	int	getprot (GETPROT *ret_, char *name_);
extern	int	setprot (char *filespec, char *prot_code);
extern	int	cmpprot (GETPROT *pr_, char *mode);

#endif	/* GETPROT_H */
