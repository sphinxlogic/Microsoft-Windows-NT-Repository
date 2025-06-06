/*
 * XBoing - An X11 blockout style computer game
 *
 * (c) Copyright 1993, 1994, 1995, Justin C. Kibell, All Rights Reserved
 *
 * The X Consortium, and any party obtaining a copy of these files from
 * the X Consortium, directly or indirectly, is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons who receive
 * copies from any such party to do so.  This license includes without
 * limitation a license to do the foregoing actions under any patents of
 * the party supplying this software to the X Consortium.
 *
 * In no event shall the author be liable to any party for direct, indirect,
 * special, incidental, or consequential damages arising out of the use of
 * this software and its documentation, even if the author has been advised
 * of the possibility of such damage.
 *
 * The author specifically disclaims any warranties, including, but not limited
 * to, the implied warranties of merchantability and fitness for a particular
 * purpose.  The software provided hereunder is on an "AS IS" basis, and the
 * author has no obligation to provide maintenance, support, updates,
 * enhancements, or modifications.
 */

/* 
 * =========================================================================
 *
 * $Id: misc.c,v 1.1.1.1 1994/12/16 01:36:47 jck Exp $
 * $Source: /usr5/legends/jck/xb/master/xboing/misc.c,v $
 * $Revision: 1.1.1.1 $
 * $Date: 1994/12/16 01:36:47 $
 *
 * $Log: misc.c,v $
 * Revision 1.1.1.1  1994/12/16  01:36:47  jck
 * The XBoing distribution requires configuration management. This is why the
 * cvs utility is being used. This is the initial import of all source etc..
 *
 *
 * =========================================================================
 */

/*
 *  Include file dependencies:
 */


#include <stdio.h>
#ifndef __DECC
#include <stdlib.h>
#endif
#include <stddef.h>
#include <ctype.h>
#include <unistd.h>

#ifdef VMS
#include "unix_time.h"
#endif  /* VMS */

#ifdef __DECC
#include "param.h"
#else
#include <sys/param.h>
#endif  /* __DECC */

#ifdef VMS
#include "pwd.h"
#else
#include <pwd.h>
#endif /* VMS */

#ifdef VMS
#ifdef __DECC
#ifdef __DECC_VER
#if (__DECC_VER < 50200000)
typedef char * caddr_t;
typedef unsigned long u_long;
typedef unsigned short ushort;
#endif /* __DECC_VER < 50200000 , that is DEC C 5.0 or 5.1 */
#else    /* DEC C 4.0 , __DECC_VER is not defined */
typedef char * caddr_t;
typedef unsigned long u_long;
typedef unsigned short ushort;
#endif /* __DECC_VER */
#else
typedef int wchar_t;
#endif
#include "in.h"
#else
#include <netinet/in.h>
#endif /* VMS */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include "init.h"
#include "error.h"
#include "stage.h"
#include "dialogue.h"

#include "misc.h"

/*
 *  Internal macro definitions:
 */

/*
 *  Internal type declarations:
 */

/*
 *  Internal variable declarations:
 */

#ifdef VMS
char * vmsusername;
int    vmsuserlen;
#include <lib$routines.h>
unsigned long int statvms;
float seconds;
#endif /* VMS */

#if NeedFunctionPrototypes
int usleep(unsigned long usec)
#else
int usleep(usec)
        unsigned long usec;
#endif
{
#ifdef SYSV
#ifdef __clipper__
    struct timeval tv;
    tv.tv_sec=((usec)/1000);
    tv.tv_usec=(((usec)%1000)*1000);
    select(1,NULL,NULL,NULL,&tv);
#else
    poll((struct poll *) 0, (size_t) 0, usec / 1000);   /* ms resolution */
#endif
#else
    struct timeval timeout;
    timeout.tv_usec = usec % (unsigned long) 1000000;
    timeout.tv_sec = usec / (unsigned long) 1000000;
#ifndef VMS
    select(0, (void *) 0, (void *) 0, (void *) 0, &timeout);
#else
      seconds = ((float) usec);
      statvms = lib$wait(&seconds);
#endif
#endif
    return 0;
}

