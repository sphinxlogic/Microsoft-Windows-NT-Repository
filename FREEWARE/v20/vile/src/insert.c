/*
 *
 *	insert.c
 *
 * Do various types of character insertion, including most of insert mode.
 *
 * Most code probably by Dan Lawrence or Dave Conroy for MicroEMACS
 * Extensions for vile by Paul Fox
 *
 * $Header: /usr2/foxharp/src/pgf/vile/RCS/insert.c,v 1.84 1995/10/13 00:14:18 pgf Exp $
 *
 */

#include	"estruct.h"
#include	"edef.h"

#define	DOT_ARGUMENT	((dotcmdmode == PLAY) && dotcmdarg)

#define	BackspaceLimit() (b_val(curbp,MDBACKLIMIT) && autoindented <= 0)\
			? DOT.o\
			: w_left_margin(curwp)

static	int	backspace P(( void ));
static	int	doindent P(( int ));
static	int	indented_newline P(( void ));
static	int	indented_newline_above P(( void ));
static	int	ins_anytime P(( int, int, int, int * ));
static	int	ins_n_times P(( int, int, int ));
static	int	insbrace P(( int, int ));
static	int	inspound P(( void ));
static	int	isallspace P(( LINEPTR, int, int ));
static	int	nextindent P(( int * ));
static	int	openlines P(( int ));
static	int	past_wrapmargin P(( int ));
static	int	shiftwidth P(( int, int ));
static	int	tab P(( int, int ));
static	int	wrap_at_col P(( int ));
static	void	advance_one_char P(( void ));
static	void	set_insertmode P(( int ));
#if !SMALLER
static	int	istring P(( int, int, int ));
#endif

/* value of insertmode maintained through subfuncs */
static	int	savedmode;  

static int allow_aindent = TRUE;
static int skipindent;

/*--------------------------------------------------------------------------*/

/* If the wrapmargin mode is active (i.e., nonzero), and if it's not wider than
 * the screen, return the difference from the current position to the wrap
 * margin.  Otherwise, return negative.
 */
static int
past_wrapmargin(c)
int c;
{
	register int	n;

	if ((n = b_val(curbp, VAL_WRAPMARGIN)) > 0
	 && (n = (term.t_ncol - (nu_width(curwp) + n))) >= 0) {
		int list = w_val(curwp,WMDLIST);
		int used = getccol(list);

		/* compute the effective screen column after adding the
		 * latest character
		 */
		return NEXT_COLUMN(used, c, list, curtabval) - n;
	}
	return -1;
}

/* Returns true iff wrap-margin or wrap-words is active and we'll wrap at the
 * current column.
 */
static int
wrap_at_col(c)
int c;
{
	register int	n;

	if (past_wrapmargin(c) >= 0)
		return TRUE;

	if (b_val(curbp, MDWRAP)
	 && (n = b_val(curbp, VAL_FILL)) > 0)
	 	return (getccol(FALSE) > n);

	return FALSE;
}

/* advance one character past the current position, for 'append()' */
static void
advance_one_char()
{
	if (! is_header_line(DOT,curbp) && !is_at_end_of_line(DOT))
		forwchar(TRUE,1); /* END OF LINE HACK */
}

/* common logic for i,I,a,A commands */
static int
ins_n_times(f,n,advance)
int f,n,advance;
{
	register int status = TRUE;
	register int i;
	int	flag	= FALSE;

	if (!f || n < 0)
		n = 1;

	for (i = 0; i < n; i++) {
		if ((status = ins_anytime((i != 0), i, n, &flag)) != TRUE)
			break;
		if (advance && !flag)
			advance_one_char();
	}
	return status;
}

