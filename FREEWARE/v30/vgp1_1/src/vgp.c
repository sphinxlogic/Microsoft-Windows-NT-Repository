/*
   X-BASED VECTOR GLOBE PROJECTION

   vgp.c

   Copyright (c) 1994, 1995 Frederick (Rick) A Niles
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
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/ScaleP.h>
#include "getopt.h"
#include <stdlib.h>

#ifndef SHOW_VEC
#define SHOW_VEC 1
#endif

/** This is for a Lesstif bug. **/
#ifndef XmFONTLIST_DEFAULT_TAG
#define XmFONTLIST_DEFAULT_TAG XmSTRING_DEFAULT_CHARSET
#endif

/*
 ** Global Variables
 */
static int scale_change = 1;
static int bBusyAnimating = 0;
static int screenClear;
int expose_flg;
static int show_vector = SHOW_VEC;
static int button_down = 0;
static int button_down_n = 1;
static XtResource block_resources[1][MAX_SETS] =
{
  {
    {"linecolor1", "LineColor", XtRPixel, sizeof (Pixel),
     XtOffset (point_block_ptr, color), XtRString, "Black"},
    {"linecolor2", "LineColor", XtRPixel, sizeof (Pixel),
     XtOffset (point_block_ptr, color), XtRString, "Red"},
    {"linecolor3", "LineColor", XtRPixel, sizeof (Pixel),
     XtOffset (point_block_ptr, color), XtRString, "Grey"},
    {"linecolor4", "LineColor", XtRPixel, sizeof (Pixel),
     XtOffset (point_block_ptr, color), XtRString, "LightBlue"}}
};

