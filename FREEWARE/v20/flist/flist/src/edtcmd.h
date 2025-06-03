/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: edtcmd.h,v 1.1 1995/02/20 02:17:36 tom Exp $ */

#ifndef	EDTCMD_H
#define	EDTCMD_H

extern	int	edtcmd_get (void);

extern	int	edtcmd_last (int c);

extern	char *	edtcmd (
	int	command,
	char	*delim,
	int	flags,
	int	do_line,
	int	do_col,
	char	*do_hlp,
	char	*do_1st,
	char	*do_ref);

extern	void	edtcmd_init (void);

extern	void	edtcmd_crt (
	char	*cmd_,
	char	*ref_,
	int	highlight,
	int	rcol,
	int	rline,
	int	ccol);

#endif	/* EDTCMD_H */