/* open lines up before this one */
int
openup(f,n)
int f,n;
{
	register int s;

	if (!f) n = 1;
	if (n < 0) return (FALSE);
	if (n == 0) return ins();

	(void)gotobol(TRUE,1);

	/* if we are in C mode and this is a default <NL> */
	if (allow_aindent && n == 1 && 
		(b_val(curbp,MDCMOD) || b_val(curbp,MDAIND)) &&
				    !is_header_line(DOT,curbp)) {
		s = indented_newline_above();
		if (s != TRUE) return (s);

		return(ins());
	}
	s = lnewline();
	if (s != TRUE) return s;

	if (!is_first_line(DOT,curbp))
		backline(TRUE,1);		/* back to the blank line */

	if ( n > 1) {
		s = openlines(n-1);
		if (s != TRUE) return s;
		s = backline(TRUE, 1);	/* backup over the first one */
		if (s != TRUE) return s;
	}

	return(ins());
}

/*
 * as above, but override all autoindenting and cmode-ing
 */
int
openup_no_aindent(f, n)
int f,n;
{
    	int s;
	int oallow = allow_aindent;
	allow_aindent = FALSE;
	s = openup(f,n);
	allow_aindent = oallow;
	return s;
}

/* open lines up after this one */
int
opendown(f,n)
int f,n;
{
	register int	s;

	if (!f) n = 1;
	if (n < 0) return (FALSE);
	if (n == 0) return ins();

	s = openlines(n);
	if (s != TRUE)
		return (s);

	return(ins());
}

/*
 * as above, but override all autoindenting and cmode-ing
 */
int
opendown_no_aindent(f, n)
int f,n;
{
    	int s;
	int oallow = allow_aindent;
	allow_aindent = FALSE;
	s = opendown(f,n);
	allow_aindent = oallow;
	return s;
}

/*
 * Open up some blank space. The basic plan is to insert a bunch of newlines,
 * and then back up over them.
 *
 * This interprets the repeat-count for the 'o' and 'O' commands.  Unlike vi
 * (which does not use the repeat-count), this specifies the number of blank
 * lines to create before proceeding with inserting the string-argument of the
 * command.
 */
static int
openlines(n)
int n;
{
	register int i = n;			/* Insert newlines. */
	register int s = TRUE;
	while (i-- && s==TRUE) {
		(void)gotoeol(FALSE,1);
		s = newline(TRUE,1);
	}
	if (s == TRUE && n)			/* Then back up over top */
		backline(TRUE, n-1);		/* of them all.		 */

	curgoal = -1;

	return s;
}

/*
 * Implements the vi 'i' command.
 */
int
insert(f, n)
int f,n;
{
	return ins_n_times(f,n,TRUE);
}

/*
 * as above, but override all autoindenting and cmode-ing
 */
int
insert_no_aindent(f, n)
int f,n;
{
    	int s;
	int oallow = allow_aindent;
	allow_aindent = FALSE;
	s = ins_n_times(f,n,TRUE);
	allow_aindent = oallow;
	return s;
}

/*
 * Implements the vi 'I' command.
 */
int
insertbol(f, n)
int f,n;
{
	if (!DOT_ARGUMENT || (dotcmdrep == dotcmdcnt))
		(void)firstnonwhite(FALSE,1);
	return ins_n_times(f,n,TRUE);
}

/*
 * Implements the vi 'a' command.
 */
int
append(f, n)
int f,n;
{
	advance_one_char();

	return ins_n_times(f,n, !DOT_ARGUMENT);
}

/*
 * Implements the vi 'A' command.
 */
int
appendeol(f, n)
int f,n;
{
	if (!is_header_line(DOT,curbp))
		(void)gotoeol(FALSE,0);

	return ins_n_times(f,n,TRUE);
}

/*
 * Unlike most flags on the mode-line, we'll only show the insertion-mode on
 * the current window.
 */
static void
set_insertmode(mode)
int mode;
{
	insertmode = mode;
	if (b_val(curbp, MDSHOWMODE))
		curwp->w_flag |= WFMODE;
}

/*
 * Implements the vi 'R' command.
 *
 * This takes an optional repeat-count and a string-argument.  The repeat-count
 * (default 1) specifies the number of times that the string argument is
 * inserted.  The length of the string-argument itself determines the number of
 * characters (beginning with the cursor position) to delete before beginning
 * the insertion.
 */
