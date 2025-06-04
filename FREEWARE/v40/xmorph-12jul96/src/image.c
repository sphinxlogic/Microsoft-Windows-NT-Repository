/* image.c : image handling and display routines
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "diw_map.h"
#include "tga.h"

#include "image.h"

#define MAX(x,y) ((x)>(y) ? (x) : (y))




/* --------------------------------------------------------------- */

/* Global "original" images */
rgba_image_t orig_image[NUM_ORIG_IMAGES];




/* --------------------------------------------------------------- */

/* free_image: free memory of the img
*/
void
free_image(rgba_image_t *imgP)
{
  if(imgP->ri != NULL) my_free(imgP->ri, "free_image");
  if(imgP->gi != NULL) my_free(imgP->gi, "free_image");
  if(imgP->bi != NULL) my_free(imgP->bi, "free_image");
  if(imgP->ai != NULL) my_free(imgP->ai, "free_image");
}




/* --------------------------------------------------------------- */

/* alloc_image: allocate memory for the img
** return -1 if failed, 0 otherwize
*/
int
alloc_image(rgba_image_t *imgP, char *proc)
{
  if((imgP->ri=MY_CALLOC(imgP->ncols * imgP->nrows,UCHAR,"alloc_image"))==NULL)
  {
    fprintf(stderr, "%s: Bad Alloc\n", proc);
    return(-1);
  }
  if((imgP->gi=MY_CALLOC(imgP->ncols * imgP->nrows,UCHAR,"alloc_image"))==NULL)
  {
    fprintf(stderr, "%s: Bad Alloc\n", proc);
    return(-1);
  }
  if((imgP->bi=MY_CALLOC(imgP->ncols * imgP->nrows,UCHAR,"alloc_image"))==NULL)
  {
    fprintf(stderr, "%s: Bad Alloc\n", proc);
    return(-1);
  }
  if((imgP->ai=MY_CALLOC(imgP->ncols * imgP->nrows,UCHAR,"alloc_image"))==NULL)
  {
    fprintf(stderr, "%s: Bad Alloc\n", proc);
    return(-1);
  }

  return(0);
}




/* --------------------------------------------------------------- */

/* reset_image: copy "original" image into image spaces of diw_maps
** type determines whether images are the "src" (1) or "dst" (2)
*/
void
reset_images(int type)
{
  int indx;
  rgba_image_t *imgP, *dmiP;

  /* Copy the "original" image into the diw_maps */
  for(indx=0; indx<NUM_DIW_MAPS; indx++) {
    if(global_diw_map[indx].width != 0) {
      switch(type) {
        case 1:
          imgP = &orig_image[0];
          dmiP = &global_diw_map[indx].src_img;
          break;

        case 2:
          imgP = &orig_image[1];
          dmiP = &global_diw_map[indx].dst_img;
          break;

        default:
          fprintf(stderr, "reset_images: Bad Value: type %i\n", type);
          return;
          break;
      }

      free_image(dmiP);

      /* Copy the image info from the original image */
      *dmiP = *imgP;

      /* Allocate memory for the image copies */
      if(alloc_image(dmiP, "reset_images")) return;

      /* Copy the original image into the diw_map image space */
      memcpy(dmiP->ri, imgP->ri, dmiP->ncols * dmiP->nrows);
      memcpy(dmiP->gi, imgP->gi, dmiP->ncols * dmiP->nrows);
      memcpy(dmiP->bi, imgP->bi, dmiP->ncols * dmiP->nrows);
      memcpy(dmiP->ai, imgP->ai, dmiP->ncols * dmiP->nrows);
    }
  }
}




/* --------------------------------------------------------------- */

