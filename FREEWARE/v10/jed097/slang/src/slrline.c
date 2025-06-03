/* SLang_read_line interface --- uses SLang tty stuff */
/* 
 * Copyright (c) 1994 John E. Davis
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <stdio.h>
#include "slang.h"
#include "_slang.h"

#ifndef __GO32__
#ifdef unix
#define real_unix
#endif
#endif

#ifdef real_unix
int SLang_RL_EOF_Char = 4;
#else
int SLang_RL_EOF_Char = 26;
#endif

static int SLang_Rline_Quit;
static SLang_RLine_Info_Type *This_RLI;

static unsigned char Char_Widths[256];
static void position_cursor (int);

static void rl_beep (void)
{
   putc(7, stdout);
   fflush (stdout);
}

/* editing functions */
static int rl_bol (void)
{
   if (This_RLI->point == 0) return 0;
   This_RLI->point = 0;
   return 1;
}

static int rl_eol (void)
{
   if (This_RLI->point == This_RLI->len) return 0;
   This_RLI->point = This_RLI->len;
   return 1;
}

static int rl_right (void)
{
   if (This_RLI->point == This_RLI->len) return 0;
   This_RLI->point++;
   return 1;
}

static int rl_left (void)
{
   if (This_RLI->point == 0) return 0;
   This_RLI->point--; 
   return 1;
}

static int rl_self_insert (void)
{
   unsigned char *pmin, *p;
   
   if (This_RLI->len == This_RLI->buf_len)
     {
	rl_beep ();
	return 0;
     }
   
   pmin = This_RLI->buf + This_RLI->point;
   p = This_RLI->buf + This_RLI->len;
   while (p > pmin) 
     {
	*p = *(p - 1);
	p--;
     }
   *pmin = SLang_Last_Key_Char;
   
   This_RLI->len++;
   This_RLI->point++;
   if ((This_RLI->curs_pos + 2 >= This_RLI->edit_width)
       || (This_RLI->tt_insert == NULL)
       || (Char_Widths[SLang_Last_Key_Char] != 1)) return 1;
   
   (*This_RLI->tt_insert)((char) SLang_Last_Key_Char);
   /* update screen buf */
   p = This_RLI->old_upd + (This_RLI->len - 1);
   pmin = This_RLI->old_upd + (This_RLI->point - 1);
   while (p > pmin) 
     {
	*p = *(p - 1);
	p--;
     }
   *pmin = SLang_Last_Key_Char;
   return 0;
}


static int rl_deln (int n)
{
   unsigned char *pmax, *p;
   
   p = This_RLI->buf + This_RLI->point;
   pmax = This_RLI->buf + This_RLI->len;
   
   if (p + n > pmax) n = (int) (pmax - p);
   while (p < pmax) 
     {
	*p = *(p + n);
	p++;
     }
   This_RLI->len -= n;
   return n;
}

static int rl_del (void)
{
   return rl_deln(1);
}

static int rl_quote_insert (void)
{
   int err = SLang_Error;
   SLang_Error = 0;
   SLang_Last_Key_Char = (*This_RLI->getkey)();
   rl_self_insert ();
   if (SLang_Error == USER_BREAK) SLang_Error = 0;
   else SLang_Error = err;
   return 1;
}

   
static int rl_trim (void)
{
   unsigned char *p, *pmax, *p1;
   p = This_RLI->buf + This_RLI->point;
   pmax = This_RLI->buf + This_RLI->len;
   
   if (p == pmax) 
     {
	if (p == This_RLI->buf) return 0;
	p--;
     }
   
   if ((*p != ' ') && (*p != '\t')) return 0;
   p1 = p;
   while ((p1 < pmax) && ((*p1 == ' ') || (*p1 == '\t'))) p1++;
   pmax = p1;
   p1 = This_RLI->buf;
   
   while ((p >= p1) && ((*p == ' ') || (*p == '\t'))) p--;
   if (p == pmax) return 0;
   p++;
   
   This_RLI->point = (int) (p - p1);
   return rl_deln ((int) (pmax - p));
}

