/*	INPUT:	Various input routines for vile
 *		written by Daniel Lawrence	5/9/86
 *		variously munged/massaged/relayered/slashed/burned
 *			since then. -pgf
 *
 *	TTgetc()	raw 8-bit key from terminal driver.
 *
 *	sysmapped_c()	single "keystroke" -- may have SPEC bit, if it was
 *			a sytem-mapped function key.  calls TTgetc().  these
 *			system-mapped keys will never map to a multi-char
 *			sequence.  the routine does have storage, to hold
 *			keystrokes gathered "in error".
 *
 *	tgetc()		fresh, pushed back, or recorded output of result of
 *			sysmapped_c() (i.e. dotcmd and the keyboard macros
 *			are recordedand played back at this level).  this is
 *			only called from mapgetc() in map.c
 *
 *	mapped_c()	(map.c) worker routine which will return a mapped
 *			or non-mapped character from the mapping engine.
 *			determines correct map, and uses its own pushback
 *			buffers on top of calls to tgetc() (see mapgetc/
 *			mapungetc).
 *
 *	mapped_keystroke() applies user-specified maps to user's input.
 *			correct map used depending on mode (insert, command,
 *			message-line).
 *
 *	keystroke()	returns pushback from mappings, the results of
 *			previous calls to mapped_keystroke().
 *
 *	keystroke8()	as above, but masks off any "wideness", i.e. SPEC bits.
 *
 *	keystroke_raw() as above, but recording is forced even if
 *			sysmapped_c() returns intrc. (old "tgetc(TRUE)")
 *
 *	kbd_seq()	the vile prefix keys (^X,^A,#) are checked for, and
 *			appropriate key pairs are turned into CTLA|c, CTLX|c,
 *			SPEC|c.
 *
 *
 *	TTtypahead() 	  true if a key is avail from TTgetc().
 *	sysmapped_c_avail() "  if a key is avail from sysmapped_c() or below.
 *	tgetc_avail()     true if a key is avail from tgetc() or below.
 *	keystroke_avail() true if a key is avail from keystroke() or below.
 *
 * $Header: /usr/build/vile/vile/RCS/input.c,v 1.188 1998/09/07 21:10:06 tom Exp $
 *
 */

#include	"estruct.h"
#include	"edef.h"
#include	"nefunc.h"

#define	DEFAULT_REG	-1

#define INFINITE_LOOP_COUNT 1200

typedef	struct	_kstack	{
	struct	_kstack	*m_link;
	int	m_save;		/* old value of 'kbdmode'		*/
	int	m_indx;		/* index identifying this macro		*/
	int	m_rept;		/* the number of times to execute the macro */
	ITBUFF  *m_kbdm;		/* the macro-text to execute		*/
	ITBUFF  *m_dots;		/* workspace for "." command		*/
#ifdef GMDDOTMACRO
	ITBUFF  *m_DOTS;		/* save-area for "." command		*/
	int	m_RPT0;		/* saves 'dotcmdcnt'			*/
	int	m_RPT1;		/* saves 'dotcmdrep'			*/
#endif
	} KSTACK;

/*--------------------------------------------------------------------------*/

/*
 * FIXME:
 * Special hacks to convert null-terminated string to/from TBUFF, for interface
 * with functions that still expect these.
 */
#define StrToBuff(buf) (buf)->tb_used = strlen(tb_values(buf))
#define BuffToStr(buf) tb_values(buf)[tb_length(buf)] = EOS

/*--------------------------------------------------------------------------*/
static	void	finish_kbm (void);

static	KSTACK *KbdStack;	/* keyboard/@-macros that are replaying */
static	ITBUFF  *KbdMacro;	/* keyboard macro, recorded	*/
static	int	last_eolchar;	/* records last eolchar-match in 'kbd_string' */

/*--------------------------------------------------------------------------*/

/*
 * Returns a pointer to the buffer that we use for saving text to replay with
 * the "." command.
 */
static ITBUFF *
TempDot(int init)
{
	static	ITBUFF  *tmpcmd;	/* dot commands, 'til we're sure */

	if (kbdmode == PLAY) {
		if (init)
			(void)itb_init(&(KbdStack->m_dots), abortc);
		return KbdStack->m_dots;
	}
	if (init || (tmpcmd == 0))
		(void)itb_init(&tmpcmd, abortc);
	return tmpcmd;
}

/*
 * Dummy function to use when 'kbd_string()' does not handle automatic completion
 */
/*ARGSUSED*/
int
no_completion(int c GCC_UNUSED, char *buf GCC_UNUSED, unsigned *pos GCC_UNUSED)
{
	return FALSE;
}

/*
 * Complete names in a shell command using the filename-completion.  We'll have
 * to scan back to the beginning of the appropriate name since that module
 * expects only one name in a buffer.
 *
 * We only do shell-completion if filename-completion is configured.
 */
#if COMPLETE_FILES
static	int	doing_shell;
int
shell_complete(
int	c,
char	*buf,
unsigned *pos)
{
	int status;
	unsigned len = *pos;
	int base;
	int first = 0;

	TRACE(("shell_complete %d:'%s'\n", *pos, buf))
	if (isShellOrPipe(buf))
		first++;

	for (base = len; base > first; ) {
		base--;
		if (isSpace(buf[base]) && (first || doing_shell)) {
			base++;
			break;
		} else if (buf[base] == '$') {
			break;
		}
	}
	len -= base;
	status = path_completion(c, buf+base, &len);
	*pos = len + base;

	return status;
}
#endif

/*
 * Ask a yes or no question in the message line. Return either TRUE, FALSE, or
 * ABORT. The ABORT status is returned if the user bumps out of the question
 * with an abortc. Used any time a confirmation is required.
 */

int
mlyesno(const char *prompt)
{
	char c; 		/* input character */

	/* in case this is right after a shell escape */
	if (!update(TRUE))
		return (ABORT);

	for_ever {
		mlforce("%s [y/n]? ",prompt);
		c = (char)keystroke();	/* get the response */

		if (ABORTED(c))		/* Bail out! */
			return(ABORT);

		if (c=='y' || c=='Y')
			return(TRUE);

		if (c=='n' || c=='N')
			return(FALSE);
	}
}

/*
 * Ask a simple question in the message line. Return the single char response,
 *  if it was one of the valid responses.
 */

int
mlquickask(const char *prompt, const char *respchars, int *cp)
{
	if (!update(TRUE))
		return (ABORT);

	for_ever {
		mlforce("%s ",prompt);
		*cp = keystroke();	/* get the response */

		if (ABORTED(*cp))	/* Bail out! */
			return(ABORT);

		if (strchr(respchars,*cp))
			return TRUE;

		kbd_alarm();
	}
}

