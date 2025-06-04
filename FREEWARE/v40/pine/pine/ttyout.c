#if !defined(lint) && !defined(DOS)
static char rcsid[] = "$Id: ttyout.c,v 4.15 1993/09/29 22:32:47 mikes Exp $";
#endif
/*----------------------------------------------------------------------

            T H E    P I N E    M A I L   S Y S T E M

   Laurence Lundblade and Mike Seibel
   Networks and Distributed Computing
   Computing and Communications
   University of Washington
   Administration Builiding, AG-44
   Seattle, Washington, 98195, USA
   Internet: lgl@CAC.Washington.EDU
             mikes@CAC.Washington.EDU

   Please address all bugs and comments to "pine-bugs@cac.washington.edu"

   Copyright 1989-1993  University of Washington

    Permission to use, copy, modify, and distribute this software and its
   documentation for any purpose and without fee to the University of
   Washington is hereby granted, provided that the above copyright notice
   appears in all copies and that both the above copyright notice and this
   permission notice appear in supporting documentation, and that the name
   of the University of Washington not be used in advertising or publicity
   pertaining to distribution of the software without specific, written
   prior permission.  This software is made available "as is", and
   THE UNIVERSITY OF WASHINGTON DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
   WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
   NO EVENT SHALL THE UNIVERSITY OF WASHINGTON BE LIABLE FOR ANY SPECIAL,
   INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, TORT
   (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF OR IN CONNECTION
   WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  
   Pine and Pico are trademarks of the University of Washington.
   No commercial use of these trademarks may be made without prior
   written permission of the University of Washington.

   Pine is in part based on The Elm Mail System:
    ***********************************************************************
    *  The Elm Mail System  -  Revision: 2.13                             *
    *                                                                     *
    * 			Copyright (c) 1986, 1987 Dave Taylor              *
    * 			Copyright (c) 1988, 1989 USENET Community Trust   *
    ***********************************************************************
 

  ----------------------------------------------------------------------*/

/*======================================================================
       ttyout.c
       Routines for painting the screen
          - figure out what the terminal type is
          - deal with screen size changes
          - save special output sequences
          - the usual screen clearing, cursor addressing and scrolling


     This library gives programs the ability to easily access the
     termcap information and write screen oriented and raw input
     programs.  The routines can be called as needed, except that
     to use the cursor / screen routines there must be a call to
     InitScreen() first.  The 'Raw' input routine can be used
     independently, however. (Elm comment)

     Not sure what the original source of this code was. It got to be
     here as part of ELM. It has been changed significantly from the
     ELM version to be more robust in the face of inconsistent terminal
     autowrap behaviour. Also, the unused functions were removed, it was
     made to pay attention to the window size, and some code was made nicer
     (in my opinion anyways). It also outputs the terminal initialization
     strings and provides for minimal scrolling and detects terminals
     with out enough capabilities. (Pine comment, 1990)


This code used to pay attention to the "am" auto margin and "xn"
new line glitch fields, but they were so often incorrect because many
terminals can be configured to do either that we've taken it out. It
now assumes it dosn't know where the cursor is after outputing in the
80th column.
*/



#include "headers.h"

#ifndef DOS /* Beginning of giant ifdef to switch between UNIX and DOS */

#ifdef HAVE_TERMIOS
#include <termios.h>
#else
#ifdef HAVE_TERMIO
#include <termio.h>
#else
#include <sgtty.h>
#endif
#endif

#ifdef _
#include <sys/tty.h>
#endif


#ifdef ANSI
static void moveabsolute(int, int);
static void CursorUp(int);
static void CursorDown(int);
static void CursorLeft(int);
static void CursorRight(int);
#else
static void moveabsolute();
static void CursorUp();
static void CursorDown();
static void CursorLeft();
static void CursorRight();
#endif


#define FARAWAY 1000


static char *_clearscreen, *_moveto, *_up, *_down, *_right, *_left,
            *_setinverse, *_clearinverse,
            *_setunderline, *_clearunderline,
            *_setbold,     *_clearbold,
            *_cleartoeoln, *_cleartoeos,
            *_startinsert, *_endinsert, *_insertchar, *_deletechar,
            *_deleteline, *_insertline,
            *_scrollregion, *_scrollup, *_scrolldown,
            *_termcap_init, *_termcap_end;
static int   _lines, _columns;
char  ttermname[40];
#ifndef USE_TERMINFO
static char  _terminal[1024];         /* Storage for terminal entry */
static char  _capabilities[1024];     /* String for cursor motion */
static char *ptr = _capabilities;     /* for buffering         */

char  *tgetstr();     		       /* Get termcap capability */

#endif

static int   _line  = FARAWAY;
static int   _col   = FARAWAY;
static enum  {NoScroll,UseScrollRegion,InsertDelete} _scrollmode;

char  *tgoto();				/* and the goto stuff    */



/*----------------------------------------------------------------------
      Initialize the screen for output, set terminal type, etc

   Args: tt -- Pointer to variable to store the tty output structure.

 Result:  terminal size is discovered and set pine state
          termcap entry is fetched and stored in local variables
          make sure terminal has adequate capabilites
          evaluate scrolling situation
          returns status of indicating the state of the screen/termcap entry

      Returns:
        -1 indicating no terminal name associated with this shell,
        -2..-n  No termcap for this terminal type known
	-3 Can't open termcap file 
        -4 Terminal not powerful enough - missing clear to eoln or screen
	                                       or cursor motion

  ----*/
int
config_screen(tt)
     struct ttyo **tt;
{
    struct ttyo   *ttyo;
#ifdef USE_TERMINFO
    int setupterm();
    char *tigetstr();
    char *getenv(), *ttnm;
    int err;

    *tt = NULL;
    ttnm = getenv ("TERM");
    if (!ttnm) return -1;
    strcpy (ttermname, ttnm);
    setupterm (ttermname, 1 /* (ignored) */, &err);
    switch (err) {
    case -1 : return -3;
    case 0 : return -2;
    }

    ttyo = (struct ttyo *)fs_get(sizeof (struct ttyo));

    _line  =  0;		/* where are we right now?? */
    _col   =  0;		/* assume zero, zero...     */

