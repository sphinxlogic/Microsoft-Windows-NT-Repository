#include <HTUtils.h>
#include <HTAlert.h>
#include <LYCurses.h>
#include <LYStyle.h>
#include <LYUtils.h>
#include <LYGlobalDefs.h>
#include <LYSignal.h>
#include <LYClean.h>
#include <LYReadCFG.h>
#include <LYStrings.h>
#include <LYCharSets.h>
#include <UCAux.h>

#include <LYexit.h>
#include <LYLeaks.h>

#ifdef VMS
#include <LYMainLoop.h>
#endif

#if defined(VMS) && defined(__GNUC__)
#include <gnu_hacks.h>
#undef LINES
#undef COLS
#define LINES lines
#define COLS cols
extern int _NOSHARE(LINES);
extern int _NOSHARE(COLS);
#endif /* VMS && __GNUC__ */

#ifdef USE_COLOR_STYLE
#include <AttrList.h>
#include <LYHash.h>
#endif

#if defined(COLOR_CURSES)
int lynx_has_color = FALSE;
#endif

#ifdef HAVE_XCURSES
char *XCursesProgramName = "Lynx";
#endif

#if defined(USE_COLOR_STYLE) && !USE_COLOR_TABLE
#define COLOR_BKGD ((s_normal != NOSTYLE) ? hashStyles[s_normal].color : A_NORMAL)
#else
#define COLOR_BKGD ((COLOR_PAIRS >= 9) ? COLOR_PAIR(9) : A_NORMAL)
#endif

/*
 *  These are routines to start and stop curses and to cleanup
 *  the screen at the end.
 */

PRIVATE int dumbterm PARAMS((char *terminal));
BOOLEAN LYCursesON = FALSE;

#if USE_COLOR_TABLE || defined(USE_SLANG)
PUBLIC int Current_Attr, Masked_Attr;
#endif

#define OMIT_SCN_KEEPING 0 /* whether to omit keeping of Style_className
    in HTML.c when lss support is on. 1 to increase performance. The value
    must correspond to the value of macro OMIT_SCN_KEEPING defined in HTML.c*/


#ifdef USE_SLANG
PUBLIC unsigned int Lynx_Color_Flags = 0;
PUBLIC BOOLEAN FullRefresh = FALSE;
PUBLIC int curscr = 0;
#ifdef SLANG_MBCS_HACK
/*
 *  Will be set by size_change. - KW
 */
PUBLIC int PHYSICAL_SLtt_Screen_Cols = 10;
#endif /* SLANG_MBCS_HACK */



PUBLIC void LY_SLrefresh NOARGS
{
    if (FullRefresh) {
	SLsmg_suspend_smg();
	SLsmg_resume_smg();
	FullRefresh = FALSE;
    } else {
	SLsmg_refresh();
    }

    return;
}

/* the following renamed from LY_SLclear since it is more like erase()
   described in curses man pages than like clear(); but for USE_SLANG
   clear() is still a macro calling this, and will do the same thing as
   erase(). - kw */
PUBLIC void LY_SLerase NOARGS
{
    SLsmg_gotorc (0, 0);
    SLsmg_erase_eos ();
}

#ifdef VMS
PUBLIC void VTHome NOARGS
{
    printf("\033[;H");

    return;
}
#endif /* VMS */

PUBLIC void LYaddAttr ARGS1(
	int,		a)
{
    Current_Attr |= a;
    SLsmg_set_color(Current_Attr & ~Masked_Attr);
}

PUBLIC void LYsubAttr ARGS1(
	int,		a)
{
    Current_Attr &= ~a;
    SLsmg_set_color(Current_Attr & ~Masked_Attr);
}

PRIVATE void lynx_setup_attrs NOARGS
{
    static int monoattr[] = {
	0,
	SLTT_BOLD_MASK,
	SLTT_REV_MASK,
	SLTT_REV_MASK | SLTT_BOLD_MASK,
	SLTT_ULINE_MASK,
	SLTT_ULINE_MASK | SLTT_BOLD_MASK,
	SLTT_ULINE_MASK | SLTT_REV_MASK,
	SLTT_ULINE_MASK | SLTT_BOLD_MASK | SLTT_REV_MASK
    };
    int n;

    for (n = 1; n <= 7; n++)
	SLtt_set_mono(n, NULL, (monoattr[n] & ~Masked_Attr));
}

PUBLIC void lynx_setup_colors NOARGS
{
    CTRACE((tfp, "lynx_setup_colors\n"));
    SLtt_set_color(0, NULL, DEFAULT_FG, DEFAULT_BG);
    SLtt_set_color(1, NULL, "blue",	DEFAULT_BG); /* bold */
    SLtt_set_color(2, NULL, "yellow",	"blue");     /* reverse */
    SLtt_set_color(4, NULL, "magenta",	DEFAULT_BG); /* underline */
    /*
     *	The other objects are '|'ed together to get rest.
     */
    SLtt_set_color(3, NULL, "green",	DEFAULT_BG); /* bold-reverse */
    SLtt_set_color(5, NULL, "blue",	DEFAULT_BG); /* bold-underline */
    SLtt_set_color(6, NULL, "red",	DEFAULT_BG); /* reverse-underline */
    SLtt_set_color(7, NULL, "magenta",	"cyan");     /* reverse-underline-bold */
    /*
     *	Now set monochrome attributes.
     */
    lynx_setup_attrs();
}

PRIVATE void sl_suspend ARGS1(
	int,		sig)
{
#ifdef SIGSTOP
#ifndef VMS
    int r, c;

    lynx_enable_mouse (0);
    if (sig == SIGTSTP)
	SLsmg_suspend_smg();
    SLang_reset_tty();
    kill(getpid(),SIGSTOP);
#if SLANG_VERSION > 9929
    SLang_init_tty(-1, 0, 1);
#else
    SLang_init_tty(3, 0, 1);
#endif /* SLANG_VERSION > 9929 */
    signal(SIGTSTP, sl_suspend);
#if defined(REAL_UNIX_SYSTEM) && !defined(__CYGWIN__)
    SLtty_set_suspend_state(1);
#endif
    if (sig == SIGTSTP)
	SLsmg_resume_smg();
    /*
     *	Get new window size in case it changed.
     */
    r = SLtt_Screen_Rows;
    c = SLtt_Screen_Cols;
    size_change(0);
    if ((r != SLtt_Screen_Rows) || (c != SLtt_Screen_Cols)) {
	recent_sizechange = TRUE;
    }
    lynx_enable_mouse (1);
#endif /* !VMS */
#endif /* SIGSTOP */
    return;
}
#endif /* USE_SLANG */

/*
**  This function boxes windows for (n)curses.
*/
PUBLIC void LYbox ARGS2(
	WINDOW *,	win,
	BOOLEAN,	formfield GCC_UNUSED)
{
#ifdef USE_SLANG
    SLsmg_draw_box(win->top_y, win->left_x, win->height, win->width + 4);
#else
#ifdef VMS
    /*
     * This should work for VAX-C and DEC-C, since they both have the same
     * win._max_y and win._max_x members -TD
     *
     * (originally VMSbox by FM)
     */
    int i;

    wmove(win, 0, 0);
    waddstr(win, "\033)0\016l");
    for (i = 1; i < win->_max_x; i++)
	waddch(win, 'q');
    waddch(win, 'k');
    for (i = 1; i < win->_max_y-1; i++) {
	wmove(win, i, 0);
	waddch(win, 'x');
	wmove(win, i, win->_max_x-1);
	waddch(win, 'x');
    }
    wmove(win, i, 0);
    waddch(win, 'm');
    for (i = 1; i < win->_max_x; i++)
	waddch(win, 'q');
    waddstr(win, "j\017");
#else /* !VMS */
    /*
     *	If the terminal is in UTF-8 mode, it probably cannot understand
     *	box drawing characters as (n)curses handles them.  (This may also
     *	be true for other display character sets, but isn't currently
     *	checked.)  In that case, substitute ASCII characters for BOXVERT
     *	and BOXHORI if they were defined to 0 for automatic use of box
     *	drawing characters.  They'll stay as they are otherwise. - KW & FM
     */
    int boxvert, boxhori;

    UCSetBoxChars(current_char_set, &boxvert, &boxhori, BOXVERT, BOXHORI);
#ifdef CSS
    if (formfield)
	wcurses_css(win, "frame", ABS_ON);
#endif
    /*
     *	If we don't have explicitly specified characters for either
     *	vertical or horizontal lines, the characters that box() would
     *	use for the corners probably also won't work well.  So we
     *	specify our own ASCII characters for the corners and call
     *	wborder() instead of box(). - kw
     */
#ifdef HAVE_WBORDER
    if (!boxvert || !boxhori)
	box(win, boxvert, boxhori);
    else if (boxvert == '*' || boxhori == '*')
	wborder(win, boxvert, boxvert, boxhori, boxhori, '*', '*', '*', '*');
    else
	wborder(win, boxvert, boxvert, boxhori, boxhori, '/', '\\', '\\', '/');
#else
    box(win, boxvert, boxhori);
#endif
#ifdef CSS
    if (formfield)
	wcurses_css(win, "frame", ABS_OFF);
#endif
#endif /* VMS */
    wrefresh(win);
#endif /* USE_SLANG */
}