/* load_img: load image into memory.  Allocate new image space.
*/
int
load_img(char *fn, rgba_image_t *imgP)
{
  int           tga_return;
  tga_hdr_t     tga_hdr;
  FILE         *infP=NULL;

  /* Open the input file for binary reading */
  if(fn!=NULL && (infP=fopen(fn, "rb"))==NULL) {
    fprintf(stderr, "load_img: could not open '%s' for input\n", fn);
    return(-1);
  }

  /* Load the image header */
    /* Targa */
    if(tga_return = load_tga_header(&tga_hdr, imgP, infP)) {
      fprintf(stderr, "load_tga_header returned %i\n", tga_return);
      return(tga_return);
    }

  /* Free the memory for the previous image planes */
  free_image(imgP);

  /* Allocate new memory for the new image planes */
  if(alloc_image(imgP, "load_img")) return(-1);

  /* Load the new image */
    /* Targa */
    load_tga_image(&tga_hdr, imgP, infP);

  /* Close the input file */
  fclose(infP);

  return(0);
}




/* --------------------------------------------------------------- */

/* save_img: dissolve 2 images and save dissolved image to file
** dimensions of the output image are the same as the srcP.
*/
int
save_img(char *fn, rgba_image_t *siP, rgba_image_t *diP, double t)
{
  register int   nx, ny;
  register int   xi, yi;
  register int   rsi, gsi, bsi, asi;
  register int   rdi, gdi, bdi, adi;
  int            tga_return;
  rgba_image_t   img;
  tga_hdr_t      tga_hdr;
  FILE          *outfP=NULL;

  /* Initialize the info for the saved image */
  ny = img.nrows = siP->nrows;
  nx = img.ncols = siP->ncols;

  img.compressed = img.color_mapped = 0;
  img.pixel_size = 32;

  if(siP->compressed || diP->compressed) img.compressed = 1;
  img.pixel_size = MAX(siP->pixel_size, diP->pixel_size);
  if(siP->color_mapped && diP->color_mapped) img.color_mapped = 1;

  /* Allocate space for image data */
  if(alloc_image(&img, "save_img")) return(-1);

  /* Dissolve the two images according to the dissolve parameter */
  for(yi=0; yi<ny; yi++) {
    for(xi=0; xi<nx; xi++) {
      rsi = (1.0-t) * siP->ri[yi * nx + xi];
      gsi = (1.0-t) * siP->gi[yi * nx + xi];
      bsi = (1.0-t) * siP->bi[yi * nx + xi];
      asi = (1.0-t) * siP->ai[yi * nx + xi];
      if((diP!=NULL) && (xi<diP->ncols) && (yi < diP->nrows)) {
        rdi = t * diP->ri[yi * diP->ncols + xi];
        gdi = t * diP->gi[yi * diP->ncols + xi];
        bdi = t * diP->bi[yi * diP->ncols + xi];
        adi = t * diP->ai[yi * diP->ncols + xi];
      } else {
        rdi = 0;
        gdi = 0;
        bdi = 0;
        adi = 0;
      }
      img.ri[yi*nx+xi] = (int)(rsi + rdi + 0.5);
      img.gi[yi*nx+xi] = (int)(gsi + gdi + 0.5);
      img.bi[yi*nx+xi] = (int)(bsi + bdi + 0.5);
      img.ai[yi*nx+xi] = (int)(asi + adi + 0.5);
    }
  }

  /* Open the output image file for binary writing */
  if(fn!=NULL && (outfP=fopen(fn, "wb"))==NULL) {
    fprintf(stderr, "save_img: could not open '%s' for output\n", fn);
    return(-1);
  }

  /* Set the image header */
    /* Targa */
    tga_hdr.id_len = 0;

    /* cmap_type depends on the img_type */
    tga_hdr.cmap_type = 0;

    /* img_type comes from the user */
    tga_hdr.img_type = TGA_RGB;

    if(img.compressed) tga_hdr.img_type += TGA_RLE;

    tga_hdr.cmap_index = 0;

    /* cmap_len depends on the img_type and pixel_size */
    tga_hdr.cmap_len = 0;

    /* cmap_size depends on the img_type and pixel_size */
    tga_hdr.cmap_size = 0;

    tga_hdr.x_off = 0;
    tga_hdr.y_off = 0;

    /* pixel_size depends on the img_type */
    tga_hdr.pixel_size = img.pixel_size;

    tga_hdr.att_bits = 0;
    tga_hdr.reserved = 0;
    tga_hdr.origin_bit = 0;
    tga_hdr.interleave = TGA_IL_None;

  /* Save the image header */
    /* Targa */
    if(tga_return = save_tga_header(&tga_hdr, &img, outfP)) {
      fprintf(stderr, "save_tga_header returned %i\n", tga_return);
      return(tga_return);
    }

  /* Save the dissolved image */
    /* Targa */
    save_tga_image(&tga_hdr, &img, outfP);

  /* Free the dissolved image */
  free_image(&img);

  /* Close the output image file */
  fclose(outfP);

  return(0);
}




