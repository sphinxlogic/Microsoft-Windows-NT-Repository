/* $Id: term.c,v 1.2 92/01/11 16:31:10 usenet Exp $
 *
 * $Log:	term.c,v $
 * Revision 1.2  92/01/11  16:31:10  usenet
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
 * Revision 4.4.2.1  1991/12/01  18:05:42  sob
 * Patchlevel 2 changes
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

#include "EXTERN.h"
#include "common.h"
#include "util.h"
#include "final.h"
#include "help.h"
#include "cheat.h"
#include "intrp.h"
#include "INTERN.h"
#include "term.h"

char ERASECH;		/* rubout character */
char KILLCH;		/* line delete character */
char tcarea[TCSIZE];	/* area for "compiled" termcap strings */

#ifdef USETHREADS
int upcost;
#endif

/* guarantee capability pointer != Nullch */
/* (I believe terminfo will ignore the &tmpaddr argument.) */

#define Tgetstr(key) ((tmpstr = tgetstr(key,&tmpaddr)) ? tmpstr : nullstr)

#ifdef PUSHBACK
struct keymap {
    char km_type[128];
    union km_union {
	struct keymap *km_km;
	char *km_str;
    } km_ptr[128];
};

#define KM_NOTHIN 0
#define KM_STRING 1
#define KM_KEYMAP 2
#define KM_BOGUS 3

#define KM_TMASK 3
#define KM_GSHIFT 4
#define KM_GMASK 7

typedef struct keymap KEYMAP;

KEYMAP *topmap INIT(Null(KEYMAP*));

void mac_init();
KEYMAP *newkeymap();
void show_keymap();
void pushstring();
#endif

void line_col_calcs();

/* terminal initialization */

void
term_init()
{
    savetty();				/* remember current tty state */

#ifdef TERMIO
    outspeed = _tty.c_cflag & CBAUD;	/* for tputs() */
    ERASECH = _tty.c_cc[VERASE];	/* for finish_command() */
    KILLCH = _tty.c_cc[VKILL];		/* for finish_command() */
    if (GT = ((_tty.c_oflag & TABDLY) != TAB3))
	/* we have tabs, so that's OK */;
    else
	_tty.c_oflag &= ~TAB3;	/* turn off kernel tabbing -- done in rn */
#else /* !TERMIO */
# ifdef TERMIOS
    outspeed = cfgetospeed(&_tty);	/* for tputs() (output) */
    ERASECH = _tty.c_cc[VERASE];	/* for finish_command() */
    KILLCH = _tty.c_cc[VKILL];		/* for finish_command() */
/*    _tty.c_oflag &= ~OXTABS;		/* turn off kernel tabbing-done in rn */
# else /* !TERMIOS */
    outspeed = _tty.sg_ospeed;		/* for tputs() */
    ERASECH = _tty.sg_erase;		/* for finish_command() */
    KILLCH = _tty.sg_kill;		/* for finish_command() */
    if (GT = ((_tty.sg_flags & XTABS) != XTABS))
	/* we have tabs, so that's OK */;
    else
	_tty.sg_flags &= ~XTABS;
# endif /* TERMIOS */
#endif /* TERMIO */

    /* The following could be a table but I can't be sure that there isn't */
    /* some degree of sparsity out there in the world. */

    switch (outspeed) {			/* 1 second of padding */
#ifdef BEXTA
        case BEXTA:  just_a_sec = 1920; break;
#else
#ifdef B19200
        case B19200: just_a_sec = 1920; break;
#endif
#endif
        case B9600:  just_a_sec =  960; break;
        case B4800:  just_a_sec =  480; break;
        case B2400:  just_a_sec =  240; break;
        case B1800:  just_a_sec =  180; break;
        case B1200:  just_a_sec =  120; break;
        case B600:   just_a_sec =   60; break;
	case B300:   just_a_sec =   30; break;
	/* do I really have to type the rest of this??? */
        case B200:   just_a_sec =   20; break;
        case B150:   just_a_sec =   15; break;
        case B134:   just_a_sec =   13; break;
        case B110:   just_a_sec =   11; break;
        case B75:    just_a_sec =    8; break;
        case B50:    just_a_sec =    5; break;
        default:     just_a_sec =  960; break;
					/* if we are running detached I */
    }					/*  don't want to know about it! */
}

