/*
 * XIMQ - display NASA/PDS "IMQ" files on a X11 workstation
 *
 * Copyright © 1991, 1994 Digital Equipment Corporation
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION.
 *
 * DIGITAL EQUIPMENT CORPORATION SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND DIGITAL EQUIPMENT CORPORATION HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Module:
 *
 *	XIMQ_IMAGE_PROCESSING.C
 *
 * Author:
 *
 *    Frederick G. Kleinsorge, November 1991
 *
 * Environment:
 *
 *    OpenVMS, VAX-C syntax
 *
 * Abstract:
 *
 *    This module contains random image processing subroutines for ximq
 *    such as convolution, subtraction, scaling, etc.
 *
 *
 */ 
#include "ximq.h"

/*
 *  Forward refrences
 *
 */
int	convolve_image_3x3();
int	convolve_image_3x3_int();
int	median_filter();
int	subtract_image();

/*
 *  convolve_image_3x3()
 *
 *  description:
 *
 *	Convolves and image using floating point arithmatic.
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
convolve_image_3x3(src, dst, ib, image_filter, divide_it_by_this, clamp, display_it)

int divide_it_by_this, clamp, display_it;
float *image_filter;
unsigned char *src, *dst, *ib;

{
  int x, y, i, j, k, line = 0;
  float pixel_sum;
  unsigned char *curr_scan, *prev_scan, *next_scan, *dp, *op;

  dp = ib;

  for (y = 0; y < state.num_scanlines; y += 1)
    {

      curr_scan = next_scan = prev_scan = src + (y * state.samples_per_line);

      if (y > 0) prev_scan -= state.samples_per_line;

      if (y < state.num_scanlines-1) next_scan += state.samples_per_line;

      op = dst;

     /*
      *  Do left edge
      *
      */
      pixel_sum  = *(prev_scan)     *  *(image_filter+0);
      pixel_sum += *(prev_scan)     *  *(image_filter+1);
      pixel_sum += *(prev_scan+1)   *  *(image_filter+2);
      pixel_sum += *(curr_scan)     *  *(image_filter+3);
      pixel_sum += *(curr_scan)     *  *(image_filter+4);
      pixel_sum += *(curr_scan+1)   *  *(image_filter+5);
      pixel_sum += *(next_scan)     *  *(image_filter+6);
      pixel_sum += *(next_scan)     *  *(image_filter+7);
      pixel_sum += *(next_scan+1)   *  *(image_filter+8);

      if (divide_it_by_this)
	k = (pixel_sum / divide_it_by_this) + 0.5;
      else k = pixel_sum;

      if (clamp)
	{
	  if (k < 0) k = 0;
	  else if (k > 255) pixel_sum = 255;
	}
      else
	{
	  if (k < 0) k *= -1;
	  k = k % 256;
	}

      *dst++ = k;

     /*
      *  Now loop through the middle of the scanline without any
      *  special case checks...
      *
      */
      for (x = 1; x < state.samples_per_line-1; x += 1)
	{
	  int d = x-1, p = x+1;

	  pixel_sum  = *(prev_scan+d)     *  *(image_filter+0);
	  pixel_sum += *(prev_scan+x)     *  *(image_filter+1);
	  pixel_sum += *(prev_scan+p)     *  *(image_filter+2);
	  pixel_sum += *(curr_scan+d)     *  *(image_filter+3);
	  pixel_sum += *(curr_scan+x)     *  *(image_filter+4);
	  pixel_sum += *(curr_scan+p)     *  *(image_filter+5);
	  pixel_sum += *(next_scan+d)     *  *(image_filter+6);
	  pixel_sum += *(next_scan+x)     *  *(image_filter+7);
	  pixel_sum += *(next_scan+p)     *  *(image_filter+8);

	  if (divide_it_by_this)
	    k = (pixel_sum / divide_it_by_this) + 0.5;
	  else k = pixel_sum;

	  if (clamp)
	    {
	      if (k < 0) k = 0;
	      else if (k > 255) pixel_sum = 255;
	    }
	  else
	    {
	      if (k < 0) k *= -1;
	      k = k % 256;
	    }

	   *dst++ = k;

	} /* End loop for x */

      x = state.samples_per_line-1;

      pixel_sum  = *(prev_scan+(x-1)) *  *(image_filter+0);
      pixel_sum += *(prev_scan+x)     *  *(image_filter+1);
      pixel_sum += *(prev_scan+x)     *  *(image_filter+2);
      pixel_sum += *(curr_scan+(x-1)) *  *(image_filter+3);
      pixel_sum += *(curr_scan+x)     *  *(image_filter+4);
      pixel_sum += *(curr_scan+x)     *  *(image_filter+5);
      pixel_sum += *(next_scan+(x-1)) *  *(image_filter+6);
      pixel_sum += *(next_scan+x)     *  *(image_filter+7);
      pixel_sum += *(next_scan+x)     *  *(image_filter+8);

      if (divide_it_by_this)
	k = (pixel_sum / divide_it_by_this) + 0.5;
      else k = pixel_sum;

      if (clamp)
	{
	  if (k < 0) k = 0;
	  else if (k > 255) pixel_sum = 255;
	}
      else
	{
	  if (k < 0) k *= -1;
	  k = k % 256;
	}

      *dst++ = k;

     /*
      *  Reformat the pixel data for the output display
      *
      */
      reformat_image_data(op, ib);
      line += 1;

     /*
      *  And display it on the fly if asked...
      *
      */
      if (display_it && (display_it == line))
	{
	  display_scanline(y-line, dp, line);
	  dp   = ib;
	  line = 0;
	}

    } /* End Y */

  if (display_it && line)
      display_scanline(y-line, dp, line);
}

