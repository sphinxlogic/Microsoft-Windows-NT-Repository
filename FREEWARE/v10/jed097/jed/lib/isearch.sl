%% isearch.sl
%% Routines here perform incremental forward/backward searches
%% 
%% ESCAPE key aborts the search
%% ENTER key finds next match
%% DELETE key deletes last character entered and returns to previous point
%%
%%  "isearch_forward"	"^FIf"   setkey  %(sample bindings)
%%  "isearch_backward"	"^FIb"   setkey

. [Isearch_Last_Search] Null_String =Isearch_Last_Search


%% This code fragment checks to see what the isearch keys are bound to
. [Isearch_Forward_Char] 19 =Isearch_Forward_Char  %^S
. [Isearch_Backward_Char] 18 =Isearch_Backward_Char  %^R

. "isearch_forward" which_key
. {
.   dup strlen 2 == 
.     { dup 
.       int '^' == 
.         { 2 1 substr int
. 	  dup 'S' != {'@' - =Isearch_Forward_Char}{pop} else
. 	}{pop}
.       else
.     }{pop} else
. } loop

. "isearch_backward" which_key
. {
.   dup strlen 2 == 
.     { dup 
.       int '^' == 
.         { 2 1 substr int
. 	  dup 'Q' != {'@' - =Isearch_Backward_Char}{pop} else
. 	}{pop}
.       else
.     }{pop} else
. } loop


. (
.   [dir] =dir
.   dir 0 > 
.     {"Search:"}
.     {"Search Backwards:"}
.   else Isearch_Last_Search Null_String read_mini 
.   =Isearch_Last_Search
  
.   Isearch_Last_Search 
.   dir 0 > 
.     {fsearch}{bsearch} else
.   {"Not Found." error} !if
. ) isearch_simple_search

. (
.    [str] =str
.    [n] str strlen =n
.    n {
.        str 1 n 1 - substr =str
.      } if
.    str
. ) isearch_del

. (
.   [dir] =dir
.   [prompt] 
.   [str] Null_String =str
.   [c m int first] push_mark 1 =m   1 =first
  
  % This is tricky.  I am leaving a 1 or a 0 on the stack which indicates
  % whether or not a character is attached to a mark.  The number of ones
  % and zeros on the stack should match the value of m.
  % Since a mark was pushed and not attached to a character, push 0
.   0
.   { m { pop 0 pop_mark} loop } ERROR_BLOCK  %% make sure we pop marks
  
.   {
.      "Isearch " dir 1 == {"forward: "}{"Backward: "} else strcat =prompt 
.      prompt str strcat message
.      push_spot
.      dir 0 > str looking_at and
.      {
.        str strlen right pop
.      } if
.      0 update
.      pop_spot
            
.      getkey =c
.      c { 27 == first and : dir isearch_simple_search break}
.        { 27 == : break}
.        { 13 == c Isearch_Forward_Char == or : % return or ^S
. 		  push_mark ++m 0             % mark not attached to char!
.                   dir 1 == { 1 right pop } if
.                   str strlen {Isearch_Last_Search =str} !if
.        }  
.        { Isearch_Backward_Char == :  %^R
.                  dir 0 > {   
.                              m {pop 0 pop_mark} loop
. 		              -1 =dir push_mark 0 1 =m
.                           }   
.                           {
.       		       push_mark ++m 0             % mark not attached to char!
.                              str strlen {Isearch_Last_Search =str} !if
.                           }
.                          else
.        }
.        { 127 == : 
                  % pop last integer off stack to see whether last mark
		  % pushed was result of a char
. 		  m 1 == or { str isearch_del =str } if
.                   m {--m 1 pop_mark} if
. 		  m {1 =m push_mark 0} !if
. 		  continue
.        }
.        { 32 < : break}   % terminate search
.        { char str exch strcat =str
.          1 ++m push_mark  % 1 pushed as well
         %%str looking_at {++m push_mark 1} !if   
         %%m 1 + =m push_mark
	 %%str looking_at 1 dir == and {1 right pop} if
.        }
.      switch
.      0 =first 

.      dir 0 < m 1 > and str looking_at c ' ' >= and {continue} if
   
.      str dir 1 == {fsearch}{bsearch} else
.      {
.        str " not found." strcat flush beep
.        10 input_pending pop
.        c 13 == {1 left pop} 
.                {str isearch_del =str 0 pop_mark --m pop}   %pop char flag
. 	     else
.      } !if
     
.   } forever
  
.   EXECUTE_ERROR_BLOCK
.   str strlen {str =Isearch_Last_Search} if
.   dir 1 == {str strlen right pop} if
.   "Done." message
.   { 0 input_pending } { getkey pop } while
   
. ) isearch_dir

. ( 1 isearch_dir )  isearch_forward 
. ( -1 isearch_dir ) isearch_backward

  
