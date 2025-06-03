%
% JED mouse interface 
%
% These routines assume a 3 button mouse
%  
%          left :  set point to mouse point
%        middle :  paste from cut buffer moving point to mouse point
%         right :  copy region from point to mouse point to cut buffer
%     drag left :  mark a region and copy it to cut buffer
%    drag right :  mark a region and copy it to cut buffer
%    shift left :  split window mouse is in
%  control left :  delete window mouse is in
% control right :  kill region between point and mouse point to cut buffer
%
%  Other buttons are undefined.
%

setkey ("mouse_set_point_open",		"^[^@Dl");   % left button down
setkey ("mouse_set_point_close",	"^[^@Ul");   % left button up
setkey ("mouse_yank_cutbuffer",		"^[^@Dm");   % middle down
setkey ("mouse_mark_and_copy",		"^[^@Dr");   % right down
setkey ("mouse_set_point_close",	"^[^@Ur");   % right up
setkey ("mouse_null",			"^[^@Um");   % middle up

% dragging
setkey ("mouse_drag",			"^[^@^@l");  % left dragging
setkey ("mouse_drag",			"^[^@^@r");  % right dragging
setkey ("mouse_null",			"^[^@^@^L"); % C-left dragging
setkey ("mouse_drag",			"^[^@^@^R"); % C-right dragging
setkey ("mouse_null",			"^[^@^@L");  % S-left dragging
setkey ("mouse_null",			"^[^@^@R");  % S-right dragging

% shifted
setkey ("mouse_split_window",		"^[^@DL");   % Shift-left button down
setkey ("mouse_null",			"^[^@UL");   % left button up
setkey ("mouse_yank_from_jed",		"^[^@DM");   % middle down
setkey ("mouse_null",			"^[^@DR");   % right down
setkey ("mouse_null",			"^[^@UR");   % right up
setkey ("mouse_null",			"^[^@UM");   % middle up

% ctrl
setkey ("mouse_delete_window",		"^[^@D^L");  % left button down
setkey ("mouse_null",			"^[^@U^L");  % left button up
setkey ("mouse_null",			"^[^@D^M");  % middle down
setkey ("mouse_kill_region",		"^[^@D^R");  % right down
setkey ("mouse_set_point_close",	"^[^@U^R");  % right up
setkey ("mouse_null",			"^[^@U^M");  % middle up

define mouse_null ();

% loop through windows moving the point to the mouse point
% if same_window is false, and mouse is on status line, call status_fun
% if use_status_fun is non-zero.
define mouse_point_mouse (force, same_window, push, use_status_fun, status_fun)
{
   variable n = nwindows ();
   variable top, bot, dy, col, want_col;
   
   while (n)
     {
        top = window_info('t');
	bot = window_info('r') + top;
	if ((MOUSE_Y >= top) and (MOUSE_Y < bot))
       	  { 
	     if (push) call("set_mark_cmd");
	     dy = MOUSE_Y - (top - 1 + window_line());
	     if (dy > 0) 
	       {       
	      	  dy -= down(dy);
	     	  eol();
	      	  if (force) loop (dy) newline();
	       }                                                   
	     else go_up(- dy);
	     
	     eol();
	     col = what_column ();
       	     want_col = window_info('c') + MOUSE_X - 1;
	     !if (force or (want_col <= col)) want_col = col;
	     goto_column_best_try(want_col); pop();
	     return 1;
	  }
	
	if (same_window) 
	  {
	     if (push) call("set_mark_cmd");
       	     if (MOUSE_Y >= bot)
       	       {
       	     	  go_down(MOUSE_Y - bot);
       	       }
       	     else
       	       {
       		  go_up(top - MOUSE_Y);
       	       }
	     x_warp_pointer ();
      	     return 1;
      	  }
	
      	if ((bot == MOUSE_Y) and (use_status_fun)) 
	  {
	     status_fun ();
	     return 0;
	  }
	
	otherwindow();
	n--;
     }
   error ("Mouse not in a window.");
}
      
variable Mouse_Drag_Mode = 0;
variable Mouse_Buffer = " *Mouse buffer*";
variable Mouse_Delete_Region = 0;
define copy_kill_to_mouse_buffer ()
{
   variable cbuf = whatbuf ();
   variable pnt, n;
   % 
   % We are not going to copy to the pastebuffer if the region is nil
   %
   n = whatline(); pnt = POINT; 
   push_spot();
   pop_mark(1);
   if ((whatline() == n) and (POINT == pnt)) 
     {
	pop_spot();
	return;
     }
   push_mark();
   pop_spot();
   
   setbuf(Mouse_Buffer);
   erase_buffer ();
   setbuf (cbuf);
   

   if (Mouse_Delete_Region) pop(dupmark());
   dupmark();  pop();		       %/* for cut buffer */
   x_copy_region_to_cutbuffer ();
   copy_region(Mouse_Buffer);
   if (Mouse_Delete_Region) 
     {
	Mouse_Delete_Region = 0;
	del_region();
     }
   message ("region copied.");
}


define mouse_next_buffer ()
{
   variable n, buf, cbuf = whatbuf ();
   
   n = buffer_list ();		       %/* buffers on stack */
   loop (n)
     {
	=buf;
	n--;
	if (buf[0] == ' ') continue;
	if (buffer_visible (buf)) continue;
	sw2buf (buf);
	loop (n) pop ();
	return;
     }
   error ("All buffers are visible.");
}

	


define mouse_set_point_open ()
{
   Mouse_Drag_Mode = 0;
   mouse_point_mouse (0, 0, 0, 1, &mouse_next_buffer); pop ();
}

define mouse_set_point_close ()
{
   if (Mouse_Drag_Mode) 
     {
	copy_kill_to_mouse_buffer ();
	Mouse_Drag_Mode = 0;
     }
   else if (MOUSE_BUTTON == 3)
     {
	dupmark(); pop();
	copy_kill_to_mouse_buffer ();
	pop_mark(1);
     }
}

define mouse_yank_from_jed ()
{
   mouse_point_mouse (0, 0, 0, 0, 0); pop ();
   if (bufferp(Mouse_Buffer)) insbuf(Mouse_Buffer);
}

define mouse_yank_cutbuffer ()
{
   if (mouse_point_mouse (0, 0, 0, 1, &splitwindow))
     {
	x_insert_cutbuffer (); pop();
	return;
     }
}

define delete_window ()
{
   call("delete_window");
}

define mouse_mark_and_copy ()
{
   Mouse_Drag_Mode = 0;
   mouse_point_mouse (0, 0, 1, 1, &delete_window); pop ();
}

   
define mouse_drag ()
{
   !if (Mouse_Drag_Mode)
     {
	call ("set_mark_cmd");
	Mouse_Drag_Mode = 1;
     }
   mouse_point_mouse (0, 1, 0, 0, 0); pop ();
   update (not(input_pending(0)));
}

   
define mouse_kill_region ()
{
   Mouse_Delete_Region = 1;
   mouse_mark_and_copy (0, 0, 1); 
}


define mouse_split_window ()
{
   mouse_point_mouse (0, 0, 0, 0, 0); pop ();
   splitwindow();
}

define mouse_delete_window ()
{
   mouse_point_mouse (0, 0, 0, 0, 0); pop ();
   delete_window ();
}

   
