/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#ifdef msdos
#include <alloc.h>
#endif

#include "config.h"
#include "buffer.h"
#include "window.h"
#include "screen.h"
#include "misc.h"
#include "ledit.h"
#include "sysdep.h"
#include "display.h"

Window_Type *JWindow;
extern Window_Type *The_MiniWindow;
int Top_Window_Row = 2;

Window_Type *create_window(int top, int rows, int col, int width)
{
    Window_Type *w;

    if (NULL == (w = (Window_Type *) SLMALLOC(sizeof(Window_Type))))
      {
	  exit_error("create_window: malloc error.", 0);
      }

   MEMSET ((char *) w, 0, sizeof (Window_Type));
   w->top = top;
   w->rows = rows;
   w->width = width;
   w->column = col;

   return(w);
}

void window_buffer(Buffer *b)
{
   if (JWindow == NULL)
     {
	JWindow = create_window(Top_Window_Row, 
				*tt_Screen_Rows - 2 - (Top_Window_Row - 1),
				1, *tt_Screen_Cols);
	JWindow->next = JWindow;
     }

   touch_window();
   JWindow->mark.line = b->line;
   JWindow->mark.point = b->point;
   JWindow->mark.n = b->linenum + b->nup;
   JWindow->beg.line = b->line;
   JWindow->beg.point = b->point;
   JWindow->buffer = b;
   JWindow->trashed = 1;
}

int other_window()
{
   switch_to_buffer(JWindow->buffer);
   /* CBuf->line = CLine;
   CBuf->point = Point; */
   JWindow->mark.point = Point;
   JWindow->mark.line = CLine;
   JWindow->mark.n = LineNum + CBuf->nup;
   /* JWindow->buffer = CBuf; */

   JWindow = JWindow->next;
   switch_to_buffer(JWindow->buffer);
   /* CBuf = JWindow->buffer; */
   Point = JWindow->mark.point;
   CLine = JWindow->mark.line;
   LineNum = JWindow->mark.n - CBuf->nup;
   return(1);
}

int find_screen_line(void)
{
   int i;
   for (i = 0; i < *tt_Screen_Rows - 2; i++)
     {
	if (JScreen[i].line == CLine) return(i + 1);
     }
   return(0);
}

int split_window()
{
   int n, top, width, row;
   Window_Type *w, *neew;
   Line *l;

    if (JWindow->rows < 5)
      {
	  msg_error("Window too small.");
	  return(0);
      }

   switch_to_buffer(JWindow->buffer);
    n = JWindow->rows / 2;
    top = JWindow->top + n + 1;
    width = JWindow->width;
    n = JWindow->rows - n - 1;
    JWindow->rows = JWindow->rows / 2;

    JWindow->beg.line = find_top();
    w = JWindow->next;
    JWindow->next = neew = create_window(top, n, JWindow->column, width);

   neew->next = w;
   neew->buffer = CBuf;
   neew->mark.point = Point;
   neew->mark.line = CLine;
   neew->mark.n = LineNum + CBuf->nup;

    other_window();
    touch_window();
   l = JWindow->beg.line = find_top();
   n = 0;
   while (l != CLine) 
     {
	l = l->next;
	n++;
     }
   JWindow->beg.n = LineNum + CBuf->nup - n;
   
   /* Try to leave Point on same line of display if possible */
   if ((row = find_screen_line()) > 0)
     {
	w = JWindow;
	do
	  {
	     if ((JWindow->buffer == CBuf) && (JWindow->top <= row)
		 && (JWindow->top + JWindow->rows > row)) break;
	     other_window();
	  }
	while (w != JWindow);
     }

    return(1);
}

int one_window()
{
    Window_Type *w, *next, *mini;
   Buffer *b;
    mini = NULL;
    if (JWindow->top == *tt_Screen_Rows) return(0);  /* mini-buffer */
    w = JWindow->next;
   b = JWindow->buffer;
    while(w != JWindow)
      {
	  next = w->next;
	  if (w != The_MiniWindow) 
	   {
	      if (w->buffer != b) touch_window_hard (w, 0);
	      SLFREE(w);
	   }
	 else mini = w;
	 w = next;
      }
    if (mini == NULL) mini = JWindow;
    JWindow->next = mini;
    mini->next = JWindow;
    JWindow->top = Top_Window_Row;
    JWindow->rows = *tt_Screen_Rows - 2 - (Top_Window_Row - 1);
    touch_window();
    return(1);
}

