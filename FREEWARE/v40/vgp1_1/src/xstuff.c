/*
   X-BASED VECTOR GLOBE PROJECTION

   Xstuff.c

   Copyright (c) 1994,1995 Frederick (Rick) A Niles, 
   niles@axp745.gsfc.nasa.gov

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "vgp.h"
#include <X11/StringDefs.h>
#include <Xm/DrawingA.h>


/***************************/
void
set_window_size (Widget w, image_data * data)
{
  Arg wargs[2];
  /*
   * Get the size of the drawing area.
   */
  XtSetArg (wargs[0], XtNwidth, &(data->width));
  XtSetArg (wargs[1], XtNheight, &(data->height));
  XtGetValues (w, wargs, 2);
}

/******************************************/
void
xor_mode (image_data_ptr data, int dash)
{
  Arg wargs[10];
  XGCValues values;

  XtSetArg (wargs[0], XtNforeground, &values.foreground);
  XtSetArg (wargs[1], XtNbackground, &values.background);
  XtGetValues (data->drawing, wargs, 2);
  values.foreground = values.foreground ^ values.background;
  values.function = GXxor;
  if (dash)
    values.line_style = LineOnOffDash;
  else
    values.line_style = LineSolid;

  XChangeGC (XtDisplay (data->drawing), data->gc,
	     GCForeground | GCBackground |
	     GCFunction | GCLineStyle, &values);
}
/******************************/
void
copy_mode (data)
     image_data *data;
{
  XGCValues values;
/*** Reset drawing mode ****/
  values.function = GXcopy;
  XChangeGC (XtDisplay (data->drawing), data->gc,
	     GCFunction, &values);
}

/******************************/
void
clear_pixmap (Widget w, image_data_ptr data)
{
  /*
   *  Free the old pixmap and create a new pixmap 
   *  the size of the window.
   */
  Pixel color;

  if (data->pix)
    XFreePixmap (XtDisplay (w), data->pix);
  data->pix = XCreatePixmap (XtDisplay (w),
			     DefaultRootWindow (XtDisplay (w)),
			     data->width, data->height,
			     DefaultDepthOfScreen (XtScreen (w)));
  XtVaGetValues (w, XtNbackground, &color, NULL);
  XSetForeground (XtDisplay (w), data->gc, color);
  XFillRectangle (XtDisplay (w), data->pix, data->gc, 0, 0,
		  data->width, data->height);
  XSetForeground (XtDisplay (w), data->gc,
		  BlackPixelOfScreen (XtScreen (w)));
}

/******************************/
void
draw_block_of_lines (Widget w, image_data_ptr data,
		     Drawable draw, XPoint points[], short npoints)
{
  short i, start_mass;

  start_mass = 0;
  for (i = 0; i < npoints; i++)
    {
      if ((points[i].x == 0 && points[i].y == 0)
	  || i == npoints - 1)
	{
	  XDrawLines (XtDisplay (w), draw, data->gc,
		      &(points[start_mass]),
		      i - start_mass, CoordModeOrigin);
	  start_mass = i + 1;
	}
    }
}


/******************************/
void
redisplay (Widget w, image_data_ptr data,
	   XmDrawingAreaCallbackStruct * call_data)
{
  XExposeEvent *event = (XExposeEvent *) call_data->event;
  extern int expose_flg;
  static long last_ana;
  /*printf("Expose Event\n"); */

  if (abs (last_ana - data->ana_c) < 2)
    {
      if (data->ana_c > 2 || data->ana_c == 0)
	{
	  /*
	   * Extract the exposed area from the event and copy
	   * from the saved pixmap to the window.
	   */
	  XCopyArea (XtDisplay (w), data->pix, XtWindow (w), data->gc,
		     event->x, event->y, event->width, event->height,
		     event->x, event->y);
	  expose_flg = 1;
	}
    }
  else
    create_image (w, data);

  last_ana = data->ana_c;
}

/***************************************/
void
draw_rectangle (Widget w, image_data_ptr data)
{
  Dimension max_dis;
  static XPoint rec_ver[5];

  max_dis = max (data->last.x - data->start.x,
		 data->last.y - data->start.y);
  rec_ver[0].x = data->start.x;
  rec_ver[0].y = data->start.y;
  rec_ver[1].x = data->start.x;
  rec_ver[1].y = data->start.y + max_dis;
  rec_ver[2].x = data->start.x + max_dis;
  rec_ver[2].y = data->start.y + max_dis;
  rec_ver[3].x = data->start.x + max_dis;
  rec_ver[3].y = data->start.y;
  rec_ver[4].x = data->start.x;
  rec_ver[4].y = data->start.y;
  /*
   * Draw the new line.
   */
  XDrawLines (XtDisplay (w), XtWindow (w), data->gc,
	      rec_ver, 5, CoordModeOrigin);

}

/**************************/
int 
get_pixel_by_name (w, colorname)
     Widget w;
     char *colorname;
{
  Display *dpy = XtDisplay (w);
  int scr = DefaultScreen (dpy);
  Colormap cmap = DefaultColormap (dpy, scr);
  XColor color, ignore;
  /* 
   * Allocate the named color.
   */
  if (XAllocNamedColor (dpy, cmap, colorname, &color, &ignore))
    return (color.pixel);
  else
    {
      printf ("Warning: Couldn't allocate color %s\n", colorname);
      return (BlackPixel (dpy, scr));
    }
}