/*
 * Prompt for a named-buffer (i.e., "register")
 */
int
mlreply_reg(
const char *prompt,
char	*cbuf,		/* 2-char buffer for register+eol */
int	*retp,		/* => the register-name */
int	at_dft)		/* default-value (e.g., for "@@" command) */
{
	register int status;
	register int c;

	if (clexec || isnamedcmd) {
		if ((status = mlreply(prompt, cbuf, 2)) != TRUE)
			return status;
		c = cbuf[0];
	} else {
		c = keystroke();
		if (ABORTED(c))
			return ABORT;
	}

	if (c == '@' && at_dft != -1) {
		c = at_dft;
	} else if (reg2index(c) < 0) {
		mlwarn("[Invalid register name]");
		return FALSE;
	}

	*retp = c;
	return TRUE;
}

/*
 * Prompt for a register-name and/or line-count (e.g., for the ":yank" and
 * ":put" commands).  The register-name, if given, is first.
 */
int
mlreply_reg_count(
int	state,		/* negative=register, positive=count, zero=either */
int	*retp,		/* returns the register-index or line-count */
int	*next)		/* returns 0/1=register, 2=count */
{
	register int status;
	char	prompt[80];
	char	expect[80];
	char	buffer[10];
	UINT	length;

	*expect = EOS;
	if (state <= 0)
		(void)strcat(expect, " register");
	if (state == 0)
		(void)strcat(expect, " or");
	if (state >= 0) {
		(void)strcat(expect, " line-count");
		length = sizeof(buffer);
	} else
		length = 2;

	(void)lsprintf(prompt, "Specify%s: ", expect);
	*buffer = EOS;
	status = kbd_string(prompt, buffer, length, ' ', 0, no_completion);

	if (status == TRUE) {
		if (state <= 0
		 && isAlpha(buffer[0])
		 && buffer[1] == EOS
		 && (*retp = reg2index(*buffer)) >= 0) {
			*next = isUpper(*buffer) ? 1 : 0;
		} else if (state >= 0
		 && string_to_number(buffer, retp)
		 && *retp) {
			*next = 2;
		} else {
			mlforce("[Expected%s]", expect);
			kbd_alarm();
			status = ABORT;
		}
	}
	return status;
}

/*
 * Write a prompt into the message line, then read back a response. Keep
 * track of the physical position of the cursor. If we are in a keyboard
 * macro throw the prompt away, and return the remembered response. This
 * lets macros run at full speed. The reply is always terminated by a carriage
 * return. Handle erase, kill, and abort keys.
 */

int
mlreply(const char *prompt, char *buf, UINT bufn)
{
	return kbd_string(prompt, buf, bufn, '\n', KBD_NORMAL, no_completion);
}

/* as above, but don't do anything to backslashes */
int
mlreply_no_bs(const char *prompt, char *buf, UINT bufn)
{
	int s;
#if COMPLETE_FILES
	doing_shell = TRUE;
	init_filec(FILECOMPLETION_BufName);
#endif
	s = kbd_string(prompt, buf, bufn, '\n', KBD_EXPAND|KBD_SHPIPE, shell_complete);
#if COMPLETE_FILES
	doing_shell = FALSE;
#endif
	return s;
}

/* as above, but neither expand nor do anything to backslashes */
int
mlreply_no_opts(const char *prompt, char *buf, UINT bufn)
{
	return kbd_string(prompt, buf, bufn, '\n', 0, no_completion);
}


/* the numbered buffer names increment each time they are referenced */
void
incr_dot_kregnum(void)
{
	if (dotcmdmode == PLAY) {
		register int	c = itb_peek(dotcmd);
		if (isDigit(c) && c < '9')
			itb_stuff(dotcmd, ++c);
	}
}

/*
 * Record a character for "." commands
 */
static void
record_dot_char(int c)
{
	if (dotcmdmode == RECORD) {
		ITBUFF	*tmp = TempDot(FALSE);
		(void)itb_append(&tmp, c);
	}
}

/*
 * Record a character for kbd-macros
 */
static void
record_kbd_char(int c)
{
	if (dotcmdmode != PLAY && kbdmode == RECORD)
		(void)itb_append(&KbdMacro, c);
}

/* if we should preserve this input, do so */
static void
record_char(int c)
{
	record_dot_char(c);
	record_kbd_char(c);
}

/* get the next character of a replayed '.' or macro */
int
get_recorded_char(
int eatit)  /* consume the character? */
{
	register int	c = -1;
	register ITBUFF	*buffer;

	if (dotcmdmode == PLAY) {

		if (interrupted()) {
			dotcmdmode = STOP;
			return intrc;
		} else {

			if (!itb_more(buffer = dotcmd)) {
				if (!eatit) {
					if (dotcmdrep > 1)
						return itb_get(buffer, 0);
				} else { /* at the end of last repetition?  */
					if (--dotcmdrep < 1) {
						dotcmdmode = STOP;
						(void)dotcmdbegin();
						/* immediately start recording
						 * again, just in case.
						 */
					} else {
						/* reset the macro to the
						 * beginning for the next rep.
						 */
						itb_first(buffer);
					}
				}
			}

			/* if there is some left... */
			if (itb_more(buffer)) {
				if (eatit)
					c = itb_next(buffer);
				else
					c = itb_peek(buffer);
				return c;
			}
		}
	}

	if (kbdmode == PLAY) { /* if we are playing a keyboard macro back, */

		if (interrupted()) {
			while (kbdmode == PLAY)
				finish_kbm();
			return intrc;
		} else {

			if (!itb_more(buffer = KbdStack->m_kbdm)) {
				if (--(KbdStack->m_rept) >= 1)
					itb_first(buffer);
				else
					finish_kbm();
			}

			if (kbdmode == PLAY) {
				buffer = KbdStack->m_kbdm;
				if (eatit)
					record_dot_char(c = itb_next(buffer));
				else
					c = itb_peek(buffer);
			}
		}
	}

	return c;
}

void
unkeystroke(int c)
{
	mapungetc(c|NOREMAP);
}

int
mapped_keystroke(void)
{
	return lastkey = mapped_c(DOMAP,NOQUOTED);
}

int
keystroke(void)
{
	return lastkey = mapped_c(NODOMAP,NOQUOTED);
}

int
keystroke8(void)
{
	int c;
	for_ever {
		c = mapped_c(NODOMAP,NOQUOTED);
		if ((c & ~0xff) == 0)
			return lastkey = c;
		kbd_alarm();
	}
}

