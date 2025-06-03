
/* TTY_VMS.C: TTYOUT + TTYIN for VMS without TermCap. */
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
          - Don't figure out what the terminal type is but assume it is
            an ANSI one. Maybe next round we'll make it better.
          - Don't deal with screen size changes presently.
          - save special output sequences
          - the usual screen clearing, cursor addressing and scrolling


     This library gives programs the ability to easily access the
     termcap information and write screen oriented and raw input
     programs.  The routines can be called as needed, except that
     to use the cursor / screen routines there must be a call to
     InitScreen() first.  The 'Raw' input routine can be used
     independently, however. (Elm comment)

     VMS note: Since there is no Termcap we assign the strings manually.

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

/* Data manually read from TERMCAP */
static char *_clearscreen = "\033[2J\033[;H",
	*_moveto = "\033[%d;%dH",
	*_up = "\033[A", *_down = "\033[B", *_right = "\033[C", *_left = "\033[D",
	*_setinverse = "\033[7m", *_clearinverse = "\033[m",
	*_setunderline = "\033[4m", *_clearunderline = "\033[24m",
	*_setbold = "\033[1m",     *_clearbold = "\033[m",
	*_cleartoeoln = "\033[K", *_cleartoeos = "\033[J",
	*_startinsert = "\033[4h", *_endinsert = "\033[4l",
	*_insertchar = "\033[@", *_deletechar = "\033[P",
	*_deleteline = "\033[M", *_insertline = "\033[L",
	*_scrollregion = "\033[%d;%dr",
	*_scrollup = "\033M", *_scrolldown = "\033D",
	*_termcap_init = "", *_termcap_end = "";

static int   _lines = 24, _columns = 80;
char  ttermname[40];

static int   _line  = FARAWAY;
static int   _col   = FARAWAY;
static enum  {NoScroll,UseScrollRegion,InsertDelete} _scrollmode;


/*============== Taken from PICO's port ===================*/
#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <lib$routines.h>
#include <descrip.h>
#include "headers.h"

/* Output channel and buffers */
static short	TerminalChan = 0;
#define	MAXBUF	1024		/* Save up to 1024 characters before sending them */
unsigned char	OutputBuffer[MAXBUF + 1];
int	OutputBufferCounter;

/* For setting and resting the PASTHRU flag: */
struct	{
	unsigned char	class, type;
	unsigned short	width;
	unsigned long	characteristics[2];
	} TerminalChar;

struct DESC {		/* String descriptor */
	short	length, type;
	char	*address;
	} ;

/*
 * ___ttopen - this function is called once to set up the terminal device 
 *          streams.  if called as pine composer, don't mess with
 *          tty modes, but set signal handlers.
 */
___ttopen()
{
	long	status;
	struct DESC TerminalDesc;
	char	TerminalName[] = "SYS$OUTPUT:";

	TerminalDesc.address = TerminalName; TerminalDesc.type = 0;
	TerminalDesc.length = strlen(TerminalName);

	status = sys$assign(&TerminalDesc, &TerminalChan, (int)(0), (int)(0));
	if((status & 0x1) == 0) {
		printf("Can't assign channel to terminal\n");
		exit(status);
	}

	OutputBufferCounter = 0;	/* Output buffer is empty */


    return(1);
}



/*
 * ___ttclose - this function gets called just before we go back home to 
 *           the command interpreter.  If called as pine composer, don't
 *           worry about modes, but set signals to default, pine will 
 *           rewire things as needed.
 */
___ttclose()
{
	int	status;

	___ttflush();

	sys$dassgn(TerminalChan);
	TerminalChan = 0;
    return(1);
}



/*
 | Physically write a string to the terminal.
 */
___write_terminal(string, size)
char	*string;
int	size;
{
	long	status;
	short	iosb[4];

	status = sys$qiow((int)(0), TerminalChan, (short)(IO$_WRITEVBLK),
			iosb, (int)(0), (int)(0),
			string, size, (int)(0), (int)(0), (int)(0), (int)(0));
	if(((status & 0x1) == 0) || ((iosb[0] & 0x1) == 0)) {
		printf("Can't write to terminal; status=%d, iosb=%d\n",
			status, iosb[0]);
		exit(status);
	}
	return 0;	/* Success */
}


/*
 * ___ttputc - Write a character to the display. Actually save it in our buffer
 * and write it only if the buffer is becoming full.
 * This function is called by TTPUTC().
 */
___ttputc(c)
{
	OutputBuffer[OutputBufferCounter++] = c;
	if(OutputBufferCounter >= MAXBUF)
		___ttflush();
	return c;
}

/*
 * Append a string to the buffer. Flush it if there won't be enough space in
 * it.
 */
___ttputs(string)
char	*string;
{
	int	size = strlen(string);

	if((OutputBufferCounter + size) >= MAXBUF)
		___ttflush();
	strncpy(&OutputBuffer[OutputBufferCounter], string, size);
	OutputBufferCounter += size;
}


/*
 * ___ttflush - Flush our buffer to the terminal.
 */
