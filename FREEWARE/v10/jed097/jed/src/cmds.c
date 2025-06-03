/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "slang.h"
#include "buffer.h"
#include "screen.h"
#include "window.h"
#include "ins.h"
#include "ledit.h"
#include "cmds.h"
#include "line.h"
#include "paste.h"
#include "display.h"
#include "vterm.h"
#include "sysdep.h"
#include "text.h"
#include "file.h"
#include "misc.h"
#include "search.h"
#include "hooks.h"

void (*X_Suspend_Hook)(void);

int Blink_Flag = 1;
int Indented_Text_Mode = 0;	       /* if non zero, intent line after wrap */
int Kill_Line_Feature = 1;	       /* non-zero means kill through eol if bolp  */
int C_Bra_Newline = 1;		       /* if zero, do not insert newline before '{' */

int C_Comment_Hint;


static char *Top_Of_Buffer_Error = "Top Of Buffer.";
static char *End_Of_Buffer_Error = "End Of Buffer.";

/* return 1 if line extends beyond the screen */
int long_line(void)
{
    int p, col;

    col = Screen_Col;
    p = Point;
    Point = CLine->len - 1;
    if ((*(CLine->data + Point) != '\n') || (CBuf == MiniBuffer)) Point++;
    if (calculate_column() >= JWindow->width - 1)
      {
	  Point = p;
	  Screen_Col = col;
	  return(1);
      }
    Screen_Col = col;
    Point = p;
    return(0);
}

#define FAST_NOT_OK ((Repeat_Factor != NULL) || JWindow->trashed\
        || (ch < ' ') || (ch > 126) || Input_Buffer_Len\
	|| Suspend_Screen_Update\
        || (Screen_Col >= JWindow->width - 2) || (JWindow->column > 1)\
	|| Batch || (CLine->len == 0)\
	|| ((CBuf->marks != NULL) && Wants_Attributes)\
	|| input_pending(&Number_Zero) || Executing_Keyboard_Macro\
        || tabs_present()\
	|| (Read_This_Character != NULL))

/* This routine moves CLine and point to the matching '{' or whatever.  It
 *  returns 1 if it lies outside the screen, 0 if inside and -1 if not found
 *  and -2 if it looked back for more than count lines and not found.  It
 *  can get messed up if there are delimeters in comments unless they are
 *  properly enclosed like the above---- Later I will add a find matching
 *  forward function, then I will search back for the beginning comment
 *  and go from there.
 */
/* count is the number of lines to go back and shift is distance back to
   start searching from. (usually 0, 1, or 2) */
int find_matching(char ch, int count, int shift)
{
   int slash, ret = 0;
   int lang = ! (CBuf->modes & WRAP_MODE), inc = 0, lang_and_not_inc;
   register int n = 1, in;
   register unsigned char *p, starp, *cline_data;
   unsigned char *prev, *p1;
   char ch1, save;
   char quote = '\\';
   char cb1 = '/', cb2 = '*', ce1 = '*', ce2  = '/';
   
   

    ch1 = '(';
    if (ch == '}') ch1 = '{'; else if (ch == ']') ch1 = '[';

    p = CLine->data + Point - shift;
    if (p < CLine->data)
      {
          if (CLine->prev == NULL) return(-1);
          if (CLine == JWindow->beg.line) ret = 1;
          CLine = CLine->prev; LineNum--;
	  p = CLine->data + (CLine->len - 1);
       }
     in = 0;
   /* if (Point > 1) prev = p - 1; else prev = p; */
   prev = p;
   lang_and_not_inc = lang;
   
   cline_data = CLine->data;

    while(count)
      {
	 p1  = p - 1;
	 starp = *p;
	 if ((starp <= '\'') && (starp >= '"')
	     && ((starp == '\'') || (starp == '"')) && lang_and_not_inc)
	   {
	      slash = 0;
	      save = starp;
	      while((p > cline_data) && (*(p - 1) == (unsigned char) quote))
		{
		   p--;
		   slash = !slash;
		}

	      if (!slash)
		{
		   if (in == 0) in = save; else if (in == save) in = 0;
		}
	   }
	 
	      
          if (!in)
            {
	       /* This is specific to C.  I need to generalize to other languages. */
	       if ((ce2 == (char) *p) && (p > cline_data)
		   && (*p1 == (unsigned char) ce1) && lang)
		 {
		    p = p1;
		    inc = 1;
		    lang_and_not_inc = 0;
		 }
	       else if ((cb2 == (char) *p) && (p > cline_data)
		   && (*p1 == (unsigned char) cb1) && lang)
		 {
		    if (inc == 0) 
		      {
			 Point = (int) (p1 - cline_data);
			 return (-2);
		      }
		    p = p1;
		    inc = 0;
		    lang_and_not_inc = lang;
		 }
	       starp = *p;
	       if (!inc && ((starp == '[') || (starp == '{') || (starp == '('))) n--;
	       else if (!inc && ((starp == '}') || (starp == ']') || (starp == ')'))) n++;
            }

	 if (!n) break;
	 if (p == cline_data)
	   {
	      if (CLine == JWindow->beg.line) ret = 1;
	      if (CLine->prev == NULL) break;
	      CLine = CLine->prev;
	      count--; LineNum--;
	      p = CLine->data + (CLine->len - 1);
	      cline_data = CLine->data;
	   }
          else p--;
      }
    Point = (int) (p - CLine->data);
    if ((n == 0) && (*p == (unsigned char) ch1)) return(ret);
    if (lang) if ((*prev == '\'') || (*prev == '"')) return(-2);
    return(-1);
}