/* set terminal characteristics */

void
term_set(tcbuf)
char *tcbuf;		/* temp area for "uncompiled" termcap entry */
{
    char *tmpaddr;			/* must not be register */
    register char *tmpstr;
    char *tgetstr();
    char *s;
    int status;
#ifdef TIOCGWINSZ
    struct winsize winsize;
#endif

#ifdef PENDING
#if ! defined (FIONREAD) && ! defined (RDCHK)
    /* do no delay reads on something that always gets closed on exit */

    devtty = open("/dev/tty",0);
    if (devtty < 0) {
	printf(cantopen,"/dev/tty") FLUSH;
	finalize(1);
    }
    fcntl(devtty,F_SETFL,O_NDELAY);
#endif
#endif
    
    /* get all that good termcap stuff */

#ifdef HAVETERMLIB
    status = tgetent(tcbuf,getenv("TERM"));	/* get termcap entry */
    if (status < 1) {
#ifdef VERBOSE
	printf("No termcap %s found.\n", status ? "file" : "entry") FLUSH;
#else
	fputs("Termcap botch\n",stdout) FLUSH;
#endif
	finalize(1);
    }
    tmpaddr = tcarea;			/* set up strange tgetstr pointer */
    s = Tgetstr("pc");			/* get pad character */
    PC = *s;				/* get it where tputs wants it */
    if (!tgetflag("bs")) {		/* is backspace not used? */
	BC = Tgetstr("bc");		/* find out what is */
	if (BC == nullstr) 		/* terminfo grok's 'bs' but not 'bc' */
	    BC = Tgetstr("le");
    } else
	BC = "\b";			/* make a backspace handy */
    UP = Tgetstr("up");			/* move up a line */
    if (!*UP)				/* no UP string? */
	marking = 0;			/* disable any marking */
    if (muck_up_clear)			/* this is for weird HPs */
	CL = "\n\n\n\n";
    else
	CL = Tgetstr("cl");		/* get clear string */
    CE = Tgetstr("ce");			/* clear to end of line string */
    TI = Tgetstr("ti");			/* initialize display */
    TE = Tgetstr("te");			/* reset display */
#if defined(CLEAREOL) || defined(USETHREADS)
    HO = Tgetstr("ho");			/* home cursor if no CM */
    CM = Tgetstr("cm");			/* cursor motion */
    if (*CM || *HO)
	can_home = TRUE;
#endif
#ifdef CLEAREOL
    CD = Tgetstr("cd");			/* clear to end of display */
    if (!*CE || !*CD || !can_home)	/* can we CE, CD, and home? */
	can_home_clear = FALSE;		/*  no, so disable use of clear eol */
    if (!*CE) CE = CD;
#endif /* CLEAREOL */
#ifdef USETHREADS
    upcost = strlen(UP);
#endif
    SO = Tgetstr("so");			/* begin standout */
    SE = Tgetstr("se");			/* end standout */
    if ((SG = tgetnum("sg"))<0)
	SG = 0;				/* blanks left by SG, SE */
    US = Tgetstr("us");			/* start underline */
    UE = Tgetstr("ue");			/* end underline */
    if ((UG = tgetnum("ug"))<0)
	UG = 0;				/* blanks left by US, UE */
    if (*US)
	UC = nullstr;			/* UC must not be NULL */
    else
	UC = Tgetstr("uc");		/* underline a character */
    if (!*US && !*UC) {			/* no underline mode? */
	US = SO;			/* substitute standout mode */
	UE = SE;
	UG = SG;
    }
    LINES = tgetnum("li");		/* lines per page */
    COLS = tgetnum("co");		/* columns on page */

#ifdef TIOCGWINSZ
    { struct winsize ws;
	if (ioctl(0, TIOCGWINSZ, &ws) >= 0 && ws.ws_row > 0 && ws.ws_col > 0) {
	    LINES = ws.ws_row;
	    COLS = ws.ws_col;
	}
    }
#endif
	
    AM = tgetflag("am");		/* terminal wraps automatically? */
    XN = tgetflag("xn");		/* then eats next newline? */
    VB = Tgetstr("vb");
    if (!*VB)
	VB = "\007";
    CR = Tgetstr("cr");
    if (!*CR) {
	if (tgetflag("nc") && *UP) {
	    CR = safemalloc((MEM_SIZE)strlen(UP)+2);
	    sprintf(CR,"%s\r",UP);
	}
	else
	    CR = "\r";
    }
#ifdef TIOCGWINSZ
	if (ioctl(1, TIOCGWINSZ, &winsize)>=0) {
		if (winsize.ws_row>0) LINES=winsize.ws_row;
		if (winsize.ws_col>0) COLS=winsize.ws_col;
	}
#endif
#else
    ??????				/* Roll your own... */
#endif
    termlib_init();
    line_col_calcs();
    noecho();				/* turn off echo */
    crmode();				/* enter cbreak mode */

#ifdef PUSHBACK
    mac_init(tcbuf);
#endif
}

