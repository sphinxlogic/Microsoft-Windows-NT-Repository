% These routines perform a better job of indenting C code.

!if (is_defined ("C_No_Brace_Offset"))
{
   variable C_No_Brace_Offset = 0;
}

define c_looking_at (token)
{
   variable cse = CASE_SEARCH, ret = 0;
   CASE_SEARCH = 0;
   
   push_spot ();
   if (looking_at(token))
     {
	go_right(strlen(token));
	POINT;
	skip_chars ("\t :({");
	ret = (POINT - ()) or eolp();
     }
   pop_spot ();
   CASE_SEARCH = cse;
   ret;
}


define c_indent_line_1 ()
{
   variable ch;

   push_spot ();
   EXIT_BLOCK {pop_spot ();}
   
   bol (); bskip_chars ("\n \t");
   go_left (1);
   ch = what_char ();
   if ((ch == ';') or (ch == '/') or (ch == '{')) return;
   
   if (ch == ')') call ("goto_match");
   bskip_chars (" \t");
   bskip_chars ("^ \t");
   
   if (orelse 
	 {c_looking_at("if");}
	 {c_looking_at("else");}
	 {c_looking_at("for");}
	 {c_looking_at("while");}
       )
     {
	pop_spot ();
	bol ();
	skip_white ();
	!if (looking_at_char ('{'))
	  {
	     %
	     %  increase indentation level
	     %
	     whitespace (C_BRACE + C_No_Brace_Offset);
	  }
	push_spot ();  % for exit block
     }
}

variable C_Indent_In_Indent = 0;
define c_colon_indent_line ()
{
   if (C_Indent_In_Indent) return 0;
   
   push_spot ();
   bol ();
   skip_white ();
   if (looking_at_char ('#')) 
     {
	bol (); 
	trim ();
	return 1;
     }

   C_Indent_In_Indent++;
   
   !if (orelse
	  { c_looking_at("case"); }
	  { c_looking_at("default"); }
	  { c_looking_at("protected");}
	  { c_looking_at("private");}
	  { c_looking_at("public");}
	)
     {
	pop_spot ();
	C_Indent_In_Indent--;
	return 0;
     }
   
   indent_line ();
   bol (); skip_white ();
   what_column() - C_INDENT;
   bol(); trim();
   whitespace(());
   pop_spot ();
   if (bolp()) skip_white ();
   C_Indent_In_Indent--;
   1;
} 

% indent_line_cmd will automatically call this.
define c_indent_line ()
{
   if (c_colon_indent_line ())
     {
	return;
     }
   
   c_indent_line_1 ();
}


	     
   