int
keystroke_raw8(void)
{
	int c;
	for_ever {
		c = mapped_c(NODOMAP,QUOTED);
		if ((c & ~0xff) == 0)
			return lastkey = c;
		kbd_alarm();
	}
}

static int
mapped_keystroke_raw(void)
{
	return lastkey = mapped_c(DOMAP,QUOTED);
}


int
keystroke_avail(void)
{
	return mapped_c_avail();
}

static ITBUFF *tgetc_ungottenchars = NULL;
static int tgetc_ungotcnt = 0;

void
tungetc(int c)
{
	(void)itb_append(&tgetc_ungottenchars, c);
	tgetc_ungotcnt++;
}

int
tgetc_avail(void)
{
	return tgetc_ungotcnt > 0 ||
		get_recorded_char(FALSE) != -1 ||
		sysmapped_c_avail();
}

int
tgetc(int quoted)
{
	register int c;	/* fetched character */

	if (tgetc_ungotcnt > 0) {
	    tgetc_ungotcnt--;
	    return itb_last(tgetc_ungottenchars);
	}

	if ((c = get_recorded_char(TRUE)) == -1) {
		/* fetch a character from the terminal driver */
		not_interrupted();
		if (setjmp(read_jmp_buf)) {
			c = kcod2key(intrc);
    			TRACE(("setjmp/getc:%c (%#x)\n", c, c))
#if defined(linux) && defined(DISP_TERMCAP)
			/*
			 * Linux bug (observed with kernels 1.2.13 & 2.0.0):
			 * when interrupted, the _next_ character that
			 * getchar() returns will be the same as the last
			 * character before the interrupt.  Eat it.
			 */
			(void)ttgetc();
#endif
		} else {
			(void) im_waiting(TRUE);
			do { /* if it's sysV style signals,
				 we want to try again, since this
				 must not have been SIGINT, but
				 was probably SIGWINCH */
				c = sysmapped_c();
			} while (c == -1);
		}
		(void) im_waiting(FALSE);
		if (quoted || ((UINT) c != kcod2key(intrc)))
			record_char(c);
	}

	/* and finally give the char back */
	return c;
}


/*	KBD_SEQ:	Get a command sequence (multiple keystrokes) from
		the keyboard.
		Process all applicable prefix keys.
		Set lastcmd for commands which want stuttering.
*/
int
kbd_seq(void)
{
	int c;		/* fetched keystroke */

	int prefix = 0;	/* accumulate prefix */

	c = mapped_keystroke();

	if (c == cntl_a) {
		prefix = CTLA;
		c = keystroke();
	} else if (c == cntl_x) {
		prefix = CTLX;
		c = keystroke();
	} else if (c == poundc) {
		prefix = SPEC;
		c = keystroke();
	}

	c |= prefix;

	/* otherwise, just return it */
	return (lastcmd = c);
}

/*
 * Get a command-key, suppressing the mapping
 */
int
kbd_seq_nomap(void)
{
	unkeystroke(keystroke());
	return kbd_seq();
}

/* get a string consisting of inclchartype characters from the current
	position.  if inclchartype is 0, return everything to eol */
int
screen_string (char *buf, int bufn, CHARTYPE inclchartype)
{
	register int i = 0;
	MARK mk;

	mk = DOT;

	/* if from gototag(), grab from the beginning of the string */
	if (b_val(curbp, MDTAGWORD)
	 && inclchartype == vl_ident
	 && DOT.o > 0
	 && istype(inclchartype, char_at(DOT))) {
		while ( DOT.o > 0 ) {
			DOT.o--;
			if ( !istype(inclchartype, char_at(DOT)) ) {
				DOT.o++;
				break;
			}
		}
	}
	while ( i < (bufn-1) && !is_at_end_of_line(DOT)) {
		buf[i] = char_at(DOT);
#if OPT_WIDE_CTYPES
		if (i == 0) {
			if (inclchartype & vl_scrtch) {
				if (buf[0] != SCRTCH_LEFT[0])
					inclchartype &= ~vl_scrtch;
			}
			if (inclchartype & vl_shpipe) {
				if (buf[0] != SHPIPE_LEFT[0])
					inclchartype &= ~vl_shpipe;
			}
		}

		/* allow "[!command]" */
		if ((inclchartype & vl_scrtch)
		 && (i == 1)
		 && (buf[1] == SHPIPE_LEFT[0])) {
			/*EMPTY*/;
		/* guard against things like "[Buffer List]" on VMS */
		} else if ((inclchartype & vl_pathn)
		 && !ispath(buf[i])
		 && (inclchartype == vl_pathn)) {
			break;
		} else
#endif
		if (inclchartype && !istype(inclchartype, buf[i]))
			break;
		DOT.o++;
		i++;
#if OPT_WIDE_CTYPES
		if (inclchartype & vl_scrtch) {
			if ((i < bufn)
			 && (inclchartype & vl_pathn)
			 && ispath(char_at(DOT)))
				continue;
			if (buf[i-1] == SCRTCH_RIGHT[0])
				break;
		}
#endif
	}

#if OPT_WIDE_CTYPES
#if OPT_VMS_PATH
	if (inclchartype & vl_pathn) {
		;	/* override conflict with "[]" */
	} else
#endif
	if (inclchartype & vl_scrtch) {
		if (buf[i-1] != SCRTCH_RIGHT[0])
			i = 0;
	}
#endif

	buf[i] = EOS;
	DOT = mk;

	return buf[0] != EOS;
}

/*
 * Returns the character that ended the last call on 'kbd_string()'
 */
int
end_string(void)
{
	return last_eolchar;
}

void
set_end_string(int c)
{
	last_eolchar = c;
}

/*
 * Returns an appropriate delimiter for /-commands, based on the end of the
 * last reply.  That is, in a command such as
 *
 *	:s/first/last/
 *
 * we will get prompts for
 *
 *	:s/	/-delimiter saved in 'end_string()'
 *	first/
 *	last/
 *
 * If a newline is used at any stage, subsequent delimiters are forced to a
 * newline.
 */
int
kbd_delimiter(void)
{
	register int	c = '\n';

	if (isnamedcmd) {
		register int	d = end_string();
		if (isPunct(d))
			c = d;
	}
	return c;
}

/*
 * Make sure that the buffer will have extra space when passing it to functions
 * that don't know it's a TBUFF.
 */
static char *
tbreserve(TBUFF **buf)
{
	char *result = tb_values(tb_alloc(buf, tb_length(*buf) + NSTRING));
	BuffToStr(*buf);
	return result;
}

#define BACKSLASH '\\'

