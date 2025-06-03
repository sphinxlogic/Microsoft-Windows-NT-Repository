/* This is my first attempt at X for JED. */
/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

#ifndef VMS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* #include <X11/Xos.h> */
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#else
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
/* #include <decw$include/Xos.h> */
#include <decw$include/Xatom.h>
#include <decw$include/keysym.h>
#include <decw$include/cursorfont.h>
#endif
#include <stdio.h>

#include "config.h"
/* #include "xterm.h"*/
#include "display.h"
#include "sysdep.h"
#include "screen.h"
#include "keymap.h"
#include "slang.h"
#include "hooks.h"
#include "ins.h"
#include "ledit.h"
#include "misc.h"
#include "cmds.h"

int JX_Screen_Cols;
int JX_Screen_Rows;
int JX_Term_Cannot_Scroll = 0;
int JX_Term_Cannot_Insert = 0;
int JX_Baud_Rate = 0;
int JX_Use_Ansi_Colors = 1;
int JX_Ignore_Beep = 3;


typedef struct 
{
   GC gc;
   unsigned long fg, bg;
   char *fg_name;
   char *bg_name;
} GC_Info_Type;

typedef struct
{
   Window w;
   
   int height, width;
   int border;			       /* inside border */
   int o_border;		       /* outside border */
   Colormap color_map;
   
   /* font information */
   XFontStruct *font;
   char *font_name;
   int font_height, font_width, font_base;
   
   /* GC info */
   GC_Info_Type *text_gc;
   GC current_gc;
   GC_Info_Type pointer_gc;
   
   int vis_curs_row, vis_curs_col;     /* position of VISIBLE cursor */
   
   /* flags */
   int cursor_showing;		       /* true if widow has cursor showing */
   int focus;			       /* true if window has focus */
   int window_mapped;		       /* true if window is mapped */
   
   /* Window tty parameters */
   int insert_mode;		       /* true if inserting */
   int scroll_r1,  scroll_r2;	       /* scrolling region */
   int cursor_row, cursor_col;	       /* row column of cursor (0, 0) origin */
   
   int visible;			       /* from visibilitynotify */
   Cursor mouse;
   
} JXWindow_Type;
   
static JXWindow_Type This_X_Window;
static JXWindow_Type *XWin;

static Display *This_XDisplay;
static Window This_XWindow;
static int This_XScreen;
static int Performing_Update;
static int No_XEvents;		       /* if true, do nothing */

typedef struct 
{
   char *name;
   char *name1;
   int type;
   char *value;
   char **dflt;
}
XWindow_Arg_Type;

static char *This_App_Name = "XJed";
static char *This_App_Title = "XJed";
#define Default_Geometry "80x24+0-0"
static char *This_Geometry = NULL;
static char *This_Font_Name = "fixed";
static char *This_Border_Width_Name = "0";
static char *This_MFG = "green";
static char *This_MBG = "white";

static GC_Info_Type Default_GC_Info[JMAX_COLORS] = 
{
   {NULL, 0, 0, "black", "white"},     /* NORMAL */
   {NULL, 0, 0, "green", "red"},       /* CURSOR */
   {NULL, 0, 0, "black", "skyblue"},   /* STATUS */
   {NULL, 0, 0, "black", "magenta"},      /* REGION */
   {NULL, 0, 0, "black", "skyblue"},      /* MENU */
   {NULL, 0, 0, "black", "white"},     /* operator */
   {NULL, 0, 0, "green", "white"},     /* numbers */
   {NULL, 0, 0, "blue", "white"},      /* strings */
   {NULL, 0, 0, "black", "gray"},      /* comments */
   {NULL, 0, 0, "red", "white"},       /* keyword */
   {NULL, 0, 0, "black", "white"},      /* delimeters */
   {NULL, 0, 0, "magenta", "white"}      /* preprocess */
};

