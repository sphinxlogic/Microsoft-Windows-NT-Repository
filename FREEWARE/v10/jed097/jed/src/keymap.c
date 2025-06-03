/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include "config.h"
#include "buffer.h"
#include "cmds.h"
#include "paste.h"
#include "screen.h"
#include "window.h"
#include "text.h"
#include "ledit.h"
#include "keymap.h"
#include "sysdep.h"
#include "misc.h"
#include "display.h"
#include "file.h"
#include "slang.h"
#include "undo.h"
#include "ins.h"
#include "hooks.h"

void (*X_Define_Keys_Hook)(SLKeyMap_List_Type *);

typedef struct
{
   jmp_buf b;
} jmp_buf_struct;

jmp_buf_struct Jump_Buffer, *Jump_Buffer_Ptr;

VOID *Last_Key_Function;
int *Repeat_Factor;                    /* repeats a key sequence if non null */


char *Read_This_Character = NULL;      /* alternate keyboard buffer */

char Jed_Key_Buffer[13];
char Key_Buffer[13];
char *Key_Bufferp = Key_Buffer;
static int Last_Key_Buffer_Len;

#ifdef HAS_MOUSE
JMouse_Type JMouse;
#endif

int beep (void)
{
   tt_beep ();
   flush_output ();
   return 1;
}

SLKeymap_Function_Type Jed_Functions[] =
  {
      {"backward_delete_char", backward_delete_char_cmd},
      {"backward_delete_char_untabify", backward_delete_char_untabify},
      {"beep", beep},
      {"begin_macro", begin_keyboard_macro},
      {"beg_of_buffer", bob},
      {"beg_of_line", bol},
      {"brace_bra_cmd", brace_bra_cmd},
      {"brace_ket_cmd", brace_ket_cmd},
      {"center_line", center_line},
      {"copy_region", copy_to_pastebuffer},
#if defined(msdos) || defined(__GO32__)
      {"coreleft", show_memory},
#endif
      {"delete_char_cmd", delete_char_cmd},
      {"delete_window", delete_window},
      {"digit_arg", digit_arg},
/*      {"double_line", double_line}, */
      {"end_macro", end_keyboard_macro},
      {"enlarge_window", enlarge_window},
      {"end_of_buffer", eob},
      {"eol_cmd", eol_cmd},
      {"evaluate_cmd", evaluate_cmd},
      {"execute_macro", execute_keyboard_macro},
      {"exchange",exchange_point_mark},
      {"exit_jed", exit_jed},
      {"exit_mini", exit_minibuffer},
      {"find_file", find_file},
      {"format_paragraph", text_format_paragraph},
      {"goto_match", goto_match},
      {"indent_line", indent_line},
      {"insert_file", insert_file_cmd},
      {"kbd_quit", kbd_quit},
      {"kill_buffer", kill_buffer},
      {"kill_line", kill_line},
      {"kill_region", kill_region},
      {"macro_query", macro_query},
      {"mark_spot", mark_spot},
      {"mini_complete", mini_complete},
      {"narrow", narrow_to_region},
      {"narrow_paragraph", narrow_paragraph},
      {"newline", newline},
      {"newline_and_indent", newline_and_indent},
      {"next_char_cmd", next_char_cmd},
      {"next_line_cmd", next_line_cmd},
      {"one_window", one_window},
      {"other_window", other_window},
      {"page_down", pagedown_cmd},
      {"page_up", pageup_cmd},
      {"pop_spot", pop_spot},
      {"previous_char_cmd", previous_char_cmd},
      {"previous_line_cmd", previous_line_cmd},
      {"quoted_insert", quoted_insert},
      {"redraw", redraw},
      /* {"replace", replace}, */
      {"save_buffers", save_some_buffers},
      {"scroll_left", scroll_left},
      {"scroll_right", scroll_right},
       /* {"search_backward", search_backward_cmd},
	  {"search_forward", search_forward_cmd}, */
      {"self_insert_cmd", ins_char_cmd},
      {"set_mark_cmd", set_mark_cmd},
      {"split_window", split_window},
      {"switch_to_buffer", get_buffer},
      {"sys_spawn_cmd", sys_spawn_cmd},
      {"text_smart_quote", text_smart_quote},
/*       {"transpose_lines", transpose_lines}, */
      {"trim_whitespace", trim_whitespace},
     {"undo", undo},
      {"widen", widen},
      {"write_buffer", write_buffer},
      {"yank", yank},
      {(char *) NULL, NULL}
   };

