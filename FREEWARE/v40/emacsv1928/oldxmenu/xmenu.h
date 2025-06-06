#include "copyright.h"

/* $Header: /u/src/emacs/19.0/oldXMenu/RCS/XMenu.h,v 1.1 1992/04/11 22:10:21 jimb Exp $ */
/* Copyright    Massachusetts Institute of Technology    1985	*/

/*
 * XMenu:	MIT Project Athena, X Window system menu package
 *
 *	XMenu.h -	Include file for the MIT Project Athena
 *			XMenu X window system menu package.
 *			
 *	Author:		Tony Della Fera, DEC
 *			August, 1984
 */

#ifndef _XMenu_h_
#define _XMenu_h_

#include <X11/Xutil.h>
#include "X10.h"

#define FAILURE			-1
#define SUCCESS			1
#define POST_ERROR		-1
#define NO_SELECTION		-1

#define XM_FAILURE		-1
#define XM_SUCCESS		1
#define XM_NO_SELECT		2
#define XM_IA_SELECT		3

#define XME_CODE_COUNT		17

#define XME_NO_ERROR		0
#define XME_NOT_INIT		1
#define XME_ARG_BOUNDS		2
#define XME_P_NOT_FOUND		3
#define XME_S_NOT_FOUND		4
#define XME_STYLE_PARAM		5
#define XME_GRAB_MOUSE		6
#define XME_INTERP_LOC		7
#define XME_CALLOC		8
#define XME_CREATE_ASSOC	9
#define XME_STORE_BITMAP	10
#define XME_MAKE_TILES		11
#define XME_MAKE_PIXMAP		12
#define XME_CREATE_CURSOR	13
#define XME_OPEN_FONT		14
#define XME_CREATE_WINDOW	15
#define XME_CREATE_TRANSP	16

/*
 * XMenu error code and error list definitions.
 */
extern int _XMErrorCode;
extern char *_XMErrorList[];

/*
 * Define the XMWindow datatypes.
 *
 * An XMWindow is either an XMPane or an XMSelect.
 *
 * XMWindow is wrapper used to identify the constant window
 * information that makes up XMPane and XMSelect objects.
 *
 * An XMPane is a menu pane made up of one or more XMSelect and a label.
 *
 * An XMSelect is a menu selection object with a label and a data pointer.
 */
typedef enum _xmwintype {PANE, SELECTION, PL_HEADER, SL_HEADER} XMWType;

typedef struct _xmwindow {
    struct _xmwindow *next;	/* Next obj pointer (for emacs_insque). */
    struct _xmwindow *prev;	/* Prev obj pointer (for emacs_insque). */
    XMWType type;		/* Type of window. */
    Window window;		/* X Window Id. */
    int window_x;		/* Window upper left X coordinate. */
    int window_y;		/* Window upper left y coordinate. */
    int window_w;		/* Window width. */
    int window_h;		/* Window height. */
    int active;			/* Window active? */
    int activated;		/* Window activated? */
    int pad_l1;			/* ---- */
    char *pad_l2;		/* ---- */
    int pad_l3;			/* ---- */
    int pad_l4;			/* ---- */
    int pad_l5;			/* ---- */
    int pad_l6;			/* ---- */
    int pad_l7;			/* ---- */
    int pad_l8;			/* ---- */
    struct _xmwindow *pad_l9;	/* ---- */
    char *pad_l10;		/* ---- */
    struct _xmwindow *pad_l11;	/* ---- */
} XMWindow;

typedef struct _xmpane {
    struct _xmpane *next;	/* Next obj pointer (for emacs_insque). */
    struct _xmpane *prev;	/* Prev obj pointer (for emacs_insque). */
    XMWType type;		/* Type of window. */
    Window window;		/* X Window Id. */
    int window_x;		/* Window upper left X coordinate. */
    int window_y;		/* Window upper left y coordinate. */
    int window_w;		/* Window width. */
    int window_h;		/* Window height. */
    int active;			/* Window active? */
    int activated;		/* Window activated? */
    int serial;			/* -- Pane serial number. */
    char *label;		/* -- Pane label. */
    int label_width;		/* -- Pane label width in pixels. */
    int label_length;		/* -- Pane label length in chars. */
    int label_x;		/* -- Pane label X offset. */
    int label_uy;		/* -- Pane label upper Y offset. */
    int label_ly;		/* -- Pane label lower Y offset. */
    int s_count;		/* -- Selections in this pane. */
    struct _xmselect *s_list;	/* -- Selection  window list. */
    char *pad_l10;		/* ---- */
    struct _xmwindow *pad_l11;	/* ---- */
} XMPane;

typedef struct _xmselect {
    struct _xmselect *next;	/* Next obj pointer (for emacs_insque). */
    struct _xmselect *prev;	/* Prev obj pointer (for emacs_insque). */
    XMWType type;		/* Type of window. */
    Window window;		/* X Window Id. */
    Window parent;		/* X Window id of parent window. */
    int window_x;		/* Window upper left X coordinate. */
    int window_y;		/* Window upper left y coordinate. */
    int window_w;		/* Window width. */
    int window_h;		/* Window height. */
    int active;			/* Window active? */
    int activated;		/* Window activated? */
    int serial;			/* -- Selection serial number. */
    char *label;		/* -- Selection label. */
    int label_width;		/* -- Selection label width in pixels. */
    int label_length;		/* -- Selection label length in chars. */
    int label_x;		/* -- Selection label X offset. */
    int label_y;		/* -- Selection label Y offset. */
    int pad_l7;			/* ---- */
    int pad_l8;			/* ---- */
    struct _xmwindow *pad_l9;	/* ---- */
    char *data;			/* -- Selection data pointer. */
    struct _xmpane *parent_p;	/* -- Selection parent pane structure. */
} XMSelect;


