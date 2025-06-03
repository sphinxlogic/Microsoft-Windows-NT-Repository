/* +-------------------------------------------------------------------+ */
/* | Copyright 1993, David Koblas (koblas@netcom.com)                  | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

/* 
**
** Copyright (C) 1989, 1991 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/


#include "Image.h"
#include <stdio.h>

void *calloc(int, int);

#define True	1
#define False	0

#define LSBFirst	0
#define MSBFirst	1

#define XYBitmap	0
#define XYPixmap	1
#define ZPixmap		2

#define StaticGray	0
#define GrayScale	1
#define StaticColor	2
#define PseudoColor	3
#define TrueColor	4
#define DirectColor	5

typedef unsigned long xwdval;
#define X11WD_FILE_VERSION 7
typedef struct {
    xwdval header_size;		/* Size of the entire file header (bytes). */
    xwdval file_version;	/* X11WD_FILE_VERSION */
    xwdval pixmap_format;	/* Pixmap format */
    xwdval pixmap_depth;	/* Pixmap depth */
    xwdval pixmap_width;	/* Pixmap width */
    xwdval pixmap_height;	/* Pixmap height */
    xwdval xoffset;		/* Bitmap x offset */
    xwdval byte_order;		/* MSBFirst, LSBFirst */
    xwdval bitmap_unit;		/* Bitmap unit */
    xwdval bitmap_bit_order;	/* MSBFirst, LSBFirst */
    xwdval bitmap_pad;		/* Bitmap scanline pad */
    xwdval bits_per_pixel;	/* Bits per pixel */
    xwdval bytes_per_line;	/* Bytes per scanline */
    xwdval visual_class;	/* Class of colormap */
    xwdval red_mask;		/* Z red mask */
    xwdval green_mask;		/* Z green mask */
    xwdval blue_mask;		/* Z blue mask */
    xwdval bits_per_rgb;	/* Log base 2 of distinct color values */
    xwdval colormap_entries;	/* Number of entries in colormap */
    xwdval ncolors;		/* Number of Color structures */
    xwdval window_width;	/* Window width */
    xwdval window_height;	/* Window height */
    long window_x;		/* Window upper left X coordinate */
    long window_y;		/* Window upper left Y coordinate */
    xwdval window_bdrwidth;	/* Window border width */
} X11WDFileHeader;

typedef struct {
	unsigned long	num;
	unsigned short	red, green, blue;
	char		flags;			/* do_red, do_green, do_blue */
	char		pad;
} X11XColor;


/*
**  Some read and write routines for long values
*/

static unsigned long readlong(FILE *fp)
{
	unsigned char	a, b, c, d;

	a = getc(fp);
	b = getc(fp);
	c = getc(fp);
	d = getc(fp);

	return ((a & 0xff) << 24) |
	       ((b & 0xff) << 16) |
	       ((c & 0xff) <<  8) |
	        (d & 0xff);
}
static unsigned short readshort(FILE *fp)
{
	unsigned char	a, b;
	a = getc(fp);
	b = getc(fp);

	return ((a & 0xff) << 8) | (b & 0xff);
}

static void writelong(FILE *fp, long l)
{
	putc( (l >> 24) & 0xff, fp);
	putc( (l >> 16) & 0xff, fp);
	putc( (l >>  8) & 0xff, fp);
	putc(  l        & 0xff, fp);
}
static void writeshort(FILE *fp, unsigned short s)
{
	putc( (s >>  8) & 0xff, fp);
	putc(  s        & 0xff, fp);
}

static long swaplong(long l)
{
	unsigned char	b1, b2, b3, b4;

	b1 = (l >> 24) & 0xff;
	b2 = (l >> 16) & 0xff;
	b3 = (l >>  8) & 0xff;
	b4 =  l        & 0xff;

	return ((long)b4 << 24) | ((long)b3 << 16) | (b2 << 8) | b1;
}
static short swapshort(short l)
{
	unsigned char	b1, b2;

	b1 = (l >>  8) & 0xff;
	b2 =  l        & 0xff;

	return (b2 << 8) | b1;
}

