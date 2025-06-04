/* File: colors.h
 *
 *	Declarations for color functions.
 */

#ifndef COLORS_H
#define COLORS_H

unsigned long get_color(Display *dsp, unsigned short red, unsigned short green, unsigned short blue);
void get_colors(Display *dsp, int count, unsigned short values[][3], unsigned long *table);
unsigned long get_cmap_color(Display *dsp, Colormap cmap,
			     unsigned short red, unsigned short green, unsigned short blue);
void get_cmap_colors(Display *dsp, Colormap cmap, int count, unsigned short values[][3], unsigned long *table);

#endif
