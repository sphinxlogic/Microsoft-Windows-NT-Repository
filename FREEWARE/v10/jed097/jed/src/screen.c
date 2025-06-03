/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#ifdef FLOAT_TYPE
# define JED_VERSION "0.F97.8b"
#else
# define JED_VERSION "0.97.8b"
#endif 

#include "config.h"

#include <stdio.h>
#include <string.h>
#ifdef pc_system
#include <dos.h>
#endif

#include "buffer.h"
#include "screen.h"
#include "window.h"
#include "paste.h"

#include "ins.h"
#include "ledit.h"
#include "display.h"
#include "sysdep.h"
#include "misc.h"
#include "vterm.h"
#include "slang.h"
#include "hooks.h"

#define fputs(s,f) write(fileno(f), s, strlen(s))

Screen_Type JScreen[MAX_SCREEN_SIZE];

int Screen_Row = 1;
int Screen_Col = 1;
int Cursor_Motion;    /* indicates cursor movement only -1 ^ v +1 < > */
int Scroll_Region_1;
int Scroll_Region_2;
int Scroll_Lines;
int User_Prefers_Line_Numbers = 0;
int Mode_Has_Syntax_Highlight;
int Wants_Syntax_Highlight;	       /* if non-zero, highlight the syntax.
					*/
int Wants_Attributes = 1;
int Wants_HScroll = 20;		       /* controls automatic horizontal
					* scrolling.  If positive, scroll
					* only line, if negative, whole wind
					*/
int Term_Supports_Color = 1;	       /* optimistic assumption */
static Line *HScroll_Line;
static int HScroll;		       /* amount to scroll line by */
static int Absolute_Column;

void (*X_Update_Open_Hook)(void);      /* hooks called when starting */
void (*X_Update_Close_Hook)(void);     /* and finishing update */

int Goal_Column;
int Display_Eight_Bit = 0;
int JED_CSI = -1;

unsigned short Point_On_Char;		       /* character at cursor position */
static int Point_Cursor_Flag = 1;      /* if non-zero, point cursor */

static Line Eob_Line =
{
   NULL, NULL, (unsigned char *) "[EOB]", 5, 5
};

int Want_Eob = 0;
int Display_Time = 1;                  /* Turn on %t processing in status line */

void blank_line(int row)
{
   int n = JWindow->width;
   register unsigned short *p, *pmax;
   p = JScreen[row].old;
   pmax = p + n;

   while (p < pmax) {*p++ = 32; *p++ = 32; *p++ = 32; *p++ = 32;}
}

void update_screen_txt(int row)
{
   unsigned short *tmp;
    tmp = JScreen[row].neew;
    JScreen[row].neew = JScreen[row].old;
    JScreen[row].old = tmp;
}
static unsigned char char_width[256];
static int dis8bit = -100;
static int j_csi = -1;
static int Selective_Display = -0x7FFF;

#define FIX_CHAR_WIDTH \
  if ((Display_Eight_Bit != dis8bit) || (JED_CSI != j_csi)\
      || (Selective_Display != Buffer_Local.sd)) fix_char_width()


static int fix_attributes(register unsigned short *p, register unsigned short *pmax, 
			  unsigned int color)
{
   if (pmax == NULL) pmax = p + JWindow->width;
   if (p >= pmax) return(0);
   color = color << 8;
   while (p < pmax)
     {
	*p = color | (*p & 0xFF);
	p++;
     }
   return(1);
}


int output(unsigned char *line, int len, int row, register int max_col, 
	   register int column_offset, unsigned char *hi_beg, unsigned char *hi_end)
{
   register unsigned char ch, *p, *pmax, *b;
   register unsigned short *outp;
   register int count;
   unsigned char expb[80];
   unsigned short *out, *mark, *hi0_outp = NULL, *hi1_outp = NULL, *sp, *spmax;
   int tab, tabsize, visible, i, mini, w;
   int done = 0;

   outp = out = JScreen[row - 1].neew;
   if (outp == NULL) return 1;
   tab = Buffer_Local.tab;
   count = 0;
   visible = 0;

   mini = 1;
   while(mini >= 0)
     {
	/* deal with mini_buffer prompt */
	if (mini && (IS_MINIBUFFER) && Mini_Info.prompt_len)
	  {
	     p = (unsigned char *) Mini_Info.prompt;
	     pmax = p + Mini_Info.prompt_len;
	     mini = 0;
	  }
	else
	  {
	     p = line;
	     pmax = p + len;
	     mini = -1;
	  }

	while(p < pmax)
	  {
	     if (p == hi_beg) hi0_outp = outp;
	     if (p == hi_end) hi1_outp = outp;

	     ch = (unsigned char) *p++;
	     b = expb;
	     /* expand char to display form */
	     
	     w = *(char_width + (unsigned int) ch);
	     if (w == 1)
	       {
		  /* try to optimize this by putting this here */
		  
		  count++;
		  if ((count < max_col) && (count >= column_offset))
		    {
		       *outp++ = (unsigned short) ch;
		    }
		  else if (count >= max_col)
		    {
		       done = 1;
		       p = pmax;
		    }
		  continue;
	       }
	     else if (w == 3)
	       {
		  if (ch == '\r')
		    {
		       *b++ = '.'; *b++ = '.'; *b++ = '.';
		       done = -1;
		       p = pmax;
		    }
		  else
		    {
		       *b++ = '~';
		       *b++ = '^';
		       
		       /* Be better to replace following by:
			* *b++ = ch - '@';
			*/
		       ch &= 0x7F;
		       *b++ = (ch == 127) ? '?' : ch  + 'A' - 1;
		    }
		  
	       }
	     else if (w == 2)
	       {
		  if ((ch == '\t') && (tab > 0))
		    {
		       tabsize = tab * (count / tab + 1) - count;
		       i = 0; while (i++ < tabsize) *b++ = ' ';
		       w = tabsize;
		    }
		  else if (ch & 0x80)
		    {
		       *b++ = '~'; *b++ = ch & 0x7F;
		    }
		  else
		    {
		       *b++ = '^';
		       /* Be better to replace following by:
			* *b++ = (ch | 0x80) - '@';
			*/
		       
		       *b++ = (ch == 127) ? '?' : ch  + 'A' - 1;
		    }
	       
	       }
	     else		       /* w = 0 */
	       {
		  ch = *p++;	       /* skip next char */
		  continue;
	       }

	     b = expb;
	     while (w--)
	       {
		  count++;
		  if (count >= max_col)
		    {
		       done = 1;
		       p = pmax;
		    }
		  
		  else if (count >= column_offset)
		    {
		       *outp++ = (unsigned short) *b;
		    }
		  b++;
	       }
	  }
     }


   sp = out;
   while (sp < outp) 
     {
	if (*sp++ != ' ') 
	  {
	     visible++;
	     break;
	  }
     }
   
   if (outp == out)
     {
	*outp++ = '$';
	visible = 1;
     }
   else if (column_offset > 1)
     {
	*out = '$';
	visible++;
     }
   
   if (Wants_Syntax_Highlight && Mode_Has_Syntax_Highlight
#if !defined(pc_system) && !defined(__os2__)
       && (*tt_Use_Ansi_Colors && Term_Supports_Color)
#endif
       && (line != Eob_Line.data))
     syntax_highlight (out, outp);

   
   mark = outp;
   sp = outp;  spmax = out + JWindow->width;
#ifdef msdos
   asm mov bx, di
   asm mov cx, word ptr spmax
   asm sub cx, word ptr outp
   asm shr cx, 1
   asm mov ax, 32
   asm les di, dword ptr outp
   asm cld
   asm rep stosw
   asm mov di, bx
   /* Note that outp is not updated here! */
#else
/*   MEMSET((char *) sp, (char) ' ',  sizeof(short) * (int) (spmax - sp)); */
   /* This line burns some CPU */
   while (sp < spmax) {*sp++ = 0x0020; *sp++ = 0x0020; *sp++ = 0x0020; *sp++ = 0x0020;}
#endif
   if (done == 1)
     {
	mark = out + (JWindow->width - 1);
	*mark = '$';
	visible += 1;
	if ((hi_beg != NULL) && (hi0_outp == NULL)) hi0_outp = mark;
	mark = NULL;
     }
   out[JWindow->width] = '\0';
   
   /* fix highlighting on this line */
   if ((Wants_Attributes) && (hi0_outp != NULL))
     {
	if (hi1_outp == NULL) hi1_outp = outp + 1;
	visible += fix_attributes(hi0_outp, hi1_outp, JREGION_COLOR);
     }
   

   if (visible)
     {
#ifdef msdos
	(void) mark;
	/* 2nd and last parameters not used in msdos smartputs so they do
	   not need to be valid */
	tt_smart_puts(out, out, *tt_Screen_Cols, row - 1);
#else
	/* if (mark != NULL) *mark = 0; */
	tt_smart_puts(out, JScreen[row-1].old, *tt_Screen_Cols, row - 1);
	/* if (mark != NULL) *mark = ' ';  */
#endif
	JScreen[row-1].n = visible;
	Point_Cursor_Flag = 1;
     }

   /* else take care of it in calling routine */
   return(visible);
}

