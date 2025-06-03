/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
    
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "buffer.h"
#include "ins.h"
#include "line.h"
#include "paste.h"
#include "screen.h"
#include "misc.h"
#include "cmds.h"

/* SLang user object be larger than 100 */
#define JED_MARK_TYPE 100

Buffer *Paste_Buffer;
Buffer *Rectangle_Buffer;



/* This is used by the narrow command so that multiple windows are 
 * handled properly. The assumption is that we are dealing with a canonical
 * region */
      
void touch_windows(void)
{
   Window_Type *w;
   Line *l;
   unsigned int n;
   
   w = JWindow;
   JWindow = JWindow->next;
   while (JWindow != w)
     {
	if (CBuf == JWindow->buffer)
	  {
	     /* The mark is set with line at top of buffer */
	     l = CBuf->marks->line;
	     n = CBuf->marks->n;
	     while ((l != NULL) && (l != JWindow->mark.line)) l = l->next, n++;
	     if (l == NULL) 
	       {
		  JWindow->mark.line = CLine;
		  JWindow->mark.point = Point;
		  JWindow->mark.n = n - 1;
	       }
	     touch_window();
	  }
	JWindow = JWindow->next;
     }
   pop_mark(&Number_Zero);
}


int line_in_buffer(Line *line)
{
   Line *l;
   
   l = CBuf->beg;
   while (l != NULL) if (l == line) return(1); else l = l->next;
   return(0);
}


/* with prefix argument, pop marks */
int set_mark_cmd()
{
   Mark *m;
   if (Repeat_Factor != NULL)
     {
	while (CBuf->marks != NULL) pop_mark(&Number_Zero);
	Repeat_Factor = NULL;
	return(1);
     }
   
   if (CBuf->marks == NULL)
     {
	push_mark();
     }
   m = CBuf->marks;
   m->line = CLine;
   m->point = Point;
   m->n = LineNum + CBuf->nup;
   if ((m->flags & VISIBLE_MARK) == 0) 
     {
	m->flags |= VISIBLE_MARK;
	CBuf->vis_marks++;
     }
   
   /* if (m != CBuf->marks) m->next = CBuf->marks;
    CBuf->marks = m; */
   if (Last_Key_Function == (VOID *) set_mark_cmd) message("Mark Set.");
   return(1);
}

int push_spot()
{
    Mark *m;

   if (CBuf->spot_ptr < SPOT_ARRAY_SIZE)
     {
	m = &CBuf->spot_array[CBuf->spot_ptr++];
     }
   else if (NULL == (m = (Mark *) SLMALLOC(sizeof(Mark))))
      {
	  exit_error("push_spot: malloc error!", 0);
      }

   m->line = CLine;
   m->point = Point;
   m->n = LineNum + CBuf->nup;
   m->next = CBuf->spots;
   CBuf->spots = m;
   return(1);
}

int push_mark()
{
    Mark *m;

   if (CBuf->mark_ptr < SPOT_ARRAY_SIZE)
     {
	m = &CBuf->mark_array[CBuf->mark_ptr++];
     }
   else if (NULL == (m = (Mark *) SLMALLOC(sizeof(Mark))))
     {
	exit_error("push_mark: malloc error!", 0);
     }
   /*
    if (NULL == (m = (Mark *) SLMALLOC(sizeof(Mark))))
      {
	 msg_error("push_mark: malloc error!");
	 return(0);
      } */

   m->line = CLine;
   m->point = Point;
   m->n = LineNum + CBuf->nup;
   m->next = CBuf->marks;
   m->flags = 0;
   CBuf->marks = m;
   return(1);
}

void goto_mark(Mark *m)
{
   Line *l;
   
   l = m->line;
   LineNum = m->n;
	
   if (LineNum <= CBuf->nup) bob();
   else if (LineNum > CBuf->nup + Max_LineNum) eob();
   else
     {
	CLine = l;
	Point = m->point;
	LineNum -= CBuf->nup;
     }
}

int pop_mark(int *go)
{
   Mark *m;
   
   m = CBuf->marks;
   if (m == NULL) return(0);

   if (*go) goto_mark(m);
   if (m->flags & VISIBLE_MARK)
     {
	CBuf->vis_marks--;
	/* touch screen since region may be highlighted */
	if (CBuf->vis_marks == 0) touch_screen();
     }
   
   CBuf->marks = m->next;
   if (m == &CBuf->mark_array[CBuf->mark_ptr - 1])
     {
	CBuf->mark_ptr--;
     }
   else SLFREE(m);
   return(1);
}

