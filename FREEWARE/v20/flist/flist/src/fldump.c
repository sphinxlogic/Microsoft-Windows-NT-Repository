/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: fldump.c,v 1.6 1995/06/06 00:54:36 tom Exp $";
#endif

/*
 * Title:	fldump.c
 * Author:	Thomas E. Dickey
 * Created:	20 Jul 1985
 * Last update:
 *		18 Mar 1995, prototypes
 *		09 Sep 1985, account for trailing '.' in FNAME
 *		20 Jul 1985
 *
 * Function:	Dump the contents of FLIST's file database for debugging.
 */

#include	"textlink.h"
#include	"flist.h"
#include	"dircmd.h"
#include	"ttrace.h"

import(pathlist);
import(filelink);
import(filelist); import(numfiles); import(numdlets);

static	void	fldump_link (TEXTLINK *p);
static	void	fldump_data (FLINK *p);
static	void	fldump_path (PATHNT *p);

tDIRCMD(fldump)
{
	PATHNT	*P;
	FLINK	*p = filelink;
	int	j;

	trace ("\n\nDUMP\n");

	trace ("PathList\n");
	for (P = pathlist; P; P = P->path_next)
	{
		fldump_link((TEXTLINK *)P);
		fldump_path(P);
		trace ("\n");
	}

	trace ("FileList: %d - %d => %d\n",
		numfiles, numdlets, numfiles-numdlets);
	for (j = 0; j < numfiles; j++)
	{
		trace ("[%03d] -> %08X", j, p = filelist[j]);
		fldump_data(p);
	}

	trace ("FileLink:\n");
	for (p = filelink; p; p = p->file_next)
	{
		fldump_link((TEXTLINK *)p);
		fldump_data(p);
	}
}

static
void	fldump_link (TEXTLINK *p)
{
	int	j;

	trace ("%08X ", p);
	for (j = 1; j < 256; j <<= 1)
		trace ("%c", p->refs & j ? '*' : '-');
}

static
void	fldump_data (FLINK *p)
{
	static	char	fmt[] =
#if	NAME_DOT
			"%s%s;%d";
#else
			"%s.%s;%d";
#endif
	fldump_path (p->fk.fpath_);
	trace (fmt, p->fk.fname, p->fk.ftype, p->fk.fvers);
	if (p->fk.fstat == RMS$_FNF) trace (" *");
	trace ("\n");
}

static
void	fldump_path (PATHNT *p)
{
	trace (" %03d: %s", p->path_sort, p->path_text);
}