/* blink the matching fence.  This assumes that the window is ok */
void blink_match(char ch)
{
   Line *save;
   int pnt, code, matchp;
   unsigned int l;
   char buf[600], strbuf[256];

   if (!Blink_Flag || (Repeat_Factor != NULL) || Batch) return;
   if (JWindow->trashed) update((Line *) NULL, 0, 0);
   if (JWindow->trashed) return;
   pnt = Point;
   save = CLine;
   l = LineNum;
   code = find_matching(ch, 500, 2);
   if (code == -1)
     {
	if ((! (CBuf->modes == WRAP_MODE)) && (!IS_MINIBUFFER)) message("Mismatch??");
     }
   else if (code == 0)
     {
	/* update((Line *) NULL, 0, 0); */
	point_cursor(0);
	input_pending(&Number_Ten);   /* sleep((unsigned) 1); */
	Point = pnt;
	CLine = save;
	LineNum = l;
	point_cursor(0);
	return;
     }
   else if (code == 1)
     {
	matchp = Point;
	Point = 0;
	strcpy(buf, "Matches ");
	skip_whitespace();
	if ((matchp == Point) && prevline(&Number_One))
	  {
	     Point = 0;
	     strcat(buf, make_line_string(strbuf));
	     nextline(&Number_One);
	     Point = 0;
	  }
	strcat(buf, make_line_string(strbuf));
	message(buf);
     }
   Point = pnt;
   CLine = save;
   LineNum = l;
}

int goto_match()
{
    char *p;
    Line *save;
    int pnt, code;
   unsigned int l;
   
    p = (char *) (CLine->data + Point);
    if ((*p != ')') && (*p != '}') && (*p != ']')) return(0);
    save = CLine;
    pnt = Point;
   l = LineNum;
    code = find_matching(*p, -1, 1);  /* -1 since we want to shif back 1 */
    if (code == -1)
      {
	 if (!IS_MINIBUFFER) msg_error("Mismatch!!");
	 CLine = save;
	 Point = pnt;
	 LineNum = l;
	 return(0);
      }
    return(1);
}


int newline()
{
   int push = 0;
   CHECK_READ_ONLY
   
   if (CBuf == MiniBuffer) return exit_minibuffer();
   
   /* This bit of nonsense gives a better looking screen update */
   if (bolp() && (CLine->prev != NULL) && (CLine->len > 1))
      {
          push = 1;
          push_spot();
          CLine = CLine->prev;
	 LineNum--;
	  Point = 0;
          eol();
      } 

    split_line();
    ins('\n');
   check_line();
    if (push)
      {
          pop_spot();
          return(1);
      }

    Point--;
    forwchars(&Number_One);
    return(1);
}

int previous_char_cmd()
{
   int pnt;
   Cursor_Motion = 1;
   if (bobp())
     {
	msg_error(Top_Of_Buffer_Error);
	return(0);
     }

   pnt = Point - 1;
   backwchars(&Number_One);
   Goal_Column = calculate_column();
      /* For syntax highlighting */
   if ((Last_Key_Function != (VOID *) previous_char_cmd) &&
       ((Last_Key_Function == (VOID *) ins_char_cmd)
       || (Last_Key_Function == (VOID *) delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_untabify)))
     register_change(0);

   return (pnt == -1) || JWindow->trashed;
}