/*
 *  convolve_image_3x3_int()
 *
 *  description:
 *
 *	Convolves and image using integer arithmatic.
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
convolve_image_3x3_int(src, dst, ib, image_filter, divide_it_by_this, clamp, display_it)

int divide_it_by_this, clamp, display_it;
int *image_filter;
unsigned char *src, *dst, *ib;

{
  int x, y, i, j, k, line = 0;
  int pixel_sum;
  unsigned char *curr_scan, *prev_scan, *next_scan, *dp, *op;

  dp = ib;

  for (y = 0; y < state.num_scanlines; y += 1)
    {

      curr_scan = next_scan = prev_scan = src + (y * state.samples_per_line);

      if (y > 0) prev_scan -= state.samples_per_line;

      if (y < state.num_scanlines-1) next_scan += state.samples_per_line;

      op = dst;

     /*
      *  Do left edge
      *
      */
      pixel_sum  = *(prev_scan)     *  *(image_filter+0);
      pixel_sum += *(prev_scan)     *  *(image_filter+1);
      pixel_sum += *(prev_scan+1)   *  *(image_filter+2);
      pixel_sum += *(curr_scan)     *  *(image_filter+3);
      pixel_sum += *(curr_scan)     *  *(image_filter+4);
      pixel_sum += *(curr_scan+1)   *  *(image_filter+5);
      pixel_sum += *(next_scan)     *  *(image_filter+6);
      pixel_sum += *(next_scan)     *  *(image_filter+7);
      pixel_sum += *(next_scan+1)   *  *(image_filter+8);

      if (divide_it_by_this)
	k = pixel_sum / divide_it_by_this;
      else k = pixel_sum;

      if (clamp)
	{
	  if (k < 0) k = 0;
	  else if (k > 255) pixel_sum = 255;
	}
      else
	{
	  if (k < 0) k *= -1;
	  k = k % 256;
	}

      *dst++ = k;

     /*
      *  Now loop through the middle of the scanline without any
      *  special case checks...
      *
      */
      for (x = 1; x < state.samples_per_line-1; x += 1)
	{
	  int d = x-1, p = x+1;

	  pixel_sum  = *(prev_scan+d)     *  *(image_filter+0);
	  pixel_sum += *(prev_scan+x)     *  *(image_filter+1);
	  pixel_sum += *(prev_scan+p)     *  *(image_filter+2);
	  pixel_sum += *(curr_scan+d)     *  *(image_filter+3);
	  pixel_sum += *(curr_scan+x)     *  *(image_filter+4);
	  pixel_sum += *(curr_scan+p)     *  *(image_filter+5);
	  pixel_sum += *(next_scan+d)     *  *(image_filter+6);
	  pixel_sum += *(next_scan+x)     *  *(image_filter+7);
	  pixel_sum += *(next_scan+p)     *  *(image_filter+8);

	  if (divide_it_by_this)
	    k = pixel_sum / divide_it_by_this;
	  else k = pixel_sum;

	  if (clamp)
	    {
	      if (k < 0) k = 0;
	      else if (k > 255) pixel_sum = 255;
	    }
	  else
	    {
	      if (k < 0) k *= -1;
	      k = k % 256;
	    }

	   *dst++ = k;

	} /* End loop for x */

      x = state.samples_per_line-1;

      pixel_sum  = *(prev_scan+(x-1)) *  *(image_filter+0);
      pixel_sum += *(prev_scan+x)     *  *(image_filter+1);
      pixel_sum += *(prev_scan+x)     *  *(image_filter+2);
      pixel_sum += *(curr_scan+(x-1)) *  *(image_filter+3);
      pixel_sum += *(curr_scan+x)     *  *(image_filter+4);
      pixel_sum += *(curr_scan+x)     *  *(image_filter+5);
      pixel_sum += *(next_scan+(x-1)) *  *(image_filter+6);
      pixel_sum += *(next_scan+x)     *  *(image_filter+7);
      pixel_sum += *(next_scan+x)     *  *(image_filter+8);

      if (divide_it_by_this)
	k = pixel_sum / divide_it_by_this;
      else k = pixel_sum;

      if (clamp)
	{
	  if (k < 0) k = 0;
	  else if (k > 255) pixel_sum = 255;
	}
      else
	{
	  if (k < 0) k *= -1;
	  k = k % 256;
	}

      *dst++ = k;

     /*
      *  Reformat the pixel data for the output display
      *
      */
      reformat_image_data(op, ib);
      line += 1;

     /*
      *  And display it on the fly if asked...
      *
      */
      if (display_it && (display_it == line))
	{
	  display_scanline(y-line, dp, line);
	  dp   = ib;
	  line = 0;
	}

    } /* End Y */

  if (display_it && line)
      display_scanline(y-line, dp, line);
}

