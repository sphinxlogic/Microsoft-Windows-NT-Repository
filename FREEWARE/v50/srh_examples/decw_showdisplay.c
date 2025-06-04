#module	EXAMPLE "SRH X1.0-000"
#pragma builtins

/*
** COPYRIGHT (c) 1992 BY
** DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.
** ALL RIGHTS RESERVED.
**
** THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
** ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE
** INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
** COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
** OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY
** TRANSFERRED.
**
** THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
** AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT
** CORPORATION.
**
** DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
** SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
*/

/*
**++
**  Facility:
**
**	Examples
**
**  Version: V1.0
**
**  Abstract:
**
**	Uses Xlib to gather info on the X display
**
**  Author:
**	Guenther GORE:: Froehlin
**	Steve Hoffman
**
**  Creation Date:  1-Jan-1990
**
**  Modification History:
**--
*/

#include <decw$include/cursorfont.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>

Display *dpy;
GC gc;
int    screen_number;
Screen *screen;
XGCValues xgcv;

char *visual_type[] = {
    "StaticGray",
    "GrayScale",
    "StaticColor",
    "PseudoColor",
    "TrueColor",
    "DirectColor"
    };

int main()
{          
    dpy = XOpenDisplay(0);	
    if (!dpy){
        printf("Display not opened!\n");
        exit(-1);
    }
    screen = XDefaultScreenOfDisplay(dpy);
    screen_number = XDefaultScreen(dpy);
    printf("visual type      : %s\n", 
           visual_type[XDefaultVisualOfScreen(screen)->class]);
    printf("display in pixels: %d height x %d width\n",
           XDisplayHeight(dpy,screen_number),XDisplayWidth(dpy,screen_number));
    printf("display in mm    : %d height x %d width\n",
           XDisplayHeightMM(dpy,screen_number),XDisplayWidthMM(dpy,screen_number));
    printf("bitmap unit      : %d, black pixel: %d, white pixel: %d\n",
           XBitmapUnit(dpy),XBlackPixel(dpy,screen_number),XWhitePixel(dpy,screen_number));
    printf("cells of screen  : %d\n",XCellsOfScreen(screen));
    printf("display cells    : %d\n",XDisplayCells(dpy,screen_number));
    printf("display planes   : %d\n",XDisplayPlanes(dpy,screen_number));
    printf("planes of screen : %d\n",XPlanesOfScreen(dpy,screen));
    printf("protocol revision: %d, protocol version: %d\n",
           XProtocolRevision(dpy),XProtocolVersion(dpy));
    printf("server vendor    : %s\n",XServerVendor(dpy));
    printf("vendor release   : %d\n",XVendorRelease(dpy));
} 
