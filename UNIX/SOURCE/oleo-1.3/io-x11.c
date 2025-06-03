#ifdef HAVE_X11_X_H
/*	Copyright (C) 1992, 1993 Free Software Foundation, Inc.
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this software; see the file COPYING.  If not, write to
	the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include  <stdio.h>
#include <ctype.h>
#include <sys/param.h>
#define NeedFunctionPrototypes 0
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/keysym.h>
#include "global.h"
#include "utils.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-utils.h"
#include "io-term.h"
#include "cmd.h"
#include "window.h"
#include "line.h"
#include "font.h"
#include "lists.h"
#include "ir.h"
#include "display.h"
#include "io-abstract.h"
#include "regions.h"
#include "math.h"
#include "input.h"
#include "info.h"


#if defined(HAVE_RINT)
#ifdef __STDC__
extern double rint (double);
#else
extern double rint ();
#endif
#else
#define rint(x) (((x)<0) ? ceil((x)-.5) : floor((x)+.5))
#endif
static void record_damage ();

#ifdef __STDC__
extern char * x_get_string_resource (XrmDatabase, char *, char *);
extern XrmDatabase x_load_resources (Display *, char *, char *);
extern char * getenv (const char *);
#else
extern char * x_get_string_resource ();
extern XrmDatabase x_load_resources ();
extern char * getenv ();
#endif


static char *emergency_font_name = "8x13";
static char *cell_font_name = "times_roman12";
static char *default_font_name = "8x13";
static char *input_font_name = "8x13";
static char *status_font_name = "6x10";
static char *text_line_font_name = "8x13";
static char *label_font_name = "5x8";
int cell_font_point_size = 12;
/* Modified by Greg Lehey, 12 April 1993. Defaults were the other way
 * round, which does *not* look good on a high-res screen */
static char *default_bg_color_name = "white";
static char *default_fg_color_name = "black";

/* If non-0 and !no_x, these display functions will be used. */
char *io_x11_display_name = 0;

/* The geometry of the first window. */
static int geom_x = 0;
static int geom_y = 0;
static int geom_w = 675;
static int geom_h = 350;
static char geom_string[] = "675x350+0+0";

/* This global is used only during command line and .Xdefaults handling. */
static Display * theDisplay;

static char * rdb_class_name = "Oleo";
static char * rdb_name = "oleo";

#if 0
static XrmOptionDescRec x11_options[] =
{
  {0, 0, 0, 0}
};

/* This dynamicly computes the size of x11_options to make 
 * upkeep of this file less error prone.
 */
static int
  count_options ()
{
  int x;
  for (x = 0; x11_options[x].option; ++x)
    ;
  return x;
}
#endif
/* This synthesizes class and generic names for resources (fancy strcat).
 * Memory is reused after a reasonable number of calls. 
 */
#define RDB_NAME_BUF	8
#define RDB_NAME_SIZE	256

#ifdef __STDC__
static char * 
resource_name (char * left, char * right)
#else
static char * 
resource_name (left, right)
     char * left;
     char * right;
#endif
{
  static char bufs[RDB_NAME_BUF][RDB_NAME_SIZE];
  static int buf_pos = 0;
  int len = strlen(left);
  
  bcopy (left, bufs[buf_pos], len);
  bufs[buf_pos][len] = '.';
  strcpy (bufs[buf_pos] + len + 1, right);
  return bufs [buf_pos++];
}

#ifdef __STDC__
static char *
class_of (char * foo)
#else
static char *
class_of (foo)
     char * foo;
#endif
{
  return resource_name (rdb_class_name, foo);
}

#ifdef __STDC__
static char *
name_of (char * foo)
#else
static char *
name_of (foo)
     char * foo;
#endif
{
  return resource_name (rdb_name, foo);
}

static XrmDatabase rdb;

#ifdef __STDC__
void
get_x11_args (int * argc_p, char ** argv)
#else
void
get_x11_args (argc_p, argv)
     int * argc_p;
     char ** argv;
#endif
{
  XrmInitialize ();
  
#if 0
  XrmDatabase argv_resources;
  /* Get the command line arguments. */
  XrmParseCommand (&argv_resources, x11_options,
		   count_options (x11_options),
		   rdb_name, argc_p, argv);
  
  /* Compute the display from either resources or getenv. */
  
  io_x11_display_name =
    x_get_string_resource (argv_resources, class_of ("Display"),
			   name_of ("display"));
#endif					       
  if (!io_x11_display_name)
    io_x11_display_name = ck_savestr (getenv ("DISPLAY"));
  
  if (!io_x11_display_name)
    return;
  
  theDisplay = XOpenDisplay (io_x11_display_name);
  if (!theDisplay)
    panic ("Can not connect to X.  Check your DISPLAY evironment variable.");
  
  FD_SET (ConnectionNumber (theDisplay), &read_fd_set);
  FD_SET (ConnectionNumber (theDisplay), &exception_fd_set);
  
  /* Load the resource databases in a manner not unlike emacs :). */
  rdb = x_load_resources (theDisplay, 0, rdb_class_name);
  
#if 0
  /* Merge in the command line database.  */
  XrmMergeDatabases (argv_resources, rdb);
#endif
  
  /* Set up the various defaults (staticly declared above). */
  {
    char * val;
    
    val = x_get_string_resource (rdb, class_of ("Foreground"),
				 name_of("foreground"));
    if (val)
      default_fg_color_name = val;
    
    val = x_get_string_resource (rdb, class_of ("Background"),
				 name_of("background"));
    if (val)
      default_bg_color_name = val;
    
    val = x_get_string_resource (rdb, class_of ("Geometry"),
				 name_of("geometry"));
    if (val)
      XGeometry (theDisplay, DefaultScreen(theDisplay), val, geom_string,
		 0, 1, 1, 0, 0, &geom_x, &geom_y, &geom_w, &geom_h);
  }
}


static int x11_opened = 0;
static struct sXport *thePort;
typedef struct sXport *Xport;

struct sXport
{
  Display *dpy;
  int screen;
  Colormap color_map;
  XColor fg_color;
  XColor bg_color;
  unsigned long fg_color_pixel;
  unsigned long bg_color_pixel;
  Window window;
  Cursor mouse_cursor;
  
  XFontStruct *input_font;
  XFontStruct *text_line_font;
  XFontStruct *label_font;
  XFontStruct *status_font;
  
  GC neutral_gc;
  GC normal_gc;
  GC standout_gc;
  GC input_gc;
  GC standout_input_gc;
  GC text_line_gc;
  GC text_line_standout_gc;
  GC label_gc;
  GC label_standout_gc;
  GC status_gc;
  
  struct x_window *xwins;
  int cursor_visible;		/* init to 1 */
  int redisp_needed;
  Atom wm_delete_window;
  
  /* The state of the input area. */
  struct input_view input_view;

  /* If there is a message, that gets put in the input area. */
  char * message;
  int message_timeout;
};

#ifdef __STDC__
static void
beep (Xport xport)
#else
static void
beep (xport)
     Xport xport;
#endif
{
#if 0
  XBell (xport->dpy, 30);
  XFlush (xport->dpy);
#endif
}

#ifdef __STDC__
static void
xio_bell (void)
#else
static void
xio_bell ()
#endif
{
  beep (thePort);
}


#ifdef __STDC__
static void
draw_text_item (Xport xport, int c, int r, int wid, int hgt,
		XFontStruct *font, GC gc, XTextItem *text, int do_clip)
#else
static void
draw_text_item (xport, c, r, wid, hgt, font, gc, text, do_clip)
     Xport xport;
     int c;
     int r;
     int wid;
     int hgt;
     XFontStruct *font;
     GC gc;
     XTextItem *text;
     int do_clip;
