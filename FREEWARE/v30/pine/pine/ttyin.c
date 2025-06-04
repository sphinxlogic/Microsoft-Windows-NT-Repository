#if !defined(lint) && !defined(DOS)
static char rcsid[] = "$Id: ttyin.c,v 4.26 1993/11/18 01:43:50 mikes Exp $";
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
       ttyin.c
       Things having to do with reading from the tty driver and keyboard
          - initialize tty driver and reset tty driver
          - read a character from terminal with keyboard escape seqence mapping
          - initialize keyboard (keypad or such) and reset keyboard
          - prompt user for a line of input
          - read a command from keyboard with timeouts.

 ====*/

#include "headers.h"

#ifdef	ANSI
void   line_paint(int, int *);
#else
void   line_paint();
#endif

#ifndef DOS  /* Beginning of giant switch between UNIX and DOS input driver */

#ifdef USE_POLL
#include <stropts.h>
#include <poll.h>
#endif

#ifdef HAVE_TERMIOS
#include <termios.h>

struct termios _raw_tty, _original_tty;

#else
#ifdef HAVE_TERMIO
#include <termio.h>

static struct termio _raw_tty, _original_tty;


#else /* HAVE_TERMIO */

#include <sgtty.h>

static struct sgttyb  _raw_tty,     _original_tty;
static struct ltchars _raw_ltchars, _original_ltchars;
static struct tchars  _raw_tchars,  _original_tchars;
static int            _raw_lmode,   _original_lmode;
#endif /* HAVE_TERMIO  */
#endif /* HAVE_TERMIOS */

#define STDIN_FD 0

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

    fflush(stdout);
    dprint(2, (debugfile, "about to end_tty_driver\n"));

    Raw(0);
}



/*----------------------------------------------------------------------
    Actually set up the tty driver                             (UNIX)

   Args: state -- which state to put it in. 1 means go into raw, 0 out of

  Result: returns 0 if successful and -1 if not.
  ----*/