int mark_spot()
{
    push_spot();
    message("Spot Marked.");
    return(1);
}

int pop_spot()
{
   Mark *m;
   
   m = CBuf->spots;
   if (m == NULL) return(0);
   
   goto_mark (m);
   
   CBuf->spots = m->next;
   
   if (m == &CBuf->spot_array[CBuf->spot_ptr - 1])
     {
	CBuf->spot_ptr--;
     }
   else SLFREE(m);
   return(1);
}

int exchange_point_mark(void)
{
   Line *save_line;
   int save_point;
   unsigned int save_n;
   Mark *m;
   
   if ((m = CBuf->marks) == NULL) return(0);
   
   save_point = Point;
   save_line = CLine;
   save_n = LineNum + CBuf->nup;
   
   goto_mark (m);
   
   m->point = save_point; m->line = save_line; m->n = save_n;
   return(1);
}

   
 /*returns 0 if the mark is not set and gives error.  Exchanges point and mark
  * to produce valid region.  A valid region is one with mark
  * earlier in the buffer than point.  Always call this if using a region
  * which reqires point > mark.  Also, push spot first then pop at end. 
  */
int check_region(int *push)
{
   register Line *beg, *tthis = CLine;
   int pbeg;

   if (CBuf->marks == NULL)
     {
	msg_error("Set mark first.");
	return(0);
     }

   if (*push) push_spot();
   beg = CBuf->marks->line;
   pbeg = CBuf->marks->point;

   if (beg == CLine)
     {
	if (pbeg <= Point) return(1);
     }

   else
     {
	while((beg != NULL) && (beg != tthis)) beg = beg->next;
	if (beg == tthis) return(1);
     }

   exchange_point_mark();
   return(1);
}


int widen_buffer(Buffer *b)
{
   Narrow_Type *n;
   Buffer *save = CBuf;
   
   if (NULL == (n = b->narrow)) return(0);
   
   /* make sure buffer ends in final newline */
   
   switch_to_buffer(b);
   push_spot();
   eob();
   if ((n->end != NULL)
       && (!CLine->len || ('\n' != *(CLine->data + (CLine->len - 1)))))
     ins('\n');
   
   pop_spot();
   
   if (n->end != NULL) n->end->prev = b->end;
   if (n->beg != NULL) n->beg->next = b->beg;
   b->end->next = n->end;
   b->beg->prev = n->beg;
   b->beg = n->beg1;
   if (n->end != NULL) b->end = n->end1;
   
   Max_LineNum += n->ndown + n->nup;
   LineNum += n->nup;
   
   /* adjust absolute offsets */
   b->nup -= n->nup;
   b->ndown -= n->ndown;
   b->narrow = n->next;
   
   SLFREE(n);
   switch_to_buffer(save);
   return(1);
}
   
int widen()
{
   return widen_buffer(CBuf);
}
   

/* not really a region of points but a region of lines. */
int narrow_to_region()
{
   Line *beg;
   Narrow_Type *nt;
   
   if (NULL == (nt = (Narrow_Type *) SLMALLOC(sizeof(Narrow_Type))))
     {
	msg_error("Malloc Error during narrow.");
	return(0);
     }
   
   if (!check_region(&Number_One)) return(0);       /* spot pushed */

   push_spot();
   pop_mark(&Number_One);
   push_mark();			       /* popped and used in touch_windows! */
   beg = CLine;
   nt->nup = LineNum - 1;
   
   pop_spot();  /* eor now */
   
   nt->ndown = Max_LineNum - LineNum;

   Max_LineNum = LineNum = LineNum - nt->nup;
   CBuf->nup += nt->nup;
   CBuf->ndown += nt->ndown;
   
   
   nt->next = CBuf->narrow;
   CBuf->narrow = nt;
   nt->beg = beg->prev;
   nt->end = CLine->next;
   nt->beg1 = CBuf->beg;
   nt->end1 = CBuf->end;
   
   CBuf->beg = beg;
   CBuf->end = CLine;
   beg->prev = NULL;
   CLine->next = NULL;
   
   pop_spot();
   touch_windows();
   return(1);
}

int yank()
{
    if (Paste_Buffer == NULL) return(0);
    insert_buffer(Paste_Buffer);
    return(1);
}