int
overwritechars(f, n)
int f,n;
{
	set_insertmode(OVERWRITE);
	return ins_n_times(f,n, TRUE);
}

/*
 * Implements the vi 'r' command.
 *
 * This takes an optional repeat-count and a single-character argument.  The
 * repeat-count (default 1) specifies the number of characters beginning with
 * the cursor position that are replaced by the argument.  Newline is treated
 * differently from the other characters (only one newline is inserted).
 *
 * Unlike vi, the number of characters replaced can be longer than a line.
 * Also, vile allows quoted characters.
 */
int
replacechar(f, n)
int f,n;
{
	register int	s = TRUE;
	register int	t = FALSE;
	register int	c;

	if (!f && lLength(DOT.l) == 0)
		return FALSE;

	if (clexec || isnamedcmd) {
		int status;
		static char cbuf[NLINE];
		if ((status=mlreply("Replace with: ", cbuf, 2)) != TRUE)
			return status;
		c = cbuf[0];
	} else {
		set_insertmode(REPLACECHAR);  /* need to fool SPEC prefix code */
		if (dotcmdmode != PLAY)
			(void)update(FALSE);
		c = keystroke();
		if (ABORTED(c)) {
			set_insertmode(FALSE);
			return ABORT;
		}

	}
	c = kcod2key(c);

	if (!f || !n)
		n = 1;
	if (n < 0)
		s = FALSE;
	else {
		int	vi_fix = (!DOT_ARGUMENT || (dotcmdrep <= 1));

		(void)ldelete((B_COUNT)n, FALSE);
		if (c == quotec) {
			t = s = quote(f,n);
		} else {
			if (isreturn(c)) {
				if (vi_fix)
					s = lnewline();
			} else {
				if (isbackspace(c)) {	/* vi beeps here */
					s = TRUE;	/* replaced with nothing */
				} else {
					t = s = linsert(n, c);
				}
			}
		}
		if ((t == TRUE) && (DOT.o > w_left_margin(curwp)) && vi_fix)
			s = backchar(FALSE,1);
	}
	set_insertmode(FALSE);
	return s;
}

/*
 * This routine performs the principal decoding for insert mode (i.e.., the
 * i,I,a,A,R commands).  It is invoked via 'ins_n_times()', which loops over
 * the repeat-count for direct commands.  One complicating factor is that
 * 'ins_n_times()' (actually its callers) is called once for each repetition in
 * a '.' command.  At this level we compute the effective loop counter (the '.'
 * and the direct commands), because we have to handle the vi-compatibilty case
 * of inserting a newline.
 *
 * We stop repeating insert after the first newline in the insertion-string
 * (that's what vi does).  If a user types
 *
 *	3iABC<nl>foo<esc>
 *
 * then we want to insert
 *
 *	ABCABCABC<nl>foo
 */
static last_insert_char;