#if NeedFunctionPrototypes
void sleepSync(Display *display, unsigned long ms)
#else
void sleepSync(display, ms)
    Display *display;
    unsigned long ms;
#endif
{
    struct timeval st, et;
    long SyncTime;

    gettimeofday(&st, NULL);
    XSync(display, False);
    gettimeofday(&et, NULL);

    SyncTime = (((et.tv_sec - st.tv_sec) * 1000) +
               ((et.tv_usec - st.tv_usec) / 1000) );

    if ((ms) > ((1000 / 60) + SyncTime))
        usleep(ms - SyncTime);
}

#if NeedFunctionPrototypes
void DrawLine(Display *display, Window window, int x, int y, int x2, int y2, 
	int colour, int width)
#else
void DrawLine(display, window, x, y, x2, y2, colour, width)
	Display *display;
	Window window;
	int x;
	int y;
	int x2;
	int y2; 
	int colour;
	int width;
#endif
{
	/* Change the width of the line */
	XSetLineAttributes(display, gcxor, width, LineSolid, CapProjecting, 
		JoinMiter);
	XSetLineAttributes(display, gcand, width, LineSolid, CapProjecting, 
		JoinMiter);

	/* Set to the desired colours */
	XSetBackground(display, gcxor, colour);
	XSetForeground(display, gcxor, colour);

	/* Now draw the line */
	XDrawLine(display, window, gcxor, x, y, x2, y2);
	XDrawLine(display, window, gcand, x, y, x2, y2);
	XDrawLine(display, window, gcxor, x, y, x2, y2);
}

#if NeedFunctionPrototypes
void DrawShadowCentredText(Display *display, Window window, XFontStruct *font,
	char *string, int y, int colour, int width)
#else
void DrawShadowCentredText(display, window, font, string, y, colour, width)
	Display *display;
	Window window;
	XFontStruct *font;
	char *string;
	int y;
	int colour;
	int width;
#endif
{
    int plen, len, x;

	/* String length */
    len = strlen(string);

	/* Length of string in pixels */
    plen = XTextWidth(font, string, len);

	/* Start drawing so the text is centered */
    x = (width / 2) - (plen / 2);

	/* Draw the text with a shadow */
    DrawText(display, window, x+2, y + 2, font, black, string, -1);
    DrawText(display, window, x, y, font, colour, string, -1);
}

#if NeedFunctionPrototypes
void DrawShadowText(Display *display, Window window, XFontStruct *font,
	char *string, int x, int y, int colour)
#else
void DrawShadowText(display, window, font, string, x, y, colour)
	Display *display;
	Window window;
	XFontStruct *font;
	char *string;
	int x;
	int y;
	int colour;
#endif
{
    int len;

	/* String length */
    len = strlen(string);

	/* Draw the text with a shadow */
    DrawText(display, window, x+2, y + 2, font, black, string, -1);
    DrawText(display, window, x, y, font, colour, string, -1);
}

#if NeedFunctionPrototypes
void DrawTextFast(Display *display, Window window, int x, int y, XFontStruct *font, 
	int colour, char *text, int numChar)
#else
void DrawTextFast(display, window, x, y, font, colour, text, numChar)
	Display *display;
	Window window;
	int x;
	int y;
	XFontStruct *font;
	int colour;
	char *text;
	int numChar;
#endif
{
	int len = strlen(text);

	/* If numchar is passed > 0 then only draw numChar characters */
	if (numChar > 0) 
		len = numChar;

	/* Change to the new font */
	XSetFont(display, gccopy, font->fid);

	/* Change the drawing function */
	XSetForeground(display, gccopy, colour);

	/* Draw the string into the drawable */
	XDrawString(display, window, gccopy, x, y + font->ascent, text, len);
}