static int rl_bdel (void)
{
   if (rl_left()) return rl_del();
   return 0;
}

static int rl_deleol (void)
{
   if (This_RLI->point == This_RLI->len) return 0;
   *(This_RLI->buf + This_RLI->point) = 0;
   This_RLI->len = This_RLI->point;
   return 1;
}

static int rl_delete_line (void)
{
   This_RLI->point = 0;
   *(This_RLI->buf + This_RLI->point) = 0;
   This_RLI->len = 0;
   return 1;
}

static int rl_enter (void)
{
   *(This_RLI->buf + This_RLI->len) = 0;
   SLang_Rline_Quit = 1;
   return 1;
}

static SLKeyMap_List_Type *RL_Keymap;

/* This update is designed for dumb terminals.  It assumes only that the 
 * terminal can backspace via ^H, and move cursor to start of line via ^M.
 * There is a hook so the user can provide a more sophisicated update if 
 * necessary.
 */

static void position_cursor (int col)
{
   unsigned char *p, *pmax;
   int dc;
   
   if (col == This_RLI->curs_pos) 
     {
	fflush (stdout);
	return;
     }
   
   
   if (This_RLI->tt_goto_column != NULL)
     {
	(*This_RLI->tt_goto_column)(col);
	This_RLI->curs_pos = col;
	fflush(stdout);
	return;
     }
   
   dc = This_RLI->curs_pos - col;
   if (dc < 0)
     {
	p = This_RLI->new_upd + This_RLI->curs_pos;
	pmax = This_RLI->new_upd + col;
	while (p < pmax) putc((char) *p++, stdout);
     }
   else
     {
	if (dc < col)
	  {
	     while (dc--) putc(8, stdout);
	  }
	else
	  {
	     putc('\r', stdout);
	     p = This_RLI->new_upd;
	     pmax = This_RLI->new_upd + col;
	     while (p < pmax) putc((char) *p++, stdout);
	  }
     }
   This_RLI->curs_pos = col;
   fflush (stdout);
}

void erase_eol (SLang_RLine_Info_Type *rli)
{
   unsigned char *p, *pmax;
   
   p = rli->old_upd + rli->curs_pos;
   pmax = rli->old_upd + rli->old_upd_len;
   
   while (p++ < pmax) putc(' ', stdout);
	
   rli->curs_pos = rli->old_upd_len;
}

   
unsigned char *spit_out(SLang_RLine_Info_Type *rli, unsigned char *p)
{
   unsigned char *pmax;
   position_cursor ((int) (p - rli->new_upd));
   pmax = rli->new_upd + rli->new_upd_len;
   while (p < pmax) putc((char) *p++, stdout);
   rli->curs_pos = rli->new_upd_len;
   return pmax;
}

void really_update (SLang_RLine_Info_Type *rli, int new_curs_position)
{
   unsigned char *b = rli->old_upd, *p = rli->new_upd, chb, chp;
   unsigned char *pmax;
   
   if (rli->update_hook != NULL)
     {
	(*rli->update_hook)(p, rli->edit_width, new_curs_position);
     }
   else
     {
	pmax = p + rli->edit_width;
	while (p < pmax)
	  {
	     chb = *b++; chp = *p++;
	     if (chb == chp) continue;
	     
	     if (rli->old_upd_len <= rli->new_upd_len)
	       {
		  /* easy one */
		  (void) spit_out (rli, p - 1);
		  break;
	       }
	     spit_out(rli, p - 1);
	     erase_eol (rli);
	     break;
	  }
	position_cursor (new_curs_position);
     }
   
   /* update finished, so swap */
   
   rli->old_upd_len = rli->new_upd_len;
   p = rli->old_upd;
   rli->old_upd = rli->new_upd;
   rli->new_upd = p;
}