___ttflush()
{
	if(OutputBufferCounter > 0) {
		___write_terminal(OutputBuffer, OutputBufferCounter);
		OutputBufferCounter = 0;
	}
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
config_screen(tt)
struct ttyo **tt;
{
    struct ttyo   *ttyo;

#define TT_STRING_SIZE 20
       char string[TT_STRING_SIZE] ;   /* string for holding tt:'s dev name */
       int rows, col ;                 /* row / column vars */
       unsigned long status ;          /* status return var */
       unsigned long terminal_item = 797 ; /* JPI$_TERMINAL */
       unsigned long tt_page_item = 170 ;  /* DVI$_TT_PAGE */
       unsigned long devbufsz_item = 8 ;   /* DVI$_DEVBUFSIZ */
       struct dsc$descriptor_vs tt_sdesc =
                               {TT_STRING_SIZE,/* max string size */
                               DSC$K_DTYPE_VT, /* descriptor type */
                               DSC$K_CLASS_VS, /* descriptor class = var str */
                               string} ;       /* pointer to var string */

    ttyo = (struct ttyo *)fs_get(sizeof (struct ttyo));

               /* Get our terminal's device name */
       status = lib$getjpi(&terminal_item,NULL,NULL,NULL,&tt_sdesc,NULL) ;
       if ((status&1) == 1)
          {
               /* Get our terminal's page size */
          status = lib$getdvi(&tt_page_item,NULL,&tt_sdesc,&rows,NULL,NULL) ;
               /* Get our terminal's page width */
          status = lib$getdvi(&devbufsz_item,NULL,&tt_sdesc,&col,NULL,NULL) ;
               /* Set the globals for rows and columns */
          _lines = rows-1 ;
          _columns = col ;
          }


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

    _scrollmode = NoScroll;	/* Use the dumbest mode */

    *tt = ttyo;
    return 0;
}



/*----------------------------------------------------------------------
   Initialize the screen with the termcap string 
  ----*/
void
init_screen()
{
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
    /*___ttflush();*/
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
    _line = 1;	/* clear leaves us at top... */
    _col  = 1;

    if(ps_global->in_init_seq)
	return;

    if (!_clearscreen) {
        for(j = 0; j < _lines; j++) {
            MoveCursor(0, j);
            CleartoEOLN();
        }
        MoveCursor(0,0);
	/*___ttflush();*/
    } else {
	___ttputs(_clearscreen);
	/*___ttflush();*/
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

	char stuff[256];

	sprintf(stuff, _moveto, row, col);
	___ttputs(stuff);
	/*___ttflush();*/
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

    row++; col++;	/* They start from 0, our term from 1... */
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
      ___ttputc('\n');	/* that's */
      ___ttputc('\r');	/*  easy! */
    }
    else 
      moveabsolute(col, row);

    _line = row;	/* to ensure we're really there... */
    _col  = col;

    if (scrollafter) {
      while (scrollafter--) {
        ___ttputc('\n');
        ___ttputc('\r');

      }
    }

    /*___ttflush();*/
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

    _line = (_line-n > 0? _line - n: 1);	/* up 'n' lines... */

    while (n-- > 0)
      ___ttputs(_up);
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
    	___ttputs(_down);
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

    _col = (_col - n > 0 ? _col - n : 1);	/* left 'n' chars... */

    while (n-- > 0)
      ___ttputs(_left);
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
      ___ttputs(_right);

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

    ___ttputs(_setinverse);
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

    ___ttputs(_clearinverse);
}


void
StartUnderline()
{
    if (!_setunderline)
    	return;

    ___ttputs(_setunderline);
}
void
EndUnderline()
{
    if (!_clearunderline)
    	return;

    ___ttputs(_clearunderline);
}

void
StartBold()
{
    if (!_setbold)
    	return;
    ___ttputs(_setbold);
}

void
EndBold()
{
    if (!_clearbold)
    	return;
    ___ttputs(_clearbold);
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
	___ttputs(_insertchar);
	Writechar(c, 0);
	/*___ttflush();*/
    } else if(_startinsert != NULL && *_startinsert != '\0') {
	___ttputs(_startinsert);
	Writechar(c, 0);
	___ttputs(_endinsert);
	/*___ttflush();*/
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
	___ttputs(_deletechar);
	n--;
    }
    /*___ttflush();*/
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
    char stuff[1024];

    if(_scrollmode == NoScroll)
      return(-1);

    __t = top;
    __b = bottom;
    if(_scrollmode == UseScrollRegion){
        sprintf(stuff, _scrollregion, bottom, top);
        ___ttputs(stuff);
        /*-- a location  very far away to force a cursor address --*/
        _line = FARAWAY;
        _col  = FARAWAY;
    }
    /*___ttflush();*/
    return(0);
}



/*----------------------------------------------------------------------
   End scrolling -- clear scrolling regions if necessary

 Result: Clear scrolling region on terminal
  -----*/
