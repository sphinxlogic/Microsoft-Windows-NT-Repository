/* x2uil.c - convert X window bitmap (.h file) to UIL bitmap format

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static void GiveUp(char *textP)
{
    printf ("%s\n", textP);
    exit (0);
}

#define xbitmapFilenameP	argv[1]
#define iconNameP		argv[2]

main(int argc, char **argv)
{
    int status;
    Display *dpy;
    unsigned int width, height;
    int hotX, hotY, x, y;
    Pixmap pixmap;
    XImage *imageP;
    unsigned long pixel;

    dpy = XOpenDisplay (0);
    if (!dpy) GiveUp ("x2uil - Can't open display");

    if (argc < 3) GiveUp ("usage:     x2uil xbitmapFilename iconName");

    status = XReadBitmapFile (
	dpy,
	RootWindowOfScreen (XScreenOfDisplay (dpy, XDefaultScreen (dpy))),
	xbitmapFilenameP,
	&width, &height, &pixmap, &hotX, &hotY);

    if (status == BitmapFileInvalid) GiveUp ("x2uil - BitmapFileInvalid");
    if (status == BitmapNoMemory)    GiveUp ("x2uil - BitmapNoMemory");

    imageP = XGetImage (dpy, pixmap, 0, 0, width, height, -1, ZPixmap);

    printf ("value %sColorTable : color_table (\n", iconNameP);
    printf ("  background color = \".\",\n");
    printf ("  foreground color = \"#\");\n");
    printf ("\n");
    printf ("/* width = %d, height = %d */\n", width, height);
    printf ("value %s : exported icon (color_table = %sColorTable,\n", iconNameP, iconNameP);

    for (y = 0; y < height; y++) {
        printf ("'");
	for (x = 0; x < width; x++) {
	    pixel = XGetPixel (imageP, x, y);
	    if (pixel) printf ("#");
	    else       printf (".");
	}
        if (y != (height - 1)) printf ("',\n");
	else                   printf ("');\n");
    }
}
