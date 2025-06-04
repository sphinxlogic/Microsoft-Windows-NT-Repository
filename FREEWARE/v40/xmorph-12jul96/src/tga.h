/* tga.h -- Targa file handling routine stuff
**
** Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/
#ifndef _TGA_H__INCLUDED_
#define _TGA_H__INCLUDED_

#include <stdio.h>

#include "image.h"

typedef struct tga_hdr_ {
  unsigned char id_len;
  unsigned char cmap_type;
  unsigned char img_type;
  int           cmap_index;
  int           cmap_len;
  unsigned char cmap_size; /* cmap entry size in bits */
  int           x_off;
  int           y_off;
  unsigned char pixel_size;
  unsigned char att_bits;
  unsigned char reserved;
  unsigned char origin_bit;   /* origin location: 0=lower 1=upper */
  unsigned char interleave;
  int           mapped;  /* whether image is colormapped (not in file) */
} tga_hdr_t;

/* Targe image types */
#define TGA_Null     0
#define TGA_Map      1
#define TGA_RGB      2
#define TGA_Mono     3
#define TGA_RLE_Map  9
#define TGA_RLE_RGB  10
#define TGA_RLE_Mono 11
#define TGA_CompMap  32
#define TGA_CompMap4 33

/* TGA_RLE is not an image type, but just a value that means "RLE" is used */
#define TGA_RLE      8

/* Interleave flag values */
#define TGA_IL_None 0
#define TGA_IL_Two  1
#define TGA_IL_Four 2

#define TARGA_MAGIC 'T' + 256 * 'G'

/* ---------------------------------------------------------------------- */

extern rgba_image_t tga_cmap;

/* ---------------------------------------------------------------------- */

/* load_tga_image: Load a Targa image file */
extern int load_tga_image(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio);


/* load_tga_header: load a Targa image header */
extern int load_tga_header(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio);

/* save_tga_image: Save a Targa image file into fio from imgP */
extern int save_tga_image(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio);

/* save_tga_header: save a Targa image header into fio from tgaP and imgP */
extern int save_tga_header(tga_hdr_t *tgaP, rgba_image_t *imgP, FILE *fio);

#endif /* _TGA_H__INCLUDED_ */
