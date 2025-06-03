/* Copyright (C) 1992 Aladdin Enterprises.  All rights reserved.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* gdevmem3.c */
/* 2- and 4-bit-per-pixel "memory" (stored bitmap) devices */
/* for Ghostscript library. */
#include "memory_.h"
#include "gx.h"
#include "gxdevice.h"
#include "gxdevmem.h"			/* semi-public definitions */
#include "gdevmem.h"			/* private definitions */

/* The current implementations are quite inefficient. */
/* We intend to improve them someday.... */

/* ------ Generic procedures ------ */

/* Import the color mapping procedures from gdevmem2. */
extern dev_proc_map_rgb_color(mem_mapped_map_rgb_color);
extern dev_proc_map_color_rgb(mem_mapped_map_color_rgb);

/* We do tiling in big chunks. */
#define chunk mono_chunk

/* Implement fill_rectangle by tiling. */
/* This uses the same algorithm as mem_mono_fill_rectangle. */
/* Note that x and w are in bit units, not pixel units. */
#define x_to_byte(x) ((x) >> 3)
private int
fill_2or4_by_tiling(gx_device *dev, int x, int y, int w, int h,
  const ulong pattern)
{	uint bit;
	chunk right_mask;
	declare_scan_ptr(dest);
	setup_rect(dest);
#define write_loop(stat)\
 { int line_count = h;\
   chunk *ptr = dest;\
   do { stat; inc_chunk_ptr(ptr, draster); }\
   while ( --line_count );\
 }
#define write_partial(msk)\
   write_loop(*ptr = (*ptr & ~msk) | (pattern & msk))
	bit = x & chunk_align_bit_mask;
	if ( bit + w <= chunk_bits )
	   {	set_mono_thin_mask(right_mask, w, bit);
	   }
	else
	   {	int byte_count;
		if ( bit )
		   {	chunk mask;
			set_mono_left_mask(mask, bit);
			write_partial(mask);
			dest++;
			w += bit - chunk_bits;
		   }
		set_mono_right_mask(right_mask, w & chunk_bit_mask);
		if ( (byte_count = (w >> 3) & -chunk_bytes) != 0 )
		   {	write_loop(memset(ptr, (byte)pattern, byte_count));
			inc_chunk_ptr(dest, byte_count);
		   }
	   }
	if ( right_mask )
		write_partial(right_mask);
	return 0;
}

/* We do everything else byte-by-byte. */
#undef chunk
#define chunk byte

/* ------ Mapped 2-bit color ------ */

/* Procedures */
declare_mem_procs(mem_mapped2_copy_mono, mem_mapped2_copy_color, mem_mapped2_fill_rectangle);

/* The device descriptor. */
private gx_device_procs mem_mapped2_procs =
  mem_procs(mem_mapped_map_rgb_color, mem_mapped_map_color_rgb,
    mem_mapped2_copy_mono, mem_mapped2_copy_color, mem_mapped2_fill_rectangle);

/* The instance is public. */
const gx_device_memory mem_mapped2_color_device =
  mem_device("image(2)", 2, mem_mapped2_procs);

/* Convert x coordinate to byte offset in scan line. */
#undef x_to_byte
#define x_to_byte(x) ((x) >> 2)

/* Fill a rectangle with a color. */
private int
mem_mapped2_fill_rectangle(gx_device *dev,
  int x, int y, int w, int h, gx_color_index color)
{	int code;
	fit_fill(dev, x, y, w, h);
	/* Patch the width in the device temporarily. */
	dev->width <<= 1;
	if ( color == 0 || color == 3 )
	   {	/* Use monobit fill_rectangle. */
		code = (*mem_mono_device.procs->fill_rectangle)
		  (dev, x << 1, y, w << 1, h, color & 1);
	   }
	else
	   {	/* Use monobit tile_rectangle. */
		static const ulong tile_patterns[4] =
		   {	0, 0x55555555, 0xaaaaaaaa, 0xffffffff
		   };
		code = fill_2or4_by_tiling(dev, x << 1, y, w << 1, h,
					   tile_patterns[color]);
	   }
	/* Restore the correct width. */
	dev->width >>= 1;
	return code;
}

/* Copy a bitmap. */
private int
mem_mapped2_copy_mono(gx_device *dev,
  const byte *base, int sourcex, int sraster, gx_bitmap_id id,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	const byte *line;
	int first_bit;
	byte first_mask, b0, b1;
	static byte btab[4] = { 0, 0x55, 0xaa, 0xff };
	static byte bmask[4] = { 0xc0, 0x30, 0xc, 3 };
	declare_scan_ptr(dest);
	fit_copy(dev, base, sourcex, sraster, id, x, y, w, h);
	setup_rect(dest);
	line = base + (sourcex >> 3);
	first_bit = 0x80 >> (sourcex & 7);
	first_mask = bmask[x & 3];
	b0 = btab[zero & 3];
	b1 = btab[one & 3];
	while ( h-- > 0 )
	   {	register byte *pptr = (byte *)dest;
		const byte *sptr = line;
		register int sbyte = *sptr++;
		register int bit = first_bit;
		register byte mask = first_mask;
		int count = w;
		do
		   {	if ( sbyte & bit )
			   {	if ( one != gx_no_color_index )
				  *pptr = (*pptr & ~mask) + (b1 & mask);
			   }
			else
			   {	if ( zero != gx_no_color_index )
				  *pptr = (*pptr & ~mask) + (b0 & mask);
			   }
			if ( (bit >>= 1) == 0 )
				bit = 0x80, sbyte = *sptr++;
			if ( (mask >>= 2) == 0 )
				mask = 0xc0, pptr++;
		   }
		while ( --count > 0 );
		line += sraster;
		inc_chunk_ptr(dest, draster);
	   }
	return 0;
}

