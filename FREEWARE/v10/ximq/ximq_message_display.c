/*
 * XIMQ - display NASA/PDS "IMQ" files on a X11 workstation
 *
 * Copyright © 1991, 1994 Digital Equipment Corporation
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION.
 *
 * DIGITAL EQUIPMENT CORPORATION SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND DIGITAL EQUIPMENT CORPORATION HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Module:
 *
 *	XIMQ_MESSAGE_DISPLAY.C
 *
 * Author:
 *
 *    Frederick G. Kleinsorge, November 1991
 *
 * Environment:
 *
 *    OpenVMS, VAX-C syntax
 *
 * Abstract:
 *
 * Display message logic for XIMQ
 *
 */ 
#include "ximq.h"


void event_on_message_window(XEvent *, int);
void event_on_dismiss_window(XEvent *, int);

/*
 *  Forward refrences
 *
 */
int hide_message_window();
int display_message_window();
int display_message_text();
int display_dismiss_text();
int create_message_window();


/*
 *  X11 stuff
 *
 */
static Window message_window = 0, message_dismiss = 0;

static XFontStruct *font_info;

static GC ms_gc, msR_gc;

static Font file_font;

static unsigned char *ms_text_array[100], *ms_text_ptr, *ms_title, *ms_icon, *ms_dismiss;
static int ms_count, ms_ascent, ms_descent,
           ms_text_start[100], ms_text_length[100], ms_text_width[100];
static unsigned char ms_text_array_buffer[8192];


/*
 *  create_message_window()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
create_message_window(text_array, count, title, icon, dismiss, map_it)

char *text_array[], *title, *icon, *dismiss;
int count, map_it;

{
  int i, j, a, d, temp, x, y,
	dx, dy, d_width, d_height, max_width = 0, max_height = 0;

  float xform;
  XSetWindowAttributes xswa;
  XGCValues xgcv;
  XSizeHints xhint;
  XCharStruct overall;

  if (state.debugging)
    printf("create_message_window\n");

  if (!state.use_message_window) return (FALSE);

 /*
  *  Make sure that the workstation is open
  *
  */
  if (!open_workstation()) return FALSE;

  if (state.message_window_mapped)
    XUnmapWindow(state.display, message_window);

  if (!message_window)
    {
      font_info = XLoadQueryFont(state.display, FONT1);
      file_font = font_info->fid;
    }

  ms_text_ptr = &ms_text_array_buffer[0];

  for (i = 0; i < count; i += 1)
    {
      j = strlen(text_array[i]+1),
      strcpy(ms_text_ptr, text_array[i]+1);

      XTextExtents(font_info, ms_text_ptr, j,
		&temp, &a, &d, &overall);

      ms_text_array[i] = ms_text_ptr;
      ms_text_ptr += j+1;
      ms_text_length[i] = j;
      ms_text_width[i] = overall.width;

      if (state.debugging)
        printf("Message Line %d: %50.50s\n",i, ms_text_array[i]);

      if (overall.width > max_width) max_width = overall.width;

      if ((a+d) > max_height)
	{
	  max_height = a+d;
	  ms_ascent  = a;
	  ms_descent = d;
	}
    }

  XTextExtents(font_info, dismiss, strlen(dismiss),
		&temp, &a, &d, &overall);

  max_width  += 10;
  max_height += 10;

  d_height = a+d;
  d_width  = overall.width;
  dy = (max_height * count) + d_height;
  dx = (max_width-d_width)/2;
  max_height = dy + (d_height*2);

 /*
  *  Center the text...
  *
  */
  for (i = 0; i < count; i += 1)
    {
      if ((text_array[i]+1)[0] == 'c')
        ms_text_start[i] = (max_width-ms_text_width[i])/2;
      else ms_text_start[i] = 5;
    }

  if (state.debugging)
    {
      printf("ascent %d, descent %d, width %d\n", ms_ascent, ms_descent, max_width);
      printf("max_width %d, max_height %d, d_width %d, d_height %d, dx %d, dy %d\n",
		max_width, max_height, d_width, d_height, dx, dy);
    }

  if (!message_window)
    {

      xswa.background_pixel = state.black_pixel;
      xswa.border_pixel     = state.white_pixel;
      xswa.event_mask = ExposureMask;

      FindBestWindowPosition( state.display, 0,
                              max_width, max_height, 0, 0,
                              &x, &y, 0, 0);

      if (state.debugging)
        printf("Best position for message window is X = %d, Y = %d\n",x, y);

      message_window = Add_Window(state.display, state.root,
				x, y,
				max_width, max_height,
				1, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_message_window);

      xgcv.background = state.black_pixel;
      xgcv.foreground = state.white_pixel;
      xgcv.function   = GXcopy;

      ms_gc = XCreateGC(state.display, message_window, 
                    GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, ms_gc, file_font);

      xgcv.background = state.white_pixel;
      xgcv.foreground = state.black_pixel;
      msR_gc = XCreateGC(state.display, message_window, 
                    GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, msR_gc, file_font);

      xswa.background_pixel = state.white_pixel;
      xswa.border_pixel     = state.black_pixel;

      xswa.event_mask = ExposureMask | ButtonMotionMask | ButtonPressMask |
			ButtonReleaseMask | KeyReleaseMask | KeyPressMask;

      message_dismiss = Add_Window(state.display, message_window,
				dx, dy,
				d_width, d_height,
				1, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_dismiss_window);

    }
  else
    {
      XMoveResizeWindow(state.display, message_window,
	100, 100, max_width, max_height);

      XMoveResizeWindow(state.display, message_dismiss,
	dx, dy, d_width, d_height);
    }

 /*
  *  Set the window manager stuff up.
  *
  */
  xhint.flags  = 0;
  xhint.x      = x;
  xhint.y      = y;
  xhint.width  = max_width;
  xhint.height = max_height;
  XSetStandardProperties(state.display, message_window,
				title, icon,
				None, 0, 0, &xhint);

  if (map_it)
    {
      XMapWindow(state.display, message_window);
      state.message_window_mapped = TRUE;
    }
  else
    state.message_window_mapped = FALSE;

  ms_count   = count;
  ms_title   = title;
  ms_icon    = icon;
  ms_dismiss = dismiss;

 /*
  *  Install the colormap
  *
  */
  if (state.need_colormap)
    XInstallColormap(state.display, state.colormap);

  XFlush(state.display);
  XSync(state.display, 0);

  return(TRUE);
}

