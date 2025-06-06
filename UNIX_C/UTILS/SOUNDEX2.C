From riacs!eos!agate!violet.berkeley.edu!dean Mon Jan 16 14:11:47 PST 1989

Following is an implementation of the Soundex keying routine.  It uses
the first letter and a function of the next few letters of a name to
derive a four-character key.  The algorithm tends to cluster names
which sound similar.  It was originally developed by the Bureau of the
Census to let them find an individual's record even if the name is
slightly misspelled.

There was another version on the net, but when I looked at it, the 
implementation seemed excessively arcane.  This routine is reasonably
straightforward, documented, and includes test data for verification.

Released to the public domain.  Have fun.

-Dean

---- cut (and check for .signature at the end) --- cut --- cut --- cut ---
/* Note: code text edited with a 4-character tab stop.
 *
 * This implementation of the Soundex algorithm is released to the public
 * domain: anyone may use it for any purpose.  See if I care.
 *
 * N. Dean Pentcheff
 * 1/13/89
 * Dept. of Zoology
 * University of California
 * Berkeley, CA  94720
 * dean@violet.berkeley.edu
 *
 * char * soundex( char * )
 *
 * Given as argument: Pointer to a character string.
 * Returns: Pointer to a static string, 4 characters long, plus a terminal
 *    '\0'.  This string is the Soundex key for the argument string.
 * Side effects and limitations:
 *    Does not clobber the string passed in as the argument.
 *    No limit on argument string length.
 *    Assumes a character set with continuously ascending and contiguous
 *       letters within each case and within the digits (e.g. this works for
 *       ASCII and bombs in EBCDIC.  But then, most things do.).
 * Reference: Adapted from Knuth, D.E. (1973) The art of computer programming;
 *    Volume 3: Sorting and searching.  Addison-Wesley Publishing Company:
 *    Reading, Mass. Page 392.
 * Special cases:
 *    Leading or embedded spaces, numerals, or punctuation are squeezed out
 *       before encoding begins.
 *    Null strings or those with no encodable letters return the code 'Z000'.
 * Test data from Knuth (1973):
 *    Euler   Gauss   Hilbert Knuth   Lloyd   Lukasiewicz
 *    E460    G200    H416    K530    L300    L222
 */

#include	<string.h>
#include	<ctype.h>

char *
soundex(in)
	char	*in;
{
	static	int code[] =
		{  0,1,2,3,0,1,2,0,0,2,2,4,5,5,0,1,2,6,2,3,0,1,0,2,0,2 };
		/* a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z */
	static	char key[5];
	register	char ch;
	register	int last;
	register	int count;

	/* Set up default key, complete with trailing '0's */
	key[0] = 'Z';
	key[1] = key[2] = key[3] = '0';
	key[4] = '\0';

	/* Advance to the first letter.  If none present, return default key */
	while (*in != '\0'  &&  !isalpha(*in))
		++in;
    if (*in == '\0')
		return(key);

	/* Pull out the first letter, uppercase it, and set up for main loop */
	key[0] = islower(*in) ? toupper(*in) : *in;
	last = code[key[0] - 'A'];
	++in;

	/* Scan rest of string, stop at end of string or when the key is full */
	for (count = 1;  count < 4  &&  *in != '\0';  ++in) {
		/* If non-alpha, ignore the character altogether */
		if (isalpha(*in)) {
			ch = isupper(*in) ? tolower(*in) : *in;
			/* Fold together adjacent letters sharing the same code */
            if (last != code[ch - 'a']) {
				last = code[ch - 'a'];
				/* Ignore code==0 letters except as separators */
				if (last != 0)
					key[count++] = '0' + last;
			}
		}
	}

	return(key);
}

#ifdef	TESTPROG
/*
 * If compiled with -DTESTPROG, main() will print back the key for each
 * line from stdin.
 */
#include	<stdio.h>
void
main()
{
	char	instring[80];

	while (fgets(instring, 79, stdin) != (char *)NULL)
		printf("%s\n", soundex(instring));
}
#endif	TESTPROG

Dean Pentcheff        dean@violet.berkeley.edu
----------------------------------------------
As an adolescent I aspired to lasting fame, I craved factual certainty, and I
thirsted for a meaningful vision of human life - so I became a scientist.  This
is like becoming an archbishop so you can meet girls.               M. Cartmill


