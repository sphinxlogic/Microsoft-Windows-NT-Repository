/* tga-write.c -- Targa file writing routines (24/32 bit version)
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#ifndef VMS
#include <memory.h>
#endif

#include "file.h"
#include "tga.h"

/* ====================================================================== */

#define CURR_ROW(irow) ((tgaP->origin_bit)?(irow):(imgP->nrows-(irow)-1))

#define ROW_INC \
{ \
  row_count++; \
  if(tgaP->interleave==TGA_IL_Four) { ilace_row += 4 ; } \
  else if(tgaP->interleave==TGA_IL_Two) { ilace_row += 2 ; } \
  else  { ilace_row ++ ; } \
  if(ilace_row > imgP->nrows) ilace_row = ++ top_row; \
  c_row = CURR_ROW(ilace_row); \
}

/* ---------------------------------------------------------------------- */

#define RPIX(row, col) (imgP->ri[(row) * imgP->ncols + (col)])
#define GPIX(row, col) (imgP->gi[(row) * imgP->ncols + (col)])
#define BPIX(row, col) (imgP->bi[(row) * imgP->ncols + (col)])
#define APIX(row, col) (imgP->ai[(row) * imgP->ncols + (col)])

/* ---------------------------------------------------------------------- */

/* save_tga_pixel: save Targa pixels to fio from imgP
**
** fio is the pointer to the output image file opened for binary output.
**
** imgP is used for the image arrays (and imgP->ncols used to index)
**
** npixels is the number of consecutive pixels to write.
**
** mpsize is the size of the pixels in memory, not the size of the
**   pixels being written.
** For mpsize 8, use only the red channel.
** For mpsize 15|16, use the red and green channels.
**   Use red as the MSB and green as the LSB.
**
** mapped tells whether pixel values are gray/coded RGB, or cmap index
**   if mpsize==8|15|16 then mapped implies that a lookup ought to be
**   done, and the mapped pixel should be written.
** [There are two kinds of map: 24/32 bit pixel and 15 bit pixel.
**   "mpsize" refers to the size of the stored image, not the written
**   image.  To date, mapped saves are not supported, so this issue is moot.]
*/
static int
save_tga_pixel(FILE *fio, rgba_image_t *imgP, int col, int row, int npixels, int mpsize, int mapped)
{
  register int pcount;

  switch(mpsize) {
    case 8:
      if(mapped) {
        fprintf(stderr, "save_tga_pixel: I only do non-mapped 8\n");
        return(EOF);
      }
      {
        for(pcount=col; pcount < col+npixels; pcount++) {
          if(putc(RPIX(row, pcount), fio)==EOF) return(EOF);
        }
      }

    case 15: case 16:
      if(mapped) {
        fprintf(stderr, "save_tga_pixel: I only do non-mapped 15/16\n");
        return(EOF);
      }
      {
        for(pcount=col; pcount < col+npixels; pcount++) {
          if(putc(GPIX(row, pcount), fio)==EOF) return(EOF);
          if(putc(RPIX(row, pcount), fio)==EOF) return(EOF);
        }
      }
      break;

    case 32: case 24:
      if(mapped) {
        fprintf(stderr, "save_tga_pixel: 24/32 can't be mapped\n");
      }
      {
        for(pcount=col; pcount < col+npixels; pcount++) {
          if(putc(BPIX(row, pcount), fio)==EOF) return(EOF);
          if(putc(GPIX(row, pcount), fio)==EOF) return(EOF);
          if(putc(RPIX(row, pcount), fio)==EOF) return(EOF);
          if(mpsize == 32) {
            if(putc(APIX(row, pcount), fio)==EOF) return(EOF);
          }
        }
      }
      break;

    default:
      fprintf(stderr, "save_tga_pixel: bad pixel size %i\n", mpsize);
      return(EOF);
      break;
  }
  return(0);
}

/* ---------------------------------------------------------------------- */
/* tga_run_length: find RLE run length at current col, row of img
** depth is the number of bits per pixel
** For depth 8 Use only red channel.
** For depth 15|16 Use red as the MSB and green as the LSB.
**
** If pixel repeat 2 or 3 times, return negative of number of repeats
** otherwise Return positive number of distinct pixels until a 2|3+
**
** If error, return 0.
**
** A return value of 0 is indistinguishible from a start-at-end-of-row
** occurance, so the caller must check for end-of-row before calling
** this routine;
**
** Only runs along rows;  Will not read into next row.
*/
int
tga_run_length(rgba_image_t *imgP, int col, int row, int depth)
{
  int xi, ri;
  int run_length;

  switch(depth) {
    case 8: case 15: case 16: case 24: case 32:
      break;

    default:
      fprintf(stderr, "tga_run_length: invalid depth %i\n", depth);
      return(0);
  }

  /* Check for a run of (at least 2 or 3, at most 128) identical pixels */
  /* Don't look at the first pixel;  It's obviously equal to itself. */
  for(ri=col+1; ri<imgP->ncols && ri-col<128; ri++) {
    if(RPIX(row, ri) != RPIX(row, col)) break;
    if(depth>8) {
      if(GPIX(row, ri) != GPIX(row, col)) break;
      if(depth>16) {
        if(BPIX(row, ri) != BPIX(row, col)) break;
        if(depth==32) {
          if(APIX(row, ri) != APIX(row, col)) break;
        }
      }
    }
  } run_length=ri-col;

  switch(depth) {
    case 8:
      if(run_length>=3) return(-run_length);
      break;

    case 15: case 16: case 24: case 32:
      if(run_length>=2) return(-run_length);
      break;
  }

  /* If we've reached this far, we've into a run of distinct pixels. */
  /* Look for runs of (at most 128) distinct pixels. */
  for(xi=col+1; xi<imgP->ncols && xi-col<128; xi+=run_length) {

    for(ri=xi+1; ri<imgP->ncols && ri-xi<3; ri++) {
      if(RPIX(row, ri) != RPIX(row, xi)) break;
      if(depth>8) {
        if(GPIX(row, ri) != GPIX(row, xi)) break;
        if(depth>16) {
          if(BPIX(row, ri) != BPIX(row, xi)) break;
          if(depth==32) {
            if(APIX(row, ri) != APIX(row, xi)) break;
          }
        }
      }
    } run_length=ri-xi;

    switch(depth) {
      case 8:
        if(run_length>=3) return(xi-col);
        break;

      case 15: case 16: case 24: case 32:
        if(run_length>=2) return(xi-col);
        break;
    }
  }
  return(xi-col);
}