#ifdef PUSHBACK
void
mac_init(tcbuf)
char *tcbuf;
{
    char tmpbuf[1024];

    tmpfp = fopen(filexp(getval("RNMACRO",RNMACRO)),"r");
    if (tmpfp != Nullfp) {
	while (fgets(tcbuf,1024,tmpfp) != Nullch) {
	    mac_line(tcbuf,tmpbuf,(sizeof tmpbuf));
	}
	fclose(tmpfp);
    }
}

void
mac_line(line,tmpbuf,tbsize)
char *line;
char *tmpbuf;
int tbsize;
{
    register char *s, *m;
    register KEYMAP *curmap;
    register int ch;
    register int garbage = 0;
    static char override[] = "\nkeymap overrides string\n";

    if (topmap == Null(KEYMAP*))
	topmap = newkeymap();
    if (*line == '#' || *line == '\n')
	return;
    if (line[ch = strlen(line)-1] == '\n')
	line[ch] = '\0';
    m = dointerp(tmpbuf,tbsize,line," \t");
    if (!*m)
	return;
    while (*m == ' ' || *m == '\t') m++;
    for (s=tmpbuf,curmap=topmap; *s; s++) {
	ch = *s & 0177;
	if (s[1] == '+' && isdigit(s[2])) {
	    s += 2;
	    garbage = (*s & KM_GMASK) << KM_GSHIFT;
	}
	else
	    garbage = 0;
	if (s[1]) {
	    if ((curmap->km_type[ch] & KM_TMASK) == KM_STRING) {
		fputs(override,stdout) FLUSH;
		free(curmap->km_ptr[ch].km_str);
		curmap->km_ptr[ch].km_str = Nullch;
	    }
	    curmap->km_type[ch] = KM_KEYMAP + garbage;
	    if (curmap->km_ptr[ch].km_km == Null(KEYMAP*))
		curmap->km_ptr[ch].km_km = newkeymap();
	    curmap = curmap->km_ptr[ch].km_km;
	}
	else {
	    if ((curmap->km_type[ch] & KM_TMASK) == KM_KEYMAP)
		fputs(override,stdout) FLUSH;
	    else {
		curmap->km_type[ch] = KM_STRING + garbage;
		curmap->km_ptr[ch].km_str = savestr(m);
	    }
	}
    }
}

KEYMAP*
newkeymap()
{
    register int i;
    register KEYMAP *map;

#ifndef lint
    map = (KEYMAP*)safemalloc(sizeof(KEYMAP));
#else
    map = Null(KEYMAP*);
#endif /* lint */
    for (i=127; i>=0; --i) {
	map->km_ptr[i].km_km = Null(KEYMAP*);
	map->km_type[i] = KM_NOTHIN;
    }
    return map;
}

void
show_macros()
{
    char prebuf[64];

    if (topmap != Null(KEYMAP*)) {
	print_lines("Macros:\n",STANDOUT);
	*prebuf = '\0';
	show_keymap(topmap,prebuf);
    }
    else {
	print_lines("No macros defined.\n", NOMARKING);
    }
}

