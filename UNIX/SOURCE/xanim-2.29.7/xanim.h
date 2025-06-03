
/*
 * xanim.h
 *
 * Copyright (C) 1990,1991,1992 by Mark Podlipec. 
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
#include <sys/types.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <memory.h>
#include "mytypes.h"
#include <X11/Xlib.h>

#ifndef MIN
#define MIN(x,y)    ( ((x)>(y))?(y):(x) )
#endif

#ifndef MAX
#define MAX(x,y)    ( ((x)>(y))?(x):(y) )
#endif

/* Read xanim.readme at the end for more info on this line */
#define HZ 100

#define MS_PER_60HZ 17

/* X11 variables
 */ 
#define MONOCHROME    0
#define GRAY_SCALE    1
#define STATIC_COLOR  2
#define PSEUDO_COLOR  3
#define TRUE_COLOR    4

extern int x11_depth;
extern int x11_class;
extern int x11_bytes_pixel;
extern int x11_bitmap_unit;
extern int x11_bit_order;
extern int x11_cmap_flag;
extern int x11_cmap_size;
extern int x11_display_type;
extern int x11_red_mask;
extern int x11_green_mask;
extern int x11_blue_mask;
extern int x11_red_shift;
extern int x11_green_shift;
extern int x11_blue_shift;
extern int x11_red_size;
extern int x11_green_size;
extern int x11_blue_size;
extern int x11_black;
extern int x11_white;
extern int x11_verbose_flag;

#define X11_MSB  1
#define X11_LSB  0

extern int anim_holdoff;
extern int anim_status;
#define XA_RUN_NEXT    0
#define XA_RUN_PREV    1
#define XA_STEP_NEXT   2
#define XA_STEP_PREV   3
#define XA_FILE_NEXT   4
#define XA_FILE_PREV   5
#define XA_STOP_NEXT   6
#define XA_STOP_PREV   7
#define XA_UNSTARTED   8

extern int anim_type;
#define UNKNOWN_ANIM  0
#define IFF_ANIM      1
#define FLI_ANIM      2
#define GIF_ANIM      3
#define TXT_ANIM      4
#define FADE_ANIM     5
#define DL_ANIM       6
#define JPG_ANIM      7

typedef struct ACTION_STRUCT
{
 int type;      /* type of action */
 int time;      /* # of jiffies (1/60th sec) */
 UBYTE *data;    /* data */
} ACTION;

extern ACTION action[];
extern int action_cnt;
extern int action_start;

typedef struct ANIM_HDR_STRUCT
{
 int file_num;
 int anim_type;
 int imagex;
 int imagey;
 int imagec;
 int imaged;
 int anim_flags;
 int loop_num;
 int loop_frame;
 int last_frame;
 int *frame_lst;          /* array of ints which index to actions */
 struct ANIM_HDR_STRUCT *next_file;
 struct ANIM_HDR_STRUCT *prev_file;
} ANIM_HDR;

typedef struct
{
 int cmap_size;
 UBYTE data[4];
} CMAP_HDR;

#define ANIM_HAM   0x00000001
#define ANIM_LACE  0x00000002
#define ANIM_CYCLE 0x00000004
#define ANIM_DOUBL 0x00000008
#define ANIM_IFF   0x01000000
#define ANIM_DEBUG 0x00010000
#define ANIM_NOLOP 0x00020000
#define ANIM_CYCON 0x00040000

typedef struct
{
 int imagex;
 int imagey;
 int xoff;
 int yoff;
} SIZE_HDR;

typedef struct
{ 
 int xpos;
 int ypos;
 int xsize;
 int ysize;
 XImage *image;
} ACT_IMAGE_HDR;

typedef struct
{ 
 int xpos;
 int ypos;
 int xsize;
 int ysize;
 Pixmap pixmap;
} ACT_PIXMAP_HDR;

typedef struct
{ 
 int xpos;
 int ypos;
 int xsize;
 int ysize;
 int pk_size;
 UBYTE data[4]; 
} ACT_PACKED_HDR;

typedef struct
{ 
 int xpos;
 int ypos;
 int xsize;
 int ysize;
 XImage *image;
 UBYTE *clip_ptr;
} ACT_CLIP_HDR;

#define ACT_NOP         0x0000
#define ACT_DELAY       0x0001
#define ACT_IMAGE       0x0002
#define ACT_CMAP        0x0003
#define ACT_SIZE        0x0004
#define ACT_FADE        0x0005
#define ACT_CLIP        0x0006
#define ACT_PIXMAP      0x0007
#define ACT_PACKED      0x0008
#define ACT_FLI_LC7     0x1007
#define ACT_FLI_COLOR   0x100b
#define ACT_FLI_LC      0x100c
#define ACT_FLI_BLACK   0x100d
#define ACT_FLI_BRUN    0x100f
#define ACT_FLI_COPY    0x1010
#define ACT_IFF_BODY    0x2000
#define ACT_IFF_DLTA0   0x2100
#define ACT_IFF_DLTA1   0x2101
#define ACT_IFF_DLTA2   0x2102
#define ACT_IFF_DLTA3   0x2103
#define ACT_IFF_DLTA4   0x2104
#define ACT_IFF_DLTA5   0x2105
#define ACT_IFF_DLTAJ   0x2149
#define ACT_IFF_DLTAl   0x216c
#define ACT_IFF_HMAP    0x2200
#define ACT_IFF_CRNG    0x2201


/* flags */
extern int verbose;
extern int debug;
extern int ham_flag;
extern int cycle_flag;
extern int anim_flags;
extern int jiffy_flag;
extern int cycle_on_flag;
extern int cycle_wait;
extern int buff_flag;
extern int pack_flag;
extern int optimize_flag;
extern int pixmap_flag;
extern int dither_flag;
extern int anm_map_flag;

#define DEBUG_LEVEL1   if (debug >= 1) 
#define DEBUG_LEVEL2   if (debug >= 2) 
#define DEBUG_LEVEL3   if (debug >= 3) 
#define DEBUG_LEVEL4   if (debug >= 4) 
#define DEBUG_LEVEL5   if (debug >= 5) 

/* variables */
extern int imagex;
extern int imagey;
extern int imagec;
extern int imaged;
extern int disp_y;

#define CMAP_SIZE 256
#define HMAP_SIZE 16
typedef struct
{
 UBYTE red,green,blue,pad;
 ULONG map;
} ColorReg;
extern ColorReg *cmap;
extern ColorReg ham_map[16];

/* CMAP function flags for ACT_Setup_CMAP */
#define CMAP_DIRECT		0x000000
#define CMAP_ALLOW_REMAP	0x000001

extern int cmap_luma_sort;
extern int cmap_try_to_1st;
extern int cmap_map_to_1st;

typedef struct
{
 WORD pad1;
 WORD rate;
 WORD active;
 UBYTE low,high;
} CRNG_HDR;
#define CRNG_HDR_SIZE 8

/* Range flags values */
#define CRNG_ACTIVE  1
#define CRNG_REVERSE 2
#define CRNG_INTERVAL   (16384*16)

extern void TheEnd();
extern void TheEnd1();
extern ULONG GetWord();
extern ULONG GetHalfWord();
extern void ShowAnimation();
extern void ShowAction();
extern void Cycle_It();
extern ULONG X11_Get_True_Color();
extern ULONG X11_Get_Line_Size();
#define X11_Get_Monochrome(r,g,b) (((ULONG)(r)*11 + (ULONG)(g)*16 + (ULONG)(b)*5) >> 5)
