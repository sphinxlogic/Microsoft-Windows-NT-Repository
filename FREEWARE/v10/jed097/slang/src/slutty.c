/* slutty.c --- Unix Low level terminal (tty) functions for S-Lang */
/* 
 * Copyright (c) 1994 John E. Davis
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <stdio.h>
#include <signal.h>
/* sequent support thanks to Kenneth Lorber <keni@oasys.dt.navy.mil> */
/* SYSV (SYSV ISC R3.2 v3.0) provided by iain.lea@erlm.siemens.de */

#ifndef sequent
# include <stdlib.h>
#endif
#include <sys/time.h>
#include <sys/ioctl.h>
#ifndef sequent
# include <unistd.h>
# include <termios.h>
#endif

#ifdef SYSV
# ifndef CRAY
#   include <sys/termio.h>
#   include <sys/stream.h>
#   include <sys/ptem.h>
#   include <sys/tty.h>
# endif
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "slang.h"
#include "_slang.h"

#define TTY_DESC 2
static int Read_FD = TTY_DESC;

int SLang_TT_Baud_Rate;

static int Baud_Rates[20] = 
{
   0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 
   9600, 19200, 38400, 0, 0, 0, 0
};



#ifdef sequent
struct ttystuff
  {
      struct tchars t;
      struct ltchars lt;
      struct sgttyb s;
  };
struct ttystuff OLDTTY;
#else
struct termios OLDTTY;
#endif

     /* this next works on ultrix for setting termios */
#ifdef TCGETS
#define GET_TERMIOS(fd, x) ioctl(fd, TCGETS, x)
#define SET_TERMIOS(fd, x) ioctl(fd, TCSETS, x)
#else
# ifdef sequent
#  define X(x,m)  &(((struct ttystuff*)(x))->m)
#  define GET_TERMIOS(fd, x)	\
	if(ioctl(fd, TIOCGETC, X(x,t))<0 || \
	ioctl(fd, TIOCGLTC, X(x,lt))<0 || \
	ioctl(fd, TIOCGETP, X(x,s))<0)exit_error("Can't get terminal info", 0)
#  define SET_TERMIOS(fd, x)	\
	if(ioctl(fd, TIOCSETC, X(x,t))<0 || \
	ioctl(fd, TIOCSLTC, X(x,lt))<0 || \
	ioctl(fd, TIOCSETP, X(x,s))<0)exit_error("Can't set terminal info", 0)
# else
#  define GET_TERMIOS(fd, x) tcgetattr(fd, x)
#  define SET_TERMIOS(fd, x) tcsetattr(fd, TCSAFLUSH, x)
/* #  define SET_TERMIOS(fd, x) tcsetattr(fd, TCSANOW, x) */
# endif
#endif

static int tty_inited = 0;

int SLang_init_tty (int abort_char, int no_flow_control, int opost)
{
#ifdef sequent
    struct ttystuff newtty;
#else
    struct termios newtty;
#endif

   SLang_Abort_Char = abort_char;
   GET_TERMIOS(Read_FD, &OLDTTY);
   GET_TERMIOS(Read_FD, &newtty);
#ifdef sequent
   newtty.s.sg_flags &= ~(ECHO);
   newtty.s.sg_flags &= ~(CRMOD);
   /*   if (Flow_Control == 0) newtty.s.sg_flags &= ~IXON; */
   newtty.t.t_eofc = 1;
   newtty.t.t_intrc = SLang_Abort_Char;	/* ^G */
   newtty.t.t_quitc = 255;
   newtty.lt.t_suspc = 255;   /* to ignore ^Z */
   newtty.lt.t_dsuspc = 255;    /* to ignore ^Y */
   newtty.lt.t_lnextc = 255;
   newtty.s.sg_flags |= CBREAK;		/* do I want cbreak or raw????? */
#else
   
   /* get baud rate */
   
   newtty.c_iflag &= ~(ECHO | INLCR | ICRNL);
#ifdef ISTRIP
   newtty.c_iflag &= ~ISTRIP;
#endif
   if (opost == 0) newtty.c_oflag &= ~OPOST;

   if (SLang_TT_Baud_Rate == 0)
     {
/* Note:  if this generates an compiler error, simply remove 
   the statement */
	SLang_TT_Baud_Rate = cfgetospeed (&newtty);
	
	
	SLang_TT_Baud_Rate = ((SLang_TT_Baud_Rate > 0) && (SLang_TT_Baud_Rate < 19)
			      ? Baud_Rates[SLang_TT_Baud_Rate]
			      : 0);
     }
   
   if (no_flow_control) newtty.c_iflag &= ~IXON;
   newtty.c_cc[VMIN] = 1;
   newtty.c_cc[VTIME] = 0;
   newtty.c_cc[VEOF] = 1;
   newtty.c_lflag = ISIG | NOFLSH;
   newtty.c_cc[VINTR] = SLang_Abort_Char;   /* ^G */
   newtty.c_cc[VQUIT] = 255;
   newtty.c_cc[VSUSP] = 255;   /* to ignore ^Z */
#ifdef VSWTCH
   newtty.c_cc[VSWTCH] = 255;   /* to ignore who knows what */
#endif
#endif /*sequent*/
   SET_TERMIOS(Read_FD, &newtty);
   tty_inited = 1;
   return 0;
}

void SLang_reset_tty (void)
{
   if (!tty_inited) return;
   SET_TERMIOS(Read_FD, &OLDTTY);
}


static void default_sigint (int sig)
{
   SLKeyBoard_Quit = 1;
   if (SLang_Ignore_User_Abort == 0) SLang_Error = USER_BREAK;
   signal (SIGINT, default_sigint);
}

void SLang_set_abort_signal (void (*hand)(int))
{
   if (hand == NULL) hand = default_sigint;
   signal (SIGINT, hand);
}

#ifndef FD_SET
#define FD_SET(fd, tthis) *(tthis) = 1 << fd
#define FD_ZERO(tthis)    *(tthis) = 0
typedef int fd_set;
#endif

static fd_set Read_FD_Set;


int SLsys_input_pending(int tsecs)
{
   struct timeval wait;
   long usecs, secs;
   int ret;

   secs = tsecs / 10;
   usecs = (tsecs % 10) * 100000;
   wait.tv_sec = secs;
   wait.tv_usec = usecs;
   
   FD_ZERO(&Read_FD_Set);
   FD_SET(Read_FD, &Read_FD_Set);
   ret = select(Read_FD + 1, &Read_FD_Set, NULL, NULL, &wait);
   return (ret);
}


unsigned int SLsys_getkey (void)
{
   int count = 10;		       /* number of times to retry if read fails */
   unsigned char c;
   
   while ((SLKeyBoard_Quit == 0)
	  && !SLsys_input_pending(100));
   
   while (count-- && !SLKeyBoard_Quit && (read(Read_FD, &c, 1) < 0)) sleep(1);
   
   if (count <= 0)
     {
	return 0xFFFF;
     }

   /* only way for keyboard quit to be non zero is if ^G recived and sigint processed */
   if (SLKeyBoard_Quit) c = SLang_Abort_Char;
   SLKeyBoard_Quit = 0;
   return((unsigned int) c);
}