#endif
{
  XRectangle clip;
  int widused;
  int tab_char;
  XTextItem to_draw;

  clip.x = c;
  clip.y = r;
  clip.width = wid;
  clip.height = hgt;
  if (!x11_opened)
    return;
  if (do_clip)
    XSetClipRectangles (xport->dpy, gc, 0, 0, &clip, 1, YXBanded);


  tab_char = 0;
  to_draw = *text;
  widused = 0;
  while (to_draw.nchars)
    {
      XTextItem draw_now;
      XTextItem after_tab;

      /* Draw tab characters carefully. */
      {
	int x;
	for (x = 0; x < to_draw.nchars; ++x)
	  if (to_draw.chars[x] == '\t')
	    {
	      draw_now = to_draw;
	      draw_now.nchars = x;
	      after_tab = to_draw;
	      after_tab.chars += x + 1;
	      after_tab.nchars -= x + 1;
	      goto draw_burst;
	    }
	/* No tabs found. */
	draw_now = to_draw;
	bzero (&after_tab, sizeof (after_tab));
      }

    draw_burst:
      XDrawImageString (xport->dpy, xport->window, gc,
			c + widused, r + font->ascent,
			draw_now.chars, draw_now.nchars);
      widused += XTextWidth (font, draw_now.chars, draw_now.nchars);
      tab_char += draw_now.nchars;
      if (after_tab.chars)
	{
	  int tab_stop_col = ((tab_char + 8 - tab_char % 8)
			      * font->max_bounds.width);
	  XFillRectangle (xport->dpy, xport->window, xport->neutral_gc,
			  c + widused, r, tab_stop_col - widused, hgt);
	  widused = tab_stop_col;
	  tab_char += 8 - tab_char % 8;
	}
      to_draw = after_tab;
    }

  if (widused < wid)
    XFillRectangle (xport->dpy, xport->window, xport->neutral_gc,
		    c + widused, r, wid - widused, hgt);
  if (do_clip)
    XSetClipMask (xport->dpy, gc, None);
}

#ifdef __STDC__
static void
xio_redraw_input_cursor (int on)
#else
static void
xio_redraw_input_cursor (on)
     int on;
#endif
{
  struct input_view * iv = &thePort->input_view;
  int offset = iv->input_cursor - iv->visibility_begin;
  int start;
  XTextItem cursor_text;
  int cwid;
  int ypos = (iv->current_info ? 0 : input);
  char * inp;

  inp = (iv->input_area
	 ? iv->input_area->buf + iv->visibility_begin
	 : "");
  start = (XTextWidth (thePort->input_font, inp, offset) + iv->prompt_wid);

  cursor_text.font = thePort->input_font->fid;
  cursor_text.nchars = 1;
  if (iv->input_cursor <= iv->visibility_end)
    {
      cwid = XTextWidth (thePort->input_font, inp + offset, 1);
      cursor_text.chars = inp + offset;
    }
  else
    {
      cwid = XTextWidth (thePort->input_font, " ", 1);
      cursor_text.chars = " ";
    }
  draw_text_item (thePort, start, ypos, cwid, input_rows,
		  thePort->input_font,
		  (on ? thePort->standout_input_gc : thePort->input_gc),
		  &cursor_text, 1);
}

#ifdef __STDC__
static void
xio_cellize_cursor (void)
#else
static void
xio_cellize_cursor ()
#endif
{
  xio_redraw_input_cursor (0);
}

#ifdef __STDC__
static void
xio_inputize_cursor (void)
#else
static void
xio_inputize_cursor ()
#endif
{
  xio_redraw_input_cursor (1);
}

static int
x_input_metric (str, len)
     char * str;
     int len;
{
  return XTextWidth (thePort->input_font, str, len);
}

/* This redraws the input area without recomputing anything. */
#ifdef __STDC__
static void
xio_redraw_input (void)
#else
static void
xio_redraw_input ()
#endif
{
  if (thePort->message)
    {				/* This isn't in use. */
      XTextItem text;
      text.chars = thePort->message;
      text.nchars = strlen (thePort->message);
      text.font = thePort->input_font->fid;
      draw_text_item (thePort, 0, input, scr_cols, input_rows,
		      thePort->input_font, thePort->input_gc,
		      &text, 1);
      return;
    }

  {
    struct input_view * iv = &thePort->input_view;
    int ypos = (iv->current_info ? 0 : input);

    if (iv->redraw_needed == NO_REDRAW)
      return;
    if (iv->redraw_needed == FULL_REDRAW)
      {
	XTextItem text;
	if (iv->expanded_keymap_prompt)
	  {
	    text.font = thePort->input_font->fid;
	    text.chars = iv->expanded_keymap_prompt;
	    text.nchars = strlen (iv->expanded_keymap_prompt);
	    draw_text_item (thePort,
			    0, ypos, scr_cols, input_rows,
			    thePort->input_font, thePort->input_gc,
			    &text, 0);
	    iv->redraw_needed = NO_REDRAW;
	    if (input_active || iv->current_info)
	      xio_redraw_input_cursor (1);
	    return;
	  }
	else if (iv->prompt_wid)
	  {
	    text.font = thePort->input_font->fid;
	    text.chars = iv->prompt;
	    text.nchars = prompt_len (text.chars);
	    draw_text_item (thePort, 0, ypos, iv->prompt_wid, input_rows,
			    thePort->input_font, thePort->input_gc,
			    &text, 0);
	  }
      }
    
    if (!iv->input_area
	|| (iv->visibility_begin > iv->visibility_end))
      {
	XFillRectangle (thePort->dpy, thePort->window, thePort->neutral_gc,
			iv->prompt_wid, ypos,
			scr_cols - iv->prompt_wid, input_rows);
	iv->redraw_needed = NO_REDRAW;
	return;
      }
    
    
    if (iv->visibility_end >= iv->visibility_begin)
      {
	int pos = ((iv->redraw_needed == FULL_REDRAW)
		   ? iv->visibility_begin
		   : iv->redraw_needed);

	int xpos = (iv->prompt_wid
		    + XTextWidth (thePort->input_font,
				  iv->input_area->buf + iv->visibility_begin,
				  pos - iv->visibility_begin));
	int wid = scr_cols - xpos;
	int hgt = input_rows;

	XTextItem text;

	text.font = thePort->input_font->fid;
	text.chars = iv->input_area->buf + pos;
	text.nchars = iv->visibility_end - pos + 1;

	draw_text_item (thePort, xpos, ypos, wid, hgt, 
			thePort->input_font, thePort->input_gc,
			&text, 0);

	if (input_active || iv->current_info)
	  xio_redraw_input_cursor (1);
	iv->redraw_needed = NO_REDRAW;
      }
  }
}


#ifdef __STDC__
static void
xio_fix_input (void)
#else
static void
xio_fix_input ()
#endif
{
  iv_fix_input (&thePort->input_view);
}

#ifdef __STDC__
static void
xio_move_cursor (void)
#else
static void
xio_move_cursor ()
#endif
{
  if (input_active)
    xio_redraw_input_cursor (0);
  iv_move_cursor (&thePort->input_view);
  if (input_active)
    xio_redraw_input_cursor (1);
}

#ifdef __STDC__
static void
xio_erase (int len)
#else
static void
xio_erase (len)
     int len;
#endif
{
  iv_erase (&thePort->input_view, len);
}

#ifdef __STDC__
static void
xio_insert (int len)
#else
static void
xio_insert (len)
     int len;
#endif
{
  iv_insert (&thePort->input_view, len);
}
	   
#ifdef __STDC__
static void
xio_over (char * str, int len)
#else
static void
xio_over (str, len)
     char * str;
     int len;
#endif
{
  iv_over (&thePort->input_view, len);
}

#ifdef __STDC__
int
io_col_to_input_pos (int c)
#else
int
io_col_to_input_pos (c)
     int c;
#endif
{
  struct input_view * iv = &thePort->input_view;
  char * prompt = (iv->expanded_keymap_prompt
		   ? iv->expanded_keymap_prompt
		   : (iv->prompt ? iv->prompt : ""));
  int prompt_wid = iv->prompt_metric (prompt, strlen(prompt));

  c -= prompt_wid;
  {
    int cpos;
    int max = iv->visibility_end - iv->visibility_begin + 1;
    for (cpos = 1; cpos < max; ++cpos)
      if (iv->input_metric (iv->input_area->buf + iv->visibility_begin, cpos)
	  >= c)
	break;
    return iv->visibility_begin + cpos - 1;
  }
}


/****************************************************************
 * Low level Input
 ****************************************************************/

/* To be like curses, we offer this option: */
static int block_on_getch = 1;

/* This is the buffer of decoded keyboard events. */
static char *input_buf = 0;
static int input_buf_allocated = 0;
static int chars_buffered = 0;
#define MAX_KEY_TRANSLATION	(1024)

static XComposeStatus compose;

#ifdef __STDC__
static void
xio_scan_for_input (int blockp)
#else
static void
xio_scan_for_input (blockp)
     int blockp;
