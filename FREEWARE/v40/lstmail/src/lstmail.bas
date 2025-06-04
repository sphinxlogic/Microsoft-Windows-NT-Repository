%TITLE "LSTMAIL"
%SBTTL "List mail directory"
%IDENT "V5.4"

PROGRAM lstmail

!++
! FACILITY:
! 
!   Mail Utilities
! 
! FUNCTIONAL DESCRIPTION:
! 
!   Program to display mail message summary for specified
!   mail file. 
!
!   Uses CLI to parse command line.  It then displays message
!   summary according to selection qualifiers.
!
!   If /DIRECTORY is specified, a list of folder names is
!   output instead.
!
! COMMAND SEQUENCE:
!
!   LSTMAIL [/qualifiers] [mail-file-name]
!
! PARAMETERS:
!
!   Mail-file-name
!
!	Mail file to process.
!
!	Unspecified parameters default to login device, mail
!	directory, and MAIL filename with a type of .MAI.
!
! COMMAND QUALIFIERS:
!
!   /BEFORE=time
!
!	Select messages received before specified time.
!
!   /DIRECTORY
!
!	Display folder names.
!
!   /FLAGS=(type[,...])
!
!	Specify message types to process.  Legal message types are
!	EXTERNAL, MARKED, NEWMAIL, and REPLIED.
!
!   /FOLDER=foldername
!
!	Specify folder to display.  Foldername may contain wildcard
!	characters.
!
!	If not specified, all folders are processed.
!
!   /FROM=string
!
!	Select messages with FROM field containing specified string.
!
!   /OUTPUT [=file-spec]
!
!	Write output to specified file.
!
!	The default output spec is SYS$OUTPUT.
!
!   /SINCE=time
!
!	Select messages received since specified time.
!
!   /SUBJECT=string
!
!	Select messages with SUBJECT field containing specified string.
!
!   /TO=string
!
!	Select messages with TO field containing specified string.
!
! IMPLICIT INPUTS:
! 
!   None
! 
! IMPLICIT OUTPUTS:
! 
!   None
! 
! SIDE EFFECTS:
! 
!   None
! 
! AUTHOR:
! 
!   Dave Kachelmyer
! 
! CREATION DATE: 27-Jan-1985
! 
! MODIFICATION HISTORY:
!
!   V5.5	Dave Kachelmyer		May 25, 1992
!	Add /TO qualifier.
!
!   V5.4	Dave Kachelmyer		December 31, 1988
!	Remove references to BASIC_STARLET.
!
!   X5.3	Dave Kachelmyer		29-Apr-1988
!	    Fix problem with /OUTPUT that creates zero-length output files.
!
!   X5.2	Dave Kachelmyer		13-Apr-1988
!	    Fix problem that caused messages to be listed on a /DIRECTORY
!	    operation.
!
!   X5.1	Dave Kachelmyer		 3-Apr-1988
!	    Add BEFORE, AFTER, FROM, and SUBJECT selection qualifiers.
!	    Change /SELECT qualifier to /FLAGS.
!
!   X5.0	Dave Kachelmyer		27-Mar-1988
!	    Convert to V5 callable mail.
!--

    OPTION TYPE=EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    %PAGE
    %SBTTL "Declarations"

    !+
    ! Local Constants
    !-

    %INCLUDE "$fscndef" %FROM %LIBRARY
    %INCLUDE "$maildef" %FROM %LIBRARY
    %INCLUDE "$smgdef" %FROM %LIBRARY

    !+
    ! External Routines
    !-

    %INCLUDE "mail$routines"
    %INCLUDE "smg$routines" %FROM %LIBRARY
    %INCLUDE "str$routines" %FROM %LIBRARY

    EXTERNAL LONG FUNCTION chksts ( &
				LONG), &
			   mailfile_folder_routine ( &
				LONG, &
				STRING, &
				LONG, &
				LONG)
    EXTERNAL LONG FUNCTION cli$get_value ( &
				STRING, &
				STRING, &
				WORD), &
			   cli$present ( &
				STRING), &
			   sys$filescan ( &
				STRING, &
				LONG, &
				LONG), &
			   lib$sys_fao

    !+
    ! MAP Declarations
    !-

    MAP (main_local) LONG	mail_default_directory_len, &
		     STRING	mail_default_directory = 255

    
    !+
    ! Local Variables
    !-

    DECLARE LONG    device_status, &
		    filescan_flags, &
		    mailfile_user_data, &
		    pbid, &
		    stat, &
		    temp, &
	    STRING  command_line, &
		    folder_name, &
		    keyword_name, &
		    mail_file, &
		    output_file

    %INCLUDE "lstcommon"
    
    %PAGE
    %SBTTL "Program initialization"

    null_list::item(1)::terminator = 0

    stat = CTRLC
    ! Enable ^C trapping.

    %PAGE
    %SBTTL "Process command line"

    output_list::item(1)::itmcod = mail$_mailfile_mail_directory
    CALL   str$analyze_sdesc ( &
	       mail_default_directory, &
	       output_list::item(1)::buflen, &
	       output_list::item(1)::bufadr)
    output_list::item(1)::lenadr = LOC (mail_default_directory_len)
    output_list::item(2)::terminator = 0

    stat = chksts (mail$mailfile_begin ( &
			mailfile_context, &
			null_list, &
			output_list))

    stat = cli$present("MAILFILE")
    SELECT stat

	CASE = cli$_present, = cli$_defaulted
	    stat = cli$get_value ("MAILFILE", mail_file, )
	    SELECT stat

		CASE = ss$_normal
		    stat = chksts (sys$filescan ( &
				    mail_file, &
				    0% BY REF, &
				    filescan_flags))
		    
		    IF (filescan_flags AND &
			   (fscn$m_node OR fscn$m_device OR &
			    fscn$m_root OR fscn$m_directory)) = 0%
		    THEN
			mail_file = LEFT$ (mail_default_directory, &
					   mail_default_directory_len) + &
				    mail_file
		    END IF
		
		CASE ELSE
		    stat = chksts (stat)

	    END SELECT

	CASE = cli$_absent
	    mail_file = LEFT$ (mail_default_directory, &
			       mail_default_directory_len) + "MAIL.MAI"
	
	CASE ELSE
	    stat = chksts (stat)

    END SELECT
    ! Get mail file name,
    !  default to MAIL.

    stat = cli$present ("DIRECTORY")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_directory

	CASE = cli$_absent
	
	CASE ELSE

    END SELECT
    ! Process DIRECTORY qualifier.

    stat = cli$present ("FOLDER")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_select_folder
	    stat = cli$get_value ( &
		       "folder", &
		       folder_pattern, &
		       folder_pattern_len)

	CASE = cli$_absent, = cli$_negated
	
	CASE ELSE

    END SELECT
    ! Process FOLDER qualifier.

    stat = cli$present ("OUTPUT")
    SELECT stat

	CASE = cli$_present
	    stat = cli$get_value ( &
		       "OUTPUT", &
		       output_file,)

	CASE = cli$_absent
	    output_file = "SYS$OUTPUT"

	CASE ELSE

	    stat = chksts (stat)

    END SELECT
    ! Process OUTPUT qualifier.

    stat = cli$present ("FLAGS")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_select_flags
	    select_flag = 0%