/*
 * Define the XMStyle datatype.
 *
 * Menu presentation style information.
 *
 */
typedef enum _xmstyle {
    LEFT,			/* Left oriented obejct. */
    RIGHT,			/* Right oriented obejct. */
    CENTER			/* Center oriented object. */
} XMStyle;


/*
 * Define the XMMode datatype.
 *
 * Menu presentation mode information.
 *
 */
typedef enum _xmmode {
    BOX,			/* BOXed graphic rendition. */
    INVERT			/* INVERTed graphic rendition. */
} XMMode;


/* 
 * Define the XMenu datatype. 
 *
 * All dimensions are in pixels unless otherwise noted.
 */
typedef struct _xmenu {
    /* -------------------- Menu data -------------------- */
    XMStyle menu_style;		/* Menu display style. */
    XMMode menu_mode;		/* Menu display mode. */
    int freeze;			/* Freeze server mode? */
    int aeq;			/* Asynchronous Event Queueing mode? */
    int recompute;		/* Recompute menu dependencies? */
    Window parent;		/* Menu's parent window. */
    int width;			/* Overall menu width. */
    int height;			/* Overall menu height. */
    int x_pos;			/* Oveall menu origin. */
    int y_pos;			/* Overall menu origin. */
    Cursor mouse_cursor;	/* Mouse cursor raster. */
    XAssocTable *assoc_tab;	/* XMWindow association table. */
    XMPane *p_list;		/* List of XMPanes. */
    /* -------------------- Pane window data -------------------- */
    XMStyle p_style;		/* Pane display style. */
    int p_events;		/* Pane window X events. */
    XFontStruct *p_fnt_info;	/* Flag font info structure. */
    GC pane_GC;			/* Pane graphics context. */
    int p_fnt_pad;		/* Fixed flag font padding in pixels. */
    double p_spread;		/* Pane spread in flag height fractions. */
    int p_bdr_width;		/* Pane border width. */
    int flag_height;		/* Flag height. */
    int p_width;		/* Menu pane width. */
    int p_height;		/* Menu pane height. */
    int p_x_off;		/* Pane window X offset. */
    int p_y_off;		/* Pane window Y offset. */
    int p_count;		/* Number of panes per menu. */
    /* -------------------- Selection window data -------------------- */
    XMStyle s_style;		/* Selection display style. */
    int s_events;		/* Selection window X events. */
    XFontStruct *s_fnt_info;	/* Body font info structure. */
    int s_fnt_pad;		/* Fixed body font padding in pixels. */
    double s_spread;		/* Select spread in line height fractions. */
    int s_bdr_width;		/* Select border width. */
    int s_width;		/* Selection window width. */
    int s_height;		/* Selection window height. */
    int s_x_off;		/* Selection window X offset. */
    int s_y_off;		/* Selection window Y offset. */
    int s_count;		/* Maximum number of selections per pane. */
    GC normal_select_GC;	/* GC used for inactive selections. */
    GC inverse_select_GC;	/* GC used for active (current) selection. */  
    GC inact_GC;		/* GC used for inactive selections and */
				/* panes headers. */
    /* -------------------- Color data -------------------- */
    unsigned long p_bdr_color;	/* Color of pane border pixmap. */
    unsigned long s_bdr_color;	/* Color of selection border pixmap. */
    unsigned long p_frg_color;	/* Color of pane foreground pixmap. */
    unsigned long s_frg_color;	/* Color of selection pixmap. */
    unsigned long bkgnd_color;	/* Color of menu background pixmap. */
    /* -------------------- Pixmap data -------------------- */
    Pixmap p_bdr_pixmap;	/* Pane border pixmap. */
    Pixmap s_bdr_pixmap;	/* Selection border pixmap. */
    Pixmap p_frg_pixmap;	/* Pane foreground pixmap. */
    Pixmap s_frg_pixmap;	/* Selection foreground pixmap. */
    Pixmap bkgnd_pixmap;	/* Menu background pixmap. */
    Pixmap inact_pixmap;	/* Menu inactive pixmap. */
} XMenu;

/*
 * XMenu library routine declarations.
 */
XMenu *XMenuCreate();
int XMenuAddPane();
int XMenuAddSelection();
int XMenuInsertPane();
int XMenuInsertSelection();
int XMenuFindPane();
int XMenuFindSelection();
int XMenuChangePane();
int XMenuChangeSelection();
int XMenuSetPane();
int XMenuSetSelection();
int XMenuRecompute();
int XMenuEventHandler();	/* No value actually returned. */
int XMenuLocate();
int XMenuSetFreeze();		/* No value actually returned. */
int XMenuActivate();
char *XMenuPost();
int XMenuDeletePane();
int XMenuDeleteSelection();
int XMenuDestroy();		/* No value actually returned. */
char *XMenuError();

#endif
/* Don't add after this point. */
