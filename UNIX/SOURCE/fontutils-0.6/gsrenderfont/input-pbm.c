/* input-pbm.c: read a PBM file.

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

#include "bitmap.h"
#include "file-input.h"
#include "pbm.h"

/* From main.c.  */
extern int debug;
extern boolean trace_scanlines;

/* Where the input comes from.  */
static FILE *pbm_input_file;
static string pbm_input_filename;

static void get_row (one_byte *, int, int);

/* Only one file can be open at a time.  We do no path searching.  If
   FILENAME can't be opened, we quit.  */

void
pbm_open_input_file (string filename)
{
  assert (pbm_input_file == NULL);
  
  pbm_input_file = xfopen (filename, "r");
  pbm_input_filename = filename;
}


/* Close the input file.  If it hasn't been opened, we quit.  */

void
pbm_close_input_file ()
{
  assert (pbm_input_file != NULL);
  
  xfclose (pbm_input_file, pbm_input_filename);
  pbm_input_file = NULL;
}

/* Read a block exactly HEIGHT scanlines high from the image.  The last
   few scanlines must all be blank (we don't bother to return them).  If
   they aren't, then something is wrong.  A rounding error has caused
   Ghostscript to give us fewer lines than expected.  Just return a
   short block, and save the first nonblank line (we don't read past the
   first) for next time.  */

/* The constant here must match that in writefont.PS's calculation of
   max-char-tall-in-pixels.  */
#define BLANK_COUNT 4

bitmap_type *
pbm_get_block (unsigned height)
{
  static int image_width = -1;
  static one_byte *saved_scanline = NULL;
  static one_byte *blank_row;
  static int image_format;
  static int image_height;
  int c;
  one_byte *row;
  one_byte *block;
  unsigned block_height, blank;
  bitmap_type *b = XTALLOC1 (bitmap_type);

  /* Initialize the PBM info if necessary.  */
  if (image_width == -1)
    {
      pbm_readpbminit (pbm_input_file, &image_width, &image_height,
                       &image_format);
      blank_row = xmalloc (image_width);
    }


  /* If we have a saved scanline, let that start us off.  */
  block = saved_scanline;
  block_height = !!block;

  /* Attach rows until we get to an all-white row that is after row #
     `height - BLANK_COUNT'.  */
  do
    {
      /* If we're at the end of the file, quit.  Presumably there was
         one last blank row at the end or something.  If something was
         seriously wrong, it'll manifest itself later.  */
      c = getc (pbm_input_file);
      if (c == EOF)
        return NULL;
      ungetc (c, pbm_input_file);

      block_height++;
      block = xrealloc (block, block_height * image_width);
      row = block + (block_height - 1) * image_width;
      get_row (row, image_width, image_format);
    }
  while (block_height <= height - BLANK_COUNT
         || memchr (row, BLACK, image_width));

  /* Skip the next BLANK_COUNT rows (they had better be blank).  We can
     keep using `row' as our buffer, since it just points to the
     all-blank row we found; we won't be including it in the bitmap we
     return.  We might hit the end of the image here, in which case we
     just stop.  */
  for (blank = 1; blank < BLANK_COUNT; blank++)
    {
      c = getc (pbm_input_file);
      if (c == EOF)
        break;
        
      ungetc (c, pbm_input_file);
      get_row (blank_row, image_width, image_format);

      if (memchr (blank_row, BLACK, image_width) != NULL)
        break;
    }

  if (blank == BLANK_COUNT)
    saved_scanline = NULL;
  else
    {
      saved_scanline = xmalloc (image_width);
      memcpy (saved_scanline, blank_row, image_width);
    }
  
  /* Fill in the bitmap to return.  */
  BITMAP_WIDTH (*b) = image_width;
  BITMAP_HEIGHT (*b) = block_height;
  BITMAP_BITS (*b) = block;

  if (debug) print_bitmap (stdout, *b);

  return b;
}


/* Read a single row from the image.  */

static void
get_row (one_byte *row, int width, int format)
{
  static unsigned scanline_count = 0;

  pbm_readpbmrow (pbm_input_file, row, width, format);
  scanline_count++;

  if (trace_scanlines)
    {
      int c;
      printf ("%5d:", scanline_count);
      for (c = 0; c < width; c++)
        putchar (row[c] ? '*' : ' ');
      putchar ('\n');
    }
}