/* --------------------------------------------------------------- */

#define WHITE 0
#define BLACK 1

/* dither_image: dissolve 2 images and store into ximage
**
** t: determines the percent of dissolve between images.
** -- 0.0 means source image.
** -- 1.0 means dest image.
** -- 0.5 means halfway between, etc.
**
** brite: brightness factor applied to dissolved image, for dimming.
*/
void
dither_image(Visual *visual, rgba_image_t *srcP, rgba_image_t *dstP, double t, double brite, XImage *ximage)
{
  int    xi, yi;         /* image coordinate indices */
  int    re=0;           /* dithering error in red channel */
  int    ge=0;           /* dithering errir in green channel or gray */
  int    be=0;           /* dithering error in blue channel */
  UCHAR  byte = 0;       /* 8 bits */
  int    *this_err=NULL; /* error diffusion array */
  int    *next_err=NULL; /* error diffusion array */


  /* 24bit variables added WA (MJG 13sep95) */
  int           rShift, gShift, bShift, rRange, gRange, bRange;

  /* 24bit color code added WA (MJG 13sep95) */
  /* Based on xpaint sources: palette.c */
  if (visual->class == TrueColor) {
    int v;
    rShift = gShift = bShift = 0;
    for (v = visual->red_mask; (v & 1) == 0; v >>= 1) rShift++;
    for (rRange=0; v; v>>=1) rRange++;
    for (v = visual->green_mask; (v & 1) == 0; v >>= 1) gShift++;
    for (gRange=0; v; v>>=1) gRange++;
    for (v = visual->blue_mask; (v & 1) == 0; v >>= 1) bShift++;
    for (bRange=0; v; v>>=1) bRange++;
  } else if (visual->class == StaticGray) {
    /* Allocate error diffusion array */
    this_err=MY_CALLOC(srcP->ncols, int, "dither_image");
    next_err=MY_CALLOC(srcP->ncols, int, "dither_image");
  }

  /* Dissolve and dither the image */
  /* ----------------------------- */
  for(yi=0; yi<srcP->nrows; yi++) {
    for(xi=0; xi<srcP->ncols; xi++) {
      int   rsi, gsi, bsi;  /* source image pixel channels */
      int   rdi, gdi, bdi;  /* dest image pixel channels */
      int   ri, gi, bi;     /* dissolved image pixel channels */
      Pixel pixel;          /* ximage pixel value */
                            /* changed from int to Pixel: WA (MJG 13sep95) */

      /* Assign source image pixel channel values */
      /* ---------------------------------------- */
      rsi = (1.0-t) * srcP->ri[yi * srcP->ncols + xi];
      gsi = (1.0-t) * srcP->gi[yi * srcP->ncols + xi];
      bsi = (1.0-t) * srcP->bi[yi * srcP->ncols + xi];

      /* Assign dest image pixel channel values */
      /* -------------------------------------- */
      if((dstP!=NULL) && (xi<dstP->ncols) && (yi < dstP->nrows)) {
        rdi = t * dstP->ri[yi * dstP->ncols + xi];
        gdi = t * dstP->gi[yi * dstP->ncols + xi];
        bdi = t * dstP->bi[yi * dstP->ncols + xi];
      } else {
        rdi = 0;
        gdi = 0;
        bdi = 0;
      }

      /* Dissolve and dim source and dest images */
      /* --------------------------------------- */
      ri = (int)(brite * (rsi + rdi) + 0.5);
      gi = (int)(brite * (gsi + gdi) + 0.5);
      bi = (int)(brite * (bsi + bdi) + 0.5);


      /* ======================================= */
      /* Dither and store rgba_image into ximage */
      /* ======================================= */

      /* TrueColor code due to WA (MJG 13sep95) */
      if (visual->class == TrueColor) {
        /* True color requires no dithering */
        pixel = (((ri<<rRange)>>8) << rShift)
               |(((gi<<gRange)>>8) << gShift)
               |(((bi<<bRange)>>8) << bShift);
        /* Store TrueColor pixel in image */
        XPutPixel(ximage, xi, yi, pixel);

      } else if (visual->class == PseudoColor) {
        UCHAR  rb, gb, bb;    /* dithered image pixel channels */

        /* assume 8-bit PseudoColor */
        if(ximage->bits_per_pixel != 8) {
          fprintf(stderr,
          "dither_image: assuming 8 bit per pixel but ximage has %i\n",
          ximage->bits_per_pixel);
        }

        /* Add dithering error from previous iteration */
        /* ------------------------------------------- */
        ri += re;  re = 0;
        gi += ge;  ge = 0;
        bi += be;  be = 0;


        /* Truncate pixel channel values to fit inside limits */
        /* -------------------------------------------------- */
        if(ri>RGBA_IMAGE_MAXVAL) {
          re += ri - RGBA_IMAGE_MAXVAL;/**/
          ri = RGBA_IMAGE_MAXVAL;
        } else if(ri<0) {
          re -= ri;
          ri = 0;
        }

        if(gi>RGBA_IMAGE_MAXVAL) {
          ge += gi - RGBA_IMAGE_MAXVAL;/**/
          gi = RGBA_IMAGE_MAXVAL;
        } else if(gi<0) {
          ge -= gi;
          gi = 0;
        }

        if(bi>RGBA_IMAGE_MAXVAL) {
          be += bi - RGBA_IMAGE_MAXVAL;/**/
          bi = RGBA_IMAGE_MAXVAL;
        } else if(bi<0) {
          be -= bi;
          bi = 0;
        }

        /* Approximate true color */
        /* ---------------------- */
        /* Pick out the bits that will make up the pixel index */
        /* Use only most significant bits per channel */
        rb = ri & RED_MSB_MASK;
        gb = gi & GRN_MSB_MASK;
        bb = bi & BLU_MSB_MASK;

        /* Construct a pixel index from the pixel color */
        pixel = (rb>>RED_R_SHIFT)|(gb>>GRN_R_SHIFT)|(bb>>BLU_R_SHIFT);

        /* Store dithering error for next iteration */
        re += ri - (diw_xcolors[pixel].red   >> TRUNC_SHIFT);
        ge += gi - (diw_xcolors[pixel].green >> TRUNC_SHIFT);
        be += bi - (diw_xcolors[pixel].blue  >> TRUNC_SHIFT);

        /* Store pixel index in colormapped image */
        /* MJG 19jul94: fixed indexing */
        /* MJG 12jul95: use bytes_per_line instead of width to index */
        ximage->data[yi * ximage->bytes_per_line + xi] =
          diw_xcolors[pixel].pixel;

      } else if (visual->class == GrayScale) {
        fprintf(stderr, "dither_image: GrayScale not implemented\n");

      } else if (visual->class == StaticGray) {
        int    gray;  /* gray value of pixel */
        UCHAR  gb;    /* dithered image pixel gray value */
        int    xi_xi; /* ximage x index */
        int    bit;   /* bit index for byte fraction depths */

        /* Assume 1-bit depth */
        if(ximage->bits_per_pixel != 1) {
          fprintf(stderr,
          "dither_image: assuming 1 bit per pixel but ximage has %i\n",
          ximage->bits_per_pixel);
        }

        /* Convert color into gray */
        /* ----------------------- */
        gray = 0.299 * ri + 0.587 * gi + 0.114 * bi;


        /* Add dithering error from previous iteration */
        /* ------------------------------------------- */
        gray += this_err[xi];  this_err[xi] = 0;

        /* Truncate pixel value to fit inside limits */
        /* ----------------------------------------- */
        ge = 0;
        if(gray > RGBA_IMAGE_MAXVAL) {
          ge += gray - RGBA_IMAGE_MAXVAL;
          gray = RGBA_IMAGE_MAXVAL;
        } else if(gray < 0) {
          ge -= gray;
          gray = 0;
        }

        /* Approximate a gray pixel value and compute dithering error */
        /* ---------------------------------------------------------- */
        if(gray > GRAY_THRESHOLD) {
          gb = WHITE;
          ge += gray - RGBA_IMAGE_MAXVAL;
        } else {
          gb = BLACK;
          ge += gray;
        }

        /* Diffuse and store dithering error */
        /* --------------------------------- */
        next_err[xi]     += ( ge * 5 ) / 16;

        if(xi+1 < srcP->ncols) {
          this_err[xi+1] += ( ge * 5 ) / 16;
          next_err[xi+1] += ( ge * 3 ) / 16;
        } else {
          next_err[xi]   += ( ge * 8 ) / 16;
        }

        if(xi-1 >= 0) {
          next_err[xi-1] += ( ge * 3 ) / 16;
        } else {
          next_err[xi]   += ( ge * 3 ) / 16;
        }

        /* Compute bit index and ximage index */
        /* ---------------------------------- */
        xi_xi = xi / 8;
        if(((bit = xi % 8) == 0) && (xi_xi > 0)) {
          /* Store the accumulated 8 bits into the previous ximage byte */
          ximage->data[yi * ximage->bytes_per_line + xi_xi-1] = byte;

          /* Restart the bit accumulator */
          byte = 0;
        }

        /* Store the bit in the right place in the byte */
        /* -------------------------------------------- */
        /* Note that bit 0 goes on the left of the byte
        ** and that bit 7 goes on the right of the byte.
        */
        byte |= (gb << (7-bit));

      } else {
        fprintf(stderr, "dither_image: visual class %i not supported\n",
          visual->class);
      }
    } /* for xi */

    /* Swap error diffusion arrays */
    if(this_err != NULL) {
      int *tmp = this_err;
      this_err = next_err;
      next_err = tmp;
    }
  } /* for yi */

  /* Free error diffusion arrays */
  if(this_err != NULL) my_free(this_err, "dither_image");
  if(next_err != NULL) my_free(next_err, "dither_image");
}