    /* load in all those pesky values */
    _clearscreen       = tigetstr("clear");
    _moveto            = tigetstr("cup");
    _up                = tigetstr("cuu1");
    _down              = tigetstr("cud1");
    _right             = tigetstr("cuf1");
    _left              = tigetstr("cub1");
    _setinverse        = tigetstr("smso");
    _clearinverse      = tigetstr("rmso");
    _setunderline      = tigetstr("smul");
    _clearunderline    = tigetstr("rmul");
    _setbold           = tigetstr("bold");
    _clearbold         = tigetstr("sgr0");
    _cleartoeoln       = tigetstr("el");
    _cleartoeos        = tigetstr("ed");
    _deletechar        = tigetstr("dch1");
    _insertchar        = tigetstr("ich1");
    _startinsert       = tigetstr("smir");
    _endinsert         = tigetstr("rmir");
    _deleteline        = tigetstr("dl1");
    _insertline        = tigetstr("il1");
    _scrollregion      = tigetstr("csr");
    _scrolldown        = tigetstr("ind");
    _scrollup          = tigetstr("ri");
    _termcap_init      = tigetstr("smcup");
    _termcap_end       = tigetstr("rmcup");
    _lines	       = tigetnum("lines");
    _columns	       = tigetnum("cols");
#else
    int  tgetent(),      /* get termcap entry */
         err;

    *tt = NULL;
    if (getenv("TERM") == NULL) return(-1);

    if (strcpy(ttermname, getenv("TERM")) == NULL)
    	return(-1);

    if ((err = tgetent(_terminal, ttermname)) != 1)
    	return(err-2);

    ttyo = (struct ttyo *)fs_get(sizeof (struct ttyo));

    _line  =  0;		/* where are we right now?? */
    _col   =  0;		/* assume zero, zero...     */

    /* load in all those pesky values */
    ptr                = _capabilities;
    _clearscreen       = tgetstr("cl", &ptr);
    _moveto            = tgetstr("cm", &ptr);
    _up                = tgetstr("up", &ptr);
    _down              = tgetstr("do", &ptr);
    _right             = tgetstr("nd", &ptr);
    _left              = tgetstr("bs", &ptr);
    _setinverse        = tgetstr("so", &ptr);
    _clearinverse      = tgetstr("se", &ptr);
    _setunderline      = tgetstr("us", &ptr);
    _clearunderline    = tgetstr("ue", &ptr);
    _setbold           = tgetstr("md", &ptr);
    _clearbold         = tgetstr("me", &ptr);
    _cleartoeoln       = tgetstr("ce", &ptr);
    _cleartoeos        = tgetstr("cd", &ptr);
    _deletechar        = tgetstr("dc", &ptr);
    _insertchar        = tgetstr("ic", &ptr);
    _startinsert       = tgetstr("im", &ptr);
    _endinsert         = tgetstr("ei", &ptr);
    _deleteline        = tgetstr("dl", &ptr);
    _insertline        = tgetstr("al", &ptr);
    _scrollregion      = tgetstr("cs", &ptr);
    _scrolldown        = tgetstr("sf", &ptr);
    _scrollup          = tgetstr("sr", &ptr);
    _termcap_init      = tgetstr("ti", &ptr);
    _termcap_end       = tgetstr("te", &ptr);
    _lines	       = tgetnum("li");
    _columns	       = tgetnum("co");
#endif

    if(_lines == -1) {
        /* tgetnum failed */
        _lines = DEFAULT_LINES_ON_TERMINAL;
    }
    if(_columns == -1) {
        /* tgetunm failed */
        _columns = DEFAULT_COLUMNS_ON_TERMINAL;
    }

    get_windsize(ttyo);

    if(_columns > MAX_SCREEN_COLS || _lines > MAX_SCREEN_ROWS) {
        ttyo->screen_cols = MAX_SCREEN_COLS;
        ttyo->screen_rows = MAX_SCREEN_ROWS;
    }
    else {
        ttyo->screen_cols = _columns ;
        ttyo->screen_rows = _lines ;
    }


    /*---- Make sure this terminal has the capability.
        All we need is cursor address, clear line, and 
        reverse video.
      ---*/
    if(_moveto == NULL || _cleartoeoln == NULL ||
       _setinverse == NULL || _clearinverse == NULL) {
          return(-4);
    }

    dprint(1, (debugfile, "Terminal type: %s\n", ttermname));

    /*------ Figure out scrolling mode -----*/
    if(_scrollregion != NULL && _scrollregion[0] != '\0' &&
    	  _scrollup != NULL && _scrollup[0] != '\0'){
        _scrollmode = UseScrollRegion;
    } else if(_insertline != NULL && _insertline[0] != '\0' &&
       _deleteline != NULL && _deleteline[0] != '\0') {
        _scrollmode = InsertDelete;
    } else {
        _scrollmode = NoScroll;
    }
    dprint(7, (debugfile, "Scroll mode: %s\n",
               _scrollmode==NoScroll ? "No Scroll" :
               _scrollmode==InsertDelete ? "InsertDelete" : "Scroll Regions"));


    if (!_left) {
    	_left = "\b";
    }

    *tt = ttyo;

    return(0);
}



/*----------------------------------------------------------------------
   Initialize the screen with the termcap string 
  ----*/
void
init_screen()
{
    if(_termcap_init != NULL) {
        tputs(_termcap_init, 1, outchar);
	fflush(stdout);
    }
}
        



/*----------------------------------------------------------------------
       Get the current window size
  
   Args: ttyo -- pointer to structure to store window size in

  Gets window size. 
 ----*/
void
get_windsize(ttyo)
struct ttyo *ttyo;     
{
#ifdef RESIZING 
    struct winsize win;

    /*---- Get the window size from the tty driver ---*/
    if ( ioctl(1, TIOCGWINSZ, &win) >= 0) { /* 1 is stdout */
        _lines    = min(win.ws_row, MAX_SCREEN_ROWS);
	_columns  = min(win.ws_col, MAX_SCREEN_COLS);
        if(_lines == 0)
	  _lines   = DEFAULT_LINES_ON_TERMINAL;

	if(_columns == 0)
	  _columns = DEFAULT_COLUMNS_ON_TERMINAL;

        dprint(2, (debugfile, "new win size -----<%d %d>------\n",
                   _lines, _columns));
    } else {
	/* Depending on the OS, the ioctl() may have failed because
	   of a 0 rows, 0 columns setting.  That happens on DYNIX/ptx 1.3
	   (with a kernel patch that happens to involve the negotiation
	   of window size in the telnet streams module.)  In this case
	   the error is EINVARG.  Anyway, why not use defaults here. */

        _lines   = DEFAULT_LINES_ON_TERMINAL;
        _columns = DEFAULT_COLUMNS_ON_TERMINAL;
        dprint(1, (debugfile, "ioctl(TIOCWINSZ) failed :%s\n",
                   error_description(errno)));
    }
    ttyo->screen_cols = _columns;
    ttyo->screen_rows = _lines ;
#else
    /* Termcap takes care of it above */ 
#endif
}




