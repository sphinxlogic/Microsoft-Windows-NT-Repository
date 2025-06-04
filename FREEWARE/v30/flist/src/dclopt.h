/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: dclopt.h,v 1.6 1995/06/04 23:20:50 tom Exp $
 *
 * public interface of dclopt.c (DCL-option decoder)
 */

#ifndef DCLOPT_H
#define DCLOPT_H

#include "dclarg.h"

typedef	struct	{
	char	*opt_name;	/* => full option name			*/
	void	*opt_init;	/* => initial value for area		*/
	void	*opt_ini2;	/* => default, if no value given	*/
	void	*opt_area;	/* => buffer area for value		*/
	unsigned
	short	opt_size,	/* 1=flag, 2=value, 3+=string		*/
		opt_abbr,	/* minimum length of abbreviation	*/
		opt_flag;	/* bit-vector for disjoint option test	*/
	} DCLOPT;

extern	char *	dclarea (char *name, int size, DCLOPT *opt, int size_opt);
extern	int	dclopt (char *msg_, DCLARG *d_, DCLOPT *opt, int size_opt);

#endif /* DCLOPT_H */