Raw(state)
int state;
{
    static int _inraw = 0;
    /** state is either ON or OFF, as indicated by call **/
    /* Check return code only on first call. If it fails we're done for and
       if it goes OK the other will probably go OK too. */

    if (state == 0 && _inraw) {
        /*----- restore state to original -----*/
#ifdef HAVE_TERMIOS
	if (tcsetattr (STDIN_FD, TCSADRAIN, &_original_tty) < 0)
		return -1;
#else  /* HAVE_TERMIOS */
#ifdef HAVE_TERMIO
        if(ioctl(STDIN_FD, TCSETAW, &_original_tty) < 0)
          return(-1);
#else
	if(ioctl(STDIN_FD, TIOCSETP, &_original_tty) < 0)
          return(-1);
	(void) ioctl(STDIN_FD, TIOCSLTC, &_original_ltchars);
	(void) ioctl(STDIN_FD, TIOCSETC, &_original_tchars);
        (void) ioctl(STDIN_FD, TIOCLSET, &_original_lmode);
#endif /* HAVE_TERMIO */
#endif /* HAVE_TERMIOS */
        _inraw = 0;
    } else if (state == 1 && ! _inraw) {
        /*----- Go into raw mode (cbreak actually) ----*/

#ifdef HAVE_TERMIOS
	if (tcgetattr (STDIN_FD, &_original_tty) < 0)
		return -1;
	tcgetattr (STDIN_FD, &_raw_tty);
	_raw_tty.c_lflag &= ~(ICANON | ECHO | IEXTEN);	/* noecho raw mode  */

 	_raw_tty.c_lflag &= ~ISIG;            /* disable signals */
 	_raw_tty.c_iflag &= ~ICRNL;           /* turn off CR->NL on input */
 	_raw_tty.c_oflag &= ~ONLCR;           /* turn off NL->CR on output */
 	_raw_tty.c_oflag &= ~OPOST;

	/* Only go into 8 bit mode if we are doing something other
	 * than plain ASCII. This will save the folks that have
	 * their parity on their serial lines wrong thr trouble of
	 * getting it right
	 */
        if(ps_global->VAR_CHAR_SET && ps_global->VAR_CHAR_SET[0] &&
	   strucmp(ps_global->VAR_CHAR_SET, "us-ascii"))
	  _raw_tty.c_iflag &= ~ISTRIP; 	       /* turn off hi bit stripping */

	_raw_tty.c_cc[VMIN]  = '\01'; /* minimum # of chars to queue  */
	_raw_tty.c_cc[VTIME] = '\0';  /* minimum time to wait for input */
	ps_global->low_speed = cfgetospeed(&_raw_tty) < B4800;
	tcsetattr (STDIN_FD, TCSADRAIN, &_raw_tty);

#else
#ifdef HAVE_TERMIO
        if(ioctl(STDIN_FD, TCGETA, &_original_tty) < 0)
          return(-1);
	(void) ioctl(STDIN_FD, TCGETA, &_raw_tty);    /** again! **/

	_raw_tty.c_lflag &= ~(ICANON | ECHO);	/* noecho raw mode  */

 	_raw_tty.c_lflag &= ~ISIG;            /* disable signals */
 	_raw_tty.c_iflag &= ~ICRNL;           /* turn off CR->NL on input */
 	_raw_tty.c_oflag &= ~ONLCR;           /* turn off NL->CR on output */
 	_raw_tty.c_oflag &= ~OPOST;
	_raw_tty.c_cc[VMIN]  = 1;  /* minimum # of chars to queue  */
	_raw_tty.c_cc[VTIME] = 0;  /* minimum time to wait for input */
        ps_global->low_speed = (_raw_tty.c_cflag & CBAUD) < (unsigned int)B4800;
	(void) ioctl(STDIN_FD, TCSETAW, &_raw_tty);

#else/* HAVE_TERMIO */
        if(ioctl(STDIN_FD, TIOCGETP, &_original_tty) < 0)
          return(-1);
	(void) ioctl(STDIN_FD, TIOCGETP, &_raw_tty);   
        (void) ioctl(STDIN_FD, TIOCGETC, &_original_tchars);
	(void) ioctl(STDIN_FD, TIOCGETC, &_raw_tchars);
	(void) ioctl(STDIN_FD, TIOCGLTC, &_original_ltchars);
	(void) ioctl(STDIN_FD, TIOCGLTC, &_raw_ltchars);
        (void) ioctl(STDIN_FD, TIOCLGET, &_original_lmode);
        (void) ioctl(STDIN_FD, TIOCLGET, &_raw_lmode);

	_raw_tty.sg_flags &= ~(ECHO);	/* echo off */
	_raw_tty.sg_flags |= CBREAK;	/* raw on    */
        _raw_tty.sg_flags &= ~CRMOD;    /* Turn off CR -> LF mapping */

	_raw_tchars.t_intrc = -1; /* Turn off ^C and ^D */
	_raw_tchars.t_eofc  = -1;

	_raw_ltchars.t_lnextc = -1;   /* Turn off ^V so we can use it */
	_raw_ltchars.t_dsuspc = -1;   /* Turn off ^Y so we can use it */
	_raw_ltchars.t_suspc  = -1;   /* Turn off ^Z; we just read 'em */
	_raw_ltchars.t_werasc = -1;   /* Turn off ^w word erase */
	_raw_ltchars.t_rprntc = -1;   /* Turn off ^R reprint line */
        _raw_ltchars.t_flushc = -1;   /* Turn off ^O output flush */

	/* Only go into 8 bit mode if we are doing something other
	 * than plain ASCII. This will save the folks that have
	 * their parity on their serial lines wrong thr trouble of
	 * getting it right
	 */
        if(ps_global->VAR_CHAR_SET && ps_global->VAR_CHAR_SET[0] &&
	   strucmp(ps_global->VAR_CHAR_SET, "us-ascii")) {
	    _raw_lmode |= LPASS8;
#ifdef NXT  /* Hope there aren't many specific like this! */
            _raw_lmode |= LPASS8OUT;
#endif
	}
            
	(void) ioctl(STDIN_FD, TIOCSETP, &_raw_tty);
	(void) ioctl(STDIN_FD, TIOCSLTC, &_raw_ltchars);
        (void) ioctl(STDIN_FD, TIOCSETC, &_raw_tchars);
        (void) ioctl(STDIN_FD, TIOCLSET, &_raw_lmode);
        ps_global->low_speed =  (int)_raw_tty.sg_ispeed < B4800;

#endif
#endif
         _inraw = 1;
    }
    return(0);
}



jmp_buf winch_state;
int     winch_occured = 0;
int     ready_for_winch = 0;

/*----------------------------------------------------------------------
     Lowest level read command. This reads one character with timeout. (UNIX)

    Args:  time_out --  number of seconds before read will timeout

  Result: Returns a single character read or a NO_OP_COMMAND if the
          timeout expired, or a KEY_RESIZE if a resize even occured.

  ----*/