void
EndScroll()
{
    char	stuff[1024];

    if(_scrollmode == UseScrollRegion && _scrollregion != NULL){
	/* Use tgoto even though we're not cursor addressing because
           the format of the capability is the same.
         */
        sprintf(stuff, _scrollregion, ps_global->ttyo->screen_rows -1, 0);
	___ttputs(stuff);
        /*-- a location  very far away to force a cursor address --*/
        _line = FARAWAY;
        _col  = FARAWAY;
	/*___ttflush();*/
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
	      ___ttputs((_scrolldown == NULL || _scrolldown[0] =='\0') ? "\n" :
		    _scrolldown);
	} else {
	    MoveCursor(__t, 0);
	    for(l = -lines; l > 0; l--)
	      ___ttputs(_scrollup);
	}
    } else if(_scrollmode == InsertDelete) {
	if(lines > 0) {
	    MoveCursor(__t, 0);
	    for(l = lines; l > 0; l--) 
	      ___ttputs(_deleteline);
	    MoveCursor(__b, 0);
	    for(l = lines; l > 0; l--) 
	      ___ttputs(_insertline);
	} else {
	    for(l = -lines; l > 0; l--) {
	        MoveCursor(__b, 0);
	        ___ttputs(_deleteline);
		MoveCursor(__t, 0);
		___ttputs(_insertline);
	    }
	}
    } else {
	return(-1);
    }
    /*___ttflush();*/
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
        ___ttputc(ch);
        if(esc_len > 0)
          esc_len--;
        else
          _col++;

    } else if(ch == '\n') {
        /*-- Don't have to watch out for auto wrap or newline glitch
          because we never let it happen. See below
          ---*/
        ___ttputc('\n');
        _line = min(_line+1,ps_global->ttyo->screen_rows);
        esc_len = 0;

    /* if return, move to column 0 */
    } else if(ch == '\r') {
        ___ttputc('\r');
        _col = 1;
        esc_len = 0;

    /* if backspace, move back  one space  if not already in column 0 */
    } else if (ch == BACKSPACE) {
      if(_col <= 1) {
          ___ttputc('\b');
          _col--;
      } /* else BACKSPACE does nothing */

    /* if bell, ring the bell but don't advance the column */
    } else if (ch == '\007') {
        ___ttputc(ch);

    /* if a tab, output it */
    } else if (ch == '\t') {
	do				/* BUG? ignores tty driver's spacing */
	  ___ttputc(' ');
	while(_col < ps_global->ttyo->screen_cols - 1 && ((++_col)&0x07) != 0);

    } else if(ch == '\033'){
        /* If we're outputting an escape here, it may be part of an iso2022
           escape sequence in which case take up no space on the screen.
           Unfortunately such sequences are variable in length.
           */
        esc_len = new_esc_len  - 1;
        ___ttputc(ch);

    } else {
        /* Change any control characters to ?, except for ESC. Most of the
           junk that might come through is handled in copy_text() before it
           gets anywhere near here.
           */
        if(iscntrl(ch) && ch != '\033')
          ch = '?';

        ___ttputc(ch);
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
        _col = 1;
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
    /*___ttflush();*/
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
    int   len = 0;
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
    /*___ttflush();*/
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
int
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
    /*___ttflush();*/
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

    ___ttputs(_cleartoeoln);
    /*___ttflush();*/
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
        ___ttputs(_cleartoeos);
    }
    /*___ttflush();*/
}


/*================== TTYIN =========================*/
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
       ttyin.c
       Things having to do with reading from the tty driver and keyboard
          - initialize tty driver and reset tty driver
          - read a character from terminal with keyboard escape seqence mapping
          - initialize keyboard (keypad or such) and reset keyboard
          - prompt user for a line of input
          - read a command from keyboard with timeouts.

 ====*/

#ifdef	ANSI
void   line_paint(int, int *);
#else
void   line_paint();
#endif

/*----------------------------------------------------------------------
    Initialize the tty driver to do single char I/O and whatever else  (UNIX)

   Args:  struct pine

 Result: tty driver is put in raw mode so characters can be read one
         at a time. Returns -1 if unsuccessful, 0 if successful.

Some file descriptor voodoo to allow for pipes across vforks. See 
open_mailer for details.
  ----------------------------------------------------------------------*/
init_tty_driver(ps)
     struct pine *ps;
{
    ps = ps; /* Get rid of unused parameter warning */

    if(TerminalChan == 0)	/* Not initied yet */
	___ttopen();
    return(Raw(1));
}



/*----------------------------------------------------------------------
       End use of the tty, put it back into it's normal mode     (UNIX)

   Args: ps --  struct pine

 Result: tty driver mode change. 
  ----------------------------------------------------------------------*/
void
end_tty_driver(ps)
     struct pine *ps;
{
    ps = ps; /* get rid of unused parameter warning */

    /*___ttflush();*/
    dprint(2, (debugfile, "about to end_tty_driver\n"));

    Raw(0);
}



/*----------------------------------------------------------------------
    Actually set up the tty driver                             (UNIX)

   Args: state -- which state to put it in. 1 means go into raw, 0 out of

  Result: returns 0 if successful and -1 if not.
  ----*/

/* For setting and resting the PASTHUR flag: */
struct	{
	unsigned char	class, type;
	unsigned short	width;
	unsigned long	characteristics[2];
	} TerminalChar;


Raw(state)
int state;
{
	int	status;

/* Set the PASTHRU bit */
	status = sys$qiow((int)(0), TerminalChan, (short)(IO$_SENSEMODE),
			NULL, (int)(0), (int)(0),
			&TerminalChar, sizeof(TerminalChar),
			(int)(0), (int)(0), (int)(0), (int)(0));
	if((status & 0x1) == 0) {
		printf("Can't read for set terminal to /PASSALL\n");
		exit(status);
	}

	if(state != 0)
		TerminalChar.characteristics[1] |= TT2$M_PASTHRU;
	else
		TerminalChar.characteristics[1] &= ~TT2$M_PASTHRU;

	status = sys$qiow((int)(0), TerminalChan, (short)(IO$_SETMODE),
			NULL, (int)(0), (int)(0),
			&TerminalChar, sizeof(TerminalChar),
			(int)(0), (int)(0), (int)(0), (int)(0));
	if((status & 0x1) == 0) {
		printf("Can't set terminal to /PASSALL\n");
		exit(status);
	}

    return(0);
}



/*----------------------------------------------------------------------
     Lowest level read command. This reads one character with timeout. (UNIX)

~~~~~~~ >Do not use timeout at present.
    Args:  time_out --  number of seconds before read will timeout

  Result: Returns a single character read or a NO_OP_COMMAND if the
          timeout expired, or a KEY_RESIZE if a resize even occured.

  ----*/
int
read_with_timeout(time_out)
     int time_out;
{
	unsigned char c;
	int i;
	long	status;
	unsigned long Terminator[4] = { -1, -1, -1, -1};
	struct DESC	TerminatorDesc = { sizeof(Terminator), 0, Terminator };
	short	iosb[4];

	status = sys$qiow((int)(0), TerminalChan,
		(short)(IO$_READLBLK | IO$M_NOFORMAT | IO$M_NOFILTR | IO$M_NOECHO | IO$M_TRMNOECHO),
			iosb, (int)(0), (int)(0),
			&c, 1, (int)(0), &TerminatorDesc, (int)(0), (int)(0));
	if(((status & 0x1) == 0) || ((iosb[0] & 0x1) == 0)) {
		printf("Can't read from terminal; status=%d, iosb=%d\n",
			status, iosb[0]);
		exit(status);
	}

	return((int)(c & 0xff));
}
 


