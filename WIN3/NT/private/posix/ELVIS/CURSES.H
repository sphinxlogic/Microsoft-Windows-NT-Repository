/* curses.h */

/* Author:
 *	Steve Kirkendall
 *	14407 SW Teal Blvd. #C
 *	Beaverton, OR 97005
 *	kirkenda@cs.pdx.edu
 */


/* This is the header file for a small, fast, fake curses package */

/* termcap stuff */
extern char	*tgoto();
extern char	*tgetstr();
extern void	tputs();

#if MSDOS
/* BIOS interface used instead of termcap for MS-DOS */
extern int	vmode;
extern void	v_up();
extern void	v_cb();
extern void	v_cs();
extern void	v_ce();
extern void	v_cl();
extern void	v_cd();
extern void	v_al();
extern void	v_dl();
extern void	v_sr();
extern void	v_move();
#endif

/* faddch() is a function.  a pointer to it is passed to tputs() */
extern int	faddch();

/* data types */

#ifndef DF_POSIX
#define	bool	char
//#define	reg	register

struct _win_st {
	short		_cury, _curx;
	short		_maxy, _maxx;
	short		_begy, _begx;
	short		_flags;
	short		_ch_off;
	bool		_clear;
//DFC	bool		_leave;		// Compiler had a problem with "_leave"
	bool		_nleave;
	bool		_scroll;
	char		**_y;
	short		*_firstch;
	short		*_lastch;
	struct _win_st	*_nextp, *_orig;
};

#define	WINDOW	struct _win_st
#else
#define WINDOW	char
#endif

/* CONSTANTS & SYMBOLS */
#define TRUE		1
#define FALSE		0
#define A_NORMAL	0
#define A_STANDOUT	1
#define A_BOLD		2
#define A_UNDERLINE	3
#define A_ALTCHARSET	4
#if MSDOS
#define KBSIZ		(10*1024)
#else
#define KBSIZ		(6*1024)
#endif

/* extern variables, defined in curses.c */
extern short	ospeed;		/* tty speed, eg B2400 */
#if OSK
extern char PC_;	/* Pad char */
extern char	*BC;	/* Backspace char string */
#else
extern char	PC;		/* Pad char */
#endif
extern WINDOW	*stdscr;	/* pointer into kbuf[] */
#ifndef DF_POSIX
extern char	kbuf[KBSIZ];	/* a very large output buffer */
#else
extern WINDOW	kbuf[KBSIZ];	/* a very large output buffer */
#endif
extern int	LINES;		/* :li#: number of rows */
extern int	COLS;		/* :co#: number of columns */
extern int	AM;		/* :am:  boolean: auto margins? */
extern int	PT;		/* :pt:  boolean: physical tabs? */
extern char	*VB;		/* :vb=: visible bell */
extern char	*UP;		/* :up=: move cursor up */
extern char	*SO;		/* :so=: standout start */
extern char	*SE;		/* :se=: standout end */
extern char	*US;		/* :us=: underline start */
extern char	*UE;		/* :ue=: underline end */
extern char	*MD;		/* :md=: bold start */
extern char	*ME;		/* :me=: bold end */
extern char	*AS;		/* :as=: alternate (italic) start */
extern char	*AE;		/* :ae=: alternate (italic) end */
extern char	*CM;		/* :cm=: cursor movement */
extern char	*CE;		/* :ce=: clear to end of line */
extern char	*CD;		/* :cd=: clear to end of screen */
extern char	*AL;		/* :al=: add a line */
extern char	*DL;		/* :dl=: delete a line */
#if OSK
extern char	*SR_;		/* :sr=: scroll reverse */
#else
extern char	*SR;		/* :sr=: scroll reverse */
#endif
extern char	*KS;		/* :ks=: init string for cursor */
extern char	*KE;		/* :ke=: restore string for cursor */
extern char	*KU;		/* :ku=: sequence sent by up key */
extern char	*KD;		/* :kd=: sequence sent by down key */
extern char	*KL;		/* :kl=: sequence sent by left key */
extern char	*KR;		/* :kr=: sequence sent by right key */
extern char	*PU;		/* :PU=: key sequence sent by PgUp key */
extern char	*PD;		/* :PD=: key sequence sent by PgDn key */
extern char	*HM;		/* :HM=: key sequence sent by Home key */
extern char	*EN;		/* :EN=: key sequence sent by End key */
extern char	*IM;		/* :im=: insert mode start */
extern char	*IC;		/* :ic=: insert following char */
extern char	*EI;		/* :ei=: insert mode end */
extern char	*DC;		/* :dc=: delete a character */
extern char	*TI;		/* :ti=: terminal init */	/* GB */
extern char	*TE;		/* :te=: terminal exit */	/* GB */
#ifndef NO_CURSORSHAPE
extern char	*CQ;		/* :cQ=: normal cursor */
extern char	*CX;		/* :cX=: cursor used for EX command/entry */
extern char	*CV;		/* :cV=: cursor used for VI command mode */
extern char	*CI;		/* :cI=: cursor used for VI input mode */
extern char	*CR;		/* :cR=: cursor used for VI replace mode */
#endif
extern char	*aend;		/* end an attribute -- either UE or ME */
extern char	ERASEKEY;	/* taken from the ioctl structure */