int
read_with_timeout(time_out)
     int time_out;
{
#ifdef USE_POLL
     struct pollfd pollfd;
     int    res;
#else
     struct timeval tmo;
     int            readfds, res;
#endif
     unsigned char  c;

     fflush(stdout);

     if(winch_occured) {
         winch_occured = 0;
         return(KEY_RESIZE);
     }


     if(setjmp(winch_state) != 0) {
         ready_for_winch = 0;
         winch_occured   = 0;
#ifdef POSIX_SIGNALS
	 /* Need to reset signal after longjmp from handler. */
	 sigrelse (SIGWINCH);
#endif
         return((int)KEY_RESIZE);
     } else {
         ready_for_winch = 1;
     }

     if(time_out > 0) {
         /* Check to see if there's bytes to read with a timeout */
#ifdef USE_POLL
	pollfd.fd = STDIN_FD;
	pollfd.events = POLLIN;
	dprint(9,(debugfile,"poll event %d, timeout %d\n", pollfd.events, time_out));
	res = poll (&pollfd, 1, time_out * 1000);
	dprint(9, (debugfile, "poll on tty returned %d, events %d\n", res, pollfd.revents));
#else
         if(STDIN_FD == 0)
           readfds    = 1;
         else
           readfds    = 1 << STDIN_FD; /* First bit for file desc 0 */
         tmo.tv_sec  = time_out;
         tmo.tv_usec = 0; 

         dprint(9,(debugfile,"Select readfds:%d timeval:%d,%d\n",readfds, tmo.tv_sec,tmo.tv_usec));

         res = select(STDIN_FD+1, &readfds, 0, &readfds, &tmo);

         dprint(9, (debugfile, "Select on tty returned %d\n", res));
#endif

         if(res < 0) {
             if(errno == EINTR) {
                 ready_for_winch = 0;
                 return(NO_OP_COMMAND);
             }
             panic1("Select error: %s\n", error_description(errno));
         }

         if(res == 0) { /* the select timed out */
             ready_for_winch = 0;
             return(time_out > 25 ? NO_OP_IDLE: NO_OP_COMMAND);
         }
     }

     res = read(STDIN_FD, &c, 1);
     dprint(9, (debugfile, "Read returned %d\n", res));

     ready_for_winch = 0;

     if(res <= 0) {
         /* Got an error reading from the terminal. Treat this like
            a SIGHUP: clean up and exit. */
         dprint(1, (debugfile, "\n\n** Error reading from tty : %s\n\n",
                error_description(errno)));

         if(errno == EINTR)
           return(NO_OP_COMMAND);

         if(ps_global->inbox_stream != NULL){
             if(ps_global->inbox_stream == ps_global->mail_stream)
               ps_global->mail_stream = NULL; 
             mail_close(ps_global->inbox_stream);
         }
         if(ps_global->mail_stream != NULL &&
            ps_global->mail_stream != ps_global->inbox_stream)
           mail_close(ps_global->mail_stream);
     
         MoveCursor(ps_global->ttyo->screen_rows -1, 0);
         NewLine();
         end_keyboard(F_ON(F_USE_FK,ps_global));
         end_tty_driver(ps_global);
         printf("\n\n\nPine finished. Error reading from terminal: %s\n",
           error_description(errno));
         exit(0);
     }

     return((int)c);
     
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
#ifndef OLD_READ_CHAR
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


#else
int
read_char(time_out)
     int time_out;
{
    register int  rx, ch, num_keys;

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
                  ch = read_with_timeout(time_out);
                  if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND ||
                     ch == KEY_RESIZE)
                    goto done;
                  ch &= 0x7f;
                  if(ch == '1') {
                      ch = read_with_timeout(time_out);
                      if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND ||
                         ch == KEY_RESIZE) goto done;
                      ch &= 0x7f;
                      switch (ch) {
                        case '5':  rx = KEY_UP; break;
                        case '7':  rx = KEY_LEFT; break;
                        case '9':  rx = KEY_RIGHT; break;
                        default:   rx = KEY_JUNK;
                      }
                  } else if (ch == '2') {
                      ch = read_with_timeout(time_out);
                      if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND ||
                         ch == KEY_RESIZE) goto done;
                      ch &= 0x7f;
                      if(ch == '1')
                        rx = KEY_DOWN;
                      else
                        rx = KEY_JUNK;
                  }
                    swallow_till_z:		  
    
		  num_keys = 0;
                  do {
                      ch = read_with_timeout(time_out);
                      if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND ||
                         ch == KEY_RESIZE) goto done;
                      ch &= 0x7f;
                  } while (num_keys++ < 4 && ch != 'z');
                  return(rx);			 
                      
                         
              default:
                /* DEC function keys */
                num_keys = 0;
                do {
                    ch = read_with_timeout(time_out);
                    if(ch == NO_OP_IDLE || ch == NO_OP_COMMAND ||
                       ch == KEY_RESIZE) goto done;
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
#endif



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
      fflush(stdout);
    }

}

    
#else /* DOS  Middle of giant siwtch UNX and DOS input drivers */

