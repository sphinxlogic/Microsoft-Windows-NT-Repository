/*# include <X11/copyright.h>*/

/* $XConsortium: TemplateP.h,v 1.4 89/07/21 01:41:48 kit Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#ifndef _CtwP_h
#define _CtwP_h

# include "ctw.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRCtwResource "CtwResource"

typedef struct {
/* private: */
	int	dummy;
} CtwClassPart;

typedef struct _CtwClassRec {
    CoreClassPart	core_class;
    CtwClassPart	ctw_class;
} CtwClassRec;

extern CtwClassRec ctwClassRec;

# define	BEL		0x07
# define	ESC		0x1b
# define	XON		('q' & 0x1f)
# define	XOFF		('s' & 0x1f)

# define	MAX_ESCBUF	128

/**********************************************************************/
/*   Structure containing stats.				      */
/**********************************************************************/
# define COLLECT_STATS	1
# if COLLECT_STATS
struct stats {
	unsigned long	lines_scrolled;
	unsigned long	lines_jumped;
	unsigned long	scroll_ups;
	} ctw_stats;
# endif

/**********************************************************************/
/*   Definitions for the multi-click code.			      */
/**********************************************************************/
# define	CLICK_CHAR	0
# define	CLICK_WORD	1
# define	CLICK_LINE	2

/**********************************************************************/
/*   Define  a  'byte'. A byte represents a character on the screen.  */
/*   Its  4-bytes  wide containing an 8-bit character, plus a 'font'  */
/*   indicator.   This  is  used  to  indicate  whether  we  have  a  */
/*   line-drawing  character  at that position. Another byte is used  */
/*   to   encode  color  information.  One  more  byte  for  display  */
/*   attributes  (such  as  bold,  reverse  video). And one which is  */
/*   spare for now.						      */
/**********************************************************************/
# define	VB_LINE		0x01
# define	VB_REVERSE	0x02
# define	VB_BOLD		0x04
# define	VB_FLASHING	0x08
# define	VB_UNDERLINE	0x10
# define	VB_SELECT	0x20

typedef struct vbyte_t {
	union {
		unsigned long	lbyte;
		struct vb {
			unsigned char	byte;
			unsigned char	attr;
			unsigned char	color;
			unsigned char	spare;
			} vbyte;
		} u;
	} vbyte_t;
# define	vb_byte		u.vbyte.byte
# define	vb_attr		u.vbyte.attr
# define	vb_color	u.vbyte.color
# define	vb_spare	u.vbyte.spare
# define	vb_all		u.lbyte

/**********************************************************************/
/*   Color manipulation macros.					      */
/**********************************************************************/
# define	FG_COLOR	0xf0
# define	BG_COLOR	0x0f
# define	FG(x)		((x) << 4)
# define	BG(x)		(x)
# define	GET_FG(x)	((x) >> 4)
# define	GET_BG(x)	((x) & 0x0f)

enum cursor_type {
	CURSOR_OFF,
	CURSOR_ON,
	CURSOR_SORT_OF,
	CURSOR_HIDDEN
	};

typedef struct {
	/* resources */
	Pixel		window_background;
	String		table;
	Pixel		cursor_color;
	int		rows;
	int		columns;
	XtCallbackList	kbd_callback;
	Boolean		sun_function_keys;
	int		max_lines;
	XtCallbackList	resize_callback;
	XtCallbackList	top_callback;
	XtCallbackList	mouse_callback;
	XtCallbackList	appl_callback;
	char		*geometry;
	int		flashrate;
	char		*font;
	char		*font1;
	char		*font2;
	char		*font3;
	char		*font4;
	char		*font5;
	char		*font6;
	Pixel		hilite_fg;
	Pixel		hilite_bg;
	int		multiClickTime;

/* private: */
	XFontStruct	*fontp;
	XFontStruct	*line_fontp;	/* Font for line-drawing graphics. */
	/***********************************************/
	/*   Need  to  keep  track  of size of window  */
	/*   allocated  so  if  window  is resized to  */
	/*   be  larger  than what we have we need to  */
	/*   allocate a new window.		       */
	/***********************************************/
	int	win_width_allocated;
	int	win_height_allocated;

	/***********************************************/
	/*   Graphics contexts			       */
	/***********************************************/
	GC	gc;		/* Normal text.		*/
	GC	line_gc;	/* Line-drawing GC.	*/
	GC	cursor_gc;	/* For drawing cursor.	*/

	int	font_height;
	int	font_width;
	int	char_set;

	int	x, y;
	int	saved_x, saved_y;
	int	cursor_visible;
	XtIntervalId	cursor_timer;
	enum cursor_type	cursor_state;
	int	have_focus;	/* Used for cursor flashing.	*/

	char	escbuf[MAX_ESCBUF];	/* Used to assemble escape sequences*/
	char	*escp;

	int	num_exposures;	/* Number of Exposure events to wait for. */

	int	old_top_line;
	int	top_line;

	int	scroll_top;	/* Top and bottom scrolling region lines. */
	int	scroll_bot;

	int	old_rows;
	int	old_columns;
	vbyte_t	*memory;	/* Pointer to originally allocated */
				/* memory.			   */
	vbyte_t	**orig_lines;	/* Array of lines pointers.	   */
	vbyte_t	**lines;	/* Array of lines corresponding to screen */

	Time	timestamp;	/* Time of last event.	*/
	char	*sel_string;	/* Copy of selected text.	*/
	int	sel_length;
	int	sel_start_x, sel_start_y; /* Starting co-ordinate of hilite*/
	int	sel_cur_x, sel_cur_y;	/* Current motion position.	*/
	int	num_clicks;	/* Number of button clicks for selecting text */
	/***********************************************/
	/*   ANSI escape attributes.		       */
	/***********************************************/
	int	old_color;
	vbyte_t	attr;		/* Current display attributes.		*/
	int	flags[CTW_MAX_ATTR];
} CtwPart;

typedef struct _CtwRec {
    CorePart		core;
    CtwPart		ctw;
} CtwRec;

#endif /* _CtwP_h */
