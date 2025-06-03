static char *RcsID = "$Id: ctw.c,v 1.1 1993/02/13 18:39:38 rfs Exp rfs $";
/*
 * $Log: ctw.c,v $
 * Revision 1.1  1993/02/13  18:39:38  rfs
 * Initial revision
 *
 *
*/

/**********************************************************************/
/*                                                                    */
/*  File:          ctw.c                                              */
/*  Author:        P. D. Fox                                          */
/*  Created:       25 Nov 1991                     		      */
/*                                                                    */
/*  Copyright (c) 1990, 1991, 1992 Paul Fox                           */
/*                All Rights Reserved.                                */
/*                                                                    */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*  Description:  Color terminal widget.                              */
/*                                                                    */
/*   Sequences supported:					      */
/*   								      */
/*   ESC 7	Save cursor position    			      */
/*   ESC 8	Restore cursor position 			      */
/*   ESC c	Reset terminal					      */
/*   ESC D	Index (down cursor or scroll).   	      	      */
/*   ESC M	Reverse index (up cursor or insline).   	      */
/*   ESC Z	Identify terminal.      			      */
/*   ESC >	Numeric keypad					      */
/*   ESC =	Application keypad      			      */
/*   ^H		Backspace					      */
/*   ^I		Tabs (fixed-width -- 8 chars wide).		      */
/*   ^J		Line feed					      */
/*   ^L		Clear screen.					      */
/*   ^M 	Carriage return 				      */
/*   ESC ( 0  	Graphics char set.      			      */
/*   ESC ) 0  	Graphics char set.      			      */
/*   ESC ( B  	Normal character set.   			      */
/*   ESC [ 0m	Reset attributes.                      	              */
/*   ESC [ 1m	Bold                                   	              */
/*   ESC [ 4m	Underline                              	              */
/*   ESC [ 7m	Reverse video.                         	              */
/*   ESC [ 3Xm	Set foreground color.                  	      	      */
/*   ESC [ 4Xm	Set background color.                  	      	      */
/*   ESC [ n @	Insert characters.      			      */
/*   ESC [ n A	Move cursor up					      */
/*   ESC [ n B	Move cursor down				      */
/*   ESC [ n C	Move cursor right       			      */
/*   ESC [ n D	Move cursor left				      */
/*   ESC [ n ; m H						      */
/*		Move cursor to absolute position.		      */
/*   ESC [ n L	Insert lines at current row     		      */
/*   ESC [ n M	Delete lines at current row     		      */
/*   ESC [ n ; m r	Set scrolling region.			      */
/*   ESC [ n P	Delete characters.              		      */
/*   ESC [ S	Index (down cursor or scroll).   	      	      */
/*   ESC [ T	Reverse index (up cursor or insline).   	      */
/*   ESC [ n X	Erase to blank (no cursor move) 		      */
/*   ESC [ n g	Print ASCII char 'n' (SCO mode only)    	      */
/*   ESC [n;m r	Set scrolling region to lines n..m.     	      */
/*   ESC [ n t	Shelltool/cmdtool compatable escape sequence          */
/*   		ESC [1t	Open window.				      */
/*   		ESC [2t	Iconise window.				      */
/*   		ESC [3t	Move window.				      */
/*   		ESC [4t	Change size (in pixels) window.		      */
/*   		ESC [5t	Raise window.				      */
/*   		ESC [6t	Lower window.				      */
/*   								      */
/*   ESC [ ? 1 h	Application cursor keys			      */
/*   ESC [ ? 4 h	Set smooth scroll mode.			      */
/*   ESC [ ? 7 h	Set autowrap           			      */
/*   ESC [ ? 47 h	Use alternate screen buffer.           	      */
/*   ESC [ ? 1962 h	Turn on application mouse.             	      */
/*   								      */
/*   ESC [ ? 1 l	Normal cursor keys			      */
/*   ESC [ ? 4 l	Set jump scroll mode.			      */
/*   ESC [ ? 7 l	Disable autowrap       			      */
/*   ESC [ ? 47 l	Use normal screen buffer.              	      */
/*   ESC [ ? 1962 l     Turn off application mouse.            	      */
/*   								      */
/*   ESC [ ? r;c;h;w;n S Scroll rectangular region up or down n lines */
/*   								      */
/*   								      */
/*   Sequences reported:					      */
/*     ESC [ b; m; s; r; c M  Mouse position report   		      */
/*   			b == Button being reported (1..3)	      */
/*   			s == 0 button pressed  			      */
/*   			  == 1 button released 			      */
/*   			  == 2 button motion   			      */
/*   								      */
/*   			m == Modifier status as a set of bits         */
/*   			     0x01 Shift down    		      */
/*   			     0x02 Ctrl down     		      */
/*   			     0x04 Meta down     		      */
/*   								      */
/*   			r == current row (decimal 0..n)		      */
/*   			c == current col (decimal 0..n)		      */
/*   								      */
/**********************************************************************/

/*static char sccs_id[] = "%Z% %M% %R%.%L%";*/

/*# include 	<X11/copyright.h>*/
# include 	<X11/IntrinsicP.h>
# include 	<X11/StringDefs.h>
# include 	<X11/Shell.h>
# include	<X11/Xatom.h>
# include	<signal.h>
# include 	"ctwP.h"
/**********************************************************************/
/*   Following  is  used  if  we  want  to  use my version of malloc  */
/*   checking.							      */
/**********************************************************************/
# if 0
# 	include	<chkalloc.h>
# else
#	define	chk_alloc	malloc
#	define	chk_free	free
# endif
# include	<stdio.h>
# include	<X11/keysym.h>
# include	<ctype.h>
# include	<memory.h>
# include	<stdlib.h>
# include	<string.h>

# define	ROW_TO_PIXEL(w, r)	((r) * w->ctw.font_height + w->ctw.fontp->ascent)
# define	BLACK	0
# define	WHITE	7

# define	MAX_ARGS	32

/**********************************************************************/
/*   Default size of window if too small or not defined.	      */
/**********************************************************************/
# define	DEFAULT_HEIGHT	(13 * 24)
# define	DEFAULT_WIDTH	(7 * 80)
# define	DEFAULT_FONT	XtDefaultFont

static int 	default_rows = 24;
static int 	default_columns = 80;
static int	default_max_lines = 512;
static Boolean	defaultFALSE = FALSE;
static int	default_flashrate = 500;
static int	default_multiClickTime = 250;

static XtResource resources[] = {
#define offset(field) XtOffset(CtwWidget, ctw.field)
	/* {name, class, type, size, offset, default_type, default_addr}, */
	{ XtNfont, XtCFont, XtRString, sizeof(char*),
	  offset(font), XtRString, "7x13bold"},
	{ XtNgeometry, XtCGeometry, XtRString, sizeof(char*),
	  offset(geometry), XtRString, ""},
	{ XtNcursorColor, XtCCursorColor, XtRPixel, sizeof(unsigned long),
	  offset(cursor_color), XtRString, "red"},
	{ XtNhiliteBackground, XtCHiliteBackground, XtRPixel, sizeof(unsigned long),
	  offset(hilite_bg), XtRString, "CadetBlue"},
	{ XtNhiliteForeground, XtCHiliteForeground, XtRPixel, sizeof(unsigned long),
	  offset(hilite_fg), XtRString, "yellow"},
	{ XtNrows, XtCRows, XtRInt, sizeof(int),
	  offset(rows), XtRInt, (char *) &default_rows},
	{ XtNcolumns, XtCColumns, XtRInt, sizeof(int),
	  offset(columns), XtRInt, (char *) &default_columns},
	{ XtNkbdCallback, XtCKbdCallback, XtRCallback, sizeof(caddr_t),
	  offset(kbd_callback), XtRCallback, (caddr_t) NULL},
	{ XtNresizeCallback, XtCResizeCallback, XtRCallback, sizeof(caddr_t),
	  offset(resize_callback), XtRCallback, (caddr_t) NULL},
	{ XtNtopCallback, XtCTopCallback, XtRCallback, sizeof(caddr_t),
	  offset(top_callback), XtRCallback, (caddr_t) NULL},
	{ XtNmouseCallback, XtCMouseCallback, XtRCallback, sizeof(caddr_t),
	  offset(mouse_callback), XtRCallback, (caddr_t) NULL},
	{ XtNapplCallback, XtCApplCallback, XtRCallback, sizeof(caddr_t),
	  offset(appl_callback), XtRCallback, (caddr_t) NULL},
	{ XtNtranslations, XtCTranslations, XtRTranslationTable, sizeof(char*),
	  offset(table), XtRString, ""},
	{ XtNsunFunctionKeys, XtCSunFunctionKeys, XtRBoolean, sizeof(Boolean),
	  offset(sun_function_keys), XtRBoolean, &defaultFALSE},
	{ XtNsaveLines, XtCSaveLines, XtRInt, sizeof(int),
	  offset(max_lines), XtRInt, (char *) &default_max_lines},
	{ XtNflashrate, XtCFlashrate, XtRInt, sizeof(int),
	  offset(flashrate), XtRInt, (char *) &default_flashrate},
	{ XtNfont1, XtCFont1, XtRString, sizeof(char *),
	  offset(font1), XtRString, "5x8"},
	{ XtNfont2, XtCFont2, XtRString, sizeof(char *),
	  offset(font2), XtRString, "6x9"},
	{ XtNfont3, XtCFont3, XtRString, sizeof(char *),
	  offset(font3), XtRString, "6x10"},
	{ XtNfont4, XtCFont4, XtRString, sizeof(char *),
	  offset(font4), XtRString, "6x13"},
	{ XtNfont5, XtCFont5, XtRString, sizeof(char *),
	  offset(font5), XtRString, "7x13bold"},
	{ XtNfont6, XtCFont6, XtRString, sizeof(char *),
	  offset(font6), XtRString, "8x13bold"},
	{ XtNmultiClickTime, XtCMultiClickTime, XtRInt, sizeof(int),
	  offset(multiClickTime), XtRInt, (char *) &default_multiClickTime},
#undef offset
};

static void	initialize();
static void	realize();
static void	Destroy();
static void	Resize();
static void	redisplay();
static Boolean	Set_values();
void	HandleFocusChange();

