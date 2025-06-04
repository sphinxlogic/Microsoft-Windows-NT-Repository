[environment('PQM_OBJ:GLOBALDEF'), inherit('SYS$LIBRARY:STARLET')]
Module GLOBALDEF;
{*******************************************************************************

  	GLOBALDEF		Global definitions for PQM

  This module contains global definitions used by other modules in the PQM
  package.


  	Created 7-Nov-2000 by J.Begg, VSM Software Services Pty Ltd.
  	Copyright © 2000 VSM Software Development.  All rights reserved.

*******************************************************************************}

CONST
	%include 'PQM_VERSION.TXT/NOLIST'

	MESSAGE_SILENT		= FALSE;

	DESCRIPTION_SIZE	= 255;
	DEVICE_NAME_SIZE	= 31;
	FILE_NAME_SIZE		= NAM$C_MAXRSS;
	FORM_NAME_SIZE		= 31;
	JOB_NAME_SIZE		= 39;
	NODE_NAME_SIZE		= 6;
	QUEUE_NAME_SIZE		= 31;
	USERNAME_SIZE		= 12;

	{
	  Change these two constants to change the rendition of headings in
	  the Detail displays at the top of the Queue and Job screens.
	}
	DETAIL_HEADING_RENDITION	= 0;
	DETAIL_HEADING_COMPLEMENT	= 0;

	{
	  Change these two constants to change the rendition of value fields
	  in the Detail displays at the top of the Queue and Job screens.
	}
	DETAIL_VALUE_RENDITION		= SMG$M_BOLD;
	DETAIL_VALUE_COMPLEMENT		= 0;


TYPE
	VMS_datetime	= [quad] packed record lo:unsigned; hi:integer end;

	IO_Status_Block	= [quad] packed record
			      sts	: [long] integer;
			      reserved	: [long] integer;
			  end;

	Item_List_3	= packed record
			      item_len	: [word] 0..65535;
			      item_code	: [word] 0..65535;
			      bufaddr	: [long] integer;
			      retaddr	: [long] integer;
			  end;

        Que_Status_Text	= varying [80] of char;
	Job_Status_Text	= varying [80] of char;

	browse_list	= record
			      headings		: unsigned;	{ Virtual display for column headings }
			      display		: unsigned;	{ Virtual display id }
			      rows, cols	: integer;	{ Size of 'display' }
			      current_row	: integer;	{ Currently highlighted row }
			      viewport_start	: integer;	{ Row in 'display' at which viewport starts }
			      viewport_rows	: integer;	{ No. of rows in the viewport }
			      invalidated	: boolean;	{ Force re-placement of viewport }
			  end;

	queue_info_ptr	= ^queue_info;
	job_info_ptr	= ^job_info;

	file_info	= record
			      name		: varying [FILE_NAME_SIZE] of char;
			      status		: FILE_STATUS$TYPE;
			      start_page	: integer;
			      finish_page	: integer;
			  end;
	file_info_arr(count:integer)		= array [1..count] of file_info;
	file_info_ptr	= ^file_info_arr;

	job_info	= record
			      prev, next	: job_info_ptr;
			      queue		: queue_info_ptr;
			      name		: varying [JOB_NAME_SIZE] of char;
			      username		: varying [USERNAME_SIZE] of char;
			      form		: varying [FORM_NAME_SIZE] of char;
			      entry_number	: integer;
			      size		: integer;
			      blocks_done	: integer;
			      priority		: integer;
			      file_count	: integer;
			      files		: file_info_ptr;
			      submit_time	: VMS_datetime;
			      completed_time	: VMS_datetime;
			      retain_until	: VMS_datetime;
			      flags		: JOB_FLAGS$TYPE;
			      status		: JOB_STATUS$TYPE;
			      pending_reason	: PENDING_JOB_REASON$TYPE;
			      condition		: packed array [1..3] of integer;
			      decoded_status	: Job_Status_Text;
			      display_row	: integer;
			      selected		: boolean;
			  end;

	queue_info	= record
			      prev, next	: queue_info_ptr;
			      name		: varying [QUEUE_NAME_SIZE] of char;
			      description	: varying [DESCRIPTION_SIZE] of char;
			      form		: varying [FORM_NAME_SIZE] of char;
			      node		: packed array [1..6] of char;
			      device		: varying [DEVICE_NAME_SIZE] of char;
			      flags		: QUEUE_FLAGS$TYPE;
			      status		: QUEUE_STATUS$TYPE;
			      display_row	: integer;
			      holding_jobs	: integer;
			      pending_jobs	: integer;
			      retained_jobs	: integer;
			      timed_jobs	: integer;
			      job_status	: JOB_STATUS$TYPE;
			      job_pending	: PENDING_JOB_REASON$TYPE;
			      job_first_file	: varying [NAM$C_MAXRSS] of char;
			      job_form		: varying [FORM_NAME_SIZE] of char;
			      job_entry_number	: unsigned;	
			      decoded_status	: Que_Status_Text;
			      selected		: boolean;
			  end;

	SMG_Menu(count,width:integer)	= record
					      display   : unsigned;
					      selection : [word] 0..65535;
					      choices   : array [1..count] of packed array [1..width] of char;
					  end;
	SMG_Menu_ptr			= ^SMG_Menu;