void
show_keymap(curmap,prefix)
register KEYMAP *curmap;
char *prefix;
{
    register int i;
    register char *next = prefix + strlen(prefix);
    register int kt;

    for (i=0; i<128; i++) {
	if (kt = curmap->km_type[i]) {
	    if (i < ' ')
		sprintf(next,"^%c",i+64);
	    else if (i == ' ')
		strcpy(next,"\\040");
	    else if (i == 127)
		strcpy(next,"^?");
	    else
		sprintf(next,"%c",i);
	    if ((kt >> KM_GSHIFT) & KM_GMASK) {
		sprintf(cmd_buf,"+%d", (kt >> KM_GSHIFT) & KM_GMASK);
		strcat(next,cmd_buf);
	    }
	    switch (kt & KM_TMASK) {
	    case KM_NOTHIN:
		sprintf(cmd_buf,"%s	%c\n",prefix,i);
		print_lines(cmd_buf,NOMARKING);
		break;
	    case KM_KEYMAP:
		show_keymap(curmap->km_ptr[(char)i].km_km, prefix);
		break;
	    case KM_STRING:
		sprintf(cmd_buf,"%s	%s\n",prefix,curmap->km_ptr[i].km_str);
		print_lines(cmd_buf,NOMARKING);
		break;
	    case KM_BOGUS:
		sprintf(cmd_buf,"%s	BOGUS\n",prefix);
		print_lines(cmd_buf,STANDOUT);
		break;
	    }
	}
    }
}

#endif

/* routine to pass to tputs */

char
putchr(ch)
register char_int ch;
{
    putchar(ch);
#ifdef lint
    ch = Null(char);
    ch = ch;
#endif
    return((char) 0);
}

/* input the 2nd and succeeding characters of a multi-character command */
/* returns TRUE if command finished, FALSE if they rubbed out first character */

bool
finish_command(donewline)
int donewline;
{
    register char *s;
    register bool quoteone = FALSE;

    s = buf;
    if (s[1] != FINISHCMD)		/* someone faking up a command? */
	return TRUE;
    do {
      top:
	if (*(unsigned char *)s < ' ') {
	    putchar('^');
	    putchar(*s | 64);
	}
	else if (*s == '\177') {
	    putchar('^');
	    putchar('?');
	}
	else
	    putchar(*s);		/* echo previous character */
	s++;
re_read:
	fflush(stdout);
	getcmd(s);
	if (quoteone) {
	    quoteone = FALSE;
	    continue;
	}
	if (errno || *s == '\f') {
	    *s = Ctl('r');		/* force rewrite on CONT */
	}
	if (*s == '\033') {		/* substitution desired? */
#ifdef ESCSUBS
	    char tmpbuf[4], *cpybuf;

	    tmpbuf[0] = '%';
	    read_tty(&tmpbuf[1],1);
#ifdef RAWONLY
	    tmpbuf[1] &= 0177;
#endif
	    tmpbuf[2] = '\0';
	    if (tmpbuf[1] == 'h') {
		(void) help_subs();
		*s = '\0';
		reprint();
		goto re_read;
	    }
	    else if (tmpbuf[1] == '\033') {
		*s = '\0';
		cpybuf = savestr(buf);
		interp(buf, (sizeof buf), cpybuf);
		free(cpybuf);
		s = buf + strlen(buf);
		reprint();
		goto re_read;
	    }
	    else {
		interp(s,(sizeof buf) - (s-buf),tmpbuf);
		fputs(s,stdout);
		s += strlen(s);
	    }
	    goto re_read;
#else
	    notincl("^[");
	    *s = '\0';
	    reprint();
	    goto re_read;
#endif
	}
	else if (*s == ERASECH) {	/* they want to rubout a char? */
	    rubout();
	    s--;			/* discount the char rubbed out */
	    if (*(unsigned char *)s < ' ' || *s == '\177')
		rubout();
	    if (s == buf) {		/* entire string gone? */
		fflush(stdout);		/* return to single char command mode */
		return FALSE;
	    }
	    else
		goto re_read;
	}
	else if (*s == KILLCH) {	/* wipe out the whole line? */
	    while (s-- != buf) {	/* emulate that many ERASEs */
		rubout();
		if (*(unsigned char *)s < ' ' || *s == '\177')
		    rubout();
	    }
	    fflush(stdout);
	    return FALSE;		/* return to single char mode */
	}
#ifdef WORDERASE
	else if (*s == Ctl('w')) {	/* wipe out one word? */
	    *s-- = ' ';
	    while (!isspace(*s) || isspace(s[1])) {
		rubout();
		if (s-- == buf) {
		    fflush(stdout);
		    return FALSE;	/* return to single char mode */
		}
		if (*(unsigned char *)s < ' ' || *s == '\177')
		    rubout();
	    }
	    s++;
	    goto re_read;
	}
#endif
	else if (*s == Ctl('r')) {
	    *s = '\0';
	    reprint();
	    goto re_read;
	}
	else if (*s == Ctl('v')) {
	    putchar('^');
	    backspace();
	    fflush(stdout);
	    getcmd(s);
	    goto top;
	}
	else if (*s == '\\') {
	    quoteone = TRUE;
	}
#ifdef cray
    } while (*s != '\r');		/* till a newline (not echoed) */
#else
    } while (*s != '\n');		/* till a newline (not echoed) */
