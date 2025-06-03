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
 * modified:
 *
 * fgk 10/23/92 - add scaling and put 8-bit to 24-bit conversion here.
 *
 */

#include "xanim.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/signal.h>

#ifndef VMS
#include <sys/param.h>
#include <sys/times.h>
#include <memory.h>
#else
#include <time.h>
#endif

#include <ctype.h>
#include "xanim_x11.h"

void xanim_expose();
void xanim_key();
void xanim_quit();
void xanim_events();
void X11Setup();
void XAnimPutScaledImage();

/*
 *  When converting from 8-plane to 24-plane
 *  we need to know where the RGB components
 *  are so we can shift and mask the values.
 *
 */
int x11_red_mask;
int x11_green_mask;
int x11_blue_mask;
int x11_red_shift;
int x11_green_shift;
int x11_blue_shift;

/*
 *  Just for reference now.
 */
int x11_max_image_width;
int x11_max_image_height;

/*
 *  To make life simple, scaling and pixel size conversion
 *  is done via a call from a putimage macro in XAnim.  If
 *  the image is scaled or turned into a 24-plane image, the
 *  call comes here, and is pixel replicated into the buffer
 *  pointed to by this pointer.
 */
char *workImageBuffer;
int workBufferSize; /* it's size */

/*********************************** X11 stuff */
Display       *theDisp;
Visual        *theVisual;
Colormap       theCmap;
Window         mainW;
GC             theGC;
XImage        *theImage, *x11_work_image;
XColor         defs[256];
/******************************** Xt stuff */
XtAppContext  theContext;

#define ACTIONTABLE_SIZE 4
XtActionsRec actionTable[] = {
        {"Expose", xanim_expose},
        {"Quit", xanim_quit},
        {"Key", xanim_key},
        {"Go", ShowAnimation}
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
<Key>:Key()";

ULONG X11_Get_Shift(mask)
ULONG mask;
{
 int i;

 i = 0;
 while( (i < 32) && (!(mask & 0x01)) )
 {
  mask >>= 1;
  i++;
 }

 if (i >= 32)
 {
  fprintf(stderr,"X11_Get_Shift: wierd mask value %lx\n",mask);
  i = 0;
 }
 return(i);
}


void X11_Pre_Setup()
{
 int i,vis_num;
 XVisualInfo *vis;

 XtToolkitInitialize();
 theContext = XtCreateApplicationContext();
 XtAppAddActions(theContext, actionTable, ACTIONTABLE_SIZE);

 i = 0;
 theDisp = XtOpenDisplay(theContext, NULL, "xanim", "XAnim",NULL,0,&i,NULL);
 if (theDisp==NULL)
 {
  TheEnd1("Unable to open display\n");
 }

 x11_depth = DefaultDepth(theDisp, DefaultScreen(theDisp));

 theVisual = DefaultVisual(theDisp, DefaultScreen(theDisp));
 x11_class = theVisual->class;
 if ( (x11_class == PseudoColor) && (x11_depth <=8) && (x11_depth >=1) )
 {
  x11_bytes_pixel = 1;
  x11_cmap_flag = TRUE;
  x11_display_type = PSEUDO_COLOR_8BIT;
 }
 else if ( (x11_class == TrueColor) && (x11_depth == 24) )
      {
       x11_bytes_pixel = 4;
       x11_cmap_flag = FALSE;
       x11_display_type = TRUE_COLOR_24BIT;
       x11_red_mask = theVisual->red_mask;
       x11_green_mask = theVisual->green_mask;
       x11_blue_mask = theVisual->blue_mask;
       x11_red_shift   = X11_Get_Shift(x11_red_mask);
       x11_green_shift = X11_Get_Shift(x11_green_mask);
       x11_blue_shift  = X11_Get_Shift(x11_blue_mask);

/* fprintf(stderr,"mask= R %8lx G%8lx B%8lx\nshift= R%8ld G%8ld B%8ld\n",
		  x11_red_mask,x11_green_mask ,x11_blue_mask,
		  x11_red_shift, x11_green_shift, x11_blue_shift ); */
      }
      else /* not yet supported */
      {
       fprintf(stderr,"Unsupported Visual class=%ld depth=%ld \n",
		x11_class,x11_depth);
       TheEnd();
      }

 x11_cmap_size = DisplayCells(theDisp, DefaultScreen(theDisp));


#ifdef X11_VERBOSE
 fprintf(stderr,"Class = %ld depth=%ld bytes_pixel=%lx\n",
		x11_class,x11_depth,x11_bytes_pixel);
 fprintf(stderr,"x11_cmap_size = %ld\n",x11_cmap_size);
#endif


#ifdef X11_VERBOSE
 vis = XGetVisualInfo (theDisp, VisualNoMask, NULL, &vis_num);
 if ((vis == NULL) || (vis_num == 0) )
 {
  TheEnd1("X11: Couldn't get any Visuals\n");
 }
 else
 {
  for(i=0;i<vis_num;i++)
  {
   fprintf(stderr,"visual %ld) depth= %ld  class= %ld  ",
     i, vis[i].depth, vis[i].class, vis[i].colormap_size );
   fprintf(stderr,"%ld) class = %ld\n",i,vis[i].class);
   fprintf(stderr,"%ld) cmap sz = %ld\n",i,vis[i].colormap_size);
  }
 }
 XFree(vis);
#endif

  cmap = (ColorReg *) malloc( x11_cmap_size * sizeof(ColorReg) );

  if (x11_cmap_flag == TRUE)
  {
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
      cmap[i].map   = i;
    }
  }
}

