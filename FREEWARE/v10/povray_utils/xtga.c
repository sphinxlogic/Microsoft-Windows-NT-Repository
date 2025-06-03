/*
 *	xtga - a simple targa file viewer for X11
 *
 *	Frederick G. Kleinsorge
 *	(kleinsorge@star.enet.dec.com)
 *
 *	usage:
 *
 *		xtga filename [x11 options]
 *
 *	This program is a simple hack of the XWIND module from the
 *	POVRAY module in machine/vax.  It provides 24-bit truecolor
 *	output if the depth is 24, and uses the logic from xwind if
 *	the depth is not 24.  It does not do monochrome.
 *
 *	Note that this is quick and dirty, the following are little
 *	items that I should fix one day:
 *
 *		- This code assumes that a depth of 24-planes
 *		  means Truecolor.  It should check the visual
 *		  type.
 *
 *		- The code does not look at the RGB masks, and
 *		  assumes that RED is in the low byte, green is
 *		  the next byte, and BLUE is the next byte.  It
 *		  should get the RGB masks from the visual, and
 *		  use them to build the shift counts and masks.
 *
 *		- This code only uses the default visual, it does
 *		  not look to see if there is a 24-plane non-default
 *		  visual.
 *
 *		- The code makes a few assumptions about X11 image
 *		  padding and takes shortcuts.  It all seems to work.
 *
 *	For OpenVMS, the code maps the image file as a private section
 *	so that it has immediate access to the entire file in memory.
 *	That makes it fairly quick, since the data is simply paged in
 *	as needed.
 *
 *	For non-OpenVMS systems, I have included (and tested) file logic
 *	to read the data.  Of course, I've only compiled this on OpenVMS,
 *	so I have no real idea if it works on a Unix system.
 *
 *	To build on OpenVMS AXP:
 *
 *		cc/stand=vaxc/prefix=all/define=(VMS) xtga
 *		link xtga, sys$input/opt 
 *		name="xtga"
 *		ident="X001"
 *		SYS$SHARE:DECW$Xlibshr/SHAREABLE
 *		SYS$SHARE:DECW$Xmlibshr/SHAREABLE
 *		SYS$SHARE:DECW$XtlibshrR5/SHAREABLE
 *		^Z
 *
 *	Header from XWIND.C follows:
 *
 */

/****************************************************************************
*                xwind.c
*
*  This file contains the XWindows code for the display-as-you-trace feature.
*
*  from Persistence of Vision Raytracer
*  Copyright 1993 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's Graphics Developer's
*  Forum.  The latest version of POV-Ray may be found there as well.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

/******************************************************************************/
/*                                                                            */
/* X Windows code for POV-Ray.                                                */
/* Written by Christopher J. Cason.                                           */
/* CIS 100032,1644                                                            */
/* Internet 100032.1644@compuserve.com                                        */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Original IBM VGA "colour" output routines for MS/DOS by Aaron A. Collins.  */
/*                                                                            */
/* Converted for X Windows and arbitrary #of colours by Christopher J. Cason. */
/*                                                                            */
/* This will deliver approximate colorings using HSV values for the selection.*/
/* The palette map is divided into 4 parts - upper and lower half generated   */
/* with full and half "value" (intensity), respectively.  These halves are    */
/* further halved by full and half saturation values of each range (pastels). */
/* There are three constant colors, black, white, and grey.  They are used    */
/* when the saturation is low enough that the hue becomes undefined, and which*/
/* one is selected is based on a simple range map of "value".  Usage of the   */
/* palette is accomplished by converting the requested color RGB into an HSV  */
/* value.  If the saturation is too low (< .25) then black, white or grey is  */
/* selected.  If there is enough saturation to consider looking at the hue,   */
/* then the hue range of 1-63 is scaled into one of the 4 palette quadrants   */
/* based on its "value" and "saturation" characteristics.                     */
/*                                                                            */
/******************************************************************************/

#ifdef VMS
#include <rms.h>
#include <secdef.h>
#include <descrip.h>
#include <ssdef.h>
#include <ctype.h>
#endif

#include <stdio.h>
#include <unixio.h>
#include <math.h>
#include <x11/xlib.h>
#include <x11/xutil.h>
#include <x11/stringdefs.h>
#include <x11/intrinsic.h>
#include <x11/shell.h>

#define MIN_COLOURS     128
#define MAX_COLOURS     256