Select_loop:
	    WHILE 1
		stat = cli$get_value ( &
			   "FLAGS", &
			   keyword_name,)
		SELECT stat

		    CASE = ss$_normal
			SELECT LEFT$ (keyword_name,1%)
			    CASE = "E"
				    select_flag = select_flag OR mmh_m_extmsg
			    CASE = "N"
				    select_flag = select_flag OR mmh_m_newmsg
			    CASE = "M"
				    select_flag = select_flag OR mmh_m_marked
			    CASE = "R"
				    select_flag = select_flag OR mmh_m_replied
			    CASE ELSE
			END SELECT

		    CASE = cli$_absent
			EXIT select_loop
		    
		    CASE ELSE
			EXIT select_loop

		END SELECT
	    NEXT

	CASE = cli$_absent, = cli$_negated
	
	CASE ELSE

    END SELECT
    ! Process SELECT qualifier.
    !  Allowable types are EXTERNAL, NEWMAIL, MARKED, and REPLIED.

    stat = cli$present ("BEFORE")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_select_before
	    stat = chksts (cli$get_value ( &
			       "BEFORE", &
			       select_before, &
			       select_before_len))

	CASE = cli$_absent
	
	CASE ELSE
	    stat = chksts (stat)

    END SELECT

    stat = cli$present ("SINCE")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_select_since
	    stat = chksts (cli$get_value ( &
			       "SINCE", &
			       select_since, &
			       select_since_len))

	CASE = cli$_absent
	
	CASE ELSE
	    stat = chksts (stat)

    END SELECT

    stat = cli$present ("FROM")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_select_from
	    stat = chksts (cli$get_value ( &
			       "FROM", &
			       select_from, &
			       select_from_len))

	CASE = cli$_absent

	CASE ELSE
	    stat = chksts (stat)

    END SELECT

    stat = cli$present ("SUBJECT")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_select_subj
	    stat = chksts (cli$get_value ( &
			       "SUBJECT", &
			       select_subj, &
			       select_subj_len))

	CASE = cli$_absent

	CASE ELSE
	    stat = chksts (stat)

    END SELECT

    stat = cli$present ("TO")
    SELECT stat

	CASE = cli$_present
	    option_flag = option_flag OR opt_m_select_to
	    stat = chksts (cli$get_value ( &
			       "TO", &
			       select_to, &
			       select_to_len))

	CASE = cli$_absent

	CASE ELSE
	    stat = chksts (stat)

    END SELECT

    %PAGE
    %SBTTL "Main routine"

    input_list::item(1)::itmcod = mail$_mailfile_name
    CALL   str$analyze_sdesc ( &
	       mail_file, &
	       input_list::item(1)::buflen, &
	       input_list::item(1)::bufadr)
    input_list::item(1)::lenadr = 0
    input_list::item(2)::terminator = 0

    stat = chksts (mail$mailfile_open ( &
			mailfile_context, &
			input_list,&
			null_list))
    ! Open mail file.

    OPEN output_file FOR OUTPUT AS FILE #1, &
	    DEFAULTNAME "SYS$DISK:LSTMAIL.LIS"
    device_status = STATUS
    ! Open output list file.

    IF (option_flag AND opt_m_directory) <> 0
    THEN
	MARGIN #1%, 80
	
	WHEN ERROR IN
		mailfile_user_data = lstm_c_list_folders

		input_list::item(1)::itmcod = mail$_mailfile_folder_routine
		input_list::item(1)::buflen = 4
		input_list::item(1)::bufadr = LOC (mailfile_folder_routine)
		input_list::item(1)::lenadr = 0
		input_list::item(2)::itmcod = mail$_mailfile_user_data
		input_list::item(2)::buflen = 4
		input_list::item(2)::bufadr = LOC (mailfile_user_data)
		input_list::item(2)::lenadr = 0
		input_list::item(3)::terminator = 0

		stat = chksts (mail$mailfile_info_file ( &
				   mailfile_context, &
				   input_list, &
				   null_list))
	USE

	    CONTINUE exit_routine
	END WHEN
	! If this is a folder-list operation, 

    ELSE
	MARGIN #1%, 132%
	IF (device_status AND 4%) <> 0	    ! If this is a terminal device
	THEN
	    
	    WHEN ERROR IN
		stat = chksts (smg$create_pasteboard ( &
				   pbid, &
				   output_file,,,))
		stat = chksts (smg$change_pbd_characteristics ( &
				   pbid, &
				   132,,,,,))
	    USE
		CONTINUE exit_routine
	    END WHEN
	END IF

	WHEN ERROR IN
	    mailfile_user_data = lstm_c_list_messages

	    input_list::item(1)::itmcod = mail$_mailfile_folder_routine
	    input_list::item(1)::buflen = 4
	    input_list::item(1)::bufadr = LOC (mailfile_folder_routine)
	    input_list::item(1)::lenadr = 0
	    input_list::item(2)::itmcod = mail$_mailfile_user_data
	    input_list::item(2)::buflen = 4
	    input_list::item(2)::bufadr = LOC (mailfile_user_data)
	    input_list::item(2)::lenadr = 0
	    input_list::item(3)::terminator = 0

	    stat = chksts (mail$mailfile_info_file ( &
			       mailfile_context, &
			       input_list, &
			       null_list))
	USE
	    CONTINUE exit_routine
	END WHEN

    END IF
    ! Open output device and get device characteristics.
    ! If this is a /DIRECTORY, then set BASIC margin to 80 and dispatch.
    ! If not, then set BASIC margin to 132.  If output device is a terminal,
    !  set terminal width to 132.

    %PAGE
    %SBTTL "Exit routine"