static XWindow_Arg_Type X_Arg_List[] =
{
   /* These MUST be in this order!!! */
   {"Display",		"d", 	STRING_TYPE,	NULL,	NULL},
   {"Name",		NULL,	STRING_TYPE,	NULL,	&This_App_Name},
   {"Geometry",		NULL, 	STRING_TYPE,	NULL,	&This_Geometry},

   /* These are in arbitrary order */
   {"Font",		"fn",	STRING_TYPE,	NULL,	&This_Font_Name},
   {"Mouse_fg",		"mfg", 	STRING_TYPE,	NULL,	&This_MFG},
   {"Mouse_bg",		"mbg", 	STRING_TYPE,	NULL,	&This_MBG},
   {"Background",	"bg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JNORMAL_COLOR].bg_name},
   {"Foreground",	"fg",	STRING_TYPE,	NULL,	&Default_GC_Info[JNORMAL_COLOR].fg_name},
   {"Status_fg",	"sfg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JSTATUS_COLOR].fg_name},
   {"Status_bg",	"sbg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JSTATUS_COLOR].bg_name},
   {"Region_fg",	"rfg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JREGION_COLOR].fg_name},
   {"Region_bg",	"rbg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JREGION_COLOR].bg_name},
   {"Cursor_fg",	"cfg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JCURSOR_COLOR].fg_name},
   {"Cursor_bg",	"cbg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JCURSOR_COLOR].bg_name},
   {"Menu_fg",		"mfg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JMENU_COLOR].fg_name},
   {"Menu_bg",		"mbg", 	STRING_TYPE,	NULL,	&Default_GC_Info[JMENU_COLOR].bg_name},
   {"Operator_fg",	"opfg",	STRING_TYPE,	NULL,	&Default_GC_Info[JOP_COLOR].fg_name},
   {"Operator_bg",	"opbg",	STRING_TYPE,	NULL,	&Default_GC_Info[JOP_COLOR].bg_name},
   {"Number_fg",	"nmfg",	STRING_TYPE,	NULL,	&Default_GC_Info[JNUM_COLOR].fg_name},
   {"Number_bg",	"nmbg",	STRING_TYPE,	NULL,	&Default_GC_Info[JNUM_COLOR].bg_name},
   {"String_fg",	"stfg",	STRING_TYPE,	NULL,	&Default_GC_Info[JSTR_COLOR].fg_name},
   {"String_bg",	"stbg",	STRING_TYPE,	NULL,	&Default_GC_Info[JSTR_COLOR].bg_name},
   {"Comments_fg",	"cofg",	STRING_TYPE,	NULL,	&Default_GC_Info[JCOM_COLOR].fg_name},
   {"Comments_bg",	"cobg",	STRING_TYPE,	NULL,	&Default_GC_Info[JCOM_COLOR].bg_name},
   {"Keyword_fg",	"kwfg",	STRING_TYPE,	NULL,	&Default_GC_Info[JKEY_COLOR].fg_name},
   {"Keyword_bg",	"kwbg",	STRING_TYPE,	NULL,	&Default_GC_Info[JKEY_COLOR].bg_name},
   {"Delimiter_fg",	"defg",	STRING_TYPE,	NULL,	&Default_GC_Info[JDELIM_COLOR].fg_name},
   {"Delimiter_bg",	"debg",	STRING_TYPE,	NULL,	&Default_GC_Info[JDELIM_COLOR].bg_name},
   {"Preprocess_fg",	"prfg",	STRING_TYPE,	NULL,	&Default_GC_Info[JPREPROC_COLOR].fg_name},
   {"Preprocess_bg",	"prbg",	STRING_TYPE,	NULL,	&Default_GC_Info[JPREPROC_COLOR].bg_name},

   {"Borderwidth",	"bw", 	STRING_TYPE,	NULL,	&This_Border_Width_Name},
   {"Title",		NULL,	STRING_TYPE,	NULL,	&This_App_Title},
   {"Bordercolor",	"bd", 	STRING_TYPE,	NULL,	NULL},
   {"Iconic",		NULL,	VOID_TYPE,	NULL,	NULL},
   {"xrm",		NULL,	STRING_TYPE,	NULL,	NULL},
   {NULL,		NULL,	0,		NULL,	NULL}
};


static void hide_cursor (void)
{
   unsigned short *s;
   char ch;
   int b = XWin->border, col = XWin->vis_curs_col, row = XWin->vis_curs_row;

   if (No_XEvents) return;
   if (XWin->cursor_showing == 0) return;
   XWin->cursor_showing = 0;
   
   s = JScreen[row].old;
   if (s == NULL) return;
   s += col;
   ch = (char) (*s & 0xFF);
   
   XDrawImageString(This_XDisplay, XWin->w,
		    XWin->text_gc[*s >> 8].gc,
		    col * XWin->font_width + b,
		    row * XWin->font_height + b + XWin->font_base,
		    &ch, 1);
   
}

/* This routine assumes that cursor is in the correct location.  The 
   cursor is placed at the end of the string. */
static void tt_write(char *s, int n)
{
   int b = XWin->border;
   if (XWin->window_mapped == 0) return;
   if (No_XEvents) return;
   if (XWin->cursor_showing) hide_cursor ();
   XDrawImageString(This_XDisplay, This_XWindow, XWin->current_gc,
		    (XWin->cursor_col) * XWin->font_width + b,
		    (XWin->cursor_row) * XWin->font_height + b + XWin->font_base,
		    s, n);
		   
   XWin->cursor_col += n;
   if (XWin->cursor_col >= JX_Screen_Cols) XWin->cursor_col = JX_Screen_Cols - 1;
}



static void copy_rect(int x1, int y1, int x2, int y2, int x3, int y3)
{
   int w, h;
   
   if (No_XEvents) return;
   if (XWin->window_mapped == 0) return;

   w = (x2 - x1) * XWin->font_width;
   h = (y2 - y1) * XWin->font_height;
   
   if ((w <= 0) || (h <= 0)) return;
   
   x3 = XWin->border + x3 * XWin->font_width; 
   x1 = XWin->border + x1 * XWin->font_width; 
   y3 = XWin->border + y3 * XWin->font_height;
   y1 = XWin->border + y1 * XWin->font_height;
   hide_cursor ();
   XCopyArea (This_XDisplay, This_XWindow, This_XWindow, XWin->current_gc,
	      x1, y1, w, h, x3, y3);
}

static void blank_rect (int x1,  int y1, int x2, int y2)
{
   int w, h;
   
   if (No_XEvents) return;
   if (XWin->window_mapped == 0) return;

   w = (x2 - x1) * XWin->font_width;
   h = (y2 - y1) * XWin->font_height;
   
   if ((w <= 0) || (h <= 0)) return;
   
   x1 = XWin->border + x1 * XWin->font_width; 
   y1 = XWin->border + y1 * XWin->font_height;
   hide_cursor ();
   XClearArea (This_XDisplay, This_XWindow, x1, y1, w, h, 0);
}

void JX_set_scroll_region(int r1, int r2)
{
    XWin->scroll_r1 = r1;
    XWin->scroll_r2 = r2;
}

void JX_reset_scroll_region (void)
{
   XWin->scroll_r1 = 0;
   XWin->scroll_r2 = JX_Screen_Rows - 1;
}


static void show_cursor (void)
{
   unsigned short *s;
   char ch;
   int c, r, b;
   GC gc;
   
   if (No_XEvents) return;
   if (XWin->cursor_showing) hide_cursor ();

   XWin->cursor_showing = 1;
   r = XWin->vis_curs_row = XWin->cursor_row;
   c = XWin->vis_curs_col = XWin->cursor_col;
   b = XWin->border;
   
   s = JScreen[r].old;
   if (s == NULL) return;
   
   s += c;
   ch = (char) (*s & 0xFF);
   gc = XWin->text_gc[JCURSOR_COLOR].gc;
      
   if (XWin->focus)
     {
	XDrawImageString(This_XDisplay, This_XWindow, 
			 gc, 
			 c * XWin->font_width + b,
			 r * XWin->font_height + b + XWin->font_base,
			 &ch, 1);
     }
   else
     {
	XDrawRectangle( This_XDisplay, This_XWindow, 
		       gc,
		       c * XWin->font_width + b,
		       r * XWin->font_height + b,
		       XWin->font_width - 1,
		       XWin->font_height - 1);
     }
   
   XFlush(This_XDisplay);
}


static void toggle_cursor (int on)
{
   if (on)
     {
	if (XWin->focus) return;
	XWin->focus = 1;
     }
   else 
     {
	if (XWin->focus == 0) return;
	XWin->focus = 0;
     }
   show_cursor ();
}

   
void JX_write_string (char *s)
{
   tt_write(s, strlen(s));
   if (Performing_Update) return;
   show_cursor ();
}
   

void JX_goto_rc(int r, int c)
{
   if (XWin == NULL) return;
   if (XWin->cursor_showing) hide_cursor ();
   if (r >= JX_Screen_Rows) r = JX_Screen_Rows - 1; 
   if (c >= JX_Screen_Cols) c = JX_Screen_Cols - 1; 
   XWin->cursor_row = r + XWin->scroll_r1;
   XWin->cursor_col = c;
   if (Performing_Update) return;
   show_cursor ();
}

void JX_begin_insert(void)
{
   XEvent ev;
   
   if (No_XEvents) return;
   hide_cursor ();
   copy_rect(XWin->cursor_col, XWin->cursor_row, JX_Screen_Cols - 1, XWin->cursor_row + 1,
	     XWin->cursor_col + 1, XWin->cursor_row);
   XWin->insert_mode = 1;
   
   /* The noexpose events really mess up input pending timing.  
      Stop them now. */
   XFlush(This_XDisplay);
   if (!XPeekEvent(This_XDisplay, &ev) || (ev.type != NoExpose)) return;
   XNextEvent(This_XDisplay, &ev);
}

void JX_end_insert(void)
{
   XWin->insert_mode = 0;
}

void JX_delete_char (void)
{
   copy_rect(XWin->cursor_col + 1, XWin->cursor_row, JX_Screen_Cols, XWin->cursor_row + 1,
	     XWin->cursor_col, XWin->cursor_row);
}

void JX_erase_line(void)
{
   if (XWin == NULL) return;
   blank_rect(0, XWin->cursor_row, JX_Screen_Cols, XWin->cursor_row + 1);
}

/* Must respect scrolling region */
void JX_delete_nlines(int n)
{
   int r1, r2;
   r1 = XWin->cursor_row;
   r2 = XWin->scroll_r2;
   
   if (r1 <= r2 - n) copy_rect(0, r1 + n, JX_Screen_Cols, r2 + 1,
			       0, r1);
   
   if (Scroll_By_Copying == 0) blank_rect(0, r2 - n, JX_Screen_Cols, r2);
}


void JX_reverse_index(int n)
{
   int r1, r2;
   r1 = XWin->scroll_r1;
   r2 = XWin->scroll_r2;
   
   if (r2 >= r1 + n) copy_rect(0, r1, JX_Screen_Cols, r2 - n + 1,
			       0, r1 + n);
   
   if (Scroll_By_Copying == 0) blank_rect(0, r1, JX_Screen_Cols, r1 + n); 
}

void JX_beep(void)
{
   GC gc;
   XGCValues gcv;
   
   if (No_XEvents) return;
   flush_input();
   if (JX_Ignore_Beep & 0x1) XBell (This_XDisplay, 50);
   
   /* visible bell */

   if (JX_Ignore_Beep & 0x2)
     {
	gc = XCreateGC(This_XDisplay, This_XWindow, 0, &gcv);
	
        XSetState(This_XDisplay, gc, 
		  WhitePixel (This_XDisplay, This_XScreen),
                  BlackPixel(This_XDisplay, This_XScreen), 
		  GXinvert, AllPlanes);

        XFillRectangle (This_XDisplay, This_XWindow, gc, 
                        0, 0,
			XWin->font_width * JX_Screen_Cols,
			XWin->font_height * JX_Screen_Rows);
                        
        XFlush (This_XDisplay);
	
	/* I attempted to put a pause in here but it was too slow. */
	
	
        XFillRectangle (This_XDisplay, This_XWindow, gc, 
                        0, 0,
			XWin->font_width * JX_Screen_Cols,
			XWin->font_height * JX_Screen_Rows);
	
	XFreeGC(This_XDisplay, gc);
     }   
   XFlush (This_XDisplay);
}

void JX_del_eol(void)
{
   blank_rect(XWin->cursor_col, XWin->cursor_row, JX_Screen_Cols, XWin->cursor_row + 1);
}

static int Rev_Vid_Flag;
void JX_reverse_video(int color)
{
   Rev_Vid_Flag = color;
   XWin->current_gc = XWin->text_gc[color].gc;
}

void JX_normal_video(void)
{
   if (XWin == NULL) return;
   Rev_Vid_Flag = JNORMAL_COLOR;
   XWin->current_gc = XWin->text_gc[JNORMAL_COLOR].gc;
}




static void send_attr_str(unsigned short *s, unsigned short *smax)
{
   unsigned char out[250], ch, attr, *p;
   register unsigned short sh;
   
   p = out;
   while (s < smax)
     {
	sh = (unsigned short) *s++;
	ch = sh & 0xFF;
	attr = sh >> 8;
	if ((attr == 0) && (Rev_Vid_Flag != 0))
	  {
	     if (p != out)
	       {
		  *p = 0;
		  JX_write_string ((char *) out);
		  p = out;
	       }
	     tt_normal_video();
	     /* Rev_Vid_Flag = 0; */
	  }
	else if ((attr != 0) && (Rev_Vid_Flag != attr))
	  {
	     if (p != out)
	       {
		  *p = 0;
		  JX_write_string ((char *) out);
		  p = out;
	       }
	     JX_reverse_video(attr);
	     /* Rev_Vid_Flag = 1; */
	  }
	*p++ = ch;
     }
   *p = 0;
   if (p != out) JX_write_string ((char *) out);
   /* if (Rev_Vid_Flag) tt_normal_video(); */
}



#define SPACE_CHAR (32 | (JNORMAL_COLOR << 8))

static void forward_cursor (int n, int row)
{
   JX_goto_rc (row, XWin->cursor_col + n);
}

void JX_smart_puts(unsigned short *neww,unsigned short *oldd, int len, int row)
{
   register unsigned short *p, *q, *qmax, *pmax, *buf;
   unsigned short buffer[256];
   int n_spaces;
   unsigned short *space_match, *last_buffered_match;
   
   q = oldd; p = neww;
   qmax = oldd + len;
   pmax = p + len;
   
   /* Find out where to begin --- while they match, we are ok */
   for (;;)
     {
	if (q == qmax) return;
	if (*q != *p) break;
	q++; p++;
     }

   /*position the cursor */
   JX_goto_rc (row, (int) (p - neww));

   /* Find where the last non-blank character on old/new screen is */
   
   while (qmax > q)
     {
	qmax--;
	if (*qmax != SPACE_CHAR) 
	  {
	     qmax++;
	     break;
	  }
     }
   /* if (*qmax != SPACE_CHAR) qmax++; */
   while (pmax > p)
     {
	pmax--;
	if (*pmax != SPACE_CHAR) 
	  {
	     pmax++;
	     break;
	  }
     }
   
   last_buffered_match = buf = buffer;		       /* buffer is empty */
   
   /* loop using overwrite then skip algorithm until done */
   while (1)
     {
	/* while they do not match and we do not hit a space, buffer them up */
	n_spaces = 0;
	while (p < pmax)
	  {
	     if ((*q == SPACE_CHAR) && (*p == SPACE_CHAR))
	       {
		  /* If *q is not a space, we would have to overwrite it.  
		   * However, if *q is a space, then while *p is also one, 
		   * we only need to skip over the blank field.
		   */
		  space_match = p;
		  p++; q++;
		  while ((p < pmax) 
			 && (*p == SPACE_CHAR) && (*q == SPACE_CHAR))
		    {
		       p++;
		       q++;
		    }
		  n_spaces = (int) (p - space_match);
		  break;
	       }
	     if (*q == *p) break;
	     *buf++ = *p++;
	     q++;
	  }
	*buf = 0;
	
	if (buf != buffer) send_attr_str (buffer, buf);
	buf = buffer;
	
	if (n_spaces && (p < pmax)) 
	  {
	     forward_cursor (n_spaces, row);
	  }
	
	/* Now we overwrote what we could and cursor is placed at position 
	 * of a possible match of new and old.  If this is the case, skip 
	 * some more.
	 */
	
	while ((p < pmax) && (*p == *q))
	  {
	     *buf++ = *p++;
	     q++;
	  }
	
	last_buffered_match = buf;
	if (p >= pmax) break;
	
	/* jump to new position is it is greater than 5 otherwise
	 * let it sit in the buffer and output it later.
	 */
	if ((int) (buf - buffer) >= 5) 
	  {
	     forward_cursor (buf - buffer, row);
	     last_buffered_match = buf = buffer;
	  }
     }
   if (buf != buffer)
     {
	if (q < qmax)
	  {
	     if ((buf == last_buffered_match) 
		 && ((int) (buf - buffer) >= 5))
	       {
		  forward_cursor (buf - buffer, row);
	       }
	     else
	       {
		  *buf = 0;
		  send_attr_str (buffer, buf);
	       }
	  }
     }
   if (q < qmax) JX_del_eol ();
}  

static int push_mouse_event (int type, int x, int y, unsigned int button, unsigned int state )
{
   register unsigned int s = 0, but;
   int ch;
   static int last_button;

   if (button == 0) button = last_button;
   JMouse.type = (type == ButtonPress) ? 1 : 0;
   if (JMouse.type) last_button = button;
   
   JMouse.x = 1 + (x - XWin->border) / XWin->font_width;
   JMouse.y = 1 + (y - XWin->border) / XWin->font_height;
   but = JMouse.button = (int) button;
   if (but == Button1) ch = 'l'; else if (but == Button2) ch = 'm'; else ch = 'r';
   if (state & Button1Mask) s |= 1;
   if (state & Button2Mask) s |= 2;
   if (state & Button3Mask) s |= 4;
   if (state & ShiftMask) 
     {
	s |= 16;
	ch = 'A' + (ch - 'a');
     }
   if (state & ControlMask) 
     {
	s |= 8;
	if (ch >= 'a') ch = ch - 'a' + 1;
     }
   JMouse.state = s;
   return ch;
}

static void cover_exposed_area (int x, int y, int width, int height, int count)
{
   unsigned short *s, *smax;
   int row, save_row, save_col, max_col, max_row, col;
   
   Performing_Update++;
   hide_cursor ();
   save_row = XWin->cursor_row;
   save_col = XWin->cursor_col;
   col = (x - XWin->border) / XWin->font_width;
   row = (y - XWin->border) / XWin->font_height;
   
   max_col = 2 + col + width / XWin->font_width;
   max_row = 2 + row + height / XWin->font_height;
   if (max_col > JX_Screen_Cols) max_col = JX_Screen_Cols;
   if (max_row > JX_Screen_Rows) max_row = JX_Screen_Rows;
   
   for (XWin->cursor_row = row; XWin->cursor_row < max_row; XWin->cursor_row++)
     {
	XWin->cursor_col = col;
	s = JScreen[XWin->cursor_row].old + XWin->cursor_col;
	smax = JScreen[XWin->cursor_row].old + max_col;
	send_attr_str(s, smax);
     }
   XWin->cursor_row = save_row;
   XWin->cursor_col = save_col;
   Performing_Update--;

   if (count == 0) show_cursor ();
}

#include "xkeys.c"

/* Return 1 if event is listed in the switch or zero otherwise.  The switch
 * events are considered harmless--- that is, processing them does not really
 * interfere with internal JED state (redisplay, etc...).  More bluntly, 
 * harmless means that the events can be processesed while checking for 
 * pending input.
 */
static int x_handle_harmless_events (XEvent *report)
{
   switch (report->type)
     {
      case EnterNotify:
	toggle_cursor(report->xcrossing.focus);
	break;
	     
      case LeaveNotify:
	/* toggle_cursor(0); */
	break;
	     
      case UnmapNotify: 
	XWin->window_mapped = 0;
	break;
      case MapNotify:
	XWin->window_mapped = 1;
	break;
	
      case FocusIn:
	toggle_cursor(1);
	break;	     
	
      case FocusOut:
	toggle_cursor(0);
	break;
	
      case VisibilityNotify: XWin->visible = report->xvisibility.state;
	break;
	
      case GraphicsExpose: 
	cover_exposed_area (report->xgraphicsexpose.x,
			    report->xgraphicsexpose.y,
			    report->xgraphicsexpose.width,
			    report->xgraphicsexpose.height,
			    report->xgraphicsexpose.count);
	break;
	     
      case NoExpose: break;

      default:
	/* fprintf(stderr, "harmless: %d\n", report->type); */
	return 0;
     }
   return 1;
}

static int X_Alt_Char = 27;

/* if force is true, wait for an event.  If force is false, only
 *  process events that exist.  This will return either when there 
 *  are no more events or a key/mouse event is processed returning
 *  1 in the process */
static int X_process_events (int force, char *buf, int *n_chars)
{
   XEvent report;
   int ch1;
   int block_expose = 0;
   char *bufp;
   KeySym ks;
   int esc = 27;
   

   Window root, child;
   int posx, posy, rootx, rooty;
   unsigned int keys_buttons;
   int last_x, last_y;
   static int last_event, motion_button;
   static unsigned int motion_state;
   
   int width, height;
   
   while (force || XPending(This_XDisplay))
     {
	XNextEvent(This_XDisplay, &report);
	
	switch  (report.type) 
	  {
	   case MotionNotify:
	     
	     while (XCheckMaskEvent (This_XDisplay, ButtonMotionMask, &report));
	     if (!XQueryPointer(This_XDisplay, report.xmotion.window,
				&root, &child, &rootx, &rooty, &posx, &posy,
				&keys_buttons)) break;

	     /* This will ensure that modifier keys are not pressed while
	        we are in motion. */

	     if ((last_event == MotionNotify) && (motion_state != keys_buttons)) break;
	     motion_state = keys_buttons;

	     last_x = JMouse.x;
	     last_y = JMouse.y;
	     
	     
	     ch1 = push_mouse_event (ButtonPress,
				     posx, posy,
				     0,
				     keys_buttons);

	     if ((last_x == JMouse.x) && (last_y == JMouse.y)) break;

	     /* return ESC ^@ ^@ .... */
	     *buf++ = esc; *buf++ = 0; *buf++ = 0; *buf++ = ch1;
	     *n_chars = 4;
	     
	     last_event = MotionNotify;
	     motion_button = JMouse.button;
	     return 1;
	     
	     
	   case Expose:
	     
	     if (block_expose == 0) cover_exposed_area (report.xexpose.x,
							report.xexpose.y,
							report.xexpose.width,
							report.xexpose.height,
							report.xexpose.count);
	     else 
	       {
		  if (report.xexpose.count == 0)
		    {
		       redraw_screen (1);
		       block_expose = 0;
		    }
	       }
	     break;
	     
	   case ConfigureNotify:
	     width = report.xconfigure.width;
	     height = report.xconfigure.height;
	     if ((width != XWin->width) || 
		 (height != XWin->height))
	       {
		  XWin->width = width;
		  XWin->height = height;
		  reset_display ();
		  init_display(1);
		  block_expose = -1;
	       }
	     break;
	     
	   case ButtonPress:
	     /* Prohibit dragging more than one button at a time. */
	     if (last_event == MotionNotify) break;
	     /* drop */
	     
	   case ButtonRelease:
	     if ((last_event == MotionNotify) && 
		 (report.xbutton.button != motion_button)) break;
	     
	     last_event = 0;
	     
	     /* ESC ^@ U/D is a  mouse prefix */
	     *buf++ = esc; *buf++ = 0;
	     if (report.type == ButtonPress) ch1 = 'D'; else ch1 = 'U';
	     *buf++ = ch1;
	     
	     *buf++ = push_mouse_event (report.xbutton.type,
					report.xbutton.x, report.xbutton.y,
					report.xbutton.button,
					report.xbutton.state);
	     *n_chars = 4;
	     return 1;
	     break;
	     
	   case KeyPress:
	     bufp = buf;
	     *n_chars = XLookupString(&report.xkey, buf, 20, &ks, NULL);
	     
	     ks = ks & 0xFFFF;
	     if (ks >= 0xFF00)
	       {
		  ks = ks & 0xFF;
		  if (report.xkey.state & ShiftMask) bufp = Shift_KeySym_Mapping [ks];
		  else if (report.xkey.state & ControlMask) bufp = Control_KeySym_Mapping [ks];
		  else bufp = KeySym_Mapping [ks];
		  
		  if (0 != (*n_chars = (int) *bufp++)) MEMCPY(buf, bufp, *n_chars);
	       }
	     else if ((*n_chars == 1) && (report.xkey.state & Mod1Mask))
	       {
		  if (X_Alt_Char <= 0)
		    {
		       *buf |= 0x80;
		    }
		  else
		    {
		       ch1 = *bufp;
		       *bufp++ = (unsigned char) X_Alt_Char;
		       *bufp = (unsigned char) ch1;
		       *n_chars = 2;
		    }
	       }
	     
	     if (*n_chars == 0) break;
	     return 1;
	  
	   default:
	     (void) x_handle_harmless_events (&report);
	  }   
     }
   return 0;
}

static int X_read_key (void)
{
   int nread;
   char buf[30];
   (void) X_process_events (1, buf, &nread);
   if (nread > 1) ungetkey_string(buf + 1, nread - 1);
   return (int) *buf;
}

static int X_input_pending (void)
{
   XEvent ev;
   int n;
   
   if (No_XEvents) return 0;

   n = XPending (This_XDisplay);
   if (!n) return (0);
   
   /* I need some way of getting only kbd events. */ 
   while (n--)
     {
	XPeekEvent(This_XDisplay, &ev);
	if (0 == x_handle_harmless_events (&ev)) return 1;
	XNextEvent(This_XDisplay, &ev);
     }
   return 0;
}


static void X_Get_Term_Size(int *cols, int *rows)
{
   *cols = (XWin->width - XWin->border) / XWin->font_width;
   *rows = (XWin->height - XWin->border) / XWin->font_height;
}

void JX_set_term_vtxxx (int *n)
{
}


void JX_narrow_width (void)
{
}
void  JX_wide_width (void)
{
}

void JX_enable_cursor_keys(void)
{
}

void JX_cls(void)
{
   if (No_XEvents) return;
   if (XWin->window_mapped == 0) return;
   XClearWindow(This_XDisplay, This_XWindow);
}

/* This function is called assuming that cursor is in correct 
   position */
void JX_putchar(char ch)
{
   if (ch == '\b')
     {
	ch = ' ';
	if (XWin->cursor_col == 0) return;
	XWin->cursor_col--;
     }
   
   if (Rev_Vid_Flag != JNORMAL_COLOR) tt_normal_video();
   tt_write(&ch, 1);
   show_cursor ();
}

/* This routine is called from S-Lang inner interpreter.  It serves
   as a poor mans version of an interrupt 9 handler */
static void xjed_check_kbd(void)
{
   char buf[30];
   int n;
   
   if (Batch || No_XEvents) return;
   while (XPending(This_XDisplay))
     {
	if (X_process_events (0, buf, &n) == 0) continue;
	
	if (*buf == Abort_Char) 
	  {
	     SLang_Error = USER_BREAK;
	     return;
	  }
	buffer_keystring (buf, n);
     }
}

static void xjed_suspend (void)
{
   if (No_XEvents) return;
   /* XIconifyWindow (This_XDisplay, XWin->w, This_XScreen); */
   if (XWin->visible == VisibilityUnobscured) XLowerWindow (This_XDisplay, This_XWindow);
   else XRaiseWindow (This_XDisplay, This_XWindow);
   return;
}

static int load_font (char *font)
{   
   static XFontStruct *xfont;

   xfont = XLoadQueryFont(This_XDisplay, font);
   if (xfont == NULL) return 0;
   
   XWin->font = xfont;
   XWin->font_name = font;
   XWin->font_height = XWin->font->ascent + XWin->font->descent;
   XWin->font_width = XWin->font->max_bounds.width;
   XWin->font_base = XWin->font->ascent;
   return 1;
}



static void get_xdefaults (void)
{
   XWindow_Arg_Type *xargs = X_Arg_List + 2;  /* skip display, name, etc */
   
   while (xargs->name != NULL)
     {
	if ((xargs->type != VOID_TYPE) && (xargs->dflt != NULL))
	  {
	     if (xargs->value == NULL)
	       {
		  xargs->value = XGetDefault(This_XDisplay, This_App_Name, xargs->name);
	       }

	     if (xargs->value != NULL) 
	       {
		  *xargs->dflt = xargs->value;
	       }
	  }
	xargs++;
     }
}



static void set_window_name (char *s)
{
   if (Batch) return;
   XStoreName (This_XDisplay, XWin->w, s);
   XSetIconName(This_XDisplay, XWin->w, s);
}

static void set_wm_hints (JXWindow_Type *w, int xpos,  int ypos, int orflags)
{
   XSizeHints h;
   XWMHints h1;
#if 0
   XClassHint ch = {"xjed", "XJed"};
#endif
   
   h.width_inc = w->font_width;
   h.height_inc = w->font_height;
   h.min_width = 5 * w->font_width + w->border;
   h.min_height = 5 * w->font_height + w->border;
   h.base_height = 0;
   h.base_width = 0;
   h.x = xpos; h.y = ypos;
   h.height = w->height;
   h.width = w->width;
   
   h.flags = PMinSize | PResizeInc | PBaseSize;
   h.flags |= orflags;
   
   XSetWMNormalHints(This_XDisplay, w->w, &h);
   
   /* This bit allows me to track the focus.  It is not at all clear from 
      the documentation. */
   h1.input = 1;
   h1.flags = InputHint;
   XSetWMHints(This_XDisplay, w->w, &h1);
#if 0
   XSetClassHint(This_XDisplay, w->w, &ch);
#endif
}

/* This parses the colors in the XWin structure and setting 
   defaults to fg, bg upon failure */
static void setup_ith_color (int i, unsigned long fg, unsigned long bg)
{
   XColor xcol;
   
   if (Term_Supports_Color &&
       XParseColor(This_XDisplay, XWin->color_map, XWin->text_gc[i].fg_name, &xcol)
       && XAllocColor(This_XDisplay, XWin->color_map, &xcol))
     {
	fg = xcol.pixel;
     }
   
   XWin->text_gc[i].fg = fg;
   
   if (Term_Supports_Color &&
       XParseColor(This_XDisplay, XWin->color_map, XWin->text_gc[i].bg_name, &xcol)
       && XAllocColor(This_XDisplay, XWin->color_map, &xcol))
     {
	bg = xcol.pixel;
     }
   
   XWin->text_gc[i].bg = bg;
}


/* This is used to set the colors in the Win structure and if f is non-zero,
   the previous definitions are freed.  f is 0 when the colors correspond to the 
   default. */

static void x_set_color_free (int i, char *fgcolor, char *bgcolor, int do_free)
{
   char *save_fg, *save_bg, *fg, *bg;
   unsigned long old_fg, old_bg;
   
   if (NULL == (fg = (char *) SLMALLOC(strlen(fgcolor) + 1)))
     {
	SLang_Error = SL_MALLOC_ERROR;
	return;
     }
   strcpy (fg, fgcolor);
   if (NULL == (bg = (char *) SLMALLOC(strlen(bgcolor) + 1)))
     {
	SLang_Error = SL_MALLOC_ERROR;
	SLFREE (fg);
	return;
     }
   strcpy (bg, bgcolor);
   
   old_fg = XWin->text_gc[i].fg;
   old_bg = XWin->text_gc[i].bg;
   
   save_fg = XWin->text_gc[i].fg_name;
   XWin->text_gc[i].fg_name = fg;
   save_bg = XWin->text_gc[i].bg_name;
   XWin->text_gc[i].bg_name = bg;
   
   setup_ith_color (i, old_fg, old_bg);
   if (do_free)
     {
	if (save_fg != NULL) SLFREE (save_fg);
	if (save_bg != NULL) SLFREE (save_bg);
     }
}



static void setup_and_parse_colors (void)
{
   unsigned long fg, bg, tmp;
   char *fg_name, *bg_name;

   int i;
   GC_Info_Type *d;
   
   /* Check to see if this is a color display */
   
   bg = WhitePixel (This_XDisplay, This_XScreen); bg_name = "white";
   fg = BlackPixel (This_XDisplay, This_XScreen); fg_name = "black";

   XWin->color_map = DefaultColormap (This_XDisplay, This_XScreen);

   if (DisplayCells (This_XDisplay, This_XScreen) > 2) 
     {
	Term_Supports_Color = 1;
     }
   else Term_Supports_Color = 0;
   
   for (i = 0; i < JMAX_COLORS; i++)
     {
	d = Default_GC_Info + i;
	/* The assumption here is that ALL colors beyond JNORMAL_COLOR (0)
	 * take reversed fg, bgs.  I really ought to have flags if this is
	 * not the case. */
	d->fg = fg;
	d->bg = bg;
	if (d->fg_name == NULL) d->fg_name = fg_name;
	if (d->bg_name == NULL) d->bg_name = bg_name;
	
	if (i == JNORMAL_COLOR)
	  {
	     tmp = fg; fg = bg; bg = tmp;
	     fg_name = bg_name;
	     bg_name = "black";
	  }
	
	x_set_color_free (i, d->fg_name, d->bg_name, 0);
     }
}

static void set_mouse_color (char *fgc, char *bgc)
{
   XColor xfg, xbg;
   
   if (!Term_Supports_Color) return;
   if (!XParseColor(This_XDisplay, XWin->color_map, fgc, &xfg)
       || !XAllocColor(This_XDisplay, XWin->color_map, &xfg))
     {
	return;
     }

   if (!XParseColor(This_XDisplay, XWin->color_map, bgc, &xbg)
       || !XAllocColor(This_XDisplay, XWin->color_map, &xbg))
     {
	return;
     }
   XRecolorCursor (This_XDisplay, XWin->mouse, &xfg, &xbg);
}



static void create_needed_gcs (void)
{
   int i;
   XGCValues xgcv;
   xgcv.font = XWin->font->fid;
   for (i = 0; i < JMAX_COLORS; i++)
     {
	xgcv.foreground = XWin->text_gc[i].fg;
	xgcv.background = XWin->text_gc[i].bg;
	
	XWin->text_gc[i].gc = XCreateGC(This_XDisplay, This_XWindow,
					GCForeground | GCBackground | GCFont,
					&xgcv);
     }
}



static Window create_XWindow (JXWindow_Type *win, char *geom, int orflags)
{
   int bdr, xpos, ypos;
   int width, height, geo;
   Window w;
   
   bdr = atoi(This_Border_Width_Name);
   
   width = JX_Screen_Cols; height = JX_Screen_Rows;   
   xpos = 0, ypos = 0;
   if (geom != NULL)
     {
	geo = XGeometry (This_XDisplay, This_XScreen,
			  geom, Default_Geometry,
			  bdr,
			  win->font_width, win->font_height,
			  win->border, win->border,
			  &xpos, &ypos, 
			  &width, &height);
        if ((geo & XValue) && (geo & YValue)) orflags |= USPosition;
        if ((geo & WidthValue) && (geo & HeightValue)) orflags |= USSize;
     }
   
   win->width = width * win->font_width + 2 * win->border;
   win->height = height * win->font_height + 2 * win->border;

   /* create and display window */
   w = XCreateSimpleWindow(This_XDisplay, RootWindow(This_XDisplay, This_XScreen),
			   xpos, ypos,     /* xpos, ypos */
			   win->width,     /* width, height */
			   win->height,     /* width, height */
			   bdr,	       /* border width */
			   win->text_gc[JNORMAL_COLOR].fg,
			   win->text_gc[JNORMAL_COLOR].bg 
			   );
   
   win->w = w;
   
   set_wm_hints (win, xpos, ypos, orflags);
   
   /* select event types */
   XSelectInput(This_XDisplay, w, 
		ExposureMask | KeyPressMask 
		| ButtonPressMask | ButtonReleaseMask
		| StructureNotifyMask
		| PointerMotionHintMask | ButtonMotionMask
		| EnterWindowMask
		/* | LeaveWindowMask */
		| FocusChangeMask
		| VisibilityChangeMask
		);
   
   if (XWin->mouse) XDefineCursor(This_XDisplay, w, XWin->mouse);
   return w;
}



static int x_err_handler (Display *d, XErrorEvent *ev)
{
   char errmsg[256];
   No_XEvents = 1;
   XGetErrorText (d, ev->error_code, errmsg, 255);
   exit_error (errmsg, 0);
   return 1;
}

static int x_ioerr_handler (Display *d)
{
   No_XEvents = 1;
   exit_error("XWindows IO error", 0);
   return d == NULL;  /* just use d to avoid a warning */
}

/* returns socket descriptor */
static int init_Xdisplay (void)
{
   int orflags = 0;
   char dname[80], *n;
   
   n = X_Arg_List[0].value;
   if (n != NULL)
     {
	strncpy (dname, X_Arg_List[0].value, 70);
	dname[70] = 0;
	n = dname;
	while (*n && (*n != ':')) n++;
	if (*n == 0) strcpy(n, ":0.0");
	n = dname;
     }
   
   XSetIOErrorHandler (x_ioerr_handler);

   if ( (This_XDisplay = XOpenDisplay(n)) == NULL )
     {
  	(void) fprintf( stderr, "XJED: cannot connect to X server %s\n", 
		       XDisplayName(n));
	exit( -1 );
     }
   
   XSetErrorHandler (x_err_handler);

   if (X_Arg_List[1].value != NULL) This_App_Name = X_Arg_List[1].value;
   XWin = &This_X_Window;
   get_xdefaults ();
   
   XWin->font_name = This_Font_Name;

   This_XScreen = DefaultScreen(This_XDisplay);

   if (!load_font(XWin->font_name))
     {
	(void) fprintf( stderr, "XJED: cannot load font %s, using fixed.\n", XWin->font_name);
	if (!load_font("fixed"))
	  {
	     (void) fprintf( stderr, "XJED: cannot load fixed font.\n");
	     exit( -1 );
	  }
     }
   
   
   XWin->text_gc = Default_GC_Info;
   setup_and_parse_colors ();		       /* This allocs and parses colors */
   
   XWin->mouse = XCreateFontCursor (This_XDisplay, XC_xterm);
   set_mouse_color (This_MFG, This_MBG);
   
   /* USPosition and USSize are now set in create_XWindow depending on
    * what the user specified in the geometry string */
   /* if (X_Arg_List[2].value != NULL) orflags = USPosition | USSize; */
   
   This_XWindow = create_XWindow(XWin, This_Geometry, orflags);
   
   set_window_name (This_App_Title);
   
   /* GCs and their colors */
   create_needed_gcs ();		       /* This uses info from previous call */
   
   XWin->current_gc = XWin->text_gc[JNORMAL_COLOR].gc;
   
   XSetFont (This_XDisplay, XWin->current_gc, XWin->font->fid);

   /* display window */
   
   XMapWindow(This_XDisplay, This_XWindow);
   
   return ConnectionNumber (This_XDisplay);
}


static void reset_Xdisplay (void)
{
   if (This_XDisplay != NULL) XCloseDisplay(This_XDisplay);
}


#define UPCSE(x)  (((x) <= 'z') && ((x) >= 'a') ? (x) - 32 : (x))
static int myXstrcmp(char *a, char *b)
{
   register char cha, chb;
   /* do simple comparison */
   
   cha = *a++;  chb = *b++;
   if ((cha != chb) && (UPCSE(cha) != UPCSE(chb))) return 0;
   while ((cha = *a++), (chb = *b++), (cha && chb) != 0)
     {
	if (cha != chb) return 0;
     }
   
   return (cha == chb);
}

#define STREQS(a, b) myXstrcmp(a, b)

static int X_eval_command_line (int argc, char **argv)
{
   char *arg;
   int i;
   XWindow_Arg_Type *opt;
   
   for (i = 1; i < argc; i++)
     {
	arg = argv[i];
	if (*arg != '-') break;
	arg++;
	opt = X_Arg_List;
	while (opt->name != NULL)
	  {
	     if (STREQS(opt->name, arg) 
		 || ((opt->name1 != NULL) && STREQS(opt->name1, arg))) break;
	     opt++;
	  }
	
	if (opt->name == NULL) break;
	
	if (opt->type == VOID_TYPE) opt->value = "on";
	else if (i + 1 < argc)
	  {
	     i++;
	     opt->value = argv[i];
	  }
	else break;
     }
   
   /* Out of this loop, argv[i] is the last unprocessed argument */
   return i;
}



void JX_set_color (int i, char *what, char *fg, char *bg)
{
   if (!Term_Supports_Color) return;

   if (i == -1)
     {
	if (!strcmp("mouse", what))
	  {
	     set_mouse_color (fg, bg);
	  }
	
	return;
     }
   
   x_set_color_free (i, fg, bg, 1);
   XSetForeground(This_XDisplay, XWin->text_gc[i].gc, XWin->text_gc[i].fg);
   XSetBackground(This_XDisplay, XWin->text_gc[i].gc, XWin->text_gc[i].bg);
}

static int X_Warp_Pending = 0;
static void x_warp_pointer (void)
{
   X_Warp_Pending = 1;
}

static void x_region_2_cutbuffer (void)
{
   int nbytes;
   char *dat;

   dat = make_buffer_substring(&nbytes);
   if (dat == NULL) return;

   XStoreBytes (This_XDisplay, dat, nbytes);
   SLFREE (dat);
}

static int x_insert_cutbuffer (void)
{
   int nbytes;
   char *dat;

   CHECK_READ_ONLY
   dat = XFetchBytes (This_XDisplay, &nbytes);
   if (nbytes && (dat != NULL)) ins_chars ((unsigned char *) dat, nbytes);
   if (dat != NULL) XFree (dat);
   return nbytes;
}

static void x_set_keysym (int *np, int *shift, char *str)
{
   unsigned int n = (unsigned int) *np;
   KeySym_Mapping_Type *map;
   
   n &= 0xFF;
   str = SLang_process_keystring (str);
   if (*shift == '$') map = Shift_KeySym_Mapping;
   else if (*shift == '^') map = Control_KeySym_Mapping;
   else map = KeySym_Mapping;
   MEMCPY (map[n], str, MAX_KEYSYM_STRING_LEN);
   
   *(map[n]) -= 1;
}

  
   
static SLang_Name_Type sl_x_table[] =
{
   MAKE_INTRINSIC(".x_set_window_name", set_window_name, VOID_TYPE, 1),
   MAKE_INTRINSIC(".x_warp_pointer", x_warp_pointer, VOID_TYPE, 0),
   MAKE_INTRINSIC(".x_insert_cutbuffer", x_insert_cutbuffer, INT_TYPE, 0),
   /*Prototype: Integer x_insert_cut_buffer ();
     Inserts cutbuffer into the current buffer and returns the number
     of characters inserted.
     */
   MAKE_INTRINSIC(".x_copy_region_to_cutbuffer", x_region_2_cutbuffer, VOID_TYPE, 0),
   /*Prototype: Void x_copy_region_to_cutbuffer();
    */
   MAKE_INTRINSIC(".x_set_keysym", x_set_keysym, VOID_TYPE, 3),
  /*Prototype Void x_set_keysym (Integer keysym, Integer shift, String str);
   * 
   * This function may be used to assocate a string 'str' with a key 'keysym'
   * modified by mask 'shift'. Pressing the key associated with 'keysym' will
   * then generate the keysequence given by 'str'. The function keys are
   * mapped to integers in the range 0xFF00 to 0xFFFF.  On most systems, the
   * keys that these mappings refer to are located in the file
   * /usr/include/X11/keysymdef.h. For example, on my system, the keysyms for
   * the function keys XK_F1 to XK_F35 fall in the range 0xFFBE to 0xFFE0.
   * So to make the 'F1' key correspond to the string given by the two
   * characters Ctrl-X Ctrl-C, simply use:
   * 
   *	  x_set_keysym (0xFFBE, 0, "^X^C");
   *	  
   * The 'shift' argument is an integer with the following meanings:
   * 
   *	 0   : unmodified key
   *	'$'  : shifted
   *	'^'  : control
   *	
   * Any other value for shift will default to 0 (unshifted).
   *	  
   */
   MAKE_VARIABLE(".ALT_CHAR", &X_Alt_Char, INT_TYPE, 0),

     
   /*Prototype: Void x_copy_region_to_cutbuffer();
    */
      
   SLANG_END_TABLE
};


static int X_init_slang (void)
{
   return (SLang_add_table(sl_x_table, "XJed") && SLdefine_for_ifdef("XWINDOWS"));
}


static void X_update_open (void)
{
   hide_cursor ();
   Performing_Update = 1;
}

static void X_update_close (void)
{
   Performing_Update = 0;
   if (XWin->window_mapped == 0) JWindow->trashed = 1;
   if (JWindow->trashed) return;
   show_cursor ();
   if (X_Warp_Pending) XWarpPointer (This_XDisplay, None, XWin->w, 0, 0, 0, 0,
				     XWin->vis_curs_col * XWin->font_width + XWin->border,
				     (XWin->vis_curs_row + 1) * XWin->font_height + XWin->border);
   X_Warp_Pending = 0;
}

static void x_define_xkeys (SLKeyMap_List_Type *map)
{
   SLang_define_key1("^[[^D", (VOID *) scroll_right, SLKEY_F_INTRINSIC, map);
   SLang_define_key1("^[[d", (VOID *) scroll_right, SLKEY_F_INTRINSIC, map);
   SLang_define_key1("^[[^C", (VOID *) scroll_left, SLKEY_F_INTRINSIC, map);
   SLang_define_key1("^[[c", (VOID *) scroll_left, SLKEY_F_INTRINSIC, map);
   SLang_define_key1("^[[a", (VOID *) bob, SLKEY_F_INTRINSIC, map);
   SLang_define_key1("^[[^A", (VOID *) bob, SLKEY_F_INTRINSIC, map);
   SLang_define_key1("^[[b", (VOID *) eob, SLKEY_F_INTRINSIC, map);
   SLang_define_key1("^[[^B", (VOID *) eob, SLKEY_F_INTRINSIC, map);
}

void JX_reset_video (void)
{
   tt_normal_video ();
}


void JX_init_video (void)
{
}


void flush_output (void)
{
   fflush (stdout);
}


void JX_get_terminfo (void)
{
   JX_Screen_Cols = 80;
   JX_Screen_Rows = 24;
   Scroll_By_Copying = 1;
   
   /* init hooks */
   X_Read_Hook = X_read_key;
   X_Input_Pending_Hook = X_input_pending; 
   X_Get_Term_Size_Hook = X_Get_Term_Size;
   X_Update_Open_Hook = X_update_open;
   X_Update_Close_Hook = X_update_close;
   X_Suspend_Hook = xjed_suspend;
   X_Argc_Argv_Hook = X_eval_command_line;
   X_Init_SLang_Hook = X_init_slang;
   X_Init_Term_Hook = init_Xdisplay;
   X_Reset_Term_Hook = reset_Xdisplay;
   X_Define_Keys_Hook = x_define_xkeys;
   SLang_Interrupt = xjed_check_kbd;   
}

/* the links to functions and variables here */
void (*tt_goto_rc)(int, int)  		= JX_goto_rc;
void (*tt_begin_insert)(void)  		= JX_begin_insert;
void (*tt_end_insert)(void) 	 	= JX_end_insert;
void (*tt_del_eol)(void)  		= JX_del_eol;
void (*tt_delete_nlines)(int)  		= JX_delete_nlines;
void (*tt_delete_char)(void)  		= JX_delete_char;
void (*tt_erase_line)(void)  		= JX_erase_line;
void (*tt_tt_normal_video)(void)  	= JX_normal_video;
void (*tt_cls)(void)  			= JX_cls;
void (*tt_beep)(void)  			= JX_beep;
void (*tt_reverse_index)(int) 		= JX_reverse_index;
void (*tt_smart_puts)(unsigned short *, unsigned short *, int, int)  = JX_smart_puts;
void (*tt_write_string)(char *)  	= JX_write_string;
void (*tt_putchar)(char)  		= JX_putchar;
void (*tt_init_video)(void)  		= JX_init_video;
void (*tt_reset_video)(void)  		= JX_reset_video;
void (*tt_normal_video)(void)  		= JX_normal_video;
void (*tt_set_scroll_region)(int, int)  = JX_set_scroll_region;
void (*tt_reset_scroll_region)(void)  	= JX_reset_scroll_region;
void (*tt_get_terminfo)(void)  		= JX_get_terminfo;
void (*tt_set_color)(int, char *, char *, char *) = JX_set_color;
void (*tt_set_color_esc)(int, char *);

void (*tt_wide_width)(void)  		= JX_wide_width;
void (*tt_narrow_width)(void)  		= JX_narrow_width;
void (*tt_enable_cursor_keys)(void)  	= JX_enable_cursor_keys;
void (*tt_set_term_vtxxx)(int *)  	= JX_set_term_vtxxx;

int *tt_Ignore_Beep  		= &JX_Ignore_Beep;
int *tt_Use_Ansi_Colors  	= &JX_Use_Ansi_Colors;
int *tt_Term_Cannot_Scroll  	= &JX_Term_Cannot_Scroll;
int *tt_Term_Cannot_Insert  	= &JX_Term_Cannot_Insert;
int *tt_Screen_Rows  		= &JX_Screen_Rows;
int *tt_Screen_Cols  		= &JX_Screen_Cols;

static int Not_Used;
int *tt_Baud_Rate               = &Not_Used;