void display_line(Line *line, int row)
{
   int len, cofs = JWindow->column;
   Screen_Type *s = &JScreen[row - 1];

   if (line != NULL)
     {
	len = line->len;
	if (len && (row != *tt_Screen_Rows)
	    && (line->data[len - 1] == '\n')) 
	  {
	     if ((s->hi1 != NULL) && (s->hi1 == line->data + len)) s->hi1--;
	     len--;
	  }
	
	if ((len > 0) || (cofs > 1)
	    || ((row == *tt_Screen_Rows) && Mini_Info.prompt_len))
	  {
	     if (Wants_HScroll && (line == HScroll_Line) && HScroll)
	       {
		  cofs += HScroll;
	       }
		 
	     len = output(line->data, len, row,
			  cofs + JWindow->width - 1, cofs,
			  s->hi0, s->hi1);
	  }
     }
   else len = 0;
   
   if (len <= 0)
     {
	if (s->n)
	  {
	     tt_goto_rc(row - 1, 0);
	     Screen_Row = row;
	     tt_del_eol();
	  }
	blank_line(row - 1);
	s->n = 0;
     }
   s->line = line;
   s->flags = 0;
   /* s->hi0 = s->hi1 = NULL; */
   if (len > 0) update_screen_txt(row-1);
}

static void mark_window_attributes(int wa)
{
   register Screen_Type *s = &JScreen[JWindow->top - 1], 
     *smax = s + JWindow->rows, *s1, *s2;
   Mark *m;
   register Line *l = JWindow->beg.line, *ml;
   unsigned char *hi0, *hi1;
   int mn, pn, dn;
   
   s1 = s;
   
   if ((CBuf->vis_marks == 0) || (wa == 0) || (Wants_Attributes == 0))
     {
	s2 = s;
	goto done;		       /* I hate gotos but they are convenient */
     }
   m = CBuf->marks;
   
   while ((m->flags & VISIBLE_MARK) == 0) m = m->next;	
   ml = m->line;
   mn = m->n;			       /* already in canonical form */
   pn = LineNum + CBuf->nup;	       /* not in canonical form */
   dn = pn - mn;
   
   while (l != CLine)		       /* find pos of point in window */
     {
	s1++;
	l = l->next;
     }
   
   /* s1 now points at current line */
   /* The whole point of all of this is to preserve the screen flags without
      touching the screen.  */
   
   if (dn > 0)			       /* mark on prev lines */
     {
	s2 = s1 + 1;
	hi0 = l->data;
	hi1 = l->data + Point;
	if ((s1->hi0 != hi0) || (s1->hi1 != hi1))
	  {
	     s1->hi0 = hi0; s1->hi1 = hi1;
	     s1->flags = 1;
	  }
	l = l->prev; s1--;
	while ((s1 >= s) && (l != ml)) 
	  {
	     hi0 = l->data;
	     hi1 = l->data + l->len;
	     if ((s1->hi0 != hi0) || (s1->hi1 != hi1))
	       {
		  s1->hi0 = hi0; s1->hi1 = hi1;
		  s1->flags = 1;
	       }
	     l = l->prev; s1--;
	  }
	if (s1 >= s)
	  {
	     hi0 = ml->data + m->point;
	     hi1 = ml->data + ml->len;
	     if ((s1->hi0 != hi0) || (s1->hi1 != hi1))
	       {
		  s1->hi0 = hi0; s1->hi1 = hi1;
		  s1->flags = 1;
	       }
	     s1--;
	  }
     }
   else if (dn < 0)		       /* mark ahead of point */
     {
	s2 = s1;
	s1--;
	hi0 = l->data + Point;
	hi1 = l->data + l->len;
	if ((s2->hi0 != hi0) || (s2->hi1 != hi1))
	  {
	     s2->hi0 = hi0; s2->hi1 = hi1;
	     s2->flags = 1;
	  }
	
	l = l->next;
	s2++;
	while ((s2 < smax) && (l != ml)) 
	  {
	     hi0 = l->data;
	     hi1 = l->data + l->len;
	     if ((s2->hi0 != hi0) || (s2->hi1 != hi1))
	       {
		  s2->hi0 = hi0; s2->hi1 = hi1;
		  s2->flags = 1;
	       }
	     l = l->next;
	     s2++;
	  }
	if (s2 < smax)
	  {
	     hi0 = ml->data;
	     hi1 = ml->data + m->point;
	     if ((s2->hi0 != hi0) || (s2->hi1 != hi1))
	       {
		  s2->hi0 = hi0; s2->hi1 = hi1;
		  s2->flags = 1;
	       }
	     s2++;
	  }
     }
   else				       /* same line */
     {
	if (Point < m->point)
	  {
	     s1->hi0 = l->data + Point;
	     s1->hi1 = l->data + m->point;
	  }
	else
	  {
	     s1->hi1 = l->data + Point;
	     s1->hi0 = l->data + m->point;
	  }
	s1->flags = 1;
	s2 = s1 + 1;
	s1--;
     }
   
   done:			       /* reached if there is no mark */
   
   /* now do area outside the region */
   while (s1 >= s)
     {
	if (s1->hi0 != NULL)
	  {
	     s2->hi1 = s1->hi0 = NULL;
	     s1->flags = 1;
	  }
	s1--;
     }
   
   while (s2 < smax)
     {
	if (s2->hi0 != NULL)
	  {
	     s2->hi1 = s2->hi0 = NULL;
	     s2->flags = 1;
	  }
	s2++;
     }
}


