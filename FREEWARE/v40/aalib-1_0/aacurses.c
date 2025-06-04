#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include "config.h"
#ifdef CURSES_DRIVER
#ifdef USE_NCURSES
#include <ncurses.h>
#else
#include <curses.h>
#ifdef VMS
#include "vms_curses.h"
#endif
#endif
#ifdef GPM_MOUSEDRIVER
#include <gpm.h>
#endif
#include "aalib.h"
struct aa_driver curses_d;
int __curses_is_up = 0;
int __resized_curses = 0;
static int uninitcurses;

static int curses_init(struct aa_hardware_params *p, void *none)
{
    int a;
    if (!__curses_is_up) {
	fflush(stdout);
	if ((initscr()) == NULL)
	    return 0;
	__curses_is_up = 1;
	uninitcurses = 1;
    }
#ifdef HAVE_TERMATTRS
    a = termattrs();
    if (a & A_DIM)
	curses_d.params.supported |= AA_DIM_MASK;
    if (a & A_BOLD)
	curses_d.params.supported |= AA_BOLD_MASK;
    if (a & A_BOLD)
	curses_d.params.supported |= AA_BOLDFONT_MASK;
    if (a & A_REVERSE)
	curses_d.params.supported |= AA_REVERSE_MASK;
#else
    curses_d.params.supported |= AA_REVERSE_MASK | AA_DIM_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK;
#endif
#ifndef VMS
    intrflush(stdscr, FALSE);
#endif
    aa_recommendlowkbd("curses");
    return 1;
}
static void curses_uninit(aa_context * c)
{
    wclear(stdscr);
#ifndef VMS
    intrflush(stdscr, TRUE);
#endif
#if 0
#ifndef VMS
    nocbreak();
#endif
    echo();
    nl();
#endif
    refresh();
    if (uninitcurses) {
	uninitcurses = 0;
	__curses_is_up = 0;
	endwin();
    }
}
static void curses_getsize(aa_context * c, int *width, int *height)
{
    if (__resized_curses)
	curses_uninit(c), curses_init(&c->params, NULL), __resized_curses = 0;
    *width = stdscr->_maxx + 1;
    *height = stdscr->_maxy + 1;
#ifdef GPM_MOUSEDRIVER
    gpm_mx = *width;
    gpm_my = *height;
#endif

}
static void curses_setattr(aa_context * c, int attr)
{
    switch (attr) {
    case AA_NORMAL:
	attrset(A_NORMAL);
	break;
    case AA_DIM:
	attrset(A_DIM);
	break;
    case AA_BOLD:
	attrset(A_BOLD);
	break;
    case AA_BOLDFONT:
	attrset(A_BOLD);
	break;
    case AA_REVERSE:
	attrset(A_REVERSE);
	break;
    case AA_SPECIAL:
	attrset(A_REVERSE);
	break;
    }
}
static void curses_print(aa_context * c, char *text)
{
    addstr(text);
}
static void curses_flush(aa_context * c)
{
    refresh();
}
static void curses_gotoxy(aa_context * c, int x, int y)
{
    move(y, x);
}
static void curses_cursor(aa_context * c, int mode)
{
#ifdef HAVE_CURS_SET
    curs_set(mode);
#endif
}



struct aa_driver curses_d =
{
    "curses", "Curses driver 1.0",
    {NULL, AA_NORMAL_MASK},
    curses_init,
    curses_uninit,
    curses_getsize,
    curses_setattr,
    curses_print,
    NULL,
    curses_gotoxy,
    /*curses_getchar,
       NULL, */
    curses_flush,
    curses_cursor,
};
#endif