/*----------------------------------------------------------------------
      End use of the screen. 

 Result: the cursor is position at the lower right corner
  ----*/
void
end_screen()
{
    dprint(9, (debugfile, "end_screen called\n"));
    EndInverse();
    MoveCursor( _lines - 1, 0);
    NewLine();
    if(_termcap_end != NULL)
      tputs(_termcap_end, 1, outchar);
    fflush(stdout);
}



/*----------------------------------------------------------------------
    Indicate to the screen painting here that the position of the cursor
 has been disturbed and isn't where these functions might think.
 ----*/
void
clear_cursor_pos()
{
    _line = FARAWAY;
    _col  = FARAWAY;
}
    


/*----------------------------------------------------------------------
     Clear the terminal screen

 Result: The screen is cleared
         internal cursor position set to 0,0
  ----*/
void
ClearScreen()
{
    int j;
    _line = 0;	/* clear leaves us at top... */
    _col  = 0;

    if(ps_global->in_init_seq)
	return;

    if (!_clearscreen) {
        for(j = 0; j < _lines; j++) {
            MoveCursor(0, j);
            CleartoEOLN();
        }
        MoveCursor(0,0);
    } else {
	tputs(_clearscreen, 1, outchar);
    }
}


/*----------------------------------------------------------------------
            Internal move cursor to absolute position

  Args: col -- column to move cursor to
        row -- row to move cursor to

 Result: cursor is moved (variables, not updates)
  ----*/

static void
moveabsolute(col, row)
{

	char *stuff, *tgoto();

	stuff = tgoto(_moveto, col, row);
	tputs(stuff, 1, outchar);
}


/*----------------------------------------------------------------------
        Move the cursor to the row and column number
  Args:  row number
         column number

 Result: Cursor moves
         internal position updated
  ----*/
void
MoveCursor(row, col)
     int row, col;
{
    /** move cursor to the specified row column on the screen.
        0,0 is the top left! **/

    int scrollafter = 0;

    /* we don't want to change "rows" or we'll mangle scrolling... */

    if (col < 0)
      col = 0;
    if (col >= ps_global->ttyo->screen_cols)
      col = ps_global->ttyo->screen_cols - 1;
    if (row < 0)
      row = 0;
    if (row > ps_global->ttyo->screen_rows) {
      if (col == 0)
        scrollafter = row - ps_global->ttyo->screen_rows;
      row = ps_global->ttyo->screen_rows;
    }

    if (!_moveto)
    	return;

    if (row == _line) {
      if (col == _col)
        return;				/* already there! */

      else if (abs(col - _col) < 5) {	/* within 5 spaces... */
        if (col > _col && _right)
          CursorRight(col - _col);
        else if (col < _col &&  _left)
          CursorLeft(_col - col);
        else
          moveabsolute(col, row);
      }
      else 		/* move along to the new x,y loc */
        moveabsolute(col, row);
    }
    else if (col == _col && abs(row - _line) < 5) {
      if (row < _line && _up)
        CursorUp(_line - row);
      else if (_line > row && _down)
        CursorDown(row - _line);
      else
        moveabsolute(col, row);
    }
    else if (_line == row-1 && col == 0) {
      putchar('\n');	/* that's */
      putchar('\r');	/*  easy! */
    }
    else 
      moveabsolute(col, row);

    _line = row;	/* to ensure we're really there... */
    _col  = col;

    if (scrollafter) {
      while (scrollafter--) {
        putchar('\n');
        putchar('\r');

      }
    }

    return;
}



/*----------------------------------------------------------------------
         Newline, move the cursor to the start of next line

 Result: Cursor moves
  ----*/
void
NewLine()
{
   /** move the cursor to the beginning of the next line **/

    Writechar('\n', 0);
    Writechar('\r', 0);
}



/*----------------------------------------------------------------------
        Move cursor up n lines with terminal escape sequence
 
   Args:  n -- number of lines to go up

 Result: cursor moves, 
         internal position updated

 Only for ttyout use; not outside callers
  ----*/
static void
CursorUp(n)
int n;
{
	/** move the cursor up 'n' lines **/
	/** Calling function must check that _up is not null before calling **/

    _line = (_line-n > 0? _line - n: 0);	/* up 'n' lines... */

    while (n-- > 0)
      tputs(_up, 1, outchar);
}



/*----------------------------------------------------------------------
        Move cursor down n lines with terminal escape sequence
 
    Arg: n -- number of lines to go down

 Result: cursor moves, 
         internal position updated

 Only for ttyout use; not outside callers
  ----*/
static void
CursorDown(n)
     int          n;
{
    /** move the cursor down 'n' lines **/
    /** Caller must check that _down is not null before calling **/

    _line = (_line+n < ps_global->ttyo->screen_rows ? _line + n
             : ps_global->ttyo->screen_rows);
                                               /* down 'n' lines... */

    while (n-- > 0)
    	tputs(_down, 1, outchar);
}



/*----------------------------------------------------------------------
        Move cursor left n lines with terminal escape sequence
 
   Args:  n -- number of lines to go left

 Result: cursor moves, 
         internal position updated

 Only for ttyout use; not outside callers
  ----*/
static void 
CursorLeft(n)
int n;
{
    /** move the cursor 'n' characters to the left **/
    /** Caller must check that _left is not null before calling **/

    _col = (_col - n> 0? _col - n: 0);	/* left 'n' chars... */

    while (n-- > 0)
      tputs(_left, 1, outchar);
}


/*----------------------------------------------------------------------
        Move cursor right n lines with terminal escape sequence
 
   Args:  number of lines to go right

 Result: cursor moves, 
         internal position updated

 Only for ttyout use; not outside callers
  ----*/
static void 
CursorRight(n)
int n;
{
    /** move the cursor 'n' characters to the right (nondestructive) **/
    /** Caller must check that _right is not null before calling **/

    _col = (_col+n < ps_global->ttyo->screen_cols? _col + n :
             ps_global->ttyo->screen_cols);	/* right 'n' chars... */

    while (n-- > 0)
      tputs(_right, 1, outchar);

}