/* Slang calls by reference so make this a pointer ! */
void insert_whitespace(int *n)
{
   int tab = Buffer_Local.tab;
   int c1, c2, i, k, nspace;

   if ((nspace = *n) <= 0) return;
   CHECK_READ_ONLY_VOID
   c1 = calculate_column() - 1;
   c2 = c1 + nspace;

   if (tab)
     {
	i = c1 / tab;
	k = c2 / tab - i;
	if (k) nspace = c2 - (i + k) * tab;
	ins_char_n_times('\t', k);
     }
   ins_char_n_times(' ', nspace);
}

/* check from point to end of line looking for tabs */
int tabs_present(void)
{
    unsigned char *p, *pmax;

    if (!Buffer_Local.tab) return(0);
    pmax = CLine->data + CLine->len;
    p = CLine->data + Point;

    while (p < pmax) if (*p++ == '\t') return(1);
    return(0);
}

int delete_char_cmd()
{
   int upd, n;
   char ch;

   CHECK_READ_ONLY
   if (eobp())
      {
          msg_error(End_Of_Buffer_Error);
          return(0);
      }

    ch = *(CLine->data + Point);

    if (FAST_NOT_OK || long_line ()
	|| *tt_Term_Cannot_Insert
	)
      {
	 
	 if ((Point == 0) && eolp()) n = 1; else n = 0;
	 /* same effect, update looks better */	 
	 n = backwchars(&n);
	 del();
	 forwchars(&n);
	 upd = 1;
      }
    else
      {
	 upd = ((CBuf->flags & BUFFER_TRASHED) == 0);
	 fast_del();
	 tt_delete_char ();
      }

    return(upd);
}

int backward_delete_char_cmd()
{
   char ch;
   int ret;

   CHECK_READ_ONLY
    if (bobp())
      {
          msg_error(Top_Of_Buffer_Error);
          return(0);
      }

    if (bolp())
      {
	 backwchars(&Number_One);
	 return delete_char_cmd();
      }

    ch = *(CLine->data + (Point - 1));

    if (FAST_NOT_OK || long_line ()
	|| *tt_Term_Cannot_Insert
	)
      {
	  backwchars(&Number_One);
	  del();
	  return(1);
      }

   ret = ((CBuf->flags & BUFFER_TRASHED) == 0);
   backwchars(&Number_One);
   tt_putchar('\b');
    Screen_Col--;
   fast_del();
    tt_delete_char();
    return(ret);
}

int backward_delete_char_untabify()
{
    unsigned char *p;
    int n;

   CHECK_READ_ONLY
    p = CLine->data + (Point - 1);

    /* note that short circuit is assumed to avoid seg fault */
    if (!Point || bobp() || (*p != '\t') || !Buffer_Local.tab) return backward_delete_char_cmd();

    n = calculate_column() - 1;
    backwchars(&Number_One);
    del();
    n = n - calculate_column();
    ins_char_n_times(' ', n);

    return(1);
}

int previous_line_cmd()
{
   int ret, gc;

   if (CLine == CBuf->beg)
     {
          msg_error(Top_Of_Buffer_Error);
          return(0);
     }
   check_line();
    gc = calculate_column();
    if (Cursor_Motion <= 0) Goal_Column = gc;
    else if (Goal_Column < gc) Goal_Column = gc;

   Cursor_Motion = 2;
   
      /* For syntax highlighting */
   if ((Last_Key_Function != (VOID *) previous_line_cmd) &&
       ((Last_Key_Function == (VOID *) ins_char_cmd)
       || (Last_Key_Function == (VOID *) delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_untabify)))
     register_change(0);   
   
   ret = JWindow->trashed || (CLine == JScreen[JWindow->top - 1].line);
   
   CLine = CLine->prev;
   LineNum--;
   point_column(Goal_Column);
   return(ret);
}

int next_line_cmd()
{
   int ret, gc;

   if (CLine == CBuf->end)
     {
	msg_error(End_Of_Buffer_Error);
	return(0);
     }
   check_line();
   gc = calculate_column();
   if (Cursor_Motion <= 0) Goal_Column = gc;
   else if (Goal_Column < gc) Goal_Column = gc;

   Cursor_Motion = 2;
   
   /* For syntax highlighting */
   if ((Last_Key_Function != (VOID *) next_line_cmd) &&
       ((Last_Key_Function == (VOID *) ins_char_cmd)
       || (Last_Key_Function == (VOID *) delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_untabify)))
     register_change(0);

   ret = JWindow->trashed || (CLine == JScreen[JWindow->top + JWindow->rows - 2].line);
   
   CLine = CLine->next;
   LineNum++;
   point_column(Goal_Column);
   return(ret);
}

