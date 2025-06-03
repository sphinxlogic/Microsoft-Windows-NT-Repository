
/*
 * xanim_x11.c
 *
 * Copyright (C) 1990,1991,1992 by Mark Podlipec. 
 * All rights reserved.
 *
 * This software may be freely copied, modified and redistributed
 * without fee provided that this copyright notice is preserved 
 * intact on all copies and modified copies.
 * 
 * There is no warranty or other guarantee of fitness of this software.
 * It is provided solely "as is". The author(s) disclaim(s) all
 * responsibility and liability with respect to this software's usage
 * or its effect upon hardware or computer systems.
 *
 */

#include "xanim.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <sys/signal.h>
#include <sys/times.h>
#include <ctype.h>
#include "xanim_x11.h"
 
void xanim_expose();
void xanim_key();
/*
void xanim_quit();
*/
void xanim_events();
void X11Setup();

/*********************************** X11 stuff */
Display       *theDisp;
Visual        *theVisual;
Colormap       theCmap;
Window         mainW;
GC             theGC;
XImage        *theImage;
XColor         defs[256];
/******************************** Xt stuff */
XtAppContext  theContext;

#define ACTIONTABLE_SIZE 2
XtActionsRec actionTable[] = {
        {"Expose", xanim_expose},
        {"KeyUp", xanim_key}
};

static struct _resource {
  int anim;
} resource;

#define offset(field)   XtOffset(struct _resource *, field)

XtResource application_resources[] = {
  {"anim", "Anim", XtRBoolean, sizeof (Boolean),
     offset(anim), XtRString, "False" },
};

String   Translation =
"<Expose>:Expose()\n\
<KeyUp>:KeyUp()";

X11_Get_Shift(mask,shift,size)
ULONG mask,*shift,*size;
{
  LONG i,j;

  i=0;
  while( (i < 32) && !(mask & 0x01) ) 
  {
    mask >>= 1;
    i++;
  }
  if (i >= 32)
  {
    fprintf(stderr,"X11_Get_Shift: wierd mask value %lx\n",mask);
    i = 0;
  }
  *shift = i;
  j=0;
  while( (i < 32) && (mask & 0x01) ) 
  {
    mask >>= 1;
    i++;
    j++;
  }
  *size = j;
}

ULONG X11_Get_True_Color(r,g,b,size)
register ULONG r,g,b,size;
{
  register ULONG temp,temp_color;

  temp = (x11_red_size >= size)?(r << (x11_red_size - size))
                               :(r >> (size - x11_red_size));
  temp_color  = (temp << x11_red_shift) & x11_red_mask;

  temp = (x11_green_size >= size)?(g << (x11_green_size - size))
                                 :(g >> (size - x11_green_size));
  temp_color |= (temp << x11_green_shift) & x11_green_mask;

  temp = (x11_blue_size >= size)?(b << (x11_blue_size - size))
                                :(b >> (size - x11_blue_size));
  temp_color |= (temp << x11_blue_shift) & x11_blue_mask;

  return(temp_color);
}