static int
ins_anytime(playback, cur_count, max_count, splice)
int playback;
int cur_count;
int max_count;
int *splice;
{
#if OPT_MOUSE || OPT_B_LIMITS
	WINDOW	*wp0 = curwp;
#endif
	register int status;
	int	c;		/* command character */
	int backsp_limit;
	static ITBUFF *insbuff;
	int osavedmode;

	if (DOT_ARGUMENT) {
		max_count = cur_count + dotcmdcnt;
		cur_count += dotcmdcnt - dotcmdrep;
	}

	if (playback && (insbuff != 0))
		itb_first(insbuff);
	else if (!itb_init(&insbuff, abortc))
		return FALSE;

	if (insertmode == FALSE)
		set_insertmode(INSERT);
	osavedmode = savedmode;
	savedmode = insertmode;

	backsp_limit = BackspaceLimit();

	last_insert_char = EOS;

	for_ever {

		/*
		 * Read another character from the insertion-string.
		 */
		c = abortc;
		if (playback) {
			if (*splice && !itb_more(insbuff))
				playback = FALSE;
			else
				c = itb_next(insbuff);
		}
		if (!playback) {
			if (dotcmdmode != PLAY)
				(void)update(FALSE);

			c = mapped_keystroke();

#if OPT_MOUSE
			/*
			 * Prevent user from starting insertion into a
			 * modifiable buffer, then clicking on another
			 * buffer to continue inserting.  This assumes that
			 * 'setcursor()' handles entry into the other
			 * buffer.
			 */
			if (curwp != wp0) {
			    	/* end insert mode for window we started in */
			    	wp0->w_traits.insmode = FALSE;
				if (b_val(wp0->w_bufp, MDSHOWMODE))
				    wp0->w_flag |= WFMODE;
				unkeystroke(c);
				goto leave;
			}
#endif
			if (!itb_append(&insbuff, c)) {
			    status = FALSE;
			    break;
			}
		}


		if (isspecial(c)) {
		    	/* if we're allowed to honor SPEC bindings,
				then see if it's bound to something, and
				execute it */
			CMDFUNC *cfp = kcod2fnc(c);
			if (cfp) {
				int savedexecmode = insertmode;

				backsp_limit = w_left_margin(curwp);
				if (curgoal < 0)
					curgoal = getccol(FALSE);
				(void)execute(cfp,FALSE,1);
				insertmode = savedexecmode;
			}
			continue;
		}

		if (!isident(c))
			abbr_check(&backsp_limit);

		if (isreturn(c)) {
			if ((cur_count+1) < max_count) {
				if (DOT_ARGUMENT) {
					while (itb_more(dotcmd))
						(void)mapped_keystroke();
				}
				*splice = TRUE;
				status = TRUE;
				break;
			} else if (DOT_ARGUMENT) {
				*splice = TRUE;
			}
		}
		/*
		 * Decode the character
		 */
		if (ABORTED(c)) {
#if OPT_MOUSE
	leave:
#endif
			 /* an unfortunate Vi-ism that ensures one
				can always type "ESC a" if you're not sure
				you're in insert mode. */
			if (DOT.o > w_left_margin(wp0))
				backchar(TRUE,1);
			if (autoindented >= 0) {
				(void)trimline((void *)0,0,0);
				autoindented = -1;
			}
			if (cur_count+1 == max_count)
				*splice = TRUE;
			status = TRUE;
			break;
		} else if ((c & HIGHBIT) && b_val(curbp, MDMETAINSBIND)) {
		    	/* if we're allowed to honor meta-character bindings,
				then see if it's bound to something, and
				insert it if not */
			CMDFUNC *cfp = kcod2fnc(c);
			if (cfp) {
				int savedexecmode = insertmode;

				backsp_limit = w_left_margin(curwp);
				if (curgoal < 0)
					curgoal = getccol(FALSE);
				(void)execute(cfp,FALSE,1);
				insertmode = savedexecmode;
				continue;
			}
		}

		if (c == startc || c == stopc) {  /* ^Q and ^S */
			continue;
		}
#if SYS_UNIX && defined(SIGTSTP)	/* job control, ^Z */
		else if (c == suspc) {
			status = bktoshell(FALSE,1);
		}
#endif
		else {
			status = inschar(c,&backsp_limit);
			curgoal = -1;
		}

		if (status != TRUE)
			break;

#if OPT_CFENCE
		/* check for CMODE fence matching */
	        if (b_val(curbp, MDSHOWMAT))
			fmatch(c);
#endif

		/* check auto-save mode */
		if (b_val(curbp, MDASAVE)) {
			if (--curbp->b_acount <= 0) {
				/* and save the file if needed */
				(void)update(TRUE);
				filesave(FALSE, 0);
				curbp->b_acount = b_val(curbp,VAL_ASAVECNT);
			}
		}
	}

	set_insertmode(FALSE);
	savedmode = osavedmode;
	return (status);
}

/* grunt routine for insert mode */
int
ins()
{
	int	flag;
	return ins_anytime(FALSE,1,1,&flag);
}