void RLupdate (SLang_RLine_Info_Type *rli)
{
   int len, dlen, start_len = 0, prompt_len = 0, tw = 0, count;
   int want_cursor_pos;
   unsigned char *b, chb, *b_point, *p;
   
   b_point = (unsigned char *) (rli->buf + rli->point);
   *(rli->buf + rli->len) = 0;
   
   /* expand characters for output buffer --- handle prompt first.  
    * Do two passes --- first to find out where to begin upon horiz
    * scroll and the second to actually fill the buffer. */
   len = 0;
   count = 2;			       /* once for prompt and once for buf */
   
   b = (unsigned char *) rli->prompt;
   while (count--)
     {
	while ((chb = *b) != 0)
	  {
	     /* This will ensure that the screen is scrolled a third of the edit
	      * width each time */
	     if (b_point == b) break;
	     dlen = Char_Widths[chb];
	     if ((chb == '\t') && tw)
	       {
		  dlen = tw * ((len - prompt_len) / tw + 1) - (len - prompt_len);
	       }
	     len += dlen;
	     b++;
	  }
	tw = rli->tab;
	b = (unsigned char *) rli->buf;
	if (count == 1) want_cursor_pos = prompt_len = len;
     }
   
   if (len < rli->edit_width - rli->dhscroll) start_len = 0;
   else if ((rli->start_column > len)
	    || (rli->start_column + rli->edit_width <= len))
     {
	start_len = len - (rli->edit_width - rli->dhscroll);
	if (start_len < 0) start_len = 0;
     }
   else start_len = rli->start_column;
   rli->start_column = start_len;
   
   want_cursor_pos = len - start_len;

   
   /* second pass */
   p = rli->new_upd;
	
   len = 0;
   count = 2;
   b = (unsigned char *) rli->prompt;
   while ((len < start_len) && (*b))
     {
	len += Char_Widths[*b++];
     }
   
   tw = 0;
   if (*b == 0)
     {
	b = (unsigned char *) rli->buf;
	while (len < start_len)
	  {
	     len += Char_Widths[*b++];
	  }
	tw = rli->tab;
	count--;
     }
      
   len = 0;
   while (count--)
     {
	while ((len < rli->edit_width) && ((chb = *b++) != 0))
	  {
	     dlen = Char_Widths[chb];
	     if (dlen == 1) *p++ = chb;
	     else 
	       {
		  if ((chb == '\t') && tw)
		    {
		       dlen = tw * ((len + start_len - prompt_len) / tw + 1) - (len + start_len - prompt_len);
		       len += dlen;	       /* ok since dlen comes out 0  */
		       if (len > rli->edit_width) dlen = len - rli->edit_width;
		       while (dlen--) *p++ = ' ';
		       dlen = 0;
		    }
		  else 
		    {
		       if (dlen == 3)
			 {
			    chb &= 0x7F;
			    *p++ = '~';
			 }
		  
		       *p++ = '^';
		       if (chb == 127)  *p++ = '?';
		       else *p++ = chb + '@';
		    }
	       }
	     len += dlen;
	  }
	/* if (start_len > prompt_len) break; */
	tw = rli->tab;
	b = (unsigned char *) rli->buf;
     }
   
   rli->new_upd_len = (int) (p - rli->new_upd);
   while (p < rli->new_upd + rli->edit_width) *p++ = ' ';
   really_update (rli, want_cursor_pos);
}
	