#define DEFAULT_FILE   "DATA.TGA"
#define FILENAME_MAX	256

#define min(x,y)        ((x) < (y) ? (x) : (y))
#define max(x,y)        ((x) > (y) ? (x) : (y))

unsigned                gWidth ;                      /* +wxxx                */
unsigned                gHeight ;                     /* +hyyy                */
unsigned                gScreenNumber ;               /* screen number        */
unsigned                nColours ;                    /* colours available    */
unsigned                quarterPalette ;              /* 1/4 of nColours      */
unsigned                CooperationLevel = 5 ;        /* for message loop     */
unsigned                screenWidth ;                 /* of root screen       */
unsigned                screenHeight ;                /* ditto                */
unsigned long           gColours [MAX_COLOURS] ;      /* colour lookup        */
int			gDepth ;                      /* default screen depth */
GC                      gGc ;                         /* graphics context     */
Screen                  *gScreen ;                    /* the X screen         */
XImage                  *gXimage = NULL ;             /* to buffer the trace  */
Widget                  gParent ;                     /* parent widget        */
Widget                  gPopupwindow ;                /* trace displayed here */
Window                  gWindow ;                     /* child of gPopupWin.. */
Display                 *gDisplay ;                   /* X display connection */
Colormap                gColourmap ;                  /* the colourmap        */
XtAppContext            gAppcontext ;                 /* application context  */

typedef struct fileinfo {

  int type;
  int width;
  int height;

  int start_addr;
  int end_addr;
  unsigned char *current_addr;

#ifdef VMS
  struct FAB fab;
#else
  FILE *image_file;
  int eof;
#endif

  int filename_valid;
  int file_mapped;
  int file_open;
  unsigned char filename[FILENAME_MAX];

  } FILEINFO;


XImage *CreateXImage (Display *display, Visual *visual,
                      unsigned depth, unsigned width, unsigned height)
{
  unsigned    format ;
  unsigned    number_of_bytes ;
  XImage      *ximage ;

  format = depth == 1 ? XYBitmap : ZPixmap ;

  ximage = XCreateImage (display, visual, depth, format,
                         0, NULL, width, height, XBitmapPad (display), 0) ;

  if (ximage == NULL) return (NULL) ;

  number_of_bytes = ximage->bytes_per_line * ximage->height ;
  if ((ximage->data = malloc (number_of_bytes)) == NULL) return (NULL) ;
  return (ximage) ;
}

XImage *CreateXImageFromWidget (Widget widget, unsigned width, unsigned height)
{
  unsigned    depth ;
  XImage      *ximage ;
  Visual      *visual ;
  Display     *display ;

  XtVaGetValues (widget, XtNdepth, &depth, XtNvisual, &visual, NULL) ;
  display = XtDisplay (widget) ;
  ximage = CreateXImage (display, visual, depth, width, height) ;
  return (ximage) ;
}

XImage *MakeImage (Widget widget, unsigned width, unsigned height)
{
  unsigned              x ;
  unsigned              y ;
  XImage                *ximage ;

  if ((ximage = CreateXImageFromWidget (widget, width, height)) != NULL)
  {
    for (x = 0 ; x < width ; x++)
      for (y = 0 ; y < height ; y++)
        XPutPixel (ximage, x, y, gColours [0]) ;
  }

  return (ximage) ;
}

void processExpose (XExposeEvent *expose)
{
  if (gXimage == NULL || (void *) gWindow == NULL) return ;
  XPutImage (gDisplay, gWindow, gGc, gXimage,
             expose->x, expose->y,
             expose->x, expose->y,
             expose->width, expose->height) ;
}

void eventHandler (void)
{
  XEvent                event ;
  static unsigned       cooperationCounter = 10 ;

  if (CooperationLevel < 10 && --cooperationCounter) return ;
  cooperationCounter = 11 - CooperationLevel ;

  if (CooperationLevel > 5)
  {
    /* loop until all messages retrieved */
    while (XtAppPending (gAppcontext))
    {
      XtAppNextEvent (gAppcontext, &event) ;

      if (event.type == Expose && event.xexpose.window == gWindow)
      {
        processExpose (&event.xexpose) ;
        return ;
      }
      XtDispatchEvent (&event) ;
    }
  }
  else
  {
    /* only process 1 event */
    if (XtAppPending (gAppcontext))
    {
      XtAppNextEvent (gAppcontext, &event) ;
      if (event.type == Expose && event.xexpose.window == gWindow)
      {
        processExpose (&event.xexpose) ;
        return ;
      }
      XtDispatchEvent (&event) ;
    }
  }
}

