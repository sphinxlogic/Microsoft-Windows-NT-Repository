[inherit('PQM_OBJ:GLOBALDEF'
	,'SYS$LIBRARY:STARLET'
	,'SYS$LIBRARY:PASCAL$CLI_ROUTINES'
	,'SYS$LIBRARY:PASCAL$LIB_ROUTINES'
	,'SYS$LIBRARY:PASCAL$SMG_ROUTINES'
)]
Program PRINT_QUEUE_MANAGER (input, output);
{*******************************************************************************

  	PRINT_QUEUE_MANAGER	  Screen-based interface to Printer Queues

  This program provides a full-screen interface for managing printer queues
  and the jobs in them.  The following functions are provided:

  Queues:  Display all queues
  	   Start, Pause, Stop or Reset a queue
  	   Change the mounted form
  	   "Cleanse" the queue -- delete all completed jobs older than a
           specified age, or whose files have been deleted.

  Jobs:    Display all jobs in a given queue
  	   Start, Stop, Delete, Release or Requeue a job
  	   Reprint selected pages from a job
  	   Change the form type
  	   Change the mounted form type for the job's queue
	   Display the file in a job
  	   "Cleanse" the queue (see definition above).

  A similar display format is used for both queues and jobs.  A panel at the
  top of the screen shows detailed information about the currently selected
  queue or job; the rest of the screen shows a list of queues or jobs as
  appropriate.  The "current" queue or job is indicated by being displayed in
  reverse video.

  PQM is intended to be invoked as a foreign command.  The following command
  line qualifiers are provided:

     /BROWSER=command

	Specifies a DCL command to invoke to display the contents of a file.
	The command will be invoked with a single parameter being the file to
	display.

	If this qualifier is not specified PQM will attempt to translate the
	logical name PQM_BROWSER and use that instead.  If the logical name
	is required, it will be translated each time PQM attempts to display
	a file.

	If neither /BROWSER nor the PQM_BROWSER logical are specified, an error
	message will be displayed when attempting to display a file.

     /INITIAL_SCREEN=QUEUE=queue
     /INITIAL_SCREEN=JOBS=queue

	/INITIAL_SCREEN=QUEUE=queue causes PQM to start up witht the specified
	queue highlighted.

	/INITIAL_SCREEN=JOBS=queue causes PQM to go straight to the JOBS
	display, displaying the jobs (if any) in the specified queue.

  	Default is to start PQM with the QUEUES display, highlighting the
        first queue in the list.

     /[NO]SERVER_SYMBIONT

  	[Do not] display server queues.  The default is to display all output
        queues.

     /UPDATE=n
     /NOUPDATE

  	Automatically refresh the screen every 'n' seconds.
  	/NOUPDATE suppresses automatic screen updates.
  	The default is to update the screen every 15 seconds.

  All of the above qualifiers are optional.
	

  	Created November 2000 by J.Begg, VSM Software Services Pty Ltd.
  	Copyright © 2000 VSM Software Development.  All rights reserved.

*******************************************************************************}

Var
	ret_status		: integer;
	pasteboard_type		: unsigned;
	status_display		: [volatile] unsigned;
	exit_handler_block	: packed record
				      flink	: [long] integer;
				      handler	: [long] unsigned;
				      arg_count	: [long] unsigned;
				      exit_sts  : [long] unsigned;
				  end value [arg_count:1; otherwise ZERO];
	foreign_command		: varying [1024] of char;
	browser			: varying [512] of char value '';	{ Command verb to display file }
	initial_queue		: varying [QUEUE_NAME_SIZE] of char value '';
	start_with_jobs_display	: boolean value FALSE;
	ignore_server_queues	: boolean value FALSE;

	PQM_FOREIGN		: [external] integer;

