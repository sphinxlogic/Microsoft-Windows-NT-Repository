
static char rcsid[] = "@(#)$Id: strings.c,v 5.2 1992/10/17 22:30:43 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.2 $   $State: Exp $
 *
 * 			Copyright (c) 1986, 1987 Dave Taylor
 * 			Copyright (c) 1988, 1989, 1990, 1991 USENET Community Trust
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: strings.c,v $
 * Revision 5.2  1992/10/17  22:30:43  syd
 * Force text arithimetic into unsigned for those systems
 * with signed chars
 * From: Marius Olafsson <marius@rhi.hi.is>
 *
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** This file contains all the string oriented functions for the
    ELM Mailer, and lots of other generally useful string functions! 

    For BSD systems, this file also includes the function "tolower"
    to translate the given character from upper case to lower case.

**/

#include "headers.h"
#include "s_elm.h"
#include <ctype.h>

#ifdef BSD
#undef tolower
#undef toupper
#endif

/** forward declarations **/

char *format_long(), *strip_commas(), *tail_of_string(),
     *get_token(), *strip_parens(), *argv_zero(), *strcpy(), *strncpy();

char *index();


copy_sans_escape(dest, source, len)
unsigned char *dest, *source;
int  len;
{
	/** this performs the same function that strncpy() does, but
	    also will translate any escape character to a printable
	    format (e.g. ^(char value + 32))
	**/

	register int i = 0, j = 0;

	while (i < len && j < len && source[i] != '\0') {
	  if (iscntrl(source[i]) && source[i] != '\t') {
	     dest[j++] = '^';
	     if (j < len)
		dest[j++] = source[i++] + 'A' - 1;
	     else
		break;
	  }
	  else
	    dest[j++] = source[i++];
	}

	dest[j] = '\0';
}

char *format_long(inbuff, init_len)
char *inbuff;
int   init_len;
{
	/** Return buffer with \n\t sequences added at each point where it 
	    would be more than 80 chars long.  It only allows the breaks at 
	    legal points (ie commas followed by white spaces).  init-len is 
	    the characters already on the first line...  Changed so that if 
            this is called while mailing without the overhead of "elm", it'll 
            include "\r\n\t" instead.
	    Changed to use ',' as a separator and to REPLACE it after it's
	    found in the output stream...
	**/

	static char ret_buffer[VERY_LONG_STRING];
	register int iindex = 0, current_length = 0, depth=15, i, len;
	char     buffer[VERY_LONG_STRING];
	char     *word, *bufptr;

	strcpy(buffer, inbuff);

	bufptr = (char *) buffer;

	current_length = init_len + 2;	/* for luck */

	while ((word = get_token(bufptr,",", depth)) != NULL) {

	    /* first, decide what sort of separator we need, if any... */

	  if (strlen(word) + current_length > 80) {
	    if (iindex > 0) {
	      ret_buffer[iindex++] = ',';	/* close 'er up, doctor! */
	      ret_buffer[iindex++] = '\n';
	      ret_buffer[iindex++] = '\t';
	    }
	    
	    /* now add this pup! */

	    for (i=(word[0] == ' '? 1:0), len = strlen(word); i<len; i++)
	      ret_buffer[iindex++] = word[i];
	    current_length = len + 8;	/* 8 = TAB */
	  }

	  else {	/* just add this address to the list.. */

	    if (iindex > 0) {
	      ret_buffer[iindex++] = ',';	/* comma added! */
	      ret_buffer[iindex++] = ' ';
	      current_length += 2;
	    }
	    for (i=(word[0] == ' '? 1:0), len = strlen(word); i<len; i++)
	      ret_buffer[iindex++] = word[i];
	    current_length += len;
	  }
	
	  bufptr = NULL;
	}
	
	ret_buffer[iindex] = '\0';

	return( (char *) ret_buffer);
}

char *strip_commas(string)
char *string;
{
	/** return string with all commas changed to spaces.  This IS
	    destructive and will permanently change the input string.. **/

	register char *strptr = string;
	register int len;

	while (*strptr) {
	  len = len_next_part(strptr);
	  if (len == 1 && *strptr == COMMA)
	    *strptr = SPACE;
	  strptr += len;
	}

	return( (char *) string);
}

split_word(buffer, first, rest)
char *buffer, *first, *rest;
{
	int len;
	/** Rip the buffer into first word and rest of word, translating it
	    all to lower case as we go along..
	**/

	/** skip leading white space, just in case.. **/

	while(whitespace(*buffer)) buffer++;

	/** now copy into 'first' until we hit white space or EOLN **/

	while (*buffer) {
	  len = len_next_part(buffer);
	  if (len == 1 && isspace(*buffer))
	    break;

	  while (--len >= 0) {
	    if (islower(*buffer))
	      *first = *buffer;
	    else
	      *first = tolower(*buffer);
	    buffer++;
	    first++;
	  }
	}

	*first = '\0';
	
	while (whitespace(*buffer)) buffer++;

/*
 *	no need to check rest of buffer for quoted strings, we are taking
 *	it all anyway.
 */

	for (; *buffer; buffer++, rest++)
	  if (islower(*buffer))
	    *rest = *buffer;
	  else
	    *rest = tolower(*buffer);

	*rest = '\0';

	return;
}