#if NeedFunctionPrototypes
void DrawText(Display *display, Window window, int x, int y, XFontStruct *font, 
	int colour, char *text, int numChar)
#else
void DrawText(display, window, x, y, font, colour, text, numChar)
	Display *display;
	Window window;
	int x;
	int y;
	XFontStruct *font;
	int colour;
	char *text;
	int numChar;
#endif
{
	int len = strlen(text);

	/* If numchar is passed > 0 then only draw numChar characters */
	if (numChar > 0) 
		len = numChar;

	/* Change to the new font */
	XSetFont(display, gcxor, font->fid);
	XSetFont(display, gcand, font->fid);

	/* Change the drawing function */
	XSetBackground(display, gcxor, colour);
	XSetForeground(display, gcxor, colour);

	/* Draw the string into the drawable */
	XDrawString(display, window, gcxor, x, y + font->ascent, text, len);
	XDrawString(display, window, gcand, x, y + font->ascent, text, len);
	XDrawString(display, window, gcxor, x, y + font->ascent, text, len);
}

#if NeedFunctionPrototypes
void RenderShape(Display *display, Window window, Pixmap pixmap, 
	Pixmap mask, int x, int y, int w, int h, int clear)
#else
void RenderShape(display, window, pixmap, mask, x, y, w, h, clear)
	Display *display;
	Window window;
	Pixmap pixmap;
	Pixmap mask;
	int x;
	int y;
	int w;
	int h;
	int clear;
#endif
{
	/* Clear the background first? */
    if (clear) XClearArea(display, window, x, y, w, h, False);

	/* Set to dest x and y clip origin */
    XSetClipOrigin(display, gc, x, y); 

	/* Set the clipping mask */
    XSetClipMask(display, gc, mask);   

    XCopyArea(display, pixmap, window, gc, 0, 0, w, h, x, y);

	/* Unset clip (or add a clip gc) */
    XSetClipMask(display, gc, None);   
}

#if NeedFunctionPrototypes
void FreeMisc(Display *display)
#else
void FreeMisc(display)
    Display *display;
#endif
{
}

#if NeedFunctionPrototypes
int ColourNameToPixel(Display *display, Colormap colormap, char *colourName)
#else
int ColourNameToPixel(display, colormap, colourName)
    Display *display;
    Colormap colormap;
    char *colourName;
#endif
{
    XColor colour;

    /* Obtain the exact colour from the colour name */
    if (XParseColor(display, DefaultColormap(display,
        XDefaultScreen(display)), colourName, &colour) != 0)
    {
        /* Now allocate the colour */
        if (XAllocColor(display, colormap, &colour) != 0)
		{
        	/* Success - return the pixel id */
        	return colour.pixel;
		}
    }

    /* Obviously a problem so barf */
    ShutDown(display, 1, "Error while parsing colours.");

    /* NOT REACHED */
    return 1;
}


#if NeedFunctionPrototypes
char *getUsersFullName(void)
#else
char *getUsersFullName()
#endif
{
    struct passwd *pass;
    char *comma;
    char *cp1, *cp2;
    static char fullname[80];

    /* Get user information from password file */
#ifndef VMS
    if (!(pass = getpwuid(getuid())))
        return("Anonymous?");       /* Unknown user oops. */

    /* find a comma indicating further info after name */
    comma = strchr(pass->pw_gecos, ',');

    /* NULL out the comma */
    if (comma) *comma = '\0';

    /* Use the nickname if not null otherwise password file name */
    cp1 = pass->pw_gecos;
    cp2 = fullname;

    /* Search through the gecos field looking for an '&' which on very
     * old UNIX systems is supposed to be the users user name with the
     * first letter uppercased.
     */
    while(*cp1)
    {
        /* Look for the '&' symbol */
        if(*cp1 != '&')
            *cp2++ = *cp1++;
        else
        {
            /* A ha. Now copy the users name to be in place of '&' */
            strcpy(cp2, pass->pw_name);
       
            /* Convert the first letter to uppercase. */
            if(islower(*cp2))
                *cp2 = toupper(*cp2);

            /* Continue with the remaining string */
            while(*cp2) cp2++;
                cp1++;
        }
    }
#else
    vmsusername = getenv("USER");
    vmsuserlen  = strlen(vmsusername);
    strncpy(fullname, (const) vmsusername,vmsuserlen);
#endif /* VMS */

    /* Return their name without any trailing stuff */
    return(fullname);
}