{*******************************************************************************
*	Foreign Command Line parsing					       *
*******************************************************************************}	

    Procedure Parse_Command_Line;

    type signal_array	= array [0..9] of integer;
	 mechanism_array = array [0..(size(CHF2$TYPE)-4 div 4)] of integer;

    var ret_status	: integer;
	c		: integer;
	cli_buffer	: varying [1024] of char;

	[asynchronous]
	function cli_handler (var sigargs : signal_array;
			      var mechargs : mechanism_array) : integer;
	begin
	$PUTMSG (sigargs,, 'PQM');
	cli_handler := SS$_CONTINUE;
	end;
	
    begin
    establish(cli_handler);
    ret_status := CLI$DCL_PARSE ('PQM ' + foreign_command, PQM_FOREIGN);
    revert;
    if odd(ret_status) then
	begin
	if CLI$PRESENT ('BROWSER') = CLI$_PRESENT then
	    begin
	    ret_status := CLI$GET_VALUE ('BROWSER', %descr browser);
	    if not odd(ret_status) then LIB$SIGNAL (ret_status);
	    end;
	if CLI$PRESENT ('INITIAL_SCREEN.QUEUE') = CLI$_PRESENT then
	    begin
	    ret_status := CLI$GET_VALUE ('INITIAL_SCREEN.QUEUE', %descr initial_queue);
	    if not odd(ret_status) then LIB$SIGNAL (ret_status)
	    end
	else if CLI$PRESENT ('INITIAL_SCREEN.JOBS') = CLI$_PRESENT then
	    begin
	    ret_status := CLI$GET_VALUE ('INITIAL_SCREEN.JOBS', %descr initial_queue);
	    if not odd(ret_status) then LIB$SIGNAL (ret_status);
	    start_with_jobs_display := TRUE
	    end
	else
	    initial_queue := '';
	for c := 1 to length(initial_queue) do
	    if (initial_queue[c] >= 'a') and (initial_queue[c] <= 'z') then
		initial_queue[c] := chr(ord(initial_queue[c])-ord('a')+ord('A'));

	ignore_server_queues := CLI$PRESENT ('SERVER_SYMBIONT') = CLI$_NEGATED;

	ret_status := CLI$PRESENT ('UPDATE');
	if (ret_status = CLI$_DEFAULTED) or (ret_status = CLI$_PRESENT) then
	    begin
	    ret_status := CLI$GET_VALUE ('UPDATE', %descr cli_buffer);
	    if not odd(ret_status) then LIB$SIGNAL (ret_status);
	    readv(cli_buffer, read_timeout)
	    end
	else if (ret_status = CLI$_NEGATED) then read_timeout := 0;
	end
    end;    

{*******************************************************************************
*	EXIT HANDLER	Remove the pasteboard without erasing the screen       *
*******************************************************************************}

    [asynchronous]
    Function Exit_Handler (var sts : integer) : integer;
    begin
    SMG$SET_CURSOR_ABS (status_display, 1);
    SMG$SET_CURSOR_MODE (pasteboard_id, SMG$M_CURSOR_ON);
    SMG$DELETE_PASTEBOARD (pasteboard_id, 0);
    LIB$PUT_OUTPUT(''(0));
    Exit_Handler := sts;
    end;