/*----------------------------------------------------------------------
       Start painting inverse on the screen
 
 Result: escape sequence to go into inverse is output
         returns 0 if it was done, -1 if not.
  ----*/
void
StartInverse()
{
    /** set inverse video mode **/

    if (!_setinverse)
    	return;

    tputs(_setinverse, 1, outchar);
}



/*----------------------------------------------------------------------
      End painting inverse on the screen
 
 Result: escape sequence to go out of inverse is output
         returns 0 if it was done, -1 if not.
  ----------------------------------------------------------------------*/
void
EndInverse()
{
    /** compliment of startinverse **/

    if (!_clearinverse)
    	return;

    tputs(_clearinverse, 1, outchar);
}


void
StartUnderline()
{
    if (!_setunderline)
    	return;

    tputs(_setunderline, 1, outchar);
}
void
EndUnderline()
{
    if (!_clearunderline)
    	return;

    tputs(_clearunderline, 1, outchar);
}

void
StartBold()
{
    if (!_setbold)
    	return;
    tputs(_setbold, 1, outchar);
}

void
EndBold()
{
    if (!_clearbold)
    	return;
    tputs(_clearbold, 1, outchar);
}



/*----------------------------------------------------------------------
       Insert character on screen pushing others right

   Args: c --  character to insert

 Result: charcter is inserted if possible
         return -1 if it can't be done
  ----------------------------------------------------------------------*/
InsertChar(c)
     int c;
{
    if(_insertchar != NULL && *_insertchar != '\0') {
	tputs(_insertchar, 1, outchar);
	Writechar(c, 0);
    } else if(_startinsert != NULL && *_startinsert != '\0') {
	tputs(_startinsert, 1, outchar);
	Writechar(c, 0);
	tputs(_endinsert, 1, outchar);
    } else {
	return(-1);
    }
    return(0);
}



/*----------------------------------------------------------------------
         Delete n characters from line, sliding rest of line left

   Args: n -- number of characters to delete


 Result: characters deleted on screen
         returns -1 if it wasn't done
  ----------------------------------------------------------------------*/
DeleteChar(n)
     int n;
{
    if(_deletechar == NULL || *_deletechar == '\0')
      return(-1);

    while(n) {
	tputs(_deletechar, 1, outchar);
	n--;
    }
    return(0);
}



/*----------------------------------------------------------------------
  Go into scrolling mode, that is set scrolling region if applicable

   Args: top    -- top line of region to scroll
         bottom -- bottom line of region to scroll

 Result: either set scrolling region or
         save values for later scrolling
         returns -1 if we can't scroll

 Unfortunately this seems to leave the cursor in an unpredictable place
 at least the manuals don't say where, so we force it here.
-----*/
static int __t, __b;

BeginScroll(top, bottom)
     int top, bottom;
{
    char *stuff;

    if(_scrollmode == NoScroll)
      return(-1);

    __t = top;
    __b = bottom;
    if(_scrollmode == UseScrollRegion){
        stuff = tgoto(_scrollregion, bottom, top);
        tputs(stuff, 1, outchar);
        /*-- a location  very far away to force a cursor address --*/
        _line = FARAWAY;
        _col  = FARAWAY;
    }
    return(0);
}



/*----------------------------------------------------------------------
   End scrolling -- clear scrolling regions if necessary

 Result: Clear scrolling region on terminal
  -----*/
void
EndScroll()
{
    if(_scrollmode == UseScrollRegion && _scrollregion != NULL){
	/* Use tgoto even though we're not cursor addressing because
           the format of the capability is the same.
         */
        char *stuff = tgoto(_scrollregion, ps_global->ttyo->screen_rows -1, 0);
	tputs(stuff, 1, outchar);
        /*-- a location  very far away to force a cursor address --*/
        _line = FARAWAY;
        _col  = FARAWAY;
    }
}


/* ----------------------------------------------------------------------
    Scroll the screen using insert/delete or scrolling regions

   Args:  lines -- number of lines to scroll, positive forward

 Result: Screen scrolls
         returns 0 if scroll succesful, -1 if not

 positive lines goes foward (new lines come in at bottom
 Leaves cursor at the place to insert put new text

 0,0 is the upper left
 -----*/
ScrollRegion(lines)
    int lines;
{
    int l;

    if(lines == 0)
      return(0);

    if(_scrollmode == UseScrollRegion) {
	if(lines > 0) {
	    MoveCursor(__b, 0);
	    for(l = lines ; l > 0 ; l--)
	      tputs((_scrolldown == NULL || _scrolldown[0] =='\0') ? "\n" :
		    _scrolldown, 1, outchar);
	} else {
	    MoveCursor(__t, 0);
	    for(l = -lines; l > 0; l--)
	      tputs(_scrollup, 1, outchar);
	}
    } else if(_scrollmode == InsertDelete) {
	if(lines > 0) {
	    MoveCursor(__t, 0);
	    for(l = lines; l > 0; l--) 
	      tputs(_deleteline, 1, outchar);
	    MoveCursor(__b, 0);
	    for(l = lines; l > 0; l--) 
	      tputs(_insertline, 1, outchar);
	} else {
	    for(l = -lines; l > 0; l--) {
	        MoveCursor(__b, 0);
	        tputs(_deleteline, 1, outchar);
		MoveCursor(__t, 0);
		tputs(_insertline, 1, outchar);
	    }
	}
    } else {
	return(-1);
    }
    fflush(stdout);
    return(0);
}



/*----------------------------------------------------------------------
    Write a character to the screen, keeping track of cursor position

   Args: ch -- character to output

 Result: character output
         cursor position variables updated
  ----*/
