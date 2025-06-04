/********************************************************************
 * lindner
 * 3.18
 * 1994/05/02 07:40:16
 * /home/mudhoney/GopherSrc/CVS/gopher+/gopher/CURcurses.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: CURcurses.h
 * Header file access methods for CURcurses.c
 *********************************************************************
 * Revision History:
 * CURcurses.h,v
 * Revision 3.18  1994/05/02  07:40:16  lindner
 * Mods to use setlocale()
 *
 * Revision 3.17  1994/04/13  19:14:31  lindner
 * AskL modifications
 *
 * Revision 3.16  1994/04/07  17:25:54  lindner
 * Fix for pyramids
 *
 * Revision 3.15  1994/03/04  23:39:48  lindner
 * Fix for log entries
 *
 * Revision 3.14  1994/03/04  23:36:30  lindner
 * figure out whether the terminal can do alternate character set.
 *
 * Revision 3.13  1994/02/20  21:45:20  lindner
 * Better sanity checks for box drawing characters
 *
 * Revision 3.12  1994/02/20  16:49:20  lindner
 * Fix bug in gcc for Solaris 2.x for curses routines
 *
 * Revision 3.11  1993/12/28  17:28:52  lindner
 * Better method of drawing box characters
 *
 * Revision 3.10  1993/11/29  01:09:46  lindner
 * Don't use the alternate character set for character graphics under AIX.
 * They don't work very well in xterms or aixterms.  [However, it does work
 * if I use Mac NCSA Telnet to connect to our RS/6000....]  (Beckett)
 *
 * Revision 3.9  1993/11/02  06:21:30  lindner
 * Fix for osf curses
 *
 * Revision 3.8  1993/09/26  09:19:21  lindner
 * Add bold #defines
 *
 * Revision 3.7  1993/09/22  15:44:36  lindner
 * Fix for sysv systems without KEY_HELP
 *
 * Revision 3.6  1993/09/22  01:15:47  lindner
 * Add support for DEC HELP key/KEY_HELP
 *
 * Revision 3.5  1993/09/21  01:46:08  lindner
 * Implement all remaining ASK block items..
 *
 * Revision 3.4  1993/09/08  05:20:02  lindner
 * Fix for multiple CURcurses.h includes
 *
 * Revision 3.3  1993/07/30  17:31:39  lindner
 * Mods to support AskP:
 *
 * Revision 3.2  1993/04/15  21:25:20  lindner
 * Fixes for CURbox() definitions
 *
 * Revision 3.1.1.1  1993/02/11  18:02:56  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.2  1992/12/31  05:57:38  lindner
 * Mods for VMS
 *
 * Revision 1.1  1992/12/10  23:32:16  lindner
 * gopher 1.1 release
 *
 * Revision 1.1  1992/12/10  06:16:51  lindner
 * Initial revision
 *
 *
 *********************************************************************/

#ifndef CURCURSES_H
#define CURCURSES_H

#include "Locale.h"
#include <ctype.h>

#ifdef __svr4__
  /* Fix bug in gcc for Solaris 2.x */
#  define SYSV
#endif

#if defined(ultrix)
#  include <cursesX.h>
#else
#  include <curses.h>
#endif

#include "boolean.h"
#include "STRstring.h"

#ifdef KEY_RIGHT
#  define SYSVCURSES
#endif

struct CursesStruct {
     WINDOW *Screen;
     String *Termtype;

     /** Termcap/terminfo stuff **/
     String *Clearscreen;
     String *AudibleBell;
     String *Highlighton;
     String *Highlightoff;
     
     boolean canUseACS;
     boolean inCurses;
     
     int COLS;
     int ROWS;
     
     int Box_ul;
     int Box_ur;
     int Box_ll;
     int Box_lr;
     int Box_vline;
     int Box_hline;
     int Box_tt;
     int Box_bt;
     int Box_lt;
     int Box_rt;
     int Box_ct;

     void (*sigtstp)();
     void (*sigwinch)();
};

typedef struct CursesStruct CursesObj;
typedef struct CursesStruct *CursesObjp;

