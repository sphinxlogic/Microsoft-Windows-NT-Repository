% This file should not be byte-compiled.
% It is loaded from site.sl and permits various flavors of jed to share the
% same set of S-Lang files.

% It is written in RPN for efficiency.

#ifdef XWINDOWS
. "mouse" evalfile pop
. "HOST" getenv =$1
. $1 strlen { "XJed@" $1 strcat x_set_window_name } if
. "skip_word" 		"^[[c" setkey      %/* shift-right */
. "bskip_word"		"^[[d" setkey      %/* shift-left */
. "goto_top_of_window"	"^[[a" setkey      %/* shift-up */
. "goto_bottom_of_window"	"^[[b" setkey      %/* shift-down */
#endif
