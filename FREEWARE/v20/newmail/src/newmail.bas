%TITLE "NEWMAIL"
%SBTTL "Display New Mail Summary"
%IDENT "V5.01"

PROGRAM newmail

!++
! FACILITY:
! 
!   Display new mail count and summary
! 
! FUNCTIONAL DESCRIPTION:
! 
!   Program to display the new mail count and new mail summary for
!   current user.  A user with SYSNAM privilege may examine the new
!   mail count for another user.
!
!
!  Calling sequence
!
!	Invoked via CLI command line
!
!  Command sequence
!
!	NEWMAIL
!
!  Qualifiers
!
!	/FULL
!
!	 Display From, Date, Subject for each new message.
!
!	/OUTPUT [=file-spec]
!
!	 Write output to specified file.
!
!	 The default output spec is SYS$OUTPUT.
!	 
!	/SAVE_COUNT [=symbol]
!
!	 Place new mail count in a DCL symbol.
!
!	 The default symbol name is NEWMAIL$COUNT
!
!	/USER=xxxxx
!
!	 Display new mail count for another user.
!
!	 This qualifier requires SYSNAM privilege.
!
!	 The /USER qualifier may not be used with the /FULL qualifier.
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
!   David L. Kachelmyer
! 
! CREATION DATE: 8-Nov-1987
! 
! MODIFICATION HISTORY:
!
!   V5.01		David L. Kachelmyer	December 31, 1988
!	Remove references to library BASIC_STARLET.
!--
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = SEVERE

    !+
    ! External Constants
    !-

    EXTERNAL LONG CONSTANT cli$_absent, cli$_defaulted, cli$_negated, &
			   cli$_present, ss$_normal

    !+
    ! Local Constants
    !-

    %INCLUDE "$libclidef" %FROM %LIBRARY
    %INCLUDE "$maildef" %FROM %LIBRARY
    %INCLUDE "$mailmsgdef"

    DECLARE INTEGER CONSTANT qual_full = 1, &
			     qual_output = 2, &
			     qual_save_count = 4, &
			     qual_user = 8

    DECLARE STRING CONSTANT fao_format_1 = "!5UL !20AD  !11AD !AD"

    !+
    ! External Routines
    !-

    %INCLUDE "mail$routines"
    
    EXTERNAL LONG FUNCTION chksts ( &
				LONG), &
			   cli$get_value ( &
				STRING, &
				STRING, &
				WORD), &
			   cli$present ( &
				STRING), &
			   lib$match_cond, &
			   lib$set_symbol ( &
				STRING, &
				STRING, &
				LONG), &
			   sys$fao

    !+
    ! RECORD Declarations
    !-

    RECORD item_list_3
	VARIANT
	    CASE
		WORD	buflen
		WORD	itmcod
		LONG	bufadr
		LONG	lenadr
	    CASE
		LONG	terminator
	END VARIANT
    END RECORD item_list_3
      
    !+
    ! MAP Declarations
    !-

    MAP (local) INTEGER	cnt, &
		LONG	exit_status, &
			messages_selected, &
			mailfile_context, &
			message_context, &
			message_date_len, &
			message_sender_len, &
			message_subject_len, &
			qualifiers, &
			stat, &
			stat1, &
			user_context, &
			user_directory_len, &
		WORD	message_flags, &
			new_message_cnt, &
			output_line_len, &
			output_spec_len, &
			symbol_name_len, &
			username_len, &
		STRING	username_value = 31, &
			user_directory = 255, &
			message_date = 11, &
			message_sender = 255, &
			message_subject = 255, &
			output_line = 80, &
			newmail_folder_name = 7, &
			output_spec_value = 255, &
			symbol_name_value = 255, &
		item_list_3	null_list, &
				input_list(1 TO 15), &
				output_list(1 TO 15)

    !+
    ! Local Variables
    !-


    %PAGE
    %SBTTL "Initialization routine"
