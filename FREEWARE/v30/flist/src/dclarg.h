/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: dclarg.h,v 1.6 1995/06/05 23:33:08 tom Exp $
 *
 *	Define structure returned by DCL/argument parser 'dclarg'
 */

#ifndef	DCLARG_H
#define	DCLARG_H

#define	DCLARG	struct	DCL_arguments

DCLARG	{
	DCLARG	*dcl_next;		/* => next item in list		*/
	char	*dcl_text;		/* => string contents		*/
	unsigned dcl_stat,		/* Status of entry		*/
		dcl$l_fnb;		/* (NAM) filename status bits	*/
	char	dcl_mfld,		/* Main-field index (by ' ')	*/
		dcl_sfld,		/* Sub-field index (by ',')	*/
		dcl_from,		/* Beginning index in string	*/
		dcl_size,		/* ...number of characters	*/
		dcl$b_node,		/* node-name string length	*/
		dcl$b_dev,		/* device string length		*/
		dcl$b_dir,		/* directory string length	*/
		dcl$b_name,		/* file name string length	*/
		dcl$b_type,		/* file type string length	*/
		dcl$b_ver;		/* file version string length	*/
	};

extern	DCLARG*	argvdcl (int argc, char *argv[], char *dft_, int cmd_arg);
extern	DCLARG*	dclarg (char *inp_, char *dft_, int cmd_arg, int cpy_dft);
extern	char*	dclarg_keyw (char *c_);
extern	char*	dclarg_spec (char *i_, char *also);
extern	DCLARG*	dclarg_text (DCLARG *this_, DCLARG *last_, char *s_, int uc);
extern	int	dclchk (DCLARG *dcl_, char *co_);
extern	char*	dclinx (DCLARG *dcl_, int mfld, int sfld);
extern	DCLARG*	dclinx2 (DCLARG *dcl_, int mfld, int sfld);
extern	int	dclwild (DCLARG *dcl_);

#define	isopt(c) ((c == '/') || (c == '='))	/* Begins an option?	*/
#define	isopt2(c) ((c == '=') || (c == ':'))	/* Begins option-value?	*/

#endif	/* DCLARG_H */
