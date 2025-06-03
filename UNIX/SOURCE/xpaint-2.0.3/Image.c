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

#include <X11/Intrinsic.h>
#include "Image.h"
#include "Hash.h"
#include "Palette.h"

#define HASH_SIZE	128

Image *ImageNew(int width, int height)
{
	Image	*image = XtNew(Image);

	image->refCount = 1;
	image->isBW = False;
	image->isGrey = False;
	image->cmapPacked = False;
	image->cmapSize = 0;
	image->cmapData = NULL;
	image->width = width;
	image->height = height;
	if (width == 0 || height == 0)
		image->data = NULL;
	else
		image->data = (unsigned char *)XtMalloc(width * height * sizeof(char) * 3);
	return image;
}

Image *ImageNewCmap(int width, int height, int size)
{
	Image	*image = ImageNew(0, 0);
	int	scale  = sizeof(unsigned short) / sizeof(unsigned char);

	if (size <= 256)
		scale = 1;

	image->width    = width;
	image->height   = height;
	image->data     = (unsigned char *)XtMalloc(width * height * sizeof(char) * scale);
	image->cmapData = (unsigned char *)XtMalloc(size * sizeof(char) * 3);
	image->cmapSize = size;

	return image;
}

Image *ImageNewBW(int width, int height)
{
	Image	*image = ImageNewCmap(width, height, 2);

	image->cmapData[0] = 0;
	image->cmapData[1] = 0;
	image->cmapData[2] = 0;
	image->cmapData[3] = 255;
	image->cmapData[4] = 255;
	image->cmapData[5] = 255;

	return image;
}

Image *ImageNewGrey(int width, int height)
{
	int	i;
	Image	*image = ImageNewCmap(width, height, 256);

	for (i = 0; i < image->cmapSize; i++) {
		image->cmapData[i * 3 + 0] = i;
		image->cmapData[i * 3 + 1] = i;
		image->cmapData[i * 3 + 2] = i;
	}

	return image;
}


void ImageDelete(Image *image)
{
	image->refCount--;
	if (image->refCount > 0)
		return;

	if (image->cmapSize > 0 && image->cmapData != NULL)
		XtFree((XtPointer)image->cmapData);
	if (image->data != NULL)
		XtFree((XtPointer)image->data);
	XtFree((XtPointer)image);
}

/*
**  Convert a colormap image into a RGB image
**    useful for writers which only deal with RGB and not
**    colormaps
*/
Image *ImageToRGB(Image *image)
{
	unsigned char	*ip, *op;
	Image	*out;
	int	x, y;

	if (image->cmapSize == 0) {
		image->refCount++;
		return image;
	}
	 
	out = ImageNew(image->width, image->height);
	op = image->data;

	for (y = 0; y < image->height; y++) { 
		for (x = 0; x < image->width; x++) { 
			ImagePixel(image, x, y, ip);
			*op++ = *ip++;
			*op++ = *ip++;
			*op++ = *ip++;
		}
	}

	out->isBW   = image->isBW;
	out->isGrey = image->isGrey;
	out->width  = image->width;
	out->height = image->height;

	image->cmapPacked = False;

	return out;
}

/*
**  Create a nice image for writing routines.
*/

static int  writeCMP(XColor *a, XColor *b)
{
	return a->pixel - b->pixel;
}

