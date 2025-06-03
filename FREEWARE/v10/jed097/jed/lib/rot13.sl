%%
%%  rot13.sl---- rotates text by 13 characters
%%
define do_rot13 (ch)
{
   variable delta = 0;
   if (ch > 'Z') 
     {
	delta = 32;
	ch -= 32;
     }
   
   if ((ch < 'A') or (ch > 'Z'))
     {
	go_right(1);
	return;
     }
   
   if (ch > 'M') ch -= 13;
   else ch += 13;
   del ();
   insert_char (ch + delta);
}

. ( 
.   [n r1 c1 c2]
.   markp { "Set mark!" error } !if
  
.   push_spot
  
.   what_column =c2
.   whatline "exchange" call whatline =r1
.   r1 - =n  %number of lines - 1
.   what_column =c1
  
.   {n 0 ==} {c1 c2 ==} andelse {"bad region!" error} if
  
.   n 0 < { n chs =n 
.           r1 n - =r1
. 	  c1 c2 =c1 =c2
. 	} if

.   n 0 == { c1 c2 > { c1 c2 =c1 =c2  "hmm" message } if } if
  
.   r1 goto_line c1 goto_column
 
.   n { 
.        {eolp 0 ==}
.          {
. 	    what_char do_rot13
. 	 } while
.        1 down pop bol
.     } loop
    
  %% take care of final line
    
.     {what_column c2 <}
.      {
.        what_char do_rot13
.      } while
.   0 pop_mark   
.   pop_spot
. ) rot13


  