#endif
    *s = '\0';				/* terminate the string nicely */
    if (donewline)
	putchar('\n') FLUSH;
    return TRUE;			/* say we succeeded */
}

/* discard any characters typed ahead */

void
eat_typeahead()
{
#ifdef PUSHBACK
    if (!typeahead && nextin==nextout)	/* cancel only keyboard stuff */
#else
    if (!typeahead)
#endif
    {
#ifdef PENDING
	while (input_pending())
	    read_tty(buf,sizeof(buf));
#else /* this is probably v7 */
#if defined(TERMIO) || defined(TERMIOS)
	ioctl(_tty_ch,TCSETAW,&_tty);
#else
	ioctl(_tty_ch,TIOCSETP,&_tty);
#endif
#endif
    }
}

void
settle_down()
{
    dingaling();
    fflush(stdout);
/*    sleep(1); */
#ifdef PUSHBACK
    nextout = nextin;			/* empty circlebuf */
#endif
    eat_typeahead();
}

#ifdef PUSHBACK
/* read a character from the terminal, with multi-character pushback */

int
read_tty(addr,size)
char *addr;
int size;
{
    if (nextout != nextin) {
	*addr = circlebuf[nextout++];
	nextout %= PUSHSIZE;
	return 1;
    }
    else {
	size = read(0,addr,size);
#ifdef RAWONLY
	*addr &= 0177;
#endif
	return size;
    }
}

#ifdef PENDING
#if ! defined (FIONREAD) && ! defined (RDCHK)
int
circfill()
{
    register int Howmany;

    errno = 0;
    Howmany = read(devtty,circlebuf+nextin,1);

    if (Howmany < 0 && (errno == EAGAIN || errno == EINTR))
	Howmany = 0;
    if (Howmany) {
	nextin += Howmany;
	nextin %= PUSHSIZE;
    }
    return Howmany;
}
#endif /* PENDING */
#endif /* FIONREAD */

void
pushchar(c)
char_int c;
{
    nextout--;
    if (nextout < 0)
	nextout = PUSHSIZE - 1;
    if (nextout == nextin) {
	fputs("\npushback buffer overflow\n",stdout) FLUSH;
	sig_catcher(0);
    }
    circlebuf[nextout] = c;
}

#else /* PUSHBACK */
#ifndef read_tty
/* read a character from the terminal, with hacks for O_NDELAY reads */

int
read_tty(addr,size)
char *addr;
int size;
{
    if (is_input) {
	*addr = pending_ch;
	is_input = FALSE;
	return 1;
    }
    else {
	size = read(0,addr,size);
#ifdef RAWONLY
	*addr &= 0177;
#endif
	return size;
    }
}
#endif /* read_tty */
#endif /* PUSHBACK */

/* print an underlined string, one way or another */