/* Msdos-versions may use bios; others always termcap.
 * Will emit some 'code has no effect' warnings in unix.
 */
 
#if MSDOS
extern char o_pcbios[1];		/* BAH! */
#define	CHECKBIOS(x,y)	(*o_pcbios ? (x) : (y))
#define VOIDBIOS(x,y)	{if (*o_pcbios) {x;} else {y;}}
#else
#define	CHECKBIOS(x,y)	(y)
#define VOIDBIOS(x,y)	{y;}
#endif

#define	do_VB()		VOIDBIOS(;, tputs(VB, 1, faddch))
#define	do_UP()		VOIDBIOS(v_up(), tputs(UP, 1, faddch))
#define	do_SO()		VOIDBIOS((vmode=A_STANDOUT), tputs(SO, 1, faddch))
#define	do_SE()		VOIDBIOS((vmode=A_NORMAL), tputs(SE, 1, faddch))
#define	do_US()		VOIDBIOS((vmode=A_UNDERLINE), tputs(US, 1, faddch))
#define	do_UE()		VOIDBIOS((vmode=A_NORMAL), tputs(UE, 1, faddch))
#define	do_MD()		VOIDBIOS((vmode=A_BOLD), tputs(MD, 1, faddch))
#define	do_ME()		VOIDBIOS((vmode=A_NORMAL), tputs(ME, 1, faddch))
#define	do_AS()		VOIDBIOS((vmode=A_ALTCHARSET), tputs(AS, 1, faddch))
#define	do_AE()		VOIDBIOS((vmode=A_NORMAL), tputs(AE, 1, faddch))
#undef	do_CM		/* move */
#define	do_CE()		VOIDBIOS(v_ce(), tputs(CE, 1, faddch))
#define	do_CD()		VOIDBIOS(v_cd(), tputs(CD, 1, faddch))
#define	do_AL()		VOIDBIOS(v_al(), tputs(AL, LINES, faddch))
#define	do_DL()		VOIDBIOS(v_dl(), tputs(DL, LINES, faddch))
#if OSK
#define	do_SR()		VOIDBIOS(v_sr(), tputs(SR_, 1, faddch))
#else
#define	do_SR()		VOIDBIOS(v_sr(), tputs(SR, 1, faddch))
#endif
#define do_KS()		VOIDBIOS(1, tputs(KS, 1, faddch))
#define do_KE()		VOIDBIOS(1, tputs(KE, 1, faddch))
#define	do_IM()		VOIDBIOS(;, tputs(IM, 1, faddch))
#define	do_IC()		VOIDBIOS(;, tputs(IC, 1, faddch))
#define	do_EI()		VOIDBIOS(;, tputs(EI, 1, faddch))
#define	do_DC()		VOIDBIOS(;, tputs(DC, COLS, faddch))
#define	do_TI()		VOIDBIOS(;, (void)ttywrite(TI, (unsigned)strlen(TI)))
#define	do_TE()		VOIDBIOS(;, (void)ttywrite(TE, (unsigned)strlen(TE)))
#ifndef NO_CURSORSHAPE
# define do_CQ()	VOIDBIOS(v_cs(), tputs(CQ, 1, faddch))
# define do_CX()	VOIDBIOS(v_cs(), tputs(CX, 1, faddch))
# define do_CV()	VOIDBIOS(v_cs(), tputs(CV, 1, faddch))
# define do_CI()	VOIDBIOS(v_cb(), tputs(CI, 1, faddch))
# define do_CR()	VOIDBIOS(v_cb(), tputs(CR, 1, faddch))
#endif
#define	do_aend()	VOIDBIOS((vmode=A_NORMAL), tputs(aend, 1, faddch))