int next_char_cmd()
{
   Cursor_Motion = 1;
   
   /* For syntax highlighting */
   if ((Last_Key_Function != (VOID *) next_char_cmd) &&
       ((Last_Key_Function == (VOID *) ins_char_cmd)
       || (Last_Key_Function == (VOID *) delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_cmd)
       || (Last_Key_Function == (VOID *) backward_delete_char_untabify)))
     register_change(0);
       
   if (!forwchars(&Number_One))
     {
	msg_error(End_Of_Buffer_Error);
	return (0);
     }
   Goal_Column = calculate_column();
   return !Point || JWindow->trashed;  /* Point = 0 ==> moved a line */
}

extern Buffer *Paste_Buffer;

int kill_line()
{
   int n, pnt, flag = 0;
   
   CHECK_READ_ONLY

    if (eobp())
      {
          msg_error(End_Of_Buffer_Error);
          return(0);
      }
   
   push_mark();
   push_spot();
   pnt = Point;
   eol();
   n = Point - pnt;
   if ((!pnt && Kill_Line_Feature) || !n)
     {

	/* Either of these (flag =0,1) have the same effect on the buffer.
	 *  However, the first sets the mark at the end of the line and moves
	 *  the point to the end of the previous line.  This way the current
	 *  line structure is deleted and the screen update looks better. 
	 */
	if (!pnt && (CLine->prev != NULL) && (CLine->next != NULL)) 
	  {
	     flag = 1;
	     forwchars(&Number_One);
	  }
	else n += forwchars(&Number_One);
	
     }
	
   if ((Last_Key_Function == (VOID *) kill_line) && (Paste_Buffer != NULL))
     {
	copy_region_to_buffer(Paste_Buffer);
     }
   else copy_to_pastebuffer();
   pop_spot();
   if (flag) n += backwchars(&Number_One);
   generic_deln(&n);
   if (flag) forwchars(&Number_One);
   return(1);
}

   
/* get indent value of current line, n is the column */
unsigned char *get_current_indent(int *n)
{
   unsigned char *p, *pmax;
   int tab;

   tab = Buffer_Local.tab;
   p = CLine->data;
   pmax = CLine->data + CLine->len;
   *n = 0;
   while((p < pmax) && ((*p == ' ') || (tab && (*p == '\t'))))
     {
	if (*p == '\t')
	  *n = tab * (*n / tab + 1);
	else (*n)++;

	p++;
     }
   return(p);
}

/* returns desired indentation as well as a char that may be used for 
   subsequent indentation */
unsigned char *get_indent(int *pt)
{
   int n, ret;
   
   unsigned char *p;

   p = get_current_indent(&n);

   if (CBuf->modes & C_MODE)
     {
	C_Comment_Hint = 0;
	push_spot();
	eol();
	if (find_matching(')', 50, 0) >= 0)   /* goback 50 lines */
	  {
	     n = calculate_column() - User_Vars.c_indent;
	     p = CLine->data + Point;
	  }
	else
	  {
	     pop_spot(); push_spot (); eol ();
	     ret = find_matching('}', 500, 0);
	     if (ret >= 0)
	       {
		  p = get_current_indent(&n);
		  push_spot ();
		  if ((*p != '{') && Point)
		    {
		       /* handle this sort of thing:
			* if (line1
			*     || line_2) {
			*   then_this;
		        * }
			*/
		       p = CLine->data + (Point - 1);

		       while ((*p <= ' ') && (p > CLine->data)) p--;
		       if (*p == ')')
			 {
			    Point = (int) (p - CLine->data);
			    find_matching(')', -1, 1); /* -1 since we want to shif back 1 */
			    if (*(CLine->data + Point) ==  '(')
			      (void) get_current_indent (&n);
			 }
		    }
		  pop_spot ();
		  p = CLine->data + Point;
	       }
	     else if ((ret == -2) && looking_at("/*"))
	       {
		  n = calculate_column() - User_Vars.c_indent;
		  p = CLine->data + Point;
		  C_Comment_Hint = 1;
	       }
	     else
	     n = 0;		       /* give up */
#if 0
	     /* Not found and not in comment */
	       {
		  /* try this */
		  pop_spot (); push_spot (); eol ();
		  if (search_backward(")"))
		    {
		       find_matching(')', -1, 1);  /* -1 since we want to shif back 1 */
		    }
		  p = get_current_indent(&n);
		  /* n -= User_Vars.c_indent; */
	       }
#endif
	  }
	pop_spot ();
     }
   if (n < 0) n = 0;
   *pt = n;
   return(p);
}