/*----------------------------------------------------------------------
  Read input characters with lots of processing for arrow keys and such  (UNIX)

 Args:  time_out -- The timeout to for the reads 
        char_in     -- Input character that needs processing

 Result: returns the character read. Possible special chars defined h file

    This deals with function and arrow keys as well. 

  The idea is that this routine handles all escape codes so it done in
  only one place. Especially so the back arrow key can work when entering
  things on a line. Also so all function keys can be disabled and not
  cause weird things to happen.


  Assume here that any chars making up an escape sequence will be close 
  together over time. It's possible for a timeout to occur waiting for rest
  of escape sequence if it takes more than 30 seconds to type the 
  escape sequence. The timeout will effectively cancel the escape sequence.

  ---*/
int
read_char(time_out)
     int time_out;
{
    register int  rx, ch, num_keys;
    static int firsttime = 1;

    /* commands in config file */
    if(ps_global->initial_cmds && *ps_global->initial_cmds) {
      /*
       * There are a few commands that may require keyboard input before
       * we enter the main command loop.  That input should be interactive,
       * not from our list of initial keystrokes.
       */
      if(ps_global->dont_use_init_cmds)
	goto get_one_char;
      ch = *ps_global->initial_cmds++;
      return(ch);
    }
    if(firsttime) {
      firsttime = 0;
      if(ps_global->in_init_seq) {
        ps_global->in_init_seq = 0;
        ps_global->save_in_init_seq = 0;
	F_SET(F_USE_FK,ps_global,ps_global->orig_use_fkeys);
        /* draw screen */
        return(ctrl('L'));
      }
    }
get_one_char:

    rx = 0; /* avoid ref before set errors */
    ch = read_with_timeout(time_out);
    if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND || ch == KEY_RESIZE) goto done;
    ch &= 0x7f;
    switch(ch) {
    
      case '\033':
        ch = read_with_timeout(time_out);
        if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND || ch == KEY_RESIZE)
          goto done;
        ch &= 0x7f;
        if(ch == 'O') {
            /* For DEC terminals, vt100s */
            ch = read_with_timeout(time_out);
            if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND || ch == KEY_RESIZE)
              goto done;
            ch &= 0x7f;
            switch(ch) {
              case 'P': return(PF1);
              case 'Q': return(PF2);
              case 'R': return(PF3);
              case 'S': return(PF4);
              case 'p': return(PF5);
              case 'q': return(PF6);
              case 'r': return(PF7);
              case 's': return(PF8);
              case 't': return(PF9);
              case 'u': return(PF10);
              case 'v': return(PF11);
              case 'w': return(PF12);
              case 'A': return(KEY_UP);
              case 'B': return(KEY_DOWN);
              case 'C': return(KEY_RIGHT);
              case 'D': return(KEY_LEFT);
              default: return(KEY_JUNK);
            }
        } else if(ch == '[') {
            /* For dec terminals, vt200s, and some weird Sun stuff */
            ch = read_with_timeout(time_out);
            if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND || ch == KEY_RESIZE)
              goto done;
            ch &= 0x7f;
            switch(ch) {
              case 'A': return(KEY_UP);
              case 'B': return(KEY_DOWN);
              case 'C': return(KEY_RIGHT);
              case 'D': return(KEY_LEFT);
    
              case '=': /* ansi terminal function keys */
                ch = read_with_timeout(time_out);
                if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND || ch == KEY_RESIZE)
                  goto done;
                ch &= 0x7f;
                switch(ch) {
                  case 'a': return(PF1);
                  case 'b': return(PF2);
                  case 'c': return(PF3);
                  case 'd': return(PF4);
                  case 'e': return(PF5);
                  case 'f': return(PF6);
                  case 'g': return(PF7);
                  case 'h': return(PF8);
                  case 'i': return(PF9);
                  case 'j': return(PF10);
                  case 'k': return(PF11);
                  case 'l': return(PF12);
                  default: return(KEY_JUNK);
                }
              case '1': /* Sun keys */
                  rx = KEY_JUNK; goto swallow_till_z;
    
              case '2': /* Sun keys */
                  ch = read_with_timeout(2);
                  if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND)
		    return(KEY_JUNK);
		  if(ch == KEY_RESIZE)
                    goto done;
                  ch &= 0x7f;
                  if(ch == '1') {
                      ch = read_with_timeout(2);
                      if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND)
			return(KEY_JUNK);
		      if(ch == KEY_RESIZE)
			goto done;
                      ch &= 0x7f;
                      switch (ch) {
                        case '5':  rx = KEY_UP; break;
                        case '7':  rx = KEY_LEFT; break;
                        case '9':  rx = KEY_RIGHT; break;
                        default:   rx = KEY_JUNK;
                      }
                  } else if (ch == '2') {
                      ch = read_with_timeout(2);
                      if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND)
			return(KEY_JUNK);
		      if(ch == KEY_RESIZE)
			goto done;
                      ch &= 0x7f;
                      if(ch == '1')
                        rx = KEY_DOWN;
                      else
                        rx = KEY_JUNK;
                  } else {
		      rx = KEY_JUNK;
		  }
                swallow_till_z:		  
    
                 while (ch != 'z' && ch != '~' ){
		      /* Read characters in escape sequence. "z"
                         ends things for suns and "~" for vt100's
                       */ 
                      ch = read_with_timeout(2);
                      if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND)
			return(KEY_JUNK);
		      if(ch == KEY_RESIZE)
			  goto done;
                      ch &= 0x7f;
                  } 
                  return(rx);			 
                      
                         
              default:
                /* DEC function keys */
                num_keys = 0;
                do {
                    ch = read_with_timeout(2);
                    if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND)
		      break;
		    if(ch == KEY_RESIZE)
		      goto done;
                    ch &= 0x7f;
                } while (num_keys++ < 6 && ch != '~');
                return(KEY_JUNK);
            }   
        } else if(ch == '?') {
             /* DEC vt52 application keys, and some Zenith 19 */
             ch = read_with_timeout(time_out);
             if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND ||
                ch == KEY_RESIZE) goto done;
             ch &= 0x7f;
             switch(ch) {
               case 'r': return(KEY_DOWN);
               case 't': return(KEY_LEFT);
               case 'v': return(KEY_RIGHT);
               case 'x': return(KEY_UP);
               default: return(KEY_JUNK);
             }    
        } else if(ch == '\033'){
	     /* special hack to get around comm devices eating 
	      * control characters.
	      */
             ch = read_with_timeout(time_out);
             if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND ||
                ch == KEY_RESIZE) goto done;
             ch &= 0x7f;
	     if(islower(ch))		/* canonicalize if alpha */
	       ch = toupper(ch);

	     return((isalpha(ch) || ch == '@' || (ch >= '[' && ch <= '_'))
		    ? ctrl(ch) : ch);
	    
	} else {
             /* This gets most Z19 codes, and some VT52 modes */
             switch(ch) {
               case 'A': return(KEY_UP);
               case 'B': return(KEY_DOWN);
               case 'C': return(KEY_RIGHT);
               case 'D': return(KEY_LEFT);
               default:  return(KEY_JUNK);
             }    
        }

     default:
     done:
        dprint(9, (debugfile, "Read char returning: %d %s\n",
                   ch, pretty_command(ch)));
        return(ch);
    }
}


