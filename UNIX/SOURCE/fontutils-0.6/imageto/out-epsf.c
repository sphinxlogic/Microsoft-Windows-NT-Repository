/* out-epsf.c: output the whole image as an EPS file.

Copyright (C) 1992 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "config.h"

#include "hexify.h"
#include "report.h"

#include "main.h"
#include "out-epsf.h"

extern string version_string;

static void pack_scanline (one_byte *, unsigned);

/* Convert a number in pixels to big points, which are Adobe's units.
   (72 big points to the inch.)  */
#define PIXELS_TO_BP(v, dpi) (72 * (v) / (dpi))

/* Abbreviations for output to `eps_file'.  */
#define OUT_STRING(s) fprintf (eps_file, "%s", s)
#define OUT_SIMPLE(s) fprintf (eps_file, "%%%%%s\n", s)
#define OUT_COMMENT(s, v) fprintf (eps_file, "%%%%%s: %s\n", s, v)
#define OUT1(s, v1) fprintf (eps_file, s, v1)
#define OUT2(s, v1, v2) fprintf (eps_file, s, v1, v2)
#define OUT3(s, v1, v2, v3) fprintf (eps_file, s, v1, v2, v3)

/* Write an Encapsulated PostScript file corresponding to the image.  */

void
write_epsf (string output_name, image_header_type image_header)
{
  /* Just black & white, or do we have grayscale?  */
  boolean monochrome_p = image_header.depth == 1;
  
  /* Just for convenience.  */
  unsigned width = image_header.width;
  unsigned height = image_header.height;
  
  /* We pack the image tightly if it's monochrome.  */
  unsigned width_used = monochrome_p ? width / 8 + !!(width % 8) : width;

  /* Buffer into which we'll read the image data.  */
  unsigned scanline_count = 0;
  one_byte *scanline = xmalloc (width);
  
  /* Open the output file OUTPUT_NAME.  */
  FILE *eps_file = xfopen (output_name, "w");

  OUT_STRING ("%!PS-Adobe-3.0 EPSF-3.0\n");
  OUT2 ("%%%%BoundingBox: 0 0 %u %u\n", width, height);
  OUT_COMMENT ("Creator", version_string);
  OUT_COMMENT ("Title", output_name);
  OUT_COMMENT ("CreationDate", now ());
  OUT_COMMENT ("DocumentData", "Clean7Bit");
  OUT_SIMPLE ("EndComments");
  
  /* We map the image to the unit square for image(mask) and scale the
     coordinate system to get back to the original size.  I can't grasp
     how to use the matrix argument to image(mask) to avoid the scaling,
     but I'm sure it's possible. */
  OUT2 ("gsave\n  %u %u scale\n", width, height);
  
  /* We need a buffer to hold the string chunks as we read them.  It
     can't be of arbitrary size: it must be an exact multiple of the
     total number of data characters.  Otherwise, we will read past the
     end of the data.  */
  OUT1 ("/image-buffer %u string def\n", width_used);
  
  /* If we are monochrome, we use the `imagemask' operator; else `image'.  */
  OUT2 ("  %u %u", width, height);

  if (monochrome_p)
    OUT_STRING (" true"); /* The `invert' argument.  */
  else
    OUT1 (" %u", image_header.depth); /* bits/sample */
  
  OUT3 (" [%u 0 0 -%u 0 %u]\n", width, height, height);
  OUT_STRING ("{currentfile image-buffer readhexstring pop}\n");
  OUT1 ("%s\n", monochrome_p ? "imagemask" : "image");
  
  /* Read the image.  */
  while ((*image_get_scanline) (scanline))
    {
      string h;
      unsigned loc;

      scanline_count++;
      if (scanline_count % 10 == 0)
        REPORT1 (".%s", scanline_count % 790 == 0 ? "\n" : "");
      
      /* Monochrome images are output with eight samples/byte; grayscale
         images are output with one sample/byte.  */
      if (monochrome_p)
        pack_scanline (scanline, width);

      /* Convert binary to ASCII hexadecimal.  */
      h = hexify (scanline, width_used);

      /* Adobe says lines in EPS files should be no more than 255
         characters.  How silly.  */
      for (loc = 1; loc <= 2 * width_used; loc++)
        {
          putc (h[loc - 1], eps_file);
          if (loc % 255 == 0)
            putc ('\n', eps_file);
	}
      
      free (h);
      putc ('\n', eps_file);
    }
  
  /* Restore the ctm.  */
  OUT_STRING ("grestore\n");

  if (scanline_count != image_header.height)
    WARNING2 ("Expected %u scanlines, read %u", image_header.height,
              scanline_count); 
  
  OUT_SIMPLE ("TRAILER");
  OUT_SIMPLE ("EOF");
}

/* Change the one bit/byte representation (call each byte a `cell') of
   LENGTH bits in DATA to be eight bits/byte.  Pad the last byte with
   zero.  We don't change those bytes beyond the end of packed portion,
   thus assuming they are not looked at.  */

static void
pack_scanline (one_byte *data, unsigned length)
{
  unsigned cell; /* Which bit in the original data we're on.  */
  unsigned packing_loc = 0; /* Which byte we're currently packing.  */
  unsigned packing_bit = 8; /* How much to shift.  */
  
  /* The very first cell has to be treated specially, because we must
     initialize it with itself shifted left (if we're going to use data
     in place, that is.)  */
  data[0] <<= 7;
  
  for (cell = 0; cell < length; cell++)
    {
      packing_bit--;
      data[packing_loc] |= data[cell] << packing_bit;
      if (packing_bit == 0)
        {
          packing_bit = 8;
          packing_loc++;
          
          /* After the first byte, we can just clear the byte at
             `packing_loc', since `cell' has already moved beyond it.  */
          data[packing_loc] = 0;
	}
    }
}
