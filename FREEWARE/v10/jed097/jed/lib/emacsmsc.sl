% More emacs functions.  This file is autoloaded upon demand.

define find_buffer_other_window ()
{  
   variable n, buf, trybuf = Null_String;
   variable ch;
   
   n = buffer_list();
   loop (n)
     { 
	=buf; n--;
	ch = buf[0];
	if ((ch == ' ') or (ch == '*') or 
	    not(strcmp (whatbuf (), buf)))
	  continue;
	trybuf = buf;
	break;
     }
   loop (n) pop ();
   
   trybuf = read_with_completion ("Switch to buffer:",
				  trybuf, Null_String, 'b');

   if (strlen (trybuf)) pop2buf (trybuf);
}

define find_file_other_window ()
{
   variable file;

   file = read_file_from_mini ("Find file:");
   !if (strlen(extract_filename(file))) return;
   
   !if (read_file(file)) message ("New file.");
   pop2buf (whatbuf());
}

define find_alternate_file ()
{
   variable file;

   file = read_file_from_mini ("Find alternate file:");
   !if (strlen(extract_filename(file))) return;
   
   delbuf (whatbuf());
   !if (find_file (file)) message ("New file.");
}