/*
**
*/
static int 		bits_per_item, bits_used, bit_shift, bits_per_pixel;
static unsigned long 	pixel_mask;
static int 		byte_order, bit_order;


static unsigned long getpixnum(FILE *fp)
{
	static unsigned char	b8;
	static unsigned short	b16;
	static unsigned long	b32;
	unsigned long 		val;

	if (bits_used == bits_per_item ) {
		switch (bits_per_item) {
		case 8:
			b8 = getc(fp);
			break;
		case 16:
			b16 = readshort(fp);
			if (byte_order != MSBFirst)
				b16 = swapshort(b16);
			break;
		case 32:
			b32 = readlong(fp);
			if (byte_order != MSBFirst)
				b32 = swaplong(b32);
			break;
		}
		bits_used = 0;

		if (bit_order == MSBFirst)
			bit_shift = bits_per_item - bits_per_pixel;
		else
			bit_shift = 0;
	}

	switch (bits_per_item) {
	case 8:
		val = (b8 >> bit_shift) & pixel_mask;
		break;
	case 16:
		val = (b16 >> bit_shift) & pixel_mask;
		break;
	case 32:
		val = (b32 >> bit_shift) & pixel_mask;
		break;
	}

	if (bit_order == MSBFirst)
		bit_shift -= bits_per_pixel;
	else
		bit_shift += bits_per_pixel;
	bits_used += bits_per_pixel;

	return val;
}