/*----------------------------------------------------------------------
         Highest level read for reading Pine commands.    (UNIX)

   Args:  none

 Result: Retuns key press read. (keyboard escape sequences decoded)

 Calculates the timeout for the read, and does a few other house keeping 
things.
  ----*/
int
read_command()
{
    int ch, tm;

    tm = timeout > 0 ? messages_queued() ? messages_queued() : timeout : 0;
    ch = read_char(tm);
    dprint(9, (debugfile, "Read command returning: %d %s\n", ch,
              pretty_command(ch)));
    if(ch != NO_OP_COMMAND && ch != NO_OP_IDLE && ch != KEY_RESIZE)
      zero_new_mail_count();
    return(ch);
}



extern char ttermname[]; /* termname from ttyout.c-- affect keyboard*/
/* -------------------------------------------------------------------
     Set up the keyboard -- usually enable some function keys  (UNIX)

    Args: struct pine 

So far all we do here is turn on keypad mode for certain terminals

Hack for NCSA telnet on an IBM PC to put the keypad in the right mode.
This is the same for a vtXXX terminal or [zh][12]9's which we have 
a lot of at UW
  ----*/
void
init_keyboard(use_fkeys)
     int use_fkeys;
{
    if(use_fkeys && (!strucmp(ttermname,"vt102") || !strucmp(ttermname,"vt100")))
      printf("\033\133\071\071\150");

}



/*----------------------------------------------------------------------
     Clear keyboard, usually disable some function keys           (UNIX)

   Args:  pine state (terminal type)

 Result: keyboard state reset
  ----*/
void
end_keyboard(use_fkeys)
     int use_fkeys;
{
    if(use_fkeys && (!strcmp(ttermname, "vt102") || !strcmp(ttermname, "vt100"))){
      printf("\033\133\071\071\154");
      /*___ttflush();*/
    }

}

/*
 *
 */
static struct display_line {
    int   row, col;			/* where display starts		 */
    int   dlen;				/* length of display line	 */
    char *dl;				/* line on display		 */
    char *vl;				/* virtual line 		 */
    int   vlen;				/* length of virtual line        */
    int   vused;			/* length of virtual line in use */
    int   vbase;			/* first virtual char on display */
} dline;



static struct key oe_keys[] =
    {{"^G","Help",0},     {"RETURN","Enter",0},     {"^C","Cancel",0},
     {"^T","xxx",0},      {NULL,NULL,0},            {NULL,NULL,0},
     {NULL,NULL,0},       {NULL,NULL,0},            {NULL,NULL,0},
     {NULL,NULL,0},       {NULL,NULL,0},            {NULL,NULL,0}};
static struct key_menu oe_keymenu =
	{sizeof(oe_keys)/(sizeof(oe_keys[0])*12), 0, 0,0,0,0, oe_keys};
#define OE_HELP_KEY 0
#define OE_ENTER_KEY 1
#define OE_CANCEL_KEY 2
#define OE_CTRL_T_KEY 3


/*---------------------------------------------------------------------- 
       Prompt user for a string in status line with various options

  Args: string -- the buffer result is returned in, and original string (if 
                   any) is passed in.
        y_base -- y position on screen to start on. 0,0 is upper left
                    negative numbers start from bottom
        x_base -- column position on screen to start on. 0,0 is upper left
        field_len -- Maximum length of string to accept
        append_current -- flag indicating string should not be truncated before
                          accepting input
        passwd -- a pass word is being fetch. Don't echo on screen
        prompt -- The string to prompt with
	escape_list -- pointer to array of ESCKEY_S's.  input chars matching
                       those in list return value from list.
        help   -- Arrary of strings for help text in bottom screen lines
        disallow_cancel -- flag indicating ^C for cancel is not allowed

  Result:  editing input string
            returns -1 unexpected errors
            returns 0  normal entry typed (editing and return or PF2)
            returns 1  typed ^C or PF3 (cancel)
            returns 3  typed ^G or PF1 (help)
            returns 4  typed ^L for a screen redraw

  WARNING: Care is required with regard to the escape_list processing.
           The passed array is terminated with an entry that has ch = -1.
           Function key labels and key strokes need to be setup externally!
	   Traditionally, a return value of 2 is used for ^T escapes.

   Unless in escape_list, tabs are trapped by isprint().
This allows near full weemacs style editing in the line
   ^A beginning of line
   ^E End of line
   ^R Redraw line
   ^G Help
   ^F forward
   ^B backward
   ^D delete
----------------------------------------------------------------------*/

