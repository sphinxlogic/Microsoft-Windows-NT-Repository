/* tga-24.c -- Targa file handling routines (24/32 bit version)
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#ifndef VMS
#include <memory.h>
#endif

#include "my_malloc.h"
#include "file.h"
#include "tga.h"

/* Global Targa colormap */
static UCHAR tga_cmap_r[16384];
static UCHAR tga_cmap_g[16384];
static UCHAR tga_cmap_b[16384];
static UCHAR tga_cmap_a[16384];

rgba_image_t tga_cmap = {
  0, 0, 0, 0, 0, 0,
  tga_cmap_r, tga_cmap_g, tga_cmap_b, tga_cmap_a,
};

/* ====================================================================== */

/* load_tga_pixel: read a Targa pixel from fio into imgP
** npixels is the number of literal pixels to read.
** size is the size of the pixel in the file, in bits.
** mapped tells whether bits are gray, coded RGB, or cmap index.
**
** note: This code is optimized for speed, not readability or compactness
** so the redundancy is intentional.
**
** -- MJG 12apr94
*/

static int
load_tga_pixel(FILE *fio, rgba_image_t *imgP, int npixels, int size, int mapped)
{
  int   pcount;
  switch(size) {
    case 8:
      if(get_block(fio, (char*)imgP->ri, npixels)) {
        fprintf(stderr, "load_tga_pixel: read error\n");
        return(-1);
      }
      if(mapped) {
        for(pcount=0; pcount < npixels; pcount++) {
          /* cmap indices are stored in ri, so do lookups with r last  */
          imgP->bi[pcount] = tga_cmap.bi[imgP->ri[pcount]];
          imgP->gi[pcount] = tga_cmap.gi[imgP->ri[pcount]];
          imgP->ri[pcount] = tga_cmap.ri[imgP->ri[pcount]];
        }
      } else {
        memcpy(imgP->gi, imgP->ri, npixels);
        memcpy(imgP->bi, imgP->ri, npixels);
      }
      memset(imgP->ai, RGBA_IMAGE_OPAQUE, npixels);
      break;

    case 16: case 15:
      {
        int ip, jp;
        int pixel;

        for(pcount=0; pcount < npixels; pcount++) {
          GET_BYTE(ip, fio);
          GET_BYTE(jp, fio);
          if(mapped) {
            pixel = ((unsigned int) jp << 8) + ip;
            imgP->ri[pcount] = tga_cmap.ri[pixel];
            imgP->gi[pcount] = tga_cmap.gi[pixel];
            imgP->bi[pcount] = tga_cmap.bi[pixel];
          } else {
            /* Unpack color bits (5 each for red, green, blue */
            imgP->ri[pcount] = (jp & 0x7c) >> 2;
            imgP->gi[pcount] = ((jp & 0x03) << 3) + ((ip & 0xe0) >> 5);
            imgP->bi[pcount] = ip & 0x1f;
          }
          imgP->ai[pcount] = RGBA_IMAGE_OPAQUE;
        }
      }
      break;

    case 32: case 24:
      {
        for(pcount=0; pcount < npixels; pcount++) {
          GET_BYTE(imgP->bi[pcount], fio);
          GET_BYTE(imgP->gi[pcount], fio);
          GET_BYTE(imgP->ri[pcount], fio);
          if(size == 32) {
            GET_BYTE(imgP->ai[pcount], fio);
          } else {
            imgP->ai[pcount] = RGBA_IMAGE_OPAQUE;
          }
        }
      }
      break;

    default:
      fprintf(stderr, "load_tga_pixel: unknown pixel size %i\n", size);
      return(-1);
      break;

  }
  return(0);
}

/* ---------------------------------------------------------------------- */

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

