%_traceback = 1;
% 
% 		      Site specific initialiation file.
% 
% This file must be present in the JED_LIBRARY.  JED loads it first--- even
% before reading command line arguments.  The command line arguments are then
% passed to a hook declared in this file for further processing.
% 
% In addition to some hooks, this file declares some autoloads for various
% functions and defines utility functions.  Any user specific stuff should be
% placed in the jed.rc (.jedrc) user startup file.  Only put here what you
% believe EVERY user on your system should get!
% 
% The best way to make changes in this file is to put all your changes in a
% separate file, defaults.sl.  defaults.sl is NOT distributed with JED.  Code
% at the edn of this file checks for the existence of `defaults.sl' and loads
% it if found. Functions occuring in this file (site.sl) may be overloaded in
% defaults.sl. Making changes this way also makes it easier to upgrade to
% future JED versions.
% 


%!% A function to contat a directory with a filename.  Basically checks
%!% for the final slash on the dir and adds on if necessary
define dircat(dir, file)
{
   variable n = strlen(dir);
   
#ifdef MSDOS OS2
   variable slash = "\\";
   if (n)
     {
	if (strcmp(substr(dir, n, 1), slash)) dir = strcat(dir, slash);
     }
   strcat(dir, file);
#endif
#ifdef UNIX
   variable slash = "/";
   if (n)
     {
	if (strcmp(substr(dir, n, 1), slash)) dir = strcat(dir, slash);
     }
   strcat(dir, file);
#endif
#ifdef VMS
   % convert a.dir;1 to [.a] first
   variable f1, d1;
   dir = extract_element(dir, 0, ';');
   f1 = extract_element(dir, 1, ']');
   if (strlen(f1)) f1 = strcat(".", extract_element(f1, 0, '.'));
   d1 = extract_element(dir, 0, ']');
   strcat(d1, f1);
   if (':' != int(substr(dir, strlen(dir), 1))) strcat((), "]");
   strcat ((), file);
#endif
   expand_filename(());
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%     Global Variables
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
variable Null_String = "";

%!% A Comma separated list of info directories to search.
variable Info_Directory;
variable Jed_Bin_Dir;


%!% Prototype: Void strncat (String a, String b, ..., Integer n);
%!% Returns concatenated string "abc..."
define strncat (n)
{
   n--;
   loop (n) strcat ();
}


%% Convert Unix- or OS/2- style path to comma-delimited list
define path2list(path)
{
#ifdef VMS
    path;
#else
    variable n, pathsep, a_path;
 
#ifdef UNIX
    pathsep = ':';
#else
    pathsep = ';';
#endif
 
    n = 0; Null_String; Null_String;
    while (a_path = extract_element(path, n, pathsep), strlen(a_path))
      {
	 strncat ((), (), a_path, 3);
	 % strcat((), strcat((), a_path));
 	","; n++;
      }
    pop();
#endif
}

#ifdef VMS
   Info_Directory = strcat(JED_ROOT, "[info]");
   Jed_Bin_Dir = strcat(JED_ROOT, "[bin]");
#else
   Info_Directory = dircat(JED_ROOT, "info");
   Jed_Bin_Dir = dircat(JED_ROOT, "bin");
#endif
 
$1 = getenv("INFOPATH");
if (strlen($1)) Info_Directory = path2list($1);
  
%!% Column to begin a C comment--- used by c_make_comment
variable C_Comment_Column = 40;


#ifdef UNIX
 if (OUTPUT_RATE > 9600) OUTPUT_RATE = 0;   %% coming through a network?
#endif

%%
%% Some key definitions
%%
% These two are for compatability:
  setkey("search_forward", "^Ff");
  setkey("search_backward", "^Fb");
#ifdef OS2
   setkey("bob",  "^@\x84");
   setkey("eob", "^@\x76");
   setkey("skip_word", "^@\x74");  % C-left arrow
   setkey("bskip_word", "^@\x73"); % C-right arrow
#endif
  setkey("skip_word", "^[^[[C");  %escape right arrow.
  setkey("bskip_word", "^[^[[D");  %escape left arrow
  setkey("upcase_word", "^[U");
  setkey("downcase_word", "^[L");
  setkey("capitalize_word", "^[C");
  setkey("emacs_escape_x", "^[X");
  setkey("undo", "^Xu");  %% Also ^_ but vtxxx have problems with it
  setkey("transpose_lines", "^X^T");
  setkey("help_prefix", "^H");
  setkey("c_make_comment", "^[;");
  setkey("indent_line_cmd", "^I");
  setkey("insert_colon_cmd", ":");
  setkey("newline_and_indent_cmd", "^M");
  setkey("do_shell_cmd", "^[!");
  setkey("find_tag", "^[.");
  setkey("dabbrev", "\e/");
  setkey("save_buffers", "^Xs");
  setkey("whatpos", "^X?");
  setkey("list_buffers", "^X^B");
  setkey ("set_fill_column", "^Xf");
#ifdef UNIX OS2
  setkey("ispell", "^[$");
#endif
#ifndef MSDOS OS2
  setkey("mail", "^Xm");
#endif

%%
%%  Autoloads
%%

  autoload("find_binary_file",		"binary");
  autoload("jed_easy_help",		"jedhelp");
  autoload("query_replace_match",	"regexp");
  autoload("re_search_forward",		"regexp");
  autoload("re_search_backward",	"regexp");
  autoload("c_make_comment",		"cmisc");
  autoload("c_indent_line",		"indent");
  autoload("c_colon_indent_line",	"indent");
  autoload("dired",			"dired");
  autoload("calendar",			"cal");
  autoload("menu_main_cmds",		"menu");
  autoload("trim_buffer",		"util");  %% trims excess lines and space
  autoload("occur",			"util");  %% find all command
  autoload("info_mode",			"info");
  autoload("info_find_node",		"info");
  autoload("list_buffers",		"buf");
  autoload("append_region",		"buf");
  autoload("write_region",		"buf");
  autoload("recover_file",		"buf");
  autoload("most_mode",			"most");
  autoload("run_most",			"most");
  autoload("compile",			"compile");
  autoload("sort",			"sort");
  autoload("untab",			"untab");
  autoload("fortran_mode",		"fortran");
  autoload("save_buffers",		"buf");
  autoload("rot13",			"rot13");
  autoload("search_forward",		"search");
  autoload("search_backward",		"search");
  autoload("replace_cmd",		"search");
  autoload("replace_maybe_query",	"search");
  autoload("replace_across_buffer_files","replace");
  autoload("isearch_forward",		"isearch");
  autoload("isearch_backward",		"isearch");
  autoload("shell",			"shell");
  autoload("mute_set_mute_keys",	"mutekeys");
#ifndef UNIX OS2
  autoload("shell_cmd",			"shell");
#endif
  autoload("do_shell_cmd",		"shell");
  autoload("find_tag",			"ctags");
  autoload("apropos",			"help");
  autoload("expand_keystring",		"help");
  autoload("describe_bindings",		"help");
  autoload("describe_function",		"help");
  autoload("describe_variable",		"help");
  autoload("help_for_function",		"help");
  autoload("where_is",			"help");
  autoload("showkey",			"help");
  autoload("describe_mode",		"help");
  autoload("format_paragraph_hook",	"tmisc");
  autoload("dabbrev",			"dabbrev");
  autoload("tex_mode",			"tex");
  autoload("bkmrk_goto_mark",           "bookmark");
  autoload("bkmrk_set_mark",            "bookmark");

%%
%% By default, tabs are every TAB columns (default 8).  Calling this function
%% will allow the user to set the tabs arbitrarily and bind the TAB key
%% appropriately.
  autoload("edit_tab_stops",		"tabs");
  autoload("tab_to_tab_stop",		"tabs");
  autoload("append_string_to_file",	"misc");   
  autoload("write_string_to_file",	"misc");   
  autoload("make_tmp_buffer_name",	"misc");
  autoload("toggle_abbrev",		"abbrev");
  autoload("define_global_abbrev",	"abbrev");
  autoload("define_local_abbrev",	"abbrev");

#ifdef UNIX VMS
  autoload("mail",			"mail");
  autoload("mail_format_buffer",	"mail");
  autoload("dcl_mode",			"dcl");
#endif

#ifdef UNIX OS2
  autoload("unix_man",			"man");
  autoload("ispell",			"ispell");
#endif
#ifdef UNIX
  autoload("rmail",			"rmail");
#endif

#ifdef VMS
  autoload("vms_help",			"vmshelp");
#endif

% Utility functions

%!% Prototype: Void go_up (Integer n);
%!% Move up 'n' lines.
%!% See also: up, go_down
define go_up(n) { pop(up(n)); }

%!% Prototype: Void go_down (Integer n);
%!% Move down 'n' lines.
%!% See also: go_up, down
define go_down(n) { pop(down(n));}

%!% Prototype: Void go_left (Integer n);
%!% Move backward 'n' characters.
%!% See also: left, go_right
define go_left(n) { pop(left(n));}

%!% Prototype: Void go_right (Integer n);
%!% Move forward 'n' characters.
%!% See also: right, go_left
define go_right(n) { pop(right(n));}

%% emacs-like escape-x function
define emacs_escape_x()
{
   variable f = Null_String, i = 0;
  
   if (MINIBUFFER_ACTIVE)
     {
	call("evaluate_cmd");
	return;
     }
   
   forever
     {
	if (is_internal(f)) 
	  {
	     call(f);
	     return;
	  }
	
	if (is_defined(f))
	  {
	     eval(f);
	     return;
	  }
	
	if (i == 1) ungetkey(13);
	ungetkey(' ');
	++i;
	f = read_with_completion("M-x", Null_String, f, 'F')
     } 
}

define goto_line_cmd()
{
   read_mini("Goto line:", Null_String, Null_String);
   goto_line(integer(()));
}

define goto_column_cmd()
{
   read_mini("Goto Column:", Null_String, Null_String);
   goto_column(integer(()));
}

%!% Prototype: Void runhooks (String fun)
%!% if S-Lang function 'fun' is defined, execute it.  It does nothing if 'fun'
%!% does not exist.
define runhooks(fun)
{
   if (is_defined (fun)) eval(fun);
}

%!% Prototype: Void local_setkey (String fun, String key);
%!% This function is like 'setkey' but unlike 'setkey' which operates on the
%!% global keymap, 'local_setkey' operates on the current keymap which may or
%!% may not be the global one.
%!% See also: setkey, definekey, local_unsetkey
define local_setkey(f, key)
{
   definekey(f, key, what_keymap());
}

%!% Prototype: Void local_unsetkey (String key);
%!% This function is like 'unsetkey' but unlike 'unsetkey' which unsets a key
%!% from the global keymap, 'local_unsetkey' operates on the current keymap
%!% which may or may not be the global one.
%!% See also: unsetkey, undefinekey, local_setkey
define local_unsetkey(key)
{
   undefinekey(key, what_keymap());
}

%!% insert a character into a buffer.
%!% This function should be called instead of 'insert' when it is desired to
%!% insert a 1 character string.  Unlike 'insert', insert_char takes an integer
%!% argument.  For example, 
%!%    'x' insert_char
%!% and 
%!%    "x" insert
%!% are functionally equivalent but insert_char is more memory efficient.
define insert_char(ch) 
{ 
   insert (char(ch));
}


%!% Prototype: Void newline (Void);
%!% insert a newline in the buffer at point.
%!% See also: insert, insert_char
define newline () 
{ 
   insert_char('\n');
}

   
%!% insert a single space into the buffer.
define insert_single_space ()
{
   insert_char(' ');
}

%!% Prototype: Integer looking_at_char (Integer ch);
%!% This function returns non-zero if the character at the current editing
%!% point is 'ch' otherwise it retuns zero.  This function performs a case 
%!% sensitive comparison.
define looking_at_char(ch)
{
   what_char() == ch;
}


%!% returns type of file.  e.g., /usr/a.b/file.c --> c
define file_type(file)
{
   variable n;
   file = extract_filename(file);
   
   n = is_substr(file, ".");
   !if (n) return (Null_String);
   
   substr(file, n + 1, strlen(file));
}



%;; scroll other window macros-- bind them yourself
define next_wind_up()
{
   otherwindow();  call("page_up");
   loop (nwindows() - 1) otherwindow();
}

define next_wind_dn()
{
   otherwindow();  call("page_down");
   loop (nwindows() - 1) otherwindow();
}

%!% Mode dedicated to facilitate the editing of C language files.  Functions
%!% that affect this mode include:
%!%
%!%   function:             default binding:
%!%   brace_bra_cmd               {
%!%   brace_ket_cmd               }
%!%   newline_and_indent          RETURN
%!%   indent_line_cmd             TAB
%!%   goto_match                  ^\
%!%   c_make_comment              ESC ;
%!%
%!%  Variables affecting indentation include:
%!%
%!%   C_INDENT
%!%   C_BRACE
%!%   C_Comment_Column  --- used by c_make_comment
define c_mode()
{
   setmode("C", 2);
   use_keymap("global");
   runhooks("c_mode_hook");
}

define slang_mode()
{
   setmode("SL", 2 | 8);
   use_keymap("global");
   runhooks("slang_mode_hook");
}

%!%  Mode for indenting and wrapping text
%!%  Functions that affect this mode include:
%!%
%!%    Function:                     Default Binding:
%!%      indent_line_cmd                 TAB
%!%      newline_and_indent_cmd          RETURN
%!%      format_paragraph                ESC Q
%!%      narrow_paragraph                ESC N
%!%
%!%  Variables include:
%!%      WRAP_INDENTS, WRAP
%!%      TAB, TAB_DEFAULT
define text_mode()
{
   setmode("Text", 1);
   use_keymap("global");
   runhooks ("text_mode_hook");
}

%!%  Generic mode not designed for anything in particular.
%!%  See:  text_mode, c_mode
define no_mode()
{
   setmode(Null_String, 0);
   use_keymap("global");
}

% Function prototypes
% These 'functions' are only here to initialize function pointers.
define _function_pop_0 (x) {0}

%!% called from mode_hook.  Returns 0 if it is desired that control return
%!% to mode_hook or 1 if mode hook should exit after calling mode_hook_ptr
variable mode_hook_pointer = &_function_pop_0;

variable Default_Mode = &text_mode;



% Emacs allows a mode definition on the first line of a file
% -*- mode: MODENAME; VAR: VALUE; ... -*-
% which can also include values of local variables 

%!% check first line for the simplest Emacs mode statement
%!% -*- modename -*-
define modeline_hook()
{
   variable mode = Null_String;
   push_spot(); bob();
   
   if (looking_at("#!/")) mode = "sh";	% #!/bin/sh, #!/usr/local/bin/perl, ...
       
   push_mark (); narrow ();
   if (re_fsearch ("-\\*- *\\([A-Za-z]+\\) *-\\*-"))
     mode = strlow (regexp_nth_match (1));
   
   widen (); pop_spot ();		% restore place
   
   if ( strlen(mode) )
     {
	mode = strcat (mode, "_mode" );   
	if ( is_defined(mode) )
	  {
	     eval (mode);
	     return 1;
	  }
     }
   0;
}

variable Mode_List_Exts = "c,h,cc,cpp,sl,txt,doc,tex,f,for";
variable Mode_List_Modes = "c,c,c,c,slang,text,text,tex,fortran,fortran";

#ifdef MSDOS OS2
Mode_List_Exts = strcat (Mode_List_Exts, ",rc");     %  resource file
Mode_List_Modes = strcat (Mode_List_Modes, ",c");
#endif

#ifdef VMS UNIX
Mode_List_Exts = strcat (Mode_List_Exts, ",com");     %  resource file
Mode_List_Modes = strcat (Mode_List_Modes, ",dcl");
#endif

#ifdef UNIX
Mode_List_Exts = strcat (Mode_List_Exts, ",cshrc,tcshrc,login,profile");     %  resource file
Mode_List_Modes = strcat (Mode_List_Modes, ",no,no,no,no");
#endif


%!% Prototype: Void add_mode_for_extension (String mode, String ext);
%!% This function modifies Mode_List in such a way that when a file with 
%!% filename extension `ext' is read in, function strcat (mode, "_mode")
%!% will be called to set the mode.   That is, the first parameter 'mode' 
%!% is the name of a mode without the '_mode' added to the end of it.
define add_mode_for_extension (mode, ext)
{
   variable comma = ",";
   
   Mode_List_Modes = strncat (mode, comma, Mode_List_Modes, 3);
   Mode_List_Exts = strncat (ext, comma, Mode_List_Exts, 3);
}