#define	has_AM		CHECKBIOS(1, AM)
#define	has_PT		CHECKBIOS(0, PT)
#define	has_VB		CHECKBIOS((char *)0, VB)
#define	has_UP		CHECKBIOS((char *)1, UP)
#define	has_SO		CHECKBIOS((char)1, (*SO))
#define	has_SE		CHECKBIOS((char)1, (*SE))
#define	has_US		CHECKBIOS((char)1, (*US))
#define	has_UE		CHECKBIOS((char)1, (*UE))
#define	has_MD		CHECKBIOS((char)1, (*MD))
#define	has_ME		CHECKBIOS((char)1, (*ME))
#define	has_AS		CHECKBIOS((char)1, (*AS))
#define	has_AE		CHECKBIOS((char)1, (*AE))
#undef	has_CM		/* cursor move: don't need */
#define	has_CB		CHECKBIOS(1, 0)
#define	has_CS		CHECKBIOS(1, 0)
#define	has_CE		CHECKBIOS((char *)1, CE)
#define	has_CD		CHECKBIOS((char *)1, CD)
#define	has_AL		CHECKBIOS((char *)1, AL)
#define	has_DL		CHECKBIOS((char *)1, DL)
#if OSK
#define	has_SR		CHECKBIOS((char *)1, SR_)
#else
#define	has_SR		CHECKBIOS((char *)1, SR)
#endif
#define has_KS		CHECKBIOS((char)1, (*KS))
#define has_KE		CHECKBIOS((char)1, (*KE))
#define	has_KU		CHECKBIOS("#H", KU)
#define	has_KD		CHECKBIOS("#P", KD)
#define	has_KL		CHECKBIOS("#K", KL)
#define	has_KR		CHECKBIOS("#M", KR)
#define has_HM		CHECKBIOS("#G", HM)
#define has_EN		CHECKBIOS("#O", EN)
#define has_PU		CHECKBIOS("#I", PU)
#define has_PD		CHECKBIOS("#Q", PD)
#define	has_IM		CHECKBIOS((char)0, (*IM))
#define	has_IC		CHECKBIOS((char)0, (*IC))
#define	has_EI		CHECKBIOS((char)0, (*EI))
#define	has_DC		CHECKBIOS((char *)0, DC)
#define	has_TI		CHECKBIOS((char)0, (*TI))
#define	has_TE		CHECKBIOS((char)0, (*TE))
#ifndef NO_CURSORSHAPE
#define has_CQ		CHECKBIOS((char *)1, CQ)
#endif

/* (pseudo)-Curses-functions */

#ifdef lint
# define _addCR		VOIDBIOS(;, (stdscr[-1] == '\n' ? qaddch('\r') : (stdscr[-1] = '\n')))
#else
# if OSK
#  define _addCR		VOIDBIOS(;, (stdscr[-1] == '\n' ? qaddch('\l') : (stdscr[-1] = stdscr[-1])))
# else
# define _addCR		VOIDBIOS(;, (stdscr[-1] == '\n' ? qaddch('\r') : 0))
#endif
#endif
#define qaddch(ch)	CHECKBIOS(v_put(ch), (*stdscr++ = (ch)))
#if OSK
#define addch(ch)	if (qaddch(ch) == '\n') qaddch('\l'); else
#else
#define addch(ch)	if (qaddch(ch) == '\n') qaddch('\r'); else
#endif

extern void initscr();
extern void endwin();
extern void suspend_curses();
extern void resume_curses();
extern void attrset();
extern void insch();
extern void qaddstr();
#define addstr(str)	{qaddstr(str); _addCR;}
#define move(y,x)	VOIDBIOS(v_move(x,y), \
					tputs(tgoto(CM, x, y), 1, faddch))
#define mvaddch(y,x,ch)	{move(y,x); addch(ch);}
#define refresh()	VOIDBIOS(;, wrefresh(stdscr))
#define wrefresh(w)	if ((w) != kbuf) VOIDBIOS((w) = kbuf, {ttywrite(kbuf, (unsigned)((w) - kbuf)); (w) = kbuf;}) else
#define wqrefresh(w)	if ((w) - kbuf > 2000) VOIDBIOS((w) = kbuf, {ttywrite(kbuf, (unsigned)((w) - kbuf)); (w) = kbuf;}) else
#define standout()	do_SO()
#define standend()	do_SE()
#define clrtoeol()	do_CE()
#define clrtobot()	do_CD()
#define insertln()	do_AL()
#define deleteln()	do_DL()
#define delch()		do_DC()
#ifdef DF_POSIX
//#define	scrollok(win,bf) win->_scroll = bf
#define scrollok(w,b)
#define cbreak(TTY) (TTY.c_lflag &= ~ICANON, \
	tcsetattr(2, TCSANOW, &TTY))
#define nocbreak(TTY) (TTY.c_lflag |= ICANON, \
	tcsetattr(2, TCSANOW, &TTY))
#define echo(TTY)	 (TTY.c_lflag |= ECHO, \
	tcsetattr(2, TCSANOW, &TTY))
#define noecho(TTY) (TTY.c_lflag &= ~ECHO, \
	tcsetattr(2, TCSANOW, &TTY))
#else
#define scrollok(w,b)
#define raw()
#define echo()
#define cbreak()
#define noraw()
#define noecho()
#define nocbreak()
#endif
