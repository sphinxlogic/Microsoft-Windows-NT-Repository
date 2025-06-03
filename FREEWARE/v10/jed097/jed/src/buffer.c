/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

#include <stdio.h>
#include <setjmp.h>
#if defined (msdos) || defined (__os2_16__)
#include <process.h>
#include <dos.h>
#endif
#include <limits.h>
#include <string.h>

#include "config.h"
#include "buffer.h"
#include "window.h"
#include "file.h"
#include "ins.h"
#include "misc.h"
#include "paste.h"
#include "sysdep.h"

extern int Goal_Column;

typedef struct
{
   jmp_buf b;
} jmp_buf_struct;

extern jmp_buf_struct Jump_Buffer, *Jump_Buffer_Ptr;


typedef struct Buffer_List_Type
  {
      Buffer *buf;
      struct Buffer_List_Type *next;
  }
Buffer_List_Type;

Buffer_List_Type *Buffer_Stack;   /* a filo */

#ifndef ULONG_MAX
#define MAX_LONG (0xFFFFFFFFL | (0xFFFFFFFFL << (sizeof(long) / 8)))
#else
#define MAX_LONG ULONG_MAX
#endif
 

Buffer *CBuf;
Line *CLine;
Buffer_Local_Type Buffer_Local = {8, 0};

int Number_One = 1;		       /* these should be const but  */
int Number_Zero = 0;		       /* some compilers complain */
int Number_Two = 2;
int Number_Ten = 10;
char *Read_Only_Error = (char *) "Buffer is read only!";

unsigned int LineNum = 1;
unsigned int Max_LineNum;
int Point = 0;

/* move point to top of buffer */
int bob()
{
   CLine = CBuf->beg;
   Point = 0;
   LineNum = 1;
   return(1);
}

int eob()                      /* point to end of buffer */
{
   CLine = CBuf->end;
   LineNum = Max_LineNum;
   if (CLine == NULL)
     {
	Point = 0;
	return(0);
     }

    Point = CLine->len - 1;
    if (Point < 0)
      {
          Point = 0;
          return(1);
      }

    if ((CLine->data[Point] != '\n') || (CBuf == MiniBuffer)) Point++;
    return(1);
}

int bol()
{
    Point = 0;
    Goal_Column = 1;
    return(1);
}

int eol()
{
    int newline = 1;
    if (CLine != NULL)
      {
          Point = CLine->len - 1;
          if (Point < 0)
            {
                Point = 0;
                return(0);
            }

          if (((CLine->data)[Point] != '\n') || (CBuf == MiniBuffer)) newline = 0;
          if (!newline) Point++;
      }

    else Point = 0;
    return(1);
}

int bobp()                /* returns 1 if top line of buffer, 0 otherwise */
{
    if ((CBuf->beg == CLine) && (Point == 0)) return(1); else return(0);
}

int eolp()
{
   int len;

   if (CLine == NULL) return(1);
   len = CLine->len;
   if (len == 0) return(1);
   if (Point < len - 1) return(0);
   
   if (CBuf == MiniBuffer)
     {
	if (Point < len) return 0; else return (1);
     }
   

    /* Point is either len or len - 1 */

   if ((CLine->data)[len-1] == '\n') return (1);

   if (Point == len) return(1);
   Point = len - 1;
   return(0);
}

int eobp()
{
    if (CLine != CBuf->end) return(0);
    return(eolp());
}

int bolp()
{
    if (Point) return(0); else return(1);
}

/*  Attempt to goback n lines, return actual number. */
int prevline(int *n)
{
    int i = 0;
    Line *prev;

    Point = 0;     /* return to beginning of this line */
    while (i < *n)
      {
          prev = CLine->prev;
          while((prev != NULL) && (prev->len == 0)) prev = prev->prev;
          if (prev == NULL) break;
          i++;
          CLine = prev;
      }

   if (i) eol();
   LineNum -= (unsigned int) i;
   return(i);
}