int SLang_read_line (SLang_RLine_Info_Type *rli)
{
   unsigned char *p, *pmax;
   SLang_Key_Type *key;
   
   SLang_Rline_Quit = 0;
   This_RLI = rli;
   p = rli->old_upd; pmax = p + rli->edit_width;
   while (p < pmax) *p++ = ' ';
   
   /* Sanity checking */
   rli->len = strlen ((char *) rli->buf);
   if (rli->len >= rli->buf_len) 
     {
	rli->len = 0;
	*rli->buf = 0;
     }
   if (rli->point > rli->len) rli->point = rli->len;
   if (rli->point < 0) rli->point = 0;
   
   rli->curs_pos = rli->start_column = 0;
   rli->new_upd_len = rli->old_upd_len = 0;
   
   This_RLI->last_fun = NULL;
   RLupdate (rli);
   
   while (1)
     {
	key = SLang_do_key (RL_Keymap, (int (*)(void)) rli->getkey);
	
	if ((key == NULL) || (key->f == NULL))
	  {
	     rl_beep ();
	  }
	else if (key->type == SLKEY_F_INTRINSIC) 
	  {
	     if (((int (*)(void))(key->f))())
	       RLupdate (rli);
	  }
	
	if (SLang_Rline_Quit) 
	  {
	     This_RLI->buf[This_RLI->len] = 0;
	     if (SLang_Error == USER_BREAK) 
	       {
		  SLang_Error = 0;
		  return -1;
	       }
	     return This_RLI->len;
	  }
	if (key != NULL) This_RLI->last_fun = key->f;
     }
}

static int rl_abort (void)
{
   rl_delete_line ();
   return rl_enter ();
}

static int rl_eof_insert (void)
{
   if (This_RLI->len == 0)
     {
	SLang_Last_Key_Char = SLang_RL_EOF_Char;
	rl_self_insert ();
	return rl_enter ();
     }
   return 0;
}


/* TTY interface --- ANSI */

static void ansi_goto_column (int n)
{
   putc('\r', stdout);
   if (n) fprintf(stdout, "\033[%dC", n);
}

static void rl_select_line (SLang_Read_Line_Type *p)
{
   This_RLI->last = p;
   strcpy ((char *) This_RLI->buf, (char *) p->buf);
   This_RLI->point = This_RLI->len = strlen((char *) p->buf);
}
static int rl_next_line (void);
static int rl_prev_line (void)
{
   SLang_Read_Line_Type *prev;
   
   if (((This_RLI->last_fun != (VOID *) rl_prev_line)
	&& (This_RLI->last_fun != (VOID *) rl_next_line))
       || (This_RLI->last == NULL))
     {
	prev = This_RLI->tail;
     }
   else prev = This_RLI->last->prev;
   
   if (prev == NULL)
     {
	rl_beep ();
	return 0;
     }
   
   rl_select_line (prev);
   return 1;
}

static int rl_next_line (void)
{
   SLang_Read_Line_Type *next;
   
   if (((This_RLI->last_fun != (VOID *) rl_prev_line)
	&& (This_RLI->last_fun != (VOID *) rl_next_line))
       || (This_RLI->last == NULL))
      {
	 rl_beep ();
	 return 0;
      }
   
   next = This_RLI->last->next;
   
   if (next == NULL)
     {
	This_RLI->len = This_RLI->point = 0;
	*This_RLI->buf = 0;
	This_RLI->last = NULL;
     }
   else rl_select_line (next);
   return 1;
}



static SLKeymap_Function_Type SLReadLine_Functions[] = 
{
   {"up", rl_prev_line},
   {"down", rl_next_line},
   {"bol", rl_bol},
   {"eol", rl_eol},
   {"right", rl_right},
   {"left", rl_left},
   {"self_insert", rl_self_insert},
   {"bdel", rl_bdel},
   {"del", rl_del},
   {"deleol", rl_deleol},
   {"enter", rl_enter},
   {"trim", rl_trim},
   {"quoted_insert", rl_quote_insert},
   {(char *) NULL, NULL}
};