static int
isallspace(ln,lb,ub)
LINEPTR ln;
int lb;
int ub;
{
	while (lb <= ub) {
		if (!isspace(lGetc(ln,ub)))
			return FALSE;
		ub--;
	}
	return TRUE;
}

int
inschar(c,backsp_limit_p)
int c;
int *backsp_limit_p;
{
	CmdFunc execfunc;	/* ptr to function to execute */

	execfunc = NULL;
	if (c == quotec) {
		execfunc = quote;
	} else {
		/*
		 * If a space was typed, fill column is defined, the
		 * argument is non- negative, wrap mode is enabled, and
		 * we are now past fill column, perform word wrap.
		 */
		if (wrap_at_col(c)) {
			int wm_flag = (past_wrapmargin(c) >= 0);
			if (isspace(c)) {
				int status = wrapword(wm_flag, 1);
				*backsp_limit_p = w_left_margin(curwp);
				if (wm_flag)
					return status;
			} else if (wm_flag
			    && (c == '\t'
			     || (!isspecial(c) && isprint(c)))) {
				kbd_alarm();	/* vi beeps past the margin */
			}
		}

		if ( c == '\t') { /* tab */
			execfunc = tab;
			autoindented = -1;
		} else if (isreturn(c)) {
			execfunc = newline;
			if (autoindented >= 0) {
				(void)trimline((void *)0,0,0);
				autoindented = -1;
			}
			*backsp_limit_p = w_left_margin(curwp);
		} else if ( isbackspace(c) ||
				c == tocntrl('D') ||
				c == killc ||
				c == wkillc) { /* ^U and ^W */
			execfunc = nullproc;
			/* all this says -- "is this a regular ^D for
				backing up a shiftwidth?".  otherwise,
				we treat it as ^U, below */
			if (c == tocntrl('D')
			 && !(DOT.o > *backsp_limit_p
			      && ((lGetc(DOT.l,DOT.o-1) == '0'
				  && last_insert_char == '0')
				  || (lGetc(DOT.l,DOT.o-1) == '^'
				  && last_insert_char == '^'))
			      && isallspace(DOT.l,w_left_margin(curwp),
			                          DOT.o-2))) {
				int goal, col, sw;

				sw = curswval;
				if (autoindented >=0)
					*backsp_limit_p = w_left_margin(curwp);
				col = getccol(FALSE);
				if (col > 0)
					goal = ((col-1)/sw)*sw;
				else
					goal = 0;
				while (col > goal &&
					DOT.o > *backsp_limit_p) {
					backspace();
					col = getccol(FALSE);
				}
				if (col < goal)
					linsert(goal - col,' ');
			} else {
				/* have we backed thru a "word" yet? */
				int saw_word = FALSE;

				/* was it '^^D'?  then set the flag
					that tells us to skip a line
					when calculating the autoindent
					on the next newline */
				if (c == tocntrl('D') && 
					last_insert_char == '^')
					skipindent = 1;

				while (DOT.o > *backsp_limit_p) {
					if (c == wkillc) {
						if (isspace( lGetc(DOT.l,
								DOT.o-1))) {
							if (saw_word)
								break;
						} else {
							saw_word = TRUE;
						}
					}
					backspace();
					autoindented--;
					if (c != wkillc && c != killc
					    && c != tocntrl('D'))
						break;
				}
			}
		} else if ( c ==  tocntrl('T')) { /* ^T */
			execfunc = shiftwidth;
		}

		last_insert_char = c;

	}

	if (execfunc != NULL)
		return (*execfunc)(FALSE, 1);

	/* make it a real character again */
	c = kcod2key(c);

	/* if we are in overwrite mode, not at eol,
	   and next char is not a tab or we are at a tab stop,
	   delete a char forword			*/
	if ((insertmode == OVERWRITE)
	 && (!DOT_ARGUMENT || (dotcmdrep <= 1))
	 && (DOT.o < lLength(DOT.l))
	 && (char_at(DOT) != '\t' || DOT.o % curtabval == curtabval-1)) {
		autoindented = -1;
		(void)ldelete(1L, FALSE);
	}

	/* do the appropriate insertion */
	if (allow_aindent && b_val(curbp, MDCMOD)) {
	    int dir;
	    if (is_user_fence(c, &dir) && dir == REVERSE) {
		    return insbrace(1, c);
	    } else if (c == '#') {
		    return inspound();
	    }
	}

	autoindented = -1;
	return linsert(1, c);

}