#if NeedFunctionPrototypes
char *GetHomeDir(void)
#else
char *GetHomeDir()
#endif
{
    int uid;
    struct passwd *pw;
    register char *ptr;

#ifdef VMS
#define MAXPATHLEN  256
#endif  /* VMS */

    static char dest[MAXPATHLEN];

    /* This function will return the home directory of the user
     * by either using the HOME environment variable or constructing
     * it through the USER environment variable.
     */

    if ((ptr = getenv("HOME")) != NULL)
        (void) strcpy(dest, ptr);
    else
    {
        /* HOME variable is not present so get USER var */
        if ((ptr = getenv("USER")) != NULL)
            pw = getpwnam(ptr);
        else
        {
            /* Obtain user id etc. */
            uid = getuid();
#ifndef VMS
            pw = getpwuid(uid);
#endif /* VMS */
        }

        if (pw)
            (void) strcpy(dest, pw->pw_dir);
        else
            *dest = '\0';
    }

    /* This will be NULL on error or the actual path */
    return dest;
}

#if NeedFunctionPrototypes
int ResizeMainWindow(Display *display, Window window, int width, int height)
#else
int ResizeMainWindow(display, window, width, height)
	Display	*display;
	Window	window;
	int 	width;
 	int 	height;
#endif
{
    XWindowChanges values;
    unsigned int value_mask;
	int screen = XDefaultScreen(display);

    values.width = width;
    values.height = height;
    value_mask = CWWidth | CWHeight;

    if (XReconfigureWMWindow(display, window, screen, value_mask, &values) == 0)
		return False;

	sleepSync(display, 100);
	return True;
}

#if NeedFunctionPrototypes
int ObtainWindowWidthHeight(Display *display, Window window, 
	int *width, int *height)
#else
int ObtainWindowWidthHeight(display, window, width, height)
	Display	*display;
	Window	window;
	int 	*width;
 	int 	*height;
#endif
{
    XWindowAttributes attributes;

    if (XGetWindowAttributes(display, window, &attributes) == 0)
		ErrorMessage("Unable to obtain window attributes.");

    *width = attributes.width;
    *height = attributes.height;
}

#if NeedFunctionPrototypes
int ObtainMousePosition(Display *display, Window window, int *x, int *y)
#else
int ObtainMousePosition(display, window, x, y)
    Display *display;
    Window  window;
    int     *x, *y;
#endif
{
    int rx, ry, x1, y1;
    unsigned int mask;
    Window root, child;

    *x = 0;
    *y = 0;

    /* Obtain the position of the pointer in window */
    if (XQueryPointer(display, window, &root, &child,
        &rx, &ry, &x1, &y1, &mask) == True)
    {
        *x = x1;
        *y = y1;
		return True;
    }

	return False;
}

#if NeedFunctionPrototypes
int YesNoDialogue(Display *display, char *message)
#else
int YesNoDialogue(display, message)
    Display *display;
	char	*message;
#endif
{
    char str[80];

    str[0] = '\0';

    /* Obtain a string from the user. Should contain NULL or numbers */
    strcpy(str, UserInputDialogueMessage(display, message, TEXT_ICON, 
		YES_NO_ENTRY));

    /* Nothing input so just return */
    if (str[0] == '\0') return False;

	DEBUG("YesNoDialogue() got an answer.");

	if (tolower(str[0]) == 'y') return True;
	if (tolower(str[0]) == 'n') return False;

	/* Just in case */
	return False;
}

#if NeedFunctionPrototypes
Pixmap ScalePixmap(Display *display, Window window, Pixmap source, 
	int swidth, int sheight, int dwidth, int dheight)
