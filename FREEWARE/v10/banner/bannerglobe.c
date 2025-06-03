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


#include "stdio.h"
#include "file.h"
#include "MrmAppl.h"


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

    pixmaps[image] = XCreatePixmapFromBitmapData(
			XtDisplay(Bnr.globe_widget), 
			XtWindow(Bnr.globe_widget),
			buf,
                        bnr_world_size,
                        bnr_world_size,
			Bnr.foreground,
			Bnr.background,
			DefaultDepth(XtDisplay(Bnr.globe_widget),
			    XDefaultScreen(XtDisplay(Bnr.main_widget))));

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
    int file, globe_offset;

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

    globe_offset = Bnr.globe_width/2;
    globe_offset = globe_offset - bnr_world_size/2;

    if (globe_offset > 0)
	    x = x + globe_offset;

    globe_offset = Bnr.globe_height/2;
    globe_offset = globe_offset - bnr_world_size/2;

    if (globe_offset > 0)
	y = y + globe_offset;

/*
 * now copy the desired pixmap into the window. Note we reverse the
 * foreground, background to get it display correctly.
 */

    XCopyArea (XtDisplay(Bnr.globe_widget),
	pixmaps[current_frame],
	XtWindow(Bnr.globe_widget),
	BannerGC,
	0,
	0,
	bnr_world_size,
	bnr_world_size,
	x,
	y);


    XFlush(XtDisplay(Bnr.globe_widget));

}
