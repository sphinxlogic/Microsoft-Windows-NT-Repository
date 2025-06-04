[inherit('PQM_OBJ:GLOBALDEF'
	,'SYS$LIBRARY:STARLET'
	,'SYS$LIBRARY:PASCAL$LIB_ROUTINES'
	,'SYS$LIBRARY:PASCAL$SMG_ROUTINES'
)]
Module QUEUES (output);
{*******************************************************************************

  	QUEUES		Routines which operate on a queue

  This module contains the routines which manipulate queues.

  	Created 7-Nov-2000 by J.Begg, VSM Software Services Pty Ltd.
  	Copyright © 2000 VSM Software Development.  All rights reserved.

*******************************************************************************}

CONST
	{ Define the layout of the queue detail display }

	DETAIL_LABEL	= 'Queue Details for ';		{ Line 1:        Queue Details for XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX     On: XXXXXX }
	DETAIL_ROW	= 1;

	NODE_LABEL	= 'On: ';
	NODE_ROW	= 1;

	ROW_1_LENGTH	= length(DETAIL_LABEL) + QUEUE_NAME_SIZE + length(NODE_LABEL) + 5 + NODE_NAME_SIZE;

	DESCR_LABEL	= 'Description: ';		{ Line 2: Description: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX	}
	DESCR_ROW	= 2;
	DESCR_COL	= length(DESCR_LABEL)+1;

	PENDING_LABEL	= 'Jobs: pending ';		{ Line 3: Jobs: pending XXXX  holding XXXX  completed XXXX   Device: XXXXXXXXXXXXXXXXXXXXX	}
	PENDING_ROW	= 3;
	PENDING_COL	= length(PENDING_LABEL)+1;

	HOLDING_LABEL	= ' holding ';
	HOLDING_ROW	= 3;
	HOLDING_COL	= length(HOLDING_LABEL)+PENDING_COL+4;

	COMPLETED_LABEL	= ' completed ';
	COMPLETED_ROW	= 3;
	COMPLETED_COL	= length(COMPLETED_LABEL)+HOLDING_COL+4;

	DEVICE_LABEL	= 'Device: ';
	DEVICE_ROW	= 3;
	DEVICE_COL	= length(DEVICE_LABEL)+COMPLETED_COL+5;

	QUEUE_DETAIL_ROWS	= 3;

	QUEUE_BROWSER_PASTE_ROW	= QUEUE_DETAIL_ROWS + 4;
	QUEUE_BROWSER_QUEUE	= 26;		{ No. of characters for 'Queue' column }
	QUEUE_BROWSER_STATUS	= 25;		{ No. of characters for 'Status' column }
	QUEUE_BROWSER_FORM	= 25;		{ No. of characters for 'Form' column }
	QUEUE_BROWSER_HEADING	= 'QUEUE                       STATUS                     FORM';

VAR
	detail_display		: unsigned;
	DETAIL_COL		: integer;
	NODE_COL		: integer;

	queue_browser		: browse_list value ZERO;

	first, last		: queue_info_ptr value NIL;
	queue_count		: integer value 0;

	force_queue_refresh	: boolean value FALSE;
	show_filenames		: boolean value TRUE;

	qui_search_name		: varying [QUEUE_NAME_SIZE] of char;
	qui_search_flags	: [readonly] SEARCH_FLAGS$TYPE
				  value [QUI$V_SEARCH_SYMBIONT: TRUE;
					 QUI$V_SEARCH_ALL_JOBS: TRUE;
				         QUI$V_SEARCH_WILDCARD: TRUE;
					 otherwise ZERO];

	qui_queue_flags		: QUEUE_FLAGS$TYPE;
	qui_queue_status	: QUEUE_STATUS$TYPE;
	qui_holding_jobs	: integer;
	qui_pending_jobs	: integer;
	qui_retained_jobs	: integer;
	qui_timed_jobs		: integer;
	qui_job_status		: JOB_STATUS$TYPE;
	qui_pending_job_reason	: PENDING_JOB_REASON$TYPE;
	qui_job_entry		: unsigned;
	qui_queue_name		: varying [QUEUE_NAME_SIZE] of char;
	qui_form_name		: varying [FORM_NAME_SIZE] of char;
	qui_device_name		: varying [DEVICE_NAME_SIZE] of char;
	qui_job_name		: varying [JOB_NAME_SIZE] of char;
	qui_job_form		: varying [FORM_NAME_SIZE] of char;
	qui_description		: varying [DESCRIPTION_SIZE] of char;
	qui_scsnode_name	: varying [NODE_NAME_SIZE] of char;
	qui_job_first_file	: varying [NAM$C_MAXRSS] of char;
	qui_decoded_status	: Que_Status_Text;

	qui_queue_itemlist	: packed array [1..14] of Item_List_3 value ZERO;
	qui_job_itemlist	: packed array [1..7] of Item_List_3 value ZERO;
	qui_file_itemlist	: packed array [1..2] of Item_List_3 value ZERO;
	qui_form_itemlist	: packed array [1..3] of Item_List_3 value ZERO;

{*******************************************************************************
*									       *
*	DISPLAY_HELP							       *
*									       *
* Displays on-line help about the commands available in the Queue display.     *
*									       *
*******************************************************************************}

Procedure DISPLAY_HELP;
var help_display : [static] unsigned value 0;
    terminator	 : [word] 0..65535;
begin
if help_display = 0 then
    begin
    SMG$CREATE_VIRTUAL_DISPLAY (pasteboard_rows, pasteboard_cols, help_display);
    SMG$PUT_LINE_HIGHWIDE (help_display, '    PQM '+PQM_VERSION+ '    Queues Display', 3, SMG$M_BOLD);

    SMG$PUT_LINE (help_display, ' The Queues Display is divided into two panels.  The lower panel lists all the');
    SMG$PUT_LINE (help_display, ' queues defined on this system, together with the queue status and currently');
    SMG$PUT_LINE (help_display, ' mounted formtype, or the job/file currently being printed (if any).  The upper');
    SMG$PUT_LINE (help_display, ' panel shows specific details about the currently selected queue (as indicated');
    SMG$PUT_LINE (help_display, ' by the reverse video text).', 2);

    SMG$PUT_LINE (help_display, ' Within the lower panel you can use the following keys to move around:');
    SMG$PUT_LINE (help_display, ' Prev Screen   Move up the queue list by approximately 2/3 of the screen');
    SMG$PUT_LINE (help_display, ' Next Screen   Move down the queue list by approximately 2/3 of the screen');
    SMG$PUT_LINE (help_display, ' Up Arrow      Move up the queue list by one line');
    SMG$PUT_LINE (help_display, ' Down Arrow    Move down the queue list by one line');
    SMG$PUT_LINE (help_display, ' Do or PF4     Display a menu of operations appropriate to this queue');
    SMG$PUT_LINE (help_display, ' Return/Enter  Display the jobs in this queue');
    SMG$PUT_LINE (help_display, ' Space bar     Force an update of the queue list');
    SMG$PUT_LINE (help_display, ' Letter "F"    Display the file currently being printed by each queue (if any)');
    SMG$PUT_LINE (help_display, ' Letter "J"    Display the job currently being printed by each queue (if any)');
    SMG$PUT_LINE (help_display, ' Letter "Q"    Quit PQM');
    SMG$PUT_LINE (help_display, ' Ctrl/W        Redraw the screen');
    SMG$PUT_LINE (help_display, ' Ctrl/Z        Quit PQM', 2);

    SMG$PUT_LINE (help_display, 'Press any key to return to PQM', pasteboard_rows,, SMG$M_REVERSE);
    end;