void open_scroll(void)
{
    Scroll_Region_1 = 1;
    Scroll_Region_2 = *tt_Screen_Rows;
    Scroll_Lines = 0;
}

void do_scroll_up(int n)
{
   tt_goto_rc(0,0);
   tt_delete_nlines(n);
}

void do_scroll_down(int n)
{
   tt_goto_rc(0, 0);
   tt_reverse_index(n);
}

void execute_scroll(void)
{
   if (Scroll_Lines > 0)
     {
	tt_set_scroll_region(Scroll_Region_1 - 1, Scroll_Region_2 - 1);
	execute_vscroll_up(Scroll_Region_1, Scroll_Region_2, Scroll_Lines);
	do_scroll_up(Scroll_Lines);
	
     }
   else if (Scroll_Lines < 0)
     {
	tt_set_scroll_region(Scroll_Region_1 - 1, Scroll_Region_2 - 1);
	execute_vscroll_down(Scroll_Region_1, Scroll_Region_2, -Scroll_Lines);
	do_scroll_down(-Scroll_Lines);
     }
   Scroll_Lines = 0;
}

void queue_scroll(int n, int r1, int r2)
{
   if ((r1 != Scroll_Region_1) || (r2 != Scroll_Region_2))
     {
/*	if ((n == -1) && (Scroll_Region_1 == r1 + Scroll_Lines)
	       && (Scroll_Region_2 == r2))  */
	if ((n == -1) && (Scroll_Region_1 == r1 + Scroll_Lines)
	    && (Scroll_Region_2 == r2))  Scroll_Lines--;
	else if ((n == -1) && (Scroll_Region_1 == r1 + Scroll_Lines)
		 && (Scroll_Region_2 == r2 - 1))
	  {
	     Scroll_Region_2 = r2;
	     Scroll_Lines--;
	  }

	else
	  {
	     execute_scroll();
	     Scroll_Region_1 = r1;
	     Scroll_Region_2 = r2;
	     Scroll_Lines = n;
	  }
     }
   else Scroll_Lines += n;
}

void close_scroll(void)
{
    if (Scroll_Lines) execute_scroll();
    if ((Scroll_Region_1 != 1) || (Scroll_Region_2 != *tt_Screen_Rows))
     tt_reset_scroll_region();
}

/* Here a scrolling region (t,b) is used to scroll line t + n to t.  All
   that is assumed is that  the line at t + n exists! */

int scroll_up(int n, int t, int b)
{
    int i, necess;

    if (n == 0) return(0);

    /* t = JWindow->top - 1 + t;
       b = JWindow->top - 1 + b; */

    /* check to see if this is really necessary */
    necess = 0;
    for (i = t - 1; i < b; i++)
      {
          if (JScreen[i].n)
            {
                necess = 1;
                break;
            }
      }

    if (!necess) return(0);

    queue_scroll(n, t, b);

    vscroll_up(t,b,n);

    return(necess);
}

/* Here a scrolling region (t,b) is used to scroll line t + n to t. */

void scroll_down(int n, int t, int b)
{
    if (n == 0) return;

    /* t = JWindow->top - 1 + t;
       b = JWindow->top - 1 + b; */
    tt_set_scroll_region(t - 1,b - 1);
    tt_reverse_index(n);
    tt_reset_scroll_region();

    vscroll_down(t,b,n);
}

int update_insert_line(int r1, Line *line)
{
    int i, r2, r, necess;
    Line *bot;

    /* normalize r1: */
    /* r1 = JWindow->top - 1 + r1; */

    /* find the first one that is blank to delimit the region so that the
       loss will be minimal */

    r = r1;   /* not r1 + 1 as obvious (but naive) as it seems */
    r2 = JWindow->rows + JWindow->top - 1;
    while (r < r2)
      {
	 bot = JScreen[r - 1].line;
	 if ((bot == NULL) || (bot == &Eob_Line)) break;
          r++;
      }

    if ((r1 != r2) && (r == r2))
    /* we may have failed so check the bottom up so we don't push linesoff. */
     {
	bot = line;
	for (r = r1 + 1; r <= r2; r++)
	  {
	     bot = bot->next;
	     if (bot == NULL) break;
	     if (JScreen[r-1].line == bot) break;
	  }
	if ((bot != NULL) && (r <= r2)) r--;
	if (r > r2) r = r2;
     }

   if (r < r1) r = r1;
   r2 = r;

    /* check to see if we gain by doing this */
    necess = 0;
    for (i = r1 - 1; i < r2; i++)
      {
          if (JScreen[i].n)
            {
                necess = 1;
                break;
            }
      }
    if (r1 == r2) return(0);
    if (!necess) return(0);

    queue_scroll(-1, r1, r2);

    vscroll_down(r1, r2, 1);
    return(1);
}