void X11_Pre_Setup(argc, argv)
int argc;
char *argv[];
{
  int i,vis_num;
  XVisualInfo *vis;
 
  XtToolkitInitialize();
  theContext = XtCreateApplicationContext();
  XtAppAddActions(theContext, actionTable, ACTIONTABLE_SIZE);

  i = 0;
  theDisp = XtOpenDisplay(theContext, NULL, "xanim", "XAnim",NULL,0,&argc,argv);
  if (theDisp==NULL) 
  {
    TheEnd1("Unable to open display\n"); 
  }

  if (x11_verbose_flag == TRUE)
  {
    fprintf(stderr,"Looking Through Supported Visuals:\n");
    vis = XGetVisualInfo (theDisp, VisualNoMask, NULL, &vis_num);
    if ((vis == NULL) || (vis_num == 0) )
    {
      TheEnd1("X11: Couldn't get any Visuals\n");
    }
    else
    {
      for(i=0;i<vis_num;i++)
      {
        fprintf(stderr,"  visual %ld) depth= %ld  class= %ld  cmap size=%ld  ",
                         i, vis[i].depth, vis[i].class, vis[i].colormap_size );
        X11_OutPut_Visual_Class(vis[i].class);
        fprintf(stderr,"\n");
      }
      fprintf(stderr,"\n");
    }
    XFree(vis);  
  }

  theGC     = DefaultGC(theDisp,DefaultScreen(theDisp));
  x11_depth = DefaultDepth(theDisp, DefaultScreen(theDisp));

  theVisual = DefaultVisual(theDisp, DefaultScreen(theDisp));
  x11_class = theVisual->class;
  x11_bit_order   = BitmapBitOrder(theDisp);
  if (x11_bit_order == MSBFirst) x11_bit_order = X11_MSB;
  else x11_bit_order = X11_LSB;
  x11_bitmap_unit = BitmapUnit(theDisp);
  x11_cmap_size   = DisplayCells(theDisp, DefaultScreen(theDisp));

  if ( (x11_class == PseudoColor) && (x11_depth <=8) && (x11_depth >=1) )
  {
    x11_bytes_pixel = 1;
    x11_cmap_flag = TRUE;
    x11_display_type = PSEUDO_COLOR;
    if (x11_verbose_flag == TRUE)
    {
      fprintf(stderr,"PSEUDO_COLOR    ");
      X11_OutPut_Visual_Class(x11_class); fprintf(stderr,"\n");
      fprintf(stderr,"  cmap_size = %ld depth=%ld bytes_pixel=%ld\n",
		x11_cmap_size,x11_depth,x11_bytes_pixel);
      fprintf(stderr,"  Bit Order = %lx  bitmapunit = %lx bitmappad = %lx\n",
		x11_bit_order,x11_bitmap_unit,BitmapPad(theDisp) );
    }
  }
  else 
  if (x11_class == TrueColor) 
  {
    if (x11_depth > 16) x11_bytes_pixel = 4;
    else if (x11_depth > 8) x11_bytes_pixel = 2;
    else x11_bytes_pixel = 1;
    x11_cmap_flag = FALSE;
    x11_display_type = TRUE_COLOR;
    x11_red_mask = theVisual->red_mask;
    x11_green_mask = theVisual->green_mask;
    x11_blue_mask = theVisual->blue_mask;
    X11_Get_Shift(x11_red_mask  , &x11_red_shift  , &x11_red_size  );
    X11_Get_Shift(x11_green_mask, &x11_green_shift, &x11_green_size);
    X11_Get_Shift(x11_blue_mask , &x11_blue_shift , &x11_blue_size );

    if (x11_verbose_flag == TRUE)
    {
      fprintf(stderr,"TRUE_COLOR    ");
      X11_OutPut_Visual_Class(x11_class); fprintf(stderr,"\n");
      fprintf(stderr,"  cmap_size = %ld depth=%ld bytes_pixel=%ld\n",
		  x11_cmap_size,x11_depth,x11_bytes_pixel);
      fprintf(stderr,"  Bit Order = %lx  bitmapunit = %lx bitmappad = %lx\n",
		x11_bit_order,x11_bitmap_unit,BitmapPad(theDisp) );
      fprintf(stderr,"  X11 Color Masks =%lx %lx %lx\n",
		  x11_red_mask,x11_green_mask ,x11_blue_mask);
      fprintf(stderr,"  X11 Color Shifts=%ld %ld %ld\n",
		  x11_red_shift, x11_green_shift, x11_blue_shift );
      fprintf(stderr,"  X11 Color Sizes =%ld %ld %ld\n",
		  x11_red_size,x11_green_size ,x11_blue_size);
    }
  }
  else
  if (x11_depth == 1)
  {
    x11_display_type = MONOCHROME;
    x11_bytes_pixel = 1;
    x11_cmap_flag = FALSE;
    x11_black = BlackPixel(theDisp,DefaultScreen(theDisp));
    x11_white = WhitePixel(theDisp,DefaultScreen(theDisp));
    if (x11_verbose_flag == TRUE)
    {
      fprintf(stderr,"MONOCHROME    ");
      X11_OutPut_Visual_Class(x11_class); fprintf(stderr,"\n");
      fprintf(stderr,"  cmap_size = %ld depth=%ld bytes_pixel=%ld\n",
		x11_cmap_size,x11_depth,x11_bytes_pixel);
      fprintf(stderr,"  Bit Order = %lx  bitmapunit = %lx bitmappad = %lx\n",
		x11_bit_order,x11_bitmap_unit,BitmapPad(theDisp) );
    }
  }
  else /* not yet supported */
  {
    fprintf(stderr,"Unsupported Visual class=%ld depth=%ld \n",
    x11_class,x11_depth);
    X11_OutPut_Visual_Class(x11_class); fprintf(stderr,"\n");
    TheEnd();
  }


  if (x11_cmap_flag == TRUE)
  {
    cmap = (ColorReg *) malloc( x11_cmap_size * sizeof(ColorReg) );
    if (cmap==0) fprintf(stderr,"X11 CMAP: couldn't malloc\n");

    for(i=0;i<x11_cmap_size;i++) 
    {
      defs[i].pixel = i;
      defs[i].flags = DoRed | DoGreen | DoBlue;
    }
 
    XQueryColors(theDisp,DefaultColormap(theDisp, DefaultScreen(theDisp)),
                 defs,x11_cmap_size);

    for(i=0; i<x11_cmap_size; i++)
    {
      cmap[i].red   = (defs[i].red   >> 8) & 0xff;
      cmap[i].green = (defs[i].green >> 8) & 0xff;
      cmap[i].blue  = (defs[i].blue  >> 8) & 0xff;
      if (x11_display_type == TRUE_COLOR)
        cmap[i].map = X11_Get_True_Color(
              cmap[i].red, cmap[i].green, cmap[i].blue, 8);
      else cmap[i].map   = i;
    }
  }
}