Initialization_routine:

    exit_status = ss$_normal
    message_flags = mail$m_newmsg
    newmail_folder_name = "NEWMAIL"
    null_list::terminator = 0
    mailfile_context, message_context, user_context, qualifiers = 0

    %PAGE
    %SBTTL "Command line parsing"
Command_line_parse:

    WHEN ERROR IN
	stat = cli$present ("FULL")
	SELECT stat

	    CASE = cli$_present, = cli$_defaulted
		qualifiers = qualifiers OR qual_full

	    CASE = cli$_absent, = cli$_negated
		qualifiers = qualifiers AND NOT qual_full
	    
	    CASE ELSE
		stat = chksts (stat)

	END SELECT

	stat = cli$present ("OUTPUT")
	SELECT stat

	    CASE = cli$_present, = cli$_defaulted
		qualifiers = qualifiers OR qual_output
		stat1 = cli$get_value ( &
			    "OUTPUT", &
			    output_spec_value, &
			    output_spec_len)
		SELECT stat1

		    CASE = ss$_normal

		    CASE = cli$_absent
			output_spec_value = "SYS$OUTPUT"
			output_spec_len = 10
		    
		    CASE ELSE
			stat1 = chksts (stat1)

		END SELECT

	    CASE = cli$_absent
		qualifiers = qualifiers AND NOT qual_output
		output_spec_value = "SYS$OUTPUT"
		output_spec_len = 10

	    CASE = cli$_negated
		qualifiers = qualifiers AND NOT qual_output
		output_spec_value = "NLA0:"
		output_spec_len = 5

	    CASE ELSE
		stat = chksts (stat)

	END SELECT

	stat = cli$present ("SAVE_COUNT")
	SELECT stat

	    CASE = cli$_present, = cli$_defaulted
		qualifiers = qualifiers OR qual_save_count
		stat1 = cli$get_value ( &
			    "SAVE_COUNT", &
			    symbol_name_value, &
			    symbol_name_len)
		SELECT stat1

		    CASE = ss$_normal

		    CASE = cli$_absent
			symbol_name_value = "NEWMAIL$COUNT"
			symbol_name_len = 13
		    
		    CASE ELSE
			stat1 = chksts (stat1)

		END SELECT

	    CASE = cli$_absent, = cli$_negated
		qualifiers = qualifiers AND NOT qual_save_count

	    CASE ELSE
		stat = chksts (stat)

	END SELECT

	stat = cli$present ("USER")
	SELECT stat

	    CASE = cli$_present, = cli$_defaulted
		qualifiers = qualifiers OR qual_user
		stat1 = chksts (cli$get_value ( &
				    "USER", &
				    username_value, &
				    username_len))

	    CASE cli$_absent, = cli$_negated
		qualifiers = qualifiers AND NOT qual_user
	    
	    CASE ELSE
		stat = chksts (stat)

	END SELECT
    USE
	exit_status = VMSSTATUS	
	CONTINUE exit_routine
    END WHEN

    %PAGE
    %SBTTL "Main routine"