#if ! SMALLER
int
appstring(f, n)
int f, n;
{
	advance_one_char();
	return istring(f,n,INSERT);
}

int
insstring(f, n)
int f, n;
{
	return istring(f,n,INSERT);
}

int
overwstring(f, n)
int f, n;
{
	return istring(f,n,OVERWRITE);
}

/* ask for and insert or overwrite a string into the current */
/* buffer at the current point */
static int
istring(f,n,mode)
int f,n;
int mode;
{
	register char *tp;	/* pointer into string to add */
	register int status;	/* status return code */
	int backsp_limit;
	static char tstring[NPAT+1];	/* string to add */

	/* ask for string to insert */
	status = mlreply("String to insert: ", tstring, NPAT);
	if (status != TRUE)
		return(status);


	if (f == FALSE)
		n = 1;

	if (n < 0)
		n = - n;

	set_insertmode(mode);

	backsp_limit = BackspaceLimit();

	/* insert it */
	while (n--) {
		tp = &tstring[0];
		while (*tp) {
			status = inschar(*tp++,&backsp_limit);
			if (status != TRUE) {
				set_insertmode(FALSE);
				return(status);
			}
		}
	}

	set_insertmode(FALSE);
	return(TRUE);
}
#endif

static int
backspace()
{
	register int	s;

	if ((s=backchar(TRUE, 1)) == TRUE && insertmode != OVERWRITE)
		s = ldelete(1L, FALSE);
	return (s);
}

/*
 * Insert a newline. If we are in CMODE, do automatic
 * indentation as specified.
 */
int
newline(f, n)
int f,n;
{
	register int	s;

	if (!f)
		n = 1;
	else if (n < 0)
		return (FALSE);

	/* if we are in C or auto-indent modes and this is a default <NL> */
	if (allow_aindent
	 && (n == 1)
	 && (b_val(curbp,MDCMOD) || b_val(curbp,MDAIND))
	 && !is_header_line(DOT,curbp))
		return indented_newline();

	/*
	 * If a newline was typed, fill column is defined, the argument is non-
	 * negative, wrap mode is enabled, and we are now past fill column,
	 * perform word wrap.
	 */
	if (!is_at_end_of_line(DOT) && wrap_at_col(EOS))
		wrapword(FALSE,1);

	/* insert some lines */
	while (n--) {
		if ((s=lnewline()) != TRUE)
			return (s);
		curwp->w_flag |= WFINS;
	}
	return (TRUE);
}

/* insert a newline and indentation for C */
static int
indented_newline()
{
	int cmode = allow_aindent && b_val(curbp, MDCMOD);
	register int indentwas; /* indent to reproduce */
	int bracef; /* was there a brace at the end of line? */

	if (lnewline() == FALSE)
		return FALSE;

	indentwas = previndent(&bracef);
	skipindent = 0;

	if (cmode && bracef)
		indentwas = nextsw(indentwas);

	return doindent(indentwas);
}

/* insert a newline and indentation for autoindent */
static int
indented_newline_above()
{
	int cmode = allow_aindent && b_val(curbp, MDCMOD);
	register int indentwas;	/* indent to reproduce */
	int bracef; /* was there a brace at the beginning of line? */

	indentwas = nextindent(&bracef);
	if (lnewline() == FALSE)
		return FALSE;
	if (backline(TRUE,1) == FALSE)
		return FALSE;
	if (cmode && bracef)
		indentwas = nextsw(indentwas);

	return doindent(indentwas);
}

/* get the indent of the last previous non-blank line.	also, if arg
	is non-null, check if line ended in a brace */
