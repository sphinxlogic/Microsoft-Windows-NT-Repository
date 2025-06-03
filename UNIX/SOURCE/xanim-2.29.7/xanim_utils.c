
/*
 * xanim_utils.c
 *
 * Copyright (C) 1991,1992 by Mark Podlipec. 
 * All rights reserved.
 *
 * This software may be freely copied, modified and redistributed
 * without fee provided that this copyright notice is preserved 
 * intact on all copies and modified copies.
 * 
 * There is no warranty or other guarantee of fitness of this software.
 * It is provided solely "as is". The author(s) disclaim(s) all
 * responsibility and liability with respect to this software's usage
 * or its effect upon hardware or computer systems.
 *
 */
#include "xanim.h"
#include "xanim_utils.h"

void UTIL_Sub_Image();
void UTIL_Create_Clip();
void UTIL_Sub_Mapped_To_True();
void UTIL_Mapped_To_Bitmap();
void UTIL_Half_Image_Height();
void UTIL_Pack_Image();


void
UTIL_Half_Image_Height(out,in,xpos,ypos,xsize,ysize,line_size)
UBYTE *out,*in;
ULONG xpos,*ypos,xsize,*ysize,line_size;
{
  ULONG t_ysize, t_ypos, y_start, j;
  register ULONG y,start;
  register UBYTE *src;

  t_ypos  = (*ypos);
  t_ysize = (*ysize);

  if (t_ypos & 0x01) y_start = 1;	/* start on 2nd in line */
  else y_start = 0;			/* start on 1st in line */

  start = (ULONG)in;
  j=0;
  for(y = y_start; y < t_ysize; y+=2)
  {
    src = (UBYTE *)(start + y * line_size);
    memcpy( (char *)out, (char *)src, line_size);
    out += line_size;
    j++;
  }
  *ypos  = (t_ypos + 1)  >> 1;
  *ysize = j;
}

void
UTIL_Sub_Image(out,in,xosize,yosize,xipos,yipos,xisize,pix_size)
UBYTE *out,*in;
ULONG xosize,yosize,xipos,yipos,xisize,pix_size;
{
 ULONG y,x;
 UBYTE *in_ptr;
 xosize *= pix_size;
 xisize *= pix_size;
 xipos  *= pix_size;
 for(y=yipos; y < (yipos + yosize); y++)
 {
  in_ptr = (UBYTE *)( (ULONG)(in) + y * xisize + xipos);
  x = xosize;
  while(x--) *out++ = *in_ptr++;
 } 
}

void
UTIL_Sub_Mapped_To_True(out,in,map,xosize,yosize,xipos,yipos,xisize)
UBYTE *out,*in;
ColorReg *map;
ULONG xosize,yosize,xipos,yipos,xisize;
{
  ULONG y,x;
  UBYTE *in_ptr;
  for(y=yipos; y < (yipos + yosize); y++)
  {
    in_ptr = (UBYTE *)( (ULONG)(in) + y * xisize + xipos);
    x = xosize;
    if (x11_bytes_pixel == 4)
    { 
      ULONG *ulp = (ULONG *)out;
      while(x--) *ulp++ = (ULONG)(map[*in_ptr++].map);
      out = (UBYTE *)ulp;
    }
    else if (x11_bytes_pixel == 2)
    {
      USHORT *usp = (USHORT *)out;
      while(x--) *usp++ = (USHORT)(map[*in_ptr++].map);
      out = (UBYTE *)usp;
    }
    else while(x--) *out++ = (UBYTE)(map[*in_ptr++].map);
 } 
}

void
UTIL_Create_Clip(out,in,pix_mask,xsize,ysize,pix_size)
UBYTE *out,*in;
ULONG pix_mask,xsize,ysize,pix_size;
{
  register ULONG data_in,bit_mask,x,y,mask_start,mask_end;
  register UBYTE data_out;

/* NOTE: should probably look at bit order or are CLIPs always LSB */

  mask_start = 0x01;
  mask_end = 0x80;

  for(y=0; y<ysize; y++)
  {
    bit_mask = mask_start;
    data_out = 0;
    x = xsize;
    while(x--)
    {
      if (pix_size == 4) 
	{ ULONG *ulp = (ULONG *)in;	data_in = *ulp; }
      else if (pix_size == 2)
	{ USHORT *usp = (USHORT *)in;	data_in = (ULONG)(*usp); }
      else				data_in = (ULONG)(*in);
      in += pix_size;

      if (data_in != pix_mask) data_out |= bit_mask;

      if (bit_mask == mask_end)
      {
        *out++ = data_out;
        bit_mask = mask_start;
	data_out = 0;
      }
      else if (mask_start == 0x01) bit_mask <<= 1;
           else bit_mask >>= 1;
    } /* end of while x */
    if (bit_mask != mask_start) *out++ = data_out;
  } /* end of for y */
}

