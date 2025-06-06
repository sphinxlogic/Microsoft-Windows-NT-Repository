/* Implements a lightweight menubar widget.  
   Copyright (C) 1992 Lucid, Inc.

This file is part of the Lucid Widget Library.

The Lucid Widget Library is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

The Lucid Widget Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Created by devin@lucid.com */

#include <stdio.h>

#include <sys/types.h>
#include <X11/Xos.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#ifdef VMS
#include <DECW$bitmaps/gray.xbm>
#else
#include <X11/bitmaps/gray>
#endif
#include "xlwmenuP.h"

static int pointer_grabbed;
static XEvent menu_post_event;

static char 
xlwMenuTranslations [] = 
"<BtnDown>:	start()\n\
<Motion>:	drag()\n\
<BtnUp>:	select()\n\
";

#define offset(field) XtOffset(XlwMenuWidget, field)
static XtResource 
xlwMenuResources[] =
{ 
  {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(menu.font),XtRString, "XtDefaultFont"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(menu.foreground), XtRString, "XtDefaultForeground"},
  {XtNbuttonForeground, XtCButtonForeground, XtRPixel, sizeof(Pixel),
     offset(menu.button_foreground), XtRString, "XtDefaultForeground"},
  {XtNmargin, XtCMargin, XtRDimension,  sizeof(Dimension),
     offset(menu.margin), XtRImmediate, (XtPointer)0},
  {XtNhorizontalSpacing, XtCMargin, XtRDimension,  sizeof(Dimension),
     offset(menu.horizontal_spacing), XtRImmediate, (XtPointer)3},
  {XtNverticalSpacing, XtCMargin, XtRDimension,  sizeof(Dimension),
     offset(menu.vertical_spacing), XtRImmediate, (XtPointer)1},
  {XtNarrowSpacing, XtCMargin, XtRDimension,  sizeof(Dimension),
     offset(menu.arrow_spacing), XtRImmediate, (XtPointer)10},

  {XmNshadowThickness, XmCShadowThickness, XtRDimension,
     sizeof (Dimension), offset (menu.shadow_thickness),
     XtRImmediate, (XtPointer) 2},
  {XmNtopShadowColor, XmCTopShadowColor, XtRPixel, sizeof (Pixel),
     offset (menu.top_shadow_color), XtRImmediate, (XtPointer)-1},
  {XmNbottomShadowColor, XmCBottomShadowColor, XtRPixel, sizeof (Pixel),
     offset (menu.bottom_shadow_color), XtRImmediate, (XtPointer)-1},
  {XmNtopShadowPixmap, XmCTopShadowPixmap, XtRPixmap, sizeof (Pixmap),
     offset (menu.top_shadow_pixmap), XtRImmediate, (XtPointer)None},
  {XmNbottomShadowPixmap, XmCBottomShadowPixmap, XtRPixmap, sizeof (Pixmap),
     offset (menu.bottom_shadow_pixmap), XtRImmediate, (XtPointer)None},

  {XtNopen, XtCCallback, XtRCallback, sizeof(XtPointer), 
     offset(menu.open), XtRCallback, (XtPointer)NULL},
  {XtNselect, XtCCallback, XtRCallback, sizeof(XtPointer), 
     offset(menu.select), XtRCallback, (XtPointer)NULL},
  {XtNmenu, XtCMenu, XtRPointer, sizeof(XtPointer),
     offset(menu.contents), XtRImmediate, (XtPointer)NULL},
  {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(menu.cursor_shape), XtRString, (XtPointer)"right_ptr"},
  {XtNhorizontal, XtCHorizontal, XtRInt, sizeof(int),
     offset(menu.horizontal), XtRImmediate, (XtPointer)True},
};
#undef offset

static Boolean XlwMenuSetValues();
static void XlwMenuRealize();
static void XlwMenuRedisplay();
static void XlwMenuResize();
static void XlwMenuInitialize();
static void XlwMenuRedisplay();
static void XlwMenuDestroy();
static void XlwMenuClassInitialize();
static void Start();
static void Drag();
static void Select();

static XtActionsRec 
xlwMenuActionsList [] =
{
  {"start",		Start},
  {"drag",		Drag},
  {"select",		Select},
};

#define SuperClass ((CoreWidgetClass)&coreClassRec)