/* turn \X into X */
static void
remove_backslashes(TBUFF *buf)
{
	register char *cp = tb_values(buf);
	register ALLOC_T s, d;

	for (s = d = 0; s < tb_length(buf); ) {
		if (cp[s] == BACKSLASH)
			s++;
		cp[d++] = cp[s++];
	}

	buf->tb_used = d;
}

/* count backslashes so we can tell at any point whether we have the current
 * position escaped by one.
 */
static UINT
countBackSlashes(TBUFF * buf, UINT len)
{
	char *buffer = tb_values(buf);
	register UINT	count;

	if (len && buffer[len-1] == BACKSLASH) {
		count = 1;
		while (count+1 <= len &&
			buffer[len-1-count] == BACKSLASH)
			count++;
	} else {
		count = 0;
	}
	return count;
}

static void
showChar(int c)
{
	if (disinp) {
		int	save_expand = kbd_expand;
		kbd_expand = 1;	/* show all controls */
		kbd_putc(c);
		kbd_expand = save_expand;
	}
}

static void
show1Char(int c)
{
	if (disinp) {
		showChar(c);
		kbd_flush();
	}
}

/*
 * Macro result is true if the buffer is a normal :-command with a leading "!",
 * or is invoked from one of the places that supplies an implicit "!" for shell
 * commands.
 */
#define editingShellCmd(buf,options) \
	 (((options & KBD_EXPCMD) && isShellOrPipe(buf)) \
	|| (options & KBD_SHPIPE))

/* expand a single character (only used on interactive input) */
static int
expandChar(
TBUFF **buf,
unsigned * position,
int	c,
UINT	options)
{
	register int	cpos = *position;
	register char *	cp;
	register BUFFER *bp;
	char str[NFILEN];
	char *buffer = tb_values(*buf);
	int  shell = editingShellCmd(buffer,options);
	int  expand = ((options & KBD_EXPAND) || shell);
	int  exppat = (options & KBD_EXPPAT);

	/* Are we allowed to expand anything? */
	if (!(expand || exppat || shell))
	 	return FALSE;

	/* Is this a character that we know about? */
	if (strchr(global_g_val_ptr(GVAL_EXPAND_CHARS),c) == 0)
		return FALSE;

	switch (c)
	{
	case EXPC_THIS:
	case EXPC_THAT:
		if (!expand)
			return FALSE;

		bp = (c == EXPC_THIS) ? curbp : find_alt();
		if (bp == 0 || b_is_invisible(bp)) {
			kbd_alarm();	/* complain a little */
			return FALSE;	/* ...and let the user type it as-is */
		}

		cp = bp->b_fname;
		if (isInternalName(cp)) {
			cp = bp->b_bname;
		} else if (!global_g_val(GMDEXPAND_PATH)) {
			cp = shorten_path(strcpy(str, cp), FALSE);
#if OPT_MSDOS_PATH
			/* always use backslashes if invoking external prog */
			if (shell)
				cp = SL_TO_BSL(cp);
#endif
		}

		break;

	case EXPC_TOKEN:
		if (!expand)
			return FALSE;

		if (screen_string(str, sizeof(str), vl_pathn))
			cp = str;
		else
			cp = NULL;

		break;

	case EXPC_RPAT:
		if (!exppat)
			return FALSE;

		if (rpat[0])
			cp = rpat;
		else
			cp = NULL;

		break;

	case EXPC_SHELL:
		if (!shell)
			return FALSE;

#ifdef only_expand_first_bang
		/* without this check, do as vi does -- expand '!'
		   to previous command anywhere it's typed */
		if (cpos > (buffer[0] == '!'))
			return FALSE;
#endif
		cp = tb_values(save_shell[!isShellOrPipe(buffer)]);
		if (cp != NULL && isShellOrPipe(cp))
			cp++;	/* skip the '!' */
		break;

	default:
		return FALSE;
	}

	if (cp != NULL) {
		while ((c = *cp++) != EOS) {
			tb_insert(buf, cpos++, c);
			showChar(c);
		}
		kbd_flush();
	}
	*position = cpos;
	return TRUE;
}

/*
 * Returns true for the (presumably control-chars) that we use for line edits
 */
int
is_edit_char(int c)
{
	return (isreturn(c)
	  ||	isbackspace(c)
	  ||	(c == wkillc)
	  ||	(c == killc));
}

/*
 * Erases the response from the screen for 'kbd_string()'
 */
void
kbd_kill_response(TBUFF * buffer, unsigned * position, int c)
{
	char *buf = tb_values(buffer);
	TBUFF	*tmp = 0;
	int	cpos = *position;
	UINT	mark = cpos;

	tmp = tb_copy(&tmp, buffer);
	while (cpos > 0) {
		cpos--;
		kbd_erase();
		if (c == wkillc) {
			if (!isSpace(buf[cpos])) {
				if (cpos > 0 && isSpace(buf[cpos-1]))
					break;
			}
		}

		if (c != killc && c != wkillc)
			break;
	}
	if (disinp)
		kbd_flush();

	*position = cpos;
	buffer->tb_used = cpos;
	if (mark < tb_length(tmp)) {
		tb_bappend(&buffer, tb_values(tmp)+mark, tb_length(tmp)-mark);
	}
	(void)tb_free(&tmp);
}

/*
 * Display the default response for 'kbd_string()', escaping backslashes if
 * necessary.
 */
int
kbd_show_response(
TBUFF	**dst,		/* string with escapes */
char	*src,		/* string w/o escapes */
unsigned bufn,		/* # of chars we read from 'src[]' */
int	eolchar,
UINT	options)
{
	register unsigned k;

	/* add backslash escapes in front of volatile characters */
	tb_init(dst, 0);

	for (k = 0; k < bufn; k++) {
		register int c = src[k];

		if ((c == BACKSLASH) || (c == eolchar && eolchar != '\n')) {
			if (options & KBD_QUOTES)
				tb_append(dst, BACKSLASH); /* add extra */
		} else if (strchr(global_g_val_ptr(GVAL_EXPAND_CHARS),c) != 0) {
			if (c == EXPC_RPAT && !(options & KBD_EXPPAT))
				/*EMPTY*/;
			else if (c == EXPC_SHELL && !(options & KBD_SHPIPE))
				/*EMPTY*/;
			else if ((options & KBD_QUOTES)
			 && (options & KBD_EXPAND))
				tb_append(dst, BACKSLASH); /* add extra */
		}
		tb_append(dst, c);
	}

	/* put out the default response, which is in the buffer */
	kbd_init();
	for (k = 0; k < tb_length(*dst); k++) {
		showChar(tb_values(*dst)[k]);
	}
	if (disinp)
		kbd_flush();
	return tb_length(*dst);
}