SMG$PASTE_VIRTUAL_DISPLAY (help_display, pasteboard_id, 1, 1);
SMG$READ_KEYSTROKE (keyboard_id, terminator);
SMG$UNPASTE_VIRTUAL_DISPLAY (help_display, pasteboard_id);
end;

{*******************************************************************************
*									       *
*	DISPLAY_QUEUE_DETAILS						       *
*									       *
* This routine updates the queue details heading at the top of the queue page. *
*									       *
*	ONE_LINE_SUMMARY						       *
*									       *
* This routine builds the text string which provides a one-line summary of the *
* queue for the queue browser display.					       *
*									       *
*******************************************************************************}

Procedure DISPLAY_QUEUE_DETAILS (queue : queue_info);
var f : string(pasteboard_cols-2);
begin
SMG$BEGIN_DISPLAY_UPDATE (detail_display);
with queue do
    begin
    f := '';  APPEND_FIELD (f, name, size(name.body));  SMG$PUT_CHARS (detail_display, f, DETAIL_ROW, DETAIL_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
    if not flags.QUI$V_QUEUE_GENERIC then
	SMG$PUT_CHARS (detail_display, node, NODE_ROW, NODE_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT)
    else
	SMG$PUT_CHARS (detail_display, '-n/a- ', NODE_ROW, NODE_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
    f := '';  APPEND_FIELD (f, description, pasteboard_cols-2-DESCR_COL);  SMG$PUT_CHARS (detail_display, f, DESCR_ROW, DESCR_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
    writev (f, pending_jobs:1); SMG$PUT_CHARS (detail_display, pad(f, ' ', 4), PENDING_ROW, PENDING_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
    writev (f, (holding_jobs+timed_jobs):1); SMG$PUT_CHARS (detail_display, pad(f,' ', 4), HOLDING_ROW, HOLDING_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
    writev (f, retained_jobs:1); SMG$PUT_CHARS (detail_display, pad(f, ' ', 4), COMPLETED_ROW, COMPLETED_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
    f := '';  APPEND_FIELD (f, device, size(name.body));  SMG$PUT_CHARS (detail_display, f, DEVICE_ROW, DEVICE_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
    end;
SMG$END_DISPLAY_UPDATE (detail_display);
end;


Procedure ONE_LINE_SUMMARY (var buffer : string; queue : queue_info);
begin
buffer := '';
APPEND_FIELD (buffer, queue.name, QUEUE_BROWSER_QUEUE, 2);
if queue.status.QUI$V_QUEUE_BUSY then
    APPEND_FIELD (buffer, queue.decoded_status, QUEUE_BROWSER_STATUS + 2 + QUEUE_BROWSER_FORM)
else
    begin
    APPEND_FIELD (buffer, queue.decoded_status, QUEUE_BROWSER_STATUS, 2);
    if queue.flags.QUI$V_QUEUE_GENERIC then
	APPEND_FIELD (buffer, ' -- generic queue -- ', QUEUE_BROWSER_FORM)
    else
	APPEND_FIELD (buffer, queue.form, QUEUE_BROWSER_FORM);
    end;
end;

{*******************************************************************************
*									       *
*	GET_QUEUE_INFO							       *
*									       *
* This routine makes three calls to SYS$GETQUIW to retrieve information about  *
* a single queue.  It assumes that the queue name being searched on has been   *
* put into qui_search_name.  This routine is called by UPDATE_ONE_QUEUE and    *
* by update_queue_list.							       *
*									       *
*******************************************************************************}

Function Get_Queue_Info (efn : integer; var context : unsigned; var qc : [truncate] integer; ignore_servers : boolean) : integer;
var ret_status : integer;
    que_status : integer;
    IOSB       : IO_Status_Block;

    procedure decode_queue_status;
    var f : [volatile] varying [NAM$C_MAXRSS] of char;
    begin
    with qui_queue_status, qui_job_status do
	begin
	{ Set a default string -- assume queue is idle }
	if QUI$V_QUEUE_CLOSED then qui_decoded_status := 'Closed' else qui_decoded_status := '';

	{ Now try to be a little more precise about it! }
	if QUI$V_QUEUE_AVAILABLE or QUI$V_QUEUE_BUSY or QUI$V_QUEUE_STALLED or QUI$V_QUEUE_IDLE then
	    { Determine the job status and/or file which is currently being printed }
	    begin
	    if QUI$V_JOB_EXECUTING or QUI$V_JOB_STALLED then
		{ Get the file which is being printed }
		begin
		if      QUI$V_JOB_STALLED   then qui_decoded_status := 'Stalled on '
		else if QUI$V_QUEUE_SERVER  then qui_decoded_status := 'Processing '
		else                             qui_decoded_status := 'Printing ';
		if show_filenames then
		    begin
		    LIB$TRIM_FILESPEC (qui_job_first_file, %descr f, QUEUE_BROWSER_STATUS + QUEUE_BROWSER_FORM + 2 - length(qui_decoded_status));
		    qui_decoded_status := qui_decoded_status + f
		    end
		else
		    qui_decoded_status := qui_decoded_status + qui_job_name;
		end
	    else if QUI$V_JOB_PENDING then
		{ Work out why the job is not printing }
		DECODE_PENDING_JOB_REASON (qui_decoded_status, qui_pending_job_reason);
	    end  { available/busy/stalled/idle }

	else if QUI$V_QUEUE_ALIGNING		then qui_decoded_status := 'Aligning'
	else if QUI$V_QUEUE_AUTOSTART_INACTIVE	then qui_decoded_status := 'Stopped (a/start inactive)'
	else if QUI$V_QUEUE_DISABLED		then qui_decoded_status := 'Stopped (disabled)'
	else if QUI$V_QUEUE_PAUSED		then qui_decoded_status := 'Paused'
	else if QUI$V_QUEUE_PAUSING		then qui_decoded_status := 'Pausing'
	else if QUI$V_QUEUE_RESETTING		then qui_decoded_status := 'Stopping (reset)'
	else if QUI$V_QUEUE_RESUMING		then qui_decoded_status := 'Starting (resume)'
	else if QUI$V_QUEUE_STARTING		then qui_decoded_status := 'Starting'
	else if QUI$V_QUEUE_STOP_PENDING	then qui_decoded_status := 'Stop pending'
	else if QUI$V_QUEUE_STOPPED		then qui_decoded_status := 'Stopped'
	else if QUI$V_QUEUE_STOPPING		then qui_decoded_status := 'Stopping'
	else if QUI$V_QUEUE_UNAVAILABLE		then qui_decoded_status := 'Device unavailable';

	end; { with }
    end; { Decode_Queue_Status }

begin { Get_Queue_Info }
repeat
    que_status := $GETQUIW (efn, QUI$_DISPLAY_QUEUE, context, qui_queue_itemlist, IOSB);
    if odd(que_status) then que_status := IOSB.sts;
    if odd(que_status) then
	begin
	ret_status := $GETQUIW (efn, QUI$_DISPLAY_JOB, context, qui_job_itemlist, IOSB);
	if odd(ret_status) then ret_status := IOSB.sts;
	if odd(ret_status) then
	    begin
	    qui_job_first_file := '';
	    ret_status := $GETQUIW (efn, QUI$_DISPLAY_FILE, context, qui_file_itemlist, IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    if not odd(ret_status) and (ret_status <> JBC$_NOMOREFILE) and (ret_status <> JBC$_NOSUCHFILE) then LIB$SIGNAL (ret_status);
	    end
	else
	    { SYS$GETQUIW(JOB) failed }
	    begin
	    qui_job_status := ZERO;
	    qui_pending_job_reason := ZERO;
	    if not odd(ret_status) and (ret_status <> JBC$_NOMOREJOB) and (ret_status <> JBC$_NOSUCHJOB) then LIB$SIGNAL (ret_status);
	    end;
	decode_queue_status;
	end
    else
	if (que_status <> JBC$_NOMOREQUE) and (que_status <> JBC$_NOSUCHQUE) then LIB$SIGNAL (que_status);
until not odd(que_status) or_else not (present(ignore_servers) and_then (ignore_servers and qui_queue_status.QUI$V_QUEUE_SERVER));
if odd(que_status) and present(qc) then qc := qc + 1;
Get_Queue_Info := que_status
end;

{*******************************************************************************
*									       *
*	UPDATE_ONE_QUEUE						       *
*									       *
* This routine gets the latest information about the specified queue, and      *
* updates the queue display accordingly.  It is called by routines in this     *
* module and also from the JOBS module.					       *
*									       *
*******************************************************************************}

[global]
Function UPDATE_ONE_QUEUE (var queue : queue_info) : integer;
var efn	       : [static] unsigned value 0;
    ret_status : integer;
    context    : unsigned;
    buffer     : string (queue_browser.cols);
begin
if efn = 0 then LIB$GET_EF (efn);
context := -1;
qui_search_name := queue.name;
qui_queue_itemlist[1].item_len := length(queue.name);
ret_status := get_queue_info (efn, context);
if not odd (ret_status) then
    begin
    force_queue_refresh := TRUE;
    return ret_status
    end;
with queue do
    begin
    description		:= qui_description;
    form		:= qui_form_name;
    flags		:= qui_queue_flags;
    status		:= qui_queue_status;
    holding_jobs	:= qui_holding_jobs;
    pending_jobs	:= qui_pending_jobs;
    retained_jobs	:= qui_retained_jobs;
    timed_jobs		:= qui_timed_jobs;
    node		:= qui_scsnode_name;
    device		:= qui_device_name;
    job_status		:= qui_job_status;
    job_pending		:= qui_pending_job_reason;
    job_first_file	:= qui_job_first_file;
    job_form		:= qui_job_form;
    job_entry_number	:= qui_job_entry;
    decoded_status	:= qui_decoded_status;
    end;

display_queue_details (queue);
one_line_summary (buffer, queue);
SMG$SET_CURSOR_ABS (queue_browser.display, queue.display_row, 1);
SMG$PUT_LINE (queue_browser.display, buffer);
BROWSE_SELECT_ROW (queue_browser, queue.display_row);
UPDATE_ONE_QUEUE := ret_status
end;

{*******************************************************************************
*									       *
*	BUILD_FORM_SELECTOR						       *
*									       *
* This routine builds an SMG menu which lists the available form types, and    *
* puts it on the screen.						       *
*									       *
*******************************************************************************}

[global]
Procedure BUILD_FORM_SELECTOR (var forms    : SMG_Menu_Ptr;
			       target_row   : integer;
			       initial_form : [truncate] packed array [l0..h0:integer] of char);
type
    form_ptr	= ^form;
    form	= record
		      next : form_ptr;
		      name : varying [FORM_NAME_SIZE] of char;
		  end;
var efn		: [static] unsigned value 0;
    ret_status	: integer;
    context	: unsigned;
    form_count	: integer value 0;
    form_idx	: integer;
    longest	: integer value 0;
    IOSB	: IO_Status_Block;
    first_form	: form_ptr value NIL;
    last_form	: form_ptr value NIL;

begin
if forms = NIL then
    begin
    { Build a list of the known form names }
    MESSAGE ('Getting form names ...', MESSAGE_SILENT);
    if efn = 0 then LIB$GET_EF (efn);
    context := -1;
    qui_search_name := '*';
    qui_form_itemlist[1].item_len := length(qui_search_name);
    ret_status := $GETQUIW (efn, QUI$_DISPLAY_FORM, context, qui_form_itemlist, IOSB);
    if odd(ret_status) then ret_status := IOSB.sts;
    while odd(ret_status) do
        begin
        form_count := form_count + 1;
        if last_form <> NIL then
	    begin
	    new(last_form^.next);
	    last_form := last_form^.next
	    end
	else
	    begin
	    new(first_form);
	    last_form := first_form
	    end;
	last_form^.next := NIL;
	last_form^.name := qui_form_name;
	if length(qui_form_name) > longest then longest := length(qui_form_name);
	ret_status := $GETQUIW (efn, QUI$_DISPLAY_FORM, context, qui_form_itemlist, IOSB);
	if odd(ret_status) then ret_status := IOSB.sts;
	end;
    if (ret_status <> JBC$_NOSUCHFORM) and (ret_status <> JBC$_NOMOREFORM) then LIB$SIGNAL (ret_status);
    if last_form <> NIL then
	begin
	new(last_form^.next);
	last_form := last_form^.next;
	last_form^.name := '<no change>';
	end
    else
	begin
	new(first_form);
	last_form := first_form;
	last_form^.name := '<no forms found>'
	end;
    last_form^.next := NIL;
    form_count := form_count + 1;

    {
	Use the above list to build the menu choices.
	The list is destroyed in the process.
    }
    new(forms,form_count,longest);
    forms^ := ZERO;
    for form_idx := 1 to form_count do with forms^ do
	begin
	choices[form_idx] := first_form^.name;
	if (selection = 0) and_then eq(first_form^.name,initial_form) then selection := form_idx;
	last_form := first_form;
	first_form := first_form^.next;
	dispose(last_form);
	end;

    end

else with forms^ do
    { Menu already exists }
    if present(initial_form) then
	{ Binary search of the form menu for the initial value string }
	begin
	form_idx := 1; form_count := forms^.count;
	repeat
	    selection := (form_idx + form_count) div 2;
	    if gt(initial_form, forms^.choices[selection]) then
		form_idx := selection + 1
	    else
		form_count := selection - 1
	until eq(forms^.choices[selection], initial_form) or (form_idx > form_count);
	if ne(forms^.choices[selection], initial_form) then selection := 1
	end
    else
	selection := 1;

{
  Prepare the SMG menu and put it on the screen
}
PREPARE_MENU (forms^, target_row, SMG$M_FIXED_FORMAT);
end;

{*******************************************************************************
*									       *
*	BUILD_QUEUE_SELECTOR						       *
*									       *
* This routine builds an SMG menu which lists the available queues to which    *
* job(s) can be moved, and puts it on the screen.			       *
*									       *
*******************************************************************************}

[global]
Procedure BUILD_QUEUE_SELECTOR (var queue_selector : SMG_Menu_Ptr;
				target_row         : integer;
				exclude_queue      : [truncate] packed array [l0..h0:integer] of char);
var queue_idx	: integer;
    queue	: queue_info_ptr;

begin

{ Create the menu structure }
new(queue_selector,queue_count,QUEUE_NAME_SIZE);
queue_selector^ := ZERO;
queue := first;
queue_idx := 1;
with queue_selector^ do
    begin
    while (queue_idx < queue_count) and (queue <> NIL) do
	begin
	if ne(queue^.name, exclude_queue) then
	    begin
	    choices[queue_idx] := queue^.name;
	    queue_idx := queue_idx+1
	    end;
	queue := queue^.next
	end;
    choices[queue_count] := '<no change>';
    selection := 1;
    end;

{ Prepare the SMG menu and put it on the screen }
PREPARE_MENU (queue_selector^, target_row, 0);
end;

{*******************************************************************************
*									       *
*	MANAGE_QUEUES							       *
*									       *
* This is the outermost routine for managing queues.  It builds a list of all  *
* the "printer" queues on the system, then displays that list and invites the  *
* user to manipulate the selected queue or the jobs contained therein.         *
*									       *
*******************************************************************************}

[global]
Procedure MANAGE_QUEUES (initial_queue		: string;
			 start_with_jobs	: boolean;
			 ignore_server_queues	: boolean);

Var ret_status		: integer;
    scroll_index	: integer;
    read_terminator	: [word] 0..65535;
    selected		: queue_info_ptr value NIL;
    get_next_queue	: boolean;


    function update_queue_list : boolean;
    {
	Calls SYS$GETQUIW to get details on all queues.  The queues are
	provided in name order, so we merge them into the existing list
	(if any).
    }
    var efn		: [static] unsigned value 0;
	ret_status	: integer;
	context		: unsigned;
	q, tmp_q	: queue_info_ptr;

	function create_queue_info (prev_q, next_q : queue_info_ptr) : queue_info_ptr;
	var nq : queue_info_ptr;
	begin
	new(nq);
	with nq^ do
	    begin
	    prev := prev_q;
	    next := next_q;
	    name		:= qui_queue_name;
	    description		:= qui_description;
	    form		:= qui_form_name;
	    flags		:= qui_queue_flags;
	    status		:= qui_queue_status;
	    holding_jobs	:= qui_holding_jobs;
	    pending_jobs	:= qui_pending_jobs;
	    retained_jobs	:= qui_retained_jobs;
	    timed_jobs		:= qui_timed_jobs;
	    node		:= qui_scsnode_name;
	    device		:= qui_device_name;
	    job_status		:= qui_job_status;
	    job_pending		:= qui_pending_job_reason;
	    job_first_file	:= qui_job_first_file;
	    job_form		:= qui_job_form;
	    job_entry_number	:= qui_job_entry;
	    decoded_status	:= qui_decoded_status;
	    display_row := 0;
	    selected := FALSE
	    end;
	create_queue_info := nq;
	end;  { create_queue_info }

    begin  { update_queue_list }
    if efn = 0 then LIB$GET_EF (efn);
    queue_count := 0;
    q := first;
    context := -1;
    qui_search_name := '*';
    qui_queue_itemlist[1].item_len := length(qui_search_name);
    ret_status := get_queue_info (efn, context, queue_count, ignore_server_queues);
    while odd(ret_status) and (q <> NIL) do
	begin
	get_next_queue := FALSE;
	{
	   Merge the existing queue list with the queues returned by repeated
	   calls to SYS$GETQUIW.
	   'q' is a queue which we found on an earlier scan
	  ' qui_xxx' are the details found on this scan
	}
	if eq(qui_queue_name, q^.name) then with q^ do
	    { Update the existing queue's data }
	    begin
	    description		:= qui_description;
	    form		:= qui_form_name;
	    flags		:= qui_queue_flags;
	    status		:= qui_queue_status;
	    holding_jobs	:= qui_holding_jobs;
	    pending_jobs	:= qui_pending_jobs;
	    retained_jobs	:= qui_retained_jobs;
	    timed_jobs		:= qui_timed_jobs;
	    node		:= qui_scsnode_name;
	    device		:= qui_device_name;
	    job_status		:= qui_job_status;
	    job_pending		:= qui_pending_job_reason;
	    job_first_file	:= qui_job_first_file;
	    job_form		:= qui_job_form;
	    job_entry_number	:= qui_job_entry;
	    decoded_status	:= qui_decoded_status;
	    get_next_queue	:= TRUE;
	    q := q^.next;
	    end
	else if lt(qui_queue_name, q^.name) then
	    { Insert this new queue into the queue list }
	    begin
	    tmp_q := create_queue_info (q^.prev, q);
	    if q^.prev = NIL then
		{ New queue is first in list }
		first := tmp_q
	    else
		q^.prev^.next := tmp_q;
	    q^.prev := tmp_q;
	    get_next_queue := TRUE
	    end
	else
	    {
	      This queue from the queue list is no longer accessible (e.g. it
	      has been deleted), so remove it from the list.
	    }
	    begin
	    tmp_q := q;
	    if q^.prev <> NIL then q^.prev^.next := q^.next else first := q^.next;
	    if q^.next <> NIL then q^.next^.prev := q^.prev else last := q^.prev;
	    if q^.selected then
		{ We have to select a different queue to highlight }
		begin
		if q^.next <> NIL then selected := q^.next else selected := q^.prev;
		MESSAGE ('Warning: '+q^.name+' is no longer accessible; selected queue has changed.');
		end;
	    q := q^.next;
	    dispose(tmp_q)
	    end;
	if get_next_queue then ret_status := get_queue_info (efn, context, queue_count, ignore_server_queues);
	end;

    {
	If we've run out of queues from SYS$GETQUIW before we've reached
	the end of the queue list, the queues at the end of the list must
	no longer be accessible (e.g. deleted)
    }
    if (q <> NIL) then
	begin
	last := q^.prev;
	repeat
	    tmp_q := q;
	    if q^.selected then
		begin
		{ We have to select a different queue to highlight }
		selected := NIL;
		MESSAGE ('Warning: '+q^.name+' is no longer accessible; selected queue has changed.');
		end;
	    q := q^.next;
	    dispose(tmp_q);
	until q = NIL;
	if last <> NIL then last^.next := NIL else first := NIL;
	end
    else if odd(ret_status) then
	{
	  If we've run off the end of the queue list but still have queues to
	  process from SYS$GETQUIW, add them to the end of the list.
	}
	begin
	last := create_queue_info (last, NIL);
	if first = NIL then first := last;
	ret_status := get_queue_info (efn, context, queue_count, ignore_server_queues);
	while odd(ret_status) do
	    begin
	    last^.next := create_queue_info (last, NIL);
	    last := last^.next;
	    ret_status := get_queue_info (efn, context, queue_count, ignore_server_queues);
	    end
	end;

    { Check the status from SYS$GETQUIW, and exit if error }
    if ret_status <> JBC$_NOMOREQUE then
	begin
	if ret_status = SS$_DEVOFFLINE then
	    MESSAGE ('Unable to get queue information: JOB_CONTROLLER process not running')
	else if ret_status = JBC$_JOBQUEDIS then
	    MESSAGE ('Unable to get queue information: System job/queue manager has not been started')
	else
	    LIB$SIGNAL (ret_status);
	update_queue_list := FALSE
	end
    else
	begin
	if selected = NIL then selected := first;
	if selected <> NIL then selected^.selected := TRUE;
	update_queue_list := TRUE
	end;

    end;  { update_queue_list }


    procedure display_queue_list;
    {
	Displays the collected queue information
    }
    var q	   : queue_info_ptr;
	buffer	   : string(queue_browser.cols);
	r, sel	   : integer;

    begin
    { Work down the queue information, updating each line in the display }
    SMG$BEGIN_DISPLAY_UPDATE (queue_browser.display);
    BROWSE_ERASE (queue_browser, queue_count);
    q := first;
    r := 1;
    sel := 0;
    while (q <> NIL) do with q^ do
	begin
	{ Build the line of information about this queue }
	one_line_summary (buffer, q^);
	SMG$PUT_LINE (queue_browser.display, buffer);
	display_row := r;		{ Remember what line we displayed this queue on }
	if selected then sel := r;	{ Remember what line to highlight }
	r := r + 1;			{ Move to the next line }
	q := q^.next;			{ And the next queue }
	end;
    if (sel = 0) and (first <> NIL) then
	begin
	sel := 1;
	first^.selected := TRUE
	end;
    BROWSE_SELECT_ROW (queue_browser, sel);
    SMG$END_DISPLAY_UPDATE (queue_browser.display);
    end;


    procedure queue_stop_menu (queue : queue_info);
    {
	Displays a menu of commands which are applicable to a queue that is
	not stopped and not stopping:
		Set Formtype
		Pause Queue
		Stop Queue
		Reset Queue
		Abort Job
		Display Jobs
		SHOW QUEUE/FULL
    }
    const MENU_ITEM_COUNT	= 8;
	  MENU_ITEM_WIDTH	= 15;
    var ret_status	: integer;
	queue_row	: integer;
	queue_menu	: SMG_Menu(MENU_ITEM_COUNT, MENU_ITEM_WIDTH);
	terminator	: [word] 0..65535;
	sjc_itemlist	: array [1..3] of Item_List_3 value ZERO;
	IOSB		: IO_Status_Block;
	warn_str	: string(pasteboard_cols);

    begin
    sjc_itemlist[1] := Item_List_3 [item_code : SJC$_QUEUE;
				    item_len  : length(queue.name);
				    bufaddr   : iaddress(queue.name.body);
				    retaddr   : ZERO];
    queue_menu := ZERO;
    queue_menu.choices[1] := 'Set Formtype';
    queue_menu.choices[2] := 'Pause Queue';
    queue_menu.choices[3] := 'Stop Queue';
    queue_menu.choices[4] := 'Reset Queue';
    queue_menu.choices[5] := 'Abort Job';
    queue_menu.choices[6] := 'Display Jobs';
    queue_menu.choices[7] := 'SHOW QUEUE/FULL';
    queue_menu.choices[8] := 'Exit this menu';
    queue_row := queue.display_row - queue_browser.viewport_start + QUEUE_BROWSER_PASTE_ROW;
    PREPARE_MENU (queue_menu, queue_row);
    { Determine the default selection }
    with queue.status do
	if QUI$V_QUEUE_BUSY then
	    queue_menu.selection := 2
	else if QUI$V_QUEUE_IDLE and (queue.pending_jobs > 0) and queue.job_pending.QUI$V_PEND_STOCK_MISMATCH then
	    queue_menu.selection := 1
	else
	    queue_menu.selection := 3;
    ret_status := SMG$SELECT_FROM_MENU (keyboard_id
				       ,queue_menu.display
				       ,queue_menu.selection	{ selected item }
				       ,queue_menu.selection	{ initial item }
				       ,			{ flags }
				       ,'PQM_HELPLIB'		{ help library }
				       ,300			{ 5-minute timeout }
				       ,terminator);
    if not odd(ret_status) then
	if (ret_status = SMG$_EOF) then
	    { Don't do anything }
	    begin
	    queue_menu.selection := MENU_ITEM_COUNT;
	    ret_status := SS$_NORMAL
	    end
	else
	    begin
	    LIB$SIGNAL (ret_status);
	    queue_menu.selection := MENU_ITEM_COUNT;
	    end;

    { Process the selected item }
    case queue_menu.selection of
	1: { Set Formtype }
	   begin
	   BUILD_FORM_SELECTOR (form_selector, queue_row, queue.job_form);
	   ret_status := SMG$SELECT_FROM_MENU (keyboard_id
					      ,form_selector^.display
					      ,form_selector^.selection	{ selected item }
					      ,form_selector^.selection	{ initial item }
					      ,				{ control flags }
					      ,'PQM_HELPLIB'		{ help library }
					      ,300			{ timeout - 5 mins }
					      ,terminator);
	   if not odd(ret_status) then with form_selector^ do
	       begin
	       selection := count;  { Don't change the form }
	       if (ret_status = SMG$_EOF) then
		   ret_status := SS$_NORMAL
	       else
		   LIB$SIGNAL (ret_status);
	       end;
	   SMG$UNPASTE_VIRTUAL_DISPLAY (form_selector^.display, pasteboard_id);
	   if form_selector^.selection < form_selector^.count then with form_selector^ do
	       begin
	       sjc_itemlist[2] := Item_List_3 [item_code : SJC$_FORM_NAME;
					       item_len  : length(choices[selection]);
					       bufaddr   : iaddress(choices[selection]);
					       otherwise ZERO];
	       ret_status := $SNDJBCW (, SJC$_ALTER_QUEUE,, sjc_itemlist, IOSB);
	       if odd(ret_status) then ret_status := IOSB.sts;
	       end;
	   end;

	2: { Pause Queue }
	   begin
	   MESSAGE ('Pausing queue ' + queue.name, MESSAGE_SILENT);
	   ret_status := $SNDJBCW (, SJC$_PAUSE_QUEUE,, sjc_itemlist, IOSB);
	   if odd(ret_status) then ret_status := IOSB.sts;
	   end;

	3: { Stop Queue }
	   begin
	   MESSAGE ('Stopping queue ' + queue.name, MESSAGE_SILENT);
	   ret_status := $SNDJBCW (, SJC$_STOP_QUEUE,, sjc_itemlist, IOSB);
	   if odd(ret_status) then ret_status := IOSB.sts;
	   end;

	4: { Stop/Reset queue }
	   begin
	   MESSAGE ('Resetting queue ' + queue.name, MESSAGE_SILENT);
	   ret_status := $SNDJBCW (, SJC$_RESET_QUEUE,, sjc_itemlist, IOSB);
	   if odd(ret_status) then ret_status := IOSB.sts;
	   end;

	5: { Abort Job }
	   begin
	   writev(warn_str, 'Really delete job ', queue.job_entry_number:1);
	   if CONFIRM (warn_str) then
		begin
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
						item_len  : size(queue.job_entry_number);
						bufaddr   : iaddress(queue.job_entry_number);
						retaddr   : ZERO];
		writev(warn_str, 'Deleting job ', queue.job_entry_number:1, '...');
		MESSAGE (warn_str, MESSAGE_SILENT);
		ret_status := $SNDJBCW (, SJC$_ABORT_JOB,, sjc_itemlist, IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end
	   end;

	6: { Go to JOBS menu }
	   begin
	   MESSAGE ('Press ENTER to go into JOBS display');
	   end;

	7: { DCL command SHOW QUEUE/FULL }
	   begin
	   DISPLAY_DCL_COMMAND ('SHOW QUEUE/FULL ' + queue.name, QUEUE_BROWSER_PASTE_ROW-1, queue_browser.viewport_rows+1);
	   end;

	8: { Don't do anything }
	   ret_status := SS$_NORMAL;
        end;
    if (ret_status = JBC$_NORMAL) or (ret_status = SS$_NORMAL) then
	ERASE_MESSAGE
    else
	SYS_MESSAGE (ret_status);

    SMG$BEGIN_PASTEBOARD_UPDATE (pasteboard_id);
    SMG$POP_VIRTUAL_DISPLAY (queue_menu.display, pasteboard_id);
    end; { queue_stop_menu }


    procedure queue_start_menu (queue : queue_info);
    {
	Displays a menu of commands which are applicable to a queue that is
	either stopped or stopping:
		Start Queue
		Set Formtype
		Reset Queue
		Display Jobs
		SHOW QUEUE/FULL
    }
    const MENU_ITEM_COUNT	= 6;
	  MENU_ITEM_WIDTH	= 15;
    var ret_status	: integer;
	queue_row	: integer;
	queue_menu	: SMG_Menu(MENU_ITEM_COUNT, MENU_ITEM_WIDTH);
	terminator	: [word] 0..65535;
	sjc_itemlist	: array [1..3] of Item_List_3 value ZERO;
	IOSB		: IO_Status_Block;

    begin
    sjc_itemlist[1] := Item_List_3 [item_code : SJC$_QUEUE;
				    item_len  : length(queue.name);
				    bufaddr   : iaddress(queue.name.body);
				    retaddr   : ZERO;];
    queue_menu := ZERO;
    queue_menu.choices[1] := 'Start Queue';
    queue_menu.choices[2] := 'Set Formtype';
    queue_menu.choices[3] := 'Reset Queue';
    queue_menu.choices[4] := 'Display Jobs';
    queue_menu.choices[5] := 'SHOW QUEUE/FULL';
    queue_menu.choices[6] := 'Exit this menu';
    queue_row := queue.display_row - queue_browser.viewport_start + QUEUE_BROWSER_PASTE_ROW;
    PREPARE_MENU (queue_menu, queue_row);
    { Determine the default selection }
    with queue.status do
	if QUI$V_QUEUE_AUTOSTART_INACTIVE or
	   QUI$V_QUEUE_PAUSED or
	   QUI$V_QUEUE_STALLED or
	   QUI$V_QUEUE_STOPPED then
	    queue_menu.selection := 1
	else
	    queue_menu.selection := 3;
    ret_status := SMG$SELECT_FROM_MENU (keyboard_id
				       ,queue_menu.display
				       ,queue_menu.selection	{ selected item }
				       ,queue_menu.selection	{ initial item }
				       ,			{ flags }
				       ,'PQM_HELPLIB'		{ help library }
				       ,300			{ 5-minute timeout }
				       ,terminator);
    if not odd(ret_status) then
	if (ret_status = SMG$_EOF) then
	    { Don't do anything }
	    begin
	    queue_menu.selection := MENU_ITEM_COUNT;
	    ret_status := SS$_NORMAL
	    end
	else
	    begin
	    LIB$SIGNAL (ret_status);
	    queue_menu.selection := MENU_ITEM_COUNT;
	    end;

    { Process the selected item }
    case queue_menu.selection of
	1: { Start Queue without changing any queue properties }
	   begin
	   MESSAGE ('Starting queue ' + queue.name, MESSAGE_SILENT);
	   ret_status := $SNDJBCW (, SJC$_START_QUEUE,, sjc_itemlist, IOSB);
	   if odd(ret_status) then ret_status := IOSB.sts;
	   end;

	2: { Set Formtype }
	   begin
	   BUILD_FORM_SELECTOR (form_selector, queue_row, queue.job_form);
	   ret_status := SMG$SELECT_FROM_MENU (keyboard_id
					      ,form_selector^.display
					      ,form_selector^.selection	{ selected item }
					      ,form_selector^.selection	{ initial item }
					      ,				{ control flags }
					      ,'PQM_HELPLIB'		{ help library }
					      ,300			{ timeout - 5 mins }
					      ,terminator);
	   if not odd(ret_status) then with form_selector^ do
	       begin
	       selection := count;  { Don't change the form }
	       if (ret_status = SMG$_EOF) then
		   ret_status := SS$_NORMAL
	       else
		   LIB$SIGNAL (ret_status);
	       end;
	   SMG$UNPASTE_VIRTUAL_DISPLAY (form_selector^.display, pasteboard_id);
	   if form_selector^.selection < form_selector^.count then with form_selector^ do
	       begin
	       sjc_itemlist[2] := Item_List_3 [item_code : SJC$_FORM_NAME;
					       item_len  : length(choices[selection]);
					       bufaddr   : iaddress(choices[selection]);
					       otherwise ZERO];
	       ret_status := $SNDJBCW (, SJC$_ALTER_QUEUE,, sjc_itemlist, IOSB);
	       if odd(ret_status) then ret_status := IOSB.sts;
	       end;
	   end;

	3: { Stop/Reset queue }
	   begin
	   MESSAGE ('Resetting queue ' + queue.name, MESSAGE_SILENT);
	   ret_status := $SNDJBCW (, SJC$_RESET_QUEUE,, sjc_itemlist, IOSB);
	   if odd(ret_status) then ret_status := IOSB.sts;
	   end;

	4: { Go to JOBS menu }
	   begin
	   MESSAGE ('Press ENTER to go into JOBS display');
	   end;

	5: { DCL command SHOW QUEUE/FULL }
	   begin
	   DISPLAY_DCL_COMMAND ('SHOW QUEUE/FULL ' + queue.name, QUEUE_BROWSER_PASTE_ROW-1, queue_browser.viewport_rows+1);
	   end;

	6: { Don't do anything }
	   ret_status := SS$_NORMAL;
        end;
    if (ret_status = JBC$_NORMAL) or (ret_status = SS$_NORMAL) then
	ERASE_MESSAGE
    else
	SYS_MESSAGE (ret_status);

    SMG$BEGIN_PASTEBOARD_UPDATE (pasteboard_id);
    SMG$POP_VIRTUAL_DISPLAY (queue_menu.display, pasteboard_id);
    end; { queue_start_menu }


Begin	{ MANAGE_QUEUES }

{ Build the queue structures }

if not update_queue_list then return;
if length(initial_queue) > 0 then
    begin
    while (selected <> NIL) and_then ne(selected^.name, initial_queue) do
	begin
	selected^.selected := FALSE;
	selected := selected^.next
	end;
    if selected = NIL then selected := first;
    if selected <> NIL then selected^.selected := TRUE
    end;


{ Create the queue detail display }

ret_status := SMG$CREATE_VIRTUAL_DISPLAY (QUEUE_DETAIL_ROWS, pasteboard_cols-2, detail_display, SMG$M_BORDER);
if not odd(ret_status) then
    begin
    writeln('Unable to create Queue detail display');
    LIB$SIGNAL (ret_status);
    end;
DETAIL_COL := (pasteboard_cols-2-ROW_1_LENGTH) div 2 + length(DETAIL_LABEL) + 1;
NODE_COL   := DETAIL_COL + QUEUE_NAME_SIZE + 5 + length(NODE_LABEL);
SMG$PUT_CHARS (detail_display, DETAIL_LABEL, DETAIL_ROW, (pasteboard_cols-2-ROW_1_LENGTH) div 2 + 1,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
SMG$PUT_CHARS (detail_display, NODE_LABEL, NODE_ROW, DETAIL_COL + QUEUE_NAME_SIZE + 5,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
SMG$PUT_CHARS (detail_display, DESCR_LABEL, DESCR_ROW, 1,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
SMG$PUT_CHARS (detail_display, PENDING_LABEL, PENDING_ROW, 1,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
SMG$PUT_CHARS (detail_display, HOLDING_LABEL, HOLDING_ROW, PENDING_COL+4,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
SMG$PUT_CHARS (detail_display, COMPLETED_LABEL, COMPLETED_ROW, HOLDING_COL+4,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
SMG$PUT_CHARS (detail_display, DEVICE_LABEL, DEVICE_ROW, COMPLETED_COL+5,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
SMG$LABEL_BORDER (detail_display, ' PQM '+PQM_VERSION+' ', SMG$K_TOP, 2, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);


{ Create the queue browser display }

ret_status := BROWSE_CREATE (queue_count, pasteboard_rows-QUEUE_BROWSER_PASTE_ROW, QUEUE_BROWSER_HEADING, queue_browser);
if not odd(ret_status) then
    begin
    writeln('Unable to create Queue list');
    LIB$SIGNAL (ret_status);
    end;


{
  If the user specified PQM /INITIAL_SCREEN=JOBS=queue then go to the JOBS
  display.
}
if start_with_jobs then
    MANAGE_JOBS (selected)
else
    SMG$BEGIN_PASTEBOARD_UPDATE (pasteboard_id);

    
{
  Reveal what we've done.
  The headings display must be pasted "under" the detail display, otherwise
  the job displays get occluded by it.  (Bug in SMG, perhaps?)
}
display_queue_details (selected^);
display_queue_list;
SMG$PASTE_VIRTUAL_DISPLAY (detail_display, pasteboard_id, 2, 2, time_display);
SMG$PASTE_VIRTUAL_DISPLAY (queue_browser.headings, pasteboard_id, QUEUE_BROWSER_PASTE_ROW-1, 1, detail_display);
SMG$PASTE_VIRTUAL_DISPLAY (queue_browser.display, pasteboard_id, QUEUE_BROWSER_PASTE_ROW, 1);
SMG$END_PASTEBOARD_UPDATE (pasteboard_id);


{ Run in a loop, updating the queue information and processing user requests }

repeat

    UPDATE_TIME_DISPLAY;			{ Update the time displayed on the screen }

    if read_timeout > 0 then
	ret_status := SMG$READ_KEYSTROKE (keyboard_id, read_terminator,, read_timeout)
    else
	ret_status := SMG$READ_KEYSTROKE (keyboard_id, read_terminator);
    if not odd(ret_status) and (ret_status <> SS$_TIMEOUT) then
	begin
	if ret_status = SMG$_EOF then return;
	LIB$SIGNAL (ret_status);
	end;

    case read_terminator of
	SMG$K_TRM_HELP,
	SMG$K_TRM_QUESTION_MARK : { Display some help text }
			  DISPLAY_HELP;

	SMG$K_TRM_PREV_SCREEN : { Jump to the queue which appears 2/3 way up the screen }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (queue_browser.display);
			  if selected^.prev = NIL then
			      { We're 'up' as far as we can go }
			      MESSAGE ('Already at top of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      scroll_index := 0;
			      while (selected^.prev <> NIL) and (scroll_index < (queue_browser.viewport_rows * 2 div 3)) do
				  begin
				  selected := selected^.prev;
				  scroll_index := scroll_index + 1
				  end;
			      if selected^.prev = NIL then MESSAGE('Top of list', MESSAGE_SILENT);
			      selected^.selected := TRUE;
			      update_one_queue (selected^);
			      end;
			  SMG$END_DISPLAY_UPDATE (queue_browser.display);
			  end;

	SMG$K_TRM_NEXT_SCREEN : { Jump to the queue which appears 2/3 way down the screen }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (queue_browser.display);
			  if selected^.next = NIL then
			      { We're 'down' as far as we can go }
			      MESSAGE ('Already at bottom of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      scroll_index := 0;
			      while (selected^.next <> NIL) and (scroll_index < (queue_browser.viewport_rows * 2 div 3)) do
				  begin
				  selected := selected^.next;
				  scroll_index := scroll_index + 1
				  end;
			      if selected^.next = NIL then MESSAGE('Bottom of list', MESSAGE_SILENT);
			      selected^.selected := TRUE;
			      update_one_queue (selected^);
			      end;
			  SMG$END_DISPLAY_UPDATE (queue_browser.display);
			  end;

	SMG$K_TRM_UP	: { Highlight the queue which appears above the current queue }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (queue_browser.display);
			  if selected^.prev = NIL then
			      { We're 'up' as far as we can go }
			      MESSAGE ('Already at top of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      selected := selected^.prev;
			      selected^.selected := TRUE;
			      update_one_queue (selected^);
			      end;
			  SMG$END_DISPLAY_UPDATE (queue_browser.display);
			  end;

	SMG$K_TRM_DOWN	: { Highlight the queue which appears below the current queue }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (queue_browser.display);
			  if selected^.next = NIL then
			      { We're 'down' as far as we can go }
			      MESSAGE ('Already at bottom of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      selected := selected^.next;
			      selected^.selected := TRUE;
			      update_one_queue (selected^);
			      end;
			  SMG$END_DISPLAY_UPDATE (queue_browser.display);
			  end;

	SMG$K_TRM_DO,
	SMG$K_TRM_PF4	: { Display a menu of queue operations and perform the user's selection }
			  begin
			  ERASE_MESSAGE;
			  with selected^.status do
			      if QUI$V_QUEUE_AUTOSTART_INACTIVE or
				 QUI$V_QUEUE_PAUSED or
				 QUI$V_QUEUE_PAUSING or
				 QUI$V_QUEUE_RESETTING or
				 QUI$V_QUEUE_STALLED or
				 QUI$V_QUEUE_STOP_PENDING or
				 QUI$V_QUEUE_STOPPED or
				 QUI$V_QUEUE_STOPPING then
				  queue_start_menu (selected^)
			      else
				  queue_stop_menu (selected^);
			  update_one_queue (selected^);
			  SMG$END_PASTEBOARD_UPDATE (pasteboard_id)
			  end;

	SMG$K_TRM_CTRLM,
	SMG$K_TRM_ENTER	: { Move to the JOBS display }
			  begin
			  MANAGE_JOBS (selected);
			  if exit_flag then continue;  { Should exit now }
			  exit_flag := not update_queue_list;
			  display_queue_list;
			  display_queue_details (selected^);
			  SMG$END_PASTEBOARD_UPDATE (pasteboard_id);
			  end;

	SMG$K_TRM_TIMEOUT,
	SMG$K_TRM_SPACE	: { Update the entire queue list }
			  force_queue_refresh := TRUE;

	SMG$K_TRM_CTRLW	: { Redraw the screen without changing it }
			  SMG$REPAINT_SCREEN (pasteboard_id);

	SMG$K_TRM_UPPERCASE_F,
	SMG$K_TRM_LOWERCASE_F:
			  { Display filenames instead of job names }
			  begin
			  show_filenames := TRUE;
			  force_queue_refresh := TRUE
			  end;

	SMG$K_TRM_UPPERCASE_J,
	SMG$K_TRM_LOWERCASE_J:
			  { Display jobnames instead of file names }
			  begin
			  show_filenames := FALSE;
			  force_queue_refresh := TRUE
			  end;

	SMG$K_TRM_UPPERCASE_Q,
	SMG$K_TRM_LOWERCASE_Q:
			  { Exit program }
			  exit_flag := TRUE;

	      otherwise	  { Unexpected input }
			  MESSAGE ('Unexpected input; ignored');
	end;

    if force_queue_refresh then
	begin
	if update_queue_list then
	    begin
	    display_queue_list;
	    display_queue_details (selected^)
	    end
	else
	    exit_flag := TRUE;
	force_queue_refresh := FALSE
	end;

until exit_flag;

End;


{*******************************************************************************
*	Module Initialisation						       *
*******************************************************************************}

TO BEGIN DO

begin

{-------------------------------------------------------------------------------
	Set up the QUEUE item list for SYS$GETQUIW
-------------------------------------------------------------------------------}

qui_queue_itemlist[1]	:= Item_List_3 [item_code : QUI$_SEARCH_NAME;
					item_len  : 0;
					bufaddr   : iaddress(qui_search_name.body);
					retaddr   : ZERO];

qui_queue_itemlist[2]	:= Item_List_3 [item_code : QUI$_SEARCH_FLAGS;
					item_len  : 4;
					bufaddr   : iaddress(qui_search_flags);
					retaddr	  : ZERO];

qui_queue_itemlist[3]	:= Item_List_3 [item_code : QUI$_QUEUE_NAME;
					item_len  : size(qui_queue_name.body);
					bufaddr   : iaddress(qui_queue_name.body);
					retaddr   : iaddress(qui_queue_name.length)];

qui_queue_itemlist[4]	:= Item_List_3 [item_code : QUI$_FORM_NAME;
					item_len  : size(qui_form_name.body);
					bufaddr   : iaddress(qui_form_name.body);
					retaddr   : iaddress(qui_form_name.length)];

qui_queue_itemlist[5]	:= Item_List_3 [item_code : QUI$_DEVICE_NAME;
					item_len  : size(qui_device_name.body);
					bufaddr   : iaddress(qui_device_name.body);
					retaddr   : iaddress(qui_device_name.length)];

qui_queue_itemlist[6]	:= Item_List_3 [item_code : QUI$_SCSNODE_NAME;
					item_len  : size(qui_scsnode_name.body);
					bufaddr   : iaddress(qui_scsnode_name.body);
					retaddr   : iaddress(qui_scsnode_name.length)];

qui_queue_itemlist[7]	:= Item_List_3 [item_code : QUI$_QUEUE_FLAGS;
					item_len  : size(qui_queue_flags);
					bufaddr   : iaddress(qui_queue_flags);
					retaddr   : ZERO];

qui_queue_itemlist[8]	:= Item_List_3 [item_code : QUI$_QUEUE_STATUS;
					item_len  : size(qui_queue_status);
					bufaddr   : iaddress(qui_queue_status);
					retaddr   : ZERO];

qui_queue_itemlist[9]	:= Item_List_3 [item_code : QUI$_HOLDING_JOB_COUNT;
					item_len  : 4;
					bufaddr   : iaddress(qui_holding_jobs);
					retaddr	  : ZERO];

qui_queue_itemlist[10]	:= Item_List_3 [item_code : QUI$_PENDING_JOB_COUNT;
					item_len  : 4;
					bufaddr	  : iaddress(qui_pending_jobs);
					retaddr   : ZERO];

qui_queue_itemlist[11]	:= Item_List_3 [item_code : QUI$_QUEUE_DESCRIPTION;
					item_len  : size(qui_description.body);
					bufaddr   : iaddress(qui_description.body);
					retaddr   : iaddress(qui_description.length)];

qui_queue_itemlist[12]	:= Item_List_3 [item_code : QUI$_RETAINED_JOB_COUNT;
					item_len  : 4;
					bufaddr   : iaddress(qui_retained_jobs);
					retaddr   : ZERO];

qui_queue_itemlist[13]	:= Item_List_3 [item_code : QUI$_TIMED_RELEASE_JOB_COUNT;
					item_len  : 4;
					bufaddr   : iaddress(qui_timed_jobs);
					retaddr   : ZERO];

{-------------------------------------------------------------------------------
	Set up the JOB item list
-------------------------------------------------------------------------------}

qui_job_itemlist[1]	:= Item_List_3 [item_code : QUI$_SEARCH_FLAGS;
					item_len  : size(qui_search_flags);
					bufaddr   : iaddress(qui_search_flags);
					retaddr   : ZERO];

qui_job_itemlist[2]	:= Item_List_3 [item_code : QUI$_JOB_STATUS;
					item_len  : size(qui_job_status);
					bufaddr   : iaddress(qui_job_status);
					retaddr   : ZERO];

qui_job_itemlist[3]	:= Item_List_3 [item_code : QUI$_PENDING_JOB_REASON;
					item_len  : size(qui_pending_job_reason);
					bufaddr   : iaddress(qui_pending_job_reason);
					retaddr   : ZERO];

qui_job_itemlist[4]	:= Item_List_3 [item_code : QUI$_FORM_NAME;
					item_len  : size(qui_job_form.body);
					bufaddr   : iaddress(qui_job_form.body);
					retaddr   : iaddress(qui_job_form.length)];

qui_job_itemlist[5]	:= Item_List_3 [item_code : QUI$_ENTRY_NUMBER;
					item_len  : size(qui_job_entry);
					bufaddr   : iaddress(qui_job_entry);
					retaddr   : ZERO];

qui_job_itemlist[6]	:= Item_List_3 [item_code : QUI$_JOB_NAME;
					item_len  : size(qui_job_name.body);
					bufaddr   : iaddress(qui_job_name.body);
					retaddr   : iaddress(qui_job_name.length)];


{-------------------------------------------------------------------------------
	Set up the FILE item list
-------------------------------------------------------------------------------}

qui_file_itemlist[1]	:= Item_List_3 [item_code : QUI$_FILE_SPECIFICATION;
					item_len  : size(qui_job_first_file.body);
					bufaddr   : iaddress(qui_job_first_file.body);
					retaddr   : iaddress(qui_job_first_file.length)];

{-------------------------------------------------------------------------------
	Set up the FORMS item list
-------------------------------------------------------------------------------}

qui_form_itemlist[1]	:= Item_List_3 [item_code : QUI$_SEARCH_NAME;
					bufaddr   : iaddress(qui_search_name.body);
					otherwise ZERO];

qui_form_itemlist[2]	:= Item_List_3 [item_code : QUI$_FORM_NAME;
					item_len  : size(qui_form_name.body);
					bufaddr   : iaddress(qui_form_name.body);
					retaddr   : iaddress(qui_form_name.length)];
end;

End.