/*
 * Setup X11 Display, Window and Toolkit
 */
void X11_Setup(max_imagex,max_imagey,startx,starty,xp,yp)
int max_imagex,max_imagey;
int startx,starty,xp,yp;
{
 int i,off;
 Widget wg;
 int n;
 Arg arglist[20];

 if ((xp < 0) || (yp < 0))
	FindBestWindowPosition(
			theDisp, XDefaultRootWindow(theDisp),
			max_imagex, max_imagey, 0, 0, &xp, &yp, 0, 0);

 x11_max_image_width  = max_imagex;
 x11_max_image_height = max_imagey;

 n = 0;
 XtSetArg(arglist[n], XtNwidth, startx*x11_scale); n++;
 XtSetArg(arglist[n], XtNheight, starty*x11_scale); n++;
 XtSetArg(arglist[n], XtNmaxWidth, max_imagex*x11_scale); n++;
 XtSetArg(arglist[n], XtNmaxHeight, max_imagey*x11_scale); n++;
 XtSetArg(arglist[n], XtNx, xp); n++;
 XtSetArg(arglist[n], XtNy, yp); n++; 
 wg = XtAppCreateShell(WINDOW_TITLE, "XAnim", applicationShellWidgetClass,
                        theDisp, arglist, n);

 XtGetApplicationResources (wg, &resource, application_resources,
                            XtNumber(application_resources), NULL, 0);

 XtRealizeWidget(wg);
 XtOverrideTranslations(wg, XtParseTranslationTable(Translation));

 mainW = XtWindow(wg);

/*
 *  The *source* image is always 8-bits.  If the image is later transformed
 *  to 24-bits, then the output will be through the x11_work_image struct.
 *
 */
 theImage = XCreateImage(theDisp,DefaultVisual(theDisp,DefaultScreen(theDisp)),
                            x11_depth,ZPixmap,0,0,max_imagex,max_imagey,
			    8,max_imagex);

 x11_work_image = XCreateImage(theDisp,DefaultVisual(theDisp,DefaultScreen(theDisp)),
                            x11_depth,ZPixmap,
			    0,0,max_imagex*x11_scale, max_imagey*x11_scale,
			    32, 0);

/*
 *  Allocate enough memory to contain the largest image
 *
 */
 workBufferSize  = ((x11_scale * max_imagey) *
        	    ((((max_imagex * x11_bytes_pixel) * x11_scale) + 3) & ~3));
	 
 workImageBuffer =
  (char *) malloc (workBufferSize); 

 x11_work_image->data = workImageBuffer;

 theGC     = DefaultGC(theDisp,DefaultScreen(theDisp));

 if (x11_cmap_flag == TRUE)
 {

/*
   for(i=0;i<x11_cmap_size;i++)
   {
      defs[i].pixel = i;
      defs[i].flags = DoRed | DoGreen | DoBlue;
   }
   XQueryColors(theDisp,DefaultColormap(theDisp, DefaultScreen(theDisp)),
                defs,x11_cmap_size);

*/
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
   XInstallColormap(theDisp,theCmap);
   XSetWindowColormap(theDisp, mainW, theCmap);
 }

 XSelectInput(theDisp, mainW, ExposureMask | KeyPressMask);

 XMapWindow(theDisp,mainW);
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

/*
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
 if (event->type == KeyPress)
 {
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
 }
}

void xanim_quit(wg, event, str, np)
Widget          wg;
XKeyEvent      *event;
String         *str;
int            *np;
{
 TheEnd();
}

void xanim_events()
{
 XtAppMainLoop(theContext);
}

/*
 *  This routine does a simple pixel replication of an input image.  It
 *  uses the global x11_scale to determine how much to blow up the image.
 *  The call may be displaying only a portion of the image data, so the
 *  input image is massaged as part of the replication only to expand the
 *  subimage being displayed.  The entire new image is then output to the
 *  destination X/Y.
 *
 */
void XAnimPutScaledImage(dsp,win,igc,pImage,srcx,srcy,dstx,dsty,width,height,cmap)
Display   *dsp;
Window    win;
GC        igc;
XImage    *pImage;
int       srcx,srcy,dstx,dsty,width,height;
ColorReg *cmap;

{
 int x, y, rx, ry, ospan, ispan;

 if (x11_bytes_pixel == 1)
   {
    /*
     *  Scale a 8-bit image up in X and Y for an 8-bit output
     *
     */
     UBYTE *pCurISrc, *pCurOSrc;
     unsigned int *pSrcImage, *pDstImage, *pStart, *pScanline, ipix, opix;
     int doff, scnt, dmsk, scanw;

    /*
     *  Find out the width of a image scanline
     *
     */
     ispan = pImage->bytes_per_line;

     if (ispan & 3) {
	  printf("Assumption error in scaling.  Image width not a multiple of 4\n");
	  sys$exit();
	}

    /*
     *   Point to the first byte to be output (which is offset into the
     *   image by srcx, srcy
     */
     pSrcImage = pCurISrc = (pImage->data + (ispan * srcy) + srcx);
     pDstImage = pCurOSrc = x11_work_image->data;

     width = (width + 3) & ~3;

    /*
     *   Make the output span the width scaled, rounded to 4 bytes.
     */
     ospan = x11_work_image->bytes_per_line = (x11_scale * width);

    /*
     *   Outer Loop for Y
     */
     for (y = 0; y < height; y += 1)
       {
         pStart = pDstImage;

        /*
         *   Scale the scanline in X
	 *   Use longword addressing of the picture data and
	 *   extract the bytes by hand... it should be faster.
	 *
         */
	 ipix  = *pSrcImage++;
	 opix  = 0;
	 doff  = 0;
	 scnt  = 4;
	 dmsk  = 0xFF;

	/*
	 *  For each byte in the scanline...
	 */
         for (x = 0; x < width; x += 1)
	   {
	    /*
	     *  Inner loop is the scale factor...
	     */
	     for (rx = 0; rx < x11_scale; rx += 1)
	       {

		/*
		 *  Get the current input pixel, shift it to the
		 *  right position in the output longword and OR it.
		 */
		 opix |= (ipix & 0xFF) << doff;
		 doff += 8;

		/*
		 *  If we overflow the destination longword, write
		 *  the longword and reset the offset.  Clear the output.
		 */
		 if (doff > 24) {
		     *pDstImage++ = opix;
		     opix = 0;
		     doff = 0;
		   }
	       }

	    /*
	     *  Shift the input pixel left a byte and decrement the
	     *  source pixel count.  When it becomes zero, then get
	     *  the next 4 pixels and set the counter to 4.
	     */
	     ipix  >>= 8;
	     scnt -= 1;
	     if (!scnt) {
		 scnt = 4;
	         ipix  = *pSrcImage++;
	       }

	   }

         pCurOSrc += ospan;
	 pDstImage = pCurOSrc;

        /*
         *   Replicate the scanline
         */
         for (ry = 1; ry < x11_scale; ry += 1)
           {

             pScanline = pStart;

	    /*
	     *  Get the longword count
	     */
	     scanw = ((width * x11_scale) + 3) >> 2;

	    /*
	     *  Copy longwords...
	     */
	     for (rx = 0; rx < scanw; rx += 1)
		*pDstImage++ = *pScanline++;

	     pCurOSrc += ospan;
             pDstImage = pCurOSrc;
	   }
	 pCurISrc += ispan;
         pSrcImage = pCurISrc;
       }
   }
 else
   {
    /*
     *   Scale an 8-bit image up in X and Y and map the pixels to 24-planes!
     *
     */
     ULONG  pixel, *pDstImage, *pStart, *pScanline;
     unsigned char *pSrcImage,  *pCurISrc;
     int inpix;

     ispan = pImage->bytes_per_line;

    /*
     *   Point to the first byte in the source to be output (which
     *   is offset into the image by srcx, srcy
     */
     pSrcImage = pCurISrc = (pImage->data + (ispan * srcy) + srcx);

    /*
     *   Get a pointer to the output image
     */
     pDstImage = x11_work_image->data;

    /*
     *   Make the output span the width scaled, rounded to 4 bytes, this
     *   also becomes the bytes_per_line in the image buffer.
     */
     x11_work_image->bytes_per_line =
        (((width * x11_bytes_pixel) * x11_scale ) + 3) & ~3;

    /*
     *   Outer Loop for Y
     */
     for (y = 0; y < height; y += 1)
       {
         pStart = pDstImage;

        /*
         *   Scale the scanline in X
         */
         for (x = 0; x < width; x += 1)
	   {
             inpix = *pSrcImage++;

             pixel  = (cmap[ inpix ].red   << x11_red_shift)   & x11_red_mask;
             pixel |= (cmap[ inpix ].green << x11_green_shift) & x11_green_mask;
             pixel |= (cmap[ inpix ].blue  << x11_blue_shift)  & x11_blue_mask;

	     for (rx = 0; rx < x11_scale; rx += 1)
	       {
	         *pDstImage++ = pixel;
	       }
	   }

        /*
         *   Replicate the scanline
         */
         for (ry = 1; ry < x11_scale; ry += 1)
           {
             pScanline = pStart;

	     for (rx = 0; rx < (width * x11_scale); rx += 1)
		*pDstImage++ = *pScanline++;
	   }

         pSrcImage = pCurISrc = (pCurISrc + ispan);

       }
    }

  dstx   *= x11_scale;
  dsty   *= x11_scale;

  x11_work_image->width  = width  = (width  * x11_scale);
  x11_work_image->height = height = (height * x11_scale);

  XPutImage(dsp,win,igc,x11_work_image,0,0,dstx,dsty,width,height);

}

Image8_to_Image24(pImage,cmap,x,y,width,height)
XImage *pImage;
ColorReg *cmap;
int x,y,width,height;
{
 int i, j, ispan;
 UBYTE *picin, *from;
 ULONG *to;
 register ULONG pixel,tmp;

 ispan = pImage->bytes_per_line;

 if (x11_display_type != TRUE_COLOR_24BIT) return;

 x11_work_image->bytes_per_line = width * 4;
 x11_work_image->width  = width;
 x11_work_image->height = height;

 from = picin = pImage->data + (ispan * y) + x;
 to   = x11_work_image->data;
 
 for (i = 0; i < height; i += 1)
   {
     for (j = 0; j < width; j += 1)
       {
         pixel  = (cmap[ *from ].red   << x11_red_shift)   & x11_red_mask;
         pixel |= (cmap[ *from ].green << x11_green_shift) & x11_green_mask;
         pixel |= (cmap[ *from ].blue  << x11_blue_shift)  & x11_blue_mask;
         *to++ = pixel;
         from++;
       }
     from = picin = picin + ispan;
   }
}