{*******************************************************************************
*	Utility Routines						       *
*******************************************************************************}

    [global]
    Procedure UPDATE_TIME_DISPLAY;
    {
	Update the date & time in the time display which appears at the top of
	the screen.
    }
    var date_string,
	time_string  : packed array [1..11] of char;
    begin
    date(date_string);
    time(time_string);
    SMG$PUT_CHARS (time_display, date_string+' '+substr(time_string,1,8), 1, 2);
    end;


    [global]
    Procedure ERASE_MESSAGE;
    begin
    SMG$ERASE_DISPLAY (status_display);
    end;


    [global]
    Procedure MESSAGE (str   : [readonly] packed array [l0..ho:integer] of char;
		       alert : boolean := TRUE);
    {
	Put some text into the status line and ring the bell
    }
    begin
    SMG$PUT_CHARS (status_display, str, 1, 1, SMG$M_ERASE_TO_EOL);
    if alert then SMG$RING_BELL (status_display);
    end;


    [global]
    Procedure SYS_MESSAGE (sts : integer);
    {
	Displays the message text for the specified system status code
    }
    var ret_status : integer;
	 buffer    : varying [256] of char;
    begin
    ret_status := $GETMSG (sts, buffer.length, buffer.body, 1);
    if not odd(ret_status) then LIB$SIGNAL (ret_status);
    MESSAGE (buffer);
    end;


    [global]
    Procedure APPEND_FIELD (var dest : string;
			    src	     : [readonly] packed array [l0..h0:integer] of char;
			    fieldlen : integer;
			    padding  : [truncate] integer);
    {
	Appends text from 'src' to 'dest'.  If 'src' is too long to fit within
	'fieldlen', it is truncated; otherwise it is padded on the right with
	spaces.  If 'padding' is specified, extra spaces are added.
    }
    var spaces : integer;
	maxlen : integer;
    begin
    maxlen := dest.capacity;
    if (length(dest) + fieldlen) > maxlen then fieldlen := maxlen - length(dest);
    if fieldlen = 0 then return;
    spaces := fieldlen - length(src);
    if spaces >= 0 then
	dest := dest + src
    else
	begin
	dest := dest + substr(src,1,fieldlen);
	spaces := 0
	end;
    if present(padding) then spaces := spaces + padding;
    if (length(dest) + spaces) > maxlen then spaces := maxlen - length(dest);
    if spaces > 0 then dest := dest + pad(' ',' ',spaces);
    end;


    [global]
    Function CONFIRM (prompt : [readonly] packed array [l0..ho:integer] of char) : boolean;
    {
	Get a Yes/No response from the user.  Valid responses are:
	   Y		N
	   YE		NO
	   YES
    }
    var ret_status : integer;
	YN_prompt  : varying [132] of char;
	response   : varying [132] of char;
    begin
    YN_prompt := prompt + ' (Yes or No) ? ';
    repeat
	SMG$ERASE_DISPLAY (status_display);
	ret_status := SMG$READ_STRING (keyboard_id
				      ,%descr response
				      ,YN_prompt
				      ,	{ maximum-length }
				      ,TRM$M_TM_CVTLOW+TRM$M_TM_NOEDIT+TRM$M_TM_NORECALL+TRM$M_TM_PURGE+TRM$M_TM_TRMNOECHO
				      ,	{ timeout }
				      ,	{ terminator-set }
				      ,	{ resultant-length }
				      ,	{ word-terminator-code }
				      ,status_display);
	if odd(ret_status) then
	    begin
	    if length(response) = 0 then
		begin
		SMG$RING_BELL (status_display);
		ret_status := 0
		end
	    else
		if eq(response,substr('YES',1,length(response))) then
		    confirm := TRUE
		else if eq(response,substr('NO',1,length(response))) then
		    confirm := FALSE
		else
		    begin
		    SMG$RING_BELL (status_display);
		    ret_status := 0
		    end
	    end
	else
	    if ret_status = SMG$_EOF then
		begin
		ret_status := 1;
		confirm := FALSE
		end
	    else
		SMG$RING_BELL (status_display);
    until odd(ret_status);
    SMG$ERASE_DISPLAY (status_display)
    end;


    [global]
    Procedure PREPARE_MENU (var menu   : SMG_Menu;
			    target_row : integer;
			    flags      : integer := SMG$M_FIXED_FORMAT+SMG$M_WRAP_MENU);
    {
	Creates the virtual display for an SMG menu, populates it from the
	supplied array of choices, and positions it on the screen so that it
	falls either fully below the target row, or fully above the target row,
	or at the bottom of the screen.  The menu is horizontally centered.
    }
    var ret_status  : integer;
	paste_row   : integer;
	menu_height : integer;
    begin
    menu_height := min (menu.count, pasteboard_rows-3);
    if menu.display = 0 then
	begin
	ret_status := SMG$CREATE_VIRTUAL_DISPLAY (menu_height, menu.width+2, menu.display, SMG$M_BORDER);
	if not odd(ret_status) then LIB$SIGNAL(ret_status);
	ret_status := SMG$CREATE_MENU (menu.display, menu.choices, SMG$K_VERTICAL, flags);
	if not odd(ret_status) then LIB$SIGNAL(ret_status);
	end;
    { Calculate where to put the menu }
    if (target_row + menu_height + 3) > pasteboard_rows then
	{ Bottom of menu will occlude the status display, so find another row }
	begin
	if (target_row - menu_height - 2) > 1 then
	    { Menu will fit above the selected queue }
	    paste_row := target_row - menu_height - 1
	else
	    { Put the menu at the bottom of the screen }
	    paste_row := pasteboard_rows - menu_height - 1;
	end
    else
	{ Menu fits below the queue }
	paste_row := target_row + 2;
    SMG$PASTE_VIRTUAL_DISPLAY (menu.display, pasteboard_id, paste_row, (pasteboard_cols-menu.width-4) div 2);
    if menu_height < menu.count then MESSAGE ('Note: menu height exceeds screen - use arrow keys to scroll.');
    end;


    [global]
    Procedure DISPLAY_DCL_COMMAND (command : packed array [l0..h0:integer] of char;
				   paste_row, display_rows : integer);
    {
	Execute the specified DCL command using SMG$CREATE_SUBPROCESS and
	SMG$EXECUTE_COMMAND.  The output is written to a display which occupies
	the space described by paste_row and display_rows.
    }
    var sub_display	: [static] unsigned value 0;
	ret_status	: integer;
	terminator	: [word] 0..65535;
    begin
    if sub_display = 0 then
	begin
	SMG$CREATE_VIRTUAL_DISPLAY (display_rows, pasteboard_cols, sub_display,, SMG$M_REVERSE);
	ret_status := SMG$CREATE_SUBPROCESS (sub_display,,, SMG$M_TRUSTED);
	if not odd(ret_status) then
	    begin
	    SMG$DELETE_VIRTUAL_DISPLAY (sub_display);
	    sub_display := 0;
	    SYS_MESSAGE (ret_status);
	    return
	    end;
	end
    else
	begin
	SMG$CHANGE_VIRTUAL_DISPLAY (sub_display, display_rows);
	SMG$ERASE_DISPLAY (sub_display);
	end;
    SMG$PASTE_VIRTUAL_DISPLAY (sub_display, pasteboard_id, paste_row, 1);
    ret_status := SMG$EXECUTE_COMMAND (sub_display, command);
    if odd(ret_status) then
	begin
	{ Ensure the message area is not occluded, then prompt to continue ... }
	SMG$ERASE_DISPLAY (status_display);
	SMG$READ_KEYSTROKE (keyboard_id, terminator, 'Press any key to return to PQM ...',, status_display, SMG$M_BLINK+SMG$M_REVERSE,SMG$M_REVERSE);
	end
    else
	SYS_MESSAGE (ret_status);
    SMG$UNPASTE_VIRTUAL_DISPLAY (sub_display, pasteboard_id);
    end;