/* default function for 'edithistory()' */
int
/*ARGSUSED*/
eol_history(const char * buffer GCC_UNUSED, unsigned cpos GCC_UNUSED, int c, int eolchar)
{
	if (isPrint(eolchar) || eolchar == '\r') {
		if (c == eolchar || (eolchar == '\r' && c == '\n'))
			return TRUE;
	}
	return FALSE;
}

/*
 * Store a one-level push-back of the shell command text. This allows simple
 * prompt/substitution of shell commands, while keeping the "!" and text
 * separate for the command decoder.
 */
static	int	pushed_back;
static	int	pushback_flg;
static	const char * pushback_ptr;

void
kbd_pushback(TBUFF *buf, int skip)
{
	static	TBUFF	*PushBack;
	char *buffer = tb_values(buf);	/* FIXME */

	TRACE(("kbd_pushback(%s,%d)\n", tb_visible(buf), skip))
	if (macroize(&PushBack, buf, skip)) {
		pushed_back  = TRUE;
		pushback_flg = clexec;
		pushback_ptr = execstr;
		clexec       = TRUE;
		execstr      = tb_values(PushBack);
		buffer[skip] = EOS;
		buf->tb_used = skip;
	}
}

int
kbd_is_pushed_back(void)
{
	return clexec && pushed_back;
}

/*	A more generalized prompt/reply function allowing the caller
	to specify a terminator other than '\n'.  Both are accepted.
	Assumes the buffer already contains a valid (possibly
	null) string to use as the default response.
*/
int
kbd_string(
const char *prompt,	/* put this out first */
char *extbuf,		/* the caller's (possibly full) buffer */
unsigned bufn,		/* the length of  " */
int eolchar,		/* char we can terminate on, in addition to '\n' */
UINT options,		/* KBD_EXPAND/KBD_QUOTES, etc. */
int (*complete)(DONE_ARGS)) /* handles completion */
{
	int code;
	static TBUFF *temp;
	tb_scopy(&temp, extbuf);
	code = kbd_reply(prompt, &temp, eol_history, eolchar, options, complete);
	if (bufn > tb_length(temp))
		bufn = tb_length(temp);
	if (bufn != 0)
		memcpy(extbuf, tb_values(temp), bufn);
	extbuf[bufn-1] = EOS;
	return code;
}

/*
 * We use the editc character to toggle between insert/command mode in the
 * minibuffer.  This is normally bound to ^G (the position command).
 */
static int
isMiniEdit(int c)
{
	if (c == editc)
		return TRUE;
	if (miniedit) {
		const CMDFUNC *cfp = kcod2fnc(c);
		if ((cfp != 0)
		 && (cfp->c_flags & MOTION) != 0)
			return TRUE;
	}
	return FALSE;
}

/*
 * Shift the minibuffer left/right to keep the cursor visible, as well as the
 * prompt, if that's feasible.
 */
static void
shiftMiniBuffer(int offs)
{
	static const int slack = 5;
	int shift = w_val(wminip,WVAL_SIDEWAYS);
	int adjust;
	BUFFER *savebp;
	WINDOW *savewp;
	MARK savemk;

	beginDisplay();
	savebp = curbp;
	savewp = curwp;
	savemk = MK;

	curbp  = bminip;
	curwp  = wminip;

	adjust = offs - (shift + LastMsgCol - 1);
	if (adjust >= 0)
		mvrightwind(TRUE, 1+adjust);
	else if (shift > 0 && (slack + adjust) < 0)
		mvleftwind(TRUE, slack-adjust);

	curbp = savebp;
	curwp = savewp;
	MK = savemk;

	kbd_flush();
	endofDisplay();
}

static int
editMinibuffer(TBUFF **buf, unsigned *cpos, int c, int margin, int quoted)
{
	int edited = FALSE;
	const CMDFUNC *cfp = kcod2fnc(c);
	int savedexecmode = insertmode;
	BUFFER *savebp;
	WINDOW *savewp;
	MARK savemk;

	beginDisplay();
	savebp = curbp;
	savewp = curwp;
	curbp  = bminip;
	curwp  = wminip;
	savemk = MK;

	/* Use editc (normally ^G) to toggle insert/command mode */
	if (c == editc && !quoted) {
		miniedit = !miniedit;
	} else if (isspecial(c)
	  ||  (miniedit && cfp != 0 && cfp->c_flags & MOTION)) {

		/* If we're allowed to honor SPEC bindings, then see if it's
		 * bound to something, and execute it.
		 */
		if (cfp) {
			int first = *cpos + margin;
			int old_clexec = clexec;
			int old_named  = isnamedcmd;

			/*
			 * Reset flags that might cause a recursion into the
			 * prompt/reply code.
			 */
			clexec = 0;
			isnamedcmd = 0;

			/*
			 * Set limits so we don't edit the prompt, and allow us
			 * to move the cursor with the arrow keys just past the
			 * end of line.
			 */
			b_set_left_margin(bminip, margin);
			DOT.o = llength(DOT.l);
			linsert(1,' ');	 /* pad the line so we can move */

			DOT.o = first;
			MK = DOT;
			curwp->w_line = DOT;
			(void)execute(cfp,FALSE,1);
			insertmode = savedexecmode;
			edited = TRUE;
			*cpos = DOT.o - margin;

			llength(DOT.l) -= 1;	/* strip the padding */
			b_set_left_margin(bminip, 0);

			clexec = old_clexec;
			isnamedcmd = old_named;

			/*
			 * Cheat a little, since we may have used an alias for
			 * '$', which can set the offset just past the end of
			 * line.
			 */
			if (DOT.o > llength(DOT.l))
				DOT.o = llength(DOT.l);

			/* Do something reasonable if user tried to page up
			 * in the minibuffer
			 */
			if ((first == DOT.o)
			 && (cfp->c_flags & MOTION))
				kbd_alarm();
		} else
			kbd_alarm();
	/* FIXME: Below are some hacks for making it appear that we're
	 * doing the right thing for certain non-motion commands...
	 */
	} else if (miniedit && cfp == &f_insert) {
	    miniedit = FALSE;
	} else if (miniedit && cfp == &f_insertbol) {
	    edited = editMinibuffer(buf, cpos, fnc2kcod(&f_firstnonwhite),
	                            margin, quoted);
	    miniedit = FALSE;
	} else if (miniedit && cfp == &f_appendeol) {
	    edited = editMinibuffer(buf, cpos, fnc2kcod(&f_gotoeol),
	                            margin, quoted);
	    miniedit = FALSE;
	} else if (miniedit && cfp == &f_append) {
	    edited = editMinibuffer(buf, cpos, fnc2kcod(&f_forwchar_to_eol),
	                            margin, quoted);
	    miniedit = FALSE;
	/* FIXME:  reject non-motion commands for now, since we haven't
	 * resolved what to do with the minibuffer if someone inserts a
	 * newline.
	 */
	} else if (miniedit && cfp != 0) {
		kbd_alarm();
	} else {
		miniedit = FALSE;
		if (disinp) {
			show1Char(c);
			tb_init(buf, EOS);
			tb_bappend(buf, DOT.l->l_text + margin,
				   llength(DOT.l) - margin);
		}
		else {
			char tmp = c;
			tb_bappend(buf, &tmp, 1);
		}
		*cpos += 1;
		edited = TRUE;
	}

	shiftMiniBuffer(DOT.o);

	curbp = savebp;
	curwp = savewp;
	MK = savemk;
	kbd_flush();
	endofDisplay();

	return edited;
}