/*
**  Straight forward writer.
**   Take advantage of the fact that a Grey image is 8 bits deep
**   and some simple properties of colormap images for simplicity.
*/
int WriteXWD(char *file, Image *image)
{
	static char	*name = "XPaint Image";
	FILE		*fp;
	X11WDFileHeader	h11;
	X11XColor	color;
	int		i, x, y;

	if ((fp = fopen(file, "w")) == NULL)
		return False;

	/* 
	**  Set up the header.
	*/
	h11.header_size = sizeof(h11) + strlen(name) + 1;
	h11.file_version = X11WD_FILE_VERSION;
	h11.pixmap_format = ZPixmap;
	h11.pixmap_width = image->width;
	h11.pixmap_height = image->height;
	h11.xoffset = 0;
	h11.byte_order = MSBFirst;
	h11.bitmap_bit_order = MSBFirst;
	h11.window_width = image->width;
	h11.window_height = image->height;
	h11.window_x = 0;
	h11.window_y = 0;
	h11.window_bdrwidth = 0;

	if (image->isBW) {
		h11.pixmap_depth = 1;
		h11.bits_per_pixel = 1;
		h11.colormap_entries = image->cmapSize;
		h11.ncolors = image->cmapSize;
		h11.bytes_per_line = ( image->width + 7 ) / 8;

		/*
		**  Common between grey and BW
		*/
		h11.bitmap_unit = 8;
		h11.bitmap_pad = 8;
		h11.visual_class = StaticGray;
		h11.red_mask = 0;
		h11.green_mask = 0;
		h11.blue_mask = 0;
	} else if (image->isGrey) {
		h11.pixmap_depth = 8;
		h11.bits_per_pixel = 8;
		h11.colormap_entries = image->cmapSize;
		h11.ncolors = image->cmapSize;
		h11.bytes_per_line = image->width;

		/*
		**  Common between grey and BW
		*/
		h11.bitmap_unit = 8;
		h11.bitmap_pad = 8;
		h11.visual_class = StaticGray;
		h11.red_mask = 0;
		h11.green_mask = 0;
		h11.blue_mask = 0;
	} else if (image->cmapSize > 0) {
		/*
		**  Color mapped image
		*/
		h11.pixmap_depth = (image->cmapSize > 256) ? 16 : 8;
		h11.bits_per_pixel = (image->cmapSize > 256) ? 16 : 8;
		h11.visual_class = PseudoColor;
		h11.colormap_entries = image->cmapSize;
		h11.ncolors = image->cmapSize;
		h11.red_mask = 0;
		h11.green_mask = 0;
		h11.blue_mask = 0;
		h11.bytes_per_line = image->width;
		h11.bitmap_unit = 8;
		h11.bitmap_pad = 8;
	} else {
		/*
		**  RGB Image
		*/
		h11.pixmap_depth = 24;
		h11.bitmap_unit = 32;
		h11.bitmap_pad = 32;
		h11.bits_per_pixel = 32;
		h11.visual_class = DirectColor;
		h11.colormap_entries = 256;
		h11.ncolors = 0;
		h11.red_mask = 0xff0000;
		h11.green_mask = 0xff00;
		h11.blue_mask = 0xff;
		h11.bytes_per_line = image->width * 4;
	}
	h11.bits_per_rgb = h11.pixmap_depth;

	/* Write out the header in big-endian order. */
	writelong(fp, h11.header_size );
	writelong(fp, h11.file_version );
	writelong(fp, h11.pixmap_format );
	writelong(fp, h11.pixmap_depth );
	writelong(fp, h11.pixmap_width );
	writelong(fp, h11.pixmap_height );
	writelong(fp, h11.xoffset );
	writelong(fp, h11.byte_order );
	writelong(fp, h11.bitmap_unit );
	writelong(fp, h11.bitmap_bit_order );
	writelong(fp, h11.bitmap_pad );
	writelong(fp, h11.bits_per_pixel );
	writelong(fp, h11.bytes_per_line );
	writelong(fp, h11.visual_class );
	writelong(fp, h11.red_mask );
	writelong(fp, h11.green_mask );
	writelong(fp, h11.blue_mask );
	writelong(fp, h11.bits_per_rgb );
	writelong(fp, h11.colormap_entries );
	writelong(fp, h11.ncolors );
	writelong(fp, h11.window_width );
	writelong(fp, h11.window_height );
	writelong(fp, h11.window_x );
	writelong(fp, h11.window_y );
	writelong(fp, h11.window_bdrwidth );

	/* 
	**  Write out the dump name.
	*/
	fwrite(name, 1, strlen(name) + 1, fp);

	/* 
	**   Write out the colormap, big-endian order.
	*/
	color.flags = 7;
	color.pad = 0;
	for ( i = 0; i < image->cmapSize; ++i ) {
		color.num = i;

		if (image->isBW) {
			color.red   = (1 - i) * 65535L;
			color.green = color.red;
			color.blue  = color.red;
		} else if (image->isGrey) {
			color.red   = i << 8;
			color.green = color.red;
			color.blue  = color.red;
		} else {
			color.red   = image->cmapData[i * 3 + 0] << 8;
			color.green = image->cmapData[i * 3 + 1] << 8;
			color.blue  = image->cmapData[i * 3 + 2] << 8;
		}
		writelong( fp, color.num );
		writeshort( fp, color.red );
		writeshort( fp, color.green );
		writeshort( fp, color.blue );
		(void)putc( color.flags, fp );
		(void)putc( color.pad, fp );
	}

	/* 
	** Finally, write out the data.
	*/
	for (y = 0; y < image->height; y++) {
		if (image->cmapSize == 0) {
			/*
			**  Write RGB Image
			*/
			unsigned char	*dp = &image->data[y * image->width * 3];
			for (x = 0; x < image->width; x++) {
				putc(0, fp);
				putc(*dp, fp); dp++;
				putc(*dp, fp); dp++;
				putc(*dp, fp); dp++;
			}
		} else if (image->isBW) {
			/*
			**  Write a B&W Image, which is 1 bit per pixel.
			*/
			unsigned char	*dp = &image->data[y * image->width];
			int		v = 0, idx = 7;
			for (x = 0; x < image->width; x++, dp++) {
				v |= *dp << idx;
				if (--idx < 0) {
					idx = 7;
					putc(v & 0xff, fp);
				}
			}
			if (idx != 7) 
				putc(v & 0xff, fp);
		} else if (image->cmapSize > 256) {
			/*
			**  Write wide colormapped image
			*/
			unsigned short	*dp = &((unsigned short *)image->data)[y * image->width];
			for (x = 0; x < image->width; x++, dp++)
				writeshort(fp, *dp);
		} else {
			/*
			**  Write normal colormapped image, which is also a Grey image
			*/
			unsigned char	*dp = &image->data[y * image->width];
			for (x = 0; x < image->width; x++, dp++)
				putc(*dp, fp);
		}
	}

	fclose(fp);

	return True;
}