int trim_whitespace()
{
    unsigned char *p;
    int n, save_point, len;

   CHECK_READ_ONLY
    len = CLine->len;
    if (len == 0) return(0);

    save_point = Point;
    if (Point == len) Point--;

    /* Note that we save the point before --ing it.  This way the comparison
       made below will effectively restore it if pointing at non whitespace */

    p = CLine->data + Point;
    if (!bolp() && (*p == '\n') && (CBuf != MiniBuffer))
      {
          Point--;
          p--;
      }

    while ((Point > 0) && ((*p == '\t') || (*p == ' ')))
      {
          Point--;
          p--;
      }

    if (save_point != Point) if (!eolp() && ((*p != ' ') && (*p != '\t')))
      {
          Point++;
          p++;
      }

    n = 0;
    /* this needs to be inlined.  Actually for undo, I need del(n)! */
   while (((*p == ' ') || (*p == '\t')) && !eolp()) p++, n++;
   deln(&n);
   return(1);
}

/* indent line to column n */
void indent_to(int n)
{
    int m;

    get_current_indent(&m);

    if (n != m)
      {
	 Point = 0;
	 trim_whitespace();
	 if (n >= 0) insert_whitespace(&n);
      }
}

int indent_line()
{
    char ch, ch1;
    int n, n1;

   CHECK_READ_ONLY
    if (CLine == CBuf->beg) return(0);

    push_spot();
    CLine = CLine->prev;
    ch = (char) *get_indent(&n);
    CLine = CLine->next;

    if (CBuf->modes & C_MODE)
      {
	 ch1 = (char) *get_current_indent(&n1);

	 if (n || (ch == '{') || (C_Bra_Newline == 0))
	   {
	      if (ch1 == '{') n += User_Vars.c_brace;
	      if ((ch1 != '}') && ((n != 0) || (ch == '{'))) n += User_Vars.c_indent;
	   }

	 /* else if (ch == '}') n -= User_Vars.c_brace; */

	 /* if (n && (ch1 == '{')) n += User_Vars.c_brace;
	 else if (ch1 == '}') n -= User_Vars.c_indent;*/

	 if (ch1 == '#') n = 0;
      }

    indent_to(n);

    pop_spot();

    /* This moves the cursor to the first non whitspace char if we are
       before it.  Otherwise leave it alone */

    n1 = calculate_column();
    get_current_indent(&n);
    if (n1 <= n) point_column(n + 1);
   
   if (CBuf->indent_hook != NULL)  SLexecute_function(CBuf->indent_hook);

    return(1);
}

int newline_and_indent()
{
    newline();

    indent_line();
    return(1);
}

int ins_char_cmd()
{
   char ch;
   
   int wrap = User_Vars.wrap_column;
   int ret, ll;

   CHECK_READ_ONLY
    ch = (char) SLang_Last_Key_Char;

    if (ch == '\n')
      {
          newline();
          return(1);
      }
   
#ifdef HAS_ABBREVS   
   if (CBuf->flags & ABBREV_MODE) 
     {
	expand_abbrev (ch);
     }
#endif
   
   if ((CBuf->flags & OVERWRITE_MODE) && !eolp()) del();
   
    if (((ch == ' ') || (Point >= wrap)) && (CBuf->modes & WRAP_MODE)
	&& (calculate_column() > wrap)) /* JWindow->width */
      {
	 ins(ch);
	 wrap_line(0);			/* do not format--- just wrap */
	 if (Indented_Text_Mode) indent_line();
	 if (CBuf->wrap_hook != NULL)
	   SLexecute_function(CBuf->wrap_hook);

	 return(1);
      }

    if ((ch == ' ') || FAST_NOT_OK  || (CBuf->flags & OVERWRITE_MODE) 
	|| (*tt_Term_Cannot_Insert && !eolp())
	)
      {
	 ins(ch);
	 if (((ch == '}') || (ch == ')') || (ch == ']'))
	     && !input_pending(&Number_Zero)) blink_match(ch);
	 return(1);
      }


   ret = ((CBuf->flags & BUFFER_TRASHED) == 0) || (User_Prefers_Line_Numbers > 1);
   
   if (long_line ()) ll = 1; else ll = 0;
   ret = ret || ll;

   /* fast_ins goes first so that screen structure is updated now
      for following calls to use. */	

   fast_ins(ch);
   if (!eolp() || ll)
     {
	tt_begin_insert();
	tt_putchar(ch);
	tt_end_insert();
	if (ll) register_change(0);
     }
   else tt_putchar(ch);
   Screen_Col++;
   if ((ch == ')') || (ch == '}') || (ch == ']')) blink_match(ch);
   return(ret);
}

