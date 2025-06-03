%
%  Miscellaneous functions for C mode
%

% produce a comment for C---
define c_make_comment ()
{
   variable d, cbeg, cend, file, mode;
   cbeg = "/*";
   cend = "*/";
   
   (, mode) = whatmode ();
   !if (mode & 2) return;
   
   (file,,,) = getbuf_info();
   
   if (mode & 8) 
     {
	cbeg = "%";
	cend = Null_String;
     }
   

   
   %% search for a comment on the line
   eol();
   if (bfind(cbeg))
     {
	!if (bolp())
	  {
	     go_left(1);
	     trim(); 
	     ffind(cbeg); pop();
	  }
	d = C_Comment_Column - what_column;
	if (d > 0) whitespace(d);
	!if (ffind(cend))
	  {
	     eol();
	     insert("  ");
	     insert (cend);
	  }
	
	bfind(cbeg); pop();
	go_right(strlen(cbeg) + 1);
     }
   else				       %/* not found */
     {
	if (what_column() <= C_Comment_Column)
	  {
	     goto_column(C_Comment_Column);
	  }
	else insert("   ");
	insert(cbeg); insert("  "); 
	if (strlen(cend))
	  {
	     insert (cend);
	     go_left(3);
	  }
     } 
}

