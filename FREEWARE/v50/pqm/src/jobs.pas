[inherit('PQM_OBJ:GLOBALDEF'
	,'SYS$LIBRARY:STARLET'
	,'SYS$LIBRARY:PASCAL$LIB_ROUTINES'
	,'SYS$LIBRARY:PASCAL$SMG_ROUTINES'
)]
Module JOBS (output);
{*******************************************************************************

  	JOBS		Routines which operate on the jobs in a queue

  This module contains the routines which manipulate jobs.

  	Created 7-Nov-2000 by J.Begg, VSM Software Services Pty Ltd.
  	Copyright © 2000 VSM Software Development.  All rights reserved.

*******************************************************************************}

CONST
	{ Define the layout of the job detail display }

	QUEUE_LABEL	= 'Queue: ';				{ Line 1: Queue: XXXXXXXXXXXXXXXXXXXX   Form: XXXXXXXXXXXXXXXXX }
	QUEUE_ROW	= 1;
	QUEUE_COL	= length(QUEUE_LABEL)+1;

	QFORM_LABEL	= '   Form: ';
	QFORM_ROW	= 1;
	QFORM_COL	= QUEUE_COL + max(QUEUE_NAME_SIZE,JOB_NAME_SIZE) + length(QFORM_LABEL);

	JOB_LABEL	= '  Job: ';				{ Line 2:   Job: XXXXXXXXXXXXXXXXXXXX   Form: XXXXXXXXXXXXXXXXX }
	JOB_ROW		= 2;
	JOB_COL		= length(JOB_LABEL)+1;

	JFORM_LABEL	= '   Form: ';
	JFORM_ROW	= 2;
	JFORM_COL	= JOB_COL + max(QUEUE_NAME_SIZE,JOB_NAME_SIZE) + length(JFORM_LABEL);

	SUBMITTED_LABEL	= 'Submitted: ';			{ Line 3: Submitted: XX-XXX-XXXX XX:XX:XX.XX			}
	SUBMITTED_ROW	= 4;
	SUBMITTED_COL	= length(SUBMITTED_LABEL)+1;

	FILE_LABEL	= ' File: ';				{ Line 4: File: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX }
	FILE_ROW	= 3;
	FILE_COL	= length(FILE_LABEL)+1;

	JOB_DETAIL_ROWS	= 4;

	JOB_BROWSER_HEADING	= 'ENTRY JOB                                 USERNAME     STATUS';
	JOB_BROWSER_PASTE_ROW	= JOB_DETAIL_ROWS + 4;
	JOB_BROWSER_ENTRY	= 5;			{ No. of characters for 'ENTRY' column }
	JOB_BROWSER_JOB		= 34;			{ No. of characters for 'JOB' column }
	JOB_BROWSER_USERNAME	= USERNAME_SIZE;	{ No. of characters for 'USERNAME' column }
	JOB_BROWSER_STATUS	= 25;			{ No. of characters for 'STATUS' column }

VAR
	FILE_LEN		: integer;

	detail_display		: unsigned;

	job_browser		: browse_list value ZERO;

	qui_search_number	: integer;
	qui_search_name		: varying [QUEUE_NAME_SIZE] of char;
	qui_search_flags	: [readonly] SEARCH_FLAGS$TYPE
				  value [QUI$V_SEARCH_ALL_JOBS: TRUE;
					 QUI$V_SEARCH_WILDCARD: TRUE;
					 otherwise ZERO];

	qui_after_time		: VMS_datetime;
	qui_completed_at	: VMS_datetime;
	qui_retain_until	: VMS_datetime;
	qui_submitted_at	: VMS_datetime;
	qui_form_name		: varying [FORM_NAME_SIZE] of char;
	qui_blocks_done		: integer;
	qui_job_blocks		: integer;
	qui_condition		: packed array [1..3] of [long] integer;
	qui_entry_number	: integer;
	qui_job_flags		: JOB_FLAGS$TYPE;
	qui_job_name		: varying [JOB_NAME_SIZE] of char;
	qui_job_status		: JOB_STATUS$TYPE;
	qui_job_priority	: integer;
	qui_file_count		: integer;
	qui_files		: file_info_ptr value NIL;
	qui_pending_reason	: PENDING_JOB_REASON$TYPE;
	qui_queue_name		: varying [QUEUE_NAME_SIZE] of char;
	qui_username		: varying [USERNAME_SIZE] of char;
	qui_decoded_job_status	: Job_Status_Text;
	qui_filename		: varying [FILE_NAME_SIZE] of char;
	qui_file_status		: FILE_STATUS$TYPE;
	qui_file_first_page	: integer;
	qui_file_last_page	: integer;

	qui_queue_itemlist	: array [1..3] of Item_List_3 value ZERO;
	qui_job_itemlist	: array [0..20] of Item_List_3 value ZERO;
	qui_file_itemlist	: array [1..5] of Item_List_3 value ZERO;


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
    SMG$PUT_LINE_HIGHWIDE (help_display, '    PQM '+PQM_VERSION+ '    Jobs Display', 3, SMG$M_BOLD);

    SMG$PUT_LINE (help_display, ' The Jobs Display is divided into two panels.  The lower panel lists all the');
    SMG$PUT_LINE (help_display, ' jobs in the selected queue, together with the entry number, username and job');
    SMG$PUT_LINE (help_display, ' status.  The upper panel shows specific details about the currently selected');
    SMG$PUT_LINE (help_display, ' job (as indicated by the reverse video text).', 2);

    SMG$PUT_LINE (help_display, ' Within the lower panel you can use the following keys to move around:');
    SMG$PUT_LINE (help_display, ' Prev Screen   Move up the job list by approximately 2/3 of the screen');
    SMG$PUT_LINE (help_display, ' Next Screen   Move down the job list by approximately 2/3 of the screen');
    SMG$PUT_LINE (help_display, ' Up Arrow      Move up the job list by one line');
    SMG$PUT_LINE (help_display, ' Down Arrow    Move down the job list by one line');
    SMG$PUT_LINE (help_display, ' Do or PF4     Display a menu of operations appropriate to this job');
    SMG$PUT_LINE (help_display, ' Return/Enter  Return to the Queues Display');
    SMG$PUT_LINE (help_display, ' Space bar     Force an update of the job list');
    SMG$PUT_LINE (help_display, ' Letter "Q"    Return to the Queues Display');
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
*	DECODE_PENDING_JOB_REASON					       *
*									       *
* This routine decodes the QUI$_PENDING_JOB_REASON status		       *
*									       *
*******************************************************************************}

[global]
Procedure DECODE_PENDING_JOB_REASON (var str : varying [len] of char;
				     p       : PENDING_JOB_REASON$TYPE);
begin
with p do
    if      QUI$V_PEND_STOCK_MISMATCH		then str := 'Pending (formtype)'
    else if QUI$V_PEND_CHAR_MISMATCH		then str := 'Pending (characteristics)'
    else if QUI$V_PEND_JOB_SIZE_MAX or
	    QUI$V_PEND_JOB_SIZE_MIN		then str := 'Pending (job size)'
    else if QUI$V_PEND_NO_ACCESS		then str := 'Pending (no access)'
    else if QUI$V_PEND_LOWERCASE_MISMATCH	then str := 'Pending (lowercase reqd)'
    else str := 'Pending';
end;

{*******************************************************************************
*									       *
*	DISPLAY_JOB_DETAILS						       *
*									       *
* Updates the job detail display at the top of the page.		       *
*									       *
*	ONE_LINE_SUMMARY						       *
*									       *
* Builds the text string which provides a one-line summary of the job in the   *
* job browser display.							       *
*									       *
*******************************************************************************}

Procedure DISPLAY_JOB_DETAILS (job : job_info_ptr);
var f : string(pasteboard_cols);
begin
SMG$BEGIN_DISPLAY_UPDATE (detail_display);
if job = NIL then
    { There are no jobs in this queue }
    begin
    SMG$ERASE_CHARS (detail_display, JOB_NAME_SIZE, JOB_ROW, JOB_COL);
    SMG$ERASE_CHARS (detail_display, FORM_NAME_SIZE, JFORM_ROW, JFORM_COL);
    SMG$ERASE_LINE (detail_display, SUBMITTED_ROW, SUBMITTED_COL);
    SMG$ERASE_LINE (detail_display, FILE_ROW, FILE_COL)
    end
