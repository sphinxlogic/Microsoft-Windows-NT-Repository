/*
   X-BASED VECTOR GLOBE PROJECTION

   vgp.h

   Copyright (c) 1994, 1995 Frederick (Rick) A Niles,
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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Intrinsic.h>
#include <Xm/Scale.h>
#include "algebra.h"

#ifndef PI
#define PI            3.14159265359
#endif
#define ONE_DEGREE    (180.0/PI)
#define INC           50
#define GLOBEPTS      200
#define IDEN_MAT      {{1,0,0},{0,1,0},{0,0,1}}
#define MAXSIZE       10000
#define INIT_SPEED    100
#define MAX_SHORT     32768
#define MAX_SCREEN     2000
#define MAX_SETS     50

/*** Data set types ****/
#define HEMILINES  0
#define BACKGROUND 1
#define TIMEDATA   2

/*** Macros *****/
#define max(A,B) ((A) > (B) ? (A) : (B))
#define min(A,B) ((A) < (B) ? (A) : (B))

typedef struct
  {
    double x;
    double y;
  }
DPoint;

/*
 * Assorted information needed to generate and draw the image.
 */
typedef struct
  {
    Vector *vector;
    XPoint *points;
    int npoints3D;
    int npoints2D;
    int color;
    int type;
  }
point_block, *point_block_ptr;


typedef struct
  {
    int depth, ncolors;
    GC gc;
    Pixmap pix;
    Dimension width, height;
    Vector xyplane[INC], xzplane[INC], yzplane[INC];
    double rotation[3];
    double T[3][3], start_T[3][3];
    point_block block[MAX_SETS];
    int nblocks;
    double zoom;
    DPoint pan;
    XPoint start, last;
    long ana_c;
    Widget drawing;
    int ana_delay;
    XtIntervalId iid;
    char label[80];
  }
image_data, *image_data_ptr;
/*  long          vec_c; */

/******************************************/
/******* Function Declarations ************/
/******************************************/
/* *INDENT-OFF* */
/******* vgp.c ***************************/
int      main        (int, char *[]);
void     init_data   (Widget, image_data_ptr);
int      clean_up    (Widget, image_data_ptr, caddr_t);
void     resize      (Widget, image_data_ptr, caddr_t);
void     full_size   (Widget, image_data_ptr);
void     create_image(Widget, image_data_ptr);
void     draw_to     (Widget, image_data_ptr, Drawable);
void     scale_label (image_data_ptr);
Widget   make_slider (Widget, image_data_ptr, char *, void *);
void     slider_moved(Widget, image_data_ptr, XmScaleCallbackStruct *);

XtPointer keybd_event         (Widget, image_data_ptr, XEvent *);
XtPointer start_rubber_band   (Widget, image_data_ptr, XEvent *);
XtPointer track_button1_motion(Widget, image_data_ptr, XEvent *);
XtPointer track_rubber_band   (Widget, image_data_ptr, XEvent *);
XtPointer track_pan_motion    (Widget, image_data_ptr, XEvent *);
XtPointer end_rubber_band     (Widget, image_data_ptr, XEvent *);
/* *INDENT-ON* */



void restart_anim (Widget, image_data_ptr, XmAnyCallbackStruct *);
XtTimerCallbackProc do_animation (image_data_ptr, XtIntervalId *);

FILE *get_input (int, char *[], image_data_ptr);
int wc (char *, unsigned long *, unsigned long *, unsigned long *);

/******* rotation.c *********************/
void create3DSphere (image_data_ptr);
void create_rot_matrix (image_data_ptr);
void screen_to_3D (int, int, double, DPoint, double[3][3], double[3]);
void TD_to_screen (double[3][3], double, DPoint, Vector *,
		   int *, XPoint[], int);
void move_sphere (image_data_ptr);


/******* Xstuff.c ********************/
void set_window_size (Widget, image_data_ptr);
void xor_mode (image_data_ptr, int);
void copy_mode (image_data_ptr);
void clear_pixmap (Widget, image_data_ptr);
void draw_block_of_lines (Widget, image_data_ptr, Drawable,
			  XPoint[], short);
void redisplay (Widget, image_data_ptr,
		XmDrawingAreaCallbackStruct *);
void draw_rectangle (Widget, image_data_ptr);
int get_pixel_by_name (Widget, char *);