char *tail_of_string(string, maxchars)
char *string;
int  maxchars;
{
	/** Return a string that is the last 'maxchars' characters of the
	    given string.  This is only used if the first word of the string
	    is longer than maxchars, else it will return what is given to
	    it... 
	**/

	static char buffer[SLEN];
	register char *s;
	register int iindex, i, len;

	iindex=0;
	len = strlen(string);
	s = string;

	while (iindex < len) {
	  i = len_next_part(s);
	  if (i == 1 && isspace(*s))
	    break;
	  iindex += i;
	  s += i;
	}

	if (iindex < maxchars) {
	  strncpy(buffer, string, maxchars-2);	/* word too short */
	  buffer[maxchars-2] = '.';
	  buffer[maxchars-1] = '.';
	  buffer[maxchars]   = '.';
	  buffer[maxchars+1] = '\0';
	} 
	else {
	  i = maxchars;
	  buffer[i--] = '\0';
	  while (i > 1) 
	    buffer[i--] = string[iindex--];
	  buffer[2] = '.';
	  buffer[1] = '.';
	  buffer[0] = '.';
	}

	return( (char *) buffer);
}

int
get_word(buffer, start, word)
char *buffer, *word;
int start;
{
	/**	return next word in buffer, starting at 'start'.
		delimiter is space or end-of-line.  Returns the
		location of the next word, or -1 if returning
		the last word in the buffer.  -2 indicates empty
		buffer!  **/

	register int len;
	register char *p = buffer + start;

	while (*p == ' ' && *p != '\0')
	  p++;

	if (*p == '\0') return(-2);	 /* nothing IN buffer! */

	while (*p != '\0') {
	  len = len_next_part(p);
	  if (len == 1 && isspace(*p))
	    break;

	  while (--len >= 0)
	    *word++ = *p++;
	}

	*word = '\0';
	return(p - buffer);
}

Centerline(line, string)
int line;
char *string;
{
	/** Output 'string' on the given line, centered. **/

	register int length, col;

	length = strlen(string);

	if (length > COLUMNS)
	  col = 0;
	else
	  col = (COLUMNS - length) / 2;

	PutLine0(line, col, string);
}

char *argv_zero(string)
char *string;
{
	/** given a string of the form "/something/name" return a
	    string of the form "name"... **/

	static char buffer[NLEN];
	register int i, j=0;

	for (i=strlen(string)-1; string[i] != '/'; i--)
	  buffer[j++] = string[i];
	buffer[j] = '\0';

	reverse(buffer);

	return( (char *) buffer);
}

#define MAX_RECURSION		20		/* up to 20 deep recursion */

char *get_token(source, keys, depth)
char *source, *keys;
int   depth;
{
	/** This function is similar to strtok() (see "opt_utils")
	    but allows nesting of calls via pointers... 
	**/

	register int  last_ch;
	static   char *buffers[MAX_RECURSION];
	char     *return_value, *sourceptr;

	if (depth > MAX_RECURSION) {
	   error1(catgets(elm_msg_cat, ElmSet, ElmGetTokenOverNested,
		"Get_token calls nested greater than %d deep!"), 
		  MAX_RECURSION);
	   emergency_exit();
	}

	if (source != NULL)
	  buffers[depth] = source;
	
	sourceptr = buffers[depth];
	
	if (*sourceptr == '\0') 
	  return(NULL);		/* we hit end-of-string last time!? */

	sourceptr += qstrspn(sourceptr, keys);	  /* skip the bad.. */
	
	if (*sourceptr == '\0') {
	  buffers[depth] = sourceptr;
	  return(NULL);			/* we've hit end-of-string   */
	}

	last_ch = qstrcspn(sourceptr, keys);   /* end of good stuff   */

	return_value = sourceptr;	      /* and get the ret     */

	sourceptr += last_ch;		      /* ...value            */

	if (*sourceptr != '\0')		/** don't forget if we're at end! **/
	  sourceptr++;			      
	
	return_value[last_ch] = '\0';	      /* ..ending right      */

	buffers[depth] = sourceptr;	      /* save this, mate!    */

	return((char *) return_value);	     /* and we're outta here! */
}


quote_args(out_string,in_string)
register char *out_string, *in_string;
{
	/** Copy from "in_string" to "out_string", collapsing multiple
	    white space and quoting each word.  Returns a pointer to
	    the resulting word.
	**/

	int empty_string = TRUE, len;

	while ( *in_string != '\0' ) {

	    /**	If this is a space then advance to the start of the next word.
		Otherwise, copy through the word surrounded by quotes.
	    **/

	    if ( isspace(*in_string) ) {
		while ( isspace(*in_string) )
			++in_string;
	    } else {
		*out_string++ = '"';

		while ( *in_string != '\0') {
		  len = len_next_part(in_string);
		  if (len == 1 && isspace(*in_string))
		    break;

		  while (--len >= 0) {
		    if (*in_string == '"' || *in_string == '\\' || *in_string == '$')
		      *out_string++ = '\\';
		    *out_string++ = *in_string++;
		  }
		}
		*out_string++ = '"';
		*out_string++ = ' ';
		empty_string = FALSE;
	    }

    }

    if ( !empty_string )
	--out_string;
    *out_string = '\0';
}