void destroyProc (Widget widget)
{
  XDestroyImage (gXimage) ;
  XFreeGC (gDisplay, gGc) ;
  gPopupwindow = NULL ;
}

Widget CreatePopupWindow (Widget parent, char *name, unsigned width, unsigned height, void (*destroyProc) ())
{
  int         n ;
  unsigned    x = 0 ;
  unsigned    y = 0 ;
  Arg         args [8] ;
  Widget      window ;

  n = 0 ;
  if (width < screenWidth)
    x = screenWidth / 2 - width / 2 ;
  if (height < screenHeight)
    y = screenHeight / 2 - height / 2 ;
  XtSetArg (args [n], XtNwidth, width) ; n++ ;
  XtSetArg (args [n], XtNheight, height) ; n++ ;
  XtSetArg (args [n], XtNx, x) ; n++ ;
  XtSetArg (args [n], XtNy, y) ; n++ ;
  window = XtCreatePopupShell (name, topLevelShellWidgetClass, parent, args, n) ;
  XtAddCallback (window, XtNdestroyCallback, destroyProc, window) ;
  return (window) ;
}

void set_palette (unsigned index, unsigned red, unsigned green, unsigned blue)
{
  XColor      colour ;

  colour.pixel = gColours [index] ;
  colour.flags = DoRed | DoGreen | DoBlue ;
  colour.red = (65535 * red) / 256 ;
  colour.blue = (65535 * blue) / 256 ;
  colour.green = (65535 * green) / 256 ;
  XStoreColor (gDisplay, gColourmap, &colour) ;
}

void hsv_to_rgb (double hue, double s, double v, unsigned *r, unsigned *g, unsigned *b)
{
  double         i ;
  double         f ;
  double         p1 ;
  double         p2 ;
  double         p3 ;
  double         xh ;
  double         nr ;
  double         ng ;
  double         nb ;

  if (hue == 360.0) hue = 0.0 ;

  xh = hue / 60.0 ;               /* convert hue to be in 0..6 */
  i = floor (xh) ;                /* i = greatest integer <= h */
  f = xh - i ;                    /* f = fractional part of h  */
  p1 = v * (1 - s) ;
  p2 = v * (1 - (s * f)) ;
  p3 = v * (1 - (s * (1 - f))) ;

  switch ((int) i)
  {
    case 0 :
         nr = v ;
         ng = p3 ;
         nb = p1 ;
         break ;

    case 1 :
         nr = p2 ;
         ng = v ;
         nb = p1 ;
         break ;

    case 2 :
         nr = p1 ;
         ng = v ;
         nb = p3 ;
         break ;

    case 3 :
         nr = p1 ;
         ng = p2 ;
         nb = v ;
         break ;

    case 4 :
         nr = p3 ;
         ng = p1 ;
         nb = v ;
         break ;

    case 5 :
         nr = v ;
         ng = p1 ;
         nb = p2 ;
         break ;

    default :
         nr = ng = nb = 0 ;
  }
  *r = (unsigned) (nr * 255.0) ;
  *g = (unsigned) (ng * 255.0) ;
  *b = (unsigned) (nb * 255.0) ;
}

void rgb_to_hsv (unsigned r, unsigned g, unsigned b, double *h, double *s, double *v)
{
  double         m ;
  double         r1 ;
  double         g1 ;
  double         b1 ;
  double         nr ;
  double         ng ;
  double         nb ;
  double         nh = 0.0 ;
  double         ns = 0.0 ;
  double         nv ;

  nr = (double) r / 255.0 ;
  ng = (double) g / 255.0 ;
  nb = (double) b / 255.0 ;

  nv = max (nr, max (ng, nb)) ;
  m = min (nr, min (ng, nb)) ;

  if (nv != 0.0) ns = (nv - m) / nv ;

  if (ns == 0.0)
  {
    /* hue undefined if no saturation */
    *h = 0.0 ;
    *s = 0.0 ;
    *v = nv ;
    return ;
  }

  r1 = (nv - nr) / (nv - m) ;     /* distance of color from red   */
  g1 = (nv - ng) / (nv - m) ;     /* distance of color from green */
  b1 = (nv - nb) / (nv - m) ;     /* distance of color from blue  */

  if (nv == nr)
  {
    if (m == ng)
      nh = 5.0 + b1 ;
    else
      nh = 1.0 - g1 ;
  }

  if (nv == ng)
  {
    if (m == nb)
      nh = 1.0 + r1 ;
    else
      nh = 3.0 - b1 ;
  }

  if (nv == nb)
  {
    if (m == nr)
      nh = 3.0 + g1 ;
    else
      nh = 5.0 - r1 ;
  }

  *h = nh * 60.0 ;                /* return h converted to degrees */
  *s = ns ;
  *v = nv ;
}

