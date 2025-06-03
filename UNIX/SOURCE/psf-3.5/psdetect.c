/* ta=4 */
/************************************************************************
*		p s d e t e c t . c												*
*																		*
*	determine if a a text file contains postscript code.				*
************************************************************************/
/*
 * $Id: psdetect.c,v 3.1 1991/11/27 06:02:58 ajf Exp ajf $
 *
*/

/*	this routine is intended for use within shell scripts.  It exits
	with a return code of 0 if it detects a postscript file or
	a value of 1 if it is not a postscript file
*/

#include <stdio.h>
#include "patchlevel.h"

main ()
{	char	stuff[20];
	int		i;
	
	stuff[0] = getchar();
	if (stuff[0] != '%')
		exit (1);
	for (i = 1;  i < 5;  i++)
	{	if (stuff[i-1] == '%'  &&  (stuff[i] = getchar()) == '!')
			exit (0);
		if (feof (stdin)  ||  ferror (stdin))
			break;
	}
	exit (1);
}