Line *find_top()
{
    int n, i;
    Line *line, *next, *prev, *tthis;
   
    n = JWindow->rows - 1;

    if (!n) return(CLine);
    /* Check the top window line.  Chances are that one of the lines
     above CLine will match in usual situations */
    line = JScreen[JWindow->top - 1].line;
    prev = CLine;

    if (line != NULL) for (i = 0; i < JWindow->rows; i++)
      {
          if (prev == line) return(line);
          tthis = prev->prev;
          if (tthis == NULL) break;
          else prev = tthis;
      }

    /*  That was the obvious choice now try some others */
    /* try bottom (or punt) */
   if (*tt_Term_Cannot_Scroll) 
     {
	if (*tt_Term_Cannot_Scroll == -1)
	  {
	     if ((line != NULL) && (CLine->next == line)) return (CLine);
	     if (CLine->prev != JScreen[JWindow->rows + JWindow->top - 2].line) n = n / 2;
	  }
	else n = n / 2;
	goto no_scroll;
     }
   
   line = JScreen[JWindow->rows + JWindow->top - 2].line;
   next = CLine;

    if (line != NULL) for (i = 0; i < JWindow->rows; i++)
      {
          if (next == line)
            {
                line = CLine;
                while(i < JWindow->rows - 1)
                  {
                      prev = line->prev;
                      if (prev == NULL) return(line);
                      line = prev;
                      i++;
                  }
                return(line);
            }
          next = next->next;
          if (next == NULL) break;
      }
    /* try to find CLine somewhere in the window */

    next = CLine->next;
    prev = CLine->prev;

    for (i = JWindow->top - 1; i < JWindow->top - 1 + JWindow->rows; i++)
      {
         tthis = JScreen[i].line;
         if (tthis == CLine)
           {
              line = CLine;
              while(i > JWindow->top - 1)
                {
		    i--;
		    line = line->prev;
		    if (line == NULL) return(CLine);
                }

              return(line);
           }
         else if ((tthis == prev) && (tthis != NULL))
           {
               i--;
               while(i-- > JWindow->top - 1)
                 {
                     if (prev->prev == NULL) return(prev);
                     prev = prev->prev;
                 }
               return(prev);
           }
         else if ((tthis == next) && (tthis != NULL))
           {
               i++;
               while(i-- > JWindow->top - 1)
                 {
                     if (next->prev == NULL) return(next);
                     next = next->prev;
                 }
               return(next);
           }

      }

    /* not found so check neighbors */
    line = CLine->next;
    if ((line != NULL) && (line == JScreen[JWindow->top - 1].line)) return(CLine);

    line = CLine->prev;
    if ((line == NULL) || (line != JScreen[n + JWindow->top - 1].line))
      {
         n = n / 2;
      }
   
   no_scroll:			       /* on terms that cannot scroll */
   
    line = prev = CLine;
    while(n--)
      {
          line = line->prev;
          if (line == NULL) return(prev);
          prev = line;
      }

    return(line);
}

void do_scroll(register Line *top)
{
   Line *bot, *middle1, *middle2, *s;
   register Line *sline;
   int r1, r, r2, match, overwrite = 1, scroll_region_set = 0, srs, nrows;
   int n1, n2, n3 = 0;
   /* unsigned short sh; */
   
   nrows = JWindow->rows;
    r1 = JWindow->top;
    r2 = r1 + nrows - 1;
   
   /* Actually it might be more benificial to extend r2 to a status line.  
     The status line is easy to detect since it begins with a highlighted
     minus sign.  */
/*   
   r = r2;
   while (r < *tt_Screen_Rows - 1)
     {
	sh = JScreen[r].old[0];
	if (((sh & 0xFF) == '-') && (sh >> 8)) break;
	r++;
     }
   nrows = r - r1 + 1;
   r2 = r;
*/
     

    open_scroll();
    if (top != NULL) bot = top->next; else bot = NULL;

   
    middle1 = middle2 = NULL;
   r = 1;
   bot = top;
   n1 = nrows / 3;
   n2 = 2 * n1;
   
   /* I do not want to use CLine and the previous one as a basis 
    for determining scroll region since we may have just created it. 
    There is nothing right or wrong about it--- just a feeling that
    results in optimization of screen update. */
   while (r++ < nrows)
     {
	if ((r >= n1) && (middle1 == NULL) && (bot != NULL)
	    && (bot != CLine) && (bot != CLine->prev)) middle1 = bot;
	if ((r >= n2) && (middle2 == NULL) && (bot != NULL)
	    && (bot != CLine) && (bot != CLine->prev))
	
	  {
	     middle2 = bot;
	     n2 = bot->len;
	  }
	
	if (bot != NULL) bot = bot -> next;
     }
   /* if the middle or the bottom lines match, we do not overwrite */

   if (bot != NULL) n3 = bot->len;
   if (middle1 == NULL) overwrite = 0; 
   else
     {
	n1 = middle1->len;
	/* I do not want to scroll if only 1 thing matches */
	for (r = r1; r <= r2; r++) 
	  {
	     s = JScreen[r - 1].line;
	     if (((middle1 == s) && (n1 == s->len)
		  && (middle1->data[n1 / 2] == s->data[n1 / 2]))
		 || ((middle2 == s) && (middle2 != NULL) && (n2 == s->len)
		  && (middle2->data[n2 / 2] == s->data[n2 / 2]))
		 || ((bot != NULL) && (bot == s) && (n3 > 4) && (n3 == s->len)
		     && (bot->data[n3/2] == s->data[n3 / 2])))
	       {
		  overwrite = 0;
		  break;
	       }
	  }
     }
   
   while(r1 <= r2)
     {
	 if (top == NULL) break;
	 match = 0;
	 for(r = r1; r <= r2; r++)
	   {
	      sline = JScreen[r - 1].line;
	      if (sline == top)
		{
		   if (r != r1)
		     {
			scroll_region_set = 0;
			scroll_up(r - r1, r1, r2);
		     }

		   r1++;
		   match = 1;
		   if (top != NULL) top = top->next;
		   break;
		}
	   }

	 if (match) continue;

	 /* not found so insert it */
	 if ((top != NULL) && (!overwrite))
	   {
	      srs = update_insert_line(r1, top);
	      if (!scroll_region_set) scroll_region_set = srs;
	   }
	 if (top != NULL) top = top->next;
	 r1++;
      }

   /*     if (scroll_region_set) reset_scroll_region(); */
   close_scroll();
   if (overwrite) touch_window();
}

static void fix_char_width(void)
{
   int i;
   for (i = 0; i < 32; i++) 
     {
	char_width[i] = 2;
	char_width[i + 128] = 3;
     }
   
   for (i = 32; i < 127; i++) 
     {
	char_width[i] = 1;
	char_width[i + 128] = 2;
     }
   char_width[127] = 2;
   char_width[255] = 3;
   
   if (Display_Eight_Bit > 0)
     {
	for (i = (Display_Eight_Bit > 127 ? Display_Eight_Bit : 128);
	     i < 256; i++)
	  {
	     char_width[i] = 1;
	  }
     }
   dis8bit = Display_Eight_Bit;
   if ((Selective_Display = Buffer_Local.sd) < 0)
     {
	char_width[(unsigned char) '\r'] = 3;
     }
   if (JED_CSI > 0) char_width[(unsigned char) JED_CSI] = 0;
   j_csi = JED_CSI;
}


void point_column(int n)
{
   register unsigned char *p, *pmax;
   register int i;
   int tab, w;

   FIX_CHAR_WIDTH;
   if (IS_MINIBUFFER) n -= Mini_Info.effective_prompt_len;

    p = CLine->data;

    pmax = p + (CLine->len - 1);
    if (*pmax != '\n') pmax++;

    tab = Buffer_Local.tab;
    i = 0;
    n--;   /* start at 0 */
    while(p < pmax)
      {
         if ((*p == '\t') && tab)
	   {
	      i = tab * (i / tab + 1);
	   }
	 else 
	   {
	      w = char_width[*p];
	      i +=  w;
	      if (w == 0) p++;
	   }
	 
	 if ((i > n) || ((Selective_Display < 0) && (*p == '\r'))) break;
	 p++;
      }
    Point = (int) (p - CLine->data);
}