#ifndef SYSVCURSES
#  define KEY_DOWN        0402           /* The four arrow keys ...*/
#  define KEY_UP          0403
#  define KEY_LEFT        0404
#  define KEY_RIGHT       0405           
#  define KEY_NPAGE       0522           /* Next page */
#  define KEY_PPAGE       0523           /* Previous page */
#  define KEY_ENTER       0527
#  define KEY_BACKSPACE   0407
#  define KEY_HELP        0553            /* help key */
#endif

/*
 * Definitions for character graphics
 */

#if defined(SYSVCURSES) && !defined(ultrix) && !defined(hpux) && !defined(_AUX_SOURCE) && !defined(__osf__) && !defined(_AIX)
#  define CUR_FANCY_BOXES
#endif

#if defined(VMS) || defined(__convex__) || defined(sequent) || defined(pyr)
#if !defined(cbreak)
#define cbreak crmode
#endif
#endif

#ifndef KEY_HELP
#define KEY_HELP        0553            /* help key */
#endif

/*
 * Request types....  More to be added later..
 */

#define CUR_LABEL  1
#define CUR_PROMPT 2
#define CUR_PASSWD 3
#define CUR_CHOICE 4
#define CUR_ASKL   5

struct Requestitem_struct {
     char *prompt;
     char *stowage;
     int  thing;
     int  chooseitem;
     char **choices;
};

typedef struct Requestitem_struct Requestitem;

#define max(A, B) ((A) > (B) ? (A) : (B))
#define min(A, B) ((A) < (B) ? (A) : (B))

/*
 * access functions
 */

#define CURgetScreen(a)    ((a)->Screen)
#define CURgetTerm(a)      (STRget((a)->Termtype))
#define CURgetCLS(a)       (STRget((a)->Clearscreen))
#define CURgetBell(a)      (STRget((a)->AudibleBell))
#define CURgetHighon(a)    (STRget((a)->Highlighton))
#define CURgetHighoff(a)   (STRget((a)->Highlightoff))

#ifdef VMS
#define CURsetScreen(a,b)  ((b),(a)->Screen=stdscr)
#else
#define CURsetScreen(a,b)  ((a)->Screen=(b))
#endif
#define CURsetTerm(a,b)    (STRset((a)->Termtype, (b)))
#define CURsetCLS(a,b)     (STRset((a)->Clearscreen,(b)))
#define CURsetBell(a,b)    (STRset((a)->AudibleBell,(b)))
#define CURsetHighon(a,b)  (STRset((a)->Highlighton,(b)))
#define CURsetHighoff(a,b) (STRset((a)->Highlightoff,(b)))
#define CURsetSIGTSTP(a,b) ((a)->sigtstp=(b))
#define CURsetSIGWINCH(a,b) ((a)->sigwinch=(b))

#if defined(SYSVCURSES) || defined(A_BOLD)
#define wboldout(win)       (wattron(win, A_BOLD))
#define wboldend(win)       (wattroff(win, A_BOLD))
#else
#define wboldout(win)       (wstandout(win))
#define wboldend(win)       (wstandend(win))
#endif

#define CURgetBox_ul(a)  ((a)->Box_ul)
#define CURgetBox_ur(a)  ((a)->Box_ur)
#define CURgetBox_ll(a)  ((a)->Box_ll)
#define CURgetBox_lr(a)     ((a)->Box_lr)
#define CURgetBox_vline(a)  ((a)->Box_vline)
#define CURgetBox_hline(a)  ((a)->Box_hline)
#define CURgetBox_tt(a)     ((a)->Box_tt)
#define CURgetBox_bt(a)     ((a)->Box_bt)
#define CURgetBox_lt(a)     ((a)->Box_lt)
#define CURgetBox_rt(a)     ((a)->Box_rt)
#define CURgetBox_ct(a)     ((a)->Box_cr)


CursesObj *CURnew();
void      CURinit();
void      CURcenterline();
void      CURenter();
void      CURexit();
int       CURgetstr();
int       CURgetch();
void      CURresize();
int       CURoutchar();
int       CURGetOneOption();
void      CURBeep();
void      CURwenter();
void      CURsetGraphicsChars();

#endif  /* CURCURSES_H */