void palette_init (void)
{
  unsigned    m ;
  unsigned    r ;
  unsigned    g ;
  unsigned    b ;
  double         hue ;
  double         sat ;
  double         val ;

  for (nColours = MAX_COLOURS ; nColours >= MIN_COLOURS ; nColours--)
    if (XAllocColorCells (gDisplay, gColourmap, False, NULL, 0, gColours, nColours))
      break ;
  if (nColours < MIN_COLOURS)
  {
    printf ("failed to allocate colour cells needed for display\r\n") ;
    exit (1) ;
  }

  quarterPalette = nColours / 4 ;

  /* for the first quarter of the palette ... */
  for (m = 1 ; m < quarterPalette ; m++)
  {
    /* normalise to 360 */
    hue = 360.0 * ((double) (m)) / (double) quarterPalette ;
    hsv_to_rgb (hue, 0.5, 0.5, &r, &g, &b) ;
    set_palette (m, r, g, b) ;

    hue = 360.0 * ((double) (m)) / (double) quarterPalette ;
    hsv_to_rgb (hue, 1.0, 0.5, &r, &g, &b) ;
    set_palette (m + quarterPalette, r, g, b) ;

    hue = 360.0 * ((double) (m)) / (double) quarterPalette ;
    hsv_to_rgb (hue, 0.5, 1.0, &r, &g, &b) ;
    set_palette (m + quarterPalette * 2, r, g, b) ;

    hue = 360.0 * ((double) (m)) / (double) quarterPalette ;
    hsv_to_rgb (hue, 1.0, 1.0, &r, &g, &b) ;
    set_palette (m + quarterPalette * 3, r, g, b) ;
  }

  set_palette (0, 0, 0, 0) ;                        /* black        */
  set_palette (quarterPalette, 255, 255, 255) ;     /* white        */
  set_palette (quarterPalette * 2, 128, 128, 128) ; /* dark grey    */
  set_palette (quarterPalette * 3, 192, 192, 192) ; /* light grey   */
}

void display_init (FILEINFO *fin, unsigned width, unsigned height)
{
  unsigned              n ;
  unsigned              y = 0 ;
  unsigned              x = 0 ;
  unsigned long         white ;
  unsigned long         black ;
  Arg                   args [20] ;

  gDisplay = XtDisplay (gParent) ;
  gScreen = XtScreen (gParent) ;
  gScreenNumber = XScreenNumberOfScreen (gScreen) ;
  gColourmap = DefaultColormapOfScreen (gScreen) ;
  gDepth = DefaultDepthOfScreen (gScreen) ;
  screenWidth = DisplayWidth (gDisplay, gScreenNumber) ;
  screenHeight = DisplayHeight (gDisplay, gScreenNumber) ;

  white = WhitePixel (gDisplay, gScreenNumber) ;
  black = BlackPixel (gDisplay, gScreenNumber) ;

  n = 0 ;
  x = screenWidth / 2 - gWidth / 2 ;
  y = screenHeight / 2 - gHeight / 2 ;
  XtSetArg (args [n], XtNx, x) ; n++ ;
  XtSetArg (args [n], XtNy, y) ; n++ ;
  XtSetValues (gParent, args, n) ;

  gWidth = width ;
  gHeight = height ;
  gPopupwindow = CreatePopupWindow (gParent, fin->filename, width, height, destroyProc) ;

  XtRealizeWidget (gPopupwindow) ;
  XtMapWidget (gPopupwindow) ;

  gWindow = XCreateSimpleWindow (gDisplay, XtWindow (gPopupwindow), 0, 0, width, height, 0, white, black) ;
  XSetWindowBackground (gDisplay, gWindow, black) ;
  XMapWindow (gDisplay, gWindow) ;
  XtPopup (gPopupwindow, XtGrabNonexclusive) ;

  if (gDepth != 24)
    palette_init () ;

  gGc = XCreateGC (gDisplay, gWindow, 0L, NULL) ;
  gXimage = MakeImage (gPopupwindow, width, height) ;

  XSelectInput (gDisplay, gWindow, ExposureMask) ;
}