/*
 * Same as 'kbd_string()', except for adding the 'endfunc' parameter.
 *
 * Returns:
 *	ABORT - abort character given (usually ESC)
 *	SORTOFTRUE - backspace from empty-buffer
 *	TRUE - buffer is not empty
 *	FALSE - buffer is empty
 */
int
kbd_reply(
const char *prompt,		/* put this out first */
TBUFF **extbuf,			/* the caller's (possibly full) buffer */
int (*endfunc)(EOL_ARGS),	/* parsing with 'eolchar' delimiter */
int eolchar,			/* char we can terminate on, in addition to '\n' */
UINT options,			/* KBD_EXPAND/KBD_QUOTES */
int (*complete)(DONE_ARGS))	/* handles completion */
{
	int	c;
	int	done;
	unsigned cpos;		/* current character position in string */
	int	status;
	int	shell;
	int	margin;
	ALLOC_T	save_len;

	register int quotef;	/* are we quoting the next char? */
	register UINT backslashes; /* are we quoting the next expandable char? */
	UINT dontmap = (options & KBD_NOMAP);
	int firstch = TRUE;
	int lastch;
	unsigned newpos;
	TBUFF *buf = 0;

	miniedit = FALSE;
	last_eolchar = EOS;	/* ...in case we don't set it elsewhere */
	tb_unput(*extbuf);	/* FIXME: trim null */

	if (clexec) {
		execstr = token(execstr, tbreserve(extbuf), eolchar);
		StrToBuff(*extbuf); /* FIXME: token should use TBUFF */
		status = (tb_length(*extbuf) != 0);
		if (status) { /* i.e. we got some input */
#if !SMALLER
			/* FIXME: may have nulls */
			if ((options & KBD_LOWERC))
				(void)mklower(tb_values(*extbuf));
			else if ((options & KBD_UPPERC))
				(void)mkupper(tb_values(*extbuf));
#endif
			if (!(options & KBD_NOEVAL)) {
				buf = tb_copy(&buf, *extbuf);
				tb_append(&buf, EOS); /* FIXME: for tokval() */
				(void)tb_scopy(extbuf,
					tokval(tb_values(buf)));
				tb_free(&buf);
				tb_unput(*extbuf); /* trim the null */
			}
			if (complete != no_completion
			 && !editingShellCmd(tb_values(*extbuf),options)) {
				cpos =
				newpos = tb_length(*extbuf);
				if ((*complete)(NAMEC, tbreserve(extbuf), &newpos)) {
					StrToBuff(*extbuf);
				} else {
					status = ABORT;
					tb_put(extbuf, cpos, EOS);
				}
			}
			/*
			 * Splice for multi-part command
			 */
			if ((last_eolchar = *execstr) != EOS)
				last_eolchar = ' ';
		}
		if (pushed_back && (*execstr == EOS)) {
			pushed_back = FALSE;
			clexec  = pushback_flg;
			execstr = pushback_ptr;
#if	OPT_HISTORY
			if (!pushback_flg) {
				hst_append(*extbuf, EOS);
			}
#endif
		}
		tb_append(extbuf, EOS);	/* FIXME */
		return status;
	}

	quotef = FALSE;
	reading_msg_line = TRUE;

	/* prompt the user for the input string */
	if (prompt != 0)
		mlprompt("%s", prompt);

	/*
	 * Use the length of the prompt (plus whatever led up to it) as the
	 * left-margin for editing.  We'll automatically scroll the edited
	 * field left/right within the margins.
	 */
	margin = llength(wminip->w_dot.l);
	cpos = kbd_show_response(&buf, tb_values(*extbuf), tb_length(*extbuf), eolchar, options);
	backslashes = 0; /* by definition, there is an even
					number of backslashes */
	c = -1;			/* initialize 'lastch' */

	for_ever {
		int	EscOrQuo = (quotef ||
				((backslashes & 1) != 0));

		lastch = c;

		/*
		 * Get a character from the user. If not quoted, treat escape
		 * sequences as a single (16-bit) special character.
		 */
		if (quotef)
			c = keystroke_raw8();
		else if (dontmap)
			/* this looks wrong, but isn't.  no mapping will happen
			anyway, since we're on the command line.  we want SPEC
			keys to be expanded to #c, but no further.  this does
			that */
			c = mapped_keystroke_raw();
		else
			c = keystroke();

		/* Ignore nulls if the calling function is not prepared to
		 * process them.  We want to be able to search for nulls, but
		 * must not use them in filenames, for example.  (We don't
		 * support name-completion with embedded nulls, either).
		 */
		if (c == 0 && !(options & KBD_0CHAR))
			continue;

		/* if we echoed ^V, erase it now */
		if (quotef) {
			firstch = FALSE;
			kbd_erase();
			kbd_flush();
		}

		/* If it is a <ret>, change it to a <NL> */
		if (c == '\r' && !quotef)
			c = '\n';

		/*
		 * If they hit the line terminate (i.e., newline or unescaped
		 * eolchar), wrap it up.
		 *
		 * Don't allow newlines in the string -- they cause real
		 * problems, especially when searching for patterns
		 * containing them -pgf
		 */
		done = FALSE;
		if (c == '\n') {
			done = TRUE;
		} else if (!EscOrQuo
		    && !is_edit_char(c)
		    && !isMiniEdit(c)) {
			BuffToStr(buf);
			if ((*endfunc)(tb_values(buf),tb_length(buf),c,eolchar)) {
				done = TRUE;
			}
		}

		if (complete != no_completion) {
			kbd_unquery();
			shell = editingShellCmd(tb_values(buf),options);
			if (done && (options & KBD_NULLOK) && cpos == 0)
				/*EMPTY*/;
			else if ((done && !(options & KBD_MAYBEC))
			 || (!EscOrQuo
			  && !(shell && isPrint(c))
			  && (c == TESTC || c == NAMEC))) {
				if (shell && isreturn(c)) {
					/*EMPTY*/;
				} else if ((*complete)(c, tbreserve(&buf), &cpos)) {
					done = TRUE;
					StrToBuff(buf); /* FIXME */
					if (c != NAMEC) /* cancel the unget */
						(void)keystroke();
				} else {
					StrToBuff(buf); /* FIXME */
					if (done) {	/* stay til matched! */
						kbd_unquery();
						(void)((*complete)(TESTC, tbreserve(&buf), &cpos));
					}
					firstch = FALSE;
					continue;
				}
			}
		}

		if (done) {
			last_eolchar = c;
			if (options & KBD_QUOTES)
				remove_backslashes(buf); /* take out quoters */

			save_len = tb_length(buf);
			hst_append(buf, eolchar);
			(void)tb_copy(extbuf, buf);
			status = (tb_length(*extbuf) != 0);

			/* If this is a shell command, push-back the actual
			 * text to separate the "!" from the command.  Note
			 * that the history command tries to do this already,
			 * but cannot for some special cases, e.g., the user
			 * types
			 *	:execute-named-command !ls -l
			 * which is equivalent to
			 *	:!ls -l
			 */
			if (status == TRUE	/* ...we have some text */
			 && (options & KBD_EXPCMD)
#if	OPT_HISTORY
			 && (tb_length(buf) == save_len) /* history didn't split it */
#endif
			 && isShellOrPipe(tb_values(buf))) {
				kbd_pushback(*extbuf, 1);
			}
			break;
		}

#if	OPT_HISTORY
		if (!EscOrQuo
		 && edithistory(&buf, &cpos, &c, options, endfunc, eolchar)) {
			backslashes = countBackSlashes(buf, cpos);
			firstch = TRUE;
			continue;
		} else
#endif
		/*
		 * If editc and abortc are the same, don't abort unless the
		 * user presses it twice in a row.
		 */
		if ((c != editc || c == lastch)
		  && ABORTED(c)
		  && !quotef
		  && !dontmap) {
			tb_init(&buf, abortc);
			status = esc_func(FALSE, 1);
			break;
		} else if ((isbackspace(c) ||
			c == wkillc ||
			c == killc) && !quotef) {

			if (prompt == 0 && c == killc)
				cpos = 0;

			if (cpos == 0) {
				if (prompt)
					mlerase();
				if (isbackspace(c)) {	/* splice calls */
					unkeystroke(c);
					status = SORTOFTRUE;
				} else {
					status = FALSE;
				}
				break;
			}

			kbd_kill_response(buf, &cpos, c);

			/*
			 * If we backspaced to erase the buffer, it's ok to
			 * return to the caller, who would be the :-line
			 * parser, so we can do something reasonable with the
			 * address specication.
			 */
			if (cpos == 0
			 && isbackspace(c)
			 && (options & KBD_STATED)) {
				status = SORTOFTRUE;
				break;
			}
			backslashes = countBackSlashes(buf, cpos);

		} else if (firstch == TRUE
		  && !quotef
		  && !isMiniEdit(c)
		  && !isspecial(c)) {
			/* clean the buffer on the first char typed */
			unkeystroke(c);
			kbd_kill_response(buf, &cpos, killc);
			backslashes = countBackSlashes(buf, cpos);

		} else if (c == quotec && !quotef) {
			quotef = TRUE;
			show1Char(c);
			continue;	/* keep firstch==TRUE */

		} else if (EscOrQuo || !expandChar(&buf, &cpos, c, options)) {
			if (c == BACKSLASH)
				backslashes++;
			else
				backslashes = 0;
			quotef = FALSE;

#if !SMALLER
			if (!isspecial(c)) {
				if ((options & KBD_LOWERC)
				 && isUpper(c))
					c = toLower(c);
				else if ((options & KBD_UPPERC)
				 && isLower(c))
					c = toUpper(c);
			}
#endif
			if (!editMinibuffer(&buf, &cpos, c, margin, EscOrQuo))
				continue;	/* keep firstch==TRUE */
		}
		firstch = FALSE;
	}
#if OPT_POPUPCHOICE
	popdown_completions();
#endif
	miniedit = FALSE;
	reading_msg_line = FALSE;
	tb_free(&buf);
	shiftMiniBuffer(0);

	TRACE(("reply:%d:%d:%s\n", status,
		(int) tb_length(*extbuf),
		tb_visible(*extbuf)))
	tb_append(extbuf, EOS);	/* FIXME */
	return status;
}