%!% This is a hook called by find_file routines to set the mode
%!% for the buffer. This function takes one parameter, the filename extension
%!% and returns nothing.
define mode_hook (ext)
{
   variable n, mode;
#ifdef VMS
   ext = extract_element(ext, 0, ';');
#endif
   
#ifdef MSDOS OS2 VMS
   ext = strlow (ext);
#endif
   
   if (mode_hook_pointer(ext)) return;

   if (modeline_hook ()) return;
   
   n = is_list_element (Mode_List_Exts, ext, ',');
   if (n)
     {
	n--;
	mode = strcat (extract_element (Mode_List_Modes, n, ','), "_mode");
	if (is_defined(mode)) 
	  {
	     eval (mode);
	     return;
	  }
     }
   Default_Mode ();
}

define is_c_mode ()
{
   variable mode;
   (, mode) = whatmode ();
   mode & 2;
}

define indent_line_cmd ()
{
   indent_line ();
   if (is_c_mode) c_indent_line ();
}

define insert_colon_cmd ()
{
   insert_char (':');
   if (is_c_mode ())
   {
      c_colon_indent_line (); pop ();
   }
}


define newline_and_indent_cmd ()
{
   % Note that the 'call' is necessary here so that exit_min is called
   % if this is called from mini buffer at startup.
   call ("newline");
   indent_line_cmd ();
   if (is_c_mode() and C_COMMENT_HINT) 
     {
	if (bolp ()) insert_single_space ();
	insert ("* ");
     }
}