void display_close ()
{
  if (gPopupwindow)
    XtDestroyWidget (gPopupwindow) ;
}

int
close_image_file(image)

FILEINFO *image;

{
  int status = 0;

  if (image == 0) return FALSE;

#ifdef VMS

  if (image->file_mapped)
    {
      status = sys$deltva(&image->start_addr, 0);
      if (!status & 1)
        {
	  printf("Error Unmapping section file %d\n", status);
        }      
      sys$close( &image->fab);
    }

  image->file_open = 0;
  image->file_mapped = 0;
  image->start_addr = 0;
  image->end_addr   = 0;
  image->current_addr = 0;
  image->type = 0;

#else
#endif

  return (TRUE);
}


#ifndef VMS
int
get_byte(image)

FILEINFO *image;

  {
    int data;

    if (image->eof) return FALSE;
    else
      {
        data = getc(image->image_file);
	if (data == EOF)
	  {
	    image->eof = 1;
	    return FALSE;
	  }
	return (data & 0xFF);
      }
  }
#endif

int open_image_file(image)

FILEINFO *image;

{
  int i;

#ifdef VMS

  int status;
  int addr_base[2], use_P0[2] = {0x200, 0x200};  /* Use a P0 address */
  unsigned char *pbuf;

 /*
  *  Open input file.
  *
  */
  image->fab = cc$rms_fab;
  image->fab.fab$l_fop = FAB$M_UFO;
  image->fab.fab$b_fac = FAB$M_GET;
  image->fab.fab$b_rtv = 0xFF;
  image->fab.fab$l_fna = &image->filename[0];
  image->fab.fab$b_fns = strlen(&image->filename[0]);
  image->fab.fab$l_dna = DEFAULT_FILE;
  image->fab.fab$b_dns = sizeof DEFAULT_FILE - 1;

  status = sys$open( &image->fab);

  if (status != RMS$_NORMAL)
    {
      lib$signal( status);
      return (FALSE);
    }

  image->file_open = 1;

 /*
  *  Map the input file as a private section in P0 space, let VMS figure out
  *  where.
  *
  */
  status = sys$crmpsc( &use_P0,                     /* inadr */
                       &addr_base,                  /* retadr */
                       0,                           /* acmode */
                       SEC$M_EXPREG | SEC$M_CRF,    /* flags */
                       0,                           /* gsdnam */
                       0,                           /* ident */
                       0,                           /* relpag */
                       image->fab.fab$l_stv,          /* chan */
                       0,                           /* pagcnt */
                       0,                           /* vbn */
                       0xEEEE,                      /* prot */
                       0);                          /* pfc */

  if (!(status & 1))
    {
      lib$signal( status);

      close_image_file(image);

      return (FALSE);
    }

  image->file_mapped = 1;

  image->start_addr     = addr_base[0];
  image->end_addr       = addr_base[1];

  pbuf = addr_base[0] + 2;

 /*
  *  Get file type.  Byte 3 is the type
  *
  */
  image->type = *pbuf++;

  pbuf += 9;

 /*
  *  Get x and Y image resolution
  *
  */
  image->width  = *pbuf++ & 0xFF;
  image->width |= ((*pbuf++ & 0xFF) << 8) & 0xFFFF;

  image->height  = *pbuf++ & 0xFF;
  image->height |= ((*pbuf++ & 0xFF) << 8) & 0xFFFF;

 /*
  *  Last 2 bytes of header
  *
  */
  pbuf += 2;

 /*
  *  Point to first byte of image
  *
  */
  image->current_addr = pbuf;

#else

  image->image_file = fopen(&image->filename[0], "rb");
  image->eof = 0;

  if (!(image->image_file))
    {
      printf("Error opening file %s as input\n", &image->filename[0]);
      return (FALSE);
    }

 /*
  *  Skip two bytes
  *
  */
  get_byte(image);
  get_byte(image);

 /*
  *  Get type
  *
  */
  image->type = get_byte(image);

 /*
  *  Skip the next bytes
  *
  */
  for (i = 3; i < 12; i += 1)
    {
      get_byte(image);
    }

 /*
  *  Get x and Y image resolution
  *
  */
  image->width  = get_byte(image) & 0xFF;
  image->width |= ((get_byte(image) & 0xFF) << 8) & 0xFFFF;

  image->height  = get_byte(image) & 0xFF;
  image->height |= ((get_byte(image) & 0xFF) << 8) & 0xFFFF;

 /*
  *  Skip two bytes
  *
  */
  get_byte(image);
  get_byte(image);

#endif

  if (image->type != 2)
    {
      printf("%s is not a type 2 targa file, found %d at byte 3\n", &image->filename[0], image->type);
      close_image_file(image);
      return 0;
    }

  if ((image->width == 0) || (image->height == 0))
    {
      printf("%s has a bad image size.  Found width = %d, height = %d\n", &image->filename[0], image->width, image->height);
      close_image_file(image);
      return 0;
    }

  return (TRUE);
}

