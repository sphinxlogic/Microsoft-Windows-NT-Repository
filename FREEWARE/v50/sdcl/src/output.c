/*
**  Copyright Dick Munroe, 1985-2000.
**  Rights to distribution and modification are granted as specified in the GNU
**  Public License.
**
**	All functions within this file are for formatting and performing
**	the output of the DCL output code.  Included here are the
**	functions outdone(), outch(), emitstring(), emitlabel(),
**	emitqstring(), emittarget(), itoa(), reverse(), and genlab().
**
**	Also within this file is a function for outputting a
**	string (usually indicating an error) passed to it.  The
**	name of the function is errmsg().
**
** 0.001 Dick Munroe 03-Sep-90
**	Add a function to actually emit lines much longer than MAXCOL.
*/

#include <stdio.h>
#include "defs.h"

/* 
 *	MAXCOL is the maximum number of columns in the 
 *	output line.  MAXBUF is one greater to allow for
 *	appending a NULL character to make a string. 
 */

#define MAXCOL	80
#define MAXBUF  MAXCOL + 1

/*
 *	outbuf[] and outp are the output buffer and index 
 *	into same.  These two variables are used in building and 
 *	outputting the final output from the sdcl program.
 *
 *	External variable outfile is initialized within sdcl.c.
 *	It points to either stdout or to a user-specified output
 *	file.
 */

static char outbuf[MAXBUF];
static outp = (-1);
extern FILE *outfile;

void itoa(int, char*) ;
void reverse(char*) ;

void outdone()
/*
 *	This function flushes the contents of outbuf to stdout and
 *	resets outp to -1.  This is the only function within this
 *	file that makes a direct call to the fprintf() function and
 *	hence is the only function that produces any output.  It is
 *	used by most of the other functions within this file.
 */
    {
	/* Append a newline and NULL characters, output, and set outp. */

	outbuf[++outp] = '\n';
	outbuf[++outp] = '\0';
	fputs(outbuf, outfile );
	outp = (-1);
    }

void outch(c)
    char c;
/*	
 *	This function places its character input parameter into outbuf.
 *	Insertion of DCL continuation characters are also handled here.
 *	When a newline character is received by this function outbuf is
 *	flushed by a call to outdone().
 */
    {
	/* If c is a newline it is a signal to flush outbuf[]. */
	if (c == '\n')
		outdone();
	/* 
	 *	Otherwise if there are now two less than the maximum
	 *	characters currently in outbuf[] add a dcl continuation
	 *	character and flush the buffer.  Begin a new outbuf,
	 *	without a prepended dollar sign.  The first character
	 *	of this new outbuf[] will be the inputted character c.
	 */
	else if (outp == MAXCOL - 2) {
		outbuf[++outp] = '-';
		outdone();
		outbuf[++outp] = c;
	}
	/* Otherwise simply append c to outbuf[]. */
	else
		outbuf[++outp] = c;
    }

void emitstring(string)
    char *string;
/*
 *	The characters in string are placed in the output buffer
 *	one character at a time.  Function outch() does the
 *	necessary bookeeping to prevent too many characters
 *	being placed in outbuf[].
 */
    {
	char c;

	while (c = *string++)
		outch(c);
    }

void emitlabel(label)
    int label;
/*
 *	This function converts integer intput label into a string of
 *	characters.  To this string it appends  a colon, and a blank
 *	character.  This character label is then added to outbuf.
 */
    {
	/* 
	 *	Allow five characters for the number, one each for
	 *	the colon, blank, and NULL characters for a total
	 *	of eight characters in str.
	 */
	char str[8];

	/* Convert label from integer to character, placing result into str.*/
	itoa(label, str);

	/* Add colon, blank, and NULL characters to str. */
	str[5] = ':';
	str[6] = ' ';
	str[7] = '\0';

	/* Send the character version label to outbuf[]. */
	emitstring(str);
    }

void emittarget(label)
    int label;