int enlarge_window()
{
   Window_Type *w, *w1;
   int min = 2;

   if (JWindow == The_MiniWindow) return(0);
   /* if (IS_MINIBUFFER) return(0); */
   if (JWindow == JWindow->next) return(0);
   w = JWindow->next;
   while(w->rows <= min) w = w->next;
   if (w == JWindow) return(0);

   if (w->top < JWindow->top)
     {
	w->rows -= 1;
	JWindow->rows += 1;
	do
	  {
	     w = w->next;
	     w->top -= 1;
	  }
	while (w != JWindow);
     }
   else
     {
	JWindow->rows += 1;
	w1 = JWindow;
	while(w1 != w)
	  {
	     w1 = w1->next;
	     w1->top += 1;
	  }
	w->rows -= 1;
     }
   w = JWindow;
   do
     {
	touch_window();
	JWindow = JWindow->next;
     }
   while (w != JWindow);
   return(1);
}

void adjust_windows(int height)
{
   Window_Type *w = JWindow;
   int rows;

   do
     {
	if (w->rows + w->top + 1 == *tt_Screen_Rows)
	  {
	     rows = height - 1 - w->top;
	     if (rows > 1)
	       {
		  w->rows = rows;
		  return;
	       }
	     while(JWindow->top != Top_Window_Row) other_window();
	     one_window();
	     JWindow->rows = height - 2;
	     return;
	  }
	w = w->next;
     }
   while (w != JWindow);
   /* not reached! */
}

void change_screen_size(int width, int height)
{
   Window_Type *w;

   if (JWindow == NULL) return;

   if (height != *tt_Screen_Rows)
     {
	adjust_windows(height);
     }
   reset_display();
   *tt_Screen_Cols = width;
   *tt_Screen_Rows = height;
   init_display(0);
   w = JWindow;
   do
     {
	JWindow->width = width;

	/* touch_window(); */
	JWindow = JWindow->next;
     }
   while(w != JWindow);
   if (The_MiniWindow != NULL)
     {
	The_MiniWindow->top = height;
	The_MiniWindow->width = width;
     }
   /* cls();
   update((Line*) NULL); */
   redraw_screen (1);
}

Window_Type *buffer_visible(Buffer *b)
{
   Window_Type *w = JWindow;

   do
     {
	if (w->buffer == b) return(w);
	w = w->next;
     }
   while (w != JWindow);
   return(NULL);
}

int delete_window()
{
   Window_Type *tthis, *prev, *next;
   int nr1;
   
   tthis = JWindow;
   next = tthis->next;
   if ((MiniBuffer_Active && ((tthis == The_MiniWindow) || (tthis == next->next)))
       || (tthis == next)) return(0);
   
   
   
   nr1 = tthis->top + tthis->rows + 1;
   if (nr1 != *tt_Screen_Rows)
     {
	while (JWindow->top != nr1) other_window();
	JWindow->top = tthis->top;
     }
   else
     {
	while(JWindow->top + JWindow->rows + 1 != tthis->top) other_window();
     }
   
   JWindow->rows += tthis->rows + 1;
   touch_window();
   
   prev = next;
   while(prev->next != tthis) prev = prev->next;
   prev->next = next;

   SLFREE(tthis);
   return(1);
}

void touch_window_hard(Window_Type *w, int all)
{
   int i;
   Window_Type *wsave = w;
   
   do 
     {
	for (i = 0; i < w->rows; i++)
	  {
	     JScreen[i + w->top - 1].flags = 1;
	     JScreen[i + w->top - 1].line = NULL;
	  }
	w->trashed = 1;
	w = w->next;
     }
   while (all && (w != wsave));
}

void touch_screen_for_buffer(Buffer *b)
{
   Window_Type *w;
   
   w = JWindow;
   do
     {
	if (w->buffer == b)
	  {
	     touch_window_hard (w, 0);
	  }
	w = w->next;
     }
   while(w != JWindow);
}