/*
 * Setup X11 Display, Window and Toolkit
 */
void X11_Setup(max_imagex,max_imagey,startx,starty)
int max_imagex,max_imagey;
int startx,starty;
{
  int i;
  Widget wg;
  int n;
  Arg arglist[20];
  XWMHints xwm_hints;

  n = 0;
  XtSetArg(arglist[n], XtNwidth, startx); n++;
  XtSetArg(arglist[n], XtNheight, starty); n++;
  XtSetArg(arglist[n], XtNmaxWidth, max_imagex); n++;
  XtSetArg(arglist[n], XtNmaxHeight, max_imagey); n++;
  wg = XtAppCreateShell("xanim", "XAnim", applicationShellWidgetClass, 
                         theDisp, arglist, n);

  XtGetApplicationResources (wg, &resource, application_resources,
                             XtNumber(application_resources), NULL, 0);

  XtRealizeWidget(wg);
  XtOverrideTranslations(wg, XtParseTranslationTable(Translation));

  mainW = XtWindow(wg);

  if (x11_display_type == MONOCHROME)
  {
    i = X11_Get_Line_Size(max_imagex);
    theImage = XCreateImage(theDisp,DefaultVisual(theDisp,
			DefaultScreen(theDisp)),
			x11_depth,XYPixmap,0,0,max_imagex,max_imagey,
			x11_bytes_pixel,i);
  }
  else
  {
    theImage = XCreateImage(theDisp,DefaultVisual(theDisp,
			DefaultScreen(theDisp)),
			x11_depth,ZPixmap,0,0,max_imagex,max_imagey,
			x11_bytes_pixel,(x11_bytes_pixel * max_imagex));
  }
  if (theImage == 0)
    fprintf(stderr,"X11 Setup: create image failed\n");

  if (x11_cmap_flag == TRUE)
  {
    for(i=0; i<x11_cmap_size; i++)
    {
      defs[i].pixel = i;
      defs[i].red   = cmap[i].red   << 8;
      defs[i].green = cmap[i].green << 8;
      defs[i].blue  = cmap[i].blue  << 8;
      defs[i].flags = DoRed | DoGreen | DoBlue;
    }
    theCmap = XCreateColormap(theDisp,mainW,
                              DefaultVisual(theDisp, DefaultScreen(theDisp)),
                              AllocAll);
    if (theCmap==0) 
    {
      fprintf(stderr,"cmap==0 error\n");
    }
    XStoreColors(theDisp,theCmap,defs,x11_cmap_size);
    /* XInstallColormap(theDisp,theCmap); */
    XSetWindowColormap(theDisp, mainW, theCmap);
  }

  xwm_hints.flags = InputHint;
  XSetWMHints(theDisp,mainW,&xwm_hints);
  XMapWindow(theDisp,mainW);
/*
  XSetInputFocus(theDisp,mainW,RevertToPointerRoot,CurrentTime);
*/
  XFlush(theDisp);
}

