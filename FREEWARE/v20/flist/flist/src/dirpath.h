/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: dirpath.h,v 1.1 1995/05/28 21:11:43 tom Exp $
 *
 * interface of dirpath.c
 */
#ifndef DIRPATH_H
#define DIRPATH_H

extern	void	dirpath_add (int refs, PATHNT *p);
extern	void	dirpath_free (int level);
extern	void	dirpath_init (int level);
extern	void	dirpath_rename (FILENT *znew, FILENT *zold);
extern	PATHNT*	dirpath_sort (char *esa, int len);

#endif /* DIRPATH_H */
