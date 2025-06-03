
/*
 * I. ARIT 1992 Hidirbeyli,AYDIN,TR.  09400 
 *              Golden,    CO,   USA. 80401 
 *
 *
 * Copyright (C) 1992 Ismail ARIT 
 *
 * This file  is distributed in the hope that it will be useful, but without any
 * warranty.  No author or distributor accepts responsibility to anyone for
 * the consequences of using it or for whether it serves any particular
 * purpose or works at all. 
 *
 *
 * Everyone is granted permission to copy, modify and redistribute this file
 * under the following conditions: 
 *
 * Permission is granted to anyone to make or distribute copies of the source
 * code, either as received or modified, in any medium, provided that all
 * copyright notices, permission and nonwarranty notices are preserved, and
 * that the distributor grants the recipient permission for further
 * redistribution as permitted by this document. 
 *
 * No part of this program can be used in any commercial product. 
 */


#include  <X11/Xlib.h>
#include  <X11/Xutil.h>


extern Display *disp;
extern Colormap Cmap;
extern GC       gc;


unsigned long   Pixels[66];

char           *ColorNames[] =
{
 "Black", "Thistle", "SeaGreen", "PaleGreen",
 "YellowGreen", "SkyBlue", "VioletRed",
 "MediumVioletRed", "Red", "ForestGreen", "Blue", "Green",
 "Khaki", "LightBlue", "Pink", "GreenYellow", "MediumSpringGreen",
 "MediumTurquoise", "SkyBlue", "DarkTurquoise", "SlateBlue",
 "VioletRed", "OrangeRed", "Red", "FireBrick", "Maroon", "Magenta",
 "DarkSlateBlue", "LimeGreen", "Magenta", "Maroon", "MidnightBlue",
 "Navy", "Orange", "Orchid", "Pink", "SkyBlue", "SpringGreen",
 "SteelBlue", "Tan", "Thistle", "Turquoise", "Violet", "Wheat", "White",
 "Yellow"
};

setcolor_with_name(fg_name, bg_name)
	char            fg_name[];
char            bg_name[];
{
	int             i = 0;

	/*
	 * while((strcmp(fg_name, ColorNames[i] ) != 0) && ( i < 66)) { i++
	 * ;} if ( i < 66 ) { XSetForeground( disp, gc,Pixels[i] ); } i=0;
	 * while((strcmp(bg_name,ColorNames[i] ) != 0) && (i < 66)) {i++;}
	 * if( i < 66 ) { XSetBackground(disp,gc,Pixels[i] ); } 
	 */

}

int
get_color(name)
	char           *name;
{
	XColor          exact, color;

	XAllocNamedColor(disp, Cmap, name, &color, &exact);
	return (color.pixel);
}


setcolor(int color)
{
	XSetForeground(disp, gc, color);
};

setbkcolor(int color)
{
	XSetBackground(disp, gc, color);
};


init_colors()
{
	XColor          RGBColor, HardwareColor;

	int             Stat, ii;
	/* I was using this for another project, don't need this for now */

	/*
	 * if ( DefaultDepth(disp) > 1) { for (ii = 0; ii < 66; ii++ ) { Stat
	 * = XLookupColor( disp, Cmap, ColorNames[ii],
	 * &RGBColor,&HardwareColor ); if( Stat  != 0) {  Stat = XAllocColor(
	 * disp, Cmap,&HardwareColor); 
	 *
	 * if (Stat != 0) Pixels[ii] = HardwareColor.pixel; else Pixels[ii] =
	 * BlackPixel(disp,DefaultScreen(disp)); } } } else  { for( ii =0; ii
	 * < 66; ii++) { if( strcmp( "white", ColorNames[ii] ) == 0) Pixels
	 * [ii] = WhitePixel(disp,DefaultScreen(disp)); else Pixels [ ii ] =
	 * BlackPixel(disp,DefaultScreen(disp)); } } 
	 *
	 */

}