int TestXWD(char *file)
{
	FILE	*fd = fopen(file, "r");
	long	v;

	if (fd == NULL)
		return False;

	readlong(fd);
	v = readlong(fd);

	fclose(fd);

	return v == X11WD_FILE_VERSION;
}

Image *ReadXWD(char *file)
{
	int		isGrey = True, isBW = False;
	FILE		*fp;
	X11WDFileHeader	h11;
	Image		*image = NULL;
	unsigned char	*ibp;
	unsigned short	*isp;
	int		i, x, y;
	unsigned long 	red_mask, green_mask, blue_mask;
	int		byte_swap = False;
	int		scale, padright;

	if ((fp = fopen(file, "r")) == NULL)
		return NULL;

	if (fread(&h11, sizeof(h11), 1, fp) != 1) {
		fclose(fp);
		return NULL;
	}

	if (swaplong(h11.file_version) == X11WD_FILE_VERSION) {
		byte_swap = True;
		h11.header_size = swaplong( h11.header_size );
		h11.file_version = swaplong( h11.file_version );
		h11.pixmap_format = swaplong( h11.pixmap_format );
		h11.pixmap_depth = swaplong( h11.pixmap_depth );
		h11.pixmap_width = swaplong( h11.pixmap_width );
		h11.pixmap_height = swaplong( h11.pixmap_height );
		h11.xoffset = swaplong( h11.xoffset );
		h11.byte_order = swaplong( h11.byte_order );
		h11.bitmap_unit = swaplong( h11.bitmap_unit );
		h11.bitmap_bit_order = swaplong( h11.bitmap_bit_order );
		h11.bitmap_pad = swaplong( h11.bitmap_pad );
		h11.bits_per_pixel = swaplong( h11.bits_per_pixel );
		h11.bytes_per_line = swaplong( h11.bytes_per_line );
		h11.visual_class = swaplong( h11.visual_class );
		h11.red_mask = swaplong( h11.red_mask );
		h11.green_mask = swaplong( h11.green_mask );
		h11.blue_mask = swaplong( h11.blue_mask );
		h11.bits_per_rgb = swaplong( h11.bits_per_rgb );
		h11.colormap_entries = swaplong( h11.colormap_entries );
		h11.ncolors = swaplong( h11.ncolors );
		h11.window_width = swaplong( h11.window_width );
		h11.window_height = swaplong( h11.window_height );
		h11.window_x = swaplong( h11.window_x );
		h11.window_y = swaplong( h11.window_y );
		h11.window_bdrwidth = swaplong( h11.window_bdrwidth );
	} else if (h11.file_version != X11WD_FILE_VERSION) {
		goto out;
	}

	/*
	**  Skip stored name
	*/
	for (i = 0; i < h11.header_size - sizeof(h11); ++i)
		if (getc(fp) == EOF) 
			goto out;

	/* Check whether we can handle this dump. */
	if (h11.pixmap_depth > 24)
		goto out;
	if (h11.bits_per_rgb > 24) 
		goto out;
	if (h11.pixmap_format != ZPixmap && h11.pixmap_depth != 1)
		goto out;
	if (h11.bitmap_unit != 8 && h11.bitmap_unit != 16 && h11.bitmap_unit != 32)
		goto out;

	if (h11.ncolors > 0) {
		X11XColor	*colors;

		if ((colors = (X11XColor*)calloc(sizeof(X11XColor), h11.ncolors)) == NULL)
			goto out;
		if (fread(colors, sizeof(X11XColor), h11.ncolors, fp) != h11.ncolors)
			goto out;

		image = ImageNewCmap(h11.pixmap_width, h11.pixmap_height, h11.ncolors);

		for (i = 0; i < h11.ncolors; i++) {
			unsigned char	r, g, b;
			if (byte_swap) {
				colors[i].red = swapshort(colors[i].red);
				colors[i].green = swapshort(colors[i].green);
				colors[i].blue = swapshort(colors[i].blue);
			}
			r = colors[i].red >> 8;
			g = colors[i].green >> 8;
			b = colors[i].blue >> 8;
			ImageSetCmap(image, i, r, g, b);

			if (r != g || g != b)
				isGrey = False;
		}

		if (isGrey && image->cmapData[0] == 0 && image->cmapData[3] == 255)
				isBW = True;
	} else {
		/*
		**  RGB Image
		*/
		isGrey = False;
	}

	if (h11.visual_class == TrueColor || h11.visual_class == DirectColor) {
	} else if (h11.visual_class == StaticGray && h11.bits_per_pixel == 1) {
		/*
		**  B&W bitmap
		*/
	} else if (h11.visual_class == StaticGray) {
	}

	switch (h11.visual_class) {
	case TrueColor:
	case DirectColor:
		image = ImageNew(h11.pixmap_width, h11.pixmap_height);
		if (h11.bits_per_pixel == 16)
			scale = 4;
		else
			scale = 1;
		break;
	case StaticGray:
		if (image != NULL)
			break;
		if (h11.bits_per_pixel == 1) {
			image = ImageNewBW(h11.pixmap_width, h11.pixmap_height);
		} else {
			image = ImageNewGrey(h11.pixmap_width, h11.pixmap_height);
			scale = 256 / (1 << h11.bits_per_pixel) - 1;
		}
		break;
	default:	/* StaticColor, PseudoColor, GrayScale */
		if (image != NULL)
			break;
		image = ImageNew(h11.pixmap_width, h11.pixmap_height);
		break;
	}

	padright = h11.bytes_per_line * 8 / h11.bits_per_pixel - h11.pixmap_width;

	bits_per_item = h11.bitmap_unit;
	bits_used     = bits_per_item;
	bits_per_pixel= h11.bits_per_pixel;
	byte_order    = h11.byte_order;
	bit_order     = h11.bitmap_bit_order;
	if ( bits_per_pixel == sizeof(pixel_mask) * 8 )
		pixel_mask = -1;
	else
		pixel_mask = ( 1 << bits_per_pixel ) - 1;
        red_mask   = h11.red_mask;
        green_mask = h11.green_mask;
        blue_mask  = h11.blue_mask;

	ibp = image->data;
	isp = (unsigned short *)image->data;
	for (y = 0; y < image->height; y++) {
		switch (h11.visual_class) {
		case StaticGray:
		case GrayScale:
		case StaticColor:
		case PseudoColor:
			for (x = 0; x < image->width; x++) {
				if (image->cmapSize > 256)
					*isp++ = getpixnum(fp);
				else if (image->isBW == 2)
					*ibp++ = (getpixnum(fp) == 1) ? 0 : 1;
				else
					*ibp++ = getpixnum(fp);
			}
			break;
		case TrueColor:
		case DirectColor:
			for (x = 0; x < image->width; x++) {
				unsigned long	 ul = getpixnum(fp);
				switch (h11.bits_per_pixel) {
				case 16:
					*ibp++ = (ul & red_mask)   >> 0;
					*ibp++ = (ul & green_mask) >> 5;
					*ibp++ = (ul & blue_mask)  >> 10;
					break;
				case 24:
				case 32:
					*ibp++ = (ul & 0xff0000) >> 16;
					*ibp++ = (ul & 0x00ff00) >>  8;
					*ibp++ = (ul & 0x0000ff);
					break;
				}
			}
		}

		for (x = 0; x < padright; x++)
			getpixnum(fp);
	}

out:
	fclose(fp);
	return image;
}