{-------------------------------------------------------------------------------
*	Global variables						       *
-------------------------------------------------------------------------------}

VAR
	pasteboard_id	: [volatile] unsigned;
	pasteboard_rows	: integer;
	pasteboard_cols	: integer;
	keyboard_id	: unsigned;
	time_display	: unsigned;
	read_timeout	: integer value 15;
	form_selector	: SMG_Menu_Ptr value NIL;
	exit_flag	: boolean value FALSE;

{-------------------------------------------------------------------------------
*	Routines declared in PQM.PAS					       *
-------------------------------------------------------------------------------}

Procedure UPDATE_TIME_DISPLAY;  external;
Procedure ERASE_MESSAGE;  external;
Function CONFIRM (prompt : [readonly] packed array [l0..h0:integer] of char) : boolean; external;
Procedure MESSAGE (str   : [readonly] packed array [l0..h0:integer] of char;
		   alert : boolean := TRUE);  external;
Procedure SYS_MESSAGE (sts : integer);  external;
Procedure APPEND_FIELD (var dest : string;
			src      : [readonly] packed array [l0..h0:integer] of char;
			fieldlen : integer;
			padding  : [truncate] integer);  external;
Procedure PREPARE_MENU (var menu   : SMG_Menu;
			target_row : integer;
			flags      : integer := SMG$M_FIXED_FORMAT+SMG$M_WRAP_MENU);  external;
Procedure DISPLAY_DCL_COMMAND (command : packed array [l0..h0:integer] of char;
			       paste_row, display_rows : integer);  extern;
Procedure DISPLAY_FILE (filespec : packed array [l0..h0:integer] of char); extern;


{-------------------------------------------------------------------------------
*	Routines declared in QUEUES.PAS					       *
-------------------------------------------------------------------------------}

Procedure BUILD_FORM_SELECTOR (var forms    : SMG_Menu_Ptr;
			       target_row   : integer;
			       initial_form : [truncate] packed array [l0..h0:integer] of char); external;
Procedure BUILD_QUEUE_SELECTOR (var queue_selector : SMG_Menu_Ptr;
				target_row         : integer;
				exclude_queue      : [truncate] packed array [l0..h0:integer] of char); external;
Procedure MANAGE_QUEUES (initial_queue		: string;
			 start_with_jobs	: boolean;
			 ignore_server_queues	: boolean); external;
Function UPDATE_ONE_QUEUE (var queue : queue_info) : integer; external;

{-------------------------------------------------------------------------------
*	Routines declared in JOBS.PAS					       *
-------------------------------------------------------------------------------}

Procedure MANAGE_JOBS (queue : queue_info_ptr);  external;
Procedure DECODE_PENDING_JOB_REASON (var str : varying [len] of char;
				     p       : PENDING_JOB_REASON$TYPE);  external;

{-------------------------------------------------------------------------------
*	Routines declared in BROWSER.PAS				       *
-------------------------------------------------------------------------------}

Function BROWSE_CREATE (r : integer;
			v : integer;
			h : packed array [l0..h0:integer] of char;
			var b : browse_list
			) : integer;  external;
Procedure BROWSE_DOWN (var b : browse_list);  external;
Procedure BROWSE_ERASE (var b : browse_list; new_rows : [truncate] integer);  external;
Procedure BROWSE_SELECT_ROW (var b : browse_list; selected_row : integer);  external;
Procedure BROWSE_UP (var b : browse_list);  external;

END.