/* given a position in a line, return apparant distance from bol
   expanding tabs, etc... up to pos.  

   Note**** misc.c calls this using the minibuffer prompt.  This routine
   can only depend on Point and CLine->data but not CLine->len!!
*/
int calculate_column()
{
   register int i;
   int tab, w;
   register unsigned char *cw = char_width;
   register unsigned char ch, *pos, *this_pos;

   FIX_CHAR_WIDTH;
   pos = CLine->data;
    this_pos = pos + Point;
    i = 1;
    tab = Buffer_Local.tab;
    while(pos < this_pos)
      {
	 ch = *pos++;
	 if ((ch == '\t') && tab)
	   {
	      i = tab * ((i - 1)/tab + 1) + 1;  /* tab column tabs */
	   }
	 else 
	   {
	      w = cw[ch];
	      /* w = char_width[ch]; */
	      i += w;
	      if (w == 0) pos++;
	   }
	 
	 if ((ch == '\r') && (Selective_Display < 0)) break;
      }
   Absolute_Column = i;

    if (IS_MINIBUFFER) i += Mini_Info.effective_prompt_len;

    Screen_Col = i;
    return (i);
}

void point_cursor(int c)
{
    int r, row;
    Line *tthis;

   if (JWindow->trashed) return;
    r = 0;
   Point_Cursor_Flag = 0;
   for (row = JWindow->top; row < JWindow->top + JWindow->rows; row++)
     {
	 tthis = JScreen[row-1].line;
	 if (tthis == NULL) break;
	 if ((tthis == CLine) || (tthis == &Eob_Line))
	   {
	      r = row;
	      break;
	   }
      }
   
    if (Point >= CLine->len)
      {
	  Point = CLine->len - 1;

	  if (Point < 0) Point = 0;
	  else if ((*(CLine->data + Point) != '\n') 
		   || (CBuf == MiniBuffer)) Point++;
      }

    if (r)
      {
	 if (!c) c = calculate_column();
	 
	 c = c - JWindow->column + 1;
	 if (CLine == HScroll_Line) c -= HScroll;
	 if (c < 1) c = 1; else if (c > JWindow->width) c = JWindow->width;
	 Point_On_Char = JScreen[r - 1].old[c - 1];
	 tt_goto_rc (r - 1 , c - 1);
	 Screen_Row = r;
	 Screen_Col = c;
      }
    else
     {
	msg_error("Point not visible in Window.");
	c = 1;
     }
   flush_output ();
   if (!Cursor_Motion) Goal_Column = c;
}

static unsigned short *stat_cpy(register unsigned short *s1, register unsigned char *v, register unsigned short *smax)
{
   register char ch;
   if (v == NULL) return (s1);
   
   while ((s1 < smax) && ((ch = *v++) != 0))
     {
	if (ch < ' ')
	  {
	     *s1++ = (unsigned short) '^';
	     if (s1 < smax) *s1++ = (unsigned short) (ch + '@');
	  }
	else *s1++ = ch;
     }
   return (s1);
}

/* site.sl should modify this */

char Default_Status_Line[80] = 
       " ^Ke: quit, ^Kg: get file, ^K^W: write file | %b  (%m%n%o) %p";

static unsigned long Status_Last_Time;
static unsigned long Status_This_Time;

static char *status_get_time(void)
{
   static char status_time[10];
   register char *t, ch, *t1;
   char am;
   int n;
   
   if (Display_Time == 0) return (NULL);
   if (Status_This_Time == 0) Status_This_Time = sys_time();
   if (Status_This_Time - Status_Last_Time >= 30)
     {
	Status_Last_Time = Status_This_Time;
	am = 'a';
	t = get_time();
	/* returns a string like:  "Tue Nov 2 13:18:19 1993" */
	t1 = status_time;
	while (ch = *t, (ch <= '0') || (ch > '9')) t++;
	/* on date number, skip it */
	while (*t++ != ' ');
	if (*t == '0') t++;
	if (Display_Time > 0)
	  {
	     n = 0;
	     while ((ch = *t++) != ':')
	       {
		  n = 10 * n + (int) (ch - '0');
	       }
	     if (n >= 12) am = 'p';
	     n = n % 12;
	     if (n == 0) n = 12;
	     if (n >= 10)
	       {
		  n -= 10;
		  *t1++ = '1';
	       }
	     *t1++ = '0' + n;
	     *t1++ = ':';
	     while ((*t1++ = *t++) != ':');
	     *(t1 - 1) = am; *t1++ = 'm'; *t1 = 0;
	  }
	else
	  {
	     *t1++ = '[';
	     while ((*t1++ = *t++) != ':');
	     while ((*t1++ = *t++) != ':');
	     *--t1 = ']'; *++t1 = 0;
	  }
     }
   return (status_time);
}

static unsigned short *finish_status(unsigned short *s1, unsigned short *smax, int col_flag)
{
   unsigned int line, maxline;
   register char *v, ch;
   Line *l;
   int top, rows;
   char pstr[20], *str, col_buf[20];
   unsigned short *s1_save = s1;
   

   /* line = w->mark.n - buf->nup; */
   line = LineNum;
   maxline = Max_LineNum;
   
   if (!User_Prefers_Line_Numbers)
     {
	top = JWindow->top - 1;	rows = JWindow->rows - 1;
	l = JScreen[top + rows].line;
	if (l == CBuf->end) l = NULL;
	if (JScreen[top].line == CBuf->beg)
	  {
	     if (l == NULL) strcpy(pstr,"All");
	     else strcpy(pstr,"Top");
	  }
	else if (l == NULL) strcpy(pstr, "Bot");
	else
	  {
	     sprintf(pstr, "%d%%", 
		     (int) ((line * 100L) / (long) maxline));
	  } 
     }
   else sprintf(pstr, "%d/%d", line, maxline);
   
   v = CBuf->status_line;
   if (*v == 0) v = Default_Status_Line;
   
   while (((ch = *v++) != 0) && (s1 < smax))
     {
	if (ch != '%') *s1++ = ch;
	else 
	  {
	     ch = *v++;
	     switch (ch)
	       {
		case 'f': str = CBuf->file; break;
		case 'n': 
		  if (CBuf->narrow != NULL) str = " Narrow"; else str = NULL;
		  break;
		case 'o':
		  if (CBuf->flags & OVERWRITE_MODE) str = " Ovwrt"; else str = NULL;
		  break;
		case 'b': str = CBuf->name; break;
		case 'p': str = pstr; break;
		case 'v': str = JED_VERSION; break;
		case 'm': str = CBuf->mode_str; break;
		case 't': str = status_get_time(); break;
		case 'c': if (User_Prefers_Line_Numbers > 1)
		    {
		       if (col_flag) (void) calculate_column ();
		       sprintf(col_buf, "%d",  Absolute_Column);
		       str = col_buf;
		    }
		  else 
		    {
		       /* 100 to 1 that there is punctuation before this, kill it */
		       if (s1 > s1_save) s1--;
		       str = NULL;
		    }
		  
		  break;
		    
		       
		case '%': str = "%"; break;
		default: return(s1);
	       }
	     if (str != NULL) s1 = stat_cpy(s1, (unsigned char *) str, smax);
	  }
     }
   return (s1);
}