int display_image(image)

FILEINFO *image;

{
  int x, y;
  unsigned int pixel, red, green, blue, colour;
  double         h ;
  double         s ;
  double         v ;

  if (gPopupwindow == NULL) return ;

  if (gDepth == 24)
    {
      unsigned int *pImage;
      pImage = (unsigned int *) gXimage->data;

      for (y = 0; y < image->height; y += 1)
        {
          for (x = 0; x < image->width; x += 1)
            {
#ifdef VMS
              pixel  = *image->current_addr++ << 16;
              pixel |= *image->current_addr++ << 8;
              pixel |= *image->current_addr++;
#else
              pixel  = get_byte(image) << 16;
              pixel |= get_byte(image) << 8;
              pixel |= get_byte(image);
#endif
              *pImage++ = pixel;
            }
          XPutImage (gDisplay, gWindow, gGc, gXimage, 0, y, 0, y, gWidth, 1);
          eventHandler();
        }
    }
  else
    {
      unsigned char *pImage;

      for (y = 0; y < image->height; y += 1)
        {
          pImage = gXimage->data + (gXimage->bytes_per_line * y);

          for (x = 0; x < image->width; x += 1)
            {
#ifdef VMS
              blue  = *image->current_addr++;
              green = *image->current_addr++;
              red   = *image->current_addr++;
#else
              blue  = get_byte(image);
              green = get_byte(image);
              red   = get_byte(image);
#endif
              rgb_to_hsv (red, green, blue, &h, &s, &v) ;

              if (s < 0.20)
                {
                  if (v < 0.25) colour = 0;
                  else if (v > 0.8) colour = quarterPalette;
		  else if (v > 0.5) colour = quarterPalette * 3;
                  else colour = quarterPalette * 2;
		}
	      else
		{
		  colour = (unsigned) ((double) quarterPalette * h / 360.0);

		  if (colour == 0) colour = 1; /* avoid black, white or grey */
		  if (colour > quarterPalette) colour = quarterPalette;

		  if (v > 0.50) colour += quarterPalette * 2; /* colours 128-255 for high intensity */

		  if (s > 0.50)                    /* more than half saturated ?         */
		  colour += quarterPalette ;       /* colour range 64-128 or 192-255     */

		}
	      *pImage++ = gColours[colour];
            }
          XPutImage (gDisplay, gWindow, gGc, gXimage, 0, y, 0, y, gWidth, 1);
          eventHandler(); /* Handle any exposures */
        }
    }
}

int main (int argc, char *argv [])
{
  unsigned              n;
  int                   count = 0, status ;
  Arg                   args [20] ;
  FILEINFO 		image;

  strncpy (image.filename, argv [1], FILENAME_MAX) ;

  n = 0 ;
  XtSetArg (args [n], XtNmappedWhenManaged, False) ; n++ ;
  XtSetArg (args [n], XtNallowShellResize, False) ; n++ ;
  XtSetArg (args [n], XtNwidth, 10) ; n++ ;
  XtSetArg (args [n], XtNheight, 10) ; n++ ;

  gParent = XtAppInitialize (&gAppcontext, image.filename, NULL, 0, &argc, argv, NULL, args, n) ;

  if (!open_image_file(&image)) return 0;

  display_init (&image, image.width, image.height);
  display_image (&image);

  close_image_file(&image);

 /*
  *  Just spin handling exposure events
  *
  */
  while (1) eventHandler();

  return 0;

}