#include <dos.h>
#include <bios.h>

/* global to tell us if we have an enhanced keyboard! */
static int enhanced = 0;
/* global function to execute while waiting for character input */
void   (*while_waiting)() = NULL;

/* 
   This code compiled once, but it out of date with respect to the rest of
  Pine. Substantial work is needed to make it run
 */



/*----------------------------------------------------------------------
    Initialize the tty driver to do single char I/O and whatever else  (DOS)

 Input:  struct pine

 Result: tty driver is put in raw mode
  ----------------------------------------------------------------------*/
init_tty_driver(pine)
     struct pine *pine;
{
    union REGS in;

    /* detect enhanced keyboard */
    enhanced = enhanced_keybrd();	/* are there extra keys? */
    pine     = pine;			/* Get rid of unused parm warning */
    return(Raw(1));
}



/*----------------------------------------------------------------------
       End use of the tty, put it back into it's normal mode          (DOS)

 Input:  struct pine

 Result: tty driver mode change
  ----------------------------------------------------------------------*/
void
end_tty_driver(pine)
     struct pine *pine;
{
    dprint(2, (debugfile, "about to end_tty_driver\n"));
}

/*----------------------------------------------------------------------
   translate IBM Keyboard Extended Functions to things pine understands.
   More work can be done to make things like Home, PageUp and PageDown work. 

/*
 * extended_code - return special key definition
 */
extended_code(kc)
unsigned  kc;
{
    switch(kc){
	case 0x3b00 : return(PF1);
	case 0x3c00 : return(PF2);
	case 0x3d00 : return(PF3);
	case 0x3e00 : return(PF4);
	case 0x3f00 : return(PF5);
	case 0x4000 : return(PF6);
	case 0x4100 : return(PF7);
	case 0x4200 : return(PF8);
	case 0x4300 : return(PF9);
	case 0x4400 : return(PF10);
	case 0x8500 : return(PF11);
	case 0x8600 : return(PF12);
	case 0x4800 : return(KEY_UP);
	case 0x5000 : return(KEY_DOWN);
	case 0x4b00 : return(KEY_LEFT);
	case 0x4d00 : return(KEY_RIGHT);
	case 0x4700 : return(KEY_HOME);
	case 0x4f00 : return(KEY_END);
	case 0x4900 : return(KEY_PGUP);
	case 0x5100 : return(KEY_PGDN);
	case 0x5300 : return(KEY_DEL);
	case 0x48e0 : return(KEY_UP);			/* grey key version */
	case 0x50e0 : return(KEY_DOWN);			/* grey key version */
	case 0x4be0 : return(KEY_LEFT);			/* grey key version */
	case 0x4de0 : return(KEY_RIGHT);		/* grey key version */
	case 0x47e0 : return(KEY_HOME);			/* grey key version */
	case 0x4fe0 : return(KEY_END);			/* grey key version */
	case 0x49e0 : return(KEY_PGUP);			/* grey key version */
	case 0x51e0 : return(KEY_PGDN);			/* grey key version */
	case 0x53e0 : return(KEY_DEL);			/* grey key version */
	default   : return(NO_OP_COMMAND);
    }
}



/*----------------------------------------------------------------------
   Read input characters with lots of processing for arrow keys and such (DOS)

 Input:  none

 Result: returns the character read. Possible special chars defined h file


    This deals with function and arrow keys as well. 
  It returns ^T for up , ^U for down, ^V for forward and ^W for back.
  These are just sort of arbitrarily picked and might be changed.
  They are defined in defs.h. Didn't want to use 8 bit chars because
  the values are signed chars, though it ought to work with negative 
  values. 

  The idea is that this routine handles all escape codes so it done in
  only one place. Especially so the back arrow key can work when entering
  things on a line. Also so all function keys can be broken and not
  cause weird things to happen.
----------------------------------------------------------------------*/

