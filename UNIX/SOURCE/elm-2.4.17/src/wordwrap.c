/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
 *
 *			Copyright (c) 1988-1992 USENET Community Trust
 *			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: wordwrap.c,v $
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/***  Routines to wrap lines when using the "builtin" editor

***/

#include "headers.h"
#include <errno.h>
#include <ctype.h>

#ifdef BSD
#  undef tolower
#endif

extern int errno;		/* system error number */

unsigned alarm();

#define isstopchar(c)		(c == ' ' || c == '\t' || c == '/')
#define isslash(c)		(c == '/')
#define erase_a_char()		{ Writechar(BACKSPACE); Writechar(' '); \
			          Writechar(BACKSPACE); fflush(stdout); }

int
wrapped_enter(string, tail, x, y, edit_fd, append_current)
char *string, *tail;
int  x,y, *append_current;
FILE *edit_fd;

{
	/** This will display the string on the screen and allow the user to
	    either accept it (by pressing RETURN) or alter it according to
	    what the user types.   The various flags are:
	         string    is the buffer to use (with optional initial value)
		 tail	   contains the portion of input to be wrapped to the
			   next line
	 	 x,y	   is the location we're at on the screen (-1,-1 means
			   that we can't use this info and need to find out
			   the current location)
		 append_current  means that we have an initial string and that
			   the cursor should be placed at the END of the line,
			   not the beginning (the default).
	      
	    If we hit an interrupt or EOF we'll return non-zero.
	**/

	int ch, wrapcolumn = 70, iindex = 0;
	register int ch_count = 0, escaped = OFF;
	long newpos, pos;
	char line[SLEN];

	clearerr(stdin);

	if(!(x >=0 && y >= 0))
	  GetXYLocation(&x, &y);
	PutLine1(x, y, "%s", string);	

	CleartoEOLN();

	if (! *append_current) {
	  MoveCursor(x,y);
	}
	else
	  iindex = strlen(string);

	if (cursor_control)
	  transmit_functions(OFF);

	/** now we have the screen as we want it and the cursor in the 
	    right place, we can loop around on the input and return the
	    string as soon as the user presses <RETURN> or the line wraps.
	**/

	do {
	  ch = getchar();

	  if (ch == ctrl('D') || ch == EOF) {		/* we've hit EOF */
	    if (cursor_control)
	      transmit_functions(ON);
	    *append_current = 0;
	    return(1);
	  }

	  if (ch_count++ == 0) {
	    if (ch == '\n' || ch == '\r') {
	      if (cursor_control)
	        transmit_functions(ON);
	      *append_current = 0;
	      return(0);
	    }
	    else if (! *append_current) {
	      CleartoEOLN();
	      iindex = (*append_current? strlen(string) : 0);
	    }
	  }

	  /* the following is converted from a case statement to
	     allow the variable characters (backspace, kill_line
	     and break) to be processed.  Case statements in
	     C require constants as labels, so it failed ...
	  */

	  if (ch == backspace) {
	    escaped = OFF;
	    if (iindex > 0) {
	      Writechar(BACKSPACE);
  	      iindex--;
	      Writechar(' ');
	      Writechar(BACKSPACE);

#ifdef FTRUNCATE

	    } else { /** backspace to end of previous line **/

	      fflush(edit_fd);
	      if ((pos = ftell(edit_fd)) <= 0L) { /** no previous line **/
		Writechar('\007');

	      } else {

		/** get the last 256 bytes written **/
		if ((newpos = pos - 256L) <= 0L) newpos = 0;
		(void) fseek(edit_fd, newpos, 0L);
		(void) fread(line, sizeof(*line), (int) (pos-newpos),
		             edit_fd);
		pos--;

		/** the last char in line should be '\n'
			change it to null **/
		if (line[(int) (pos-newpos)] == '\n')
		  line[(int) (pos-newpos)] = '\0';

		/** find the end of the previous line ('\n') **/
		for (pos--; pos > newpos && line[(int) (pos-newpos)] != '\n';
			pos--);
		/** check to see if this was the first line in the file **/
		if (line[(int) (pos-newpos)] == '\n') /** no - it wasn't **/
		  pos++;
		(void) strcpy(string, &line[(int) (pos-newpos)]);
		line[(int) (pos-newpos)] = '\0';

		/** truncate the file to the current position
			THIS WILL NOT WORK ON SYS-V **/
		(void) fseek(edit_fd, newpos, 0L);
		(void) fputs(line, edit_fd);
		fflush(edit_fd);
		(void) ftruncate(fileno(edit_fd), (int) ftell(edit_fd));
		(void) fseek(edit_fd, ftell(edit_fd), 0L);

		/** rewrite line on screen and continue working **/
		GetXYLocation(&x, &y);
		if (x > 0) x--;
		PutLine1(x, y, "%s", string);
		CleartoEOLN();
		iindex = strlen(string);
	      }

#endif

	    }
	    fflush(stdout);
	  }
	  else if (ch == EOF || ch == '\n' || ch == '\r') {
	    escaped = OFF;
	    string[iindex] = '\0';
	    if (cursor_control)
	      transmit_functions(ON);
	    *append_current = 0;
	    return(0);
	  }
	  else if (ch == ctrl('W')) {	/* back up a word! */
	    escaped = OFF;
	    if (iindex == 0)
	      continue;		/* no point staying here.. */
	    iindex--;
	    if (isslash(string[iindex])) {
	      erase_a_char();
	    }
	    else {
	      while (iindex >= 0 && isspace(string[iindex])) {
	        iindex--;
	        erase_a_char();
	      }

	      while (iindex >= 0 && ! isstopchar(string[iindex])) {
	        iindex--;
	        erase_a_char();
	      }
	      iindex++;	/* and make sure we point at the first AVAILABLE slot */
	    }
	  }
	  else if (ch == ctrl('R')) {
	    escaped = OFF;
	    string[iindex] = '\0';
	    PutLine1(x,y, "%s", string);	
	    CleartoEOLN();
	  }
	  else if (!escaped && ch == kill_line) {
	    /* needed to test if escaped since kill_line character could
	     * be a desired valid printing character */
	    escaped = OFF;
	    MoveCursor(x,y);
	    CleartoEOLN();
	    iindex = 0;
	  }
	  else if (ch == '\0') {
	    escaped = OFF;
	    if (cursor_control)
	      transmit_functions(ON);
	    fflush(stdin); 	/* remove extraneous chars, if any */
	    string[0] = '\0'; /* clean up string, and... */
	    *append_current = 0;
	    return(-1);
	  }
	  else if (!isprint(ch)) {
	    /* non-printing character - warn with bell*/
	    /* don't turn off escaping backslash since current character
	     * doesn't "use it up".
	     */
	    Writechar('\007');
	  }
	  else {  /* default case */
	      if(escaped && (ch == backspace || ch == kill_line)) {
		/* if last character was a backslash,
		 * and if this character is escapable
		 * simply write this character over it even if
		 * this character is a backslash.
		 */
		Writechar(BACKSPACE);
		iindex--;
		string[iindex++] = ch;
		Writechar(ch);
	        escaped = OFF;
	      } else {
		string[iindex++] = ch;
		Writechar(ch);
		escaped = ( ch == '\\' ? ON : OFF);
	      }
	  }
	} while (iindex < wrapcolumn);

	string[iindex] = '\0';
	*append_current = line_wrap(string,tail,&iindex);

	if (cursor_control)
	  transmit_functions(ON);
	return(0);
}

int
line_wrap(string,tail,count)
char *string;	/* The string to be wrapped */
char *tail;	/* The part of the string which is wrapped */
int *count;	/* Offset of string terminator */
{
	/** This will check for line wrap.  If the line was wrapped,
	    it will back up to white space (if possible), write the
	    shortened line, and put the remainder at the beginning
	    of the string.  Returns 1 if wrapped, 0 if not.
	**/

	int n = *count, wrapcolumn = 70;
	int i, j;

	if (*count < wrapcolumn) return(0);

	/* Back up past the wrapcolumn point */
	for (; n > wrapcolumn; --n);

	/* Look for a space */
	while (n && !isstopchar(string[n]))
	  --n;

	/* If no break found, return no wrap */
	if (!n) return(0);

	/* Copy part to be wrapped */
	for (i=0,j=n+1;j<=*count;tail[i++]=string[j++]);

	/* Skip the break character and any whitespace */
	while (n && isstopchar(string[n]))
	  --n;
	if (!n) return(0);
	++n; /* Move back into the whitespace */

	/* Erase the stuff that will wrap */
	while (*count > n) {
	  erase_a_char();
	  --(*count);
	}
	string[*count] = '\0';
	return(1);
}