Main_routine:

    WHEN ERROR IN
	
	OPEN LEFT$ (output_spec_value, output_spec_len) FOR OUTPUT AS FILE #1

	output_list(1)::itmcod = mail$_user_new_messages
	output_list(1)::buflen = 2
	output_list(1)::bufadr = LOC (new_message_cnt)
	output_list(1)::lenadr = 0
	output_list(2)::itmcod = mail$_user_full_directory
	output_list(2)::buflen = 255
	output_list(2)::bufadr = LOC (user_directory)
	output_list(2)::lenadr = LOC (user_directory_len)
	output_list(3)::terminator = 0

	IF (qualifiers AND qual_user) = 0
	THEN
	    stat = mail$user_begin ( &
			user_context, &
			null_list BY REF, &
			output_list() BY REF)
	    stat = mail$user_end ( &
			user_context, &
			null_list BY REF, &
			null_list BY REF)
	ELSE
	    stat = mail$user_begin ( &
			user_context, &
			null_list BY REF, &
			null_list BY REF)

	    input_list(1)::itmcod = mail$_user_username
	    input_list(1)::buflen = username_len
	    input_list(1)::bufadr = LOC (username_value)
	    input_list(1)::lenadr = 0
	    input_list(2)::itmcod = mail$_nosignal
	    input_list(2)::buflen = 0
	    input_list(2)::bufadr = 0
	    input_list(2)::lenadr = 0
	    input_list(3)::terminator = 0

	    stat = mail$user_get_info ( &
			user_context, &
			input_list() BY REF, &
			output_list() BY REF)

	    SELECT stat

		CASE = ss$_normal

		CASE = mail$_nosuchusr, = mail$_nosysnam
		    EXIT PROGRAM (stat)
		
		CASE ELSE
		    stat = chksts (stat)

	    END SELECT
	    stat = mail$user_end ( &
			user_context, &
			null_list BY REF, &
			null_list BY REF)
	END IF

	MID$ (user_directory, &
	      user_directory_len+1, &
	      user_directory_len+5) = 	".MAI"
	user_directory_len = user_directory_len + 4
    USE
	exit_status = VMSSTATUS	
	CONTINUE exit_routine
    END WHEN
    ! Get newmail for current or specified user.
    ! This has been optimized for speed (relatively speaking).

    
    IF (qualifiers AND qual_save_count) <> 0
    THEN
	stat = chksts (lib$set_symbol ( &
			   LEFT$ (symbol_name_value, symbol_name_len), &
			   NUM1$ (new_message_cnt), &
			   lib$k_cli_global_sym))
    END IF
      
    WHEN ERROR IN

	IF (qualifiers AND qual_full) <> 0
	THEN
	    stat = mail$mailfile_begin ( &
			mailfile_context, &
			null_list BY REF, &
			null_list BY REF)

	    input_list(1)::itmcod = mail$_mailfile_default_name
	    input_list(1)::buflen = user_directory_len
	    input_list(1)::bufadr = LOC (user_directory)
	    input_list(1)::lenadr = 0
	    input_list(2)::itmcod = mail$_nosignal
	    input_list(2)::buflen = 0
	    input_list(2)::bufadr = 0
	    input_list(2)::lenadr = 0
	    input_list(3)::terminator = 0

	    stat = mail$mailfile_open ( &
			mailfile_context, &
			input_list() BY REF, &
			null_list BY REF)

	    SELECT lib$match_cond ( &
		       stat, &
		       mail$_nomsgs)

		CASE = 1
		    GO TO display_message_count
		
		CASE ELSE
		    stat = chksts (stat)

	    END SELECT

	    input_list(1)::itmcod = mail$_message_file_ctx
	    input_list(1)::buflen = 4
	    input_list(1)::bufadr = LOC (mailfile_context)
	    input_list(1)::lenadr = 0
	    input_list(2)::terminator = 0

	    stat = mail$message_begin (&
			message_context, &
			input_list() BY REF, &
			null_list BY REF)

	    input_list(1)::itmcod = mail$_message_folder
	    input_list(1)::buflen = LEN (newmail_folder_name)
	    input_list(1)::bufadr = LOC (newmail_folder_name)
	    input_list(1)::lenadr = 0
	    input_list(2)::itmcod = mail$_message_flags
	    input_list(2)::buflen = 2
	    input_list(2)::bufadr = LOC (message_flags)
	    input_list(2)::lenadr = 0
	    input_list(3)::itmcod = mail$_nosignal
	    input_list(3)::buflen = 0
	    input_list(3)::bufadr = 0
	    input_list(3)::lenadr = 0
	    input_list(4)::terminator = 0

	    output_list(1)::itmcod = mail$_message_selected
	    output_list(1)::buflen = 4
	    output_list(1)::bufadr = LOC (messages_selected)
	    output_list(1)::lenadr = 0
	    output_list(2)::terminator = 0

	    stat = mail$message_select ( &
			message_context, &
			input_list() BY REF, &
			output_list() BY REF)
	    
	    IF messages_selected > 0
	    THEN
		PRINT #1
		PRINT #1, "    # From                  Date        Subject"
		PRINT #1

		input_list(1)::itmcod = mail$_message_next
		input_list(1)::buflen = 0
		input_list(1)::bufadr = 0
		input_list(1)::lenadr = 0
		input_list(2)::itmcod = mail$_nosignal
		input_list(2)::buflen = 0
		input_list(2)::bufadr = 0
		input_list(2)::lenadr = 0
		input_list(3)::terminator = 0

		output_list(1)::itmcod = mail$_message_date
		output_list(1)::buflen = LEN (message_date)
		output_list(1)::bufadr = LOC (message_date)
		output_list(1)::lenadr = LOC (message_date_len)
		output_list(2)::itmcod = mail$_message_sender
		output_list(2)::buflen = LEN (message_sender)
		output_list(2)::bufadr = LOC (message_sender)
		output_list(2)::lenadr = LOC (message_sender_len)
		output_list(3)::itmcod = mail$_message_subject
		output_list(3)::buflen = LEN (message_subject)
		output_list(3)::bufadr = LOC (message_subject)
		output_list(3)::lenadr = LOC (message_subject_len)
		output_list(4)::terminator = 0

