%
% dabbrev.sl - Author: Adrian Savage (A.F.Savage@bradford.ac.uk)
%              Date:   May 1994
%
% This file may be freely distributed and modified
%
% find the previous match for the current word and add it to the current word

!if (is_defined("Dabbrev_Num_Matches"))
{
   variable Dabbrev_Num_Matches = 0;
   variable Dabbrev_Last_Matches = create_array ('s', 10, 1);
}


define dabbrev_is_already_matched (str, old)
{
   variable i;
   !if (strcmp(str, old)) return 1;

   _for (0, Dabbrev_Num_Matches - 1, 1)
     {
	=i;
	!if (strcmp (str, Dabbrev_Last_Matches[i])) return 1;
     }
   if (Dabbrev_Num_Matches == 10) return 0;
   
   Dabbrev_Last_Matches [Dabbrev_Num_Matches] = str;
   Dabbrev_Num_Matches++;
   return 0;
}


define dabbrev_skip_word_chars (dir)
{
   if (dir > 0)
     {
	do 
	  {
	     skip_chars ("_");
	     skip_word_chars ();
	  }
	while (looking_at_char ('_'));
	return;
     }
  
   forever
     {
	bskip_word_chars ();
	if (bolp()) break;
	go_left(1);
	!if (looking_at_char ('_'))
	  {
	     go_right(1);
	     break;
	  }
     }
}

define dabbrev_expand_dir (dir)
{
   variable hilite_wrd, num_spots = 0, num_marks = 0, ndel, 
     complete, fun, use_call, success = 0, ch, search_fun;
   
   ERROR_BLOCK 
     {
	loop (num_spots) pop_spot ();
	loop (num_marks) pop_mark (1);
	!if (success)
	  {
	     push_mark ();
	     dabbrev_skip_word_chars(-1);
	     del_region ();
	     insert (hilite_wrd);
	  }
     }
   
   
   push_mark ();  num_marks++;	       % remember initial position
   
   push_mark();			       % mark end of this word  
   dabbrev_skip_word_chars(-1);	       % jump to beginning of word    
   hilite_wrd = bufsubstr();	       % get the word  
   push_mark ();  num_marks++;	       % remember beginning position

   ndel = strlen (hilite_wrd);
   
   search_fun = &bsearch;
   if (dir) 
     {
	search_fun = &fsearch;
	go_right (ndel);
     }
   
   
   forever 
     {
	!if (search_fun (hilite_wrd))
	  {
	     error (strcat ("No more completions for ", hilite_wrd));
	  }
	
	
	dabbrev_skip_word_chars (-1);
	!if (looking_at(hilite_wrd)) 
	  {
	     if (dir) dabbrev_skip_word_chars (1);
	     continue;
	  }
	
	push_mark ();		       % set a mark there  
	dabbrev_skip_word_chars(1);    % skip to end of	word      
	complete = bufsubstr();	       % get the completed word
	
	!if (dir) dabbrev_skip_word_chars (-1);
	if (dabbrev_is_already_matched(complete, hilite_wrd)) continue;
	
	push_spot (); num_spots++;     %  remember how far we got
	
	pop_mark(1);		       %  back to beginning
	push_mark;
	
	insert (complete);
	deln (ndel);		       %  delete word
	ndel = strlen (complete);
	
	update (1);		       %  force update
	
	ch = getkey (); ungetkey (ch);
	
	fun = get_key_function ();
	if (strlen (fun))
	  {
	     use_call = ();
	  }

	if (strcmp (fun, "dabbrev")) break;   %  use hit another key, done
	variable zzzz = create_user_mark ();
	pop_spot (); num_spots--;      %  returns and continue looking
     }
   
   % we get here only when we are finished
   success = 1;
   
   EXECUTE_ERROR_BLOCK;		       %  pop spots/marks
   
   update (0);
   !if (strlen (fun)) return;
   if (use_call) call(fun); else eval(fun);
}

define dabbrev ()
{
   Dabbrev_Num_Matches = 0;
   ERROR_BLOCK
     {
	_clear_error ();
	dabbrev_expand_dir (1);
     }
   dabbrev_expand_dir (0);
}