static void CtwInput(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwSelectStart(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwSelectExtend(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwSelectEnd(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwString(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwExpose(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwButtonDown(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwButtonUp(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwButtonMotion(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwInsertSelection(/* Widget, XEvent*, String*, Cardinal* */);

static void	application_mouse PROTO((CtwWidget, int, int, XEvent *, int, int));
static void	ctw_add_string2 PROTO((CtwWidget, char *, int));
static int	compute_length PROTO((CtwWidget, int, int, int));
static void	cursor_flash_proc();
static void	convert_click PROTO((CtwWidget));
static void	reset_screen PROTO((CtwWidget));
static void	alloc_screen PROTO((CtwWidget, int));
static int	setup_x11_colors PROTO((Display *));
static void	exposed_region PROTO((CtwWidget, int, int, int, int));
static void	update_region PROTO((CtwWidget, int, int, int, int));
static void	print_string PROTO((CtwWidget, int, int, int, vbyte_t *));
static void	show_cursor PROTO((CtwWidget));
static void	draw_string PROTO((CtwWidget, int, int, char *, int, Pixel, Pixel, int));
static void	draw_line PROTO((CtwWidget, int, int, unsigned char *, int, Pixel, Pixel, int));
static int	reset_font PROTO((CtwWidget, int));
static void	get_xy PROTO((CtwWidget, int *, int *, int, int));
static int	sunfuncvalue PROTO((int));
static int	funcvalue PROTO((int));
static char	*handle_escape PROTO((CtwWidget, char *, char *));
static void	do_rev_index PROTO((CtwWidget));
static void	do_index PROTO((CtwWidget));
static void	scroll_up PROTO((CtwWidget, int));
static void	scroll_rectangle PROTO((CtwWidget, int, int, int, int, int));
static void	scroll_up_local  PROTO((CtwWidget, int, int));
static void	blank_line PROTO((CtwWidget, vbyte_t *));
static void	clear_lines PROTO((CtwWidget, int, int));
static void	delete_line PROTO((CtwWidget, int, int));
static void	insert_line PROTO((CtwWidget, int, int, int));
static void	wait_for_exposure PROTO((CtwWidget));
static int	process_escape PROTO((CtwWidget));
static void	send_input PROTO((CtwWidget, char *, int));
static void	scroll_down PROTO((CtwWidget, int, int));
static void	turn_off_cursor PROTO((CtwWidget));
static void	turn_on_cursor PROTO((CtwWidget));
static void	toggle_cursor();
static void	send_str PROTO((CtwWidget, char *, int));
static int	do_quest PROTO((CtwWidget, int, int, int *));
static int	do_escequ PROTO((CtwWidget, int, int, int *));
static int	do_text_parms PROTO((Widget, char *));
static void	down_line PROTO((CtwWidget, char *));
static void	clear_to_eol PROTO((CtwWidget));
static void	clear_from_beginning PROTO((CtwWidget));
static void	clear_screen PROTO((CtwWidget));
static void	hilite PROTO((CtwWidget, int, int, int, int, int));
static Boolean convert_proc();
static Atom 	FetchAtom();
static void lose_selection();
static void	requestor_callback();


static XtActionsRec actions[] = {
  /* {name, procedure}, */
    {"input",		CtwInput},
    {"expose",		CtwExpose},
    {"string",		CtwString},
/* not good or cool for xod
    {"select-start",	CtwSelectStart},
    {"select-extend",	CtwSelectExtend},
    {"select-end",	CtwSelectEnd},
    {"insert-selection",CtwInsertSelection},
*/
    {"button_motion",	CtwButtonMotion},
    {"button_down",	CtwButtonDown},
    {"button_up",	CtwButtonUp},
};
/*
<Btn1Down>:		select-start()\n\
<Btn1Motion>:		select-extend()\n\
<Btn1Up>:		select-end(PRIMARY, CUT_BUFFER0)\n\
*/
static char translations[] =
"\
<KeyPress>:		input()\n\
<Expose>:		expose()\n\
<GraphicsExpose>:	expose()\n\
<Btn1Down>:		button_down()\n\
<Btn1Up>:		button_up()\n\
<Btn2Up>:		button_up()\n\
<Btn2Down>:		button_down()\n\
<Btn3Up>:		button_up()\n\
<Btn3Down>:		button_down()\n\
<BtnMotion>:		button_motion()\n";

CtwClassRec ctwClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Ctw",
    /* widget_size		*/	sizeof(CtwRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	FALSE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	redisplay,
    /* set_values		*/	Set_values,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	XtInheritAcceptFocus,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* ctw fields */
    /* empty			*/	0
  }
};

WidgetClass ctwWidgetClass = (WidgetClass)&ctwClassRec;

/**********************************************************************/
/*   Array of colors corresponding to the ANSI.			      */
/**********************************************************************/
static Pixel x11_colors[16];

/**********************************************************************/
/*   Array  of  characters  used  to  map  from  PC character set to  */
/*   line-drawing character set.				      */
/**********************************************************************/
static char pc_chars[256];

/**********************************************************************/
/*   Index  array  by  char  value  to  see  whether we need special  */
/*   processing.						      */
/**********************************************************************/
static unsigned char	parse_tbl[256];

/* ARGSUSED */
static void
initialize(Widget treq, Widget tnew) {
	CtwWidget	new = (CtwWidget) tnew;
	Display	*dpy = XtDisplay(new);

	/***********************************************/
	/*   Initialise the parse table.	       */
	/***********************************************/
	parse_tbl['\0'] = 1;
	parse_tbl[BEL] = 1;
	parse_tbl['\b'] = 1;
	parse_tbl['\t'] = 1;
	parse_tbl['\r'] = 1;
	parse_tbl['\n'] = 1;
	parse_tbl['n' & 0x1f] = 1;
	parse_tbl['o' & 0x1f] = 1;
	parse_tbl[ESC] = 1;
	parse_tbl[XON] = 1;
	parse_tbl[XOFF] = 1;

	if (setup_x11_colors(dpy) == FALSE) {
		/***********************************************/
		/*   Monochrome screen.			       */
		/***********************************************/
		new->ctw.cursor_color = x11_colors[7];
		}

	/***********************************************/
	/*   Set  up  the  PC  charcter  set  mapping  */
	/*   matrix.				       */
	/***********************************************/
	pc_chars[213] = 'l';
	pc_chars[184] = 'k';
	pc_chars[212] = 'm';
	pc_chars[190] = 'j';
	pc_chars[179] = 'x';
	pc_chars[0xcd] = 'q';
	pc_chars[0xd1] = 'w';
	pc_chars[0xcf] = 'v';
	pc_chars[0xd8] = 'n';
	pc_chars[0xb5] = 'u';
	pc_chars[0xc6] = 't';

	if (new->core.height < 10)
		new->core.height = DEFAULT_HEIGHT;
	if (new->core.width < 10)
		new->core.width = DEFAULT_WIDTH;
	new->ctw.win_height_allocated = new->core.height;
	new->ctw.win_width_allocated = new->core.width;
	/***********************************************/
	/*   Set   initial   position  and  state  of  */
	/*   cursor.				       */
	/***********************************************/
	new->ctw.x = new->ctw.y = 0;
	new->ctw.cursor_visible = TRUE;	

	if ((new->ctw.fontp = XLoadQueryFont(XtDisplay(new), new->ctw.font)) != NULL) {
		new->ctw.font_height = new->ctw.fontp->ascent + 
				 new->ctw.fontp->descent;
		new->ctw.font_width = XTextWidth(new->ctw.fontp, "A", 1);
		new->core.height = new->ctw.rows * new->ctw.font_height;
		new->core.width = new->ctw.columns * new->ctw.font_width;
		new->ctw.win_height_allocated = new->core.height;
		new->ctw.win_width_allocated = new->core.width;
		}
	/***********************************************/
	/*   Look  for  focus  related  events on the  */
	/*   parent     widget.     We     want    to  */
	/*   enable/disable      flashing      cursor  */
	/*   depending   on   whether   we  have  the  */
	/*   keyboard focus or not.		       */
	/***********************************************/
	XtAddEventHandler(XtParent(new), FocusChangeMask, FALSE,
		HandleFocusChange, new);
}
/**********************************************************************/
/*   Try  and  keep  track of whether we have the focus or not so we  */
/*   can avoid flashing cursor when its not our responsibility.	      */
/**********************************************************************/
void
HandleFocusChange(Widget widget, CtwWidget w, XFocusChangeEvent *event) {
	if (event->type == FocusIn) {
		w->ctw.have_focus = TRUE;
		/***********************************************/
		/*   If timer isn't running then restart it.   */
		/***********************************************/
		if (w->ctw.cursor_timer == 0 && w->ctw.flashrate)
			cursor_flash_proc(w, (XtIntervalId *) NULL);
		}
	else {
		w->ctw.have_focus = FALSE;
		if (w->ctw.cursor_timer && w->ctw.flashrate) {
			XtRemoveTimeOut(w->ctw.cursor_timer);
			w->ctw.cursor_timer = 0;
			turn_on_cursor(w);
			}
		}
}
# define	superclass	(&widgetClassRec)
static void
realize(Widget w, XtValueMask *valueMask, XSetWindowAttributes *attributes) {
	CtwWidget	new = (CtwWidget) w;
	XGCValues	values;
	Arg	args[10];
	int	n;
	XSetWindowAttributes	win_attr;

	(*superclass->core_class.realize)(w, valueMask, attributes);
	/***********************************************/
	/*   Need  graphics  exposure  events because  */
	/*   we  support  the  insert and delete line  */
	/*   functionality, which uses XCopyArea.      */
	/***********************************************/
	values.graphics_exposures = TRUE;
	values.background = x11_colors[0];
	new->ctw.gc = XCreateGC(XtDisplay(new), XtWindow(new), 
		GCBackground | GCGraphicsExposures, &values);
	XSetWindowBackground(XtDisplay(w), XtWindow(w), x11_colors[0]);
	values.graphics_exposures = FALSE;
	values.background = new->ctw.cursor_color;
	new->ctw.cursor_gc = XCreateGC(XtDisplay(new), XtWindow(new), 
		GCBackground | GCGraphicsExposures, &values);
	new->ctw.line_gc = XCreateGC(XtDisplay(new), XtWindow(new), GCGraphicsExposures, &values);

	/***********************************************/
	/*   Make sure we have a sane window size.     */
	/***********************************************/
	if (new->ctw.rows < 0)
		new->ctw.rows = 1;
	if (new->ctw.columns < 0)
		new->ctw.columns = 1;
	reset_font((CtwWidget)w, FALSE);

	XSetFont(XtDisplay(new), new->ctw.gc, new->ctw.fontp->fid);
	XSetFont(XtDisplay(new), new->ctw.cursor_gc, new->ctw.fontp->fid);

	win_attr.bit_gravity = NorthWestGravity;
	XChangeWindowAttributes(XtDisplay(w), XtWindow(w),
		CWBitGravity, &win_attr);
	
	n = 0;
	XtSetArg(args[n], XtNinput, 1); n++;
	XtSetValues(w, args, n);

	n = 0;
	XtSetArg(args[n], XtNwidthInc, new->ctw.font_width); n++;
	XtSetArg(args[n], XtNheightInc, new->ctw.font_height); n++;
	XtSetValues(XtParent(w), args, n);

	new->ctw.old_top_line = -1;
	new->ctw.sel_string = (char *) NULL;
	alloc_screen(new, TRUE);

	new->ctw.flags[CTW_ERASE_BLACK] = TRUE;
	new->ctw.flags[CTW_CUT_NEWLINES] = TRUE;

	reset_screen(new);
	/***********************************************/
	/*   Timer for the flashing cursor.	       */
	/***********************************************/
	new->ctw.cursor_timer = 0;
	if (new->ctw.flashrate)
		new->ctw.cursor_timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget)new),
			(long) new->ctw.flashrate, cursor_flash_proc, new);
}
/**********************************************************************/
/*   Function called when widget is destroyed.			      */
/**********************************************************************/
static void
Destroy(CtwWidget w) {
	Display *dpy;

	dpy = XtDisplay(w);
	if (w->ctw.memory)
		chk_free((void *) w->ctw.memory);
	if (w->ctw.lines)
		chk_free((void *) w->ctw.lines);
	if (w->ctw.sel_string)
		chk_free((void *) w->ctw.sel_string);
	XFreeGC(dpy, w->ctw.gc);
	XFreeGC(dpy, w->ctw.cursor_gc);
	XFreeGC(dpy, w->ctw.line_gc);
	XtRemoveTimeOut(w->ctw.cursor_timer);

	XtRemoveEventHandler(XtParent(w), FocusChangeMask, FALSE,
		(XtEventHandler)HandleFocusChange, w);

}
/* ARGSUSED */
static void
Resize(CtwWidget w) {
	int	x, y;

	if (!XtIsRealized((Widget)w))
		return;
	/***********************************************/
	/*   Stop   get_xy()   from  stopping  window  */
	/*   from getting bigger.		       */
	/***********************************************/
	w->ctw.rows = 32767;
	w->ctw.columns = 32767;
	get_xy(w, &y, &x, w->core.width, w->core.height);
	if (y < 1)
		y = 1;
	if (x < 1)
		x = 1;
	w->ctw.rows = y;
	w->ctw.columns = x;
	w->ctw.scroll_top = 0;
	w->ctw.scroll_bot = y;

	alloc_screen(w, FALSE);
}
/**********************************************************************/
/*   Method   called  when  application  wants  to  change  resource  */
/*   values. Intercept font change.				      */
/**********************************************************************/
static Boolean
Set_values(CtwWidget cur, CtwWidget req, CtwWidget new, ArgList args, 
	Cardinal *num_args) {
	int	refresh_needed = FALSE;
	XFontStruct	*fp;
	Dimension	req_width, req_height;

	if (cur->ctw.font != new->ctw.font && 
	    strcmp(cur->ctw.font, new->ctw.font) != 0) {
		if ((fp = XLoadQueryFont(XtDisplay(new), new->ctw.font)) != NULL) {
			new->ctw.fontp = fp;
			reset_font(new, FALSE);
			XSetFont(XtDisplay(new), new->ctw.gc, fp->fid);
			XSetFont(XtDisplay(new), new->ctw.cursor_gc, fp->fid);
			refresh_needed = TRUE;
			/***********************************************/
			/*   Try and resize our window.		       */
			/***********************************************/
			req_width = new->ctw.columns * new->ctw.font_width;
			req_height = new->ctw.rows * new->ctw.font_height;
			}
		}
	return refresh_needed;
}
/**********************************************************************/
/*   Xterm compatable string action routine.			      */
/**********************************************************************/
static void 
CtwString(Widget w, XEvent *event, String *x, Cardinal *y) {
	register int	i;
	register char	*cp;
	ctw_callback_t	reason;

	reason.reason = CTW_INPUT;
	for (i = 0; i < *y; i++) {
		cp = x[i];
		reason.ptr = cp;
		reason.len = strlen(cp);
		XtCallCallbacks((Widget)w, XtNkbdCallback, (caddr_t) &reason);
		}
}
/**********************************************************************/
/*   Xterm compatable routine for making a selection.		      */
/**********************************************************************/
static void 
CtwSelectStart(CtwWidget w, XEvent *event, String *x, Cardinal *y) {
	int	r, c;
	ctw_callback_t	reason;

	if ((unsigned long) ((long) event->xbutton.time - w->ctw.timestamp) >
		w->ctw.multiClickTime)
		w->ctw.num_clicks = 0;
	else
		w->ctw.num_clicks = (w->ctw.num_clicks + 1) & 3;
	w->ctw.timestamp = event->xbutton.time;

	/***********************************************/
	/*   Unhilite  any  previous selection before  */
	/*   starting the new one.		       */
	/***********************************************/
	if (w->ctw.sel_string) {
		if (w->ctw.sel_start_y >= 0)
			hilite(w, w->ctw.sel_start_x, w->ctw.sel_start_y,
				w->ctw.sel_cur_x, w->ctw.sel_cur_y, FALSE);
		chk_free((void *) w->ctw.sel_string);
		w->ctw.sel_string = (char *) NULL;
		}

	get_xy(w, &r, &c, event->xbutton.x, event->xbutton.y);

	w->ctw.sel_start_x = c;
	w->ctw.sel_start_y = w->ctw.top_line + r;

	w->ctw.sel_cur_x = w->ctw.sel_start_x;
	w->ctw.sel_cur_y = w->ctw.sel_start_y;

	convert_click(w);

	/***********************************************/
	/*   Turn  off  cursor  and stop the flashing  */
	/*   code from turning it back on.	       */
	/***********************************************/
	turn_off_cursor(w);
	w->ctw.cursor_state = CURSOR_HIDDEN;

	/***********************************************/
	/*   Tell  user  that we're doing a selection  */
	/*   so  its  best not to keep adding text to  */
	/*   the window to avoid scrolling problems.   */
	/***********************************************/
	reason.reason = CTW_SELECTION;
	XtCallCallbacks((Widget)w, XtNmouseCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Routine   to  convert  co-ordinates  got  via  a  button  click  */
/*   depending on how many clicks were typed.			      */
/**********************************************************************/
static void
convert_click(CtwWidget w) {
	if (w->ctw.num_clicks == CLICK_LINE) {
		w->ctw.sel_start_x = 0;
		w->ctw.sel_cur_x = w->ctw.columns;
		hilite(w, 
			w->ctw.sel_start_x, w->ctw.sel_start_y,
			w->ctw.sel_cur_x, w->ctw.sel_cur_y, FALSE);
		}
}
/**********************************************************************/
/*   Xterm compatable routine for making a selection.		      */
/**********************************************************************/
static void 
CtwSelectExtend(CtwWidget w, XEvent *event, String *str, Cardinal *num) {
	int	x, y;
	int	start, cur, here;

	get_xy(w, &y, &x, event->xbutton.x, event->xbutton.y);
	y += w->ctw.top_line;
	if (w->ctw.num_clicks == CLICK_LINE) {
		x = w->ctw.columns;
		}

	start = w->ctw.sel_start_y * w->ctw.columns + w->ctw.sel_start_x;
	cur = w->ctw.sel_cur_y * w->ctw.columns + w->ctw.sel_cur_x;
	here = y * w->ctw.columns + x;
	/***********************************************/
	/*   Work   out  which  regions  need  to  be  */
	/*   hilighted  and  make  the  display  look  */
	/*   correct.				       */
	/***********************************************/
	if (cur < start) {
		if (here < cur) {
			hilite(w, x, y, w->ctw.sel_cur_x, w->ctw.sel_cur_y, TRUE);
			}
		else if (here < start) {
			hilite(w, w->ctw.sel_cur_x, w->ctw.sel_cur_y, x, y, FALSE);
			}
		else {
			hilite(w, w->ctw.sel_cur_x, w->ctw.sel_cur_y, 
				w->ctw.sel_start_x, w->ctw.sel_start_y, FALSE);
			hilite(w, w->ctw.sel_start_x, w->ctw.sel_start_y, x, y, TRUE);
			}
		}
	else if (here >= cur) {
		if (cur < start)
			hilite(w, w->ctw.sel_cur_x, w->ctw.sel_cur_y, 
				w->ctw.sel_start_x, w->ctw.sel_start_y, FALSE);
		hilite(w, w->ctw.sel_cur_x, w->ctw.sel_cur_y, x, y, TRUE);
		}
	else if (here >= start && here < cur) {
		hilite(w, x, y, w->ctw.sel_cur_x, w->ctw.sel_cur_y, FALSE);
		}
	else if (here < start) {
		hilite(w, w->ctw.sel_start_x, w->ctw.sel_start_y, 
			w->ctw.sel_cur_x, w->ctw.sel_cur_y, FALSE);
		hilite(w, x, y, w->ctw.sel_start_x, w->ctw.sel_start_y, TRUE);
		}
	w->ctw.sel_cur_x = x;
	w->ctw.sel_cur_y = y;
}
/**********************************************************************/
/*   Routine to hilite a marked area.				      */
/**********************************************************************/
static void
hilite(CtwWidget w, int x1, int y1, int x2, int y2, int set_flag) {
	int	start_x = x1;
	int	end_x, x;
	int	y = y1;
	vbyte_t	*vp;

	if (y < 0)
		return;

	while (y <= y2) {
		if (y == y2)
			end_x = x2;
		else
			end_x = w->ctw.columns;
		vp = &w->ctw.lines[y][start_x];
		x = start_x;
		if (set_flag) {
			while (start_x++ < end_x) {
				vp->vb_attr |= VB_SELECT;
				vp++;
				}
			}
		else {
			while (start_x++ < end_x) {
				vp->vb_attr &= ~VB_SELECT;
				vp++;
				}
			}
		update_region(w, y - w->ctw.top_line, x, 
			y+1 - w->ctw.top_line, end_x+1);
		start_x = 0;
		y++;
		}
}
/**********************************************************************/
/*   Function  to  compute  the  length  of text in a line ready for  */
/*   cutting.							      */
/**********************************************************************/
static int
compute_length(CtwWidget w, int row, int start_col, int end_col) {
	vbyte_t	*vp;
	vbyte_t	*svp;

	vp = w->ctw.lines[row] + end_col;
	svp = w->ctw.lines[row] + start_col;
	while (vp > svp) {
		if (vp->vb_byte != ' ')
			break;
		vp--;
		}
	return (vp - svp) + 1;
}
/**********************************************************************/
/*   Xterm compatable routine for making a selection.		      */
/**********************************************************************/
static void 
CtwSelectEnd(CtwWidget w, XEvent *event, String *str, Cardinal *num) {
	int	r, t, len;
	int	y, x, end_x;
	vbyte_t	*vp;
	int	cur;
	int	start;
	char	*cp;
	int	nl;
	Atom	atom;

	start = w->ctw.sel_start_y * w->ctw.columns + w->ctw.sel_start_x;
	cur = w->ctw.sel_cur_y * w->ctw.columns + w->ctw.sel_cur_x;
	if (cur < start) {
		t = w->ctw.sel_start_x;
		w->ctw.sel_start_x = w->ctw.sel_cur_x;
		w->ctw.sel_cur_x = t;

		t = w->ctw.sel_start_y;
		w->ctw.sel_start_y = w->ctw.sel_cur_y;
		w->ctw.sel_cur_y = t;

		start = w->ctw.sel_start_y * w->ctw.columns + w->ctw.sel_start_x;
		cur = w->ctw.sel_cur_y * w->ctw.columns + w->ctw.sel_cur_x;
		}

	/***********************************************/
	/*   Free up any previous selection.	       */
	/***********************************************/
	if (w->ctw.sel_string) {
		chk_free((void *) w->ctw.sel_string);
		}
	/***********************************************/
	/*   Work  out  how  much  room is needed for  */
	/*   copy.  We  may  allocate  too  much  but  */
	/*   thats better than too little.	       */
	/***********************************************/
	if (w->ctw.sel_start_y == w->ctw.sel_cur_y) {
		len = compute_length(w, w->ctw.sel_start_y, w->ctw.sel_start_x, w->ctw.sel_cur_x);
		}
	else {
		len = compute_length(w, w->ctw.sel_start_y, w->ctw.sel_start_x, w->ctw.columns);
		for (r = w->ctw.sel_start_y + 1; r < w->ctw.sel_cur_y; r++)
			len += compute_length(w, r, 0, w->ctw.columns);
		len += compute_length(w, 0, w->ctw.sel_cur_x, w->ctw.columns);
		}
	w->ctw.sel_length = len;
	w->ctw.sel_string = (char *) chk_alloc(len);
	cp = w->ctw.sel_string;
	y = w->ctw.sel_start_y;
	x = w->ctw.sel_start_x;
	nl = '\0';
	while (y <= w->ctw.sel_cur_y) {
		if (y == w->ctw.sel_cur_y)
			end_x = w->ctw.sel_cur_x - 1;
		else {
			end_x = w->ctw.columns - 1;
			}
		vp = &w->ctw.lines[y][x];
		/***********************************************/
		/*   Remove trailing spaces.		       */
		/***********************************************/
		len = compute_length(w, y, x, end_x);
		if (len != end_x - x + 1)
			nl = '\n';
		end_x = x + len - 1;
		while (x++ <= end_x) {
			*cp++ = vp->vb_byte;
			vp++;
			}
		if (nl)
			*cp++ = nl;
		y++;
		x = 0;
		if (w->ctw.flags[CTW_CUT_NEWLINES])
			nl = '\n';
		}
	*cp = '\0';
	/***********************************************/
	/*   Turn cursor back on.		       */
	/***********************************************/
	w->ctw.cursor_state = CURSOR_OFF;
	turn_on_cursor(w);

	/***********************************************/
	/*   Try and grab ownership of selection.      */
	/***********************************************/
	atom = XA_PRIMARY;
	if (*num > 0 && strcmp(*str, "SECONDARY") == 0)
		atom = XA_SECONDARY;
	if (XtOwnSelection((Widget)w, atom, event->xbutton.time,
		convert_proc, lose_selection, NULL) == FALSE) {
		XBell(XtDisplay(w), 100);
		}
}
static Atom
FetchAtom(Widget w, String name) {
	Atom	a;
	XrmValue	source, dest;

	source.size = strlen(name) + 1;
	source.addr = name;
	dest.size = sizeof(Atom);
	dest.addr = (caddr_t) &a;

	XtConvertAndStore(w, XtRString, &source, XtRAtom, &dest);
	return a;
}
/**********************************************************************/
/*   Function   called   when   some  other  application  wants  the  */
/*   selection.							      */
/**********************************************************************/
static Boolean
convert_proc(CtwWidget w, Atom *selection, Atom *target, Atom *type, 
	XtPointer *value, unsigned long *length, int *format) {
	static Atom	targets = 0;

	if (w->ctw.sel_string == (char *) NULL)
		return FALSE;

	if (targets == 0) {
		targets = FetchAtom((Widget)w, "TARGETS");
		}

	if (*target == targets) {
		*type = XA_ATOM;
		*value = (XtPointer) XtNew(Atom);
		*(Atom *) *value = XA_STRING;
		*length = 1;
		*format = 32;
		return TRUE;
		}

	if (*target == XA_STRING) {
		*type = XA_STRING;
		*value = (XtPointer) XtNewString(w->ctw.sel_string);
		*length = strlen(*value);
		*format = 8;
		return TRUE;
		}
	return FALSE;
}
/**********************************************************************/
/*   Function  called  when  we lose the selection. Just free up the  */
/*   allocated memory.						      */
/**********************************************************************/
static void
lose_selection(CtwWidget w, Atom *selection) {
	if (w->ctw.sel_string) {
		chk_free((void *) w->ctw.sel_string);
		w->ctw.sel_string = (char *) NULL;
		hilite(w, w->ctw.sel_start_x, w->ctw.sel_start_y,
			w->ctw.sel_cur_x, w->ctw.sel_cur_y, FALSE);
		w->ctw.sel_start_x = -1;
		}
}
/**********************************************************************/
/*   Function  to  grab  contents  of  selection  and give to owning  */
/*   application.						      */
/**********************************************************************/
static void 
CtwInsertSelection(Widget w, XEvent *event, String *x, Cardinal *y) {
	XtGetSelectionValue(w, XA_PRIMARY, XA_STRING, requestor_callback, 
		NULL, event->xbutton.time);
}
/* ARGSUSED */
static void
requestor_callback(Widget w, XtPointer client_data, Atom *selection, 
	Atom *type, XtPointer *value, unsigned long *length, int *format) {
	ctw_callback_t	reason;

	reason.reason = CTW_PASTE;

	if (value == NULL || (*value == NULL && *length == 0)) {
		return;
		}
	reason.ptr = (char *) value;
	reason.len = (int) *length;
	XtCallCallbacks((Widget)w, XtNkbdCallback, (caddr_t) &reason);

	/***********************************************/
	/*   Free it.				       */
	/***********************************************/
	XtFree((char *)value);
}
/**********************************************************************/
/*   Convert mouse button presses into pseudo keystrokes.	      */
/**********************************************************************/
static void 
CtwButtonDown(CtwWidget w, XEvent *event, String *x, Cardinal *y) {
	ctw_callback_t reason;

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(w, event->xbutton.state, 0, event, 
			event->xbutton.x,
			event->xbutton.y);
		return;
		}
	w->ctw.timestamp = event->xbutton.time;

	reason.reason = CTW_BUTTON_DOWN;
	reason.event = event;
	XtCallCallbacks((Widget)w, XtNmouseCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Function to handle the application mouse mode.		      */
/**********************************************************************/
static void
application_mouse(CtwWidget w, int state, int flag, XEvent *event, 
	int x, int y) {
	ctw_callback_t	reason;
	char	buf[64];
	int	r, c;
	int	m = 0;

	get_xy(w, &r, &c, x, y);
	/***********************************************/
	/*   Calculate modifer byte value.	       */
	/***********************************************/
	if (state & ShiftMask)
		m |= 0x01;
	if (state & ControlMask)
		m |= 0x02;
	if (state & (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask))
		m |= 0x04;

	sprintf(buf, "\033[%d;%d;%d;%d;%dM", 
		event->xbutton.button, 
		flag,
		m, r, c);
	reason.reason = CTW_INPUT;
	reason.ptr = buf;
	reason.len = strlen(buf);
	XtCallCallbacks((Widget)w, XtNkbdCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Event  handler  to  handle  the mouse moving whilst a button is  */
/*   held down.							      */
/**********************************************************************/
static void 
CtwButtonMotion(CtwWidget w, XEvent *event, String *x, Cardinal *y) {
	ctw_callback_t reason;

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(w, 
			event->xmotion.state & Button1Mask ? Button1 : 
			event->xmotion.state & Button2Mask ? Button2 : 
			event->xmotion.state & Button3Mask ? Button3 : 
			event->xmotion.state & Button4Mask ? Button4 : 
				       	Button5,
			2, event,
			event->xmotion.x,
			event->xmotion.y);
		return;
		}
	reason.reason = CTW_BUTTON_MOTION;
	reason.event = event;
	XtCallCallbacks((Widget)w, XtNmouseCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Convert mouse button presses into pseudo keystrokes.	      */
/**********************************************************************/
static void 
CtwButtonUp(CtwWidget w, XEvent *event, String *x, Cardinal *y) {
	ctw_callback_t reason;

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(w, event->xbutton.state, 1, event,
			event->xbutton.x,
			event->xbutton.y);
		return;
		}
	w->ctw.timestamp = event->xbutton.time;
	reason.reason = CTW_BUTTON_UP;
	reason.event = event;
	XtCallCallbacks((Widget)w, XtNmouseCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Come here on a keypress event.				      */
/**********************************************************************/
static void 
CtwInput(Widget w, XEvent *event, String *x, Cardinal *y) {
	CtwWidget	cw = (CtwWidget) w;
	char	buf[BUFSIZ];
	char	reply[BUFSIZ];
	char	*rp = reply;
	static XComposeStatus compose_status;
	KeySym	keysym;
	int	nbytes;
	ctw_callback_t	reason;
static char *kypd_num = " XXXXXXXX\tXXX\rXXXxxxxXXXXXXXXXXXXXXXXXXXXX*+,-.\\0123456789XXX=";
static char *kypd_apl = " ABCDEFGHIJKLMNOPQRSTUVWXYZ??????abcdefghijklmnopqrstuvwxyzXXX";
static char *cur = "DACB";

	reason.reason = CTW_INPUT;
	reason.len = 0;
	reason.ptr = reply;

	nbytes = XLookupString((XKeyEvent *) event, buf, sizeof buf,
		&keysym, &compose_status);
	if (IsPFKey(keysym)) {
		*rp++ = ESC;
		*rp++ = 'O';
		*rp = keysym - XK_KP_F1 + 'P';
		reason.len = 3;
		}
	else if (IsKeypadKey(keysym)) {
		if (cw->ctw.flags[CTW_APPL_KEYPAD]) {
			*rp++ = ESC;
			*rp++ = 'O';
			*rp = kypd_apl[keysym - XK_KP_Space];
			reason.len = 3;
			}
		else  {
			*rp = kypd_num[keysym - XK_KP_Space];
			reason.len = 1;
			}
		}
	else if (IsCursorKey(keysym) && keysym != XK_Prior && keysym != XK_Next) {
		*rp++ = ESC;
		if (cw->ctw.flags[CTW_CURSOR_KEYPAD])
			*rp++ = 'O';
		else
			*rp++ = '[';
		*rp = cur[keysym - XK_Left];
		reason.len = 3;
		}
	else if (IsFunctionKey(keysym) || IsMiscFunctionKey(keysym) ||
		keysym == XK_Prior || keysym == XK_Next) {
		*rp++ = ESC;
		*rp++ = '[';
		if (cw->ctw.sun_function_keys) {
			sprintf(rp, "%dz", sunfuncvalue(keysym));
			}
		else {
			sprintf(rp, "%d~", funcvalue(keysym));
			}
		rp += strlen(rp);
		reason.len = rp - reply;
		}
	else if (nbytes > 0) {
		reason.ptr = buf;
		reason.len = nbytes;
		}

	if (reason.len)
		XtCallCallbacks((Widget)w, XtNkbdCallback, (caddr_t) &reason);
}
static void
redisplay(Widget w, XExposeEvent *event) {
	exposed_region((CtwWidget)w, event->x, event->y, event->width, 
		event->height);
}
/**********************************************************************/
/*   Action routine to handle redrawing of window.		      */
/**********************************************************************/
static void 
CtwExpose(Widget w, XEvent *event, String *x, Cardinal *y) {
	CtwWidget	cw = (CtwWidget) w;

	if (!XtIsRealized(w) /*|| need_resize*/)
		return;
	/***********************************************/
	/*   If  window  has  changed  size,  then we  */
	/*   need  to  resize  it.  Also  we  need to  */
	/*   clear  any  garbage  which may be at the  */
	/*   bottom or right edge of the window.       */
	/***********************************************/
	if (cw->ctw.win_height_allocated != cw->core.height ||
	    cw->ctw.win_width_allocated != cw->core.width) {
/*		need_resize = TRUE;*/
		cw->ctw.win_height_allocated = cw->core.height;
		cw->ctw.win_width_allocated = cw->core.width;
		return;
		}
	redisplay(w, (XExposeEvent *)event);
}
int
ctw_set_font(Widget w, char *font_name) {
	XFontStruct	*font;
	CtwWidget	cw = (CtwWidget) w;
	Display	*dpy = XtDisplay(cw);
	Arg	args[20];
	int	n;
		
	if ((font = XLoadQueryFont(dpy, font_name)) == NULL)
		return -1;
	cw->ctw.fontp = font;
	XSetFont(dpy, cw->ctw.gc, font->fid);

	reset_font((CtwWidget)w, TRUE);

	n = 0;
	XtSetArg(args[n], XtNwidthInc, cw->ctw.font_width); n++;
	XtSetArg(args[n], XtNheightInc, cw->ctw.font_height); n++;
	XtSetValues(XtParent(cw), args, n);

	return 0;
}
/**********************************************************************/
/*   Common code to handle a change in the font.		      */
/**********************************************************************/
static int
reset_font(CtwWidget w, int delete_flag) {
	w->ctw.font_height = w->ctw.fontp->ascent + w->ctw.fontp->descent;
	w->ctw.font_width = XTextWidth(w->ctw.fontp, "A", 1);
	return 0;
}
/**********************************************************************/
/*   Function to allocate color pixels.				      */
/**********************************************************************/
static int
setup_x11_colors(Display *dpy) {
	static int first_time = TRUE;
	static int is_color;

	Colormap	default_color_map;
	int	default_depth;
	XColor		exact_def;
	int		i;
	static char	*color_tbl[] = {
  "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white", 
  "dim gray", "red", "green", "yellow", "blue", "magenta", "cyan", "white", 
		NULL
		};
	static char	*mono_tbl[] = {
  "black", "gray80", "gray85", "gray90", "gray95", "gray96", "gray98", "white",
  "black", "gray80", "gray85", "gray90", "gray95", "gray96", "gray98", "white",
		NULL
		};
	char	**color_names = color_tbl;

	if (!first_time)
		return is_color;
	first_time = FALSE;
	default_color_map = DefaultColormap(dpy, DefaultScreen(dpy));
	/***********************************************/
	/*   If  we've  got  a  mono-only screen then  */
	/*   set  up  the  flag  saying whether we're  */
	/*   running on mono or color screen.	       */
	/***********************************************/
	default_depth = DefaultDepth(dpy, DefaultScreen(dpy));
	if (default_depth == 1 || getenv("CRMONO") != (char *) NULL) {
		color_names = mono_tbl;
		is_color = FALSE;
		}
	else
		is_color = TRUE;

	for (i = 0; color_names[i]; i++) {
		if (!XParseColor(dpy, default_color_map, color_names[i], &exact_def)) {
			fprintf(stderr, "Color name %s not in database.\n",
				color_names[i]);
			exit(0);
			}
		if (!XAllocColor(dpy, default_color_map, &exact_def)) {
			fprintf(stderr, "Can't allocate color: %s -- all color cells allocated.\n",
				color_names[i]);
			exit(1);
			}
		x11_colors[i] = exact_def.pixel;
		}
	return is_color;
}
/**********************************************************************/
/*   Handle exposure events. Redraw appropriate sections of window.   */
/**********************************************************************/
static void
exposed_region(CtwWidget w, int x, int y, int width, int height) {
	int	start_row, start_col;
	int	end_row, end_col;

	/***********************************************/
	/*   Convert     pixel     co-ordinates    to  */
	/*   character positions.		       */
	/***********************************************/
	get_xy(w, &start_row, &start_col, x, y);
	get_xy(w, &end_row, &end_col, x + width, y + height);

	update_region(w, start_row, start_col, end_row + 1, end_col + 1);
	/***********************************************/
	/*   Redraw cursor if we just obliterated it.  */
	/***********************************************/
	if (start_row <= w->ctw.y && w->ctw.y <= end_row + 1 &&
	    start_col <= w->ctw.x && w->ctw.x <= end_col + 1)
	    	show_cursor(w);
}
/**********************************************************************/
/*   Function to allocate (or re-allocate) screen memory structure.   */
/**********************************************************************/
static void
alloc_screen(CtwWidget w, int clear_flag) {
	int	i, j, c;
	int	rows;
	ctw_callback_t	reason;
	vbyte_t	**old_lines = (vbyte_t **) NULL;
	vbyte_t	*old_memory = (vbyte_t *) NULL;

	turn_off_cursor(w);
	/***********************************************/
	/*   Dont  do  anything if screen is the same  */
	/*   size.				       */
	/***********************************************/
	if (w->ctw.rows == w->ctw.old_rows && w->ctw.columns == w->ctw.old_columns)
		return;

	/***********************************************/
	/*   Save  copy  of  old  screen  contents in  */
	/*   case   we   want  to  keep  as  much  as  */
	/*   possible.				       */
	/***********************************************/
	old_memory = w->ctw.memory;
	old_lines = w->ctw.lines;

	rows = w->ctw.rows + w->ctw.max_lines;
	w->ctw.memory = (vbyte_t *) 
		chk_alloc(rows * w->ctw.columns * sizeof(vbyte_t));
	w->ctw.orig_lines = (vbyte_t **) chk_alloc(rows * sizeof(vbyte_t *));
	w->ctw.lines = w->ctw.orig_lines;
	for (i = 0; i < rows; i++) {
		w->ctw.lines[i] = &w->ctw.memory[i * w->ctw.columns];
		}
	/***********************************************/
	/*   Set  whole  screen  to  white  spaces on  */
	/*   black background.			       */
	/***********************************************/
	blank_line(w, w->ctw.lines[0]);
	for (i = 1; i < rows; i++)
		memcpy(w->ctw.lines[i], 
			w->ctw.lines[0], sizeof(vbyte_t) * w->ctw.columns);
	/***********************************************/
	/*   Keep cursor within bounds of the screen.  */
	/***********************************************/
	if (w->ctw.y >= w->ctw.rows)
		w->ctw.y = w->ctw.rows - 1;
	if (w->ctw.saved_y >= w->ctw.rows)
		w->ctw.saved_y = w->ctw.rows - 1;
	if (w->ctw.x >= w->ctw.columns)
		w->ctw.x = w->ctw.columns - 1;
	if (w->ctw.saved_x >= w->ctw.columns)
		w->ctw.saved_x = w->ctw.columns - 1;

	/***********************************************/
	/*   If  we're  resizing the screen then copy  */
	/*   back  as  much  of  the  old  screen  as  */
	/*   possible.				       */
	/***********************************************/
	if (!clear_flag) {
		int	last_line;
		c = w->ctw.old_columns;
		if (w->ctw.columns < c)
			c = w->ctw.columns;
		if (w->ctw.rows < w->ctw.old_rows)
			last_line = w->ctw.top_line + w->ctw.rows;
		else
			last_line = w->ctw.top_line + w->ctw.old_rows;
		i = 0;
		while (i < last_line) {
			for (j = 0; j < c; j++)
				w->ctw.lines[i][j] = old_lines[i][j];
			i++;
			}
		}
	/***********************************************/
	/*   Save size in case we get a resize event.  */
	/***********************************************/
	w->ctw.old_rows = w->ctw.rows;
	w->ctw.old_columns = w->ctw.columns;

	if (old_memory)
		chk_free((void *) old_memory);
	if (old_lines)
		chk_free((void *) old_lines);
	turn_on_cursor(w);
	/***********************************************/
	/*   Allow   owner   of  widget  to  see  the  */
	/*   resize event.			       */
	/***********************************************/
	reason.reason = CTW_RESIZE;
	XtCallCallbacks((Widget)w, XtNresizeCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Function to display cursor at current position.		      */
/**********************************************************************/
static void
show_cursor(CtwWidget w) {
	vbyte_t	attr;
	int	y = w->ctw.top_line + w->ctw.y;
	int	x;

	/***********************************************/
	/*   Dont do anything if cursor not visible.   */
	/***********************************************/
	if (w->ctw.cursor_visible == FALSE || 
	    w->ctw.cursor_state == CURSOR_HIDDEN ||
	    w->ctw.old_top_line >= 0)
		return;
	x = w->ctw.x >= w->ctw.columns ? w->ctw.columns - 1 : w->ctw.x;
	attr = w->ctw.lines[y][x];
	XDrawImageString(XtDisplay(w), XtWindow(w), w->ctw.cursor_gc, 
		x * w->ctw.font_width, 
		ROW_TO_PIXEL(w, w->ctw.y),
		&attr.vb_byte, 1);
	w->ctw.cursor_state = CURSOR_ON;
}
/**********************************************************************/
/*   Function to turn off cursor.				      */
/**********************************************************************/
static void
turn_off_cursor(CtwWidget w) {
	int	y = w->ctw.y;
	int	x;

	if (w->ctw.cursor_state == CURSOR_OFF || w->ctw.cursor_state == CURSOR_HIDDEN)
		return;
	w->ctw.cursor_state = CURSOR_OFF;
	x = w->ctw.x >= w->ctw.columns ? w->ctw.columns - 1 : w->ctw.x;
	update_region(w, y, x, y+1, x+1);
}
/**********************************************************************/
/*   Function to toggle cursor.					      */
/**********************************************************************/
static void
toggle_cursor(CtwWidget w) {
	if (w->ctw.cursor_state == CURSOR_OFF)
		turn_on_cursor(w);
	else
		turn_off_cursor(w);
}
/**********************************************************************/
/*   Function to turn on cursor.				      */
/**********************************************************************/
static void
turn_on_cursor(CtwWidget w) {
	if (w->ctw.cursor_state == CURSOR_ON || w->ctw.cursor_state == CURSOR_HIDDEN)
		return;
	w->ctw.cursor_state = CURSOR_ON;
	show_cursor(w);
}
/**********************************************************************/
/*   Routine  to  update  a  region of the screen. We get passed the  */
/*   co-ordinates as character positions (not pixels).		      */
/**********************************************************************/
static void
update_region(CtwWidget w, int start_line, int start_col, int end_line,
	int end_col) {
	int	r;
	vbyte_t	*vp;

	if (end_line > w->ctw.rows)
		end_line = w->ctw.rows;
	if (end_col > w->ctw.columns)
		end_col = w->ctw.columns;
	if (start_line < 0)
		r = 0;
	else
		r = start_line;
	for (; r < end_line; r++) {
		vp = w->ctw.lines[r + w->ctw.top_line] + start_col;
		print_string(w, r, start_col, end_col - start_col, vp);
		}
}
/**********************************************************************/
/*   Routine  to  draw  a  single  string at the specified position.  */
/*   Handles the changes in attributes.				      */
/**********************************************************************/
static void
print_string(CtwWidget w, int row, int col, int len, vbyte_t *vp) {
	char	buf[80];
	char	*end_buf;
	char	*bp;
	Pixel	fg, bg;
	int	c;
	int	end_col = col + len;

	for (c = col; c < end_col; ) {
		vbyte_t	attr;
		attr.vb_all = vp->vb_all;
		/***********************************************/
		/*   Draw  as  many consecutive characters as  */
		/*   we can until we hit a color change.       */
		/***********************************************/
		bp = buf;
		if (end_col - c > sizeof buf)
			end_buf = &buf[sizeof buf];
		else
			end_buf = &buf[end_col - c];
		while (bp < end_buf) {
			if (vp->vb_color != attr.vb_color ||
			    vp->vb_attr != attr.vb_attr)
				break;
			*bp++ = vp->vb_byte;
			vp++;
			}
		if (attr.vb_attr & VB_SELECT) {
			fg = w->ctw.hilite_fg;
			bg = w->ctw.hilite_bg;
			}
		else {
			fg = x11_colors[GET_FG(attr.vb_color)];
			bg = x11_colors[GET_BG(attr.vb_color)];
			}
		/***********************************************/
		/*   Handle   line   drawing   by   doing  it  */
		/*   ourselves.				       */
		/***********************************************/
		len = bp - buf;
		if (attr.vb_attr & VB_LINE) {
			for (bp--; bp >= buf; bp--) {
				if (*bp >= 0x5f && *bp <= 0x7e)
					*bp = *bp == 0x5f ? 0x7f : (*bp - 0x5f);
				}
			draw_line(w, row, c, buf, len, fg, bg, attr.vb_attr);
			}
		else {
			draw_string(w, row, c, buf, len, fg, bg, attr.vb_attr);
			}
		c += len;
		}
}
/**********************************************************************/
/*   Function to draw the line-drawing characters.		      */
/**********************************************************************/
static void
draw_line(CtwWidget w, int row, int col, unsigned char *str, int len,
	 Pixel fg, Pixel bg, int attr) {
	if (w->ctw.line_fontp == (XFontStruct *) NULL) {
		draw_string(w, row, col, str, len, fg, bg, attr);
		return;
		}

	XSetForeground(XtDisplay(w), w->ctw.line_gc, fg);
	XSetBackground(XtDisplay(w), w->ctw.line_gc, bg);
	XDrawImageString(XtDisplay(w), XtWindow(w), w->ctw.line_gc, 
		col * w->ctw.font_width, 
		ROW_TO_PIXEL(w, row),
		str, len);
}
/**********************************************************************/
/*   Public  function  to  draw  a  string  at the specified row/col  */
/*   with the specified foreground and background colors.	      */
/**********************************************************************/
static void
draw_string(CtwWidget w, int row, int col, char *str, int len,
	 Pixel fg, Pixel bg, int attr) {
	int	x = col * w->ctw.font_width;
	int	y = ROW_TO_PIXEL(w, row);
	
	XSetForeground(XtDisplay(w), w->ctw.gc, fg);
	XSetBackground(XtDisplay(w), w->ctw.gc, bg);
	XDrawImageString(XtDisplay(w), XtWindow(w), w->ctw.gc, 
		x, y,
		str, len);
	if (attr & VB_BOLD) {
		XDrawString(XtDisplay(w), XtWindow(w), w->ctw.gc, 
			x + 1, y,
			str, len);
		}
	if (attr & VB_UNDERLINE) {
		XDrawLine(XtDisplay(w), XtWindow(w), w->ctw.gc,
			x, y,
			x + len * w->ctw.font_width, y);
		}
}
/**********************************************************************/
/*   Routine  to  convert  an (x,y) pixel co-ordinate to a character  */
/*   position.							      */
/**********************************************************************/
static void
get_xy(CtwWidget w, int *row, int *col, int x, int y) {
	*row = y / w->ctw.font_height;
	*col = x / w->ctw.font_width;
	if (*row > w->ctw.rows)
		*row = w->ctw.rows;
	if (*col > w->ctw.columns)
		*col = w->ctw.columns;
}
static int
funcvalue (int keycode) {
	switch (keycode) {
		case XK_F1:	return(11);
		case XK_F2:	return(12);
		case XK_F3:	return(13);
		case XK_F4:	return(14);
		case XK_F5:	return(15);
		case XK_F6:	return(17);
		case XK_F7:	return(18);
		case XK_F8:	return(19);
		case XK_F9:	return(20);
		case XK_F10:	return(21);
		case XK_F11:	return(23);
		case XK_F12:	return(24);
		case XK_F13:	return(25);
		case XK_F14:	return(26);
		case XK_F15:	return(28);
		case XK_Help:	return(28);
		case XK_F16:	return(29);
		case XK_Menu:	return(29);
		case XK_F17:	return(31);
		case XK_F18:	return(32);
		case XK_F19:	return(33);
		case XK_F20:	return(34);

		case XK_Find :	return(1);
		case XK_Insert:	return(2);
		case XK_Delete:	return(3);
# if defined(DXK_Remove)
		case DXK_Remove: return(3);
# endif
		case XK_Select:	return(4);
		case XK_Prior:	return(5);
		case XK_Next:	return(6);
		default:	return(-1);
	}
}


static int 
sunfuncvalue (int keycode) {
  	switch (keycode) {
		case XK_F1:	return(224);
		case XK_F2:	return(225);
		case XK_F3:	return(226);
		case XK_F4:	return(227);
		case XK_F5:	return(228);
		case XK_F6:	return(229);
		case XK_F7:	return(230);
		case XK_F8:	return(231);
		case XK_F9:	return(232);
		case XK_F10:	return(233);
		case XK_F11:	return(192);
		case XK_F12:	return(193);
		case XK_F13:	return(194);
		case XK_F14:	return(195);
		case XK_F15:	return(196);
		case XK_Help:	return(196);
		case XK_F16:	return(197);
		case XK_Menu:	return(197);
		case XK_F17:	return(198);
		case XK_F18:	return(199);
		case XK_F19:	return(200);
		case XK_F20:	return(201);

		case XK_R1:	return(208);
		case XK_R2:	return(209);
		case XK_R3:	return(210);
		case XK_R4:	return(211);
		case XK_R5:	return(212);
		case XK_R6:	return(213);
		case XK_R7:	return(214);
		case XK_R8:	return(215);
		case XK_R9:	return(216);
		case XK_R10:	return(217);
		case XK_R11:	return(218);
		case XK_R12:	return(219);
		case XK_R13:	return(220);
		case XK_R14:	return(221);
		case XK_R15:	return(222);
  
		case XK_Find :	return(1);
		case XK_Insert:	return(2);
		case XK_Delete:	return(3);
# if defined(DXK_Remove)
		case DXK_Remove: return(3);
# endif
		case XK_Select:	return(4);
		case XK_Prior:	return(5);
		case XK_Next:	return(6);
		default:	return(-1);
	}
}

/**********************************************************************/
/*   Function to reset the screen.				      */
/**********************************************************************/
static void
reset_screen(CtwWidget w) {
	w->ctw.x = w->ctw.y = 0;
	w->ctw.saved_x = w->ctw.saved_y = 0;
	w->ctw.flags[CTW_NEWLINE_GLITCH] = TRUE;

	w->ctw.attr.vb_all = 0;
	w->ctw.attr.vb_color = FG(WHITE);
	w->ctw.old_color = FG(WHITE);
	w->ctw.char_set = 'B';

	w->ctw.escp = (char *) NULL;

	w->ctw.scroll_top = 0;
	w->ctw.scroll_bot = w->ctw.rows;

	w->ctw.flags[CTW_AUTOWRAP] = TRUE;
	w->ctw.flags[CTW_AUTOLINEFEED] = FALSE;
	w->ctw.flags[CTW_APPL_KEYPAD]  = FALSE;
	w->ctw.flags[CTW_CURSOR_KEYPAD] = FALSE;
	w->ctw.flags[CTW_LITERAL_MODE] = FALSE;
}
# define	FLUSH_OUTPUT() \
		{ \
		if (vp != start_vp) { \
			w->ctw.x = vp - vp_start; \
			update_region(w, w->ctw.y, start_vp - vp_start, \
				w->ctw.y + 1, w->ctw.x); \
			} \
		}
/**********************************************************************/
/*   Main public function for adding a string to the widget.	      */
/**********************************************************************/
void
ctw_add_string(CtwWidget w, char *str, int len) {
	if (w->ctw.flags[CTW_SLOW_MODE] == FALSE) {
		ctw_add_string2(w, str, len);
		return;
		}
	while (len-- > 0) {
		ctw_add_string2(w, str++, 1);
		XFlush(XtDisplay(w));
		usleep(20000);
		}
}
/**********************************************************************/
/*   Function  to  add  a  string  to  the  display.  Processes ANSI  */
/*   escape sequences.						      */
/**********************************************************************/
static void
ctw_add_string2(CtwWidget w, char *str, int len) {
	ctw_callback_t	reason;
	int	x;
	vbyte_t	attr;
	static int nest_level = 0;
	char	*str_end = str + len;
	vbyte_t	*vp_start, *vp_end;
	vbyte_t	*start_vp, *vp;
	int	top_line = w->ctw.top_line;
	int	pc_charset = w->ctw.flags[CTW_PC_CHARSET];

	if (nest_level++ == 0)
		turn_off_cursor(w);

	/***********************************************/
	/*   Reframe   the   area  if  the  user  has  */
	/*   scrolled us.			       */
	/***********************************************/
	if (w->ctw.old_top_line >= 0) {
		ctw_callback_t reason;
		if (w->ctw.old_top_line != w->ctw.top_line) {
			w->ctw.top_line = w->ctw.old_top_line;
			update_region(w, 0, 0, 
				w->ctw.rows + 1, w->ctw.columns + 1);
			}
		w->ctw.old_top_line = -1;
		reason.reason = CTW_TOP_LINE;
		reason.top_line = w->ctw.top_line;
		XtCallCallbacks((Widget)w, XtNtopCallback, (caddr_t) &reason);
		}
	/***********************************************/
	/*   If  we  were in the middle of processing  */
	/*   an  escape  sequence  then carry on from  */
	/*   where we left off.			       */
	/***********************************************/
	if (w->ctw.escp)
		str = handle_escape(w, str, str_end);

	attr = w->ctw.attr;
start_again:
	vp_start = w->ctw.lines[w->ctw.top_line + w->ctw.y];
	start_vp = vp = vp_start + w->ctw.x;
	vp_end = vp_start + w->ctw.columns;
	while (str < str_end) {
		if (parse_tbl[*str] == 0) {
DEFAULT:
		  	if (vp >= vp_end) {
				FLUSH_OUTPUT();
				w->ctw.x = 0;
				down_line(w, str);
				goto start_again;
				}
			attr.vb_byte = *str++;
			*vp = attr;
			if (pc_charset && pc_chars[attr.vb_byte]) {
				vp->vb_attr |= VB_LINE;
				vp->vb_byte = pc_chars[attr.vb_byte];
				}

			if (++vp < vp_end)
				continue;
			FLUSH_OUTPUT();
			if (w->ctw.flags[CTW_AUTOWRAP]) {
				if (w->ctw.flags[CTW_NEWLINE_GLITCH])
					continue;
				w->ctw.x = 0;
				down_line(w, str);
				goto start_again;
				}
			else
				vp--;
			continue;
			}
		switch (*str++) {
		  case BEL:
			break;
		  case '\b':
		  	FLUSH_OUTPUT();
		  	if (w->ctw.flags[CTW_LITERAL_MODE]) {
				ctw_add_string2(w, "<BS>", 4);
				goto start_again;
				}
			if (vp > vp_start) {
				start_vp = --vp;
				w->ctw.x = vp - vp_start;
				}
			break;
		  case '\t':
		  	if (w->ctw.flags[CTW_LITERAL_MODE]) {
			  	FLUSH_OUTPUT();
				ctw_add_string2(w, "<TAB>", 5);
				goto start_again;
				}
			x = vp - vp_start;
			if (w->ctw.flags[CTW_DESTRUCTIVE_TABS] == FALSE) {
				x = (x | 7) + 1;
				vp = vp_start + x;
				break;
				}
		  	attr.vb_byte = ' ';
			do {
				*vp++ = attr;
				}
			while (vp < vp_end && (++x & 7) != 0);
		  	break;
		  case '\r':
		  	FLUSH_OUTPUT();
		  	if (w->ctw.flags[CTW_LITERAL_MODE]) {
				ctw_add_string2(w, "<CR>", 4);
				goto start_again;
				}
			w->ctw.x = 0;
			goto start_again;
		  case '\n':
		  	FLUSH_OUTPUT();
			down_line(w, str);
			goto start_again;
		  case 'n' & 0x1f:
		  	attr.vb_attr |= VB_LINE;
		  	break;
		  case 'o' & 0x1f:
		  	attr.vb_attr &= ~VB_LINE;
		  	break;
		  case XON:
		  	if (!w->ctw.flags[CTW_LITERAL_MODE])
				goto DEFAULT;
		  	FLUSH_OUTPUT();
			ctw_add_string2(w, "<XON>", 5);
			break;
		  case XOFF:
		  	if (!w->ctw.flags[CTW_LITERAL_MODE])
				goto DEFAULT;
		  	FLUSH_OUTPUT();
			ctw_add_string2(w, "<XOFF>", 6);
			break;
		  case ESC:
		  	str--;
		  	if (w->ctw.flags[CTW_LITERAL_MODE])
				goto DEFAULT;
		  	FLUSH_OUTPUT();
			w->ctw.escp = w->ctw.escbuf;
		  	str = handle_escape(w, str+1, str_end);
			attr = w->ctw.attr;
			goto start_again;
		  case '\0':
			break;
		  }
		}
	FLUSH_OUTPUT();
	w->ctw.attr = attr;
	if (w->ctw.num_exposures)
		wait_for_exposure(w);

	if (--nest_level == 0)
		turn_on_cursor(w);
	/***********************************************/
	/*   If  top  line  changed then tell user so  */
	/*   he/she   can   update  a  scrollbar  for  */
	/*   instance.				       */
	/***********************************************/
	if (top_line != w->ctw.top_line) {
		reason.reason = CTW_TOP_LINE;
		reason.top_line = w->ctw.top_line;
		XtCallCallbacks((Widget)w, XtNtopCallback, (caddr_t) &reason);
		}
}
/**********************************************************************/
/*   Function  to  move  cursor  down  a  line,  possibly  forcing a  */
/*   scroll.  Also  look  ahead  for more new-lines to see if we can  */
/*   bit-blt more than a line at a time.			      */
/**********************************************************************/
static void
down_line(CtwWidget w, char *str) {
	int	y = w->ctw.y;
	int	i;
	int	max_rows;
# if COLLECT_STATS
	ctw_stats.lines_scrolled++;
# endif
	if (y < w->ctw.scroll_bot-1) {
		w->ctw.y++;
		return;
		}
	/***********************************************/
	/*   Look  ahead  to  see  if  we  can scroll  */
	/*   more than one line.		       */
	/***********************************************/
	i = 1;
	max_rows = w->ctw.scroll_bot - w->ctw.scroll_top;
	while (1) {
		switch (*str++) {
		  case ESC:
		  	goto after_loop;
		  case '\n':
		  	if (++i >= max_rows)
				goto after_loop;
			break;
		  case '\0':
		  	goto after_loop;
		  }
		}
after_loop:

	/***********************************************/
	/*   If  we  have  a  hilited  selection then  */
	/*   adjust place where its displayed.	       */
	/***********************************************/
	if (w->ctw.sel_string) {
		w->ctw.sel_start_y -= i;
		w->ctw.sel_cur_y -= i;
		if (w->ctw.sel_start_y < 0 ||
		    w->ctw.sel_cur_y < 0) {
			w->ctw.sel_start_y -= -1;
			w->ctw.sel_cur_y -= -1;
			}
		}

	w->ctw.y -= i-1;
	if (w->ctw.flags[CTW_SCROLLING_REGION] == FALSE &&
	    w->ctw.top_line < w->ctw.max_lines - 1) {
		if (w->ctw.top_line + i > w->ctw.max_lines)
			w->ctw.top_line = w->ctw.max_lines;
		else
			w->ctw.top_line += i;
		delete_line(w, 0, i);
		}
	else {
# if COLLECT_STATS
		ctw_stats.lines_jumped += i;
		ctw_stats.scroll_ups++;
# endif
		scroll_up(w, i);
		}
}
/**********************************************************************/
/*   Scroll screen up number of lines. Called on '\n'.		      */
/**********************************************************************/
static void
scroll_up(CtwWidget w, int num_lines) {
	int	j;
	vbyte_t	*vp;
	int	end;
	int	start_line;
	int	disp_start;

	if (w->ctw.flags[CTW_SCROLLING_REGION]) {
		start_line = w->ctw.top_line + w->ctw.scroll_top;
		disp_start = w->ctw.scroll_top;
		end = w->ctw.top_line + w->ctw.scroll_bot;
		}
	else {
		start_line = 0;
		disp_start = 0;
		end = w->ctw.max_lines + w->ctw.rows;
		}
# if 0
	/***********************************************/
	/*   Blank  the  lines  at  the  top  of  the  */
	/*   screen  which  are  about  to be wrapped  */
	/*   around to the bottom.		       */
	/***********************************************/
	for (j = 0; j < num_lines; j++) {
		blank_line(w, w->ctw.lines[start_line + j]);
		}
	rotate_mem(&w->ctw.lines[start_line],
		&w->ctw.lines[end - start_line],
		-num_lines * sizeof(vbyte_t *));
# else
	for (j = 0; j < num_lines; j++) {
		vp = w->ctw.lines[start_line];
		memcpy(&w->ctw.lines[start_line], 
			&w->ctw.lines[start_line+1],
			(end - start_line - 1) * sizeof(vbyte_t *));
		w->ctw.lines[end-1] = vp;
		blank_line(w, vp);
		}
# endif
	delete_line(w, disp_start, num_lines);
}
/**********************************************************************/
/*   Scroll screen up number of lines. Called on ESC[M		      */
/**********************************************************************/
static void
scroll_up_local(CtwWidget w, int start_line, int num_lines) {
	int	j;
	vbyte_t	*vp;
	int	end;
	int	top = w->ctw.top_line;

	
	if (w->ctw.flags[CTW_SCROLLING_REGION]) {
		end = w->ctw.scroll_bot;
		}
	else {
		end = w->ctw.rows;
		}
	if (num_lines > w->ctw.scroll_bot - w->ctw.scroll_top)
		num_lines = w->ctw.scroll_bot - w->ctw.scroll_top;
	for (j = 0; j < num_lines; j++) {
		vp = w->ctw.lines[top + start_line];
		memcpy(&w->ctw.lines[top + start_line], 
			&w->ctw.lines[top + start_line+1],
			(end - start_line - 1) * sizeof(vbyte_t *));
		w->ctw.lines[top + end-1] = vp;
		blank_line(w, vp);
		}
	delete_line(w, start_line, num_lines);
}
/**********************************************************************/
/*   Function to parse or continue parsing an escape sequence.	      */
/**********************************************************************/
static char *
handle_escape(CtwWidget w, char *str, char *str_end) {
	char	*cp = w->ctw.escp;
	char	*cpend = &w->ctw.escbuf[MAX_ESCBUF - 1];

	while (str < str_end) {
		/***********************************************/
		/*   If sequence too large then just junk it.  */
		/***********************************************/
		if (cp >= cpend) {
			w->ctw.escp = (char *) NULL;
			send_str(w, "<ESC>", 5);
			send_str(w, w->ctw.escbuf, MAX_ESCBUF);
			return str;
			}
		*cp++ = *str++;
		switch(w->ctw.escbuf[0]) {
		  case '[':
		  	if (!isalpha(str[-1]) && str[-1] != '@')
				continue;
			if (str[-1] == '-')
				continue;
			break;
		  case ']':
		  	if (str[-1] == BEL)
				break;
			/***********************************************/
			/*   Check  for  shelltool  style icon/window  */
			/*   name escape			       */
			/***********************************************/
			if (cp < &w->ctw.escbuf[3])
				continue;
			if (str[-1] == '\\' && str[-2] == ESC)
				break;
			continue;
		  case '(':
		  case ')':
		  	if (cp > &w->ctw.escbuf[1])
				break;
			continue;
		  case '7':
		  	w->ctw.saved_x = w->ctw.x;
		  	w->ctw.saved_y = w->ctw.y;
			w->ctw.escp = NULL;
		  	return str;
		  case '8':
		  	w->ctw.x = w->ctw.saved_x;
		  	w->ctw.y = w->ctw.saved_y;
			w->ctw.escp = NULL;
		  	return str;
		  case 'c':
		  	reset_screen(w);
			return str;
		  case '<':
		  	return str;
		  case '>':
		  	w->ctw.flags[CTW_APPL_KEYPAD] = FALSE;
			w->ctw.escp = (char *) NULL;
		  	return str;
		  case '=':
		  	w->ctw.flags[CTW_APPL_KEYPAD] = TRUE;
			w->ctw.escp = (char *) NULL;
		  	return str;
		  case 'D':
		  	do_index(w);
		  	return str;
		  case 'M':
		  	do_rev_index(w);
		  	return str;
		  case 'Z':
		  	send_input(w, "\033[?1;2c", 7);
			w->ctw.escp = NULL;
		  	return str;
		  default:
		  	if (w->ctw.flags[CTW_ESC_LITERAL]) {
				char	buf[1];
				buf[0] = w->ctw.escbuf[0];
				w->ctw.escp = NULL;
			  	send_str(w, buf, 1);
				return str;
				}
		  	break;
		  }
		*cp = '\0';
		w->ctw.escp = NULL;
		if (process_escape(w) == FALSE) {
			send_str(w, "<ESC>", 5);
			send_str(w, w->ctw.escbuf, cp - w->ctw.escbuf);
			}
		w->ctw.escp = NULL;
		return str;
		}
	w->ctw.escp = cp;
	return str;
}
/**********************************************************************/
/*   Execute a compiled escape sequence.			      */
/**********************************************************************/
static int
process_escape(CtwWidget w) {
	int	args[MAX_ARGS];
	int	arg1;
	int	arg_no = 0;
	int	n, i, y;
	int	quest = FALSE;
	int	esc_equ = FALSE;
	int	isneg;
	char	*cp;
	char	buf[32];
  	vbyte_t space, attr;
	ctw_callback_t	reason;

	cp = w->ctw.escbuf;
	switch (*cp++) {
	  case '[':
	  	if (*cp == '?') {
			cp++;
		  	quest = TRUE;
			}
		break;
	  case ']':
	  	do_text_parms((Widget) w, cp);
		return TRUE;
	  case ')':
	  	/***********************************************/
	  	/*   Change character set.		       */
	  	/***********************************************/
		switch (*cp) {
		  case '0':
		  	w->ctw.attr.vb_attr &= ~VB_LINE;
			break;
		  case 'B':
		  default:
		  	w->ctw.attr.vb_attr |= VB_LINE;
			break;
		  }
	  	return TRUE;
	  case '(':
	  	/***********************************************/
	  	/*   Change character set.		       */
	  	/***********************************************/
		switch (*cp) {
		  case '0':
		  	w->ctw.attr.vb_attr |= VB_LINE;
			break;
		  case 'B':
		  default:
		  	w->ctw.attr.vb_attr &= ~VB_LINE;
			break;
		  }
	  	return TRUE;
	  default:
	  	return FALSE;
	  }

	/***********************************************/
	/*   Handle ISC cursor size changes.	       */
	/***********************************************/
	if (*cp == '=') {
		cp++;
		esc_equ = TRUE;
		}
	while (!isalpha(*cp) && *cp != '@') {
		n = 0;
		isneg = FALSE;
		if (*cp == '-') {
			cp++;
			isneg = TRUE;
			}
		while (isdigit(*cp))
			n = 10 * n + *cp++ - '0';
		args[arg_no++] = isneg ? -n : n;
		if (*cp != ';')
			break;
		cp++;
		}
	if (quest) {
		return do_quest(w, *cp, arg_no, args);
		}
	if (esc_equ)
		return do_escequ(w, *cp, arg_no, args);

	if (arg_no > 0)
		arg1 = args[0];
	else
		arg1 = 1;
	switch (*cp) {
	  case '@': {
		y = w->ctw.top_line + w->ctw.y;
		space.vb_all = 0;
		space.vb_byte = ' ';
		space.vb_color = FG(WHITE) | BG(BLACK);
	  	for (n = arg1; n-- > 0; ) {
		  	for (i = w->ctw.columns - 1; i > w->ctw.x; i--) {
				w->ctw.lines[y][i] = w->ctw.lines[y][i-1];
				}
			w->ctw.lines[y][w->ctw.x] = space;
			}
		update_region(w, w->ctw.y, w->ctw.x, w->ctw.y+1, w->ctw.columns);
	  	break;
		}
	  case 'A':
	  	w->ctw.y -= arg1;
		goto check_cursor;
	  case 'B':
	  	w->ctw.y += arg1;
		goto check_cursor;
	  case 'C':
	  	w->ctw.x += arg1;
		goto check_cursor;
	  case 'D':
	  	w->ctw.x -= arg1;
		goto check_cursor;
	  case 'H':
	  	if (arg_no > 1)
			w->ctw.x = args[1] - 1;
		else
			w->ctw.x = 0;
		if (arg_no > 0)
			w->ctw.y = args[0] - 1;
		else {
			w->ctw.x = 0;
			w->ctw.y = 0;
			}
check_cursor:
		if (w->ctw.x < 0)
			w->ctw.x = 0;
		else if (w->ctw.x >= w->ctw.columns)
			w->ctw.x = w->ctw.columns - 1;
		if (w->ctw.y < w->ctw.scroll_top)
			w->ctw.y = w->ctw.scroll_top;
		else if (w->ctw.y >= w->ctw.scroll_bot)
			w->ctw.y = w->ctw.scroll_bot - 1;
		break;
	  case 'J':
		if (arg_no == 0)
			args[0] = 0;
		switch (args[0]) {
		  case 0:
		  	clear_to_eol(w);
			clear_lines(w, w->ctw.y, w->ctw.rows - 1);
		  	break;
		  case 1:
		  	clear_from_beginning(w);
			clear_lines(w, 0, w->ctw.y - 1);
			break;
		  case 2:
		  	clear_screen(w);
			break;
		  }
	  	break;
	  case 'K':
	  	if (arg_no == 0)
			args[0] = 0;
		switch (args[0]) {
		  case 0:
		  	clear_to_eol(w);
			break;
		  case 1:
		  	clear_from_beginning(w);
			break;
		  case 2:
		  	clear_from_beginning(w);
			clear_to_eol(w);
			break;
		  }
	  	break;
	  case 'L':
	  	if (w->ctw.y >= w->ctw.scroll_top && w->ctw.y < w->ctw.scroll_bot)
		  	scroll_down(w, w->ctw.y, arg1);
	  	break;
	  case 'M':
	  	if (w->ctw.y >= w->ctw.scroll_top && w->ctw.y < w->ctw.scroll_bot)
		  	scroll_up_local(w, w->ctw.y, arg1);
	  	break;
	  case 'P': {
		y = w->ctw.top_line + w->ctw.y;
		space.vb_all = 0;
		space.vb_byte = ' ';
		space.vb_color = FG(WHITE) | BG(BLACK);

	  	for (n = arg1; n-- > 0; ) {
		  	for (i = w->ctw.x; i < w->ctw.columns - 1; i++) {
				w->ctw.lines[y][i] = w->ctw.lines[y][i+1];
				}
			w->ctw.lines[y][i] = space;
			}
		update_region(w, w->ctw.y, w->ctw.x, w->ctw.y+1, w->ctw.columns);
	  	break;
		}
	  case 'S':
	  	do_index(w);
		break;
	  case 'T':
	  	do_rev_index(w);
		break;

	  case 'X':
	  	/***********************************************/
	  	/*   ISC  &  SCO  allow ESC[X have to erase a  */
	  	/*   sequence of blanks.		       */
	  	/***********************************************/
		if (w->ctw.flags[CTW_ISC_DRIVER] == FALSE && 
		    w->ctw.flags[CTW_SCO_DRIVER] == FALSE)
		    	return FALSE;
		attr = w->ctw.attr;
		attr.vb_byte = ' ';
		attr.vb_attr &= ~VB_LINE;
		y = w->ctw.top_line + w->ctw.y;
		for (i = 0; i < arg1; i++) {
			if (w->ctw.x + i >= w->ctw.columns)
				break;
			w->ctw.lines[y][w->ctw.x + i] = attr;
			}
		update_region(w, w->ctw.y, w->ctw.x, w->ctw.y+1, w->ctw.x + arg1);
		break;

	  case 'g':
	  	if (w->ctw.flags[CTW_SCO_DRIVER] == FALSE)
			return FALSE;
		buf[0] = arg1;
		send_str(w, buf, 1);
	  	break;
	  case 'm':
	  	if (arg_no == 0) {
			arg_no = 1;
			args[0] = 0;
			}
	  	for (i = 0; i < arg_no; i++) {
			switch (args[i]) {
			  case 0: {
				w->ctw.attr.vb_all = 0;
				if (w->ctw.flags[CTW_COLOR_RESET] ||
				    w->ctw.old_color == 0)
				  	w->ctw.attr.vb_color = FG(WHITE) | BG(0);
				else
				  	w->ctw.attr.vb_color = w->ctw.old_color;
				break;
				}
			  case 1:
			  	w->ctw.attr.vb_attr |= VB_BOLD;
				break;
			  case 4:
			  	w->ctw.attr.vb_attr |= VB_UNDERLINE;
				break;
			  case 7: {
			  	w->ctw.old_color = w->ctw.attr.vb_color;
			  	w->ctw.attr.vb_color = 
					BG(GET_FG(w->ctw.attr.vb_color)) |
					FG(GET_BG(w->ctw.attr.vb_color));
				break;
				}
			  case 30: case 31: case 32: case 33:
			  case 34: case 35: case 36: case 37:
			  	w->ctw.attr.vb_color = 
					FG(args[i] - 30) | 
					BG(GET_BG(w->ctw.attr.vb_color));
			  	break;
			  case 40: case 41: case 42: case 43:
			  case 44: case 45: case 46: case 47:
			  	w->ctw.attr.vb_color = 
					BG(args[i] - 40) | 
					FG(GET_FG(w->ctw.attr.vb_color));
			  	break;
			  }
			}
	  	break;
	  case 'r':
		w->ctw.scroll_top = 0;
		w->ctw.scroll_bot = w->ctw.rows;
		w->ctw.flags[CTW_SCROLLING_REGION] = FALSE;
		if (arg_no >= 1) {
			w->ctw.flags[CTW_SCROLLING_REGION] = TRUE;
			w->ctw.scroll_top = args[0] ? args[0] - 1 : 0;
			if (arg_no >= 2)
				w->ctw.scroll_bot = args[1] ? args[1] : 0;
			if (w->ctw.scroll_top < 0)
				w->ctw.scroll_top = 0;
			if (w->ctw.scroll_bot < w->ctw.scroll_top || 
			    w->ctw.scroll_bot > w->ctw.rows)
				w->ctw.scroll_bot = w->ctw.rows;
			/***********************************************/
			/*   If  scrolling  region  to  entire window  */
			/*   then turn flag off.		       */
			/***********************************************/
			if (w->ctw.scroll_top == 0 &&
			    w->ctw.scroll_bot == w->ctw.rows)
				w->ctw.flags[CTW_SCROLLING_REGION] = FALSE;
			}
		w->ctw.x = 0;
		w->ctw.y = 0;
	  	break;
	  case 't':
	  	/***********************************************/
	  	/*   Shelltool/cmdtool    compatable   escape  */
	  	/*   sequences.				       */
	  	/***********************************************/
		switch (args[0]) {
		  case 1:
		  	reason.reason = CTW_OPEN_WINDOW;
		  	XtCallCallbacks((Widget)w, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 2:
		  	reason.reason = CTW_CLOSE_WINDOW;
		  	XtCallCallbacks((Widget)w, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 3:
		  	reason.reason = CTW_MOVE_WINDOW;
			reason.x = args[1];
			reason.y = args[2];
		  	XtCallCallbacks((Widget)w, XtNapplCallback, (caddr_t) &reason);
			break;
		  case 4:
		  	reason.reason = CTW_SIZE_WINDOW_PIXELS;
			reason.height = args[1];
			reason.width = args[2];
		  	XtCallCallbacks((Widget)w, XtNapplCallback, (caddr_t) &reason);
			break;
		  case 5:
		  	reason.reason = CTW_FRONT_WINDOW;
		  	XtCallCallbacks((Widget)w, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 6:
		  	reason.reason = CTW_BACK_WINDOW;
		  	XtCallCallbacks((Widget)w, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 8:
		  	reason.reason = CTW_SIZE_WINDOW_CHARS;
			reason.height = args[1];
			reason.width = args[2];
		  	XtCallCallbacks((Widget)w, XtNapplCallback, (caddr_t) &reason);
			break;
		  }
	  	break;
	  default:
	  	return FALSE;
	  }
	return TRUE;
}
/**********************************************************************/
/*   Handle  Sun  compatable escape sequences where we have a string  */
/*   to process.						      */
/**********************************************************************/
static int
do_text_parms(Widget w, char *str) {
	Arg	args[20];
	int	n;
	int	type = atoi(str);
	char	*cp;

	/***********************************************/
	/*   Check for Sun style shell tool sequence.  */
	/***********************************************/
	if (str[0] == 'l') {
		type = 1;
		str++;
		str[strlen(str) - 2] = '\0';
		}
	else if (str[0] == 'L') {
		type = 2;
		str++;
		str[strlen(str) - 2] = '\0';
		}
	else {
		while (*str && *str != ';')
			str++;
			
		if (*str++ == '\0')
			return 0;
		for (cp = str; *cp && *cp != BEL; )
			cp++;
		if (*cp == '\0')
			return 0;
		*cp = '\0';
		}

	/***********************************************/
	/*   Find top level shell.		       */
	/***********************************************/
	while (XtParent(w))
		w = XtParent(w);

	switch (type) {
	  case 0:
	  	/***********************************************/
	  	/*   Window Name and Title.		       */
	  	/***********************************************/
		n = 0;
		XtSetArg(args[n], XtNtitle, str); n++;
		XtSetArg(args[n], XtNiconName, str); n++;
		XtSetValues(w, args, n);
		break;
	  case 1:
	  	/***********************************************/
	  	/*   Window name.			       */
	  	/***********************************************/
		n = 0;
		XtSetArg(args[n], XtNtitle, str); n++;
		XtSetValues(w, args, n);
		break;
	  case 2:
	  	/***********************************************/
	  	/*   Window title.			       */
	  	/***********************************************/
		n = 0;
		XtSetArg(args[n], XtNiconName, str); n++;
		XtSetValues(w, args, n);
		break;
	  case 46:
	  	/***********************************************/
	  	/*   Set log file name.			       */
	  	/***********************************************/
		break;
	  }
	return 1;
}
/**********************************************************************/
/*   Handle ESC ? escape sequences.				      */
/**********************************************************************/
static int
do_quest(CtwWidget w, int cmd, int arg_no, int *args) {
	int	flag = TRUE;
	int	r, c, h, width, num;

	switch (cmd) {
	  case 'l':
	  	flag = FALSE;
		/* Fallthru.. */
	  case 'h':
	  	switch (args[0]) {
		  case 1:
		  	w->ctw.flags[CTW_CURSOR_KEYPAD] = flag;
			return TRUE;
		  case 4:
		  	w->ctw.flags[CTW_SMOOTH_SCROLL] = flag;
			return TRUE;
		  case 7:
		  	w->ctw.flags[CTW_AUTOWRAP] = flag;
			return TRUE;
		  case 47:
		  	/***********************************************/
		  	/*   Alternate screen buffer.		       */
		  	/***********************************************/
			return TRUE;
		  case 1962:
		  	/***********************************************/
		  	/*   My birthyear! Mouse report mode.	       */
		  	/***********************************************/
			w->ctw.flags[CTW_APPL_MOUSE] = flag;
			return TRUE;
		  }
		break;
	  case 'S':
	  	/***********************************************/
	  	/*   Scroll a rectangular region.	       */
	  	/***********************************************/
	  	if (arg_no != 5)
			break;
		r = args[0];
		c = args[1];
		h = args[2];
		width = args[3];
		num = args[4];
		/***********************************************/
		/*   Make sure arguments in range.	       */
		/***********************************************/
		if (r < 0)
			r = 0;
		else if (r >= w->ctw.rows)
			r = w->ctw.rows - 1;
		if (c < 0)
			c = 0;
		else if (c >= w->ctw.columns)
			c = w->ctw.columns - 1;
		if (h <= 0)
			h = 1;
		else if (h >= w->ctw.rows - r)
			h = w->ctw.rows - r;
		if (width <= 0)
			width = 1;
		else if (width >= w->ctw.columns - width)
			width = w->ctw.columns - width;
		if (num < -h)
			num = -h;
		else if (num > h)
			num = h;
		scroll_rectangle(w, r, c, width, h, num);
		return TRUE;
	  }
	return FALSE;
}
/**********************************************************************/
/*   Handle ISC's ESC[=... sequence.				      */
/**********************************************************************/
static int
do_escequ(CtwWidget w, int cmd, int arg_no, int *args) {
	if (w->ctw.flags[CTW_ISC_DRIVER] == FALSE &&
	    w->ctw.flags[CTW_SCO_DRIVER] == FALSE) {
		return FALSE;
		}
	switch (cmd) {
	  case 'C':
	  	/***********************************************/
	  	/*   Ignore cursor size changes for now.       */
	  	/***********************************************/
	  	return TRUE;
	  }
	return FALSE;
}
/**********************************************************************/
/*   Function  to  execute  the  Index  escape  sequences  (ESC D or  */
/*   ESC[S).  Move  down  a  line  or  scroll screen if at bottom of  */
/*   window.							      */
/**********************************************************************/
static void
do_index(CtwWidget w) {
	w->ctw.escp = NULL;
	if (w->ctw.y >= w->ctw.scroll_bot-1)
		scroll_up(w, 1);
	else
		w->ctw.y++;
}
/**********************************************************************/
/*   Function  to  execute a Reverse Index escape sequence (ESC M or  */
/*   ESC[T).  Move  up a line unless at top of region, in which case  */
/*   scroll region down.					      */
/**********************************************************************/
static void
do_rev_index(CtwWidget w) {
	w->ctw.escp = NULL;
	if (w->ctw.y > w->ctw.scroll_top)
		w->ctw.y--;
	else
		scroll_down(w, w->ctw.scroll_top, 1);
}
static void
blank_line(CtwWidget w, vbyte_t *vp) {
	int	i;
	struct copy {
		vbyte_t	array[80];
		};
	static struct copy blank;
	static int first_time = TRUE;

	if (first_time) {
		for (i = 0; i < 80; i++) {
			blank.array[i].vb_all = 0;
			blank.array[i].vb_byte = ' ';
			blank.array[i].vb_color = BG(0) | FG(7);
			}
		first_time = FALSE;
		}

	for (i = w->ctw.columns; i >= 80; i -= 80) {
		*(struct copy *) vp = blank;
		vp += 80;
		}
	while (i-- > 0) {
		*vp++ = blank.array[0];
		}
}
/**********************************************************************/
/*   Do a fast delete line.					      */
/**********************************************************************/
static void
delete_line(CtwWidget w, int start_line, int num_lines) {
	int	width, height;
	int	src_x, src_y, dst_y;

	width = w->ctw.font_width * w->ctw.columns;
	height = w->ctw.font_height * (w->ctw.scroll_bot - start_line - num_lines);

	src_x = 0;
	src_y = w->ctw.font_height * (start_line + num_lines);
	dst_y = w->ctw.font_height * start_line;
	if (height) {
		w->ctw.num_exposures++;
		XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w), w->ctw.gc,
			0, src_y, width, height,
			0, dst_y);
		}
	XClearArea(XtDisplay(w), XtWindow(w),
		0, dst_y + height, 
		0, w->ctw.font_height * w->ctw.scroll_bot - (dst_y + height),
		FALSE);
}
/**********************************************************************/
/*   Do a fast insert line.					      */
/**********************************************************************/
static void
insert_line(CtwWidget w, int row, int bot, int nchunk) {
	int	y, dst_y, height;

	y = row * w->ctw.font_height;
	dst_y = y + nchunk * w->ctw.font_height;
	height = (bot - row - nchunk) * w->ctw.font_height;

	if (height) {	
		w->ctw.num_exposures++;
		XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w), w->ctw.gc,
			0, y, w->ctw.columns * w->ctw.font_width, height,
			0, dst_y);
		}

	XClearArea(XtDisplay(w), XtWindow(w), 
		0, y,
		0, w->ctw.font_height * nchunk, False);

}
/**********************************************************************/
/*   Wait  for  GraphicsExpose  or  NoExpose events to come in after  */
/*   scrolling screen.						      */
/**********************************************************************/
static void
wait_for_exposure(CtwWidget w) {
	XEvent	ev;

	while (w->ctw.num_exposures > 0) {
		XWindowEvent(XtDisplay(w), XtWindow(w), ExposureMask, &ev);
		switch (ev.type) {
		  case Expose:
		  	exposed_region(w, ev.xexpose.x, ev.xexpose.y, 
				ev.xexpose.width, ev.xexpose.height);
			break;
		  case NoExpose:
		  	break;
	  	  case GraphicsExpose:
		  	exposed_region(w, ev.xgraphicsexpose.x, ev.xgraphicsexpose.y, 
				ev.xgraphicsexpose.width, ev.xgraphicsexpose.height);
			break;
		  }
		w->ctw.num_exposures--;
		}
	w->ctw.num_exposures = 0;
}
/**********************************************************************/
/*   Send input via the callback list to the owner.		      */
/**********************************************************************/
static void
send_input(CtwWidget w, char *buf, int len) {
	ctw_callback_t	reason;
	reason.reason = CTW_INPUT;
	reason.ptr = buf;
	reason.len = len;

	XtCallCallbacks((Widget)w, XtNkbdCallback, (caddr_t) &reason);
}
static void
scroll_down(CtwWidget w, int start_line, int num_lines) {
	int	i, j;
	vbyte_t	*vp;
	int	top = w->ctw.top_line;

	if (num_lines > w->ctw.scroll_bot - w->ctw.scroll_top)
		num_lines = w->ctw.scroll_bot - w->ctw.scroll_top;
	for (j = 0; j < num_lines; j++) {
		i = top + w->ctw.scroll_bot - 1;
		vp = w->ctw.lines[i];
		for (; i > top + start_line; i--) {
			w->ctw.lines[i] = w->ctw.lines[i-1];
			}
		w->ctw.lines[top + start_line] = vp;
		blank_line(w, vp);
		}
	insert_line(w, start_line, w->ctw.scroll_bot, num_lines);
}
/**********************************************************************/
/*   Function to display a string internally.			      */
/**********************************************************************/
static void
send_str(CtwWidget w, char *buf, int len) {
	vbyte_t	attr;
	char	*escp = w->ctw.escp;

	attr = w->ctw.attr;
	w->ctw.attr.vb_color = FG(7) | BG(0);
	ctw_add_string2(w, buf, len);
	w->ctw.attr = attr;
	w->ctw.escp = escp;
}
/**********************************************************************/
/*   Clear to end of line.					      */
/**********************************************************************/
static void
clear_to_eol(CtwWidget w) {
	vbyte_t	space;
	int	i;
	int	y = w->ctw.top_line + w->ctw.y;

	space.vb_all = 0;
	space.vb_byte = ' ';
	space.vb_color = FG(WHITE) | BG(BLACK);
	for (i = w->ctw.x; i < w->ctw.columns; i++)
		w->ctw.lines[y][i] = space;

	XClearArea(XtDisplay(w), XtWindow(w), 
		w->ctw.x * w->ctw.font_width,
		w->ctw.y * w->ctw.font_height,
		0, w->ctw.font_height,
		False);

}
/**********************************************************************/
/*   Clear from beginning of line to current character.		      */
/**********************************************************************/
static void
clear_from_beginning(CtwWidget w) {
	vbyte_t	space;
	int	i;
	int	y = w->ctw.top_line + w->ctw.y;

	space.vb_all = 0;
	space.vb_byte = ' ';
	space.vb_color = FG(WHITE) | BG(BLACK);
	for (i = 0; i < w->ctw.x; i++)
		w->ctw.lines[y][i] = space;

	XClearArea(XtDisplay(w), XtWindow(w), 
		0,
		w->ctw.y * w->ctw.font_height,
		w->ctw.x * w->ctw.font_width,
		w->ctw.font_height,
		False);
}
/**********************************************************************/
/*   Clear entire screen.					      */
/**********************************************************************/
static void
clear_screen(CtwWidget w) {
	int	i;
	int	y = w->ctw.top_line + w->ctw.y;

	blank_line(w, w->ctw.lines[y]);
	for (i = 1; i < w->ctw.rows; i++)
		memcpy(w->ctw.lines[w->ctw.top_line + i], 
			w->ctw.lines[y], sizeof(vbyte_t) * w->ctw.columns);

	XClearWindow(XtDisplay(w), XtWindow(w));
}
/**********************************************************************/
/*   Function  to  clear  a selected range of lines. Lines specified  */
/*   relative to current window.				      */
/**********************************************************************/
static void
clear_lines(CtwWidget w, int top, int bot) {
	int	i;

	if (top >= bot)
		return;

	top += w->ctw.top_line;
	bot += w->ctw.top_line;
		
	for (i = top; i < bot; i++) {
		blank_line(w, w->ctw.lines[i]);
		}
	XClearArea(XtDisplay(w), XtWindow(w),
		0, top * w->ctw.font_height,
		0, (bot - top) * w->ctw.font_height, FALSE);
}
/**********************************************************************/
/*   Function  to  retrieve  current setting for the line at the top  */
/*   of the screen.						      */
/**********************************************************************/
int
ctw_get_top_line(CtwWidget ctw) {
	return ctw->ctw.top_line;
}
/**********************************************************************/
/*   Function  to  retrieve  current setting for the line at the top  */
/*   of the screen.						      */
/**********************************************************************/
void
ctw_set_top_line(CtwWidget ctw, int top_line) {
	ctw_callback_t	reason;
	int	old_top;
	int	amt_to_scroll;

	/***********************************************/
	/*   Make sure value in range.		       */
	/***********************************************/
	if (top_line < 0)
		top_line = 0;
	else if (top_line > ctw->ctw.max_lines)
		top_line = ctw->ctw.max_lines;

	/***********************************************/
	/*   Save  the  top  line so that when we get  */
	/*   some  more  data we put it where we left  */
	/*   off and not where the user scrolled.      */
	/***********************************************/
	if (ctw->ctw.old_top_line < 0)
		ctw->ctw.old_top_line = ctw->ctw.top_line;
	if (top_line != ctw->ctw.top_line) {
		old_top = ctw->ctw.top_line;
		/***********************************************/
		/*   Try  and  update window intelligently if  */
		/*   we are scrolling a little up or down.     */
		/***********************************************/
		ctw->ctw.top_line = top_line;
		if (top_line < old_top && top_line + ctw->ctw.rows > old_top) {
			amt_to_scroll = old_top - top_line;
			insert_line(ctw, 0, ctw->ctw.rows, amt_to_scroll);
			update_region(ctw, 0, 0, 
				amt_to_scroll + 1, ctw->ctw.columns + 1);
			}
		else if (top_line < old_top + ctw->ctw.rows) {
			amt_to_scroll = top_line - old_top;
			delete_line(ctw, 0, amt_to_scroll);
			update_region(ctw, ctw->ctw.rows - amt_to_scroll, 0, 
				ctw->ctw.rows, ctw->ctw.columns + 1);
			}
		else {
			update_region(ctw, 0, 0, ctw->ctw.rows + 1, ctw->ctw.columns + 1);
			}
		if (ctw->ctw.num_exposures)
			wait_for_exposure(ctw);
		}
	else
		ctw->ctw.top_line = top_line;
	/***********************************************/
	/*   Let user affect a scrollbar.	       */
	/***********************************************/
	reason.reason = CTW_TOP_LINE;
	reason.top_line = ctw->ctw.top_line;
	XtCallCallbacks((Widget)ctw, XtNtopCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Function to let user see current flags.			      */
/**********************************************************************/
int
ctw_get_attributes(CtwWidget w, int **ip, char ***strp) {
	static char *attr_names[] = {
	"Autowrap",
	"Auto line feed",
	"Application keypad",
	"Application cursor",
	"Application mouse",
	"Color reset",
	"Cut newlines",
	"Destructive tabs",
	"Erase black",
	"Esc + literal",
	"Font size",
	"Literal mode",
	"Newline glitch",
	"PC char set",
	"Reset",
	"Scrolling region",
	"Size",
	"Slow mode",
	"Smooth scroll",
	"Sun keys",
	"ISC driver",
	"SCO driver",
	(char *) NULL
	};

	w->ctw.flags[CTW_RESET] = FALSE;
	w->ctw.flags[CTW_SUN_FUNCTION_KEYS] = w->ctw.sun_function_keys;
	w->ctw.flags[CTW_FONT_SIZE] = (w->ctw.font_width << 16) | w->ctw.font_height;
	w->ctw.flags[CTW_SIZE] = (w->ctw.columns << 16) | w->ctw.rows;
	*ip = &w->ctw.flags[0];
	*strp = attr_names;
	return CTW_MAX_ATTR;
}
/**********************************************************************/
/*   Function to set current attributes.			      */
/**********************************************************************/
void
ctw_set_attributes(CtwWidget w, int *ip) {
	memcpy((char *) w->ctw.flags, (char *) ip, sizeof w->ctw.flags);
	w->ctw.sun_function_keys = ip[CTW_SUN_FUNCTION_KEYS];

	if (w->ctw.flags[CTW_ISC_DRIVER] || w->ctw.flags[CTW_SCO_DRIVER])
		w->ctw.flags[CTW_PC_CHARSET] = TRUE;
	else
		w->ctw.flags[CTW_PC_CHARSET] = FALSE;

	if (w->ctw.flags[CTW_RESET]) {
		reset_screen(w);
		}
}

/**********************************************************************/
/*   Function  called  on  expiration  of timer to cause flashing to  */
/*   occur.							      */
/**********************************************************************/
static void
cursor_flash_proc(XtPointer client_data, XtIntervalId *timer) {
	CtwWidget	w = (CtwWidget) client_data;

	/***********************************************/
	/*   If we dont have focus, turn off timer.    */
	/***********************************************/
	w->ctw.cursor_timer = 0;
	if (!w->ctw.have_focus) {
		turn_on_cursor(w);
		return;
		}
	if (w->ctw.flashrate) {
		toggle_cursor(w);
		w->ctw.cursor_timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget)w),
			(long) w->ctw.flashrate, cursor_flash_proc, w);
		}
}
/**********************************************************************/
/*   Function to get the selection.				      */
/**********************************************************************/
void
ctw_get_selection(CtwWidget w) {
	XtGetSelectionValue((Widget)w, XA_PRIMARY, XA_STRING, 
		(XtSelectionCallbackProc)requestor_callback, 
		NULL, w->ctw.timestamp);
}
/**********************************************************************/
/*   Function  to  handle  the ESC[?r;c;w;h;numS scrolling rectangle  */
/*   escape sequence.						      */
/**********************************************************************/
static void
scroll_rectangle(CtwWidget ctw, int r, int c, int w, int h, int num) {
	int	j, k;
	vbyte_t	space;

	space.vb_all = 0;
	space.vb_byte = ' ';
	space.vb_color = FG(GET_FG(ctw->ctw.attr.vb_color)) | BG(0);
	if (num > 0) {
		if (h != num)
			XCopyArea(XtDisplay(ctw), XtWindow(ctw), XtWindow(ctw), ctw->ctw.gc,
				c * ctw->ctw.font_width, 
				(r + num) * ctw->ctw.font_height,
				w * ctw->ctw.font_width,
				(h - num) * ctw->ctw.font_height,
				c * ctw->ctw.font_width, r * ctw->ctw.font_height);
		XClearArea(XtDisplay(ctw), XtWindow(ctw),
			c * ctw->ctw.font_width, 
			(r + h - num) * ctw->ctw.font_height,
			w * ctw->ctw.font_width, 
			num * ctw->ctw.font_height, FALSE);
		for (j = r; j < r + h - num; j++) {
			memcpy(&ctw->ctw.lines[ctw->ctw.top_line + j][c],
				&ctw->ctw.lines[ctw->ctw.top_line + j + num][c],
					w * sizeof(vbyte_t));
			}
		for ( ; j < r + h; j++) {
			for (k = c; k < c + w; k++) {
				ctw->ctw.lines[j][k] = space;
				}
			}
		}
	else {
		num = -num;
		if (h != num)
			XCopyArea(XtDisplay(ctw), XtWindow(ctw), XtWindow(ctw), ctw->ctw.gc,
				c * ctw->ctw.font_width, 
				r * ctw->ctw.font_height,
				w * ctw->ctw.font_width,
				(h - num) * ctw->ctw.font_height,
				c * ctw->ctw.font_width, (r + num) * ctw->ctw.font_height);
		XClearArea(XtDisplay(ctw), XtWindow(ctw),
			c * ctw->ctw.font_width, 
			r * ctw->ctw.font_height,
			w * ctw->ctw.font_width, 
			num * ctw->ctw.font_height, FALSE);
		for (j = r + h; j > r + num; j--) {
			memcpy(&ctw->ctw.lines[ctw->ctw.top_line + j][c],
				&ctw->ctw.lines[ctw->ctw.top_line + j - num][c],
					w * sizeof(vbyte_t));
			}
		for ( ; j >= r; j--) {
			for (k = c; k < c + w; k++) {
				ctw->ctw.lines[j][k] = space;
				}
			}
		}
}

