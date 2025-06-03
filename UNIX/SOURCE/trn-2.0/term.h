/* $Id: term.h,v 1.3 92/01/11 16:31:13 usenet Exp $
 *
 * $Log:	term.h,v $
 * Revision 1.3  92/01/11  16:31:13  usenet
 * Lots of bug fixes:
 * 
 *    o    More upgrade notes for trn 1.x users (see ** LOOK ** in NEW).
 *    o    Enhanced the article-reading code to not remember our side-trip
 *         to the end of the group between thread selections (to fix '-').
 *    o    Extended trrn's handling of new articles (we fetch the active file
 *         more consistently, and listen to what the GROUP command tells us).
 *    o    Enhanced the thread selector to mention when new articles have
 *         cropped up since the last visit to the selector.
 *    o    Changed strftime to use size_t and added a check for size_t in
 *         Configure to make sure it is defined.  Also made it a bit more
 *         portable by using gettimeofday() and timezone() in some instances.
 *    o    Fixed a problem with the "total" structure not getting zero'ed in
 *         mthreads (causing bogus 'E'rrors on null groups).
 *    o    Fixed a reference to tmpbuf in intrp.c that was bogus.
 *    o    Fixed a problem with using N and Y with the newgroup code and then
 *         trying to use the 'a' command.
 *    o    Fixed an instance where having trrn get ahead of the active file
 *         might declare a group as being reset when it wasn't.
 *    o    Reorganized the checks for Apollo's C library to make sure it doesn't
 *         find the Domain OS version.
 *    o    Added a check for Xenix 386's C library.
 *    o    Made the Configure PATH more portable.
 *    o    Fixed the arguments prototypes to int_catcher() and swinch_catcher().
 *    o    Fixed the insert-my-subject-before-my-sibling code to not do this
 *         when the sibling's subject is the same as the parent.
 *    o    Fixed a bug in the RELAY code (which I'm suprised is still being used).
 *    o    Twiddled the mthreads.8 manpage.
 *    o    mthreads.8 is now installed and the destination is prompted for in
 *         Configure, since it might be different from the .1 destination.
 *    o    Fixed a typo in newsnews.SH and the README.
 * 
 * Revision 1.2  92/01/11  16:09:09  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:27:37  sob
 * release 4.4
 *
 *
 * 
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

#ifdef PUSHBACK
EXT char circlebuf[PUSHSIZE];
EXT int nextin INIT(0);
EXT int nextout INIT(0);
#ifdef PENDING
#ifdef FIONREAD
EXT long iocount INIT(0);
#ifndef lint
#define input_pending() (nextin!=nextout || (ioctl(0, FIONREAD, &iocount),(int)iocount))
#else
#define input_pending() bizarre
#endif /* lint */
#else /* FIONREAD */
#ifdef RDCHK
#define input_pending() (rdchk(0) > 0)		/* boolean only */
#else /*  RDCHK */
int circfill();
EXT int devtty INIT(0);
#ifndef lint
#define input_pending() (nextin!=nextout || circfill())
#else
#define input_pending() bizarre
#endif /* lint */
#endif /* RDCHK */
#endif /* FIONREAD */
#else /* PENDING */
#ifndef lint
#define input_pending() (nextin!=nextout)
#else
#define input_pending() bizarre
#endif /* lint */
#endif /* PENDING */
#else /* PUSHBACK */
#ifdef PENDING
#ifdef FIONREAD	/* must have FIONREAD or O_NDELAY for input_pending() */
#define read_tty(addr,size) read(0,addr,size)
#ifndef lint
#define input_pending() (ioctl(0, FIONREAD, &iocount),(int)iocount)
#else
#define input_pending() bizarre
#endif /* lint */
EXT long iocount INIT(0);

#else /* FIONREAD */

#ifdef RDCHK
#define input_pending() (rdchk(0) > 0)		/* boolean only */
#else /*  RDCHK */

EXT int devtty INIT(0);
EXT bool is_input INIT(FALSE);
EXT char pending_ch INIT(0);
#ifndef lint
#define input_pending() (is_input || (is_input=read(devtty,&pending_ch,1)))
#else
#define input_pending() bizarre
#endif /* lint */
#endif /*  RDCHK */
#endif /* FIONREAD */
#else /* PENDING */
#define read_tty(addr,size) read(0,addr,size)
#define input_pending() (FALSE)
#endif /* PENDING */
#endif /* PUSHBACK */

/* stuff wanted by terminal mode diddling routines */

#ifdef TERMIO
EXT struct termio _tty, _oldtty;
#else
# ifdef TERMIOS
EXT struct termios _tty, _oldtty;
# else
EXT struct sgttyb _tty;
EXT int _res_flg INIT(0);
# endif
#endif