int SLang_init_readline (SLang_RLine_Info_Type *rli)
{
   int ch;
   char simple[2];
   
   if (RL_Keymap == NULL)
     {
	simple[1] = 0;
	if (NULL == (RL_Keymap = SLang_create_keymap ("ReadLine", NULL)))
	  return -1;
	
	RL_Keymap->functions = SLReadLine_Functions;
	
	for (ch = ' '; ch < 256; ch++)
	  {
	     simple[0] = (char) ch;
	     SLang_define_key1(simple, (VOID *) rl_self_insert, SLKEY_F_INTRINSIC, RL_Keymap);
	  }
	
	simple[0] = SLang_Abort_Char;
	SLang_define_key1(simple, (VOID *) rl_abort, SLKEY_F_INTRINSIC, RL_Keymap);
	simple[0] = SLang_RL_EOF_Char;
	SLang_define_key1(simple, (VOID *) rl_eof_insert, SLKEY_F_INTRINSIC, RL_Keymap);
#ifndef pc_system	
	SLang_define_key1 ("^[[A", (VOID *) rl_prev_line, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^[[B", (VOID *) rl_next_line, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^[[C", (VOID *) rl_right, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^[[D", (VOID *) rl_left, SLKEY_F_INTRINSIC, RL_Keymap);
#else
	SLang_define_key1 ("^@H", (VOID *) rl_prev_line, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^@P", (VOID *) rl_next_line, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^@M", (VOID *) rl_right, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^@K", (VOID *) rl_left, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^@S", (VOID *) rl_del, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^@O", (VOID *) rl_eol, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^@G", (VOID *) rl_bol, SLKEY_F_INTRINSIC, RL_Keymap);
#endif
	SLang_define_key1 ("^E", (VOID *) rl_eol, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^I", (VOID *) rl_self_insert, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^A", (VOID *) rl_bol, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^M", (VOID *) rl_enter, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^L", (VOID *) rl_deleol, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^V", (VOID *) rl_del, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^D", (VOID *) rl_del, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^F", (VOID *) rl_right, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^B", (VOID *) rl_left, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^?", (VOID *) rl_bdel, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^H", (VOID *) rl_bdel, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^P", (VOID *) rl_prev_line, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("^N", (VOID *) rl_next_line, SLKEY_F_INTRINSIC, RL_Keymap);
	
	SLang_define_key1 ("`", (VOID *) rl_quote_insert, SLKEY_F_INTRINSIC, RL_Keymap);
	SLang_define_key1 ("\033\\", (VOID *) rl_trim, SLKEY_F_INTRINSIC, RL_Keymap);
	
	if (SLang_Error) return -1;
     }
   
   if (rli->keymap == NULL) rli->keymap = RL_Keymap;
   rli->old_upd = rli->upd_buf1;
   rli->new_upd = rli->upd_buf2;
   *rli->buf = 0; 
   rli->point = 0;
   
   if (rli->flags & SLRL_USE_ANSI)
     {
	if (rli->tt_goto_column == NULL) rli->tt_goto_column = ansi_goto_column;
     }
   
   if (Char_Widths[0] == 2) return 0;
	
   for (ch = 0; ch < 32; ch++) Char_Widths[ch] = 2;
   for (ch = 32; ch < 256; ch++) Char_Widths[ch] = 1;
   Char_Widths[127] = 2;
#ifndef pc_system
   for (ch = 128; ch < 160; ch++) Char_Widths[ch] = 3;
#endif
   
   return 0;
}

SLang_Read_Line_Type *SLang_rline_save_line (SLang_RLine_Info_Type *rli)
{
   SLang_Read_Line_Type *rl = NULL;
   unsigned char *buf;
   
   if (NULL == (rl = (SLang_Read_Line_Type *) SLMALLOC (sizeof (SLang_Read_Line_Type)))
       || (NULL == (buf = (unsigned char *) SLMALLOC (rli->buf_len))))
     {
	if (rl != NULL) SLFREE (rl);
	return NULL;
     }
   rl->buf = buf;
   rl->buf_len = rli->buf_len;
   rl->num = rl->misc = 0;
   rl->next = rl->prev = NULL;
   
   strcpy ((char *) rl->buf, (char *) rli->buf);
   
   if (rli->tail != NULL) 
     {
	rli->tail->next = rl;
	rl->prev = rli->tail;
     }
   rli->tail = rl;
   
   return rl;
}