int nextline(int *np)
{
   register Line *next;
   register int i = 0, n = *np;

    while(i < n)
      {
	 next = CLine->next;
	 if (next == NULL) break;
	 CLine = next;
	 i++;
      }

   if (i) Point = 0;
   LineNum += (unsigned int) i;
   return(i);
}

/* The algorithm:  go to Point = len - 1.  Then forward a line counted as 1 */
int forwchars(int *np)
{
   int len, total = 0;
   unsigned char *p;
   Line *next;
   int n = *np;

   if (n < 0) return 0;
   if (CBuf == MiniBuffer)
     {
	total = CLine->len - Point;
	if (n > total) n = total;
	Point += n;
	return n;
     }
	
    while(1)
      {
          len = CLine->len;
          /* if (Point == len) return(total); */
          p = CLine->data + Point;
          while(n && (Point < len) && (*p != '\n'))
            {
                Point++;
                n--;
                p++;
                total++;
            }
          if (!n) return(total);
          if (*p == '\n')
            {
                total++;
                n--;
            }

          if (NULL == (next = CLine->next))
            {
                if (*p == '\n') total--;
                return(total);
            }

          Point = 0;
          CLine = next;
	 LineNum++;
      }
}

int backwchars(int *np)
{
   int total = 0;
   int n = *np;
   
   if (n < 0) return 0;
   if (CBuf == MiniBuffer)
     {
	if (n > Point) n = Point;
	Point -= n;
	return (n);
     }
   

    do
      {
          if (n <= Point)
            {
                total += n;
                Point = Point - n;
                return(total);
            }
          n = n - Point - 1;   /* Point + newline */
          total += Point + 1;   /* ditto */
      }
    while(prevline(&Number_One));

    return(total - 1);
}
/* assuming 8 bit bytes */
#define BUNCH_SIZE 8 * sizeof(long)
static unsigned char NewLine_Buffer[1] = {'\n'};
typedef struct Bunch_Lines_Type
{
   struct Bunch_Lines_Type *next;
   unsigned long flags;			       /* describes which are free */
   Line lines[BUNCH_SIZE];
} Bunch_Lines_Type;

static int Next_Free_Offset = BUNCH_SIZE;
static int Last_Free_Offset;
static Bunch_Lines_Type *Last_Free_Group;
static Bunch_Lines_Type *Bunch_Lines;
static unsigned int Number_Freed;      /* amount of Lines available */

static Line *create_line_from_bunch(void)
{
   register Bunch_Lines_Type *b, *bsave;
   Line *l;
   unsigned long flags;
   int count;
   
   if (Last_Free_Group != NULL) 
     {
	l = &Last_Free_Group->lines[Last_Free_Offset];
	flags = ((unsigned long) 1L << Last_Free_Offset);
	if ((Last_Free_Group->flags & flags) == 0) 
	  {
	     exit_error("create group: internal error 1", 1);
	  }
	
	Last_Free_Group->flags &= ~flags;
	Last_Free_Group = NULL;
	Number_Freed--;
	return (l);
     }
   
   if (Next_Free_Offset < BUNCH_SIZE)
     {
	flags = ((unsigned long) 1L << Next_Free_Offset);
	if ((Bunch_Lines->flags & flags) == 0) 
	  {
	     exit_error("free group: internal error 2", 1);
	  }
	
	
	Bunch_Lines->flags &= ~flags;
	Number_Freed--;
	return(&Bunch_Lines->lines[Next_Free_Offset++]);
     }
   
   /* search list */
   b = Bunch_Lines;
   if (b != NULL)
     {
	b = b->next;
     }
   
   if ((b != NULL) && Number_Freed)
     {
	bsave = b;
	do
	  {
	     if (b->flags)
	       {
		  flags = b->flags;
		  count = 0;
		  while ((flags & 1) == 0) 
		    {
		       flags = flags >> 1;
		       count++;
		    }
		  l = &b->lines[count];
		  flags = (unsigned long) 1 << count;
		  if ((b->flags & flags) == 0) 
		    {
		            exit_error("free group: internal error 2", 1);
		    } 
		  b->flags &= ~flags;
		  Number_Freed--;
		  return (l);
	       }
	     b = b->next;
	  }
	while (b != bsave);
     }
   
   /* failed so now malloc new bunch */
   
   if (NULL != (b = (Bunch_Lines_Type *) SLMALLOC(sizeof(Bunch_Lines_Type))))
     {
	if (Bunch_Lines == NULL)
	  {
	     Bunch_Lines = b;
	     b->next = b;
	  }
	else
	  {
	     b->next = Bunch_Lines->next;
	     Bunch_Lines->next = b;
	     Bunch_Lines = b;
	  }

	b->flags = MAX_LONG;
	Next_Free_Offset = 1;
	b->flags &= ~(unsigned long) 1;
	Number_Freed += BUNCH_SIZE - 1;
	return(&b->lines[0]);
     }
   return(NULL);
}
   

