/*	AT386:	 hacked tcap.c for the 386 console, when you don't
 *		have libtermcap.   grrr.
 *
 * $Header: /usr2/foxharp/src/pgf/vile/RCS/at386.c,v 1.15 1994/11/29 04:02:03 pgf Exp $
 *
 */
#error This module is not actively maintained as part of vile.
#error It can likely be made to work without much difficulty, but unless
#error  I know someone is using it, i have little incentive to fix it.
#error  If you use it when you build vile, please let me know.  -pgf

#define termdef 1			/* don't define "term" external */

#include	"estruct.h"
#include	"edef.h"

#if DISP_AT386

#define NROW	25			/* Screen size. 		*/
#define NCOL	80			/* Edit if you want to. 	*/
#define MARGIN	8
#define SCRSIZ	64
#define NPAUSE	10			/* # times thru update to pause */

extern int	ttopen();
extern int	ttgetc();
extern int	ttputc();
extern int	tgetnum();
extern int	ttflush();
extern int	ttclose();
extern int	at386kopen();
extern int	at386kclose();
extern int	at386move();
extern int	at386eeol();
extern int	at386eeop();
extern int	at386beep();
extern int	at386rev();
extern int	at386cres();
extern int	at386open();
extern int	tput();
extern char	*tgoto();
#if	OPT_COLOR
extern	int	at386fcol();
extern	int	at386bcol();
#endif
extern	int	at386scroll_delins();

#define TCAPSLEN 315
char at386buf[TCAPSLEN];
char *UP, PC, *CM, *CE, *CL, *SO, *SE;

char *DL, *AL, *SF, *SR;

TERM term = {
	NROW,
	NROW,
	NCOL,
	NCOL,
	MARGIN,
	SCRSIZ,
	NPAUSE,
	at386open,
	ttclose,
	at386kopen,
	at386kclose,
	ttgetc,
	ttputc,
	tttypahead,
	ttflush,
	at386move,
	at386eeol,
	at386eeop,
	at386beep,
	at386rev,
	at386cres
#if	OPT_COLOR
	, at386fcol,
	at386bcol
#endif
	, NULL		/* set dynamically at open time */
};

at386open()
{
	char *t, *p, *tgetstr();
	char tcbuf[1024];
	char *tv_stype;
	char err_str[72];

	CL = "\033[2J\033[H";
	CE = "\033[K";
	UP = "\033[A";
	SE = "\033[m";
	SO = "\033[7m";
	revexist = TRUE;

	DL = "\033[1M";
	AL = "\033[1L";
	term.t_scroll = at386scroll_delins;
	ttopen();
}

at386kopen()
{
	strcpy(sres, "NORMAL");
}

at386kclose()
{
}

csi()
{
	ttputc(ESC);
	ttputc('[');
}

ansiparm(n)
register int	n;
{
	register int q,r;

	q = n/10;
	if (q != 0) {
		r = q/10;
		if (r != 0) {
			ttputc((r%10)+'0');
		}
		ttputc((q%10) + '0');
	}
	ttputc((n%10) + '0');
}

at386move(row, col)
register int row, col;
{
	csi();
	if (row) ansiparm(row+1);
	ttputc(';');
	if (col) ansiparm(col+1);
	ttputc('H');
}

at386eeol()
{
	fputs(CE,stdout);
}

at386eeop()
{
	fputs(CL,stdout);
}

at386rev(state) 	/* change reverse video status */
int state;		/* FALSE = normal video, TRUE = reverse video */
{
	static int revstate = -1;
	if (state == revstate)
		return;
	revstate = state;
	if (state) {
		if (SO != NULL)
			fputs(SO,stdout);
	} else {
		if (SE != NULL)
			fputs(SE,stdout);
	}
}

at386cres()	/* change screen resolution */
{
	return(TRUE);
}


/* 
OPT_PRETTIER_SCROLL is prettier but slower -- it scrolls 
		a line at a time instead of all at once.
*/

/* move howmany lines starting at from to to */
at386scroll_delins(from,to,n)
{
	int i;
	if (to == from) return;
#if OPT_PRETTIER_SCROLL
	if (absol(from-to) > 1) {
		at386scroll_delins(from, (from<to) ? to-1:to+1, n);
		if (from < to)
			from = to-1;
		else
			from = to+1;    
	}
#endif
	if (to < from) {
		at386move(to,0);
		for (i = from - to; i > 0; i--)
			fputs(DL,stdout);
		at386move(to+n,0);
		for (i = from - to; i > 0; i--)
			fputs(AL,stdout);
	} else {
		at386move(from+n,0);
		for (i = to - from; i > 0; i--)
			fputs(DL,stdout);
		at386move(from,0);
		for (i = to - from; i > 0; i--)
			fputs(AL,stdout);
	}
}


spal(dummy)	/* change palette string */
{
	/*	Does nothing here	*/
}


#if	OPT_COLOR
at386fcol()	/* no colors here, ignore this */
{
}

at386bcol()	/* no colors here, ignore this */
{
}
#endif

at386beep()
{
	ttputc(BEL);
}

#else

hello()
{
}

#endif
