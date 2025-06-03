%
%  Fortran mode for jed
%
%   Loading this file, then executing 'fortran_mode' will start fortran mode
%   on the current buffer.  The only keys that get remapped are:
%     ^M (return)   which runs the fortan newline command
%     ^I (tab) which indents the current line
%     ^[; (escape semicolon) runs the fortran comment command
%     ^[: (escape colon) runs fortran uncomment command
%     ^[^M : start a continuation line

!if (is_defined ("Fortran_Continue_Char"))
{
   variable Fortran_Continue_Char = "&";      % default continuation char
}

!if (is_defined ("Fortran_Comment_String"))
{
   variable Fortran_Comment_String = "C ";
}

!if (is_defined ("Fortran_Indent_Amount"))
{
   variable Fortran_Indent_Amount = 2;
}

% gotobeginning of line and skip past continuation char
define fortran_skip_label ()
{
   bol ();
   skip_chars ("0-9 \t");
   if (looking_at(Fortran_Continue_Char)) go_right(1);
   skip_white ();
}

% computes fortran indent
define fortran_get_indent ()
{
   variable col = 7;    %/*  at top of buffer it should be 7 n'est pas? */
   variable cse = CASE_SEARCH;
   CASE_SEARCH = 0;

   push_spot ();
   
   while (up(1))
     {
	bol ();
	skip_white();
	if (eolp() or looking_at(Fortran_Continue_Char)) continue;
	fortran_skip_label ();
	col = what_column ();
     
	if (col == 1) continue;
	
	if (looking_at("do ") 
	    or looking_at("else"))
	  col += Fortran_Indent_Amount;
	else if (looking_at("if ") or looking_at("if("))
	  {
	     if (ffind ("then")) col += Fortran_Indent_Amount;
	  }
	break;
     }
   
  % now check current line
   pop_spot ();
   push_spot ();
   fortran_skip_label ();
  
   if (looking_at("end") or
       looking_at("continue") or
       looking_at("else")) col -= Fortran_Indent_Amount;
  
   if (col < 7) col = 7;
   pop_spot ();
   CASE_SEARCH = cse;
   col;
}

% used below-- skips range 
% assumes we are after the label or continuation char 
% and indents the rest to goal
define fortran_tough_indent (goal)
{
   skip_chars ("0-9");
   trim ();
   if (looking_at (Fortran_Continue_Char))
     {
	insert_spaces (6 - what_column());
	go_right(1); trim();
	goal += Fortran_Indent_Amount;
     }
   
   insert_spaces (goal - what_column());
}



% fortran indent routine
define fortran_indent ()
{
   variable col, ch, n, goal;
   push_spot ();
   goal = fortran_get_indent ();
   bol (); skip_white ();
   col = what_column ();
   
   switch (char(what_char()))
     {
	isdigit (()) :     %  label

	if (col >= 6)
	  {
	     bol (); trim ();
	     insert_single_space ();
	  }
	
	fortran_tough_indent (goal);
     }
     {
	case Fortran_Continue_Char :  % continuation character
	bol (); trim (); insert ("     ");
	fortran_tough_indent (goal);
     }
     {
	pop (); not (bolp()) or eolp ():                  % general case
	bol (); trim ();
	goal--; insert_spaces (goal);
     }
   pop_spot ();   
   skip_white ();
}

define fortran_is_comment ()
{
   bol ();
   skip_chars (" \t0-9");
   bolp () and not (eolp());
}


define fortran_newline ()
{
   variable p, cont;
   
   if (bolp ())
     {
	newline ();
	return;
     }

   fortran_indent ();
   push_spot ();
   bskip_chars (" \t"); trim();
   
   if (what_column () > 72)
     {
	push_spot ();
	bol(); skip_white();
	!if (bolp()) message ("Line exceeds 72 columns.");
	pop_spot ();
     }
   
   p = POINT;
   bskip_chars("-+*=/,(");
   
   cont = (p != POINT);
   
   if (fortran_is_comment ()) cont = 0;
   
   bol ();
   skip_white ();
   if (looking_at("data ")) cont = 0;
   
   pop_spot ();
   
   newline ();
   insert_single_space ();
   if (cont) insert(Fortran_Continue_Char);
   fortran_indent ();
}


define fortran_continue_newline ()
{
   fortran_newline ();
   
   push_spot ();
   bol ();
   skip_white ();
   if (looking_at(Fortran_Continue_Char)) pop_spot ();
   else
     {
	insert (Fortran_Continue_Char);
	pop_spot ();
	fortran_indent ();
	go_right(1);
	skip_white ();
     }
}

%
%   electric labels
%
define fortran_electric_label ()
{
   variable ch, col;
   insert_char (LAST_CHAR);
   push_spot ();
   bol (); skip_white (); % test for comment
   if (bolp ()) pop_spot ();
   else
     {
	skip_chars ("0-9"); trim ();
	pop_spot ();
	fortran_indent ();
     }
}


% fortran comment/uncomment functions

define fortran_uncomment ()
{
   push_spot ();
   if (fortran_is_comment ())
     {
	bol ();
	if (looking_at (Fortran_Comment_String)) 
	  deln (strlen (Fortran_Comment_String));
	else del ();
     }
   
   fortran_indent ();
   pop_spot ();
   go_down (1);
}

define fortran_comment ()
{
   !if (fortran_is_comment ())
     {
	push_spot ();
	bol ();
	insert (Fortran_Comment_String);
     }
   pop_spot ();
   go_down(1);
}

%
% main entry point into the fortran mode
%

!if (is_defined("Fortran_Mode"))
{
   variable Fortran_Mode = "Fortran";
   make_keymap (Fortran_Mode);
   definekey ("fortran_newline", "^M",  Fortran_Mode);
   definekey ("fortran_indent",	"^I",	Fortran_Mode);
   definekey ("fortran_comment",	"^[;",	Fortran_Mode);
   definekey ("fortran_uncomment",	"^[:",	Fortran_Mode);
   definekey ("fortran_continue_newline",	"^[^M",	Fortran_Mode);
   definekey ("self_insert_cmd",	"'",	Fortran_Mode);
   definekey ("self_insert_cmd",	"\"",	Fortran_Mode);
   _for (0, 9, 1,)
     {
	=$1;
	definekey ("fortran_electric_label", string($1), Fortran_Mode);
     }
}

%!% Mode designed for the purpose of editing FORTRAN files.
%!% After the mode is loaded, the hook 'fortran_hook' is called.
%!% Useful functions include
%!% 
%!%  Function:                    Default Binding:
%!%   fortran_indent                    TAB
%!%   fortran_newline                   RETURN  
%!%     indents current line, inserts newline and indents it.
%!%   fortran_continue_newline          ESC RETURN
%!%     indents current line, and creates a continuation line on next line.
%!%   fortran_comment                   ESC ;
%!%     comments out current line
%!%   fortran_uncomment                 ESC :
%!%     uncomments current line
%!%   fortran_electric_label            0-9
%!%     Generates a label for current line or simply inserts a digit.
%!%  
%!% Variables include:
%!%   Fortran_Continue_Char   --- character used as a continuation character.  
%!%     By default, its value is "&"
%!%   Fortran_Comment_String  --- string used by 'fortran_comment' to 
%!%     comment out a line.  The default string is "C ";
%!%   Fortran_Indent_Amount   --- number of spaces to indent statements in 
%!%                               a block.  The default is 2.
define fortran_mode ()
{
   use_keymap (Fortran_Mode);
   setmode (Fortran_Mode, 0x4 | 0x10);
   runhooks ("fortran_hook");
}