void xanim_expose(wg, event, str, np)
        Widget          wg;
        XExposeEvent   *event;
        String         *str;
        int            *np;
{
 if (anim_status == XA_UNSTARTED) 
 {
  anim_status = XA_RUN_NEXT;
  anim_holdoff = TRUE;
  XtAppAddTimeOut(theContext, 1, ShowAction, NULL);
 }

/* need pointer to global theImage
  XPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,imagex,disp_y);
*/

}

void xanim_key(wg, event, str, np)
Widget          wg;
XKeyEvent      *event;
String         *str;
int            *np;
{
        char            buff[10];
        KeySym          ks;
        XComposeStatus  st;
/* should always be KeyRelease 
 if (event->type == KeyRelease)
 {
*/
        XLookupString(event, buff, 10, &ks, &st);
	switch(buff[0])
	{
	 case 'q':
	 case 'Q': TheEnd();
		   break;
	 case 'g':
         case 'G': anim_flags &= (~ANIM_CYCLE);
		   break;
         case 's': 
         case 'S': 
		   if (cycle_on_flag) cycle_on_flag = 0;
		   else  cycle_on_flag = 1;
		   break;
	
         case 'r': 
         case 'R': 
		   if (x11_cmap_flag == TRUE)
		   {
		    int j,c_off;

		    c_off = x11_cmap_size-imagec;
                    for(j=0;j<imagec;j++)
                    {
                     defs[j].pixel = j+c_off;
                     defs[j].red   = cmap[j+c_off].red   << 8;
                     defs[j].green = cmap[j+c_off].green << 8;
                     defs[j].blue  = cmap[j+c_off].blue  << 8;
                     defs[j].flags = DoRed | DoGreen | DoBlue;
                    }
                    XStoreColors(theDisp,theCmap,defs,imagec);
 		    XFlush(theDisp);
		   }
		   break;
         case '.': 
		   if (   (anim_status == XA_STOP_NEXT)
		       || (anim_status == XA_STOP_PREV) )
		   {
		     anim_status = XA_STEP_NEXT;
		     if (anim_holdoff == TRUE) return;
		     anim_holdoff = TRUE;
  	             XtAppAddTimeOut(theContext, 1, ShowAction, NULL);
		   }
		   else anim_status = XA_STOP_NEXT;
		   break;
         case ',': 
		   if (   (anim_status == XA_STOP_NEXT)
		       || (anim_status == XA_STOP_PREV) )
		   {
		     anim_status = XA_STEP_PREV;
		     if (anim_holdoff == TRUE) return;
		     anim_holdoff = TRUE;
  	             XtAppAddTimeOut(theContext, 1, ShowAction, NULL);
		   }
		   else anim_status = XA_STOP_PREV;
		   break;
         case '>': 
		   if (   (anim_status == XA_STOP_NEXT)
		       || (anim_status == XA_STOP_PREV) )
		   {
		     anim_status = XA_FILE_NEXT;
		     if (anim_holdoff == TRUE) return;
		     anim_holdoff = TRUE;
  	             XtAppAddTimeOut(theContext, 1, ShowAction, NULL);
		   }
		   else anim_status = XA_STOP_NEXT;
		   break;
         case '<': 
		   if (   (anim_status == XA_STOP_NEXT)
		       || (anim_status == XA_STOP_PREV) )
		   {
		     anim_status = XA_FILE_PREV;
		     if (anim_holdoff == TRUE) return;
		     anim_holdoff = TRUE;
  	             XtAppAddTimeOut(theContext, 1, ShowAction, NULL);
		   }
		   else anim_status = XA_STOP_PREV;
		   break;
         case ' ': 
		   switch(anim_status)
		   {
		    case XA_RUN_NEXT:   
					anim_status = XA_STOP_NEXT; 
					break;
		    case XA_RUN_PREV:   
					anim_status = XA_STOP_PREV; 
					break;
		    case XA_STOP_NEXT:  
					anim_status = XA_RUN_NEXT;  
		     			if (anim_holdoff == TRUE) return;
					anim_holdoff = TRUE;
  	           			XtAppAddTimeOut(theContext, 1, 
							ShowAction, NULL);
					break;
		    case XA_STOP_PREV:  
					anim_status = XA_RUN_PREV;  
		     			if (anim_holdoff == TRUE) return;
					anim_holdoff = TRUE;
  	           			XtAppAddTimeOut(theContext, 1, 
							ShowAction, NULL);
					break;
		    case XA_STEP_NEXT:  
					anim_status = XA_RUN_NEXT;  
					break;
		    case XA_STEP_PREV:  
					anim_status = XA_RUN_PREV;  
					break;
		    case XA_FILE_NEXT:  
					anim_status = XA_RUN_NEXT;  
					break;
		    case XA_FILE_PREV:  
					anim_status = XA_RUN_PREV;  
					break;
		    default: 		anim_status = XA_STOP_NEXT; 
					break;
		   }
		   break;
	}
/* 
 }
*/
}