#endif
{
  XEvent event_return;
  static int pendingw;
  static int pendingh;
  static int resize_pending = 0;
  int events_pending;
  int len;
  
  events_pending = XPending (thePort->dpy);
  do
    {
      if (resize_pending && !events_pending)
	{
	  if (   ((pendingh / height_scale) > 9.)
	      && ((pendingw / width_scale) > 9.))
	      {
		io_set_scr_size (pendingh, pendingw);
		resize_pending = 0;
	      }
	}
      
      if (!events_pending)
	io_redisp ();
      
      if (events_pending || blockp)
	XNextEvent (thePort->dpy, &event_return);
      else
	return;
      
      switch (event_return.type)
	{
	case ClientMessage:
	  if (event_return.xclient.data.l[0] == thePort->wm_delete_window)
	    {
	      XCloseDisplay(thePort->dpy);
	      exit(0);
	    }
	  break;
	case KeyPress:
	case ButtonPress:
	case ButtonRelease:
	  if (chars_buffered + MAX_KEY_TRANSLATION >= input_buf_allocated)
	    {
	      input_buf_allocated =
		2 * (input_buf_allocated
		     ? input_buf_allocated
		     : MAX_KEY_TRANSLATION);
	      input_buf =
		(char *) ck_remalloc (input_buf, input_buf_allocated);
	    }
	}
      
      
      switch (event_return.type)
	{
	case KeyPress:
	  len = XLookupString (&event_return,
			       input_buf + chars_buffered,
			       MAX_KEY_TRANSLATION, 0, &compose);
	  if ((len == 1) && (event_return.xkey.state & Mod1Mask))
	    input_buf[chars_buffered] |= 0x80;
	  chars_buffered += len;
	  break;
	  
	case ButtonPress:
	case ButtonRelease:
	  {
	    int seq =
	      enqueue_mouse_event (event_return.xbutton.y,
				   event_return.xbutton.x,
				   event_return.xbutton.button,
				   event_return.type == ButtonPress);
	    input_buf[chars_buffered++] = MOUSE_CHAR;
	    input_buf[chars_buffered++] =
	      event_return.xbutton.button - 1 + '0';
	    input_buf[chars_buffered++] = (char) seq;
	    break;
	  }
	  
	case Expose:
	  record_damage (thePort,
			 event_return.xexpose.x, event_return.xexpose.y,
			 event_return.xexpose.width,
			 event_return.xexpose.height);
	  break;
	  
	case MapNotify:
	  break;
	  
	case ReparentNotify:
	  break;
	  
	case ConfigureNotify:
	  pendingw = event_return.xconfigure.width;
	  pendingh = event_return.xconfigure.height;
	  resize_pending = 1;
	  break;
	  
	default:
	  break;
	}
      events_pending = XPending (thePort->dpy);
    }
  while (events_pending || !chars_buffered);
}


#ifdef __STDC__
static int
xio_input_avail (void)
#else
static int
xio_input_avail ()
#endif
{
  return chars_buffered;
}

#ifdef __STDC__
static void
xio_wait_for_input (void)
#else
static void
xio_wait_for_input ()
#endif
{
  io_scan_for_input (1);
}

#ifdef __STDC__
static int
xio_read_kbd (VOLATILE char *buffer, int size)
#else
static int
xio_read_kbd (buffer, size)
     VOLATILE char *buffer;
     int size;
#endif
{
  int amt_read = size < chars_buffered ? size : chars_buffered;
  bcopy (input_buf, (char *) buffer, amt_read);
  chars_buffered -= amt_read;
  if (chars_buffered)
    bcopy ((char *) input_buf + amt_read, (char *) input_buf, chars_buffered);
  return amt_read;
}

#ifdef __STDC__
static void
xio_nodelay (int delayp)
#else
static void
xio_nodelay (delayp)
     int delayp;
#endif
{
  block_on_getch = delayp;
}

#ifdef __STDC__
static int
xio_getch (void)
#else
static int
xio_getch ()
#endif
{
  char buf;
  
  if (!chars_buffered)
    io_scan_for_input (block_on_getch);
  
  if (chars_buffered)
    {
      io_read_kbd (&buf, 1);
      return buf;
    }
  else
    return 0;
}


/****************************************************************
 * Low level support for input and status areas.
 ****************************************************************/

/*
 * CRAM tells how many characters of a string will fit within
 * a given number of cols, presuming that if they don't all fit,
 * the string CONTINUE must be added to the end of those that do.
 */
#ifdef __STDC__
static int
cram (int cols, XFontStruct *font, char *text, int len, char *continuation)
#else
static int
cram (cols, font, text, len, continuation)
     int cols;
     XFontStruct *font;
     char *text;
     int len;
     char *continuation;
#endif
{
  int cont_cols = XTextWidth (font, continuation, strlen (continuation));
  int cols_used = 0;
  int x = 0;
  
  if (XTextWidth (font, text, len) < cols)
    return len;
  
  cols_used = 0;
  while (x < len && cols_used < (cols - cont_cols))
    cols_used += XTextWidth (font, text + x, 1);
  
  return x;
}




#ifdef __STDC__
static void
set_text (XTextItem *xtext, char *text, int len)
#else
static void
set_text (xtext, text, len)
     XTextItem *xtext;
     char *text;
     int len;
#endif
{
  if (xtext->nchars < len)
    {
      xtext->chars = ck_remalloc (xtext->chars, len);
      xtext->nchars = len;
    }
  bcopy (text, xtext->chars, len);
  while (len < xtext->nchars)
    xtext->chars[len++] = ' ';
}



/* The input area. */






/*
 * Low level interface to the input area specificly.
 */

static XTextItem input_text;
static int term_cursor_visible = 0;
static int input_cursor = 0;	/* Position of cursor, if visible */
static int textout;
static int input_more_mode = 0;

#ifdef __STDC__
static void
draw_input (void)
#else
static void
draw_input ()
#endif
{
  if (!input_text.chars)
    {
      XFillRectangle (thePort->dpy, thePort->window, thePort->neutral_gc,
		      input, 0, scr_cols, input_rows);
      return;
    }
  input_text.font = thePort->input_font->fid;
  draw_text_item (thePort, 0, input, scr_cols, input_rows, thePort->input_font,
		  thePort->input_gc, &input_text, 1);
  if (input_more_mode)
    {
      XTextItem more_text;
      int mwid = XTextWidth (thePort->input_font, "[more]", 6);
      more_text.chars = "[MORE]";
      more_text.nchars = 6;
      more_text.delta = 0;
      more_text.font = thePort->input_font->fid;
      draw_text_item (thePort, scr_cols - mwid, input, mwid, input_rows,
		      thePort->input_font, thePort->standout_input_gc,
		      &more_text, 1);
    }
  else if (term_cursor_visible)
    {
      int start = XTextWidth (thePort->input_font, input_text.chars, input_cursor);
      int cwid = XTextWidth (thePort->input_font,
			     input_text.chars + input_cursor, 1);
      XTextItem cursor_text;
      cursor_text.chars = input_text.chars + input_cursor;
      cursor_text.nchars = 1;
      cursor_text.font = thePort->input_font->fid;
      draw_text_item (thePort, start, input, cwid, input_rows,
		      thePort->input_font, thePort->standout_input_gc,
		      &cursor_text, 1);
    }
}


#ifdef __STDC__
static void
xio_clear_input_before (void)
#else
static void
xio_clear_input_before ()
#endif
{
  textout = 0;
  if (topclear == 2)
    {
      int x;
      for (x = 0; x < input_text.nchars; ++x)
	input_text.chars[x] = ' ';
      input_cursor = 0;
      draw_input ();
      topclear = 0;
    }
}

#ifdef __STDC__
static void
xio_clear_input_after (void)
#else
static void
xio_clear_input_after ()
#endif
{
  if (topclear)
    {
      int x;
      for (x = 0; x < input_text.nchars; ++x)
	input_text.chars[x] = ' ';
      input_cursor = 0;
      draw_input ();
      topclear = 0;
    }
}

#ifdef __STDC__
static void
set_input (char *text, int len, int cursor)
#else
static void
set_input (text, len, cursor)
     char *text;
     int len;
     int cursor;
#endif
{
  set_text (&input_text, text, len);
  if (cursor + 1 > input_text.nchars)
    {
      input_text.chars = (char *) ck_remalloc (input_text.chars, cursor + 1);
      while (input_text.nchars < cursor + 1)
	input_text.chars[input_text.nchars++] = ' ';
    }
  input_cursor = cursor;
  draw_input ();
}


/*
 * Low level interface to the status area specificly.
 */
static XTextItem status_text;