/*
 * Make the "." replay the keyboard macro
 */
#ifdef GMDDOTMACRO
static void
dot_replays_macro(int macnum)
{
	extern	const	CMDFUNC	f_kbd_mac_exec;
	char	temp[NSTRING];
	ITBUFF	*tmp;
	int	c;

	if (macnum == DEFAULT_REG) {
		if ((c = fnc2kcod(&f_kbd_mac_exec)) == -1)
			return;
		(void)kcod2str(c, temp);
	} else {
		(void)lsprintf(temp, "@%c", index2reg(macnum));
	}
	dotcmdbegin();
	tmp = TempDot(FALSE);
	(void)itb_sappend(&tmp, temp);
	dotcmdfinish();
	dotcmdbegin();
}
#endif

/*
 * Begin recording the dot command macro.
 * Set up variables and return.
 * we use a temporary accumulator, in case this gets stopped prematurely
 */
int
dotcmdbegin(void)
{
	/* never record a playback */
	if (dotcmdmode != PLAY) {
		(void)TempDot(TRUE);
		dotcmdmode = RECORD;
		return TRUE;
	}
	return FALSE;
}

/*
 * Finish dot command, and copy it to the real holding area
 */
int
dotcmdfinish(void)
{
	if (dotcmdmode == RECORD) {
		ITBUFF	*tmp = TempDot(FALSE);
		if (itb_length(tmp) == 0	/* keep the old value */
		 || itb_copy(&dotcmd, tmp) != 0) {
			itb_first(dotcmd);
			dotcmdmode = STOP;
			return TRUE;
		}
	}
	return FALSE;
}


/* stop recording a dot command,
	probably because the command is not re-doable */
void
dotcmdstop(void)
{
	if (dotcmdmode == RECORD)
		dotcmdmode = STOP;
}

