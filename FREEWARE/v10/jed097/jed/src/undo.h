#ifndef DAVIS_UNDO_H_
#define DAVIS_UNDO_H_
/*
 *  Copyright (c) 1993, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

#if defined (msdos) || defined (__os2_16__)
/* size is something like 2000 x (16) = 32K */
#define MAX_UNDOS 2000
#else
#define MAX_UNDOS 10000
#endif

typedef struct Undo_Object_Type
{
   unsigned short type;		       /* type of damage */
   int linenum;			       /* where damage was */
   int point;			       /*  */
   unsigned char buf[8];	       /* buffer for chars */
   /* VOID *obj; */		       /* pointer to object info */
   int misc;			       /* misc information */
} Undo_Object_Type;


typedef struct Undo_Type
{
   Undo_Object_Type Undo_Ring[MAX_UNDOS];
   Undo_Object_Type *Last_Undo;
   Undo_Object_Type *First_Undo;
} Undo_Type;


extern void record_deletion(unsigned char *, int);
extern void record_insertion(int);
extern void record_newline_insertion(void);
extern int undo(void);
extern void create_undo_ring(void);
extern int Undo_Buf_Unch_Flag;	       /* 1 if buffer prev not modified */
#endif