int
previndent(bracefp)
int *bracefp;
{
	int ind;
	int cmode = allow_aindent && b_val(curbp, MDCMOD);

	if (bracefp) *bracefp = FALSE;

	MK = DOT;

	/* backword() will leave us either on this line, if there's something
		non-blank here, or on the nearest previous non-blank line. */
	/* (at start of buffer, may leave us on empty line) */
	do {
	    if (backword(FALSE,1) == FALSE || lLength(DOT.l) == 0) {
		    (void)gomark(FALSE,1);
		    return 0;
	    }
	    DOT.o = 0;
	/* if the line starts with a #, then don't copy its indent */
	} while ((skipindent-- > 0) || (cmode && lGetc(DOT.l, 0) == '#'));

	ind = indentlen(l_ref(DOT.l));
	if (bracefp) {
		int lc = lastchar(l_ref(DOT.l));
		int c = lGetc(DOT.l,lc);
		int dir;
		*bracefp = (lc >= 0 && (c == ':' || 
				(is_user_fence(c, &dir) && dir == FORWARD)));

	}

	(void)gomark(FALSE,1);

	return ind;
}

/* get the indent of the next non-blank line.	also, if arg
	is non-null, check if line starts in a brace */
static int
nextindent(bracefp)
int *bracefp;
{
	int ind;
	int fc;

	MK = DOT;

	/* we want the indent of this line if it's non-blank, or the indent
		of the next non-blank line otherwise */
	fc = firstchar(l_ref(DOT.l));
	if (fc < 0 && (   forwword(FALSE,1) == FALSE
	               || (fc = firstchar(l_ref(DOT.l))) < 0)) {
		if (bracefp)
			*bracefp = FALSE;
		DOT = MK;
		return 0;
	}
	ind = indentlen(l_ref(DOT.l));
	if (bracefp) {
		*bracefp = ((lGetc(DOT.l,fc) == RBRACE) ||
				(lGetc(DOT.l,fc) == RPAREN) ||
				(lGetc(DOT.l,fc) == RBRACK));
	}

	DOT = MK;

	return ind;
}

static int
doindent(ind)
int ind;
{
	register int i;

	/* first clean up existing leading whitespace */
	DOT.o = w_left_margin(curwp);
	i = firstchar(l_ref(DOT.l));
	if (i > 0)
		(void)ldelete((B_COUNT)i,FALSE);

	autoindented = 0;
	/* if no indent was asked for, we're done */
	if (ind > 0) {
		if ((i=ind/curtabval)!=0) {
			autoindented += i;
			if (tab(TRUE,i) == FALSE)
				return FALSE;
		}
		if ((i=ind%curtabval) != 0) {
			autoindented += i;
			if (linsert(i,	' ') == FALSE)
				return FALSE;
		}
	}
	if (!autoindented)
		autoindented = -1;

	return TRUE;
}

/* return the column indent of the specified line */
int
indentlen(lp)
LINE *lp;
{
	register int ind, i, c;
	ind = 0;
	for (i=0; i<llength(lp); ++i) {
		c = lgetc(lp, i);
		if (!isspace(c))
			break;
		if (c == '\t')
			ind = nextab(ind);
		else
			++ind;
	}
	return ind;
}


/* insert a brace or paren into the text here... we are in CMODE */
static int
insbrace(n, c)
int n;	/* repeat count */
int c;	/* brace/paren to insert (always '}' or ')' for now) */
{

#if ! OPT_CFENCE
	/* wouldn't want to back up from here, but fences might take us
		forward */
	/* if we are at the beginning of the line, no go */
	if (DOT.o <= w_left_margin(curwp))
		return(linsert(n,c));
#endif

	if (autoindented >= 0) {
		(void)trimline((void *)0,0,0);
	} else {
		return linsert(n,c);
	}
	skipindent = 0;
#if ! OPT_CFENCE /* no fences?	then put brace one tab in from previous line */
	doindent(((previndent(NULL)-1) / curtabval) * curtabval);
#else /* line up brace with the line containing its match */
	doindent(fmatchindent(c));
#endif
	autoindented = -1;

	/* and insert the required brace(s) */
	return(linsert(n, c));
}