#ifdef __STDC__
static void
draw_status (void)
#else
static void
draw_status ()
#endif
{
  if (!x11_opened || thePort->input_view.current_info)
    return;
  if (user_status)
    draw_text_item (thePort, 0, status, scr_cols,
		    status_rows, thePort->status_font,
		    thePort->status_gc, &status_text, 1);
}

#ifdef __STDC__
static void
set_status (char *text)
#else
static void
set_status (text)
     char *text;
#endif
{
  set_text (&status_text, text, strlen (text));
  status_text.font = thePort->status_font->fid;
  draw_status ();
}


/****************************************************************
 * High level interfaces for the input and status areas.
 ****************************************************************/

#ifdef __STDC__
static void
xio_update_status (void)
#else
static void
xio_update_status ()
#endif
{
  CELL *cp;
  char *dec;
  char *ptr;
  int plen;
  int dlen;
  int mplen;
  char buf[1024];
  char *assembled;
  char *pos = buf;
  
  if (!user_status)
    return;
  
  if (mkrow != NON_ROW)
    {
      struct rng r;
      *pos++ = '*';
      set_rng (&r, curow, cucol, mkrow, mkcol);
      ptr = range_name (&r);
    }
  else
    ptr = cell_name (curow, cucol);
  bcopy (ptr, pos, strlen (ptr));
  pos += strlen (ptr);
  if (how_many != 1)
    {
      sprintf (pos, " {%d}", how_many);
      pos += strlen (pos);
    }
  *pos++ = ' ';
  mplen = XTextWidth (thePort->status_font, buf, pos - buf);
  
  if ((cp = find_cell (curow, cucol)) && cp->cell_formula)
    {
      dec = decomp (curow, cucol, cp);
      dlen = XTextWidth (thePort->status_font, dec, strlen (dec));
    }
  else
    {
      dec = 0;
      dlen = 0;
    }
  
  ptr = cell_value_string (curow, cucol);
  plen = XTextWidth (thePort->status_font, ptr, strlen (ptr));
  
  assembled = (char *) ck_malloc (plen + dlen + mplen);
  bcopy (buf, assembled, pos - buf);
  pos = assembled + (pos - buf);
  
  {
    int c;
    int l;
    int wid;
    
    l = strlen (ptr);
    c = cram (scr_cols - mplen - thePort->status_font->max_bounds.width,
	      thePort->status_font, ptr, l, (dec ? " [...]" : "..."));
    bcopy (ptr, pos, c);
    pos += c;
    if (c == l)
      *pos++ = ' ';
    else if (dec)
      {
	bcopy (" [...]", pos, 6);
	pos += 6;
	dec = 0;
	dlen = 0;
	decomp_free ();
      }
    else
      {
	bcopy ("...", pos, 3);
	pos += 3;
      }
    *pos++ = ' ';
    wid = XTextWidth (thePort->status_font, assembled, pos - assembled);
    
    if (dec)
      {
	l = strlen (dec);
	c = cram (scr_cols - wid, thePort->status_font, dec, l, "[...]");
	*pos++ = '[';
	if (c < l)
	  {
	    bcopy (dec, pos, c);
	    bcopy ("...]", pos, 4);
	    pos += c + 4;
	  }
	else
	  {
	    bcopy (dec, pos, c);
	    pos += c;
	    *pos++ = ']';
	  }
      }
    
    *pos++ = '\0';
    set_status (assembled);
    draw_status ();
    free (assembled);
  }
}


#ifdef __STDC__
static int
xio_get_chr (char *prompt)
#else
static int
xio_get_chr (prompt)
     char *prompt;
#endif
{
  int len = strlen (prompt);
  set_input (prompt, len, len);
  topclear = 2;
  draw_input ();
  return io_getch ();
}


/*
 * Multi-line informational messages to the user:
 */

/* old: */
struct text_line
{
  struct text_line *next;
  int nchars;
  int standout;
  char line[TAIL_ARRAY];
};
static int text_damaged = 0;
static struct text_line *text_lines = 0;
extern int auto_recalc;


/****************************************************************
 * Low level support for the cell windows.
 ****************************************************************/

/* Every cell has an associated font name.  This is a cache mapping
 * font names and scales to graphics contexts.
 */

struct cell_gc
{
  struct cell_gc *next;
  struct cell_gc *prev;
  Xport port;
  GC gc;
  char *font_name;
  double scale;
  XFontStruct *font;
  int clipped_to;
  int cursor:1;
};

/* The cell_gc cache is only valid for a specific size default font.  If the
 * defualt font size changes, then the cache must be flushed.
 */
static int cell_gc_basis = 12;


/* Individual cache entries are also validated by the clipping set for their
 * gc.  To the rest of the system, it appears that there is one clipping region
 * for all cell_gc's.
 */
static int clipcnt = 0;
static int cliprectc = 0;
static int cliprect_alloc = 0;
static XRectangle *cliprectv = 0;

#define GC_CACHE_SIZE	10
static struct cell_gc *cell_gc_cache = 0;


/* This takes the full name of an X11 font, and returns its point size (in
 * tenths of a point.  If the string is not a valid font name, 0 is returned.
 */

#ifdef __STDC__
static int 
name_to_ps (char *xfont)
#else
static int 
name_to_ps (xfont)
     char *xfont;
#endif
{
  while (*xfont)
    if (*xfont == '-')
      {
	++xfont;
	if (*xfont == '-')	/* look for -- */
	  {
	    ++xfont;
	    break;
	  }
      }
    else
      ++xfont;
  if (!*xfont)
    return 0;
  while (isdigit (*xfont++));
  --xfont;
  if (!*xfont++ == '-')
    return 0;
  return atoi (xfont);
}


#define ABS(A) ((A) < 0 ? -(A) : (A))

#ifdef __STDC__
static struct cell_gc *
cell_gc (Xport port, struct font_memo *font_passed, int cursor)
#else
static struct cell_gc *
cell_gc (port, font_passed, cursor)
     Xport port;
     struct font_memo *font_passed;
     int cursor;
#endif
{
  struct font_memo *font =
    (font_passed ? font_passed : default_font());
  char *font_name = font->names->x_name;
  double scale = font->scale;
  struct cell_gc *c = cell_gc_cache;
  if (cell_gc_basis != cell_font_point_size)
    {
      do
	{
	  if (c->font_name)
	    {
	      XFreeFont (c->port->dpy, c->font);
	      free (c->font_name);
	    }
	  c->font_name = 0;
	  c->port = 0;
	  c->font = 0;
	  c = c->next;
	}
      while (c != cell_gc_cache);
      cell_gc_basis = cell_font_point_size;
    }
  else
    {
      do
	{
	  if ((c->scale == scale)
	      && port == c->port
	      && c->font_name && !strcmp (font_name, c->font_name))
	    {
	      if (cell_gc_cache == c)
		cell_gc_cache = cell_gc_cache->next;
	      c->next->prev = c->prev;
	      c->prev->next = c->next;
	      c->next = cell_gc_cache;
	      c->prev = cell_gc_cache->prev;
	      c->prev->next = c;
	      c->next->prev = c;
	      cell_gc_cache = c;
	      goto check_clipping;
	    }
	  c = c->next;
	}
      while (c != cell_gc_cache);
    }
  
  c = c->prev;
  cell_gc_cache = c;
  if (c->font_name)
    {
      XFreeFont (c->port->dpy, c->font);
      free (c->font_name);
    }
  c->port = port;
  c->scale = scale;
  {
    char **fontv;
    int fontc;
    fontv = XListFonts (port->dpy, font_name, 1024, &fontc);
    if (fontv)
      {
	int x, best;
	int ideal_size = rint (scale * (double) cell_font_point_size * 10.);
	int best_dist;
	best = 0;
	best_dist = ideal_size - name_to_ps (fontv[0]);
	for (x = 1; x < fontc; ++x)
	  {
	    int ps = name_to_ps (fontv[x]);
	    int tdist = ideal_size - ps;
	    if (ABS (tdist) < ABS (best_dist)
		|| ((ABS (tdist) == ABS (best_dist)) && (tdist > best_dist)))
	      {
		best_dist = tdist;
		best = x;
	      }
	  }
	c->font = XLoadQueryFont (port->dpy, fontv[best]);
	XFreeFontNames (fontv);
      }
    else
      c->font = 0;
  }
  if (c->font)
    c->font_name = (char *) ck_savestr (font_name);
  else
    {
      c->font = XLoadQueryFont (port->dpy, cell_font_name);
      if (c->font)
	c->font_name = ck_savestr (cell_font_name);
      else
	{
	  c->font = XLoadQueryFont (port->dpy, emergency_font_name);
	  if (c->font)
	    c->font_name = ck_savestr (emergency_font_name);
	  else
	    panic ("Unable to load even the emergency font.");
	}
    }
  
  {
    XGCValues v;
    v.font = c->font->fid;
    XChangeGC (port->dpy, c->gc, GCFont, &v);
  }
  
 check_clipping:
  
  if (clipcnt != c->clipped_to)
    {
      XSetClipRectangles (port->dpy, c->gc, 0, 0, cliprectv, cliprectc, Unsorted);
      c->clipped_to = clipcnt;
    }
  
  {
    XGCValues v;
    v.foreground = cursor ? port->bg_color_pixel : port->fg_color_pixel;
    v.background = cursor ? port->fg_color_pixel : port->bg_color_pixel;
    XChangeGC (port->dpy, c->gc, GCForeground | GCBackground, &v);
    c->cursor = cursor;
  }
  return c;
}



