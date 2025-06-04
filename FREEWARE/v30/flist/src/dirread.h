/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: dirread.h,v 1.1 1995/06/04 21:40:00 tom Exp $
 *
 * interface of dirread.c
 */

extern	void	dirread_init (void);
extern	void	dirread_put (struct FAB *fab_);
extern	char*	dirread_get (char *filespec, int inx);
extern	void	dirread_free (void);
extern	char*	dirread_path (int *inx_);

#ifdef	DEBUG
extern	void	dirread_show (void);
#endif