void destroy_bunch_line(Line *l)
{
#ifdef _MSC_VER
   Line *ll;
#else
   register Line *ll;
#endif
   
   register Bunch_Lines_Type *b, *bsave, *last, *next;
   static Bunch_Lines_Type *slast;

   if (slast != NULL) last = slast;
   else last = Bunch_Lines;
   
   b = bsave = last;
   
   do
     {
	ll = b->lines;
	if (
#if defined (msdos) || defined (__os2_16__)
	     /* stupid DOS and its memory segmentation forces me to consider
	        segment then offset */
	    (FP_SEG(ll) == FP_SEG(l)) &&
#endif
	    ((ll <= l) && (l < ll + BUNCH_SIZE)))
	  {
	     while (ll != l) ll++;
	     Last_Free_Offset = (int) (l - b->lines);
	     if (b->flags & (1L << Last_Free_Offset)) 
	       {
		  exit_error("free group: internal error 2", 1);
	       }
	     
	     b->flags |= (unsigned long) 1L << Last_Free_Offset;
	     
	     /* if this whole structure is free, free it */
	     if (b->flags == MAX_LONG)
	       {
		  if (last == b)
		    {
		       while ((next = last->next) != b) last = next;
		    }
		  
		  last->next = b->next;

		  if (b == Bunch_Lines)
		    {
		       if (last == b)
			 {
			    last = NULL;
			    /*
			     last = last->next;
			     if (last == b) last = NULL; */
			 }

		       Bunch_Lines = last;
		       Next_Free_Offset = BUNCH_SIZE;
		    }
		  
		  SLFREE(b);
		  
		  if (last == b) last = NULL;
		  b = NULL;
		  Number_Freed -= BUNCH_SIZE - 1;
		  slast = last;
	       }
	     else 
	       {
		  Number_Freed++;
	       }
	     
	     Last_Free_Group = b;
	     if (Bunch_Lines == NULL) goto L1;
	     return;
	  }
	last = b;
	b = b->next;
     }
   while (b != bsave);
   L1:
   exit_error("destroy_bunch_line: internal error 1", 1);
}

Line *make_line1(int size)
{
   Line *new_line;
   unsigned char *data = NULL;
   char buff[80];
   unsigned int chunk;

   /* 4 byte chunks */
#ifdef msdos
   chunk = (unsigned) (size + 3) & 0xFFFC;
#else
   chunk = (unsigned) (size + 3) & 0xFFFFFFFC; 
#endif
   new_line = (Line *) create_line_from_bunch();
   if (new_line != NULL) 
     {
	if (size == 1) 
	  {
	     data = NewLine_Buffer;
	     chunk = 1;
	  }
	else data = (unsigned char *) SLMALLOC(chunk);   /* was chunk + 1 */
     }

   if ((new_line == NULL) || (data == NULL))
     {
	*Error_Buffer = 0;	       /* this is critical */
	sprintf(buff, "Malloc Error in make_line: requested size: %d.", size);
	msg_error(buff);
	longjmp(Jump_Buffer_Ptr->b, 1);
	/* exit_error(buff); */
     }
   new_line->data = data;
   new_line->len = 0;
   new_line->space = chunk;
   return(new_line);
 }