Writechar(ch, new_esc_len)
     register unsigned int ch;
     int      new_esc_len;
{
    register int nt;
    static   int esc_len = 0;

    if(ps_global->in_init_seq)
        return;

    if(ch >= ' ') {
        putchar(ch);
        if(esc_len > 0)
          esc_len--;
        else
          _col++;

    } else if(ch == '\n') {
        /*-- Don't have to watch out for auto wrap or newline glitch
          because we never let it happen. See below
          ---*/
        putchar('\n');
        _line = min(_line+1,ps_global->ttyo->screen_rows);
        esc_len = 0;

    /* if return, move to column 0 */
    } else if(ch == '\r') {
        putchar('\r');
        _col = 0;
        esc_len = 0;

    /* if backspace, move back  one space  if not already in column 0 */
    } else if (ch == BACKSPACE) {
      if(_col != 0) {
          putchar('\b');
          _col--;
      } /* else BACKSPACE does nothing */

    /* if bell, ring the bell but don't advance the column */
    } else if (ch == '\007') {
        putchar(ch);

    /* if a tab, output it */
    } else if (ch == '\t') {
	do				/* BUG? ignores tty driver's spacing */
	  putchar(' ');
	while(_col < ps_global->ttyo->screen_cols - 1 && ((++_col)&0x07) != 0);

    } else if(ch == '\033'){
        /* If we're outputting an escape here, it may be part of an iso2022
           escape sequence in which case take up no space on the screen.
           Unfortunately such sequences are variable in length.
           */
        esc_len = new_esc_len  - 1;
        putchar(ch);

    } else {
        /* Change any control characters to ?, except for ESC. Most of the
           junk that might come through is handled in copy_text() before it
           gets anywhere near here.
           */
        if(iscntrl(ch) && ch != '\033')
          ch = '?';

        putchar(ch);
        if(esc_len > 0)
          esc_len--;
        else
          _col++;
    }

    /* Here we are at the end of the line. We've decided to make no
       assumptions about how the terminal behaves at this point.
       What can happen now are the following
           1. Cursor is at start of next line, and next character will
              apear there. (autowrap, !newline glitch)
           2. Cursor is at start of next line, and if a newline is output
              it'll be ignored. (autowrap, newline glitch)
           3. Cursor is still at end of line and next char will apear
              there over the top of what is there now (no autowrap).
       We ignore all this and force the cursor to the next line, just 
       like case 1. A little expensive but worth it to avoid problems
       with terminals configured so they don't match termcap
       */
    if(_col == ps_global->ttyo->screen_cols) {
        _col = 0;
        if(_line + 1 < ps_global->ttyo->screen_rows){
            _line++;
            moveabsolute(_col, _line);
        }
    }

     return(0);
}



/*----------------------------------------------------------------------
       Write string to screen at current cursor position

   Args: string -- strings to be output

 Result: Line written to the screen
  ----*/
/*VARARGS2*/
void
Write_to_screen(string)
      register char *string; 
{
    while(*string)
      Writechar(*string++, 0);
}



/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 0 args

  Args:  x -- column position on the screen
         y -- row position on the screen
         line -- line of text to output

 Result: text is output
         cursor position is update
  ----*/
void
PutLine0(x, y, line)
     int            x,y;
     register char *line;
{
    MoveCursor(x,y);
    Write_to_screen(line);
}



/*----------------------------------------------------------------------
  Printf style output line to the screen at given position with given length

 Args:  x      -- column position on the screen
        y      -- column position on the screen
        line   -- text to be output
        length -- length of text to be output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
PutLine0n8b(x, y, line, length)
     int            x,y, length;
     register char *line;
{
    char *line_end;
    int   len;
    unsigned char c;

    MoveCursor(x,y);
    for(line_end = line + length;*line && line < line_end; line++) 
      if(*line == '\01')
        StartBold();
      else if(*line == '\02')
        EndBold();
      else if(*line == '\03')
        StartUnderline();
      else if(*line == '\04')
        EndUnderline();
      else if(*line == '\033') {
          /* check for iso-2022 escape */
          len = match_iso2022(line);
          c = (unsigned char)*line;
          Writechar(c, len);
      } else {
          c = (unsigned char)*line;
          Writechar(c, len);
      }
}



static char temp_format[10240]; /*Very large because there's no bounds checks*/
/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 1 arg

 Args:  x -- column position on the screen
        y -- column position on the screen
        line -- printf style line of text to output
        arg1 -- first argument for printf

 Result: text is output
         cursor position is update
  ----*/
void
/*VARARGS2*/
PutLine1(x,y, line, arg1)
     int         x,y;
     char       *line;
     void       *arg1;
{
	sprintf(temp_format, line, arg1);
	PutLine0(x, y, temp_format);
}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 2 args


 Args:  x -- column position on the screen
        y -- column position on the screen
        line -- printf style line of text to output
        arg1 --
        arg2 --

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine2(x,y, line, arg1, arg2)
     int          x,y;
     char        *line;
     void        *arg1, *arg2;
{
    sprintf(temp_format, line, arg1, arg2);
    PutLine0(x, y, temp_format);
}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 3 args

 Args:  x -- column position on the screen
        y -- column position on the screen
        line -- printf style line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine3(x,y, line, arg1, arg2, arg3)
     int x,y;
     char *line;
     void *arg1, *arg2, *arg3;
{
    sprintf(temp_format, line, arg1, arg2, arg3);
    PutLine0(x, y, temp_format);
}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 4 args

 Args:  x -- column position on the screen
        y -- column position on the screen
        line -- printf style line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine4(x, y, line, arg1, arg2, arg3, arg4)
     int   x,y;
     char *line;
     void *arg1, *arg2, *arg3, *arg4;
{
    sprintf(temp_format, line, arg1, arg2, arg3, arg4);
    PutLine0(x, y, temp_format);
}



/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 5 args

 Args:  x -- column position on the screen
        y -- column position on the screen
        line -- printf style line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine5(x, y, line, arg1, arg2, arg3, arg4, arg5)
     int   x,y;
     char *line;
     void *arg1, *arg2, *arg3, *arg4, *arg5;
{
    sprintf(temp_format, line, arg1, arg2, arg3, arg4, arg5);
    PutLine0(x, y, temp_format);
}



/*----------------------------------------------------------------------
       Output a line to the screen, centered

   Args:  line -- Line number to print on
          string -- string to output
  
 Result:  String is output to screen
          Returns column number line is output on
  ----------------------------------------------------------------------*/
Centerline(line, string)
     int line;
     char *string;
{
	/** Output 'string' on the given line, centered. **/

	register int length, col;

	length = strlen(string);

	if (length > ps_global->ttyo->screen_cols)
	  col = 0;
	else
	  col = (ps_global->ttyo->screen_cols - length) / 2;

	PutLine0(line, col, string);
	return(col);
}



/*----------------------------------------------------------------------
    Output line at given position, with parts in reverse video, 0 args

  Args:  line   -- row position on screen
         column -- column number to begin on
         key    -- key character to turn on inverse
         pmt    -- string to output

 Result: string is output
         cursor position is update
  ----*/
/* 
 * show pmt, with every character after key highlighted, on line line 
 * beginning at column column.
 */