/* This is the data for an oleo window, displayed under X. */

enum kinds_of_layout_needed
{
  damaged_display = 1,
  new_display = 2
};

struct x_window
{
  struct x_window *next;
  struct window *win;
  struct display display;
  int layout_needed;
  int label_damage;
  Xport port;
};



#ifdef __STDC__
static void
collect_clipping (xx_IntRectangle rect)
#else
static void
collect_clipping (rect)
     xx_IntRectangle rect;
#endif
{
  if (cliprectc == cliprect_alloc)
    {
      cliprect_alloc = cliprect_alloc ? cliprect_alloc * 2 : 16;
      cliprectv =
	((XRectangle *)
	 ck_remalloc (cliprectv,
		      cliprect_alloc * sizeof (XRectangle)));
    }
  cliprectv[cliprectc].x = rect->x;
  cliprectv[cliprectc].y = rect->y;
  cliprectv[cliprectc].width = rect->w;
  cliprectv[cliprectc].height = rect->h;
  ++cliprectc;
}

#ifdef __STDC__
static void 
clip_to_intrectangle (struct x_window * xwin, xx_IntRectangle rect)
#else
static void 
clip_to_intrectangle (xwin, rect)
     struct x_window * xwin;
     xx_IntRectangle rect;
#endif
{
  struct window * win = xwin->win;
  int x_ceil = win->win_over + win->numc;
  int y_ceil = win->win_down + win->numr;

  if (rect->w + rect->x >= x_ceil)
    {
      rect->w = x_ceil - rect->x;
    }
  if (rect->h + rect->y >= y_ceil)
    {
      rect->h = y_ceil - rect->y;
    }
  cliprectc = 0;
  collect_clipping (rect);
  ++clipcnt;
}


#ifdef __STDC__
static void 
place_text (xx_IntRectangle out, struct display *disp, struct cell_display *cd, XFontStruct *font, char *string)
#else
static void 
place_text (out, disp, cd, font, string)
     xx_IntRectangle out;
     struct display *disp;
     struct cell_display *cd;
     XFontStruct *font;
     char *string;
#endif
{
  int *widths = disp->widths;
  int *heights = disp->heights;
  int *colx = disp->colx;
  int *rowy = disp->rowy;
  struct rng *range = &disp->range;
  int hout = font->ascent + font->descent;
  int wout = XTextWidth (font, string, strlen (string));
  int ci = cd->c - range->lc;
  int ri = cd->r - range->lr;
  int yout = rowy[ri] + heights[ri] - (font->ascent + font->descent);
  int xout;
  switch (cd->justification)
    {
    default:
    case JST_LFT:
      xout = colx[ci];
      break;
    case JST_RGT:
      xout = colx[ci] + widths[ci] - 1 - wout;
      break;
    case JST_CNT:
      xout = colx[ci] + (widths[ci] - wout) / 2;
      break;
    }
  xx_IRinit (out, xout, yout, wout, hout);
}



#ifdef __STDC__
static void
x_metric (struct cell_display *cd, struct display *disp)
#else
static void
x_metric (cd, disp)
     struct cell_display *cd;
     struct display *disp;
#endif
{
  struct x_window *xw = (struct x_window *) disp->vdata;
  Xport port = xw->port;
  if (!cd->unclipped)
    xx_IRinit (&cd->goal, 0, 0, 0, 0);
  else
    {
      struct cell_gc *cgc = cell_gc (port, cd->font, 0);
      place_text (&cd->goal, disp, cd, cgc->font, cd->unclipped);
    }
}



#ifdef __STDC__
static struct x_window *
x_win (Xport port, struct window *win, int rebuild)
#else
static struct x_window *
x_win (port, win, rebuild)
     Xport port;
     struct window *win;
     int rebuild;
#endif
{
  struct x_window *xw;
  
  for (xw = port->xwins; xw && xw->win != win; xw = xw->next);
  
  if (xw && !rebuild)
    return xw;
  
  if (xw)
    {
      free_display (&xw->display);
    }
  else
    {
      xw = (struct x_window *) ck_malloc (sizeof (*xw));
      xw->next = port->xwins;
      port->xwins = xw;
      xw->port = port;
    }
  xw->win = win;
  build_display (&xw->display, &win->screen, x_metric, xw);
  xw->label_damage = 1;
  xw->layout_needed = new_display;
  port->redisp_needed = 1;
  return xw;
}

#ifdef __STDC__
static void
flush_x_windows (Xport port)
#else
static void
flush_x_windows (port)
     Xport port;
#endif
{
  while (port->xwins)
    {
      struct x_window * xw = port->xwins;
      free_display (&xw->display);
      port->xwins = xw->next;
      free (xw);
      port->redisp_needed = 1;
    }
}




#ifdef __STDC__
static void
record_damage (Xport port, int x, int y, int w, int h)
#else
static void
record_damage (port, x, y, w, h)
     Xport port;
     int x;
     int y;
     int w;
     int h;
#endif
{
  struct x_window *xwin = port->xwins;
  
  if (port->input_view.current_info)
    {
      port->input_view.info_redraw_needed = 1;
      return;
    }

  if (text_lines)
    {
      text_damaged = 1;
      return;
    }
  
  while (xwin)
    {
      struct window *win = xwin->win;
      record_display_damage (&xwin->display,
			     x - win->win_over,
			     y - win->win_down,
			     w, h);
      port->redisp_needed = 1;
      if (   (   (x + w >= win->win_over - win_label_cols(win, win->screen.hr))
	      && (x <= win->win_over)
	      && (y + h >= win->win_down - win_label_rows(win))
	      && (y <= win->win_down + display_height(&xwin->display)))
	  || (   (y + h >= win->win_down - win_label_rows(win))
	      && (y <= win->win_down)
	      && (x + w >= win->win_over - win_label_cols(win, win->screen.hr))
	      && (x <= win->win_over + display_width(&xwin->display))))
	xwin->label_damage = 1;
      xwin = xwin->next;
    }

  if (((input + input_rows) >= y) && (input <= y + h))
    port->input_view.redraw_needed = FULL_REDRAW;
}



#ifdef __STDC__
static void
xio_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp)
#else
static void
xio_pr_cell_win (win, r, c, cp)
     struct window *win;
     CELLREF r;
     CELLREF c;
     CELL *cp;
#endif
{
  struct x_window *xwin = x_win (thePort, win, 0);
  struct display *disp = &xwin->display;
  
  if (pr_display_cell (disp, r, c, cp))
    {
      thePort->redisp_needed = 1;
      if (!xwin->layout_needed)
	xwin->layout_needed = damaged_display;
    }
}


#ifdef __STDC__
static void
xio_repaint_win (struct window *win)
#else
static void
xio_repaint_win (win)
     struct window *win;
#endif
{
  x_win (thePort, win, 1);
  XFillRectangle (thePort->dpy, thePort->window, thePort->neutral_gc,
		  win->win_over, win->win_down, win->numc, win->numr);
  record_damage (thePort, win->win_over, win->win_down, win->numc, win->numr);
}



#ifdef __STDC__
static void
xio_repaint (void)
#else
static void
xio_repaint ()
#endif
{
  struct window *win;
  if (thePort->input_view.current_info)
    {
      thePort->input_view.info_redraw_needed = 1;
      return;
    }
  flush_x_windows (thePort);
  for (win = wins; win < &wins[nwin]; win++)
    xio_repaint_win (win);
}