/*
 *  median_filter()
 *
 *  description:
 *
 *	Apply a median filter to the current image.  Based on a routine from
 *	Graphics Gems...
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
median_filter(src, dst, ib, display_it)

int display_it;
unsigned char *src, *dst, *ib;

{
  int line = 0, y;
  register int x, r1, r2, r3, r4, r5, r6;
  register unsigned char *b1, *b2, *b3;

  unsigned char *a1, *a2, *a3, *dp, *op;

  dp = ib;

  for (y = 0; y < state.num_scanlines; y += 1)
    {

      a1 = a2 = a3 = src + ((y * state.samples_per_line) - 1);

      if (y > 0)
	a1 = src + (((y-1) * state.samples_per_line) - 1);

      if (y < state.num_scanlines-1)
	a3 = src + (((y+1) * state.samples_per_line) - 1);

      op = dst;

      for (x = 0; x < state.samples_per_line; x += 1)
	{

	  if (x == 0)
	    {
	      b1 = a1 + x+1;
	      b2 = a2 + x+1;
	      b3 = a3 + x+1;
	    }
	  else if (x == state.samples_per_line-1)
	    {
	      b1 = a1 + (x-1);
	      b2 = a2 + (x-1);
	      b3 = a3 + (x-1);
	    }
	  else
	    {
	      b1 = a1 + x;
	      b2 = a2 + x;
	      b3 = a3 + x;
	    }

         /*
 	  *  Sort the pixels to find the median
	  *
	  */
	  r1 = *b1++;
	  r2 = *b1++;
	  r3 = *b1++;
	  r4 = *b2++;
	  r5 = *b2++;
	  r6 = *b2++;

	  mnmx6(r1, r2, r3, r4, r5, r6);

	  r1 = *b3++;
	  mnmx5(r1, r2, r3, r4, r5);

	  r1 = *b3++;
	  mnmx4(r1, r2, r3, r4);

	  r1 = *b3++;
	  mnmx3(r1, r2, r3);

	  *dst++ = r2;

	} /* End X */

      reformat_image_data(op, ib);
      line += 1;

      if (display_it && (display_it == line))
	{
	  display_scanline(y-line, dp, line);
	  dp   = ib;
	  line = 0;
	}
    }

  if (display_it && line)
    display_scanline(y-line, dp, line);

} /* End function */

