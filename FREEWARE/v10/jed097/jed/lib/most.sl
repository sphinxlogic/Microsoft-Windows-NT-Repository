%%
%%  Most/more/less file viewing
%%  

define most_edit ()
{
   set_readonly(0);
   getbuf_info(); pop(); pop(); pop(); 
   file_type(); runhooks("mode_hook");
   set_status_line(Null_String, 0);
}


. (
.    [flags]
.    most_edit
.    getbuf_info =flags pop pop pop
.    flags 1 & {whatbuf delbuf} !if
.    otherwindow
.    "Done." message 
. ) most_exit_most

. (
.  "SPC:pg dn, DEL:pg up, /:search forw, ?:search back, q:quit, e:edit, h:this help"
.  message
. ) most_help

. [Most_Map] "Most" =Most_Map
. Most_Map keymap_p
.  {  
. 	Most_Map make_keymap %% need a make sparse map!
. 	'A' 'z' 1 {char Most_Map undefinekey} _for
. 	"page_up"    "^?" Most_Map definekey
. 	"page_down"  " "  Most_Map definekey
. 	"most_exit_most" "q"  Most_Map definekey
. 	"most_edit"	"e"	Most_Map definekey
. 	"most_find_f" "s" Most_Map definekey
. 	"most_find_f" "S" Most_Map definekey
. 	"most_find_f" "/" Most_Map definekey
. 	"most_find_b" "?" Most_Map definekey
. 	"most_find_n" "N" Most_Map definekey
. 	"most_help"	"h"	Most_Map definekey
. 	"eob" "B" Most_Map definekey
. 	"bob" "T" Most_Map definekey
.  }
. !if

%!% Emulates MOST fileviewer
%!% The following keys are defined:
%!% SPACE            next screen
%!% DELETE           previous screen
%!% /                search_forward
%!% ?                search_backward
%!% n                find next occurrence
%!% q                quit most mode  (usually kills buffer if not modified)
%!% e                edit buffer
%!% h                help summary
%!% t                Top of Buffer
%!% b                End of Buffer
define most_mode ()
{
.   Most_Map use_keymap
.   1 set_readonly
.   Most_Map 0 setmode
.   " MOST : press 'h' for help.   %b    (%p)" 0 set_status_line
%.   most_help
}
  
. [most_dir] 1 =most_dir

. ( [r]
.   LAST_SEARCH int 0 ==
.     { "Find What?" error}
.     { LAST_SEARCH
.       most_dir 1 == { 1 right =r fsearch }{ 0 =r bsearch } else
.       { "Not Found." error 
.         r left pop
.       } !if
.     } else
. ) most_find_n


. ( -1 =most_dir search_backward) most_find_b
. ( 1 =most_dir search_forward) most_find_f

. [Most_Min_Arg] 
. [Most_Current_Arg] 
%%% Emulate MOST fileviewer.
define run_most (arg)
{
   Most_Min_Arg = arg;
   Most_Current_Arg = arg;

   if (arg != MAIN_ARGC) pop (find_file (command_line_arg (arg)));
   most_mode ();
   definekey ("most_next_file", ":N", Most_Map);
   definekey ("exit_jed", "q", Most_Map);
   definekey ("exit_jed", "Q", Most_Map);
   definekey ("most_exit_most", ":Q", Most_Map);
   help_for_help_string = 
   "Most Mode: SPC- next screen, DEL- prev screen, Q Quit, :n next file, :Q edit";
}

. (
.   [f file dir] 1 =dir
.   {
.      Most_Current_Arg dir + =Most_Current_Arg
.      Most_Current_Arg MAIN_ARGC == {Most_Min_Arg =Most_Current_Arg} if
.      Most_Current_Arg Most_Min_Arg < {MAIN_ARGC 1 - =Most_Current_Arg} if
     
.      Most_Current_Arg command_line_arg =file
.      "Next File (Use Arrow keys to select): " file strcat flush
.      get_key_function =f
.      f strlen
.       {
.          pop % pops off second return value of get_key_function
. 	 "previous_line_cmd" f strcmp {1 =dir continue} !if
. 	 "next_line_cmd" f strcmp {-1 =dir continue} !if
. 	 break
.       }{
. 	 beep
.       } else
.    } forever
.   file find_file pop " " message
.   most_mode
. ) most_next_file