#if defined(USE_COLOR_STYLE)
/* Ok, explanation of the USE_COLOR_STYLE styles.  The basic styles (ie non
 * HTML) are set the same as the SLANG version for ease of programming.  The
 * other styles are simply the HTML enum from HTMLDTD.h + 16.
 */
PUBLIC HTCharStyle displayStyles[DSTYLE_ELEMENTS];

/*
 * set a style's attributes - RP
 */
PUBLIC void setStyle ARGS4(int,style,int,color,int,cattr,int,mono)
{
    displayStyles[style].color = color;
    displayStyles[style].cattr = cattr;
    displayStyles[style].mono = mono;
}

PUBLIC void setHashStyle ARGS5(int,style,int,color,int,cattr,int,mono,char*,element)
{
    bucket* ds = &hashStyles[style];
    CTRACE((tfp, "CSS(SET): <%s> hash=%d, ca=%#x, ma=%#x\n", element, style, color, mono));
    ds->color = color;
    ds->cattr = cattr;
    ds->mono = mono;
    ds->code = style;
    FREE(ds->name);
    StrAllocCopy(ds->name, element);
}

/*
 * set the curses attributes to be color or mono - RP
 */
PRIVATE int LYAttrset ARGS3(WINDOW*,win,int,color,int,mono)
{
	CTRACE((tfp, "CSS:LYAttrset (%#x, %#x)\n", color, mono));
	if (lynx_has_color && LYShowColor >= SHOW_COLOR_ON && color > -1)
	{
		wattrset(win,color);
		return color;
	}
	if (mono > -1)
	{
		wattrset(win,mono);
		return mono;
	}
	wattrset(win,A_NORMAL);
	return A_NORMAL;
}

PUBLIC void curses_w_style ARGS3(
	WINDOW*,	win,
	int,		style,
	int,		dir)
{
#if OMIT_SCN_KEEPING
# define SPECIAL_STYLE /*(CSHASHSIZE+1) */ 88888
/* if TRACEs are not compiled in, this macro is redundant - we needn't valid
'ds' to stack off. */
#endif

    int YP,XP;
#if !OMIT_SCN_KEEPING
    bucket* ds= (style == NOSTYLE ? &nostyle_bucket : &hashStyles[style]);
#else
    bucket* ds= (style == NOSTYLE ?	     &nostyle_bucket :
	    (style== SPECIAL_STYLE ? &special_bucket :&hashStyles[style]) );
#endif


    if (!ds->name) {
	CTRACE((tfp, "CSS.CS:Style %d not configured\n",style));
#if !OMIT_SCN_KEEPING
	return;
#endif
    }

    CTRACE((tfp, "CSS.CS:<%s%s> (%d)\n",(dir?"":"/"),ds->name,ds->code));

    getyx (win, YP, XP);

    if (style == s_normal && dir) {
	wattrset(win,A_NORMAL);
	if (win==stdscr) cached_styles[YP][XP]=s_normal;
	return;
    }

    switch (dir)
    {
	/* ABS_OFF is the same as STACK_OFF for the moment */
    case STACK_OFF:
	if (last_colorattr_ptr) {
	    int last_attr = last_styles[--last_colorattr_ptr];
	    LYAttrset(win,last_attr,last_attr);
	}
	else
	    LYAttrset(win,A_NORMAL,-1);
	break;

    case STACK_ON: /* remember the current attributes */
	if (last_colorattr_ptr > 127) {
	    CTRACE((tfp,"........... %s (0x%x) %s\r\n",
			"attribute cache FULL, dropping last",
			last_styles[last_colorattr_ptr],
			"in LynxChangeStyle(curses_w_style)"));
	    last_colorattr_ptr--;
	}
	last_styles[last_colorattr_ptr++] = getattrs(stdscr);
	/* don't cache style changes for active links */
#if OMIT_SCN_KEEPING
	/* since we don't compute the hcode to stack off in HTML.c, we
	 * don't know whether this style is configured.  So, we
	 * shouldn't simply return on stacking on unconfigured
	 * styles, we should push curr attrs on stack.  -HV
	 */
	if (!ds->name) break;
#endif
	/* FALL THROUGH */
    case ABS_ON: /* change without remembering the previous style */
	    /* don't cache style changes for active links and edits */
	if ( style != s_alink && style != s_aedit
	     && style != s_aedit_pad && style != s_aedit_arr ) {
	    CTRACE((tfp, "CACHED: <%s> @(%d,%d)\n", ds->name, YP, XP));
	    if (win == stdscr) cached_styles[YP][XP] = style;
	}
	LYAttrset(win, ds->color, ds->mono);
	break;
    }
}

/*
 * wrapper function to set on-screen styles - RP
 */
PUBLIC void wcurses_css ARGS3(
    WINDOW *,	win,
    char*,	name,
    int,	dir)
{
    int try_again = 1;

    while (try_again) {
	int tmpHash = hash_code(name);
	CTRACE((tfp, "CSSTRIM:trying to set [%s] style - ", name));
	if (tmpHash == NOSTYLE) {
	    char *class = strrchr(name, '.');
	    CTRACE((tfp, "undefined, trimming at %p\n", class));
	    if (class)	*class = '\0';
	    else	try_again = 0;
	} else {
	    CTRACE((tfp, "ok (%d)\n", hash_code(name)));
	    curses_w_style(win, hash_code(name), dir);
	    try_again = 0;
	}
    }
}

PUBLIC void curses_css ARGS2(char *,name,int,dir)
{
    wcurses_css(stdscr, name, dir);
}

PUBLIC void curses_style ARGS2(
	int,	style,
	int,	dir)
{
    curses_w_style(stdscr, style, dir);
}

#ifdef NOT_USED
void attribute ARGS2(int,style,int,dir)
{
    curses_style(style, dir, 0);
}
#endif
#endif /* USE_COLOR_STYLE */

PRIVATE BOOL lynx_called_initscr = FALSE;

#if HAVE_USE_DEFAULT_COLORS && USE_DEFAULT_COLORS
/*
 * If we find a "default" color while reading the config-file, set default
 * colors on the screen.
 */
PUBLIC int lynx_default_colors NOARGS
{
    int code = 0;
    if (lynx_called_initscr) {
	code = -1;
	if (!default_color_reset && use_default_colors() == OK) {
	    default_fg = DEFAULT_COLOR;
	    default_bg = DEFAULT_COLOR;
	    code = 1;
	}
    }
    return code;
}
#endif /* HAVE_USE_DEFAULT_COLORS && USE_DEFAULT_COLORS */

#if USE_COLOR_TABLE && defined(COLOR_CURSES)
/*
 * This block of code is designed to produce the same color effects using SVr4
 * curses as the slang library's implementation in this module.  That maps the
 * SGR codes into a 0-7 index into the color table, with special treatment for
 * backgrounds.  There's a bit of convoluted (but necessary) code handling the
 * special case of initialization before 'initscr()' is called.
 * 1997/1/19 - T.E.Dickey <dickey@clark.net>
 */

PRIVATE struct {
    int fg, bg;
    chtype attr;
} lynx_color_cfg[] = {
    /*0*/ { DEFAULT_FG,    DEFAULT_BG,	A_NORMAL}, /* A_NORMAL */
    /*1*/ { COLOR_BLUE,    DEFAULT_BG,	A_NORMAL}, /* A_BOLD */
    /*2*/ { COLOR_YELLOW,  COLOR_BLUE,	A_BOLD},   /* A_REVERSE */
    /*3*/ { COLOR_GREEN,   DEFAULT_BG,	A_NORMAL}, /* A_REVERSE | A_BOLD */
    /*4*/ { COLOR_MAGENTA, DEFAULT_BG,	A_NORMAL}, /* A_UNDERLINE */
    /*5*/ { COLOR_BLUE,    DEFAULT_BG,	A_NORMAL}, /* A_UNDERLINE | A_BOLD */
    /*6*/ { COLOR_RED,	   DEFAULT_BG,	A_NORMAL}, /* A_UNDERLINE | A_REVERSE */
    /*7*/ { COLOR_MAGENTA, COLOR_CYAN,	A_NORMAL}  /* A_UNDERLINE | A_BOLD | A_REVERSE */
};

/*
 * Hold the codes for color-pairs here until 'initscr()' is called.
 */
PRIVATE struct {
    int fg;
    int bg;
} lynx_color_pairs[25];

