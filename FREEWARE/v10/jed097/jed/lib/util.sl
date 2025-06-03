%%
%%  utility functions which Must be available (e.g., autoload)
%%

%%
%%  A routine that trims the buffer by:  removing excess whitespace at the
%%  end of lines and removing excess newlines
%%

define trim_buffer()
{
   push_spot();
   bob();
   forever
     {
	eol(); trim(); bol();
	if (eolp())
	  {
	     go_down(1);
	     while (eol(), trim(), bol(), 
		    eolp() and not(eobp())) del();
	  } 
	!if (down(1)) break();
     }

   bob(); eol(); trim(); bol(); if (eolp()) del();
   pop_spot();
   message ("done.");
}


%% occur function
%%
define occur()
{
   variable str, cbuf, tmp, n;
   
   str = read_mini("Find All (Regexp):", LAST_SEARCH, Null_String);
   tmp = "*occur*";
   cbuf = whatbuf();
   pop2buf(tmp);
   erase_buffer();
   pop2buf(cbuf);
   
   push_spot();
   while (re_fsearch(str))
     {
	n = string(whatline()); 
	bol(); push_mark(); eol();
	bufsubstr();  % stack
	
	setbuf(tmp);
	insert(n);
	goto_column(5);  insert_char (':');
	insert(()); newline();
	setbuf(cbuf);
	eol();             %% so we do not find another occurance on same line
     }
   pop_spot();
   setbuf(tmp);
   bob(); set_buffer_modified_flag(0);
}