void set_status_format(char *f, int *local)
{
   char *s;
   if (*local) s = Default_Status_Line; else s = CBuf->status_line;
   strncpy(s, f, 79);
   s[79] = 0;
}



void make_status_line(int col_flag)
{
   unsigned short mrk, flag, spot, star, ubit = '-';
   register unsigned short *s1, *s, *smax;

   if (JWindow->top == *tt_Screen_Rows) return;   /* minibuffer ? */
   s = JScreen[JWindow->rows + JWindow->top - 1].neew;
   smax = s + JWindow->width;
   mrk = flag = spot = star = '-';
   if (CBuf->marks != NULL) mrk = 'm';
   if (CBuf->flags & FILE_MODIFIED) flag = 'd';
   if (CBuf->spots != NULL) spot = 's';
   if (CBuf->flags & BUFFER_TRASHED) star = '*';
   if (CBuf->flags & READ_ONLY) star = '%';
   if (CBuf->flags & UNDO_ENABLED) ubit = '+';

   s1 = s;
   if (JWindow->column != 1) *s1++ = '<'; else *s1++ = '-';
   *s1++ = star; *s1++ = star; *s1++ = mrk; *s1++ = flag; 
   *s1++ = spot; 

   if (CBuf->flags & BINARY_FILE) *s1++ = 'B'; 
#ifdef pc_system
   else if ((CBuf->flags & ADD_CR_ON_WRITE_FLAG) == 0) *s1++ = 'L';
#else
#ifdef unix
   else if (CBuf->flags & ADD_CR_ON_WRITE_FLAG) *s1++ = 'C';
#endif
#endif
   else *s1++ = '-';

   *s1++ = ubit; 
   s1 = finish_status(s1, smax, col_flag);
   if (Defining_Keyboard_Macro) s1 = stat_cpy(s1, (unsigned char *) " [Macro]", smax);
   while (s1 < smax) *s1++ = '-';
   *smax = 0;
   (void) fix_attributes (s, smax, JSTATUS_COLOR);
}

int update_status_line(int col_flag)
{
    int r;

   if (JWindow->top == *tt_Screen_Rows) return(0);   /* minibuffer ? */
   r = JWindow->rows + JWindow->top - 1;
   make_status_line(col_flag);
   
   tt_smart_puts(JScreen[r].neew, JScreen[r].old, *tt_Screen_Cols, r);
   Point_Cursor_Flag = 1;
   update_screen_txt(r);
   JScreen[r].n = *tt_Screen_Cols;
   return(1);
}

/* if force then do update otherwise return 1 if update or 0 if not */
int update_1(Line *top, int force)
{
   int i;
   Window_Type *w;
   int did_eob = 0, time_has_expired = 0;
   

   if (Batch || 
       (!force 
	&& (Executing_Keyboard_Macro || (Repeat_Factor != NULL)
	    || Input_Buffer_Len || input_pending(&Number_Zero) 
	    || (Read_This_Character != NULL)))
       || (CBuf != JWindow->buffer))
       
     {
	 return(0);
     }
   
   if (Suspend_Screen_Update != 0) 
     {
	Suspend_Screen_Update = 0;
	touch_screen ();
     }
   
   
   JWindow->mark.line = CLine;
   JWindow->mark.point = Point;
   JWindow->mark.n = LineNum + CBuf->nup;
   CBuf->linenum = LineNum;
   CBuf->max_linenum = Max_LineNum;
   
   if (Wants_Attributes && CBuf->vis_marks)
     {
	JWindow->trashed = 1;
     }
   
   /* Do not bother setting this unless it is really needed */
   if (Display_Time) 
     {
	Status_This_Time = sys_time();
	time_has_expired = (Status_This_Time > Status_Last_Time + 45);
     }
   
   /* if cursor moves just left right, do not update status line */
   if (!force && !JWindow->trashed && 
       ((JWindow == JWindow->next) || (User_Prefers_Line_Numbers && Cursor_Motion))
	/* if % wanted, assume user is like me and gets annoyed with 
	   screen updates */
       && (User_Prefers_Line_Numbers || time_has_expired))
	   
     {
	update_status_line(0);
	return(1);
     }

   if (!JWindow->trashed && Cursor_Motion) return(1);

    w = JWindow;
    do
      {
	 if (Wants_Syntax_Highlight) init_syntax_highlight ();
	 did_eob = 0;
	 if (top == NULL) top = find_top();
	 JWindow->beg.line = top;

	 /* scroll the screen to optimal location */
	 if (0 == *tt_Term_Cannot_Scroll) do_scroll(top);
	 
	 mark_window_attributes((w == JWindow) || (w->buffer != CBuf));

	 for (i = JWindow->top - 1; i < JWindow->rows + JWindow->top - 1; i++)
	   {
	      /* the next line is really optional */
#ifndef pc_system
	      if (!force && (Exit_From_MiniBuffer ||  
			     input_pending(&Number_Zero))) break;
#endif
	      if ((JScreen[i].line != top) || (JScreen[i].flags)
		  || (Want_Eob && !did_eob && (i != *tt_Screen_Rows - 1)))
		{
		   if (((top == NULL) || (top->len == 0)) 
		       && (Want_Eob && !did_eob && !(CBuf->flags & READ_ONLY)))
		     {
			display_line(&Eob_Line, i + 1);
			
			/* JScreen[i].line = top; */
			did_eob = 1;
		     }
		   else display_line(top, i + 1);
		}
	      if (top != NULL) top = top->next;
	   }
	 
	 HScroll_Line = NULL;
	 Mode_Has_Syntax_Highlight = 0;
	 if (!force && input_pending(&Number_Zero))
	   {
	      while(JWindow != w) other_window();
	      JWindow->trashed = 1;  /* since cursor not pointed */
	      return(0);
	   }
	 else update_status_line(w != JWindow);
	 JWindow->trashed = 0;
	 other_window();
	 top = NULL;
	 /* if (!JWindow->trashed) top = JWindow->beg.line; else  top = NULL; */
	 
      } while(JWindow != w);
   return(1);
}

int Mini_Ghost = 0;