%!% sets buf modified flag. If argument is 1, mark
%!% buffer as modified.  If argument is 0, mark buffer as unchanged.
define set_buffer_modified_flag(modif)
{
   getbuf_info();
   if (modif) () | 1; else () & ~(1);
   setbuf_info(());
}
%!%  returns non-zero if the buffer modified flag is set.  It returns zero
%!%  if the buffer modified flag is not been set.  This works on the 
%!%  current buffer.  See also 'set_buffer_modified_flag'.
define buffer_modified ()
{
   variable flags;
   (, , , flags) = getbuf_info ();
   flags & 1;
}

%!% set undo mode for buffer.  If argument is 1, undo is on.  0 turns it off
define set_buffer_undo(modif)
{
   getbuf_info();
   if (modif) () | 32; else () & ~(32);
   setbuf_info(());
}


%!% Takes 1 parameter: 0 turn off readonly
%!%                    1 turn on readonly
define set_readonly(n)
{
   getbuf_info();
   if (n) () | 8; else () & ~(8);
   setbuf_info(());
}

%!% Takes 1 parameter: 0 turn off overwrite
%!%                    1 turn on overwrite
define set_overwrite(n)
{
   getbuf_info();
   if (n) () | 16; else () & ~(16);
   setbuf_info(());
}
   