XlwMenuClassRec xlwMenuClassRec =
{
  {  /* CoreClass fields initialization */
    (WidgetClass) SuperClass,		/* superclass		  */	
    "XlwMenu",				/* class_name		  */
    sizeof(XlwMenuRec),			/* size			  */
    XlwMenuClassInitialize,		/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    XlwMenuInitialize,			/* initialize		  */
    NULL,				/* initialize_hook	  */
    XlwMenuRealize,			/* realize		  */
    xlwMenuActionsList,			/* actions		  */
    XtNumber(xlwMenuActionsList),	/* num_actions		  */
    xlwMenuResources,			/* resources		  */
    XtNumber(xlwMenuResources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    TRUE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    XlwMenuDestroy,			/* destroy		  */
    XlwMenuResize,			/* resize		  */
    XlwMenuRedisplay,			/* expose		  */
    XlwMenuSetValues,			/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    xlwMenuTranslations,		/* tm_table		  */
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* XlwMenuClass fields initialization */
  {
    0					/* dummy */
  },
};

WidgetClass xlwMenuWidgetClass = (WidgetClass) &xlwMenuClassRec;

int submenu_destroyed;

static int next_release_must_exit;

/* Utilities */
static void
push_new_stack (mw, val)
     XlwMenuWidget mw;
     widget_value* val;
{
  if (!mw->menu.new_stack)
    {
      mw->menu.new_stack_length = 10;
      mw->menu.new_stack =
	(widget_value**)XtCalloc (mw->menu.new_stack_length,
				  sizeof (widget_value*));
    }
  else if (mw->menu.new_depth == mw->menu.new_stack_length)
    {
      mw->menu.new_stack_length *= 2;
      mw->menu.new_stack =
	(widget_value**)XtRealloc ((char*)mw->menu.new_stack,
				   mw->menu.new_stack_length * sizeof (widget_value*));
    }
  mw->menu.new_stack [mw->menu.new_depth++] = val;
}

static void
pop_new_stack_if_no_contents (mw)
     XlwMenuWidget mw;
{
  if (mw->menu.new_depth)
    {
      if (!mw->menu.new_stack [mw->menu.new_depth - 1]->contents)
	mw->menu.new_depth -= 1;
    }
}

static void
make_old_stack_space (mw, n)
     XlwMenuWidget mw;
     int n;
{
  if (!mw->menu.old_stack)
    {
      mw->menu.old_stack_length = 10;
      mw->menu.old_stack =
	(widget_value**)XtCalloc (mw->menu.old_stack_length,
				  sizeof (widget_value*));
    }
  else if (mw->menu.old_stack_length < n)
    {
      mw->menu.old_stack_length *= 2;
      mw->menu.old_stack =
	(widget_value**)XtRealloc ((char*)mw->menu.old_stack,
				   mw->menu.old_stack_length * sizeof (widget_value*));
    }
}

/* Size code */
static Boolean
all_dashes_p (s)
     char *s;
{
  char* p;
  for (p = s; *p == '-'; p++);
  return !*p;
}

int
string_width (mw, s)
     XlwMenuWidget mw;
     char *s;
{
  XCharStruct xcs;
  int drop;
  
  XTextExtents (mw->menu.font, s, strlen (s), &drop, &drop, &drop, &xcs);
  return xcs.width;
}

static int
arrow_width (mw)
     XlwMenuWidget mw;
{
  return mw->menu.font->ascent / 2 | 1;
}

static XtResource
nameResource[] =
{ 
  {"labelString",  "LabelString", XtRString, sizeof(String),
     0, XtRImmediate, 0},
};

static char*
resource_widget_value (mw, val)
     XlwMenuWidget mw;
     widget_value *val;
{
  if (!val->toolkit_data)
    {
      char* resourced_name = NULL;
      char* complete_name;
      XtGetSubresources ((Widget) mw,
			 (XtPointer) &resourced_name,
			 val->name, val->name,
			 nameResource, 1, NULL, 0);
      if (!resourced_name)
	resourced_name = val->name;
      if (!val->value)
	{
	  complete_name = (char *) XtMalloc (strlen (resourced_name) + 1);
	  strcpy (complete_name, resourced_name);
	}
      else
	{
	  int complete_length =
	    strlen (resourced_name) + strlen (val->value) + 2;
	  complete_name = XtMalloc (complete_length);
	  *complete_name = 0;
	  strcat (complete_name, resourced_name);
	  strcat (complete_name, " ");
	  strcat (complete_name, val->value);
	}

      val->toolkit_data = complete_name;
      val->free_toolkit_data = True;
    }
  return (char*)val->toolkit_data;
}

/* Returns the sizes of an item */
static void
size_menu_item (mw, val, horizontal_p, label_width, rest_width, height)
     XlwMenuWidget mw;
     widget_value* val;
     int horizontal_p;
     int* label_width;
     int* rest_width;
     int* height;
{
  if (all_dashes_p (val->name))
    {
      *height = 2;
      *label_width = 1;
      *rest_width = 0;
    }
  else
    {
      *height =
	mw->menu.font->ascent + mw->menu.font->descent
	  + 2 * mw->menu.vertical_spacing + 2 * mw->menu.shadow_thickness;
      
      *label_width =
	string_width (mw, resource_widget_value (mw, val))
	  + mw->menu.horizontal_spacing + mw->menu.shadow_thickness;
      
      *rest_width =  mw->menu.horizontal_spacing + mw->menu.shadow_thickness;
      if (!horizontal_p)
	{
	  if (val->contents)
	    *rest_width += arrow_width (mw) + mw->menu.arrow_spacing;
	  else if (val->key)
	    *rest_width +=
	      string_width (mw, val->key) + mw->menu.arrow_spacing;
	}
    }
}

static void
size_menu (mw, level)
     XlwMenuWidget mw;
     int level;
{
  int		label_width = 0;
  int		rest_width = 0;
  int		max_rest_width = 0;
  int		height = 0;
  int		horizontal_p = mw->menu.horizontal && (level == 0);
  widget_value*	val;
  window_state*	ws;

  if (level >= mw->menu.old_depth)
    abort ();

  ws = &mw->menu.windows [level];  
  ws->width = 0;
  ws->height = 0;
  ws->label_width = 0;

  for (val = mw->menu.old_stack [level]->contents; val; val = val->next)
    {
      size_menu_item (mw, val, horizontal_p, &label_width, &rest_width,
		      &height);
      if (horizontal_p)
	{
	  ws->width += label_width + rest_width;
	  if (height > ws->height)
	    ws->height = height;
	}
      else
	{
	  if (label_width > ws->label_width)
	    ws->label_width = label_width;
	  if (rest_width > max_rest_width)
	    max_rest_width = rest_width;
	  ws->height += height;
	}
    }
  
  if (horizontal_p)
    ws->label_width = 0;
  else
    ws->width = ws->label_width + max_rest_width;

  ws->width += 2 * mw->menu.shadow_thickness;
  ws->height += 2 * mw->menu.shadow_thickness;
}


/* Display code */
static void
draw_arrow (mw, window, gc, x, y, width)
     XlwMenuWidget mw;
     Window window;
     GC gc;
     int x;
     int y;
     int width;
{
  XPoint points [3];
  points [0].x = x;
  points [0].y = y + mw->menu.font->ascent;
  points [1].x = x;
  points [1].y = y;
  points [2].x = x + width;
  points [2].y = y + mw->menu.font->ascent / 2;
  
  XFillPolygon (XtDisplay (mw), window, gc, points, 3, Convex,
		CoordModeOrigin);
}

static void
draw_shadow_rectangle (mw, window, x, y, width, height, erase_p)
     XlwMenuWidget mw;
     Window window;
     int x;
     int y;
     int width;
     int height;
     int erase_p;
{
  Display *dpy = XtDisplay (mw);
  GC top_gc = !erase_p ? mw->menu.shadow_top_gc : mw->menu.background_gc;
  GC bottom_gc = !erase_p ? mw->menu.shadow_bottom_gc : mw->menu.background_gc;
  int thickness = mw->menu.shadow_thickness;
  XPoint points [4];
  points [0].x = x;
  points [0].y = y;
  points [1].x = x + width;
  points [1].y = y;
  points [2].x = x + width - thickness;
  points [2].y = y + thickness;
  points [3].x = x;
  points [3].y = y + thickness;
  XFillPolygon (dpy, window, top_gc, points, 4, Convex, CoordModeOrigin);
  points [0].x = x;
  points [0].y = y + thickness;
  points [1].x = x;
  points [1].y = y + height;
  points [2].x = x + thickness;
  points [2].y = y + height - thickness;
  points [3].x = x + thickness;
  points [3].y = y + thickness;
  XFillPolygon (dpy, window, top_gc, points, 4, Convex, CoordModeOrigin);
  points [0].x = x + width;
  points [0].y = y;
  points [1].x = x + width - thickness;
  points [1].y = y + thickness;
  points [2].x = x + width - thickness;
  points [2].y = y + height - thickness;
  points [3].x = x + width;
  points [3].y = y + height - thickness;
  XFillPolygon (dpy, window, bottom_gc, points, 4, Convex, CoordModeOrigin);
  points [0].x = x;
  points [0].y = y + height;
  points [1].x = x + width;
  points [1].y = y + height;
  points [2].x = x + width;
  points [2].y = y + height - thickness;
  points [3].x = x + thickness;
  points [3].y = y + height - thickness;
  XFillPolygon (dpy, window, bottom_gc, points, 4, Convex, CoordModeOrigin);
}


/* Display the menu item and increment where.x and where.y to show how large
** the menu item was. 
*/
static void
display_menu_item (mw, val, ws, where, highlighted_p, horizontal_p, just_compute_p)
     XlwMenuWidget mw;
     widget_value* val;
     window_state* ws;
     XPoint* where;
     Boolean highlighted_p;
     Boolean horizontal_p;
     Boolean just_compute_p;
{
  GC deco_gc;
  GC text_gc;
  int font_ascent = mw->menu.font->ascent;
  int font_descent = mw->menu.font->descent;
  int shadow = mw->menu.shadow_thickness;
  int separator_p = all_dashes_p (val->name);
  int h_spacing = mw->menu.horizontal_spacing;
  int v_spacing = mw->menu.vertical_spacing;
  int label_width;
  int rest_width;
  int height;
  int width;
  int button_p;

  /* compute the sizes of the item */
  size_menu_item (mw, val, horizontal_p, &label_width, &rest_width, &height);

  if (horizontal_p)
    width = label_width + rest_width;
  else
    {
      label_width = ws->label_width;
      width = ws->width - 2 * shadow;
    }

  /* see if it should be a button in the menubar */
  button_p = horizontal_p && val->call_data;

  /* Only highlight an enabled item that has a callback. */
  if (highlighted_p)
    if (!val->enabled || !(val->call_data || val->contents))
      highlighted_p = 0;

  /* do the drawing. */
  if (!just_compute_p)
    {
      /* Add the shadow border of the containing menu */
      int x = where->x + shadow;
      int y = where->y + shadow;

      /* pick the foreground and background GC. */
      if (val->enabled)
	text_gc = button_p ? mw->menu.button_gc : mw->menu.foreground_gc;
      else
	text_gc =
	  button_p ? mw->menu.inactive_button_gc : mw->menu.inactive_gc;
      deco_gc = mw->menu.foreground_gc;

      if (separator_p)
	{
	  XDrawLine (XtDisplay (mw), ws->window, mw->menu.shadow_bottom_gc,
		     x, y, x + width, y);
	  XDrawLine (XtDisplay (mw), ws->window, mw->menu.shadow_top_gc,
		     x, y + 1, x + width, y + 1);
	}
      else 
	{
	  char* display_string = resource_widget_value (mw, val);
	  draw_shadow_rectangle (mw, ws->window, x, y, width, height, True);
	  XDrawString (XtDisplay (mw), ws->window, text_gc,
		       x + h_spacing + shadow,
		       y + v_spacing + shadow + font_ascent,
		       display_string, strlen (display_string));
	  
	  if (!horizontal_p)
	    {
	      if (val->contents)
		{
		  int a_w = arrow_width (mw);
		  draw_arrow (mw, ws->window, deco_gc,
			      x + width - arrow_width (mw)
			      - mw->menu.horizontal_spacing 
			      - mw->menu.shadow_thickness,
			      y + v_spacing + shadow, a_w);
		}
	      else if (val->key)
		{
		  XDrawString (XtDisplay (mw), ws->window, text_gc,
			       x + label_width + mw->menu.arrow_spacing,
			       y + v_spacing + shadow + font_ascent,
			       val->key, strlen (val->key));
		}
	    }

	  else if (button_p)
	    {
#if 1
	      XDrawRectangle (XtDisplay (mw), ws->window, deco_gc,
			      x + shadow, y + shadow,
			      label_width + h_spacing - 1,
			      font_ascent + font_descent + 2 * v_spacing - 1);
	      draw_shadow_rectangle (mw, ws->window, x, y, width, height,
				     False);
#else
	      highlighted_p = True;
#endif
	    }
	  else
	    {
	      XDrawRectangle (XtDisplay (mw), ws->window, 
			      mw->menu.background_gc,
			      x + shadow, y + shadow,
			      label_width + h_spacing - 1,
			      font_ascent + font_descent + 2 * v_spacing - 1);
	      draw_shadow_rectangle (mw, ws->window, x, y, width, height,
				     True);
	    }

	  if (highlighted_p)
	    draw_shadow_rectangle (mw, ws->window, x, y, width, height, False);
	}
    }
  
  where->x += width;
  where->y += height;
}

static void
display_menu (mw, level, just_compute_p, highlighted_pos, hit, hit_return,
	      this, that)
     XlwMenuWidget mw;
     int level;
     Boolean just_compute_p;
     XPoint* highlighted_pos;
     XPoint* hit;
     widget_value** hit_return;
     widget_value* this;
     widget_value* that;
{
  widget_value*	val;
  widget_value* following_item;
  window_state* ws;
  XPoint	where;
  int horizontal_p = mw->menu.horizontal && (level == 0);
  int highlighted_p;
  int just_compute_this_one_p;

  if (level >= mw->menu.old_depth)
    abort ();

  if (level < mw->menu.old_depth - 1)
    following_item = mw->menu.old_stack [level + 1];
  else 
    following_item = NULL;

  if (hit)
    *hit_return = NULL;

  where.x = 0;
  where.y = 0;

  ws = &mw->menu.windows [level];
  for (val = mw->menu.old_stack [level]->contents; val; val = val->next)
    {
      highlighted_p = val == following_item;
      if (highlighted_p && highlighted_pos)
	{
	  if (horizontal_p)
	    highlighted_pos->x = where.x;
	  else
	    highlighted_pos->y = where.y;
	}
      
      just_compute_this_one_p =
	just_compute_p || ((this || that) && val != this &&  val != that);

      display_menu_item (mw, val, ws, &where, highlighted_p, horizontal_p,
			 just_compute_this_one_p);

      if (highlighted_p && highlighted_pos)
	{
	  if (horizontal_p)
	    highlighted_pos->y = where.y;
	  else
	    highlighted_pos->x = where.x;
	}

      if (hit
	  && !*hit_return
	  && (horizontal_p ? hit->x < where.x : hit->y < where.y)
	  && !all_dashes_p (val->name))
	*hit_return = val;

      if (horizontal_p)
	where.y = 0;
      else
	where.x = 0;
    }
  
  if (!just_compute_p)
    draw_shadow_rectangle (mw, ws->window, 0, 0, ws->width, ws->height, False);
}

/* Motion code */
static void
set_new_state (mw, val, level)
     XlwMenuWidget mw;
     widget_value* val;
     int level;
{
  int i;
  
  mw->menu.new_depth = 0;
  for (i = 0; i < level; i++)
    push_new_stack (mw, mw->menu.old_stack [i]);
  push_new_stack (mw, val);
}

static void
make_windows_if_needed (mw, n)
     XlwMenuWidget mw;
     int n;
{
  int i;
  int start_at;
  XSetWindowAttributes xswa;
  int mask;
  Window root = RootWindowOfScreen (DefaultScreenOfDisplay (XtDisplay (mw)));
  window_state* windows;
  
  if (mw->menu.windows_length >= n)
    return;

  xswa.save_under = True;
  xswa.override_redirect = True;
  xswa.background_pixel = mw->core.background_pixel;
  xswa.border_pixel = mw->core.border_pixel;
  xswa.event_mask =
    ExposureMask | PointerMotionMask | PointerMotionHintMask
      | ButtonReleaseMask | ButtonPressMask;
  xswa.cursor = mw->menu.cursor_shape;
  mask = CWSaveUnder | CWOverrideRedirect | CWBackPixel | CWBorderPixel
    | CWEventMask | CWCursor;
  
  if (!mw->menu.windows)
    {
      mw->menu.windows =
	(window_state*)XtMalloc (n * sizeof (window_state));
      start_at = 0;
    }
  else
    {
      mw->menu.windows =
	(window_state*)XtRealloc ((char*)mw->menu.windows,
				  n * sizeof (window_state));
      start_at = mw->menu.windows_length;
    }
  mw->menu.windows_length = n;

  windows = mw->menu.windows;

  for (i = start_at; i < n; i++)
   {
     windows [i].x = 0;
     windows [i].y = 0;
     windows [i].width = 1;
     windows [i].height = 1;
     windows [i].window =
       XCreateWindow (XtDisplay (mw), root, 0, 0, 1, 1,
		      0, 0, CopyFromParent, CopyFromParent, mask, &xswa);
  }
}

/* Make the window fit in the screen */
static void
fit_to_screen (mw, ws, previous_ws, horizontal_p)
     XlwMenuWidget mw;
     window_state* ws;
     window_state* previous_ws;
     Boolean horizontal_p;
{
  int screen_width = WidthOfScreen (XtScreen (mw));
  int screen_height = HeightOfScreen (XtScreen (mw));

  if (ws->x < 0)
    ws->x = 0;
  else if (ws->x + ws->width > screen_width)
    {
      if (!horizontal_p)
	ws->x = previous_ws->x - ws->width;
      else
	ws->x = screen_width - ws->width;
    }
  if (ws->y < 0)
    ws->y = 0;
  else if (ws->y + ws->height > screen_height)
    {
      if (horizontal_p)
	ws->y = previous_ws->y - ws->height;
      else
	ws->y = screen_height - ws->height;
    }
}

/* Updates old_stack from new_stack and redisplays. */
static void
remap_menubar (mw)
     XlwMenuWidget mw;
{
  int i;
  int last_same;
  XPoint selection_position;
  int old_depth = mw->menu.old_depth;
  int new_depth = mw->menu.new_depth;
  widget_value** old_stack;
  widget_value** new_stack;
  window_state* windows;
  widget_value* old_selection;
  widget_value* new_selection;

  /* Check that enough windows and old_stack are ready. */
  make_windows_if_needed (mw, new_depth);
  make_old_stack_space (mw, new_depth);
  windows = mw->menu.windows;
  old_stack = mw->menu.old_stack;
  new_stack = mw->menu.new_stack;

  /* compute the last identical different entry */
  for (i = 1; i < old_depth && i < new_depth; i++)
    if (old_stack [i] != new_stack [i])
      break;
  last_same = i - 1;

  /* Memorize the previously selected item to be able to refresh it */
  old_selection = last_same + 1 < old_depth ? old_stack [last_same + 1] : NULL;
  if (old_selection && !old_selection->enabled)
    old_selection = NULL;
  new_selection = last_same + 1 < new_depth ? new_stack [last_same + 1] : NULL;
  if (new_selection && !new_selection->enabled)
    new_selection = NULL;

  /* updates old_state from new_state.  It has to be done now because
     display_menu (called below) uses the old_stack to know what to display. */
  for (i = last_same + 1; i < new_depth; i++)
    old_stack [i] = new_stack [i];
  mw->menu.old_depth = new_depth;

  /* refresh the last seletion */
  selection_position.x = 0;
  selection_position.y = 0;
  display_menu (mw, last_same, new_selection == old_selection,
		&selection_position, NULL, NULL, old_selection, new_selection);

  /* Now popup the new menus */
  for (i = last_same + 1; i < new_depth && new_stack [i]->contents; i++)
    {
      window_state* previous_ws = &windows [i - 1];
      window_state* ws = &windows [i];

      ws->x =
	previous_ws->x + selection_position.x + mw->menu.shadow_thickness;
      if (!mw->menu.horizontal || i > 1)
	ws->x += mw->menu.shadow_thickness;
      ws->y =
	previous_ws->y + selection_position.y + mw->menu.shadow_thickness;

      size_menu (mw, i);

      fit_to_screen (mw, ws, previous_ws, mw->menu.horizontal && i == 1);

      XClearWindow (XtDisplay (mw), ws->window);
      XMoveResizeWindow (XtDisplay (mw), ws->window, ws->x, ws->y,
			 ws->width, ws->height);
      XMapRaised (XtDisplay (mw), ws->window);
      display_menu (mw, i, False, &selection_position, NULL, NULL, NULL, NULL);
    }

  /* unmap the menus that popped down */
  for (i = new_depth - 1; i < old_depth; i++)
    if (i >= new_depth || !new_stack [i]->contents)
      XUnmapWindow (XtDisplay (mw), windows [i].window);
}

static Boolean
motion_event_is_in_menu (mw, ev, level, relative_pos)
     XlwMenuWidget mw;
     XMotionEvent* ev;
     int level;
     XPoint* relative_pos;
{
  window_state* ws = &mw->menu.windows [level];
  int x = level == 0 ? ws->x : ws->x + mw->menu.shadow_thickness;
  int y = level == 0 ? ws->y : ws->y + mw->menu.shadow_thickness;
  relative_pos->x = ev->x_root - x;
  relative_pos->y = ev->y_root - y;
  return (x < ev->x_root && ev->x_root < x + ws->width
	  && y < ev->y_root && ev->y_root < y + ws->height);
}

static Boolean
map_event_to_widget_value (mw, ev, val, level)
     XlwMenuWidget mw;
     XMotionEvent* ev;
     widget_value** val;
     int* level;
{
  int 		i;
  XPoint	relative_pos;
  window_state*	ws;

  *val = NULL;
  
  /* Find the window */
  for (i = mw->menu.old_depth - 1; i >= 0; i--)
    {
      ws = &mw->menu.windows [i];
      if (ws && motion_event_is_in_menu (mw, ev, i, &relative_pos))
	{
	  display_menu (mw, i, True, NULL, &relative_pos, val, NULL, NULL);

	  if (*val)
	    {
	      *level = i + 1;
	      return True;
	    }
	}
    }
  return False;
}

/* Procedures */
static void
make_drawing_gcs (mw)
     XlwMenuWidget mw;
{
  XGCValues xgcv;

  xgcv.font = mw->menu.font->fid;
  xgcv.foreground = mw->menu.foreground;
  xgcv.background = mw->core.background_pixel;
  mw->menu.foreground_gc = XtGetGC ((Widget)mw,
				    GCFont | GCForeground | GCBackground,
				    &xgcv);
  
  xgcv.font = mw->menu.font->fid;
  xgcv.foreground = mw->menu.button_foreground;
  xgcv.background = mw->core.background_pixel;
  mw->menu.button_gc = XtGetGC ((Widget)mw,
				GCFont | GCForeground | GCBackground,
				&xgcv);
  
  xgcv.font = mw->menu.font->fid;
  xgcv.foreground = mw->menu.foreground;
  xgcv.background = mw->core.background_pixel;
  xgcv.fill_style = FillStippled;
  xgcv.stipple = mw->menu.gray_pixmap;
  mw->menu.inactive_gc = XtGetGC ((Widget)mw,
				  (GCFont | GCForeground | GCBackground
				   | GCFillStyle | GCStipple), &xgcv);
  
  xgcv.font = mw->menu.font->fid;
  xgcv.foreground = mw->menu.button_foreground;
  xgcv.background = mw->core.background_pixel;
  xgcv.fill_style = FillStippled;
  xgcv.stipple = mw->menu.gray_pixmap;
  mw->menu.inactive_button_gc = XtGetGC ((Widget)mw,
				  (GCFont | GCForeground | GCBackground
				   | GCFillStyle | GCStipple), &xgcv);
  
  xgcv.font = mw->menu.font->fid;
  xgcv.foreground = mw->core.background_pixel;
  xgcv.background = mw->menu.foreground;
  mw->menu.background_gc = XtGetGC ((Widget)mw,
				    GCFont | GCForeground | GCBackground,
				    &xgcv);
}

static void
release_drawing_gcs (mw)
     XlwMenuWidget mw;
{
  XtReleaseGC ((Widget) mw, mw->menu.foreground_gc);
  XtReleaseGC ((Widget) mw, mw->menu.button_gc);
  XtReleaseGC ((Widget) mw, mw->menu.inactive_gc);
  XtReleaseGC ((Widget) mw, mw->menu.inactive_button_gc);
  XtReleaseGC ((Widget) mw, mw->menu.background_gc);
  /* let's get some segvs if we try to use these... */
  mw->menu.foreground_gc = (GC) -1;
  mw->menu.button_gc = (GC) -1;
  mw->menu.inactive_gc = (GC) -1;
  mw->menu.inactive_button_gc = (GC) -1;
  mw->menu.background_gc = (GC) -1;
}

#define MINL(x,y) ((((unsigned long) (x)) < ((unsigned long) (y))) \
		   ? ((unsigned long) (x)) : ((unsigned long) (y)))

static void
make_shadow_gcs (mw)
     XlwMenuWidget mw;
{
  XGCValues xgcv;
  unsigned long pm = 0;
  Display *dpy = XtDisplay ((Widget) mw);
  Colormap cmap = DefaultColormapOfScreen (XtScreen ((Widget) mw));
  XColor topc, botc;
  int top_frobbed = 0, bottom_frobbed = 0;

  if (mw->menu.top_shadow_color == -1)
    mw->menu.top_shadow_color = mw->core.background_pixel;
  if (mw->menu.bottom_shadow_color == -1)
    mw->menu.bottom_shadow_color = mw->menu.foreground;

  if (mw->menu.top_shadow_color == mw->core.background_pixel ||
      mw->menu.top_shadow_color == mw->menu.foreground)
    {
      topc.pixel = mw->core.background_pixel;
      XQueryColor (dpy, cmap, &topc);
      /* don't overflow/wrap! */
      topc.red   = MINL (65535, topc.red   * 1.2);
      topc.green = MINL (65535, topc.green * 1.2);
      topc.blue  = MINL (65535, topc.blue  * 1.2);
      if (XAllocColor (dpy, cmap, &topc))
	{
	  mw->menu.top_shadow_color = topc.pixel;
	  top_frobbed = 1;
	}
    }
  if (mw->menu.bottom_shadow_color == mw->menu.foreground ||
      mw->menu.bottom_shadow_color == mw->core.background_pixel)
    {
      botc.pixel = mw->core.background_pixel;
      XQueryColor (dpy, cmap, &botc);
      botc.red   *= 0.6;
      botc.green *= 0.6;
      botc.blue  *= 0.6;
      if (XAllocColor (dpy, cmap, &botc))
	{
	  mw->menu.bottom_shadow_color = botc.pixel;
	  bottom_frobbed = 1;
	}
    }

  if (top_frobbed && bottom_frobbed)
    {
      int top_avg = ((topc.red / 3) + (topc.green / 3) + (topc.blue / 3));
      int bot_avg = ((botc.red / 3) + (botc.green / 3) + (botc.blue / 3));
      if (bot_avg > top_avg)
	{
	  Pixel tmp = mw->menu.top_shadow_color;
	  mw->menu.top_shadow_color = mw->menu.bottom_shadow_color;
	  mw->menu.bottom_shadow_color = tmp;
	}
      else if (topc.pixel == botc.pixel)
	{
	  if (botc.pixel == mw->menu.foreground)
	    mw->menu.top_shadow_color = mw->core.background_pixel;
	  else
	    mw->menu.bottom_shadow_color = mw->menu.foreground;
	}
    }

  if (!mw->menu.top_shadow_pixmap &&
      mw->menu.top_shadow_color == mw->core.background_pixel)
    {
      mw->menu.top_shadow_pixmap = mw->menu.gray_pixmap;
      mw->menu.top_shadow_color = mw->menu.foreground;
    }
  if (!mw->menu.bottom_shadow_pixmap &&
      mw->menu.bottom_shadow_color == mw->core.background_pixel)
    {
      mw->menu.bottom_shadow_pixmap = mw->menu.gray_pixmap;
      mw->menu.bottom_shadow_color = mw->menu.foreground;
    }

  xgcv.fill_style = FillStippled;
  xgcv.foreground = mw->menu.top_shadow_color;
  xgcv.stipple = mw->menu.top_shadow_pixmap;
  pm = (xgcv.stipple ? GCStipple|GCFillStyle : 0);
  mw->menu.shadow_top_gc = XtGetGC ((Widget)mw, GCForeground | pm, &xgcv);

  xgcv.foreground = mw->menu.bottom_shadow_color;
  xgcv.stipple = mw->menu.bottom_shadow_pixmap;
  pm = (xgcv.stipple ? GCStipple|GCFillStyle : 0);
  mw->menu.shadow_bottom_gc = XtGetGC ((Widget)mw, GCForeground | pm, &xgcv);
}


static void
release_shadow_gcs (mw)
     XlwMenuWidget mw;
{
  XtReleaseGC ((Widget) mw, mw->menu.shadow_top_gc);
  XtReleaseGC ((Widget) mw, mw->menu.shadow_bottom_gc);
}

static void
XlwMenuInitialize (request, new, args, num_args)
     Widget request;
     Widget new;
     ArgList args;
     Cardinal *num_args;
{
  /* Get the GCs and the widget size */
  XlwMenuWidget mw = (XlwMenuWidget)new;
  
  XSetWindowAttributes xswa;
  int mask;
  
  Window window = RootWindowOfScreen (DefaultScreenOfDisplay (XtDisplay (mw)));
  Display* display = XtDisplay (mw);
  
#if 0
  widget_value *tem = (widget_value *) XtMalloc (sizeof (widget_value));

  /* _XtCreate is freeing the object that was passed to us,
     so make a copy that we will actually keep.  */
  lwlib_bcopy (mw->menu.contents, tem, sizeof (widget_value));
  mw->menu.contents = tem;
#endif

/*  mw->menu.cursor = XCreateFontCursor (display, mw->menu.cursor_shape); */
  mw->menu.cursor = mw->menu.cursor_shape;
  
  mw->menu.gray_pixmap = XCreatePixmapFromBitmapData (display, window,
						      gray_bits, gray_width,
						      gray_height, 1, 0, 1);
  
  make_drawing_gcs (mw);
  make_shadow_gcs (mw);
  
  xswa.background_pixel = mw->core.background_pixel;
  xswa.border_pixel = mw->core.border_pixel;
  mask = CWBackPixel | CWBorderPixel;
  
  mw->menu.popped_up = False;
  
  mw->menu.old_depth = 1;
  mw->menu.old_stack = (widget_value**)XtMalloc (sizeof (widget_value*));
  mw->menu.old_stack_length = 1;
  mw->menu.old_stack [0] = mw->menu.contents;
  
  mw->menu.new_depth = 0;
  mw->menu.new_stack = 0;
  mw->menu.new_stack_length = 0;
  push_new_stack (mw, mw->menu.contents);
  
  mw->menu.windows = (window_state*)XtMalloc (sizeof (window_state));
  mw->menu.windows_length = 1;
  mw->menu.windows [0].x = 0;
  mw->menu.windows [0].y = 0;
  mw->menu.windows [0].width = 0;
  mw->menu.windows [0].height = 0;
  size_menu (mw, 0);
  
  mw->core.width = mw->menu.windows [0].width;
  mw->core.height = mw->menu.windows [0].height;
}

static void
XlwMenuClassInitialize ()
{
}

static void
XlwMenuRealize (w, valueMask, attributes)
     Widget w;
     Mask *valueMask;
     XSetWindowAttributes *attributes;
{
  XlwMenuWidget mw = (XlwMenuWidget)w;
  XSetWindowAttributes xswa;
  int mask;

  (*xlwMenuWidgetClass->core_class.superclass->core_class.realize)
    (w, valueMask, attributes);

  xswa.save_under = True;
  xswa.cursor = mw->menu.cursor_shape;
  mask = CWSaveUnder | CWCursor;
  XChangeWindowAttributes (XtDisplay (w), XtWindow (w), mask, &xswa);

  mw->menu.windows [0].window = XtWindow (w);
  mw->menu.windows [0].x = w->core.x;
  mw->menu.windows [0].y = w->core.y;
  mw->menu.windows [0].width = w->core.width;
  mw->menu.windows [0].height = w->core.height;
}

/* Only the toplevel menubar/popup is a widget so it's the only one that
   receives expose events through Xt.  So we repaint all the other panes
   when receiving an Expose event. */
static void 
XlwMenuRedisplay (w, ev, region)
     Widget w;
     XEvent* ev;
     Region region;
{
  XlwMenuWidget mw = (XlwMenuWidget)w;
  int i;

  /* If we have a depth beyond 1, it's because a submenu was displayed.
     If the submenu has been destroyed, set the depth back to 1.  */
  if (submenu_destroyed)
    {
      mw->menu.old_depth = 1;
      submenu_destroyed = 0;
    }

  for (i = 0; i < mw->menu.old_depth; i++)
    display_menu (mw, i, False, NULL, NULL, NULL, NULL, NULL);
}

static void 
XlwMenuDestroy (w)
     Widget w;
{
  int i;
  XlwMenuWidget mw = (XlwMenuWidget) w;

  if (pointer_grabbed)
    XtUngrabPointer ((Widget)w, CurrentTime);
  pointer_grabbed = 0;

  submenu_destroyed = 1;

  release_drawing_gcs (mw);
  release_shadow_gcs (mw);

  /* this doesn't come from the resource db but is created explicitly
     so we must free it ourselves. */
  XFreePixmap (XtDisplay (mw), mw->menu.gray_pixmap);
  mw->menu.gray_pixmap = (Pixmap) -1;

#if 0
  /* Do free mw->menu.contents because nowadays we copy it
     during initialization.  */
  XtFree (mw->menu.contents);
#endif

  /* Don't free mw->menu.contents because that comes from our creator.
     The `*_stack' elements are just pointers into `contents' so leave
     that alone too.  But free the stacks themselves. */
  if (mw->menu.old_stack) XtFree ((char *) mw->menu.old_stack);
  if (mw->menu.new_stack) XtFree ((char *) mw->menu.new_stack);

  /* Remember, you can't free anything that came from the resource
     database.  This includes:
         mw->menu.cursor
         mw->menu.top_shadow_pixmap
         mw->menu.bottom_shadow_pixmap
         mw->menu.font
     Also the color cells of top_shadow_color, bottom_shadow_color,
     foreground, and button_foreground will never be freed until this
     client exits.  Nice, eh?
   */

  /* start from 1 because the one in slot 0 is w->core.window */
  for (i = 1; i < mw->menu.windows_length; i++)
    XDestroyWindow (XtDisplay (mw), mw->menu.windows [i].window);
  if (mw->menu.windows)
    XtFree ((char *) mw->menu.windows);
}

static Boolean 
XlwMenuSetValues (current, request, new)
     Widget current;
     Widget request;
     Widget new;
{
  XlwMenuWidget oldmw = (XlwMenuWidget)current;
  XlwMenuWidget newmw = (XlwMenuWidget)new;
  Boolean redisplay = False;
  int i;

  if (newmw->menu.contents
      && newmw->menu.contents->contents
      && newmw->menu.contents->contents->change >= VISIBLE_CHANGE)
    redisplay = True;

  if (newmw->core.background_pixel != oldmw->core.background_pixel
      || newmw->menu.foreground != oldmw->menu.foreground
      || newmw->menu.font != oldmw->menu.font)
    {
      release_drawing_gcs (newmw);
      make_drawing_gcs (newmw);
      redisplay = True;
      
      for (i = 0; i < oldmw->menu.windows_length; i++)
	{
	  XSetWindowBackground (XtDisplay (oldmw),
				oldmw->menu.windows [i].window,
				newmw->core.background_pixel);
	  /* clear windows and generate expose events */
	  XClearArea (XtDisplay (oldmw), oldmw->menu.windows[i].window,
		      0, 0, 0, 0, True);
	}
    }

  return redisplay;
}

static void 
XlwMenuResize (w)
     Widget w;
{
  XlwMenuWidget mw = (XlwMenuWidget)w;

  if (mw->menu.popped_up)
    {
      /* Don't allow the popup menu to resize itself.  */
      mw->core.width = mw->menu.windows [0].width;
      mw->core.height = mw->menu.windows [0].height;
      mw->core.parent->core.width = mw->core.width ;
      mw->core.parent->core.height = mw->core.height ;
    }
  else
    {
      mw->menu.windows [0].width = mw->core.width;
      mw->menu.windows [0].height = mw->core.height;
    }
}

/* Action procedures */
static void
handle_single_motion_event (mw, ev)
     XlwMenuWidget mw;
     XMotionEvent* ev;
{
  widget_value*	val;
  int 		level;

  if (!map_event_to_widget_value (mw, ev, &val, &level))
    pop_new_stack_if_no_contents (mw);
  else
    set_new_state (mw, val, level);
  remap_menubar (mw);
  
#if 0
  /* Sync with the display.  Makes it feel better on X terms. */
  XSync (XtDisplay (mw), False);
#endif
}

static void
handle_motion_event (mw, ev)
     XlwMenuWidget mw;
     XMotionEvent* ev;
{
  int x = ev->x_root;
  int y = ev->y_root;
  int state = ev->state;

  handle_single_motion_event (mw, ev);

  /* allow motion events to be generated again */
  if (ev->is_hint
      && XQueryPointer (XtDisplay (mw), ev->window,
			&ev->root, &ev->subwindow,
			&ev->x_root, &ev->y_root,
			&ev->x, &ev->y,
			&ev->state)
      && ev->state == state
      && (ev->x_root != x || ev->y_root != y))
    handle_single_motion_event (mw, ev);
}

static void 
Start (w, ev, params, num_params)
     Widget w;
     XEvent *ev;
     String *params;
     Cardinal *num_params;
{
  XlwMenuWidget mw = (XlwMenuWidget)w;

  if (!mw->menu.popped_up)
    {
      menu_post_event = *ev;
      next_release_must_exit = 0;
    }
  else
    /* If we push a button while the menu is posted semipermanently,
       releasing the button should always pop the menu down.  */
    next_release_must_exit = 1;

  XtCallCallbackList ((Widget)mw, mw->menu.open, NULL);
  
  /* notes the absolute position of the menubar window */
  mw->menu.windows [0].x = ev->xmotion.x_root - ev->xmotion.x;
  mw->menu.windows [0].y = ev->xmotion.y_root - ev->xmotion.y;

  /* handles the down like a move, slots are compatible */
  handle_motion_event (mw, &ev->xmotion);
}

static void 
Drag (w, ev, params, num_params)
     Widget w;
     XEvent *ev;
     String *params;
     Cardinal *num_params;
{
  XlwMenuWidget mw = (XlwMenuWidget)w;
  handle_motion_event (mw, &ev->xmotion);
}

static void 
Select (w, ev, params, num_params)
     Widget w;
     XEvent *ev;
     String *params;
     Cardinal *num_params;
{
  XlwMenuWidget mw = (XlwMenuWidget)w;
  widget_value* selected_item = mw->menu.old_stack [mw->menu.old_depth - 1];
  
  /* If user releases the button quickly, without selecting anything,
     after the initial down-click that brought the menu up,
     do nothing.  */
  if ((selected_item == 0
       || ((widget_value *) selected_item)->call_data == 0)
      && !next_release_must_exit
      && (ev->xbutton.time - menu_post_event.xbutton.time
	  < XtGetMultiClickTime (XtDisplay (w))))
    return;

  /* pop down everything.  */
  mw->menu.new_depth = 1;
  remap_menubar (mw);

  if (mw->menu.popped_up)
    {
      mw->menu.popped_up = False;
      XtUngrabPointer ((Widget)mw, ev->xmotion.time);
      XtPopdown (XtParent (mw));
    }

  /* callback */
  XtCallCallbackList ((Widget)mw, mw->menu.select, (XtPointer)selected_item);
  
}


/* Special code to pop-up a menu */
void
pop_up_menu (mw, event)
     XlwMenuWidget mw;
     XButtonPressedEvent* event;
{
  int		x = event->x_root;
  int		y = event->y_root;
  int		w;
  int		h;
  int		borderwidth = mw->menu.shadow_thickness;
  Screen*	screen = XtScreen (mw);

  next_release_must_exit = 0;

  XtCallCallbackList ((Widget)mw, mw->menu.open, NULL);

  size_menu (mw, 0);

  w = mw->menu.windows [0].width;
  h = mw->menu.windows [0].height;

  x -= borderwidth;
  y -= borderwidth;
  if (x < borderwidth)
    x = borderwidth;
  if (x + w + 2 * borderwidth > WidthOfScreen (screen))
    x = WidthOfScreen (screen) - w - 2 * borderwidth;
  if (y < borderwidth)
    y = borderwidth;
  if (y + h + 2 * borderwidth> HeightOfScreen (screen))
    y = HeightOfScreen (screen) - h - 2 * borderwidth;

  mw->menu.popped_up = True;
  XtConfigureWidget (XtParent (mw), x, y, w, h,
		     XtParent (mw)->core.border_width);
  XtPopup (XtParent (mw), XtGrabExclusive);
  display_menu (mw, 0, False, NULL, NULL, NULL, NULL, NULL);
#ifdef emacs
  x_catch_errors ();
#endif
  XtGrabPointer ((Widget)mw, False,
		 (PointerMotionMask | PointerMotionHintMask | ButtonReleaseMask
		  | ButtonPressMask),
		 GrabModeAsync, GrabModeAsync, None, mw->menu.cursor_shape,
		 event->time);
  pointer_grabbed = 1;
#ifdef emacs
  if (x_had_errors_p ())
    {
      pointer_grabbed = 0;
      XtUngrabPointer ((Widget)mw, event->time);
    }
  x_uncatch_errors ();
#endif

  mw->menu.windows [0].x = x + borderwidth;
  mw->menu.windows [0].y = y + borderwidth;

  handle_motion_event (mw, (XMotionEvent*)event);
}
