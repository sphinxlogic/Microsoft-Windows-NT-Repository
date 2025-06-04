/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-pat.h				     */
/*									     */
/*	Copyright (C) 1993, 1994 by Heiko Eissfeldt and Michael Bischoff     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*	This file is included by all sources for the X interface	     */
/*									     */
/*									     */
/*****************************************************************************/

#if !defined(useXlib) && !defined(useXaw) && !defined(useMotif) && !defined(useXview)
#error "You must define one of useXlib, useXaw, useMotif, useXview"
#endif

#include "xpat.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#ifndef useXview
#include <X11/Intrinsic.h>
#endif

#ifdef useXaw
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>	
#include <X11/Xaw/Label.h>	
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Toggle.h>
#endif

#ifdef useMotif
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#endif

#ifdef useXview
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/cms.h>
#include <xview/canvas.h>
#include <xview/defaults.h>
#include <xview/svrimage.h>
#include <xview/server.h>
#include <xview/notice.h>
#include <xview/openmenu.h>
#endif

typedef unsigned int XSize_t;   /* type used by X for width and height   */
                                /* this is not consistent used by X11R5  */
/* in X11R3, XSize_t was int, since R4 we seem to have a mixture of int  */
/* and unsigned int! (complain!)					 */

void resize_event(XSize_t w, XSize_t h);
#ifdef useXview
#define soft_Resize Force_Resize
#else
#ifdef useXlib
#define soft_Resize Force_Resize
#else
extern Widget toplevel;
void AskWidgetForResize(XSize_t x, XSize_t y);
#define soft_Resize AskWidgetForResize
#endif
#endif

void init_gfx(void);
void init_fonts(const char *, const char *);
void init_windows(int, char **, const char *, int, int, int, int,
   int, int, int, const char *);
void init_layout(void);
void generic_minwindow(XSize_t *, XSize_t *);
void init_cards(const char *, int, int, unsigned long, unsigned long, int, const char *);
void PaintCard(int, int, int, int);
void do_show_rules(void);
void init_exposed_card(void);
void show_exposed_card(boolean);
#ifdef useXview
void init_arrow(unsigned long, int, int);
#else
void init_arrow(const char *, int, int);
void handle_expose_event(XExposeEvent *);
#endif
void init_mark(unsigned long pixel, int markwidth);
void Force_Resize(XSize_t, XSize_t);

#ifdef useXview
void table_events(Xv_Window, Event *);
void table_resize(Canvas, int, int);
void table_repaint(Canvas, Xv_Window, Rectlist *);
void frame_resize(Xv_Window, Event *);
void frame_pack_all(Frame);
#endif
/* events called from Tableau widget or from Xlib */
void key_press(XKeyPressedEvent *xev);
void redraw_table(XExposeEvent *xev);
void button_press(XButtonPressedEvent *xev);
void button_release(XButtonPressedEvent *xev);
void mouse_motion(XPointerMovedEvent *xev);
int main(int, char **);


/* more abbrevs */

/* definition of a card: */
/* bits 0 and 1 give the color: 0 = diamond, 1 = heart, 2 = spades, 3 = club */
#define SUITSYMBOL	52	/* for empty stacks */
#define CARDBACK	56
#define OUTLINE         58
#define NOTHING         59

#define ROUND_W		(card.rx)
#define ROUND_H		(card.ry)
#define CARD_WIDTH	(card.w)
#define CARD_HEIGHT	(card.h)
#define STD_DELTA	(card.stddelta)

extern struct graphic {
    boolean is_color;           /* True, if more than one bit per pixel  */ 
    boolean autolayout;         /* automatic new layout at resize events */
    XSize_t width;              /* the width of the table window         */
    XSize_t height;             /* the height of the table window        */
    XSize_t xgap;		/* room between two cards horizontally   */
    XSize_t ygap;		/* room between two cards vertically     */
#ifndef useXlib
    XSize_t w_width;            /* the width of the Viewport window      */
    XSize_t w_height;           /* the height of the Viewport window     */
#endif
    XSize_t preferred_width;    /* the required width for the layout	 */
    XSize_t preferred_height;   /*  " height			         */
    XSize_t min_width;  	/* the required minwidth for these rules */
    XSize_t min_height;		/*  " height				 */
    struct pile {
	int x;			/* x position of this pile               */
	int y;			/* y position of this pile               */
	int xtotal;		/* current height of pile in pixels      */
	int ytotal;		/* current width of pile in pixels       */
	int xmaxwidth;		/* maximum width of this pile 		 */
	int ymaxheight;		/* maximum height of this pile 		 */
	int xdelta;		/* x-delta value of this pile (0)        */
	int delta;		/* delta value of this pile              */
    } pile[MAXPILES];
    int cardy[MAXCARDS];	/* y-offset of card relative to pile     */
    Cardindex zoomed_card;      /* -1 or the index of an exposed card    */
    int ya_w, ya_h, xa_w, xa_h;	/* arrow head dimension, set by init 	 */
    int aw, ah;			/* width and height of an arrow 	 */
#ifdef useXlib
    int message_x;		/* start of text */
    int message_y;
    XSize_t buttons_height;	/* the height of the buttons (non-Xaw)	 */
    XSize_t message_height;	/* the height of the message window (non-Xaw) */
    int yoff;			/* where the actual table starts         */
    XFontStruct *message_font;
    GC textgc;
#endif
} graphic;

#define NOT_DISPLAYED(p)       ((p)->x == -1 && (p)->y == -1)

extern struct card {
    const char *cardset;        /* name of file for cards, or NULL for internals */
    int rx;			/* round edge, x */
    int ry;			/* round edge, y */
    XSize_t w;			/* width of a card */
    XSize_t h;			/* height of a card */
    int stddelta;		/* standard delta */
    int back_delta_x;		/* from spider */
    int back_delta_y;		/* (how much to modify the TS origin by) */
} card;


#ifdef useXview
extern Window table;
extern Frame mainwindow;
extern Panel buttonbox;
extern Canvas canvas;
extern Menu cmenu;
extern Event *bp_event;
#else
extern Window table, mainwindow;
#endif

extern Display *dpy;
extern int screen;
extern unsigned long blackpixel;
extern unsigned long whitepixel;
extern GC blackgc;
extern GC whitegc;

#ifdef useXlib
#define	TABLE_BW	2
#define MAXBUTTONS	32

extern struct buttons {
    int num;
    int rx;			/* radius of round corner, x */
    int ry;			/* radius of round corner, y */
    XSize_t bx;			/* border size, x */
    XSize_t by;			/* border size, y */
    XFontStruct *font;
    int fontheight;		/* ascend + descend */
    GC gc;
    struct singlebutton {
	int x;
	int y;
        XSize_t w;
        XSize_t h;
        const char *text;
        void (*left)(void);
        void (*middle)(void);
        void (*right)(void);
    } b[MAXBUTTONS];
} button;

extern struct singlebutton confirmbuttons[3];
void (*check_button_list(struct singlebutton *p, int num,
    XButtonPressedEvent *xev))(void);
void event_loop(void);

#define B_ROUND_W	(button.rx)
#define B_ROUND_H	(button.ry)

int arrange_buttons(struct singlebutton *, int *, int *, XSize_t, long *);
void redraw_buttons(int, int, int, int, Window, int, struct singlebutton *);

extern struct confirm {
    const char *text;
    int x, y;
    XSize_t w, h;
    Window win;
} confirm;

extern Window finished_win;
#define FINISHED_W	100
#define FINISHED_H	 50

/* endif */

#endif