/* TEST
void xanim_quit(wg, event, str, np)
Widget          wg;
XKeyEvent      *event;
String         *str;
int            *np;
{
 TheEnd(); 
}
*/

void xanim_events()
{
 XtAppMainLoop(theContext);
}

ULONG X11_Get_Line_Size(xsize)
ULONG xsize;
{
  ULONG line_size;

  if (x11_display_type == MONOCHROME)
  {
    line_size = xsize / x11_bitmap_unit;
    if (xsize % x11_bitmap_unit) line_size++;
    line_size = (line_size * x11_bitmap_unit) / 8;
  }
  else line_size = xsize * x11_bytes_pixel;
  return(line_size);
}


  /*
   * What's this!? Direct access to X11 structures. tsch tsch.
   *
   * With half-height images, I fool X11 by telling it the image is
   * twice as wide and half as high, then only display left half of image.
   */
X11_Init_Image_Struct(image,xsize,ysize,flags)
XImage *image;
ULONG xsize,ysize,flags;
{
  ULONG line_size;

  line_size = X11_Get_Line_Size(xsize);

  if (flags & ANIM_LACE)
  {
    image->width = 2 * xsize;
    image->height = ysize/2;
    image->bytes_per_line = 2 * line_size;
  }
  else
  {
    image->width = xsize;
    image->height = ysize;
    image->bytes_per_line = line_size;
  }
}

X11_OutPut_Visual_Class(vis_class)
ULONG vis_class;
{
  switch(vis_class)
  {
   case GrayScale:   fprintf(stderr,"GrayScale"); break;
   case StaticGray:  fprintf(stderr,"StaticGray"); break;
   case PseudoColor: fprintf(stderr,"PseudoColor"); break;
   case StaticColor: fprintf(stderr,"StaticColor"); break;
   case DirectColor: fprintf(stderr,"DirectColor"); break;
   case TrueColor:   fprintf(stderr,"TrueColor"); break;
  }
}
