/* Copyright (C) 1989, 1992 Aladdin Enterprises.  All rights reserved.

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

/* gdevprn.h */
/* Common header file for memory-buffered printers */

#include "memory_.h"
#include "string_.h"
#include "gx.h"
#include "gserrors.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxdevice.h"
#include "gxdevmem.h"
#include "gxclist.h"

/* Define the page size parameters. */
/* U.S. letter paper (8.5" x 11"). */
#define DEFAULT_WIDTH_10THS_US_LETTER 85
#define DEFAULT_HEIGHT_10THS_US_LETTER 110
/* A4 paper (210mm x 297mm).  The dimensions are off by a few mm.... */
#define DEFAULT_WIDTH_10THS_A4 83
#define DEFAULT_HEIGHT_10THS_A4 117
/* Choose a default.  A4 may be set in the makefile. */
#ifdef A4
#  define DEFAULT_WIDTH_10THS DEFAULT_WIDTH_10THS_A4
#  define DEFAULT_HEIGHT_10THS DEFAULT_HEIGHT_10THS_A4
#else
#  define DEFAULT_WIDTH_10THS DEFAULT_WIDTH_10THS_US_LETTER
#  define DEFAULT_HEIGHT_10THS DEFAULT_HEIGHT_10THS_US_LETTER
#endif

/* Define the parameters for the printer rendering method. */
/* If the entire bitmap fits in PRN_MAX_BITMAP, and there is at least */
/* PRN_MIN_MEMORY_LEFT memory left after allocating it, render in RAM, */
/* otherwise use a command list with a size of PRN_BUFFER_SPACE. */
/* (These are "properties" that can be changed by a Ghostscript program.) */
#if arch_ints_are_short
/* 16-bit machines have little dinky RAMs.... */
#  define PRN_MAX_BITMAP 32000
#  define PRN_BUFFER_SPACE 25000
#  define PRN_MIN_MEMORY_LEFT 32000
#else
/* 32-bit machines have great big hulking RAMs.... */
#  define PRN_MAX_BITMAP 10000000L
#  define PRN_BUFFER_SPACE 1000000L
#  define PRN_MIN_MEMORY_LEFT 500000L
#endif
#define PRN_MIN_BUFFER_SPACE 10000	/* give up if less than this */

/* Define the declaration macro for print_page procedures. */
#define dev_proc_print_page(proc)\
  int proc(P2(gx_device_printer *, FILE *))

/* Structure for generic printer devices. */
/* This must be preceded by gx_device_common. */
/* Printer devices are actually a union of a memory device */
/* and a clist device, plus some additional state. */
#define prn_fname_sizeof 80
#define gx_prn_device_common\
	byte skip[max(sizeof(gx_device_memory), sizeof(gx_device_clist)) -\
		  sizeof(gx_device) + sizeof(double) /* padding */];\
	/* The following is required only for devices where */\
	/* output_page is gdev_prn_output_page; */\
	/* it is ignored for other devices. */\
	dev_proc_print_page((*print_page));\
		/* ------ The following items must be set before ------ */\
		/* ------ calling the device open routine. ------ */\
	long max_bitmap;		/* max size of non-buffered bitmap */\
	long use_buffer_space;		/* space to use for buffer */\
	char fname[prn_fname_sizeof];	/* output file name */\
		/* ------ End of preset items ------ */\
	int file_is_new;	/* boolean, true iff file just opened */\
	FILE *file;			/* output file */\
	char ccfname[60];		/* clist file name */\
	FILE *ccfile;			/* command list scratch file */\
	char cbfname[60];		/* clist block file name */\
	FILE *cbfile;			/* command list block scratch file */\
	long buffer_space;	/* amount of space for clist buffer, */\
					/* 0 means not using clist */\
	byte *buf;			/* buffer for rendering */\
	int page_count;			/* # of pages printed so far */\
	gx_device_procs *orig_procs;	/* original procs */\
	gx_device_procs mod_procs	/* modified procs */

/* The device descriptor */
typedef struct gx_device_printer_s gx_device_printer;
struct gx_device_printer_s {
	gx_device_common;
	gx_prn_device_common;
};

/* Macro for casting gx_device argument */
#define prn_dev ((gx_device_printer *)dev)

/* Standard device procedures for printers */
dev_proc_open_device(gdev_prn_open);
dev_proc_output_page(gdev_prn_output_page);
dev_proc_close_device(gdev_prn_close);
dev_proc_map_rgb_color(gdev_prn_map_rgb_color);
dev_proc_map_color_rgb(gdev_prn_map_color_rgb);
dev_proc_get_props(gdev_prn_get_props);
dev_proc_put_props(gdev_prn_put_props);

/* Macro for generating procedure table */
#define prn_procs(p_open, p_output_page, p_close)\
  prn_matrix_procs(p_open, gx_default_get_initial_matrix, p_output_page, p_close)
