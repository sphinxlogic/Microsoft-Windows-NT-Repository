/* image.h : image display routines
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#ifndef _IMAGE_H__INCLUDED_
#define _IMAGE_H__INCLUDED_

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>




#ifndef UCHAR
#define UCHAR unsigned char
#endif




/* X_BITS_PER_CHANNEL is the number of bit per channel X Window System
** uses to represent a color.
*/
#define X_BITS_PER_CHANNEL 16

/* STATIC_GRAY_MAXVAL is the maximum value I assume that a visual with
** StaticGray visual class uses to represent a pixel.  This is in general
** not a compile-time constant.  It should be a run-time value, set to
** something like ((1 << visual->depth) - 1).  Essentially, I assume a
** depth of 1 bit.
*/
#define STATIC_GRAY_MAXVAL 1

/* GRAY_THRESHOLD determines when I set a 1-bit monochrime pixel to
** "on" when the gray value exceeds this threshold.
*/
#define GRAY_THRESHOLD     RGBA_IMAGE_MAXVAL/2

/* TRUNC_SHIFT is the number of bits to shift right when converting
** between the X Window System color representation and the
** representation used by the rgba_image type.
*/
#define TRUNC_SHIFT (X_BITS_PER_CHANNEL - BITS_PER_CHANNEL)


/* ???_MSB_MASK is used to pick out the appropriate bits from a
** channel of an rgba_image in order to convert it into an image which is
** colormapped using my peculiar dithering colormap system (DCI).
*/
#define RED_MSB_MASK (RED_MAXVAL << (BITS_PER_CHANNEL - RED_NUM_BITS))
#define GRN_MSB_MASK (GRN_MAXVAL << (BITS_PER_CHANNEL - GRN_NUM_BITS))
#define BLU_MSB_MASK (BLU_MAXVAL << (BITS_PER_CHANNEL - BLU_NUM_BITS))

/* Rightward bit shifts for converting channel bits into a pixel index
*/
#define BLU_R_SHIFT (BITS_PER_CHANNEL - BLU_NUM_BITS)
#define GRN_R_SHIFT (BITS_PER_CHANNEL - BLU_NUM_BITS - GRN_NUM_BITS)
#define RED_R_SHIFT (BITS_PER_CHANNEL - BLU_NUM_BITS - GRN_NUM_BITS - RED_NUM_BITS)




/* BITS_PER_CHANNEL is the number of bits per channel per pixel that
** an rgba_image uses to store image data.
** This should be the same as the size of the storage class used for
** rgba_image channels, i.e. UCHAR.
*/
#define BITS_PER_CHANNEL 8

/* RGBA_IMAGE_MAXVAL is the maximum value a channel can have for a
** single pixel.
*/
#define RGBA_IMAGE_MAXVAL ((1<<BITS_PER_CHANNEL) - 1)

/* RGBA_IMAGE_OPAQUE is the same as RGBA_IMAGE_MAXVAL, except that
** it's for the alpha (opacity) channel, and this is a reminder that zero
** is transparent and the MAXVAL is opaque.
*/
#define RGBA_IMAGE_OPAQUE RGBA_IMAGE_MAXVAL




typedef struct rgba_image_ {
  int nrows;       /* number of rows in the image */
  int ncols;       /* number of columns in the image */
  int compressed;
  int pixel_size;
  int color_mapped;
  int type;
  UCHAR *ri;       /* red channel image data */
  UCHAR *gi;       /* green channel image data */
  UCHAR *bi;       /* blue channel image data */
  UCHAR *ai;       /* alpha (opacity) channel image data */
} rgba_image_t;




#define NUM_ORIG_IMAGES 2

extern rgba_image_t orig_image[];




extern void reset_images(int type);

extern int load_img(char *fn, rgba_image_t *imgP);

extern int save_img(char *fn, rgba_image_t *si, rgba_image_t *di, double t);

extern void dither_image(Visual *visual, rgba_image_t *srcP, rgba_image_t *dstP, double t, double brite, XImage *ximage);

extern int make_test_image(rgba_image_t *imgP, int type);

#endif