Image *PixmapToImage(Widget w, Pixmap pix, Colormap cmap)
{
	XImage		*xim;
	Image		*image;
        Window          root;
	Display		*dpy = XtDisplay(w);
        int             x, y;
        unsigned int    width, height, bw, depth;
	unsigned char	*ptr;
	int		format = 0;
	void		*htable;

        XGetGeometry(dpy, pix, &root, &x, &y, &width, &height, &bw, &depth);
	xim = XGetImage(dpy, pix, 0, 0, width, height, ~0, ZPixmap);

	image = ImageNew(width, height);
	ptr = image->data;

	htable = HashCreate(writeCMP, NULL, HASH_SIZE);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			XColor		c, *col;
			Pixel		pixel = XGetPixel(xim, x, y);
			unsigned char	r, g, b;

			c.pixel = pixel;
			if ((col = HashFind(htable, pixel % HASH_SIZE, &c)) == NULL) {
				col = XtNew(XColor);
				col->flags = DoRed|DoGreen|DoBlue;
				col->pixel = pixel;
				XQueryColor(dpy, cmap, col);
				HashAdd(htable, pixel % HASH_SIZE, col);
			}

			*ptr++ = r = col->red >> 8;
			*ptr++ = g = col->green >> 8;
			*ptr++ = b = col->blue >> 8;

			if (format == 0) {
				if (r != g || g != b)
					format = 2;
				else if (r != 0 || r != 255)
					format = 1;
			} else if (format == 1) {
				if (r != g || g != b)
					format = 2;
			}
		}
	}

	/*
	**  Check to see if we just created a B&W or Grey scale image?
	*/
	if (format == 0 || format == 1) {
		int		inc, v = 0;
		unsigned char	*cp, *ip;

		if (format == 0) {
			image->cmapSize = 2;
			image->cmapData = (unsigned char *)XtCalloc(sizeof(char) * 3, 2);
		} else {
			image->cmapSize = 256;
			image->cmapData = (unsigned char *)XtCalloc(sizeof(char) * 3, 256);
		}

		/*
		**  Create the colormap
		*/
		inc = 255 / (image->cmapSize - 1);
		for (y = 0; y < image->cmapSize; v += inc, y++) {
			image->cmapData[y * 3 + 0] = v;
			image->cmapData[y * 3 + 1] = v;
			image->cmapData[y * 3 + 2] = v;
		}
	
		cp = image->data;
		ip = image->data;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++, ip += 3, cp++) {
				*cp = *ip;
				if (format == 0 && *cp == 255)
					*cp = 1;
			}
		}
	}

	if (htable != NULL)
		HashDestroy(htable);
	XDestroyImage(xim);

	return image;
}

/*
**  Compress an image into a nice number of
**   colors for display purposes.
*/
static Image *quantizeColormap(Image *input, Palette *map, Boolean flag)
{
	Image		*output;
	unsigned char	*op;
	int		x, y;
	int		ncol;

	/*
	**  If the output is either B&W or grey do something
	**   fast an easy.
	*/
	if (!map->isGrey) 
		return ImageCompress(input, map->ncolors);
	
	ncol = map->ncolors > 256 ? 256 : map->ncolors;
	output = ImageNewCmap(input->width, input->height, ncol); 

	op = output->data;

	for (y = 0; y < ncol; y++) {
		unsigned char	v = ((float)y / 256.0) * (float)ncol;
		ImageSetCmap(output, y, v, v, v);
	}

	for (y = 0; y < input->height; y++) {
		for (x = 0; x < input->width; x++, op++) {
			unsigned char	*dp, v;

			ImagePixel(input, x, y, dp);
			v = (dp[0]*11 + dp[1]*16 + dp[2]*5) >> 5;  /* pp=.33R+.5G+.17B */
			*op = (int)(((float)v / 256.0) * (float)ncol);
		}
	}

	ImageDelete(input);
	return output;
}

