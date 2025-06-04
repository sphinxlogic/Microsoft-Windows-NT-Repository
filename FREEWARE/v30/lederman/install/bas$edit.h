#ifndef __BAS$EDIT_LOADED
#define __BAS$EDIT_LOADED	1
#endif

/*
   Define a prototype and values for the VAX/BASIC EDIT$ function.

   The EDIT$ function performs one or more string editing functions,
   depending on the value of its integer argument.

   Example (in BASIC)

   New_string$ = EDIT$(Old_string$, 48%)

   Example (in C)

   (void) BAS$EDIT (&New_String, &Old_String, Edit-Exp);

   New_String and Old_String are string descriptors passed by address
   (reference).

   Edit-Exp is an integer value specifying one or more edit actions
   from the set listed below, which can be ANDed together.

   There does not appear to be a return status to check.

   All of the definitions here are my own and NOT Digital's.

   Bart Z. Lederman	15-Jun-1992
*/

void BAS$EDIT ();

/*			Values		Effect				*/

#define BAS$M_NOPARITY	1	/* Trim parity bits			*/
#define BAS$M_COLLAPSE	2	/* Discard all spaces and tabs		*/
#define BAS$M_DISFORM	4	/* Discard characters: CR, LF, FF, ESC,	*/
				/* RUBOUT, and NULL			*/
#define BAS$M_DISLEAD	8	/* Discard leading spaces and tabs	*/
#define BAS$M_COMPRESS	16	/* Reduce spaces and tabs to one space	*/
#define BAS$M_UPCASE	32	/* Convert lowercase to uppercase	*/
#define BAS$M_BRACKET	64	/* Convert [ to ( and ] to )		*/
#define BAS$M_DISTRAIL	128	/* Discard trailing spaces and tabs	*/
#define BAS$M_KEEPQUOTE	256	/* Do not alter characters inside	*/
				/* quotes				*/