void
Stripe0(line, column, key,  pmt)
     int  line;
     int  column;
     char *pmt;
     int key;
{
  char  *buf;
  int   i = column;

    buf = pmt;
    do{
        if(*buf == key){
            buf++;
            StartInverse();
            PutLine1(line, i++, "%c", (void *)(int)*buf);
            EndInverse();
        }
        else{
            PutLine1(line, i++, "%c", (void *)(int)*buf);
        }
    }
    while(*++buf != '\0');
    MoveCursor(line, i);
}



/*----------------------------------------------------------------------
    Output line at given position, with parts in reverse video, 3 args

 Input:  line -- position on screen
         columne --
         key -- key character to turn on inverse
         string -- printf style string to output
         arg1   -- printf arguments
         arg2
         arg3

 Result: string is output
         cursor position is update
  ----*/
void
Stripe3(line, column, key, string, arg1, arg2, arg3)
     int line, column, key;
     char *string, *arg1, *arg2, *arg3;
{
    sprintf(temp_format, string, arg1, arg2, arg3);
    Stripe0(line, column, key, temp_format);
}



/*----------------------------------------------------------------------
    Clear screen to end of line on current line

 Result: Line is cleared
  ----*/
void
CleartoEOLN()
{
    /** clear to end of line **/

    if (!_cleartoeoln)
    	return;

    tputs(_cleartoeoln, 1, outchar);
}



/*----------------------------------------------------------------------
     Clear screen to end of screen from current point

 Result: screen is cleared
  ----*/
CleartoEOS()
{
    int j;

    if (!_cleartoeos) {
        CleartoEOLN();
        for(j = _line; j < _lines; j++){
            MoveCursor(j, 0);
            CleartoEOLN();
        }
    } else {
        tputs(_cleartoeos, 1, outchar);
    }
}



/*----------------------------------------------------------------------
     function to output character used by termcap

   Args: c -- character to output

 Result: character output to screen via stdio
  ----*/
void
outchar(c)
int c;
{
	/** output the given character.  From tputs... **/
	/** Note: this CANNOT be a macro!              **/

	putc(c, stdout);
}



#else /* DOS */  /* Middle of giant ifdef for dos driver */

#include <dos.h>


/*  This DOS code compiled at one time, though it was never run.
    Substantial work is needed here    LL 91.12.24
 */
/*
 *  Work's been done here to make it work. MS 92.04.20
 */


#ifdef TO_UP_LOW_MACRO
#undef tolower
#endif


#define FARAWAY 1000
#define VERY_LONG_STRING 256


static int   _lines, _columns;
static int   _line  = FARAWAY;
static int   _col   = FARAWAY;

static union REGS regs;


/*----------------------------------------------------------------------
      Initialize the screen for output, set terminal type, etc

 Input: TERM and TERMCAP environment variables
        Pointer to variable to store the tty output structure.

 Result:  terminal size is discovered and set pine state
          termcap entry is fetched and stored in local variables
          make sure terminal has adequate capabilites
          evaluate scrolling situation
          returns status of indicating the state of the screen/termcap entry
  ----------------------------------------------------------------------*/
void
init_screen()					/* DOS */
{
    return;					/* NO OP */
}



/*----------------------------------------------------------------------
      End use of the screen. 
  ----------------------------------------------------------------------*/
void
end_screen()					/* DOS */
{
    extern void exit_text_mode();

    MoveCursor(_lines - 1, 0);
    exit_text_mode(NULL);
    ibmclose();
/*
    EndInverse();
    MoveCursor( _lines - 2, 0);
    ibmeeol();
    MoveCursor( _lines - 1, 0);
    ibmeeol();
*/
}


    
/*----------------------------------------------------------------------
      Initialize the screen for output, set terminal type, etc

   Args: tt -- Pointer to variable to store the tty output structure.

 Result:  terminal size is discovered and set pine state
          termcap entry is fetched and stored in local variables
          make sure terminal has adequate capabilites
          evaluate scrolling situation
          returns status of indicating the state of the screen/termcap entry

      Returns:
        -1 indicating no terminal name associated with this shell,
        -2..-n  No termcap for this terminal type known
	-3 Can't open termcap file 
        -4 Terminal not powerful enough - missing clear to eoln or screen
	                                       or cursor motion

  ----*/
int
config_screen(tt)				/* DOS */
     struct ttyo **tt;
{
    struct ttyo *ttyo;
    extern void enter_text_mode();

    _line  =  0;		/* where are we right now?? */
    _col   =  0;		/* assume zero, zero...     */

    _lines   = DEFAULT_LINES_ON_TERMINAL;
    _columns = DEFAULT_COLUMNS_ON_TERMINAL;
    ttyo = (struct ttyo *)fs_get(sizeof(struct ttyo));
    ttyo->screen_cols = _columns;
    ttyo->screen_rows = _lines ;

    enter_text_mode(NULL);
    ibmopen();

    *tt = ttyo;

    return(0);
}


    
/*----------------------------------------------------------------------
       Get the current window size
  
   Args: ttyo -- pointer to structure to store window size in

  Gets window size. 
 ----*/
void
get_windsize(ttyo)				/* DOS */
struct ttyo *ttyo;     
{
    ttyo->screen_cols = _columns = DEFAULT_COLUMNS_ON_TERMINAL;
    ttyo->screen_rows = _lines = DEFAULT_LINES_ON_TERMINAL;
}
    



/*----------------------------------------------------------------------
     Clear the terminal screen

 Input:  none

 Result: The screen is cleared
         internal cursor position set to 0,0
  ----------------------------------------------------------------------*/
void
ClearScreen()					/* DOS */
{
    /* clear the screen with good old interrupt 0x10 */

    _line = 0;			/* clear leaves us at top... */
    _col  = 0;

    if(ps_global->in_init_seq)
	return;

    regs.h.ah = 6;		/* scroll page up function code */
    regs.h.al = 0;		/* # lines to scroll (clear it) */
    regs.x.cx = 0;		/* upper left corner of scroll */
    regs.x.dx = ((ps_global->ttyo->screen_rows)<<8) 
		| (ps_global->ttyo->screen_cols - 1);
    regs.h.bh = 0x07;		/* normal color */
    int86(0x10, &regs, &regs);		/* video services */
}



/*----------------------------------------------------------------------
        Move the cursor to the row and column number
 Input:  row number
         column number

 Result: Cursor moves
         internal position updated
  ----------------------------------------------------------------------*/
