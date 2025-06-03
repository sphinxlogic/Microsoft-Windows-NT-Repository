 #module BannerGlobe "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the code, to run the rotating globe.
 **
 **  AUTHORS:
 **
 **      JIM SAUNDERS
 **
 **
 **  CREATION DATE:     20th June 1988
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*6D
 #include <stdio.h>
 #include <file.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*6E
*6I
 
 #include "stdio.h"
 #include "file.h"
 #include "MrmAppl.h"
 
 
*6E
 #include "Banner"
 
 
 void BannerreadFrame ();
 void BannerGlobe ();
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 #define Nframes  30
 #define bnr_world_size  64
 /*
  * now our runtime data structures
  */
 
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
 
 extern void BannerSignalError();
 
 static char buf[512];
 static int  pixmap_init = 0;
 static int  pixmaps[Nframes];
 static XImage  *image_vector[Nframes];
 static int current_frame = 0;
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerReadFrame (file, image)
 **
 **      This routine reads in the next frame from the Image file that 
 **	contains the pictures of our world.
 **
 **  FORMAL PARAMETERS:
 **
 **      file		The open file to read from
 **	image		The image to build
 **
 **  IMPLICIT INPUTS:
 **
 **      BannerGC
 **
 **  IMPLICIT OUTPUTS:
 **
 **
 **  COMPLETION CODES:
 **
 **
 **  SIDE EFFECTS:
 **
 **
 **--
 **/
 static void	
 BannerReadFrame(file, image)
 int image;
 {
 
 /*
  * Now read the next record from the file.
  */
     if (! read(file, buf, 512))
 	BannerSignalError ("Failed to read image record, for Globe data file",
 	    0);
 
 /*
  * Now create the desired pixmap, and image
  */
 
*2D
     pixmaps[image] = XCreatePixmap(XtDisplay(Bnr.cube_widget), 
*2E
*2I
     pixmaps[image] = XCreatePixmapFromBitmapData(
*5D
 			XtDisplay(Bnr.cube_widget), 
*5E
*2E
*5D
 			XtWindow(Bnr.cube_widget),
*5E
*5I
 			XtDisplay(Bnr.globe_widget), 
 			XtWindow(Bnr.globe_widget),
*5E
*2D
 			bnr_world_size,
 			bnr_world_size,
 			DefaultDepth(XtDisplay(Bnr.dp_id),0));
 
     image_vector[image] = XCreateImage (XtDisplay(Bnr.cube_widget),
 			0,    
 			DefaultDepth(XtDisplay(Bnr.dp_id),0),
 			XYPixmap,
 			0,
 			&buf,
 			bnr_world_size,
 			bnr_world_size,
 			8,
 			((bnr_world_size + 7) / 8));
 
 /*    image_vector[image]->Byte_Order = LSBFirst;
     image_vector[image]->Bitmap_Bit_Order = LSBFirst; */
 
     XPutImage (XtDisplay(Bnr.cube_widget), pixmaps[image], BannerGC,
 	image_vector[image], 0, 0, 0, 0, bnr_world_size, bnr_world_size);
*2E
*2I
 			buf,
                         bnr_world_size,
                         bnr_world_size,
*3D
 			Bnr.foregorund,
*3E
*3I
 			Bnr.foreground,
*3E
 			Bnr.background,
*4D
 			DefaultDepth(XtDisplay(Bnr.cube_widget),0));
*4E
*4I
*5D
 			DefaultDepth(XtDisplay(Bnr.cube_widget),
*5E
*5I
 			DefaultDepth(XtDisplay(Bnr.globe_widget),
*5E
 			    XDefaultScreen(XtDisplay(Bnr.main_widget))));
*4E
 
*2E
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerGlobe (x, y, width, height, refresh);
 **
 **      This Routine is the control routine for the Banner Globe. It reads
 **	the data for the images, from the data file in SYS$SHARE, and creates 
 **	pixmaps from it. These are then copied into the desired window, in 
 **	order, as to create the desired images.
 **
 **  FORMAL PARAMETERS:
 **
 **      x, y, width, height of the window.
 **
 **  IMPLICIT INPUTS:
 **
 **
 **  IMPLICIT OUTPUTS:
 **
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 void	
 BannerGlobe(x, y, width, height, refresh)
 int x, y, width, height, refresh;
 {
*5D
     int file, cube_offset;
*5E
*5I
     int file, globe_offset;
*5E
 
 /*
  * If we have initialised, then do it now.
  */
 
     if (!pixmap_init)
 	{
 	int i;
 
 	pixmap_init = 1;
 	file = open("SYS$LIBRARY:DECW$BANNERGLOBE.DAT", 
 	    O_RDONLY, 0004, "rfm=fix", "mrs=512");
 
 	for (i=0; i <= Nframes-1; i++)
 	    BannerReadFrame (file, i);
 
 	close(file);
 	}
 
 /*
  * Now find the next frame to display;
  */
 
     if (! refresh)
 	if (current_frame++ == Nframes - 1)
 	    current_frame = 0;    
 
*5D
     cube_offset = Bnr.cube_width/2;
     cube_offset = cube_offset - bnr_world_size/2;
*5E
*5I
     globe_offset = Bnr.globe_width/2;
     globe_offset = globe_offset - bnr_world_size/2;
*5E
 
*5D
     if (cube_offset > 0)
 	    x = x + cube_offset;
*5E
*5I
     if (globe_offset > 0)
 	    x = x + globe_offset;
*5E
 
*5D
     cube_offset = Bnr.cube_height/2;
     cube_offset = cube_offset - bnr_world_size/2;
*5E
*5I
     globe_offset = Bnr.globe_height/2;
     globe_offset = globe_offset - bnr_world_size/2;
*5E
 
*5D
     if (cube_offset > 0)
 	y = y + cube_offset;
*5E
*5I
     if (globe_offset > 0)
 	y = y + globe_offset;
*5E
 
 /*
*2D
  * now copy the desired pixmap into the window. Note we reverse the 
*2E
*2I
  * now copy the desired pixmap into the window. Note we reverse the
*2E
  * foreground, background to get it display correctly.
  */
*2D
     XCopyPlane (XtDisplay(Bnr.cube_widget),
 		pixmaps[current_frame],
 		XtWindow(Bnr.cube_widget),
 		BannerGC,
 		0,
 		0,
 		bnr_world_size,
 		bnr_world_size,
 		x,
 		y,
 		1);
*2E
*2I
 
*5D
     XCopyArea (XtDisplay(Bnr.cube_widget),
*5E
*5I
     XCopyArea (XtDisplay(Bnr.globe_widget),
*5E
 	pixmaps[current_frame],
*5D
 	XtWindow(Bnr.cube_widget),
*5E
*5I
 	XtWindow(Bnr.globe_widget),
*5E
 	BannerGC,
 	0,
 	0,
 	bnr_world_size,
 	bnr_world_size,
 	x,
 	y);
*2E
 
 
*2I
*5D
     XFlush(XtDisplay(Bnr.cube_widget));
*5E
*5I
     XFlush(XtDisplay(Bnr.globe_widget));
*5E
*2E
 
 }