Exit_routine:
    !+
    !	Close files and exit the program.
    !-

    CLOSE #1%

    IF mailfile_context <> 0
    THEN
	WHEN ERROR IN
	    stat = mail$mailfile_close ( &
		       mailfile_context, &
		       null_list, &
		       null_list)
	    stat = mail$mailfile_end ( &
		       mailfile_context, &
		       null_list, &
		       null_list)
	USE
	    CONTINUE
	END WHEN
    END IF

    IF ((device_status AND 4%) <> 0%) AND &
	((option_flag AND opt_m_directory) = 0)
    THEN
	LINPUT "Press RETURN to restore screen"; command_line
	stat = smg$delete_pasteboard ( &
		   pbid, &
		   0)
    END IF
    ! If output is to terminal, then pause before resetting terminal
    !  to 80-column mode.

END PROGRAM

%TITLE "CHKSTS"
%SBTTL "Check return status"
%IDENT "V1.0"

FUNCTION LONG chksts (LONG stat)

    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER

    EXTERNAL SUB lib$signal (LONG BY VALUE)
    CALL lib$signal (stat)  IF (stat AND 1) = 0

END FUNCTION stat  ! End of FUNCTION chksts

%TITLE "MAILFILE_FOLDER_ROUTINE"
%SBTTL "Folder action routine"
%IDENT "X5.0"