#ifdef __STDC__
static void
draw_cell (struct x_window *xwin, struct cell_display *cd_passed, int cursor)
#else
static void
draw_cell (xwin, cd_passed, cursor)
     struct x_window *xwin;
     struct cell_display *cd_passed;
     int cursor;
     
#endif
{
  struct cell_display *cd = cd_passed->used_by;
  Xport port = xwin->port;
  struct window *win = xwin->win;
  int ov = win->win_over;
  int dn = win->win_down;
  CELLREF r = cd->r;
  CELLREF c = cd->c;
  struct display *disp = &xwin->display;
  int *rowy = disp->rowy;
  int *colx = disp->colx;
  int *widths = disp->widths;
  int *heights = disp->heights;
  struct cell_gc *cgc;
  int ri = r - disp->range.lr;
  int ci = c - disp->range.lc;
  
  if (!(widths[ci] && heights[ri]))
    return;
  
  if (!cd->unclipped)
    {
      cgc = cell_gc (port, default_font(), !cursor);
      XFillRectangle (port->dpy, port->window, cgc->gc,
		      colx[ci] + ov, rowy[ri] + dn,
		      widths[ci], heights[ri]);
      return;
    }
  
  cgc = cell_gc (port, cd->font, cursor);
  {
    int isclipped;
    char *str;
    struct xx_sIntRectangle *strbox;
    
    isclipped = !xx_IRencloses_width (&cd->layout, &cd->goal);
    if (!isclipped)
      {
	str = cd->unclipped;
	strbox = &cd->goal;
      }
    else
      {
	if (!cd->clipped)
	  {
	    int chr_scale = XTextWidth (cgc->font, "8", 1);
	    int w_avail = xx_IRw (&cd->layout) / chr_scale;
	    int cell_wid = widths[ci] / chr_scale;
	    CELL *cp = find_cell (r, c);
	    cd->clipped =
	      ck_savestr (adjust_prc (cd->unclipped, cp, w_avail, cell_wid,
				      cd->justification));
	    place_text (&cd->clip, disp, cd, cgc->font, cd->clipped);
	  }
	str = cd->clipped;
	strbox = &cd->clip;
      }
    XDrawImageString (port->dpy, port->window, cgc->gc,
		      strbox->x + ov, strbox->y + cgc->font->ascent + dn,
		      str, strlen (str));
    {
      struct xx_sIntRectangle tofill[4];
      int fillc;
      
      fillc = xx_IRsubtract (tofill, &cd->layout, strbox);
      {
	XGCValues v;
	v.foreground = cursor ? port->fg_color_pixel : port->bg_color_pixel;
	v.background = cursor ? port->bg_color_pixel : port->fg_color_pixel;
	XChangeGC (port->dpy, cgc->gc, GCForeground | GCBackground, &v);
	cgc->cursor = !cursor;
      }
      while (fillc--)
	XFillRectangle (port->dpy, port->window, cgc->gc,
			tofill[fillc].x + ov, tofill[fillc].y + dn,
			tofill[fillc].w, tofill[fillc].h);
    }
  }
}


/* Cell values */

#ifdef __STDC__
static void
set_cursor (int on)
#else
static void
set_cursor (on)
     int on;
#endif
{
  struct x_window *xwin = x_win (thePort, cwin, 0);
  struct display *disp = &xwin->display;
  int *rowy = disp->rowy;
  int *colx = disp->colx;
  int *widths = disp->widths;
  int *heights = disp->heights;
  int ri = curow - disp->range.lr;
  int ci = cucol - disp->range.lc;
  int ov = xwin->win->win_over;
  int dn = xwin->win->win_down;
  struct cell_display *cd = cell_display_of (disp, curow, cucol);
  struct xx_sIntRectangle clip;

  /* The cursor might not even be visible.  Though commands should never leave
   * the user in such a state, it can happen in several ways transiently.
   */
  if (!cd)
    return;

  thePort->cursor_visible = on;
  
  if (xwin->layout_needed == new_display)
    return;
  
  if (cd->used_by == cd)
    {
      clip = cd->layout;
      clip.x += ov;
      clip.y += dn;
    }
  else
    xx_IRinit (&clip,
	       colx[ci] + ov, rowy[ri] + dn,
	       widths[ci], heights[ri]);
  clip_to_intrectangle (xwin, &clip);
  draw_cell (xwin, cd, on);
}


#ifdef __STDC__
static void
xio_hide_cell_cursor (void)
#else
static void
xio_hide_cell_cursor ()
#endif
{
  if (thePort->cursor_visible)
    set_cursor (0);
}

#ifdef __STDC__
static void
xio_display_cell_cursor (void)
#else
static void
xio_display_cell_cursor ()
#endif
{
  if (!thePort->cursor_visible)
    {
      if (   (curow < cwin->screen.lr)
	  || (cucol < cwin->screen.lc)
	  || (curow > cwin->screen.hr)
	  || (cucol > cwin->screen.hc))
	io_recenter_cur_win ();
      set_cursor (1);
    }
}


