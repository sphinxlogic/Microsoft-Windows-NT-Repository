%
%  forward and backward search functions.  These functions can search across
%  lines.
%
define search_across_lines (str, dir)
{
   variable n, s, s1, fun;

   fun = &fsearch;
   if (dir < 0) fun = &bsearch;

   n = is_substr (str, "\n");

   !if (n) return fun(str);
   
   s = substr (str, 1, n);
   s1 = substr (str, n + 1, strlen(str));
   n = strlen(s);
   
   push_mark ();
   
   while (fun(s))
     {
	% we are matched at end of the line.
	go_right (n);
	if (looking_at(s1)) 
	  {
	     go_left(n);
	     pop_mark(0);
	     return 1;
	  }
	if (dir < 0) go_left (n);
     }
   pop_mark(1);
   0;
}


define search_maybe_again (str, dir)
{
   variable ch;
   while (search_across_lines (str, dir))
     {
	if (EXECUTING_MACRO or DEFINING_MACRO) return 1;
	
	message ("Press RET to continue searching.");
	update (1);
	ch = getkey ();
	if (ch != '\r')
	  {
	     ungetkey (ch);
	     return 1;
	  }
	if (dir > 0) go_right (1);
     }
   return 0;
}

define search_forward ()
{
   variable str, not_found;

   % if (MINIBUFFER_ACTIVE) return;   % the second statement makes this unnecessary
   str = read_mini("Search forward:", LAST_SEARCH, Null_String);
   !if (strlen(str)) return;
   
   push_mark ();
   if (looking_at(str)) go_right(1);
   
   save_search_string (str);
   not_found = not (search_maybe_again (str, 1));
   pop_mark (not_found);
   
   if (not_found) error ("Not found.");
}

define search_backward ()
{
   variable str;
   
   % if (MINIBUFFER_ACTIVE) return;
   str = read_mini("Search backward:", LAST_SEARCH, Null_String);
   !if (strlen(str)) return;
   
   save_search_string (str);
   !if (search_maybe_again (str, -1)) error ("Not found.");
}


define replace_do_replace (str, len)
{
   push_mark ();
   go_right(len);
   del_region ();
   insert (str);
}

define replace_maybe_query (pat, rep, query)
{
   variable n, prompt, doit, err, ch, len;
   variable last, last_ok = 0;

   ERROR_BLOCK 
     {
	if (last_ok) pop_mark (0);
     }
   
   len = strlen (pat);
   
   prompt =  Sprintf ("Replace '%s' with '%s'? (y/n/!/+/q/h)", pat, rep, 2);
   
   while (n = search_across_lines(pat, 1), n)
     {
	!if (query)
	  {
	     replace_do_replace (rep, len);
	     continue;
	  }

	do 
	  {
	     message(prompt);
	     update (0);
	     % regexp_mark_pattern(n);  this might be nice 
	
	     ch = getkey ();
	     if (ch == 'r')
	       {
		  recenter (window_info('r') / 2);
		  go_left(n);
	       }
	     
	  } while (ch == 'r');
	
	switch(ch)
	  { case 'u' and last_ok:
	     pop_mark (1); push_spot ();
	     replace_do_replace (last, strlen(rep));
	     pop_spot ();
	     last_ok = 0;
	  }   
	  { case 'y' :
	     if (last_ok) pop_mark (0); last_ok = 1;
	     push_spot(); push_mark ();
	     go_right (len); last = bufsubstr ();
	     pop_spot (); push_mark ();
	     replace_do_replace(rep, len);
	  }
	  { case 'n' : go_right(1);}
	  { case '+' : replace_do_replace(rep, len);
	               break;
	  }
	  { case '!' :
	     do 
	       {
		  replace_do_replace (rep, len);
	       }
	     while(search_across_lines(pat, 1));
	  }
          { case 'q' : break; }
          {  pop();
	     flush ("y:replace, n:skip, !:replace all, u: undo last, +:replace then quit, q:quit");
	     input_pending (30); pop ();
	  }
     }
   EXECUTE_ERROR_BLOCK;
}

define replace_cmd ()
{
   variable pat, prompt, rep;
   
   pat = read_mini("Replace:", Null_String, Null_String);
   !if (strlen (pat)) return;
   
   prompt = strcat (strcat ("Replace '", pat), "' with:");
   rep = read_mini(prompt, Null_String, Null_String);
   replace_maybe_query (pat, rep, 1);
   message ("done.");   
}