/* load_tga_image: Load a Targa image file from fio into imgP
*/
int
load_tga_image(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio)
{
  int   col;
  int   rle_count;      /* run-length of data */
  int   c_row;          /* current row index being loaded */
  int   rl_encoded;     /* boolean flag */
  int   row_count;      /* total count of rows loaded */
  int   ilace_row;      /* interlaced row counter */
  int   top_row;        /* where to start over for interlaced images */
  rgba_image_t timg;

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
    int   nbytes;
    int   rle_compressed; /* whether run is encoded or literal */
    int   blir;           /* bytes left in row */
    UCHAR ibyte, tri, tgi, tbi, tai;

    /* Load the temporary image with info from imgP */
    timg = *imgP;

    while(row_count < imgP->nrows) {

      for(col=0 ; col < imgP->ncols; ) {

        /* Read in the RLE count */
        GET_BYTE(ibyte, fio);
        if(ibyte & 0x80) {
          /* run-length encoded pixel */
          rle_count = ibyte - 127;
          rle_compressed = 1;
          /* Read the repeated byte */
          timg.ri = &tri; timg.gi = &tgi; timg.bi = &tbi; timg.ai = &tai;
          if(load_tga_pixel(fio, &timg, 1, tgaP->pixel_size, tgaP->mapped))
          {
            fprintf(stderr,"load_tga_image: read error in rle row %i\n", c_row);
            return(EOF);
          }

        } else {
          /* stream of unencoded pixels */
          rle_count = ibyte + 1;
          rle_compressed = 0;
        }

        blir = imgP->ncols - col;

        /* Put run data into image memory */
        while(rle_count) {
          if(rle_count <= blir) {
            /* finish the RLE block */
            nbytes = rle_count;
          } else {
            /* finish the row */
            nbytes = blir;
          }
          if(row_count >= imgP->nrows) {
            fprintf(stderr, "load_tga_image: overread image.\n");
            col = imgP->ncols;
            break;
          }
          if(rle_compressed) {
            memset(&imgP->ri[c_row*imgP->ncols+col], timg.ri[0], nbytes);
            memset(&imgP->gi[c_row*imgP->ncols+col], timg.gi[0], nbytes);
            memset(&imgP->bi[c_row*imgP->ncols+col], timg.bi[0], nbytes);
            memset(&imgP->ai[c_row*imgP->ncols+col], timg.ai[0], nbytes);
          } else {
            timg.ri = &(imgP->ri[c_row*imgP->ncols+col]);
            timg.gi = &(imgP->gi[c_row*imgP->ncols+col]);
            timg.bi = &(imgP->bi[c_row*imgP->ncols+col]);
            timg.ai = &(imgP->ai[c_row*imgP->ncols+col]);
            if(load_tga_pixel(fio, &timg,nbytes,tgaP->pixel_size, tgaP->mapped))
            {
              fprintf(stderr, "load_tga_image: read err 3 in row %i\n", c_row);
              return(EOF);
            }
          }
          if(rle_count <= blir) {
            /* just emptied the RLE block */
            col += rle_count;
            rle_count = 0;
          } else {
            /* just emptied a row */
            rle_count -= blir;
            col = 0;
            blir = imgP->ncols;
            ROW_INC;
          }
        }
      } /* for col */

      ROW_INC;
    } /* while row_count */

  } else {
    /* Not run-length encoded */
    /* load pixel data one row at a time */
    while(row_count < imgP->nrows) {
      timg.ri = &(imgP->ri[c_row*imgP->ncols]);
      timg.gi = &(imgP->gi[c_row*imgP->ncols]);
      timg.bi = &(imgP->bi[c_row*imgP->ncols]);
      timg.ai = &(imgP->ai[c_row*imgP->ncols]);
      if(load_tga_pixel(fio, &timg,imgP->ncols, tgaP->pixel_size, tgaP->mapped))
      {
        fprintf(stderr, "load_tga_image: read error in row %i\n", c_row);
        return(EOF);
      }
      ROW_INC;
    }
  }

  return(0);
}

/* ---------------------------------------------------------------------- */