static int xx[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


#ifdef __STDC__
static void
draw_labels (void)
#else
static void
draw_labels ()
#endif
{
  struct x_window *xwin;
  XRectangle rect;
  for (xwin = thePort->xwins; xwin; xwin = xwin->next)
    if (xwin->win->lh_wid && xwin->label_damage)
      {
	XGCValues gcv;
	struct window *win = xwin->win;
	int wid2 = win->lh_wid / 2;
	if (xx[0]) {
	  XFillRectangle (thePort->dpy, thePort->window,
			  thePort->label_standout_gc,
			  win->win_over - win->lh_wid,
			  win->win_down - label_rows,
			  wid2, label_rows);
	};
	gcv.line_width = win->lh_wid;
	gcv.cap_style = CapRound;
	XChangeGC (thePort->dpy, thePort->label_gc,
		   GCLineWidth | GCCapStyle, &gcv);
	if (xx[1]) {
	  XDrawLine (thePort->dpy, thePort->window, thePort->label_gc,
		     win->win_over - wid2,
		     win->win_down - label_rows + wid2,
		     win->win_over - wid2,
		     win->win_down + win->numr - wid2);
	  
	};
	gcv.cap_style = CapButt;
	XChangeGC (thePort->dpy, thePort->label_gc, GCCapStyle, &gcv);
	if (xx[2]) {
	  XDrawLine (thePort->dpy, thePort->window, thePort->label_gc,
		     win->win_over - wid2,
		     win->win_down + wid2,
		     win->win_over - wid2,
		     win->win_down + win->numr);
	};
	
	if (xx[3]) {
	  XFillRectangle (thePort->dpy, thePort->window, thePort->label_gc,
			  win->win_over - wid2,
			  win->win_down - label_rows,
			  win->numc + wid2,
			  label_rows);
	};
	
	rect.x = 0;
	rect.y = 0;
	rect.width = scr_cols;
	rect.height = scr_lines;
	XSetClipRectangles (thePort->dpy, thePort->label_standout_gc,
			    0, 0, &rect, 1, YXBanded);
	{
	  char buf[100];
	  CELLREF cr;
	  int x;
	  int len;
	  
	  sprintf (buf, "#%d", 1 + win - wins);
	  for (x = len = 0; buf[x]; ++x)
	    {
	      int cwid = XTextWidth (thePort->label_font, buf, 1);
	      if (cwid + len > wid2)
		break;
	      len += cwid;
	    }
	  XDrawImageString (thePort->dpy, thePort->window,
			    thePort->label_standout_gc,
			    win->win_over - wid2,
			    win->win_down - thePort->label_font->descent,
			    buf, x);
	  
	  rect.x = win->win_over - win->lh_wid;
	  rect.y = win->win_down;
	  rect.width = win->lh_wid;
	  rect.height = win->numr;
	  XSetClipRectangles (thePort->dpy, thePort->label_standout_gc,
			      0, 0, &rect, 1, YXBanded);
	  
	  x = win->win_down + thePort->label_font->ascent;
	  for (cr = win->screen.lr; cr <= win->screen.hr; ++cr)
	    {
	      int hgt = get_scaled_height (cr);
	      if (hgt)
		{
		  if (a0)
		    sprintf (buf, "%d", cr);
		  else
		    sprintf (buf, "R%d", cr);
		  XDrawImageString (thePort->dpy, thePort->window,
				    thePort->label_standout_gc,
				    win->win_over - win->lh_wid, x,
				    buf, strlen (buf));
		  x += hgt;
		}
	      if (cr == MAX_ROW)
		break;
	    }
	  rect.x = 0;
	  rect.y = 0;
	  rect.width = scr_cols;
	  rect.height = scr_lines;
	  XSetClipRectangles (thePort->dpy, thePort->label_standout_gc,
			      0, 0, &rect, 1, YXBanded);
	  
	  x = win->win_over;
	  for (cr = win->screen.lc; cr <= win->screen.hc; ++cr)
	    {
	      int wid = get_scaled_width (cr);
	      if (wid > win->numc)
		wid = win->numc;
	      if (wid)
		{
		  int txtwid;
		  char *ptr;
		  if (a0)
		    ptr = col_to_str (cr);
		  else
		    {
		      ptr = buf;
		      sprintf (ptr, "C%u", cr);
		    }
		  txtwid = XTextWidth (thePort->label_font, ptr, strlen (ptr));
		  if (txtwid > wid - thePort->label_font->max_bounds.width)
		    {
		      int txtlen =
			((wid - thePort->label_font->max_bounds.width)
			 / XTextWidth (thePort->label_font, "#", 1));
		      txtwid = txtlen * XTextWidth (thePort->label_font,
						    "#", 1);
		      buf[txtlen] = 0;
		      while (txtlen)
			buf[txtlen--] = '#';
		      ptr = buf;
		    }
		  XDrawImageString (thePort->dpy, thePort->window,
				    thePort->label_standout_gc,
				    x + (wid - txtwid) / 2,
				    (win->win_down
				     - thePort->label_font->descent),
				    ptr, strlen (ptr));
		  x += wid;
		}
	      if (cr == MAX_COL)
		break;
	    }
	}
	xwin->label_damage = 0;
      }
}


/* Refresh the existing image. */


#ifdef __STDC__
static void
xio_redisp (void)
#else
static void
xio_redisp ()
#endif
{
  static int was_text = 0;
  if (thePort->input_view.current_info)
    {
      if (!was_text)
	thePort->input_view.redraw_needed = FULL_REDRAW;

      if (thePort->input_view.redraw_needed != NO_REDRAW)
	xio_redraw_input ();

      if (thePort->input_view.info_redraw_needed)
	{
	  int ipos = thePort->input_view.info_pos;
	  int top = ipos + (scr_lines - input_rows) / info_rows - 1;
	  int ypos;
	  XTextItem text;

	  if (top >= thePort->input_view.current_info->len)
	    top = thePort->input_view.current_info->len - 1;

	  ypos = input_rows;

	  text.font = thePort->text_line_font->fid;
	  while (ipos <= top)
	    {
	      text.chars = thePort->input_view.current_info->text[ipos];
	      text.nchars = strlen (text.chars);
	      draw_text_item (thePort, 0, ypos, scr_cols, input_rows,
			      thePort->text_line_font, thePort->text_line_gc,
			      &text, 0);
	      ypos += info_rows;
	      ++ipos;
	    }
	  XFillRectangle (thePort->dpy, thePort->window,
			  thePort->neutral_gc, 0, ypos,
			  scr_cols, scr_lines - ypos - 1);
	  thePort->input_view.redraw_needed = FULL_REDRAW;
	  xio_redraw_input ();
	  thePort->input_view.info_redraw_needed = 0;
	  was_text = 1;
	}
    }
  else
    {
      struct x_window *xwin;
      if (was_text)
	{
	  thePort->input_view.info_redraw_needed = 0;
	  was_text = 0;
	  io_repaint ();
	}
      else
	{
	  struct rng * rng = &cwin->screen;
	  if (   (curow > rng->hr)
	      || (curow < rng->lr)
	      || (cucol > rng->hc)
	      || (cucol < rng->lc))
	    io_recenter_cur_win ();
	}
      thePort->redisp_needed = 0;
      if (thePort->redisp_needed != NO_REDRAW)
	xio_redraw_input ();
      draw_status ();
      draw_labels ();
      for (xwin = thePort->xwins; xwin; xwin = xwin->next)
	{
	  struct display *disp = &xwin->display;
	  int ov = xwin->win->win_over;
	  int dn = xwin->win->win_down;
	  struct cell_display * cd;
	  struct cell_display * cursor_cd = 0;
	  int must_draw_cursor;
	  
	  if (xwin->layout_needed)
	    {
	      layout (disp);
	      xwin->layout_needed = 0;
	    }
	  
	  /* If the cursor cell has been damaged, 
	   * it will be redrawn.  However, if the 
	   * cursor cell is empty, then redrawing
	   * some other cell might damage the cursor.
	   * This watches for that condition and
	   * redraws the cursor if it occurs.
	   */
	  must_draw_cursor = 0;
	  if (thePort->cursor_visible
	      && (xwin->win == cwin)
	      && (curow >= disp->range.lr)
	      && (curow <= disp->range.hr)
	      && (cucol >= disp->range.lc)
	      && (cucol <= disp->range.hc))
	    {
	      cursor_cd = cell_display_of (disp, curow, cucol);
	      /* If the cursor cell is not empty, we never have
	       * to explicitly redraw the cursor.
	       */
	      if (cursor_cd->used_by == cursor_cd)
		cursor_cd = 0;
	    }
	  
	  cd = disp->damaged;
	  while (cd != (struct cell_display *) disp)
	    {
	      struct xx_sIntRectangle clip;
	      struct cell_display *owner = cd->used_by;
	      clip = owner->layout;
	      clip.x += ov;
	      clip.y += dn;
	      clip_to_intrectangle (xwin, &clip);
	      if (cursor_cd && (cd->used_by == cursor_cd->used_by))
		must_draw_cursor = 1;
	      draw_cell (xwin, owner,
			 (thePort->cursor_visible
			  && (xwin->win == cwin)
			  && (owner->r == curow)
			  && (owner->c == cucol)));
	      {	
		struct cell_display *cdt = cd;
		cd = cd->next_damaged;
		cdt->next_damaged = 0;
	      }
	    }
	  disp->damaged = (struct cell_display *) disp;
	  if (must_draw_cursor)
	    set_cursor (1);
	}
    }
}




#ifdef __STDC__
static XFontStruct *
reasonable_font (Xport port, char *name)
#else
static XFontStruct *
reasonable_font (port, name)
     Xport port;
     char *name;
#endif
{
  XFontStruct *f = XLoadQueryFont (port->dpy, name);
  
  if (!f)
    {
      printf("(warning) Font %s could not be loaded.\n", name);
      f = XLoadQueryFont (port->dpy, default_font_name);
      if (!f)
	{
	  panic ("Default font %s could not be loaded.\n",
		 default_font_name);
	  exit(0);
	}
    }
  return f;
}

#ifdef __STDC__
extern void 
xio_open_display (void)
#else
extern void 
xio_open_display ()
#endif
{
  XGCValues gcv;
  XWMHints wmhints;
  
  thePort = (Xport) ck_malloc (sizeof (*thePort));
  thePort->cursor_visible = 1;
  thePort->redisp_needed = 1;
  thePort->xwins = 0;
  
  thePort->dpy = theDisplay;

  bzero (&thePort->input_view, sizeof (struct input_view));
  thePort->input_view.prompt_metric = x_input_metric;
  thePort->input_view.input_metric = x_input_metric;
  
  thePort->screen = DefaultScreen (thePort->dpy);
  thePort->color_map = DefaultColormap (thePort->dpy, thePort->screen);
  
  if (default_fg_color_name && 
      XParseColor (thePort->dpy, thePort->color_map, default_fg_color_name, &thePort->fg_color) &&
      XAllocColor(thePort->dpy, thePort->color_map, &thePort->fg_color))
    thePort->fg_color_pixel = thePort->fg_color.pixel;
  else
    if (default_fg_color_name && !stricmp (default_fg_color_name, "black"))
      thePort->fg_color_pixel = BlackPixel (thePort->dpy, thePort->screen);
    else
      if (default_fg_color_name && !stricmp (default_fg_color_name, "white"))
	thePort->fg_color_pixel = WhitePixel (thePort->dpy, thePort->screen);
  
  if (default_bg_color_name && 
      XParseColor (thePort->dpy, thePort->color_map, default_bg_color_name, &thePort->bg_color) &&
      XAllocColor(thePort->dpy, thePort->color_map, &thePort->bg_color))
    thePort->bg_color_pixel = thePort->bg_color.pixel;
  else
    if (default_bg_color_name && !stricmp (default_bg_color_name, "black"))
      thePort->bg_color_pixel = BlackPixel (thePort->dpy, thePort->screen);
    else
      if (default_bg_color_name && !stricmp (default_bg_color_name, "white"))
	thePort->fg_color_pixel = WhitePixel (thePort->dpy, thePort->screen);
  
  thePort->window =
    XCreateSimpleWindow (thePort->dpy, DefaultRootWindow (thePort->dpy),
			 geom_x, geom_y, geom_w, geom_h,
			 1, thePort->fg_color_pixel, thePort->bg_color_pixel);
  XStoreName (thePort->dpy, thePort->window, oleo_version_string);
  wmhints.flags = InputHint;
  wmhints.input = True;
  XSetWMHints(thePort->dpy, thePort->window, &wmhints);

  thePort->wm_delete_window = XInternAtom(thePort->dpy, "WM_DELETE_WINDOW", False);
  (void) XSetWMProtocols( thePort->dpy, thePort->window, &thePort->wm_delete_window, 1);
  
  gcv.foreground = thePort->bg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->neutral_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground), &gcv);
  
  gcv.foreground = thePort->fg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->normal_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground), &gcv);
  
  gcv.foreground = thePort->bg_color_pixel;
  gcv.background = thePort->fg_color_pixel;
  thePort->standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground), &gcv);
  
  thePort->input_font = reasonable_font (thePort, input_font_name);
  gcv.font = thePort->input_font->fid;
  gcv.foreground = thePort->fg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->input_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.foreground = thePort->bg_color_pixel;
  gcv.background = thePort->fg_color_pixel;
  thePort->standout_input_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.foreground = thePort->fg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->status_font = reasonable_font (thePort, status_font_name);
  gcv.font = thePort->status_font->fid;
  thePort->status_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  thePort->label_font = reasonable_font (thePort, label_font_name);
  gcv.font = thePort->label_font->fid;
  gcv.cap_style = CapRound;
  thePort->label_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont | GCCapStyle), &gcv);
  
  gcv.background = thePort->fg_color_pixel;
  gcv.foreground = thePort->bg_color_pixel;
  thePort->label_standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont | GCCapStyle), &gcv);
  
  gcv.background = thePort->bg_color_pixel;
  gcv.foreground = thePort->fg_color_pixel;
  thePort->text_line_font = reasonable_font (thePort, text_line_font_name);
  gcv.font = thePort->text_line_font->fid;
  thePort->text_line_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);

  info_rows = (thePort->text_line_font->max_bounds.ascent
	       + thePort->text_line_font->max_bounds.descent);
  
  gcv.background = thePort->fg_color_pixel;
  gcv.foreground = thePort->bg_color_pixel;
  thePort->text_line_standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.background = thePort->fg_color_pixel;
  gcv.foreground = thePort->bg_color_pixel;
  thePort->text_line_standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.background = thePort->bg_color_pixel;
  gcv.foreground = thePort->fg_color_pixel;
  
  {
    int x;
    for (x = 0; x < GC_CACHE_SIZE; ++x)
      {
	struct cell_gc *cg =
	  (struct cell_gc *) ck_malloc (sizeof (struct cell_gc));
	cg->font_name = 0;
	cg->font = 0;
	cg->clipped_to = 0;
	cg->cursor = 0;
	cg->gc = XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
			    (GCForeground | GCBackground), &gcv);
	if (!cell_gc_cache)
	  {
	    cg->next = cg;
	    cg->prev = cg;
	  }
	else
	  {
	    cg->next = cell_gc_cache;
	    cg->prev = cg->next->prev;
	    cg->next->prev = cg;
	    cg->prev->next = cg;
	  }
	cell_gc_cache = cg;
      }
  }
  
  XSelectInput (thePort->dpy, thePort->window,
		(ExposureMask | StructureNotifyMask | KeyPressMask
		 | ButtonPressMask));
  
  io_init_windows (geom_h, geom_w, 1, 2,
		   thePort->input_font->ascent + thePort->input_font->descent,
		   (thePort->status_font->ascent
		    + thePort->status_font->descent),
		   thePort->label_font->ascent + thePort->label_font->descent,
		   thePort->label_font->max_bounds.width);
  x11_opened = 1;
  
  {
    struct cell_gc *cgc = cell_gc (thePort, default_font(), 0);
    height_scale = cgc->font->ascent + cgc->font->descent;
    width_scale = XTextWidth (cgc->font, "M", 1);
  }

  /* Setup the arrow keys.  Modifiers effect the last character
   * of the translation.
   */
  {
    static KeySym meta_shift_ctrl[]
      = {Mod1MapIndex, ShiftMapIndex, ControlMapIndex };
#define shift_ctrl &meta_shift_ctrl[1]
#define ctrl  &meta_shift_ctrl[2]
    static KeySym meta_shift[] = {Mod1MapIndex, ShiftMapIndex};
#define shift &meta_shift[1]
    static KeySym ctrl_meta[] = {ControlMapIndex, Mod1MapIndex};
#define meta &ctrl_meta[1]

    static KeySym * mod_combos [] =
      {
	0,
	ctrl, shift, meta,
	shift_ctrl, ctrl_meta, meta_shift,
	meta_shift_ctrl
      };
    static char meta_modp [] = { 0, 0, 0, 1, 0, 1, 1, 1 };
    static char base_char [] =
      {
	'A', '\001', 'a', 'A',
	'\001', '\001', 'a', '\001'
      };
    /* In ansi order: */
    static KeySym arrows[] = {XK_Up, XK_Down, XK_Left, XK_Right};
    int arrow;

    for (arrow = 0; arrow < 4; ++arrow)
      {
	int mod;
	for (mod = 0; mod < 8; ++mod)
	  {
	    char string[10];
	    sprintf (string, "\033[%s%c",
		     meta_modp[mod] ? "\033" : "",
		     base_char[mod] + arrow);
	    XRebindKeysym (thePort->dpy, arrows[arrow],
			   mod_combos[mod], (mod + 2) / 3,
			   string, strlen(string));
	  }
      }
  }
  
  XMapWindow (thePort->dpy, thePort->window);
}