New_message_loop:
		
		FOR cnt = 1 TO messages_selected
		  
		    stat = mail$message_info ( &
				message_context, &
				input_list() BY REF, &
				output_list() BY REF)
		    SELECT stat

			CASE = ss$_normal
			    stat = chksts (sys$fao ( &
					       fao_format_1 BY DESC, &
					       output_line_len BY REF, &
					       output_line BY DESC, &
					       cnt BY VALUE, &
					       message_sender_len BY VALUE, &
					       message_sender BY REF, &
					       message_date_len BY VALUE, &
					       message_date BY REF, &
					       message_subject_len BY VALUE, &
					       message_subject BY REF))
			    PRINT #1, LEFT$ (output_line, output_line_len)

			CASE = mail$_nomoremsg
			    EXIT new_message_loop
			
			CASE ELSE
			    stat = chksts (stat)

		    END SELECT
		NEXT cnt
		  
	    END IF

	    stat = mail$message_end ( &
			message_context, &
			null_list BY REF, &
			null_list BY REF)

	    stat = mail$mailfile_close ( &
			mailfile_context, &
			null_list BY REF, &
			null_list BY REF)

	    stat = mail$mailfile_end ( &
			mailfile_context, &
			null_list BY REF, &
			null_list BY REF)
	END IF
    USE
	exit_status = VMSSTATUS	
	CONTINUE exit_routine

    END WHEN

Display_message_count:

    PRINT #1
    SELECT new_message_cnt

	CASE = 0
	    PRINT #1, "You have no new mail messages."

	CASE = 1
	    PRINT #1, "You have 1 new mail message."
	
	CASE ELSE
	    PRINT #1, "You have "; NUM1$ (new_message_cnt); &
		      " new mail messages."

    END SELECT    

    PRINT #1
    
    IF (qualifiers AND qual_full) <> 0
    THEN
	IF (new_message_cnt <> messages_selected)
	THEN
	    PRINT #1
	    PRINT #1, "Your new mail message count is incorrect."
	    PRINT #1
	END IF
    END IF
      
    %PAGE
    %SBTTL "Exit routine"

Exit_routine:
    
    EXIT PROGRAM exit_status

END PROGRAM

FUNCTION LONG chksts (LONG stat BY REF)

    
    OPTION TYPE = EXPLICIT

    EXTERNAL SUB lib$signal (LONG BY VALUE)
    CALL lib$signal (stat)  IF (stat AND 1%) = 0%

END FUNCTION stat  ! End of FUNCTION chksts