{*******************************************************************************
*	File Viewer							       *
*									       *
* This routine implements the "Display File" menu function.		       *
*									       *
*******************************************************************************}

    [global]
    Procedure DISPLAY_FILE (filespec : packed array [l0..h0:integer] of char);
    var ret_status	: integer;
	browse_status	: integer value 1;
	browse_command	: varying [1024] of char value '';
	save_display	: unsigned;
	trnlnm_itemlist	: array [1..2] of Item_List_3 value ZERO;

    begin
    { If the user specified /BROWSER=command, use that command }
    if length(browser) > 0 then
	browse_command := browser + ' ' + filespec
    else
	{ Look for the logical namq PQM_BROWSER to specify the browse command }
	begin
	trnlnm_itemlist[1] := Item_List_3[item_code : LNM$_STRING;
					  item_len  : size(browse_command.body) - 1 - length(filespec);
					  bufaddr   : iaddress(browse_command.body);
					  retaddr   : iaddress(browse_command.length)];
	ret_status := $TRNLNM (LNM$M_CASE_BLIND, 'LNM$DCL_LOGICAL', 'PQM_BROWSER',, trnlnm_itemlist);
	if (ret_status = SS$_NOLOGNAM) or (odd(ret_status) and (length(browse_command) = 0)) then
	    { No browser specified }
	    begin
	    MESSAGE('No file browser specified; define PQM_BROWSER or use /BROWSER=command');
	    return
	    end
	else if not odd(ret_status) then
	    begin
	    LIB$SIGNAL (ret_status);
	    return
	   end;
	browse_command := browse_command + ' ' + filespec
	end;

    { Save the screen state, SPAWN the browse command, and restore the screen }
    SMG$SAVE_PHYSICAL_SCREEN (pasteboard_id, save_display);
    ret_status := LIB$SPAWN (browse_command
			    ,	{ input-file }
			    ,	{ output-file }
			    ,CLI$M_TRUSTED
			    ,	{ process-name }
			    ,	{ process-ID }
			    ,browse_status);
    if not odd(ret_status) then SYS_MESSAGE (ret_status);
    if not odd(browse_status) then
	begin
	write('Press <Return> to continue...'); readln;
	end;
    SMG$RESTORE_PHYSICAL_SCREEN (pasteboard_id, save_display);
    end;

