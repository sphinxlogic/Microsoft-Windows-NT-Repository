/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/*
 * Title:	cmdstk.h
 * Author:	Thomas E. Dickey
 * Created:	27 May 1995
 * Last update:
 *
 * Interface of cmdstk.c
 *
 * $Id: cmdstk.h,v 1.2 1995/05/28 11:19:14 tom Exp $
 */

#ifndef CMDSTK_H
#define CMDSTK_H

#include "crt.h"

#define	PAGESIZE	4096
#define	MAXSTK		((PAGESIZE-4)/(1+CRT_COLS))

typedef	struct	{
	char	head;	/* index of queue-head	*/
	char	stored;	/* number of items actually in queue		*/
	char	depth;	/* current depth back from head (0..(stored-1))	*/
	char	text[MAXSTK][CRT_COLS+1];
	} CMDSTK;

extern	void	cmdstk_chg (char *string);
extern	void	cmdstk_free (CMDSTK *old_);
extern	int	cmdstk_get (char *string, int dir);
extern	CMDSTK*	cmdstk_init (void);
extern	void	cmdstk_put (char *s_);
extern	void	cmdstk_tos (void);

#endif /* CMDSTK_H */