optionally_enter(string, y_base, x_base, field_len, append_current, passwd,
                 prompt, escape_list, help, disallow_cancel)
     char       *string, *prompt;
     ESCKEY_S   *escape_list;
     HelpType	 help;
     int         x_base, y_base, field_len, append_current, passwd,
                 disallow_cancel;
{
    register char *s2;
    register int   field_pos;
    int            j, return_v, cols, ch, prompt_len, too_thin, real_y_base,
                   cursor_moved;
    char          *saved_original = NULL, *k, *kb;
    char          *kill_buffer = NULL;
    char          **help_text;

    dprint(5, (debugfile, "=== optionally_enter called ===\n"));
    dprint(9, (debugfile, "string:\"%s\"  y:%d  x:%d  length: %d append: %d\n",
               string, x_base, y_base, field_len, append_current));
    dprint(9, (debugfile, "passwd:%d   prompt:\"%s\"   label:\"%s\"\n",
               passwd, prompt, escape_list == NULL ? "" : 
	       escape_list[0].label));

    cols       = ps_global->ttyo->screen_cols;
    prompt_len = strlen(prompt);
    too_thin   = 0;
    if(y_base > 0) {
        real_y_base = y_base;
    } else {
        real_y_base=  y_base + ps_global->ttyo->screen_rows;
        if(real_y_base < 2)
          real_y_base = ps_global->ttyo->screen_rows;
    }

    mark_status_dirty();

    if(append_current) {
        /*---- save a copy in case of cancel -----*/
	saved_original = fs_get(strlen(string) + 1);
	strcpy(saved_original, string);
    }


    help_text = help;
    if(help_text != (char **)NULL) {
        /*---- Show help text -----*/
        MoveCursor(real_y_base + 1, x_base);
        CleartoEOLN();
        PutLine0(real_y_base+1, x_base, help_text[0]);
        MoveCursor(real_y_base + 2, x_base);
        CleartoEOLN();
        if(help_text[1] != NULL) 
          PutLine0(real_y_base + 2, x_base, help_text[1]);

    } else {
	BITMAP bitmap;
	struct key_menu *km;

	clrbitmap(bitmap);
	km = &oe_keymenu;
	setbitn(OE_HELP_KEY, bitmap);
	setbitn(OE_ENTER_KEY, bitmap);
        if(!disallow_cancel)
	    setbitn(OE_CANCEL_KEY, bitmap);
	setbitn(OE_CTRL_T_KEY, bitmap);

        /*---- Show the usual possible keys ----*/
	for(j=3; escape_list && j < 12; j++){
	    if(escape_list[j-3].ch == -1)
	      break;

	    oe_keymenu.keys[j].label = escape_list[j-3].label;
	    oe_keymenu.keys[j].name  = escape_list[j-3].name;
	    setbitn(j, bitmap);
	}

	for(; j < 12; j++)
	  oe_keymenu.keys[j].name = NULL;

	draw_keymenu(km, bitmap, cols, -2, 0, FirstMenu, 0);
    }
    
    
    StartInverse();  /* Always in inverse  */

    /*
     * if display length isn't wide enough to support input,
     * shorten up the prompt...
     */
    if((dline.dlen = cols - (x_base + prompt_len + 1)) < 5){
	prompt_len += (dline.dlen - 5);	/* adding negative numbers */
	prompt     -= (dline.dlen - 5);	/* subtracting negative numbers */
	dline.dlen  = 5;
    }

    dline.dl    = fs_get((size_t)dline.dlen + 1);
    memset((void *)dline.dl, 0, (size_t)dline.dlen * sizeof(char));
    dline.row   = real_y_base;
    dline.col   = x_base + prompt_len;
    dline.vl    = string;
    dline.vlen  = --field_len;		/* -1 for terminating NULL */
    dline.vbase = field_pos = 0;

    PutLine0(real_y_base, x_base, prompt);
    /* make sure passed in string is shorter than field_len */
    /* and adjust field_pos..                               */

    while(append_current && (field_pos < field_len) && (string[field_pos] != '\0'))
      field_pos++;

    string[field_pos] = '\0';
/*~~~    dline.vused = (int)(&string[field_pos] - string); */
    dline.vused = (int)(field_pos);
    line_paint(field_pos, &passwd);
    ___ttflush();

    /*----------------------------------------------------------------------
      The main loop
   
    here field_pos is the position in the string.
    s always points to where we are in the string.
    loops until someone sets the return_v.
      ----------------------------------------------------------------------*/
    return_v = -10;

    while(return_v == -10) {

	/* Timeout 5 min to keep imap mail stream alive */
        ch = read_char(600);

        if(too_thin && ch != KEY_RESIZE && ch != ctrl('Z'))
          goto bleep;

	switch(ch) {

	    /*--------------- KEY RIGHT ---------------*/
          case ctrl('F'):  
	  case KEY_RIGHT:
	    if(field_pos >= field_len || string[field_pos] == '\0')
              goto bleep;

	    line_paint(++field_pos, &passwd);
	    break;

	    /*--------------- KEY LEFT ---------------*/
          case ctrl('B'):
	  case KEY_LEFT:
	    if(field_pos <= 0)
	      goto bleep;

	    line_paint(--field_pos, &passwd);
	    break;

          /*--------------------  RETURN --------------------*/
	  case PF2:
	    if(F_OFF(F_USE_FK,ps_global)) goto bleep;
	  case ctrl('J'): 
	  case ctrl('M'): 
	    return_v = 0;
	    break;

#ifdef DEBUG
          case ctrl('X'):
            if(debug < 9)
              goto bleep;
            printf("field_pos: %d \n", field_pos);
	    for(s2 = string; s2 < &string[field_len] ; s2++) 
	      printf("%s%d --%c--\n", s2 == &string[field_pos] ?"-->" :"   ",
                     *s2, *s2);
            break;
#endif
 
          /*-------------------- Destructive backspace --------------------*/
	  case '\177': /* DEL */
	  case ctrl('H'):
            /*   Try and do this with by telling the terminal to delete a
                 a character. If that fails, then repaint the rest of the
                 line, acheiving the same much less efficiently
             */
	    if(field_pos <= 0) goto bleep;
	    field_pos--;
	    /* drop thru to pull line back ... */

          /*-------------------- Delete char --------------------*/
	  case ctrl('D'): 
	  case KEY_DEL: 
            if(field_pos >= field_len || !string[field_pos]) goto bleep;

	    dline.vused--;
	    for(s2 = &string[field_pos]; *s2 != '\0'; s2++)
	      *s2 = s2[1];

	    *s2 = '\0';			/* Copy last NULL */
	    line_paint(field_pos, &passwd);
	    break;


            /*--------------- Kill line -----------------*/
          case ctrl('K'):
            if(kill_buffer != NULL)
              fs_give((void **)&kill_buffer);

            kill_buffer = cpystr(string);
            string[0] = '\0';
            field_pos = 0;
	    dline.vused = 0;
	    line_paint(field_pos, &passwd);
            break;

            /*------------------- Undelete line --------------------*/
          case ctrl('U'):
            if(kill_buffer == NULL)
              goto bleep;

            /* Make string so it will fit */
            kb = cpystr(kill_buffer);
            dprint(2, (debugfile, "Undelete: %d %d\n", strlen(string), field_len));
            if(strlen(kb) + strlen(string) > field_len) 
                kb[field_len - strlen(string)] = '\0';
            dprint(2, (debugfile, "Undelete: %d %d\n", field_len - strlen(string),
                       strlen(kb)));
            if(string[field_pos] == '\0') {
                /*--- adding to the end of the string ----*/
                for(k = kb; *k; k++)
		  string[field_pos++] = *k;

                string[field_pos] = '\0';
            } else {
                goto bleep;
                /* To lazy to do insert in middle of string now */
            }

	    dline.vused = strlen(string);
            fs_give((void **)&kb);
	    line_paint(field_pos, &passwd);
            break;
            

	    /*-------------------- Interrupt --------------------*/
	  case ctrl('C'): /* ^C */ 
	    if(F_ON(F_USE_FK,ps_global) || disallow_cancel) goto bleep;
	    goto cancel;
	  case PF3:
	    if(F_OFF(F_USE_FK,ps_global) || disallow_cancel) goto bleep;
	  cancel:
	    return_v = 1;
	    if(saved_original != NULL)
	      strcpy(string, saved_original);
	    break;
	    

          case ctrl('R'):
            /*----------- Repaint current line --------------*/
            PutLine0(real_y_base, x_base, prompt);
	    memset((void *)dline.dl, 0, dline.dlen);
	    line_paint(field_pos, &passwd);
	    break;

          case ctrl('A'):
	  case KEY_HOME:
            /*-------------------- Start of line -------------*/
	    line_paint(field_pos = 0, &passwd);
            break;


          case ctrl('E'):
	  case KEY_END:
            /*-------------------- End of line ---------------*/
	    line_paint(field_pos = dline.vused, &passwd);
            break;


          case NO_OP_COMMAND:
          case NO_OP_IDLE:
            new_mail(&cursor_moved, 0, 2); /* Keep mail stream alive */
            if(!cursor_moved) /* In case checkpoint happened */
              break;
            /* Else fall into redraw */

	    
	    /*-------------------- Help --------------------*/
	  case ctrl('G') : 
	  case PF1:
	    return_v = 3;
	    break;

	    /*-------------------- Redraw --------------------*/
	  case ctrl('L'):
            /*---------------- re size ----------------*/
          case KEY_RESIZE:
            
	    dline.row = real_y_base = y_base > 0 ? y_base :
					 y_base + ps_global->ttyo->screen_rows;
            EndInverse();
            ClearScreen();
            redraw_titlebar();
            if(ps_global->redrawer != (void (*)())NULL)
              (*ps_global->redrawer)();
            redraw_keymenu();

            StartInverse();
            
            PutLine0(real_y_base, x_base, prompt);
            cols     =  ps_global->ttyo->screen_cols;
            too_thin = 0;
            if(cols < x_base + prompt_len + 4) {
                PutLine0(real_y_base, 0, "\007Screen's too thin. Ouch!");
                too_thin = 1;
            } else {
		dline.col   = x_base + prompt_len;
		dline.dlen  = cols - x_base - prompt_len - 1;
		fs_resize((void **)&dline.dl, (size_t)dline.dlen);
		memset((void *)dline.dl, 0, (size_t)dline.dlen);
		line_paint(field_pos, &passwd);
            }
            /*___ttflush();*/

            dprint(9, (debugfile,
                    "optionally_enter  RESIZE new_cols:%d  too_thin: %d\n",
                       cols, too_thin));
            break;

          case ctrl('Z'):
            if(!have_job_control() || passwd)
              goto bleep;
            if(F_ON(F_CAN_SUSPEND,ps_global)) {
                EndInverse();
                Writechar('\n', 0);
                Writechar('\n', 0);
                do_suspend(ps_global);
                return_v = 4;
            } else {
                goto bleep;
            }

  
          default:
	    if(escape_list){		/* in the escape key list? */
		for(j=0; escape_list[j].ch != -1; j++){
		    if(escape_list[j].ch == ch){
			return_v = escape_list[j].rval;
			break;
		    }
		}

		if(return_v != -10)
		  break;
	    }

	    if(!isprint(ch)) {
       bleep:
		___ttputc('\007');
		continue;
	    }

	    /*--- Insert a character -----*/
	    if(dline.vused >= field_len)
	      goto bleep;

	    /*---- extending the length of the string ---*/
	    for(s2 = &string[++dline.vused]; s2 - string > field_pos; s2--)
	      *s2 = *(s2-1);

	    string[field_pos++] = ch;
	    line_paint(field_pos, &passwd);
		    
	}   /*---- End of switch on char ----*/
	___ttflush();
    }

    fs_give((void **)&dline.dl);
    if(append_current) 
      fs_give((void *)&saved_original);

    if(kill_buffer)
      fs_give((void **)&kill_buffer);

    removing_trailing_white_space(string);
    EndInverse();
    MoveCursor(real_y_base, x_base); /* Move the cursor to show we're done */
    /*___ttflush();*/
    return(return_v);
}