#ifdef __STDC__
extern void
xio_close_display (void)
#else
extern void
xio_close_display ()
#endif
{
  XCloseDisplay (thePort->dpy);
  x11_opened = 0;
}

#ifdef __STDC__
static void
xio_flush (void)
#else
static void
xio_flush ()
#endif
{
  XFlush (theDisplay);
}




#define _io_open_display xio_open_display
#define _io_redisp xio_redisp
#define _io_repaint xio_repaint
#define _io_repaint_win xio_repaint_win
#define _io_close_display xio_close_display
#define _io_input_avail xio_input_avail
#define _io_scan_for_input xio_scan_for_input
#define _io_wait_for_input xio_wait_for_input
#define _io_read_kbd xio_read_kbd
#define _io_nodelay xio_nodelay
#define _io_getch xio_getch
#define _io_bell xio_bell
#define _io_get_chr xio_get_chr
#define _io_update_status xio_update_status
#define _io_info_msg xio_info_msg
#define _io_error_msg xio_error_msg
#define _io_fix_input xio_fix_input
#define _io_move_cursor xio_move_cursor
#define _io_erase xio_erase
#define _io_insert xio_insert
#define _io_over xio_over
#define _io_flush xio_flush
#define _io_clear_input_before xio_clear_input_before
#define _io_clear_input_after xio_clear_input_after
#define _io_pr_cell_win xio_pr_cell_win
#define _io_hide_cell_cursor xio_hide_cell_cursor
#define _io_display_cell_cursor xio_display_cell_cursor
#define _io_cellize_cursor xio_cellize_cursor
#define _io_inputize_cursor xio_inputize_cursor


#ifdef __STDC__
void
x11_graphics (void)
#else
void
x11_graphics ()
#endif
{
  IO_SETUP;
}

#ifdef __STDC__
void
set_x_default_point_size (int l)
#else
void
set_x_default_point_size (l)
     int l;
#endif
{
  if (l > 4)
    {
      cell_font_point_size = l;
      {
	struct cell_gc *cgc = cell_gc (thePort, default_font(), 0);
	height_scale = cgc->font->ascent + cgc->font->descent;
	width_scale = cgc->font->max_bounds.width;
      }
      io_set_scr_size (scr_lines, scr_cols);
    }
}


#endif /* HAVE_X11_X_H */

