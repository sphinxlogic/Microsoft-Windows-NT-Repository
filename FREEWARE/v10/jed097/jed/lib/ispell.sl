%%
%%  Ispell interface
%%


define ispell()
{
   variable ibuf, buf, file, letters, num_win, old_buf;
   variable word, cmd, p, word, num, n, new_word;
   
#ifdef OS2   
   file = make_tmp_file("jedt");
#else   
   file = make_tmp_file("/tmp/jed_ispell");
#endif   
   letters = "a-zA-Z";
   
   ibuf = " *ispell*";
   buf = whatbuf();
   
   skip_chars(letters); bskip_chars(letters); push_mark(); push_mark();

   n = POINT;
   skip_chars(letters);
   if (POINT == n)
     {
	pop_mark(0); pop_mark(0);
	return;
     }
   
   word = bufsubstr();
   
   pipe_region(strcat("ispell -a > ", file));
   setbuf(ibuf); erase_buffer();
   insert_file(file); pop();
   delete_file(file); pop();
   
   %%
   %% parse output
   %%
   bob();
   if (looking_at_char('@'))   % ispell header
     {
	push_mark();
	eol();
	go_right(1);
	del_region();
     }
   
   if (looking_at_char('*') or looking_at_char('+'))
     {
	message ("Correct");   % '+' ==> is derived from
	bury_buffer (ibuf);
	return;
     }
   
   if (looking_at_char('#')) 
     {
      	bury_buffer (ibuf);
	return (message("No clue."));
     }

   del(); trim(); eol(); trim(); bol();
   if (ffind(":"))
     {
	skip_chars(":\t ");
	push_mark();
	bol();
	del_region();
     }
   
   insert ("(0) ");
   n = 1;
   while (ffind(" "))
     {
	go_left(1);
	if (looking_at_char(',')) del(); else go_right(1);
	trim(); newline();
	insert_char('(');
	insert(string(n));
	insert_char(')');
	insert_single_space();
	++n;
     } 
   
   bob();
   num_win = nwindows();
   pop2buf(buf);
   old_buf = pop2buf_whatbuf(ibuf);   
   
   ERROR_BLOCK 
     {
	sw2buf(old_buf);
	pop2buf(buf);
	if (num_win == 1) onewindow();
	bury_buffer(ibuf);
     }

   set_buffer_modified_flag(0);
   num = read_mini("Enter choice. (^G to abort)", "0", Null_String);
   num = strcat(strcat("(", num), ")");
   
   if (fsearch(num))
     {
	ffind(" "); pop();  trim();
	push_mark(); eol(); trim(); new_word = bufsubstr();
	set_buffer_modified_flag(0);
	sw2buf(old_buf);
	pop2buf(buf);
	bskip_chars(letters); push_mark();
	skip_chars(letters); del_region();
	insert(new_word);
     }
   else 
     {
	sw2buf(old_buf);
	pop2buf(buf);
     }
   if (num_win == 1) onewindow();
   bury_buffer(ibuf);
}