static int
inspound()	/* insert a # into the text here...we are in CMODE */
{

	/* if we are at the beginning of the line, no go */
	if (DOT.o <= w_left_margin(curwp))
		return(linsert(1,'#'));

	if (autoindented > 0) { /* must all be whitespace before us */
		DOT.o = w_left_margin(curwp);
		(void)ldelete((B_COUNT)autoindented,FALSE);
	}
	autoindented = -1;

	/* and insert the required pound */
	return(linsert(1, '#'));
}

/* insert a tab into the file */
static int
tab(f, n)
int f,n;
{
	int ccol;
	if (!f) n = 1;
	if (n <= 0)
		return FALSE;

	if (b_val(curbp,MDTABINSERT))
		return linsert(n, '\t');

	ccol = getccol(FALSE);
	return linsert((nextab(ccol) - ccol) + (n-1)*curtabval,' ');
}

/*ARGSUSED*/
static int
shiftwidth(f, n)
int f,n;
{
	int logical_col;
	int char_index;
	int space_count;
	int all_white;
	int add_spaces;
	int c;
	int s;

	char_index = DOT.o;

	/*
	 * Compute the "logical" column; i.e. the column the cursor is
	 * in on the screen.
	 *
	 * While we're at it, compute the spaces just before the insert
	 * point.
	 */

	(void) gocol(0);
    	logical_col = 0;
	space_count = 0;
	all_white = TRUE;
	while (DOT.o < char_index) {
	    c = char_at(DOT);
	    if (c == ' ') {
		space_count++;
	    } else {
		space_count = 0;
	    }
	    if (!isspace(c)) {
		all_white = FALSE;
	    }

	    if (c == '\t') {
		logical_col += curtabval - (logical_col % curtabval);
    	    } else {
		logical_col++;
	    }

	    DOT.o++;
	}

	DOT.o = char_index;

	/*
	 * Now we can compute the destination column. If this is the same
	 * as the tab column, delete the spaces before the insert point
	 * & insert a tab; otherwise, insert spaces as required.
	 */
	add_spaces = curswval - (logical_col % curswval);
	if (space_count + add_spaces > curtabval) {
	    space_count = curtabval - add_spaces;
	}
	if ( (add_spaces + logical_col) % curtabval == 0) {
	    if (space_count > 0) {
		DOT.o -= space_count;
		s = ldelete(space_count, FALSE);
	    } else {
		space_count = 0;
		s = TRUE;
	    }
	    if (s) {
		space_count += add_spaces;
		s = linsert((space_count + curtabval - 1) / curtabval, '\t');
	    }
	} else {
	    s = linsert(add_spaces, ' ');
	}

	if (all_white && s) {
	    if (autoindented >= 0) {
		int fc = firstchar(l_ref(DOT.l));
		if (fc >= 0)
			autoindented = fc;
		else /* all white */
			autoindented = lLength(DOT.l);
	    }
	}
	return s;
}

/*
 * Quote the next character, and insert it into the buffer. All the characters
 * are taken literally, with the exception of the newline, which always has
 * its line splitting meaning. The character is always read, even if it is
 * inserted 0 times, for regularity.
 */
int
quote(f, n)
int f,n;
{
	register int	s;
	register int	c;

	c = keystroke_raw8();
	if (!f)
		n = 1;
	if (n < 0)
		return FALSE;
	if (n == 0)
		return TRUE;
	if (c == '\n') {
		do {
			s = lnewline();
		} while (s==TRUE && --n);
		return s;
	}
	return linsert(n, c);
}

#if OPT_EVAL
char *
current_modename()
{
	switch (savedmode) {
		default:
			return "command";
		case INSERT:
			return "insert";
		case OVERWRITE:
			return "overwrite";
		case REPLACECHAR:
			return "replace";
	}
}
#endif
