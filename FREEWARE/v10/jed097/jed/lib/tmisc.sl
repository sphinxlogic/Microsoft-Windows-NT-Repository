%
%  Miscellaneous text functions
%

define text_justify_line ()
{
   variable space, min, max, r, count;
   count = 100;
   space = char (' ');
   push_spot(); bol(); skip_white();
   if (eolp) {pop_spot(); return; }
   min = what_column();
   eol(); trim();
   max = what_column();
   while ((max < WRAP) and count)
     {
	r = random(0, max);
	--count;
	if (r < min) continue;
	goto_column(r); skip_white();
	if (ffind(space)){ insert_single_space(); ++max; eol()}
     }
  pop_spot();
}


define format_paragraph_hook()
{
   variable n;
   push_spot();
   
   backward_paragraph();
   n = whatline();
   forward_paragraph ();
   go_up(2);
   if (n - whatline() > 0) {pop_spot(); return; }
   push_mark();
   backward_paragraph();
   if (whatline() != 1) go_down (1);
   narrow();
   bob();
   forever
     {
	text_justify_line();
	!if (down (1)) break;
     }
   widen();
   pop_spot();
}

