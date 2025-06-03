/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <string.h>

#include "slang.h"
#include "config.h"
#include "buffer.h"
#include "keymap.h"
#include "file.h"
#include "ins.h"
#include "ledit.h"
#include "screen.h"
#include "window.h"
#include "display.h"
#include "search.h"
#include "misc.h"
#include "replace.h"
#include "paste.h"
#include "sysdep.h"
#include "cmds.h"
#include "text.h"
#ifdef __GO32__
#define LANG_INTERRUPT i386_check_kbd
extern void i386_check_kbd (void);
#endif


#if defined(msdos) || defined(__os2__)
#include <process.h>
#endif

extern char Jed_Root_Dir[256];

static void do_buffer_keystring (char *s)
{
   buffer_keystring (s, strlen(s));
}

static void do_tt_write_string (char *s)
{
   tt_write_string (s);
}

#ifndef pc_system
static void do_tt_set_term_vtxxx (int *i)
{
   tt_set_term_vtxxx (i);
}
#endif


static int jed_getpid (void)
{
   return getpid ();
}

static int map_color_object_to_number (char *what)
{
   int i;
   
   if (!strcmp("normal", what)) i = JNORMAL_COLOR;
   else if (!strcmp("status", what)) i = JSTATUS_COLOR;
   else if (!strcmp("region", what)) i = JREGION_COLOR;
   else if (!strcmp("cursor", what)) i = JCURSOR_COLOR;
   else if (!strcmp("operator", what)) i = JOP_COLOR;
   else if (!strcmp("number", what)) i = JNUM_COLOR;
   else if (!strcmp("string", what)) i = JSTR_COLOR;
   else if (!strcmp("comment", what)) i = JCOM_COLOR;
   else if (!strcmp("keyword", what)) i = JKEY_COLOR;
   else if (!strncmp("delimiter", what, 5)) i = JDELIM_COLOR;
   /* Oops!-- earlier version had delimiter spelled delimeter.  This is
    * for backward compatability.
    */
   else if (!strcmp("preprocess", what)) i = JPREPROC_COLOR;
   else if (!strcmp("menu", what)) i = JMENU_COLOR;
   
   else i = -1;
   return i;
}

static void enable_menu_bar (int *what)
{
   Window_Type *w;
   
   /* find the top window */
   w = JWindow;
   
   while (w->top != Top_Window_Row) w = w->next;
   if (*what) 
     {
	if (Top_Window_Row != 1) return;
	if (w->rows < 3) 
	  {
	     /* window is too small --- fix it. */
	     one_window ();  w = JWindow;
	     if (w->rows < 3) return;
	  }
	w->top = Top_Window_Row = 2;
	w->rows -= 1;
     }
   else 
     {
	if (Top_Window_Row == 1) return;
	w->top = Top_Window_Row = 1;
	w->rows += 1;
     }
   touch_screen ();
}

	     
static int rename_file (char *f1, char *f2)
{
   return rename (f1, f2);
}

	
   
void set_term_colors(char *what, char *fg, char *bg)
{
   int i;
   i = map_color_object_to_number (what);
   tt_set_color (i, what, fg, bg);
}

#ifndef pc_system
void set_term_color_esc (char *what, char *esc)
{
   int i;
   if (tt_set_color_esc == NULL) return;
   
   i = map_color_object_to_number (what);
   tt_set_color_esc (i, esc);
}
#endif


static void exit_error_cmd (char *msg, int *severity)
{
   exit_error (msg, *severity);
}

void save_search_string(char *s)
{
   strncpy(Last_Search_Str, s, 131);
   Last_Search_Str[131] = 0;
}

int do_prefix_argument(int *n)
{
   int ret = *n;
   if (Repeat_Factor != NULL)
     {
	ret = *Repeat_Factor;
	Repeat_Factor = NULL;
     }
   return ret;
}



static void bury_buffer(char *name)
{
   Buffer *b, *cp, *bp, *bn;
   
   if ((NULL == (b = find_buffer(name)))
       || (b == CBuf)
       || (CBuf == (bn = b->next))) return;
   
   cp = CBuf->prev;
   bp = b->prev;
   
   CBuf->prev = b;		       /* my definition of bury buffer */
   b->next = CBuf;
   b->flags |= BURIED_BUFFER;
   bp->next = bn;
   bn->prev = bp;
   cp->next = b;
   b->prev = cp;
}


static void set_buffer_hook (char *s, char *h)
{
   SLang_Name_Type *f;
   
   f = SLang_get_function (h);
   
   if (!strcmp("par_sep", s)) CBuf->par_sep = f;
   else if (!strcmp("indent_hook", s)) CBuf->indent_hook = f;
   else if (!strcmp("wrap_hook", s)) CBuf->wrap_hook = f;
}
   
	

