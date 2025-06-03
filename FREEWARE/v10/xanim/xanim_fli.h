
/*
 * xanim_fli.h
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
 int size;      /*  0 size 4  */
 int magic;     /*  4 size 2  */
 int frames;    /*  6 size 2  */
 int width;     /*  8 size 2  */
 int height;    /* 10 size 2  */
 int flags;     /* 12 size 2  */
 int res1;      /* 14 size 2  */
 int speed;     /* 16 size 2  */
 int next;      /* 18 size 4  */
 int frit;      /* 22 size 4  */
                /* 26 size 102 future enhancement */
} Fli_Header;

typedef struct
{
 int size;      /*  0 size 4 size of chunk */
 int magic;     /*  4 size 2 */
 int chunks;    /*  4 size 2 number of chunks in frame */
                /*  4 size 8 future*/
} Fli_Frame_Header;

#define CHUNK_4          4
#define FLI_LC7          7
#define FLI_COLOR       11
#define FLI_LC          12
#define FLI_BLACK       13
#define FLI_BRUN        15
#define FLI_COPY        16

#define FLI_MAX_COLORS  256

extern void Decode_Fli_BRUN();
extern void Decode_Fli_LC();
extern void Fli_Buffer_Action();
extern void Read_Fli_File();

/* Words(32 bits) and HalfWords(16 bits) are stored little endian
 * according to the FLI spec.
 */
#define GETWORD      Fli_Get_Word
#define GETHALFWORD  Fli_Get_HalfWord