int
nearest_neighbor_zoom(src, dst, ib, x1, y1, width, lines, display_it)

int x1, y1, width, lines, display_it;
unsigned char *src, *dst, *ib;

{
  float x_scale_factor, y_scale_factor;
  int close_y, close_x, y_off, x, y, line = 0;
  char unsigned *zb, *op;

  x_scale_factor = state.samples_per_line / (1.0 * width);
  y_scale_factor = state.num_scanlines    / (1.0 * lines);

  zb = ib;

  for (y = 0; y < state.num_scanlines; y += 1)
    {
      close_y  = (((y / y_scale_factor)) + y1) + 0.5;

      if (close_y >= state.num_scanlines)
	close_y = state.num_scanlines - 1;

      y_off = close_y * state.samples_per_line;

      op = dst;

      for (x = 0; x < state.samples_per_line; x += 1)
	{

	  close_x = (((x / x_scale_factor)) + x1) + 0.5;
	  *dst++ = *(src + close_x + y_off);

	}

      reformat_image_data(op, ib);
      line += 1;

      if (display_it && (display_it == line))
	{
	  display_scanline(y-line, zb, line);
	  zb   = ib;
	  line = 0;
	}

    }

  if (display_it && line)
    display_scanline(y-line, zb, line);

}

int
matrix_3x3_zoom(src, dst, ib, x1, y1, width, lines, display_it, image_filter, divide_it_by_this, clamp)

int x1, y1, width, lines, display_it, *image_filter, divide_it_by_this, clamp;
unsigned char *src, *dst, *ib;

{
  float x_scale_factor, y_scale_factor;
  int pixel_sum, k, close_y, c, d, p, x, y, line = 0;
  char unsigned *zb, *op, *curr_scan, *prev_scan, *next_scan;

  x_scale_factor = state.samples_per_line / (1.0 * width);
  y_scale_factor = state.num_scanlines    / (1.0 * lines);

  zb = ib;

  for (y = 0; y < state.num_scanlines; y += 1)
    {
      close_y  = (((y / y_scale_factor)) + y1) + 0.5;

      if (close_y >= state.num_scanlines)
	close_y = state.num_scanlines - 1;

      curr_scan = next_scan = prev_scan = (close_y * state.samples_per_line) + src;

      if (close_y > 0) prev_scan -= state.samples_per_line;
      if (close_y < state.num_scanlines-1) next_scan += state.samples_per_line;

      op = dst;

      for (x = 0; x < state.samples_per_line; x += 1)
	{
          c = (((x / x_scale_factor)) + x1) + 0.5;
          d = c-1;
	  p = c+1;

	  pixel_sum  = *(prev_scan+d)     *  *(image_filter+0);
	  pixel_sum += *(prev_scan+c)     *  *(image_filter+1);
	  pixel_sum += *(prev_scan+p)     *  *(image_filter+2);
	  pixel_sum += *(curr_scan+d)     *  *(image_filter+3);
	  pixel_sum += *(curr_scan+c)     *  *(image_filter+4);
	  pixel_sum += *(curr_scan+p)     *  *(image_filter+5);
	  pixel_sum += *(next_scan+d)     *  *(image_filter+6);
	  pixel_sum += *(next_scan+c)     *  *(image_filter+7);
	  pixel_sum += *(next_scan+p)     *  *(image_filter+8);

	  if (divide_it_by_this)
	    pixel_sum = pixel_sum / divide_it_by_this;

	  if (clamp)
	    {
	      if (pixel_sum < 0) pixel_sum = 0;
	      else if (pixel_sum > 255) pixel_sum = 255;
	    }
	  else
	    {
	      if (pixel_sum < 0) pixel_sum *= -1;
	      pixel_sum = pixel_sum % 256;
	    }

	   *dst++ = pixel_sum;

	} /* End loop for x */

      reformat_image_data(op, ib);
      line += 1;

      if (display_it && (display_it == line))
	{
	  display_scanline(y-line, zb, line);
	  zb   = ib;
	  line = 0;
	}
    }

  if (display_it && line)
    display_scanline(y-line, zb, line);
}