void
MoveCursor(row, col)				/* DOS */
     int row, col;
{
    /** move cursor to the specified row column on the screen.
        0,0 is the top left! **/

    ibmmove(row, col);
    _line = row;
    _col = col;
}



/*----------------------------------------------------------------------
         Newline, move the cursor to the start of next line

 Input:  none

 Result: Cursor moves
  ----------------------------------------------------------------------*/
void
NewLine()					/* DOS */
{
    /** move the cursor to the beginning of the next line **/

    MoveCursor(_line+1, 0);
}




/*----------------------------------------------------------------------
       Start painting inverse on the screen
 
 Input:  none

 Result: escape sequence to go into inverse is output
         returns 0 if it was done, -1 if not.
  ----------------------------------------------------------------------*/
void
StartInverse()					/* DOS */
{
    ibmrev(1);					/* libpico call */
}


/*----------------------------------------------------------------------
      End painting inverse on the screen
 
 Input:  none

 Result: escape sequence to go out of inverse is output
         returns 0 if it was done, -1 if not.
  ----------------------------------------------------------------------*/
void
EndInverse()					/* DOS */
{
    ibmrev(0);					/* libpico call */
}


/*
 * Character attriute stuff that could use some work
 * MS 92.05.18
 */

void
StartUnderline()				/* DOS */
{

}

void						/* DOS */
EndUnderline()
{
}

void
StartBold()					/* DOS */
{
}

void
EndBold()					/* DOS */
{
}


/*----------------------------------------------------------------------
       Insert character on screen pushing others right

 Input:  character to output
         termcap escape sequences

 Result: charcter is inserted if possible
         return -1 if it can't be done
  ----------------------------------------------------------------------*/
InsertChar(c)					/* DOS */
     int c;
{
    return(-1);
}


/*----------------------------------------------------------------------
         Delete n characters from line, sliding rest of line left

 Input:  number of characters to delete
         termcap escape sequences

 Result: characters deleted on screen
         returns -1 if it wasn't done
  ----------------------------------------------------------------------*/
DeleteChar(n)					/* DOS */
     int n;
{
    char c;
    int oc;				/* original column */

    oc = _col;
/* cursor OFF */
    while(_col < 79){
	ibmmove(_line, _col+1);
    	readscrn(&c);
	ibmmove(_line, _col++);
	ibmputc(c);
    }
    ibmputc(' ');
    ibmmove(_line, oc);
/* cursor ON */
}



/*----------------------------------------------------------------------
  Go into scrolling mode, that is set scrolling region if applicable

 Input: top line of region to scroll
        bottom line of region to scroll

 Result: either set scrolling region or
         save values for later scrolling
         returns -1 if we can't scroll

 Unfortunately this seems to leave the cursor in an unpredictable place
 at least the manuals don't say were, so we force it here.
----------------------------------------------------------------------*/
static int __t, __b;
BeginScroll(top, bottom)			/* DOS */
     int top, bottom;
{
    __t = top;
    __b = bottom;
}

/*----------------------------------------------------------------------
   End scrolling -- clear scrolling regions if necessary

 Input:  none

 Result: Clear scrolling region on terminal
  ----------------------------------------------------------------------*/
void
EndScroll()					/* DOS */
{
}


/* ----------------------------------------------------------------------
    Scroll the screen using insert/delete or scrolling regions

 Input:  number of lines to scroll, positive forward

 Result: Screen scrolls
         returns 0 if scroll succesful, -1 if not

 positive lines goes foward (new lines come in at bottom
 Leaves cursor at the place to insert put new text

 0,0 is the upper left
 ----------------------------------------------------------------------*/
ScrollRegion(lines)				/* DOS */
    int lines;
{
    return(-1);
}


/*----------------------------------------------------------------------
    Write a character to the screen, keeping track of cursor position

 Input:  charater to write

 Result: character output
         cursor position variables updated
  ----------------------------------------------------------------------*/
Writechar(ch, new_esc_len)				/* DOS */
     register unsigned int ch;
     int      new_esc_len;
{
    register int nt;

    if(ps_global->in_init_seq)
      return(0);

    if(ch == '\n') {
	_line = min(_line+1,ps_global->ttyo->screen_rows);
        _col =0;
        ibmmove(_line, _col);

    /* if return, move to column 0 */
    } else if(ch == '\r') {
	_col = 0;
        ibmmove(_line, _col);

    /* if backspace, move back  one space  if not already in column 0 */
    } else if (ch == BACKSPACE) {
	if(_col > 0)
          ibmmove(_line, --_col);
	/* else BACKSPACE does nothing */

    /* if bell, ring the bell but don't advance the column */
    } else if (ch == '\007') {
	ibmbeep();				/* libpico call */

	/* if a tab, output it */
    } else if (ch == '\t') {
	do
	  ibmputc(' ');
	while(((++_col)&0x07) != 0);
    } else {
	/*if some kind of control or  non ascii character change to a '?'*/
	if(iscntrl(ch) && ch != '\033')
	  ch = '?';

	ibmputc(ch);
	_col++;
    }

    if(_col == ps_global->ttyo->screen_cols) {
	  _col  = 0;
	  _line = min(_line+1, ps_global->ttyo->screen_rows);
    }

    return(0);
}


/*----------------------------------------------------------------------
      Printf style write directly to the terminal at current position

 Input: printf style control string
        number of printf style arguments
        up to three printf style arguments

 Result: Line written to the screen
  ----------------------------------------------------------------------*/
/*VARARGS2*/
void
Write_to_screen( string )			/* DOS */
      register char *string;
{
    while(*string)
      Writechar(*string++, 0);
}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 0 args

 Input:  position on the screen
         line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
PutLine0(x, y, line)				/* DOS */
     int            x,y;
     register char *line;
{
    MoveCursor(x,y);
    while(*line)
      Writechar(*line++, 0);
}


/*----------------------------------------------------------------------
  Printf style output line to the screen at given position with given length

 Args:  x      -- column position on the screen
        y      -- column position on the screen
        line   -- text to be output
        length -- length of text to be output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
PutLine0n8b(x, y, line, length)			/* DOS */
     int            x,y, length;
     register char *line;
{
    char *line_end;
    int   len;
    unsigned char c;

    MoveCursor(x,y);
    for(line_end = line + length;*line && line < line_end; line++) 
      if(*line == '\01')
        StartBold();
      else if(*line == '\02')
        EndBold();
      else if(*line == '\03')
        StartUnderline();
      else if(*line == '\04')
        EndUnderline();
      else if(*line == '\033') {
          /* check for iso-2022 escape */
          len = match_iso2022(line);
          c = (unsigned char)*line;
          Writechar(c, len);
      } else {
          c = (unsigned char)*line;
          Writechar(c, len);
      }

}