EXT int _tty_ch INIT(2);
EXT bool bizarre INIT(FALSE);			/* do we need to restore terminal? */

/* terminal mode diddling routines */

#ifdef TERMIO

#define crmode() ((bizarre=1),_tty.c_lflag &=~ICANON,_tty.c_cc[VMIN] = 1,ioctl(_tty_ch,TCSETAF,&_tty))
#define nocrmode() ((bizarre=1),_tty.c_lflag |= ICANON,_tty.c_cc[VEOF] = CEOF,stty(_tty_ch,&_tty))
#define echo()	 ((bizarre=1),_tty.c_lflag |= ECHO, ioctl(_tty_ch, TCSETA, &_tty))
#define noecho() ((bizarre=1),_tty.c_lflag &=~ECHO, ioctl(_tty_ch, TCSETA, &_tty))
#define nl()	 ((bizarre=1),_tty.c_iflag |= ICRNL,_tty.c_oflag |= ONLCR,ioctl(_tty_ch, TCSETAW, &_tty))
#define nonl()	 ((bizarre=1),_tty.c_iflag &=~ICRNL,_tty.c_oflag &=~ONLCR,ioctl(_tty_ch, TCSETAW, &_tty))
#define	savetty() (ioctl(_tty_ch, TCGETA, &_oldtty),ioctl(_tty_ch, TCGETA, &_tty))
#define	resetty() ((bizarre=0),ioctl(_tty_ch, TCSETAF, &_oldtty))
#define unflush_output()

#else /* !TERMIO */
# ifdef TERMIOS

#define crmode() ((bizarre=1), _tty.c_lflag &= ~ICANON,_tty.c_cc[VMIN]=1,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nocrmode() ((bizarre=1),_tty.c_lflag |= ICANON,_tty.c_cc[VEOF] = CEOF,tcsetattr(_tty_ch, TCSAFLUSH,&_tty))
#define echo()	 ((bizarre=1),_tty.c_lflag |= ECHO, tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define noecho() ((bizarre=1),_tty.c_lflag &=~ECHO, tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nl()	 ((bizarre=1),_tty.c_iflag |= ICRNL,_tty.c_oflag |= ONLCR,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nonl()	 ((bizarre=1),_tty.c_iflag &=~ICRNL,_tty.c_oflag &=~ONLCR,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define	savetty() (tcgetattr(_tty_ch, &_oldtty),tcgetattr(_tty_ch, &_tty))
#define	resetty() ((bizarre=0),tcsetattr(_tty_ch, TCSAFLUSH, &_oldtty))
#define unflush_output()

# else /* !TERMIOS */

#define raw()	 ((bizarre=1),_tty.sg_flags|=RAW, stty(_tty_ch,&_tty))
#define noraw()	 ((bizarre=1),_tty.sg_flags&=~RAW,stty(_tty_ch,&_tty))
#define crmode() ((bizarre=1),_tty.sg_flags |= CBREAK, stty(_tty_ch,&_tty))
#define nocrmode() ((bizarre=1),_tty.sg_flags &= ~CBREAK,stty(_tty_ch,&_tty))
#define echo()	 ((bizarre=1),_tty.sg_flags |= ECHO, stty(_tty_ch, &_tty))
#define noecho() ((bizarre=1),_tty.sg_flags &= ~ECHO, stty(_tty_ch, &_tty))
#define nl()	 ((bizarre=1),_tty.sg_flags |= CRMOD,stty(_tty_ch, &_tty))
#define nonl()	 ((bizarre=1),_tty.sg_flags &= ~CRMOD, stty(_tty_ch, &_tty))
#define	savetty() (gtty(_tty_ch, &_tty), _res_flg = _tty.sg_flags)
#define	resetty() ((bizarre=0),_tty.sg_flags = _res_flg, stty(_tty_ch, &_tty))
#  ifdef LFLUSHO
#   ifndef lint
EXT int lflusho INIT(LFLUSHO);
#   else
EXT long lflusho INIT(LFLUSHO);
#   endif /* lint */
#define unflush_output() (ioctl(_tty_ch,TIOCLBIC,&lflusho))
#  else
#define unflush_output()
#  endif /* LFLUSHO */
# endif /* TERMIOS */

#endif /* TERMIO */

#ifdef TIOCSTI
#ifdef lint
#define forceme(c) ioctl(_tty_ch,TIOCSTI,Null(long*))	/* ghad! */
#else
#define forceme(c) ioctl(_tty_ch,TIOCSTI,c) /* pass character in " " */
#endif /* lint */
#else
#define forceme(c)
#endif

/* termcap stuff */

/*
 * NOTE: if you don't have termlib you'll either have to define these strings
 *    and the tputs routine, or you'll have to redefine the macros below
 */