int copy_region_to_buffer(Buffer *b)
{
   int first_point, last_point, n, tmpm;
   Line *first, *last;
   Buffer *save_buffer;

   if (b->flags & READ_ONLY) 
     {
	msg_error(Read_Only_Error);
	return (0);
     }
   
   if (!check_region(&Number_One)) return(0);  /* spot pushed */
   last = CLine;
   last_point = Point;

   tmpm = 1; pop_mark(&tmpm);
   if (b == CBuf)
     {
	msg_error("A buffer cannot be inserted upon itself.");
	pop_spot();
	return(0);
     }

   first = CLine;
   first_point = Point;

   save_buffer = CBuf;
   switch_to_buffer(b);

   /* go through standard routines for undo comapatability */
   Suspend_Screen_Update = 1;
   if (first == last)
     {
	n = last_point - first_point;
	if (save_buffer == MiniBuffer)
	  {
	     ins_chars(first->data + first_point, n);
	  }
	else quick_insert(first->data + first_point, n);

     }
   else 
     {
	n = first->len - first_point;
	quick_insert(first->data + first_point, n);
	while (first = first->next, first != last)
	  {
	     quick_insert(first->data, first->len);
	  }
	quick_insert(first->data, last_point);
     }
   switch_to_buffer(save_buffer);
   pop_spot();
   return(1);
}

int copy_to_pastebuffer()
{
   /* delete paste buffer */
   if (Paste_Buffer != NULL) delete_buffer(Paste_Buffer);
   Paste_Buffer = make_buffer();
   strcpy(Paste_Buffer->name, " <paste>");

   copy_region_to_buffer(Paste_Buffer);
   return(0);
}

int delete_region (void)
{
   int beg_point, tmpm, end_point, n;
   Line *beg, *end;
   
   CHECK_READ_ONLY
    if (!check_region(&Number_Zero)) return(0);

   /* make this go through standard ins/del routines to ensure undo */
   
   end = CLine; end_point = Point;
   push_spot();
   tmpm = 1; pop_mark(&tmpm);
   beg = CLine; beg_point = Point;
   pop_spot();
   
   Point = 0;
   
   if (end != beg) 
     {
	deln(&end_point);
	
	/* go back because we do not want to mess with Line structures
	   changing on us --- shouldn't happen anyway */
	
	while (CLine = CLine->prev, LineNum--, CLine != beg)
	  {
	     eol(); 
	     n = Point + 1;
	     Point = 0;
	     generic_deln(&n);
	  }
	eol();
	n = Point - beg_point + 1;  /* the \n char */
	Point = beg_point;
     }
   else 
     {
	Point = beg_point;
	n = end_point - Point;
     }
   
   generic_deln(&n);
   return(1);
}

int kill_region()
{
   int tmpm = 1;
   
   CHECK_READ_ONLY

   /* need two marks for this one */
   push_spot();
   if (!pop_mark(&tmpm))
     {
	check_region(&Number_Zero);
	pop_spot();
	return(0);
     }
   push_mark();
   push_mark();
   pop_spot();

   copy_to_pastebuffer();
   delete_region();
   return(1);
}

static char *Rect_Error = "Rectangle has 0 width.";
int insert_rectangle()
{
   int c1;
   Line *rline;

   CHECK_READ_ONLY
   if (Rectangle_Buffer == NULL) return(0);

   Suspend_Screen_Update = 1;
   c1 = calculate_column();
   rline = Rectangle_Buffer->beg;
   if (rline != NULL) while (1)
     {
	goto_column(&c1);
	quick_insert(rline->data, rline->len);
	rline = rline->next;
	if (rline == NULL) break;
	if (CLine->next == NULL)
	  {
	     eol();
	     newline();
	  }
	else 
	  {
	     CLine = CLine->next;
	     LineNum++;
	  }
     }
   return(1);
}

int open_rectangle()
{
   int c1, n, c2, tmpm;
   Line *save_line;
   
   CHECK_READ_ONLY
   if (!check_region(&Number_One)) return(0); /* push_spot(); performed */

   c1 = calculate_column();
   save_line = CLine;
   tmpm = 1; pop_mark(&tmpm);
   c2 = calculate_column();
   n = c2 - c1;
   if (n < 0)
     {
	n = -n;
	c1 = c2;
     }
   
   Suspend_Screen_Update = 1;
   while(1)
     {
	goto_column(&c1);
	ins_char_n_times(' ', n);
	if (CLine == save_line) break;
	CLine = CLine->next;
	LineNum++;
     }
   pop_spot();

   return(1);
}