/*----------------------------------------------------------------------
  Printf style output line to the screen at given position with given length

 Input:  position on the screen
         line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
PutLine0n(x, y, line, length)			/* DOS */
     int            x,y, length;
     register char *line;
{
	/** Write a zero argument line at location x,y **/
    char *line_end;

    MoveCursor(x,y);
    for(line_end = line + length;*line && line < line_end; line++) 
      if(*line == '\01')
        StartBold();
      else if(*line == '\02')
        EndBold();
      else if(*line == '\03')
        StartUnderline();
      else if(*line == '\04')
        EndUnderline();
      else
        Writechar(*line, 0);

}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 1 arg

 Input:  position on the screen
         line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS2*/
PutLine1(x,y, line, arg1)			/* DOS */
     int         x,y;
     char       *line;
     void       *arg1;
{
	/** write line at location x,y - one argument... **/

	char buffer[VERY_LONG_STRING];

	sprintf(buffer, line, arg1);

	PutLine0(x, y, buffer);
}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 2 args

 Input:  position on the screen
         line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine2(x,y, line, arg1, arg2)			/* DOS */
     int          x,y;
     char        *line;
     void        *arg1, *arg2;
{
	/** write line at location x,y - one argument... **/

	char buffer[VERY_LONG_STRING];

	sprintf(buffer, line, arg1, arg2);

	PutLine0(x, y, buffer);
}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 3 args

 Input:  position on the screen
         line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine3(x,y, line, arg1, arg2, arg3)		/* DOS */
     int x,y;
     char *line;
     void *arg1, *arg2, *arg3;
{
	/** write line at location x,y - one argument... **/

	char buffer[VERY_LONG_STRING];

	sprintf(buffer, line, arg1, arg2, arg3);

	PutLine0(x, y, buffer);
}


/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 4 args

 Args:  x -- column position on the screen
        y -- column position on the screen
        line -- printf style line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine4(x, y, line, arg1, arg2, arg3, arg4)			/* DOS */
     int   x,y;
     char *line;
     void *arg1, *arg2, *arg3, *arg4;
{
    char buffer[4096];

    sprintf(buffer, line, arg1, arg2, arg3, arg4);
    PutLine0(x, y, buffer);
}



/*----------------------------------------------------------------------
      Printf style output line to the screen at given position, 5 args

 Args:  x -- column position on the screen
        y -- column position on the screen
        line -- printf style line of text to output

 Result: text is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
/*VARARGS3*/
PutLine5(x, y, line, arg1, arg2, arg3, arg4, arg5)		/* DOS */
     int   x,y;
     char *line;
     void *arg1, *arg2, *arg3, *arg4, *arg5;
{
    char buffer[4096];

    sprintf(buffer, line, arg1, arg2, arg3, arg4, arg5);
    PutLine0(x, y, buffer);
}



/*----------------------------------------------------------------------
       Output a line to the screen, centered

  Input:  Line number to print on, string to output
  
 Result:  String is output to screen
          Returns column number line is output on
  ----------------------------------------------------------------------*/
Centerline(line, string)			/* DOS */
     int line;
     char *string;
{
	/** Output 'string' on the given line, centered. **/

	register int length, col;

	length = strlen(string);

	if (length > ps_global->ttyo->screen_cols)
	  col = 0;
	else
	  col = (ps_global->ttyo->screen_cols - length) / 2;

	PutLine0(line, col, string);
	return(col);
}



/*----------------------------------------------------------------------
    Output line at given position, with parts in reverse video, 0 args

 Input:  position on screen
         key character to turn on inverse
         string to output

 Result: string is output
         cursor position is update
  ----------------------------------------------------------------------*/
/* 
 * show pmt, with every character after key highlighted, on line line 
 * beginning at column column.
 */
void
Stripe0(line, column, key,  pmt)		/* DOS */
     int  line;
     int  column;
     char *pmt;
     int key;
{
  char  *buf;
  int   i = column;

    buf = pmt;
    do{
        if(*buf == key){
            buf++;
            StartInverse();
            PutLine1(line, i++, "%c", (void *)*buf);
            EndInverse();
        }
        else{
            PutLine1(line, i++, "%c", (void *)*buf);
        }
    }
    while(*++buf != '\0');
    MoveCursor(line, i);
}



/*----------------------------------------------------------------------
    Output line at given position, with parts in reverse video, 3 args

 Input:  position on screen
         key character to turn on inverse
         string to output

 Result: string is output
         cursor position is update
  ----------------------------------------------------------------------*/
void
Stripe3(line, column, key, string, arg1, arg2, arg3) /* DOS */
     int line, column, key;
     char *string, *arg1, *arg2, *arg3;
{
    char buffer[VERY_LONG_STRING];

    sprintf(buffer, string, arg1, arg2, arg3);
    Stripe0(line, column, key, buffer);
}



/*----------------------------------------------------------------------
    Clear screen to end of line on current line

 Input:  none

 Result: Line is cleared
  ----------------------------------------------------------------------*/
void
CleartoEOLN()					/* DOS */
{
    ibmeeol();					/* libpico call */
}


/*----------------------------------------------------------------------
          Clear screen to end of screen from current point

 Input: none

 Result: screen is cleared
  ----------------------------------------------------------------------*/
CleartoEOS()					/* DOS */
{
    int n;

    ibmeeol();					/* libpico call */

    for(n = _line + 1; n < _lines; n++) {
        ibmmove(0, n);
        ibmeeol();				/* libpico call */
    }
}

/*----------------------------------------------------------------------
    Indicate to the screen painting here that the position of the cursor
 has been disturbed and isn't where these functions might think.
 ----*/
void
clear_cursor_pos()				/* DOS */
{
    _line = FARAWAY;
    _col  = FARAWAY;
}


/*
 * readscrn - DOS magic to read the character on the display at the
 *            current cursor position
 */
readscrn(c)					/* DOS */
char *c;
{
    regs.h.ah = 8;			/* vid services, read char at cursor */
    regs.h.bh = 0;			/* vid services, read char at cursor */
    int86(0x10, &regs, &regs);
    *c = regs.h.al;
}
#endif /* DOS */ /* End of giant ifdef for DOS drivers */