void update_minibuffer(void)
{
   Window_Type *w;

   if (Executing_Keyboard_Macro) return;
   if (MiniBuffer != NULL)
     {
	w = JWindow;
	while (!IS_MINIBUFFER) other_window();
	if ((*Message_Buffer) && JScreen[*tt_Screen_Rows - 1].n) (void) input_pending(&Number_Ten);
	JWindow->beg.line = CLine;
	mark_window_attributes(1);
	display_line(CLine, *tt_Screen_Rows);
	while (w != JWindow) other_window();
	Mini_Ghost = 1;
     }
   else if (Mini_Ghost && !*Error_Buffer && !*Message_Buffer)
     {
	/* if < 0, it is a result of flush message so let it pass this round */
	if (Mini_Ghost < 0) Mini_Ghost = 1;
	else
	  {
	     display_line(NULL, *tt_Screen_Rows);
	     if (!JWindow->trashed) point_cursor(0);
	     Mini_Ghost = 0;
	  }
     }
   else Mini_Ghost = ((*Message_Buffer) || (*Error_Buffer));
}

void do_dialog(char *b)
{
   int len = 0, dout, row = *tt_Screen_Rows - 1;
   char *quit = "Quit!";
   
   if (Batch) return;
   FIX_CHAR_WIDTH;
   if (! *b)
     {
	if(!SLKeyBoard_Quit) return;
	b = quit; 
     }
   if (MiniBuffer)
     {
	len = Mini_Info.prompt_len;
	Mini_Info.prompt_len = 0;
     }

   if ((b == Error_Buffer) || (b == quit)) touch_screen();
   dout = output((unsigned char *) b, strlen(b), 
		 *tt_Screen_Rows, *tt_Screen_Cols, 1, NULL, NULL);
   if (MiniBuffer) Mini_Info.prompt_len = len;
   
   if ((b == Error_Buffer) || (SLKeyBoard_Quit))
     {
	beep();
	flush_input();
     }
   
   if (!dout)
     {
	if (JScreen[row].n)
	  {
	     tt_goto_rc(*tt_Screen_Rows - 1, 0);
	     tt_del_eol();
	  }
	blank_line(row);
	JScreen[row].n = 0;
      }
   else 
     {
	update_screen_txt(row);
	if (MiniBuffer != NULL) 
	  {
	     flush_output ();
	     (void) input_pending(&Number_Ten);
	  }
     }
   
   Mini_Ghost = -dout;
}

static void set_hscroll(int col)
{
   int hdiff, whs = abs(Wants_HScroll), wc = JWindow->column - 1,
      sw = *tt_Screen_Cols - 1;
   static Line *last;
   Line *tmp;
   
   /* take care of last effect of horizontal scroll */
   if (last != NULL)
     {
	tmp = CLine;
	CLine = last;
	register_change(0);
	CLine = tmp;
	if (last != CLine) 
	  {
#if 0
	     /* I need to think about this more */
	     if (Wants_HScroll < 0) 
	       {
		  if (wc != 0)
		    {
		       JWindow->column = 1;
		       wc = 0;
		       touch_window ();
		    }
	       }
#endif
	     HScroll = 0;
	  }
	
	last = NULL;
     }
   
   col--;			       /* use 0 origin */
   hdiff = col - wc;
   if ((HScroll >= hdiff) 
       || (HScroll <= hdiff - sw))
     {
	if (hdiff >= sw)
	  {
	     HScroll = hdiff - sw + whs;
	  }
	else if ((hdiff == 0) && (wc == 0)) HScroll = 0;
	else if (hdiff <= 1)
	  {
	     HScroll = hdiff - whs - 1;
	  }
	else HScroll = 0;
     }
   
   if (HScroll)
     {
	if (wc + HScroll < 0) HScroll = -wc;

	if (Wants_HScroll < 0)
	  {
	     JWindow->column += HScroll;
	     touch_window();
	     HScroll = 0;
	  }
	else 
	  {
	     register_change(0);
	     last = HScroll_Line = CLine;
	  }
     }
}
   
static char Top_Screen_Line_Buffer[132] = "If you see this, you have an installation problem.";

void define_top_screen_line (char *neew)
{
   SLang_push_string (Top_Screen_Line_Buffer);
   strncpy (Top_Screen_Line_Buffer, neew, 130);
   Top_Screen_Line_Buffer[131] = 0;
   JScreen[0].flags = 1;
}

static void update_top_screen_line (void)
{
   register unsigned short *s, *smax, *s1;
   register unsigned char ch, *chp;
   
   if (Top_Window_Row == 1) return;
   
   chp = (unsigned char *) Top_Screen_Line_Buffer;
   
   s = s1 = JScreen[0].neew;
   smax = s + JWindow->width;
   
   while ((s1 < smax) && ((ch = *chp++) != 0))
     {
	*s1++ = (unsigned short) ch;
     }
   while (s1 < smax) *s1++ = ' ';
   *smax = 0;
   (void) fix_attributes (s, smax, JMENU_COLOR);
   
   tt_smart_puts(JScreen[0].neew, JScreen[0].old, *tt_Screen_Cols, 0);
   Point_Cursor_Flag = 1;
   update_screen_txt(0);
   JScreen[0].n = *tt_Screen_Cols;
   JScreen[0].flags = 0;
}

	

/* if flag is non-zero, do not touch the message/error buffers */
void update(Line *line, int force, int flag)
{
   int pc_flag = 1;
   int col;
   static unsigned long last_time;
   Line *hscroll_line_save;

   if (Batch) return;
   
   if (!force && !SLang_Error && !SLKeyBoard_Quit && (!*Error_Buffer))
     {
	if (Input_Buffer_Len || input_pending (&Number_Zero))
	  {
	     JWindow->trashed = 1;
	     return;
	  }
     }
   
   
   if (last_time + 30 < Status_This_Time)
     {
	if (last_time == 0) last_time = Status_This_Time;
	else
	  {
	     last_time = Status_This_Time;
	     (void) SLang_run_hooks ("update_timer_hook", NULL, NULL);
	     flag = 0;
	  }
     }
   

   if (Suspend_Screen_Update != 0) 
     {
	Suspend_Screen_Update = 0;
	touch_screen ();
     }

   if (X_Update_Open_Hook != NULL) (*X_Update_Open_Hook) ();
   FIX_CHAR_WIDTH;
   
   col = calculate_column();
   HScroll_Line = NULL;
   if (Wants_HScroll) set_hscroll(col); else HScroll = 0;
   hscroll_line_save = HScroll_Line;
   
   if (SLang_Error) flag = 0;	       /* update hook invalidates flag */
   
   if (SLang_Error && !(*Error_Buffer || SLKeyBoard_Quit)) SLang_doerror(NULL);
   
   if (!flag && (*Error_Buffer || SLKeyBoard_Quit))
     {
	do_dialog(Error_Buffer);
	SLKeyBoard_Quit = 0;
	SLang_restart(0);
	SLang_Error = 0;
	flag = 0;
	Mini_Ghost = 1;
	(void) update_1(line, 1);
	update_minibuffer();
     }
   else if (!flag && *Message_Buffer)
     {
	if (!update_1(line, force))
	  {
	     /* *Message_Buffer = 0; */
	     goto done;
	  }
	Mini_Ghost = 1;
	do_dialog(Message_Buffer);
	update_minibuffer();
     }
   else
     {
	pc_flag = JWindow->trashed || (JWindow != JWindow->next) || Cursor_Motion;
	if (!flag) update_minibuffer();
	if (!update_1(line, force)) goto done;
     }
   if (!flag) *Error_Buffer = *Message_Buffer = 0;
   
   if ((Top_Window_Row != 1) && JScreen[0].flags)
     {
	update_top_screen_line ();
     }
   
   
   done:

   HScroll_Line = hscroll_line_save;

   if (Point_Cursor_Flag || pc_flag) point_cursor(col);
   if (X_Update_Close_Hook != NULL) (*X_Update_Close_Hook) ();
   flush_output ();
}