FUNCTION LONG mailfile_folder_routine ( &
		       LONG function_code, &
		       STRING folder_name, &
		       LONG dummy1, &
		       LONG dummy2)

![function-header]
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    %PAGE
    %SBTTL "Declarations"

    !+
    ! Local Constants
    !-

    %INCLUDE "$maildef" %FROM %LIBRARY
    
    !+
    ! External Routines
    !-

    %INCLUDE "mail$routines"
    %INCLUDE "str$routines" %FROM %LIBRARY

    EXTERNAL LONG FUNCTION chksts (LONG), &
			   lib$sys_fao
    
    !+
    ! MAP Declarations
    !-

    MAP (mfr_local) LONG	line_count, &
				mail_message_selected, &
				message_flags, &
				message_size, &
				stat, &
				message_sender_len, &
				message_subject_len, &
		    STRING	message_date = 11, &
				message_sender = 20, &
				message_subject = 255, &
				external_flag = 1, &
				marked_flag = 1, &
				newmail_flag = 1, &
				replied_flag = 1, &
				sysmsg_flag = 1

    DECLARE STRING	output_line
    
    %INCLUDE "lstcommon"
    
    %PAGE
    
    IF folder_name = ""
    THEN
	PRINT #1
	EXIT FUNCTION ss$_normal
    END IF
    ! If this is the last invocation of this routine, then
    ! finish up any output and exit routine.
      
    IF option_flag AND opt_m_select_folder
    THEN
	stat = str$match_wild ( &
		   EDIT$ (folder_name, 128), &
		   LEFT$ (folder_pattern, folder_pattern_len))
	SELECT stat

	    CASE = str$_match

	    CASE = str$_nomatch
		EXIT FUNCTION ss$_normal
	    
	    CASE ELSE
		EXIT FUNCTION ss$_normal		! Ignore for now

	END SELECT
    END IF
    ! Check for folder-name selection.

    SELECT function_code

	CASE = lstm_c_list_messages
	    
	    input_list::item(1)::itmcod = mail$_message_file_ctx
	    input_list::item(1)::buflen = 4
	    input_list::item(1)::bufadr = LOC (mailfile_context)
	    input_list::item(1)::lenadr = 0
	    input_list::item(2)::terminator = 0

	    stat = chksts (mail$message_begin ( &
			       message_context, &
			       input_list, &
			       null_list))

	    input_list::item(1)::itmcod = mail$_nosignal
	    input_list::item(1)::buflen = 0
	    input_list::item(1)::bufadr = 0
	    input_list::item(1)::lenadr = 0
	    input_list::item(2)::itmcod = mail$_message_folder
	    CALL   str$analyze_sdesc ( &
		       folder_name, &
		       input_list::item(2)::buflen, &
		       input_list::item(2)::bufadr)
	    input_list::item(2)::lenadr = 0
	    
	    IF option_flag AND opt_m_select_flags
	    THEN
		input_list::item(3)::itmcod = mail$_message_flags
		input_list::item(3)::buflen = 2
		input_list::item(3)::bufadr = LOC (select_flag)
		input_list::item(3)::lenadr = 0
	    ELSE
		input_list::item(3)::itmcod = mail$_noop
		input_list::item(3)::buflen = 0
		input_list::item(3)::bufadr = 0
		input_list::item(3)::lenadr = 0
	    END IF

	    IF option_flag AND opt_m_select_before
	    THEN
		input_list::item(4)::itmcod = mail$_message_before
	        input_list::item(4)::buflen = select_before_len
	        input_list::item(4)::bufadr = LOC (select_before)
		input_list::item(4)::lenadr = 0
	    ELSE
		input_list::item(4)::itmcod = mail$_noop
		input_list::item(4)::buflen = 0
		input_list::item(4)::bufadr = 0
		input_list::item(4)::lenadr = 0
	    END IF

	    IF option_flag AND opt_m_select_since
	    THEN
		input_list::item(5)::itmcod = mail$_message_since
	        input_list::item(5)::buflen = select_since_len
	        input_list::item(5)::bufadr = LOC (select_since)
		input_list::item(5)::lenadr = 0
	    ELSE
		input_list::item(5)::itmcod = mail$_noop
		input_list::item(5)::buflen = 0
		input_list::item(5)::bufadr = 0
		input_list::item(5)::lenadr = 0
	    END IF

	    IF option_flag AND opt_m_select_from
	    THEN
		input_list::item(6)::itmcod = mail$_message_from_substring
	        input_list::item(6)::buflen = select_from_len
	        input_list::item(6)::bufadr = LOC (select_from)
		input_list::item(6)::lenadr = 0
	    ELSE
		input_list::item(6)::itmcod = mail$_noop
		input_list::item(6)::buflen = 0
		input_list::item(6)::bufadr = 0
		input_list::item(6)::lenadr = 0
	    END IF
	      
	    IF option_flag AND opt_m_select_subj
	    THEN
		input_list::item(7)::itmcod = mail$_message_subj_substring
	        input_list::item(7)::buflen = select_subj_len
	        input_list::item(7)::bufadr = LOC (select_subj)
		input_list::item(7)::lenadr = 0
	    ELSE
		input_list::item(7)::itmcod = mail$_noop
		input_list::item(7)::buflen = 0
		input_list::item(7)::bufadr = 0
		input_list::item(7)::lenadr = 0
	    END IF

	    IF option_flag AND opt_m_select_to
	    THEN
		input_list::item(8)::itmcod = mail$_message_to_substring
	        input_list::item(8)::buflen = select_to_len
	        input_list::item(8)::bufadr = LOC (select_to)
		input_list::item(8)::lenadr = 0
	    ELSE
		input_list::item(8)::itmcod = mail$_noop
		input_list::item(8)::buflen = 0
		input_list::item(8)::bufadr = 0
		input_list::item(8)::lenadr = 0
	    END IF

	    input_list::item(9)::terminator = 0

	    output_list::item(1)::itmcod = mail$_message_selected
	    output_list::item(1)::buflen = 4
	    output_list::item(1)::bufadr = LOC (mail_message_selected)
	    output_list::item(1)::lenadr = 0
	    output_list::item(2)::terminator = 0

	    WHEN ERROR IN
		stat = chksts (mail$message_select ( &
				   message_context, &
				   input_list, &
				   output_list))
	    USE
		EXIT HANDLER
	    END WHEN
	    
	    input_list::item(1)::itmcod = mail$_nosignal
	    input_list::item(1)::buflen = 0
	    input_list::item(1)::bufadr = 0
	    input_list::item(1)::lenadr = 0
	    input_list::item(2)::itmcod = mail$_message_next
	    input_list::item(2)::buflen = 0
	    input_list::item(2)::bufadr = 0
	    input_list::item(2)::lenadr = 0
	    input_list::item(3)::terminator = 0

	    output_list::item(1)::itmcod = mail$_message_date
	    CALL   str$analyze_sdesc ( &
		       message_date, &
		       output_list::item(1)::buflen, &
		       output_list::item(1)::bufadr)
	    output_list::item(1)::lenadr = 0
	    output_list::item(2)::itmcod = mail$_message_return_flags
	    output_list::item(2)::buflen = 2
	    output_list::item(2)::bufadr = LOC (message_flags)
	    output_list::item(2)::lenadr = 0
	    output_list::item(3)::itmcod = mail$_message_sender
	    CALL   str$analyze_sdesc ( &
		       message_sender, &
		       output_list::item(3)::buflen, &
		       output_list::item(3)::bufadr)
	    output_list::item(3)::lenadr = LOC (message_sender_len)
	    output_list::item(4)::itmcod = mail$_message_size
	    output_list::item(4)::buflen = 4
	    output_list::item(4)::bufadr = LOC (message_size)
	    output_list::item(4)::lenadr = 0
	    output_list::item(5)::itmcod = mail$_message_subject
	    CALL   str$analyze_sdesc ( &
		       message_subject, &
		       output_list::item(5)::buflen, &
		       output_list::item(5)::bufadr)
	    output_list::item(5)::lenadr = LOC (message_subject_len)
	    output_list::item(6)::terminator = 0

	    FOR line_count = 1 TO mail_message_selected
		
		WHEN ERROR IN
		    stat = chksts (mail$message_info ( &
				       message_context, &
				       input_list, &
				       output_list))
		USE
		    EXIT HANDLER
		END WHEN

		external_flag, newmail_flag, marked_flag, replied_flag, &
		    sysmsg_flag = ""
		external_flag = "E"  IF (message_flags AND mmh_m_extmsg)
		marked_flag   = "M"  IF (message_flags AND mmh_m_marked)
		newmail_flag  = "N"  IF (message_flags AND mmh_m_newmsg)
		replied_flag  = "R"  IF (message_flags AND mmh_m_replied)
		sysmsg_flag   = "S"  IF (message_flags AND mmh_m_sysmsg)
		! Set mail flag indicatiors as appropriate.

		stat = chksts (lib$sys_fao ( &
				"!20AS  !4UL !5(1AS) !20AS  !11AS  !AS",, &
				output_line, &
				folder_name, &
				message_size by value, &
				external_flag, &
				marked_flag, &
				newmail_flag, &
				replied_flag, &
				sysmsg_flag, &
				LEFT$ (message_sender, message_sender_len), &
				message_date, &
				LEFT$ (message_subject, message_subject_len)))
		PRINT #1, output_line
	    NEXT line_count

	    stat = chksts (mail$message_end ( &
			       message_context, &
			       null_list, &
			       null_list))

	CASE = lstm_c_list_folders
	    PRINT #1, folder_name,
	
	CASE ELSE
	    PRINT "Invalid function code"; function_code; &
			"to routine MAILFILE_FOLDER_ROUTINE"
	    EXIT FUNCTION X"0000002"L

    END SELECT
    ! Perform action based on function code.
    ! Display error if invalid function code.

END FUNCTION  ss$_normal  ! End of FUNCTION mailfile_folder_routine