SLKeyMap_List_Type *Global_Map;
SLKeyMap_List_Type *Mini_Map;

int kbd_quit(void)
{
   int sle = SLang_Error;
   SLKeyBoard_Quit = 1;
   
   /* --- never serious, testing only
   if (Repeat_Factor != NULL) 
     {
	msg_error("Bang!");
	longjmp(Jump_Buffer_Ptr->b, 1);
     } */
   
   msg_error("Quit!");
   if (Ignore_User_Abort) SLang_Error = sle;
   return(1);
}

void init_keymaps(void)
{
   int ch;
   char simple[2];
   simple[1] = 0;
   
   if (NULL == (Global_Map = SLang_create_keymap ("global", NULL)))
     exit_error ("Malloc error creating keymap.", 0);
   
   Global_Map->functions = Jed_Functions;
   
   for (ch = ' '; ch < 256; ch++)
     {
	simple[0] = (char) ch;
	SLang_define_key1(simple, (VOID *) ins_char_cmd, SLKEY_F_INTRINSIC, Global_Map);
     }
   

    SLang_define_key1("^[1", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[2", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[3", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[4", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[5", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[6", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[7", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[8", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[9", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[0", (VOID *) digit_arg, SLKEY_F_INTRINSIC, Global_Map);

#ifndef pc_system
   /* give vtxxx arrow keys */
    SLang_define_key1("^[[D", (VOID *) previous_char_cmd,SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[[A", (VOID *) previous_line_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[[B", (VOID *) next_line_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[[C", (VOID *) next_char_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[[6~", (VOID *) pagedown_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[[5~", (VOID *) pageup_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^[[C", (VOID *) scroll_left, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^[[D", (VOID *) scroll_right, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^[[A", (VOID *) bob, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^[[B", (VOID *)eob, SLKEY_F_INTRINSIC, Global_Map);
#else
#include "doskeys.c"
#endif

    SLang_define_key1("'", (VOID *) text_smart_quote, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("\"", (VOID *) text_smart_quote, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^_", (VOID *) undo, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^?", (VOID *) backward_delete_char_untabify, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^B", (VOID *) bol, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^D", (VOID *) pagedown_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^E", (VOID *) eol_cmd, SLKEY_F_INTRINSIC, Global_Map);
/*
    SLang_define_key1("^FB", (VOID *) search_backward_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^FF", (VOID *) search_forward_cmd, SLKEY_F_INTRINSIC, Global_Map);
*/
    SLang_define_key1("^G", (VOID *) kbd_quit, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^I", (VOID *) indent_line, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K(", (VOID *) begin_keyboard_macro, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K)", (VOID *) end_keyboard_macro, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^KD", (VOID *) evaluate_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^KE", (VOID *) exit_jed, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^KG", (VOID *) find_file, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^KK", (VOID *) copy_to_pastebuffer, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^KM", (VOID *) mark_spot, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^KX", (VOID *) execute_keyboard_macro, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^B", (VOID *) set_mark_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^I", (VOID *) insert_file_cmd, SLKEY_F_INTRINSIC, Global_Map);
/*     SLang_define_key1("^K^L", (VOID *) double_line, SLKEY_F_INTRINSIC, Global_Map); */
    SLang_define_key1("^K^M", (VOID *) pop_spot, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^P", (VOID *) yank, SLKEY_F_INTRINSIC, Global_Map);
/*     SLang_define_key1("^K^R", (VOID *) replace, SLKEY_F_INTRINSIC, Global_Map); */
    SLang_define_key1("^K^V", (VOID *) kill_region, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^K^W", (VOID *) write_buffer, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^L", (VOID *) kill_line, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^M", (VOID *) newline_and_indent, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^R", (VOID *) redraw, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^U", (VOID *) pageup_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^V", (VOID *) delete_char_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^W0", (VOID *) delete_window, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^W1", (VOID *) one_window, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^W2", (VOID *) split_window, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^WO", (VOID *) other_window, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^XB", (VOID *) get_buffer, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^XK", (VOID *) kill_buffer, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^XN", (VOID *) narrow_to_region, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^XQ", (VOID *) macro_query, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^XS", (VOID *) save_some_buffers, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^XW", (VOID *) widen, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^X^", (VOID *) enlarge_window, SLKEY_F_INTRINSIC, Global_Map);
/*     SLang_define_key1("^X^T", (VOID *) transpose_lines, SLKEY_F_INTRINSIC, Global_Map); */
   SLang_define_key1("^X^[", (VOID *) evaluate_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^X^X", (VOID *) exchange_point_mark, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^Z", (VOID *) sys_spawn_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[<", (VOID *) bob, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[>", (VOID *) eob, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[N", (VOID *) narrow_paragraph, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[Q", (VOID *) text_format_paragraph, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[S", (VOID *) center_line, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[X", (VOID *) evaluate_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^[\\", (VOID *) trim_whitespace, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("^\\", (VOID *) goto_match, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("`", (VOID *) quoted_insert, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("{", (VOID *) brace_bra_cmd, SLKEY_F_INTRINSIC, Global_Map);
    SLang_define_key1("}", (VOID *) brace_ket_cmd, SLKEY_F_INTRINSIC, Global_Map);

   if (X_Define_Keys_Hook != NULL)  (*X_Define_Keys_Hook)(Global_Map);
}

static int key_interpret(SLang_Key_Type *key)
{
   char *rtc_save;
   char *str;
   int ret;
   static int macro_depth;
   
   if (key->type == SLKEY_F_INTRINSIC)
     {
	ret = (int) ((int (*)(void)) key->f)();
     }
   else
     {
	str = (char *) key->f;
	
	if (*str == ' ')
	  {
	     /* string to insert */
	     insert_string(str + 1);
	  }
	else if ((*str == '@') && (0 != *(str + 1)))
	  {
	     if (macro_depth > 10) 
	       {
		  macro_depth = 0;
		  msg_error ("Possible runaway macro aborted.");
		  return 1;
	       }
	     macro_depth++;
	     rtc_save = Read_This_Character;
	     Read_This_Character = str + 1;
	     do
	       {
		  do_key();
	       }
	     while (Read_This_Character != NULL);
	     Read_This_Character = rtc_save;
	     macro_depth--;
	  }
	else if ((*str == '.')
		 || !SLang_execute_function(str))
	  SLang_load_string(str);

	ret = 1;
     }
   
   Last_Key_Function = key->f;
   return(ret);
}

static void update_jed_keybuffer (void)
{
   Last_Key_Buffer_Len = (int) (Key_Bufferp - Key_Buffer);
   if (Last_Key_Buffer_Len == 1)
     {
	*Jed_Key_Buffer = *Key_Buffer;
	*(Jed_Key_Buffer + 1) = 0;
     }
   else
     {
        *Key_Bufferp = 0;
	strcpy (Jed_Key_Buffer, Key_Buffer);
     }
   
   Key_Bufferp = Key_Buffer;
}

   
#define XKEY(key)  key_interpret((key))

int do_key (void)
{
   SLang_Key_Type *key;
   int repeat;
   
   if (SLang_Key_TimeOut_Flag == 0)
     {
	Key_Bufferp = Key_Buffer;
	*Key_Bufferp = 0;
     }
   
   key = SLang_do_key (CBuf->keymap, jed_getkey);
   update_jed_keybuffer ();
   
   if ((key != NULL) && (key->f != NULL))
     {
	if (Repeat_Factor == NULL) return XKEY(key);
	repeat = *Repeat_Factor;
	Suspend_Screen_Update = 1;

	/* some routines may use the repeat factor as a prefix argument */
	while (repeat-- > 0)
	  {
	     if (SLKeyBoard_Quit || SLang_Error ||
		 (Repeat_Factor == NULL)) break;
	 /* (!Executing_Keyboard_Macro && (Repeat_Factor == NULL))) break; */
	     XKEY(key);
	  }
	Repeat_Factor = NULL;

	return(1);
     }
   else if (!Executing_Keyboard_Macro && !SLKeyBoard_Quit)
     {
	beep ();
	flush_input();
     }
   if (SLKeyBoard_Quit) kbd_quit();
   return(0);
}

void do_jed(void)
{
   char *name;
   Buffer *tthis = CBuf;
   
   /* Mark Undo boundary now because tthis might not be valid later */
   mark_undo_boundary(tthis);
   
   Repeat_Factor = NULL;
   
   if (do_key()) JWindow->trashed = 1;

    /* internal editing commands may have selected a different buffer
       so put it back. */
    if (CBuf != JWindow->buffer)
      {
	 if (buffer_exists(JWindow->buffer)) name = JWindow->buffer->name; else name = "*scratch*";
	 switch_to_buffer_cmd(name);
      }
}

void jed(void)
{
   /* This routine is called from main.  So before actually strting, check
      one more hook to just to make sure  all things are go. */
   SLang_run_hooks("jed_startup_hook", NULL, NULL);

   Jump_Buffer_Ptr = &Jump_Buffer;
   
   if (setjmp(Jump_Buffer.b) != 0)
     {
	SLang_restart(1);   /* just in case */
     }

   if (CBuf != JWindow->buffer)
     {
	switch_to_buffer(JWindow->buffer);
	window_buffer(CBuf);
     }
   JWindow->trashed = 1;
   update((Line *) NULL, 0, 0);
   Read_This_Character = NULL;
   while(1)
     {
	Suspend_Screen_Update = 0;
	do_jed();
	if (!SLKeyBoard_Quit && (CBuf->flags & BUFFER_TRASHED)
	    && (!Cursor_Motion)) CBuf->hits += 1;
	if (CBuf->hits > User_Vars.max_hits)
	  {
	     auto_save_buffer(CBuf);
	     check_buffers();   /* check files on disk to see if they are recent */
	  }
	
	if ((Last_Key_Function != (VOID *) ins_char_cmd)
	    || (JWindow->trashed) || (JWindow != JWindow->next)
	    || Suspend_Screen_Update ||
	    (Mini_Ghost) || (*Message_Buffer) || (*Error_Buffer))
	  {
	     update((Line *) NULL, 0, 0);
	  }
     }
}

int digit_arg(void)
{
   static int repeat;
   char buf[20];
   int key;
   int i;
   
   i = 0;
   buf[i++] = (char) SLang_Last_Key_Char;
   
   /* After do_key (what called this), Key_Bufferp is reset.  However, I want 
    * to keep it for echoing subsequent characters.  I restore its previous 
    * value so that echoing will continue. 
    */
   
   Key_Bufferp = Key_Buffer + Last_Key_Buffer_Len;

   SLang_Key_TimeOut_Flag = 1;
   while(1)
     {
	buf[i] = 0;
	key = jed_getkey();
	if ((key < '0') || (key > '9')) break;
	buf[i++] = (char) key;
     }
   repeat = atoi(buf);
   Repeat_Factor = &repeat;
   if (Executing_Keyboard_Macro) Macro_Buffer_Ptr--;
   else 
     {
	ungetkey (&key);
	if (Defining_Keyboard_Macro) Macro_Buffer_Ptr--;
     }
   
   if (Key_Bufferp != Key_Buffer) Key_Bufferp--;

   /* Key_Timeout is still active and is only reset after this call. */
   do_key();
   return(1);
}

int which_key (char *f)
{
   int num = 0, i;
   SLang_Key_Type *key, *key_root;
   VOID *fp;
   unsigned char type;
   unsigned char buf[5];
   
   if (NULL == (fp = (VOID *) SLang_find_key_function(f, CBuf->keymap)))
     type = SLKEY_F_INTERPRET;
   else type = SLKEY_F_INTRINSIC;

   i = 256;
   key_root = CBuf->keymap->keymap;
   while (i--)
     {
	key = key_root->next;
	if ((key == NULL) && (type == key_root->type) &&
	    (((type == SLKEY_F_INTERPRET) && (!strcmp((char *) f, (char *) key_root->f)))
	    || ((type == SLKEY_F_INTRINSIC) && (fp == key_root->f))))
	  {
	     *buf = 2;
	     *(buf + 1) = 256 - 1 - i;
	     SLang_push_string(SLang_make_keystring(buf));
	     num++;
	  }
	while (key != NULL)
	  {
	     if ((key->type == type) &&
		 (((type == SLKEY_F_INTERPRET) && (!strcmp((char *) f, (char *) key->f)))
		 || ((type == SLKEY_F_INTRINSIC) && (fp == key->f))))
	       {
		  SLang_push_string(SLang_make_keystring(key->str));
		  num++;
	       }
	     key = key->next;
	  }
	key_root++;
     }
   return(num);
}


static char *find_function_string (VOID *f)
{
   SLKeymap_Function_Type *fp;

   fp = Jed_Functions;

   if (f == (VOID *) ins_char_cmd) return "self_insert_cmd";
   
   while((fp != NULL) && (fp->name != NULL))
     {
	if ((VOID *) fp->f == f) return fp->name;
	fp++;
     }
   return NULL;
}


static void dump_this_binding (unsigned char *s, VOID *f, int type)
{
   unsigned char ch;
   char *str,  ctrl[2];
   char *fun;
   int n, len;
   
   ctrl[0] = '^';
   len = *s++ - 1;;
   
   while (len-- > 0)
     {
	n = 1;
        ch = *s++;
	if (ch == 127) 
	  {
	     str = "DEL"; n = 3;
	  }
	else if (ch > ' ') str = (char *) &ch;
	else if (ch == 27) 
	  {
	     str = "ESC";
	     n = 3;
	  }
	else if (ch == ' ')
	  {
	     str = "SPACE";
	     n = 5;
	  }
	else if (ch == '\t')
	  {
	     str = "TAB"; n = 3;
	  }
	else
	  {
	     str = ctrl;
	     *(str + 1) = ch + '@';
	     n = 2;
	  }
	quick_insert((unsigned char *) str, n);
	ins(' ');
     }
   ins_char_n_times('\t', 3);
   
   if (type == SLKEY_F_INTRINSIC)
     {
   	fun = find_function_string (f);
     }
   else fun = (char *) f;

   insert_string (fun);
   newline();
}

void  dump_bindings(char *map)
{
   int i;
   SLang_Key_Type *next, *key_root;
   SLKeyMap_List_Type *kml;

   if (NULL == (kml = SLang_find_keymap(map)))
     {
	msg_error("Keymap undefined.");
	return;
     }
   key_root = kml->keymap;
   
   for (i = 0; i < 256; i++)
     {
	next = key_root->next;
	if (next != NULL)
	  {
	     while (next != NULL)
	       {
		  dump_this_binding (next->str, next->f, next->type);
		  next = next->next;
	       }
	  }
	else if (key_root->f != NULL) dump_this_binding (key_root->str, key_root->f, key_root->type);
	key_root++;
     }
}

char *find_key(int *ret)
{
   char *fstr = NULL;
   SLang_Key_Type *key;

   *ret = 0;
   
   SLang_Key_TimeOut_Flag = 0;
   Key_Bufferp = Key_Buffer;
   key = SLang_do_key (CBuf->keymap, jed_getkey);
   update_jed_keybuffer ();
   
   if (key == NULL) return(NULL);
   if (key->type == SLKEY_F_INTRINSIC)
     {
	*ret = 1;
	fstr = find_function_string (key->f);
     }
   else fstr = (char *) key->f;

   return(fstr);
}


void use_keymap(char *name)
{
   SLKeyMap_List_Type *map;
   if (NULL == (map = SLang_find_keymap(name)))
     {
	msg_error("Unknown keymap.");
     }
   else (CBuf->keymap = map);
}

char *what_keymap()
{
   return CBuf->keymap->name;
}

void set_abort_char(int *c)
{
   char str[2];
   int i;
#ifdef pc_system
   char ch, *s, *scan = "@#$%^&*()_+?IQWERTYUIOP[]!!ASDFGHJKL!!!!\\ZXCVBNM";
   
   ch = 64 + (char) *c;
   s = scan; while (*s && (*s != ch)) s++;
   if (*s == 0) return;
   Abort_Char = (int) (s - scan) + 0x03;
#else
   Abort_Char = *c;
#endif
   reset_tty();
   init_tty();
   str[0] = *c; str[1] = 0;
    
   for (i = 0; i < SLANG_MAX_KEYMAPS; i++)
     {
	if (SLKeyMap_List[i].keymap == NULL) continue;
	SLang_undefine_key(str, &SLKeyMap_List[i]);
	SLang_define_key1(str, (VOID *) kbd_quit, SLKEY_F_INTRINSIC, &SLKeyMap_List[i]);
     }
}


static SLKeymap_Function_Type *Flist_Context;
static int Flist_Context_Len;

#define MAX_USER_FLIST 100
static char *Slang_Functions[MAX_USER_FLIST];
static char **Slang_Flist_Context;


int next_function_list(char *buf)
{
   SLKeymap_Function_Type *tthis = Jed_Functions;
   register char *name;
   char **max;
   
   /* COnvert '-' to '_' */
   
   name = buf;
   while (*name != 0) 
     {
	if (*name == '-') *name = '_';
	name++;
     }
   
   while (1)
     {
	tthis = Flist_Context;
	name = tthis->name;
	if (name == NULL) break;
	Flist_Context++;
	if (!Flist_Context_Len || !strncmp(buf, name, Flist_Context_Len))
	  {
	     strcpy(buf, name);
	     return(1);
	  }
     }
   
   max = Slang_Functions + MAX_USER_FLIST;
   while (Slang_Flist_Context < max)
     {
	name = *Slang_Flist_Context;
	if (name == NULL) return(0);
	name++;  /* skip past hash mark */
	Slang_Flist_Context++;
	if (!Flist_Context_Len || !strncmp(buf, name, Flist_Context_Len))
	  {
	     strcpy(buf, name);
	     return(1);
	  }
     }
   return(0);
}

int open_function_list(char *buf)
{
   Flist_Context = Jed_Functions;
   Slang_Flist_Context = Slang_Functions;
   Flist_Context_Len = strlen(buf);
   return next_function_list(buf);
}

void add_to_completion(char *name)
{
   char **p, *n;
   static char **last = Slang_Functions;
   
   n = SLang_find_name(name);
   if (n == NULL)
     {
	msg_error("Undefined Symbol.");
	return;
     }
   
   p = last;  /* Slang_Functions; */
   
   while (p < Slang_Functions + MAX_USER_FLIST)
     {
	if (*p == NULL) 
	  {
	     *p = n;
	     last = ++p;
	     return;
	  }
	p++;
     }
   msg_error("Completion Quota Exceeded.");
}

int is_internal(char *f)
{
   if (NULL == SLang_find_key_function(f, CBuf->keymap)) return(0);
   return 1;
}

void create_keymap (char *name)
{
   if (NULL == SLang_create_keymap (name, Global_Map))
     {
	msg_error ("Unable to create keymap.");
     }
}
