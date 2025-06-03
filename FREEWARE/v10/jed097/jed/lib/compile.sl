% run compiler in a subshell and parse error messages
% The file compile.dat is a database for various compilers.  Set the
% string 'compile_parse_error_function' to the approriate compiler.
% This file reads the function from compile.dat.

!if (is_defined ("compile_parse_error_function"))
{
   error ("'compile_parse_error_function' needs set.  See jed.rc for info.");
}


% this function pulls the correct function out of the compile.dat file
. (
.   [type] =type
.   [ret] 0 =ret
  
.   "compile.dat" find_jedlib_file
.     { "compile.dat file not found!" error} !if

.   widen bob
.   "%@" type strcat bol_fsearch
.      {
.         push_mark   
. 	"--------" bol_fsearch {1 go_up}{eob} else
. 	narrow
.         evalbuffer
. 	1 =ret
.      } if
.    whatbuf delbuf
.    widen
.    ret
. ) compile_select_compiler

. compile_parse_error_function compile_select_compiler
. { "Compiler type not in database!." error } !if

. "compile_parse_errors"    "^X'"   setkey

define compile_parse_errors ()
{
   variable obuf = "*shell-output*";
   variable cbuf = whatbuf ();
   variable istr, file;
   
   pop2buf (obuf);
   if (markp ()) pop_mark (0);
   eol ();
   if (eobp) message ("No more errors!");
   
   while (eol, not (eobp()))
     {
	if (compile_is_this_line_error ())
	  {
	     (istr, file) = ();
	     message (strcat ("Line ", istr));
	     bol(); call ("set_mark_cmd"); eol ();
	     !if (strlen (istr)) return;
	     find_file (file); pop ();
	     cbuf = whatbuf ();
	     goto_line (integer(istr));
	     bol (); skip_white ();
	     sw2buf (obuf);
	     break;
	  }
	go_down (1);
     }
   
   go_down (1);
   pop2buf (cbuf);
}

. (
.    [b] whatbuf =b
.    "save_buffers" call
.    do_shell_cmd bob % "\n" insert  % so we can assume the top line is junk
.    b pop2buf
.    compile_parse_errors
. ) compile

define compile_previous_error ()
{
   variable buf = "*shell-output*";
   !if (bufferp(buf))
   {
      flush("Did you compile?");
      return;
   }
   pop2buf(buf);
   go_up(2);
   compile_parse_errors();
}