/* Copy a color bitmap. */
private int
mem_mapped2_copy_color(gx_device *dev,
  const byte *base, int sourcex, int sraster, gx_bitmap_id id,
  int x, int y, int w, int h)
{	int code;
	fit_copy(dev, base, sourcex, sraster, id, x, y, w, h);
	/* Use monobit copy_mono. */
	/* Patch the width in the device temporarily. */
	dev->width <<= 1;
	code = (*mem_mono_device.procs->copy_mono)
	  (dev, base, sourcex << 1, sraster, id,
	   x << 1, y, w << 1, h, (gx_color_index)0, (gx_color_index)1);
	/* Restore the correct width. */
	dev->width >>= 1;
	return code;
}

/* ------ Mapped 4-bit color ------ */

/* Procedures */
declare_mem_procs(mem_mapped4_copy_mono, mem_mapped4_copy_color, mem_mapped4_fill_rectangle);

/* The device descriptor. */
private gx_device_procs mem_mapped4_procs =
  mem_procs(mem_mapped_map_rgb_color, mem_mapped_map_color_rgb,
    mem_mapped4_copy_mono, mem_mapped4_copy_color, mem_mapped4_fill_rectangle);

/* The instance is public. */
const gx_device_memory mem_mapped4_color_device =
  mem_device("image(4)", 4, mem_mapped4_procs);

/* Convert x coordinate to byte offset in scan line. */
#undef x_to_byte
#define x_to_byte(x) ((x) >> 1)

/* Fill a rectangle with a color. */
private int
mem_mapped4_fill_rectangle(gx_device *dev,
  int x, int y, int w, int h, gx_color_index color)
{	int code;
	fit_fill(dev, x, y, w, h);
	/* Patch the width in the device temporarily. */
	dev->width <<= 2;
	if ( color == 0 || color == 15 )
	   {	/* Use monobit fill_rectangle. */
		code = (*mem_mono_device.procs->fill_rectangle)
		  (dev, x << 2, y, w << 2, h, color & 1);
	   }
	else
	   {	/* Use monobit tile_rectangle. */
		static const ulong tile_patterns[16] =
		   {	0, 0x11111111, 0x22222222, 0x33333333,
			0x44444444, 0x55555555, 0x66666666, 0x77777777,
			0x88888888, 0x99999999, 0xaaaaaaaa, 0xbbbbbbbb,
			0xcccccccc, 0xdddddddd, 0xeeeeeeee, 0xffffffff
		   };
		code = fill_2or4_by_tiling(dev, x << 2, y, w << 2, h,
					   tile_patterns[color]);
	   }
	dev->width >>= 2;
	return code;
}

/* Copy a bitmap. */
private int
mem_mapped4_copy_mono(gx_device *dev,
  const byte *base, int sourcex, int sraster, gx_bitmap_id id,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	const byte *line;
	int first_bit;
	byte first_mask, b0, b1;
	declare_scan_ptr(dest);
	fit_copy(dev, base, sourcex, sraster, id, x, y, w, h);
	setup_rect(dest);
	line = base + (sourcex >> 3);
	first_bit = 0x80 >> (sourcex & 7);
	first_mask = (x & 1 ? 0xf : 0xf0);
	b0 = ((byte)zero << 4) + (byte)zero;
	b1 = ((byte)one << 4) + (byte)one;
	while ( h-- > 0 )
	   {	register byte *pptr = (byte *)dest;
		const byte *sptr = line;
		register int sbyte = *sptr++;
		register int bit = first_bit;
		register byte mask = first_mask;
		int count = w;
		do
		   {	if ( sbyte & bit )
			   {	if ( one != gx_no_color_index )
				  *pptr = (*pptr & ~mask) + (b1 & mask);
			   }
			else
			   {	if ( zero != gx_no_color_index )
				  *pptr = (*pptr & ~mask) + (b0 & mask);
			   }
			if ( (bit >>= 1) == 0 )
				bit = 0x80, sbyte = *sptr++;
			if ( (mask = ~mask) == 0xf0 )
				pptr++;
		   }
		while ( --count > 0 );
		line += sraster;
		inc_chunk_ptr(dest, draster);
	   }
	return 0;
}

/* Copy a color bitmap. */
private int
mem_mapped4_copy_color(gx_device *dev,
  const byte *base, int sourcex, int sraster, gx_bitmap_id id,
  int x, int y, int w, int h)
{	int code;
	fit_copy(dev, base, sourcex, sraster, id, x, y, w, h);
	/* Use monobit copy_mono. */
	/* Patch the width in the device temporarily. */
	dev->width <<= 2;
	code = (*mem_mono_device.procs->copy_mono)
	  (dev, base, sourcex << 2, sraster, id,
	   x << 2, y, w << 2, h, (gx_color_index)0, (gx_color_index)1);
	/* Restore the correct width. */
	dev->width >>= 2;
	return code;
}
