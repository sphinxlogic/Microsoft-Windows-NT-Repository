[inherit('PQM_OBJ:GLOBALDEF'
	,'SYS$LIBRARY:STARLET'
	,'SYS$LIBRARY:PASCAL$SMG_ROUTINES')]
Module BROWSER (output);
{*******************************************************************************

  	BROWSER			Manage a Browse List

  This module contains routines to manipulate a Browse List.  This is a data
  structure which describes a list of items displayed on the screen.


  	Created 8-Nov-2000 by J.Begg, VSM Software Services Pty Ltd.
  	Copyright © 2000 VSM Software Development.  All rights reserved.


*******************************************************************************}

[global]
Function BROWSE_CREATE (r : integer;					{ No. of rows for virtual display }
			v : integer;					{ No. of rows available on screen }
			h : packed array [l0..h0:integer] of char;	{ Text for column headings        }
			var b : browse_list) : integer;
{
    Create a browse list by creating the virtual display and setting the
    associated controls to known values.
}
var ret_status : integer;
begin
b := ZERO;
with b do
    begin

    { Create the column headings display }
    ret_status := SMG$CREATE_VIRTUAL_DISPLAY (1, pasteboard_cols, headings,, SMG$M_BOLD+SMG$M_UNDERLINE);
    if not odd(ret_status) then return ret_status;
    SMG$PUT_LINE (headings, h);

    { Create the contents display, ensuring it's large enough to fill the available area }
    if r < v then r := v;
    ret_status := SMG$CREATE_VIRTUAL_DISPLAY (r, pasteboard_cols, display);
    if not odd(ret_status) then return ret_status;
    rows := r;
    cols := pasteboard_cols;
    current_row := 1;

    { Create the viewport }
    viewport_start := current_row;
    viewport_rows  := v;
    ret_status := SMG$CREATE_VIEWPORT (display, viewport_start, 1, v, cols);
    if not odd(ret_status) then return ret_status;

    { Set the rendition for the current row to reverse video }
    SMG$CHANGE_RENDITION (display, current_row, 1, 1, cols, 0, SMG$M_REVERSE);

    end;

BROWSE_CREATE := ret_status;
end; { BROWSE_CREATE }



[global]
Procedure BROWSE_UP (var b : browse_list);
{
   Move 'up' the browse list by highlighting the row immediately above the
   current row.
}
begin
with b do
    begin
    if invalidated then
	{ Force a reset of the viewport }
	begin
	if current_row = 1 then
	    BROWSE_SELECT_ROW (b, 1)
	else
	    BROWSE_SELECT_ROW (b, current_row-1);
	return
	end;
    if current_row = 1 then return;	{ Nothing to do. }

    SMG$BEGIN_DISPLAY_UPDATE (display);

    { Restore the rendition of the current row }
    SMG$CHANGE_RENDITION (display, current_row, 1, 1, cols, 0, 0);

    { Set the current row to the next row up, and set its rendition }
    current_row := current_row - 1;
    SMG$CHANGE_RENDITION (display, current_row, 1, 1, cols, 0, SMG$M_REVERSE);

    { Check that the new current row is within the viewport }
    if current_row < viewport_start then
	begin
	SMG$SCROLL_VIEWPORT (display, SMG$M_DOWN);
	viewport_start := viewport_start - 1;
	end;

    SMG$END_DISPLAY_UPDATE (display);
    end;
end;  { BROWSE_UP }



[global]
Procedure BROWSE_DOWN (var b : browse_list);
{
   Move 'down' the browse list by highlighting the row immediately below the
   current row.
}
begin
with b do
    begin
    if invalidated then
	{ Force a reset of the viewport }
	begin
	if b.current_row = rows then
	    BROWSE_SELECT_ROW (b, rows)
	else
	    BROWSE_SELECT_ROW (b, current_row + 1);
	return
	end;
    if current_row = rows then return;	{ Nothing to do. }

    SMG$BEGIN_DISPLAY_UPDATE (display);

    { Restore the rendition of the current row }
    SMG$CHANGE_RENDITION (display, current_row, 1, 1, cols, 0, 0);

    { Set the current row to the next row down, and set its rendition }
    current_row := current_row + 1;
    SMG$CHANGE_RENDITION (display, current_row, 1, 1, cols, 0, SMG$M_REVERSE);

    { Check that the new current row is within the viewport }
    if current_row = (viewport_start + viewport_rows) then
	begin
	SMG$SCROLL_VIEWPORT (display, SMG$M_UP);
	viewport_start := viewport_start + 1;
	end;

    SMG$END_DISPLAY_UPDATE (display);
    end;
end;  { BROWSE_UP }



[global]
Procedure BROWSE_SELECT_ROW (var b : browse_list; selected_row : integer);
{
    Highlight the indicated row, and make sure it is within the viewport
}
var new_start : integer;
    change_v  : boolean value FALSE;
begin
with b do
    begin
    SMG$BEGIN_DISPLAY_UPDATE (display);

    { Restore the rendition of the current row }
    if current_row > 0 then SMG$CHANGE_RENDITION (display, current_row, 1, 1, cols, 0, 0);

    { Set the current row to the selected row, and set its rendition }
    current_row := selected_row;
    SMG$CHANGE_RENDITION (display, current_row, 1, 1, cols, 0, SMG$M_REVERSE);

    { Check that the new current row is within the viewport }
    if rows = viewport_rows then
	new_start := 1
    else
	new_start := current_row - (viewport_rows div 2);
    if current_row < viewport_start then
	begin
	if new_start < 1 then new_start := 1;
	change_v := TRUE
	end
    else if current_row >= (viewport_start + viewport_rows) then
	begin
	if (new_start + viewport_rows - 1) > rows then new_start := rows - viewport_rows + 1;
	change_v := TRUE
	end;
    if change_v or invalidated then
	begin
	viewport_start := new_start;
	SMG$CHANGE_VIEWPORT (display, viewport_start, 1, viewport_rows, cols);
	invalidated := FALSE
	end;

    SMG$END_DISPLAY_UPDATE (display);
    end;
end;



[global]
Procedure BROWSE_ERASE (var b : browse_list; new_rows : [truncate] integer);
{
    Clear the browse list and remove any highlighting.
}
begin
with b do
    begin
    SMG$BEGIN_DISPLAY_UPDATE (display);
    SMG$ERASE_DISPLAY (display);
    SMG$CHANGE_RENDITION (display, 1, 1, rows, cols, 0, 0);
    if present(new_rows) then
	SMG$CHANGE_VIRTUAL_DISPLAY (display, max(viewport_rows,new_rows));
    current_row := 0;
    viewport_start := 1;
    SMG$CHANGE_VIEWPORT (display, viewport_start, 1, viewport_rows, cols);
    invalidated := FALSE;
    SMG$END_DISPLAY_UPDATE (display);
    end;
end;  { BROWSE_ERASE }


END.