int brace_bra_cmd()
{
   int n;
   unsigned char *p;
   CHECK_READ_ONLY
   
   p = CLine->data + (Point - 1);
   
   if ((!(CBuf->modes & C_MODE)) || (!eolp())
         || (Point && ((*p == '\'') || (*p == '"'))))
      {
         return ins_char_cmd();
      }

   /* some people prefer insert,newline.  I prefer newline, insert, newline. */

   p = get_current_indent(&n);
   if (C_Bra_Newline && 
       ((p < CLine->data + CLine->len) && (*p > (unsigned char) ' '))) newline();
   else ins(' ');
   indent_line();
   ins('{');
   indent_line();
   newline_and_indent();
   return(1);
}

int brace_ket_cmd()
{
   int n;
   int pnt = Point;
   unsigned char *prev, *p;

   CHECK_READ_ONLY
   if ((CBuf->modes & C_MODE) == 0) return ins_char_cmd();
   
   prev = CLine->data + (Point - 1);
   skip_whitespace();
   if (!eolp() || (pnt && ((*prev == '"') || (*prev == '\''))))
     {
	Point = pnt;
	return ins_char_cmd();
     }

   p = get_current_indent(&n);
   if ((p < CLine->data + CLine->len) && (*p > (unsigned char) ' ')) newline();
   ins('}');
   indent_line();
   JWindow->trashed = 1;
   update((Line *) NULL, 0, 0);
   blink_match('}');
   if (C_Bra_Newline) newline_and_indent();
   return(1);
}

int eol_cmd()
{
    if (!eolp())
      eol();
    if (! (CBuf->flags & READ_ONLY)) trim_whitespace();
    return(1);
}


int sys_spawn_cmd()
{
   if (Batch) return (0);
   if (X_Suspend_Hook != NULL)
     {
	(*X_Suspend_Hook) ();
	return (0);
     }

   SLang_run_hooks("suspend_hook", NULL, NULL);
   if (SLang_Error) return(0);
   reset_display();
   reset_tty();
   sys_suspend();
   init_tty();
   init_display(1);
   /* cls(); */
   SLang_run_hooks("resume_hook", NULL, NULL);
   check_buffers();
   return(1);
}

int quit_jed(void)
{
   Buffer *b = CBuf;
   
   /* Any buffer marked with AUTO_SAVE_JUST_SAVE flag should be saved 
    * if it has not already been.  That is what the flag is for and this
    * code fragment carries this out.
    */
   do 
     {
	if ((b->flags & AUTO_SAVE_JUST_SAVE) 
	    && (b->flags & BUFFER_TRASHED)
	    && (*b->file))
	  {
	     while (b->narrow != NULL) widen_buffer(b);
	     auto_save_buffer(b);      /* actually, it will save it */
	  }
	b = b->next;
     }
   while (b != CBuf);

   reset_display();
   reset_tty();
#ifdef VMS
   vms_cancel_exithandler();
#endif
#ifdef SLANG_STATS
   SLang_dump_stats("slang.dat");
#endif
#ifdef MALLOC_DEBUG
   SLmalloc_dump_statistics ();
#endif
   exit(0);
   return(1);
}


int quoted_insert()
{
   char ch;
   int lerr = SLang_Error;
   
   CHECK_READ_ONLY
   if (*Error_Buffer || SLKeyBoard_Quit) return(0);
   if (Repeat_Factor != NULL)
     {
	ch = *Repeat_Factor;
	Repeat_Factor = NULL;
     }
   else
     {
	SLang_Key_TimeOut_Flag = 1;
	ch = jed_getkey();
	SLang_Key_TimeOut_Flag = 0;
     }
   
   
   SLang_Error = lerr;  /* ^G may set it */
  
   if ((ch == '\n') && (CBuf == MiniBuffer))
     {
	ins('\n');
	/* msg_error("Not allowed!"); */
	return (1);
     }

   SLKeyBoard_Quit = 0;
   ins_char_n_times(ch, 1);
   
   if (((ch == '}') || (ch == ')') || (ch == ']'))
       && !input_pending(&Number_Zero)) blink_match(ch);

   return(1);
}

/* I should try it like emacs--- if prefix argument, then save all without user
   intervention */