/*
 *	This function converts integer intput label into a string of
 *	characters.  To this string it appends  a blank
 *	character.  This character label is then added to outbuf.
 *	This function is almost exactly like emitlabel except that
 *	no colon is appended to the character string.  This is
 *	used whenever a target must be specified after a goto is
 *	generated from within statements.c.
 */
    {
	/* 
	 *	Allow five characters for the number and one for
	 *	the NULL character for a total of six characters in str.
	 */
	char str[6];

	itoa(label, str);
	str[5] = '\0';
	emitstring(str);
    }

void emitqstring(str)
    char *str;
/*
 *	This procedure is called whenever a quoted string is to
 *	be output.  It insures that the string is not broken by
 *	a newline character.  It compares the length of the 
 *	string to be output to the amount of room currently remaining
 *	in outbuf.  If there isn't enough room in outbuf to contain
 *	the output string a dash is appended to outbuf to indicate
 *	line continuation.  Then a newline is appended and the string
 *	is sent to outbuf.
 */
    {
	int i,c ;

	/* 
	 *	Check to see if outbuf has enough room left to hold
	 *	the string that is to be output.  If not append a
	 *	dash and a newline character to outbuf and flush it
	 *	by calling emitstring.
	 */
	c = strlen(str) ;

	/*
	** If the quoted string will fit completely within the buffer,
	** transfer it and exit.
	*/

	if (c + outp + 3 < MAXCOL)
	{
	 	while (c = *str++)
			outch(c);
		return ;
	} ;

	/*
	** Flush the current buffer before emitting the string.  There is a
	** race between this logic and the logic in outch that prevents using
	** emitstring to continue and terminate the current line.
	*/

	outbuf[++outp] = '-' ;
	outbuf[++outp] = ' ' ;
	outdone() ;
	
	/*
	** If the string will fit completely within the buffer,
	** move it.
	*/

	if (c + 3 < MAXCOL)
	{
	 	while (c = *str++)
			outch(c);
		return ;
	} ;

	/* 
	** The string will not fit completely within the buffer,
	** output the part that won't fit and then move the rest.
	*/
	
	for ( ;
	     c >= MAXCOL - 3 ;
	     c-- )
	{
		i = *str++ ;
		fputc( i, outfile ) ;
	} ;

	while (c = *str++)
		outch(c);
    }

void emitlstring(str)
    char *str;
/*
 *	This procedure is called whenever a literal string is to
 *	be output.  It insures that the string is not broken by
 *	either a newline or continuation character.  
 */
    {
	int c;

	/* 
	** If there has been anything buffered, flush it and dump the
	** string to the output file without going through outbuf.
	*/

	if (outp != -1)
	{
		outdone() ;
	} ;

	/* Output the rest of the input string. */
	while (c = *str++)
		fputc(c, outfile);
    }

void itoa(n, str)
    int n;
    char str[];
/*
 *	Converts the integer n into its string representation.
 */
    {
	int i = 0, sign;

	/* Save the sign of n and make n positive if its negative. */
	if ((sign = n) < 0)
		n = -n;

	/* Generate the digits in reverse order.  */
	do
	{
		str[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	/* If n was negative add a minus sign , then append a NULL. */
	if (sign < 0)
		str[i++] = '-';
	str[i] = '\0';

	/* Reverse the string. */
	reverse(str);
    }

void reverse(str)
    char str[];
/* 
 *	Reverses the digits in an inputted string.  The reversal
 *	is done in place.
 */
    {
	int c, i, j;

	for (i = 0, j = strlen(str) - 1; i < j; i++, j--) {
		c = str[i];
		str[i] = str[j];
		str[j] = c;
	}
    }

int genlab()
/*
 *	This function increments a static integer. This will 
 *	generate a new value that can be used as a label
 *	during code generation.
 */
    {
	static int label = 23000;
	return(label++);
    }

void errmsg(string)
    char *string;
    {
	fputs(string, stderr);
    }