#else
Pixmap ScalePixmap(display, window, source, swidth, sheight, dwidth, dheight)
	Display *display;
	Window 	window;
	Pixmap 	source;
	int 	swidth, sheight;
	int 	dwidth,dheight;
#endif
{
   	Pixmap 	temp, dest;
   	int 		j, end;
   	float 	i;
   	float 	xscale, yscale;

   	xscale = (float) swidth / (float) dwidth;         /* Scaling factors */
   	yscale = (float) sheight / (float) dheight;

   	dest = XCreatePixmap(display, window, dwidth, dheight, 
		DefaultDepth(display, XDefaultScreen(display)));
   	temp = XCreatePixmap(display, window, dwidth, sheight, 
		DefaultDepth(display, XDefaultScreen(display)));

   	j = 0;
   	end = dwidth * xscale;

   	/* Scale width of source into temp pixmap */
   	for(i = 0; i < end; i += xscale)
	{
      XCopyArea(display, source, temp, gccopy, 
		(int) i, 0, 1, sheight, j++, 0);
	}

   	j = 0;
   	end = dheight * yscale;

   	/* Scale height of temp into dest pixmap */
   	for(i = 0; i < end; i += yscale)
	{
      	XCopyArea(display, temp, dest, gccopy, 
			0, (int) i, dwidth, 1, 0, j++);
	}

   	XFreePixmap(display, temp);

   	return (Pixmap) dest;
}

#if NeedFunctionPrototypes
void Draw4PointCurve(Display *display, Window window, XPoint *p, int num_steps)
#else
void Draw4PointCurve(display, window, p, num_steps)
	Display *display;
	Window 	window;
	XPoint 	*p;
 	int num_steps;
#endif
{
    double t, t_sq, t_cb, incr;
    double r1, r2, r3, r4;
    ushort curve_x, curve_y;
	int ptIndex;
	long maxLines;
	XPoint 	*pts;

	/* 
	 * First check and see if the maximum number of points the X Server can
     * draw is less than the number of steps wanted - most likely to be ok.
     */

	maxLines = (double) (XMaxRequestSize(display) - 3) / 2.0;
	if (num_steps > maxLines)
		num_steps = maxLines;
	
    incr = 1.0 / (double) num_steps;

	if ((pts = (XPoint *) malloc(num_steps * sizeof(XPoint))) == 
		(XPoint *)0)
	{
    	/* Obviously a problem so barf */
    	ShutDown(display, 1, "Unable to malloc memory for Draw4PointCurve().");
	}

	ptIndex = 0;

	pts[ptIndex].x = p[0].x;
	pts[ptIndex].y = p[0].y;

    for (t = incr; t <= 1.01; t += incr)
    {
        t_sq = t * t;
        t_cb = t * t_sq;

        r1 = (1 - 3*t + 3*t_sq -   t_cb) * (double) p[0].x;
        r2 = (    3*t - 6*t_sq + 3*t_cb) * (double) p[1].x;
        r3 = (          3*t_sq - 3*t_cb) * (double) p[2].x;
        r4 = (                     t_cb) * (double) p[3].x;
        curve_x = (ushort) (r1 + r2 + r3 + r4);

        r1 = (1 - 3*t + 3*t_sq -   t_cb) * (double) p[0].y;
        r2 = (    3*t - 6*t_sq + 3*t_cb) * (double) p[1].y;
        r3 = (          3*t_sq - 3*t_cb) * (double) p[2].y;
        r4 = (                     t_cb) * (double) p[3].y;
        curve_y = (ushort) (r1 + r2 + r3 + r4);

		ptIndex++;
		pts[ptIndex].x = curve_x;
		pts[ptIndex].y = curve_y;
    }

	/* Now draw the curve based on a whole heap of lines */
	XDrawLines(display, window, gc, pts, ptIndex, CoordModeOrigin);

	/* Don't forget to free points */
	free(pts);
}

