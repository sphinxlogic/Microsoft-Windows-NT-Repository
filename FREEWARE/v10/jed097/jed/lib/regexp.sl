%
%  Interactive Regular expression searches.  These highlight region matched
%  until next key is pressed.
%


define regexp_mark_pattern(n)
{
   ERROR_BLOCK {pop_mark(0)}
   push_mark(); call ("set_mark_cmd");
   go_right(n);
   update(1);
   ungetkey(getkey());
   EXECUTE_ERROR_BLOCK;
}


define re_search_forward()
{
   variable pat, n; 
   pat = read_mini("Search (Regexp):", Null_String, Null_String);
   !if (strlen(pat)) return;
   push_mark();
   go_right(1);
   n = re_fsearch(pat);		       %  returns 1 + length of match

   !if (n) 
     {
	pop_mark(1);
	error("Not Found!");
     }
   pop_mark(0);
   regexp_mark_pattern(n - 1);
}
 
define re_search_backward()
{
   variable pat, n;    % n n 
   pat = read_mini("Backward Search (Regexp):", Null_String, Null_String);
   !if (strlen(pat)) return;
   
   push_mark();
   % go_left(1);
   n = re_bsearch(pat);
   !if (n)
     {
	pop_mark(1);
	error("Not Found!");
     }
   pop_mark(0);
   
   push_spot();
   go_right(n - 1);		       %  since 1 + length of match returned
   push_mark(); call ("set_mark_cmd");
   pop_spot();
   update(1);
   ungetkey(getkey());
   pop_mark(0);
}
 
define query_replace_match()
{
   variable pat, n, rep, prompt, doit, err, ch;
   
   err = "Replace Failed!";
   pat = read_mini("Regexp:", Null_String, Null_String);
   !if (strlen(pat)) return;
   prompt = strcat (strcat ("Replace '", pat), "' with:");
   rep = read_mini(prompt, Null_String, Null_String);
   
   prompt =  strcat(strcat(strcat (prompt, " '"), rep), "' ? (y/n/!/q)");
   while (n = re_fsearch(pat), n)
     {
	--n;			       %  1 + match length returned
	do 
	  {
	     message(prompt);
	     regexp_mark_pattern(n);
	
	     ch = getkey ();
	     if (ch == 'r')
	       {
		  recenter (window_info('r') / 2);
		  go_left(n);
	       }
	     
	  } while (ch == 'r');
	
	switch(ch)
	  { case 'y' : !if (replace_match(rep, 0)) error(err)}
	  { case 'n' : ()}
	  { case '+' : !if (replace_match(rep, 0)) error(err);
	                return;
	  }
	  { case '!' :
	     do 
	       {
		  !if (replace_match(rep, 0)) return;
		  if (eolp()) 
		    {
		       !if (right(1)) break;
		    }
	       }
	     while(re_fsearch(pat));
	     return;
	  }
	     
	  { pop(); return; }
	
	if (eolp()) 
	  {
	     !if (right(1)) break;
	  }
     }
}


	     
       
	
	     
	  
	
	     
	
   
   
