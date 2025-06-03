/* ************************************************************************* *
   xfsm - (C) Copyright 1993 Robert Gasch (rgasch@nl.oracle.com)

   Permission to use, copy, modify and distribute this software for any 
   purpose and without fee is hereby granted, provided that this copyright
   notice appear in all copies as well as supporting documentation. All
   work developed as a consequence of the use of this program should duly
   acknowledge such use.

   No representations are made about the suitability of this software for
   any purpose. This software is provided "as is" without express or implied 
   warranty.

   All commercial uses of xfsm must be done by agreement with the autor.
 * ************************************************************************* */



/* ****** include files ****** */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/bitmaps/gray1>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#if defined (SUNOS) || defined (TOS) || defined (LINUX)
#include <sys/vfs.h>
# else
# ifdef SVR4
# include <sys/statvfs.h>
#  else
#  ifdef AIX
#  include <sys/statfs.h>
#  include <sys/select.h>		/* required for msleep */
#  endif
# endif
#endif


/* ******************************************************************** */
/* ******************** general program constants ********************* */
/* ******************************************************************** */
#define TRUE		1
#define FALSE		0
#define NOGOOD		-1
#define MAXCPL          80
#define MAXFS           50
#define SLEEPT		50000L
#define QUIT_KEY	'q'
#define UPDATE_KEY	'u'
#define TOGGLE_KEY	't'
#define GET_HOSTNAME	"hostname"
#define MAX_SLEEP_SCALE	50
#define MILLION		1000000
#define MB		1048576

#ifdef SVR4
#define GET_FS		"/etc/mount | cut -f1 -d\" \""
# else
# if defined (SUNOS) || defined (TOS) || defined (LINUX)
# define GET_FS		"/etc/mount | cut -f3 -d\" \""
# else
#  ifdef AIX
#  define GET_FS	"/etc/mount |cut -f10 -d\" \""
#  endif
# endif
#endif


#if defined (SVR4) || defined (AIX)
#define DETAIL_NUM	10
#else
# if defined (SUNOS) || defined (LINUX)
# define DETAIL_NUM	9
# else
#  ifdef TOS
#  define DETAIL_NUM	7
#  endif
# endif
#endif



#define HELPARG		strcmp (argv[i], "-?") == NULL || \
			strcmp (argv[i], "help") == NULL || \
			strcmp (argv[i], "-help") == NULL

#define CHECKNA		if (strcmp (s, "-1") == NULL) strcpy (s, "N.A.")

#define PRINT_KNOWN_BUGS 	printf ("\n\
Known Bugs: \n\
	1) When a detailed information window is opened it does not \n\
	   receive the expose event until the mouse reenters the main window.\n\
	2) Update field does not receive initial expose event until mouse\n\
	   enters window.\n");

#define PRINT_COPYRIGHT		printf ("\
xfsm v1.23 - (C) Copyright 1993 Robert Gasch (rgasch@nl.oracle.com)\n");



/* ****** define window constants - these are positions and sizes ****** */
#define WIN_X 	150
#define WIN_Y 	150
#define MIN_WIN_X	100
#define MENU_HEIGHT 	15
#define MIN_MENU_HEIGHT	4
#define LETTER_HEIGHT	12
#define LETTER_SPACE	15
#define FREE_LETTER_SP	(LETTER_SPACE-LETTER_HEIGHT)
#define NFS_TEXT_Y	(fs_win[i].y-(LETTER_SPACE-(LETTER_HEIGHT+1)))
#define SM_MENU_WIDTH	80
#define MENU_WIDTH 	110
#define MENU_ITEMS	2
#define MENU_SPACE	(MENU_HEIGHT*(MENU_ITEMS+2))
#define BEGIN_NFS	MENU_SPACE
#define MENU_Y		10
#define OFF_X		10
#define OFF_Y		10
#define INTERVAL	((MENU_HEIGHT+FREE_LETTER_SP)*2) 
#define	MIN_INTERVAL	(MIN_MENU_HEIGHT+FREE_LETTER_SP+MENU_HEIGHT)
#define DETAIL_X	195
#define DETAIL_Y	((MENU_HEIGHT*(DETAIL_NUM+3))+(OFF_X*2))
#define DPC 		6 	/* Dots per character */
/* ****** define constants used to identify menus and windows(buttons) ****** */
#define UPDATE		0
#define	QUIT		1



/* ******************************************************************** */
/* ********************* program data structures ********************** */
/* ******************************************************************** */

/* ****** XWindow struct - this simplifies function calls ****** */
typedef struct {
	Window 		win;		/* window ID */
	GC		gc;		/* window graphics content */
	char		text[40];	/* title (for menus) */
	int		x, y, 		/* position */
			width, height, 	/* size */
			line_thick;	/* line thickness of window border */
	unsigned long	fg, bg;		/* foreground and background */
	long		event_mask, 	/* which events will be registered */ 
			flags;		/* window flags */
		} WinType, *WinTypePtr;

/* *** string to hold file system name *** */
typedef char string[MAXCPL];


/* ******************************************************************** */
/* ********************** function declarations *********************** */
/* ******************************************************************** */
void	do_event_loop();
void	toggle_mode();
void	redraw_main_win();
void 	write_detail();
void 	write_percent();
void 	redraw_fs_win();
void	get_fs_stat();
void 	create_window ();
int	highlight_menu();
int	expose_win();
void	destroy_menu();
int	which_button_press();
void	msleep();
void 	init_all_windows();