/* rectangle commands */
int copy_rectangle()
{
    Line *save_line, *line, *beg;
    int c1, c2, dc, tmp, tmpm, dc_malloc;
    unsigned char *p1, *p2, *data;
   
    if (!check_region(&Number_One)) return(0);       /* spot pushed */
    /* delete Rectangle buffer */
    if (Rectangle_Buffer != NULL) delete_buffer(Rectangle_Buffer);

    Rectangle_Buffer = make_buffer();
    strcpy(Rectangle_Buffer->name, " <rect>");
    c2 = calculate_column();
    save_line = CLine;

    tmpm = 1; pop_mark(&tmpm);
    c1 = calculate_column();
    if (c1 == c2)
      {
	 msg_error(Rect_Error);
	 pop_spot();
	 return(0);
      }
    if (c1 > c2)
      {
	 tmp = c1;
	 c1 = c2;
	 c2 = tmp;
	 goto_column(&c1);
      }

    /* go through the region copying rectanglar blocks to Rectanglebuffer */
    dc = c2 - c1;
   if (dc == 1) dc_malloc = 2; else dc_malloc = dc;
    line = beg = make_line1(dc_malloc);
   
    beg->prev = NULL;
    while (1)
      {
	 data = line->data;
	 /* p1 = data;
	 p2 = data + dc;
	 while (p1 < p2) *p1++ = ' '; */
	 MEMSET ((char *) data, ' ', dc);
	 
	 line->len = dc;

	 if (c1 == goto_column1(&c1))
	   {
	      p1 = CLine->data + Point;
	      (void) goto_column1(&c2);
	      p2 = CLine->data + Point;

	      /* while(p1 < p2) *data++ = *p1++; */
	      MEMCPY((char *) data, (char *) p1, (int) (p2 - p1));
	   }
	 if (CLine == save_line) break;
	 CLine = CLine->next;
	 LineNum++;

	 line->next = make_line1(dc_malloc);
	 line->next->prev = line;
	 line = line->next;
      }

    line->next = NULL;

    Rectangle_Buffer->line = Rectangle_Buffer->beg = beg;
    Rectangle_Buffer->end = line;
    Rectangle_Buffer->point = 0;

    pop_spot();
    return(0);
}

int kill_rectangle()
{
    Line *save_line, *line, *beg;
   int c1, c2, dc, tmp, n, tmpm, dc_malloc;
   unsigned char *p1, *p2, *data;

   CHECK_READ_ONLY
   if (!check_region(&Number_One)) return(0);

   /* delete Rectangle buffer */
   if (Rectangle_Buffer != NULL) delete_buffer(Rectangle_Buffer);

   Rectangle_Buffer = make_buffer();
   strcpy(Rectangle_Buffer->name, " <rect>");
   c2 = calculate_column();
   save_line = CLine;

   tmpm = 1; pop_mark(&tmpm);
    c1 = calculate_column();
    if (c1 == c2)
      {
	 msg_error(Rect_Error);
	 pop_spot();
	 return(0);
      }
    if (c1 > c2)
      {
	 tmp = c1;
	 c1 = c2;
	 c2 = tmp;
	 goto_column(&c1);
      }

   Suspend_Screen_Update = 1;
    /* go through the region copying rectanglar blocks to Rectanglebuffer */
    dc = c2 - c1;
   if (dc == 1) dc_malloc = 2; else dc_malloc = dc;
   		       /* length of 1 not usable here.
			* See makeline1 to see why 
			*/
    line = beg = make_line1(dc_malloc);
    beg->prev = NULL;
    while (1)
      {
	 data = line->data;
	 
	 /* p2 = data + dc; p1 = data;
	 while (p1 < p2) *p1++ = ' '; */
	 MEMSET((char *) data, ' ', dc);
	 line->len = dc;

	 if (c1 == goto_column1(&c1))
	   {
	      p1 = CLine->data + Point;
	      (void) goto_column1(&c2);
	      p2 = CLine->data + Point;
	      Point = (int) (p1 - CLine->data);
	      n = (int) (p2 - p1);
	      MEMCPY((char *) data, (char *) p1, n);
	      deln (&n);
	      /* while(n-- > 0)
	        {
		   *data++ = *p1;
		   del();
		} */
	   }
	 if (CLine == save_line) break;
	 CLine = CLine->next;  LineNum++;

	 line->next = make_line1(dc_malloc);
	 line->next->prev = line;
	 line = line->next;
      }

    line->next = NULL;

    Rectangle_Buffer->line = Rectangle_Buffer->beg = beg;
    Rectangle_Buffer->end = line;
    Rectangle_Buffer->point = 0;

    pop_spot();
    return(0);
}