/* adds a new link to list of lines at current point */
unsigned char *make_line(int size)
{
    Line *new_line;

    new_line = make_line1(size);
    /* if CLine is Null, then we are at the top of a NEW buffer.  Make this
       explicit. */
    if (CLine == NULL)
      {
          new_line -> prev = NULL;
          new_line -> next = NULL;
          CBuf -> beg = CBuf ->end = new_line;
      }
    else if (CLine == CBuf->end) /* at end of buffer */
      {
          CBuf->end  = new_line;
          new_line->next = NULL;
          new_line->prev = CLine;
          CLine->next = new_line;
      }
    else
      {
          new_line -> next = CLine -> next;
          if (CLine->next != NULL) CLine->next->prev = new_line;
          CLine->next = new_line;
          new_line->prev = CLine;
      }

   if (CLine == NULL)
     {
	Max_LineNum = LineNum = 1;
     }
   else
     {
	Max_LineNum++;
	LineNum++;
     }
   CLine = new_line;
   
   return(CLine->data);
}

void free_line(Line *line)
{
   register unsigned char *dat = line->data;
   
   if (dat != NewLine_Buffer) SLFREE(dat);
   destroy_bunch_line(line);
}

/* deletes the line we are on and returns the prev one.  It does not
 * delete the top line of the buffer.   Furthermore, it does not
 *  update any marks.  */

int delete_line()
{
    Line *n, *p, *tthis;

    p = CLine -> prev;
    if (p == NULL) return(1);

    n = CLine -> next;
    tthis = CLine;
    if (n == NULL)
      {
          CBuf->end = p;
          p->next = NULL;
      }
    else
      {
          p->next = n;
          n->prev = p;
      }

    free_line(tthis);
    CLine = p;
   LineNum--;
   Max_LineNum--;
   
   return(0);
}

unsigned char *remake_line(int size)
{
   char buf[80];
   unsigned char *d = CLine->data;
#ifdef msdos   
   size = (unsigned) (size + 7) & 0xFFF8u;   /* 16 byte chunks */
#else
   size = (unsigned) (size + 15) & 0xFFFFFFF0;
#endif
   
   if (d == NewLine_Buffer)
     {
	if (NULL != (d = (unsigned char *) SLMALLOC(size))) *d = '\n';
     }
   else d = (unsigned char *) SLREALLOC(d, size);
   if (d == NULL)
     {
	*Error_Buffer = 0;	       /* critical error */
	sprintf(buf, "remake_line: realloc error!, size = %d", size);
	msg_error(buf);
	longjmp(Jump_Buffer_Ptr->b, 1);
	/* exit_error(buf); */
     }

   CLine->space = size;
   CLine->data = d;
   return(d);
}

void uniquely_name_buffer(char *trry)
{
    Buffer *b;
    int exists, version = 0, n;
    char neew[48];

    *CBuf->name = 0;
    strncpy(neew, trry, 45);  neew[45] = 0;
   n = strlen(neew);
    while (1)
      {
	  exists = 0;
	  b = CBuf->next;
	  while(b != CBuf)
	    {
		if (!strcmp(neew, b->name)) exists = 1;
		b = b->next;
	    }
	  if (!exists)
	    {
		strcpy(CBuf->name,neew);
		return;
	    }
	  version++;
	  sprintf(neew + n, "<%d>", version);
      }
}

