/* File: colors.c
 *
 *	Color functions.
 */

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include "colors.h"


unsigned long get_color(Display *dsp, unsigned short red, unsigned short green, unsigned short blue)
{
	XColor	color;

	color.red = red;
	color.green = green;
	color.blue = blue;
	color.flags = DoRed | DoGreen | DoBlue;

	if (!XAllocColor(dsp, XDefaultColormap(dsp, XDefaultScreen(dsp)), &color)) {
		fprintf(stderr, "ERROR:  cannot allocate color\n");
		return 0;
	}

	return color.pixel;
}


void get_colors(Display *dsp, int count, unsigned short values[][3], unsigned long *table)
{
	int	i;

	for (i = 0; i < count; i++) {
		table[i] = get_color(dsp, values[i][0], values[i][1], values[i][2]);
	}
}


unsigned long get_cmap_color(Display *dsp, Colormap cmap,
			     unsigned short red, unsigned short green, unsigned short blue)
{
	XColor	color;

	color.red = red;
	color.green = green;
	color.blue = blue;
	color.flags = DoRed | DoGreen | DoBlue;

	if (!XAllocColor(dsp, cmap, &color)) {
		fprintf(stderr, "ERROR:  cannot allocate color\n");
		return 0;
	}

	return color.pixel;
}


void get_cmap_colors(Display *dsp, Colormap cmap, int count, unsigned short values[][3], unsigned long *table)
{
	int	i;

	for (i = 0; i < count; i++) {
		table[i] = get_cmap_color(dsp, cmap, values[i][0], values[i][1], values[i][2]);
	}
}