int blank_rectangle()
{
   int c1, n, c2, pnt, tmpm;
   Line *save_line;
   int nn;
   

   CHECK_READ_ONLY
   if (!check_region(&Number_One)) return(0); /* push_spot(); performed */

   c1 = calculate_column();
   save_line = CLine;
   tmpm = 1; pop_mark(&tmpm);
   c2 = calculate_column();
   n = c2 - c1;
   if (n < 0)
     {
	n = -n;
	c1 = c2;
     }
   
   Suspend_Screen_Update = 1;
   while(1)
     {
	goto_column(&c1);
	pnt = Point;
	eol();
	nn = Point - pnt;
	if (nn > n) nn = n;
	Point = pnt;
	
	deln(&nn);
	ins_char_n_times( ' ', nn);
	
	if (CLine == save_line) break;
	CLine = CLine->next;
	LineNum++;
     }
   pop_spot();
   /* mark_buffer_modified(&Number_One); */
   return(1);
}

/*  User Marks */

typedef struct 
{
   Mark m;			       /* MUST be the first */
   Buffer *b;
}
User_Mark_Type;

static void free_user_mark (User_Mark_Type *um)
{
   Mark *m, *m1;
   Buffer *b;

   m1 = &um->m;

   /* The mark is only valid if the buffer that it was created for still
    * exists.
    */
   if ((m1->flags & MARK_INVALID) == 0)
     {
	/* Unlink the mark from the chain. */
	b = um->b;
	m = b->user_marks;
   
	if (m == m1)	b->user_marks = m1->next;
	else 
	  {
	     while (m->next != m1) m = m->next;
	     m->next = m1->next;
	  }
     }
   
   SLFREE (um);
}

void free_user_marks (Buffer *b)
{
   Mark *m = b->user_marks;
   
   while (m != NULL)
     {
	m->flags |= MARK_INVALID;
	m = m->next;
     }
}

static int mark_valid (Mark *m)
{
   if (m->flags & MARK_INVALID)
     {
	msg_error ("Mark is invalid.");
	return 0;
     }
   return 1;
}


void goto_user_mark (void)
{
   SLuser_Object_Type *uo;
   User_Mark_Type *um;
   
   if ((uo = SLang_pop_user_object (JED_MARK_TYPE)) == NULL) return;
   um = (User_Mark_Type *) uo->obj;
   
   if (mark_valid (&um->m))
     {
	if (CBuf != um->b) msg_error ("Mark not in buffer.");
	else
	  goto_mark (&um->m);
     }
   SLang_free_user_object (uo);
}

void create_user_mark (void)
{
   User_Mark_Type *um;
   SLuser_Object_Type *uo;
   Mark *m;
   
   uo = SLang_create_user_object (JED_MARK_TYPE);
   if (uo == NULL) return;
   

   if (NULL == (um = (User_Mark_Type *) SLMALLOC (sizeof(User_Mark_Type))))
     {
	SLang_Error = SL_MALLOC_ERROR;
	SLFREE (uo);
	return;
     }
   m = &um->m;
   
   m->line = CLine;
   m->point = Point;
   m->n = LineNum + CBuf->nup;
   m->next = CBuf->user_marks;
   m->flags = 0;
   
   CBuf->user_marks = m;
   
   um->b = CBuf;   
   uo->obj = (long *) um;
   SLang_push_user_object (uo);
}

   
   
char *user_mark_buffer (void)
{
   SLuser_Object_Type *uo;
   User_Mark_Type *um;
   char *s = "";
   
   if (NULL == (uo = SLang_pop_user_object (JED_MARK_TYPE))) return s;
   um = (User_Mark_Type *) uo->obj;
   
   if (mark_valid (&um->m))
     {
	s = um->b->name;
     }
   
   SLang_free_user_object (uo);
   return s;
}

   
int register_jed_classes (void)
{
   return SLang_register_class (JED_MARK_TYPE, (VOID *) free_user_mark, NULL);
}