/* make a buffer and insert it in the list */
Buffer *make_buffer()
{
    Buffer *newB;

   /* SLCALLOC used since it zeros region too */
    newB = (Buffer *) SLCALLOC(sizeof(Buffer), 1);
    if (newB == (Buffer *) NULL)
      {
          exit_error("make_buffer: malloc error", 0);
      }

   MEMSET ((char *) newB, sizeof(Buffer), 0);
   
   newB->keymap = Global_Map;
   newB->c_time = sys_time();
   newB->tab = User_Vars.tab;
   if (CBuf == NULL)
     {
	newB->next = newB;
	newB->prev = newB;
     }
   else
     {
	newB->next = CBuf;
	newB->prev = CBuf->prev;
	CBuf->prev->next = newB;
	CBuf->prev = newB;
     }

#ifdef pc_system
   newB->flags |= ADD_CR_ON_WRITE_FLAG;
#endif
   
   return(newB);
}

/* if there is a window attached to this buffer, then there are problems
 *  if we get to update() without attaching another one to it.  So
 *  beware! Always make sure CBuf is set too!   kill_buffer command
 *  takes care of this */
void delete_buffer(Buffer *buf)
{
   Line *l,*n;
   Mark *m, *m1;

   while (buf->narrow != NULL) widen_buffer(buf);
   if (buf -> beg != NULL) for (l = buf -> beg; l != NULL; l = n)
     {
	n = l -> next;
	free_line(l);
	/* SLFREE( l->data); SLFREE( l); */
     }
   m = buf->marks;
   while (m != NULL)
      {
	 m1 = m->next;
	 if (m == &buf->mark_array[buf->mark_ptr - 1])
	   {
	      buf->mark_ptr--;
	   }
	 else SLFREE(m);
	 m = m1;
      }
   m = buf->spots;
   while (m != NULL)
     {
	m1 = m->next;
	if (m == &buf->spot_array[buf->spot_ptr - 1])
	  {
	     buf->spot_ptr--;
	  }
	else SLFREE(m);
	m = m1;
     }
   
   if (buf->user_marks != NULL) free_user_marks (buf);

   if (buf->undo != NULL) delete_undo_ring(buf);
   buf->prev->next = buf->next;
   buf->next->prev = buf->prev;

   SLFREE( buf);
}

/* need to make this exploit MaxLine info! */
void goto_line(int *np)
{
   unsigned int n;
   unsigned int half1 = LineNum / 2;
   unsigned int half2 = (Max_LineNum + LineNum) / 2;
   
   if (*np <= 1) n = 0; else n = (unsigned int) *np;
   
   if (n < LineNum)
     {
	if (n > half1) 
	  {
	     n = LineNum - n;
	     prevline((int *) &n);
	  }
	else
	  {
	     bob(); n--;
	     nextline((int *) &n);
	  }
     }
   else if (n < half2)
     {
	n = n - LineNum;
	nextline((int *) &n);
     }
   else
     {
	eob();
	n = Max_LineNum - n;
	prevline((int *) &n);
     }
   
}

Line *dup_line(Line *l)
{
    Line *neew;
    int n;
    unsigned char *p, *q;

    neew = (Line *) SLMALLOC(sizeof(Line));
    if ((neew == NULL) ||
        (NULL == (neew->data = (unsigned char *) SLMALLOC(l->space))))
      {
          exit_error("Malloc Error in dup_line.", 0);
      }
    neew->next = l->next;
    neew->prev = l->prev;
    neew->len = l->len;
    n = neew->space = l->space;
    p = neew->data;
    q = l->data;

    while (n--) *p++ = *q++;
    return(neew);
}

Buffer *find_buffer(char *name)
{
    Buffer *b;

    b = CBuf;
    do
      {
	  if (!strcmp(b->name, name)) return(b);
	  b = b->next;
      }
    while(b != CBuf);
   
#ifdef __os2__
   b = CBuf;
   do {
      if (!strcmpi(b->name, name)) return(b);
      b = b->next;
   } while(b != CBuf);
#endif
    return(NULL);
}

int buffer_exists(Buffer *b)
{
   Buffer *c = CBuf;
   do
     {
	if (b == c) return 1;
	c = c->next;
     }
   while (c != CBuf);
   return 0;
}