/*
 * Map the SGR attributes (0-7) into ANSI colors, modified with the actual BOLD
 * attribute we'll get 16 colors.
 */
PRIVATE void LYsetWAttr ARGS1(WINDOW *, win)
{
    if (lynx_has_color && LYShowColor >= SHOW_COLOR_ON) {
	int code = 0;
	int attr = A_NORMAL;
	int offs = 1;

	if (Current_Attr & A_BOLD)
		code |= 1;
	if (Current_Attr & A_REVERSE)
		code |= 2;
	if (Current_Attr & A_UNDERLINE)
		code |= 4;
	attr = lynx_color_cfg[code].attr;

	if (code+offs < COLOR_PAIRS) {
		attr |= COLOR_PAIR(code+offs);
	}

	wattrset(win, attr & ~Masked_Attr);
    } else {
	wattrset(win, Current_Attr & ~Masked_Attr);
    }
}

PRIVATE void lynx_map_color ARGS1(int, n)
{
    int m;

    CTRACE((tfp, "lynx_map_color(%d)\n", n));

    lynx_color_pairs[n+1].fg = lynx_color_cfg[n].fg;
    lynx_color_pairs[n+1].bg = lynx_color_cfg[n].bg;

    lynx_color_pairs[n+9].fg = lynx_color_cfg[n].fg;
    lynx_color_pairs[n+9].bg = lynx_color_cfg[0].bg;

    lynx_color_pairs[n+17].fg = lynx_color_cfg[n].bg;
    lynx_color_pairs[n+17].bg = lynx_color_cfg[n].bg;

    if (lynx_called_initscr) {
	for (m = 0; m <= 16; m += 8) {
	    int pair = n + m + 1;
	    if (pair < COLOR_PAIRS)
		init_pair((short)pair,
		    (short)lynx_color_pairs[pair].fg,
		    (short)lynx_color_pairs[pair].bg);
	}
	if (n == 0 && LYShowColor >= SHOW_COLOR_ON)
	    bkgd(COLOR_BKGD | ' ');
    }
}

PUBLIC int lynx_chg_color ARGS3(
	int, color,
	int, fg,
	int, bg
	)
{
    if (fg == ERR_COLOR || bg == ERR_COLOR) return -1;
    if (color >= 0 && color < 8) {
	lynx_color_cfg[color].fg = (fg > 7) ? (fg & 7) : fg;
	lynx_color_cfg[color].bg = (bg > 7) ? (bg & 7) : bg;
	lynx_color_cfg[color].attr = ((fg > 7) && (fg & 8)) ? A_BOLD : A_NORMAL;
	lynx_map_color(color);
    } else {
	return -1;
    }
    return 0;
}

PUBLIC void lynx_set_color ARGS1(int, a)
{
    if (lynx_has_color && LYShowColor >= SHOW_COLOR_ON) {
	attrset(lynx_color_cfg[a].attr
		| (((a+1) < COLOR_PAIRS)
			? COLOR_PAIR(a+1)
			: A_NORMAL));
    }
}

PUBLIC void lynx_standout ARGS1(int, flag)
{
    if (flag)
	LYaddAttr(A_REVERSE);
    else
	LYsubAttr(A_REVERSE);
}

PRIVATE void lynx_init_colors NOARGS
{
    if (lynx_has_color) {
	size_t n, m;

	CTRACE((tfp, "lynx_init_colors\n"));

	lynx_color_cfg[0].fg = default_fg;
	lynx_color_cfg[0].bg = default_bg;

	for (n = 0; n < TABLESIZE(lynx_color_cfg); n++) {
	    for (m = 0; m <= 16; m += 8) {
		int pair = n + m + 1;
		if (pair < COLOR_PAIRS)
		    init_pair((short)pair,
			(short)lynx_color_pairs[pair].fg,
			(short)lynx_color_pairs[pair].bg);
	    }
	    if (n == 0 && LYShowColor >= SHOW_COLOR_ON)
		bkgd(COLOR_BKGD | ' ');
	}
    } else if (LYShowColor != SHOW_COLOR_NEVER) {
	LYShowColor = SHOW_COLOR_OFF;
    }
}

PUBLIC void lynx_setup_colors NOARGS
{
    int n;
    CTRACE((tfp, "lynx_setup_colors\n"));
    for (n = 0; n < 8; n++)
	lynx_map_color(n);
}
#endif /* USE_COLOR_TABLE */

PUBLIC void LYnoVideo ARGS1(
	int,		a)
{
    CTRACE((tfp, "LYnoVideo(%d)\n", a));
#ifdef USE_SLANG
    if (a & 1) Masked_Attr |= SLTT_BOLD_MASK;
    if (a & 2) Masked_Attr |= SLTT_REV_MASK;
    if (a & 4) Masked_Attr |= SLTT_ULINE_MASK;
    lynx_setup_attrs();
#else
#if USE_COLOR_TABLE
    if (a & 1) Masked_Attr |= A_BOLD;
    if (a & 2) Masked_Attr |= A_REVERSE;
    if (a & 4) Masked_Attr |= A_UNDERLINE;
#endif
#endif
}

#if       !defined(VMS) && !defined(USE_SLANG)
/*
 * If newterm is not defined, assume a curses subset which
 * supports only initscr.  --gil
 */
#if defined(HAVE_NEWTERM) && !defined(NCURSES) && !defined(HAVE_RESIZETERM)
static SCREEN *LYscreen = NULL;
#define LYDELSCR() { \
if (recent_sizechange) { \
    delscreen(LYscreen); \
    LYscreen = NULL; } }
/*
 * Surrogates for newterm annd delscreen
 */
#else  /* HAVE_NEWTERM   */
static WINDOW *LYscreen = NULL;
#undef  newterm
#define newterm(type, out, in) (initscr())
#define LYDELSCR()  /* nothing */
#endif /* HAVE_NEWTERM   */
#else  /* !defined(VMS) && !defined(USE_SLANG) */
/*
 * Provide last recourse definitions of LYscreen and LYDELSCR for
 * stop_curses, which only tests LYscreen for zero/nonzero but
 * never uses it as a pointer or L-value.
 */
#define LYscreen TRUE
#define LYDELSCR()  /* nothing */
#endif /* !defined(VMS) && !defined(USE_SLANG) */