{*******************************************************************************
*	MAIN PROGRAM							       *
*******************************************************************************}

Begin

{ Get the command line qualifiers }

ret_status := LIB$GET_FOREIGN (%descr foreign_command);
if not odd(ret_status) then $EXIT (ret_status);
if length(foreign_command) > 0 then parse_command_line;


{ Create the pasteboard }

ret_status := SMG$CREATE_PASTEBOARD (pasteboard_id,, pasteboard_rows, pasteboard_cols, SMG$M_KEEP_CONTENTS, pasteboard_type);
if not odd(ret_status) then $EXIT(ret_status);
if pasteboard_type <> SMG$K_VTTERMTABLE then
    begin
    SMG$DELETE_PASTEBOARD (pasteboard_id, 0);
    writeln('PQM can not be run on this kind of output device');
    return;
    end;


{ Declare an exit handler }

exit_handler_block.handler := iaddress(Exit_Handler);
exit_handler_block.exit_sts := iaddress(ret_status);
ret_status := $DCLEXH (%ref exit_handler_block);
if not odd(ret_status) then
    begin
    SMG$DELETE_PASTEBOARD (pasteboard_id, 0);
    writeln('Unable to declare exit handler');
    $EXIT(ret_status);
    end;
    

{ Create the virtual keyboard }

ret_status := SMG$CREATE_VIRTUAL_KEYBOARD (keyboard_id);
if not odd(ret_status) then
    begin
    writeln('Unable to create virtual keyboard');
    $EXIT(ret_status);
    end;


{ Create a 'status' display for the bottom of the screen, and put our name in it }

ret_status := SMG$CREATE_VIRTUAL_DISPLAY (1, pasteboard_cols, status_display, SMG$M_TRUNC_ICON, SMG$M_REVERSE);
if not odd(ret_status) then
    begin
    writeln('Unable to create status display');
    $EXIT(ret_status);
    end;
SMG$PUT_CHARS (status_display, 'Print Queue Manager '+PQM_VERSION+'.  Copyright © 2000 VSM Software Development.');

{ Create a 'title' display which is always at the top of the screen }

ret_status := SMG$CREATE_VIRTUAL_DISPLAY (1, 22, time_display,, SMG$M_BOLD);
if not odd(ret_status) then
    begin
    writeln('Unable to create time display');
    $EXIT(ret_status);
    end;


{ Ready to roll.  Call MANAGE_QUEUES to get things moving. }

SMG$ERASE_PASTEBOARD (pasteboard_id);
SMG$PASTE_VIRTUAL_DISPLAY (time_display, pasteboard_id, 1, pasteboard_cols-24);
SMG$PASTE_VIRTUAL_DISPLAY (status_display, pasteboard_id, pasteboard_rows, 1);
SMG$SET_CURSOR_MODE (pasteboard_id, SMG$M_CURSOR_OFF);

MANAGE_QUEUES (initial_queue, start_with_jobs_display, ignore_server_queues);

End.
