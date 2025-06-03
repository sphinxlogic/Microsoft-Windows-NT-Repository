/*#include <X11/copyright.h>*/

/* $XConsortium: Template.h,v 1.4 89/07/21 01:41:49 kit Exp $ */

#ifndef _Ctw_h
#define _Ctw_h

# if	defined(X11r3)
#	define	XtPointer	void *
# endif
/****************************************************************
 *
 * Ctw widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

ctwWidget:
 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 applCallback	     ApplCallback	Callback	NULL
 font		     Font		String		"fixed"
 cursorColor	     CursorColor	Pixel		"red"
 rows		     Rows		Integer		24
 columns	     Columns		Integer		80
 kbdCallback	     KbdCallback	Callback	NULL
 sunFunctionKeys     SunFunctionKeys	Boolean		false
 saveLines	     SaveLines		Integer		512
 resizeCallback	     ResizeCallback	Callback	NULL
 topCallback	     TopCallback	Callback	NULL
 mouseCallback	     MouseCallback	Callback	NULL
 geometry	     Geometry		String		NULL
 flashrate	     Flashrate		Integer		500
 font1		     Font1		String		"5x8"
 font2		     Font2		String		"6x9"
 font3		     Font3		String		"6x10"
 font4		     Font4		String		"6x13"
 font5		     Font5		String		"7x13bold"
 font6		     Font6		String		"8x13bold"
 hiliteBackground    HiliteBackground	String		"CadetBlue"
 hiliteForeground    HiliteForeground	String		"yellow"
 multiClickTime	     MultiClickTime	Integer		250
*/

/**********************************************************************/
/*   define  any  special  resource  names  here  that  are  not  in  */
/*   <X11/StringDefs.h>						      */
/**********************************************************************/

#define	XtNapplCallback "applCallback"
#define	XtNcursorColor	"cursorColor"
#define	XtNrows "rows"
#define	XtNcolumns "columns"
#define	XtNkbdCallback "kbdCallback"
#define	XtNsunFunctionKeys "sunFunctionKeys"
#define	XtNsaveLines "saveLines"
#define	XtNresizeCallback "resizeCallback"
#define	XtNtopCallback "topCallback"
#define	XtNmouseCallback "mouseCallback"
/* #define	XtNgeometry "geometry" */
#define	XtNflashrate "flashrate"
#define XtNfont1 "font1"
#define XtNfont2 "font2"
#define XtNfont3 "font3"
#define XtNfont4 "font4"
#define XtNfont5 "font5"
#define XtNfont6 "font6"
#define	XtNhiliteBackground "hiliteBackground"
#define	XtNhiliteForeground "hiliteForeground"
#define	XtNmultiClickTime "multiClickTime"

#define	XtCApplCallback "ApplCallback"
#define	XtCCursorColor "CursorColor"
#define	XtCRows "Rows"
#define	XtCColumns "Columns"
#define	XtCKbdCallback "KbdCallback"
#define	XtCSunFunctionKeys "SunFunctionKeys"
#define XtCSaveLines "SaveLines"
#define	XtCResizeCallback "ResizeCallback"
#define	XtCTopCallback "TopCallback"
#define	XtCMouseCallback "MouseCallback"
/* #define	XtCGeometry "Geometry" */
#define	XtCFlashrate "Flashrate"
#define XtCFont1 "Font1"
#define XtCFont2 "Font2"
#define XtCFont3 "Font3"
#define XtCFont4 "Font4"
#define XtCFont5 "Font5"
#define XtCFont6 "Font6"
#define	XtCHiliteBackground "HiliteBackground"
#define	XtCHiliteForeground "HiliteForeground"
#define	XtCMultiClickTime "MultiClickTime"

/**********************************************************************/
/*   declare specific CtwWidget class and instance datatypes	      */
/**********************************************************************/

typedef struct _CtwClassRec*	CtwWidgetClass;
typedef struct _CtwRec*		CtwWidget;

/**********************************************************************/
/*   declare the class constant					      */
/**********************************************************************/

extern WidgetClass ctwWidgetClass;