#ifdef HAVETERMLIB
EXT int GT;				/* hardware tabs */
EXT char *BC INIT(Nullch);		/* backspace character */
EXT char *UP INIT(Nullch);		/* move cursor up one line */
EXT char *CR INIT(Nullch);		/* get to left margin, somehow */
EXT char *VB INIT(Nullch);		/* visible bell */
EXT char *CL INIT(Nullch);		/* home and clear screen */
EXT char *CE INIT(Nullch);		/* clear to end of line */
EXT char *TI INIT(Nullch);		/* initialize terminal */
EXT char *TE INIT(Nullch);		/* reset terminal */
#if defined(CLEAREOL) || defined(USETHREADS)
EXT char *CM INIT(Nullch);		/* cursor motion */
EXT char *HO INIT(Nullch);		/* home cursor */
#endif
#ifdef CLEAREOL
EXT char *CD INIT(Nullch);		/* clear to end of display */
#endif /* CLEAREOL */
EXT char *SO INIT(Nullch);		/* begin standout mode */
EXT char *SE INIT(Nullch);		/* end standout mode */
EXT int SG INIT(0);			/* blanks left by SO and SE */
EXT char *US INIT(Nullch);		/* start underline mode */
EXT char *UE INIT(Nullch);		/* end underline mode */
EXT char *UC INIT(Nullch);		/* underline a character,
						 if that's how it's done */
EXT int UG INIT(0);			/* blanks left by US and UE */
EXT bool AM INIT(FALSE);		/* does terminal have automatic
								 margins? */
EXT bool XN INIT(FALSE);		/* does it eat 1st newline after
							 automatic wrap? */
EXT char PC INIT(0);			/* pad character for use by tputs() */

#ifdef _POSIX_SOURCE
EXT speed_t outspeed INIT(0);		/* terminal output speed, */
#else
EXT long outspeed INIT(0);		/* 	for use by tputs() */
#endif

EXT int LINES INIT(0), COLS INIT(0);	/* size of screen */
EXT int just_a_sec INIT(960);		/* 1 sec at current baud rate */
					/* (number of nulls) */

/* define a few handy macros */

#define backspace() tputs(BC,0,putchr) FLUSH
#define clear() tputs(CL,LINES,putchr) FLUSH
#define erase_eol() tputs(CE,1,putchr) FLUSH
#ifdef CLEAREOL
#define clear_rest() tputs(CD,LINES,putchr) FLUSH
#define maybe_eol() if(erase_screen&&can_home_clear)tputs(CE,1,putchr) FLUSH
#endif /* CLEAREOL */
#define underline() tputs(US,1,putchr) FLUSH
#define un_underline() tputs(UE,1,putchr) FLUSH
#define underchar() tputs(UC,0,putchr) FLUSH
#define standout() tputs(SO,1,putchr) FLUSH
#define un_standout() tputs(SE,1,putchr) FLUSH
#define up_line() tputs(UP,1,putchr) FLUSH
#define carriage_return() tputs(CR,1,putchr) FLUSH
#define dingaling() tputs(VB,1,putchr) FLUSH
#else
  ????????		/* up to you */
#endif

EXT int page_line INIT(1);	/* line number for paging in
						 print_line (origin 1) */

void	term_init ANSI((void));
void	term_set ANSI((char *));
#ifdef PUSHBACK
void	pushchar ANSI((char_int));
void	mac_init ANSI((char *));
void	mac_line ANSI((char *,char *,int));
void	show_macros ANSI((void));
#endif
char	putchr ANSI((char_int));	/* routine for tputs to call */
bool	finish_command ANSI((int));
void	eat_typeahead ANSI((void));
void	settle_down ANSI((void));
#ifdef HAVETERMLIB
void	termlib_init ANSI((void));
void	termlib_reset ANSI((void));
#endif
#ifndef read_tty
    int		read_tty ANSI((char *,int));
#endif
void	underprint ANSI((char *));
#ifdef NOFIREWORKS
    void	no_sofire ANSI((void));
    void	no_ulfire ANSI((void));
#endif
void	getcmd ANSI((char *));
int	get_anything ANSI((void));
void	in_char ANSI((char *,char_int));
int	print_lines ANSI((char *,int));
void	page_init ANSI((void));
void	pad ANSI((int));
void	printcmd ANSI((void));
void	rubout ANSI((void));
void	reprint ANSI((void));
#if defined(CLEAREOL) || defined(USETHREADS)
void	home_cursor ANSI((void));
#endif
#ifdef USETHREADS
void	goto_line ANSI((int,int));
#endif
#ifdef SIGWINCH
SIGRET	winch_catcher ANSI((int));
#endif /* SIGWINCH */