PUBLIC void start_curses NOARGS
{
    int keypad_on = 0;
#ifdef USE_SLANG
    static int slinit;

    if (LYCursesON) {
	CTRACE((tfp, "start_curses: Hmm, already ON.\n"));
	return;
    }

    if (slinit == 0) {
#if defined(USE_KEYMAPS)
	if (-1 == lynx_initialize_keymaps ())
	    exit (-1);
#else
	SLtt_get_terminfo();
#endif
#if (defined(__DJGPP__) && !defined(DJGPP_KEYHANDLER)) || defined(__CYGWIN__)
	SLkp_init ();
#endif /* __DJGPP__ && !DJGPP_KEYHANDLER */

#if defined(REAL_UNIX_SYSTEM) && !defined(__CYGWIN__)
#if SLANG_VERSION >= 9935
	SLang_TT_Read_FD = fileno(stdin);
#endif /* SLANG_VERSION >= 9935 */
#endif /* REAL_UNIX_SYSTEM && !__CYGWIN__ */

#if !defined(USE_KEYMAPS) && defined(ENHANCED_LINEEDIT) && defined(ESCDELAY)
	/* way to get ESC that's not part of a recognized sequence through */
	ESCDELAY = 2000;
#endif
	/*
	 *  Check whether a saved show_color:off override is in effect. - kw
	 */
	if (LYrcShowColor == SHOW_COLOR_NEVER) {
	    SLtt_Use_Ansi_Colors = 0;
	}
	/*
	 *  Check whether we're forcing color on. - FM
	 */
	if ((LYShowColor > 1) && (Lynx_Color_Flags & SL_LYNX_USE_COLOR))
	    SLtt_Use_Ansi_Colors = 1;
	/*
	 *  Check whether a -nocolor override is in effect. - kw
	 */
	if (Lynx_Color_Flags & SL_LYNX_OVERRIDE_COLOR)
	    SLtt_Use_Ansi_Colors = 0;
	/*
	 *  Make sure our flags are in register. - FM
	 */
	if (SLtt_Use_Ansi_Colors == 1) {
	    if (LYShowColor != SHOW_COLOR_ALWAYS) {
		LYShowColor = SHOW_COLOR_ON;
	    }
	} else {
	    if (LYShowColor != SHOW_COLOR_NEVER) {
		LYShowColor = SHOW_COLOR_OFF;
	    }
	}
	size_change(0);

#if (defined(VMS) || defined(REAL_UNIX_SYSTEM)) && !defined(__CYGWIN__)
	if ((Masked_Attr & SLTT_ULINE_MASK) == 0) {
	    SLtt_add_color_attribute(4, SLTT_ULINE_MASK);
	    SLtt_add_color_attribute(5, SLTT_ULINE_MASK);
	}
	/*
	 *  If set, the blink escape sequence will turn on high
	 *  intensity background (rxvt and maybe Linux console).
	 */
	if (LYShowColor && (Lynx_Color_Flags & SL_LYNX_USE_BLINK)) {
	    SLtt_Blink_Mode = 1;
	} else {
	    SLtt_Blink_Mode = 0;
	}
#endif /* (VMS || REAL_UNIX_SYSTEM) && !__CYGWIN__  */
    }
#ifdef __DJGPP__
#ifdef WATT32
    _eth_init();
#else
    else sock_init();
#endif /* WATT32 */
#endif /* __DJGPP__ */

    slinit = 1;
    Current_Attr = 0;
#ifndef VMS
#if SLANG_VERSION > 9929
    SLang_init_tty(-1, 0, 1);
#else
    SLang_init_tty(3, 0, 1);
#endif /* SLANG_VERSION > 9929 */
#endif /* !VMS */
    SLsmg_init_smg();
    SLsmg_Display_Eight_Bit = LYlowest_eightbit[current_char_set];
    if (SLsmg_Display_Eight_Bit > 191)
	SLsmg_Display_Eight_Bit = 191; /* may print ctrl chars otherwise - kw */
    scrollok(0,0);
    SLsmg_Backspace_Moves = 1;
#if SLANG_VERSION > 10306
    SLsmg_touch_screen ();
#endif
#ifndef VMS
#if defined(REAL_UNIX_SYSTEM) && !defined(__CYGWIN__)
    SLtty_set_suspend_state(1);
#endif /* REAL_UNIX_SYSTEM && !__CYGWIN__ */
#ifdef SIGTSTP
    if (!no_suspend)
	signal(SIGTSTP, sl_suspend);
#endif /* SIGTSTP */
    signal(SIGINT, cleanup_sig);
#endif /* !VMS */

   lynx_enable_mouse (1);

#else /* USE_SLANG; Now using curses: */


#ifdef VMS
    /*
     *	If we are VMS then do initscr() everytime start_curses()
     *	is called!
     */
    initscr();	/* start curses */
#else  /* Unix: */

    if (!LYscreen) {
	/*
	 *  If we're not VMS then only do initscr() one time,
	 *  and one time only!
	 */
#if defined(HAVE_NEWTERM) && !defined(NCURSES) && !defined(HAVE_RESIZETERM)
	{
	    /*
	     * Put screen geometry in environment variables used by
	     * XOpen curses before calling newterm().  I believe this
	     * completes work left unfinished by AJL & FM -- gil
	     */
	    static char lines_putenv[] = "LINES=abcde",
			cols_putenv[]  = "COLUMNS=abcde";
	    BOOLEAN savesize;

	    savesize = recent_sizechange;
	    size_change(0);
	    recent_sizechange = savesize;    /* avoid extra redraw */
	    sprintf(lines_putenv + 6, "%d", LYlines & 0xfff);
	    sprintf(cols_putenv  + 8, "%d", LYcols  & 0xfff);
	    putenv(lines_putenv);
	    putenv(cols_putenv);
	    CTRACE((tfp, "start_curses putenv %s, %s\n", lines_putenv, cols_putenv));
	}
#endif /* HAVE_NEWTERM   */
	if (!(LYscreen=newterm(NULL,stdout,stdin))) {  /* start curses */
	    fprintf(tfp, "%s\n",
		gettext("Terminal initialisation failed - unknown terminal type?"));
	    exit_immediately (-1);
	}
	lynx_called_initscr = TRUE;
#if defined(SIGWINCH) && defined(NCURSES_VERSION)
	size_change(0);
	recent_sizechange = FALSE; /* prevent mainloop drawing 1st doc twice */
#endif /* SIGWINCH */

#if defined(USE_KEYMAPS) && defined(NCURSES_VERSION)
#  if HAVE_KEYPAD
	/* Need to switch keypad on before initializing keymaps, otherwise
	   when the keypad is switched on, some keybindings may be overriden. */
	keypad(stdscr,TRUE);
	keypad_on = 1;
#  endif /* HAVE_KEYPAD */

	if (-1 == lynx_initialize_keymaps ()) {
	    endwin();
	    exit (-1);
	}
#endif

	/*
	 * This is a workaround for a bug in SVr4 curses, observed on Solaris
	 * 2.4:  if your terminal's alternate-character set contains codes in
	 * the range 128-255, they'll be sign-extended in the acs_map[] table,
	 * which in turn causes their values to be emitted as 255 (0xff).
	 * "Fix" this by forcing the table to 8-bit codes (it has to be
	 * anyway).
	 */
#if defined(ALT_CHAR_SET) && !defined(NCURSES_VERSION)
	{
	    int n;
	    for (n = 0; n < 128; n++)
		if (ALT_CHAR_SET[n] & 0x80) {
		    ALT_CHAR_SET[n] &= 0xff;
		    ALT_CHAR_SET[n] |= A_ALTCHARSET;
		}
	}
#endif

#if defined(USE_COLOR_STYLE) || defined(USE_COLOR_TABLE)
	if (has_colors()) {
	    lynx_has_color = TRUE;
	    start_color();
#if USE_DEFAULT_COLORS
#ifdef EXP_ASSUMED_COLOR
	    /*
	     * Adjust the color mapping table to match the ASSUMED_COLOR
	     * setting in lynx.cfg
	     */
	    if (assume_default_colors(default_fg, default_bg) != OK) {
		default_fg = COLOR_WHITE;
		default_bg = COLOR_BLACK;
	    }
	    if (default_fg >= 0 || default_bg >= 0) {
		unsigned n;
		for (n = 0; n < TABLESIZE(lynx_color_cfg); n++) {
		    if (default_fg >= 0 && lynx_color_cfg[n].fg < 0)
			lynx_color_cfg[n].fg = default_fg;
		    if (default_bg >= 0 && lynx_color_cfg[n].bg < 0)
			lynx_color_cfg[n].bg = default_bg;
		    CTRACE((tfp, "color_cfg[%d] = %d/%d\n", n,
			    lynx_color_cfg[n].fg,
			    lynx_color_cfg[n].bg));
		}
		lynx_setup_colors();
	    }
#else
	    lynx_default_colors();
#endif /* EXP_ASSUMED_COLOR */
#endif /* USE_DEFAULT_COLORS */
	}
#endif /* USE_COLOR_STYLE || USE_COLOR_TABLE */

#ifdef USE_COLOR_STYLE
	parse_userstyles();
#endif
#if USE_COLOR_TABLE
	lynx_init_colors();
#endif /* USE_COLOR_TABLE */
    }
#ifdef __DJGPP__
#ifdef WATT32
    _eth_init();
#else
    else sock_init();
#endif /* WATT32 */
#endif /* __DJGPP__ */
#endif /* not VMS */

    /* nonl();	 */ /* seems to slow things down */

#ifdef VMS
    crmode();
    raw();
#else
#if HAVE_CBREAK
    cbreak();
#else
    crmode();
#endif /* HAVE_CBREAK */
    signal(SIGINT, cleanup_sig);
#endif /* VMS */

    noecho();

#if HAVE_KEYPAD
    if (!keypad_on)
	keypad(stdscr,TRUE);
#endif /* HAVE_KEYPAD */

    lynx_enable_mouse (1);

    fflush(stdin);
    fflush(stdout);
    fflush(stderr);
#endif /* USE_SLANG */

#if defined(WIN_EX)
    clear();
#endif

    LYCursesON = TRUE;
    CTRACE((tfp, "start_curses: done.\n"));
}  /* end of start_curses() */


