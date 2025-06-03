%
%  This function executes a query-replace across all buffers attached to
%  a file.
%
define replace_across_buffer_files ()
{
   variable cbuf = whatbuf ();
   variable n = buffer_list ();
   variable buf, file, flags;
   variable pat, rep;
   
   pat = read_mini ("Replace:", Null_String, Null_String);
   !if (strlen (pat)) return;
   rep = read_mini ("Replace with:", Null_String, Null_String);
   
   push_spot ();		       %  save our location
   ERROR_BLOCK 
     {
	sw2buf (cbuf);
	pop_spot ();
	loop (n) pop ();               %  remove buffers from stack
     }
   
   while (n)
     {
	buf = ();  n--;
	
	% skip special buffers
	if ((buf[0] == '*') or (buf[0] == ' ')) continue;

	sw2buf (buf);
	
	(file,,,flags) = getbuf_info ();
	
	% skip if no file associated with buffer, or is read only
	!if (strlen (file) or (flags & 8)) continue;  
	
	% ok, this buffer is what we want.
	
	push_spot ();
	ERROR_BLOCK
	  {
	     pop_spot ();
	  }
	
	bob ();
	replace_maybe_query (pat, rep, 1);
	pop_spot ();
     }
   
   EXECUTE_ERROR_BLOCK;
   message ("Done.");
}

	
	
	
   
