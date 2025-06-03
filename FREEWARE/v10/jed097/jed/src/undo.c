/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include "config.h"
#include "buffer.h"
#include "undo.h"
#include "ins.h"
#include "line.h"
#include "misc.h"

#define LAST_UNDO CBuf->undo->Last_Undo
#define FIRST_UNDO CBuf->undo->First_Undo
#define UNDO_RING CBuf->undo->Undo_Ring

static int Undo_In_Progress = 0;
int Undo_Buf_Unch_Flag;	       /* 1 if buffer prev not modified */

#define DONT_RECORD_UNDO  (!(CBuf->flags & UNDO_ENABLED)\
			   || (CBuf->undo == NULL) || Undo_In_Progress)

#define UNDO_BD_FLAG 0x8000
#define UNDO_UNCHANGED_FLAG 0x4000

#define IS_UNDO_BD (LAST_UNDO->type & UNDO_BD_FLAG)

void prepare_next_undo(void)
{
   LAST_UNDO++;
   if (LAST_UNDO >= UNDO_RING + MAX_UNDOS) LAST_UNDO = UNDO_RING;
   if (LAST_UNDO == FIRST_UNDO) FIRST_UNDO++;
   if (FIRST_UNDO >= UNDO_RING + MAX_UNDOS) FIRST_UNDO = UNDO_RING;
   
   LAST_UNDO->type = 0;
   LAST_UNDO->misc = 0;
}


void record_deletion(unsigned char *p, int n)
{
   int misc;
   unsigned char *pb;
   
   
   if (DONT_RECORD_UNDO || !n) return;
   
   if (((LAST_UNDO->type && 0xFF) == 0) || ((LAST_UNDO->type & CDELETE)
				  && (LAST_UNDO->linenum == LineNum + CBuf->nup)
				  && (LAST_UNDO->point == Point)))
     {
	if (LAST_UNDO->type != 0) misc = LAST_UNDO->misc; else misc = 0;
	pb = LAST_UNDO->buf + misc;
	while ((misc < 8) && n) 
	  {
	     *pb++ = *p++;
	     misc++;
	     n--;
	  }
	LAST_UNDO->misc = misc;
	LAST_UNDO->type |= CDELETE;
	LAST_UNDO->linenum = LineNum + CBuf->nup;
	LAST_UNDO->point = Point;
	if (Undo_Buf_Unch_Flag) LAST_UNDO->type |= UNDO_UNCHANGED_FLAG;
	Undo_Buf_Unch_Flag = 0;
     }
   
   if (n)
     {
	prepare_next_undo();
	record_deletion(p, n);
     }
   if (Undo_Buf_Unch_Flag) LAST_UNDO->type |= UNDO_UNCHANGED_FLAG;
}


int undo()
{
   int line;
   CHECK_READ_ONLY
   if (!(CBuf->flags & UNDO_ENABLED))
     {
	msg_error("Undo not enabled for this buffer.");
	return(0);
     }
   else if ((CBuf->undo == NULL) || (LAST_UNDO->type == 0))
     {
	msg_error("No more undo information.");
	return(0);
     }
   Undo_In_Progress = 1;
   
   do
     {
	line = (int) LAST_UNDO->linenum;
	if ((line <= CBuf->nup) || (line > CBuf->nup + Max_LineNum))
	  {
	     msg_error("Next undo lies outside visible buffer.");
	     break;
	  }
	line -= CBuf->nup;
	goto_line(&line);
	Point = LAST_UNDO->point;
	
	switch (LAST_UNDO->type & 0xFF)
	  {
	   case CDELETE: ins_chars(LAST_UNDO->buf, LAST_UNDO->misc);
	     /* Point = LAST_UNDO->point; */
	     break;
	
	   case CINSERT: deln(&LAST_UNDO->misc);
	     break;
	     
	   case NLINSERT: del(); break;
	     
	   default: return(1);
	  }
	
	if (LAST_UNDO->type & UNDO_UNCHANGED_FLAG)
	  {
	     mark_buffer_modified(&Number_Zero);
	  }
	
	if (LAST_UNDO == FIRST_UNDO) LAST_UNDO->type = 0;
	else 
	  {
	     LAST_UNDO--;
	     if (LAST_UNDO < UNDO_RING) LAST_UNDO = UNDO_RING + MAX_UNDOS - 1;
	  }
     } 
   while ((!IS_UNDO_BD) && (LAST_UNDO->type));
   
   message("Undo!");
   Undo_In_Progress = 0;
   return(1);
}

void record_insertion(int n)
{   
   if (DONT_RECORD_UNDO || !n) return;
   
   if (LAST_UNDO->type == 0)
     {
	LAST_UNDO->misc = n;
	LAST_UNDO->point = Point;
     }
   else if ((LAST_UNDO->type & CINSERT) && (LAST_UNDO->linenum == LineNum + CBuf->nup)
	    && (LAST_UNDO->point + LAST_UNDO->misc == Point)
	    && (LAST_UNDO->misc <= 32))
     {
	LAST_UNDO->misc += n;
     }
   else
     {
	prepare_next_undo();
	LAST_UNDO->point = Point;
	LAST_UNDO->misc = n;
     }
   
   LAST_UNDO->type |= CINSERT;
   if (Undo_Buf_Unch_Flag) LAST_UNDO->type |= UNDO_UNCHANGED_FLAG;
   LAST_UNDO->linenum = LineNum + CBuf->nup;
}

void record_newline_insertion()
{
   if (DONT_RECORD_UNDO) return;
   if (LAST_UNDO->type != 0) prepare_next_undo();
   if (Undo_Buf_Unch_Flag) LAST_UNDO->type |= UNDO_UNCHANGED_FLAG;
   LAST_UNDO->point = Point;
   LAST_UNDO->misc = 0;
   LAST_UNDO->type |= NLINSERT;
   LAST_UNDO->linenum = LineNum + CBuf->nup;
}

void delete_undo_ring(Buffer *b)
{
   SLFREE (b->undo);   /* assume non null-- called by delete buffer */
}


void create_undo_ring()
{
   Undo_Type *ur;
   Undo_Object_Type *uo;
   int n;
   
   if (NULL == (ur = (Undo_Type *) SLMALLOC(sizeof(Undo_Type))))
     {
	msg_error("Unable to malloc space for undo!");
	return;
     }
   CBuf->undo = ur;
   uo = ur->Undo_Ring;
   ur->Last_Undo = ur->First_Undo = uo;
   n = MAX_UNDOS;
   while (n--) 
     {
	uo->type = 0;
	uo++;
     }
}

void mark_undo_boundary(Buffer *b)
{
   Buffer *s = CBuf;
   
   CBuf = b;
   
   if (!DONT_RECORD_UNDO && (LAST_UNDO->type != 0))
     {
	LAST_UNDO->type |= UNDO_BD_FLAG;
     }
   CBuf = s;
}

   
   
