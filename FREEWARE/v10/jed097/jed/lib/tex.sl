%
%  simple (La)TeX mode for JED
%
%  When tex mode is loaded, 'tex_mode_hook' is called.  This hook will allow
%  users to customize the mode.  In particular, certain functions here have
%  no keybindings, e.g., 'latex_do_environment'.  So, in your jed.rc file,
%  add something like:
%    define tex_mode_hook () {
%       local_setkey ("latex_do_environment", "^C^E");
%    }
%  which binds the function to Ctrl-C Ctrl-E.




%  This hook identifies lines containing TeX comments as paragraph separator

define tex_is_comment ()
{
   bol ();
   if (looking_at ("%")) return 1;
   push_mark (); narrow ();
   re_fsearch ("[^\\\\]%");          % return value on stack
   widen ();
}

variable Tex_Ignore_Comment = 0;       % if true, line containing a comment
                                       % does not delimit a paragraph

define tex_paragraph_separator ()
{
   bol();
   if (looking_at("\\")
       or (skip_white(), eolp())
       or looking_at("$$")) return 1;
   
   %
   %  look for comment
   %
   return not (Tex_Ignore_Comment) and tex_is_comment ();
} 


define tex_wrap_hook ()
{
   variable yep;
   push_spot ();
   yep = up(1) and tex_is_comment ();
   pop_spot ();
   if (yep) 
     {
	push_spot ();
	bol (); 
	skip_white ();
	insert ("% ");
	pop_spot ();
     }
}

define tex_isolate_paragraph ()
{
   variable ic = Tex_Ignore_Comment;
   Tex_Ignore_Comment = 1;
   push_spot (); push_mark ();
   backward_paragraph ();
   narrow ();
   Tex_Ignore_Comment = ic;
}

define tex_blink_dollar ()
{
   variable d = "$", p, unmatched, p1, n, n1;
   variable pnow = POINT, pmax;
   
   insert(d);
   if (blooking_at ("\\$")) return;
   push_spot ();
   
   tex_isolate_paragraph ();	       %  spot pushed
   pop_spot (); 
   n = whatline ();
   bob ();
   
   unmatched = 0;
   
   while (fsearch (d))
     {
	p = POINT;
	pmax = 0x7FFF;
	if (tex_is_comment ())
	  {
	     pmax = POINT;
	  }
	POINT = p;
	
	if ((n == whatline ()) and (pnow < pmax)) pmax = pnow;
	
	if (p >= pmax)
	  {
	     if (n == whatline ()) break;
	     eol ();
	     continue;
	  }
	
	if (blooking_at("\\")) 
	  {
	     POINT++;
	     continue;
	  }
	
	!if (unmatched) 
	  {
	     p1 = p;
	     n1 = whatline ();
	  }
	
	unmatched = not(unmatched);
	POINT++;
     }
   
   if (unmatched)
     {
	n = n - n1;
	goto_line (n1);
	POINT = p1;
	widen ();
	if (n >= window_info ('r'))
	  {
	     bol (); push_mark (); eol (); 
	     message (strcat ("Matches ", bufsubstr ()));
	  }
	else
	  {
	     update(0);
	     input_pending(10); pop();
	  }
     }
   else widen ();
   pop_spot ();
}

define tex_insert_quote ()
{
   variable c, l, r;
   l = char ('`');   r = char ('\'');
   

   if ((LAST_CHAR != '\'') and (LAST_CHAR != '"'))
     {
	insert(char(LAST_CHAR));
	return;
     }
   
   c = '[';
   !if (bolp())
     {
	go_left(1);
	c = what_char();
	go_right(1);
     }
   
   if (c == '\\')
     {
	insert (char(LAST_CHAR));
	return();
     }
   
   if (is_substr("[({\t ", char(c)))
     {
	insert (l);
	if (LAST_CHAR == '"') insert (l);
     }
   else
     {
	insert (r);
	if (LAST_CHAR == '"') insert (r);
     }
}


define latex_do_environment ()
{
   variable env = Null_String;
   
   push_spot ();
   if (bsearch ("\\begin{"))
     { 
	go_right (7); push_mark ();
	ffind ("}"); pop ();
	env = bufsubstr ();
     }
   pop_spot ();
   
   eol (); newline ();
   env = read_mini ("Enter Environment Name:", env, Null_String);
   insert (Sprintf("\\\\begin{%s}\n\n\\\\end{%s}\n", env, env, 2));
   go_up(2);
}

define tex_ldots ()
{
   if (blooking_at (".."))
     {
	go_left (2);
	deln (2);
	insert ("\\ldots ");
	return;
     }
   insert_char ('.');
}

   
!if (keymap_p("TeX-Mode"))
{
   make_keymap ("TeX-Mode");
   definekey ("tex_insert_quote", "\"", "TeX-Mode");
   definekey ("tex_insert_quote", "'", "TeX-Mode");
   definekey ("tex_blink_dollar", "$", "TeX-Mode");
   definekey ("tex_ldots", ".", "TeX-Mode");
}

%!% Mode useful for editing TeX and LaTeX modes.  
%!% Useful bindings:
%!%  '"'  :  tex_insert_quote
%!%  '\'' :  tex_insert_quote
%!%  '$'  :  tex_blink_dollar
%!%  '.'  :  tex_ldots.  Inserts a '.' except if preceeded by two dots.  In 
%!%           this case, the dots are converted to \ldots.
%!%
%!%  When tex mode is loaded, 'tex_mode_hook' is called.  This hook will allow
%!%  users to customize the mode.  In particular, certain functions here have
%!%  no keybindings, e.g., 'latex_do_environment'.  So, in your jed.rc file,
%!%  add something like:
%!%    define tex_mode_hook () {
%!%       local_setkey ("latex_do_environment", "^C^E");
%!%    }
%!%  which binds the function to Ctrl-C Ctrl-E.
define tex_mode ()
{
   use_keymap ("TeX-Mode");
   setmode ("TeX", 0x1 | 0x20);
   set_buffer_hook ("par_sep", "tex_paragraph_separator");
   set_buffer_hook ("wrap_hook", "tex_wrap_hook");
   runhooks ("tex_mode_hook");
}

%-----------------------------------------------------------%

define tex_info_find_node ()
{
   variable node;
   
   node = read_mini ("Node:", Null_String, Null_String);
   !if (strlen (node)) return;
   info_mode ();
   info_find_node ("(latex)top");
   info_find_node (strcat ("(latex)", node));
}