PUBLIC void lynx_enable_mouse ARGS1(int,state)
{
#ifdef USE_MOUSE
/***********************************************************************/

#if defined(WIN_EX)
/* modify lynx_enable_mouse() for pdcurses configuration so that mouse support
   is disabled unless -use_mouse is specified.  This is ifdef'd with
   __BORLANDC__ for the time being (WB).
*/
    HANDLE hConIn = INVALID_HANDLE_VALUE;
    hConIn = GetStdHandle(STD_INPUT_HANDLE);
    if (LYUseMouse == 0)
    {
	SetConsoleMode(hConIn, ENABLE_WINDOW_INPUT);
	FlushConsoleInputBuffer(hConIn);
	return;
    }
#endif

    if (LYUseMouse == 0)
	return;


#if defined(USE_SLANG)
    SLtt_set_mouse_mode (state, 0);
    SLtt_flush_output ();
#else

#if defined(WIN_EX) && defined(PDCURSES)
    if (state)
    {
	SetConsoleMode(hConIn, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
	FlushConsoleInputBuffer(hConIn);
    }
#else
#if defined(NCURSES)
    if (state) {
	/* Compensate for small value of maxclick in ncurses.  */
	static int was = 0;

	if (!was) {
	    int old = mouseinterval(-1);

	    was++;
	    if (old < 200)		/* Default 166 */
		mouseinterval(300);
	}
	/* Inform ncurses which mouse events we're interested in.
	 * We shouldn't need to include BUTTONn_PRESSED and BUTTONn_RELEASED
	 * events, since ncurses should translate them to click events. - kw
	 * However, if we do not include them, then ncurses effectively
	 * ignores mouseinterval(), thus translates *any* sequence of
	 * press/release to a click, which leads to inconveniences.
	 * We special-case these events in LYStrings.c.
	 */
	mousemask(BUTTON_CTRL | BUTTON_ALT
		  | BUTTON1_PRESSED | BUTTON1_RELEASED
		  | BUTTON1_CLICKED
		  | BUTTON1_DOUBLE_CLICKED | BUTTON1_TRIPLE_CLICKED
		  | BUTTON2_PRESSED | BUTTON2_RELEASED
		  | BUTTON2_CLICKED
		  | BUTTON3_PRESSED | BUTTON3_RELEASED
		  | BUTTON3_CLICKED
		  | BUTTON3_DOUBLE_CLICKED | BUTTON3_TRIPLE_CLICKED,
		  NULL);
    } else
	mousemask(0, NULL);
#endif /* NCURSES */
#endif /* WIN_EX and PDCURSES */

#if defined(PDCURSES)
    if (state)
	mouse_set(
		BUTTON1_CLICKED | BUTTON1_PRESSED | BUTTON1_RELEASED |
		BUTTON2_CLICKED | BUTTON2_PRESSED | BUTTON2_RELEASED |
		BUTTON3_CLICKED | BUTTON3_PRESSED | BUTTON3_RELEASED);
#endif
#endif      /* NOT USE_SLANG */

/***********************************************************************/
#endif /* USE_MOUSE */
}

PUBLIC void stop_curses NOARGS
{
    if (LYCursesON)
	echo();
#ifdef __DJGPP__
#ifdef WATT32
    _eth_release();
#else
    sock_exit();
#endif /* WATT32 */
#endif /* __DJGPP__ */
#if defined(DOSPATH) && !(defined(USE_SLANG) || _WIN_CC)
    clrscr();
#else

    /*
     *	Fixed for better dumb terminal support.
     *	05-28-94 Lynx 2-3-1 Garrett Arch Blythe
     */
    if(LYCursesON == TRUE)	{
	lynx_enable_mouse (0);
#if (!defined(WIN_EX) || defined(__CYGWIN__))	/* @@@ */
	if(LYscreen) {
	    endwin();	/* stop curses */
	    LYDELSCR();
	}
#endif
    }
#ifdef SH_EX
    {
	int i;
	for (i=0; i <= 3; i++) {
	    fprintf(stdout, "\r\n");
	}
    }
#endif

    fflush(stdout);
#endif /* defined(DOSPATH) && !(defined(USE_SLANG) || _WIN_CC) */
    fflush(stderr);

    LYCursesON = FALSE;
    CTRACE((tfp, "stop_curses: done.\n"));

#if defined(SIGTSTP) && defined(USE_SLANG)
#ifndef VMS
    if (!no_suspend)
	signal(SIGTSTP, SIG_DFL);
#endif /* !VMS */
#endif /* SIGTSTP && USE_SLANG */

#ifndef VMS
    signal(SIGINT, SIG_DFL);
#endif /* !VMS */
}

#ifdef VMS
/*
 *  Check terminal type, start curses & setup terminal.
 */
PUBLIC BOOLEAN setup ARGS1(
	char *,		terminal)
{
    int c;
    int status;
    char *dummy = 0, *cp, term[81];
#ifdef USE_SLANG
    extern void longname();
#endif /* USE_SLANG */

    /*
     *	If the display was not set by a command line option then
     *	see if it is available from the environment.
     */
    if ((cp = LYgetXDisplay()) != 0) {
	StrAllocCopy(x_display, cp);
    } else {
	FREE(x_display);
    }

    /*
     *	Get terminal type, and convert to lower case.
     */
    term[0] = '\0';
    longname(dummy, term);
    if (term[0] == '\0' && (form_get_data || form_post_data)) {
	/*
	 *  Some yoyo used these under conditions which require
	 *  -dump, so force that mode here. - FM
	 */
	dump_output_immediately = TRUE;
	LYcols = 80;
	if (keypad_mode == NUMBERS_AS_ARROWS)
	    keypad_mode = LINKS_ARE_NUMBERED;
	status = mainloop();
	(void) signal (SIGHUP, SIG_DFL);
	(void) signal (SIGTERM, SIG_DFL);
#ifdef SIGTSTP
	if (no_suspend)
	  (void) signal(SIGTSTP,SIG_DFL);
#endif /* SIGTSTP */
	exit(status);
    }
    LYLowerCase(term);

    printf("%s%s\n", gettext("Terminal ="), term);
    sleep(InfoSecs);
    if ((strlen(term) < 5) ||
	strncmp(term, "vt", 2) || !isdigit(term[2])) {
	printf("%s\n",
	    gettext("You must use a vt100, 200, etc. terminal with this program."));
	printf(CONFIRM_PROCEED, "n/y");
	c = getchar();
	if (c != 'y' && c != 'Y') {
	    printf("\n");
	    return(FALSE);
	}
	strcpy(term,"vt100");
    }

    ttopen();
    start_curses();

    LYlines = LINES;
    LYcols = COLS;
    if (LYlines <= 0)
	LYlines = 24;
    if (LYcols <= 0)
	LYcols = 80;

    return(TRUE);
}

#else	/* Not VMS: */

/*
 *  Check terminal type, start curses & setup terminal.
 */
PUBLIC BOOLEAN setup ARGS1(
	char *,		terminal)
{
    char *term_putenv = NULL;
    char *buffer = NULL;
    char *cp;

   /*
    *  If the display was not set by a command line option then
    *  see if it is available from the environment .
    */
    if ((cp = LYgetXDisplay()) != NULL) {
	StrAllocCopy(x_display, cp);
    } else {
	FREE(x_display);
    }

    if (terminal != NULL) {
	HTSprintf0(&term_putenv, "TERM=%.106s", terminal);
	(void) putenv(term_putenv);
    }

    /*
     *	Query the terminal type.
     */
    if (dumbterm(getenv("TERM"))) {
	char *s;

	printf("\n\n  %s\n\n", gettext("Your Terminal type is unknown!"));
	printf("  %s [vt100] ", gettext("Enter a terminal type:"));

	if (LYSafeGets(&buffer, stdin) != 0)
	    if ((s = strchr(buffer, '\n')) != NULL)
		*s = '\0';

	if (buffer == 0 || *buffer == 0)
	    StrAllocCopy(buffer,"vt100");

	HTSprintf0(&term_putenv,"TERM=%.106s", buffer);
	FREE(buffer);

	(void) putenv(term_putenv);
	printf("\n%s %s\n", gettext("TERMINAL TYPE IS SET TO"), getenv("TERM"));
	LYSleepMsg();
    }

    start_curses();

#if HAVE_TTYTYPE
    /*
     *	Get terminal type (strip 'dec-' from vms style types).
     */
    if (strncmp((CONST char*)ttytype, "dec-vt", 6) == 0) {
	(void) setterm(ttytype + 4);
    }

    /*
     *  Account for lossage on the 'sun' terminal type (80x24) Sun text
     *  console driver. It only supports reverse video, but all SGR
     *  sequences produce that same reverse video, and the terminfo
     *  entry lists different SGRs for 'bold' and 'rev'. As a result,
     *  the current link is indistinguishable from all other links.
     *  The workaround here is to disable the 'rev' capability.
     */
    if ((strncmp(ttytype, "sun", 3) == 0)) {
	LYnoVideo(2);
    }
#endif /* HAVE_TTYTYPE */

    LYlines = LINES;
    LYcols = COLS;

#if defined(PDCURSES_EXP) && defined(WIN_EX) && defined(CJK_EX) /* 1999/08/26 (Thu) 17:53:38 */
    {
	extern int current_codepage;	/* PDCurses lib. */

	if (current_codepage == 932)
	    LYcols = COLS - 1;
    }
#endif
    if (LYlines <= 0)
	LYlines = 24;
    if (LYcols <= 0)
	LYcols = 80;

    return(1);
}

PRIVATE int dumbterm ARGS1(
	char *,		terminal)
{
    int dumb = FALSE;

    /*
     *	Began checking for terminal == NULL in case that TERM environment
     *	variable is not set.  Thanks to Dick Wesseling (ftu@fi.ruu.nl).
     */
    if (terminal == NULL ||
	!strcasecomp(terminal, "network") ||
	!strcasecomp(terminal, "unknown") ||
	!strcasecomp(terminal, "dialup")  ||
	!strcasecomp(terminal, "dumb")	  ||
	!strcasecomp(terminal, "switch")  ||
	!strcasecomp(terminal, "ethernet")  )
	dumb = TRUE;
    return(dumb);
}

#ifdef FANCY_CURSES
#ifndef USE_COLOR_STYLE
#if USE_COLOR_TABLE
PUBLIC void LYaddWAttr ARGS2(
	WINDOW *,	win,
	int,		a)
{
    Current_Attr |= a;
    LYsetWAttr(win);
}

PUBLIC void LYaddAttr ARGS1(
	int,		a)
{
    LYaddWAttr(stdscr, a);
}

PUBLIC void LYsubWAttr ARGS2(
	WINDOW *,	win,
	int,		a)
{
    Current_Attr &= ~a;
    LYsetWAttr(win);
}

PUBLIC void LYsubAttr ARGS1(
	int,		a)
{
    LYsubWAttr(stdscr, a);
}
#endif /* USE_COLOR_TABLE */
#endif /* !USE_COLOR_STYLE */
#endif /* FANCY_CURSES */
#endif /* VMS */

/* Use this rather than the 'wprintw()' function to write a blank-padded
 * string to the given window, since someone's asserted that printw doesn't
 * handle 8-bit characters unlike addstr (though more info would be useful).
 *
 * We're blank-filling so that with SVr4 curses, it'll show the background
 * color to a uniform width in the popup-menu.
 */
#ifndef USE_SLANG
PUBLIC void LYpaddstr ARGS3(
	WINDOW *,	the_window,
	int,		width,
	CONST char *,	the_string)
{
    width -= strlen(the_string);
    LYwaddstr(the_window, the_string);
    while (width-- > 0)
	waddstr(the_window, " ");
}
#endif

PUBLIC WINDOW *LYstartPopup ARGS4(
    int,	top_y,
    int,	left_x,
    int,	height,
    int,	width)
{
    WINDOW *form_window = 0;
#ifdef USE_SLANG
    static WINDOW fake_window;
    SLsmg_fill_region(top_y, left_x - 1, height, width + 4, ' ');
    form_window = &fake_window;
    form_window->top_y  = top_y;
    form_window->left_x = left_x;
    form_window->height = height;
    form_window->width  = width;
#else
    if (!(form_window = newwin(height, width + 4, top_y, left_x - 1)) &&
	!(form_window = newwin(height, 0, top_y, 0))) {
	HTAlert(POPUP_FAILED);
    } else {
	LYsubwindow(form_window);
    }
#endif /* USE_SLANG */
    return form_window;
}

PUBLIC void LYstartTargetEmphasis NOARGS
{
#ifdef USE_COLOR_STYLE
    if (s_whereis != NOSTYLE) {
	curses_style(s_whereis, STACK_ON);
	return;
    }
#endif
#if defined(FANCY_CURSES) || defined(USE_SLANG)
    start_bold();
    start_reverse();
#endif /* FANCY_CURSES || USE_SLANG */
    start_underline();
}

PUBLIC void LYstopTargetEmphasis NOARGS
{
#ifdef USE_COLOR_STYLE
    if (s_whereis != NOSTYLE) {
	curses_style(s_whereis, STACK_OFF);
	return;
    }
#endif
    stop_underline();
#if defined(FANCY_CURSES) || defined(USE_SLANG)
    stop_reverse();
    stop_bold();
#endif /* FANCY_CURSES || USE_SLANG */
}

/*
 * Accommodate the different flavors of touchline
 */
PUBLIC void LYtouchline ARGS1(
	int,		row)
{
#if defined(HAVE_WREDRAWLN)
    wredrawln(stdscr, row, 1);
#else
#if defined(HAVE_TOUCHLINE)
    /* touchline() is not available on VMS before version 7.0, and then only on
     * Alpha, since prior ports of curses were broken.  BSD touchline() has a
     * 4th parameter since it is used internally by touchwin().
     */
    touchline(stdscr, row, 1, 0);
#else
#if !defined(USE_SLANG)
    touchwin(stdscr);
#else
    SLsmg_touch_lines(row, 1);
#endif
#endif
#endif
}

/*
 * There's no guarantee that a library won't temporarily write on its input.
 * Be safe and copy it when we have const-data.
 */
PUBLIC void LYwaddnstr ARGS3(
	WINDOW *,	w,
	CONST char *,	s,
	size_t,		len)
{
    while (len > 0) {
	char temp[MAX_LINE];
	size_t use = (len >= MAX_LINE) ? MAX_LINE - 1 : len;
	memcpy(temp, s, use);
	temp[use] = 0;
	waddstr(w, temp);
	len -= use;
    }
}

#ifdef VMS
/*
 *	Cut-down termio --
 *		Do character-oriented stream input for Jeff.
 *		Code ripped off from Micro-Emacs 3.7 by Daniel Lawrence.
 *
 *		Ever-so-slightly modified by Kathryn Huxtable.	29-Jan-1991.
 *		Cut down for Lou.  8 Sep 1992.
 *		Cut down farther for Lou.  19 Apr 1993.
 *			We don't set PASSALL or PASTHRU since we don't
 *			want to block CTRL/C, CTRL/Y, CTRL/S or CTRL/Q.
 *			Simply setting NOECHO and doing timed reads
 *			is sufficient.
 *		Further mods by Fote.  29-June-1993
 *			ttopen() and ttclose() are now terminal initialization
 *			 and restoration procedures, called once at startup
 *			 and at exit, respectively, of the LYNX image.
 *			ttclose() should be called before an exit from LYNX
 *			 no matter how the exit is invoked.
 *			setup(terminal) does the ttopen().
 *			cleanup() calls cleanup_files() and ttclose().
 *			ttgetc() now handles NOECHO and NOFLITR (instead of
 *			 setting the terminal itself to NOECHO in ttopen()).
 *			VMSsignal() added for handling both Ctrl-C *and* Ctrl-Y
 *			 interrupts, and disabling system response to Ctrl-T.
 *		Further mods by Fote.  15-Dec-1993
 *			Added edit handler in ttopen() which will invoke
 *			 VMSexit() and behave intelligently on ACCVIO's.
 *		Further mods by Fote.  29-Dec-1993
 *			Simplified ttgetc().
 *		Further mods by Fote.  16-Jan-1994
 *			Added code in ttopen() which will invoke VMSVersion()
 *			 to get the version of VMS as VersionVMS for use by
 *			 by new or modified interrupt or spawning routines.
 *		Further mods by Fote.  27-Jan-1994
 *			Added back a typeahead() which supports 'z' or 'Z' as
 *			an "Zap transfer" command via HTCheckForInterrupt()
 *			in LYUtils.c.
 */

#include <descrip.h>
#include <iodef.h>
#include <ssdef.h>
#include <msgdef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <libclidef.h>
#include <lib$routines.h>
#include <starlet.h>
#include <clidef.h>
#include <syidef.h>
#ifdef signal
#undef signal
#endif /* signal */
#include <signal.h>
#ifdef system
#undef system
#endif /* system */
#include <processes.h>
#include <LYVMSdef.h>

#define EFN	0			/* Event flag			*/

static	unsigned char buffer[20];	/* Input buffer			*/
static	int	in_pos, in_len;		/* For escape sequences		*/
static	int	oldmode[3];		/* Old TTY mode bits		*/
static	int	newmode[3];		/* New TTY mode bits		*/
static	short	iochan;			/* TTY I/O channel		*/
static	$DESCRIPTOR(term_nam_dsc,"TT"); /* Descriptor for iochan	*/
static	unsigned long mask = LIB$M_CLI_CTRLY|LIB$M_CLI_CTRLT; /* ^Y and ^T */
static	unsigned long old_msk;		/* Saved control mask		*/
static	short	trap_flag = FALSE;	/* TRUE if AST is set		*/
BOOLEAN DidCleanup = FALSE;		/* Exit handler flag		*/
static char VersionVMS[20];		/* Version of VMS		*/

PUBLIC int VMSVersion ARGS2(
	char *,		VerString,
	int,		VerLen)
{
     unsigned long status, itm_cod = SYI$_VERSION;
     int i, verlen = 0;
     struct dsc$descriptor version;
     char *m;

     version.dsc$a_pointer = VerString;
     version.dsc$w_length = VerLen - 1;
     version.dsc$b_dtype = DSC$K_DTYPE_B;
     version.dsc$b_class = DSC$K_CLASS_S;

     status = lib$getsyi(&itm_cod, 0, &version, &verlen, 0, 0);
     if (!(status&1) || verlen == 0)
	  return 0;

     /*
      *  Cut out trailing spaces
      */
     for (m = VerString+verlen, i = verlen-1; i > 0 && VerString[i] == ' '; --i)
	  *(--m) = '\0';

     return strlen(VerString)+1;	/* Transmit ending 0 too */
}

PUBLIC void VMSexit NOARGS
{
    /*
     *	If we get here and DidCleanup is not set, it was via an
     *	ACCVIO, or outofmemory forced exit, so make *sure* we
     *	attempt a cleanup and reset the terminal.
     */
    if (!DidCleanup) {
	if (LYOutOfMemory == FALSE) {
	    fprintf(stderr,
gettext("\nA Fatal error has occurred in %s Ver. %s\n"), LYNX_NAME, LYNX_VERSION);
	    fprintf(stderr,
gettext("\nPlease notify your system administrator to confirm a bug, and if\n\
confirmed, to notify the lynx-dev list.  Bug reports should have concise\n\
descriptions of the command and/or URL which causes the problem, the\n\
operating system name with version number, the TCPIP implementation, the\n\
TRACEBACK if it can be captured, and any other relevant information.\n"));

	    if (LYTraceLogFP == NULL) {
		fprintf(stderr,RETURN_TO_CLEANUP);
		(void) getchar();
	    }
	} else if (LYCursesON) {
	    HTAlert(MEMORY_EXHAUSTED_ABORT);
	}
	cleanup();
    }
    if (LYOutOfMemory == TRUE) {
	printf("\r\n%s\r\n\r\n", MEMORY_EXHAUSTED_ABORT);
	fflush(stdout);
	fflush(stderr);
    }
}

/*
 *	TTOPEN --
 *		This function is called once to set up the terminal
 *		device streams.  It translates TT until it finds
 *		the terminal, then assigns a channel to it, sets it
 *		to EDIT, and sets up the Ctrl-C and Ctrl-Y interrupt
 *		handling.
 */
PUBLIC int ttopen NOARGS
{
	extern	void cleanup_sig();
	int	iosb[2];
	int	status;
	static unsigned long condition;
	static struct _exit_block {
	    unsigned long forward;
	    unsigned long address;
	    unsigned long zero;
	    unsigned long condition;
	} exit_handler_block;

	status = sys$assign( &term_nam_dsc, &iochan, 0, 0 );
	if( status != SS$_NORMAL )
		exit( status );

	status = sys$qiow( EFN, iochan, IO$_SENSEMODE, &iosb, 0, 0,
			  &oldmode, sizeof(oldmode), 0, 0, 0, 0 );
	if( status != SS$_NORMAL )
		exit( status );

	status = iosb[0] & 0xFFFF;
	if( status != SS$_NORMAL )
		exit( status );

	newmode[0] = oldmode[0];
	newmode[1] = oldmode[1];
	newmode[2] = oldmode[2] | TT2$M_EDIT;

	status = sys$qiow( EFN, iochan, IO$_SETMODE, &iosb, 0, 0,
			  &newmode, sizeof(newmode), 0, 0, 0, 0 );
	if( status != SS$_NORMAL )
		exit( status );

	status = iosb[0] & 0xFFFF;
	if( status != SS$_NORMAL )
		exit( status );

	/*
	 *  Declare the exit handler block.
	 */
	exit_handler_block.forward   = 0;
	exit_handler_block.address   = (unsigned long) &VMSexit;
	exit_handler_block.zero      = 0;
	exit_handler_block.condition = (unsigned long) &condition;
	status = sys$dclexh(&exit_handler_block);
	if (status != SS$_NORMAL)
		exit( status );

	/*
	 *  Set the AST.
	 */
	lib$disable_ctrl(&mask, &old_msk);
	trap_flag = TRUE;
	status = sys$qiow ( EFN, iochan,
			    IO$_SETMODE|IO$M_CTRLCAST|IO$M_CTRLYAST,
			    &iosb, 0, 0,
			    &cleanup_sig, SIGINT, 0, 0, 0, 0 );
	if ( status != SS$_NORMAL ) {
		lib$enable_ctrl(&old_msk);
		exit ( status );
	}

	/*
	 *  Get the version of VMS.
	 */
	if (VMSVersion(VersionVMS, 20) < 3)
		/*
		 *  Load zeros on error.
		 */
		strcpy(VersionVMS, "V0.0-0");

	return(0);
}	/*  ttopen  */

/*
 *	TTCLOSE --
 *		This function gets called just before we go back home
 *		to the command interpreter.  It puts the terminal back
 *		in a reasonable state.
 */
PUBLIC int ttclose NOARGS
{
	int	status;
	int	iosb[1];

	status = sys$qiow( EFN, iochan, IO$_SETMODE, &iosb, 0, 0,
			  &oldmode, sizeof(oldmode), 0, 0, 0, 0 );

	if( status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL )
		exit( status );

	if (trap_flag) {
	    status = sys$dassgn (iochan);
	    status = lib$enable_ctrl(&old_msk);
	    trap_flag = FALSE;
	}
	return(0);
}	/*  ttclose  */

/*
 *	TTGETC --
 *		Read a character from the terminal, with NOECHO and NOFILTR.
 */
PUBLIC int ttgetc NOARGS
{
    int status;
    unsigned short iosb[4];

    if (in_pos < in_len)
	return(buffer[in_pos++]);

    status = sys$qiow(EFN, iochan,
		      IO$_READVBLK|IO$M_NOECHO|IO$M_NOFILTR,
		      &iosb, 0, 0,
		      &buffer, 1, 0, 0, 0, 0);
    if ((status&1) == 1)
	status = iosb[0];
    if (status == SS$_PARTESCAPE) {
	/*
	 *  Escape sequence in progress.  Fake a successful read.
	 */
	status = 1;
    }
    if ((status&1) != 1 && status != SS$_DATAOVERUN)
	exit(status);
    in_pos = 1;
    in_len = iosb[1] + iosb[3];
    return(buffer[0]);
}

/*
 *	TYPEAHEAD -- Fote Macrides 27-Jan-1994
 *		Check whether a keystroke has been entered, and return
 *		 it, or -1 if none was entered.
 */
PUBLIC int typeahead NOARGS
{
    int status;
    unsigned short iosb[4];

    if (dump_output_immediately)
	return -1;

    if (in_pos < in_len)
	return(buffer[in_pos++]);

again:
    status = sys$qiow (EFN, iochan,
		       IO$_READVBLK|IO$M_TIMED|IO$M_NOECHO|IO$M_NOFILTR,
		       &iosb, 0, 0,
		       &buffer, 1, 0, 0, 0, 0);
    if ((status&1) == 1)
	status = iosb[0];
    if (status == SS$_PARTESCAPE) {
	/*
	 *  Escape sequence in progress, finish reading it.
	 */
	goto again;
    }

    in_pos = 1;
    in_len = iosb[1] + iosb[3];
    if (status == SS$_TIMEOUT || status == SS$_DATAOVERUN)
	return(-1);
    return (buffer[0]);
}

/*
 *	VMSSIGNAL -- Fote Macrides 29-Jun-1993
 *		Sets up AST for both Ctrl-C and Ctrl-Y, with system response
 *		 to Ctrl-T disabled.  If called with a sig other than SIGINT,
 *		 it will use the C library's system(sig, func).
 *		The equivalent of VMSsignal(SIGINT, cleanup_sig) is done on
 *		 intialization by ttopen(), so don't do it again.
 *		VMSsignal(SIGINT, SIG_DFL) is treated as a call to ttclose().
 *		Call VMSsignal(SIGINT, SIG_IGN) before system() calls to
 *		 enable Ctrl-C and Ctrl-Y in the subprocess, and then call
 *		 VMSsignal(SIG_INT, cleanup_sig) on return from the subprocess.
 *		For func's which set flags and do not invoke an exit from
 *		 LYNX, the func should reassert itself.
 *		The VMS signal() calls do not fully emulate the Unix calls,
 *		 and VMSsignal() is just a "helper", also not a full emulation.
 */

PUBLIC void VMSsignal (sig,func)
int sig;
void (*func)();
{
	int status;
	short iosb[4];
	static int SIG_IGN_flag;

	/*
	 *  Pass all signals other than SIGINT to signal().
	 *  Also pass SIGINT to signal() if we're dumping.
	 */
	if (sig != SIGINT || dump_output_immediately) {
	    signal(sig, func);
	    return;
	}

	/*
	 *  If func is SIG_DFL, treat it as ttclose().
	 */
	if (func == SIG_DFL) {
	    ttclose();
	    return;
	}

	/*
	 *  Clear any previous AST.
	 */
	if (trap_flag) {
	    status = sys$dassgn (iochan);
	    status = lib$enable_ctrl(&old_msk);
	    trap_flag = FALSE;
	}

	/*
	 *  If func is SIG_IGN, leave the TT channel closed and the
	 *  system response to interrupts enabled for system() calls.
	 */
	if (func == SIG_IGN)
	    return;

	/*
	 *  If we get to here, we have a LYNX func, so set the AST.
	 */
	lib$disable_ctrl(&mask, &old_msk);
	trap_flag = TRUE;
	status = sys$assign (&term_nam_dsc, &iochan, 0, 0 );
	status = sys$qiow ( EFN, iochan,
			    IO$_SETMODE|IO$M_CTRLCAST|IO$M_CTRLYAST,
			    &iosb, 0, 0,
			    func, SIGINT, 0, 0, 0, 0 );

}	/* VMSsignal */

/*
 *  DCLspawn_exception, spawn_DCLprocess, DCLsystem -- F.Macrides 16-Jan-1994
 *	Exception-handler routines for regulating interrupts and enabling
 *	Control-T during spawns.  Includes TRUSTED flag for versions of VMS
 *	which require it in captive accounts.  This code should be used
 *	instead of the VAXC or DECC system(), by including LYUtils.h in
 *	modules which have system() calls.  It helps ensure that we return
 *	to Lynx instead of breaking out to DCL if a user issues interrupts
 *	or generates an ACCVIO during spawns.
 */
#ifdef __DECC
PRIVATE unsigned int DCLspawn_exception ARGS2(
	void *,		sigarr,
	void *,		mecharr)
#else
PRIVATE int DCLspawn_exception ARGS2(
	void *,		sigarr,
	void *,		mecharr)
#endif /* __DECC */
{
     int status;

     status = lib$sig_to_ret(sigarr, mecharr);
     return(SS$_UNWIND);
}

PRIVATE int spawn_DCLprocess ARGS1(
	char *,		command)
{
     int status;
     unsigned long Status = 0;
     /*
      *  Keep DECC from complaining.
      */
     struct dsc$descriptor_s  command_desc;
     command_desc.dsc$w_length	= strlen(command);
     command_desc.dsc$b_class	= DSC$K_CLASS_S;
     command_desc.dsc$b_dtype	= DSC$K_DTYPE_T;
     command_desc.dsc$a_pointer = command;

     VAXC$ESTABLISH(DCLspawn_exception);

#ifdef __ALPHA /** OpenVMS/AXP lacked the TRUSTED flag before v6.1 **/
     if (VersionVMS[1] > '6' ||
	 (VersionVMS[1] == '6' && VersionVMS[2] == '.' &&
	  VersionVMS[3] >= '1'))
#else
     if (VersionVMS[1] >= '6')
#endif /* __ALPHA */
     {
	 /*
	  *  Include TRUSTED flag.
	  */
	 unsigned long trusted = CLI$M_TRUSTED;
	 status = lib$spawn(&command_desc,0,0,&trusted,
			    0,0,&Status);
	 /*
	  *  If it was invalid, try again without the flag.
	  */
	 if (status == LIB$_INVARG)
	    status = lib$spawn(&command_desc,0,0,0,
			       0,0,&Status );
     } else
	 status = lib$spawn(&command_desc,0,0,0,
			    0,0,&Status);
     /*
      *  Return -1 on error.
      */
     if ((status&1) != 1 || (Status&1) != 1)
	 return(-1);
     /*
      *  Return 0 on success.
      */
     return(0);
}

PUBLIC int DCLsystem ARGS1(
	char *,		command)
{
     int status;
     extern void controlc();

     VMSsignal(SIGINT, SIG_IGN);
     status = spawn_DCLprocess(command);
     VMSsignal(SIGINT, cleanup_sig);
     /*
      *  Returns 0 on success, -1 any error.
      */
     return(status);
}
#endif /* VMS */

PUBLIC void lynx_force_repaint NOARGS
{
#if defined(COLOR_CURSES)
    chtype a;
    if (LYShowColor >= SHOW_COLOR_ON)
	a = COLOR_BKGD;
    else
	a = A_NORMAL;
    bkgdset(a | ' ');
#if !defined(USE_COLOR_STYLE) && defined(NCURSES_VERSION)
#if NCURSES_VERSION_MAJOR < 4 || (NCURSES_VERSION_MAJOR == 4 && NCURSES_VERSION_MINOR == 0)
    bkgd(a | ' ');
#endif
#endif
    attrset(a);
#endif /* COLOR_CURSES */
    clearok(curscr, TRUE);
}

PUBLIC void lynx_start_title_color NOARGS
{
#ifdef SH_EX
    start_reverse();
#endif
}

PUBLIC void lynx_stop_title_color NOARGS
{
#ifdef SH_EX
    stop_reverse();
#endif
}

PUBLIC void lynx_start_link_color ARGS2(
	int,	flag,
	int,	pending)
{
    if (flag) {
	/* makes some terminals work wrong because
	 * they can't handle two attributes at the
	 * same time
	 */
	/* start_bold();  */
	start_reverse();
#if defined(USE_SLANG)
#ifndef __DJGPP__
	if (SLtt_Use_Ansi_Colors)
#endif /* !__DJGPP__ */
	    start_underline ();
#endif /* USE_SLANG */
#if defined(FANCY_CURSES) && defined(COLOR_CURSES)
	if (lynx_has_color && LYShowColor >= SHOW_COLOR_ON)
	    start_underline ();
#endif /* USE_SLANG */
     } else {
	start_bold();
	/*
	 *  Make sure when flag is OFF that "unhighlighted" links
	 *  will be underlined if appropriate. - LE & FM
	 */
	if (pending)
	    start_underline();
     }
}

PUBLIC void lynx_stop_link_color ARGS2(
	int,	flag,
	int,	pending GCC_UNUSED)
{
#ifdef USE_COLOR_STYLE
    LynxChangeStyle(flag == ON ? s_alink : s_a, ABS_OFF, 0);
#else
    if (flag) {
	stop_reverse();
#if defined(USE_SLANG)
#ifndef __DJGPP__
	if (SLtt_Use_Ansi_Colors)
#endif /* !__DJGPP__ */
	stop_underline ();
#endif /* USE_SLANG */
#if defined(FANCY_CURSES) && defined(COLOR_CURSES)
	if (lynx_has_color && LYShowColor >= SHOW_COLOR_ON)
	    stop_underline ();
#endif /* FANCY_CURSES && COLOR_CURSES */
    } else {
	stop_bold();
	/*
	 *  If underlining was turned on above, turn it off. - LE & FM
	 */
	if (pending)
	    stop_underline();
    }
#endif
}


PUBLIC void lynx_stop_target_color NOARGS
{
   stop_underline();
   stop_reverse();
   stop_bold();
}

PUBLIC void lynx_start_target_color NOARGS
{
   start_bold();
   start_reverse();
   start_underline();
}


PUBLIC void lynx_start_status_color NOARGS
{
#if USE_COLOR_TABLE && defined(COLOR_CURSES)
    if (lynx_has_color && LYShowColor >= SHOW_COLOR_ON)
	lynx_set_color (2);
    else
#endif
	start_reverse ();
}

PUBLIC void lynx_stop_status_color NOARGS
{
#if USE_COLOR_TABLE && defined(COLOR_CURSES)
    if (lynx_has_color && LYShowColor >= SHOW_COLOR_ON)
	lynx_set_color (0);
    else
#endif
	stop_reverse ();
}

PUBLIC void lynx_start_h1_color NOARGS
{
   if (bold_H1 || bold_headers)
     start_bold();
}

PUBLIC void lynx_stop_h1_color NOARGS
{
   if (bold_H1 || bold_headers)
     stop_bold();
}

PUBLIC void lynx_start_prompt_color NOARGS
{
   start_reverse ();
}

PUBLIC void lynx_stop_prompt_color NOARGS
{
   stop_reverse ();
}

PUBLIC void lynx_start_radio_color NOARGS
{
   start_bold ();
}

PUBLIC void lynx_stop_radio_color NOARGS
{
   stop_bold ();
}

PUBLIC void lynx_stop_all_colors NOARGS
{
   stop_underline ();
   stop_reverse ();
   stop_bold ();
}

/*
 * If LYShowCursor is ON, move the cursor to the left of the current option, so
 * that blind users, who are most likely to have LYShowCursor ON, will have
 * it's string spoken or passed to the braille interface as each option is made
 * current.  Otherwise, move it to the bottom, right column of the screen, to
 * "hide" the cursor as for the main document, and let sighted users rely on
 * the current option's highlighting or color without the distraction of a
 * blinking cursor in the window.  - FM
 */
PUBLIC void LYstowCursor ARGS3(
    WINDOW *,	win,
    int,	row,
    int,	col)
{
#ifdef USE_SLANG
    if (LYShowCursor)
	SLsmg_gotorc(win->top_y + row, win->left_x + col);
    else
	LYHideCursor();
    SLsmg_refresh();
#else
    if (LYShowCursor)
	wmove(win, row, col);
    else
	LYHideCursor();
    wrefresh(win);
#endif /* USE_SLANG  */
}