static void compressColormap(Image *image)
{
	unsigned short	used[32768];
	int		size = image->width * image->height;
	int		i, count, newSize;
	unsigned char	*newMap;
	unsigned short	*isp;
	unsigned char	*icp;

	if (image->cmapSize <= 2 || image->cmapPacked)
		return;

	memset(used, False, sizeof(used));
	
	/*
	**  Find out usage information over the colormap 
	*/
	if (image->cmapSize > 256) {
		isp = (unsigned short *)image->data;
		for (i = 0; i < size; i++, isp++)
			used[*isp] = True;
	} else {
		icp = image->data;
		for (i = 0; i < size; i++, icp++)
			used[*icp] = True;
	}

	/*
	**  Count number of colors really used.
	*/
	for (count = i = 0; i < image->cmapSize; i++) {
		if (used[i])
			count++;
	}

	/*
	**  Colormap is fully used.
	*/
	if (count == image->cmapSize) {
		image->cmapPacked = True;
		return;
	}

	/*
	**  Now build the remapping colormap, and
	**    set the index.
	*/
	newSize = count;
	newMap = (unsigned char *)XtCalloc(count, sizeof(unsigned char) * 3);
	for (count = i = 0; i < image->cmapSize; i++) {
		if (!used[i])
			continue;
		newMap[count * 3 + 0] = image->cmapData[i * 3 + 0];
		newMap[count * 3 + 1] = image->cmapData[i * 3 + 1];
		newMap[count * 3 + 2] = image->cmapData[i * 3 + 2];
		used[i] = count++;
	}

	if (image->cmapSize > 256 && newSize > 256) {
		isp = (unsigned short *)image->data;
		for (i = 0; i < size; i++, isp++)
			*isp = used[*isp];
	} else if (image->cmapSize > 256 && newSize <= 256) {
		/*
		**  Map a big colormap down to a small one.
		*/
		isp = (unsigned short *)image->data;
		icp = image->data;
		for (i = 0; i < size; i++, icp++, isp++)
			*icp = used[*isp];

		image->data = (unsigned char *)XtRealloc((XtPointer)image->data, size * sizeof(unsigned char));
	} else {
		icp = image->data;
		for (i = 0; i < size; i++, icp++)
			*icp = used[*icp];
	}

	XtFree((XtPointer)image->cmapData);
	image->cmapData = newMap;
	image->cmapSize = newSize;
	image->cmapPacked = True;
	image->isGrey = False;
}

/*
**  Convert an imput image into a nice pixmap 
**   so we can edit it.
*/
Boolean ImageToPixmap(Image *image, Widget w, Pixmap *pix, Colormap *cmap)
{
	GC		gc;
	Display		*dpy = XtDisplay(w);
	Palette         *map;
	int             depth = DefaultDepthOfScreen(XtScreen(w));
	XImage		*xim;
	int		x, y;
	int		width = image->width, height = image->height;
	char		*data;

        if ((*pix = XCreatePixmap(dpy, XtWindow(w), image->width, image->height, depth)) == None)
		return False;

	if ((data = (char*)XtMalloc(image->width * image->height * sizeof(char) * 4)) == NULL) {
		XFreePixmap(dpy, *pix);
		return False;
	}

	if ((xim = XCreateImage(dpy, NULL, depth, ZPixmap, 0, data, image->width, image->height, 8, 0)) == NULL) {
		XFreePixmap(dpy, *pix);
		XtFree(data);
		return False;
	}

	map   = PaletteCreate(w);
	*cmap = map->cmap;

	if ((image->cmapSize > map->ncolors) ||
	    (image->cmapSize == 0 && map->isMapped))
		image = quantizeColormap(image, map, False);

	if (image->cmapSize > 0)
		compressColormap(image);

	if (image->cmapSize > 0) {
		unsigned short 	*sdp = (unsigned short *)image->data;
		unsigned char 	*cdp = (unsigned char  *)image->data;
		Pixel		*list = (Pixel*)XtCalloc(sizeof(Pixel), image->cmapSize);
		XColor		*xcol = (XColor*)XtCalloc(sizeof(XColor), image->cmapSize);

		for (y = 0; y < image->cmapSize; y++) {
			xcol[y].red   = image->cmapData[y * 3 + 0] << 8;
			xcol[y].green = image->cmapData[y * 3 + 1] << 8;
			xcol[y].blue  = image->cmapData[y * 3 + 2] << 8;
		}

		PaletteAllocN(map, xcol, image->cmapSize, list);

		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++, sdp++, cdp++) 
				XPutPixel(xim, x, y, list[image->cmapSize > 256 ? *sdp : *cdp]);
		
		XtFree((XtPointer)list);
		XtFree((XtPointer)xcol);
	} else {
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				unsigned char	*cp;
				XColor		c;

				ImagePixel(image, x, y, cp);

				c.red   = cp[0] << 8;
				c.green = cp[1] << 8;
				c.blue  = cp[2] << 8;

				XPutPixel(xim, x, y, PaletteAlloc(map, &c));
			}
		}
	}

	gc = XtGetGC(w, 0, NULL);
	XPutImage(dpy, *pix, gc, xim, 0, 0, 0, 0, width, height);
	XtReleaseGC(w, gc);

	XDestroyImage(xim);

	return True;
}