int save_some_buffers(void)
{
   Buffer *b, *tmp;
   int ans = 0;
   char msg[256];
   int err;

   b = CBuf;
   do
     {
	if ((b->flags & AUTO_SAVE_BUFFER) && (b->flags & BUFFER_TRASHED)
	    && (*b->file))
	  {
	     sprintf(msg,"Buffer %s not saved. Save it? (y/n)", b->name);
	     flush_message(msg);
	     ans = my_getkey();
	     while (1)
	       {
		  if ((ans == 'y') || (ans == 'Y'))
		    {
		       tmp = CBuf;
		       switch_to_buffer(b);
		       while (b->narrow != NULL) widen_buffer(b);
		       err = write_file_with_backup(b->dir, b->file);
		       switch_to_buffer(tmp);
  		       if (err < 0) return(-1);
		       b->flags &= ~BUFFER_TRASHED;
		       b->flags |= AUTO_SAVE_BUFFER;
		       b->hits = 0;
		       break;
		    }
		  else if ((ans == 'n') || (ans == 'N'))
		    {
		       /* disabling this */
		       /* auto_save_buffer(b); */
		       break;
		    }
		  else if (SLKeyBoard_Quit) 
		    {
		       /* warning--- bug here if user edits file at
		       startup and forgets to save it then aborts. */
		       return(-1);
		    }
		  else 
		    {
		       beep();
		       sprintf(msg,"Buffer %s not saved. Save it", b->name);
		       ans = get_yes_no(msg);
		       if (ans == 1) ans = 'y'; else if (!ans) ans = 'n'; 
		       else return(-1);
		    }
	       }
	  }

	b = b->next;
     }
   while (b != CBuf);
   message(" ");
   return(1);
}

int exit_jed()
{
   static int in_exit_jed = 0;
   
   if (!in_exit_jed)
     {
	in_exit_jed = 1;
	if (SLang_run_hooks("exit_hook", NULL, NULL)) 
	  {
	     in_exit_jed = 0;
	     return(1);
	  }
     }
   
   in_exit_jed = 0;
   if (SLang_Error) return(0);
   if (save_some_buffers() > 0) quit_jed();
   return(1);
}

int jed_buffer_visible (char *b)
{
   Buffer *buf;
   return ((NULL != (buf = find_buffer(b))) && (NULL != buffer_visible(buf)));
}

static void scroll_completion (int dir)
{
   Window_Type *w;
   
   if (jed_buffer_visible (Completion_Buffer))
     {
	pop_to_buffer (Completion_Buffer);
   	if (dir > 0) pagedown_cmd (); else pageup_cmd ();
	while (!IS_MINIBUFFER) other_window (); 
     }
   else
     {
	w = JWindow;
	other_window();
	if (!IS_MINIBUFFER)
	  {
	     if (dir > 0) pagedown_cmd (); else pageup_cmd ();
	  }
	while (JWindow != w) other_window ();
     }
}


static int Last_Page_Line;
static int Last_Page_Point;

/* the page up/down commands set cursor_motion to -1 because we do not
   want to use any goal column information */
int pagedown_cmd()
{
   int col;
   int n = JWindow->rows, dn, lpl, lp;
   
   Cursor_Motion = -1;
   if (IS_MINIBUFFER)
     {
	scroll_completion (1);
	return 1;
     }

   if (eobp()) 
     {
	msg_error(End_Of_Buffer_Error);
	return 1;
     }
   
   if ((CBuf != JWindow->buffer) || JWindow->trashed || (n == 1)) return(nextline(&n));
   
   lpl = window_line();
   lp = Last_Page_Point;
   Last_Page_Point = Point;
   col = calculate_column ();
   dn = n - lpl;
   if (dn == nextline(&dn)) recenter(&Number_One);
   goto_column1 (&col);
   if (Last_Key_Function == (VOID *) pageup_cmd) 
     {
	dn = Last_Page_Line - 1;
	nextline(&dn);
	Point = lp;
     }
   else if (CLine->next == NULL) eol(); else Point = 0;
   Last_Page_Line = lpl;
   
   return(1);
}

int pageup_cmd()
{
   int n = JWindow->rows, dn, lpl, lp, col;

   Cursor_Motion = -1;

   if (IS_MINIBUFFER)
     {
	scroll_completion (-1);
	return 1;
     }
   
   if (bobp()) 
     {
	msg_error(Top_Of_Buffer_Error);
	return 1;
     }
   
   if ((CBuf != JWindow->buffer) || JWindow->trashed || (n == 1)) return(prevline(&n));
  
   lpl = window_line();
   lp = Last_Page_Point;
   Last_Page_Point = Point;
   dn = lpl - 1;
   col = calculate_column ();
   prevline(&dn);
   (void) goto_column1(&col);
   recenter(&JWindow->rows);
   
   if (Last_Key_Function == (VOID *) pagedown_cmd)
     {
	dn = n - Last_Page_Line;
	prevline(&dn);
	Point = lp;
     }
   else Point = 0; /* something like: Point = point_column(JWindow->column) better? */
   Last_Page_Line = lpl; 
   return(1);
}