else
    with job^ do
	begin
	f := '';  APPEND_FIELD (f, name, JOB_NAME_SIZE);  SMG$PUT_CHARS (detail_display, f, JOB_ROW, JOB_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
	f := '';  APPEND_FIELD (f, form, FORM_NAME_SIZE);  SMG$PUT_CHARS (detail_display, f, JFORM_ROW, JFORM_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
	$ASCTIM (f.length, f.body, submit_time);  SMG$PUT_CHARS (detail_display, f, SUBMITTED_ROW, SUBMITTED_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
	LIB$TRIM_FILESPEC (files^[1].name, f.body, FILE_LEN, f.length);
	if length(f) < FILE_LEN then f := f + pad(' ',' ',FILE_LEN-length(f)); SMG$PUT_CHARS (detail_display, f, FILE_ROW, FILE_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
	end;
SMG$END_DISPLAY_UPDATE (detail_display);
end;


Procedure ONE_LINE_SUMMARY (var buffer : string; job : job_info);
begin
buffer := '';
with job do
    begin
    writev(buffer, entry_number:JOB_BROWSER_ENTRY,' ');
    APPEND_FIELD (buffer, name, JOB_BROWSER_JOB, 2);
    APPEND_FIELD (buffer, username, JOB_BROWSER_USERNAME, 1);
    APPEND_FIELD (buffer, decoded_status, JOB_BROWSER_STATUS);
    end;
end;

{*******************************************************************************
*									       *
*	MANAGE_JOBS							       *
*									       *
* This is the outermost routine for managing jobs.  It builds a list of all    *
* the jobs in the selected queue, then displays that list and invites the user *
* user to select and manipulate a job.					       *
*									       *
* The job list is held in the order in which it will be displayed on the       *
* screen, which is the same as the order in which the jobs are retrieved from  *
* the queue database (by $GETQUI) *except* that retained jobs are stored in    *
* most-recently-completed job first.					       *
*									       *
*******************************************************************************}

[global]
Procedure MANAGE_JOBS (queue : queue_info_ptr);

Var ret_status		: integer;
    read_terminator	: [word] 0..65535;
    job_count		: integer := 0;
    scroll_index	: integer;
    selected		: job_info_ptr := NIL;		{ Currently hightlighted job }
    first, last		: [static] job_info_ptr := NIL;	{ Point to entire job list }
    oldest, recent	: [static] job_info_ptr := NIL;	{ Oldest and most recently completed (retained) jobs }
    finished		: boolean := FALSE;
    force_list_update	: boolean := TRUE;


    {--------------------------------------------------------------------------}
    procedure display_queue_details;
    var f : string(pasteboard_cols);
    begin
    with queue^ do
	begin
	f := '';  APPEND_FIELD (f, name, QUEUE_NAME_SIZE);  SMG$PUT_CHARS (detail_display, f, QUEUE_ROW, QUEUE_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT);
	f := '';
	if not flags.QUI$V_QUEUE_GENERIC then
	    APPEND_FIELD (f, form, FORM_NAME_SIZE)
	else
	    APPEND_FIELD (f, ' -- generic queue --', FORM_NAME_SIZE);
	SMG$PUT_CHARS (detail_display, f, QFORM_ROW, QFORM_COL,, DETAIL_VALUE_RENDITION, DETAIL_VALUE_COMPLEMENT)
	end;
    end;


    {--------------------------------------------------------------------------}
    function get_job_info (efn           : integer;
			   var context   : unsigned;
			   GETQUI_func   : [truncate] integer;
			   target	 : pointer) : integer;
    {
	This routine makes at least two calls to SYS$GETQUIW to retrieve
	extended information about a single job.  The GETQUI_func parameter
	specifies what search context to establish; if not specified, the
	supplied context is used as-is to retrieve the next job in a queue
	which was specified earlier.
    }
    var ret_status	: integer;
	job_status	: integer;
	f		: integer;
	IOSB		: IO_Status_Block;
	buffer		: varying [FILE_NAME_SIZE] of char;

	procedure decode_job_status;
	begin
	with qui_job_status do
	    begin
	    if QUI$V_JOB_EXECUTING or QUI$V_JOB_STALLED then
		{ Attempt to determine which file is being processed }
		begin
		if QUI$V_JOB_STALLED then
		    qui_decoded_job_status := 'Stalled'
		else
		    if queue^.status.QUI$V_QUEUE_SERVER then
			qui_decoded_job_status := 'Processing'
		    else
			qui_decoded_job_status := 'Printing';
		f := 1;
		while (f <= qui_file_count) and_then not qui_files^[f].status.QUI$V_FILE_EXECUTING do f := f + 1;
		if f <= qui_file_count then
		    { Display the file currently being processed }
		    begin
		    if QUI$V_JOB_STALLED then
			begin
			qui_decoded_job_status := qui_decoded_job_status + ' on ';
			LIB$TRIM_FILESPEC (qui_files^[f].name
					  ,%descr buffer
					  ,size(qui_decoded_job_status.body) - length(qui_decoded_job_status)
					  );
			qui_decoded_job_status := qui_decoded_job_status + buffer
			end
		    else
			LIB$TRIM_FILESPEC (qui_files^[f].name, %descr qui_decoded_job_status)
		    end
		end
	    else if QUI$V_JOB_ABORTING		then qui_decoded_job_status := 'Aborting'
	    else if QUI$V_JOB_HOLDING		then qui_decoded_job_status := 'Holding'
	    else if QUI$V_JOB_INACCESSIBLE	then qui_decoded_job_status := 'no access'
	    else if QUI$V_JOB_PENDING		then decode_pending_job_reason (qui_decoded_job_status, qui_pending_reason)
	    else if QUI$V_JOB_REFUSED		then qui_decoded_job_status := 'Refused'
	    else if QUI$V_JOB_RETAINED		then
		begin
		if odd(qui_condition[1]) then
		    qui_decoded_job_status := 'Completed'
		else
		    qui_decoded_job_status := 'Failed'
		end
	    else if QUI$V_JOB_STARTING		then qui_decoded_job_status := 'Starting'
	    else if QUI$V_JOB_SUSPENDED		then qui_decoded_job_status := 'Paused'
	    else if QUI$V_JOB_TIMED_RELEASE	then
		begin
		$ASCTIM (buffer.length, buffer.body, qui_after_time);
		qui_decoded_job_status := 'Until ' + substr(buffer,1,17);
		end;
	    end; { with qui_job_status }
	end; { decode_job_status }

    begin { Get_Job_Info }
    if present(GETQUI_func) and_then (GETQUI_func = QUI$_DISPLAY_ENTRY) then
	{ Establish a new context by going direct to a specific job }
	begin
	context := -1;
	qui_search_number := target::job_info_ptr^.entry_number;
	job_status := $GETQUIW (efn, QUI$_DISPLAY_ENTRY, context, qui_job_itemlist, IOSB)
	end
    else
	begin
	if present(GETQUI_func) then
	    { Do a DISPLAY_QUEUE to prepare for retrieving all jobs in the queue }
	    begin
	    context := -1;
	    qui_search_name := target::queue_info_ptr^.name;
	    qui_queue_itemlist[1].item_len := length(qui_search_name);
	    ret_status := $GETQUIW (efn, QUI$_DISPLAY_QUEUE, context, %ref qui_queue_itemlist, IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    if not odd(ret_status) then return ret_status;
	    end;
	{ Get job information based on the established context }
	job_status := $GETQUIW (efn, QUI$_DISPLAY_JOB, context, %ref qui_job_itemlist[1], IOSB);
	end;

    { Check that we got some job data }
    if odd(job_status) then job_status := IOSB.sts;
    if not odd(job_status) then
	if (job_status = JBC$_NOMOREJOB) or (job_status = JBC$_NOSUCHJOB) or (job_status = JBC$_NOSUCHENT) then
	    return job_status
        else
	    LIB$SIGNAL (job_status);

    { Ensure the file info buffer is big enough }
    if qui_files <> NIL then
	begin
	if qui_files^.count < qui_file_count then
	    begin
	    dispose(qui_files);
	    new(qui_files,qui_file_count)
	    end
	end
    else
	new(qui_files,qui_file_count);

    { Get the file information }
    f := 0;
    repeat
	ret_status := $GETQUIW (efn, QUI$_DISPLAY_FILE, context, qui_file_itemlist, IOSB);
	if odd(ret_status) then ret_status := IOSB.sts;
	if odd(ret_status) then
	    begin
	    f := f + 1;
	    qui_files^[f].name        := qui_filename;
	    qui_files^[f].status      := qui_file_status;
	    qui_files^[f].start_page  := qui_file_first_page;
	    qui_files^[f].finish_page := qui_file_last_page;
	    end
    until not odd(ret_status);
    if (ret_status <> JBC$_NOMOREFILE) and (ret_status <> JBC$_NOSUCHFILE) then LIB$SIGNAL (ret_status);

    decode_job_status;
    get_job_info := job_status
    end; { get_job_info }


    {--------------------------------------------------------------------------}
    procedure copy_job_details (var job : job_info);
    var f : integer;
    begin
    with job do
	begin
	name		:= qui_job_name;
	form		:= qui_form_name;
	flags		:= qui_job_flags;
	status		:= qui_job_status;
	decoded_status	:= qui_decoded_job_status;
	pending_reason	:= qui_pending_reason;
	retain_until	:= qui_retain_until;
	blocks_done	:= qui_blocks_done;
	priority	:= qui_job_priority;
	if status.QUI$V_JOB_RETAINED then
	    begin
	    completed_time := qui_completed_at;
	    condition      := qui_condition;
	    end;
	if (qui_files <> NIL) and (files <> NIL) then for f := 1 to qui_file_count do
	    begin
	    files^[f].status      := qui_files^[f].status;
	    files^[f].start_page  := qui_files^[f].start_page;
	    files^[f].finish_page := qui_files^[f].finish_page;
	    end;
	end;
    end;


    {--------------------------------------------------------------------------}
    function update_one_job (job : job_info_ptr) : boolean;
    {
      Calls get_job_info to get the latest information about a specific job.
      Returns TRUE if the update could not be done.
    }
    var efn        : [static] unsigned value 0;
	ret_status : integer;
	context    : unsigned;
	buffer	   : string(job_browser.cols);
    begin
    if efn = 0 then LIB$GET_EF (efn);
    ret_status := get_job_info (efn, context, QUI$_DISPLAY_ENTRY, job);
    if not odd(ret_status) then
	begin
	if (ret_status <> JBC$_NOSUCHJOB) and (ret_status <> JBC$_NOMOREJOB) and (ret_status <> JBC$_NOSUCHENT) then
	    LIB$SIGNAL(ret_status);
	return TRUE;	{ which should cause the job to be removed from the job list }
	end;
    if ne(qui_queue_name, job^.queue^.name) then return TRUE;	{ Job is no longer in this queue }
    copy_job_details (job^);
    display_job_details (job);
    one_line_summary (buffer, job^);
    SMG$PUT_CHARS (job_browser.display, buffer, job^.display_row, 1, SMG$M_ERASE_LINE);
    BROWSE_SELECT_ROW (job_browser, job^.display_row);
    update_one_job := FALSE
    end;


    {--------------------------------------------------------------------------}
    procedure update_job_list;
    {
	Calls get_job_info to get details on all jobs in the queue.  As jobs
	are received fron get_job_info, the list of "known" jobs is searched
	and a new list of "known" jobs is built.  Jobs found in the old list
	are moved to the new list as they are seen by get_job_info.  Any jobs
	remaining in the old list after get_job_info reports "no more jobs"
	are deleted from the list.
	Note: we do it this way, rather than creating a complete new list from
	scratch each time, because it should be more efficient when a queue has
	a large number of jobs.  (And shouldn't be less efficient otherwise.)
    }
    var	efn		: [static] unsigned := 0;
	ret_status	: integer;
	context		: unsigned;
	job, n,
	first_active,
	last_active,
	oldest_retained,
	recent_retained	: job_info_ptr := NIL;
	f		: integer;
	active		: boolean := TRUE;

	function search_active_jobs (resume, top : job_info_ptr; entry : integer) : job_info_ptr;
	{
	  Search the "active" job list for this job, starting from the currently
	  indicated job.  If we hit the end of the active jobs, start again from
	  the top of the list until we reach the job we first started with.
	}
	var j : job_info_ptr;
	begin
	j := resume;
	while j <> NIL do
	    begin
	    if j^.entry_number = entry then return j;
	    if not j^.status.QUI$V_JOB_RETAINED then j := j^.next else j := NIL
	    end;
	{ Try the search again, this time starting from the top }
	j := top;
	while (j <> NIL) and (j <> resume) do
	    begin
	    if j^.entry_number = entry then return j;
	    if not j^.status.QUI$V_JOB_RETAINED then j := j^.next else j := NIL
	    end;
	{ We haven't found the job }
	search_active_jobs := NIL
	end;

	function search_retained_jobs (resume, bottom : job_info_ptr; entry : integer) : job_info_ptr;
	{
	  Search the "retained" list for this job, starting from the currently
	  indicated job.  If we hit the active jobs, start again from the bottom
	  of the retained list and continue until we find the job or until we
	  hit the job we started with.  Note that the "retained" list is held in
	  reverse chronological order, which is likely to be the opposite order
	  to which $GETQUIW returns job information.
	}
	var j : job_info_ptr;
	begin
	j := resume;
	while j <> NIL do
	    begin
	    if j^.entry_number = entry then return j;
	    if j^.status.QUI$V_JOB_RETAINED then j := j^.prev else j := NIL
	    end;
	{ Start again from the bottom of the list }
	j := bottom;
	while (j <> NIL) and (j <> resume) do
	    begin
	    if j^.entry_number = entry then return j;
	    if j^.status.QUI$V_JOB_RETAINED then j := j^.prev else j := NIL
	    end;
	{ We haven't found the job }
	search_retained_jobs := NIL
	end;


	function more_recent(date1, date2 : VMS_Datetime) : boolean;
	begin
	if date1.hi > date2.hi then more_recent := TRUE
	else if date1.hi = date2.hi then more_recent := date1.lo >= date2.lo
	else more_recent := FALSE
	end;


	function older(date1, date2 : VMS_Datetime) : boolean;
	begin
	if date1.hi < date2.hi then older := TRUE
	else if date1.hi = date2.hi then older := date1.lo <= date2.lo
	else older := FALSE
	end;


    begin { update_job_list }
    if efn = 0 then LIB$GET_EF (efn);
    job_count := 0;
    job := NIL;
    ret_status := get_job_info (efn, context, QUI$_DISPLAY_QUEUE, queue);
    while odd(ret_status) do
	begin
	job_count := job_count + 1;
	active := not qui_job_status.QUI$V_JOB_RETAINED;
	{
	  Attempt to locate the job in our lists of known jobs.  Note that the
	  following code will return "job not found" if a previously active job
	  has completed and is now retained on the queue, or if a previously
	  retained job has been made active again (e.g. SET ENTRY/RELEASE).  In
	  either case it will be set up as a "new" job and the old copy will be
	  deleted in due course.
	}
	if active then
	    job := search_active_jobs (job, first, qui_entry_number)
	else
	    job := search_retained_jobs (job, oldest, qui_entry_number);

	if job = NIL then
	    { This job has probably not been seen before }
	    begin
	    new(job);
	    job^ := ZERO;
	    job^.queue := queue;
	    with job^ do
		begin
		name		:= qui_job_name;
		username	:= qui_username;
		form		:= qui_form_name;
		entry_number	:= qui_entry_number;
		size		:= qui_job_blocks;
		blocks_done	:= qui_blocks_done;
		submit_time	:= qui_submitted_at;
		flags		:= qui_job_flags;
		status		:= qui_job_status;
		priority	:= qui_job_priority;
		file_count	:= qui_file_count;
		new(files,file_count);
		for f := 1 to file_count do files^[f] := qui_files^[f];
		if not active then
		    begin
		    completed_time	:= qui_completed_at;
		    condition		:= qui_condition;
		    retain_until	:= qui_retain_until;
		    end;
		if status.QUI$V_JOB_PENDING then
		    pending_reason	:= qui_pending_reason;
		decoded_status	:= qui_decoded_job_status;
		end
	    end
	else
	    { Update the job details and remove it from the "old" list }
	    begin
	    copy_job_details (job^);
	    if job^.prev <> NIL then job^.prev^.next := job^.next else first := first^.next;
	    if job^.next <> NIL then job^.next^.prev := job^.prev else last := last^.prev;
	    end;

	{ Move the job details to the appropriate new list }
	if active then
	    begin
	    if last_active = NIL then
		{ Assume this is the first job; create the "new" list }
		begin
		first_active := job;
		job^.prev := NIL
		end
	    else
		begin
		{ Move the job to the end of the active jobs in the "new" list }
		last_active^.next := job;
		job^.prev := last_active;
		end;
	    last_active := job;
	    job^.next := NIL;
	    end
	else
	    { Put the retained job at the appropriate place in the list }
	    begin
	    if recent_retained = NIL then
		{ Assume this is the first retained job; create the "new" list }
		begin
		oldest_retained := job;
		job^.next := NIL;
		job^.prev := NIL;
		recent_retained := job;
		end
	    else
		{
		  Put this job into the "new" retained list in descending
		  order of completion time, i.e. most recently completed
		  job first.
		}
		begin
		if more_recent (job^.completed_time, recent_retained^.completed_time) then
		    begin
		    recent_retained^.prev := job;
		    job^.next := recent_retained;
		    job^.prev := NIL;
		    recent_retained := job;
		    end
		else if older (job^.completed_time, oldest_retained^.completed_time) then
		    begin
		    oldest_retained^.next := job;
		    job^.prev := oldest_retained;
		    job^.next := NIL;
		    oldest_retained := job;
		    end
		else
		    { Search the retained list for the right place to put this job }
		    begin
		    n := oldest_retained;
		    while more_recent (job^.completed_time, n^.completed_time) do n := n^.prev;
		    job^.prev := n;
		    job^.next := n^.next;
		    job^.next^.prev := job;
		    n^.next := job;
		    end
		end
	    end; { retained job }

	{ Get the next job from $GETQUIW }
	ret_status := get_job_info (efn, context)
	end;


    { Check the status from SYS$GETQUIW, and exit if error }
    if (ret_status <> JBC$_NOMOREJOB) and (ret_status <> JBC$_NOSUCHJOB) then
	begin
	if ret_status = SS$_DEVOFFLINE then
	    MESSAGE ('Unable to get job information: JOB_CONTROLLER process not running')
	else if ret_status = JBC$_JOBQUEDIS then
	    MESSAGE ('Unable to get job information: System job/queue manager has not been started')
	else
	    LIB$SIGNAL (ret_status);
	return
	end;

    {
	At this point we have two "new" lists - active and retained - which
	must be joined into one.
	We also have a single "old" list which must be deleted.
    }
    while first <> NIL do
	{ Delete the old list }
	begin
	job := first;
	first := first^.next;
	if job = selected then selected := NIL;
	dispose(job^.files);
	dispose(job);
	end;
    if first_active  = NIL then first := recent_retained else first := first_active;
    if oldest_retained = NIL then last := last_active else last := oldest_retained;
    oldest := oldest_retained;
    recent := recent_retained;
    if (last_active <> NIL) and (recent <> NIL) then
	{ Hook the "active" and "retained" lists together }
	begin
	last_active^.next := recent;
	recent^.prev := last_active
	end;
    if selected = NIL then selected := first;
    if selected <> NIL then selected^.selected := TRUE;
    end; { update_job_list }


    {-------------------------------------------------------------------------}
    procedure display_job_list;
    {
	Refreshes the job browser display with the latest information on all
	known jobs.
    }
    var job	: job_info_ptr := NIL;
	buffer	: string(job_browser.cols);
	r, sel	: integer;
    begin
    SMG$BEGIN_DISPLAY_UPDATE (job_browser.display);
    BROWSE_ERASE (job_browser, job_count);
    if first <> NIL then
	begin
	job := first;
	r := 1;
	sel := 0;
	while job <> NIL do with job^ do
	    begin
	    { Build the one-line summary information for this job }
	    one_line_summary (buffer, job^);
	    SMG$PUT_LINE (job_browser.display, buffer);
	    display_row := r;
	    if selected then sel := r;
	    r := r + 1;
	    job := job^.next
	    end;
	BROWSE_SELECT_ROW (job_browser, sel);
	end
    else
	MESSAGE ('No jobs in '+queue^.name, MESSAGE_SILENT);
    SMG$END_DISPLAY_UPDATE (job_browser.display);
    display_job_details (selected);
    end; { display_job_list }


    {-------------------------------------------------------------------------}
    procedure job_running_menu (job : job_info);
    {
	Displays a menu of commands which are applicable to a job that is
	running:
		Pause
		Stop & Retain
		Abort Job
		Stop Queue
		Reset Queue
		SHOW ENTRY/FULL
		Exit this menu
    }
    const MENU_ITEM_COUNT	= 8;
	  MENU_ITEM_WIDTH	= 15;
	  MENU_OPTION_Pause	= 1;
	  MENU_OPTION_Display	= 2;
	  MENU_OPTION_Stop_Job	= 3;
	  MENU_OPTION_Abort	= 4;
	  MENU_OPTION_Stop_Que	= 5;
	  MENU_OPTION_Reset	= 6;
	  MENU_OPTION_SHOW	= 7;
	  MENU_OPTION_Exit	= 8;
    var ret_status	: integer;
	job_row		: integer;
	f		: integer;
	job_menu	: SMG_Menu(MENU_ITEM_COUNT, MENU_ITEM_WIDTH) value ZERO;
	terminator	: [word] 0..65535;
	sjc_itemlist	: array [1..10] of Item_List_3
			  value [1: [item_code : SJC$_QUEUE;
				     retaddr   : ZERO;
				     otherwise ZERO];
				 otherwise ZERO];
	IOSB		: IO_Status_Block;

    begin
    sjc_itemlist[1].item_len := length(job.queue^.name);
    sjc_itemlist[1].bufaddr  := iaddress(job.queue^.name.body);
    job_menu.choices[MENU_OPTION_Pause]		:= 'Pause';
    job_menu.choices[MENU_OPTION_Display]	:= 'Display File';
    job_menu.choices[MENU_OPTION_Stop_Job]	:= 'Stop & Retain';
    job_menu.choices[MENU_OPTION_Abort]		:= 'Abort Job';
    job_menu.choices[MENU_OPTION_Stop_Que]	:= 'Stop Queue';
    job_menu.choices[MENU_OPTION_Reset]		:= 'Reset Queue';
    job_menu.choices[MENU_OPTION_SHOW]		:= 'SHOW ENTRY/FULL';
    job_menu.choices[MENU_OPTION_Exit]		:= 'Exit this menu';
    job_row := job.display_row - job_browser.viewport_start + JOB_BROWSER_PASTE_ROW;
    PREPARE_MENU (job_menu, job_row);

    { Determine the default selection }
    with job.status do
	if QUI$V_JOB_ABORTING or QUI$V_JOB_STALLED then
	    job_menu.selection := 6
	else
	    job_menu.selection := 1;

    ret_status := SMG$SELECT_FROM_MENU (keyboard_id
				       ,job_menu.display
				       ,job_menu.selection	{ selected item }
				       ,job_menu.selection	{ initial item }
				       ,			{ flags }
				       ,'PQM_HELPLIB'		{ help library }
				       ,300			{ 5-minute timeout }
				       ,terminator);
    if not odd(ret_status) then
	if (ret_status = SMG$_EOF) then
	    { Don't do anything }
	    begin
	    job_menu.selection := MENU_ITEM_COUNT;
	    ret_status := SS$_NORMAL
	    end
	else
	    begin
	    LIB$SIGNAL (ret_status);
	    job_menu.selection := MENU_ITEM_COUNT;
	    end;

    { Process the selected item }
    case job_menu.selection of
	MENU_OPTION_Pause: { Pause the queue }
	    begin
	    MESSAGE ('Pausing queue ' + job.queue^.name, MESSAGE_SILENT);
	    ret_status := $SNDJBCW (, SJC$_PAUSE_QUEUE,, sjc_itemlist, IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    end;

	MENU_OPTION_Display: { Display the file being printed }
	    begin
	    f := 1;
	    while (f <= job.file_count) and_then not job.files^[f].status.QUI$V_FILE_EXECUTING do f := f + 1;
	    if f > job.file_count then f := 1;
	    DISPLAY_FILE (job.files^[f].name)
	    end;
	
	MENU_OPTION_Stop_Job, { Stop job and retain it in the queue }
	MENU_OPTION_Abort: { Abort Job }
	    begin
	    if CONFIRM ('Really stop job ' + job.name + ' (entry '+dec(job.entry_number,5)+')') then
		begin
		MESSAGE ('Stopping job ' + job.name + ' (entry '+dec(job.entry_number,5)+')', MESSAGE_SILENT);
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
						item_len  : size(job.entry_number);
						bufaddr   : iaddress(job.entry_number);
						retaddr   : ZERO];
		if job_menu.selection = 2 then
		    begin
		    sjc_itemlist[3].item_code := SJC$_REQUEUE;
		    sjc_itemlist[4].item_code := SJC$_HOLD;
		    end;
		ret_status := $SNDJBCW (, SJC$_ABORT_JOB,, sjc_itemlist, IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end
	    end;

	MENU_OPTION_Stop_Que: { Stop queue }
	    begin
	    MESSAGE ('Stopping queue ' + job.queue^.name, MESSAGE_SILENT);
	    ret_status := $SNDJBC (, SJC$_STOP_QUEUE,, sjc_itemlist, IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    end;

	MENU_OPTION_Reset: { Stop/Reset queue }
	    begin
	    MESSAGE ('Resetting queue ' + job.queue^.name, MESSAGE_SILENT);
	    ret_status := $SNDJBCW (, SJC$_RESET_QUEUE,, sjc_itemlist, IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    end;

	MENU_OPTION_SHOW: { DCL command SHOW ENTRY/FULL }
	    begin
	    DISPLAY_DCL_COMMAND ('SHOW ENTRY/FULL ' + dec(job.entry_number), JOB_BROWSER_PASTE_ROW-1, job_browser.viewport_rows+1);
	    end;

	MENU_OPTION_Exit: { Don't do anything }
	    ret_status := SS$_NORMAL;
        end;
    if (ret_status = JBC$_NORMAL) or (ret_status = SS$_NORMAL) then
	ERASE_MESSAGE
    else
	SYS_MESSAGE (ret_status);

    SMG$BEGIN_PASTEBOARD_UPDATE (pasteboard_id);
    SMG$POP_VIRTUAL_DISPLAY (job_menu.display, pasteboard_id);
    end; { job_running_menu }


    {-------------------------------------------------------------------------}
    procedure job_waiting_menu (job : job_info);
    {
	Displays a menu of commands which are applicable to a job that is
	not running:
		Display File
		Release Job
		Move to Queue
		Delete Job
		Set Job Formtype
		Set Queue Formtype
		Stop Queue
		Reset Queue
		SHOW ENTRY/FULL
		Exit this menu
    }
    const MENU_ITEM_COUNT	= 13;
	  MENU_ITEM_WIDTH	= 18;
	  MENU_OPTION_Display	= 1;
	  MENU_OPTION_Release	= 2;
	  MENU_OPTION_Hold	= 3;
	  MENU_OPTION_Move	= 4;
	  MENU_OPTION_Raise	= 5;
	  MENU_OPTION_Lower	= 6;
	  MENU_OPTION_Delete	= 7;
	  MENU_OPTION_Job_Form	= 8;
	  MENU_OPTION_Que_Form	= 9;
	  MENU_OPTION_Stop	= 10;
	  MENU_OPTION_Reset	= 11;
	  MENU_OPTION_SHOW	= 12;
	  MENU_OPTION_Exit	= 13;
    var ret_status	: integer;
	job_row		: integer;
	job_menu	: SMG_Menu(MENU_ITEM_COUNT, MENU_ITEM_WIDTH) value ZERO;
	queue_selector	: SMG_Menu_ptr;
	terminator	: [word] 0..65535;
	sjc_itemlist	: array [1..10] of Item_List_3
			  value [1: [item_code : SJC$_QUEUE;
				     retaddr   : ZERO;
				     otherwise ZERO];
				 otherwise ZERO];
	IOSB		: IO_Status_Block;

    begin
    sjc_itemlist[1].item_len := length(job.queue^.name);
    sjc_itemlist[1].bufaddr  := iaddress(job.queue^.name.body);
    job_menu.choices[MENU_OPTION_Display]	:= 'Display File';
    job_menu.choices[MENU_OPTION_Release]	:= 'Release Job';
    job_menu.choices[MENU_OPTION_Hold]		:= 'Hold Job';
    job_menu.choices[MENU_OPTION_Move]		:= 'Move to Queue';
    job_menu.choices[MENU_OPTION_Raise]		:= 'Raise job Priority';
    job_menu.choices[MENU_OPTION_Lower]		:= 'Lower job Priority';
    job_menu.choices[MENU_OPTION_Delete]	:= 'Delete Job';
    job_menu.choices[MENU_OPTION_Job_Form]	:= 'Set Job Formtype';
    job_menu.choices[MENU_OPTION_Que_Form]	:= 'Set Queue Formtype';
    job_menu.choices[MENU_OPTION_Stop]		:= 'Stop Queue';
    job_menu.choices[MENU_OPTION_Reset]		:= 'Reset Queue';
    job_menu.choices[MENU_OPTION_SHOW]		:= 'SHOW ENTRY/FULL';
    job_menu.choices[MENU_OPTION_Exit]		:= 'Exit this menu';
    job_row := job.display_row - job_browser.viewport_start + JOB_BROWSER_PASTE_ROW;
    PREPARE_MENU (job_menu, job_row);

    { Determine the default selection }
    with job.status do
	if QUI$V_JOB_RETAINED then
	    job_menu.selection := MENU_OPTION_Display
	else if QUI$V_JOB_PENDING and job.pending_reason.QUI$V_PEND_STOCK_MISMATCH then
	    job_menu.selection := MENU_OPTION_Que_Form
	else
	    job_menu.selection := MENU_OPTION_Release;
    ret_status := SMG$SELECT_FROM_MENU (keyboard_id
				       ,job_menu.display
				       ,job_menu.selection	{ selected item }
				       ,job_menu.selection	{ initial item }
				       ,			{ flags }
				       ,'PQM_HELPLIB'		{ help library }
				       ,300			{ 5-minute timeout }
				       ,terminator);
    if not odd(ret_status) then
	if (ret_status = SMG$_EOF) then
	    { Don't do anything }
	    begin
	    job_menu.selection := MENU_ITEM_COUNT;
	    ret_status := SS$_NORMAL
	    end
	else
	    begin
	    LIB$SIGNAL (ret_status);
	    job_menu.selection := MENU_ITEM_COUNT;
	    end;

    { Process the selected item }
    case job_menu.selection of
	MENU_OPTION_Display: { Display the first file in the job }
	    DISPLAY_FILE (job.files^[1].name);

	MENU_OPTION_Release: { Release the Job }
	    begin
	    MESSAGE ('Releasing job ' + job.name + ' (entry '+dec(job.entry_number,5)+')', MESSAGE_SILENT);
	    sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
					    item_len  : size(job.entry_number);
					    bufaddr   : iaddress(job.entry_number);
					    retaddr   : ZERO];
	    sjc_itemlist[3].item_code := SJC$_NO_AFTER_TIME;
	    sjc_itemlist[4].item_code := SJC$_NO_HOLD;
	    ret_status := $SNDJBCW (, SJC$_ALTER_JOB,, %ref sjc_itemlist[2], IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    end;

	MENU_OPTION_Hold: { Hold the Job }
	    begin
	    MESSAGE ('Holding job ' + job.name + ' (entry '+dec(job.entry_number,5)+')', MESSAGE_SILENT);
	    sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
					    item_len  : size(job.entry_number);
					    bufaddr   : iaddress(job.entry_number);
					    retaddr   : ZERO];
	    sjc_itemlist[3].item_code := SJC$_HOLD;
	    ret_status := $SNDJBCW (, SJC$_ALTER_JOB,, %ref sjc_itemlist[2], IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    end;

	MENU_OPTION_Move: { Move the job to another queue }
	    begin
	    BUILD_QUEUE_SELECTOR (queue_selector, job_row, job.queue^.name);
	    ret_status := SMG$SELECT_FROM_MENU (keyboard_id
					       ,queue_selector^.display
					       ,queue_selector^.selection	{ selected item }
					       ,queue_selector^.selection	{ initial item }
					       ,				{ control flags }
					       ,'PQM_HELPLIB'			{ help library }
					       ,300				{ timeout - 5 mins }
					       ,terminator);
	    if not odd(ret_status) then with queue_selector^ do
		begin
		selection := count;  { Don't change the queue }
		if (ret_status = SMG$_EOF) then
		    ret_status := SS$_NORMAL
		else
		    LIB$SIGNAL (ret_status);
		end;
	   if queue_selector^.selection < queue_selector^.count then with queue_selector^ do
		begin
		MESSAGE ('Moving job '+job.name+' (entry '+dec(job.entry_number,5)+') to queue '+choices[selection], MESSAGE_SILENT);	
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
					        item_len  : size(job.entry_number);
					        bufaddr   : iaddress(job.entry_number);
					        retaddr   : ZERO];
		sjc_itemlist[3] := Item_List_3 [item_code : SJC$_DESTINATION_QUEUE;
					        item_len  : length(choices[selection]);
					        bufaddr   : iaddress(choices[selection]);
					        retaddr   : ZERO];
		ret_status := $SNDJBCW (, SJC$_ALTER_JOB,, %ref sjc_itemlist[2], IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end;
	    end;

	MENU_OPTION_Raise: { Increase job priority }
	    if job.priority >= 255 then
		MESSAGE ('Job priority is already as high as possible')
	    else
		begin
		MESSAGE ('Raising priority of job ' + job.name + ' (entry '+dec(job.entry_number,5)+')', MESSAGE_SILENT);
		job.priority := min(255, job.priority + 5);
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
						item_len  : size(job.entry_number);
						bufaddr   : iaddress(job.entry_number);
						retaddr   : ZERO];
		sjc_itemlist[3] := Item_List_3 [item_code : SJC$_PRIORITY;
						item_len  : size(job.priority);
						bufaddr   : iaddress(job.priority);
						retaddr   : ZERO];
		ret_status := $SNDJBCW (, SJC$_ALTER_JOB,, %ref sjc_itemlist[2], IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end;

	MENU_OPTION_Lower: { Decrease job priority }
	    if job.priority <= 0 then
		MESSAGE ('Job priority is already as low as possible')
	    else
		begin
		MESSAGE ('Lowering priority of job ' + job.name + ' (entry '+dec(job.entry_number,5)+')', MESSAGE_SILENT);
		job.priority := max(0, job.priority - 5);
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
						item_len  : size(job.entry_number);
						bufaddr   : iaddress(job.entry_number);
						retaddr   : ZERO];
		sjc_itemlist[3] := Item_List_3 [item_code : SJC$_PRIORITY;
						item_len  : size(job.priority);
						bufaddr   : iaddress(job.priority);
						retaddr   : ZERO];
		ret_status := $SNDJBCW (, SJC$_ALTER_JOB,, %ref sjc_itemlist[2], IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end;

	MENU_OPTION_Delete: { Delete Job }
	    begin
	    if CONFIRM ('Really delete job ' + job.name + ' (entry '+dec(job.entry_number,5)+')') then
		begin
		MESSAGE ('Deleting job ' + job.name + ' (entry '+dec(job.entry_number,5)+')', MESSAGE_SILENT);
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
						item_len  : size(job.entry_number);
						bufaddr   : iaddress(job.entry_number);
						retaddr   : ZERO];
		ret_status := $SNDJBCW (, SJC$_DELETE_JOB,, sjc_itemlist, IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end
	    end;

	MENU_OPTION_Job_Form: { Set Job Formtype }
	    begin
	    BUILD_FORM_SELECTOR (form_selector, job_row, job.queue^.form);
	    ret_status := SMG$SELECT_FROM_MENU (keyboard_id
					       ,form_selector^.display
					       ,form_selector^.selection	{ selected item }
					       ,form_selector^.selection	{ initial item }
					       ,				{ control flags }
					       ,'PQM_HELPLIB'			{ help library }
					       ,300				{ timeout - 5 mins }
					       ,terminator);
	    if not odd(ret_status) then with form_selector^ do
		begin
		selection := count;  { Don't change the formtype }
		if (ret_status = SMG$_EOF) then
		    ret_status := SS$_NORMAL
		else
		    LIB$SIGNAL (ret_status);
		end;
	    SMG$UNPASTE_VIRTUAL_DISPLAY (form_selector^.display, pasteboard_id);
	    if form_selector^.selection < form_selector^.count then with form_selector^ do
		begin
		MESSAGE ('Setting form '+choices[selection], MESSAGE_SILENT);	
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_ENTRY_NUMBER;
					        item_len  : size(job.entry_number);
					        bufaddr   : iaddress(job.entry_number);
					        retaddr   : ZERO];
		sjc_itemlist[3] := Item_List_3 [item_code : SJC$_FORM_NAME;
					        item_len  : length(choices[selection]);
					        bufaddr   : iaddress(choices[selection]);
					        retaddr   : ZERO];
		ret_status := $SNDJBCW (, SJC$_ALTER_JOB,, sjc_itemlist, IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end;
	    end;

	MENU_OPTION_Que_Form: { Set Queue Formtype }
	    begin
	    BUILD_FORM_SELECTOR (form_selector, job_row, job.form);
	    ret_status := SMG$SELECT_FROM_MENU (keyboard_id
					       ,form_selector^.display
					       ,form_selector^.selection	{ selected item }
					       ,form_selector^.selection	{ initial item }
					       ,				{ control flags }
					       ,'PQM_HELPLIB'			{ help library }
					       ,300				{ timeout - 5 mins }
					       ,terminator);
	    if not odd(ret_status) then with form_selector^ do
		begin
		selection := count;  { Don't change the formtype }
		if (ret_status = SMG$_EOF) then
		    ret_status := SS$_NORMAL
		else
		    LIB$SIGNAL (ret_status);
		end;
	    SMG$UNPASTE_VIRTUAL_DISPLAY (form_selector^.display, pasteboard_id);
	    if form_selector^.selection < form_selector^.count then with form_selector^ do
		begin
		MESSAGE ('Setting form '+choices[selection], MESSAGE_SILENT);	
		sjc_itemlist[2] := Item_List_3 [item_code : SJC$_FORM_NAME;
					        item_len  : length(choices[selection]);
					        bufaddr   : iaddress(choices[selection]);
					        retaddr   : ZERO];
		ret_status := $SNDJBCW (, SJC$_ALTER_QUEUE,, sjc_itemlist, IOSB);
		if odd(ret_status) then ret_status := IOSB.sts;
		end;
	    end;

	MENU_OPTION_Stop: { Stop queue }
	    begin
	    MESSAGE ('Stopping queue ' + job.queue^.name, MESSAGE_SILENT);
	    ret_status := $SNDJBC (, SJC$_STOP_QUEUE,, sjc_itemlist, IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    end;

	MENU_OPTION_Reset: { Stop/Reset queue }
	    begin
	    MESSAGE ('Resetting queue ' + job.queue^.name, MESSAGE_SILENT);
	    ret_status := $SNDJBCW (, SJC$_RESET_QUEUE,, sjc_itemlist, IOSB);
	    if odd(ret_status) then ret_status := IOSB.sts;
	    end;

	MENU_OPTION_SHOW: { DCL command SHOW ENTRY/FULL }
	    DISPLAY_DCL_COMMAND ('SHOW ENTRY/FULL ' + dec(job.entry_number), JOB_BROWSER_PASTE_ROW-1, job_browser.viewport_rows+1);

	MENU_OPTION_Exit: { Don't do anything }
	    ret_status := SS$_NORMAL;
        end;
    if (ret_status = JBC$_NORMAL) or (ret_status = SS$_NORMAL) then
	ERASE_MESSAGE
    else
	SYS_MESSAGE (ret_status);

    SMG$BEGIN_PASTEBOARD_UPDATE (pasteboard_id);
    SMG$POP_VIRTUAL_DISPLAY (job_menu.display, pasteboard_id);
    end; { job_waiting_menu }


Begin  { MANAGE_JOBS }

{ Get the latest job information }
update_job_list;

SMG$BEGIN_PASTEBOARD_UPDATE (pasteboard_id);

if detail_display = 0 then
    { Create the job detail display }
    begin
    ret_status := SMG$CREATE_VIRTUAL_DISPLAY (JOB_DETAIL_ROWS, pasteboard_cols-2, detail_display, SMG$M_BORDER);
    if not odd(ret_status) then
	begin
	writeln('Unable to create Job detail display');
	LIB$SIGNAL (ret_status);
	end;
    SMG$PUT_CHARS (detail_display, QUEUE_LABEL, QUEUE_ROW, 1,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    SMG$CHANGE_RENDITION (detail_display, QUEUE_ROW, QUEUE_COL, 1, QUEUE_NAME_SIZE, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    SMG$PUT_CHARS (detail_display, QFORM_LABEL, QFORM_ROW, QUEUE_COL+max(QUEUE_NAME_SIZE,JOB_NAME_SIZE),, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    SMG$PUT_CHARS (detail_display, JOB_LABEL, JOB_ROW, 1,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    SMG$PUT_CHARS (detail_display, JFORM_LABEL, JFORM_ROW, JOB_COL+max(QUEUE_NAME_SIZE,JOB_NAME_SIZE),, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    SMG$PUT_CHARS (detail_display, SUBMITTED_LABEL, SUBMITTED_ROW, 1,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    SMG$PUT_CHARS (detail_display, FILE_LABEL, FILE_ROW, 1,, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    SMG$LABEL_BORDER (detail_display, ' PQM '+PQM_VERSION+' ', SMG$K_TOP, 2, DETAIL_HEADING_RENDITION, DETAIL_HEADING_COMPLEMENT);
    FILE_LEN := pasteboard_cols - 2 - FILE_COL;
    end;

if job_browser.display = 0 then
    { Create the job browser display }
    begin
    ret_status := BROWSE_CREATE (job_count, pasteboard_rows-JOB_BROWSER_PASTE_ROW, JOB_BROWSER_HEADING, job_browser);
    if not odd(ret_status) then
	begin
	writeln('Unable to create Job List');
	LIB$SIGNAL (ret_status)
	end;
    end;


{ Reveal what we've done }

display_queue_details;
display_job_list;
SMG$PASTE_VIRTUAL_DISPLAY (job_browser.headings, pasteboard_id, JOB_BROWSER_PASTE_ROW-1, 1);
SMG$PASTE_VIRTUAL_DISPLAY (job_browser.display, pasteboard_id, JOB_BROWSER_PASTE_ROW, 1);
SMG$PASTE_VIRTUAL_DISPLAY (detail_display, pasteboard_id, 2, 2, time_display);
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
	if ret_status = SMG$_EOF then
	    begin
	    finished := TRUE;
	    exit_flag := TRUE;
	    continue
	    end
	else
	    LIB$SIGNAL (ret_status)
	end;

    case read_terminator of
	SMG$K_TRM_HELP,
	SMG$K_TRM_QUESTION_MARK : { Display on-screen help }
			  DISPLAY_HELP;

	SMG$K_TRM_PREV_SCREEN : { Highlight the job which appears 2/3 way up screen from current job }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (job_browser.display);
			  if selected = NIL then
			      { No jobs to browse }
			      MESSAGE('No jobs in '+queue^.name)
			  else if selected^.prev = NIL then
			      { We're 'up' as far as we can go }
			      message('Already at top of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      scroll_index := 0;
			      while (selected^.prev <> NIL) and (scroll_index < (job_browser.viewport_rows * 2 div 3)) do
				  begin
			          selected := selected^.prev;
				  scroll_index := scroll_index + 1
				  end;
			      if selected^.prev = NIL then MESSAGE('Top of list', MESSAGE_SILENT);
			      selected^.selected := TRUE;
			      force_list_update := update_one_job (selected);
			      end;
			  SMG$END_DISPLAY_UPDATE (job_browser.display);
			  end;

	SMG$K_TRM_NEXT_SCREEN : { Highlight the job which appears 2/3 way up down from current job }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (job_browser.display);
			  if selected = NIL then
			      { No jobs to browse }
			      MESSAGE('No jobs in '+queue^.name)
			  else if selected^.next = NIL then
			      { We're 'down' as far as we can go }
			      message('Already at bottom of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      scroll_index := 0;
			      while (selected^.next <> NIL) and (scroll_index < (job_browser.viewport_rows * 2 div 3)) do
				  begin
			          selected := selected^.next;
				  scroll_index := scroll_index + 1
				  end;
			      if selected^.next = NIL then MESSAGE('Bottom of list', MESSAGE_SILENT);
			      selected^.selected := TRUE;
			      force_list_update := update_one_job (selected);
			      end;
			  SMG$END_DISPLAY_UPDATE (job_browser.display);
			  end;

	SMG$K_TRM_UP	: { Highlight the job which appears above the current job }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (job_browser.display);
			  if selected = NIL then
			      { No jobs to browse }
			      MESSAGE('No jobs in '+queue^.name)
			  else if selected^.prev = NIL then
			      { We're 'up' as far as we can go }
			      message('Already at top of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      selected := selected^.prev;
			      selected^.selected := TRUE;
			      force_list_update := update_one_job (selected);
			      end;
			  SMG$END_DISPLAY_UPDATE (job_browser.display);
			  end;

	SMG$K_TRM_DOWN	: { Highlight the job which appears below the current job }
			  begin
			  SMG$BEGIN_DISPLAY_UPDATE (job_browser.display);
			  if selected = NIL then
			      { No jobs to browse }
			      MESSAGE('No jobs in '+queue^.name)
			  else if selected^.next = NIL then
			      { We're 'down' as far as we can go }
			      message('Already at bottom of list')
			  else
			      begin
			      ERASE_MESSAGE;
			      selected^.selected := FALSE;
			      selected := selected^.next;
			      selected^.selected := TRUE;
			      force_list_update := update_one_job (selected);
			      end;
			  SMG$END_DISPLAY_UPDATE (job_browser.display);
			  end;

	SMG$K_TRM_DO,
	SMG$K_TRM_PF4	: { Display a menu of job operations and perform the user's selection }
			  begin
			  ERASE_MESSAGE;
			  if selected <> NIL then with selected^.status do
			      begin
			      if QUI$V_JOB_ABORTING or
				 QUI$V_JOB_EXECUTING or
				 QUI$V_JOB_STALLED or
				 QUI$V_JOB_STARTING or
				 QUI$V_JOB_SUSPENDED then
				  job_running_menu (selected^)
			      else
				  job_waiting_menu (selected^);
			      if odd(UPDATE_ONE_QUEUE(queue^)) then
				  begin
				  if update_one_job (selected) then
				      force_list_update := TRUE
				  else
				      display_queue_details
				  end
			      else
				  finished := TRUE
			      end
			  else
			      { Queue is empty }
			      MESSAGE ('To manipulate this queue, return to the Queues display');
			  SMG$END_PASTEBOARD_UPDATE (pasteboard_id)
			  end;

	SMG$K_TRM_UPPERCASE_Q,
	SMG$K_TRM_LOWERCASE_Q,
	SMG$K_TRM_CTRLM,
	SMG$K_TRM_ENTER	: { Return to the QUEUES display }
			  finished := TRUE;

	SMG$K_TRM_TIMEOUT,
	SMG$K_TRM_SPACE	: { Update the entire job list }
			  if not odd(UPDATE_ONE_QUEUE (queue^)) then
			      finished := TRUE
			  else
			      force_list_update	:= TRUE;

	SMG$K_TRM_CTRLW	: { Redraw the screen without changing it }
			  SMG$REPAINT_SCREEN (pasteboard_id);

	      otherwise	  { Unexpected input }
			  MESSAGE('Unexpected input; ignored');
	end;

    if force_list_update then
	begin
	display_queue_details;
	update_job_list;
	display_job_list;
	force_list_update := FALSE
	end;

until finished;

{
  Unselect the job in case it's still around
  the next time we come into the Jobs display.
}
if selected <> NIL then selected^.selected := FALSE;

ERASE_MESSAGE;
SMG$BEGIN_PASTEBOARD_UPDATE (pasteboard_id);
SMG$UNPASTE_VIRTUAL_DISPLAY (job_browser.display, pasteboard_id);
SMG$UNPASTE_VIRTUAL_DISPLAY (job_browser.headings, pasteboard_id);
SMG$UNPASTE_VIRTUAL_DISPLAY (detail_display, pasteboard_id);

End;


{*******************************************************************************
*	Module Initialisation						       *
*******************************************************************************}

TO BEGIN DO

begin

{-------------------------------------------------------------------------------
	Prepare the QUEUE item list for SYS$GETQUIW
-------------------------------------------------------------------------------}

qui_queue_itemlist[1]	:= Item_List_3 [item_code : QUI$_SEARCH_NAME;
					bufaddr   : iaddress(qui_search_name.body);
					otherwise ZERO];

qui_queue_itemlist[2]	:= Item_List_3 [item_code : QUI$_SEARCH_FLAGS;
					item_len  : size(qui_search_flags);
					bufaddr   : iaddress(qui_search_flags);
					retaddr   : ZERO];


{-------------------------------------------------------------------------------
	Prepare the JOB item list
-------------------------------------------------------------------------------}

qui_job_itemlist[0]	:= Item_List_3 [item_code : QUI$_SEARCH_NUMBER;
					item_len  : size(qui_search_number);
					bufaddr   : iaddress(qui_search_number);
					retaddr   : ZERO];

qui_job_itemlist[1]	:= Item_List_3 [item_code : QUI$_SEARCH_FLAGS;
					item_len  : size(qui_search_flags);
					bufaddr   : iaddress(qui_search_flags);
					retaddr	  : ZERO];

qui_job_itemlist[2]	:= Item_List_3 [item_code : QUI$_AFTER_TIME;
					item_len  : size(qui_after_time);
					bufaddr   : iaddress(qui_after_time);
					retaddr   : ZERO];

qui_job_itemlist[3]	:= Item_List_3 [item_code : QUI$_FORM_NAME;
					item_len  : size(qui_form_name.body);
					bufaddr   : iaddress(qui_form_name.body);
					retaddr   : iaddress(qui_form_name.length)];

qui_job_itemlist[4]	:= Item_List_3 [item_code : QUI$_COMPLETED_BLOCKS;
					item_len  : size(qui_blocks_done);
					bufaddr   : iaddress(qui_blocks_done);
					retaddr	  : ZERO];

qui_job_itemlist[5]	:= Item_List_3 [item_code : QUI$_CONDITION_VECTOR;
					item_len  : size(qui_condition);
					bufaddr	  : iaddress(qui_condition);
					retaddr   : ZERO];

qui_job_itemlist[6]	:= Item_List_3 [item_code : QUI$_ENTRY_NUMBER;
					item_len  : size(qui_entry_number);
					bufaddr   : iaddress(qui_entry_number);
					retaddr   : ZERO];

qui_job_itemlist[7]	:= Item_List_3 [item_code : QUI$_JOB_FLAGS;
					item_len  : size(qui_job_flags);
					bufaddr   : iaddress(qui_job_flags);
					retaddr   : ZERO];

qui_job_itemlist[8]	:= Item_List_3 [item_code : QUI$_JOB_NAME;
					item_len  : size(qui_job_name.body);
					bufaddr   : iaddress(qui_job_name.body);
					retaddr   : iaddress(qui_job_name.length)];

qui_job_itemlist[9]	:= Item_List_3 [item_code : QUI$_JOB_STATUS;
					item_len  : size(qui_job_status);
					bufaddr   : iaddress(qui_job_status);
					retaddr   : ZERO];

qui_job_itemlist[10]	:= Item_List_3 [item_code : QUI$_FILE_COUNT;
					item_len  : size(qui_file_count);
					bufaddr   : iaddress(qui_file_count);
					retaddr   : ZERO];

qui_job_itemlist[11]	:= Item_List_3 [item_code : QUI$_JOB_COMPLETION_TIME;
					item_len  : size(qui_completed_at);
					bufaddr   : iaddress(qui_completed_at);
					retaddr   : ZERO];

qui_job_itemlist[12]	:= Item_List_3 [item_code : QUI$_JOB_RETENTION_TIME;
					item_len  : size(qui_retain_until);
					bufaddr   : iaddress(qui_retain_until);
					retaddr   : ZERO];

qui_job_itemlist[13]	:= Item_List_3 [item_code : QUI$_JOB_SIZE;
				   	item_len  : size(qui_job_blocks);
					bufaddr   : iaddress(qui_job_blocks);
					retaddr   : ZERO];

qui_job_itemlist[14]	:= Item_List_3 [item_code : QUI$_PENDING_JOB_REASON;
					item_len  : size(qui_pending_reason);
					bufaddr   : iaddress(qui_pending_reason);
					retaddr   : ZERO];

qui_job_itemlist[15]	:= Item_List_3 [item_code : QUI$_SUBMISSION_TIME;
					item_len  : size(qui_submitted_at);
					bufaddr   : iaddress(qui_submitted_at);
					retaddr   : ZERO];

qui_job_itemlist[16]	:= Item_List_3 [item_code : QUI$_USERNAME;
					item_len  : size(qui_username.body);
					bufaddr   : iaddress(qui_username.body);
					retaddr   : iaddress(qui_username.length)];

qui_job_itemlist[17]	:= Item_List_3 [item_code : QUI$_QUEUE_NAME;
					item_len  : size(qui_queue_name.body);
					bufaddr   : iaddress(qui_queue_name.body);
					retaddr   : iaddress(qui_queue_name.length)];

qui_job_itemlist[18]	:= Item_List_3 [item_code : QUI$_PRIORITY;
					item_len  : size(qui_job_priority);
					bufaddr   : iaddress(qui_job_priority);
					retaddr   : ZERO];

{-------------------------------------------------------------------------------
	Prepare the FILE item list
-------------------------------------------------------------------------------}

qui_file_itemlist[1]	:= Item_List_3 [item_code : QUI$_FILE_SPECIFICATION;
					item_len  : size(qui_filename.body);
					bufaddr   : iaddress(qui_filename.body);
					retaddr   : iaddress(qui_filename.length)];

qui_file_itemlist[2]	:= Item_List_3 [item_code : QUI$_FILE_STATUS;
					item_len  : size(qui_file_status);
					bufaddr   : iaddress(qui_file_status);
					retaddr   : ZERO];

qui_file_itemlist[3]	:= Item_List_3 [item_code : QUI$_FIRST_PAGE;
					item_len  : size(qui_file_first_page);
					bufaddr   : iaddress(qui_file_first_page);
					retaddr   : ZERO];

qui_file_itemlist[4]	:= Item_List_3 [item_code : QUI$_LAST_PAGE;
					item_len  : size(qui_file_last_page);
					bufaddr   : iaddress(qui_file_last_page);
					retaddr   : ZERO];

end;

End.
