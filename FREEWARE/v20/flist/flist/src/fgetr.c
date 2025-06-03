/******************************************************************************
 * Copyright 1984 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: fgetr.c,v 1.3 1984/09/17 10:50:14 tom Exp $";
#endif

/*
 * Title:	fgetr.c
 * Author:	Thomas E. Dickey
 * Created:	11 Sep 1984
 * Last update:	17 Sep 1984, return -1 on EOF
 *
 * Function:	This procedure reads a line from an (assumed) VMS carriage-
 *		control format file.  It is used by the BROWSE program as an
 *		interim solution until a suitable set of RMS-oriented routines
 *		can be developed to read other formats.
 *
 */

#include	<stdio.h>

fgetr (file_, bfr, maxbfr, mark_)
FILE	*file_;		/* file-descriptor pointer	*/
char	bfr[];		/* buffer to load		*/
int	maxbfr,		/* ...its size			*/
	*mark_;		/* file-address of buffer	*/
{
int	len = 0,	got;

	while ((got = fgetc (file_)) != EOF)
	{
		if (len < (maxbfr-1))		bfr[len++] = got;
		if (got == '\n')		break;
	}
	*mark_ = ftell(file_);
	if (len <= 0)	len = -1;
	return (len);
}