static SLang_Name_Type jed_intrinsics[] =
{
   MAKE_INTRINSIC(".setkey", set_key, VOID_TYPE, 2),
   /*Prototype: Void setkey(String fun, String key);
     'fun' is the function that 'key' is to be assigned.  'key' can contain
     the '^' character which denotes that the following character is to be
     interpreted as a control character, e.g.,
     
          setkey("bob", "^Kt");
	  
     sets the key sequence 'Ctrl-K t' to the function 'bob' which moves the 
     editing point to the beginning of the buffer.  Note that 'setkey' works 
     on the "global" keymap.
     See also: unsetkey, definekey. */

   MAKE_INTRINSIC(".push_mark", push_mark,VOID_TYPE, 0),
   /*Prototype: Void push_mark();
     This function marks the current Point as the beginning of a region.  
     Other functions operate on the region defined by the Point and the Mark. 
     See also: pop_mark, push_spot, markp, dupmark, check_region */
   
   MAKE_INTRINSIC(".bol", bol,VOID_TYPE, 0),
   /*Prototype: Void bol();
    * Moves Point to the beginning of the current line.
    * See also: eol, bob, eob, bolp */

   MAKE_INTRINSIC(".setmode", set_mode_cmd, VOID_TYPE, 2),
   /*Prototype:  Void setmode(String mode, Integer flags);
    * Sets buffer mode flags and status line mode name.  'mode' is a string 
    * which is displayed on the status line if the %m status line format 
    * specifier is used. The second argument, 'flags' is an integer with 
    * the possible values:

    *     0 : no mode. Very generic
    *     1 : Wrap mode.  Lines are automatically wrapped at wrap column.
    *     2 : C mode.
    *     4 : Language mode.  Mode does not wrap but is useful for computer 
    *         languages.
    *     8 : S-Lang mode
    *   0x10: Fortran mode highlighting
    *   0x20: TeX mode highlighting
    * See also: whatmode, getbuf_info, setbuf_info.  */
	    
   MAKE_INTRINSIC(".insert", insert_string,VOID_TYPE, 1),
   /*Prototype: Void insert(String str);
    * Inserts string 'str' in buffer at the current Point.
    * See also: del, insert_file, insbuf */
   MAKE_INTRINSIC(".eol", eol,VOID_TYPE, 0),
   /*Prototype Void eol();
    * Moves Point to the end of the current line. 
    * See also: eolp, bol, bob, eob */
   MAKE_INTRINSIC(".setbuf", set_buffer, VOID_TYPE, 1),
   /*Prototype: Void setbuf(String buf);
    * Changes the default buffer to one named 'buf' for editing.  This change
    * only lasts until top level of editor loop is reached at which point the
    * the buffer associated with current window will be made the default.
    * See also: sw2buf, pop2buf, whatbuf */
   MAKE_INTRINSIC(".message", message, VOID_TYPE, 1),
   /*Prototype: Void message(String msg);
    * Displays the string 'msg' in the MiniBuffer at the bottom of the display.
    * This does not immediately display the message.  In particular, subsequent
    * calls to 'message' will overwrite the values of previous calls.  The 
    * message is displayed when the screen is updated next.  For an immediate
    * effect, use the function 'flush' instead.
    * See also: flush, error, update */
   
   MAKE_INTRINSIC(".del_region", delete_region, VOID_TYPE, 0),
   /*Prototype: Void del_region();
    * Deletes the region specified by the Point and Mark.
    * See also: push_mark, markp, check_region */
   MAKE_INTRINSIC(".bufsubstr", buffer_substring, VOID_TYPE, 0),
   /*Prototype: String bufsubstr();
    * This function returns the region specified by the Point and Mark as 
    * a String.  If the region crosses lines, the string will contain
    * newline characters.
    * See also: insbuf, push_mark */
   MAKE_INTRINSIC(".right", forwchars,INT_TYPE, 1),
   /*Prototype: Integer right(Integer n);
    * This function moves the Point forward forward 'n' characters and returns
    * the number actually moved.  The number returned will be smaller than 'n'
    * if the end of the buffer is reached.
    * See also: left, up, down, eol, eob */

   MAKE_INTRINSIC(".definekey", set_key_in_keymap, VOID_TYPE, 3),
   /*Prototype: Void definekey(String f, String key, String kmap);
    * This function is used for binding keys to functions in a specific keymap.
    * Here 'f' is the function to be bound, 'key' is a sequence of keys and 
    * 'kmap' is the name of the keymap to be used.  See 'setkey' for a 
    * definition of 'keys'.  
    See also: setkey, undefinekey, make_keymap, use_keymap */
   MAKE_VARIABLE(".LAST_SEARCH", Last_Search_Str, STRING_TYPE, 1),
   /*A readonly string variable containing the value of the last 
    * interactively search for string.
    * See also: save_search_string. */
#ifndef pc_system
   MAKE_VARIABLE(".USE_ANSI_COLORS", &tt_Use_Ansi_Colors, INTP_TYPE, 0),
#endif
   MAKE_VARIABLE(".Status_Line_String", Default_Status_Line, STRING_TYPE, 1),
   /*A read-only String variable containing the format of the status line
    * applied to newly created buffers.
    See also: set_status_line */
   MAKE_INTRINSIC(".save_search_string", save_search_string, VOID_TYPE, 1),
   /*Prototype: Void save_search_string(String str);
    * Sets the value of the read-only variable 'LAST_SEARCH' to 'str'. 
    * See also: LAST_SEARCH */
   MAKE_VARIABLE(".MINIBUFFER_ACTIVE", &MiniBuffer_Active, INT_TYPE, 1),
   /*A read-only variable that is non-zero when the MiniBuffer is actively
    * being used. */
   MAKE_INTRINSIC(".left", backwchars,INT_TYPE, 1),
   /*Prototype: Integer left(Integer n);
    * Move Point backward 'n' characters returning number actually moved.  
    * The number returned will be less than 'n' only if the top of the buffer 
    * is reached. 
    * See Also: right, up, down, bol, bob */
   MAKE_INTRINSIC(".whatbuf", what_buffer, STRING_TYPE, 0),
   /*Prototype: String what_buffer();
    * Returns the name of the current buffer.
    * See also: getbuf_info, bufferp */
   MAKE_INTRINSIC(".error", msg_error, VOID_TYPE, 1),
   /*Prototype: Void error(String msg);
    * Signals an error condition and displays the string 'msg' at the bottom of
    * the display.  The error can be caught by an ERROR_BLOCK and cleared with
    * the '_clear_error' function.  Unless caught, the error will cause the 
    * S-Lang stack to unwind to the top level.
    * See also: _clear_error, message, flush */
   MAKE_INTRINSIC(".getbuf_info", get_buffer_info,  VOID_TYPE, 0),
   /*Prototype:  getbuf_info();
    * This function returns 4 values to the stack.  The four value from the 
    * top are:
    *  Integer'flags'        % buffer flags
    *  String 'buffer_name'  % name of buffer
    *  String 'directory'    % directory associated with buffer
    *  String 'file'         % name of file associated with buffer (if any).

    * The flags are encoded as: 
    *   bit 0: buffer modified
    *   bit 1: auto save mode
    *   bit 2: file on disk modified
    *   bit 3: read only bit
    *   bit 4: overwrite mode
    *   bit 5: undo enabled
    *   bit 6: reserved
    *   bit 7: just save instead of autosaving.  If bit 1 is set, 
    *          this is ignored.

    * For example, 
    *   (file,,,) = getbuf_info();
    * returns the file associated with the buffer.
    * See also: setbuf_info, whatbuf */
   MAKE_INTRINSIC(".otherwindow", other_window, VOID_TYPE, 0),
   /*Prototype: Void otherwindow();
    * Switch to next window. 
    * See also: nwindows, onewindow */
   MAKE_INTRINSIC(".is_internal", is_internal, INT_TYPE, 1),
   /*Prototype Integer is_internal(String f);
    * Returns non-zero is function 'f' is defined as an internal function or
    * returns zero if not.  Internal functions not immediately accessable from
    * S-Lang; rather, they must be called using the 'call' function.
    See also: call, is_defined.*/
   MAKE_INTRINSIC(".setbuf_info", set_buffer_info,  VOID_TYPE, 4),
   /*Prototype: Void setbuf_info(String file, String dir, String buf, Integer flags);
    * Sets information of the current buffer.  Here 'file' is the name of the 
    * file to be associated with the buffer; 'dir' is the directory to be 
    * associated with the buffer; buf is the name to be assigned to the buffer,
    * and flags describe the buffer attributes.  See 'getbuf_info' for a 
    * discussion of 'flags'.  Note that the actual file associated with the
    * buffer is located in directory 'dir' with the name 'file'.
    * See also: getbuf_info */
   MAKE_INTRINSIC(".up", prevline, INT_TYPE, 1),
   /*Prototype: Integer up(Integer n);
    * Move Point up 'n' lines returning number of lines actually moved.  The 
    * number returned will be less than 'n' only if the top of the buffer is
    * reached.
    See also: down, left, right */
   MAKE_INTRINSIC(".down", nextline,INT_TYPE, 1),
   /*Prototype: Integer down(Integer n);
    * Move Point down 'n' lines returning number of lines actually moved.  The 
    * number returned will be less than 'n' only if the last line of the buffer
    * has been reached.
    See also: down, left, right */
   MAKE_INTRINSIC(".call", call_cmd, VOID_TYPE, 1),
   /*Prototype: Void call(String f);
    * Execute internal function named 'f'.  An internal function is a function
    * that cannot be directly accessed from S-Lang.
    * See: is_internal. */
   MAKE_INTRINSIC(".eob", eob,VOID_TYPE, 0),
   /*Prototype: Void eob();
    * Move Point to the end of the buffer.
    * See also: eobp, bob, bol, eol */
   MAKE_INTRINSIC(".unsetkey", unset_key, VOID_TYPE, 1),
   /*Prototype: Void unsetkey(String key);
    * Removes the defeinition of 'key' from the "global" keymap.  For example,
    * by default, the "global" keymap binds the keys "^[[A", "^[[B", "^[[C", 
    * and "^[[D" to the character movement functions.  Using 
    * 'unsetkey("^[[A")' will remove the binding of "^[[A" from the global 
    * keymap but the other three will remain.  However, 'unsetkey("^[[")' 
    * will remove the definition of all the above keys.
    * See also:  setkey, undefinekey */
   MAKE_INTRINSIC(".bob", bob,VOID_TYPE, 0),
   /*Prototype: Void eob();
    * Move Point to the beginning of the buffer.
    * See also: bobp, eob, bol, eol */
   MAKE_INTRINSIC(".looking_at", looking_at, INT_TYPE, 1),
   /*Prototype: Integer looking_at(String s);
    * Returns non-zero if Point is positioned in he buffer such that the 
    * characters immediately following it match 's' otherwise it returns 0.
    * See also: ffind, fsearch, re_fsearch, bfind */
   MAKE_INTRINSIC(".del", del,VOID_TYPE, 0),
   /*Prototype: Void del();
    * Delete character at point unless at the end of the buffer in which case
    * nothing happens.
    * See also: what_char, eobp, del_region */
   MAKE_INTRINSIC(".markp", markp, INT_TYPE, 0),
   /*Prototype: Void markp();
    * Returns a non-zero value if a mark is set in the buffer otherwise it
    * returns zero.  A mark usually denotes a region is defined.
    * See also: push_mark, pop_mark, check_region, push_spot */
   MAKE_INTRINSIC(".nwindows", num_windows,INT_TYPE, 0),
   /*Prototype Integer nwindows();
    * Return number of windows currently visible.
    * See also: splitwindow, onewindow, window_size */
   MAKE_INTRINSIC(".add_completion", add_to_completion, VOID_TYPE, 1),
   /*Prototype Void add_completion(String f);
    * Add S-Lang function with name 'f' to the list of function completions.
    * See also: read_with_completion */
   MAKE_INTRINSIC(".what_column", calculate_column,INT_TYPE, 0),
   /*Prototype: Integer what_column();
    * Returns the current column number of the Point expanding tabs, etc...
    * The beginning of the line is at column 1.
    * See: whatline, whatpos, goto_column */
   MAKE_INTRINSIC(".eobp", eobp,INT_TYPE, 0),
   /*Prototype: Integer eobp();
    * Returns non-zero if the Point is positions at the beginning of the 
    * buffer otherwise it returns zero.
    * See also: eob, bolp, eolp */
   MAKE_INTRINSIC(".fsearch", search_forward, INT_TYPE, 1),
   /*Prototype: Integer fsearch(String str);
    *  Search forward in buffer looking for string 'str'.  If not found, this
    * functions returns zero.  However, if found, it returns non-zero and 
    * moves the Point to the start of the match.  It respects the setting
    * of the variable 'CASE_SEARCH'.
    * See also: ffind, bsearch, bfind, re_fsearch, CASE_SEARCH */
   
   MAKE_INTRINSIC(".buffer_visible", jed_buffer_visible, INT_TYPE, 1),
   /*Prototype: Integer buffer_visible(String buffer);
    * Returns non-zero if 'buffer' is currently visible in a window or
    * it returns zero if not. */
     
   MAKE_INTRINSIC(".exit_jed", exit_jed, VOID_TYPE, 0),
   /*Prototype: Void exit_jed();
    * Exits JED.  If any buffers are modified, the user is queried 
    * about whether or not to save first.  Calls S-Lang hook "exit_hook"
    * if defined.  If "exit_hook" is defined, it must either call 'quit_jed'
    * or 'exit_jed' to really exit the editor.  If 'exit_jed' is called from
    * 'exit_hook', 'exit_hook' will not be called again. */
   MAKE_INTRINSIC(".set_color", set_term_colors, VOID_TYPE, 3),
   /*Prototype: Void set_color(String object, String fg, String bg);
    * This function sets the foreground and background colors of an 'object'
    * to 'fg' and 'bg'.  The exact values of the strings 'fg' and 'bg' are 
    * system dependent.  
    * Valid object names are: 
    *   "status", "normal", "region", and "cursor".
    * In addition, if color syntax highlighting is enabled, the following object
    * names are also meaningful:
    *   "number", "delimeter", "keyword", "string", "comment", "operator",
    *   "preprocess"
    * See also: WANT_SYNTAX_HIGHLIGHT
    */
#ifndef pc_system
   MAKE_INTRINSIC(".set_color_esc", set_term_color_esc, VOID_TYPE, 2),
   /*Prototype: Void set_color_esc (String object, String esc_seq);
    * This function may be used to associate an escape sequence with an 
    * object.  The escape sequence will be set to the terminal prior to 
    * sending the object.  It may be used on mono terminals to underline 
    * objects, etc...
    * See 'set_color' for a list of valid object names.
    */
#endif
   MAKE_INTRINSIC(".extract_filename", extract_file, STRING_TYPE, 1),
   /*Prototype: String extract_filename (String filespec);
    * Separates the filename from the path of 'filespec'. 
    * Example: (unix)
    *    var = extract_filename ("/tmp/name");
    * assigns a value of "name" to 'var'
    */
   MAKE_INTRINSIC(".trim", trim_whitespace,VOID_TYPE, 0),
   /*Prototype: Void trim(); 
    * Removes all whitespace around point.
    * See also: skip_chars, skip_white
    */
   MAKE_INTRINSIC(".pop2buf", pop_to_buffer, VOID_TYPE, 1),
   /*Prototype: Void pop2buf (String buf);
    * Pop up a window containing a buffer named 'buf'. If 'buf' does not exist,
    * it will be created. If 'buf' already exists in a window, the window containing
    * 'buf' will be the active one.  This function will create a new window
    * if necessary.
    * See also: pop2buf_whatbuf, setbuf, sw2buf */
   MAKE_INTRINSIC(".pop2buf_whatbuf", pop_to_buffer, STRING_TYPE, 1),
   /*Prototype: String pop2buf_whatbuf (String buf);
    * This function performs the same function as 'pop2buf' except that the 
    * name of the buffer that 'buf' replaced in the window is returned.  This 
    * allows one to replace the buffer in the window with the one previously
    * there. 
    * See also: pop2buf */
   MAKE_VARIABLE(".DISPLAY_EIGHT_BIT", &Display_Eight_Bit, INT_TYPE, 0),
   /* if non zero, pass chars with hi bit set to terminal as is,
      otherwise prefix with a `~' and pass char with hi bit off. */
   MAKE_VARIABLE(".JED_CSI", &JED_CSI, INT_TYPE, 0),
   /* Control Sequence Introducer. --- reserved for future use */
   MAKE_INTRINSIC(".copy_region", copy_region_cmd, VOID_TYPE, 1),
   /*Prototype: Void copy_region (String buf);
    * Copies a marked region in the current buffer to buffer 'buf'.
    * See also: insbuf, bufsubstr */
   MAKE_INTRINSIC(".insbuf", insert_buffer_name, VOID_TYPE, 1),
   /*Prototype: Void insbuf (String buf);
    * Insert buffer named 'buf' into the current buffer at Point.
    * See also: copy_region */
   MAKE_INTRINSIC(".bolp", bolp,INT_TYPE, 0),
   /*Prototype: Integer bolp ();
    * 'bolp' is used to test if the Point is at the beginning of a line.  It 
    * returns non-zero if at the beginning of a line and 0 if not.
    * See also: bol, eolp, bobp, eobp */
   MAKE_INTRINSIC(".beep", beep, VOID_TYPE, 0),
   /* Send beep to screen. */
   MAKE_INTRINSIC(".onewindow", one_window,VOID_TYPE, 0),
   /* make current window the only one. */
   MAKE_INTRINSIC(".pop_spot", pop_spot,VOID_TYPE, 0),
   /*Prototype: Void pop_spot ();
    * This function is used after 'push_spot' to return to the location where
    * 'push_spot' was called.
    * See also: push_spot, pop_mark */
   MAKE_INTRINSIC(".push_spot", push_spot,VOID_TYPE, 0),
   /*Prototype: Void push_spot ();
    * 'push_spot' pushes the location of the current buffer location onto a 
    * stack.  This function does not set the mark.  Use push_mark for that purpose.
    * The spot can be returned to using 'pop_spot'.
    * See also: pop_spot, push_mark */
   MAKE_INTRINSIC(".bsearch", search_backward, INT_TYPE, 1),
   /*Prototype: Integer bsearch (String str);
    * Searches backward from the current Point for 'str'.  If 'str' is found, 
    * this function will return non-zero and the Point will be placed at the
    * location of the match.  If a match is not found, zero will be returned and
    * the Point will not change.
    * See also: fsearch, bol_bsearch, re_bsearch */
   MAKE_INTRINSIC(".sw2buf", switch_to_buffer_cmd, VOID_TYPE, 1),
   /* Switch to BUFFER.  If BUFFER does not exist, one is created with name
       BUFFER */
   MAKE_INTRINSIC(".tt_send", do_tt_write_string, VOID_TYPE, 1),
   /* send STRING to terminal with no interpretation */
   MAKE_INTRINSIC(".eolp", eolp,INT_TYPE, 0),
   /* Returns TRUE if Point is at the end of a line. */
   MAKE_INTRINSIC(".what_keymap", what_keymap, STRING_TYPE, 0),
   /* returns keymap name of current buffer */
   MAKE_INTRINSIC(".find_file", find_file_in_window, INT_TYPE, 1),
   /* finds FILE in current window returning non zero if file found.
       See Also: read_file */
   MAKE_INTRINSIC(".set_status_line", set_status_format, VOID_TYPE, 2),
   /*  Usage:  set_status_line(String format, Integer flag);
       If flag is non-zero, format applies to the global format string 
       otherwise it applies to current buffer only.  
       Format is a string that may contain the following format specifiers:
         %b   buffer name
	 %f   file name
	 %v   JED version
	 %t   current time --- only used if variable DISPLAY_TIME is non-zero
	 %p   line number or percent string
	 %%   literal '%' character
	 %m   mode string
	 */
   MAKE_INTRINSIC(".bury_buffer", bury_buffer, VOID_TYPE, 1),
   /*Prototype: Void bury_buffer(String name);
     Make buffer 'name' unlikley to appear in a window.  */
   
   MAKE_INTRINSIC(".dupmark", dup_mark, INT_TYPE, 0),
   /*Prototype Integer dupmark ();
     This function returns zero if the mark is not set or, if the mark is set,
     a duplicate of it is pushed and 1 is returned. */
   MAKE_INTRINSIC(".erase_buffer", erase_buffer, VOID_TYPE, 0),
   /* erases all text from the current buffer.
    See: delbuf */
   MAKE_INTRINSIC(".window_info", window_size_intrinsic, INT_TYPE, 1),
   /*Prototype Integer window_info(Integer item);
     Returns information specified by 'item' about the current window.  Here
     'item' is one of:
     
         'r'  : Number of rows
         'w'  : width of window 
	 'c'  : starting column (from 1)
	 't'  : screen line of top line of window (from 1)
    */
   MAKE_VARIABLE(".whatline", &LineNum,INT_TYPE, 1),
   /* returns current line number -- used to be a function. */
   MAKE_VARIABLE(".BLINK", &Blink_Flag, INT_TYPE, 0),
   MAKE_VARIABLE(".WRAP_INDENTS", &Indented_Text_Mode, INT_TYPE, 0),
   /* If non-zero, after wrap, line is indented as previous line. */
   MAKE_INTRINSIC(".goto_column", goto_column, VOID_TYPE, 1),
   /* Move Point to COLUMN inserting spaces and tabs if necessary. */
   MAKE_INTRINSIC(".goto_column_best_try", goto_column1, INT_TYPE, 1),
   /*Prototype: Integer goto_column_best_try (Integer c);
     This function is like goto_column except that it will not insert
     spaces.  It returns the column number is did go to. */
   /* Move Point to COLUMN inserting spaces and tabs if necessary. */
   MAKE_VARIABLE(".TAB_DEFAULT", &User_Vars.tab, INT_TYPE, 0),
   /* default tab setting applied to all newly created buffers.
      See TAB. */
   MAKE_INTRINSIC(".goto_line", goto_line,VOID_TYPE, 1),
   /* move Point to LINE. */
   MAKE_INTRINSIC(".file_status", file_status, INT_TYPE, 1),
 /* returns integer desecribing FILE:
        2 file is a directory
        1 file exists
        0 file does not exist.
       -1 no access.
       -2 path invalid
       -3 unknown error
   */
   MAKE_VARIABLE(".C_INDENT", &User_Vars.c_indent, INT_TYPE, 0),
   MAKE_INTRINSIC(".flush", flush_message, VOID_TYPE, 1),
   /* Takes 1 string argument and immediately displays it as a message 
      in the minibuffer.  It is exactly like the `message' function except
      that its effect is immediate.
      See: message, update */
      
      MAKE_VARIABLE(".IGNORE_BEEP", &tt_Ignore_Beep, INTP_TYPE, 0),
      /* If 0, do not beep the terminal.  If 1 beep.  If 2 use visible bell
       * only.  If 3 use both bells.
       */
   
      MAKE_VARIABLE(".ADD_NEWLINE", &Require_Final_Newline, INT_TYPE, 0),
   MAKE_VARIABLE(".LASTKEY", Jed_Key_Buffer, STRING_TYPE, 1),
   /* buffer containing last keysequence.  Key sequences using the
      null character will not be recorded accurately. */
   MAKE_VARIABLE(".C_BRA_NEWLINE", &C_Bra_Newline, INT_TYPE, 0),
   /* if non-zero, insert newline before inserting '{' in C mode */
   MAKE_VARIABLE(".DISPLAY_TIME", &Display_Time, INT_TYPE, 0),
   /* A non-zero value means to enable display of time whenever %t occurs
      in the status line format. */
   MAKE_VARIABLE(".WANT_EOB", &Want_Eob, INT_TYPE, 0),
   /* Set this to non zero value if it is desired to have [EOB] mark
       the end of the buffer. */
   MAKE_INTRINSIC(".input_pending", input_pending, INT_TYPE, 1),
   /* Only argument is amount of seconds/10 to wait for input.  0 returns
      right away.  Returns TRUE if there is input waiting. */
   MAKE_INTRINSIC(".insert_file",  insert_file, INT_TYPE, 1),
   /* This returns <= 0 if file not found. */
   MAKE_INTRINSIC(".keymap_p",  keymap_p, INT_TYPE, 1),
   /* Returns TRUE if KEYMAP is defined. */
   MAKE_VARIABLE(".WRAP", &User_Vars.wrap_column, INT_TYPE, 0),
   MAKE_INTRINSIC(".what_char", what_char, INT_TYPE, 0),
   /* returns ASCII value of character point is on. */
   MAKE_INTRINSIC(".bfind", backward_search_line, INT_TYPE, 1),
   /* returns TRUE if STRING found backward on current line */
   MAKE_INTRINSIC(".pop_mark", pop_mark, VOID_TYPE, 1),
   /* Pop last pushed mark off the mark stack. If argument is non zero, 
       move point to position of mark first. */
   MAKE_INTRINSIC(".read_mini", mini_read, VOID_TYPE, 3),
   /* read from minibuffer with PROMPT and DEFAULT strings using STRING
      to stuff the minibuffer.  Returns string to stack. */
   MAKE_INTRINSIC(".recenter", recenter, VOID_TYPE, 1),
   /* update window with current line on Nth line of window. 
       If N is 0, recenter */
   MAKE_INTRINSIC(".bufferp", bufferp, INT_TYPE, 1),
   /* returns  TRUE if BUFFER exists */
   MAKE_INTRINSIC(".get_key_function", get_key_function, VOID_TYPE, 0),
   /* Returns current key binding.  If key has a binding, it also 
      returns 0 if the function is S-Lang or non zero if it is internal. */
   MAKE_INTRINSIC(".dump_bindings", dump_bindings, VOID_TYPE, 1),
   /*Prototype: Void dump_bindings(String map);
     Dumps a list of the keybindings for the keymap specified by 'map'.
     */
   /* Returns current key binding.  If key has a binding, it also 
      returns 0 if the function is S-Lang or non zero if it is internal. */
   MAKE_VARIABLE(".META_CHAR", &Meta_Char, INT_TYPE, 0),
   /* When a character with the hi bit set is input, it gets mapped to
      a two character sequence, The META_CHAR, followed by the
      character with its hi bit off.  By default, META_CHAR is 27, the
      escape character. */
   MAKE_VARIABLE(".DEC_8BIT_HACK", &DEC_8Bit_Hack, INT_TYPE, 0),
   /* If set, an character between 128 and 160 will be converted into a 
      two character sequence: ESC and the character itself stripped of the
      high bit + 64.
    */
   MAKE_INTRINSIC(".undefinekey", unset_key_in_keymap, VOID_TYPE, 2),
   /* Undefines KEY from KEYMAP.
   See: make_keymap. */
   MAKE_INTRINSIC(".getpid", jed_getpid, INT_TYPE, 0),
   /*Prototype: Integer getpid();
     Returns pid of current process. */
   
   MAKE_INTRINSIC(".update", update_cmd, VOID_TYPE, 1),
   /* Update display.  If argument it TRUE, force update otherwise 
      update only if there is no input */
   MAKE_INTRINSIC(".skip_white", skip_whitespace, VOID_TYPE, 0),
   /* Skip past whitespace.  This does not cross lines.  
    See: skip_chars */
   MAKE_INTRINSIC(".skip_word_chars", skip_word_chars, VOID_TYPE, 0),
   /* skip over all characters that constitute a word. */
   MAKE_INTRINSIC(".skip_non_word_chars", skip_non_word_chars, VOID_TYPE, 0),
   /* skip over all characters that do not constitute a word. */
   MAKE_INTRINSIC(".bskip_word_chars", bskip_word_chars, VOID_TYPE, 0),
   /* skip backwards over all characters that constitute a word. */
   MAKE_INTRINSIC(".bskip_non_word_chars", bskip_non_word_chars, VOID_TYPE, 0),
   /* skip backwards over all characters that do not constitute a word. */
   MAKE_INTRINSIC(".which_key", which_key, INT_TYPE, 1),
   /* returns NUMBER of keys that are bound to argument followed
       by NUMBER keys.  Control Chars are expanded as 2 chars. */
   MAKE_INTRINSIC(".whitespace", insert_whitespace,VOID_TYPE, 1),
   /* inserts whitespace of length n using tabs and spaces.  If the global
    variable  TAB is 0, only spaces are used. */
   MAKE_VARIABLE(".C_BRACE", &User_Vars.c_brace, INT_TYPE, 0),
   MAKE_INTRINSIC(".enlargewin", enlarge_window,VOID_TYPE, 0),
   /* Makes the current window bigger by one line. */
   MAKE_INTRINSIC(".splitwindow", split_window,VOID_TYPE, 0),
   /* Splits current window in half making two. */
   MAKE_INTRINSIC(".file_time_compare", file_time_cmp, INT_TYPE, 2),
   /* compares the modification times of two files, FILE1 and FILE2.
      returns positive, negative, or zero integer for FILE1 > FILE2,
      FILE1 < FILE2, or FILE1 == FILE2, resp.  The operator '>' should
      be read 'is more recent than'.   The convention adopted by the routine
      is that if a file does not exist, it was modified at the beginning of
      time. Thus, if 'f' exists, but 'g' does not, f g file_time_compare
      will return 1. */
   MAKE_INTRINSIC(".xform_region", transform_region, VOID_TYPE, 1),
   /* takes a control character (integer) as argument:
         'u' upcase_region
         'd' downcase_region
         'c' Capitalize region
         anything else will change case of region */
#ifdef pc_system
#ifndef __GO32__   
   MAKE_VARIABLE(".NUMLOCK_IS_GOLD", &NumLock_Is_Gold, INT_TYPE, 0),
#ifndef __os2__   
   MAKE_VARIABLE(".CHEAP_VIDEO", &SLtt_Msdos_Cheap_Video, INTP_TYPE, 0),
   /* non zero if snow appears on screen when updating it. */
#endif
#endif
#else
   MAKE_VARIABLE(".OUTPUT_RATE", &tt_Baud_Rate, INTP_TYPE, 0),
   /* Terminal baud rate */
#endif
   MAKE_INTRINSIC(".skip_chars", skip_chars, VOID_TYPE, 1),
   /*Prototype: Void skip_chars(String s);
    * skip past all characters in string 's'.
    * s is a string which contains ascii chars to skip, or a rang of ascii 
    * chars.  So for example, "- \t0-9ai-o_" will skip the hyphen, space, tab
    * numerals 0 to 9, letter a, letters i to o, and underscore.
    * 
    * If the first character of 's' is '^', then the compliment of the range 
    * is skipped instead.  So for example,
    * 
    *     skip_chars("^A-Za-z");
    * 
    * skips ALL characters except the letters.  The backslash character may be
    * used to escape ONLY the FIRST character in the string.  That is, "\\^"
    * is to be used to skip over '^' characters.

    * See Also: bskip_chars, skip_white */
   
   MAKE_INTRINSIC(".bobp", bobp,INT_TYPE, 0),
   /* TRUE if at beginning of buffer */
   MAKE_INTRINSIC(".ffind", forward_search_line, INT_TYPE, 1),
   /* Returns TRUE if STRING is found forward current line. If found, Point
       is moved to string */
   MAKE_INTRINSIC(".bol_fsearch", bol_fsearch, INT_TYPE, 1),
   /* Search forward for string at beginning of line. Returns TRUE if found */
   MAKE_INTRINSIC(".bol_bsearch", bol_bsearch, INT_TYPE, 1),
   /* Search backward string at beginning of line. Returns TRUE if found */
   
   MAKE_INTRINSIC(".command_line_arg", command_line_argv, STRING_TYPE, 1),
   /* Takes integer parameter N in the range: 0 <= N < MAIN_ARGC.
    MAIN_ARGC is a global variable indicating the number of command line 
    parameters.  This function returns the Nth parameter.
    See Also the variable MAIN_ARGC */
   
   MAKE_VARIABLE(".MAIN_ARGC", &Main_Argc, INT_TYPE, 1),
   /* MAIN_ARGC is a global variable indicating the number of command line 
      parameters. 
      See Also: command_line_arg */
      
   
   MAKE_INTRINSIC(".set_file_translation", set_file_trans, VOID_TYPE, 1),
   /* 1 open files in binary, 0 in text (default) */

#if defined (unix) || defined (__os2__)
#ifndef __GO32__
   MAKE_INTRINSIC(".pipe_region", pipe_region, INT_TYPE, 1),
   /* pipes region to CMD returning number of lines written. */
   MAKE_INTRINSIC(".shell_cmd", shell_command, VOID_TYPE, 1),
   /* executes CMD in a subshell inserting output inter buffer at Point */
#endif
#endif
   MAKE_INTRINSIC(".mkdir", make_directory, INT_TYPE, 1),
   /* create a directory with NAME.  Returns TRUE if successful, 0 otherwise. */
   MAKE_INTRINSIC(".rmdir", delete_directory, INT_TYPE, 1),
   /* delete a directory with NAME.  Returns TRUE if successful, 0 otherwise. 
      The directory must be empty for the operation to succeed. */
   MAKE_INTRINSIC(".append_region_to_file", append_to_file, INT_TYPE, 1),
   /*Prototype: Integer append_region_to_file (String file);
    * Appends a marked region to 'file' returning number of lines written or -1
    * on error.  This does NOT modify a buffer visiting the file; however,
    * it does flag the buffer as being changed on disk. */
   MAKE_INTRINSIC(".autosave", auto_save, VOID_TYPE, 0),
   /* autosave current buffer if marked for autosave */
   MAKE_INTRINSIC(".autosaveall", auto_save_all, VOID_TYPE, 0),
   /* save all buffers marked for autosave */
   
   MAKE_INTRINSIC(".backward_paragraph", backward_paragraph, VOID_TYPE, 0),
   /* move point past current paragraph.  Slang hook is_paragraph_seperator
     is called (if defined) to determine if line is a paragraph seperator. */
   MAKE_INTRINSIC(".blank_rect", blank_rectangle, VOID_TYPE, 0),
   /* blanks out rectangle defined by point and mark */
   MAKE_INTRINSIC(".bskip_chars", bskip_chars, VOID_TYPE, 1),
   /* skip backward chars in STRING.  
      See skip_chars for definition of STRING */
   MAKE_INTRINSIC(".buffer_list", make_buffer_list,  VOID_TYPE, 0),
   /* returns a  list of buffers to the stack.  The top element of the 
    stack is the number of buffers */
   MAKE_INTRINSIC(".check_region", check_region, VOID_TYPE, 1),
   /* Signals Error if mark not set.  Exchanges point 
    and mark to produce valid region.  A valid region is one with mark
    earlier in the buffer than point.  Always call this if using a region
    which requires point > mark.  Also, if argument is non-zero, spot is 
    pushed. */
   MAKE_INTRINSIC(".copy_rect", copy_rectangle, VOID_TYPE, 0),
   /* save a copy of rectangle defined by point and mark in rectangle 
     buffer */
   MAKE_INTRINSIC(".define_word", define_word, VOID_TYPE, 1),
   /* Only argument is a string which is an expression which defines 
      a word.  Typically, it is a range of ascii values.  The default 
      definition is: "a-z0-9"
      To include a hyphen, make it the first character.  So for example,
      "-i-n" defines a word to consist of letters 'i' to 'n'  and '-' */
     
   MAKE_INTRINSIC(".delbuf", kill_buffer_cmd, VOID_TYPE, 1),
   /* deletes specified buffer name */
   MAKE_INTRINSIC(".delete_file",  sys_delete_file, INT_TYPE, 1),
   /* Deletes FILENAME.  Returns 1 if deletion was successful, otherwise
    it returns 0. */
   MAKE_INTRINSIC(".directory", expand_wildcards, INT_TYPE, 1),
   /* returns number of files and list of files which match filename. 
      On unix, this defaults to filename*.  It is primarily useful for
      DOS and VMS to expand wilcard filenames */
   MAKE_INTRINSIC(".evalbuffer", load_buffer, VOID_TYPE ,0),
   /* evaluates a buffer as S-Lang code.  See: evalfile */
   MAKE_INTRINSIC(".expand_filename", expand_filename, STRING_TYPE, 1),
   /* expands filename to a canonical form */
   MAKE_INTRINSIC(".filechgondsk", file_changed_on_disk, INT_TYPE, 1),
   /* Returns true if FILE on disk is more recent than editor file */
   MAKE_INTRINSIC(".forward_paragraph", forward_paragraph, VOID_TYPE, 0),
   /* move point past current paragraph.  Slang hook is_paragraph_seperator
     is called (if defined) to determine if line is a paragraph seperator. */
   
   MAKE_INTRINSIC(".get_doc_string", get_doc_string, INT_TYPE, 2),
   /* read doc string for OBJECT from FILE.  Returns 1 and string
       upon success or 0 on failure.  If OBJECT is a functiion, 
       it must be prefixed with an 'F'.  If it is a variable, the prefix
       character is a 'V'.  
       Example: "Fget_doc_string" */
   MAKE_INTRINSIC(".getkey", jed_getkey, INT_TYPE, 0),
   /* Read a key from input stream returning ASCII value read. */
   MAKE_INTRINSIC(".indent_line", indent_line, VOID_TYPE, 0),
   /* Indent line according to current mode. */

   MAKE_INTRINSIC(".insert_rect", insert_rectangle, VOID_TYPE, 0),
   /* insert contents of previously deleted rectangle at Point. */
   MAKE_INTRINSIC(".kill_rect", kill_rectangle, VOID_TYPE, 0),
   /* deletes rectangle defined  by point and mark.  The contents of 
       the rectangle are saved in the rectangle buffer destroying previous
       contents. */
   MAKE_INTRINSIC(".make_keymap", create_keymap, VOID_TYPE, 1),
   /* Creates a new keymap with name map.  The newly created keymap is an 
    * exact copy of the global map "global".
    * See: use_keymap, definekey, undefinekey */
   MAKE_INTRINSIC(".map_input", map_character, VOID_TYPE, 2),
   /* Used to remap input characters from the keyboard to a different 
       character before JED interprets the character.  For example,
     '8 127 map_input' will cause JED to think that the ^H (8) is the 
      delete character (127).  Note that '8 127 map_input 127 8 map_input
      effectively swaps the ^H and delete keys. */
   MAKE_INTRINSIC(".narrow", narrow_to_region, VOID_TYPE, 0),
   /* restrict editing to region of LINES defined by point and mark.  
       Use 'widen' to remove the restriction. Be careful with this because
       it currently does not remember a previous narrow. */
   MAKE_INTRINSIC(".open_rect", open_rectangle, VOID_TYPE, 0),
   /* insert a BLANK rectangle.  The rectangle  is defined by point and mark. */
   
   MAKE_INTRINSIC(".quit_jed", quit_jed, VOID_TYPE, 0),
   /* Quit JED saving no buffers, just get out! */
   
   MAKE_INTRINSIC(".read_file", find_file_cmd, INT_TYPE, 1),
   /*  read FILE into its own buffer  returning non zero if file exists.
       see find_file to read a file into a window. */
   MAKE_INTRINSIC(".read_with_completion", read_object_with_completion, VOID_TYPE, 4),
   /* Takes 4 parameters:  PROMPT(string) DEFAULT(string) STUFF(string) 
      and TYPE(integer).
      TYPE must be one of:
         'f'   file name
	 'b'   buffer name
	 'F'   function name
	 'V'   variable name.
	 
     STUFF is a string which is stuffed into the buffer.
     Using this function enables completion on the object.
     */
   MAKE_INTRINSIC(".replace", replace_cmd, VOID_TYPE, 2),
   /*Prototype Void replace(String old, String new);
     Replaces all occurances of 'old' with 'new' from current point to
     the end of the buffer. The Point is returned to the initial location. */
   MAKE_INTRINSIC(".set_abort_char", set_abort_char, VOID_TYPE, 1),
   /* Change Abort character to CHAR.  The default is 7 which is ^G.
      Using this function modifies ALL keymaps */
   MAKE_INTRINSIC(".suspend", sys_spawn_cmd, VOID_TYPE, 0),
   /* Suspend jed and return to calling process or spawn subprocess.
     "suspend_hook" is called before suspension and "resume_hook" is called
     after.  These are user defined S-Lang functions. */
   MAKE_INTRINSIC(".time", get_time, STRING_TYPE, 0),
   /* return current date and time string */
   MAKE_INTRINSIC(".ungetkey", ungetkey, VOID_TYPE, 1),
   /* push ASCII value of character on input stream */
   MAKE_INTRINSIC(".buffer_keystring", do_buffer_keystring, VOID_TYPE, 1),
   /*Prototype: Void buffer_keystring (String str);
    * Append string 'str' to the end of the input stream to be read by JED's
    * getkey routines.
    * See also: ungetkey, getkey
    */
   MAKE_INTRINSIC(".use_keymap",  use_keymap, VOID_TYPE, 1),
   /* Asscoiate KEYMAP with buffer. */
#ifndef pc_system
   MAKE_INTRINSIC(".w132", screen_w132, VOID_TYPE, 0),
   MAKE_INTRINSIC(".w80", screen_w80, VOID_TYPE, 0),
#endif
   MAKE_INTRINSIC(".whatmode", what_mode, INT_TYPE, 0),
   /* returns buffer mode string and mode flag.  See setmode for details. */
   MAKE_INTRINSIC(".widen", widen, VOID_TYPE, 0),
   /* Opposite of Narrow.  See narrow for additional information. */
   MAKE_INTRINSIC(".window_line", window_line, INT_TYPE, 0),
   /* returns number of line in window.  top line is 1. */
   MAKE_INTRINSIC(".write_buffer", write_buffer_cmd, INT_TYPE, 1),
   /* writes buffer to FILE. Returns number of lines written or signals
       error on failure. */
   MAKE_INTRINSIC(".write_region_to_file", write_region, INT_TYPE, 1),
   /* Prototype: Integer write_region_to_file (String file);
    * Write region to 'file'.  Returns number of lines written or signals
    * error on failure. 
    */
      
   MAKE_INTRINSIC(".count_chars", count_chars, VOID_TYPE, 0),
   /* returns a string of form "char 37, point 2150 of 10067"
      where 2150 is character number of Point and 10067 is the total.
      37 is the ascii value of current character.
      A string is returned instead of numbers because MSDOSints are only 
      16 bits and S-Lang does not have long integer types.
      */
   MAKE_INTRINSIC(".get_yes_no", get_yes_no, INT_TYPE, 1),
   /* Takes one argument-- a string that is used to get 
      yes or no responce from user.  Returns 1 if yes, 0 if no. 
      Also returns -1 if abort and signals error. */
   MAKE_INTRINSIC(".rename_file", rename_file, INT_TYPE, 2),
   /* rename file from OLD_NAME to NEW_NAME returning 0 if the 
      operation succeeds, and a non-zero value if it fails. 
      Both files must be on the same file system. */
   MAKE_INTRINSIC(".change_default_dir", ch_dir, INT_TYPE, 1),
   /*  Change default directory to new directory.  Returns 0 upon
      success and -1 upon failure.  All relative path names are expanded
      with respect to the new default directory. */
   MAKE_INTRINSIC(".prefix_argument",  do_prefix_argument, INT_TYPE, 1),
   /* Usage:  int prefix_argument(int default);
         Returns value of prefix argument if there is one otherwise
	 returns 'default'. */
   MAKE_INTRINSIC(".regexp_nth_match", regexp_nth_match, VOID_TYPE, 1),
   MAKE_INTRINSIC(".replace_match", replace_match, INT_TYPE, 2),
   /*Prototype: Integer replace_match(String s, Integer how);
    * This function replaces text previously matched with `re_fsearch' or
    * `re_bsearch' at the current editing point with string 's'.  If 'how' is
    * zero, 's' is a specially formatted string of the form described below.
    * If 'how' is non-zero, 's' is regarded as a simple string and is used
    * literally.  If the replacement fails, this function returns zero
    * otherwise, it returns non-zero.
    */
   MAKE_INTRINSIC(".re_fsearch", re_search_forward, INT_TYPE, 1),
   /*Prototype: Integer re_fsearch(String pattern);
    * Search forward for regular expression 'pattern'.  This function returns
    * the 1 + length of the string  matched.  If no match is found, it returns 
    * 0.
    * 
    * See also: fsearch, bol_fsearch, re_bsearch */
   MAKE_INTRINSIC(".re_bsearch", re_search_backward, INT_TYPE, 1),
   /*Prototype: Integer re_bsearch(String pattern);
    * Search backward for regular expression 'pattern'.  This function returns
    * the 1 + length of the string  matched.  If no match is found, it returns 
    * 0.
    * 
    * See also: bsearch, bol_bsearch, re_fsearch */

   MAKE_INTRINSIC(".set_buffer_hook", set_buffer_hook, VOID_TYPE, 2),
   /*Prototype: Void set_buffer_hook (String hook, String f);
    * Set current buffer hook 'hook' to function 'f'. 'f' is a user
    * defined S-Lang function.  Currently, name can be any one of:
    *   "par_sep"  -- returns zero if the current line does not
    *                 constitute the beginning or end of a paragraph.  
    *                 It returns non-zero otherwise.  The default value of 'hook' is 
    *                 'is_paragraph_separator'.
    *   "indent_hook" -- returns nothing.  It is called by the indent line
    *                 routines.
    *   "wrap_hook"   hook that is called after a line is wrapped.  Returns
    *                 nothing
    */
   MAKE_INTRINSIC(".insert_file_region", insert_file_region, INT_TYPE, 3),
   MAKE_INTRINSIC(".search_file", search_file, INT_TYPE, 3),
   /* search FILE for STRING returning TRUE if string found. */
   MAKE_INTRINSIC(".random", make_random_number, INT_TYPE, 2),
   /* Usage:  seed n random
      Returns a random number in the range 0 to n - 1.  If seed is 0, the 
      number generated depends on previous seed.  If seed is -1, a seed based 
      on current time and pid is used, otherwise, seed is used as a seed. */
#ifndef pc_system
   MAKE_INTRINSIC(".set_term_vtxxx", do_tt_set_term_vtxxx, VOID_TYPE, 1),
   /* Set terminal display appropriate for a vtxxx terminal.  This function 
    * takes a single integer parameter.  If non-zero, the terminal type is set 
    * for a vt100.  This means the terminal lacks the ability to insert/delete
    * lines and characters.  If the parameter is zero, the terminal is assumed
    * to be vt102 compatable.  Unless you are using a VERY old terminal or 
    * a primitive emulator, use zero as the parameter. */
    MAKE_VARIABLE(".TERM_CANNOT_INSERT", &tt_Term_Cannot_Insert, INTP_TYPE, 0),
   /* Set this variable to 1 in your jed startup file (jed.rc) if your 
    terminal is unable to insert (not vt102 compatable) */
   MAKE_VARIABLE(".TERM_CANNOT_SCROLL", &tt_Term_Cannot_Scroll, INTP_TYPE, 0),
   /* Set this variable to 1 in your jed startup file (jed.rc) if your 
    terminal is unable to scroll. */
#endif
   
   MAKE_VARIABLE(".BATCH", &Batch, INT_TYPE, 1),
   /* non-zero if JED is running in batch mode.  This variable
       is read only. */
   MAKE_VARIABLE(".TAB", &Buffer_Local.tab, INT_TYPE, 0),
   /* Tab setting for the current buffer. */
   MAKE_VARIABLE(".SELECTIVE_DISPLAY", &Buffer_Local.sd, INT_TYPE, 0),
   /* If negative, ^M (RET) makes rest of line invisible.  Hidden 
      text is indicated by '...'.  */
   MAKE_VARIABLE(".LAST_CHAR", &SLang_Last_Key_Char, INT_TYPE, 0),
   /* Last character entered from the keyboard */
   MAKE_VARIABLE(".MAX_HITS", &User_Vars.max_hits, INT_TYPE, 0),
   /* maximum number of 'hits' on a buffer before an autosave is performed. */
   MAKE_VARIABLE(".CASE_SEARCH", &Case_Sensitive, INT_TYPE, 0),
   /* if 1, searches are case sensitive.  If 0, they are not */
   MAKE_VARIABLE(".POINT", &Point, INT_TYPE, 0),
   MAKE_VARIABLE(".MESSAGE_BUFFER", Message_Buffer, STRING_TYPE, 1),
   /* Read only string indicating current message to be displayed or
      is displayed in the message buffer */
   MAKE_VARIABLE(".IGNORE_USER_ABORT", &Ignore_User_Abort, INT_TYPE, 0),
   /* If set to a non-zero value, the Abort Character will not trigger a
      S-Lang error.  When JED starts up, this value is set to 1 so that 
      the user cannot abort the loading of site.sl.  Later, it is set to 0 */


   MAKE_VARIABLE(".KILL_LINE_FEATURE", &Kill_Line_Feature, INT_TYPE, 0),
   /* If non-zero, kill_line will kill through end of line character if
    Point is at beginning of the line.  Otherwise, it will kill only until
    the end of the line.  By default, this feature is turned on. */
   MAKE_VARIABLE(".SCREEN_HEIGHT", &tt_Screen_Rows, INTP_TYPE, 1),
   /* number of rows on the screen. */
   MAKE_VARIABLE(".SCREEN_WIDTH", &tt_Screen_Cols, INTP_TYPE, 1),
   /* number of columns on the screen */
   MAKE_VARIABLE(".JED_LIBRARY", Jed_Library, STRING_TYPE, 1),
   /* Read only string variable indicating the directory where JED library
    files are kept.  This variable may be set using an environment variable */
   MAKE_VARIABLE(".JED_ROOT", Jed_Root_Dir, STRING_TYPE, 1),
   /* Read only string variable indicating JED's root directory.
      This variable may be set using an environment variable */
   MAKE_VARIABLE(".LINENUMBERS", &User_Prefers_Line_Numbers, INT_TYPE, 0),
   /* If set to 0, line numbers are not displayed on the screen. If set to 
      1, line numbers will be displayed.  If set to anything else, the 
      %c column format specifier will be parsed allowing the column number
      to be displayed on the screen. */
#ifdef pc_system
   MAKE_VARIABLE(".ALT_CHAR", &PC_Alt_Char, INT_TYPE, 0),
   /*  If this variable is non-zero, characters pressed in combination
     the ALT key will generate a two character sequence: the first character 
     is the value of the ALT_CHAR itself followed by the character pressed. 
     For example, if ALT-X is pressed and ALT_CHAR has a value of 27, the 
     characters ESCAPE X will be generated. */
#endif

#ifdef HAS_MOUSE
    MAKE_VARIABLE(".MOUSE_X", &JMouse.x, INT_TYPE, 1),
    MAKE_VARIABLE(".MOUSE_Y", &JMouse.y, INT_TYPE, 1),
    MAKE_VARIABLE(".MOUSE_DELTA_TIME", &JMouse.time, INT_TYPE, 1),
    MAKE_VARIABLE(".MOUSE_BUTTON", &JMouse.button, INT_TYPE, 1),
    MAKE_VARIABLE(".MOUSE_EVENT_TYPE", &JMouse.type, INT_TYPE, 1),
    /* Value is 1 if the event is a keypress and 0 if a release. */
    MAKE_VARIABLE(".MOUSE_STATE", &JMouse.state, INT_TYPE, 1),
    /* The state of the shift, control, and button keys BEFORE the event.
     * This is an integer with the following meaning for the bits:
     *	   0: left button pressed
     *	   1: middle button pressed
     *	   2: right button pressed
     *	   3: shift key pressed
     *	   4: control key pressed */
#endif

   MAKE_VARIABLE(".HIGHLIGHT", &Wants_Attributes, INT_TYPE, 0),
   /* Set this variable non-zero to highlight marked regions */
   MAKE_VARIABLE(".HORIZONTAL_PAN", &Wants_HScroll, INT_TYPE, 0),
   /* If this variable is non-zero, the window pans with the Point.  Actually
    * if the value is less than zero, the entire window pans.  If the value is 
    * positive, only the current line will pan.  The absolute value of the 
    * number determines the panning increment.  */
#ifdef VMS
   MAKE_INTRINSIC(".vms_get_help", vms_get_help,VOID_TYPE, 2),
   MAKE_INTRINSIC(".vms_send_mail", vms_send_mail, INT_TYPE, 2),
   /* Takes 2 string arguments: TO and SUBJECT.  
    *  TO may be a comma separated list of names.  The buffer will be
    *  mailed to names on this list with SUBJECT.  This routine uses 
    *  callable VMS mail.
    */
#endif
#ifdef unix
#ifndef __GO32__
   MAKE_INTRINSIC(".enable_flow_control", enable_flow_control, VOID_TYPE, 1),
   /* If integer argument is non-zero, ^S/^Q flow control is enabled.  If
    * argument is 0, ^S/^Q processing by the terminal is disabled.  Emacs mode
    * sets this to 0 (flow control off). */
#endif
#endif
   MAKE_INTRINSIC(".core_dump", exit_error_cmd, VOID_TYPE, 2),
   /*Prototype: Void core_dump(String msg, Integer severity);
    * Exit editor sumping the state of some crucial variables. If severity is 1, 
    * dump core if possible.  Message msg is also displayed. */
   MAKE_INTRINSIC(".get_last_macro", get_last_macro, VOID_TYPE, 0),
   /* Prototype: String get_last_macro();
    * This function returns characters composing the last keyboard macro.  The
    * charactors that make up the macro are encoded as themselves except the
    * following characters:
    * 
    *	 '\n'    ---->   \J
    *	 null    ---->   \@
    *	  \      ---->   \\
    *	  '"'    ---->   \"
    */
#ifdef __os2__
      MAKE_INTRINSIC(".IsHPFSFileSystem", IsHPFSFileSystem, INT_TYPE, 1),
      /*Prototype: Integer IsHPFSFileSystem(String path);
        Returns TRUE if drive of 'path' (possibly the default drive) is HPFS. */
#endif
#ifdef pc_system
      MAKE_INTRINSIC(".msdos_fixup_dirspec", msdos_pinhead_fix_dir, STRING_TYPE, 1),
      /*Prototype: String msdos_fixup_dirspec (String dir);
       * The motivation behind this is that DOS does not like a trailing
       * backslash '\\' except if it is for the root dir.  This function makes
       * 'dir' conform to that */
#endif
   MAKE_VARIABLE(".C_COMMENT_HINT", &C_Comment_Hint, INT_TYPE, 1),
   MAKE_VARIABLE(".WANT_SYNTAX_HIGHLIGHT", &Wants_Syntax_Highlight, INT_TYPE, 0),
   MAKE_INTRINSIC(".set_top_status_line", define_top_screen_line, VOID_TYPE, 1),
   /*Prototype: String set_top_status_line (String str);
    * This functions sets the string to be displayed at the top of the 
    * display. It returns the value of the line that was previously 
    * displayed.
    * 
    * See also: enable_top_status_line
    */
   MAKE_INTRINSIC(".enable_top_status_line", enable_menu_bar, VOID_TYPE, 1),
   /*Prototype: Void enable_top_status_line (Integer x);
    * If x is non-zero, the top status line is enabled.  If x is zero, the
    * top status line is disabled and hidden.
    * See also: set_top_status_line
    */
   MAKE_VARIABLE(".TOP_WINDOW_ROW", &Top_Window_Row, INT_TYPE, 1),
   /* This read-only variable gives the value of the starting row of the top
    * window.  
    */
   MAKE_VARIABLE(".DEFINING_MACRO", &Defining_Keyboard_Macro, INT_TYPE, 1),
   MAKE_VARIABLE(".EXECUTING_MACRO", &Executing_Keyboard_Macro, INT_TYPE, 1),
#if 0
   MAKE_INTRINSIC(".matching", new_find_matching, INT_TYPE, 2),
#endif
   MAKE_INTRINSIC(".create_user_mark", create_user_mark, VOID_TYPE, 0),
   MAKE_INTRINSIC(".goto_user_mark", goto_user_mark, VOID_TYPE, 0),
   MAKE_INTRINSIC(".user_mark_buffer", user_mark_buffer, STRING_TYPE, 0),

#ifdef HAS_ABBREVS
   MAKE_INTRINSIC(".use_abbrev_table", use_abbrev_table, VOID_TYPE, 1),
   MAKE_INTRINSIC(".create_abbrev_table", create_abbrev_table, VOID_TYPE, 2),
    /* Prototype: Void create_abbrev_table (String name, String word);
     */
   MAKE_INTRINSIC(".define_abbrev", define_abbrev, VOID_TYPE, 3),
   MAKE_INTRINSIC(".abbrev_table_p", abbrev_table_p, INT_TYPE, 1),
   MAKE_INTRINSIC(".dump_abbrev_table", dump_abbrev_table, VOID_TYPE, 1),
   MAKE_INTRINSIC(".what_abbrev_table", what_abbrev_table, STRING_TYPE, 1),

#endif
   SLANG_END_TABLE
};

int init_jed_intrinsics()
{
   int ret;
   ret = SLang_add_table(jed_intrinsics, "Jed");
#ifdef pc_system
   
   /* overload default slang version of system */
   SLadd_name ("system", (long) sys_System, SLANG_INTRINSIC, SLANG_MAKE_ARGS(INT_TYPE, 1));
#endif
   return ret;
}