int
display_message_text()
{
  int i, y;

  if (state.debugging)
    printf("display_message_text\n");

  y = ms_ascent;

  for (i = 0; i < ms_count; i += 1)
    {
      XDrawImageString(state.display, message_window, ms_gc,
		ms_text_start[i], y, ms_text_array[i], ms_text_length[i]);

      y += ms_ascent + ms_descent;
    }

  XFlush(state.display);
}

int
display_dismiss_text()
{

  if (state.debugging)
    printf("display_dismiss_text\n");

  XDrawImageString(state.display, message_dismiss, msR_gc,
			0, ms_ascent, ms_dismiss, strlen(ms_dismiss));

  XFlush(state.display);
}

/*
 *  display_message_window()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
display_message_window(text_array, count, title, icon, dismiss)

char *text_array[], *title, *icon, *dismiss;
int count;

{
  if (state.debugging)
    printf("display_message\n");

  return (create_message_window(text_array, count, title, icon, dismiss, TRUE));

}

/*
 *  hide_message_window()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
hide_message_window()
{

  if (state.debugging)
    printf("hide_message\n");

  if (state.message_window_mapped)
    XUnmapWindow(state.display, message_window);

  state.message_window_mapped = FALSE;

  return(TRUE);
}

void
event_on_message_window(XEvent *event, int from_mainline)
{
  if (event->type == Expose)
    {
      XClearWindow(state.display, message_window);
      display_message_text();
    }
}

void
event_on_dismiss_window(XEvent *event, int from_mainline)
{
  if (event->type == Expose)
    {
      XClearWindow(state.display, message_dismiss);
      display_dismiss_text();
    }
}