/* search for the CLine in the SCreen and flag it as changed */
/* n = 0 means line was changed, n = 1 means it was destroyed */
void register_change(int n)
{
   Window_Type *w;
   register Screen_Type *s, *smax;
   register Line *cl = CLine;

   JWindow->trashed = 1;
   if (Suspend_Screen_Update) return;
   if (No_Screen_Update)
      {
	  No_Screen_Update = 0;
          if (((n == CINSERT) || (n == CDELETE)) && (JWindow->next == JWindow))
	    {
	       /* Since no screen update, we are probably safe to do: */
	       /* JScreen[Screen_Row - 1].flags = 1; */
	       return;
	    }
	  w = JWindow->next;  /* skip this window */
      }
    else w = JWindow;

    do
      {
	 s = &JScreen[w->top - 1];
	 smax = s + w->rows;
	 
	 while (s < smax)
	    {
	       if (s->line == cl)
		 {
		    s->flags = 1;
		    if ((n == NLDELETE) || (n == LDELETE)) s->line = NULL;
		    w->trashed = 1;
		 }
	       s++;
	    }
	  w = w->next;
      }
    while(w != JWindow);
}

void reset_display()
{
   int i;
   unsigned short *p;
   if (Batch) return;
     
   tt_reset_video ();
   tt_del_eol ();
   for (i = 0; i < *tt_Screen_Rows; i++)
     {
	p = JScreen[i].old;  if (p != NULL) SLFREE(p); JScreen[i].old = NULL;
	p = JScreen[i].neew;  if (p != NULL) SLFREE(p); JScreen[i].neew = NULL;
     }
   flush_output ();
}

void init_display(int g)
{
   int i, r, c;
   unsigned short *old, *neew = NULL;

   if (Batch) return;
   if (g) 
     {
	get_term_dimensions(&c, &r);
	if (r > MAX_SCREEN_SIZE) r = MAX_SCREEN_SIZE;
	if (JWindow != NULL)
	  {
	     /* Note that this next call will call this routine again 
	        with g = 0. */
	     change_screen_size (c, r);
	     return;
	  }
	
	*tt_Screen_Cols = c; 
	*tt_Screen_Rows = r;
     }
   
   tt_init_video();
   
   for (i = 0; i < *tt_Screen_Rows; i++)
     {
	/* these are bigger to handle overflow in output routine in case
	 * special char occurs at end as well as allowing code as in blank_line*/
	if ((NULL == (old = (unsigned short *) SLCALLOC(*tt_Screen_Cols + 3, sizeof(short))))
	    || (NULL == (neew = (unsigned short *) SLCALLOC(*tt_Screen_Cols + 3, sizeof(short)))))
	  {
	     exit_error("init_display(): malloc error.", 0);
	  }

	JScreen[i].line = NULL;
	JScreen[i].flags = 1;
	JScreen[i].n = 0;
	JScreen[i].old = old;
	JScreen[i].neew = neew;
	neew = old + *tt_Screen_Cols;
	while (old < neew) *old++ = ' ';
     }
}


void redraw_screen(int force)
{
   int row, center;
   Window_Type *w;
   Line *l;
   
   
   if (Batch) return;
#ifndef msdos
   /* send_string_to_term("\033[?6h"); */  /* relative origin mode */
   tt_reset_scroll_region();
   tt_end_insert();
#endif
   tt_normal_video ();
   tt_cls();
   for (row = 0; row < *tt_Screen_Rows; row++)
     {
	JScreen[row].line = NULL;
	JScreen[row].n = 0;
	JScreen[row].flags = 1;
	blank_line(row);
     }
    w = JWindow;
    center = JWindow->trashed;
    do
      {
	 w->trashed = 1;
	 w = w->next;
      }
    while(w != JWindow);
   
   if (center)
     {
	for (row = 0; row < JWindow->rows; row++)
	  {
	     JScreen[row + JWindow->top - 1].line = NULL;
	  }
	l = NULL;
     }
   else l = JWindow->beg.line;
   update(l, force, 0);
}

int redraw ()
{
   redraw_screen(0);
   return 0;
}

void recenter(int *np)
{
   Line *l = CLine;
   int i, n = *np;
   
   JWindow->trashed = 1;
   if (n == 0) 
     {
	n = JWindow->rows / 2;
	for (i = 0; i < n; i++)
	  {
	     if (l->prev == NULL) break;
	     l = l->prev;
	  }
	JWindow->beg.line = l;
	JWindow->beg.n -= i;
	JWindow->beg.point = 0;
	redraw();
	return;
     }
   
   if (CBuf != JWindow->buffer) return;
   
   if ((n <= 0) || (n > JWindow->rows)) n = JWindow->rows / 2;
   while (--n)
     { 
	l = l->prev;
	if (l == NULL)
	  {
	     l = CBuf->beg;
	     break;
	  }
     }
   
   update(l, 0, 0);
}


int window_line(void)
{
   Line *top = find_top();
   int n = 1;
   
   if (CBuf != JWindow->buffer) return(0);
   while ((top != NULL) && (top != CLine))
     {
	n++;
	top = top->next;
     }
   return(n);
}

   
void touch_window()
{
    int i;

   if (Suspend_Screen_Update) return;
    for (i = 0; i < JWindow->rows; i++)
      {
	 JScreen[i + JWindow->top - 1].flags = 1;
      }

    JWindow->trashed = 1;
}


void touch_screen()
{
   Window_Type *w;
   
   No_Screen_Update = 0;
   if (Suspend_Screen_Update) return;
   w = JWindow;
   do
     {
	touch_window();
	JWindow = JWindow->next;
     }
   while(w != JWindow);
   if (Top_Window_Row != 1) JScreen[0].flags = 1;
}