/**********************************************************************/
/*   Declare a callback structure.				      */
/**********************************************************************/
enum ctw_reasons {
	CTW_INPUT,	/* Key or string input.	*/
	CTW_RESIZE,	/* Window was resized.  */
	CTW_TOP_LINE,	/* Top line changed. Allow application to change */
			/* thumb on a scrollbar.			 */
	CTW_BUTTON_DOWN,/* Button down event.				 */
	CTW_BUTTON_UP,	/* Button up event.				 */
	CTW_BUTTON_MOTION,/* Button motion event.			 */
	CTW_SELECTION,	/* User is selecting text -- tell app to not give us */
			/* any more data.				*/
	CTW_PASTE,	/* Received selection from owner.		*/
	CTW_OPEN_WINDOW,/* Uniconise window.				*/
	CTW_CLOSE_WINDOW,/* Iconise window.				*/
	CTW_MOVE_WINDOW, /* Change window position.			*/
	CTW_SIZE_WINDOW_PIXELS, /* Change window size.			*/
	CTW_FRONT_WINDOW,/* Raise window.				*/
	CTW_BACK_WINDOW,	/* Lower window.				*/
	CTW_SIZE_WINDOW_CHARS /* Change window size.			*/
	};

typedef struct ctw_callback_t {
	enum ctw_reasons	reason;
	char			*ptr;
	int			len;
	int			top_line;
	int			x, y;
	int			width, height;
	XEvent			*event;
	} ctw_callback_t;
/**********************************************************************/
/*   Index into array of flag values.				      */
/**********************************************************************/
enum ctw_flags {
	CTW_AUTOWRAP = 0,
	CTW_AUTOLINEFEED,
	CTW_APPL_KEYPAD,
	CTW_APPL_MOUSE,
	CTW_CURSOR_KEYPAD,
	CTW_COLOR_RESET,
	CTW_CUT_NEWLINES,
	CTW_DESTRUCTIVE_TABS,
	CTW_ERASE_BLACK,
	CTW_ESC_LITERAL,
	CTW_FONT_SIZE,
	CTW_LITERAL_MODE,
	CTW_NEWLINE_GLITCH,
	CTW_PC_CHARSET,
	CTW_RESET,
	CTW_SCROLLING_REGION,
	CTW_SIZE,
	CTW_SLOW_MODE,
	CTW_SMOOTH_SCROLL,
	CTW_SUN_FUNCTION_KEYS,
	CTW_ISC_DRIVER,
	CTW_SCO_DRIVER,

	CTW_MAX_ATTR
	};

/**********************************************************************/
/*   Following  macro  used  to  allow  compilation  with ANSI C and  */
/*   non-ANSI C compilers automatically.			      */
/**********************************************************************/
# if !defined(PROTO)
# 	if	defined(__STDC__)
#		define	PROTO(x)	x
#	else
#		define	PROTO(x)	()
#	endif
# endif

/**********************************************************************/
/*   Add a string to be displayed including ANSI escape sequences.    */
/**********************************************************************/
void	ctw_add_string PROTO((CtwWidget, char *, int));

/**********************************************************************/
/*   Return  line  number  at  top  of screen. Used for implementing  */
/*   scrollable area.						      */
/**********************************************************************/
int	ctw_get_top_line PROTO((CtwWidget));

/**********************************************************************/
/*   Set  the  line at the top of the display. Used for implementing  */
/*   scrollable area.						      */
/**********************************************************************/
void	ctw_set_top_line PROTO((CtwWidget, int));

/**********************************************************************/
/*   Function to retrieve current state of the various attributes.     */
/**********************************************************************/
int	ctw_get_attributes PROTO((CtwWidget, int **, char ***));
/**********************************************************************/
/*   Function  to  set  attributes  current  state  of  the  various  */
/*   attributes.						      */
/**********************************************************************/
void	ctw_set_attributes PROTO((CtwWidget, int *));

/**********************************************************************/
/*   Tell widget to grab the contents of the selection.		      */
/**********************************************************************/
void	ctw_get_selection PROTO((CtwWidget));
#endif /* _Ctw_h */