define toggle_crmode ()
{
   setbuf_info(getbuf_info() xor 0x400);
   set_buffer_modified_flag (1);
}

define toggle_readonly()
{
   setbuf_info(getbuf_info() xor 8);
}
define toggle_overwrite()
{
   setbuf_info(getbuf_info() xor 16);
}

#ifdef MSDOS OS2
setkey("toggle_overwrite", "^@R");     %/* insert key */
#endif

define toggle_undo()
{
   setbuf_info(getbuf_info() xor 32);
}

define double_line()
{
   POINT;
   bol(); push_mark(); eol(); 
   bufsubstr(); 
   newline();
   insert(());
   =POINT;
}

define transpose_lines()
{
   variable line;
   bol(); push_mark(); push_mark(); eol(); 
   line = bufsubstr();
   go_right(1);
   del_region();
   go_up(1); bol();
   insert(line);
   newline();
   go_down (1);			       %  goes to bol
}




%!% string to display at bottom of screen upon JED startup and when
%!% user executes the help function.
variable help_for_help_string;

help_for_help_string =
#ifdef VMS
 "^H -> Help:^H  Menu:?  Info:I  Apropos:A  Key:K  Where:W  Fnct:F  VMSHELP:M  Var:V";
#endif
#ifdef MSDOS
 "^H -> Help:^H  Menu:?  Info:I  Apropos:A  Key:K  Where:W  Fnct:F  Var:V  Mem:M";