void
UTIL_Pack_Image(out,in,xsize,ysize,pk_size)
UBYTE *out,*in;
ULONG xsize,ysize,pk_size;
{
 int i;
}


void
UTIL_Mapped_To_Bitmap(out,in,map,csize,xsize,ysize,line_size)
UBYTE *out,*in;
ColorReg *map;
ULONG csize, xsize, ysize, line_size;
{
  ULONG y,flag;
  register ULONG mask,invert,x,mask_start,mask_end;
  SHORT *imap,*c_ptr,*n_ptr,*t_ptr,*err_buff,err,threshold;
  UBYTE data,*o_ptr;
 
  err_buff = (SHORT *)malloc(xsize * 2 * sizeof(short));
  imap     = (SHORT *)malloc(csize * sizeof(short));

  for(x=0; x<csize; x++)
    imap[x] = X11_Get_Monochrome(map[x].red,map[x].green,map[x].blue);

  for(x = 0; x < xsize; x++) err_buff[x] = (USHORT)imap[ *in++ ];
  flag = 0;

  /* used to invert image */
  if (x11_white & 0x01) invert = 0x00;
  else invert = ~0x00;
  threshold = 128;

  if (x11_bit_order == X11_MSB) { mask_start = 0x80; mask_end = 0x01; }
  else { mask_start = 0x01; mask_end = 0x80; }

  for(y = 0; y < ysize; y++)
  {
    o_ptr = (UBYTE *)( (ULONG)(out) + line_size * y);
    /* double buffer error arrays */
    if (flag == 0) 
    {
      c_ptr = err_buff;
      n_ptr = (SHORT *)( (ULONG)(err_buff) + xsize * sizeof(short) );
      flag = 1;
    }
    else 
    {
      n_ptr = err_buff;
      c_ptr = (SHORT *)( (ULONG)(err_buff) + xsize * sizeof(short) );
      flag = 0;
    }

    data = 0x00;
    mask = mask_start;
    if (y < (ysize - 1) )  n_ptr[0] = (USHORT)imap[ *in++ ];

    for(x=0; x<xsize; x++)
    {
      if (*c_ptr >= threshold) { err = *c_ptr - 255; data |= mask; } 
      else  err = *c_ptr;

      if (mask == mask_end) 
	{ *o_ptr++ = data^invert; data = 0x00; mask = mask_start; }
      else if (mask_start == 0x80) mask >>= 1;
           else mask <<= 1;
      c_ptr++;

      if (dither_flag == FALSE)
      {
        if (x < (xsize - 1) ) *n_ptr++ = imap[ *in++ ];
      }
      else
      {
        if (x < (xsize - 1) )  *c_ptr += (7 * err)/16;
        if (y < (ysize - 1) )
        {
          if (x > 0) *n_ptr++ += (3 * err)/16;
          *n_ptr++ += (5 * err)/16;
          if (x < (xsize - 1) ) 
          { *n_ptr = (USHORT)(imap[*in++]) + (SHORT)(err/16); n_ptr--; }
        }
      }
    }
    if (mask != mask_start) *o_ptr++ = data^invert; /* send out partial */

  }
  free(err_buff);
  free(imap);
}

/* Routine to read a little endian long word.
 */
ULONG UTIL_Get_LSB_Long(fp)
FILE *fp;
{
 ULONG ret;

 ret =  fgetc(fp);
 ret |= fgetc(fp) << 8;
 ret |= fgetc(fp) << 16;
 ret |= fgetc(fp) << 24;
 return ret;
}

/* Routine to read a little endian half word.
 */
ULONG UTIL_Get_LSB_Short(fp)
FILE *fp;
{
 ULONG ret;

 ret =  fgetc(fp);
 ret |= fgetc(fp) << 8;
 return ret;
}

/* Routine to read a big endian long word.
 */
ULONG UTIL_Get_MSB_Long(fp)
FILE *fp;
{
 ULONG ret;

 ret  = fgetc(fp) << 24;
 ret |= fgetc(fp) << 16;
 ret |= fgetc(fp) << 8;
 ret |=  fgetc(fp);
 return ret;
}

/* Routine to read a big endian half word.
 */
ULONG UTIL_Get_MSB_Short(fp)
FILE *fp;
{
 ULONG ret;

 ret  =  fgetc(fp) << 8;
 ret |=  fgetc(fp);
 return ret;
}