/* load_tga_header: load a Targa image header from fio into tgaP and imgP
**
** Note: since Targa files (aka tga files) have no magic number
** it is not a simple matter to determine whether a file is a valid
** Targa image.  Therefore, there are several consistency checks in
** this header reading routine to try to determine whether the file
** is a valid targa file.
**
** If this routine returns nonzero, then either the file is not a
** valid targa file, or we don't support this type.
**
** In the case that the file is not a Targa file, then you could
** lseek to the beginning of the file and try to read it as another
** type of image.
**
** Since there is no way to be certain of whether the error is because
** this is not a Targa at all, or if it is because the file is simply
** a corrupt or unsupported Targa, no error messages are reported
** by this routine.  Instead, a different value is returned for
** every different kind of reason why this routine rejected the
** header.  The caller routine is responsible for handling this return
** value appropriately.
*/
int
load_tga_header(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio)
{
  UCHAR     flags;

  GET_BYTE(tgaP->id_len, fio);
  GET_BYTE(tgaP->cmap_type, fio);
  GET_BYTE(tgaP->img_type, fio);

  /* Verify that this is among the supported Targa types */
  switch(tgaP->img_type) {
    case TGA_RLE_Map:
    case TGA_RLE_RGB:
    case TGA_RLE_Mono:
      imgP->compressed = 1;
      break;

    case TGA_Map:
    case TGA_RGB:
    case TGA_Mono:
      break;

    default:
      /* This is not a Targa I can deal with */
      /* (or it is not a Targa at all) */
      return(1);
      break;
  }
  imgP->type = TARGA_MAGIC;

  /* Load rest of Targa header */
  tgaP->cmap_index = get_le_word(fio);
  tgaP->cmap_len = get_le_word(fio);
  GET_BYTE(tgaP->cmap_size, fio);
  tgaP->x_off = get_le_word(fio);
  tgaP->y_off = get_le_word(fio);
  imgP->ncols = get_le_word(fio);
  imgP->nrows = get_le_word(fio);
  GET_BYTE(tgaP->pixel_size, fio);

  GET_BYTE(flags, fio);
  tgaP->att_bits = flags & 0xf;
  tgaP->reserved = (flags & 0x10) >> 4;
  tgaP->origin_bit = (flags & 0x20) >> 5;
  tgaP->interleave = (flags & 0xc0) >> 6;

  /* Load the ID field */
  if(tgaP->id_len) {
    char *id_field;
    id_field=MY_CALLOC(tgaP->id_len, char, "load_tga_header");

    if(get_block(fio, id_field, tgaP->id_len)) {
      fprintf(stderr, "load_tga_header: read error in id field\n");
      return(EOF);
    }
  }

  /* Verify the validity of the colormap or pixel size */
  if(tgaP->img_type == TGA_Map || tgaP->img_type == TGA_RLE_Map
     || tgaP->img_type == TGA_CompMap || tgaP->img_type == TGA_CompMap4 )
  {
    if(tgaP->cmap_type != 1) {
      /* There was no valid colormap, but one was required */
      return(2);
    }

    imgP->color_mapped = tgaP->mapped = 1;

    switch(tgaP->cmap_size) {
      case 8:
      case 24: case 32:
      case 15: case 16:
        break;

      default:
        /* invalid colormap entry size */
        return(3);
        break;
    }
    imgP->pixel_size = tgaP->cmap_size;

    if(tgaP->pixel_size!=8 && tgaP->pixel_size!=15 && tgaP->pixel_size!=16)
    {
      return(7);
    }
  } else {

    tgaP->mapped = 0;

    switch(tgaP->pixel_size) {
      case 8:
      case 15: case 16:
      case 24: case 32:
        break;

      default:
        /* invalid pixel size */
        return(4);
        break;
    }
    imgP->pixel_size = tgaP->pixel_size;
  }

  if(tgaP->cmap_type) {
    if(tgaP->cmap_index + tgaP->cmap_len > 16384) {
      /* colormap is invalid length */
      return(5);
    }

#ifdef CMAP256
    if(tgaP->cmap_index + tgaP->cmap_len > 256) {
      /* colormap will not fit */
      return(6);
    }
#endif

    tga_cmap.ri = &tga_cmap_r[tgaP->cmap_index];
    tga_cmap.gi = &tga_cmap_g[tgaP->cmap_index];
    tga_cmap.bi = &tga_cmap_b[tgaP->cmap_index];
    tga_cmap.ai = &tga_cmap_a[tgaP->cmap_index];
    load_tga_pixel(fio, &tga_cmap, tgaP->cmap_len, tgaP->cmap_size, 0);
  }
  return(0);
}