/* --------------------------------------------------------------- */

/* make_test_image: generate a test image
**
** Uses the incoming values of ncols and nrows to determine image size.
** If ncols or nrows are zero, default values are used instead.
**
** type determines which test image to use.
**
** Memory for the images is allocated and imgP is set.
*/
int
make_test_image(rgba_image_t *imgP, int type)
{
  register int   xi, yi;
  register UCHAR p;

  if(imgP->ncols <= 0) imgP->ncols = 320;
  if(imgP->nrows <= 0) imgP->nrows = 240;
  imgP->compressed = 1;
  imgP->color_mapped = 0;
  imgP->pixel_size = 24;
  imgP->type = TARGA_MAGIC;

  if(alloc_image(imgP, "make_test_image")) return(1);

  for(yi=0; yi<imgP->nrows; yi++) {
    for(xi=0; xi<imgP->ncols; xi++) {
      p = 15 + 240*((float)xi/imgP->ncols)*((float)yi/imgP->nrows);
      if((xi%40>20 && yi%40<20) || (xi%40<20 && yi%40>20))
        p=0;

      if(type & 1) {
        imgP->ri[yi*(imgP->ncols) + xi] = p;
      } else {
        imgP->ri[yi*(imgP->ncols) + xi] = RGBA_IMAGE_MAXVAL - p;
      }

      if(type & 2) {
        imgP->gi[yi*(imgP->ncols) + xi] = p;
      } else {
        imgP->gi[yi*(imgP->ncols) + xi] = RGBA_IMAGE_MAXVAL - p;
      }

      if(type & 4) {
        imgP->bi[yi*(imgP->ncols) + xi] = p;
      } else {
        imgP->bi[yi*(imgP->ncols) + xi] = RGBA_IMAGE_MAXVAL - p;
      }

      imgP->ai[yi*(imgP->ncols) + xi] = RGBA_IMAGE_OPAQUE;
    }
  }
  return(0);
}
