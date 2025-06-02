/*
 * mwrescue.c -- rescue a damaged MacWrite file (version 3.0 and later)
 *
 * Versions of MacWrite after 3.0 and before 4.0 have been unreliable.
 * This program decodes damaged MacWrite files and recovers the text in
 * them (once you've uploaded them to Unix).  Someone should get it
 * running on the native Mac.
 *
 * It is currently very simple since it treats the entire file as compacted 
 * text.  However, MacWrite files can also contain some uncompacted text 
 * and some formatting information.  The uncompacted text can be retrieved
 * with the Unix strings command.  The formatting information is output by
 * this program as nonsense text.  
 *
 * The text retrieved by this program emerges in the order it is stored in 
 * the file by MacWrite, which is not necessarily the order in which it is 
 * supposed to appear in the document.
 *
 * As primitive as this program is, I'm releasing it because:
 *	1) I hope someone will improve it.
 *	2) I've found that panicked students are more than glad to get
 *		their papers back even if the paragraphs are in random
 *		order with garbage text in between.
 *
 * History:
 *
 * 	5/11/85		Winkler		Created
 *
 */

#include <stdio.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

/* Compressed characters for English.  MacWrite stores this string
 * in the resource fork of the document.  A future improvement would
 * be to find the string there (presumably in file.rsrc on Unix) rather 
 * than assume this one.
 */
char compchars[] = {' ', 'e', 't', 'n', 'r', 'o', 'a', 'i', 's',
			'd', 'l', 'h', 'c', 'f', 'p', '\n' } ;

main()
/* read encoded file from stdin and write text to stdout */
{
	int c ;

	do
	{
		c = getnibble() ;
		if ( c != 0x0000000f ) macputchar( compchars[c] ) ;
		else 
		{
			int c2 ;

			c = getnibble() ; c2 = getnibble() ;
			/* need variables because C doesn't guarantee
			 * the order of evaluation of arguments.  So if
			 * c and c2 below were replaced by getnibble()'s
			 * there would be no way of knowing which of the
			 * next two nibbles on stdin would be shifted
			 */
			macputchar( (c << 4) + c2 ) ;
		}
	}
	while ( TRUE ) ; /* infinite loop. getnibble() exits on EOF */
}

macputchar( c )
/* translate returns to newlines and suppress nonprinting characters */
	int c;
{
	if ( isprint(c) )
	{
		if ( c != '\r' ) putchar(c) ;
		else putchar('\n');
	}
}

getnibble()
/* return the next nibble from stdin.  exits on EOF. */
{
	static int got = FALSE ;
	static int c ;

	if ( ! got )
	{
		if ( (c = getchar()) == EOF ) exit(1) ;
		got = TRUE ;
		return ( ( c >> 4 ) & 0x0000000f) ;
	}
	else
	{
		got = FALSE ;
		return (c & 0x0000000f) ;
	}
}