#define prn_matrix_procs(p_open,p_get_initial_matrix, p_output_page, p_close)\
 prn_color_matrix_procs(p_open, p_get_initial_matrix, p_output_page, p_close,\
			 gdev_prn_map_rgb_color, gdev_prn_map_color_rgb)
/* See gdev_prn_open for explanation of the NULLs below. */
#define prn_color_procs(p_open, p_output_page, p_close, p_map_rgb_color, p_map_color_rgb)\
  prn_color_matrix_procs(p_open, gx_default_get_initial_matrix, p_output_page, p_close, p_map_rgb_color, p_map_color_rgb)
#define prn_color_matrix_procs(p_open, p_get_initial_matrix, p_output_page, p_close, p_map_rgb_color, p_map_color_rgb) {\
	p_open,\
	p_get_initial_matrix,\
	NULL,	/* sync_output */\
	p_output_page,\
	p_close,\
	p_map_rgb_color,\
	p_map_color_rgb,\
	NULL,	/* fill_rectangle */\
	NULL,	/* tile_rectangle */\
	NULL,	/* copy_mono */\
	NULL,	/* copy_color */\
	NULL,	/* draw_line */\
	NULL,	/* get_bits */\
	gdev_prn_get_props,\
	gdev_prn_put_props\
}

/* The standard printer device procedures */
/* (using gdev_prn_open/output_page/close). */
extern gx_device_procs prn_std_procs;

/* Macro for generating the device descriptor. */
/*
 * The computations of page width and height in pixels should really be
 *	((int)(page_width_inches*x_dpi))
 * but some compilers (the Ultrix 3.X pcc compiler and the HPUX compiler)
 * can't cast a computed float to an int.  That's why we specify
 * the page width and height in inches/10 instead of inches.
 *
 * Note that the macro is broken up so as to be usable for devices that
 * add further initialized state to the printer device.
 */
#define prn_device_body(devtype, procs, dev_name, width_10ths, height_10ths, x_dpi, y_dpi, l_margin, b_margin, r_margin, t_margin, num_comp, depth, max_gray, max_rgb, dither_gray, dither_rgb, print_page)\
	sizeof(devtype),\
	&procs,\
	dev_name,\
	(int)((long)width_10ths * x_dpi / 10),	/* width */\
	(int)((long)height_10ths * y_dpi / 10),	/* height */\
	x_dpi,\
	y_dpi,\
	l_margin, b_margin, r_margin, t_margin,\
	 { num_comp, depth, max_gray, max_rgb, dither_gray, dither_rgb },\
	0,		/* not initialized yet */\
	  { 0 },	/* skip */\
	print_page,\
	PRN_MAX_BITMAP,\
	PRN_BUFFER_SPACE,\
	  { 0 },	/* fname */\
	0, 0, { 0 }, 0, { 0 }, 0, 0, 0, 0, 0, { 0 }	/* ... mod_procs */
#define prn_device_std_body(devtype, procs, dev_name, width_10ths, height_10ths, x_dpi, y_dpi, l_margin, b_margin, r_margin, t_margin, color_bits, print_page)\
  prn_device_body(devtype, procs, dev_name,\
    width_10ths, height_10ths, x_dpi, y_dpi,\
    l_margin, b_margin, r_margin, t_margin,\
    (color_bits > 1 ? 3 : 1),\
    ((color_bits > 1) & (color_bits < 8) ? 8 : color_bits),\
    (color_bits >= 8 ? 255 : 1),\
    (color_bits >= 8 ? 255 : color_bits > 1 ? 1 : 0),\
    (color_bits >= 8 ? 5 : 2),\
    (color_bits >= 8 ? 5 : color_bits > 1 ? 2 : 0),\
    print_page)
#define prn_device(procs, dev_name, width_10ths, height_10ths, x_dpi, y_dpi, l_margin, b_margin, r_margin, t_margin, color_bits, print_page)\
{ prn_device_std_body(gx_device_printer, procs, dev_name,\
    width_10ths, height_10ths, x_dpi, y_dpi,\
    l_margin, b_margin, r_margin, t_margin,\
    color_bits, print_page)\
}

/* Common procedures defined in gdevprn.c */
int gdev_prn_open_printer(P2(gx_device *dev, int binary_mode));
#define gdev_prn_file_is_new(pdev) ((pdev)->file_is_new)
#define gdev_prn_raster(pdev) gx_device_raster((gx_device *)(pdev), 0)
int gdev_prn_get_bits(P4(gx_device_printer *, int, byte *, byte **));
int gdev_prn_copy_scan_lines(P4(gx_device_printer *, int, byte *, uint));
int gdev_prn_close_printer(P1(gx_device *));

/* BACKWARD COMPATIBILITY */
#define gdev_mem_bytes_per_scan_line(dev)\
  gdev_prn_raster((gx_device_printer *)(dev))
#define gdev_prn_transpose_8x8(inp,ils,outp,ols)\
  memflip8x8(inp,ils,outp,ols)