/*
 * Execute the '.' command, by putting us in PLAY mode.
 * The command argument is the number of times to loop. Quit as soon as a
 * command gets an error. Return TRUE if all ok, else FALSE.
 */
int
dotcmdplay(int f, int n)
{
	if (!f)
		n = dotcmdarg ? dotcmdcnt:1;
	else if (n < 0)
		return TRUE;

	if (f)	/* we definitely have an argument */
		dotcmdarg = TRUE;
	/* else
		leave dotcmdarg alone; */

	if (dotcmdmode != STOP || itb_length(dotcmd) == 0) {
		dotcmdmode = STOP;
		dotcmdarg = FALSE;
		return FALSE;
	}

	if (n == 0) n = 1;

	dotcmdcnt = dotcmdrep = n;  /* remember how many times to execute */
	dotcmdmode = PLAY;	/* put us in play mode */
	itb_first(dotcmd);	/*    at the beginning */

	if (ukb != 0) /* save our kreg, if one was specified */
		dotcmdkreg = ukb;
	else /* use our old one, if it wasn't */
		ukb = dotcmdkreg;

	return TRUE;
}

/*
 * Test if we are replaying either '.' command, or keyboard macro.
 */
int
kbd_replaying(int match)
{
	if (dotcmdmode == PLAY) {
		/*
		 * Force a false-return if we are in insert-mode and have
		 * only one character to display.
		 */
		if (match
		 && insertmode == INSERT
		 && b_val(curbp, MDSHOWMAT)
		 && KbdStack == 0
		 && (dotcmd->itb_last+1 >= dotcmd->itb_used)) {
			return FALSE;
		}
		return TRUE;
	}
	return (kbdmode == PLAY);
}

/*
 * Begin recording a keyboard macro.
 */
/* ARGSUSED */
int
kbd_mac_begin(int f GCC_UNUSED, int n GCC_UNUSED)
{
	if (kbdmode != STOP) {
		mlforce("[Macro already active]");
		return FALSE;
	}
	mlwrite("[Start macro]");

	kbdmode = RECORD;
	return (itb_init(&KbdMacro, abortc) != 0);
}

/*
 * End keyboard macro. Check for the same limit conditions as the above
 * routine. Set up the variables and return to the caller.
 */
/* ARGSUSED */
int
kbd_mac_end(int f GCC_UNUSED, int n GCC_UNUSED)
{
	if (kbdmode == STOP) {
		mlforce("[Macro not active]");
		return FALSE;
	}
	if (kbdmode == RECORD) {
		mlwrite("[End macro]");
		kbdmode = STOP;
#ifdef GMDDOTMACRO
		dot_replays_macro(DEFAULT_REG);
#endif
	}
	/* note that if kbd_mode == PLAY, we do nothing -- that makes
		the '^X-)' at the of the recorded buffer a no-op during
		playback */
	return TRUE;
}

/*
 * Execute a macro.
 * The command argument is the number of times to loop. Quit as soon as a
 * command gets an error. Return TRUE if all ok, else FALSE.
 */
/* ARGSUSED */
int
kbd_mac_exec(int f GCC_UNUSED, int n)
{
	if (kbdmode != STOP) {
		mlforce("[Can't execute macro while recording]");
		return FALSE;
	}

	if (n <= 0)
		return TRUE;

	return start_kbm(n, DEFAULT_REG, KbdMacro);
}

/* ARGSUSED */
int
kbd_mac_save(int f GCC_UNUSED, int n GCC_UNUSED)
{
	ksetup();
	itb_first(KbdMacro);
	while (itb_more(KbdMacro))
		if (!kinsert(itb_next(KbdMacro)))
			break;
	kdone();
	mlwrite("[Keyboard macro saved in register %c.]", index2reg(ukb));
	return TRUE;
}

/*
 * Test if the given macro has already been started.
 */
int
kbm_started(int macnum, int force)
{
	if (force || (kbdmode == PLAY)) {
		register KSTACK *sp;
		for (sp = KbdStack; sp != 0; sp = sp->m_link) {
			if (sp->m_indx == macnum) {
				while (kbdmode == PLAY)
					finish_kbm();
				mlwarn("[Error: currently executing %s%c]",
					macnum == DEFAULT_REG
						? "macro" : "register ",
					index2reg(macnum));
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*
 * Start playback of the given keyboard command-string
 */
int
start_kbm(
int	n,			/* # of times to repeat */
int	macnum,			/* register to execute */
ITBUFF *ptr)			/* data to interpret */
{
	register KSTACK *sp = 0;
	ITBUFF  *tp = 0;

	if (interrupted())
		return FALSE;

	if (kbdmode == RECORD && KbdStack != 0)
		return TRUE;

	if (itb_length(ptr)
	 && (sp = typealloc(KSTACK)) != 0
	 && itb_copy(&tp, ptr) != 0) {

		/* make a copy of the macro in case recursion alters it */
		itb_first(tp);

		sp->m_save = kbdmode;
		sp->m_indx = macnum;
		sp->m_rept = n;
		sp->m_kbdm = tp;
		sp->m_link = KbdStack;

		KbdStack   = sp;
		kbdmode    = PLAY; 	/* start us in play mode */

		/* save data for "." on the same stack */
		sp->m_dots = 0;
		if (dotcmdmode == PLAY) {
#ifdef GMDDOTMACRO
			sp->m_DOTS = dotcmd;
			sp->m_RPT0 = dotcmdcnt;
			sp->m_RPT1 = dotcmdrep;
#endif
			dotcmd     = 0;
			dotcmdmode = RECORD;
		}
#ifdef GMDDOTMACRO
		  else {
			sp->m_DOTS = 0;
		  }
#endif
		return (itb_init(&dotcmd, abortc) != 0
		  &&    itb_init(&(sp->m_dots), abortc) != 0);
	}
	return FALSE;
}

/*
 * Finish a macro begun via 'start_kbm()'
 */
static void
finish_kbm(void)
{
	if (kbdmode == PLAY) {
		register KSTACK *sp = KbdStack;

		kbdmode = STOP;
		if (sp != 0) {
			kbdmode  = sp->m_save;
			KbdStack = sp->m_link;

			itb_free(&(sp->m_kbdm));
			itb_free(&(sp->m_dots));
#ifdef GMDDOTMACRO
			itb_free(&dotcmd);
			if (sp->m_DOTS != 0) {
				dotcmd     = sp->m_DOTS;
				dotcmdcnt  = sp->m_RPT0;
				dotcmdrep  = sp->m_RPT1;
				dotcmdmode = PLAY;
			}
			dot_replays_macro(sp->m_indx);
#endif
			free((char *)sp);
		}
	}
}
