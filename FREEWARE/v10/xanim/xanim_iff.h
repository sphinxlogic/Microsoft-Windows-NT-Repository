
/*
 * xanim_iff.h
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
/* flags */
extern int verbose;

/* variables */
extern int imagex;
extern int imagey;
extern int imagec;
extern int imaged;

/* technically iff 
#define CMAP_SIZE 256
typedef struct
{
 UBYTE red,green,blue;
} ColorReg;
extern ColorReg cmap[256];
*/

typedef struct 
{
 LONG id;
 LONG size;
} Chunk_Header;

/* Graphic Stuff */
#define ANHD 0x414e4844
#define ANIM 0x414e494d
#define ANSQ 0x414e5351
#define BMHD 0x424d4844
#define BODY 0x424f4459
#define CAMG 0x43414d47
#define CMAP 0x434d4150
#define CRNG 0x43524e47
#define DLTA 0x444c5441
#define DPAN 0x4450414e
#define DPPS 0x44505053
#define DPPV 0x44505056
#define FORM 0x464f524d
#define GRAB 0x47524142
#define ILBM 0x494c424d
#define IMRT 0x494d5254

/* Grouping Stuff */
#define LIST 0x4c495354
#define PROP 0x50524f50
#define FACE 0x46414345

/* Sound stuff */
#define VHDR 0x56484452
#define ANNO 0x414e4e4f
#define CHAN 0x4348414e


typedef struct
{
 UWORD width, height;
 WORD x, y;
 UBYTE depth;
 UBYTE masking;
 UBYTE compression;
 UBYTE pad1;
 UWORD transparentColor;
 UBYTE xAspect, yAspect;
 WORD pageWidth, pageHeight;
} Bit_Map_Header;

#define BMHD_COMP_NONE 0L
#define BMHD_COMP_BYTERUN 1L

#define BMHD_MSK_NONE 0L
#define BMHD_MSK_HAS 1L
#define BMHD_MSK_TRANS 2L
#define BMHD_MSK_LASSO 3L

#define mskNone                 0
#define mskHasMask              1
#define mskHasTransparentColor  2
#define mskLasso                3

#define cmpNone      0
#define cmpByteRun1  1

/* Aspect ratios: The proper fraction xAspect/yAspect represents the pixel
 * aspect ratio pixel_width/pixel_height.
 *
 * For the 4 Amiga display modes:
 *   320 x 200: 10/11  (these pixels are taller than they are wide)
 *   320 x 400: 20/11
 *   640 x 200:  5/11
 *   640 x 400: 10/11      */
#define x320x200Aspect 10
#define y320x200Aspect 11
#define x320x400Aspect 20
#define y320x400Aspect 11
#define x640x200Aspect  5
#define y640x200Aspect 11
#define x640x400Aspect 10
#define y640x400Aspect 11


typedef struct
{
 UWORD width,height;
 WORD x, y;
 WORD xoff, yoff;
} Face_Header;



typedef struct 
{
 LONG id;
 LONG size;
 LONG subid;
} Group_Header;

typedef struct 
{
 LONG id;
 LONG size;
 LONG subid;
 UBYTE grpData[ 1 ];
} Group_Chunk;

typedef struct
{
 UBYTE op;
 UBYTE mask;
 UWORD w,h;
 UWORD x,y;
 ULONG abstime;
 ULONG reltime;
 UBYTE interleave;
 UBYTE pad0;
 ULONG bits;
 UBYTE pad[16];
} Anim_Header;
#define Anim_Header_SIZE 40

extern void IFF_Read_File();
extern void IFF_HAM_To_332();
extern void IFF_Delta5();
extern void IFF_Delta3();
extern void IFF_Deltal();
extern LONG IFF_DeltaJ();
extern LONG Is_IFF_File();
extern int UnPackRow();

