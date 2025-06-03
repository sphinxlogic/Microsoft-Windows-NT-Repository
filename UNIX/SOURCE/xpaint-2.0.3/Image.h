/* +-------------------------------------------------------------------+ */
/* | Copyright (C) 1993, David Koblas (koblas@netcom.com)              | */
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

typedef struct {
	int	refCount;	/* reference count */

	/*
	**  Special notes:
	**    if the image isBW then there will be a two entry
	**       colormap BLACK == 0, WHITE == 1
	**    if the image isGrey, then the colormap is 256 entries
	**       BLACK == 0 .. WHITE == 255
	*/
	int	isGrey, isBW;	/* simple indicator flags  GreyScale, Black & White */
	/*
	**  Colormap entries
	**   rgb rgb rgb [1..size]
	*/
	int		cmapPacked;	/* Boolean, is the colormap packed 
					   down to just the used colors */
	int		cmapSize;	/* number of colors in colormap == 0 if no colormap */
	unsigned char	*cmapData;
	/*
	**  Image data
	**   either rgb rgb rgb
	**   or     idx idx idx
	**
	**   if image has colormap, and the size > 256, then
	**     data is pointers to unsigned shorts.
	*/
	int		width, height;	/* width, height of image */
	unsigned char	*data;		
} Image;

#define ImagePixel(image, x, y, p)	do {							\
			if (image->cmapSize > 0) {						\
				if (image->cmapSize > 256) {					\
					unsigned short	*__sp = (unsigned short *)image->data;		\
					p = &image->cmapData[__sp[y * image->width + x] * 3];	\
				} else {							\
					unsigned char	*__cp = image->data;			\
					p = &image->cmapData[__cp[y * image->width + x] * 3];	\
				}								\
			} else {								\
				p = &image->data[(y * image->width + x) * 3];			\
			}									\
		} while (0)

#define ImageSetCmap(image, index, r, g, b) do {		\
			image->cmapData[(index) * 3 + 0] = r;	\
			image->cmapData[(index) * 3 + 1] = g;	\
			image->cmapData[(index) * 3 + 2] = b;	\
		} while (0)

Image	*ImageNew(int, int);
Image	*ImageNewGrey(int, int);
Image	*ImageNewBW(int, int);
Image	*ImageNewCmap(int, int, int);
Image	*ImageCompress(Image *, int);
void	ImageDelete(Image *);
#ifdef _XtIntrinsic_h
Image	*PixmapToImage(Widget, Pixmap, Colormap);
Boolean	ImageToPixmap(Image *, Widget, Pixmap*, Colormap*);
#endif
