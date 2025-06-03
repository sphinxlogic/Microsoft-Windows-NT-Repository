
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
#include "mytypes.h"
#include <X11/Xlib.h>

#define HZ 60

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MS_PER_60HZ 17

#define WINDOW_TITLE "XAnimate"

/* X11 variables
 */ 
#define PSEUDO_COLOR_8BIT  1
#define TRUE_COLOR_24BIT   2
extern int x11_depth;
extern int x11_class;
extern int x11_bytes_pixel;
extern int x11_cmap_flag;
extern int x11_cmap_size;
extern int x11_display_type;
extern int x11_red_mask;
extern int x11_green_mask;
extern int x11_blue_mask;
extern int x11_red_shift;
extern int x11_green_shift;
extern int x11_blue_shift;
extern int x11_scale;

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
#define IFF_ANIM      1
#define FLI_ANIM      2
#define GIF_ANIM      3
#define TXT_ANIM      4
#define FADE_ANIM     5

typedef struct ACTION_STRUCT
{
 int type;      /* type of action */
 int time;      /* # of jiffies (1/60th sec) */
 UBYTE *data;    /* data */
} ACTION;

extern ACTION action[];
extern int action_cnt;
extern int action_start;

typedef struct
{
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


extern ANIM_HDR anim[];
extern int anim_cnt;

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
 UBYTE data[4];
} ACT_REGION_HDR;
#define ACT_FLI_LC_HDR ACT_REGION_HDR

typedef struct
{
 int minx;
 int miny;
 int maxx;
 int maxy;
 UBYTE data[4];
} ACT_IFF_DLTA_HDR;

typedef struct
{ 
 int xpos;
 int ypos;
 int xsize;
 int ysize;
 UBYTE data[4];
} REGION_HDR;

/*
 *  This referenced here, but never declared!
 *
 *  extern REGION_HDR reg_hdr;
 */

typedef struct
{ 
 int xpos;
 int ypos;
 int xsize;
 int ysize;
 XImage *image;
} ACT_IMAGE1_HDR;

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
#define ACT_REGION      0x0006
#define ACT_IMAGE1      0x0007
#define ACT_CLIP        0x0008
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
/*
 *  These are referenced here, but never declared!
 *
 *  extern int ham_flag;
 *  extern int cycle_flag;
 *
 */
extern int anim_flags;
extern int jiffy_flag;
extern int cycle_on_flag;
extern int cycle_wait;
extern int optimize_flag;

#define DEBUG_LEVEL1   1
#define DEBUG_LEVEL2   2
#define DEBUG_LEVEL3   3
#define DEBUG_LEVEL4   4
#define DEBUG_LEVEL5   5

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

/* CMAP Quantization Variables */
extern ULONG db_cmap_op;
#define DB_CMAP_NONE  0
#define DB_CMAP_POP   1
#define DB_CMAP_MED   2
#define DB_CMAP_WMED  3

extern ULONG db_cmap_half;
#define DB_CMAP_LO  0
#define DB_CMAP_HI  1

extern ACTION *db_cmap_prev,*db_cmap_first,*db_cmap_current;


extern void TheEnd();
extern void TheEnd1();
extern ULONG GetWord();
extern ULONG GetHalfWord();
extern void ShowAnimation();
extern void ShowAction();
extern void Cycle_It();
extern void XAnimPutScaledImage();

