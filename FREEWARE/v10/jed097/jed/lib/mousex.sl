% mousex.sl
% Mouse routines for use inside an X term or Linux Console running 
% Selection.

% Mouse actions:
%
% left mouse button:
%
%   If the left button is clicked on the status line of a window, the window 
%   will switch to a different buffer.
%
%   If the button is pressed anywhere else in the window, the cursor will be
%   positioned at the location of the click.
% 
% middle button:
%   
%   On status line: split the window
%   Anywhere else:
%    If the region is highlighted, the region will be copied to the 
%    pastebuffer.  This does not delete the region.
%    Otherwise, the editing point will be moved to the location of the mouse
%    and any contents in the pastebuffer will be pasted there.
%
% Right button:
%   
%   On status line: delete the window.
%   Anywhere else:
%    If a region is highlighted, it will be deleted and a copy put in the 
%    pastebuffer.
%    Otherwise, the mark is set and a region will be defined.
%
% Cut/Paste Tips:
%  
%  To mark and manipulate a region do:
%
%   1. Click the LEFT mouse button at the beginning of the region.
%   2. Move the mouse to the end of the region and click the RIGHT
%      mouse button.  The region should now be marked.
%   3. To delete it and copy it to the paste buffer,  press the RIGHT button 
%      again.  To simply copy it without deleting it, press the MIDDLE button.
%
%   4. To paste from the pastebuffer, move the mouse to where you want to 
%      paste and press the MIDDLE button.



% enable mouse 
tt_send ("\e[?9h");


% define hooks to restore the selection state properly (See ../doc/mouse.txt).
define exit_hook ()
{
   tt_send ("\e[?9l");
   exit_jed ();
}

define suspend_hook ()
{
   tt_send ("\e[?9l");
}

define resume_hook ()
{
   tt_send ("\e[?9h");
}

% define keys
setkey ("left_button_down", "\e[M ");
setkey ("middle_button_down", "\e[M!");
setkey ("right_button_down", "\e[M\"");

% returns  values:  line/column of mouse point, number of windows away, and
% flag indicating whether or not the point in on the status line.
% usage:
%   (is_status, nwins, row, column) = whereis_mouse ();
define whereis_mouse (xmouse, ymouse)
{
   variable n = nwindows ();
   variable top, bot, dy, x, y = 0;
   variable is_status = 0;
   
   while (n)
     {
        top = window_info('t');
	bot = window_info('r') + top;
	
	if ((ymouse >= top) and (ymouse < bot))
       	  { 
	     dy = ymouse - (top - 1 + window_line());
	     y = whatline () + dy;
	     break;
	  }
	
      	if (bot == ymouse)
	  {	  
	     is_status = 1;
	     break;
	  }
	
	otherwindow();
	n--;
     }
   
   !if (n) error ("Mouse not in a window.");
   
   x = window_info('c') + xmouse - 1;
   
   loop (n) otherwindow ();
   
   % return :
   return (is_status, nwindows () - n, y, x);
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

define left_button_down ()
{
   variable use_status, x, y, n;
   
   variable x = getkey () - ' ';
   variable y = getkey () - ' ';

   % error (Sprintf ("x: %d, y: %d", x, y, 2));
   (use_status, n, y, x) = whereis_mouse (x, y);
   
   loop (n) otherwindow ();
   if (use_status)
     {
	mouse_next_buffer ();
	return;
     }
   
   goto_line (y);
   goto_column_best_try (x); pop ();
}

define middle_button_down ()
{
   variable use_status, x, y, n;
   
   variable x = getkey () - ' ';
   variable y = getkey () - ' ';

   (use_status, n, y, x) = whereis_mouse (x, y);
   
   loop (n) otherwindow ();
   if (use_status)
     {
	splitwindow ();
	return;
     }
   
   if (markp ()) 
     {	  
	call ("copy_region");
	message ("Region copied.");
	return;
     }
   
   goto_line (y);
   goto_column (x);
   call ("yank");
}
	    
define right_button_down ()
{
   variable use_status, x, y, n;
   
   variable x = getkey () - ' ';
   variable y = getkey () - ' ';

   (use_status, n, y, x) = whereis_mouse (x, y);
   
   loop (n) otherwindow ();
   if (use_status)
     {
	call("delete_window");
	return;
     }
   
   if (markp ()) 
     {
	call ("kill_region");
	return;
     }
   
   call ("set_mark_cmd");
   goto_line (y);
   goto_column (x);
}