int scroll_right()
{
   if (JWindow->column == 1) return(0);

   if ((JWindow->column = JWindow->column - JWindow->width / 2) < 1)
     {
	JWindow->column = 1;
     }

   touch_window();
   return(1);
}

int scroll_left()
{
    JWindow->column = JWindow->column + JWindow->width / 2;
    touch_window();
    return(1);
}

/* goto to column c, returns actual column */
int goto_column1(int *cp)
{
   int c1, c = *cp;
   if (c <= 0) return(0);  /* not valid */
   eol();
   c1 = calculate_column();
   if (c1 > c)
     {
	point_column(c);
	c1 = calculate_column();
     }
   return(c1);
}

/* move cursor to column c adding spaces if necessary */
void goto_column(int *c)
{
   int c1;
   if (*c <= 0) return;
   c1 = *c - goto_column1(c);
   insert_whitespace(&c1);
}

/* does not leave current line */
unsigned int skip_whitespace()
{
   unsigned char *p, *pmax;

   if (CLine->len == 0) return('\n');

   p = CLine->data + Point;
   eol();
   pmax = CLine->data + Point;
   while(p < pmax)
     {
	if ((*p != ' ') && (*p != '\t')) break;
	p++;
     }
   Point = (int) (p - CLine->data);
   return(*p);
}

#define upcase(ch) (Case_Sensitive ? ch : UPPER_CASE(ch))


/* returns < 0 if what is smaller than current thing, 0 if matched, pos otherwise */
int looking_at(char *what)
{
   register unsigned char *p, *pmax;
   register unsigned char *w = (unsigned char *) what, ch;
   Line *l = CLine;
   
   p = l->data + Point;
   while (1)
     {
	pmax = l->data + l->len;
	while (((ch = *w) != 0) && (p < pmax))
	  {
	     if ((upcase(ch) != upcase(*p))) return 0;
	     p++; w++;
	  }
	if (ch == 0) return 1;
	l = l->next;
	if (l == NULL) return 0;
	p = l->data;
     }
}

void jed_make_lut (unsigned char *lut, unsigned char *range, unsigned char reverse)
{
   register unsigned char *l = lut, *lmax = lut + 256;
   int i, r1, r2;

   while (l < lmax) *l++ = reverse;
   
   while (*range)
     {
	r1 = *range;
	if (*(range + 1) == '-')
	  {
	     range += 2;
	     r2 = *range;
	  }
	else r2 = r1;
	
	for (i = r1; i <= r2; i++) lut[i] = !reverse;
	if (*range) range++;
     }
   
   /* more flexibility is achieved with following line */ 
   if (reverse && lut['\n']) lut['\n'] = 0; 
}



void skip_chars1(char *range, int reverse)
{
   unsigned char lut[256];
   unsigned char *p, *pmax;
   Line *line;
   int n = 0;
   
   jed_make_lut(lut, (unsigned char *) range, (unsigned char) reverse);
   
   line = CLine;
   while (line != NULL)
     {
	CLine = line;
	p = line->data + Point;
	pmax = line->data + line->len;

	while (p < pmax)
	  {
	     if (0 == lut[*p])
	       {
		  Point = (int)(p - CLine->data);
                  LineNum += n;
		  return;
	       }
	     p++;
	  }
	Point = 0;
	line = CLine->next;
        n++;
     }
   eob();
}

void bskip_chars1(char *range, int reverse)
{
   unsigned char lut[256];
   unsigned char *p;
   Line *line = CLine;
   int n = 0;
   
   jed_make_lut(lut, (unsigned char *) range, (unsigned char) reverse);
   
   while (1)
     {
	if (Point == 0) 
	  {
	     if (lut['\n'] == 0) return;
	     if (NULL == (line = CLine->prev)) break;
	     Point = line->len - 1;
             n++;
	  }
	
	CLine = line;
		 
	p = line->data + (Point - 1);

	while (p >= line->data)
	  {
	     if (0 == lut[*p])
	       {
		  Point = 1 + (int)(p - line->data);
                  LineNum -= n;
		  return;
	       }
	     
	     p--;
	  }
	Point = 0;
     }
   bob();
}

void skip_chars (char *range)
{
   if (*range == '^') skip_chars1(range + 1, 1);
   else 
     {
	if (*range == '\\') range++;
	skip_chars1(range, 0);
     }
}

void bskip_chars (char *range)
{
   if (*range == '^') bskip_chars1(range + 1, 1);
   else 
     {
	if (*range == '\\') range++;
	bskip_chars1(range, 0);
     }
}