/* ---------------------------------------------------------------------- */

/* save_tga_image: Save a Targa image file into fio from imgP
** Quick and dirty version -- no RLE
*/
int
save_tga_image(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio)
{
  int   c_row;          /* current row index being saved */
  int   rl_encoded;     /* boolean flag */
  int   row_count;      /* total count of rows saved */
  int   ilace_row;      /* interlaced row counter */
  int   top_row;        /* where to start over for interlaced images */

  if(tgaP->img_type == TGA_RLE_Map || tgaP->img_type == TGA_RLE_RGB ||
     tgaP->img_type == TGA_RLE_Mono)
  {
    rl_encoded = 1;
  } else {
    rl_encoded = 0;
  }

  row_count = ilace_row = top_row = 0;
  c_row = CURR_ROW(ilace_row);
  if(rl_encoded) {
    int rle_count;  /* run-length of data */
    int col;        /* current column */

    while(row_count < imgP->nrows) {
      for(col=0; col < imgP->ncols; ) {
        rle_count = tga_run_length(imgP, col, c_row, tgaP->pixel_size);
        if(rle_count < 0) {
          /* Write the repeat count (negative) */
          putc(127 - rle_count, fio);

          /* Write out the pixels */
          if(save_tga_pixel(fio, imgP, col, c_row, 1, tgaP->pixel_size,
             tgaP->mapped))
          {
            fprintf(stderr, "save_tga_image: write error in row %i\n", c_row);
            return(EOF);
          }

          /* Advance the column counter */
          col += -rle_count;

        } else if(rle_count > 0) {
          /* Write the distinct count (positive) */
          putc(rle_count - 1, fio);

          /* Write out the pixels */
          if(save_tga_pixel(fio, imgP, col, c_row, rle_count,
               tgaP->pixel_size, tgaP->mapped))
          {
            fprintf(stderr, "save_tga_image: write error in row %i\n", c_row);
            return(EOF);
          }

          /* Advance the column counter */
          col += rle_count;

        } else {
          fprintf(stderr, "save_tga_image: bad RLE count %i\n", rle_count);
        }
      }
      ROW_INC;
    }
  } else {
    /* Not run-length encoded */
    /* save pixel data one row at a time */
    while(row_count < imgP->nrows) {
      if(save_tga_pixel(fio, imgP, 0, c_row, imgP->ncols, tgaP->pixel_size,
         tgaP->mapped))
      {
        fprintf(stderr, "save_tga_image: write error in row %i\n", c_row);
        return(EOF);
      }
      ROW_INC;
    }
  }
  return(0);
}

/* ---------------------------------------------------------------------- */

/* save_tga_header: save a Targa image header into fio from tgaP and imgP
** Quick and dirty: No colormaps or id_fields
*/
int
save_tga_header(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio)
{
  UCHAR     flags;

  tgaP->id_len = 0;
  putc(tgaP->id_len, fio);
  putc(tgaP->cmap_type, fio);
  putc(tgaP->img_type, fio);

  /* Save rest of Targa header */
  put_le_word(tgaP->cmap_index, fio);
  put_le_word(tgaP->cmap_len, fio);
  putc(tgaP->cmap_size, fio);
  put_le_word(tgaP->x_off, fio);
  put_le_word(tgaP->y_off, fio);
  put_le_word(imgP->ncols, fio);
  put_le_word(imgP->nrows, fio);
  putc(tgaP->pixel_size, fio);

  flags  = tgaP->att_bits    & 0xf;
  flags |= (tgaP->reserved   & 0x1) << 4;
  flags |= (tgaP->origin_bit & 0x1) << 5;
  flags |= (tgaP->interleave & 0x3) << 6;
  putc(flags, fio);

  if(tgaP->cmap_type) {
    tgaP->mapped = 1;

    /* Save the colormap for the Targa file */
    save_tga_pixel(fio, &tga_cmap, 0, 0, tgaP->cmap_len, tgaP->cmap_size, 0);

  } else {
    tgaP->mapped = 0;
  }

  return(0);
}