#endif
#ifdef UNIX OS2
 "^H -> Help:^H  Menu:?  Info:I  Apropos:A  Key:K  Where:W  Fnct:F  Var:V  Man:M";
#endif

% Load minibuffer routines now before any files are loaded.
% This will reduce fragmentation on PC.
!if (BATCH) {evalfile("mini"); pop();}


%for compatability with older versions
define read_file_from_mini(prompt)
{
   read_with_completion(prompt, Null_String, Null_String, 'f');
}

%!% Search for FILE in directories specified by JED_LIBRARY returning
%!% expanded pathname if found or the Null string otherwise.
define expand_jedlib_file(f)
{
   variable n = 0, dir, file;
   forever
     {
	dir = extract_element(JED_LIBRARY, n, ',');
	!if (strlen(dir)) return (Null_String);
	file = dircat(dir, f);
	if (file_status(file) == 1) break;
	++n;
   } 
   file;
}


%!% If non-zero, startup by asking user for a filename if one was
%!% not specified on the command line.
variable Startup_With_File = 0;

%% startup hook
%!% Function that gets executed right before JED enters its main editing
%!% loop.  This is for last minute modifications of data structures that
%!% did not exist when startup files were loaded (e.g., minibuffer keymap)
define jed_startup_hook()
{
   variable n, hlp, ok = 0, mini = "Mini_Map", file, do_message = 1;

#ifdef MSDOS OS2
   definekey ("next_mini_command", "^@P", mini);
   definekey ("prev_mini_command", "^@H", mini);
#else
   definekey ("next_mini_command", "^[[B", mini);
   definekey ("prev_mini_command", "^[[A", mini);
#endif

   definekey ("mini_exit_minibuffer", "^M", mini);
   definekey ("exit_mini", "^[^M", mini);
   
   % turn on Abort character processing
   IGNORE_USER_ABORT = 0;

   runhooks ("startup_hook");
   
   !if (strcmp(whatbuf(), "*scratch*") or buffer_modified())
     {
	ERROR_BLOCK 
	  {
	     erase_buffer ();
	     set_buffer_modified_flag (0);
	  }
	
	insert_file (expand_jedlib_file("cpright.hlp")); pop ();
	set_buffer_modified_flag (0);
	bob();
	file = Null_String;
	if (Startup_With_File > 0)
	  {
	     forever 
	       {
		  file = read_file_from_mini ("Enter Filename:");
		  if (strlen(extract_filename(file))) break;
	       }
	  }
	else !if (Startup_With_File)
	  {
	     message(help_for_help_string); do_message = 0;
	     update(1);
	     pop(input_pending(600));
	  }
	EXECUTE_ERROR_BLOCK;
	if (strlen (file)) pop(find_file(file));
     }
   
   if (do_message) message(help_for_help_string);
}