/*
 * line_paint - where the real work of managing what is displayed gets done.
 *              The passwd variable is overloaded: if non-zero, don't
 *              output anything, else only blat blank chars across line
 *              once and use this var to tell us we've already written the 
 *              line.
 */
void
line_paint(offset, passwd)
    int   offset;			/* current dot offset into line */
    int  *passwd;			/* flag to hide display of chars */
{
    register char *pfp, *pbp;
    register char *vfp, *vbp;
    int            extra = 0;
#define DLEN	(dline.vbase + dline.dlen)

    /*
     * for now just leave line blank, but maybe do '*' for each char later
     */
    if(*passwd){
	if(*passwd > 1)
	  return;
	else
	  *passwd == 2;		/* only blat once */

	extra = 0;
	MoveCursor(dline.row, dline.col);
	while(extra++ < dline.dlen)
	  Writechar(' ', 0);

	MoveCursor(dline.row, dline.col);
	/*___ttflush();*/
	return;
    }

    /* adjust right margin */
    while(offset >= DLEN + ((dline.vused > DLEN) ? -1 : 1))
      dline.vbase += dline.dlen/2;

    /* adjust left margin */
    while(offset < dline.vbase + ((dline.vbase) ? 2 : 0))
      dline.vbase = max(dline.vbase - (dline.dlen/2), 0);

    if(dline.vbase){				/* off screen cue left */
	vfp = &dline.vl[dline.vbase+1];
	pfp = &dline.dl[1];
	if(dline.dl[0] != '<'){
	    MoveCursor(dline.row, dline.col);
	    Writechar(dline.dl[0] = '<', 0);
	}
    }
    else{
	vfp = dline.vl;
	pfp = dline.dl;
	if(dline.dl[0] == '<'){
	    MoveCursor(dline.row, dline.col);
	    Writechar(dline.dl[0] = ' ', 0);
	}
    }

    if(dline.vused > DLEN){			/* off screen right... */
	vbp = vfp + (long)(dline.dlen-(dline.vbase ? 2 : 1));
	pbp = pfp + (long)(dline.dlen-(dline.vbase ? 2 : 1));
	if(pbp[1] != '>'){
	    MoveCursor(dline.row, dline.col+dline.dlen);
	    Writechar(pbp[1] = '>', 0);
	}
    }
    else{
	extra = dline.dlen - (dline.vused - dline.vbase);
	vbp = &dline.vl[max(0, dline.vused-1)];
	pbp = &dline.dl[dline.dlen];
	if(pbp[0] == '>'){
	    MoveCursor(dline.row, dline.col+dline.dlen);
	    Writechar(pbp[0] = ' ', 0);
	}
    }

    while(*pfp == *vfp && vfp < vbp) {			/* skip like chars */
	      pfp++; vfp++;
    }

    if(pfp == pbp && *pfp == *vfp){			/* nothing to paint! */
	MoveCursor(dline.row, dline.col + (offset - dline.vbase));
	/*___ttflush();*/
	return;
    }

    /* move backward thru like characters */
    if(extra){
	while(extra >= 0 && *pbp == ' ') 		/* back over spaces */
	  extra--, pbp--;

	while(extra >= 0)				/* paint new ones    */
	  pbp[-(extra--)] = ' ';
    }

    if((vbp - vfp) == (pbp - pfp)){			/* space there? */
	while((*pbp == *vbp) && pbp != pfp)		/* skip like chars */
	  pbp--, vbp--;
    }

    if(pfp != pbp || *pfp != *vfp){			/* anything to paint? */
	MoveCursor(dline.row, dline.col + (int)(pfp - dline.dl));

	while(pfp <= pbp){
	    if(vfp <= vbp && *vfp)
              Writechar(*pfp = *vfp++, 0);
	    else
	      Writechar(*pfp = ' ', 0);

	    pfp++;
	}
    }

    MoveCursor(dline.row, dline.col + (offset - dline.vbase));
    /*___ttflush();*/
}



/*----------------------------------------------------------------------
    Check to see if the given command is reasonably valid
  
  Args:  ch -- the character to check

 Result:  A valid command is returned, or a well know bad command is returned.
 
 ---*/
validatekeys(ch)
     int  ch;
{
    if(F_ON(F_USE_FK,ps_global)) {
	if(ch >= 'a' && ch <= 'z')
	  return(KEY_JUNK);
    } else {
	if(ch >= PF1 && ch <= PF12)
	  return(KEY_JUNK);
    }
    return(ch);
}


/* DUmmy functions */
void get_windsize()
{}
