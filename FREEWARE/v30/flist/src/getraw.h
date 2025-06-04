/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: getraw.h,v 1.1 1995/06/05 23:19:45 tom Exp $
 *
 * interface of getraw.c
 */

#ifndef	GETRAW_H
#define	GETRAW_H

extern	void	ctlx_ast (void);
extern	void	ctlx_clr (void);
extern	int	ctlx_tst (void);
extern	int	getraw (void);
extern	void	getraw_flush(void);
extern	void	getraw_init (char *cmd_, char *dft_);
extern	int	gotraw (void);

#endif/*GETRAW_H*/
