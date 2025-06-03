% mkdoc.sl ---- load this file from JED_ROOT/lib!!!!
%
%  Extract documentation from .sl and .c files
%  For this to work, a strict format must be followed.
%  In .c files, add_intrinsic and add_variable statements are checked. The
%  syntax prototype is:
%
%        MAKE_INTRINSIC("name", function, TYPE, n)%
%        /* This is a function called name which returns TYPE
%           and takes n arguments. */
%
%  Note that it is assummed that comments cannot be nested.
%  For .sl files, the syntax follows:
%
%     %!% This function pops an integer n off the stack and an object s.
%     %!% It returns n copies of s on the stack.
%     define dupn (n, s)
%     {
%        loop (n) { s; } 
%     }
%
%  Here I search for '%!%' in column 1 which make indicate a doc string and 
%  then search for 'define' preceed by only whitespace.  The '%!%' must start in 
%  column 1.
%
%  There is no ambiguity as long as the doc strings are placed BEFORE the 
%  object they describe.


%!% Extracts doc string from FILE and appends it to BUFFER.
%!% Returns 0 upon failure.
define mkdoc_process_sl_file (file, doc_buf)
{
   variable cbuf, doc_ind = "%!%", chars = "a-zA-Z0-9_$!";
   variable c, ctrlm = 1, name;
   variable has_proto;
   
   !if (read_file(file)) return 0;
   cbuf = whatbuf ();

   bob ();

   while (bol_fsearch (doc_ind))
     {
	setbuf (doc_buf);
	eob ();
	!if (bolp()) newline ();
	push_spot ();
	setbuf (cbuf);

	go_right (3); push_mark ();
	skip_white (); 
	has_proto = looking_at ("Prototype: ");
	while (down(1) and looking_at (doc_ind));
	copy_region (doc_buf);

	
	% now look for what this documents
	forever 
	  {
	     bol (); skip_chars (" \t\n"); c = what_char ();
	     if (looking_at ("variable"))
	       {
		  go_right (strlen("variable"));
		  skip_white (); push_mark ();
		  skip_chars (chars);
		  name = Sprintf ("V%s ", bufsubstr(), 1);
		  break;
	       }
	     
	     if (looking_at ("define"))
	       {
		  go_right (strlen ("define"));
		  skip_white ();
		  push_mark ();
		  skip_chars (chars);
		  !if (has_proto) if (ffind (")")) go_right (1);
		  name = strcat ("F", bufsubstr ());
		  break;
	       }
	     

	     !if (down (1)) error (strcat ("Error parsing ", file));
	  }
	
	%% now we have the name with the appropriate prefix.  Insert it and
	%% replace newlines with ctrlm
	setbuf (doc_buf);
	pop_spot (); insert (name);
	insert_char (ctrlm);
	while (eol, not(eobp()))
	  {
	     del (); insert_char (ctrlm);
	  }
	
	newline ();
	setbuf (cbuf);
     }
   1;
}

. %% same routine for C files.  Looks for MAKE_INTRINSIC and MAKE_VARIABLE
. %% macros.
. (
.   [file doc_buf cbuf] =doc_buf =file
.   [f v name ctrlm c] 1 char =ctrlm
.   
.   file read_file {0 return} !if
.   whatbuf =cbuf
.   
.   bob
.   
.   { 
.     0 =f 0 =v
.     "MAKE_" fsearch
.       {
.         "MAKE_INTRINSIC" looking_at =f
. 	"MAKE_VARIABLE" looking_at =v
.       }{break} else
.      
.     POINT bol skip_white
.     POINT - {eol continue} if %%continue if in middle of line
.     %% found
.     
.     doc_buf setbuf 
.     eob
.     bolp {newline} !if
.     cbuf setbuf 
. 
.     %% formats are MAKE_INTRINSIC(".fun", cfun, TYPE, n)
.     %% and         MAKE_VARIABLE(".var", &c_var, TYPE, flag)
.     %%   where the & may not be present for string variables
.     
.     "\"." ffind {"Corrupt file." error} !if
.     2 right pop push_mark 
.     "\"" ffind {"Corrupt file." error} !if
.     bufsubstr " " strcat =name
.     1 right pop
.     
.     %% if it is a function, get the c function info as well
.     f { 
.         "F" name strcat ":" strcat =name      %%Ffun:
.         ",\t " skip_chars
.         push_mark "$_A-Za-z0-9" skip_chars 
. 	name bufsubstr strcat ":" strcat      %%Ffun:cfun:
. 	",\t " skip_chars 
. 	what_char char strcat ":" strcat      %%Ffun:cfun:T:
. 	"," ffind pop  %% assume ok
. 	", \t" skip_chars push_mark "0-9" skip_chars
. 	bufsubstr strcat =name  %%Whew!!      %%Ffun:cfun:T:0
.       }{
. 	"V" name strcat =name
.       }
.     else
.     
.     doc_buf setbuf name insert push_spot
.     cbuf setbuf
.     
.     %% look for documentation
.     {1 down}
.     {
.         bol skip_white eolp {continue} if
. 	"/*" looking_at {break} !if
. 	2 right pop
.       what_column =c   
. 	push_mark
. 	"*/" fsearch pop  %% I assume it compiled 
. 	
. 	doc_buf copy_region
. 	doc_buf setbuf
. 	pop_spot 
.       push_spot

.        {1 down}
.        { 
.           bol skip_white 
.           "*" looking_at {del skip_white} if
.           what_column bol trim c - {" " insert} loop 
.        }
.        while
   
. 	pop_spot 
. 	ctrlm insert
	
. 	{eol eobp not}
.        {del ctrlm insert} while
. 	newline
. 	cbuf setbuf
. 	break
.     } while
.   } forever 
.   1
. ) mkdoc_process_c_file
. 
. 
. %%% Make documentation for set of files.  Function takes as top argument
. %%% the name of docfile to be produced followed by number n of files
. %%% then followed by the n filenames.
. (
.   [docfile] =docfile
.   [argc argv] =argc
.   [docbuf cbuf]  whatbuf =cbuf
.   [type]
.   
.   docfile setbuf erase_buffer
.   whatbuf =docbuf
.   cbuf setbuf
.   
.   argc 
.     { 
.       =argv
.       argv file_type =type
.       "processing " argv strcat flush
.       type
.         {"c" strcmp not type "C" strcmp not or 
. 	 type "ci" strcmp not or : 
. 	   argv docbuf mkdoc_process_c_file 
. 	     { argv " could not be processed!" strcat error } !if
. 	}
.         {"sl" strcmp not type "SL" strcmp not or : 
. 	   argv docbuf mkdoc_process_sl_file 
. 	     { argv " could not be processed!" strcat error } !if
. 	}
. 	{" File type not supported!" strcat error}
.       switch
.       whatbuf docbuf strcmp {whatbuf delbuf} if
.     }
.   loop
.   
.   
.   docbuf setbuf 
.   docfile write_buffer {"Unable to write doc file!" error} !if
.   0 set_buffer_modified_flag docbuf delbuf
.   cbuf setbuf
. ) mkdoc
. 
. _stkdepth =$1
. "site.sl" "buf.sl" "help.sl" "util.sl" "dired.sl" "most.sl" "fortran.sl"
. "misc.sl" "tex.sl"
. "../src/intrin.c" "../src/xterm.c"
. "../../slang/src/slang.c"  "../../slang/src/slstd.c"  
. "../../slang/src/slmath.c"
#ifdef UNIX
. "../../slang/src/slunix.c"
#endif
. _stkdepth $1 -
.  % number of files listed above   
.   
. "jed_funs.hlp" mkdoc
. exit_jed