int
main (int argc, char *argv[])
{
  static image_data ImageData;
  Widget toplevel, canvas, layout, slider, start, full, text, quit;
  Arg wargs[10];
  short n;


/*  toplevel = XtInitialize(argv[0], "Vgp", NULL, 0,(Cardinal *) &argc,argv); */
  toplevel = XtInitialize (argv[0], "Vgp", NULL, 0, &argc, argv);
  get_input (argc, argv, &ImageData);


  layout = XtCreateManagedWidget ("layout",
				  xmFormWidgetClass,
				  toplevel, NULL, 0);
/* *INDENT-OFF* */
  n = 0;
  XtSetArg (wargs[n], XmNwidth, 400); n++;
  XtSetArg (wargs[n], XmNheight, 400); n++;
/* *INDENT-ON* */


  canvas = XtCreateManagedWidget ("canvas",
				  xmDrawingAreaWidgetClass,
				  layout, wargs, n);
  slider = (Widget) make_slider (layout, &ImageData, "Speed",
				 (void *) slider_moved);

  start = XtCreateManagedWidget ("Start Animation",
				 xmPushButtonWidgetClass,
				 layout, NULL, 0);

  quit = XtCreateManagedWidget ("Quit",
				xmPushButtonWidgetClass,
				layout, NULL, 0);

  full = XtCreateManagedWidget ("Full Size",
				xmPushButtonWidgetClass,
				layout, NULL, 0);
  text = XtCreateManagedWidget ("Text",
				xmTextWidgetClass,
				layout, NULL, 0);
/* *INDENT-OFF* */
  n = 0;
  XtSetArg (wargs[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNtopAttachment, XmATTACH_NONE); n++;
  XtSetValues (quit, wargs, n);
  n = 0;
  XtSetArg (wargs[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (wargs[n], XmNbottomWidget, quit); n++;
  XtSetArg (wargs[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNtopAttachment, XmATTACH_NONE); n++;
  XtSetValues (full, wargs, n);
  n = 0;
  XtSetArg (wargs[n], XmNtopAttachment, XmATTACH_NONE); n++;
  XtSetArg (wargs[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (wargs[n], XmNbottomWidget, full); n++;
  XtSetArg (wargs[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetValues (start, wargs, n);
  n = 0;
  XtSetArg (wargs[n], XmNtopAttachment, XmATTACH_NONE); n++;
  XtSetArg (wargs[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (wargs[n], XmNbottomWidget, start); n++;
  XtSetArg (wargs[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetValues (slider, wargs, n);
  n = 0;
  XtSetArg (wargs[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg (wargs[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (wargs[n], XmNbottomWidget, slider); n++;
  XtSetValues (canvas, wargs, n);
/* *INDENT-ON* */



  init_data (canvas, &ImageData);
  XtAddCallback (canvas, XmNexposeCallback, (XtCallbackProc) redisplay,
		 &ImageData);
  XtAddCallback (canvas, XmNresizeCallback, (XtCallbackProc) resize,
		 &ImageData);
  XtAddCallback (start, XmNactivateCallback, (XtCallbackProc) restart_anim,
		 &ImageData);
  XtAddCallback (full, XmNactivateCallback, (XtCallbackProc) full_size,
		 &ImageData);
  XtAddCallback (quit, XmNactivateCallback, (XtCallbackProc) clean_up,
		 &ImageData);
  XtAddEventHandler (text, KeyPressMask, FALSE,
		     (XtPointer) keybd_event, &ImageData);
  XtAddEventHandler (canvas, ButtonPressMask, FALSE,
		     (XtPointer) start_rubber_band, &ImageData);
  XtAddEventHandler (canvas, Button1MotionMask, FALSE,
		     (XtPointer) track_button1_motion, &ImageData);
  XtAddEventHandler (canvas, Button2MotionMask, FALSE,
		     (XtPointer) track_rubber_band, &ImageData);
  XtAddEventHandler (canvas, Button3MotionMask, FALSE,
		     (XtPointer) track_pan_motion, &ImageData);
  XtAddEventHandler (canvas, ButtonReleaseMask, FALSE,
		     (XtPointer) end_rubber_band, &ImageData);
  XtRealizeWidget (toplevel);
  /*
   * Establish a passive grab, for any button press.
   * Force the sprite to stay within the canvas window, and
   * change the sprite to a cross_hair.
   */
  XGrabButton (XtDisplay (canvas), AnyButton, AnyModifier,
	       XtWindow (canvas), TRUE,
	       ButtonPressMask | ButtonMotionMask |
	       ButtonReleaseMask,
	       GrabModeAsync, GrabModeAsync,
	       XtWindow (canvas),
	       XCreateFontCursor (XtDisplay (canvas),
				  XC_crosshair));


  scale_change = 1;
  XtRealizeWidget (toplevel);
  full_size (full, &ImageData);
  resize (canvas, &ImageData, NULL);
  XtMainLoop ();
  return 0;
}

/**************** init_data *************/

void
init_data (Widget w, image_data_ptr data)
{
  int i;
  set_window_size (w, data);
  /*
   * Find out how many colors we have to work with, and  
   * create a default, writable, graphics context.
   */
  data->ncolors = XDisplayCells (XtDisplay (w),
				 XDefaultScreen (XtDisplay (w)));
  data->gc = XCreateGC (XtDisplay (w),
			DefaultRootWindow (XtDisplay (w)),
			(unsigned long) NULL, NULL);

  for (i = 0; i <= data->nblocks; i++)
    XtGetApplicationResources (w, &(data->block[i]), &(block_resources[0][i]),
			       XtNumber (block_resources), NULL, 0);
/*  data->block[0].color = get_pixel_by_name(w, "black"); */
/*  data->block[1].color = get_pixel_by_name(w, "red"); */
/*  data->block[2].color = get_pixel_by_name(w, "grey"); */
/*  for(i = 3;i <= data->nblocks; i++)
   data->block[i].color = get_pixel_by_name(w, "LightBlue"); */


  /*
   *  Initialize the pixmap to NULL.
   */
  data->pix = (Pixmap) NULL;
  data->block[0].type = HEMILINES;
  data->block[1].type = TIMEDATA;
  data->block[2].type = BACKGROUND;
  data->ana_c = 0;
  data->drawing = w;
  data->zoom = 1;
  data->ana_delay = INIT_SPEED;
  data->iid = (XtIntervalId) NULL;
  data->rotation[0] = 2.;
  data->rotation[1] = -0.7;
  data->rotation[2] = -2.86;
  create_rot_matrix (data);
  data->start_T[0][0] = 1;
  data->start_T[0][1] = 0;
  data->start_T[0][2] = 0;
  data->start_T[1][0] = 0;
  data->start_T[1][1] = 1;
  data->start_T[1][2] = 0;
  data->start_T[2][0] = 0;
  data->start_T[2][1] = 0;
  data->start_T[2][2] = 1;
  create3DSphere (data);

}

int
clean_up (Widget w, image_data_ptr data, caddr_t call_data)
{
  register int i;

  for (i = 0; i < data->nblocks; i++)
    {
      free (data->block[i].vector);
      free (data->block[i].points);
    }

  exit (0);
  return 0;
}
/****************************************/
/********* Display Section **************/
/****************************************/


/********************************/
void
resize (Widget w, image_data_ptr data, caddr_t call_data)
{
  set_window_size (w, data);
  scale_change = 1;
  create_image (w, data);
}

/****************************************/
void
full_size (Widget w, image_data_ptr data)
{
  double radius;

  radius = ((double) min (data->width, data->height)) / 2.0;
  data->zoom = radius;
  data->pan.x = radius;
  data->pan.y = radius;

  scale_change = 1;
  create_image (data->drawing, data);
}
/****************************************/
void
create_image (Widget w, image_data_ptr data)
{
  int num;
  int i;

  /*printf("Creating Image.\n"); */

  data->block[0].type = 0;
  data->block[0].npoints2D = 0;
  num = INC;
  TD_to_screen (data->T, data->zoom, data->pan, &data->xyplane[0],
		&num, &(data->block[0].points[data->block[0].npoints2D]), 0);
  data->block[0].npoints2D += num;
  data->block[0].points[data->block[0].npoints2D].x = 0;
  data->block[0].points[data->block[0].npoints2D++].y = 0;
  num = INC;
  TD_to_screen (data->T, data->zoom, data->pan, &(data->yzplane[0]),
		&num, &(data->block[0].points[data->block[0].npoints2D]), 0);
  data->block[0].npoints2D += num;
  data->block[0].points[data->block[0].npoints2D].x = 0;
  data->block[0].points[data->block[0].npoints2D++].y = 0;
  num = INC;
  TD_to_screen (data->T, data->zoom, data->pan, &(data->xzplane[0]),
		&num, &(data->block[0].points[data->block[0].npoints2D]), 0);
  data->block[0].npoints2D += num;

  for (i = 1; i < data->nblocks; i++)
    {
      num = data->block[i].npoints3D;

      if (i == 1)
/*** XXX FIX ME XXX ***//*** Block 1 hard coded ***/
	if (data->ana_c)
	  {
	    num = min (data->block[i].npoints3D, data->ana_c + 2);
	  }

      TD_to_screen (data->T, data->zoom, data->pan,
		    &(data->block[i].vector[0]), &num,
		    &(data->block[i].points[0]), 0);
      data->block[i].npoints2D = num;

    }
  if (scale_change)
    scale_label (data);
/*******end Calculations ******/

  /*
   * Clear the window.
   */
  if (XtIsRealized (w))
    {
      XSetForeground (XtDisplay (w), data->gc,
		      BlackPixelOfScreen (XtScreen (w)));
      XClearArea (XtDisplay (w), XtWindow (w), 0, 0, 0, 0, TRUE);
      draw_to (w, data, XtWindow (w));
    }
  clear_pixmap (w, data);
  screenClear = 1;
  draw_to (w, data, data->pix);
/*  free(planes.points);
   free(vector.points); */
}
/******************************/
void
draw_to (Widget w, image_data_ptr data, Drawable draw)
{
  int i, num;
  XPoint axes_sc[6];
  Vector paxes[3] = IDEN_MAT;
  char names[1][3] =
  {
    {'X', 'Y', 'Z'}};

  num = 3;
  TD_to_screen (data->T, data->zoom, data->pan, &(paxes[0]), &num, axes_sc, 1);
  for (i = 0; i < 3; i++)
    {
      /*  XDrawLine (XtDisplay(w), draw, data->gc,
         (int) data->pan.x,(int) data->pan.y, axes_sc[i].x,axes_sc[i].y); */
      XDrawString (XtDisplay (w), draw, data->gc, axes_sc[i].x, axes_sc[i].y,
		   &(names[0][i]), 1);
    }
  XDrawString (XtDisplay (w), draw, data->gc,
	       1, 10, data->label, strlen (data->label));

  XDrawArc (XtDisplay (w), draw, data->gc, (int) data->pan.x - (int) data->zoom,
	    (int) data->pan.y - (int) data->zoom,
	    (int) (2. * data->zoom),
	    (int) (2. * data->zoom),
	    0, 360 * 64);

  for (i = 0; i < data->nblocks; i++)
    {
/*** Change color here. ****/
      XSetForeground (XtDisplay (w), data->gc,
		      data->block[i].color);

      draw_block_of_lines (w, data, draw,
			   data->block[i].points, data->block[i].npoints2D);
    }
}
/******************************/
void
scale_label (image_data_ptr data)
{
  static int nchars;
  static unsigned short min_dim;
  XPoint top_left, bottom_right;
  double vec1[3], vec2[3], vec3[3], ang, english;

  scale_change = 0;
  min_dim = min (data->height, data->width);

  if (min_dim == data->height)
    {
      sprintf (data->label, "From top to bottom:");
      nchars = strlen (data->label);
      top_left.x = min_dim / 2;
      top_left.y = 1;
      bottom_right.x = min_dim / 2;
      bottom_right.y = min_dim;
    }
  else
    {
      sprintf (data->label, "From left to right:");
      nchars = strlen (data->label);
      top_left.x = 1;
      top_left.y = min_dim / 2;
      bottom_right.x = min_dim;
      bottom_right.y = min_dim / 2;
    }
  screen_to_3D (top_left.x, top_left.y,
		data->zoom, data->pan, data->T, vec1);
  screen_to_3D (bottom_right.x, bottom_right.y,
		data->zoom, data->pan, data->T, vec2);
  norm_vec ((Vector *) vec1);
  norm_vec ((Vector *) vec2);
  ang = acos (vdot (vec1, vec2));
  if (ang < 1.0)
    {
      vcross (vec1, vec2, vec3);
      ang = asin (sqrt (vdot (vec3, vec3)));
    }
  sprintf (&(data->label[nchars]), " %e radians,", ang);
  nchars = strlen (data->label);
  english = ang * 180 / PI;
  if (english > 1.)
    {
      sprintf (&(data->label[nchars]), " %f degrees", english);
      nchars = strlen (data->label);
    }
  else
    {
      english *= 60.;
      if (english > 1.)
	sprintf (&(data->label[nchars]), " %f arc-minutes", english);
      else
	{
	  english *= 60.;
	  sprintf (&(data->label[nchars]), "%f arc-seconds", english);
	}
    }
}



/****************************************/
/********** Slider Section **************/
/****************************************/

/*******************************************/
Widget
make_slider (Widget parent, image_data_ptr data,
	     char *name, void *callback)
/*  void (*callback)(); */
{
  Widget w;
  int n;
  Arg wargs[10];
  XmString title;
  /*
   * Create an XmScale widget.
   */
  title = XmStringCreate ("Animation Speed", XmFONTLIST_DEFAULT_TAG);
/* *INDENT-OFF* */
  n = 0;
  XtSetArg (wargs[n], XmNminimum, 100); n++;
  XtSetArg (wargs[n], XmNmaximum, 300); n++;
  XtSetArg (wargs[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (wargs[n], XmNtitleString, title); n++;
  XtSetArg (wargs[n], XmNvalue, 200); n++;
/* *INDENT-ON* */


  w = XtCreateManagedWidget (name, xmScaleWidgetClass,
			     parent, wargs, n);
  /*
   * Add callbacks to be invoked when the slider moves.
   */
  XtAddCallback (w, XmNvalueChangedCallback,
		 (XtCallbackProc) callback, data);
  XtAddCallback (w, XmNdragCallback,
		 (XtCallbackProc) callback, data);

  return (w);
}

/**********************************/
void
slider_moved (Widget w, image_data_ptr data,
	      XmScaleCallbackStruct * call_data)
{
  int v;
/** Old LessTif Bug: value of scale not in call_data for callback **/
  /*  v =  ((XmScaleWidget) w)->scale.value; */
  v = call_data->value;
  data->ana_delay = (int) pow (10.0, (4.0 - ((double) v) / 100.0));
  if (data->ana_delay < 1)
    {
      printf ("Warning: Animation Delay was less than 1.\n");
      data->ana_delay = 1;
    }
}

/****************************************/
/******** Rubber Band Section ***********/
/****************************************/

/***************************************/
XtPointer
keybd_event (Widget w, image_data_ptr data, XEvent * event)
{
  printf ("here: %u \n", event->xkey.keycode);
  return NULL;
}
/***************************************/
XtPointer
start_rubber_band (Widget w, image_data_ptr data, XEvent * event)
{
  int i, j;

  while (bBusyAnimating)
    {
    }

  button_down = 1;
  data->last.x = data->start.x = event->xbutton.x;
  data->last.y = data->start.y = event->xbutton.y;

  if (event->xbutton.button == 1)
    {
      button_down_n = 1;
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++)
	  data->start_T[i][j] = data->T[i][j];
    }

  if (event->xbutton.button == 2)
    {
      button_down_n = 2;
      if (event->xbutton.state)
	full_size (w, data);
      else
	xor_mode (data, 1);
    }
  else
    button_down_n = 3;
  return NULL;
}
/***************************************/
XtPointer
track_button1_motion (Widget w, image_data_ptr data, XEvent * event)
{
  if (button_down)
    {
      /*
       * Update the endpoints.
       */
      data->last.x = event->xbutton.x;
      data->last.y = event->xbutton.y;

      while (bBusyAnimating)
	{
	}

      move_sphere (data);
      create_image (w, data);
    }
  return NULL;
}
/***************************************/
XtPointer
track_rubber_band (Widget w, image_data_ptr data, XEvent * event)
{
  if (button_down)
    {
      /*
       * Draw once to clear the previous line.
       */
      draw_rectangle (w, data);
      /*
       * Update the endpoints.
       */
      data->last.x = event->xbutton.x;
      data->last.y = event->xbutton.y;
      /*
       * Draw the new rectangle.
       */
      draw_rectangle (w, data);
    }
  return NULL;
}
/***************************************/
XtPointer
track_pan_motion (Widget w, image_data_ptr data, XEvent * event)
{
  if (button_down)
    {
      /*
       * Update the endpoints.
       */
      data->pan.x += (event->xbutton.x - data->last.x);
      data->pan.y += (event->xbutton.y - data->last.y);
      data->last.x = event->xbutton.x;
      data->last.y = event->xbutton.y;

      create_image (w, data);
    }
  return NULL;
}
/******************************************/
XtPointer
end_rubber_band (w, data, event)
     Widget w;
     image_data *data;
     XEvent *event;
{
  XGCValues values;
  int max_dis, screen_sq;
  double ratio;

  while (bBusyAnimating)
    {
    }
  button_down = 0;
  if (event->xbutton.button == 1)
    {
      data->last.x = event->xbutton.x;
      data->last.y = event->xbutton.y;
      move_sphere (data);
      resize (w, data, NULL);
    }
  if (event->xbutton.button == 2)
    {
      /*
       * Draw once to clear the previous rectangle.
       */
      draw_rectangle (w, data);
      /*
       * Return GC to Normal Mode.
       */
      values.function = GXcopy;
      values.line_style = LineSolid;
      XChangeGC (XtDisplay (w), data->gc,
		 GCFunction | GCLineStyle, &values);
      max_dis =
	max ((data->last.y - data->start.y),
	     (data->last.x - data->start.x));
      if (max_dis > 0)
/** down and to the right square **/
	{
	  screen_sq = min (data->width, data->height);
	  ratio = (double) screen_sq / (double) max_dis;
	  data->pan.x = (data->pan.x - data->start.x) * ratio;
	  data->pan.y = (data->pan.y - data->start.y) * ratio;
	  data->zoom *= ratio;
	  scale_change = 1;
	  create_image (data->drawing, data);
	}
    }
  return NULL;
}

/****************************************/
/********* Animation Section ************/
/****************************************/

/******************************************/
void
restart_anim (Widget w, image_data_ptr data, XmAnyCallbackStruct * call_data)
{
  create_image (data->drawing, data);
  if (data->block[1].npoints3D > 0)
/*** XXX FIX ME XXX ****/
    {
      if (data->ana_c > 0)
	XtRemoveTimeOut (data->iid);
      data->ana_c = 0;
      do_animation (data, NULL);
    }
}

/***************************************/
XtTimerCallbackProc
do_animation (image_data_ptr data, XtIntervalId * id)
{

  static int visable;
  static int last_visable;
  static XPoint vec;
  static XPoint last_vec;
  static char label[80];
  int delay;

  if (!(button_down && button_down_n == 2))
/** Can't Animate while zooming. **/
    {
      if (data->ana_c == 1)
/*** Erase pattern **/
	create_image (data->drawing, data);

      bBusyAnimating = 1;

      visable = 1;
      TD_to_screen (data->T, data->zoom, data->pan,
/*** XXX FIX ME XXX ***/
		    &(data->block[1].vector[data->ana_c]), &visable,
		    &vec, 0);
      if (vec.x == 0 && vec.y == 0)
	visable = 0;
      xor_mode (data, 0);



      if (XtIsRealized (data->drawing))
	{
	  if (!screenClear && !expose_flg)
	    {
/**** Clear Last Vector and String *********/
	      /*printf("Clearing....\n"); */
	      XDrawString (XtDisplay (data->drawing), XtWindow (data->drawing),
			   data->gc, 1, 20, label, strlen (label));
	      if (last_visable && show_vector)
		{
		  XDrawLine (XtDisplay (data->drawing), XtWindow (data->drawing),
			     data->gc, (int) data->pan.x, (int) data->pan.y,
			     last_vec.x, last_vec.y);
		}
	    }
	  /*printf("Drawing...\n"); */
	  sprintf (label, "Current sample number = %3ld", data->ana_c);
	  XDrawString (XtDisplay (data->drawing), XtWindow (data->drawing),
		       data->gc, 1, 20, label, strlen (label));
	  if (visable)
	    {
	      if (show_vector)
		XDrawLine (XtDisplay (data->drawing), XtWindow (data->drawing),
			   data->gc,
			   (int) data->pan.x, (int) data->pan.y,
			   vec.x, vec.y);


/*** Now for the trace... ***/
	      if (last_visable && !screenClear)
		{
		  copy_mode (data);
		  XSetForeground (XtDisplay (data->drawing), data->gc,
				  data->block[1].color);
		  XDrawLine (XtDisplay (data->drawing), XtWindow (data->drawing),
			     data->gc, last_vec.x, last_vec.y,
			     vec.x, vec.y);
		  XDrawLine (XtDisplay (data->drawing), data->pix,
			     data->gc, last_vec.x, last_vec.y,
			     vec.x, vec.y);
		  XSetForeground (XtDisplay (data->drawing), data->gc,
			     BlackPixelOfScreen (XtScreen (data->drawing)));
		}
	    }
	}
      expose_flg = 0;
      screenClear = 0;
      last_vec = vec;
      last_visable = visable;

      copy_mode (data);

    }
/** button 2 down end if **/

/*** Call myself in a while. ***/
  delay = data->ana_delay;

/** Increment counter. **/
  data->ana_c++;

  if (data->ana_c < data->block[1].npoints3D)
/** XXX FIX ME XXX ***/
    data->iid = XtAddTimeOut (delay,
			      (XtTimerCallbackProc) do_animation, data);

  bBusyAnimating = 0;
  return 0;
}

/****************************************/
/****************************************/
/****************************************/
/*
   ** get input
 */

FILE *
get_input (int argc, char *argv[], image_data * data)
{
  FILE *fp = NULL;

  long i = 0, j;
  int nfiles;
  int errflg = 0;
  int c;
  static unsigned long lines, words, chars;
  char options[16];

  strcpy (options, "nv");

  while (!errflg && (c = getopt (argc, argv, "nv")) != -1)
    switch (c)
      {
      case 'n':
	show_vector = 0;
	break;
      case 'v':
	show_vector = 1;
	break;
      default:
	printf ("Usage: vgp [-%s] [files...]\n", options);
	exit (1);
	errflg++;
      }
  nfiles = argc - optind;

  data->nblocks = 1;
/** Block 0 reserved for hemilines. ***/
  j = 0;
  data->block[j].vector = (Vector *) calloc ((size_t) 3 * (INC + 1), sizeof (Vector));
  data->block[j].points = (XPoint *) calloc ((size_t) 3 * (INC + 1), sizeof (XPoint));
  if (data->block[j].vector == NULL || data->block[j].points == NULL)
    {
      printf ("Could not allocate memory for even the hemisphere lines.\n");
      exit (1);
    }
  for (; optind < argc; ++optind)
    {
      j++;
      wc (argv[optind], &lines, &words, &chars);
      /*printf("File %s has %lu lines, %lu words and %lu chars.\n",
         argv[optind], lines, words, chars); */

      i = 0;
      if ((fp = fopen (argv[optind], "r")) == NULL)
	{
	  printf ("cannot open data file\n");
	  exit (1);
	}
      data->block[j].vector = (Vector *) calloc ((size_t) lines, sizeof (Vector));
      data->block[j].points = (XPoint *) calloc ((size_t) lines, sizeof (XPoint));
      if (data->block[j].vector == NULL || data->block[j].points == NULL)
	{
	  printf ("Could not allocate memory for file: %s\n", argv[optind]);
	  exit (1);
	}

      data->nblocks++;
      while (fscanf (fp, "%lf %lf %lf \n",
		     &(data->block[j].vector[i].x),
		     &(data->block[j].vector[i].y),
		     &(data->block[j].vector[i].z)
	     ) == 3 && i < lines)
	{
	  norm_vec (&(data->block[j].vector[i]));
	  i++;
	}
      printf ("  There were %ld rows of data read from file %s\n",
	      i, argv[optind]);
      data->block[j].npoints3D = i;
    }

  return fp;
}