int
pixel_replicate_zoom(src, dst, ib, x1, y1, width, lines, display_it)

int x1, y1, width, lines, display_it;
unsigned char *src, *dst, *ib;

{
  int pph, ppw, x, y, xx, yy, y_off, ol = 0, pos;
  char unsigned *zb, *op, *input_pixel;

  pph = state.num_scanlines / lines;

 /*
  *  Find the nearest integral multiple...
  *
  */
  for (x = state.num_scanlines; x > 0; x = x / 2)
    {
      if (x < pph)
        {
	  pph   = x * 2; /* Number of pixels per scan */
	  lines = state.num_scanlines / pph; /* Number of lines for input... */
	  break;
        }
    }

  ppw = state.samples_per_line / width;

 /*
  *  Find the nearest integral multiple...
  *
  */
  for (x = state.samples_per_line; x > 0; x = x / 2)
    {
      if (x < ppw)
        {
	  ppw   = x * 2; /* Number of pixels per scan */
	  width = state.samples_per_line / ppw; /* Number of lines for input... */
	  break;
        }
    }

  zb = ib;

 /*
  *  Byte offset into file to the initial byte of the zoomed area
  *
  */
  y_off = (y1 * state.samples_per_line) + x1;

 /*
  *  Outer loop is for each line being scaled.
  *
  */
  for (y = lines; y > 0; y -= 1)
    {

     /*
      *  Each source line is 'pph' high in the destination
      *
      */
      for (yy = pph; yy > 0; yy -= 1)
	{

	 /*
	  *  Get the starting pixel address for the scanline...
	  *
	  */
	  input_pixel = src + y_off;

	 /*
	  *  Save the output address...
	  *
	  */
          op = dst;

	  pos = 800;

	 /*
	  *  For each bit in the zoom width
	  *
	  */
	  for (x = width; x > 0; x -= 1)
	    {
	     /*
	      *  Each input pixel is 'ppw' wide in the output!
	      *
	      */
	      for (xx = ppw; xx > 0; xx -= 1)
		*dst++ = *input_pixel;

	      pos -= ppw;
	      if (pos <= 0) break;

	      input_pixel += 1;

	    } /* End for x; */

	 /*
	  *  This forces any re-alignment
	  *
	  */
          dst = op + state.samples_per_line;

	 /*
	  *  Reformat the output data from intensity information to output
	  *  pixel data.
	  *
	  */
	  reformat_image_data(op, ib);

	 /*
	  *  Bump the output line count and make sure that we haven't
	  *  gone beyond the end of the buffer...
	  *
	  */
	  ol += 1;
	  if (ol > state.num_scanlines) break;

	} /* End for yy; */

     /*
      *  Display the image data
      *
      */
      if (display_it)
	{
	  display_scanline(ol-pph, zb, pph);
	  zb   = ib;
	}

     /*
      *  Offset to the next scanline starting pixel address
      *
      */
      y_off += state.samples_per_line;

     /*
      *  Drop out if we are done...
      *
      */
      if (ol > state.num_scanlines) break;

    } /* End for y; */
}

/*
 *  subtract_image()
 *
 *  description:
 *
 *	Takes two source images and subtracts one from the other and writes
 *	the result to a destination image.  Should only be done on input
 *	pixel data - since the output data pixel value may not be the same
 *	as the intensity.  The result is clamped at 0 (no negative pixels).
 *
 *  inputs:
 *
 *	Address of an image to subtract
 *	Address of an image to be subtracted from
 *	Address of a destination image buffer
 *	Byte count.
 *
 *  outputs:
 *
 *	Image 2 is subtracted from image 1 and the result stored at the
 *	destination.
 *
 */
int
subtract_image(src1, src2, dst, length)

unsigned char *src1, *src2, *dst;
int length;

{
  int i = 0, j;

  for (i = length; i > 0; i -= 1)
    {
      j = *src1++ - *src2++;      
      if (j > 0) *dst++ = j;
      else *dst++ = 0;
    }
}
