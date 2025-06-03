%
%  Special file for Linux systems
%

% This file sets up the console keys pgup/dn, etc.  It should be loaded
% by creating a file called defaults.sl in JED_LIBRARY with the line:
%    evalfile("linux.sl"); pop();
%
% defaults.sl is automatically loaded from site.sl.

variable UCB_Mailer;  UCB_Mailer = "/bin/mail";

$1 = getenv ("TERM");
if (is_list_element ("console,con80x25,con80x28", $1, ','))
{
%   USE_ANSI_COLORS = 1;   % uncomment if your console is a color one!
   OUTPUT_RATE = 0;
   tt_send("\e[44m");		       %/* white on blue */
   call("redraw");
   TERM_CANNOT_SCROLL = -1;
   setkey("bol",      		"^[[1~");	% home
   setkey("toggle_overwrite",	"^[[2~");       % insert
   setkey("delete_char_cmd",	"^[[3~");       % delete
   setkey("eol",		"^[[4~");       % end
   setkey("page_up",		"^[[5~");	
   setkey("page_down",		"^[[6~");
}
