%% EDT emulation for JED  -- Application Keypad.
%%
%%  To load this, put the line:  "edt.sl" evalfile
%%   in your jed.rc (.jedrc) file
%%
%% Here the user gets to choose the action of the 'subs' key (Gold-enter)
%% By default, it behaves as EDT.  By putting the next line in JED.RC, this
%% key is bound to a more conventional query replace
%% setkey ("replace_cmd",	"^[OP^[OM");  %% subs (query replace)
%%
%% The default binding for the quote keys (", ') is 'text_smart_quote'.
%% Most users do not seem to like this so it is unset here.
  setkey("self_insert_cmd",	"\"");
  setkey("self_insert_cmd",	"'");
%%
%% In addition, if you want the ^H key to move to the beginning of line
%% then uncomment the next two lines.
%    unsetkey("^H");
%  setkey("beg_of_line",	"^H");  %% beginning of line
%% By default, these are bound
%% to help functions (man page, etc...).

%% conventional subs key definition:
  setkey("edt_subs",	"^[OP^[OM");  %% subs (edt style)

#ifdef MSDOS OS2
NUMLOCK_IS_GOLD = 1; %% This sets the numlock key on the 'application keypad'
#endif

  set_status_line("(Jed %v) EDT: %b   (%m%n%o)  %p,%c   Advance   %t", 1);

%%
%% Escape sequences for EDT keypad:
%%
%% FP1 = ^[OP       FP2 = ^[OQ      FP3 = ^[OR       PF4 = ^[OS
%%   7 = ^[Ow         8 = ^[Ox        9 = ^[Oy         - = ^[Om
%%   4 = ^[Ot         5 = ^[Ou        6 = ^[Ov         , = ^[Ol
%%   1 = ^[Oq         2 = ^[Or        3 = ^[Os
%%            0 = ^[Op            . = ^[On         enter = ^[OM

WANT_EOB = 1;
#ifdef UNIX VMS
  tt_send("\e=");   % turn on application keypad
#endif

!if (is_defined("Edt_Loaded"))
{ 
   variable Edt_Loaded;
   variable edt_pbuf;    %% a real buffer
   edt_pbuf = " <edt>";
   whatbuf();
   setbuf(edt_pbuf);
   sw2buf(());
} 


%% unset some of default jed keys--- lose window capability on "^W" one
%% unsetkey ("^W");
%% setkey ("redraw", "^W");

%% unsetkey("^K"); unsetkey %%-- unset this, we lose kill line in emacs.sl
unsetkey("^U");             %% emacs.sl rebinds this to digit-arg

%% Give user ability to exit via GOLD-Q, GOLD-E
  setkey("exit_jed", "^[OPQ");
  setkey("exit_jed", "^[OPE");
define quit() {exit_jed()}
define exit() {exit_jed()}

%%
%%  Gold-Gold to toggle keypad state
%%
#ifdef MSDOS OS2
 unsetkey("^@;");
  setkey("edt_togglekp",  "^@;");	%% IBM F1 key
#endif

variable Edt_Keypad;  Edt_Keypad = 1;

define edt_togglekp ()
{
   variable on, off;
#ifdef VMS UNIX
   off = "Numeric.";  on =  "Application.";
#else
   on = "Gold ON"; off = "Gold OFF";
   NUMLOCK_IS_GOLD = not(NUMLOCK_IS_GOLD);
#endif

#ifdef UNIX VMS
   if (Edt_Keypad) { off; "\e>" } else {on; "\e="}  tt_send(());
#else
   if (Edt_Keypad) off; else on;
#endif
   message(());
   Edt_Keypad = not(Edt_Keypad);
}

  setkey("edt_togglekp",	"^[OP^[OP");	%% Gold-Gold toggles keypad
%  setkey("self_insert_cmd", "^I");	%% tab inserts tab.
  setkey("edt_delbol",	"^U");	%% delete to bol
  setkey("edt_help",	"^[OQ");	%% help
  setkey("edt_findnxt",	"^[OR");	%% findnxt
  setkey("edt_ldel",	"^[OS");	%% del l
  setkey("edt_cdel",	"^[Ol");	%% del c
  setkey("edt_wdel",	"^[Om");	%% del w
  setkey("smart_set_mark_cmd",	"^[On");	%% select
  setkey("edt_line",	"^[Op");	%% line
  setkey("edt_word",	"^[Oq");	%% word
  setkey("edt_eol",	"^[Or");	%% eol
  setkey("edt_char",	"^[Os");	%% char
  setkey("edt_advance",	"^[Ot");	%% advance
  setkey("edt_backup",	"^[Ou");	%% backup
  setkey("edt_cut",	"^[Ov");	%% cut
  setkey("edt_page",	"^[Ow");	%% page
  setkey("edt_sect",	"^[Ox");	%% sect
  setkey("edt_append",	"^[Oy");	%% append
  setkey("edt_help",	"^[OP^[OQ");	%% help
  setkey("edt_find",	"^[OP^[OR");	%% find
  setkey("edt_uldel",	"^[OP^[OS");	%% udel l
  setkey("edt_ucdel",	"^[OP^[Ol");	%% udel c
  setkey("edt_uwdel",	"^[OP^[Om");	%% udel w
  setkey("edt_reset",	"^[OP^[On");	%% reset
  setkey("edt_oline",	"^[OP^[Op");	%% open line
  setkey("edt_chgcase",	"^[OP^[Oq");	%% chgcase
  setkey("edt_deleol",	"^[OP^[Or");	%% deleol
%%
%%  There are two possible definitions for the specins key.  Let's choose
%%  the edt one though I prefer the other
%%
  setkey("edt_specins",   "^[OP^[Os");	%% specins
%  setkey("quoted_insert",	"^[OP^[Os");	%% specins
  setkey("eob",		"^[OP^[Ot");	%% bottom
  setkey("beg_of_buffer",	"^[OP^[Ou");	%% top
  setkey("edt_paste",	"^[OP^[Ov");	%% paste
  setkey("evaluate_cmd",	"^[OP^[Ow");	%% cmd
  setkey("format_paragraph", "^[OP^[Ox");	%% fill
  setkey("edt_replace",	"^[OP^[Oy");	%% replace
  setkey("exit_mini",	"^[OM");	%% enter
%%
%% the enter key requires some care--- it MUST be put in the minibuffer
%%   keymap.  But this is not created until AFTER the init files are loaded
%%   so that it inherits user definitions.  The above line puts it in the
%%   global map so that it behaves properly there.  The same applies to
%%   the 'reset' command.
%%
%% The above comment WAS true in versions prior to 0.81.  Now it is possible
%% to achieve the desired effect in the startup_hook.  I think it is time to 
%% think of a way to chain functions.  That is, I would like to define a 
%% function that startup_hook will call without explicitly modifying startup
%% hook and not destroying other hooks in the process.  Thus, startup_hook
%% should consist of a chain of functions to execute.
%%
 if (keymap_p("Mini_Map"))
   { 
      definekey("exit_mini", "^[OM", "Mini_Map");    %% enter
      definekey("edt_reset", "^[OP^[On","Mini_Map"); %% reset
   }
%%
%%  In EDT, a command may be repeated by GOLD number.  Lets have that too
%%
 _for (0, 9, 1) 
    {
       =$0;
       setkey("digit_arg", strcat("\eOP", string($0)));
    }

%% These are the keys on the vt220 keyboard
  setkey("edt_find",      	"^[[1~");	%%differs from vt220.sl
  setkey("yank",			"^[[2~");	
  setkey("kill_region",		"^[[3~");	
  setkey("smart_set_mark_cmd",		"^[[4~");	
  setkey("page_up",		"^[[5~");	
  setkey("page_down",		"^[[6~");	
%  setkey(evaluate_cmd",		"^[[29~");	
  setkey("emacs_escape_x",	"^[[29~");	
  setkey("edt_help",		"^[[28~");	
%%
%%  Finally some definitions for scrolling the screen left/right
%%
  setkey("beg_of_buffer", "^[OP^[[A");	% gold ^
  setkey("eob",           "^[OP^[[B");	% gold v
  setkey("scroll_left",   "^[OP^[[C");	% gold ->
  setkey("scroll_right",  "^[OP^[[D");	% gold <-

%% The major complication is the direction.  Here it is:
variable edt_dir; edt_dir = 1;

%!%ADVANCE - (4)
%!%Sets the current direction to forward for the CHAR, WORD, LINE, EOL, PAGE,
%!%SECT, SUBS, FIND, and FNDNXT keys.  ADVANCE means that movement will be
%!%toward the end of the buffer; that is, to the right and down.
define edt_advance()
{
   edt_dir = 1;
  set_status_line("(Jed %v) EDT: %b   (%m%n%o)  %p,%c    Advance   %t", 1);
}

%!%BACKUP - (5)
%!%Sets the cursor direction to backward for the CHAR, WORD, LINE, EOL, PAGE,
%!%SECT, SUBS, FIND, and FNDNXT keys.  BACKUP means that movement will be
%!%toward the beginning of the buffer% that is, to the left and up.
define edt_backup()
{
   edt_dir = -1;
  set_status_line("(Jed %v) EDT: %b   (%m%n%o)  %p,%c    Backup    %t", 1);
}

%% other buffers:  not buffers but strings except the char buffer which is int
variable edt_wbuf, edt_lbuf, edt_cbuf;
edt_wbuf = Null_String; edt_lbuf = Null_String; edt_cbuf = 0;

%% character (un)deletion
define edt_cdel()
{  
   !if (eobp()) { edt_cbuf = what_char(); del(); }
}
define edt_ucdel()
{
   if (edt_cbuf) { insert_char (edt_cbuf); go_left(1); } 
}

define edt_word()
{
   if (edt_dir == 1)
     {
	if (eolp()) return(go_right(1));
	skip_word_chars(); skip_non_word_chars();
     }
   else
     {
	if (bolp()) return (go_left(1));
	bskip_non_word_chars(); bskip_word_chars();
     } 
}

%!%DEL W - (-)
%!%Deletes text from the cursor to the beginning of the next word, storing the
%!%text in the delete word buffer.
define edt_wdel()
{
   push_mark(); push_mark(); 
   edt_word();            %% use whatever edt_word does as a region
   edt_wbuf = bufsubstr();
   del_region();
}

%% another one from Clive Page.
define edt_specins()
{
   insert_char (integer(read_mini("char code (decimal):", "27", Null_String)));
}

%!%UND W - (GOLD -) 
%!%Inserts the contents of the delete word buffer directly to the left of the
%!%cursor.
define edt_uwdel()
{
   push_spot(); insert (edt_wbuf); pop_spot();
}

%% aparantly deleol also saves what it did in buffer...
define edt_deleol()
{
   push_mark(); push_mark(); eol(); edt_lbuf = bufsubstr(); del_region();
}
define edt_delbol()
{
   push_mark(); push_mark(); bol(); edt_lbuf = bufsubstr(); del_region();
}

%% the line

%!%DEL L - (PF4) 
%!%Deletes text from the cursor position to the end of the current line, including
%!%the line terminator.  If the cursor is positioned at the beginning of a line,
%!%the entire line is deleted.  The deleted text is saved in the delete line
%!%buffer.
define edt_ldel()
{
   push_mark(); push_mark();
   eol(); go_right(1);
   edt_lbuf = bufsubstr();
   del_region();
}

%!%UND L - (GOLD PF4)
%!%Inserts the contents of the delete line buffer directly to the left of the
%!%cursor.
define edt_uldel()
{
   push_spot(); insert (edt_lbuf); pop_spot();
}

%!%FIND - (GOLD PF3) 
%!%Searches for an occurrence of a string.  Press the FIND key and then enter the
%!%string using the main keyboard.  End the string by pressing either the ADVANCE
%!%or BACKUP key to set the direction of the search, or the ENTER key to search in
%!%the current direction.
define edt_find()
{
   if (edt_dir == 1) search_forward (); else search_backward ();
}

%!%FNDNXT - (PF3)
%!%Searches for the next occurrence of the search string previously entered with
%!%the FIND key.  The direction of the search is the current one (ADVANCE or
%!%BACKUP).
define edt_findnxt()
{
   variable r, found;
   r = 0;
   if (strlen(LAST_SEARCH))
     { 
	if (edt_dir == 1)
	  {
	     r = right(1);
	     found = fsearch(LAST_SEARCH);
	  }
	else found = bsearch(LAST_SEARCH);
	!if (found)
	  { 
	     go_left(r);
	     error("Not Found.");
	  }
     }
   else error ("Find What?")
}

%!%SECT - (8) 
%!%Moves the cursor 16 lines (one section) forward or backward, depending on the
%!%current direction (see ADVANCE and BACKUP).  The cursor is moved to the
%!%beginning of the appropriate line.
define edt_sect()
{
   if (edt_dir == 1) go_down(16); else go_up(16);
   bol();
}

define edt_eol()
{
   if (edt_dir == 1)
     {
	if (eolp()) go_down(1);
     }
   else go_up(1);
   eol();
}

define edt_line()
{
   if (edt_dir > 0) go_down(1); 
   else if (bolp()) go_up(1);
   bol();
}

define edt_char()
{
   if (edt_dir == 1) go_right(1); else go_left(1);
}

define edt_oline()
{
   newline(); go_left(1);
}

%% reset also pops marks.
define edt_reset()
{
   edt_advance();
   while(markp()) pop_mark(0);
   call ("kbd_quit");
}

%% edt page searches for a form feed.  However, real edt allows user to
%% change this.

%!%PAGE - (7)
%!%Moves the cursor to the top of a page.  A page is defined by a delimiter
%!%string, which can be set by the SET ENTITY command.  The default page
%!%delimiter is the formfeed character (CTRL/L).
define edt_page()
{
   variable ret, ff;
   ff = char(12);
   if (edt_dir == 1) ret = fsearch(ff); else ret = bsearch(ff);
   if (ret) recenter(1);
}

define edt_cut()
{
   variable b;
   !if (dupmark()) return;
   b = whatbuf();
   setbuf(edt_pbuf);
   erase_buffer();
   setbuf(b);
   copy_region(edt_pbuf);
   del_region();
}
define edt_paste() 
{
   insbuf(edt_pbuf);
}

%% Although not documented in EDT online help, this deletes the region.
%!%APPEND - (9) 
%!%Moves the select range to the end of the PASTE buffer.  The select range is all
%!%the text between the selected position (see SELECT) and the current cursor
%!%position.  If no SELECT has been made and the cursor is positioned on the
%!%current search string, that string is appended.
define edt_append()
{
   variable b;
   b = whatbuf();
   setbuf(edt_pbuf);
   eob();
   setbuf(b);
   if (dupmark())
     {
	copy_region(edt_pbuf);
	del_region();
     }
   else error("No Region.");
}

define edt_replace()
{
   del_region(); edt_paste();
}

%% a real edt_subs function
%% deletes search string, substitutes what is in the pastebuffer and finds
%% the next.
define edt_subs()
{
   if (looking_at(LAST_SEARCH))
     {
	deln (strlen(LAST_SEARCH));
	edt_paste();
	!if (looking_at (LAST_SEARCH)) edt_findnxt();
     }
   else error("Select range not active.")
}


%% a help for the help key
define edt_help()
{
   jed_easy_help("edt.hlp");
}

%% Chngcase
define edt_chgcase()
{
   variable n;
   !if (markp())
     {
	push_mark();
	n = strlen(LAST_SEARCH);
	if (n and looking_at(LAST_SEARCH)) go_right(n);
	else if (edt_dir == 1) go_right(1); 
	else go_left(1);
     }
   xform_region('X');
}

%% is this a better defn of what edt_replace should be?
%!%REPLACE - (GOLD 9)
%!%Deletes the select range and replaces it with the contents of the PASTE 
%!%buffer.
define edt_replace()
{
   variable n;
   n = strlen(LAST_SEARCH);

   if (n and looking_at(LAST_SEARCH)) deln (n);
   else 
     {
	!if (markp()) error("Select range not active.");
	del_region();
     }
   edt_paste();
}