void
underprint(s)
register char *s;
{
    assert(UC);
    if (*UC) {		/* char by char underline? */
	while (*s) {
	    if (*(unsigned char *)s < ' ') {
		putchar('^');
		backspace();/* back up over it */
		underchar();/* and do the underline */
		putchar(*s+64);
		backspace();/* back up over it */
		underchar();/* and do the underline */
	    }
	    else {
		putchar(*s);
		backspace();/* back up over it */
		underchar();/* and do the underline */
	    }
	    s++;
	}
    }
    else {		/* start and stop underline */
	underline();	/* start underlining */
	while (*s) {
	    if (*(unsigned char *)s < ' ') {
		putchar('^');
		putchar(*s+64);
	    }
	    else
		putchar(*s);
	    s++;
	}
	un_underline();	/* stop underlining */
    }
}

/* keep screen from flashing strangely on magic cookie terminals */

#ifdef NOFIREWORKS
void
no_sofire()
{
    if (*UP && *SE) {		/* should we disable fireworks? */
	putchar('\n');
	un_standout();
	up_line();
	carriage_return();
    }
}

void
no_ulfire()
{
    if (*UP && *US) {		/* should we disable fireworks? */
	putchar('\n');
	un_underline();
	up_line();
	carriage_return();
    }
}
#endif

/* get a character into a buffer */

void
getcmd(whatbuf)
register char *whatbuf;
{
#ifdef PUSHBACK
    register KEYMAP *curmap;
    register int i;
    bool no_macros; 
    int times = 0;			/* loop detector */
    char scrchar;

tryagain:
    curmap = topmap;
    no_macros = (whatbuf != buf && nextin == nextout); 
#endif
    for (;;) {
	int_count = 0;
	errno = 0;
	if (read_tty(whatbuf,1) < 0){
	    if (!errno)
	        errno = EINTR;
	    if (errno == EINTR)
		return;
	    perror(readerr);
	    sig_catcher(0);
	}
#ifdef PUSHBACK
	if (*whatbuf & 0200 || no_macros) {
	    *whatbuf &= 0177;
	    goto got_canonical;
	}
	if (curmap == Null(KEYMAP*))
	    goto got_canonical;
	for (i = (curmap->km_type[*whatbuf] >> KM_GSHIFT) & KM_GMASK; i; --i){
	    read_tty(&scrchar,1);
	}
	switch (curmap->km_type[*whatbuf] & KM_TMASK) {
	case KM_NOTHIN:			/* no entry? */
	    if (curmap == topmap)	/* unmapped canonical */
		goto got_canonical;
	    settle_down();
	    goto tryagain;
	case KM_KEYMAP:			/* another keymap? */
	    curmap = curmap->km_ptr[*whatbuf].km_km;
	    assert(curmap != Null(KEYMAP*));
	    break;
	case KM_STRING:			/* a string? */
	    pushstring(curmap->km_ptr[*whatbuf].km_str);
	    if (++times > 20) {		/* loop? */
		fputs("\nmacro loop?\n",stdout);
		settle_down();
	    }
	    no_macros = FALSE;
	    goto tryagain;
	}
#else
#ifdef RAWONLY
	*whatbuf &= 0177;
#endif
	break;
#endif
    }

got_canonical:
#if !defined(TERMIO) && !defined(TERMIOS)
    if (*whatbuf == '\r')
	*whatbuf = '\n';
#endif
    if (whatbuf == buf)
	whatbuf[1] = FINISHCMD;		/* tell finish_command to work */
}

#ifdef PUSHBACK
void
pushstring(str)
char *str;
{
    register int i;
    char tmpbuf[PUSHSIZE];
    register char *s = tmpbuf;

    assert(str != Nullch);
    interp(s,PUSHSIZE,str);
    for (i = strlen(s)-1; i >= 0; --i) {
	s[i] ^= 0200; 
	pushchar(s[i]);
    }
}
#endif

