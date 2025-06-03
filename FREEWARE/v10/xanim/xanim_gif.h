
/*
 * xanim_gif.h
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
typedef struct
{
 int width;
 int height;
 UBYTE m;
 UBYTE cres;
 UBYTE pixbits;
 UBYTE bc;
} GIF_Screen_Hdr; 

typedef union 
{
 struct
 {
  UBYTE red;
  UBYTE green;
  UBYTE blue;
  UBYTE pad;
 } cmap;
 ULONG pixel;
} GIF_Color;

typedef struct
{
 int left;
 int top;
 int width;
 int height;
 UBYTE m;
 UBYTE i;
 UBYTE pixbits;
 UBYTE reserved;
} GIF_Image_Hdr;

typedef struct 
{
 UBYTE valid;
 UBYTE data;
 UBYTE first;
 UBYTE res;
 int last;
} GIF_Table;

extern void GIF_Read_File();
extern LONG Is_GIF_File();

