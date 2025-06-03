
static char rcsid[] = "@(#)$Id: mail_gets.c,v 5.1 1992/10/03 22:41:36 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
 *
 * 			Copyright (c) 1992 USENET Community Trust
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: mail_gets.c,v $
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** get a line from the mail file, but be tolerant of nulls

  The length of the line is returned

**/

#include <stdio.h>
#include <ctype.h>


int
mail_gets(buffer, size, mailfile)
char *buffer;
int size;
FILE *mailfile;
{
	register int line_bytes = 0, ch;
	register char *c = buffer;

	size--; /* allow room for zero terminator on end, just in case */
	while (!feof(mailfile) && !ferror(mailfile) && line_bytes < size) {
	  ch = getc(mailfile); /* Macro, faster than  fgetc() ! */

	  if (ch == EOF)
	    break;

	  *c++ = ch;
	  ++line_bytes;

	  if (ch == '\n')
	    break;
	}
	*c = 0;	/* Actually this should NOT be needed.. */
	return line_bytes;
}