int
get_anything()
{
    char tmpbuf[2];

reask_anything:
    unflush_output();			/* disable any ^O in effect */
    standout();
#ifdef VERBOSE
    IF(verbose)
	fputs("[Type space to continue] ",stdout);
    ELSE
#endif
#ifdef TERSE
	fputs("[MORE] ",stdout);
#endif
    un_standout();
    fflush(stdout);
    eat_typeahead();
    if (int_count) {
	return -1;
    }
    collect_subjects();			/* loads subject cache until */
					/* input is pending */
    getcmd(tmpbuf);
    if (errno || *tmpbuf == '\f') {
	putchar('\n') FLUSH;		/* if return from stop signal */
	goto reask_anything;		/* give them a prompt again */
    }
    if (*tmpbuf == 'h') {
#ifdef VERBOSE
	IF(verbose)
	    fputs("\nType q to quit or space to continue.\n",stdout) FLUSH;
	ELSE
#endif
#ifdef TERSE
	    fputs("\nq to quit, space to continue.\n",stdout) FLUSH;
#endif
	goto reask_anything;
    }
    else if (*tmpbuf != ' ' && *tmpbuf != '\n') {
	carriage_return();
	erase_eol();	/* erase the prompt */
	carriage_return();
	return *tmpbuf == 'q' ? -1 : *tmpbuf;
    }
    if (*tmpbuf == '\n') {
	page_line = LINES - 1;
	carriage_return();
	erase_eol();
	carriage_return();
    }
    else {
	page_line = 1;
	if (erase_screen)		/* -e? */
	    clear();			/* clear screen */
	else {
	    carriage_return();
	    erase_eol();		/* erase the prompt */
	    carriage_return();
	}
    }
    return 0;
}

#ifdef USETHREADS
int
pause_getcmd()
{
    unflush_output();			/* disable any ^O in effect */
    standout();
#ifdef VERBOSE
    IF(verbose)
	fputs("[Type space or a command] ",stdout);
    ELSE
#endif
#ifdef TERSE
	fputs("[CMD] ",stdout);
#endif
    un_standout();
    fflush(stdout);
    eat_typeahead();
    if (int_count) {
	return -1;
    }
    getcmd(buf);
    if (errno || *buf == '\f') {
	return 0;			/* if return from stop signal */
    }
    else if (*buf != ' ') {
	carriage_return();
	erase_eol();	/* erase the prompt */
	carriage_return();
	return *buf;
    }
    return 0;
}
#endif

void
in_char(prompt, newmode)
char *prompt;
char_int newmode;
{
    char oldmode = mode;

reask_in_char:
    unflush_output();			/* disable any ^O in effect */
    fputs(prompt,stdout);
    fflush(stdout);
    eat_typeahead();
    mode = newmode;
    getcmd(buf);
    if (errno || *buf == '\f') {
	putchar('\n') FLUSH;		/* if return from stop signal */
	goto reask_in_char;		/* give them a prompt again */
    }
    mode = oldmode;
}

int
print_lines(what_to_print,hilite)
char *what_to_print;
int hilite;
{
    register char *s;
    register int i;

    if (page_line < 0)			/* they do not want to see this? */
	return -1;
    for (s=what_to_print; *s; ) {
	if (page_line >= LINES || int_count) {
	    if (i = -1, int_count || (i = get_anything())) {
		page_line = -1;		/* disable further print_lines */
		return i;
	    }
	}
	page_line++;
	if (hilite == STANDOUT) {
#ifdef NOFIREWORKS
	    if (erase_screen)
		no_sofire();
#endif
	    standout();
	}
	else if (hilite == UNDERLINE) {
#ifdef NOFIREWORKS
	    if (erase_screen)
		no_ulfire();
#endif
	    underline();
	}
	for (i=0; i<COLS; i++) {
	    if (!*s)
		break;
	    if (*(unsigned char *)s >= ' ')
		putchar(*s);
	    else if (*s == '\t') {
		putchar(*s);
		i = ((i+8) & ~7) - 1; 
	    }
	    else if (*s == '\n') {
		i = 32000;
	    }
	    else {
		i++;
		putchar('^');
		putchar(*s + 64);
	    }
	    s++;
	}
	if (i) {
	    if (hilite == STANDOUT)
		un_standout();
	    else if (hilite == UNDERLINE)
		un_underline();
	    if (AM && i == COLS)
		fflush(stdout);
	    else
		putchar('\n') FLUSH;
	}
    }
    return 0;
}

void
page_init()
{
    page_line = 1;
    if (erase_screen)
	clear();
    else
	putchar('\n') FLUSH;
}

void
pad(num)
int num;
{
    register int i;

    for (i = num; i; --i)
	putchar(PC);
    fflush(stdout);
}

