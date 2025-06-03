
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
#include <stdio.h>
#include <sys/types.h>
#include "xanim.h"
#include "xanim_utils.h"

#define ACT_IMAGE8 0
#define ACT_IMAGE4 0
#define ACT_IMAGE2 0

UTIL_Sub_Image(out,in,xosize,yosize,xipos,yipos,xisize)
UBYTE *out,*in;
ULONG xosize,yosize,xipos,yipos,xisize;
{
 ULONG y,x;
 UBYTE *in_ptr;

 for(y=yipos; y < (yipos + yosize); y++)
 {
  in_ptr = (UBYTE *)( (ULONG)(in) + y * xisize + xipos);
  x = xosize;
  while(x--) *out++ = *in_ptr++;
 } 
}

UTIL_Create_Clip(out,in,pix_mask,xsize,ysize)
UBYTE *out,*in,pix_mask;
LONG xsize,ysize;
{
 ULONG bit_mask,x,y;

 for(y=0; y<ysize; y++)
 {
  x = 0;
  bit_mask = 0x01;
  *out = 0;
  while(x < xsize)
  {
   if (*in++ != pix_mask) *out |= bit_mask;
   x++;
   bit_mask <<= 1;
   if (bit_mask > (ULONG)(0x80)) 
   {
    bit_mask = 0x01;
    out++;
    *out = 0;
   }
  } /* end of while x */
  if (bit_mask != 0x01) out++;
 } /* end of for y */
}

Pack_IMAGE8(picout,picin,x_size, y_size, depth)
UBYTE *picout,*picin;
ULONG x_size,y_size,depth;
{
 ULONG pix_per_byte,pix_mask,type,pix_shift;
 ULONG pimagex;
 ULONG pad_x_size,pad_image_size,y,x,i;
 UBYTE *po,*pi;
 

  switch(depth)
  {
   case 8: 
   case 7:
   case 6:
   case 5:
	   fprintf(stderr,"Pack_IMAGE8: why are we packing %ld?\n",depth);
	   pix_per_byte = 1;
	   pix_shift = 0;
	   pix_mask = 0xff;
	   type = ACT_IMAGE8;
	   break;
   case 4:
   case 3:
	   pix_per_byte = 2;
	   pix_shift = 4;
	   pix_mask = 0x0f;
	   type = ACT_IMAGE4;
	   break;
   case 2:
	   pix_per_byte = 4;
	   pix_shift = 2;
	   pix_mask = 0x03;
	   type = ACT_IMAGE2;
	   break;
   case 1:
	   pix_per_byte = 8;
	   pix_shift = 1;
	   pix_mask = 0x01;
	   type = ACT_IMAGE1;
	   break;
   default: fprintf(stderr,"Pack_IMAGE8: illegal image depth %ld\n",depth);
	    break;
  }
 
  /* Pad to byte size */
  pad_x_size = x_size / pix_per_byte;
  if ( x_size % pix_per_byte ) pad_x_size++;

  pad_image_size = y_size * pad_x_size;
  picout = (UBYTE *) malloc( pad_image_size );
  if (picout == 0) TheEnd1("Pack_IMAGE8: malloc error\n");

  po = picout;
  pi = picin;
  for(y=0; y < y_size; y++)
  {
    x = 0;
    while(x < x_size)
    {
      register UBYTE data;

      data = 0;
      for(i=0; i<pix_per_byte; i++)
      {
        data <<= pix_shift;
        data |= (*picin++) & pix_mask;
      }
      *po++ = data;
      x++;
    }
    while(x < pad_x_size)
    {
      *po++ = 0x00;
      x++;
    }
  } /* end of y */
}