int switch_to_buffer(Buffer *buf)
{
   /* unlink buf */
   /*  save this buffer position */
   CBuf->line = CLine;
   CBuf->point = Point;
   CBuf->linenum = LineNum;
   CBuf->max_linenum = Max_LineNum;
   /* local variables */
   CBuf->tab = Buffer_Local.tab;
   CBuf->sd = Buffer_Local.sd;
   
   if (buf == CBuf) return(0);
   buf->prev->next = buf->next;
   buf->next->prev = buf->prev;

   buf->next = CBuf;
   buf->prev = CBuf->prev;
   CBuf->prev->next = buf;
   CBuf->prev = buf;


   /* now restore new buffer */
   CBuf = buf;
   CLine = CBuf->line;
   Point = CBuf->point;
   
   /* Buffer local variables */
   Buffer_Local.tab = CBuf->tab;
   Buffer_Local.sd = CBuf->sd;
   
   LineNum = CBuf->linenum;
   Max_LineNum = CBuf->max_linenum;
   
   if (CLine == NULL)
     {
	make_line(25);
	Point = 0;
     }
   CBuf->line = CLine;
   CBuf->point = Point;
   return(1);
}

/* we assume that file has already been expanded--- that is, canonical. */
Buffer *find_file_buffer(char *file)
{
   Buffer *b;
   char *f;
   long n, m;
   
   f = extract_file(file);
   n = f - file;
   
   b = CBuf;
    do
      {
	 m = strlen(b->dir);
#ifndef __os2__
	 if ((m == n) && (!strcmp(b->file,f))
	     && (!strncmp(b->dir, file, (int) n)))
#else
 	 if ((m == n) && (!strcmpi(b->file,f))
 	     && (!strncmpi(b->dir, file, (int) n)))
#endif
	   return(b);
          b = b->next;
      }
    while (b != CBuf);

    return(NULL);
}

/* take a dir and a filename, expand them then put in buffer structure */
void buffer_filename(char *dir, char *file)
{
   strcpy(CBuf->dir, dir);
   if ((file == NULL) || (*file == 0))
     {
	file = extract_file(CBuf->dir);
	strcpy(CBuf->file, file);
	*file = 0;
     }
   else strcpy(CBuf->file, file);
   uniquely_name_buffer(CBuf->file);
}


/* This kills all undo information! */
int erase_buffer()
{
   Line *beg, *tthis;

   /* CLine = CBuf->end; */
   beg = CBuf->beg;
   bob();
   CLine = CLine->next; LineNum++;
   Suspend_Screen_Update = 1;
   while(CLine != NULL)
     {
	tthis = CLine;
	beg->next = tthis->next;
	tthis->prev = beg;

	update_marks(LDELETE, 1);
	CLine = tthis->next;
	Max_LineNum--;
	free_line(tthis);
     }
   
   CLine = CBuf->beg; LineNum = 1;
   Point = 0;
   update_marks(CDELETE, CLine->len);
   CLine->len = 0;
   /*   CLine->next = NULL; */
   if (CBuf->undo != NULL) delete_undo_ring(CBuf);
   CBuf->undo = NULL;
   CBuf->end = CLine;
   touch_screen_for_buffer(CBuf);
   return(1);
}

void mark_buffer_modified(int *flag)
{
   unsigned long now;

   if (*flag)
     {
	if (CBuf->flags & BUFFER_TRASHED) return;
	now = sys_time();
	CBuf->m_time = now;
	CBuf->flags |= BUFFER_TRASHED;
	return;
     }
   else CBuf->flags &= ~BUFFER_TRASHED;
}

void check_line()
{
   if ((CLine->len == 1) && (*CLine->data == '\n') && (CLine->data != NewLine_Buffer))
     {
	SLFREE(CLine->data);
	CLine->data = NewLine_Buffer;
	CLine->space = 1;
     }
}

	