%!% display row and column information in minibuffer
define whatpos ()
{
   variable max_lines;
   
   push_spot (); eob (); max_lines = whatline (); pop_spot ();
   
   message (Sprintf ("%s, Line %d of %d lines, Column %d",  
		     count_chars (), whatline(), max_lines, what_column (),
		     4));
}



%!% find a file from JED_LIBRARY, returns number of lines read or 0 if not 
%!% found.
define find_jedlib_file(file)
{
   file = expand_jedlib_file(file);
   !if (strlen(file)) return(0);
   find_file(file);
}


%%
%% help function
%%

%!% name of the file to load when the help function is called.
variable Help_File = "jed.hlp";   %% other modes will override this.

%!% Pop up a window containing a help file.  The help file that is read
%!% in is given by the variable Help_File.
define help()
{
   variable hlp = "*help*", buf, rows;
    
   !if (buffer_visible (hlp))
     {
	!if (strlen(Help_File)) Help_File = "generic.hlp";
	Help_File = expand_jedlib_file(Help_File);
	buf = whatbuf();
	onewindow();
	rows = window_info('r');
	setbuf(hlp);
	set_readonly(0);
	erase_buffer();

	pop(insert_file(Help_File));
	pop2buf(hlp);
	eob(); bskip_chars("\n");
	rows = rows / 2 - (whatline() + 1);
	bob();
	set_buffer_modified_flag(0);
	set_readonly(1);
	pop2buf(buf);
	loop (rows) enlargewin();
     }
   
   update (1);
   message(help_for_help_string);
}

#ifdef VMS
%% This resume hook is need for VMS when returning from spawn.
%% In fact, it is NEEDED for certain JED functions on VMS so declare it.
define resume_hook()
{
   variable file = getenv("JED_FILE_NAME");
   !if (strlen(file)) return;
   
   !if (find_file(file)) error("File not found!");
}
#endif VMS


%%file hooks
%!% returns backup filename.  Arguments to function are dir and file.
define make_backup_filename(dir, file)
{
#ifdef UNIX
   strncat (dir, file, "~", 3);
#endif
#ifdef MSDOS OS2
   variable type;
#ifdef OS2
   !if (IsHPFSFileSystem(dir)) {
#endif

      % There are several things to worry about.  Here just break up the 
      % filename and truncate type to 2 chars and paste it back.
      % note that this takes a name like file.c and produces file.c~
      % Also, note that if the type is empty as in 'file', it produces 
      % 'file.~'

      type = file_type(file);
      file = strncat (extract_element(file, 0, '.'), ".", substr(type, 1, 2), 3);
      

#ifdef OS2
   }
#endif
   strncat (dir, file, "~", 3);
#endif  
}


%!% returns autosave filename.  Arguments to function are dir and file.
define make_autosave_filename(dir, file)
{
#ifdef VMS
   Sprintf ("%s_$%s;1", dir, file, 2);
#endif
   
#ifdef UNIX
   Sprintf ("%s#%s#", dir, file, 2);
#endif
   
#ifdef MSDOS OS2
#ifdef OS2
   !if (IsHPFSFileSystem(dir)) 
     {
#endif
	
	variable type = file_type(file);
	file = substr(extract_element(file, 0, '.'), 1, 7);
	if (strlen(type)) file = strcat(file, ".");
	file = strcat (file, type);

#ifdef OS2
     }
#endif
   
   strncat (dir, "#", file, 3);
#endif
}

%!% breaks a filespec into dir filename--- 
%!% this routine returns dir and filename such that a simple strcat will
%!% suffice to put them together again.  For example, on unix, /a/b/c
%!% returns /a/b/ and c
define parse_filename(fn)
{
   variable f, dir, n;
      
   f = extract_filename(fn);
   n = strlen(fn) - strlen(f);
   dir = substr(fn, 1, n);
   dir; f;
}
 
%!% called AFTER a file is read in to a buffer.  FILENAME is on the stack.
define find_file_hook(file)
{
   variable dir, a, f, m;
   (dir, f) = parse_filename(file); 

#ifndef VMS
   if (file_status(dir) != 2)
     {
	error (Sprintf("Directory %s is invalid!", dir, 1));
     }
#endif
   
   a = make_autosave_filename(dir, f);
   if (file_time_compare(a, file) > 0) 
     {
	m = "Autosave file is newer. Use ESC-X recover_file.";
	flush(m);
        input_pending(10); pop();
	message(m);
     }
}

%
% completions  -- everything here must be predefined
% I just push the strings onto the stack and loop 'add_completion' over them
%
  $0 = _stkdepth();