int
read_char(tm)
int tm;
{
    unsigned ch = 0;
    long timein;
    static int firsttime = 1;

#ifdef	MOUSE
    extern int mexist;
    extern int checkmouse();
    extern void mouseon(), mouseoff();
#endif
    
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

#ifdef	MOUSE
    mouseon();
#endif

    if(tm){
	timein = time(0L);
	while(!_bios_keybrd(enhanced ? _NKEYBRD_READY : _KEYBRD_READY)){
	    if(time(0L) >= timein+tm){
		ch = NO_OP_COMMAND;
		goto gotone;
	    }
#ifdef	MOUSE
 	    if(checkmouse(&ch))
	      goto gotone;
#endif
	    if(while_waiting)
	      (*while_waiting)();
	}
    }

    ch = _bios_keybrd(enhanced ? _NKEYBRD_READ : _KEYBRD_READ);

gotone:
#ifdef	MOUSE
    mouseoff();
#endif
    /*
     * WARNING: Hack notice.
     * the mouse interaction complicates this expression a bit as 
     * if function key mode is set, PFn values are setup for return
     * by the mouse event catcher.  For now, just special case them
     * since they don't conflict with any of the DOS special keys.
     */
    return((ch >= PF1 && ch <= PF12) ? ch : 
	((ch&0xff) && ((ch&0xff) != 0xe0)) ? (ch&0xff) : extended_code(ch));
}


/* -------------------------------------------------------------------
     Set up the keyboard -- usually enable some function keys     (DOS)

  Input: struct pine (terminal type)

  Result: keyboard set up

-----------------------------------------------------------------------*/
void
init_keyboard(use_fkeys)
     int use_fkeys;
{
}



/*----------------------------------------------------------------------
     Clear keyboard, usually disable some function keys            (DOS)

 Input:  pine state (terminal type)

 Result: keyboard state reset
  ----------------------------------------------------------------------*/
/* BUG shouldn't have to check for pine != NULL */
void
end_keyboard(use_fkeys)
     int use_fkeys;
{

}




/*----------------------------------------------------------------------
        Read a character from keyboard with timeout               (DOS)
 Input:  none

 Result: Returns command read via readch_arar 
         Times out and returns a null command every so often

  The duration of the timeout is set in pine.c.
  ----------------------------------------------------------------------*/
int
read_command()
{
    unsigned ch;
    int tm;
    
    tm = timeout > 0 ? messages_queued() ? messages_queued() : timeout : 0;
    ch = read_char(tm);
    if(ch != NO_OP_COMMAND && ch != NO_OP_IDLE && ch != KEY_RESIZE)
      zero_new_mail_count();
    return(ch);
}

/*----------------------------------------------------------------------
    Actually set up the tty driver                             (UNIX)

   Args: state -- which state to put it in. 1 means go into raw, 0 out of

  Result: returns 0 if successful and -1 if not.
  ----*/

Raw(state)
int state;
{
/* of course, DOS never runs at low speed!!! */
    ps_global->low_speed = 0;
    return(0);
}

#endif /* DOS End of giant switch between UNX and DOS input drivers */

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


#ifdef	DOS
    help_text = (help != NO_HELP) ? get_help_text(help, NULL) : (char **)NULL;
#else
    help_text = help;
#endif
    if(help_text != (char **)NULL) {
        /*---- Show help text -----*/
        MoveCursor(real_y_base + 1, x_base);
        CleartoEOLN();
        PutLine0(real_y_base+1, x_base, help_text[0]);
        MoveCursor(real_y_base + 2, x_base);
        CleartoEOLN();
        if(help_text[1] != NULL) 
          PutLine0(real_y_base + 2, x_base, help_text[1]);
#ifdef	DOS
	free_help_text(help_text);
#endif

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

    while(append_current && field_pos < field_len && string[field_pos] != '\0')
      field_pos++;

    string[field_pos] = '\0';
    dline.vused = (int)(&string[field_pos] - string);
    line_paint(field_pos, &passwd);

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
            fflush(stdout);

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
		putc('\007', stdout);
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
    }

    fs_give((void **)&dline.dl);
    if(append_current) 
      fs_give((void *)&saved_original);

    if(kill_buffer)
      fs_give((void **)&kill_buffer);

    removing_trailing_white_space(string);
    EndInverse();
    MoveCursor(real_y_base, x_base); /* Move the cursor to show we're done */
    fflush(stdout);
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

    while(*pfp == *vfp && vfp < vbp)			/* skip like chars */
      pfp++, vfp++;

    if(pfp == pbp && *pfp == *vfp){			/* nothing to paint! */
	MoveCursor(dline.row, dline.col + (offset - dline.vbase));
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