/* echo the command just typed */

#ifdef VERIFY
void
printcmd()
{
    if (verify && buf[1] == FINISHCMD) {
	if (*(unsigned char *)buf < ' ') {
	    putchar('^');
	    putchar(*buf | 64);
	    backspace();
	    backspace();
	}
	else {
	    putchar(*buf);
	    backspace();
	}
	fflush(stdout);
    }
}
#endif

void
rubout()
{
    backspace();			/* do the old backspace, */
    putchar(' ');			/*   space, */
    backspace();			/*     backspace trick */
}

void
reprint()
{
    register char *s;

    fputs("^R\n",stdout) FLUSH;
    for (s = buf; *s; s++) {
	if (*(unsigned char *)s < ' ') {
	    putchar('^');
	    putchar(*s | 64);
	}
	else
	    putchar(*s);
    }
}

#if defined(CLEAREOL) || defined(USETHREADS)
void
home_cursor()
{
    char *tgoto();

    if (!*HO) {			/* no home sequence? */
	if (!*CM) {		/* no cursor motion either? */
	    fputs ("\n\n\n", stdout);
	    return;		/* forget it. */
	}
	tputs (tgoto (CM, 0, 0), 1, putchr);	/* go to home via CM */
	return;
    }
    else {			/* we have home sequence */
	tputs (HO, 1, putchr);	/* home via HO */
    }
}
#endif

#ifdef USETHREADS
void
goto_line(from,to)	/* assumes caller is already at beginning of line */
int from,to;
{
    char *tgoto(), *str;
    int cmcost;

    if (from == to) {
	return;
    }
    if (*CM && !muck_up_clear) {
	cmcost = strlen(str = tgoto(CM,0,to));
    } else {
	cmcost = 9999;
    }
    if (to > from) {
      go_down:
	if (to - from <= cmcost) {
	    while(from++ < to) {
		putchar('\n');
	    }
	    return;
	}
    } else if(*UP) {
	if ((from - to) * upcost <= cmcost) {
	    while(from-- > to) {
		tputs(UP,1,putchr);
	    }
	    return;
	}
    } else if (cmcost == 9999) {
	home_cursor();
	from = 0;
	goto go_down;
    }
    tputs(str,1,putchr);
}
#endif

void
line_col_calcs()
{
     if (LINES > 0) {			/* is this a crt? */
	  if ((!initlines) || (!initlines_specified))
	       /* no -i or unreasonable value for initlines */
	       if (outspeed >= B9600) 	/* whole page at >= 9600 baud */
		    initlines = LINES;
	       else if (outspeed >= B4800)/* 16 lines at 4800 */
		    initlines = 16;
	       else			/* otherwise just header */
		    initlines = 8;
     }
     else {				/* not a crt */
	  LINES = 30000;		/* so don't page */
	  CL = "\n\n";			/* put a couple of lines between */
	  if ((!initlines) || (!initlines_specified))
	       /* make initlines reasonable */
	       initlines = 8;
     }
     if (COLS <= 0)
	  COLS = 80;
}


#ifdef SIGWINCH
SIGRET
winch_catcher(dummy)
int dummy;
{
     /* Reset signal in case of System V dain bramage */
     sigset(SIGWINCH, winch_catcher);

     /* Come here if window size change signal received */
#ifdef TIOCGWINSZ
     { struct winsize ws;
       if (ioctl(0, TIOCGWINSZ, &ws) >= 0 && ws.ws_row > 0 && ws.ws_col > 0) {
	 LINES = ws.ws_row;
	 COLS = ws.ws_col;
	 line_col_calcs();
       }
     }
#else
     /* Well, if SIGWINCH is defined, but TIOCGWINSZ isn't, there's    */
     /* almost certainly something wrong.  Figure it out for yourself, */
     /* because I don't know now to deal :-)                           */
#endif
}
#endif
void
termlib_init()
{
#ifdef USETITE
	if (TI && *TI)
		tputs (TI,1,putchr);
#endif
	return;
}
void
termlib_reset()
{
#ifdef USETITE
	if (TE && *TE)
		tputs (TE,1,putchr);
#endif
	return;
}