. "toggle_undo" "calendar" "trim_buffer" "toggle_abbrev"
. "define_local_abbrev" "define_global_abbrev"
. "occur" "append_region" "write_region" "replace_across_buffer_files"
. "recover_file" "compile" "sort" "untab" "fortran_mode" "save_buffers"
. "isearch_forward" "isearch_backward" "shell"
. "edit_tab_stops" "c_mode" "toggle_crmode"
. "text_mode" "no_mode" "goto_line_cmd" "goto_column_cmd" "describe_mode"
. "evalbuffer" "open_rect" "kill_rect" "insert_rect" "copy_rect" "blank_rect"
. "dired" "re_search_forward" "re_search_backward" "query_replace_match"
. "describe_bindings"  "search_backward" "search_forward" "replace_cmd"
.  "find_binary_file"
#ifndef MSDOS OS2
.  "mail" 
#endif
#ifdef UNIX OS2
. "ispell"
#endif

loop (_stkdepth - $0) add_completion(());


%!% Prototype: String buffer_filename ();
%!% Returns the name of the file associated with the current buffer.  If 
%!% there is none associated with it, the empty string is returned.
define buffer_filename ()
{
   variable file, dir;
   (file, dir, , ) = getbuf_info();
   !if (strlen (file)) dir = Null_String;
   strcat (dir, file);
}


% save buffer if necessary
define save_buffer()
{
   variable flags, dir, file;
   (file, , , flags) = getbuf_info();

   !if (flags & 1) return (message("Buffer not modified."));
   if (strlen(file))
     {
	write_buffer(buffer_filename ()); pop();
     }
   else call ("save_buffers");
} add_completion("save_buffer");

define insert_buffer()
{
   read_with_completion("Insert Buffer:", Null_String, Null_String, 'b');
   push_spot();
   ERROR_BLOCK {pop_spot()}
   insbuf(());
   EXECUTE_ERROR_BLOCK;
}  add_completion("insert_buffer");

variable Global_Top_Status_Line = " *** To activate menus, press `Ctrl-H ?'.  For help, press `Ctrl-H' twice. ***";
set_top_status_line (Global_Top_Status_Line); pop ();
define help_prefix()
{
   variable c;
   
   !if (input_pending(20)) flush (help_for_help_string);
   c = int (strup(char(getkey())));
   switch (c)
     { case  8 : help }
     { case  'A' : apropos }
     { case  'I' : info_mode}
     { case  '?' : menu_main_cmds}
     { case  'F' : describe_function}
     { case  'V' : describe_variable}
     { case  'W' : where_is}
     { case  'C' or case ('K', c) : showkey}
     { case  'M' :
#ifdef UNIX OS2
	unix_man();
#else 
#ifdef VMS
	vms_help ();
#endif
#endif
#ifdef MSDOS
	call("coreleft");
#endif
     }
     { pop(); beep(); }
}

%%
%%  word movement definitions.  Since these vary according to editors,
%%  they are S-Lang routines.
%%

define skip_word ()
{
  while (skip_non_word_chars(), eolp()) 
    {
      if (1 != right(1)) break;
    }
   skip_word_chars();
}

define bskip_word()
{
   while (bskip_non_word_chars(), bolp())
     {
	!if (left(1)) break;
     }
   bskip_word_chars();
}

define delete_word()
{
   push_mark(); skip_word(); del_region();
}

define bdelete_word ()
{  
   push_mark(); bskip_word(); del_region();
}

define xform_word(x)
{
   skip_non_word_chars();
   push_mark(); skip_word(); 
   xform_region(x);
}

define capitalize_word()
{
   xform_word('c');
}

define upcase_word()
{
   xform_word('u');
}

define downcase_word()
{
   xform_word('d');
}

define smart_set_mark_cmd ()
{
   if (markp()) pop_mark(0); else call("set_mark_cmd");
}

define flush_input()
{
     while (input_pending(0)) pop(getkey());
}

%!% Prototype Void buffer_format_in_columns();
%!% takes a buffer consisting of a sigle column of items and converts the
%!% buffer to a multi-column format.
define buffer_format_in_columns()
{
   push_spot();
   bob();
   forever 
     {
	_for (0,4,1) 
	  {
	     goto_column(() * 14 + 1);
	     if (eolp())
	       {
		  if (eobp()) 
		    {
		       pop_spot();
		       return;
		    }
		  insert_single_space;
		  del();
	       }
	  } 
	!if (down(1)) break;
	% bol (); % this is a side effect of going down
     }
   pop_spot();
}

define delete_line()
{
   bol(); push_mark(); eol(); go_right(1); del_region();
}


define set_fill_column ()
{
   push_spot(); 
   eol();
   WRAP = what_column ();
   pop_spot();
   message (Sprintf("WRAP column at %d.", WRAP, 1));
}

define rename_buffer(name)
{
   variable flags = getbuf_info(); pop(); setbuf_info(name, flags);
}

define info ()
{
   info_mode ();
   definekey("exit_jed",		"Q",  "Infomap");
   definekey("exit_jed",		"q",  "Infomap");
}

define make_tmp_file(base)
{
   variable pid = getpid(), file, n = 1000;
   while (n)
     {
	file = strcat(base, string(pid));
	!if (file_status(file)) return (file);
	pid++;
     }
   error ("Unable to create a tmp file!");
}


define set_buffer_no_backup ()
{
   setbuf_info (getbuf_info() | 0x100);
}
define set_buffer_no_autosave ()
{
   setbuf_info (getbuf_info() & ~(0x2));
}

define goto_top_of_window ()
{
   go_up (window_line () - 1);
}

define goto_bottom_of_window ()
{
   go_down (window_info ('r') - window_line ());
}


%!% called from main in JED executable.
define command_line_hook()
{
   variable n, i, file, depth, jedrc, home, next_file;
   variable script_file = Null_String;
   jedrc = "jed.rc";
  
#ifdef UNIX
   jedrc = ".jedrc";
#endif
   
   home = getenv("JED_HOME");
#ifdef VMS
%% allows JED_HOME: to be search list---
%% thanks to SYSTEM@VACMZB.chemie.Uni-Mainz.DE for suggestion
   if (strlen(home)) home = "JED_HOME:";
#endif
   !if (strlen(home))
     {
	home = getenv("HOME");
#ifdef VMS
	home = "SYS$LOGIN:";
#endif
     } 
   jedrc = dircat(home, jedrc);
   
   n = MAIN_ARGC;
   
   --n;  %% argv[0], here it is not used.
%
%  If batch then first argument is not used so start at second
%  Also, n is the number of effective command line parameters so reduce it.
%
   
   if (BATCH) {	--n; 2; } else 1;  =i;
%
% if first argument is -n then do NOT load init file
%
   % stuff left on stack for if 
   if (n) strcmp (command_line_arg(i), "-n"); else 1;
   if (())
     {
	depth = _stkdepth;
	if (file_status(jedrc) == 1) jedrc; else "jed.rc";
	evalfile(()); pop();
	if ( _stkdepth != depth)
	  {
	     flush(strcat ("Excess junk left on stack by ", jedrc));
	     pop(input_pending(10));
	  }

	loop (_stkdepth - depth) pop();
     }
   else if (n) ++i;
   
   n = MAIN_ARGC - i;
   !if (n) return;
   
   if (BATCH == 2)
     {
	% last command line argument is a script.  eval it.
	script_file = command_line_arg (n + 1);
	n--;
     }
   
%
% Is JED to emulate most?
%
   !if (strcmp(command_line_arg(i), "-most"))
     {
	run_most (i + 1);
	return;
     }
   
   while (n > 0)
     {
	file = command_line_arg(i);
	--n; ++i;
	if (n) next_file = command_line_arg(i);
	switch(file)
	  {case "-f" and n : eval(next_file)}
	  {case "-g" and n : goto_line(integer(next_file))}
	  {case "-s" and n : 
	     fsearch(next_file); pop();
	     save_search_string(next_file);
	  }
	  {case "-l" and n : evalfile(next_file); pop();}
	  {case "-i" and n : insert_file(next_file); pop();}
	  {case "-2" : splitwindow(); ++n; --i;}
	  {case "-tmp":
	     set_buffer_no_backup ();
	     set_buffer_no_autosave ();
	     ++n; --i;
	  }
	  {find_file(()); pop(); ++n; --i;}
	
	--n; ++i;
     }
   
   if (strlen(script_file)) pop(evalfile(script_file));
}
%!% Prototype: Void deln (Integer n);
%!% delete the next 'n' characters.
define deln (n)
{
   push_mark (); go_right(n); del_region ();
}

define insert_spaces (n) 
{
   loop (n) insert_single_space ();
}

define blooking_at (str)
{
   variable n = strlen (str);
   
   EXIT_BLOCK 
     {
	pop_spot ();
     }
   
   push_spot ();

   if (n != left(n)) return 0;
   return looking_at (str);
}

   
% This fixes some bug in OS2 dealing with 'dir' issued non-interactively.
#ifdef OS2
   if (strlen(getenv("DIRCMD"))) putenv("DIRCMD=/ogn");
#endif

evalfile ("os.sl"); pop();

%
%  This code fragment looks for the existence of "defaults.sl" and loads
%  it.  This file IS NOT distributed with JED.
%

if (strlen(expand_jedlib_file("defaults.sl"))) pop(evalfile("defaults"));

